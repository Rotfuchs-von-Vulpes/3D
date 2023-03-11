#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cglm/cglm.h>

unsigned int VBO;
unsigned int VAO;

const char *vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoord;\n"
		"out vec2 TexCoord;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"  gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
		"  TexCoord = aTexCoord.xy;\n"
		"}\0";

const char *fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D texture1;\n"
		"void main()\n"
		"{\n"
		" FragColor = texture(texture1, TexCoord);\n"
		"}";

unsigned int shaderProgram[2];
unsigned int texture1, texture2;
// unsigned int texture;
int textureCount = 1;
int shaderCount = 0;

double lastTime;
int nbFrames = 0;
int fps = 0;
char title[100];

int screen_width = 600;
int screen_height = 600;

void loadShader(unsigned int vertexShader, unsigned int fragmentShader)
{
	shaderProgram[shaderCount] = glCreateProgram();

	glAttachShader(shaderProgram[shaderCount], vertexShader);
	glAttachShader(shaderProgram[shaderCount], fragmentShader);
	glLinkProgram(shaderProgram[shaderCount]);

	glUseProgram(shaderProgram[shaderCount]);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	char infoLog[512];
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	printf("%s\n", infoLog);

	++shaderCount;
}

unsigned int loadTexture(const char *image)
{
	glGenTextures(++textureCount, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(image, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		puts("Failed to load texture");
	}
	stbi_image_free(data);

	return texture;
}

mat4 view = GLM_MAT4_IDENTITY_INIT;
mat4 projection;

vec3 cubePositions[] = {
  { 0.0f,  0.0f,  0.0f}, 
  { 2.0f,  5.0f, -15.0f}, 
  {-1.5f, -2.2f, -2.5f},  
  {-3.8f, -2.0f, -12.3f},  
  { 2.4f, -0.4f, -3.5f},  
  {-1.7f,  3.0f, -7.5f},  
  { 1.3f, -2.0f, -2.5f},  
  { 1.5f,  2.0f, -2.5f}, 
  { 1.5f,  0.2f, -1.5f}, 
  {-1.3f,  1.0f, -1.5f}  
};

void init()
{
	float vertices[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	loadShader(vertexShader, fragmentShader);
	loadShader(vertexShader, fragmentShader);

	texture1 = loadTexture("../src/default_wood.png");
	texture2 = loadTexture("../src/default_wood.png");

	glUseProgram(shaderProgram[0]);
	glUniform1i(glGetUniformLocation(shaderProgram[0], "texture"), 0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glm_translate(view, (vec3){0.0f, 0.0f, -3.0f});
	glm_perspective(45.0f, screen_width / screen_height, 0.1f, 100.0f, projection);
}

void render()
{
	double currentTime = glfwGetTime();

	glClear(GL_COLOR_BUFFER_BIT);

	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_rotate(model, 50.0f / 360 * 3.14 * (float)currentTime, (vec3){0.5f, 1.0f, 0.0f});

	// glm_translate(view, (vec3){sin(currentTime) / 1000, 0, 0});
	// glm_rotate(projection, 0.001, (vec3){1.0f, 0.0f, 0.0f});

	unsigned int transformLoc = glGetUniformLocation(shaderProgram[0], "view");
	glUniformMatrix4fv(transformLoc, 1, false, (float *)view);
	transformLoc = glGetUniformLocation(shaderProgram[0], "projection");
	glUniformMatrix4fv(transformLoc, 1, false, (float *)projection);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, texture1);
  glBindVertexArray(VAO);
	for(unsigned int i = 0; i < 10; i++)
	{
		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_translate(model, cubePositions[i]);
		float angle = 20.0f * i; 
		glm_rotate(model, 50.0f / 360 * 3.14 * (float)currentTime + angle, (vec3){0.5f, 1.0f, 0.0f});
		transformLoc = glGetUniformLocation(shaderProgram[0], "model");
		glUniformMatrix4fv(transformLoc, 1, false, (float *)model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// fps counter
	++nbFrames;
	if (currentTime - lastTime >= 1.0)
	{ // If last prinf() was more than 1 sec ago
		// printf and reset timer
		fps = nbFrames;
		nbFrames = 0;
		lastTime += 1.0;
	}
}

int main(int argc, char **args)
{
	GLFWwindow *window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(screen_width, screen_height, "Textures test", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	init();
	while (!glfwWindowShouldClose(window))
	{
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
		sprintf(title, "fps: %d", fps);
		glfwSetWindowTitle(window, title);
	}

	return 0;
}