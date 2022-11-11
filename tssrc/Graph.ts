import Misc from "./Misc"
import { ObjectCache, CanvasBase } from "./CanvasBase"

class Margin {
    readonly left: number;
    readonly top: number;
    readonly right: number;
    readonly bottom: number;

    constructor(top: number, right: number, bottom: number, left: number) {
        this.top = top;
        this.right = right;
        this.bottom = bottom;
        this.left = left;
    }
}


class Rect {
    x: number;
    y: number;
    width: number;
    height: number;

    constructor(x = 0, y = 0, width = 0, height = 0) {
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
    }

    get topEdge() : number {
        return this.y;
    }

    get bottomEdge() : number {
        return this.y + this.height;
    }

    get leftEdge() : number {
        return this.x;
    }

    get rightEdge() : number {
        return this.x + this.width;
    }

    Move(x: number, y: number) {
        this.x = x;
        this.y = y;
    }

    Resize(width: number, height: number) {
        this.width = width;
        this.height = height;
    }

    Pad(padding: number): Rect {
        return new Rect(this.x + padding, this.y + padding,
            this.width - 2 * padding, this.height - 2 * padding);
    }

    Outline(context: CanvasRenderingContext2D) {
        context.save();
        context.strokeStyle = "#000000";
        context.strokeRect(this.x, this.y, this.width, this.height);
        context.restore();
    }

    DrawText(context: CanvasRenderingContext2D, text: string, fillStyle: string, font: string) {
        context.save();
        context.font = font;
        context.fillStyle = fillStyle;
        let measurement = context.measureText(text);
        let textWidth = measurement.width * window.devicePixelRatio;
        let textHeight = (measurement.actualBoundingBoxAscent + measurement.actualBoundingBoxDescent) * window.devicePixelRatio
        let x = this.x + 0.5 * (this.width - textWidth);
        let y = this.y + 0.5 * (this.height + textHeight);
        context.fillText(text, x, y);
        context.restore();
    }
}

class Layout {
    top: Rect | Layout;
    bottom: Rect | Layout;
    left: Rect | Layout;
    right: Rect | Layout;
    center: Rect | Layout;

    x: number;
    y: number;
    width: number;
    height: number;

    get topEdge() : number {
        return this.y;
    }

    get bottomEdge() : number {
        return this.y + this.height;
    }

    get leftEdge() : number {
        return this.x;
    }

    get rightEdge() : number {
        return this.x + this.width;
    }

    constructor({top = new Rect(),
                 right = new Rect(),
                 bottom = new Rect(),
                 left = new Rect(),
                 center = new Rect()} : {top? : Rect | Layout, right? : Rect | Layout, bottom? : Rect | Layout, left? : Rect | Layout, center? : Rect | Layout} = {}) {
        this.x = this.y = this.width = this.height = 0;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
        this.left = left;
        this.center = center;
    }

    Move(x: number, y: number) {
        this.top.Move(x + this.top.x - this.x, y + this.top.y - this.y);
        this.bottom.Move(x + this.bottom.x - this.x, y + this.bottom.y - this.y);
        this.left.Move(x + this.left.x - this.x, y + this.left.y - this.y);
        this.right.Move(x + this.right.x - this.x, y + this.right.y - this.y);
        this.center.Move(x + this.center.x - this.x, y + this.center.y - this.y);
        this.x = x;
        this.y = y;
    }

    Outline(context: CanvasRenderingContext2D) {
        let panels = [this.top, this.bottom, this.left, this.right, this.center];
        for (let panel of panels) {
            panel.Outline(context);
        }
    }

    Layout(width: number, height: number) {
        if (this.top instanceof Layout) {
            this.top.Layout(0, 0);
        }

        this.top.Move(0, 0);

        if (this.left instanceof Layout) {
            this.left.Layout(0, 0);
        }

        this.left.Move(0, this.top.bottomEdge);

        if (this.right instanceof Layout) {
            this.right.Layout(0, 0);
        }

        this.right.Move(this.left.rightEdge, this.top.bottomEdge);

        if (this.bottom instanceof Layout) {
            this.bottom.Layout(0, 0);
        }

        this.bottom.Move(0, this.height);

        let centerWidth = Math.max(0, width - this.left.width - this.right.width);
        let centerHeight = Math.max(Math.max(this.left.height, this.right.height), height - this.top.height - this.bottom.height);

        if (this.center instanceof Layout) {
            this.center.Layout(centerWidth, centerHeight);
        } else {
            this.center.Resize(centerWidth, centerHeight);
        }

        this.width = this.left.width + this.center.width + this.right.width;
        this.height = this.top.height + this.center.height + this.bottom.height;
        this.center.Move(this.left.rightEdge, this.top.bottomEdge);
        this.top.Move(0.5 * (this.width - this.top.width), 0);
        this.left.Move(0, 0.5 * (this.height - this.left.height));
        this.right.Move(this.center.rightEdge, 0.5 * (this.height - this.right.height));
        this.bottom.Move(0.5 * (this.width - this.bottom.width), this.center.bottomEdge);
    }
}

