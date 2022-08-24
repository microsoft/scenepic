let browserify = require("browserify")
let tsify = require("tsify")
let minify = require("minify-stream")
let fs = require("fs")
let exorcist = require("exorcist")
let makeDir = require('make-dir')

makeDir.sync("dist")

let stream = fs.createWriteStream("dist/scenepic.min.js")
let mapFile = "dist/scenepic.min.js.map"

version = fs.readFileSync("VERSION")
stream.write("// " + version + "\n")

let b = browserify({
    // set this to true to produce a source map
    // for debugging the TypeScript library
    debug: false
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
.bundle()
.pipe(minify())
.pipe(exorcist(mapFile))
.pipe(stream)
