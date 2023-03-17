import { mat4, vec2 } from "gl-matrix";
import Misc from "./Misc";
import ShaderProgram from "./Shaders";
import WebGLMeshBuffers from "./WebGLMeshBuffers";

export class MeshPicker {
    readonly frameBuffer: WebGLFramebuffer;
    readonly program: ShaderProgram;
    data: Uint8Array;

    constructor(gl: WebGL2RenderingContext, width: number, height: number) {
        this.program = new ShaderProgram(gl, "pickerVertex", "pickerFragment");

        let targetTexture = Misc.AssertNotNull(gl.createTexture(), "WebGL returned a null texture");
        gl.bindTexture(gl.TEXTURE_2D, targetTexture);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA,
            width, height, 0,
            gl.RGBA, gl.UNSIGNED_BYTE, null);

        let depthBuffer = Misc.AssertNotNull(gl.createRenderbuffer(), "WebGL returned a null render buffer");
        gl.bindRenderbuffer(gl.RENDERBUFFER, depthBuffer);
        gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, width, height);

        this.frameBuffer = Misc.AssertNotNull(gl.createFramebuffer(), "WebGL returned a null frame buffer");
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.frameBuffer);

        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, targetTexture, 0);
        gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, depthBuffer);

        this.data = new Uint8Array(4);

        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }

    Pick(gl: WebGL2RenderingContext, meshes: [WebGLMeshBuffers, mat4][], point: vec2, v2sMatrix: mat4): number {
        const canvas = <HTMLCanvasElement>gl.canvas;

        gl.bindFramebuffer(gl.FRAMEBUFFER, this.frameBuffer);
        gl.viewport(0, 0, canvas.width, canvas.height);

        gl.enable(gl.CULL_FACE);
        gl.enable(gl.DEPTH_TEST);

        gl.clearColor(0, 0, 0, 0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        const program = this.program;
        gl.useProgram(program.program);

        meshes.forEach(function ([buffer, w2vMatrix]) {
            buffer.RenderPicker(program, v2sMatrix, w2vMatrix)
        });

        const pixelX = point[0] * canvas.width / canvas.clientWidth;
        const pixelY = canvas.height - point[1] * canvas.height / canvas.clientHeight - 1;
        gl.readPixels(pixelX, pixelY, 1, 1, gl.RGBA, gl.UNSIGNED_BYTE, this.data);

        gl.bindFramebuffer(gl.FRAMEBUFFER, null);

        return this.data[0] + (this.data[1] << 8) + (this.data[2] << 16);
    }
}