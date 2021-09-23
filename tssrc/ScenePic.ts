import * as $ from "jquery"
import SPScene from "./SPScene"


$(document).ready(function(){
    let currentWindow = <any>window
    let scene = new SPScene(currentWindow.Element)
    let commands = currentWindow.ScriptCommands
    scene.ExecuteSceneCommands(commands)
})
