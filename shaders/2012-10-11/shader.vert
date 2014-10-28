#version 330
 
uniform mat4 modelMatrix;        // モデル・マトリックス
uniform mat4 viewMatrix;         // ビュー・マトリックス
uniform mat4 projectionMatrix;   // 射影・マトリックス
uniform vec3 lightPos;           // 光源位置

uniform mat4 pvm;
 
in vec3 in_Position;
in vec3 in_Color;
in vec3 in_Normal;
 
out vec3 v_P; // eye direction
out vec3 v_N; // normal direction
out vec3 v_L; // light direction
out vec4 ex_color;
 
void main(void)
{
  mat4 modelViewMatrix = viewMatrix * modelMatrix;
  mat3 n_mat = mat3( transpose( inverse(modelViewMatrix) ) ); // normal Matrix
  vec3 lightVec = vec3(viewMatrix * vec4(lightPos.xyz, 1.0));
 
  v_P = vec3(modelViewMatrix * vec4(in_Position,1.0));
  v_N = normalize(n_mat * in_Normal);
  v_L = normalize(lightVec - v_P);
 
  //gl_Position = u_projectionMatrix * modelViewMatrix * vec4(in_Position.xyz, 1.0);
  gl_Position = pvm*vec4(in_Position,1.0);
  ex_color = vec4(in_Color,1.0);
}
