const int lightmax = 2;
uniform vec3 lpos[lightmax], ldir[lightmax], lbright[lightmax];
uniform mat4x4 lprojMatrix[lightmax];
uniform sampler2D depthtex[lightmax];

uniform sampler2D texture;
varying vec4 vpos;
vec4 vlpos;
varying vec3 vnorm;
uniform int lightnum;
float f(float x){
    if(x>0)
        return 1.25-1.25/(x+1);
    else
        return -1.25+1.25/(-x+1);
}
vec3 f(vec3 x){
    vec3 sg = sign(x);
    return sg*(vec3(1.25)-vec3(1.25)/(x*sg+vec3(1)));
}
void main()
{
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st);
    float depth;
    vec3 vp = vpos.xyz/vpos.w;
    int i;
    vec3 res = vec3(0.0);
    for(i=0;i<lightmax;i++)
      if(dot(lbright[i], lbright[i])>0){
        vlpos = lprojMatrix[i] * vpos;
        vec3 crd = vlpos.xyz/vlpos.w;
        depth = texture2D(depthtex[i], crd.xy).x;
        float ambient = max(dot(vnorm, normalize(lpos[i]-vp)),0);
        ambient = sqrt(2*ambient-ambient*ambient);
        float dst = (crd.x-0.5)*(crd.x-0.5)+(crd.y-0.5)*(crd.y-0.5);
        if(crd.z > depth && crd.z < 0.999 || dot(ldir[i], vp-lpos[i]) <0 || dst>0.23){
            if(!(crd.z > depth && crd.z < 0.999 || dot(ldir[i], vp-lpos[i]) <0) && dst<=0.25)
                res += lbright[i]*ambient*(0.25-dst)*50;
        }
        else
            res += lbright[i]*ambient;
    }
    res+=vec3(0.2);
    gl_FragColor *= vec4(f(res), 1);
}
