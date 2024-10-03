let browserify = require("browserify");
let tsify = require("tsify");
let minify = require("minify-stream");
let derequire = require("browserify-derequire");
let fs = require("fs");
let exorcist = require("exorcist");
const { mkdir } = require("node:fs/promises");
const { join } = require('node:path');

async function createDist() {
    const dirCreation = await mkdir("dist")
    console.log(dirCreation)
    return dirCreation    
}

if (!fs.existsSync("dist")){
    createDist().catch(console.error)
}

let stream = fs.createWriteStream(join("dist", "scenepic.min.js"))
let mapFile = join("dist", "scenepic.min.js.map")

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
