#version 130

in vec4 vertex;

out vec2 tex_coord;

uniform float rotation;
uniform vec2 scale;
uniform vec2 position;

void main()
{
	vec2 mapped_pos = vertex.xy * scale;
	mapped_pos = vec2(
		(cos(rotation) * mapped_pos.x) - (sin(rotation) * mapped_pos.y), 
		(sin(rotation) * mapped_pos.x) + (cos(rotation) * mapped_pos.y)
	);
	mapped_pos += position;

	tex_coord = (vertex.xy * 0.5) + 0.5;
	gl_Position = vec4(mapped_pos, 0.0, 1.0);
}