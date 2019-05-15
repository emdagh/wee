
attribute vec4 Position;
attribute vec3 Normal;
attribute vec4 TexCoord0;
attribute ivec4 BlendIndex;
attribute vec4 BlendWeight;

uniform mat4 worldViewProjection;

varying vec4 oNormal;
varying vec2 oTexCoord;

void main() {
    mat4 boneTransform = 0;
    boneTransform += Bones[BlendIndex[0]] * BlendWeight[0];
    boneTransform += Bones[BlendIndex[1]] * BlendWeight[1];
    boneTransform += Bones[BlendIndex[2]] * BlendWeight[2];
    boneTransform += Bones[BlendIndex[3]] * BlendWeight[3];

    vec4 PositionBS = vec4(Position.xyz, 1)) * boneTransform;
    vec4 PositionCS = PositionBS * worldViewProjection;

    vec4 NormalBS = vec4(Normal.xyz, 0.0f) * boneTransform;
    vec4 NormalCS = NormalBS * worldViewProjection;

    gl_Position = PositionCS;

}
