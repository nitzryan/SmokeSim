#pragma once

#include <vector>
#include "Material.h"

class IRenderable {
public:
	virtual int NumIndices() const = 0;
	virtual int NumPoints() const = 0;
	virtual void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) = 0;
	virtual Material GetMaterial() const = 0;
};