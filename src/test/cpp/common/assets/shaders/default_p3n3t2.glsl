
#if defined(VERTEX)

#if __VERSION__ >= 130
#define COMPAT_VARYING out
#define COMPAT_ATTRIBUTE in
#define COMPAT_TEXTURE texture
#else
#define COMPAT_VARYING varying
#define COMPAT_ATTRIBUTE attribute
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
COMPAT_ATTRIBUTE vec4 Position;
COMPAT_ATTRIBUTE vec3 Normal;
COMPAT_ATTRIBUTE vec2 TexCoord0;
COMPAT_VARYING vec2 TEX0;
COMPAT_VARYING vec3 NORMAL;
COMPAT_VARYING vec3 vertexPositionVS;
COMPAT_VARYING vec3 lightPositionVS;
uniform mat4 World, View, Projection;
 
void main()
{
    vec3 lightPositionWS = vec3(0, 10, 0);
    lightPositionVS = (vec4(lightPositionWS, 1.0f) * View).xyz;

    vec4 PositionWS = vec4(Position.xyz, 1.0) * World;
    vec4 PositionVS = vec4(PositionWS.xyz, 1.0) * View;
    vec4 PositionCS = vec4(PositionVS.xyz, 1.0) * Projection;
    vertexPositionVS = PositionVS.xyz;
    TEX0 = TexCoord0;
    vec4 NormalWS = vec4(Normal.xyz, 0.0f) * World;
    vec4 NormalVS = vec4(NormalWS.xyz, 0.0f) * View;

    NORMAL = NormalVS.xyz;
    gl_Position = PositionCS;
} 
#elif defined(FRAGMENT)

#if __VERSION__ >= 130
#define COMPAT_VARYING in
#define COMPAT_TEXTURE texture
out vec4 FragColor;
#else
#define COMPAT_VARYING varying
#define FragColor gl_FragColor
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
COMPAT_VARYING vec2 TEX0;
COMPAT_VARYING vec3 NORMAL;
COMPAT_VARYING vec3 POSITION;
COMPAT_VARYING vec3 lightPositionVS;
COMPAT_VARYING vec3 vertexPositionVS;
//uniform sampler2D base_sampler;
 
void main()
{
    vec3 lightPosition = vec3(0, 10, 0);
    vec3 n = normalize(NORMAL);
    vec3 l = normalize(lightPositionVS - vertexPositionVS);
    float d = max(dot(n, l), 0.0);
    FragColor = vec4(d,d,d, 1.0);
} 
#endif
