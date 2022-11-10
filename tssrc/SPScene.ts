import {ObjectCache, CanvasBase} from "./CanvasBase"
import Canvas3D from "./Canvas3D";
import Canvas2D from "./Canvas2D";
import Graph from "./Graph";
import TextPanel from "./TextPanel";
import DropDownMenu from "./DropDownMenu";
import InitializeCSSStyles from "./CSSStyles";
import Misc from "./Misc"
import Mesh from "./Mesh";
import { VertexBufferType } from "./VertexBuffers";
import {vec3} from "gl-matrix";
import {saveAs} from "file-saver";
import * as JSZip from "jszip";

const StatusPanelName = "___STATUS___";
const LogPanelName = "___LOG___";
const HelpPanelName = "___HELP___";

export default class SPScene
{
    div : HTMLDivElement;
    statusDiv : HTMLDivElement;
    progressDiv : HTMLDivElement;
    progressBar : HTMLSpanElement;
    canvasDiv : HTMLDivElement = null;
    statusDivNeedsPadding : boolean;

    // Scene identifier
    sceneId : string;

    // Set of global meshes
    allMeshes = {};

    // Set of global images and text labels that can be used as textures
    objectCache : ObjectCache;

    // Text panels by id
    textPanels : Map<string, TextPanel> = new Map<string, TextPanel>();

    // Drop down menus by id
    dropDownMenus : Map<string, DropDownMenu> = new Map<string, DropDownMenu>();

    // Canvases (3D and 2D)
    canvases : Map<string, CanvasBase> = new Map<string, CanvasBase>();

    // Mesh keyframes
    meshKeyframes = {};

    // Canvas groups (used to link events across canvases)
    canvasGroups = {};

    // Pointer events
    pointerCoords = {};
    initPointerCoords = {};

    // WebSocket for use in interactive mode
    ws : WebSocket = null;  

    // Playback rate
    playbackRate : number = 1.0;
    media : HTMLMediaElement[] = [];
    isPlaying : boolean = false;
    frameRate : number = 30.0;
    timePerFrame : number = 1.0 / 30.0;

    // Global controls
    recordButton : HTMLButtonElement;
    playPauseToggle : HTMLInputElement;
    volumeToggle : HTMLInputElement;
    volumeSlider : HTMLInputElement;

    // Recording
    recordingZip : JSZip;
    canvasZipFolders : {[id: string] : JSZip};
    numFramesPerCanvas : {[id: string]: number};
    currentRecordingFrame : number;
    maxFrames : number;

    constructor(element : HTMLElement = null) // element is passed in if being used from Jupyter
    {
        // Create image and label cache
        var self : SPScene = this; // Since the lambda below would have a different "this"
        this.objectCache = new ObjectCache(textureId =>
        {
            // Update all canvases that are consuming this image
            for(var canvasId in self.canvases)
            {
                var canvas : CanvasBase = self.canvases[canvasId];
                canvas.NotifyTextureUpdated(textureId);
            }
        });

        // Initialize styles
        InitializeCSSStyles();

        // Create html placeholder
        this.div = document.createElement("div"); // Main div for canvases and user-provided text panels
        this.div.className = "scenepic";
        if (element != null) element.appendChild(this.div); else document.body.appendChild(this.div);

        this.statusDiv = document.createElement("div");
        this.statusDiv.classList.add("scenepic");
        if (element == null)
            this.statusDiv.classList.add("scenepic-status-bar");
        this.statusDivNeedsPadding = element == null;
        if (element != null) element.appendChild(this.statusDiv); else document.body.appendChild(this.statusDiv);

        let controls = document.createElement("div");
        controls.className = "scenepic-controls";

        // Recording
        this.recordButton = document.createElement("button");
        this.recordButton.classList.add("scenepic-icon-record", "scenepic-medium");
        this.recordButton.addEventListener("click", () => this.Record());
        this.recordButton.title = "Record";
        this.progressDiv = document.createElement("div");
        this.progressBar = document.createElement("span");
        this.progressBar.style.width = "0%";
        this.progressDiv.style.visibility = "hidden";
        this.progressDiv.classList.add("scenepic-progress");
        this.progressDiv.append(this.progressBar);
        this.statusDiv.append(this.progressDiv);
        controls.appendChild(this.recordButton);

        // Play/pause and faster/slower
        let playPauseToggle = this.createToggleButton("Play/Pause", "playpause");
        playPauseToggle[0].classList.add("scenepic-large");
        this.playPauseToggle = playPauseToggle[1];
        this.playPauseToggle.checked = false;
        this.playPauseToggle.addEventListener("change", () => this.PlayVideo());
        let fasterButton = document.createElement("button");
        fasterButton.className = "scenepic-icon-faster";
        fasterButton.addEventListener("click", () => this.faster());
        fasterButton.title = "Faster";
        let slowerButton = document.createElement("button");
        slowerButton.className = "scenepic-icon-slower";
        slowerButton.addEventListener("click", () => this.slower());
        slowerButton.title = "Slower";
        controls.appendChild(slowerButton);
        controls.appendChild(playPauseToggle[0]);
        controls.appendChild(fasterButton);

        // Camera
        let resetCamera = document.createElement("button");
        resetCamera.classList.add("scenepic-icon-resetcamera", "scenepic-medium");
        resetCamera.addEventListener("click", () => this.ResetAllViews());
        resetCamera.title = "Reset Camera";
        controls.appendChild(resetCamera);

        // Audio
        let volumeToggle = this.createToggleButton("Sound/Mute", "soundmute");
        volumeToggle[0].style.marginLeft = "32px";
        this.volumeToggle = volumeToggle[1];
        this.volumeToggle.addEventListener("change", () => this.UpdateVolume());
        this.volumeSlider = document.createElement("input");
        this.volumeSlider.className = "scenepic-volume-slider";
        this.volumeSlider.title = "Global Volume";
        this.volumeSlider.type = "range";
        this.volumeSlider.min = "0";
        this.volumeSlider.max = "1";
        this.volumeSlider.value = "1";
        this.volumeSlider.step = "0.01";
        this.volumeSlider.addEventListener("change", () => this.UpdateVolume());
        controls.appendChild(volumeToggle[0]);
        controls.appendChild(this.volumeSlider);
        this.statusDiv.append(controls);

        // Help and status information
        this.AddTextPanel(HelpPanelName, "Click for Help", "font-size:10pt;", this.statusDiv, true);
        var helpHtml = '<span class="scenepic-textbox-content">'
        helpHtml += 'space: play/pause<br>';
        helpHtml += '+/-: adjust play speed<br>';
        helpHtml += 'shift: translate<br>';
        helpHtml += 'ctrl: twist<br>';
        helpHtml += 'alt: slow translation/rotation<br>';
        helpHtml += 'tab: toggle between first person/focus cameras<br>';
        helpHtml += 'r: reset view';
        helpHtml += '</span><span class="scenepic-textbox-content">';
        helpHtml += '\\: toggle orbit camera<br>';
        helpHtml += 'caps: move focus point (modal)<br>';
        helpHtml += 'l: lock camera translation to focus point<br>';
        helpHtml += 'o: lock camera orientation to focus point<br>';
        helpHtml += 'drag: rotate/translate (focus) swivel (first person)<br>';
        helpHtml += 'wasdqe: translate (first person)<br>';
        helpHtml += 'wheel: translate camera</span>';
        this.SetTextPanelValue(HelpPanelName, helpHtml);
        this.AddTextPanel(StatusPanelName, "Status", "", this.statusDiv);
        this.UpdateStatusPadding();
    }

