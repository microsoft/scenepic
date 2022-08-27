import {quat, vec3, vec4, mat3, mat4} from "gl-matrix";
import Mesh from "./Mesh";
import Misc from "./Misc"
import ShaderProgram from "./Shaders";

export default class WebGLMeshBuffers
{
    gl : WebGLRenderingContext;
    sp : ShaderProgram;
    m : Mesh;

    color : vec3;

    vertexDataBuffer : WebGLBuffer;
    triangleIndexBuffer : WebGLBuffer;
    lineIndexBuffer : WebGLBuffer;
    wireframeEdgeIndexBuffer : WebGLBuffer;
    instanceDataBuffer : WebGLBuffer;

    hasTexture : boolean;
    texture : WebGLTexture;
    
    indexType : number;

    m2wMatrix : mat4;

    // Used for label computations
    labelWidthNormalized : number = -1.0;
    labelHeightNormalized : number = -1.0;
    labelTranslateScreenX : number = 0.0;
    labelTranslateScreenY : number = 0.0;
    labelTranslateWorldX : number = 0.0;
    labelTranslateWorldY : number = 0.0;

    constructor(gl : WebGLRenderingContext, sp : ShaderProgram, m : Mesh, textureSrc = null)
    {
        this.gl = gl;
        this.sp = sp;
        this.m = m;

        this.color = m.color;
        
        this.indexType = m.bytesPerIndex == 2 ? gl.UNSIGNED_SHORT : gl.UNSIGNED_INT;

        // Create and bind vertex buffer
        this.vertexDataBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexDataBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, m.vertexBuffer, gl.STATIC_DRAW);

