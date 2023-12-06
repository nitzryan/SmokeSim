#include "CircleRenderable.h"
#include "../rendering/BufferWriter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CircleRenderable::CircleRenderable(Pos2F center, float radius, const Material& mat) :
	center(center),
	radius(radius),
	material(mat),
	thetaSlices(30)
{
	GenerateRenderPoints();
}

void CircleRenderable::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc)
{
	for (auto& i : renderPoints) {
		BufferWriter::AddPoint(vbo, vboLoc, i, material.color, Vec3F(0.0f, 0.0f, 1.0f), 0.0f, 0.0f);
	}

	for (int i = 0; i < thetaSlices; i++) {
		ebo[eboLoc] = pointCount;
		eboLoc++;
		ebo[eboLoc] = pointCount + i + 1;
		eboLoc++;
		ebo[eboLoc] = pointCount + i + 2;
		eboLoc++;
	}
}

void CircleRenderable::GenerateRenderPoints()
{
	renderPoints.clear();
	renderPoints.reserve(CircleRenderable::NumPoints());
	renderPoints.emplace_back(center.x, center.y);
	float thetaStep = 2 * (float)M_PI / thetaSlices;
	for (int i = 0; i <= thetaSlices; i++) {
		float theta = i * thetaStep;
		renderPoints.emplace_back(center.x + radius * cosf(theta), center.y + radius * sinf(theta));
	}
}
