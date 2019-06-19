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
COMPAT_ATTRIBUTE vec2 TexCoord0;
COMPAT_VARYING vec2 TEX0;
uniform mat4 wvp;
 
void main()
{
    vec4 PositionCS = Position * wvp;
    TEX0 = TexCoord0;
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
uniform sampler2D base_sampler;
 
void main()
{
    vec4 color = COMPAT_TEXTURE(base_sampler, TEX0.xy);
    FragColor = color * vec4(TEX0.x, TEX0.y, 0.00000000E+00, 1.00000000E+00);
} 
#endif
