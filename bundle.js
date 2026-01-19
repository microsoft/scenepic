const esbuild = require("esbuild");
const { mkdir } = require("node:fs/promises");
const { readFileSync } = require("node:fs");
const { join } = require("node:path");

async function build() {
    await mkdir("dist", { recursive: true });
    const version = readFileSync("VERSION", "utf8").trim();

    await esbuild.build({
        entryPoints: ["tssrc/ScenePic.ts"],
        bundle: true,
        platform: "browser",
        format: "iife",
        outfile: join("dist", "scenepic.min.js"),
        sourcemap: true,
        minify: true,
        banner: { js: `// ${version}\n` }
    });

    console.info("Built dist/scenepic.min.js");
}

build().catch(err => {
    console.error(err);
    process.exit(1);
});
