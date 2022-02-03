// NB ONLY TO BE USED INTERNALLY, NOT IN API
import Misc from "./Misc"
import {vec3, vec4, mat4} from "gl-matrix";
import { VertexBuffer, VertexBufferType } from "./VertexBuffers";

export default class Mesh
{    
    static readonly ElementsPerTriangle : number = 3;
    static readonly ElementsPerLine : number = 2;
    ElementsPerVertex : number;
    VertexOffsetPosition : number = 0;
    VertexOffsetNormal : number = 3;
    VertexOffsetColor : number = 0; // Dummy value - simplifies logic in webglmeshbuffers
    VertexOffsetTexture : number = 0; // Dummy value - simplifies logic in webglmeshbuffers

    ElementsPerInstance : number;
    InstanceOffsetPosition : number = 0;
    InstanceOffsetRotation : number = 0;
    InstanceOffsetColor : number = 0;

    // Host memory buffers
    centerOfMass : vec3; // Mean of vertex positions, to allow depth ordering for improved transparency
    vertexBuffer : Float32Array; // Vertex.Length elements per vertex
    bytesPerIndex : number; // 2 or 4
    triangleBuffer : ArrayBuffer; // 3 elements (v1, v2, v3) per triangle
    lineBuffer : ArrayBuffer; // 2 elements (v1, v2) per line
    wireframeEdgeBuffer : ArrayBuffer; // 2 elements (v1, v2) per edge, used for wireframe
    instanceBuffer : Float32Array; // Used for instancing
    instanceBufferHasRotations : boolean;
    instanceBufferHasColors : boolean;
    color : vec3; // Either null (per-vertex coloring stored in vertex buffer) or a 3-vector (shared color across whole mesh)
    textureId : string;
    isBillboard : boolean; // If true, zero out rotation
    isLabel : boolean; // If true, treat the mesh specially as a label
    nnTexture : boolean; // If true, render textures using nearest neighbor interpolation, otherwise linear
    useTextureAlpha : boolean; // If true, use the texture's alpha channel for transparency. Otherwise treat as opaque.
    cameraSpace : boolean; // If true, the mesh is defined in camera-space rather than world space and will thus not move with mouse commands
    layerId : string; // Unique identifier for a layer of Meshes with shared properties
    doubleSided : boolean; // If true, then back-face culling is turned off

    // Properties
    CountVertices() : number { return this.vertexBuffer.length / this.ElementsPerVertex; }
    CountTriangles() : number { return this.triangleBuffer.byteLength / (this.bytesPerIndex * Mesh.ElementsPerTriangle); }
    CountLines() : number { return this.lineBuffer.byteLength / (this.bytesPerIndex * Mesh.ElementsPerLine); }
    CountWireframeEdges() : number { return this.wireframeEdgeBuffer != null ? this.wireframeEdgeBuffer.byteLength / (this.bytesPerIndex * Mesh.ElementsPerLine) : 0; }
    CountInstances() : number { return this.instanceBuffer != null ? (this.instanceBuffer.length / this.ElementsPerInstance) : 1; }

    // Constructor
    constructor(vertexBuffer : Float32Array, bytesPerIndex : number, triangleBuffer : ArrayBuffer, lineBuffer : ArrayBuffer, color : vec3, textureId : string = null, nnTexture : boolean = true, useTextureAlpha = true, instanceBuffer : Float32Array = null, instanceBufferHasRotations = false, instanceBufferHasColors = false)
    {
        this.vertexBuffer = vertexBuffer;
        this.bytesPerIndex = bytesPerIndex;
        this.triangleBuffer = triangleBuffer;
        this.lineBuffer = lineBuffer;
        this.wireframeEdgeBuffer = null;
        this.color = color;
        this.textureId = textureId;
        this.isBillboard = false;
        this.isLabel = false;
        this.nnTexture = nnTexture;
        this.useTextureAlpha = useTextureAlpha;
        this.cameraSpace = false;
        this.layerId = null;

        // Deal with instance buffer
        if (instanceBuffer == null)
        {
            this.instanceBuffer = null;
            this.instanceBufferHasRotations = false;
            this.instanceBufferHasColors = false;
        }
        else
        {
            this.instanceBuffer = instanceBuffer;
            this.instanceBufferHasRotations = instanceBufferHasRotations;
            this.instanceBufferHasColors = instanceBufferHasColors;
        }
    
        // Compute number of elements per vertex
        this.ElementsPerVertex = 6; // Position and normal: X,Y,Z,NX,NY,NZ
        if (this.color == null) // I.e. per-vertex rather than per-mesh color
        {
            this.VertexOffsetColor = this.ElementsPerVertex;
            this.ElementsPerVertex += 3; // Per-vertex color: R,G,B
        }
        if (this.textureId != null)
        {
            this.VertexOffsetTexture = this.ElementsPerVertex;
            this.ElementsPerVertex += 2; // Texture coords: U,V
        }

        // Compute number of elements per instance
        this.ElementsPerInstance = 3; // Translation: X,Y,Z
        if (this.instanceBufferHasRotations)
        {
            this.InstanceOffsetRotation = this.ElementsPerInstance;
            this.ElementsPerInstance += 4; // Quaternion rotation: W,X,Y,Z
        }
        if (this.instanceBufferHasColors)
        {
            this.InstanceOffsetColor = this.ElementsPerInstance;
            this.ElementsPerInstance += 3; // Color: R,G,B
        }

        // Compute center of mass
        this.centerOfMass = vec3.create();
        if (this.CountVertices() > 0)
        {
            for(var i = 0; i < this.vertexBuffer.length; i += this.ElementsPerVertex)
            {
                this.centerOfMass[0] += this.vertexBuffer[i + 0];
                this.centerOfMass[1] += this.vertexBuffer[i + 1];
                this.centerOfMass[2] += this.vertexBuffer[i + 2];
            }
            this.centerOfMass[0] /= this.CountVertices();
            this.centerOfMass[1] /= this.CountVertices();
            this.centerOfMass[2] /= this.CountVertices();
        }
    }

