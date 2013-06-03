#include "main.h"

static const float plane_data[] = 
{
	 1.0,  1.0, 0.0, 1.0, 
	-1.0,  1.0, 0.0, 1.0, 
	 1.0, -1.0, 0.0, 1.0, 
	-1.0,  1.0, 0.0, 1.0, 
	 1.0, -1.0, 0.0, 1.0, 
	-1.0, -1.0, 0.0, 1.0, 
};
static const float triangle_data[] = 
{
	 0.0f,  0.43301270f, 0.0f, 1.0f, 
	-0.5f, -0.43301270f, 0.0f, 1.0f, 
	 0.5f, -0.43301270f, 0.0f, 1.0f, 
};

static const int screen_width = 640;
static const int screen_height = 640;
static const char* vert_filepath = "data/gl_fractal.vert";
static const char* frag_filepath = "data/gl_fractal.frag";
static const char* uniform_locations[] = {"rotation", "scale", "position", "color", "dimensions", "iterations"};
static const unsigned num_of_uniforms = 6;
static const char* attribute_locations[] = {"vertex"};
static const unsigned num_of_attributes = 1;

static bool AppRunning = true;

static SDL_Surface* ScreenSurface = NULL;
static GLuint ShaderProgram = 0;

static GLuint array_buffer = 0;
static std::vector<GLint> uniforms;
static std::vector<GLuint> textures;

static float pi = 3.1415926f;
static float delta = 0.0f;
static float mx = 0.0f;
static float my = 0.0f;

static int iterations = 32;

void OnFrame()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUniform1f(uniforms[0], 0.0f);
	glUniform2f(uniforms[1], 1.0f, 1.0f);
	glUniform2f(uniforms[2], 0.0f, 0.0f);
	glUniform4f(uniforms[3], 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform2f(uniforms[4], screen_width, screen_height);
	glUniform1i(uniforms[5], iterations);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapBuffers();
}
void OnTick()
{
	delta += 0.001f;
	if (delta > pi * 2.0f) delta = 0.0f;

	LOG::Out1i("Iterations", iterations);
}
void OnKey(char code)
{
	switch (code)
	{
	case SDLK_ESCAPE:
		AppRunning = false;
		break;
	case 'n':
		iterations--;
		if (iterations < 0) iterations = 0;
		break;
	case 'm':
		iterations++;
		break;
	default:
		break;
	}
}

bool BuildShader(GLuint* program, const char* vert_loc, const char* frag_loc, const char** outError)
{
	if (program == NULL || vert_loc == NULL || frag_loc == NULL) return false;
	
	char* error_log = NULL;
	const char* vs_code = gluLoadTextFile(vert_loc);
	const char* fs_code = gluLoadTextFile(frag_loc);
	if (vs_code == NULL || fs_code == NULL) return false;

	GLuint vs = gluCompileShader(GL_VERTEX_SHADER, vs_code, &error_log);
	if (error_log != 0 && strcmp("No errors.", error_log) != 0)
	{
		if (outError != NULL) *outError = error_log;
		return false;
	}
	GLuint fs = gluCompileShader(GL_FRAGMENT_SHADER, fs_code, &error_log);
	if (error_log != 0 && strcmp("No errors.", error_log) != 0)
	{
		if (outError != NULL) *outError = error_log;
		return false;
	}
	if (outError != NULL) *outError = error_log;
	
	*program = glCreateProgram();
	glAttachShader(*program, vs);
	glAttachShader(*program, fs);

	if (!gluLinkProgram(*program, &error_log)) 
		return false;

	glUseProgram(*program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	return true;
}
bool BindUniforms(GLuint program, const char** uniform_locations, unsigned int size)
{
	if (uniform_locations == NULL || size < 1) return false;

	uniforms.clear();
	for (unsigned i = 0; i < size; i++)
	{
		const char* uniform = *uniform_locations;
		if (uniform == NULL) return false;
		uniforms.push_back(glGetUniformLocation(program, uniform));
		uniform_locations++;
	}
	return true;
}
bool BindAttributes(GLuint program, const char** attributes, unsigned int size)
{
	if (attributes == NULL || size < 1) return false;

	for (unsigned i = 0; i < size; i++)
	{
		const char* attribute = *attributes;
		if (attribute == NULL) return false;
		glBindAttribLocation(program, i, attribute);
		attributes++;
	}
	return true;
}

bool Reshape(int width, int height)
{
	ScreenSurface = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_RESIZABLE);
	if (ScreenSurface == NULL) 
		exit(1);
	
	glViewport(0, 0, width, height);

	return true;
}
bool Listen(SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
		OnKey(event.key.keysym.sym);
		break;
	case SDL_KEYUP:
		break;
	case SDL_MOUSEMOTION:
		mx = float(event.motion.x) / float(screen_width);
		my = float(event.motion.y) / float(screen_height);
		break;
	case SDL_MOUSEBUTTONDOWN:
		break;
	case SDL_MOUSEBUTTONUP:
		break;
	case SDL_JOYAXISMOTION:
		break;
	case SDL_JOYBUTTONDOWN:
		break;
	case SDL_JOYBUTTONUP:
		break;
	case SDL_QUIT:
		AppRunning = false;
		break;
	case SDL_VIDEORESIZE:
		Reshape(event.resize.w, event.resize.h);
		break;
	default:
		return false;
		break;
	}
	return true;
}
bool Init()
{
	LOG::Initialize();
	glewInit();
	if (!GLEW_VERSION_3_0)
	{
		LOG::Message("Requires OpenGL 3.0 or later.\n");
		return false;
	}
	gluInitializeCompiler();

	SDL_WM_SetCaption("FractalDemo", "FractalDemo");

	const char* shader_error = "";
	
	if (!BuildShader(&ShaderProgram, vert_filepath, frag_filepath, &shader_error))
	{
		LOG::Message(shader_error);
		return false;
	}
	if (!BindUniforms(ShaderProgram, uniform_locations, num_of_uniforms))
	{
		LOG::Message("Could not bind uniforms.\n");
		return false;
	}
	if (!BindAttributes(ShaderProgram, attribute_locations, num_of_attributes))
	{
		LOG::Message("Could not bind attributes.\n");
		return false;
	}

	glGenBuffers(1, &array_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), plane_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	return true;
}
bool Uninit()
{
	LOG::Unitialize();
	return true;
}

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
		exit(1);
	srand((unsigned int)time(NULL));
	
	AppRunning = AppRunning && Reshape(screen_width, screen_height);
	AppRunning = AppRunning && Init();
	
	time_t lastTime = time(0);
	while(AppRunning)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
			Listen(event);
		
		time_t newTime = time(0);
		if (newTime > lastTime)
		{
			OnTick();
			OnFrame();
			lastTime = newTime;
		}
	}

	Uninit();
	SDL_Quit();
	return 0;
}