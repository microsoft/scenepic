let browserify = require("browserify")
let tsify = require("tsify")
let fs = require("fs")
let exorcist = require("exorcist")
let makeDir = require('make-dir')

makeDir.sync("dist")

let stream = fs.createWriteStream("dist/scenepic.min.js")
let mapFile = "dist/scenepic.min.js.map"

version = fs.readFileSync("VERSION")
stream.write("// " + version + "\n")

let b = browserify({
    debug: true
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
    "tssrc/ScenePic.ts",
    "tssrc/VertexBuffers.ts"
])
.plugin(tsify)
.transform('uglifyify', { global: true })
.bundle()
.pipe(exorcist(mapFile))
.pipe(stream)
