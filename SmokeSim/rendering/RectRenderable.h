#pragma once
#include "IRenderable.h"
#include "../geometry/Pos2F.h"

class RectRenderable :
    public IRenderable
{
public:
	RectRenderable(Pos2F ul, Pos2F ur, Pos2F ll, Pos2F lr, const Material& mat);
	int NumIndices() const override;
	int NumPoints() const override;
	void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
	bool PointInRect(const Pos2F& p) const;
	Material GetMaterial() const override { return material; }
	void SetTransparency(float a) { material.color.a = a; }
	Pos2F GetUR() const { return ur; }
	Pos2F GetLL() const { return ll; }
protected:
	Material material;
	Pos2F ul, ur, ll, lr;
};