class VerticalRule {
    frame: number;
    strokeStyle: string;
    lineWidth: number;

    constructor(frame: number, strokeStyle: string, lineWidth: number) {
        this.frame = frame;
        this.strokeStyle = strokeStyle;
        this.lineWidth = lineWidth;
    }
}

class Sparkline {
    static readonly MARKER_RADIUS = 4;

    values: Float32Array;
    name: string;
    strokeStyle: string;
    lineWidth: number;
    minValue: number;
    maxValue: number;
    verticalRules: VerticalRule[];

    constructor(name: string, values: Float32Array, strokeStyle: string, lineWidth: number, verticalRules: VerticalRule[]) {
        this.name = name;
        this.values = values;
        this.strokeStyle = strokeStyle;
        this.lineWidth = lineWidth;
        this.minValue = Math.min(...values);
        this.maxValue = Math.max(...values);
        this.verticalRules = verticalRules;
    }

    Draw(context: CanvasRenderingContext2D, box: Rect, frameIndex: number) {
        let xScale = box.width / this.values.length;
        let yScale = 0;
        let yOffset = box.y + 0.5 * box.height;
        if (this.minValue != this.maxValue) {
            yOffset = box.y + box.height;
            yScale = box.height / (this.maxValue - this.minValue);
        }

        for (let rule of this.verticalRules) {
            let x = box.x + rule.frame * xScale;
            context.beginPath();
            context.moveTo(x, box.y);
            context.lineTo(x, box.y + box.height);

            context.save()
            if (rule.strokeStyle != null && rule.lineWidth > 0) {
                context.strokeStyle = rule.strokeStyle;
                context.lineWidth = rule.lineWidth;
            }
            context.stroke();
            context.restore();
        }

        let x = box.x;
        let y = yOffset - (this.values[0] - this.minValue) * yScale;
        context.beginPath();
        context.moveTo(x, y);
        for (let value of this.values.slice(1)) {
            x += xScale;
            y = yOffset - (value - this.minValue) * yScale;
            context.lineTo(x, y);
        }

        context.save()
        if (this.strokeStyle != null && this.lineWidth > 0) {
            context.strokeStyle = this.strokeStyle;
            context.lineWidth = this.lineWidth;
        }
        context.stroke();
        context.restore();

        x = box.x + xScale * frameIndex;
        y = yOffset - (this.values[frameIndex] - this.minValue) * yScale;
        context.beginPath();
        context.arc(x, y, Sparkline.MARKER_RADIUS * window.devicePixelRatio, 0.0, 2.0 * Math.PI, false);
        context.save();
        if (this.strokeStyle != null) {
            context.fillStyle = this.strokeStyle;
        }
        context.fill();
        context.restore();
    }

    ValueAt(frameIndex: number): number {
        return this.values[frameIndex];
    }
}

export default class Graph extends CanvasBase {
    context: CanvasRenderingContext2D = null; // Rendering context

    backgroundStyle: string; // Background color

    fontFamily: string;
    nameSizeInPixels: number;
    nameAlign: string;
    valueSizeInPixels: number;
    valueAlign: string;

    sparklines: Sparkline[] = []; // The sparklines

    margin: Margin = null;

    static readonly PADDING = 7;

    constructor(canvasId: string, public frameRate: number, public width: number, public height: number, objectCache: ObjectCache, public SetStatus: (status: string) => void, public SetWarning: (message: string) => void, public RequestRedraw: () => void, public ReportFrameIdChange: (canvasId: string, frameId: string) => void) {
        // Base class constructor
        super(canvasId, frameRate, width, height, objectCache, SetStatus, SetWarning, RequestRedraw, ReportFrameIdChange);

        // Setup 2D context for drawing
        this.context = this.htmlCanvas.getContext('2d');

        this.backgroundStyle = "#000000";
        this.margin = new Margin(10, 10, 10, 10);
        this.nameAlign = "left";
        this.valueAlign = "right";

        // Start render loop
        this.StartRenderLoop();
    }

