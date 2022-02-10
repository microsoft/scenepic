export const enum VertexBufferType
{
    None = 0,
    Positions = 1,
    Normals = 2,
    Colors = 4,
    Rotations = 8        
};

export class VertexBuffer {
    private static partialCopy(buffer: Float32Array, bufferStride: number,
                               update: Float32Array, updateOffset: number, updateSize: number) : Float32Array
    {
        buffer = Float32Array.from(buffer);
        for(let b_i=0, u_i=0; b_i < buffer.length; b_i += bufferStride, u_i += updateSize)
        {
            buffer.set(update.slice(u_i, u_i + updateSize), b_i + updateOffset);
        }

        return buffer;
    }

    private static copyPosition(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        if(elementsPerRow == 3)
        {
            return update;
        }

        return this.partialCopy(buffer, elementsPerRow, update, 0, 3);
    }

    private static copyNormal(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        return this.partialCopy(buffer, elementsPerRow, update, 3, 3);
    }

    private static copyRotation(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        return this.partialCopy(buffer, elementsPerRow, update, 3, 4);
    }

    private static copyColor(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        return this.partialCopy(buffer, elementsPerRow, update, elementsPerRow - 3, 3); 
    }

    private static copyPositionRotation(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        if(elementsPerRow == 7)
        {
            return update;
        }

        return this.partialCopy(buffer, elementsPerRow, update, 0, 7);
    }

    private static copyPositionNormal(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        if(elementsPerRow == 6)
        {
            return update;
        }

        return this.partialCopy(buffer, elementsPerRow, update, 0, 6);
    }

    private static copyPositionColor(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        if(elementsPerRow == 6)
        {
            return update;
        }

        let offset = elementsPerRow - 3;
        buffer = Float32Array.from(buffer);
        for(let b_i=0, u_i=0; b_i < buffer.length; b_i += elementsPerRow, u_i += 6)
        {
            buffer.set(update.slice(u_i, u_i + 3), b_i);
            buffer.set(update.slice(u_i + 3, u_i + 6), b_i + offset);
        }

        return buffer;
    }

    private static copyRotationColor(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        return this.partialCopy(buffer, elementsPerRow, update, 3, 7);
    }

    private static copyNormalColor(buffer: Float32Array, update: Float32Array, elementsPerRow: number) : Float32Array
    {
        return this.partialCopy(buffer, elementsPerRow, update, 3, 6);
    }

    static Update(buffer: Float32Array, elementsPerRow: number, update: Float32Array, updateFlags: VertexBufferType) : Float32Array
    {
        switch(updateFlags)
        {
            case VertexBufferType.Positions:
                return this.copyPosition(buffer, update, elementsPerRow);
            
            case VertexBufferType.Normals:
                return this.copyNormal(buffer, update, elementsPerRow);
            
            case VertexBufferType.Rotations:
                return this.copyRotation(buffer, update, elementsPerRow);
            
            case VertexBufferType.Colors:
                return this.copyColor(buffer, update, elementsPerRow);
            
            case VertexBufferType.Positions | VertexBufferType.Rotations:
                return this.copyPositionRotation(buffer, update, elementsPerRow);
            
            case VertexBufferType.Positions | VertexBufferType.Normals:
                return this.copyPositionNormal(buffer, update, elementsPerRow);
            
            case VertexBufferType.Positions | VertexBufferType.Colors:
                return this.copyPositionColor(buffer, update, elementsPerRow);
            
            case VertexBufferType.Rotations | VertexBufferType.Colors:
                return this.copyRotationColor(buffer, update, elementsPerRow);
            
            case VertexBufferType.Normals | VertexBufferType.Colors:
                return this.copyNormalColor(buffer, update, elementsPerRow);
            
            case VertexBufferType.Positions | VertexBufferType.Normals | VertexBufferType.Colors:
                return update;
            
            case VertexBufferType.Positions | VertexBufferType.Rotations | VertexBufferType.Colors:
                return update;
        }

        return buffer;
    }
}