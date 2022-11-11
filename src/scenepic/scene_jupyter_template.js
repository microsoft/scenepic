let container = document.createElement("div");
let id = crypto.randomUUID().toString();
id = "sp" + id;
id = id.replace(/-/g, "");
container.id = id;

// add the scenepic container DIV to the Jupyter element
element.append(container);

// check to see if scenepic has already been loaded
let script = document.getElementById("scenepic_script");
if(script == null){
    console.info("adding scenepic script node")
    script = document.createElement("script");
    script.type = "text/javascript";
    script.src = "scenepic.min.js";
    script.id = "scenepic_script";
    script.async = false;
    script.defer = false;
    document.head.appendChild(script);
}

// this will be replaced by the scene.py
let commands = %s;

function createScenepic(id, commands) {
    if(typeof window.scenepic == "function") {
        // scenepic has loaded
        scenepic(id, commands);
    }else{
        // wait for scenepic to load
        setTimeout(() => createScenepic(id, commands), 1000);
    }
}

// allow time for the target to be added to the DOM
setTimeout(() => createScenepic(id, commands), 100);