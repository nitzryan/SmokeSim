#include "SphereRenderable.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "../rendering/BufferWriter.h"

SphereRenderable::SphereRenderable(Pos3F center, float radius, const Material& mat) : 
	material(mat)
{
	this->center = center;
	this->radius = radius;
	thetaSlices = 30;
	phiSlices = 30;
	GenerateRenderPoints();
}

float SphereRenderable::get_radius() const {
	return radius;
}

Pos3F SphereRenderable::get_center() const {
	return center;
}

void SphereRenderable::SetThetaSlices(int s)
{
	if (s > 0) {
		thetaSlices = s;
		GenerateRenderPoints();
	}
}

void SphereRenderable::SetPhiSlices(int s)
{
	if (s > 0) {
		phiSlices = s;
		GenerateRenderPoints();
	}
}

void SphereRenderable::GenerateRenderPoints()
{
	renderPoints.clear();
	normals.clear();
	int numPoints = (thetaSlices + 1) * (phiSlices + 1);
	renderPoints.reserve(numPoints);
	normals.reserve(numPoints);

	float thetaStep = 2 * (float)(M_PI) / thetaSlices;
	float phiStep = (float)(M_PI) / phiSlices;
	for (size_t p = 0; p <= phiSlices; p++) {
		float phi = (float)(-M_PI) / 2 + p * phiStep;
		for (size_t t = 0; t <= thetaSlices; t++) {
			float theta = t * thetaStep;
			float x = center.x + radius * cosf(theta) * cosf(phi);
			float y = center.y + radius * sinf(phi);
			float z = center.z + radius * sinf(theta) * cosf(phi);
			renderPoints.emplace_back(x, y, z);
			Vec3F normal = center.Subtract(renderPoints.back());
			normals.push_back(normal);
		}
	}
}

void SphereRenderable::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) 
{
	int tPlus1 = thetaSlices + 1;
	for (int i = 0; i < renderPoints.size(); i++) {
		int phiSlice = i / tPlus1;
		int thetaSlice = i % tPlus1;
		BufferWriter::AddPoint(vbo, vboLoc, renderPoints[i], material.color, normals[i], (float)(thetaSlice) / (thetaSlices),(float)(phiSlice) / phiSlices);
	}

	// Write to EBO
	for (int p = 0; p < phiSlices; p++) {
		for (int t = 0; t < thetaSlices; t++) {
			int tRight = (t + 1);// % (thetaSlices + 1); // Need to handle wrap around
			ebo[eboLoc] = p * tPlus1 + t + pointCount;
			eboLoc++;
			ebo[eboLoc] = (p + 1) * tPlus1 + t + pointCount;
			eboLoc++;
			ebo[eboLoc] = p * tPlus1 + tRight + pointCount;
			eboLoc++;
			ebo[eboLoc] = p * tPlus1 + tRight + pointCount;
			eboLoc++;
			ebo[eboLoc] = (p + 1) * tPlus1 + t + pointCount;
			eboLoc++;
			ebo[eboLoc] = (p + 1) * tPlus1 + tRight + pointCount;
			eboLoc++;
		}
	}
}