    // Execute a single canvas command
    ExecuteCanvasCommand(command: any) {
        switch (command["CommandType"]) {
            case "SetBackgroundStyle":
                this.backgroundStyle = String(command["Value"]);
                break;

            case "SetTextStyle":
                this.fontFamily = String(command["FontFamily"])
                this.nameSizeInPixels = Number(command["NameSizeInPixels"])
                this.nameAlign = String(command["NameAlign"])
                this.valueSizeInPixels = Number(command["ValueSizeInPixels"])
                this.valueAlign = String(command["ValueAlign"])
                break;

            case "SetMargin":
                let margin = command["Value"];
                let top = Number(margin["Top"]);
                let right = Number(margin["Right"]);
                let bottom = Number(margin["Bottom"]);
                let left = Number(margin["Left"]);
                this.margin = new Margin(top, right, bottom, left);
                break;

            case "AddSparkline":
                let name = String(command["Name"]);
                let values = Misc.Base64ToFloat32Array(command["ValueBuffer"]);
                let strokeStyle = String(command["StrokeStyle"]);
                let lineWidth = Number(command["LineWidth"]);
                let verticalRules: VerticalRule[] = []
                for (let rule of command["VerticalRules"]) {
                    verticalRules.push(new VerticalRule(Number(rule["FrameIndex"]), String(rule["StrokeStyle"]), Number(rule["LineWidth"])));
                }

                if (this.sparklines.length == 0) {
                    for (let i = 0; i < values.length; ++i) {
                        this.AddFrame(i.toString());
                    }
                }

                this.sparklines.push(new Sparkline(name, values, strokeStyle, lineWidth, verticalRules));
                break;

            default:
                super.ExecuteCanvasCommand(command);
                break;
        }
    }

    AllocateFrame() {
    }

    DeallocateFrame(frameIndex: number) {
    }

    // Render scene method
    Render() {
        // Clear
        this.context.fillStyle = this.backgroundStyle;
        this.context.fillRect(0, 0, this.htmlCanvas.clientWidth * window.devicePixelRatio, this.htmlCanvas.clientHeight * window.devicePixelRatio);

        let width = this.htmlCanvas.clientWidth - this.margin.left - this.margin.right;
        let height = this.htmlCanvas.clientHeight - this.margin.top - this.margin.bottom;
        let left = this.margin.left * window.devicePixelRatio;
        let top = this.margin.top * window.devicePixelRatio;
        width *= window.devicePixelRatio;
        height *= window.devicePixelRatio;

        let layout = new Layout();
        layout.center = new Rect();

        let nameFont = this.nameSizeInPixels + "px " + this.fontFamily;
        let valueFont = this.valueSizeInPixels + "px " + this.fontFamily;
        let nameColumnWidth = 0;
        let valueColumnWidth = 0;
        for (let sparkline of this.sparklines) {
            this.context.font = nameFont;
            let nameSize = this.context.measureText(sparkline.name).width;
            this.context.font = valueFont;
            let minValSize = this.context.measureText(sparkline.minValue.toFixed(2)).width;
            let maxValSize = this.context.measureText(sparkline.maxValue.toFixed(2)).width;
            nameColumnWidth = Math.max(nameSize, nameColumnWidth);
            valueColumnWidth = Math.max(minValSize, maxValSize, valueColumnWidth);
        }

        nameColumnWidth *= window.devicePixelRatio;
        let nameHeight = this.nameSizeInPixels * window.devicePixelRatio;
        valueColumnWidth *= window.devicePixelRatio;
        let valueHeight = this.valueSizeInPixels * window.devicePixelRatio;

        let rowHeight = height / this.sparklines.length;
        let rowTop = top;
        for (let sparkline of this.sparklines) {
            let layout = new Layout();

            let nameRect = new Rect(0, 0, nameColumnWidth, nameHeight);
            let nameLayout = new Layout({left: nameRect});
            switch (this.nameAlign) {
                case "top":
                    layout.top = nameLayout;
                    break;

                case "bottom":
                    layout.bottom = nameLayout;
                    break;

                case "left":
                    layout.left = nameLayout;
                    break;

                case "right":
                    layout.right = nameLayout;
                    break;
            }

            let valueRect = new Rect(0, 0, valueColumnWidth, valueHeight);
            switch (this.valueAlign) {
                case "top":
                    if(layout.top instanceof Layout){
                        layout.top.right = valueRect;
                    }else{
                        layout.top = new Layout({right: valueRect})
                    }
                    break;

                case "bottom":
                    if(layout.bottom instanceof Layout){
                        layout.bottom.right = valueRect;
                    }else{
                        layout.bottom = new Layout({right: valueRect})
                    }
                    break;

                case "left":
                    if(layout.left instanceof Layout){
                        layout.left.right = valueRect;
                    }else{
                        layout.left = new Layout({right: valueRect})
                    }
                    break;

                case "right":
                    if(layout.right instanceof Layout){
                        layout.right.right = valueRect;
                    }else{
                        layout.right = new Layout({right: valueRect})
                    }
                    break;
            }

            layout.Layout(width, rowHeight);
            layout.Move(left, rowTop);

            nameRect.DrawText(this.context, sparkline.name, sparkline.strokeStyle, nameFont);
            valueRect.DrawText(this.context, sparkline.ValueAt(this.currentFrameIndex).toFixed(2),
                sparkline.strokeStyle, valueFont);
            sparkline.Draw(this.context, (layout.center as Rect).Pad(Graph.PADDING * window.devicePixelRatio),
                this.currentFrameIndex);
            rowTop += rowHeight;
        }
    }
}
