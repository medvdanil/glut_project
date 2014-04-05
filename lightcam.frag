#version 330
varying float camdist;
uniform sampler2D texture;
uniform sampler2D depthtex;
varying vec4 lpos;
void main()
{
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st);
    float depth;
    vec2 crd = lpos.xy/lpos.w*0.5+0.5;
    depth = texture2D(depthtex, crd).x;
    gl_FragColor = vec4(depth, depth, depth, 1);
    //gl_FragColor *= min(220/camdist,1);
}
