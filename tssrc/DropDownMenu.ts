
export default class DropDownMenu
{
    title : string;
    header : HTMLSpanElement;
    wrapper : HTMLDivElement; // Contains both contents and inputBox
    contents : HTMLSpanElement;
    dropDownMenu : HTMLSelectElement;

    constructor(title : string, style : string, parent : HTMLElement)
    {
        // Create div
        var div = document.createElement("div");
        div.className = "scenepic"; // CSS style class name
        if (style != null)
            div.setAttribute("style", style);

        // Create contents and optional input
        this.wrapper = document.createElement("div");

        // Create title header
        this.header = document.createElement("span");
        this.header.className = "scenepic-textbox-header";
        this.title = title;
        div.appendChild(this.header);

        // Add contents to wrapper
        div.appendChild(this.wrapper);
        this.contents = document.createElement("div");
        this.wrapper.appendChild(this.contents);

        // Add a drop down menu
        this.dropDownMenu = document.createElement("select");
        this.dropDownMenu.innerHTML = "";
        this.dropDownMenu.size = 2;
        this.wrapper.appendChild(this.dropDownMenu);

        // Add to parent
        this.Update();
        parent.appendChild(div);
    }

    SetItems(items : string[])
    {
        items.forEach(item =>
        {
            // Create table row
            var option = document.createElement("option");
            option.value = item;
            option.text = item;
            this.dropDownMenu.appendChild(option);
        });

        this.Update();
    }

    SetItemDisabled(index : number, disabled : boolean)
    {
        this.dropDownMenu.options[index].disabled = disabled;
    }

    SetSelection(index : number)
    {
        this.dropDownMenu.selectedIndex = index;
    }

    SetTitle(htmlTitle : string)
    {
        this.title = htmlTitle;
        this.Update();
    }

    Update()
    {
        this.header.style.display = (this.title == null || this.title == "") ? "none" : "";
        this.header.innerHTML = this.title;
        this.wrapper.style.display = "inline-block";
    }
}
