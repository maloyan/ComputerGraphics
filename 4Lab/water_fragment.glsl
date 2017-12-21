#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;

uniform float time;
uniform sampler2D textureWater;

void main()
{
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 
  vec2 t   = vTexCoords + 0.005 * vec2(time, time);
  vec3 col = texture(textureWater, t).rgb;

  float kd = max(dot(vNormal, lightDir), 0.0);

  color = vec4(kd * col, 0.5f);
}