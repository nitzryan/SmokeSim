#include "Renderer.h"
#include <iostream>

// Matrix/Vector Math
#define GLM_FORCE_RADIANS
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

// Loading images into textures
#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include "../textures/stb_image.h"
#pragma warning(pop)

void loadShader(GLuint shaderID, const GLchar* shaderSource) {
	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);
	//Let’s double check the shader compiled
	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status); //Check for errors
	if (!status) {
		char buffer[512]; glGetShaderInfoLog(shaderID, 512, NULL, buffer);
		std::cout << "Shader Compile Failed. Info:\n\n" << buffer << "\n";
	}
}


const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
"in vec4 inColor;"
"in vec3 normal;"
"in vec2 texCoord;"
"out vec4 Color;"
"uniform mat4 view;"
"uniform mat4 proj;"
"uniform vec3 CameraPos;"
"out vec3 normalOut;"
"out vec3 pos;"
"out vec3 eyePos;"
"out vec2 tCoord;"
"void main() {"
" Color = inColor;"
" gl_Position = view * vec4(position, 1.0);"
" pos = position;"
" vec4 norm4 = transpose(inverse(view)) * vec4(normal, 0.0);"
" normalOut = normal;"
" gl_Position = proj * gl_Position;"
" eyePos = CameraPos;"
" tCoord = texCoord;"
"}";

const GLchar* fragmentSource =
"#version 150 core\n"
"in vec4 Color;"
"in vec3 normalOut;"
"in vec3 pos;"
"in vec3 eyePos;"
"in vec2 tCoord;"
"out vec4 outColor;"
"const float ka = 1.0;"
"const float kd = 0.0;"
"const float ks = 0.0;"
"uniform sampler2D myTexture;"
"uniform int useTexture;"
"uniform vec3 lightDir;"
"void main() {"
" vec4 FragColor;"
" if (useTexture == 1) FragColor = texture(myTexture, tCoord);"
" else FragColor = Color;"
" vec3 N = normalize(normalOut);" //Re-normalized the interpolated normals
" vec3 diffuseC = kd * FragColor.xyz*max(dot(lightDir,N),0.0);"
" vec3 ambC = FragColor.xyz*ka;"
" vec3 reflectDir = reflect(lightDir,N);"
" reflectDir = normalize(reflectDir);"
" vec3 viewDir = normalize(pos - eyePos);"
" float spec = max(dot(reflectDir, viewDir),0.0);"
" if (dot(lightDir,N) <= 0.0) spec = 0;"
" vec3 specC = vec3(ks,ks,ks)*pow(spec,20);"
" outColor = vec4(ambC+diffuseC+specC, FragColor.a);"
"}";


Renderer::Renderer() : cameraPos(0, 0, 0), cameraScale(1,1,1), cameraDir(0,0,-1), upDir(0,1,0)
{
	// Create shader, temporary, should move to seperate class
	// Load and COmpile Shaders
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	loadShader(vertexShader, vertexSource);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	loadShader(fragmentShader, fragmentSource);
	//Join the vertex and fragment shaders together into one program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
	glLinkProgram(shaderProgram); //run the linker

	// Create VBO
	vboContents = std::vector<float>(vboSize);
	for (int i = 0; i < vboSize; i++)
		vboContents[i] = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, vboSize * sizeof(float), &vboContents[0], GL_STATIC_DRAW);

	// Create VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create EBO
	indices = std::vector<unsigned int>(indicesSize);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), 0);
	//(above params: Attribute, vals/attrib., type, isNormalized, stride, offset)
	glEnableVertexAttribArray(posAttrib); //Mark the attribute’s location as valid
	colAttrib = glGetAttribLocation(shaderProgram, "inColor");
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE,
		vertexSize * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(colAttrib);

	normAttrib = glGetAttribLocation(shaderProgram, "normal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE,
		vertexSize * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(normAttrib);

	texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
		vertexSize * sizeof(float), (void*)(10 * sizeof(float)));
	glEnableVertexAttribArray(texAttrib);

	glBindVertexArray(0); //Unbind the VAO so we don’t accidentally modify it

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Load Textures using stb_image.h
	std::vector<std::string> names = {"textures//earth.jpg", "textures//usFlag.jpg"};
	for (auto& i : names) {
		int width, height, channels;
		unsigned char* imageData = stbi_load(i.c_str(), &width, &height, &channels, 0);

		unsigned int tex;
		glGenTextures(1, &tex);
		textures.push_back(tex);

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		stbi_image_free(imageData);
	}
}


