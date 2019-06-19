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
COMPAT_ATTRIBUTE vec4 Color0;
COMPAT_VARYING vec4 _NormalVS;
COMPAT_VARYING vec4 _Color0;
COMPAT_VARYING vec4 _LightPosVS;
uniform mat4 World, View, Projection;
 
void main()
{
    vec4 LightPos = vec4(0.f, 1.0f, 0.0f, 1.0f);
    vec4 LightPosWS = LightPos * World;
    _LightPosVS = LightPosWS * View;
    _LightPosVS = normalize(_LightPosVS * 0.5 + 0.5);

    vec4 PositionWS = Position * World;
    vec4 PositionVS = PositionWS * View;
    vec4 PositionCS = PositionVS * Projection;

    vec4 NormalVS = vec4(Normal.xyz, 0.0f) * World;// * View;
    _NormalVS = normalize(NormalVS * 0.5 + 0.5);
    _Color0 = vec4(Color0.xyz, 1.0); 
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

COMPAT_VARYING vec4 _LightPosVS;
COMPAT_VARYING vec4 _Color0;
COMPAT_VARYING vec4 _NormalVS;
 
void main()
{
    float dp = 0.5 + dot(normalize(_NormalVS.xyz * 2 - 1), vec3(0, 1, 0));
    FragColor = vec4(_Color0.xyz * dp, 1.0f);
} 
#endif
