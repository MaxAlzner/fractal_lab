#version 130

in vec2 tex_coord;

out vec4 outColor;

void main()
{
	outColor = vec4(tex_coord, 0., 1.);
}