#pragma once

#include "IScene.h"
#include "../ik/IKRobot.h"

#include <random>

class RobotScene1 : public IScene {
public:
	RobotScene1() :
		robot({
			IKArm(matShared, 0.4f, 0.1f, 0.6f, -1.57f, 1.57f, -1),
			IKArm(matShared, 0.4f, 0.1f, 0.0f, -1.57f, 1.57f, -1),
			IKArm(mat0, 0.4f, 0.1f, 0.4f, 0.1f, 1.57f, 0),
			IKArm(mat1, 0.4f, 0.1f, -0.4f, -1.57f, -0.1f, 1),
			IKArm(mat0, 0.5f, 0.1f, 0.0f, -1.57f, 1.57f, 0),
			IKArm(mat1, 0.2f, 0.1f, 0.0f, -1.57f, 1.57f, 1),
			IKArm(mat1, 0.3f, 0.1f, 0.0f, -1.57f, 1.57f, 1)
			}, { {1}, {2,3}, {4}, {5}, {}, {6}, {} }),
		circle0(Pos2F(0.2f, -0.8f), 0.05f, Material(ColorRGBA(0.8f, 0.2f, 0.2f, 1), 1, 0, 0, 10, -1)),
		circle1(Pos2F(-0.2f, -0.8f), 0.05f, Material(ColorRGBA(0.1f, 0.2f, 0.8f, 1), 1, 0, 0, 10, -1))
	{
		mt = std::mt19937(5611);
	}
	void Update(float dt) override {
		if (robot.IsAtPosition({ circle0.GetCenter(), circle1.GetCenter() }, 0.01f)) {
			RandomizeCirclePositions();
		}

		robot.MoveTowards({ circle0.GetCenter(), circle1.GetCenter() }, dt);
	}
	void Render(Renderer& renderer) override {
		robot.RenderRobot(renderer);
		renderer.Render(circle0);
		renderer.Render(circle1);
	}

	void RandomizeCirclePositions() {
		std::uniform_real_distribution<float> r(-1.0f, 1.0f);

		float mag;
		do {
			circle0.SetCenter(Pos2F(r(mt), r(mt)));
			circle1.SetCenter(Pos2F(r(mt), r(mt)));
			mag = (circle0.GetCenter() - circle1.GetCenter()).GetMagnitude();
		} while (mag > robot.GetMaxReach());
	}

	void MouseDown(float x, float y) override {
		ScreenCoordsToSimCoords(x, y);
		if ((circle0.GetCenter() - Pos2F(x, y)).GetMagnitude() < circle0.GetRadius()) {
			circleMouseGrabbed = &circle0;
			mousePressed = true;
			mouseX = x;
			mouseY = y;
		} else if ((circle1.GetCenter() - Pos2F(x, y)).GetMagnitude() < circle1.GetRadius()) {
			circleMouseGrabbed = &circle1;
			mousePressed = true;
			mouseX = x;
			mouseY = y;
		}
		
	}

	void MouseUp() override {
		mousePressed = false;
		circleMouseGrabbed = nullptr;
	}

	void MouseMove(float x, float y) override {
		if (mousePressed) {
			ScreenCoordsToSimCoords(x, y);
			float deltaX = x - mouseX;
			float deltaY = y - mouseY;

			circleMouseGrabbed->SetCenter(circleMouseGrabbed->GetCenter() + Vec2F(deltaX, deltaY));
			mouseX = x;
			mouseY = y;
		}
	}

private:
	IKRobot robot;
	CircleRenderable circle0, circle1;
	bool mousePressed = false;
	float mouseX, mouseY;
	CircleRenderable* circleMouseGrabbed = nullptr;

	std::mt19937 mt;

	void ScreenCoordsToSimCoords(float& x, float& y) {
		// Convert from 0..1 to -1..-1
		x *= 2;
		x -= 1;
		y *= -2;
		y += 1;

		// Convert to size of scene
		x *= 1.25;
		y *= 1.25;
	}
};