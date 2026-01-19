const esbuild = require("esbuild");
const { mkdir } = require("node:fs/promises");
const { join } = require("node:path");

async function watch() {
    await mkdir("build", { recursive: true });

    const ctx = await esbuild.context({
        entryPoints: ["tssrc/ScenePic.ts"],
        bundle: true,
        platform: "browser",
        format: "iife",
        sourcemap: true,
        outfile: join("build", "scenepic.js"),
        logLevel: "info"
    });

    await ctx.watch();
    console.info("Watching tssrc -> build/scenepic.js");
}

watch().catch(err => {
    console.error(err);
    process.exit(1);
});