#pragma once
#include "../geometry/Pos3F.h"
#include "../geometry/Vec3F.h"

class Camera
{
public:
	Camera(float zoom, float aspect, const Pos3F& startPos = Pos3F(0,0,0), const Vec3F& startLook = Vec3F(0,0,1), const Vec3F& upDir = Vec3F(0,1,0));

	void SetPosition(const Pos3F& pos);
	Pos3F GetPos() const { return pos; }
	Pos3F GetScale() const { return Pos3F(zoom, zoom * aspect, 0); }
	void IncreaseZoom(float inc);
	void SetAspect(float w, float h) { aspect = w / h; }
	void StepZ(float d);
	void StepX(float d);
	void RotateX(float w, float dt);
	void RotateY(float w, float dt);
	//void RotateZ(float w, float dt);
	Vec3F GetLookDir() const { return lookDir; }
	Vec3F GetUpDir() const { return upDir; }
private:
	Pos3F pos;
	float zoom;
	float aspect;
	Vec3F lookDir;
	Vec3F upDir;
};