    private createToggleButton(title: string, icon: string) : [HTMLLabelElement, HTMLInputElement]
    {
        let label = document.createElement("label");
        label.className = "scenepic-toggle-button";
        label.title = title;
        let checkbox = document.createElement("input");
        checkbox.type = "checkbox";
        checkbox.checked = true;

        let span = document.createElement("span");
        span.className = "scenepic-icon-" + icon;
        label.appendChild(checkbox);
        label.appendChild(span);

        return [label, checkbox];
    }

    ResetAllViews()
    {
        for(let canvasId in this.canvases)
        {
            let canvas = this.canvases[canvasId];
            if(canvas instanceof Canvas3D)
            {
                canvas.ResetView();
            }
        }
    }

    UpdateVolume()
    {
        let volume = undefined;
        if(this.volumeToggle.checked)
        {
            volume = this.volumeSlider.valueAsNumber;
        }

        for(let canvasId in this.canvases)
        {
            let canvas : CanvasBase = this.canvases[canvasId];
            canvas.Volume = volume;
        }
    }

    Record()
    {
        this.numFramesPerCanvas = {}
        this.canvasZipFolders = {}
        this.maxFrames = 0;
        this.recordingZip = new JSZip();
        for(let canvasId in this.canvases)
        {
            this.canvasZipFolders[canvasId] = this.recordingZip.folder(canvasId);
            let canvas : CanvasBase = this.canvases[canvasId];
            canvas.IsRecording = true;
            if(canvas.FrameCount > this.maxFrames)
            {
                this.maxFrames = canvas.FrameCount;
            }

            this.numFramesPerCanvas[canvasId] = canvas.FrameCount;
        }

        this.currentRecordingFrame = 0;
        this.progressBar.style.width = "0%";
        this.progressDiv.style.visibility = "visible";
        this.recordButton.classList.add("is-blinking");
        setTimeout(() => this.recordFrame(), 1);
    }

    recordFrame()
    {
        let imageName = "frame_" + String(this.currentRecordingFrame).padStart(4, "0") + ".png";
        let allDefined = true;
        for(let canvasId in this.canvases)
        {
            let canvas : CanvasBase = this.canvases[canvasId];
            canvas.ShowFrame(this.currentRecordingFrame % this.numFramesPerCanvas[canvasId]);
            if(canvas.DataUrl != null)
            {
                let imageBlob = Misc.DataUrlToBlob(canvas.DataUrl);
                this.canvasZipFolders[canvasId].file(imageName, imageBlob);
            }
            else
            {
                console.warn("Frame " + this.currentRecordingFrame + " undefined for canvas " + canvasId);
                allDefined = false;
            }
        }

        if(allDefined){
            this.currentRecordingFrame += 1;
        }

        let percentComplete = 100 * this.currentRecordingFrame / this.maxFrames;
        this.progressBar.style.width = percentComplete + "%";
        if(this.currentRecordingFrame == this.maxFrames)
        {
            let media : Set<string> = new Set<string>();
            this.recordButton.classList.remove("is-blinking");
            for(let canvasId in this.canvases)
            {
                let canvas : CanvasBase = this.canvases[canvasId];
                if(canvas.mediaId != null){
                    media.add(canvas.mediaId);
                }

                canvas.IsRecording = false;
                canvas.ShowFrame(0);
            }

            for(let mediaId of media)
            {
                let blob = this.objectCache.GetBlob(mediaId);
                this.recordingZip.file(mediaId + "." + blob.type, blob);
            }

            this.recordingZip.generateAsync({type:"blob"})
            .then(function(content) {
                let filename = document.title.replace(' ', '_').toLowerCase() + ".zip";
                 saveAs(content, filename);
             });

             this.progressDiv.style.visibility = "hidden";
        }
        else
        {
            setTimeout(() => this.recordFrame(), 0);
        }
    }

    UpdateStatusPadding()
    {
        if (this.statusDivNeedsPadding)
            this.div.setAttribute("style", `padding-bottom:${this.statusDiv.offsetHeight}px;`);
    }

    SetStatus(status : string)
    {
        var html = "";
        html += "<span class='scenepic-textbox-content'>";
        html += status;
        html += "</span>";
        this.SetTextPanelValue(StatusPanelName, html);
        this.UpdateStatusPadding();
    }

    AddLogMessage(message : string, color : string)
    {
        if (!(LogPanelName in this.textPanels))
            this.AddTextPanel(LogPanelName, "Log", null, this.statusDiv);

        // Construct inner html
        var html = "";
        html += "<span class='scenepic-textbox-content'>";
        var d = new Date();
        html += `<span style='font-size:50%; color:grey; vertical-align:middle; display: inline-block; font-family:monospace;'>${d.getFullYear()}-${d.getMonth()}-${d.getDay()} ${d.getHours()}:${d.getMinutes()}:${d.getSeconds()}&nbsp;&nbsp;</span>`;
        html += `<span style='color:${color};vertical-align:middle; display: inline-block;'>${message}</span>`;
        html += "</span><br>";
        this.SetTextPanelValue(LogPanelName, html, true);
        this.UpdateStatusPadding();
    }

