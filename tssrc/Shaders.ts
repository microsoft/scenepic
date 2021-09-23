interface ShaderDef
{
    type : string;
    script : string;
}

var defaultVertexShader : ShaderDef =
{
    type: "x-shader/x-vertex",
    script:
`
// Vertex inputs
attribute vec3 vertexPositionIn;
attribute vec3 vertexNormalIn;
attribute vec3 vertexColorIn;
attribute vec2 vertexTextureIn;

// Instance inputs
attribute vec3 instancePositionIn;
attribute vec4 instanceRotationIn; // Quaternion
attribute vec3 instanceColorIn;

// Where to get color from (0 = per-vertex, 1 = shared-color, 2 = instance-color)
uniform int colorSource;

// Shared Color
uniform vec3 sharedColor;

// Transformation matrices
uniform int useInstanceRotation;
uniform mat4 projMatrix; // Incorporates: model to world to view to screen
uniform mat4 m2vMatrix; // Incorporates: model to world to view
uniform mat3 normMatrix;

// Alpha
uniform float alpha;

// Outputs
varying vec4 vertexColor; // Color with alpha
varying vec2 vertexTextureCoord; // Texture
varying vec3 positionView;
varying vec3 normalView;

void main()
{
    // Rotation by quaternion
    vec3 vertexPosition = vertexPositionIn;
    vec3 vertexNormal = vertexNormalIn;
    if (useInstanceRotation == 1)
    {
        // Apply quaternion rotation to position and normal
        // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
        vertexPosition += 2.0 * cross(instanceRotationIn.xyz, cross(instanceRotationIn.xyz, vertexPosition) + instanceRotationIn.w * vertexPosition);
        vertexNormal += 2.0 * cross(instanceRotationIn.xyz, cross(instanceRotationIn.xyz, vertexNormal) + instanceRotationIn.w * vertexNormal); 
    }
    vertexPosition += instancePositionIn;

    // Transform position and normal
    gl_Position = projMatrix * vec4(vertexPosition, 1.0);
    positionView = (m2vMatrix * vec4(vertexPosition, 1.0)).rgb; // Unprojected version of above, for fragment shading
    normalView = normalize(normMatrix * vertexNormal).rgb;

    // Color etc. information
    if (colorSource == 2) // Per-instance color
    {
        vertexColor.rgb = instanceColorIn;
    }
    else if (colorSource == 1) // Shared color
    {
        vertexColor.rgb = sharedColor;
    }
    else // Per vertex color
    {
        vertexColor.rgb = vertexColorIn;
    }

    // Set alpha
    vertexColor.a = alpha;

    // Copy texture coordinates
    vertexTextureCoord = vertexTextureIn;
}
`
};

var defaultFragmentShader : ShaderDef =
{
    type : "x-shader/x-fragment",
    script : 
`
precision mediump float;

// Lighting
uniform float lightingMultiplier;
uniform vec3 ambientLightColor;
uniform vec3 directionalLightDir;
uniform vec3 directionalLightColor;

varying vec4 vertexColor;
varying vec2 vertexTextureCoord;
varying vec3 positionView;
varying vec3 normalView;

uniform int shadingType;
uniform sampler2D sampler; // Represents the texture

// Constants
float FresBias = 0.0;
float FresStren = 1.0;
float FresPow = 2.0;
float GaussConstant = 100.0;
float PI = 3.1415926;

float saturate(float f)
{
  return clamp(f, 0.0, 1.0);  
}

float f_lambert(vec3 normal, vec3 light)
{
  return max(0.0, dot(normal, light));
}

float f_cook_torrance(vec3 normal, vec3 viewer, vec3 light, float roughness)
{  
    // Compute intermediary values
    vec3 half_vector = normalize(light + viewer);
    float NdotL = saturate(dot(normal, light));
    float NdotH = saturate(dot(normal, half_vector));
    float NdotV = saturate(dot(normal, viewer));
    float VdotH = saturate(dot(viewer, half_vector));

    // Approximate the Fresnel value
    float F = FresBias + FresStren * pow((1.0 - NdotV), FresPow);
    F = saturate(F);

    // Microfacet distribution
    float alpha = acos(NdotH);
    float D = GaussConstant * exp(-(alpha * alpha) / (roughness * roughness));

    // Geometric attenuation factor
    float G = min(1.0, min((2.0 * NdotH * NdotV / VdotH), (2.0 * NdotH * NdotL / VdotH)));

    return saturate(max(0.001, ((F * D * G) / (PI * NdotV * NdotL))));
}

void main()
{
    if (shadingType == -1) // Vertex shading only, not pixel shading
    {
        gl_FragColor = vertexColor;
        gl_FragColor.rgb *= lightingMultiplier;
    }
    else
    {
        vec3 L = normalize(directionalLightDir);
        vec3 V = normalize(-positionView);
        vec3 N = normalize(normalView);

        // Diffuse color and alpha
        vec3 diffCol;
        float alpha;
        if (shadingType == 0) // Vertex color
        {
            diffCol = vertexColor.rgb;
            alpha = vertexColor.a;
        }
        else if (shadingType == 1 || shadingType == 2) // Texture
        {
            vec4 texCol = texture2D(sampler, vertexTextureCoord);
            diffCol = texCol.rgb;
            alpha = texCol.a * vertexColor.a; // Preserve texture alpha
        }
        else // ERROR: Unknown shader
        {
            diffCol = vec3(1.0, 0.0, 0.0);
            alpha = 1.0;
        }

        // Specular color
        vec3 specCol = vec3(1.0, 1.0, 1.0) * 0.75;
        
        // Lighting
        gl_FragColor.rgb = ambientLightColor * diffCol;
        if (shadingType != 2) // Don't apply this to labels
        {
            gl_FragColor.rgb += directionalLightColor * diffCol * f_lambert(N, L);
            gl_FragColor.rgb += directionalLightColor * specCol * f_cook_torrance(N, V, L, 0.2);
            gl_FragColor.rgb *= lightingMultiplier;
        }
        gl_FragColor.a = alpha;
    }
}
`
};