Renderer::~Renderer()
{
	
}

void Renderer::StartFrame()
{
	currentVboLoc = 0;
	currentIndicesLoc = 0;
	renderingQueue.clear();
}

void Renderer::Render(IRenderable& obj)
{
	int numVertices = obj.NumPoints() * vertexSize;
	int numIndices = obj.NumIndices();
	Material mat = obj.GetMaterial();

	if (numVertices + (int)(currentVboLoc) > vboSize) {
		std::cout << "VBO Size Exceeded\n";
		return;
	}

	if (numIndices + (int)currentIndicesLoc > indicesSize) {
		std::cout << "EBO Size Exceeded\n";
		return;
	}

	
	obj.Render(vboContents, currentVboLoc, currentVboLoc / vertexSize, indices, currentIndicesLoc);
	renderingQueue.push_back(RenderingData{ mat, (unsigned int)numIndices, (unsigned int)currentIndicesLoc });
	currentVboLoc += numVertices;
	currentIndicesLoc += numIndices;
}

void Renderer::SetCamera(const Camera& camera)
{
	cameraPos = camera.GetPos();
	cameraScale = camera.GetScale();
	cameraDir = camera.GetLookDir();
	upDir = camera.GetUpDir();
}

void Renderer::FinalizeFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);

	glm::mat4 view = glm::lookAt(
		glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), 
		glm::vec3(cameraPos.x + cameraDir.x, cameraPos.y + cameraDir.y, cameraPos.z + cameraDir.z), 
		glm::vec3(upDir.x, upDir.y, upDir.z)
	);
	GLint uView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(3.14f / 4, 1.0f, 0.1f, 100.0f);
	GLint uProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));

	GLint uCameraPos = glGetUniformLocation(shaderProgram, "CameraPos");
	glUniform3f(uCameraPos, cameraPos.x, cameraPos.y, cameraPos.z);

	GLint uLightDir = glGetUniformLocation(shaderProgram, "lightDir");
	glUniform3f(uLightDir, lightDir.x, lightDir.y, lightDir.z);

	GLint useTextures = glGetUniformLocation(shaderProgram, "useTexture");
	GLint textureLoc = glGetUniformLocation(shaderProgram, "myTexture");

	glBufferData(GL_ARRAY_BUFFER, currentVboLoc * sizeof(float), &vboContents[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentIndicesLoc * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(vao);

	
	for (auto& i : renderingQueue) {
		if (i.material.textureID < 0) { // No Texture
			glUniform1i(useTextures, 0);
		}
		else {
			glUniform1i(useTextures, 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[i.material.textureID]);
			glUniform1i(textureLoc, 0);

		}
		glDrawElements(GL_TRIANGLES, i.count, GL_UNSIGNED_INT, (void*)(i.offset * sizeof(unsigned int)));
	}
	//glDrawElements(GL_TRIANGLES, currentIndicesLoc, GL_UNSIGNED_INT, 0);
}

void Renderer::AddPoint(std::vector<float>& vbo, unsigned int& vboLoc, const Pos3F& p, const ColorRGBA& c, const Vec3F& n, float texX, float texY)
{
	vbo[vboLoc] = p.x;
	vbo[vboLoc + 1] = p.y;
	vbo[vboLoc + 2] = p.z;
	vbo[vboLoc + 3] = c.r;
	vbo[vboLoc + 4] = c.g;
	vbo[vboLoc + 5] = c.b;
	vbo[vboLoc + 6] = c.a;
	vbo[vboLoc + 7] = n.x;
	vbo[vboLoc + 8] = n.y;
	vbo[vboLoc + 9] = n.z;
	vbo[vboLoc + 10] = texX;
	vbo[vboLoc + 11] = texY;
	vboLoc += 12;
}

void Renderer::AddPoint(std::vector<float>& vbo, unsigned int& vboLoc, const Pos2F& p, const ColorRGBA& c, const Vec3F& n, float texX, float texY)
{
	AddPoint(vbo, vboLoc, Pos3F(p.x, p.y, 0), c, n, texX, texY);
}