    AddWarning(message : string)
    {
        this.AddLogMessage("WARNING: " + message, "red");
    }

    ConnectToServer(address : string)
    {
        // Try to create websocket connection to server
        this.SetStatus("Connecting to " + address);
        this.ws = new WebSocket(address);
        this.ws.onerror = event => this.AddWarning("WebSockets connection to " + address + " failed to open!");
        this.ws.onmessage = event => this.MessageReceived(event);
        this.ws.onopen = event =>
        {
            this.AddLogMessage("WebSockets connection to " + address + "opened successfully", "green");
            if (this.sceneId != null)
            {
                this.SetSceneId(this.sceneId);
            }

            // Send initial frame id for all canvases
            for(var canvasId in this.canvases)
            {
                var canvas = this.canvases[canvasId];
                this.ReportFrameIdChange(canvasId, canvas.GetCurrentFrameId());
            }
        }
        this.ws.onclose = event => this.AddLogMessage("WebSockets connection to " + address + "closed", "orange");
        this.SetStatus("done");
    }

    MessageReceived(event : MessageEvent)
    {
        // Decode from json
        var timeReceived = new Date().getTime();
        var data = JSON.parse(event.data);
        var timeParsed = new Date().getTime();

        // Send acknowledgement if requested
        if ("AckData" in data)
            this.ReportReceived(data["AckData"]);

        // Get set of commands to run
        var commands = [];
        if ("SceneCommand" in data)
            commands = [data["SceneCommand"]];
        else if ("SceneCommands" in data)
            commands = data["SceneCommands"];

        // Run the commands
        this.ExecuteSceneCommands(commands);

        // Compute timings
        var jsonEncodeDuration : any = "Unknown";
        var transmitDuration : any = "Unknown";
        var jsonDecodeDuration = timeParsed - timeReceived;
        var sceneParseDuration = new Date().getTime() - timeParsed;
        if ("Timings" in data)
        {
            var timings = data["Timings"]
            var jsonEncodeStart = timings[0];
            var messageSendStart = timings[1];
            jsonEncodeDuration = messageSendStart - jsonEncodeStart;
            transmitDuration = timeReceived - messageSendStart;
        }
        this.SetStatus("Updated.  Timings: enc:" + jsonEncodeDuration + "ms, tx:" + transmitDuration + "ms, dec:" + jsonDecodeDuration + "ms, parse:" + sceneParseDuration + "ms");
    }

    private AddTextPanel(id : string, title : string, style : string, parent : HTMLElement, startMinimized = false, addInputBox = false)
    {
        // Check unique
        if (id in this.textPanels)
        {
            this.AddWarning("TextPanel id " + id + " already exists in SPScene");
            return;
        }

        // Create
        this.textPanels[id] = new TextPanel(id, title, style, parent != null ? parent : this.div, startMinimized, addInputBox);
        let panel = this.textPanels[id];

        // Add event to report
        var host = this;
        panel.inputBox.oninput = () => host.ReportInputBoxChange(id, panel.inputBox.value);
        panel.header.addEventListener("click", () => this.UpdateStatusPadding());

        // Update padding
        this.UpdateStatusPadding();
    }

    private SetTextPanelValue(id : string, htmlContents : string, append = false)
    {
        // Check exists
        if (!(id in this.textPanels))
            this.AddWarning("TextPanel id " + id + " not found. Try AddTextPanel first.");

        // Update contents
        this.textPanels[id].SetContents(htmlContents, append);
        this.UpdateStatusPadding();
    }

    private SetTextPanelTitle(id : string, title : string)
    {
        // Check exists
        if (!(id in this.textPanels))
            this.AddWarning("TextPanel id " + id + " not found. Try AddTextPanel first.");

        this.textPanels[id].SetTitle(title);
        this.UpdateStatusPadding();
    }

    private SetTextPanelInputText(id : string, text : string)
    {
        // Check exists
        if (!(id in this.textPanels))
            this.AddWarning("TextPanel id " + id + " not found. Try AddTextPanel first.");

        this.textPanels[id].SetInputBoxContents(text);
        this.UpdateStatusPadding();
    }

    private AddDropDownMenu(id : string, title : string, style : string, parent : HTMLElement)
    {
        // Check unique
        if (id in this.dropDownMenus)
        {
            this.AddWarning("DropDownMenu id " + id + " already exists in SPScene");
            return;
        }

        // Create
        this.dropDownMenus[id] = new DropDownMenu(title, style, parent != null ? parent : this.div);

        // Add event to report
        var host = this;
        let menu = this.dropDownMenus.get(id)
        
        menu.dropDownMenu.onchange = () => host.ReportDropDownMenuChange(id, menu.dropDownMenu.selectedIndex);
        menu.header.addEventListener("click", () => this.UpdateStatusPadding());

        // Update padding
        this.UpdateStatusPadding();
    }

    private SetDropDownMenuItems(id : string, items : string[])
    {
        // Check exists
        if (!(id in this.dropDownMenus))
            this.AddWarning("DropDownMenu id " + id + " not found. Try AddDropDownMenu first.");

        // Update contents
        this.dropDownMenus[id].SetItems(items);
        this.UpdateStatusPadding();
    }

    private SetDropDownMenuItemDisabled(id : string, index : number, disabled : boolean)
    {
        // Check exists
        if (!(id in this.dropDownMenus))
            this.AddWarning("DropDownMenu id " + id + " not found. Try AddDropDownMenu first.");

        // Update contents
        this.dropDownMenus[id].SetItemDisabled(index, disabled);
        this.UpdateStatusPadding();
    }

    private SetDropDownMenuSelection(id : string, index : number)
    {
        // Check exists
        if (!(id in this.dropDownMenus))
            this.AddWarning("DropDownMenu id " + id + " not found. Try AddDropDownMenu first.");

        // Update contents
        this.dropDownMenus[id].SetSelection(index);
        this.UpdateStatusPadding();
    }


