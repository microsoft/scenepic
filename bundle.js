let browserify = require("browserify")
let tsify = require("tsify")
let minify = require("minify-stream")
let derequire = require("browserify-derequire")
let fs = require("fs")
let exorcist = require("exorcist")
let makeDir = require('make-dir')

makeDir.sync("dist")

let stream = fs.createWriteStream("dist/scenepic.min.js")
let mapFile = "dist/scenepic.min.js.map"

version = fs.readFileSync("VERSION")
stream.write("// " + version + "\n")

let b = browserify()
    .add([
        "tssrc/ScenePic.ts",
    ])
    .plugin(tsify)
    .plugin(derequire)
    .bundle()
    .pipe(minify())
    .pipe(exorcist(mapFile))
    .pipe(stream)
