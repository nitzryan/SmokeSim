#include "SmokeGridReplay.h"
#include "../geometry/Vec3F.h"
#include "../rendering/BufferWriter.h"

#include <iostream>
#include <fstream>

SmokeGridReplay::SmokeGridReplay(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "Unable to open file " << filename << std::endl;
		return;
	}

	char dummy;
	file >> w >> dummy >> h;
	file >> gridSize;
	file >> stepTime;
	for (int i = 0; i < w * h; i++) {
		bool b;
		file >> b >> dummy;
		obstacles.emplace_back(b);
	}
	for (int n = 0; n < 1000; n++) {
		for (int i = 0; i < w * h; i++) {
			double s;
			
			file >> s >> dummy;
			smoke.push_back(s);
		}
	}

	currentArrayOffset = 0;
	time = 0;

}

void SmokeGridReplay::Update(float dt)
{
	time += dt;
	currentArrayOffset = (int)(time / 0.01f) * w * h;
	if (currentArrayOffset >= smoke.size()) {
		currentArrayOffset = smoke.size() - w * h;
	}
}

int SmokeGridReplay::NumIndices() const
{
	return 6 * (w - 1) * (h - 1);
}

int SmokeGridReplay::NumPoints() const
{
	return w * h;
}

void SmokeGridReplay::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc)
{
	Vec3F normal = Vec3F(0, 0, 1);

	int n = 0;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			if (obstacles[n]) {
				BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(0.9f, 0.2f, 0.2f, 1.0f), normal, -1, -1);
			}
			else {
				float modSmoke = smoke[currentArrayOffset + n] / ((double)gridSize * gridSize);
				float alpha = modSmoke * (1 + modSmoke);
				BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(0.9f, 0.9f, 0.9f, alpha), normal, -1, -1);
			}

			n++;
		}
	}

	for (int i = 0; i < w - 1; i++) {
		for (int j = 0; j < h - 1; j++) {
			// Triangle Right, Right Up
			ebo[eboLoc] = pointCount + i * h + j;
			eboLoc++;
			ebo[eboLoc] = pointCount + (i + 1) * h + j;
			eboLoc++;
			ebo[eboLoc] = pointCount + (i + 1) * h + j + 1;
			eboLoc++;
			// Triangle Right Up, Up
			ebo[eboLoc] = pointCount + i * h + j;
			eboLoc++;
			ebo[eboLoc] = pointCount + (i + 1) * h + j + 1;
			eboLoc++;
			ebo[eboLoc] = pointCount + i * h + j + 1;
			eboLoc++;
		}
	}
}

Material SmokeGridReplay::GetMaterial() const
{
	return Material();
}
