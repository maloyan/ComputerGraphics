#version 330 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = vec4(vertex, 1.0f);
}