varying vec4 vpos;
varying vec3 vnorm;
void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    //gl_Position = gl_ModelViewProjectionMatrix * vec4(normalize(gl_Vertex.xyz), gl_Vertex.w);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vnorm = gl_Normal;
    vpos = gl_ModelViewMatrix * gl_Vertex;
}
