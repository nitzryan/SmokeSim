#pragma once
#include "IRenderable.h"
#include <vector>
#include "../geometry/Pos2F.h"
#include "../geometry/Vec2F.h"
#include "../rendering/ColorRGBA.h"

class CircleRenderable :
    public IRenderable
{
public:
	CircleRenderable(Pos2F center, float radius, const Material& mat);
	int NumIndices() const override { return 3 * thetaSlices; }
	int NumPoints() const override { return (thetaSlices + 2); } // need 0 and 2 * pi for theta, since different text coords
	void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
	Material GetMaterial() const override { return material; }
	//void Update(float dt); // porbbaly not needed as it will not move
	float GetRadius() const { return radius; }
	Pos2F GetCenter() const { return center; }
	void SetCenter(const Pos2F& p) { center = p; GenerateRenderPoints(); }
	void SetThetaSlices(int s) { thetaSlices = s; GenerateRenderPoints();}
protected:
	Pos2F center;
	float radius;
	Material material;
	int thetaSlices;

	std::vector<Pos2F> renderPoints;
	void GenerateRenderPoints();
};

