
export default class TextPanel
{
    minimized : boolean;
    title : string;
    header : HTMLSpanElement;
    wrapper : HTMLDivElement; // Contains both contents and inputBox
    contents : HTMLSpanElement;
    inputBox : HTMLTextAreaElement;

    constructor(id : string, title : string, style : string, parent : HTMLElement, startMinimized = false, addInputBox = false)
    {
        // Create div
        var div = document.createElement("div");
        div.className = "scenepic"; // CSS style class name
        div.id = id;
        if (style != null)
            div.setAttribute("style", style);

        // Create contents and optional input
        this.wrapper = document.createElement("div");
        this.minimized = startMinimized;

        // Create title header
        this.header = document.createElement("span");
        this.header.className = "scenepic-textbox-header";
        this.title = title;
        this.header.addEventListener("click", event => // Toggle visibility of contents on click
        {
            this.minimized = !this.minimized;
            this.Update();
        }, true);
        div.appendChild(this.header);

        // Add contents to wrapper
        div.appendChild(this.wrapper);
        this.contents = document.createElement("div");
        this.wrapper.appendChild(this.contents);

        // Add a text box
        this.inputBox = document.createElement("textarea");
        this.inputBox.innerHTML = "";
        this.inputBox.cols = 40;
        this.inputBox.rows = 2;
        if (addInputBox)
            this.wrapper.appendChild(this.inputBox);

        // Add to parent
        this.Update();
        parent.appendChild(div);
    }

    SetContents(htmlContents : string, append = false)
    {
        if (append)
            this.contents.innerHTML += htmlContents;
        else
            this.contents.innerHTML = htmlContents;
    }

    SetTitle(htmlTitle : string)
    {
        this.title = htmlTitle;
        this.Update();
    }

    Update()
    {
        this.header.style.display = (this.title == null || this.title == "") ? "none" : "";
        this.header.innerHTML = this.title + (this.minimized ? " &#9661;" : " &#9655;");
        var newDisplayValue = this.minimized ? "none" : "inline-block";
        this.wrapper.style.display = newDisplayValue;
    }

    SetInputBoxContents(htmlContents : string)
    {
        this.inputBox.value = htmlContents;
    }
}
