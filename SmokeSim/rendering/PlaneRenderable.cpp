#include "PlaneRenderable.h"
#include "BufferWriter.h"

PlaneRenderable::PlaneRenderable(const Pos3F& p1, const Pos3F& p2, const Pos3F& p3, const Pos3F& p4, const Material& mat) :
	p1(p1), p2(p2), p3(p3), p4(p4), material(mat)
{
	
}

int PlaneRenderable::NumIndices() const
{
	return 6;
}

int PlaneRenderable::NumPoints() const
{
	return 4;
}

void PlaneRenderable::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) 
{
	Vec3F normal = Vec3F::Cross(p1 - p3, p1 - p2);
	normal.Normalize();
	BufferWriter::AddPoint(vbo, vboLoc, p1, material.color, normal, 0, 0);
	BufferWriter::AddPoint(vbo, vboLoc, p2, material.color, normal, 1, 0);
	BufferWriter::AddPoint(vbo, vboLoc, p3, material.color, normal, 0, 1);
	BufferWriter::AddPoint(vbo, vboLoc, p4, material.color, normal, 1, 1);

	ebo[eboLoc] = pointCount;
	ebo[eboLoc + 1] = pointCount + 1;
	ebo[eboLoc + 2] = pointCount + 2;
	ebo[eboLoc + 3] = pointCount + 1;
	ebo[eboLoc + 4] = pointCount + 3;
	ebo[eboLoc + 5] = pointCount + 2;
}
