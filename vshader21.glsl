#version 150

//---- ins
in   vec4 vPosition;
in   vec3 vNormal;

//---- outs
out  vec3 fN;
out  vec3 fE;
out  vec3 fL;
out  vec4 color;

//---- uniforms
uniform vec4 LightPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vPosition;
    
    vec3 pos = (view * model * vPosition).xyz;
    
    fN = (view*model*vec4(vNormal, 0.0)).xyz;
    fE = -pos;
    fL = (view * LightPosition).xyz - pos;
}