    private GetParentDOMElement(htmlId : string)
    {
        var parent = htmlId != null ? document.getElementById(htmlId) : null;
        if (parent == null)
        {
            if (this.canvasDiv == null)
            {
                this.canvasDiv = document.createElement("div");
                this.canvasDiv.className = "scenepic"; // CSS style class name
                this.div.appendChild(this.canvasDiv);
            }
            parent = this.canvasDiv;
        }
        return parent;
    }

    private RequestRedraw()
    {
    }

    private AddCanvas3D(canvasId : string, width : number, height : number, parent : HTMLElement)
    {
        // Check unique
        if (canvasId in this.canvases) // Loops over keys (ids) in dictionary
        {
            this.AddWarning("Canvas with id " + canvasId + " already exists in SPScene.");
            return;
        }

        // Create canvas
        var canvas = new Canvas3D(canvasId, this.frameRate, width, height, this.allMeshes, this.objectCache, status => this.SetStatus(status), msg => this.AddWarning(msg), () => this.RequestRedraw(), (cid, fid) => this.ReportFrameIdChange(cid, fid), (cid, key) => this.HandleKeyDown(cid, key, false, false, false, false, true));

        // Store canvas
        this.InitializeCanvas(canvas, canvasId, parent, true);
    }

    private AddCanvas2D(canvasId : string, width : number, height : number, parent : HTMLElement)
    {
        // Check unique
        if (canvasId in this.canvases) // Loops over keys (ids) in dictionary
        {
            this.AddWarning("Canvas with id " + canvasId + " already exists in SPScene.");
            return;
        }

        // Create canvas
        var canvas = new Canvas2D(canvasId, this.frameRate, width, height, this.objectCache, status => this.SetStatus(status), msg => this.AddWarning(msg), () => this.RequestRedraw(), (cid, fid) => this.ReportFrameIdChange(cid, fid));

        // Store canvas
        this.InitializeCanvas(canvas, canvasId, parent, false);
    }

    private AddGraph(canvasId : string, width : number, height : number, parent : HTMLElement)
    {
        // Check unique
        if (canvasId in this.canvases) // Loops over keys (ids) in dictionary
        {
            this.AddWarning("Canvas with id " + canvasId + " already exists in SPScene.");
            return;
        }

        // Create canvas
        var canvas = new Graph(canvasId, this.frameRate, width, height, this.objectCache, status => this.SetStatus(status), msg => this.AddWarning(msg), () => this.RequestRedraw(), (cid, fid) => this.ReportFrameIdChange(cid, fid));

        // Store canvas
        this.InitializeCanvas(canvas, canvasId, parent, false);
    }

    InitializeCanvas(canvas : CanvasBase, canvasId : string,  parent : HTMLElement, pointerEvents : boolean)
    {
        // Save
        this.canvases[canvasId] = canvas;
        canvas.handlesMouse = pointerEvents;

        // Update html
        parent.appendChild(canvas.container);

        // Add event handlers
        var htmlCanvas = canvas.htmlCanvas;
        if (pointerEvents)
        {
            htmlCanvas.addEventListener("pointerdown", event => this.HandlePointerDown(event), true);
            htmlCanvas.addEventListener("pointerup", event => this.HandlePointerUp(event), true);
            htmlCanvas.addEventListener("pointermove", event => this.HandlePointerMove(event), true);
            htmlCanvas.addEventListener("pointerout", event => this.HandlePointerUp(event), true);
            htmlCanvas.addEventListener("wheel", event => this.HandleMouseWheel(event), true);
        }
        htmlCanvas.addEventListener("keydown", event => { 
            let propagate = this.HandleKeyDown(canvasId, event.key, event.altKey, event.ctrlKey, event.shiftKey, event.metaKey);
            if(!propagate)
                event.preventDefault()
        }, true);
        htmlCanvas.addEventListener("keyup", event => { this.HandleKeyUp(canvasId, event); })
        htmlCanvas.addEventListener("mousemove", event => { event.preventDefault(); }, true); // Consume drag in IE
        htmlCanvas.addEventListener("mousedown", event => { htmlCanvas.focus(); event.preventDefault(); }, true); // Consume drag in IE
        canvas.slider.oninput = event => this.SliderChanged(event);
        canvas.slider.onkeydown = event => { 
            let propagate = this.HandleKeyDown(canvasId, event.key, event.altKey, event.ctrlKey, event.shiftKey, event.metaKey); 
            if(!propagate)
                event.preventDefault()
        };
        htmlCanvas.focus(); // Ensure keypresses are dealt with

        // Add canvas id property to html canvas object for event handling
        htmlCanvas.setAttribute("SPCanvasId", canvasId);
        canvas.slider.setAttribute("SPCanvasId", canvasId);

        // Store in canvas group for event linking
        this.canvasGroups[canvasId] = { };
        this.canvasGroups[canvasId][canvasId] = true;
    }

    // Add new or replace existing mesh
    DefineMesh(meshId : string, layerId : string, definition : any, cameraSpace : boolean, doubleSided : boolean, isBillboard : boolean, isLabel : boolean)
    {
        try
        {
            var mesh = Mesh.Parse(definition);
            mesh.cameraSpace = cameraSpace;
            mesh.layerId = layerId;
            mesh.doubleSided = doubleSided;
            mesh.isBillboard = isBillboard;
            mesh.isLabel = isLabel;
            this.allMeshes[meshId] = mesh;
        }
        catch (e)
        {
            this.AddWarning(e);
        }

        // Update all canvases that are consuming this mesh
        for(var canvasId in this.canvases)
        {
            var canvas = this.canvases[canvasId];
            canvas.NotifyMeshUpdated(meshId);
        }
    }

