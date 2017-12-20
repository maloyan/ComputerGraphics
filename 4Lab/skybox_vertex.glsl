#version 330 core
layout(location = 0) in vec3 vertex;

out vec3 vTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 campos;

void main()
{
  gl_Position = projection * view * vec4(vertex.xyz + campos, 1.0f);

  vTexCoords = vertex;

}