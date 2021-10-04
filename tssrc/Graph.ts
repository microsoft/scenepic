import * as $ from "jquery"
import Misc from "./Misc"
import {ObjectCache, CanvasBase} from "./CanvasBase"


class Rect
{
    top : number;
    right : number;
    bottom : number;
    left : number;

    constructor(top: number, right: number, bottom : number, left : number)
    {
        this.top = top;
        this.right = right;
        this.bottom = bottom;
        this.left = left;
    }
}

class Sparkline
{
    static readonly MARKER_RADIUS = 4;

    values : Float32Array;
    name : string;
    strokeStyle : string;
    lineWidth : number;
    minValue : number;
    maxValue : number;

    constructor(name : string, values : Float32Array, strokeStyle : string, lineWidth : number)
    {
        this.name = name;
        this.values = values;
        this.strokeStyle = strokeStyle;
        this.lineWidth = lineWidth;
        this.minValue = Math.min(...values);
        this.maxValue = Math.max(...values);
    }

    Draw(context : CanvasRenderingContext2D, box : Rect, frameIndex : number)
    {
        context.beginPath();
        let xScale = (box.right - box.left) / this.values.length;
        let yScale = 0;
        let yOffset = 0.5 * (box.top + box.bottom);
        if(this.minValue != this.maxValue){
            yOffset = box.bottom;
            yScale = (box.bottom - box.top)/(this.maxValue - this.minValue);
        }

        let x = box.left;
        let y = yOffset - (this.values[0] - this.minValue) * yScale;
        context.moveTo(x, y);
        for(let value of this.values.slice(1))
        {            
            x += xScale;
            y = yOffset - (value - this.minValue) * yScale;
            context.lineTo(x, y);
        }

        if (this.strokeStyle != null && this.lineWidth > 0)
        {
            context.strokeStyle = this.strokeStyle;
            context.lineWidth = this.lineWidth;
            context.stroke();
        }

        x = box.left + xScale * frameIndex;
        y = yOffset - (this.values[frameIndex] - this.minValue) * yScale;
        context.beginPath();
        context.arc(x, y, Sparkline.MARKER_RADIUS * window.devicePixelRatio, 0.0, 2.0 * Math.PI, false);
        context.fillStyle = this.strokeStyle;
        context.fill();
    }

    ValueAt(frameIndex : number) : number
    {
        return this.values[frameIndex];
    }
}

export default class Graph extends CanvasBase
{
    context : CanvasRenderingContext2D = null; // Rendering context

    backgroundStyle : string; // Background color

    fontFamily : string;
    sizeInPixels : number;

    sparklines : Sparkline[] = []; // The sparklines

    margin : Rect = null;

    static readonly PADDING = 7;

    constructor(canvasId : string, public frameRate : number, public width : number, public height : number, objectCache : ObjectCache, public SetStatus : (status : string) => void, public SetWarning : (message : string) => void, public RequestRedraw : () => void, public ReportFrameIdChange : (canvasId : string, frameId : string) => void)
    {
        // Base class constructor
        super(canvasId, frameRate, width, height, objectCache, SetStatus, SetWarning, RequestRedraw, ReportFrameIdChange);

        // Setup 2D context for drawing
        this.context = this.htmlCanvas.getContext('2d');

        this.backgroundStyle = "#000000";
        this.margin = new Rect(10, 10, 10, 10);

        // Start render loop
        this.StartRenderLoop();
    }

    // Execute a single canvas command
    ExecuteCanvasCommand(command : any)
    {
        switch(command["CommandType"])
        {
            case "SetBackgroundStyle":
                this.backgroundStyle = String(command["Value"]);
                break;

            case "SetTextStyle":
                this.fontFamily = String(command["FontFamily"])
                this.sizeInPixels = Number(command["SizeInPixels"])
                break;

            case "SetMargin":
                let margin = command["Value"];
                let top = Number(margin["Top"]);
                let right = Number(margin["Right"]);
                let bottom = Number(margin["Bottom"]);
                let left = Number(margin["Left"]);
                this.margin = new Rect(top, right, bottom, left);
                break;

            case "AddSparkline":               
                let name = String(command["Name"]);
                let values = Misc.Base64ToFloat32Array(command["ValueBuffer"]);
                let strokeStyle = String(command["StrokeStyle"]);
                let lineWidth = Number(command["LineWidth"]);
                if(this.sparklines.length == 0){
                    for(let i=0; i<values.length; ++i)
                    {
                        this.AddFrame(i.toString());
                    }
                }

                this.sparklines.push(new Sparkline(name, values, strokeStyle, lineWidth));
                break;

            default:
                super.ExecuteCanvasCommand(command);
                break;
        }
    }

    AllocateFrame()
    {        
    }

    DeallocateFrame(frameIndex : number)
    {        
    }

    // Render scene method
    Render()
    {
        // Clear
        this.context.fillStyle = this.backgroundStyle;
        this.context.fillRect(0, 0, this.htmlCanvas.clientWidth * window.devicePixelRatio, this.htmlCanvas.clientHeight * window.devicePixelRatio);

        let width = this.htmlCanvas.clientWidth - this.margin.left - this.margin.right;
        let height = this.htmlCanvas.clientHeight - this.margin.top - this.margin.bottom;
        let left = this.margin.left * window.devicePixelRatio;
        let top = this.margin.top * window.devicePixelRatio;
        width *= window.devicePixelRatio;
        height *= window.devicePixelRatio;
        let padding = Graph.PADDING * window.devicePixelRatio;

        this.context.font = this.sizeInPixels + "px " + this.fontFamily;
        let nameColumnWidth = 0;
        let valueColumnWidth = 0;
        for(let sparkline of this.sparklines){
            let nameSize = this.context.measureText(sparkline.name).width;
            let minValSize = this.context.measureText(sparkline.minValue.toFixed(2)).width;
            let maxValSize = this.context.measureText(sparkline.maxValue.toFixed(2)).width;
            nameColumnWidth = Math.max(nameSize, nameColumnWidth);
            valueColumnWidth = Math.max(minValSize, maxValSize, valueColumnWidth);
        }

        let rowHeight = height / this.sparklines.length;
        let boxWidth = width - nameColumnWidth - valueColumnWidth - 2 * padding;
        let boxHeight = rowHeight - 2 * padding;
        let y = top;
        let boxLeft = left + nameColumnWidth + padding;
        let boxRight = boxLeft + boxWidth - padding;
        for(let sparkline of this.sparklines){
            let box = new Rect(y + padding, boxRight, y + boxHeight, boxLeft);
            sparkline.Draw(this.context, box, this.currentFrameIndex);

            this.context.fillStyle = sparkline.strokeStyle;
            let textSize = this.context.measureText(sparkline.name);
            let text_top = y + window.devicePixelRatio * 0.5 * rowHeight;
            let text_left = left + window.devicePixelRatio * 0.5 * (nameColumnWidth - textSize.width);
            this.context.fillText(sparkline.name, text_left, text_top);
            
            let value = sparkline.ValueAt(this.currentFrameIndex).toFixed(2);
            textSize = this.context.measureText(value);
            text_left = boxRight + window.devicePixelRatio * 0.5 * (valueColumnWidth - textSize.width)
            this.context.fillText(value, text_left, text_top);

            y += rowHeight;
        }
    }
}