    // Update an existing mesh to create a new mesh
    UpdateMesh(baseMeshId: string, meshId: string, buffer: Float32Array|Uint16Array, frameIndex: number, keyframeIndex: number, min: number, max: number, updateFlags: VertexBufferType)
    {
        let unquantizedBuffer: Float32Array;
        if (buffer instanceof Uint16Array) {
            let range = (max - min) / 65535.0;
            unquantizedBuffer = new Float32Array(buffer.length);
            let keyframeVertexBuffer = this.meshKeyframes[baseMeshId + keyframeIndex];
            for(let i=0; i < unquantizedBuffer.length; ++i)
            {
                unquantizedBuffer[i] = buffer[i] * range + min + keyframeVertexBuffer[i];
            }
        } else {
            unquantizedBuffer = buffer;
            this.meshKeyframes[baseMeshId + frameIndex] = unquantizedBuffer;
        }

        try
        {
            var mesh = this.allMeshes[baseMeshId];
            this.allMeshes[meshId] = mesh.Update(unquantizedBuffer, updateFlags);
        }
        catch (e)
        {
            this.AddWarning(e);
        }

        // Update all canvases that are consuming this mesh
        for(var canvasId in this.canvases)
        {
            var canvas = this.canvases[canvasId];
            canvas.NotifyMeshUpdated(meshId);
        }
    }

    // Execute commands
    ExecuteSceneCommands(command : any)
    {
        // Support recursive parsing of sub-lists of commands
        if (Array.isArray(command))
        {
            for(var com of command)
                this.ExecuteSceneCommands(com);
            return;
        }

        // Check object has CommandType value
        if (!("CommandType" in command))
            this.AddWarning("Expecting \"CommandType\" in Scene Command object: " + JSON.stringify(command));

        switch(command["CommandType"])
        {
            case "ConfigureUserInterface": // Provided for convenience - really a per canvas option
                for(var canvasId in this.canvases)
                {
                    var canvas = this.canvases[canvasId];
                    if (canvas instanceof Canvas3D)
                        canvas.ConfigureUserInterface(command);
                }
                break;

            case "SetSceneId":
                this.SetSceneId(command["SceneId"]);
                break;

            case "SetSceneProperties":
                this.frameRate = Number(command["FrameRate"]);
                this.timePerFrame = 1.0 / this.frameRate;
                this.statusDiv.style.visibility = command["StatusBarVisibility"]
                break;

            case "DefineMesh":
                var meshId = String(command["MeshId"]);
                var layerId = Misc.GetDefault(command, "LayerId", null);
                var definition = command["Definition"];
                var cameraSpace = Misc.GetDefault(command, "CameraSpace", false);
                var doubleSided = Misc.GetDefault(command, "DoubleSided", false);
                var isBillboard = Misc.GetDefault(command, "IsBillboard", false);
                var isLabel = Misc.GetDefault(command, "IsLabel", false);
                this.DefineMesh(meshId, layerId, definition, cameraSpace, doubleSided, isBillboard, isLabel);
                break;             

            case "UpdateMesh":
                var baseMeshId = String(command["BaseMeshId"]);
                var meshId = String(command["MeshId"]);
                var frameIndex = command["FrameIndex"];
                var keyframeIndex = Misc.GetDefault(command, "KeyframeIndex", frameIndex);
                var min = Misc.GetDefault(command, "MinValue", 0);
                var max = Misc.GetDefault(command, "MaxValue", 0);
                var updateFlags = command["UpdateFlags"] as number as VertexBufferType;
                var buffer: Float32Array|Uint16Array;
                if ("QuantizedBuffer" in command) 
                    buffer = Misc.Base64ToUInt16Array(command["QuantizedBuffer"])
                else
                    buffer = Misc.Base64ToFloat32Array(command["VertexBuffer"])

                this.UpdateMesh(baseMeshId, meshId, buffer, frameIndex, keyframeIndex, min, max, updateFlags);
                break;

            case "DefineImage":
                var imageId = String(command["ImageId"]);
                if ("Filename" in command)
                    this.objectCache.DefineImageFromFile(imageId, command["Filename"]);
                else
                    this.objectCache.DefineImageFromBase64(imageId, command["Type"], command["Data"]);
                break;

            case "DefineVideo":
                var video = document.createElement("video");
                video.preload = "auto";
                video.loop = true;
                var videoId = String(command["VideoId"]);
                this.media.push(video);
                this.objectCache.DefineVideoFromBase64(videoId, video, command["Type"], command["Data"]);
                break;

            case "DefineAudioTrack":
                var audio = document.createElement("audio");
                audio.preload = "auto";
                audio.loop = true;
                var audioId = String(command["AudioId"])
                this.media.push(audio);
                this.objectCache.DefineAudioTrackFromBase64(audioId, audio, command["Type"], command["Data"]);
                break;

            case "DefineLabel":
                var labelId = String(command["LabelId"]);
                var text = String(command["Text"]);
                var fillStyle = String(command["FillStyle"]);
                var fontFamily = String(command["FontFamily"]);
                var fontSizePx = Number(command["FontSizePx"]);
                var horizAlign = Number(command["HorizontalAlign"]);
                var vertAlign = Number(command["VerticalAlign"]);
                var offsetDistance = Number(command["OffsetDistance"]);
                this.objectCache.DefineLabel(labelId, text, fillStyle, fontFamily, fontSizePx, horizAlign, vertAlign, offsetDistance);
                break;

            case "AddCanvas3D":
                var canvasId = String(command["CanvasId"]);
                var width = Number(command["Width"]);
                var height = Number(command["Height"]);
                var htmlId = Misc.GetDefault(command, "HtmlId", null);
                var parent = this.GetParentDOMElement(htmlId);
                this.AddCanvas3D(canvasId, width, height, parent);
                break;

            case "AddCanvas2D":
                var canvasId = String(command["CanvasId"]);
                var width = Number(command["Width"]);
                var height = Number(command["Height"]);
                var htmlId = Misc.GetDefault(command, "HtmlId", null);
                var parent = this.GetParentDOMElement(htmlId);
                this.AddCanvas2D(canvasId, width, height, parent);
                break;

            case "AddGraph":
                var canvasId = String(command["CanvasId"])
                var width = Number(command["Width"])
                var height = Number(command["Height"])
                var htmlId = Misc.GetDefault(command, "HtmlId", null);
                var parent = this.GetParentDOMElement(htmlId);
                this.AddGraph(canvasId, width, height, parent);
                break;

            case "CanvasCommands":
                var canvasId = String(command["CanvasId"]);
                if (!(canvasId in this.canvases))
                    this.AddWarning("CanvasId " + canvasId + " not found. Try AddCanvas first.");
                else
                    this.canvases[canvasId].ExecuteCanvasCommands(command["Commands"]);
                break;

            case "LinkCanvasEvents":
                var ids = command["CanvasIds"];
                this.LinkCanvasEvents(ids);
                break;

            case "AddTextPanel":
                var textPanelId = String(command["TextPanelId"]);
                var htmlId = Misc.GetDefault(command, "HtmlId", null);
                var addInputBox = <boolean>Misc.GetDefault(command, "AddInputBox", false);
                var parent = this.GetParentDOMElement(htmlId);
                this.AddTextPanel(textPanelId, "", "", parent, false, addInputBox);
                break;

            case "DefineGrid":
                var gridWidth = String(command["Width"]);
                var gridTemplateRows = String(command["GridTemplateRows"]);
                var gridTemplateColumns = String(command["GridTemplateColumns"]);
                this.canvasDiv.style.display = "grid";
                this.canvasDiv.style.width = gridWidth;
                this.canvasDiv.style.gridTemplateRows = gridTemplateRows;
                this.canvasDiv.style.gridTemplateColumns = gridTemplateColumns;
                break;

            case "PlaceCanvasInGrid":
                var canvasId = String(command["CanvasId"]);
                var gridRow = String(command["GridRow"]);
                var gridColumn = String(command["GridColumn"]);
                var canvas = this.canvases[canvasId];
                canvas.container.style.gridRow = gridRow;
                canvas.container.style.gridColumn = gridColumn;
                break;

            case "SetTextPanelValue":
                var textPanelId = String(command["TextPanelId"]);
                var text = String(command["Value"]);
                this.SetTextPanelValue(textPanelId, text);
                break;

            case "SetTextPanelTitle":
                var textPanelId = String(command["TextPanelId"]);
                var text = String(command["Value"]);
                this.SetTextPanelTitle(textPanelId, text);
                break;

            case "SetTextPanelInputText":
                var textPanelId = String(command["TextPanelId"]);
                var text = String(command["Value"]);
                this.SetTextPanelInputText(textPanelId, text);
                break;

            case "AddDropDownMenu":
                var dropDownMenuId = String(command["DropDownMenuId"]);
                var htmlId = Misc.GetDefault(command, "HtmlId", null);
                var title = String(command["Title"]);
                var parent = this.GetParentDOMElement(htmlId);
                this.AddDropDownMenu(dropDownMenuId, title, "", parent);
                break;

            case "SetDropDownMenuItems":
                var dropDownMenuId = String(command["DropDownMenuId"]);
                var items = command["Items"].map(item => String(item));
                this.SetDropDownMenuItems(dropDownMenuId, items);
                break;

            case "SetDropDownMenuItemDisabled":
                var dropDownMenuId = String(command["DropDownMenuId"]);
                var index = Number(command["Index"]);
                var disabled = Boolean(command["Disabled"]);
                this.SetDropDownMenuItemDisabled(dropDownMenuId, index, disabled);
                break;

            case "SetDropDownMenuSelection":
                var dropDownMenuId = String(command["DropDownMenuId"]);
                var index = Number(command["Index"]);
                this.SetDropDownMenuSelection(dropDownMenuId, index);
                break;

            case "SimulateKeyPress":
                var canvasId = String(command["CanvasId"]);
                var key = String(command["Key"]);
                var alt = Misc.GetDefault(command, "Alt", false);
                var ctrl = Misc.GetDefault(command, "Ctrl", false);
                var shift = Misc.GetDefault(command, "Shift", false);
                var meta = Misc.GetDefault(command, "Meta", false);
                this.HandleKeyDown(canvasId, key, alt, ctrl, shift, meta, false);
                break;

            case "ConnectToServer":
                var url = String(command["URL"]);
                this.ConnectToServer(url);
                break;

            case "ShutDown":
                setTimeout(this.ShutDown, 0);
                break;

            case "NoOp":
                /* Do nothing */
                break;

            default:
                this.AddWarning("Unknown Scene CommandType " + command["CommandType"] + " in object: " + JSON.stringify(command));
                break;
        }
    }

