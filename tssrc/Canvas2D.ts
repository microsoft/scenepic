import Misc from "./Misc"
import {ObjectCache, CanvasBase} from "./CanvasBase"

abstract class Primitive
{
    readonly layerId : string;

    constructor(layerId: string)
    {
        this.layerId = layerId;
    }

    GetObjectCacheId() : string { return null; }
    abstract Draw(HTMLCanvasElement, CanvasRenderingContext2D, ObjectCache);
}

class LinesPrimitive extends Primitive
{
    readonly strokeStyle : string;
    readonly lineWidth : number;
    readonly fillStyle : string;
    readonly coordinates : Float32Array;
    readonly closePath : boolean;

    constructor(strokeStyle : string, lineWidth : number, fillStyle : string, coordinates : Float32Array, closePath : boolean, layerId : string)
    {
        super(layerId);
        this.strokeStyle = strokeStyle;
        this.lineWidth = lineWidth;
        this.fillStyle = fillStyle;
        this.coordinates = coordinates;
        this.closePath = closePath;
    }
    
    Draw(canvas : HTMLCanvasElement, context : CanvasRenderingContext2D, objectCache : ObjectCache)
    {
        context.beginPath();
        context.moveTo(this.coordinates[0] * window.devicePixelRatio, this.coordinates[1] * window.devicePixelRatio);
        for(var i = 0; i < this.coordinates.length; i += 2)
            context.lineTo(this.coordinates[i] * window.devicePixelRatio, this.coordinates[i + 1] * window.devicePixelRatio);
        if (this.closePath)
            context.closePath();
        if (this.fillStyle != null)
        {
            context.fillStyle = this.fillStyle;
            context.fill();
        }
        if (this.strokeStyle != null && this.lineWidth > 0)
        {
            context.strokeStyle = this.strokeStyle;
            context.lineWidth = this.lineWidth;
            context.stroke();
        }
    }
}

class CirclePrimitive extends Primitive
{
    readonly strokeStyle : string;
    readonly lineWidth : number;
    readonly fillStyle : string;
    readonly center : Float32Array;
    readonly radius : number;

    constructor(strokeStyle : string, lineWidth : number, fillStyle : string, center : Float32Array, radius : number, layerId : string)
    {
        super(layerId);
        this.strokeStyle = strokeStyle;
        this.lineWidth = lineWidth;
        this.fillStyle = fillStyle;
        this.center = center;
        this.radius = radius;
    }
    
    Draw(canvas : HTMLCanvasElement, context : CanvasRenderingContext2D, objectCache : ObjectCache)
    {
        context.beginPath();
        context.arc(this.center[0] * window.devicePixelRatio, this.center[1] * window.devicePixelRatio, this.radius * window.devicePixelRatio, 0.0, 2.0 * Math.PI, false);
        if (this.fillStyle != null)
        {
            context.fillStyle = this.fillStyle;
            context.fill();
        }
        if (this.strokeStyle != null && this.lineWidth > 0)
        {
            context.strokeStyle = this.strokeStyle;
            context.lineWidth = this.lineWidth;
            context.stroke();
        }
    }
}

class TextPrimitive extends Primitive
{
    readonly text : string;
    readonly fillStyle : string;
    readonly sizeInPixels : number;
    readonly fontFamily : string;
    readonly position : Float32Array;

    constructor(text : string, fillStyle : string, sizeInPixels : number, fontFamily : string, position : Float32Array, layerId : string)
    {
        super(layerId);
        this.text = text;
        this.fillStyle = fillStyle;
        this.sizeInPixels = sizeInPixels;
        this.fontFamily = fontFamily;
        this.position = position;
    }

    Draw(canvas : HTMLCanvasElement, context : CanvasRenderingContext2D, objectCache : ObjectCache)
    {
        context.fillStyle = this.fillStyle;
        let sizeInPixels = this.sizeInPixels * window.devicePixelRatio;
        let left = this.position[0] * window.devicePixelRatio;
        let bottom = this.position[1] * window.devicePixelRatio;
        let font = sizeInPixels.toString() + "px " + this.fontFamily;
        context.font = font;
        context.fillText(this.text, left, bottom);
    }
}

