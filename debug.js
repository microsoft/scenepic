let browserify = require("browserify")
let tsify = require("tsify")
let fs = require("fs")

stream = fs.createWriteStream("dist/scenepic.js")

let b = browserify()
    .add('./tssrc/ScenePic.ts')
    .plugin(tsify)
    .bundle()
    .pipe(stream)