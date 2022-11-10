import Misc from "./Misc"

export abstract class CachedObject
{
    object : any; // The object that is created and cached on demand
    countUsers : number; // Set of current users of htmlImage
    cachePermanently : boolean;

    // Constructor
    constructor(cachePermanently : boolean = false)
    {
        this.object = null;
        this.countUsers = 0;
        this.cachePermanently = cachePermanently;
    }

    // Add a new user of the image
    AddUser(callback : () => void)
    {
        // Store the user if new
        this.countUsers += 1;

        // Check whether already cached as htmlImage
        if (this.object != null)
            return;

        this.GenerateObject(callback);
    }

    // Remove a user of the image
    RemoveUser()
    {
        // Remove the user
        this.countUsers -= 1;

        // Check whether to keep cached permanently
        if (this.cachePermanently)
            return;

        // Retire the html image after small delay (giving chance for reuse)
        var self : CachedObject  = this;
        setTimeout(function()
        {
            if (self.countUsers == 0)
            {
                self.object = null;
            }
        }, 50);
    }

    abstract GenerateObject(callback : () => void);

    abstract GetBlob() : Blob;

    GetObject()
    {
        return this.object;
    }
}

export class CachedImage extends CachedObject
{
    src : string; // The image source, either as a base64 encoded string or as a path to a file on disk
 
    // Constructor simply stores the source
    constructor(src : string, cachePermanently : boolean = false)
    {
        super(cachePermanently);
        this.src = src;
    }

    // Generate cached object
    GenerateObject(callback : () => void)
    {
        // Create cached htmlImage
        this.object = new Image();
        this.object.addEventListener("load", callback);
        this.object.src = this.src;
    }

    GetObject()
    {
        if (this.object == null || !(<HTMLImageElement>this.object).complete)
            return null;
        else
            return this.object;
    }

    GetBlob()
    {
        return Misc.DataUrlToBlob(this.src);
    }
}

export class CachedAudioTrack extends CachedObject
{
    src : string;
    audio : HTMLAudioElement;

    // Constructor simply stores the source
    constructor(audio : HTMLAudioElement, src : string, cachePermanently : boolean = false)
    {
        super(cachePermanently);
        this.audio = audio;
        this.src = src;
    }

    // Generate cached object
    GenerateObject(callback : () => void)
    {
        this.object = this.audio;
        this.object.addEventListener("loadeddata", callback);
        let blobUrl = URL.createObjectURL(this.GetBlob());
        this.object.src = blobUrl;

    }

    GetObject()
    {
        if (this.object == null)
            return null;
        else
            return this.object;
    }

    GetBlob()
    {
        return Misc.DataUrlToBlob(this.src);
    }
}

export class CachedVideo extends CachedObject
{
    src : string;
    video : HTMLVideoElement;

    // Constructor simply stores the source
    constructor(video: HTMLVideoElement, src : string, cachePermanently : boolean = false)
    {
        super(cachePermanently);
        this.video = video;
        this.src = src;
    }

    // Generate cached object
    GenerateObject(callback : () => void)
    {
        this.object = this.video;
        this.object.addEventListener("loadeddata", () => {
            callback();
        });
        let blobUrl = URL.createObjectURL(this.GetBlob());
        this.object.src = blobUrl;
    }

    GetObject()
    {
        if (this.object == null)
            return null;
        else
            return this.object;
    }

    GetBlob()
    {
        return Misc.DataUrlToBlob(this.src);
    }
}


// A textual label
export class CachedLabel extends CachedObject
{
    text : string;
    fillStyle : string;
    fontFamily : string;
    fontSizePx : number;
    horizAlign : number;
    vertAlign : number;
    offsetDistance : number;

    // Constructor simply stores the source
    constructor(text : string, fillStyle: string, fontFamily : string, fontSizePx : number, horizAlign : number, vertAlign : number, offsetDistance : number, cachePermanently : boolean = false)
    {
        super(cachePermanently);

        this.text = text;
        this.fillStyle = fillStyle;
        this.fontFamily = fontFamily;
        this.fontSizePx = fontSizePx;
        this.horizAlign = horizAlign;
        this.vertAlign = vertAlign;
        this.offsetDistance = offsetDistance;
    }

