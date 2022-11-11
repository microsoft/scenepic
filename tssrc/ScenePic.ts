import SPScene from "./SPScene"


function scenepic(id: string, commands: any, wait=100){
    let element : HTMLElement | null = null;
    if(id != null){
        element = document.getElementById(id);
        if(element == null){
            console.warn(id + " does not exist in the DOM, retrying");
            setTimeout(() => { scenepic(id, commands, wait * 2); }, wait);
        }else{
            let scene = new SPScene(element);
            scene.ExecuteSceneCommands(commands);
        }
    }else{
        let scene = new SPScene();
        scene.ExecuteSceneCommands(commands);
    }
}

window["scenepic"] = scenepic;