// Generates a shader from the webgl context and id of an html script element containing the shader code 
function makeShader(gl : WebGLRenderingContext, shaderDef : ShaderDef)
{
    // Create shader object
    var shader : WebGLShader;
    if (shaderDef.type == "x-shader/x-fragment")
        shader = gl.createShader(gl.FRAGMENT_SHADER);
    else if (shaderDef.type == "x-shader/x-vertex")
        shader = gl.createShader(gl.VERTEX_SHADER);
    else
        return null;

    // Compile shader
    gl.shaderSource(shader, shaderDef.script);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert(gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}

export default class ShaderProgram
{
    vertexShader : WebGLShader;
    fragmentShader : WebGLShader;
    program : WebGLProgram;

    // Cached attribute locations
    vertexPositionAttribLoc : number;
    vertexNormalAttribLoc : number;
    vertexColorAttribLoc : number;
    vertexTextureAttribLoc : number;
    instancePositionAttribLoc : number;
    instanceRotationAttribLoc : number;
    instanceColorAttribLoc : number;

    // Cached uniform pointers
    useInstanceRotation : WebGLUniformLocation;
    projMatrixPtr : WebGLUniformLocation;
    m2vMatrixPtr : WebGLUniformLocation;
    normMatrixPtr : WebGLUniformLocation;
    ambientLightColorPtr : WebGLUniformLocation;
    directionalLightDirPtr : WebGLUniformLocation;
    directionalLightColorPtr : WebGLUniformLocation;
    lightingMultiplierPtr : WebGLUniformLocation;
    alphaPtr : WebGLUniformLocation;
    sharedColorPtr : WebGLUniformLocation;
    colorSourcePtr : WebGLUniformLocation;
    shadingTypePtr : WebGLUniformLocation;
    samplerPtr : WebGLUniformLocation;

    constructor(gl : WebGLRenderingContext)
    {
        // Create shaders
        this.vertexShader = makeShader(gl, defaultVertexShader);
        this.fragmentShader = makeShader(gl, defaultFragmentShader);

        // Create and use program
        this.program = gl.createProgram();
        gl.attachShader(this.program, this.vertexShader);
        gl.attachShader(this.program, this.fragmentShader);
        gl.linkProgram(this.program);
        gl.useProgram(this.program);

        // Store attributes
        this.vertexPositionAttribLoc = gl.getAttribLocation(this.program, "vertexPositionIn");
        this.vertexNormalAttribLoc = gl.getAttribLocation(this.program, "vertexNormalIn");
        this.vertexColorAttribLoc = gl.getAttribLocation(this.program, "vertexColorIn");
        this.vertexTextureAttribLoc = gl.getAttribLocation(this.program, "vertexTextureIn");
        this.instancePositionAttribLoc = gl.getAttribLocation(this.program, "instancePositionIn");
        this.instanceRotationAttribLoc = gl.getAttribLocation(this.program, "instanceRotationIn");
        this.instanceColorAttribLoc = gl.getAttribLocation(this.program, "instanceColorIn");

        // Store uniforms
        this.useInstanceRotation = gl.getUniformLocation(this.program, "useInstanceRotation");
        this.projMatrixPtr = gl.getUniformLocation(this.program, "projMatrix");
        this.m2vMatrixPtr = gl.getUniformLocation(this.program, "m2vMatrix");
        this.normMatrixPtr = gl.getUniformLocation(this.program, "normMatrix");
        this.ambientLightColorPtr = gl.getUniformLocation(this.program, "ambientLightColor");
        this.directionalLightDirPtr = gl.getUniformLocation(this.program, "directionalLightDir");
        this.directionalLightColorPtr = gl.getUniformLocation(this.program, "directionalLightColor");
        this.lightingMultiplierPtr = gl.getUniformLocation(this.program, "lightingMultiplier");
        this.alphaPtr = gl.getUniformLocation(this.program, "alpha");
        this.colorSourcePtr = gl.getUniformLocation(this.program, "colorSource");
        this.sharedColorPtr = gl.getUniformLocation(this.program, "sharedColor");
        this.shadingTypePtr = gl.getUniformLocation(this.program, "shadingType");
        this.samplerPtr = gl.getUniformLocation(this.program, "sampler");
    }
}