    GenerateObject(callback : () => void)
    {
        var HeightCompensationFudgeFactor = 1.2; // Hack since no way to get required height

        // Create cached bitmap
        var textCanvas = document.createElement("canvas");
        var textContext = textCanvas.getContext("2d");
        var font = (this.fontSizePx * window.devicePixelRatio).toString() + "px " + this.fontFamily;
        textContext.font = font;
        var width = textContext.measureText(this.text).width;
        var height = HeightCompensationFudgeFactor * this.fontSizePx * window.devicePixelRatio;
        textCanvas.width = width;
        textCanvas.height = height;
        textContext.textAlign = "center";
        textContext.textBaseline = "middle";
        textContext.clearRect(0, 0, width, height);
        textContext.fillStyle = this.fillStyle;
        textContext.font = font; // REQUIRED!!
        textContext.fillText(this.text, width / 2, height / 2);

        // Return canvas
        this.object = textCanvas;
        this.object.desiredWidthPixels = width; // Will be used in WebGLMeshBuffers to normalize for size
        this.object.desiredHeightPixels = height; // Will be used in WebGLMeshBuffers to normalize for size
        this.object.proportionTranslateScreenX = -0.5 * this.horizAlign;
        this.object.proportionTranslateScreenY = -0.5 * this.vertAlign / HeightCompensationFudgeFactor;
        this.object.offsetDistance = this.offsetDistance;
        callback();
    }

    GetBlob() : Blob
    {
        throw new Error("Not Implemented");
    }
}

export class ObjectCache
{
    cachedObjects : any;
    owners : {[id: string]: string} = {};
    token : number = 0;
    callback : (string) => void;

    constructor(callback : (string) => void)
    {
        this.cachedObjects = {}
        this.callback = callback;
    }

    AddUser(id : string)
    {
        if (id in this.cachedObjects)
            this.cachedObjects[id].AddUser(() => this.callback(id));
    }

    RemoveUser(id : string)
    {
        if (id in this.cachedObjects)
            this.cachedObjects[id].RemoveUser();
    }

    DefineImageFromSrc(id : string, src : string, cachePermanently : boolean)
    {
        this.cachedObjects[id] = new CachedImage(src, cachePermanently);
    }

    DefineImageFromFile(id : string, src : string)
    {
        this.DefineImageFromSrc(id, src, true); // For now, we always permanently cache images loaded explicitly from disk
    }

    DefineImageFromBase64(id : string, type : string, dataBase64 : string)
    {
        this.DefineImageFromSrc(id, "data:" + type + ";base64," + dataBase64, false);
    }

    DefineAudioTrackFromBase64(id : string, audio: HTMLAudioElement, type : string, dataBase64 : string)
    {
        this.cachedObjects[id] = new CachedAudioTrack(audio, "data:" + type + ";base64," + dataBase64, true);
    }

    DefineVideoFromBase64(id : string, video : HTMLVideoElement, type : string, dataBase64 : string)
    {
        this.cachedObjects[id] = new CachedVideo(video, "data:" + type + ";base64," + dataBase64, true);
    }

    DefineLabel(id : string, text : string, fillStyle: string, fontFamily : string, fontSizePx : number, horizAlign : number, vertAlign : number, offsetDistance : number)
    {
        this.cachedObjects[id] = new CachedLabel(text, fillStyle, fontFamily, fontSizePx, horizAlign, vertAlign, offsetDistance, false);
    }

    Acquire(id : string) : string
    {
        if(id in this.owners)
        {
            return null;
        }

        let token = this.token.toString()
        this.token += 1;
        this.owners[id] = token
        return token;        
    }

    Release(id : string, token : string) : boolean
    {
        if(id in this.owners && this.owners[id] == token)
        {
            delete this.owners[id];
            return true;
        }

        return false;
    }

    GetObject(id : string)
    {

        if (!(id in this.cachedObjects))
            return null;
        else
            return this.cachedObjects[id].GetObject();
    }

    GetBlob(id : string)
    {
        if (!(id in this.cachedObjects))
            return null;
        else
            return this.cachedObjects[id].GetBlob();
    }
}

