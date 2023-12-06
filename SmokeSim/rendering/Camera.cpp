#include "Camera.h"

Camera::Camera(float zoom, float aspect, const Pos3F& startPos, const Vec3F& startLook, const Vec3F& upDir) :
	pos(startPos), zoom(zoom), aspect(aspect), lookDir(startLook), upDir(upDir)
{
	this->upDir.Sub(Vec3F::Proj(upDir, startLook)); // Ensures upDir is orthogonal to lookDir
	lookDir.Normalize();
	this->upDir.Normalize();
}

void Camera::SetPosition(const Pos3F& p)
{
	pos = p;
}

void Camera::IncreaseZoom(float inc)
{
	zoom += inc;
	if (zoom < 0.01f)
		zoom = 0.01f;
	else if (zoom > 10.f)
		zoom = 10.f;
}

void Camera::StepZ(float d)
{
	pos.Add(Vec3F::Mul(lookDir, d));
}

void Camera::StepX(float d)
{
	pos.Add(Vec3F::Mul(Vec3F::Cross(lookDir, upDir), d));
}

void Camera::RotateX(float w, float dt)
{
	// Rotation Matrix 
	float theta = w * dt;
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);

	// Modify lookDir
	float tmp = lookDir.y;
	lookDir.y = lookDir.y * cosTheta + lookDir.z * sinTheta;
	lookDir.z = -tmp * sinTheta + lookDir.z * cosTheta;

	// Modify upDir
	tmp = upDir.y;
	upDir.y = upDir.y * cosTheta + upDir.z * sinTheta;
	upDir.z = -tmp * sinTheta + upDir.z * cosTheta;

	lookDir.Normalize();
	upDir.Normalize();
}

void Camera::RotateY(float w, float dt)
{
	// Rotation Matrix 
	float theta = w * dt;
	float cosTheta = cosf(theta);
	float sinTheta = sinf(theta);

	// Modify lookDir
	float tmp = lookDir.x;
	lookDir.x = lookDir.x * cosTheta - lookDir.z * sinTheta;
	lookDir.z = tmp * sinTheta + lookDir.z * cosTheta;
	
	// Modify upDir
	tmp = upDir.x;
	upDir.x = upDir.x * cosTheta - upDir.z * sinTheta;
	upDir.z = tmp * sinTheta + upDir.z * cosTheta;

	lookDir.Normalize();
	upDir.Normalize();
}