    Update(buffer : Float32Array, update_flags: VertexBufferType) : Mesh
    {
        if(this.CountInstances() > 1)
        {
            return this.ToInstanceBuffer(buffer, update_flags);
        }
        else
        {
            return this.ToVertexBuffer(buffer, update_flags);
        }
    }

    ToInstanceBuffer(update : Float32Array, updateFlags: VertexBufferType) : Mesh
    {
        let instanceBuffer = VertexBuffer.Update(this.instanceBuffer, this.ElementsPerInstance, update, updateFlags);
        let mesh = new Mesh(this.vertexBuffer,
                            this.bytesPerIndex,
                            this.triangleBuffer,
                            this.lineBuffer,
                            this.color,
                            this.textureId,
                            this.nnTexture,
                            this.useTextureAlpha,
                            instanceBuffer,
                            this.instanceBufferHasRotations,
                            this.instanceBufferHasColors)
        mesh.cameraSpace = this.cameraSpace;
        mesh.layerId = this.layerId;
        mesh.doubleSided = this.doubleSided;
        mesh.isBillboard = this.isBillboard;
        mesh.isLabel = this.isLabel;
        return mesh;
    }

    ToVertexBuffer(update : Float32Array, updateFlags: VertexBufferType) : Mesh
    {
        let vertexBuffer = VertexBuffer.Update(this.vertexBuffer, this.ElementsPerVertex, update, updateFlags);
        let mesh = new Mesh(vertexBuffer,
                            this.bytesPerIndex,
                            this.triangleBuffer,
                            this.lineBuffer,
                            this.color,
                            this.textureId,
                            this.nnTexture,
                            this.useTextureAlpha,
                            this.instanceBuffer,
                            this.instanceBufferHasRotations,
                            this.instanceBufferHasColors)
        mesh.cameraSpace = this.cameraSpace;
        mesh.layerId = this.layerId;
        mesh.doubleSided = this.doubleSided;
        mesh.isBillboard = this.isBillboard;
        mesh.isLabel = this.isLabel;
        return mesh;
    }

    static Parse(definition : any)
    {
        var color : vec3 = null;
        var textureId : string = null;
        var nnTexture : boolean = true;
        var useTextureAlpha : boolean = true;

        switch(definition["PrimitiveType"])
        {
            case "SingleColorMesh":
                color = <vec3>Misc.Base64ToFloat32Array(definition["Color"]);
                textureId = Misc.GetDefault(definition, "TextureId", null);
                nnTexture = Misc.GetDefault(definition, "NearestNeighborTexture", true);
                useTextureAlpha = Misc.GetDefault(definition, "UseTextureAlpha", false);
            case "MultiColorMesh":
                let vertexBuffer = Misc.Base64ToFloat32Array(definition["VertexBuffer"]);
                var indexBufferType = definition["IndexBufferType"];
                var bytesPerIndex : number, triangleBuffer : ArrayBuffer, lineBuffer : ArrayBuffer;
                if (indexBufferType == "UInt16")
                {
                    bytesPerIndex = 2;
                    triangleBuffer = Misc.Base64ToUInt16Array(definition["TriangleBuffer"]).buffer;
                    lineBuffer = Misc.Base64ToUInt16Array(definition["LineBuffer"]).buffer;
                }
                else // UInt32
                {
                    bytesPerIndex = 4;
                    triangleBuffer = Misc.Base64ToUInt32Array(definition["TriangleBuffer"]).buffer;
                    lineBuffer = Misc.Base64ToUInt32Array(definition["LineBuffer"]).buffer;
                }
                var instanceBuffer = Misc.Base64ToFloat32Array(definition["InstanceBuffer"]);
                var instanceBufferHasRotations = Misc.GetDefault(definition, "InstanceBufferHasRotations", false);
                var instanceBufferHasColors = Misc.GetDefault(definition, "InstanceBufferHasColors", false);
                return new Mesh(vertexBuffer, bytesPerIndex, triangleBuffer, lineBuffer, color, textureId, nnTexture, useTextureAlpha, instanceBuffer, instanceBufferHasRotations, instanceBufferHasColors);
            default:
               throw "Unknown \"Primitive\" type in mesh: " + JSON.stringify(definition);
        }
    }

    GetWireframeEdgeBuffer()
    {
        if (this.wireframeEdgeBuffer == null)
        {
            this.wireframeEdgeBuffer = new ArrayBuffer(this.triangleBuffer.byteLength * 2);
            var triangleView : any = this.bytesPerIndex == 2 ? new Uint16Array(this.triangleBuffer) : new Uint32Array(this.triangleBuffer);
            var wireframeView : any = this.bytesPerIndex == 2 ? new Uint16Array(this.wireframeEdgeBuffer) : new Uint32Array(this.wireframeEdgeBuffer);

            for(var i = 0; i < triangleView.length; i += 3)
            {
                wireframeView[2 * i + 5] = wireframeView[2 * i + 0] = triangleView[i + 0];
                wireframeView[2 * i + 2] = wireframeView[2 * i + 1] = triangleView[i + 1];
                wireframeView[2 * i + 4] = wireframeView[2 * i + 3] = triangleView[i + 2];
            }
        }
        return this.wireframeEdgeBuffer;
    }
}