class ImagePrimitive extends Primitive
{
    readonly imageId : string;
    readonly positionType : string;
    readonly position : Float32Array
    readonly scale : number;
    readonly smoothed : boolean;

    constructor(imageId : string, positionType : string, position : Float32Array, scale : number, smoothed : boolean, layerId : string)
    {
        super(layerId);
        this.imageId = imageId;
        this.positionType = positionType;
        this.position = position;
        this.scale = scale;
        this.smoothed = smoothed;
    }

    GetObjectCacheId()
    {
        return this.imageId;
    }

    Draw(canvas : HTMLCanvasElement, context : CanvasRenderingContext2D, objectCache : ObjectCache)
    {
        // Get current image
        var currentImage = objectCache.GetObject(this.imageId);
        if (currentImage == null) return; // Indicates image not available (e.g. not completely loaded yet)

        // Turn off image smoothing
        context.imageSmoothingEnabled = (<any>context).msImageSmoothingEnabled = this.smoothed;

        // Compute destination rectangle
        var centered = true;
        var scaleX = canvas.clientWidth / currentImage.naturalWidth;
        var scaleY = canvas.clientHeight / currentImage.naturalHeight;
        switch(this.positionType)
        {
            case "fill":
                scaleX = scaleY = Math.max(scaleX, scaleY);
                break;
            case "fit":
                scaleX = scaleY = Math.min(scaleX, scaleY);
                break;
            case "stretch":
                // Do nothing
                break;
            case "manual":
                scaleX = scaleY = this.scale;
                centered = this.position == null;
                break;
        }
        var width = currentImage.naturalWidth * scaleX;
        var height = currentImage.naturalHeight * scaleY;
        var x = centered ? (canvas.clientWidth - width) / 2.0 : this.position[0];
        var y = centered ? (canvas.clientHeight - height) / 2.0 : this.position[1];

        // Draw
        var pixelRatio = window.devicePixelRatio;
        context.drawImage(currentImage, 0, 0, currentImage.naturalWidth, currentImage.naturalHeight, x * pixelRatio, y * pixelRatio, width * pixelRatio, height * pixelRatio);
    }
}

class VideoPrimitive extends Primitive
{
    readonly videoId : string;
    readonly positionType : string;
    readonly position : Float32Array
    readonly scale : number;
    readonly smoothed : boolean;

    constructor(videoId : string, positionType : string, position : Float32Array, scale : number, smoothed : boolean, layerId : string)
    {
        super(layerId);
        this.videoId = videoId;
        this.positionType = positionType;
        this.position = position;
        this.scale = scale;
        this.smoothed = smoothed;
    }

    GetObjectCacheId()
    {
        return this.videoId;
    }

    Draw(canvas : HTMLCanvasElement, context : CanvasRenderingContext2D, objectCache : ObjectCache)
    {
        // Get current image
        let currentVideo : HTMLVideoElement = objectCache.GetObject(this.videoId);
        if (currentVideo == null) return; // Indicates video not available (e.g. not completely loaded yet)

        // Turn off image smoothing
        context.imageSmoothingEnabled = (<any>context).msImageSmoothingEnabled = this.smoothed;

        // Compute destination rectangle
        let centered = true;
        let scaleX = canvas.clientWidth / currentVideo.videoWidth;
        let scaleY = canvas.clientHeight / currentVideo.videoHeight;
        switch(this.positionType)
        {
            case "fill":
                scaleX = scaleY = Math.max(scaleX, scaleY);
                break;
            case "fit":
                scaleX = scaleY = Math.min(scaleX, scaleY);
                break;
            case "stretch":
                // Do nothing
                break;
            case "manual":
                scaleX = scaleY = this.scale;
                centered = this.position == null;
                break;
        }
        let width = currentVideo.videoWidth * scaleX;
        let height = currentVideo.videoHeight * scaleY;
        let x = centered ? (canvas.clientWidth - width) / 2.0 : this.position[0];
        let y = centered ? (canvas.clientHeight - height) / 2.0 : this.position[1];

        // Draw
        let pixelRatio = window.devicePixelRatio;
        context.drawImage(currentVideo, 0, 0, currentVideo.videoWidth, currentVideo.videoHeight, x * pixelRatio, y * pixelRatio, width * pixelRatio, height * pixelRatio);
    }
}


