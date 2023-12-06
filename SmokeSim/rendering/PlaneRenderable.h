#pragma once

#include "IRenderable.h"
#include "../geometry/Pos3F.h"

class PlaneRenderable :
    public IRenderable
{
public:
	PlaneRenderable(const Pos3F& p1, const Pos3F& p2, const Pos3F& p3, const Pos3F& p4, const Material& mat);
	int NumIndices() const override;
	int NumPoints() const override;
	void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
	Material GetMaterial() const override { return material; }
protected:
	Material material;
	Pos3F p1, p2, p3, p4;
};

