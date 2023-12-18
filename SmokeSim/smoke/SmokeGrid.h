#pragma once

#include <vector>

#include "../rendering/Renderer.h"
#include "../rendering/IRenderable.h"
#include "SmokeSetup.h"

class SmokeGrid : public IRenderable
{
public:
	SmokeGrid(int approxTiles, const SmokeSetup& setup, float stepTime);
	void Update(float dt);
	//void Render(Renderer& renderer);

	void MouseDown(float x, float y);
	void MouseUp();
	void MouseMove(float x, float y);

	// Rendering Functions
	int NumIndices() const override final;
	virtual int NumPoints() const override final;
	virtual void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override final;
	virtual Material GetMaterial() const override final;

	static void CalculateToFile(int approxTiles, const SmokeSetup& setup, float time, int stepsPerRender, const char* filename);
private:
	int w, h;
	float gridSize;
	float time;
	float stepTime;
	Vec2F wind;
	std::vector<double> smoke;
	std::vector<double> velX, velY;
	SmokeSetup setup;
	
	struct sourceBlock {
		int tile;
		Vec2F vel;
		float intensity;
		float heat;
		float fuel;
	};

	enum class ObstacleDirection {
		UL,
		U,
		UR,
		L,
		R,
		DR,
		DL,
		D,
		INT
	};

	struct Obstacle {
		int tile;
		ObstacleDirection dir;
	};

	std::vector<sourceBlock> sources;
	std::vector<Obstacle> obstacles;
	std::vector<bool> tileIsObstacle;
	//std::vector<RectRenderable> rects;


	enum class SolverType {
		DENSITY,
		VEL_X,
		VEL_Y
	};
	void linearSolver(std::vector<double>& x, std::vector<double>& x0, float a, float c, int w, int h, SolverType st);
	void UpdateStep(float stepTime);
};