    // Close down the webpage
    ShutDown()
    {
        var children = document.body.children;
        for (var i = 0; i < children.length; ++i)
        {
            document.body.removeChild(children[i]);
        }
        document.write("Session closed.  Window closing...");
        close();
    }

    // Load scripts from file
    LoadScripts(filenames : string[])
    {
        var loadJSON = (id : string, filename : string, successHandler, failureHandler) =>
        {
            var elm = document.createElement("script");
            elm.id = id;
            elm.setAttribute("type", "text/javascript");
            elm.src = filename;
            elm.onload = successHandler;
            elm.onerror = failureHandler;
            document.body.appendChild(elm);
        };

        var loadScript = (index : number) =>
        {
            if (index >= filenames.length) { this.SetStatus("done"); return; }
            var id = "__loaded-script-" + index.toString() + "__";

            var success = event =>
            {
                // Execute the script
                var commands = (<any>window).ScriptCommands;
                if (commands != null)
                    this.ExecuteSceneCommands(commands);

                // Remove the script from memory
                var elm = document.getElementById(id);
                elm.remove();

                // Load the next script
                loadScript(index + 1);
            };

            var filename = filenames[index];
            var failure = () => this.AddWarning("Failed to load " + filename);
            this.SetStatus("Loading file " + filename + "...");
            loadJSON(id, filename, success, failure);
        }

        loadScript(0);
    }

    private SetSceneId(id : string)
    {
        this.sceneId = id;
        this.ReportSceneIdChange();
        this.SetStatus("SceneId: " + this.sceneId);
    }

    // Link events across multiple canvases
    private LinkCanvasEvents(ids : string[])
    {
        for (var i = 0; i < ids.length; i++)
        {
            var group = this.canvasGroups[ids[i]];
            for (var j = 0; j < ids.length; j++)
            {
                var id = ids[j];
                if (id in group) continue;
                group[id] = true;
            }
        }
    }

    private GetEventCanvasId(event : any)
    {
        return (<HTMLElement>event.target).getAttribute("SPCanvasId");
    }

