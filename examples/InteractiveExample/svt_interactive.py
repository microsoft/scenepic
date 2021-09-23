# Imports
import sys
sys.path.append("../../dist") # Or wherever svt.py is located
import svt
import json
from tornado import websocket, web, ioloop

HTMLPage = """<!DOCTYPE html>
<!-- This file will be launched automatically in the browser by the svt_interactive.py script -->
<html lang="en">
    <head>
        <meta charset="utf-8"/>

        <title>ScenePic Client</title>

        <!-- This loads the svt library from the localhost server -->
        <script src="%s"></script>
        
        <script>
            // Load SVT module
            require(["SVTScene"], function(SVTScene)
            {
                // Instantiate a scene
                var MainScene = new SVTScene.default();

                // Connect to the server launched by svt_interactive.py
                MainScene.ConnectToServer("%s");
            });
        </script>
    </head>
    <body>
    </body>
</html>
"""

class InteractiveExample(object):
    """This is an example class that will generate SVT scripts in an interactive client-server manner."""

    def __init__(self):
        return

    def connection_opened(self, socket):
        """Called by svt.WebSocketHandler when a new connection is opened"""

        # Toy example - these coordinates will be updated through key-presses
        self.x = 0
        self.y = 0

        # Create a trivial scene with a cube
        self.scene = svt.Scene()
        self.canvas = self.scene.create_canvas_3d()
        self.update_mesh()
        self.frame = self.canvas.create_frame()
        self.frame.add_mesh(self.mesh)
        self.frame = self.canvas.create_frame()
        self.frame.add_mesh(self.mesh)

        self.tp = self.scene.create_text_panel(add_input_box = True)
        self.tp.set_text("Hello World 123")
        self.tp.set_title("MyTitle")
        self.tp.set_input_text("Initial input text")

        # Send the SVT script over the web socket to the web client
        if socket is not None:
            socket.send_commands(self.scene)
            self.scene.clear_script() # Clear the script so it doesn't get sent next time around

    def connection_closed(self, socket):
        """Called by svt.WebSocketHandler when connection is closed"""
        socket = None

    def update_mesh(self):
        """Helper method - regenerates a cube at x, y"""
        self.mesh = self.scene.create_mesh(mesh_id = "box")
        self.mesh.add_cube(color = svt.Color(1.0, 1.0, 0.0), transform = svt.Transforms.Translate([self.x, self.y, 0]))

    def notify_inputbox_change(self, socket, text_panel_id, value):
        print "TextPanel", text_panel_id, "updated to", value

    def notify_frame_id_change(self, socket, canvas_id, frame_id):
        print "Canvas", canvas_id, "updated to frame", frame_id

    def notify_command_received(self, socket):
        pass

    def notify_keypress(self, socket, canvasId, frameId, key, alt, ctrl, shift, meta):
        """Called by svt.WebSocketHandler whenever the HTML client reports a keypress"""

        # Convert to lower case
        key = key.lower()
        print key

        # Update x and y appropriately
        delta = 0.2
        if key == "up" or key == "arrowup":
            self.y += delta
        elif key == "down" or key == "arrowdown":
            self.y -= delta
        elif key == "left" or key == "arrowleft":
            self.x -= delta
        elif key == "right" or key == "arrowright":
            self.x += delta
        elif key == "x":
            socket.send_commands([{ "CommandType" : "ShutDown" }])
            sys.exit()
        elif key == "a":
            print "aha"
            self.tp.set_input_text("Aha! You pressed 'a'")

        # Update mesh
        self.update_mesh()

        # Send the SVT script over the web socket to the web client
        if socket is not None:
            socket.send_commands(self.scene)
            self.scene.clear_script() # Clear the script so it doesn't get sent next time around

# Main entry point
if __name__ == '__main__':

    # Init handler code - this is the class you write
    handler = InteractiveExample()

    # Set up WebSockets server
    ServerPort = 8900
    MaxPortAttempt = ServerPort + 100
    while True:
        # Get addresses
        webpage_address = "http://localhost:" + str(ServerPort) + "/"
        svt_lib_address = "http://localhost:" + str(ServerPort) + "/svt.min.js"
        websocket_address = "ws://localhost:" + str(ServerPort) + "/ws/"

        # Build html page
        html_page = HTMLPage % (svt_lib_address, websocket_address)
        class HtmlHandler(web.RequestHandler):
            def get(self):
                print "Hello World"
                self.write(html_page)       

        app = web.Application([
            (r'/', HtmlHandler),
            (r'/svt.min.js()', web.StaticFileHandler, {"path" : "../../dist/svt.min.js" }), # Or wherever located
            (r'/ws/', svt.WebSocketHandler, {'handler' : handler})
        ])
        try:
            app.listen(ServerPort)
            break
        except:
            ServerPort += 1
            if ServerPort > MaxPortAttempt:
                raise Exception("MaxPortAttempt reached.  Exiting.")
    print "Server up and running at %s" % webpage_address

    # Automatically launch the default web browser at this localhost URL
    import webbrowser
    webbrowser.open(webpage_address)

    # Run the server
    ioloop.IOLoop.instance().start()