export default class Canvas2D extends CanvasBase
{
    context : CanvasRenderingContext2D = null; // Rendering context

    backgroundStyle : string; // Background color

    // Dictionary from primitiveId to layer settings (wireframe, filled, opacity)
    layerSettings : {[layerId: string]: {[key: string]: number|boolean}} = {};
    layerIds : string[] = [];
    globalFill = true;
    globalOpacity = 1.0;

    framePrimitives : Primitive[][] = []; // The primitives for each frame [frameIndex]

    currentPrimitives : Primitive[] = null; // The primitives for the currently displayed frame

    frameCoordinates : Float32Array[] = []; // The coordinates for each frame [frameIndex]

    constructor(canvasId : string, public frameRate : number, public width : number, public height : number, objectCache : ObjectCache, public SetStatus : (status : string) => void, public SetWarning : (message : string) => void, public RequestRedraw : () => void, public ReportFrameIdChange : (canvasId : string, frameId : string) => void)
    {
        // Base class constructor
        super(canvasId, frameRate, width, height, objectCache, SetStatus, SetWarning, RequestRedraw, ReportFrameIdChange);

        // Setup 2D context for drawing
        this.context = this.htmlCanvas.getContext('2d');

        this.backgroundStyle = "#000000";

        // Start render loop
        this.StartRenderLoop();
    }

    // Adds UI for the control of certain layers
    SetLayerSettings(layerSettings : any)
    {
        // Delete any previous controls
        while (this.dropdownTable.childElementCount > 2)
            this.dropdownTable.removeChild(this.dropdownTable.lastChild);

        this.layerSettings = layerSettings;
        this.layerIds = [null];
        for(let layerId in layerSettings){
            this.layerIds.push(layerId);
        }

        // Add header row
        var BorderStyle = "1px solid #cccccc";
        var headerRow = document.createElement("tr");
        headerRow.style.borderBottom = BorderStyle;
        for(var headerName of ["Visible", "Opacity", "Layer Id"])
        {
            var headerItem = document.createElement("th");
            headerItem.className = "scenepic-dropdown-header";
            headerItem.innerHTML = headerName;
            headerRow.appendChild(headerItem);
        }
        this.dropdownTable.appendChild(headerRow);

        // Create row helper function
        var createRow = (id, label) =>
        {
            // Create fill checkbox
            var checkboxFill = document.createElement("input");
            checkboxFill.type = "checkbox";
            checkboxFill.checked = this.ShowLayerFilled(id);
            checkboxFill.addEventListener("change", event => { this.SetLayerFilled(id, checkboxFill.checked); this.PrepareBuffers(); event.stopPropagation(); });

            // Create opacity slider
            var sliderOpacity = document.createElement("input");
            sliderOpacity.type = "range";
            sliderOpacity.min = "0";
            sliderOpacity.max = "100";
            sliderOpacity.value = String(100.0 * this.GetLayerOpacity(id));
            sliderOpacity.addEventListener("change", event => { this.SetLayerOpacity(id, Number(sliderOpacity.value) / 100.0); this.PrepareBuffers(); event.stopPropagation(); })
            sliderOpacity.style.width = "50px";
            sliderOpacity.style.height = "10px";

            // Add layer label
            var labelLayer = document.createElement("label");
            labelLayer.appendChild(document.createTextNode(label));

            return [checkboxFill, sliderOpacity, labelLayer];
        };

        var addRow = (rowItems, border) =>
        {
            // Create table row
            var tr = document.createElement("tr");
            if (border)
                tr.style.borderTop = BorderStyle;
            var addControl = (el, className) =>
            {
                var td = document.createElement("td");
                td.appendChild(el);
                td.className = className;
                tr.appendChild(td);
            };
            addControl(rowItems[0], "scenepic-table-control");
            addControl(rowItems[1], "scenepic-table-control");
            addControl(rowItems[2], "scenepic-table-layerid");
            this.dropdownTable.appendChild(tr);
        };

        // Add new controls
        Object.keys(layerSettings).forEach(id => addRow(createRow(id, id), false));
        addRow(createRow("<<<GLOBAL>>>", "Global"), true);
    }

