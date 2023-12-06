#pragma once
#include <vector>
#include "../geometry/Pos3F.h"
#include "../geometry/Vec3F.h"
#include "../rendering/ColorRGBA.h"
#include "../rendering/IRenderable.h"

class SphereRenderable :
	public IRenderable{
public:
	SphereRenderable(Pos3F center, float radius, const Material& mat);
	int NumIndices() const override { return 6 * (thetaSlices + 1) * phiSlices; };
	int NumPoints() const override { return (thetaSlices + 1) * (phiSlices + 1); }; // need 0 and 2 * pi for theta, since different text coords
	void Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc) override;
	Material GetMaterial() const override { return material; }
	//void Update(float dt); // porbbaly not needed as it will not move
	float get_radius() const;
	Pos3F get_center() const;
	void SetThetaSlices(int s);
	void SetPhiSlices(int s);
protected:
	Pos3F center;
	float radius;
	Material material;
	int thetaSlices;
	int phiSlices;
	
	std::vector<Pos3F> renderPoints;
	std::vector<Vec3F> normals;
	void GenerateRenderPoints();
};