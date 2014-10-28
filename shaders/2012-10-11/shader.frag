#version 330
 
struct MaterialParam {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
 
uniform MaterialParam lightMaterial;
uniform MaterialParam material;
 
in vec3 v_P;
in vec3 v_N;
in vec3 v_L;
in vec4 ex_color;

out vec4 fragColor;
 
void main(void)
{
  vec3 V = normalize(-v_P);
  vec3 N = normalize(v_N); // 正規化が必要
  vec3 L = normalize(v_L);
 
  vec4 ambient = lightMaterial.ambient * material.ambient;
 
  float nDotL = dot(N,L);
  float diffuseLight = max(nDotL, 0.0);
  vec4 diffuse = lightMaterial.diffuse * material.diffuse * diffuseLight;
 
  vec3 R = reflect(-L,N); // 反射ベクトルによるスペキュラー
  float vDotR = pow( max(dot(V,R), 0.0), material.shininess );
  if (nDotL < 0.0) vDotR = 0.0;
  vec4 specular = lightMaterial.specular * material.specular * vDotR;
 
//  vec3 H = normalize(L+V); // ハーフベクトルによるスペキュラー
//  float nDotH = pow( max(dot(N,H), 0.0) , material.shininess );
//  if (nDotL < 0.0) nDotH = 0.0;
//  vec4 specular = lightMaterial.specular * material.specular * nDotH;
 
  //fragColor = ambient + diffuse + specular + ex_color;
  fragColor = ambient + ex_color;
  //fragColor = ex_color;
}
