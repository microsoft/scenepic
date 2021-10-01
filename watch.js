let browserify = require("browserify")
let tsify = require("tsify")
let fs = require("fs")
let exorcist = require("exorcist")
let watchify = require("watchify")

let stream = fs.createWriteStream("build/scenepic.js")
let mapFile = "build/scenepic.js.map"

let b = browserify({
    debug: true,
    cache: {},
    packageCache: {},
    plugin: [watchify]
})
.add([
    "tssrc/Canvas2D.ts",
    "tssrc/Canvas3D.ts",
    "tssrc/CanvasBase.ts",
    "tssrc/CSSStyles.ts",
    "tssrc/DropDownMenu.ts",
    "tssrc/Mesh.ts",
    "tssrc/Misc.ts",
    "tssrc/Shaders.ts",
    "tssrc/SPScene.ts",
    "tssrc/TextPanel.ts",
    "tssrc/WebGLMeshBuffers.ts",
    "tssrc/ScenePic.ts"
])
.plugin(tsify)

b.on('update', bundle)
bundle()

function bundle() {
    console.info("File updated")
    b.bundle()
    .on('error', console.error)
    .pipe(exorcist(mapFile))
    .pipe(stream)
}