        // Create and bind triangle index buffer
        this.triangleIndexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.triangleIndexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, m.triangleBuffer, gl.STATIC_DRAW);

        // Create and bind line index buffer
        this.lineIndexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.lineIndexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, m.lineBuffer, gl.STATIC_DRAW);

        // Create and bind wireframe edge index buffer on demand later
        this.wireframeEdgeIndexBuffer = null;

        // Create and bind instance buffer
        this.instanceDataBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.instanceDataBuffer);
        if (m.instanceBuffer != null)
            gl.bufferData(gl.ARRAY_BUFFER, m.instanceBuffer, gl.STATIC_DRAW);

        // Remove bind for safety
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        // Create texture (always creates one even if not used to prevent error messages in shader)
        this.hasTexture = textureSrc != null;
        this.texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
        if (this.m.isLabel)
            gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, true);
        else
            gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, false);
        if (this.hasTexture)
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, textureSrc);
        else
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([1, 1, 1, 1])); // Dummy texture
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, m.nnTexture ? gl.NEAREST : gl.LINEAR);
        if (textureSrc != null && (Misc.IsPow2(textureSrc.width) && Misc.IsPow2(textureSrc.height)))
        {
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
            gl.generateMipmap(gl.TEXTURE_2D);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, m.nnTexture ? gl.NEAREST : gl.LINEAR_MIPMAP_NEAREST)
        }
        else
        {
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, m.nnTexture ? gl.NEAREST : gl.LINEAR);            
        }
        gl.bindTexture(gl.TEXTURE_2D, null);

        // Set model to world matrix
        this.m2wMatrix = mat4.create();
    }

    Finalize()
    {
        var gl = this.gl;
        gl.deleteBuffer(this.vertexDataBuffer);
        gl.deleteBuffer(this.triangleIndexBuffer);
        gl.deleteBuffer(this.lineIndexBuffer);
        if (this.wireframeEdgeIndexBuffer != null)
            gl.deleteBuffer(this.wireframeEdgeIndexBuffer);
        gl.deleteBuffer(this.instanceDataBuffer);
        if (this.texture != null)
            gl.deleteTexture(this.texture);
    }

    ApplyBillboardEffect(m2vMatrix : mat4)
    {
        // Get rotational matrix component of m2vMatrix
        var det = mat4.determinant(m2vMatrix);
        var scale = det != 0.0 ? 1.0 / Math.pow(det, 1.0 / 3.0) : 1.0;
        var scaled = mat4.create();
        mat4.scale(scaled, m2vMatrix, [scale, scale, scale]); // Undo any uniform scaling.  Sadly won't work for non-uniform scaling, and assumes no shearing.

        // Get rotation as quaternion
        var q = quat.create();
        mat4.getRotation(q, scaled);

        // Invert and transform
        quat.invert(q, q);
        var transform = mat4.create();
        mat4.fromRotationTranslationScaleOrigin(transform, q, [0,0,0], [1,1,1], this.m.centerOfMass);
        mat4.multiply(m2vMatrix, m2vMatrix, transform);
    }

    ApplyLabel(m2vMatrix : mat4, v2sMatrix : mat4)
    {
        this.ApplyBillboardEffect(m2vMatrix);

        // Helper function - careful - this references, not copies the array
        var getVertex = (vertexIndex) => <vec3>this.m.vertexBuffer.subarray(this.m.VertexOffsetPosition + vertexIndex * this.m.ElementsPerVertex, this.m.VertexOffsetPosition + vertexIndex * this.m.ElementsPerVertex + 3);

        // Work out how large this would get drawn in pixels
        var projMatrix = mat4.create();
        mat4.multiply(projMatrix, v2sMatrix, m2vMatrix);
        var v0 = getVertex(0);
        var v1 = getVertex(1);
        var v2 = getVertex(2);
        var p0 = vec3.create(); vec3.transformMat4(p0, v0, projMatrix);
        var p1 = vec3.create(); vec3.transformMat4(p1, v1, projMatrix);
        var p2 = vec3.create(); vec3.transformMat4(p2, v2, projMatrix);
        var width = p1[0] - p0[0];
        var height = p2[1] - p1[1];
        var scaleFactorX = this.labelWidthNormalized / width;
        var scaleFactorY = this.labelHeightNormalized / height;

        // Get depth of label
        var v0V = vec3.create(); vec3.transformMat4(v0V, v0, m2vMatrix);
        var labelDepth = Math.abs(v0V[2]);

        // Transform with alignment translation and scale factor
        var translate = vec3.fromValues(this.labelTranslateScreenX * labelDepth / v2sMatrix[0] + this.labelTranslateWorldX,
                                        this.labelTranslateScreenY * labelDepth / v2sMatrix[5] + this.labelTranslateWorldY,
                                        0.0);
        var scale = vec3.fromValues(scaleFactorX, scaleFactorY, 1.0);
        var transform = mat4.create();
        mat4.fromRotationTranslationScale(transform, quat.create(), translate, scale);
        mat4.mul(m2vMatrix, m2vMatrix, transform);
    }

    RenderBuffer(v2sMatrix : mat4, w2vMatrix : mat4, opacity : number, renderFilled : boolean, renderWireframe : boolean)
    {
        var gl = this.gl;
        var sp = this.sp;
        var m = this.m;

        var ext = gl.getExtension("ANGLE_instanced_arrays");

        // Ensure we have wireframe buffer if needed
        if (renderWireframe && this.wireframeEdgeIndexBuffer == null)
        {
            this.wireframeEdgeIndexBuffer = gl.createBuffer();
            var cpuBuffer = m.GetWireframeEdgeBuffer();
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.wireframeEdgeIndexBuffer);
            gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, cpuBuffer, gl.STATIC_DRAW);
        }

        // Compute projection matrix (i.e. concatenation of m2w, w2v, v2s)
        var m2vMatrix = mat4.create();
        var projMatrix = mat4.create();
        mat4.multiply(m2vMatrix, w2vMatrix, this.m2wMatrix);
        if (this.m.isBillboard) // Turn off rotation for billboard objects
            this.ApplyBillboardEffect(m2vMatrix);
        if (this.m.isLabel && this.hasTexture) // Deal with labels specially
            this.ApplyLabel(m2vMatrix, v2sMatrix);
        mat4.multiply(projMatrix, v2sMatrix, m2vMatrix);
        gl.uniformMatrix4fv(sp.projMatrixPtr, false, projMatrix);

        // Set model view matrix (i.e. concatenation of m2w, w2v)
        gl.uniformMatrix4fv(sp.m2vMatrixPtr, false, m2vMatrix);
        
        // Compute normals transformation matrix
        var normMatrix = Misc.Mat3FromMat4(m2vMatrix);
        mat3.invert(normMatrix, normMatrix);
        mat3.transpose(normMatrix, normMatrix);
        gl.uniformMatrix3fv(sp.normMatrixPtr, false, normMatrix);

        // Setup texturing
        gl.uniform1i(sp.samplerPtr, 0); // Use texture index 0
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, this.texture);

        // Bind vertex buffers
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexDataBuffer);
        if (true)
        {
            gl.enableVertexAttribArray(sp.vertexPositionAttribLoc);
            gl.vertexAttribPointer(sp.vertexPositionAttribLoc, 3, gl.FLOAT, false, m.ElementsPerVertex * 4, m.VertexOffsetPosition * 4);
        }
        if (true)
        {
            gl.enableVertexAttribArray(sp.vertexNormalAttribLoc);
            gl.vertexAttribPointer(sp.vertexNormalAttribLoc, 3, gl.FLOAT, false, m.ElementsPerVertex * 4, m.VertexOffsetNormal * 4);
        }
        if (m.VertexOffsetColor != 0)
        {
            gl.enableVertexAttribArray(sp.vertexColorAttribLoc);
            gl.vertexAttribPointer(sp.vertexColorAttribLoc, 3, gl.FLOAT, false, m.ElementsPerVertex * 4, m.VertexOffsetColor * 4);
        }
        else
        {
            gl.disableVertexAttribArray(sp.vertexColorAttribLoc);
        }
        if (m.VertexOffsetTexture != 0)
        {
            gl.enableVertexAttribArray(sp.vertexTextureAttribLoc);
            gl.vertexAttribPointer(sp.vertexTextureAttribLoc, 2, gl.FLOAT, false, m.ElementsPerVertex * 4, m.VertexOffsetTexture * 4); // VertexOffsetTexture = 0 when no texture used
        }
        else
        {
            gl.disableVertexAttribArray(sp.vertexTextureAttribLoc);
        }
        
        // Bind instance buffers
        gl.bindBuffer(gl.ARRAY_BUFFER, this.instanceDataBuffer);
        if (m.instanceBuffer != null)
        {
            gl.enableVertexAttribArray(sp.instancePositionAttribLoc);
            gl.vertexAttribPointer(sp.instancePositionAttribLoc, 3, gl.FLOAT, false, m.ElementsPerInstance * 4, m.InstanceOffsetPosition * 4);
            ext.vertexAttribDivisorANGLE(sp.instancePositionAttribLoc, 1);
        }
        else
        {
            gl.disableVertexAttribArray(sp.instancePositionAttribLoc);
            ext.vertexAttribDivisorANGLE(sp.instancePositionAttribLoc, 0);
        }
        if (m.instanceBufferHasRotations)
        {
            gl.enableVertexAttribArray(sp.instanceRotationAttribLoc);
            gl.vertexAttribPointer(sp.instanceRotationAttribLoc, 4, gl.FLOAT, false, m.ElementsPerInstance * 4, m.InstanceOffsetRotation * 4);
            ext.vertexAttribDivisorANGLE(sp.instanceRotationAttribLoc, 1);
        }
        else
        {
            gl.disableVertexAttribArray(sp.instanceRotationAttribLoc);
            ext.vertexAttribDivisorANGLE(sp.instanceRotationAttribLoc, 0);
        }
        if (m.instanceBufferHasColors)
        {
            gl.enableVertexAttribArray(sp.instanceColorAttribLoc);
            gl.vertexAttribPointer(sp.instanceColorAttribLoc, 3, gl.FLOAT, false, m.ElementsPerInstance * 4, m.InstanceOffsetColor * 4);
            ext.vertexAttribDivisorANGLE(sp.instanceColorAttribLoc, 1);
        }
        else
        {
            gl.disableVertexAttribArray(sp.instanceColorAttribLoc);
            ext.vertexAttribDivisorANGLE(sp.instanceColorAttribLoc, 0);
        }
    
        // Set use instance rotation uniform1i
        gl.uniform1i(sp.useInstanceRotation, m.instanceBufferHasRotations ? 1 : 0);

        // Set color source uniform1i
        var colorSource = m.instanceBufferHasColors ? 2 : (this.color == null ? 0 : 1);
        gl.uniform1i(sp.colorSourcePtr, colorSource);

        // Set shared color
        if (this.color != null)
            gl.uniform3fv(sp.sharedColorPtr, this.color);

        // Set alpha
        gl.uniform1f(sp.alphaPtr, opacity);

        // Draw triangles
        if (renderFilled)
        {
            gl.uniform1i(sp.shadingTypePtr, this.m.isLabel ? 2 : (this.hasTexture ? 1 : 0));
            gl.uniform1f(sp.lightingMultiplierPtr, 1.0);
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.triangleIndexBuffer);
            ext.drawElementsInstancedANGLE(gl.TRIANGLES, m.CountTriangles() * Mesh.ElementsPerTriangle, this.indexType, 0, m.CountInstances());
        }

        // Draw wireframe edges
        if (renderWireframe)
        {
            gl.uniform1i(sp.shadingTypePtr, -1);
            gl.uniform1f(sp.lightingMultiplierPtr, opacity == 1.0 ? 0.4 : 0.5);
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.wireframeEdgeIndexBuffer);
            ext.drawElementsInstancedANGLE(gl.LINES, m.CountWireframeEdges() * Mesh.ElementsPerLine, this.indexType, 0, m.CountInstances());
        }

        // Draw lines
        gl.uniform1i(sp.shadingTypePtr, -1);
        gl.uniform1f(sp.lightingMultiplierPtr, 1.0);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.lineIndexBuffer);
        ext.drawElementsInstancedANGLE(gl.LINES, m.CountLines() * Mesh.ElementsPerLine, this.indexType, 0, m.CountInstances());

        // Unbind buffers
        gl.bindBuffer(gl.ARRAY_BUFFER, null);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
        gl.bindTexture(gl.TEXTURE_2D, null);
    }
}
