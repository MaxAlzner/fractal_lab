#version 130

in vec4 vertex;

out vec2 tex_coord;

void main()
{
	tex_coord = vertex.xy;
	gl_Position = (vertex * 2.0) - 1.0;
}