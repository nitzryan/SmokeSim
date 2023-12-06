#pragma once

#include <vector>

#include "../rendering/Renderer.h"
#include "../rendering/RectRenderable.h"

class SmokeGrid
{
public:
	SmokeGrid();
	void Update(float dt);
	void Render(Renderer& renderer);

	void MouseDown(float x, float y);
	void MouseUp();
	void MouseMove(float x, float y);
private:
	int w, h;
	float gridSize;
	float time;
	Vec2F wind;
	std::vector<float> smoke;
	std::vector<float> temp;
	std::vector<float> velX, velY;
	std::vector<RectRenderable> rects;
};