    ShowLayerFilled(layerId : string) : boolean
    {
        if (layerId == "<<<GLOBAL>>>")
            return this.globalFill;
        if (layerId == null)
            return true;
        if (!(layerId in this.layerSettings))
            return true;
        if ("filled" in this.layerSettings[layerId] && !this.layerSettings[layerId]["filled"])
            return false;
        return true;
    }

    SetLayerFilled(layerId : string, filled : boolean)
    {
        if (layerId == null)
            return;

        if (layerId == "<<<GLOBAL>>>")
        {
            this.globalFill = filled;
        }
        else
        {
            if (!(layerId in this.layerSettings))
                this.layerSettings[layerId] = {};

            this.layerSettings[layerId]["filled"] = filled;
        }

        // Prepare buffers and force redraw
        this.PrepareBuffers();
    }

    GetLayerOpacity(layerId : string) : number
    {
        if (layerId == "<<<GLOBAL>>>")
            return this.globalOpacity;
        if (layerId == null || !(layerId in this.layerSettings) || !("opacity" in this.layerSettings[layerId]))
            return 1.0; // Opaque
        else
            return <number>this.layerSettings[layerId]["opacity"];
    }

    SetLayerOpacity(layerId : string, opacity : number)
    {
        if (layerId == null)
            return;

        if (layerId == "<<<GLOBAL>>>")
        {
            this.globalOpacity = opacity;
        }
        else
        {
            if (!(layerId in this.layerSettings))
                this.layerSettings[layerId] = {};
            this.layerSettings[layerId]["opacity"] = opacity;
        }

        // Prepare buffers and force redraw
        this.PrepareBuffers();
    }    

    GetLayerRenderOrder(layerId : string) : number
    {
        if (layerId == null || !(layerId in this.layerSettings) || !("renderOrder" in this.layerSettings[layerId]))
            return -1e3; // Backmost layer - bit of a hack
        else
            return <number>this.layerSettings[layerId]["renderOrder"];
    }

    // Execute a single canvas command
    ExecuteCanvasCommand(command : any)
    {
        switch(command["CommandType"])
        {
            case "SetBackgroundStyle":
                this.backgroundStyle = String(command["Value"]);
                break;

            case "SetLayerSettings":
                var layerSettings = Misc.GetDefault(command, "Value", null);
                if (layerSettings != null)
                    this.SetLayerSettings(layerSettings);
                break;

            case "SetLayers":
                this.layerIds = command["LayerIds"];
                break;

            default:
                super.ExecuteCanvasCommand(command);
                break;
        }
    }

    AddPrimitive(frameIndex : number, primitive : Primitive)
    {
        if(primitive.layerId == null){
            this.framePrimitives[frameIndex].push(primitive);
        }else{
            if(!(primitive.layerId in this.layerSettings)){
                this.layerSettings[primitive.layerId] = {};
                this.SetLayerSettings(this.layerSettings);
            }

            let renderOrder = this.GetLayerRenderOrder(primitive.layerId);
            let index = 0;
            let lastId : string = null;
            for(let other of this.framePrimitives[frameIndex]){
                if(other.layerId == lastId){
                    index += 1;
                    continue;
                }
                
                if(other.layerId == primitive.layerId){
                    break;
                }

                lastId = other.layerId;
                let otherOrder = this.GetLayerRenderOrder(other.layerId);
                if(renderOrder < otherOrder){
                    break;
                }

                index += 1;
            }
            this.framePrimitives[frameIndex].splice(index, 0, primitive);
        }

        if (this.currentFrameIndex == frameIndex)
            this.PrepareBuffers();    
    }

