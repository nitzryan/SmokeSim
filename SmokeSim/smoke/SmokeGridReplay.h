#pragma once

#include <vector>
#include "../rendering/IRenderable.h"

class SmokeGridReplay : public IRenderable
{
public:
	SmokeGridReplay(const char* filename);
	void Update(float dt);

	// Rendering Functions
	int NumIndices() const override final;
	virtual int NumPoints() const override final;
	virtual void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override final;
	virtual Material GetMaterial() const override final;
private:
	float time, stepTime;
	int w, h;
	long currentArrayOffset;
	float gridSize;
	std::vector<double> smoke;
	std::vector<bool> obstacles;
};

