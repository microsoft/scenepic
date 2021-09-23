import {mat3, mat4, vec3, vec4, quat} from "gl-matrix";
import * as pako from "pako";

export default class Misc
{
    static Sign(value: number): number
    {
        if(value < 0)
        {
            return -1;
        }
    
        if(value > 0)
        {
            return 1;
        }
    
        return 0;
    }

    static Mat3FromMat4(m4 : mat4) : mat3
    {
        var m3 = mat3.create();
        mat3.fromMat4(m3, m4);
        return m3;
    }

    static NormalizeVec3InPlace(vec : Float32Array, ind : number)
    {
        var norm = 1.0/Math.sqrt(vec[ind] * vec[ind] + vec[ind+1] * vec[ind+1] + vec[ind+2] * vec[ind+2]);
        vec[ind] *= norm;
        vec[ind+1] *= norm;
        vec[ind+2] *= norm;
    }

    static GetDefault(obj, name, defaultValue) : any
    {
        if (name in obj)
            return obj[name];
        else
            return defaultValue;
    }

    static PushOrCreateArray(obj, key, value)
    {
        if (!(key in obj))
            obj[key] = [];
        obj[key].push(value);
    }

    static DecoderArray : any = null;

    static Base64ToArrayBuffer(base64str : string)
    {
        // Initialize decoder array if necessary (not threadsafe)
        if (Misc.DecoderArray == null)
        {
            const CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
            Misc.DecoderArray = {};
            for (var i = 0; i < CODES.length; i++)
                Misc.DecoderArray[CODES.charCodeAt(i)] = i;
        }

        // Allocate a large enough buffer
        var countBytes = (base64str.length / 4) * 3;
        if (base64str.indexOf("=") > 0) // Deal with padding
            countBytes -= base64str.length - base64str.indexOf("=");

        var aBuff = new ArrayBuffer(countBytes);
        var aView = new Uint8Array(aBuff);

        // Decode
        for(var i = 0, j = 0; i < countBytes; i += 3)
        {
            // Get characters
            var c0 = base64str.charCodeAt(j++);
            var c1 = base64str.charCodeAt(j++);
            var c2 = base64str.charCodeAt(j++);
            var c3 = base64str.charCodeAt(j++);

            // Get code indices
            var e0 = Misc.DecoderArray[c0];
            var e1 = Misc.DecoderArray[c1];
            var e2 = Misc.DecoderArray[c2];
            var e3 = Misc.DecoderArray[c3];

            aView[i] = (e0 << 2) | (e1 >> 4);
            if (e2 != 64) aView[i+1] = ((e1 & 15) << 4) | (e2 >> 2);
            if (e3 != 64) aView[i+2] = ((e2 & 3) << 6) | e3;
        }

        let result : Uint8Array;
        try {
            result = pako.inflate(aView);
        } catch (err) {
            return aBuff;
        }

        return result.buffer;
    }

    static DataUrlToBlob(dataUrl: string) : Blob
    {
        let parts = dataUrl.split(',');
        let mime = parts[0].match(/:(.*?);/)[1];
        let array = new Uint8Array(Misc.Base64ToArrayBuffer(parts[1]))
        return new Blob([array], {"type": mime});
    }

    static Base64ToBlob(base64: string, mime: string) : Blob
    {
        let array = new Uint8Array(Misc.Base64ToArrayBuffer(base64));
        return new Blob([array], {"type": mime});
    }

    static DataUrlToArrayBuffer(dataUrl: string) : ArrayBuffer
    {
        let parts = dataUrl.split(',');
        return Misc.Base64ToArrayBuffer(parts[1]);
    }

    // Convert either from Base64 string or from regular array to Float32Array
    static Base64ToFloat32Array(obj : any)
    {
        if (obj == null) return null;
        if (typeof obj == "string")
            return new Float32Array(Misc.Base64ToArrayBuffer(obj));
        else
            return new Float32Array(obj);
    }

    static Base64ToUInt8Array(obj: any)
    {
        if (obj == null) return null;
        if (typeof obj == "string")
            return new Uint8Array(Misc.Base64ToArrayBuffer(obj));
        else
            return new Uint8Array(obj)
    }

    // Convert either from Base64 string or from regular array to Int16Array
    static Base64ToUInt16Array(obj : any)
    {
        if (obj == null) return null;
        if (typeof obj == "string")
            return new Uint16Array(Misc.Base64ToArrayBuffer(obj));
        else
            return new Uint16Array(obj);
    }

    // Convert either from Base64 string or from regular array to Int32Array
    static Base64ToUInt32Array(obj : any)
    {
        if (obj == null) return null;
        if (typeof obj == "string")
            return new Uint32Array(Misc.Base64ToArrayBuffer(obj));
        else
            return new Uint32Array(obj);
    }

    static GetSearchValue(name : string)
    {
        var searchStr = location.search.substring(1);
        var vars = searchStr.split('&');
        for (var i = 0; i < vars.length; i++)
        {
            var pair = vars[i].split('=');
            if (decodeURIComponent(pair[0]) == name)
            {
                return decodeURIComponent(pair[1]);
            }
        }
        return null;
    }

    static StyleToHtmlHex(style: Uint8Array) : string | null
    {
        if(style[0] == 1){
            return null;
        }

        let red = style[1];
        let green = style[2];
        let blue = style[3];
        return "#" + Misc.Byte2Hex(red) + Misc.Byte2Hex(green) + Misc.Byte2Hex(blue);
    }

    static Byte2Hex (value: number) : string
    {
      const hex_string = "0123456789ABCDEF";
      return String(hex_string.substr((value >> 4) & 0x0F,1)) + hex_string.substr(value & 0x0F,1);
    }

    static IsPow2(val : number)
    {
        var l = Math.log(val) / Math.log(2);
        return l == Math.floor(l);
    }
}