    private GetTargetCanvases(canvasId : string)
    {
        if (canvasId == null) return null;
        var canvasGroup = this.canvasGroups[canvasId];
        var canvasIds = Object.keys(canvasGroup);
        return canvasIds.map(id => <Canvas3D>this.canvases[id]);
    }

    private ReportReceived(ack_data : any)
    {
        if (this.ws != null && this.ws.readyState == WebSocket.OPEN)
        {
            var obj = { "Type" : "AcknowledgeCommand", "AckData" : ack_data };
            this.ws.send(JSON.stringify(obj));
        }
    }

    private ReportSceneIdChange()
    {
        if (this.ws != null && this.ws.readyState == WebSocket.OPEN && this.sceneId != null)
        {
            var obj = { "Type" : "SceneIdChange", "SceneId" : this.sceneId };
            this.ws.send(JSON.stringify(obj));
        }
    }

    private ReportKeyPress(key, altKey : boolean, ctrlKey : boolean, shiftKey : boolean, metaKey : boolean, canvasId : string, frameId : string)
    {
        if (this.ws != null && this.ws.readyState == WebSocket.OPEN)
        {
            var obj = { "Type" : "KeyPress", "Key" : key, "Alt" : altKey, "Ctrl" : ctrlKey, "Shift" : shiftKey, "Meta" : metaKey, "CanvasId" : canvasId };
            if (frameId != null) obj["FrameId"] = frameId;
            this.ws.send(JSON.stringify(obj));
        }
    }

    private ReportInputBoxChange(textPanelId : string, value : string)
    {
        if (this.ws != null && this.ws.readyState == WebSocket.OPEN)
        {
            var obj = { "Type" : "InputBoxChange", "TextPanelId" : textPanelId, "Value" : value };
            this.ws.send(JSON.stringify(obj));
        }
    }

    private ReportDropDownMenuChange(dropDownMenuId : string, index : number)
    {
        if (this.ws != null && this.ws.readyState == WebSocket.OPEN)
        {
            var obj = { "Type" : "DropDownMenuChange", "DropDownMenuId" : dropDownMenuId, "Index" : index };
            this.ws.send(JSON.stringify(obj));
        }
    }

    private ReportFrameIdChange(canvasId : string, frameId : string)
    {
        if (this.ws != null && this.ws.readyState == WebSocket.OPEN)
        {
            var obj = { "Type" : "FrameIdChange", "CanvasId" : canvasId, "FrameId" : frameId };
            this.ws.send(JSON.stringify(obj));
        }
    }

    private HandleKeyUp(canvasId: string, event: KeyboardEvent)
    {
        var canvases = this.GetTargetCanvases(canvasId);
        if (canvases == null) return;
        for(var canvas of canvases)
        {
            canvas.HandleKeyUp(event.key);
        }
    }

    private HandleKeyDown(canvasId : string, key : string, altKey : boolean, ctrlKey : boolean, shiftKey : boolean, metaKey : boolean, reportToServer : boolean = true)
    {
        var returnValue = true;

        var canvases = this.GetTargetCanvases(canvasId);
        if (canvases == null) return;

        var showTimings = false;
        var timeStarted = new Date().getTime();

        let toggleFirstPerson = false;
        // Only handle unmodified keypresses
        if (!altKey && !ctrlKey && !shiftKey && !metaKey)
        {
            // Global keypresses
            switch(key)
            {
                case " ":
                    this.playPauseToggle.checked = !this.playPauseToggle.checked;
                    this.PlayVideo();
                    returnValue = false;
                    break;
                case "-":
                case "_":
                    this.slower();
                    break;
                case "=":
                case "+":
                    this.faster();
                    break;
                case "Tab":
                    toggleFirstPerson = true;
                    break
            }

            // Canvas-local keypresses
            for(var canvas of canvases)
            {
                if(toggleFirstPerson)
                {
                    canvas.FirstPerson = !canvas.FirstPerson;
                }

                var result = canvas.HandleKeyDown(key);
                var handled = result[0];
                var changedFrame = result[1];
                if (changedFrame) showTimings = true; // Only show timings if actually changed frame

                // Mark event as handled
                if (handled)
                    returnValue = false;
            }
        }

        // Display timings
        if (showTimings)
        {
            var timeTaken = new Date().getTime() - timeStarted;
            this.SetStatus("Updated.  Timings: " + timeTaken + "ms");
        }

        // Send message to any open server connections for interactive sessions
        if (reportToServer)
        {
            var canvas : Canvas3D = this.canvases[canvasId];
            var frameId = canvas.GetCurrentFrameId()
            this.ReportKeyPress(key, altKey, ctrlKey, shiftKey, metaKey, canvasId, frameId);
        }

        return returnValue;
    }

    private updatePlaybackRate()
    {
        for(let canvasId in this.canvases){
            let canvas : CanvasBase = this.canvases[canvasId];
            canvas.PlaybackRate = this.playbackRate;
        }
    }

    private faster()
    {
        this.playbackRate = Math.min(2, this.playbackRate * 1.2);
        this.updatePlaybackRate();
    }

    private slower()
    {
        this.playbackRate = Math.max(0.03, this.playbackRate / 1.2);
        this.updatePlaybackRate();
    }

    private PlayVideo()
    {
        if(this.playPauseToggle.checked)
        {
            for(var canvasId in this.canvases)
            {
                let canvas : CanvasBase = this.canvases[canvasId];
                canvas.StartPlaying();
            }

            this.isPlaying = true;
            if(this.media.length > 1)
            {
                this.sync(0);
            }
        }
        else
        {
            this.isPlaying = false;
            for(var canvasId in this.canvases)
            {
                let canvas : CanvasBase = this.canvases[canvasId];
                canvas.StopPlaying();
            } 
        }
    }

    private sync(mainIndex)
    {
        let main = this.media[mainIndex];
        let timestamp = main.currentTime;
        for(let element of this.media)
        {
            if(element == main)
            {
                continue;
            }

            if(Math.abs(element.currentTime - timestamp) < this.timePerFrame)
            {
                continue;
            }

            if(element.readyState === 4)
            {
                element.currentTime = timestamp;
            }
        }

        if(this.isPlaying)
        {
            requestAnimationFrame(() => this.sync(mainIndex));
        }
    }