export abstract class CanvasBase
{
    objectCache : ObjectCache = null // Maps from ids to image/bitmap objects

    static TIME_PER_REDRAW = 1.0/60;
    static FPS_DECAY = 0.5;
    static FPS_FREQUENCY = 200;

    container : HTMLDivElement;
    public htmlCanvas : HTMLCanvasElement;
    sliderContainer : HTMLDivElement;
    slider : HTMLInputElement;
    sliderTextBox : HTMLSpanElement;
    sliderTextBoxLength : number = 0;
    sliderTextBoxPadding = 5; // In pixels

    dropdown : HTMLDivElement;
    dropdownTable : HTMLTableElement;

    cameraModeCounterMax : number = 60;
    cameraModeCounter : number = 0;
    cameraModeDisplay : HTMLDivElement;

    // FPS
    fpsToggle : HTMLInputElement;
    fpsDisplay : HTMLDivElement;
    fps : number;
    numFrames : number;
    startTime: number;
    lastFrameTime: number;

    // media
    mediaElement : HTMLMediaElement = null;
    mediaToken : string;
    startMedia : boolean = false;
    mediaId : string = null;
    globalVolume : number | undefined = 1.0;
    volumeContainer : HTMLTableRowElement;
    volumeInput : HTMLInputElement;
    volumeToggle : HTMLInputElement;
    seekTarget : number = null;

    // playback controls
    isPlaying: boolean = false;
    startPlayback: boolean = false;
    stopPlayback: boolean = false;
    playbackRate : number = 1;


    get Volume() : number | undefined
    {
        return this.globalVolume;
    }

    set Volume(volume: number | undefined)
    {
        this.globalVolume = volume;
        this.UpdateVolume();
    }

    get PlaybackRate() : number | undefined
    {
        if(this.mediaElement == null)
        {
            return undefined;
        }

        return this.mediaElement.playbackRate;
    }

    set PlaybackRate(playbackRate: number)
    {
        if(this.mediaElement != null)
        {
            this.mediaElement.playbackRate = playbackRate;
        }

        this.playbackRate = playbackRate;
        this.fps = 0
    }

    handlesMouse : boolean;
    firstPerson : boolean = false;

    currentFrameIndex = 0; // Current frame index
    lastRedraw = 0;
    dataUrl : string = null;
    isRecording : boolean = false;

    get IsRecording() : boolean
    {
        return this.isRecording;
    }

    set IsRecording(isRecording : boolean)
    {
        this.dataUrl = null;
        this.isRecording = isRecording;
    }

    get DataUrl() : string
    {
        return this.dataUrl;
    }

    // Frames
    gotoFrame = null; // If none null, then this frame will be shown (one time only)
    frameIdToIndexMap : any = {};
    frameIds : string[] = []; // Inverse of frameIdToIndexMap

    constructor(canvasId : string, public frameRate : number, public width : number, public height : number, objectCache : ObjectCache, public SetStatus : (status : string) => void, public SetWarning : (message : string) => void, public RequestRedraw : () => void, public ReportFrameIdChange : (canvasId : string, frameId : string) => void)
    {
        // Store image
        this.objectCache = objectCache;

        // Create span
        this.container = document.createElement("div");
        this.container.className = "scenepic-canvas-container";
        
        // Create html canvas
        this.htmlCanvas = document.createElement("canvas");
        this.htmlCanvas.style.width = width.toString() + "px";
        this.htmlCanvas.style.height = height.toString() + "px";
        this.htmlCanvas.id = canvasId;
        this.htmlCanvas.tabIndex = 1;
        this.htmlCanvas.className = "scenepic-canvas";
        this.container.appendChild(this.htmlCanvas);

        // Create dropdown menu
        this.dropdown = document.createElement("div");
        this.dropdown.className = "scenepic-dropdown";
        this.dropdown.addEventListener("click", event => { 
            if (event.target == this.dropdown) { 
                this.dropdown.classList.toggle("is-active");
                this.htmlCanvas.focus(); 
            } 
        });
        this.dropdownTable = document.createElement("table");
        this.dropdownTable.className = "scenepic-dropdown-table";
        this.dropdown.appendChild(this.dropdownTable);
        this.container.appendChild(this.dropdown);
        this.htmlCanvas.addEventListener("click", () => {
             this.dropdown.classList.remove("is-active");
             this.htmlCanvas.focus(); 
        });

        // Create slider
        this.slider = document.createElement("input");
        this.slider.type = "range";
        this.slider.min = "0";
        this.slider.max = "0";
        this.slider.valueAsNumber = this.currentFrameIndex;
        this.slider.className = "scenepic-slider";
        this.slider.addEventListener("change", event => this.ReportFrameIdChange(this.htmlCanvas.id, this.GetCurrentFrameId()));
        this.sliderTextBox = document.createElement("span");
        this.sliderTextBox.className = "scenepic-slider-text";
        this.sliderTextBox.style.padding = this.sliderTextBoxPadding.toString() + "px";
        this.sliderContainer = document.createElement("div");
        this.sliderContainer.className = "scenepic-slider-container";
        this.sliderContainer.style.visibility = "hidden";
        this.sliderContainer.appendChild(this.sliderTextBox);
        this.sliderContainer.appendChild(this.slider);
        this.container.appendChild(this.sliderContainer);

        this.addFPSUI();
        this.addVolumeUI();
        this.addCameraModeUI();

        // Search for a goto frame command
        this.gotoFrame = parseInt(Misc.GetSearchValue("frame"));
    }

