#version 130

in vec2 tex_coord;

out vec4 outColor;

uniform vec4 color;
uniform vec2 dimensions;
uniform int iterations;

const float line_width = 1.0;

float on_line(vec2 coord)
{
	float d = distance(coord, vec2(coord.x, 0.5));
	return 1.0 - d;
}

void main()
{
	vec3 color = vec3(1., 1., 1.);

	vec2 coord = vec2(
#if 1
		1.3333 * (tex_coord.x - 0.5), 
		(tex_coord.y - 0.5)
#else
		3.0 * (tex_coord.x - 0.5), 
		2.0 * (tex_coord.y - 0.5)
#endif
	);
	coord *= 2.0;
	//coord -= vec2(0.5, 0.5);

	int itr;
	vec2 z = coord;
	for (itr = 0; itr < iterations; itr++)
	{
		float x = (z.x * z.x - z.y * z.y) + coord.x;
		float y = (z.y * z.x + z.x * z.y) + coord.y;

		float x2 = x * x;
		float y2 = y * y;
		if ((x2 + y2) > 4.0) break;
		z.x = x;
		z.y = y;
	}
	float v = float(itr) / float(iterations);
	if (itr == iterations) v = 0.0;

	outColor = vec4(color * v, 1.);
}