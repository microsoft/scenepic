"""Tornado web-server-based web sockets communication helper."""

import json

from tornado import gen, websocket

from ._svt import Scene


class WebSocketHandler(websocket.WebSocketHandler):
    """Server for handling interactive requests for SVT."""

    def initialize(self, handler=None):
        """Initialize the server."""
        self.handler = handler

    def check_origin(self, origin):
        """Allow cross-origin traffic."""
        return True

    def open(self):  # noqa: A003
        """Web Socket opened."""
        print("Connection established")
        if self.handler is not None:
            self.handler.connection_opened(self)

    @gen.coroutine
    def close(self):
        """Web Socket self closed."""
        print("Closing web socket...")
        if self.handler is not None:
            yield self.handler.connection_closed(self)
        websocket.WebSocketHandler.close(self)

    @gen.coroutine
    def on_close(self):
        """Web Socket externally closed."""
        print("Web socket externally closed...")
        if self.handler is not None:
            yield self.handler.connection_closed(self)

    @gen.coroutine
    def on_message(self, message):
        """Message received from WebSocket."""
        message_obj = json.loads(message)

        if "Type" in message_obj:
            message_type = message_obj["Type"]
            if message_type == "SceneIdChange":
                scene_id = message_obj["SceneId"]
                if self.handler is not None:
                    yield self.handler.notify_scene_id_change(self, scene_id)
            elif message_type == "KeyPress":
                key = message_obj["Key"]
                alt = message_obj["Alt"]
                ctrl = message_obj["Ctrl"]
                shift = message_obj["Shift"]
                meta = message_obj["Meta"]
                canvas_id = message_obj["CanvasId"]
                frame_id = message_obj["FrameId"] if "FrameId" in message_obj else None
                if self.handler is not None:
                    yield self.handler.notify_keypress(self, canvas_id, frame_id, key, alt, ctrl, shift, meta)
            elif message_type == "InputBoxChange":
                text_panel_id = message_obj["TextPanelId"]
                value = message_obj["Value"]
                if self.handler is not None:
                    yield self.handler.notify_inputbox_change(self, text_panel_id, value)
            elif message_type == "DropDownMenuChange":
                drop_down_menu_id = message_obj["DropDownMenuId"]
                index = message_obj["Index"]
                if self.handler is not None:
                    yield self.handler.notify_dropdownmenu_change(self, drop_down_menu_id, index)
            elif message_type == "FrameIdChange":
                canvas_id = message_obj["CanvasId"]
                frame_id = message_obj["FrameId"] if "FrameId" in message_obj else None
                if self.handler is not None:
                    yield self.handler.notify_frame_id_change(self, canvas_id, frame_id)
            elif message_type == "AcknowledgeCommand":
                if self.handler is not None:
                    yield self.handler.notify_command_received(self, message_obj["AckData"])

    @gen.coroutine
    def send_commands(self, script, ack_data=None):
        """Call this to send commands script to client."""
        import time
        prejson = time.time() * 1000.0
        if isinstance(script, Scene):
            scriptstr = script.get_json()
        else:
            scriptstr = json.dumps(script)
        sendtime = time.time() * 1000.0
        ack_str = ', "AckData" : ' + json.dumps(ack_data) if ack_data is not None else ''
        jsonstr = '{ "SceneCommands" : %s, "Timings" : [%f, %f] %s }' % (scriptstr, prejson, sendtime, ack_str)
        try:
            self.write_message(jsonstr)
        except Exception:
            print('send_commands failed due to closed connection')