    private addCameraModeUI()
    {
        this.cameraModeDisplay = document.createElement("div");
        this.cameraModeDisplay.className = "scenepic-camera-mode";
        this.cameraModeDisplay.style.visibility = "hidden";
        this.container.appendChild(this.cameraModeDisplay);
    }

    private addFPSUI()
    {
        // Create FPS container
        let container = document.createElement("tr");
        this.fpsToggle = document.createElement("input");
        this.fpsToggle.type = "checkbox";
        this.fpsToggle.checked = false;
        this.fpsToggle.addEventListener("change", () => this.UpdateFPSDisplay());

        let checkbox = document.createElement("td");
        checkbox.style.textAlign = "center";
        checkbox.appendChild(this.fpsToggle);
        let label = document.createElement("td");
        label.innerText = "FPS";
        container.appendChild(checkbox);
        container.appendChild(label);
        this.dropdownTable.appendChild(container);

        this.fpsDisplay = document.createElement("div");
        this.fpsDisplay.className = "scenepic-fps";
        this.fpsDisplay.style.visibility = "hidden";
        this.container.appendChild(this.fpsDisplay);

        this.fps = 0.0
    }

    private addVolumeUI()
    {
        this.volumeInput = document.createElement("input");
        this.volumeInput.type = "range";
        this.volumeInput.min = "0";
        this.volumeInput.max = "1";
        this.volumeInput.value = "0.5";
        this.volumeInput.step = "0.01";
        this.volumeInput.addEventListener("change", () => this.UpdateVolume());

        this.volumeToggle = document.createElement("input");
        this.volumeToggle.type = "checkbox";
        this.volumeToggle.checked = true;
        this.volumeToggle.addEventListener("change", () => this.UpdateVolume());

        this.volumeContainer = document.createElement("tr");
        this.volumeContainer.style.visibility = "collapse";
        let checkbox = document.createElement("td");
        checkbox.style.textAlign = "center";
        checkbox.appendChild(this.volumeToggle);
        let label = document.createElement("td");
        label.innerText = "Audio";
        let slider = document.createElement("td");
        slider.appendChild(this.volumeInput);
        this.volumeContainer.appendChild(checkbox);
        this.volumeContainer.appendChild(label);
        this.volumeContainer.appendChild(slider);
        this.dropdownTable.appendChild(this.volumeContainer);
    }

    SetMedia(mediaId: string)
    {
        this.mediaId = mediaId;
        this.mediaElement = this.objectCache.GetObject(mediaId);
        if(this.mediaElement != null)
        {
            this.mediaToken = this.objectCache.Acquire(mediaId);
            this.volumeContainer.style.visibility = "inherit";
            this.UpdateVolume();
        }
        else
        {
            console.warn("Invalid mediaId:" + mediaId);
        }
    }

