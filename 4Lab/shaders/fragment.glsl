#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform sampler2D ourTexture3;
uniform sampler2D ourTexture4;
uniform sampler2D ourTexture5;

void main()
{
  const float sandMin   = 0;
  const float sandMax   = 2;
  const float sandGross = 4;
  const float gross     = 15;
  const float grossRocks= 20;
  const float rocks     = 23;
  const float snow      = 24;

  // Blue for see level (at and below ground)
  const vec3 seeColor = vec3(0.0, 0.0, 0.5);
  // Yellow at the ground level.
  const vec3 groundColor = vec3(0.6f, 0.6f, 0.0f);
  // Green at vegetation start.
  const vec3 vegstartColor = vec3(0.0f, 0.4f, 0.0f);
  // Black at the vegetation limit.
  const vec3 veglimColor = vec3(0.0f, 0.0f, 0.0f);
  // White at the mountain top.
  const vec3 topColor = vec3(1.0f, 1.0f, 1.0f);

  float slope = 0.5;

  float minHeight, maxHeight;
  vec3 minColor, maxColor;
  vec3 mapped;
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 
  
    if (vFragPosition.y < sandMin) {
      color = texture(ourTexture1, vTexCoords);
    } else if (vFragPosition.y < sandMax) {
      color = texture(ourTexture2, vTexCoords);
    } else if (vFragPosition.y < sandGross) {
      color = mix(texture(ourTexture2, vTexCoords), texture(ourTexture3, vTexCoords), slope);
    } else if (vFragPosition.y < gross) {
      color = texture(ourTexture3, vTexCoords);
    } else if (vFragPosition.y < grossRocks) {
      color = mix(texture(ourTexture3, vTexCoords), texture(ourTexture4, vTexCoords), slope);
    } else if (vFragPosition.y < rocks) {
      color = texture(ourTexture4, vTexCoords);
    } else {
      color = texture(ourTexture5, vTexCoords);
    } 
  //mapped = (vFragPosition.y-minHeight) / (maxHeight-minHeight) * (maxColor-minColor) + minColor;
  //float kd = max(dot(vNormal, lightDir), 0.0);
  //vec4(kd * mapped, 1.0f);
}