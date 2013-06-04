#version 130

in vec2 tex_coord;

out vec4 outColor;

uniform sampler2D palette;

uniform vec2 dimensions;
uniform int iterations;

const vec3 color0 = vec3(0.4, 0.1, 0.0);
const vec3 color1 = vec3(0.8, 0.9, 0.3);
const vec3 color2 = vec3(0.9, 0.4, 0.2);

vec3 calc_color(float value)
{
	vec3 color;
	if (value < 0.5)
	{
		color = (color1 - color0) * value;
		color += color0;
	}
	else
	{
		color = (color2 - color1) * value;
		color += color1;
	}
	return color;
}

void main()
{
	vec2 coord = vec2(
		(4. / 3.) * (tex_coord.x - 0.5), 
		(tex_coord.y - 0.5)
	);
	coord *= 2.0;
	coord -= vec2(0.4, 0.0);

	int itr;
	vec2 z = coord;
	for (itr = 0; itr < iterations; itr++)
	{
		float x = ((z.x * z.x) - (z.y * z.y)) + coord.x;
		float y = ((z.y * z.x) + (z.x * z.y)) + coord.y;

		float x2 = x * x;
		float y2 = y * y;
		if ((x2 + y2) > 4.0) break;
		z.x = x;
		z.y = y;
	}
	float v = float(itr) / float(iterations);
	if (itr == iterations) v = 0.0;

	outColor = texture(palette, vec2(v, 0.));
	//outColor = vec4(vec3(1., 1., 1.) * v, 1.);
}