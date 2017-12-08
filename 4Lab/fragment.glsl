#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;


void main()
{
  
  // Blue for see level (at and below ground)
  const vec3 seeColor = vec3(0.0, 0.0, 0.5);
  // Yellow at the ground level.
  const float ground = 0.01f;
  const vec3 groundColor = vec3(0.6f, 0.6f, 0.0f);
  // Green at vegetation start.
  const float vegstart = 2;
  const vec3 vegstartColor = vec3(0.0f, 0.4f, 0.0f);
  // Black at the vegetation limit.
  const float veglim = 20;
  const vec3 veglimColor = vec3(0.0f, 0.0f, 0.0f);
  // White at the mountain top.
  const float top = 24;
  const vec3 topColor = vec3(1.0f, 1.0f, 1.0f);
  float minHeight, maxHeight;
  vec3 minColor, maxColor;
  vec3 mapped;
  vec3 lightDir = vec3(1.0f, 1.0f, 0.0f); 
  
    if(vFragPosition.y > veglim) {
        maxHeight = top;
        minHeight = veglim;
        maxColor  = topColor;
        minColor  = veglimColor;
    }
    if(vFragPosition.y > vegstart && vFragPosition.y <= veglim) {
        maxHeight = veglim;
        minHeight = vegstart;
        maxColor  = veglimColor;
        minColor  = vegstartColor;
    }
    if(vFragPosition.y > ground && vFragPosition.y <= vegstart) {
        maxHeight = vegstart;
        minHeight = ground;
        maxColor  = vegstartColor;
        minColor  = groundColor;
    }
    if(vFragPosition.y <= ground) {
        maxHeight = vegstart;
        minHeight = ground;
        maxColor  = seeColor;
        minColor  = seeColor;
    }
  mapped = (vFragPosition.y-minHeight) / (maxHeight-minHeight) * (maxColor-minColor) + minColor;
  float kd = max(dot(vNormal, lightDir), 0.0);
  color = vec4(kd * mapped, 1.0f);
}