    UpdateFPSDisplay()
    {
        if(this.fpsToggle.checked)
        {
            this.fpsDisplay.style.visibility = "visible";
        }
        else
        {
            this.fpsDisplay.style.visibility = "hidden";
        }
    }

    UpdateVolume()
    {
        if(this.mediaElement == null)
        {
            return;
        }

        if(this.globalVolume == undefined)
        {
            this.mediaElement.volume = 0.0;
        }
        else
        {
            if(this.volumeToggle.checked)
            {
                this.mediaElement.volume = this.volumeInput.valueAsNumber * this.globalVolume;
            }
            else
            {
                this.mediaElement.volume = 0;
            }
        }
    }

    get FrameCount() : number
    {
        return this.frameIds.length;
    }

    get FirstPerson() : boolean
    {
        return this.firstPerson;
    }

    set FirstPerson(firstPerson: boolean)
    {
        if(firstPerson != this.firstPerson)
        {
            this.cameraModeCounter = this.cameraModeCounterMax;
            this.firstPerson = firstPerson;
        }        
    }

    HandleKeyUp(key: string)
    {        
    }

    HandleKeyDown(key : string)
    {
        var handled = true;
        var changedFrame = true;

        switch(key.toLowerCase())
        {
            case "arrowleft":
            case "left":
                changedFrame = this.ShowPreviousFrame();
                break;
            case "arrowright":
            case "right":
                changedFrame = this.ShowNextFrame();
                break;
            case "arrowup":
            case "up":
            case "arrowdown":
            case "down":
                // Currently do nothing other than mark event as handled
                break;
            case "home":
                changedFrame = this.ShowFrame(0);
                break;
            case "end":
                changedFrame = this.ShowFrame(Number.MAX_VALUE);
                break;
            default:
                handled = false;
                break;
        }

        return [handled, changedFrame];
    }


    // Execute possibly multiple nested canvas commands
    ExecuteCanvasCommands(command : any)
    {
        // Support recursive parsing of sub-lists of commands
        if (Array.isArray(command))
        {
            for(var com of command)
                this.ExecuteCanvasCommands(com);
            return;
        }

        // Check object has CommandType value
        if (!("CommandType" in command))
            this.SetWarning("Expecting \"CommandType\" in Canvas Command object: " + JSON.stringify(command));

        // Execute a single command
        this.ExecuteCanvasCommand(command);
    }

    // Execute a single canvas command (likely to be called via sub-class call to super.ExecuteCanvasCommand)
    ExecuteCanvasCommand(command : any)
    {
        switch(command["CommandType"])
        {
            case "AddFrame":
                var frameId = String(command["FrameId"]);
                this.AddFrame(frameId);
                break;

            case "FrameCommands":
                var frameId = String(command["FrameId"]);
                if (!(frameId in this.frameIdToIndexMap))
                    this.SetWarning("FrameId " + frameId + " not found. Try AddFrame first.");
                else
                    this.ExecuteFrameCommands(frameId, command["Commands"]);
                break;

            case "SetMedia":
                var mediaId = String(command["MediaId"]);
                this.objectCache.AddUser(mediaId);
                this.SetMedia(mediaId);
                break;

            default:
                this.SetWarning("Unknown Canvas CommandType " + command["CommandType"] + " in object: " + JSON.stringify(command));
                break;
        }
    }

    // Execute possibly multiple nested frame commands
    ExecuteFrameCommands(frameId : string, command : any)
    {
        // Support recursive parsing of sub-lists of commands
        if (Array.isArray(command))
        {
            for(var com of command)
                this.ExecuteFrameCommands(frameId, com);
            return;
        }

        // Check object has CommandType value
        if (!("CommandType" in command))
            this.SetWarning("Expecting \"CommandType\" in Frame Command object: " + JSON.stringify(command));

        // Execute a single command
        var frameIndex = <number>this.frameIdToIndexMap[frameId];
        this.ExecuteFrameCommand(command, frameIndex);
    }

    // Execute a single frame command (likely to be called via sub-class call to super.ExecuteFrameCommand)
    ExecuteFrameCommand(command : any, frameIndex : number)
    {
        switch(command["CommandType"])
        {
            case "ClearFrame":
                this.ClearFrame(frameIndex);
                break;
           
            case "ShowFrame":
                this.ShowFrame(frameIndex);
                break;

            default:
                this.SetWarning("Unknown Frame CommandType " + command["CommandType"] + " in object: " + JSON.stringify(command));
                break;
        }
    }
    
