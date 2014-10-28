/* [FRAG] */
#version 330 /* Specify which version of GLSL we are using. */ 
#extension GL_EXT_gpu_shader4 : enable
uniform vec4 baseColor; 

//layout(location=0) out vec4 fragColor;
out vec4 fragColor;

uniform int gDrawIndex;
uniform int gObjectIndex;

void main() 
{ 
 fragColor = baseColor; 
 //fragColor = vec4(gObjectIndex, gDrawIndex, gl_PrimitiveID, 0.5);
} 