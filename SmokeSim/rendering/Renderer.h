#pragma once

#include "../glad/glad.h"
#include "../geometry/Pos3F.h"
#include "../geometry/Pos2F.h"
#include "IRenderable.h"
#include "Camera.h"
#include "RenderingData.h"
#include <vector>

class Renderer
{
public:
	Renderer();
	~Renderer();
	void StartFrame();

	void SetCamera(const Camera& camera);
	void Render(IRenderable& obj);
	void SetLightDirection(const Vec3F& v) { lightDir = v; lightDir.Normalize(); }
	void FinalizeFrame();

	static void AddPoint(std::vector<float>& vbo, unsigned int& vboLoc,
		const Pos3F& p, const ColorRGBA& c, const Vec3F& n, float texX, float texY);

	static void AddPoint(std::vector<float>& vbo, unsigned int& vboLoc,
		const Pos2F& p, const ColorRGBA& c, const Vec3F& n, float texX, float texY);
private:
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	GLuint shaderProgram;
	const int vertexSize = 12;
	const int vboSize = 100000 * vertexSize;
	const int indicesSize = 100000;
	std::vector<float> vboContents;
	std::vector<unsigned int> indices;
	unsigned int currentVboLoc;
	unsigned int currentIndicesLoc;

	Pos3F cameraPos, cameraScale;
	Vec3F cameraDir, upDir;
	Vec3F lightDir;
	GLint posAttrib, colAttrib, normAttrib, texAttrib;
	GLuint vertexShader, fragmentShader;

	std::vector<RenderingData> renderingQueue;
	std::vector<unsigned int> textures;
};

