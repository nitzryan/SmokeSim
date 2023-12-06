#pragma once

#include "../geometry/Pos3F.h"
#include "../geometry/Pos2F.h"
#include "ColorRGBA.h"
#include <vector>

class BufferWriter {
public:
	BufferWriter() = delete;

	static void AddPoint(std::vector<float>& vbo, unsigned int& vboLoc,
		const Pos3F& p, const ColorRGBA& c, const Vec3F& n, float texX, float texY) {
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

	static void AddPoint(std::vector<float>& vbo, unsigned int& vboLoc,
		const Pos2F& p, const ColorRGBA& c, const Vec3F& n, float texX, float texY) {
		AddPoint(vbo, vboLoc, Pos3F(p.x, p.y, 0), c, n, texX, texY);
	}
};