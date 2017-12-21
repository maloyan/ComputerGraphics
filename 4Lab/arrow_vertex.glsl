#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out Vertex
{
  vec3 normal;
  vec4 color;
} Svertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(vertex, 1.0f);
  Svertex.normal = normal;
  Svertex.color =  vec4(1.0, 1.0, 0.0, 1.0);
}