    AddFrame(frameId : string)
    {
        // Check unique
        if (frameId in this.frameIdToIndexMap)
        {
            //this.SetWarning("Frame id " + frameId + " already exists in SPCanvas.");
            // Simply ignore frame redefinitions
            return;
        }

        var frameIndex = this.frameIds.length;
        this.frameIdToIndexMap[frameId] = frameIndex;
        this.frameIds.push(frameId);

        // Call optional derived-class initialization method
        this.AllocateFrame();

        // Update slider
        this.slider.max = frameIndex.toString();
        this.sliderContainer.style.visibility = frameIndex > 0 ? "visible" : "hidden";

        // Goto frame
        if (this.gotoFrame != null && this.gotoFrame == frameIndex)
        {
            this.ShowFrame(frameIndex);
            this.gotoFrame = null;
        }

        // Update frameid text
        if (this.currentFrameIndex == frameIndex)
        {
            this.UpdateSliderTextBox(frameId);
            this.ReportFrameIdChange(this.htmlCanvas.id, this.GetCurrentFrameId());
        }
    }

    abstract AllocateFrame() : void;
    abstract DeallocateFrame(frameIndex : number) : void;
    NotifyMeshUpdated(meshId : string) {}
    NotifyTextureUpdated(meshId : string) {}
    
    ClearFrame(frameIndex : number)
    {
        // Check frame exists
        if (frameIndex >= this.frameIds.length)
            return;

        this.DeallocateFrame(frameIndex);
        this.PrepareBuffers();
    }

    GetCurrentFrameId()
    {
        if (this.frameIds == null || this.frameIds.length == 0)
            return null;
        return this.frameIds[this.currentFrameIndex];
    }

    // Shows a frame in the canvas
    ShowFrame(frameIndex : number)
    {
        if (this.currentFrameIndex == frameIndex)
        {
            // Only update if needed (avoids slider value change potentially causing infinite loop)
            return;
        }

        if(this.mediaElement != null)
        {
            if(this.mediaToken != null)
            {
                // this canvas owns the media file, so we update the timestamp
                let newTime = frameIndex / this.frameRate;
                if(this.isPlaying)
                {
                    let frameDiff = frameIndex - this.currentFrameIndex;
                    if(frameDiff < 0 || frameDiff > 3)
                    {
                        this.mediaElement.currentTime = newTime;
                    }
                }
                else
                {
                    this.mediaElement.currentTime = newTime;
                }
            }
        }

        this.currentFrameIndex = Math.min(frameIndex, this.frameIds.length - 1);
        this.slider.valueAsNumber = this.currentFrameIndex;

        var frameId = this.GetCurrentFrameId();
        this.UpdateSliderTextBox(frameId);
        this.ReportFrameIdChange(this.htmlCanvas.id, frameId);

        // Rebuild buffers (just in case)
        this.PrepareBuffers();

        return true;
    }

    ShowPreviousFrame()
    {
        var newFrameIndex = this.currentFrameIndex - 1;
        if (newFrameIndex == -1) // Wrap-around
            newFrameIndex = this.frameIds.length - 1;
        return this.ShowFrame(newFrameIndex);
    }

    ShowNextFrame()
    {
        var newFrameIndex = this.currentFrameIndex + 1;
        if (newFrameIndex >= this.frameIds.length) {
            // Wrap-around
            newFrameIndex = 0;
        }

        return this.ShowFrame(newFrameIndex);
    }

    UpdateSliderTextBox(frameId : string)
    {
        // Update text
        this.sliderTextBox.innerHTML = frameId;

        // Update min-width
        if (frameId.length > this.sliderTextBoxLength)
        {
            this.sliderTextBoxLength = frameId.length;
            var width = this.sliderTextBox.offsetWidth - 2 * this.sliderTextBoxPadding;
            this.sliderTextBox.style.minWidth = width.toString() + "px";
        }
    }
    
    // Rebuild buffers
    protected PrepareBuffers() : void
    {
    }