    SetCoordinates(frameIndex: number, coordinates: Float32Array)
    {
        this.frameCoordinates[frameIndex] = coordinates;
    }

    GetCoordinate(frameIndex: number, index: number)
    {
        let start = index * 2;
        let end = start + 2;
        return this.frameCoordinates[frameIndex].slice(start, end);
    }

    AddLines(frameIndex: number, lines: Uint16Array, style: Uint8Array, width: Float32Array, layerIds: [string, number][])
    {
        let numLines = width.length;
        let layerId = null;
        let nextLayer = numLines;
        let currentLayer = 0;
        if(layerIds != null){
            layerId = layerIds[0][0];
            nextLayer = currentLayer + layerIds[0][1];
        }

        for(let i=0; i<numLines; ++i)
        {
            let info = lines.slice(i*3, i*3 + 3);
            let coordinates = this.frameCoordinates[frameIndex].slice(info[0]*2, info[1]*2);
            let closePath = info[2] == 1;
            let strokeStyle = Misc.StyleToHtmlHex(style.slice(i*8, i*8 + 4));
            let fillStyle = Misc.StyleToHtmlHex(style.slice(i*8 + 4, i*8 + 8));
            let lineWidth = width[i];
            if(i == nextLayer){
                currentLayer += 1;
                layerId = layerIds[currentLayer][0];
                nextLayer = i + layerIds[currentLayer][1]
            }

            this.AddPrimitive(frameIndex, new LinesPrimitive(strokeStyle, lineWidth, fillStyle, coordinates, closePath, layerId))
        }
    }

    AddCircles(frameIndex: number, circles: Float32Array, style: Uint8Array, layerIds: [string, number][])
    {
        let numCircles = style.length / 8;
        let layerId = null;
        let nextLayer = numCircles;
        let currentLayer = 0;
        if(layerIds != null){
            layerId = layerIds[0][0];
            nextLayer = currentLayer + layerIds[0][1];
        }

        for(let i=0; i<numCircles; ++i)
        {
            let info = circles.slice(i*4, i*4 + 4);
            let center = info.slice(0, 2);
            let radius = info[2];
            let lineWidth = info[3];
            let strokeStyle = Misc.StyleToHtmlHex(style.slice(i*8, i*8 + 4));
            let fillStyle = Misc.StyleToHtmlHex(style.slice(i*8 + 4, i*8 + 8));
            if(i == nextLayer){
                currentLayer += 1;
                layerId = layerIds[currentLayer][0];
                nextLayer = i + layerIds[currentLayer][1]
            }

            this.AddPrimitive(frameIndex, new CirclePrimitive(strokeStyle, lineWidth, fillStyle, center, radius, layerId))
        }
    }

    // Execute a single frame command
    ExecuteFrameCommand(command : any, frameIndex : number)
    {
        switch(command["CommandType"])
        {
            case "SetCoordinates":
                var coordinates = Misc.Base64ToFloat32Array(command["CoordinateBuffer"])
                this.SetCoordinates(frameIndex, coordinates);
                break;

            case "AddImage":
                var imageId = command["ImageId"];
                var index = command["Index"];
                var positionType = Misc.GetDefault(command, "PositionType", "fit");
                var position = this.GetCoordinate(frameIndex, index);
                var scale = Misc.GetDefault(command, "Scale", 1.0);
                var smoothed = Misc.GetDefault(command, "Smoothed", false);
                var layerId = Misc.GetDefault(command, "LayerId", null);
                this.AddPrimitive(frameIndex, new ImagePrimitive(imageId, positionType, position, scale, smoothed, layerId));
                break;

            case "AddVideo":
                var index = command["Index"];
                var positionType = Misc.GetDefault(command, "PositionType", "fit");
                var position = this.GetCoordinate(frameIndex, index);
                var scale = Misc.GetDefault(command, "Scale", 1.0);
                var smoothed = Misc.GetDefault(command, "Smoothed", false);
                var layerId = Misc.GetDefault(command, "LayerId", null);
                this.AddPrimitive(frameIndex, new VideoPrimitive(this.mediaId, positionType, position, scale, smoothed, layerId));
                break;

            case "AddLines":
                var lines = Misc.Base64ToUInt16Array(command["InfoBuffer"]);
                var style = Misc.Base64ToUInt8Array(command["StyleBuffer"]);
                var width = Misc.Base64ToFloat32Array(command["WidthBuffer"]);
                var layerIds = Misc.GetDefault(command, "LayerIds", null);
                this.AddLines(frameIndex, lines, style, width, layerIds);
                break;

            case "AddCircles":
                var circles = Misc.Base64ToFloat32Array(command["InfoBuffer"]);
                var style = Misc.Base64ToUInt8Array(command["StyleBuffer"]);
                var layerIds = Misc.GetDefault(command, "LayerIds", null);
                this.AddCircles(frameIndex, circles, style, layerIds);
                break;

            case "AddText":
                var text = command["Text"];
                var fillStyle = command["FillStyle"];
                var sizeInPixels = command["SizeInPixels"]
                var fontFamily = command["Font"];
                var index = command["Index"];
                var position = this.GetCoordinate(frameIndex, index);
                var layerId = Misc.GetDefault(command, "LayerId", null);
                this.AddPrimitive(frameIndex, new TextPrimitive(text, fillStyle, sizeInPixels, fontFamily, position, layerId));
                break;
            default:
                super.ExecuteFrameCommand(command, frameIndex);
                break;
        }
    }