    private SliderChanged(event : Event)
    {
        // Get new value
        var slider = <HTMLInputElement>event.target;
        var value = slider.valueAsNumber;

        // Get parent canvas
        var canvases = this.GetTargetCanvases(this.GetEventCanvasId(event));
        if (canvases == null) return;

        // Loop over attached canvases
        for(var canvas of canvases)
        {
            canvas.ShowFrame(value);
        }
    }

    private HandlePointerDown(event : PointerEvent)
    {
        var targetCanvasId = this.GetEventCanvasId(event);
        var canvases = this.GetTargetCanvases(targetCanvasId);
        if (canvases == null) return;
        var targetCanvas = (<Canvas3D>this.canvases[targetCanvasId]);
        var clientRect = targetCanvas.htmlCanvas.getBoundingClientRect();

        this.pointerCoords[event.pointerId] = [event.clientX - clientRect.left, event.clientY - clientRect.top];
        this.initPointerCoords[event.pointerId] = [event.clientX - clientRect.left, event.clientY - clientRect.top];

        this.HandlePointerMove(event);
    }

    private HandlePointerUp(event : PointerEvent)
    {
        var canvases = this.GetTargetCanvases(this.GetEventCanvasId(event));
        if (canvases == null) return;

        delete this.pointerCoords[event.pointerId];
        delete this.initPointerCoords[event.pointerId];

        for(var canvas of canvases)
        {
            if (!canvas.handlesMouse)
                continue;

            canvas.SetCameraRotationalVelocity(0, 0);
        }
    }

    private HandlePointerMove(event : PointerEvent)
    {
        var targetCanvasId = this.GetEventCanvasId(event);
        var canvases = this.GetTargetCanvases(targetCanvasId);
        if (canvases == null) return;
        var targetCanvas = (<Canvas3D>this.canvases[targetCanvasId]);
        var clientRect = targetCanvas.htmlCanvas.getBoundingClientRect();

        // Determine number of pointers (e.g. multi-touch)
        var countPointers = Object.keys(this.pointerCoords).length;
        if (countPointers == 0) return;

        var old = this.pointerCoords[event.pointerId];
        var oldX = old[0];
        var oldY = old[1];

        var newX = event.clientX - clientRect.left;
        var newY = event.clientY - clientRect.top;

        var deltaX = newX - oldX;
        var deltaY = newY - oldY;

        // Handle twists specially
        var twistAngle = 0.0;
        if (event.ctrlKey)
            twistAngle = targetCanvas.ComputeCameraTwist(oldX, oldY, newX, newY);

        for(var canvas of canvases)
        {
            if (!canvas.handlesMouse)
                continue;

            if (event.altKey)
            {
                deltaX *= canvas.pointerAltKeyMultiplier;
                deltaY *= canvas.pointerAltKeyMultiplier;
            }

            if(canvas.FirstPerson)
            {
                let init = this.initPointerCoords[event.pointerId];
                let initX = init[0];
                let initY = init[1];

                let diffX = newX - initX;
                let diffY = newY - initY;

                let length = Math.sqrt(diffX * diffX + diffY * diffY);
                let deadZone = 10;
                if(length > deadZone)
                {
                    let scale = Math.min(2, (length - deadZone) / deadZone);
                    diffX *= scale / length;
                    diffY *= scale / length;
                }
                else
                {
                    diffX = 0;
                    diffY = 0;
                }

                canvas.SetCameraRotationalVelocity(diffX * canvas.pointerRotationSpeed, diffY * canvas.pointerRotationSpeed);
            }
            else
            {
                // Deal with basic events
                if (event.ctrlKey) // Treat as twist of camera
                {
                    canvas.RotateCamera(0.0, 0.0, twistAngle);
                }
                else if (canvas.showFocusPoint) // Translate the 3D center of rotation
                {
                    canvas.SetFocusPointPositionFromPixelCoordinates(newX, newY);
                }
                else if (event.shiftKey || countPointers > 1) // Treat as translation of camera
                {
                    var delta = canvas.ComputeFocusPointRelativeViewSpaceTranslation(oldX, oldY, newX, newY);
                    canvas.TranslateCamera(delta);
                }
                else // Treat as rotation of camera about center of rotation
                {
                    canvas.RotateCamera(deltaY * canvas.pointerRotationSpeed, deltaX * canvas.pointerRotationSpeed, 0.0); // NB y and x are deliberately crossed over
                }

                // Deal with pinch-zoom
                if (countPointers == 2)
                {
                    // Get other coordinate
                    var otherX : number, otherY : number;
                    for (var pid in this.pointerCoords)
                    {
                        if (pid == event.pointerId.toString()) continue;
                        otherX = this.pointerCoords[pid][0];
                        otherY = this.pointerCoords[pid][1];
                    }

                    // Compute delta between two points
                    var oldDX = oldX - otherX;
                    var oldDY = oldY - otherY;
                    var oldDist = Math.sqrt(oldDX*oldDX + oldDY*oldDY);
                    var newDX = newX - otherX;
                    var newDY = newY - otherY;
                    var newDist = Math.sqrt(newDX*newDX + newDY*newDY);

                    // Change in distances
                    var zOld = canvas.GetCurrentFocusPointInViewSpace()[2];
                    var zNew = zOld * oldDist / newDist;
                    canvas.TranslateCamera(vec3.fromValues(0.0, 0.0, zNew - zOld));
                }
            }
        }

        // Store updated coords
        this.pointerCoords[event.pointerId] = [newX, newY];

        event.preventDefault();
    }

    private HandleMouseWheel(event : WheelEvent)
    {
        var canvases = this.GetTargetCanvases(this.GetEventCanvasId(event));
        if (canvases == null) return;

        for(var canvas of canvases)
        {
            if (!canvas.handlesMouse)
                continue;

            
            var deltaZ = -event.deltaY * canvas.mouseWheelTranslationSpeed;

            if (event.altKey)
                deltaZ *= canvas.pointerAltKeyMultiplier;

            var delta = vec3.fromValues(0.0, 0.0, deltaZ);

            canvas.TranslateCamera(delta);
        }

        event.preventDefault();
    }
}