    // Render method
    abstract Render() : void;

    stopRenderLoop = false;

    StopRenderLoop()
    {
        this.stopRenderLoop = true;
    }

    StartRenderLoop()
    {
        this.stopRenderLoop = false;
        requestAnimationFrame(() => this.Tick());
    }

    // Render loop
    Tick()
    {
        // Check for disable flag
        if (this.stopRenderLoop)
            return;

        // Ensure gl canvas has correct size for 1:1 resolution with display
        var pixelRatio = window.devicePixelRatio;
        var displayWidth = Math.floor(this.htmlCanvas.clientWidth * pixelRatio);
        var displayHeight = Math.floor(this.htmlCanvas.clientHeight * pixelRatio);
        if (this.htmlCanvas.width != displayWidth || this.htmlCanvas.height != displayHeight)
        {
            this.htmlCanvas.width = displayWidth;
            this.htmlCanvas.height = displayHeight;
        }

        let timestamp = Date.now();

        if(this.cameraModeCounter > 0)
        {
            this.cameraModeDisplay.style.opacity = (this.cameraModeCounter / this.cameraModeCounterMax).toString();
            this.cameraModeDisplay.innerText = this.firstPerson ? "First Person" : "Focus";
            this.cameraModeCounter -= 1;
        }

        if(this.startPlayback)
        {
            this.startPlayback = false;
            this.fps = 0.0;
            this.isPlaying = true;
            this.startTime = timestamp;
            this.numFrames = 0;
            this.lastFrameTime = timestamp;
            this.startMedia = true;
        }

        if(this.stopPlayback)
        {
            this.startMedia = false;
            this.stopPlayback = false;
            this.isPlaying = false;
            this.fpsDisplay.innerText = "";
            if(this.mediaElement != null)
            {
                this.mediaElement.pause();
            }
        }

        if(this.isPlaying)
        {            
            let newFrameIndex : number;

            if(this.mediaElement != null)
            {
                newFrameIndex = Math.round(this.mediaElement.currentTime * this.frameRate);
                if (newFrameIndex >= this.frameIds.length)
                {
                    this.mediaElement.currentTime = 0;
                }
            }
            else
            {
                let frameDuration = 1000 / (this.playbackRate * this.frameRate);
                let timestamp = Date.now();
                let elapsed = timestamp - this.lastFrameTime;
                newFrameIndex = this.currentFrameIndex;
                while(elapsed > frameDuration)
                {
                    newFrameIndex += 1;
                    elapsed -= frameDuration;
                    this.lastFrameTime += frameDuration;
                    if (newFrameIndex >= this.frameIds.length)
                    {
                        // Wrap-around
                        newFrameIndex = 0;
                    }
                }        
            }

            if(newFrameIndex != this.currentFrameIndex)
            {
                this.ShowFrame(newFrameIndex);
                this.numFrames += 1;
                let duration = timestamp - this.startTime;
                if(duration >= CanvasBase.FPS_FREQUENCY)
                {
                    let sample = (1000 * this.numFrames) / (timestamp - this.startTime);
                    this.fps = this.fps * CanvasBase.FPS_DECAY + (1 - CanvasBase.FPS_DECAY) * sample;
                    this.numFrames = 0;
                    this.startTime = timestamp;
                    this.fpsDisplay.innerText = this.fps.toFixed(2) + " fps";
                }
            }
        }

        if(this.startMedia && this.mediaElement != null)
        {
            this.startMedia = false;
            this.mediaElement.playbackRate = this.playbackRate;
            this.mediaElement.play();
        }

        // Ensure this function is called again at the next sensible timeslot
        requestAnimationFrame(() => this.Tick());
  
        if(timestamp - this.lastRedraw > CanvasBase.TIME_PER_REDRAW)
        {
            // Render
            this.Render();
            this.lastRedraw = timestamp;
        }

        if(this.isRecording)
        {
            this.dataUrl = this.htmlCanvas.toDataURL();
        }
    }

    StartPlaying()
    {
        this.startPlayback = true;
        this.stopPlayback = false;
    }

    StopPlaying()
    {
        this.stopPlayback = true;
        this.startPlayback = false;
    }
}