    AllocateFrame()
    {
        this.framePrimitives.push([]);
        this.frameCoordinates.push(null);
    }

    DeallocateFrame(frameIndex : number)
    {
        this.framePrimitives[frameIndex] = [];
        this.frameCoordinates[frameIndex] = null;
    }

    NotifyTextureUpdated(textureId : string)
    {
    }

    PrepareBuffers() : void
    {
        super.PrepareBuffers();

        var currentFrameIndex = this.currentFrameIndex;

        // Get new primitives
        var newPrimitives = this.framePrimitives[currentFrameIndex].slice();

        // Deal with image caching (only applies if primitives are actually ImagePrimitives or VideoPrimitives)
        for(var primitive of newPrimitives)
        {
            var cacheId = primitive.GetObjectCacheId();
            if (cacheId != null) this.objectCache.AddUser(cacheId);
        }
        if (this.currentPrimitives != null)
        {
            for(var primitive of this.currentPrimitives)
            {
                var cacheId = primitive.GetObjectCacheId();
                if (cacheId != null) this.objectCache.RemoveUser(cacheId);
            }
        }

        // Store current primitive
        this.currentPrimitives = newPrimitives;
    }

    // Render scene method
    Render()
    {
        // Clear
        this.context.fillStyle = this.backgroundStyle;
        this.context.fillRect(0, 0, this.htmlCanvas.clientWidth * window.devicePixelRatio, this.htmlCanvas.clientHeight * window.devicePixelRatio);

        // Composite primitives
        if (this.currentPrimitives == null) return;
        for(var primitive of this.currentPrimitives){
            let opacity = this.GetLayerOpacity(primitive.layerId) * this.globalOpacity;
            let filled = this.ShowLayerFilled(primitive.layerId) && this.globalFill;
            if(!filled || opacity == 0.0){
                continue;
            }

            this.context.globalAlpha = opacity;
            primitive.Draw(this.htmlCanvas, this.context, this.objectCache);
            this.context.globalAlpha = 1.0;
        }
    }

    StartPlaying()
    {
        // pre-cache
        for(let primitives of this.framePrimitives){
            for(let primitive of primitives){
                let cacheId = primitive.GetObjectCacheId();
                if(cacheId != null)
                {
                    this.objectCache.AddUser(cacheId);
                }
            }
        }

        super.StartPlaying();
    }

    StopPlaying()
    {
        super.StopPlaying();        

        // free up the cache
        for(let primitives of this.framePrimitives){
            for(let primitive of primitives){
                let imageId = primitive.GetObjectCacheId();
                if(imageId != null)
                {
                    this.objectCache.RemoveUser(imageId);
                }
            }
        }
    }
}
