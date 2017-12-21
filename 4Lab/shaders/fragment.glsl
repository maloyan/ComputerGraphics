#version 330 core
in vec3 vFragPosition;
in vec2 vTexCoords;
in vec3 vNormal;

out vec4 color;

uniform int       state;
uniform sampler2D textureWater;
uniform sampler2D textureGrass;

float zNear = 0.1; 
float zFar  = 100.0; 
  

float LinearizeDepth(float depth) 
{
    // преобразуем обратно в NDC
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));  
}

void main()
{
  if (state == 1) {
    // Blue for see level (at and below ground)
    const vec3 seeColor = vec3(0.0, 0.0, 0.5);
    // Yellow at the ground level.
    const float ground = 0;
    const vec3 groundColor = vec3(0.4f, 0.6f, 0.0f);
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
    vec4 col;

    float density = 5;
    float perspective_far = 2500;
    float fogCoord = (gl_FragCoord.z / gl_FragCoord.w) / perspective_far;
    float fog = fogCoord * density;


      if(vFragPosition.y > veglim) {
          maxHeight = top;
          minHeight = veglim;
          maxColor  = topColor;
          minColor  = veglimColor;
          col = texture(textureGrass, vTexCoords);
      }
      if(vFragPosition.y > vegstart && vFragPosition.y <= veglim) {
          maxHeight = veglim;
          minHeight = vegstart;
          maxColor  = veglimColor;
          minColor  = vegstartColor;
          col = texture(textureGrass, vTexCoords);
      }
      if(vFragPosition.y > ground && vFragPosition.y <= vegstart) {
          maxHeight = vegstart;
          minHeight = ground;
          maxColor  = vegstartColor;
          minColor  = groundColor;
          col = texture(textureGrass, vTexCoords);
      }
      if(vFragPosition.y <= ground) {
          maxHeight = vegstart;
          minHeight = ground;
          maxColor  = seeColor;
          minColor  = seeColor;
          col = texture(textureWater, vTexCoords);
      }
    mapped = (vFragPosition.y-minHeight) / (maxHeight-minHeight) * (maxColor-minColor) + minColor;
    float kd = max(dot(vNormal, lightDir), 0.0);
    color = vec4(kd * mapped, 1.0f) + col;
    vec4 fogColor = vec4(0.5, 0.5, 0.5, 1);
    color = mix(fogColor, color, clamp(1.0 - fog, 0.0f, 1.0f));
  } else if (state == 2) {
    color = vec4(vNormal, 1.0f);
  } else {
    // деление на zFar для лучшей наглядности
    float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
    color = vec4(vec3(depth), 1.0);
  }
}