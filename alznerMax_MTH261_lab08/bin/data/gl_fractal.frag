#version 130

in vec2 tex_coord;

out vec4 outColor;

uniform sampler2D palette;

uniform vec2 zoom;
uniform vec2 center;
uniform int iterations;
uniform int flag;

const vec3 color0 = vec3(0.9, 0.5, 0.1);
const vec3 color1 = vec3(0.9, 0.8, 0.3);

vec3 calc_color(float value)
{
	vec3 color;
	color = (color1 - color0) * value;
	color += color0;
	return color;
}

float mandelbrot()
{
	vec2 coord = vec2(
		(4. / 3.) * (tex_coord.x - 0.5), 
		(tex_coord.y - 0.5)
	);
	coord *= zoom;
	coord -= center;

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

	return v;
}
float burning_ship()
{
	vec2 coord = tex_coord;
	coord *= zoom;
	coord -= center;

	int itr;
	vec2 z = coord;
	for (itr = 0; itr < iterations; itr++)
	{
		float x = ((z.x * z.x) - (z.y * z.y)) - coord.x;
		float y = abs(2. * (z.x * z.y)) - coord.y;

		float x2 = x * x;
		float y2 = y * y;
		if ((x2 + y2) > 4.0) break;
		z.x = x;
		z.y = y;
	}
	float v = float(itr) / float(iterations);

	return v;
}

void main()
{
	float v = 0.;

	if (flag == 0) v = mandelbrot();
	else if (flag == 1) v = burning_ship();

	outColor = vec4(calc_color(v), 1.);
	//outColor = vec4(vec3(1., 1., 1.) * v, 1.);
}