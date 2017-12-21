#version 330
layout(triangles) in;

layout(line_strip, max_vertices=6) out;


float normal_length = 10;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in Vertex
{
  vec3 normal;
  vec4 color;
} Svertex[];

out vec4 vertex_color;

void main()
{
  int i;
  for(i=0; i<gl_in.length(); i++)
  {
    vec3 P = gl_in[i].gl_Position.xyz;
    vec3 N = Svertex[i].normal.xyz;
    
    gl_Position = projection * view * model * vec4(P, 1.0);
    vertex_color = Svertex[i].color;
    EmitVertex();
    
    gl_Position = projection * view * model * vec4(P + N * normal_length, 1.0);
    vertex_color = Svertex[i].color;
    EmitVertex();
    
    EndPrimitive();
  }
}
