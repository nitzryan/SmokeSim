#include "SmokeSetup.h"

SmokeSetup SmokeSetup::Campfire() {
	SmokeSetup s;
	s.circles = {};
	s.rects = {};
	s.wind = Vec2F(0.0f, 0.0f);
	smokeSource ss1;
	ss1.x0 = 1.9f;
	ss1.x1 = 2.1f;
	ss1.y0 = 0.05f;
	ss1.y1 = 0.25f;
	ss1.intensity = 5.0f;
	ss1.heat = 500.0f;
	ss1.vel = Vec2F(0.0f, 0.0f);

	smokeSource ss2;
	ss2.x0 = 2.1f;
	ss2.x1 = 2.2f;
	ss2.y0 = 0.05f;
	ss2.y1 = 0.25f;
	ss2.intensity = 2.0f;
	ss2.heat = 200.0f;
	ss2.vel = Vec2F(0.0f, 0.0f);

	smokeSource ss3;
	ss3.x0 = 1.8f;
	ss3.x1 = 1.9f;
	ss3.y0 = 0.05f;
	ss3.y1 = 0.25f;
	ss3.intensity = 2.0f;
	ss3.heat = 200.0f;
	ss3.vel = Vec2F(0.0f, 0.0f);

	smokeSource ss4;
	ss4.x0 = 1.8f;
	ss4.x1 = 2.2f;
	ss4.y0 = 0.25f;
	ss4.y1 = 0.35f;
	ss4.intensity = 2.0f;
	ss4.heat = 200.0f;
	ss4.vel = Vec2F(0.0f, 0.0f);

	s.sources = { ss1, ss2, ss3, ss4 };
	s.width = 4.f;
	s.height = 4.f;
	return s;
}

SmokeSetup SmokeSetup::Campfire2() {
	SmokeSetup s;
	s.circles = {};
	s.rects = {};
	s.wind = Vec2F(0.0f, 0.0f);
	smokeSource ss;
	ss.x0 = 1.0f;
	ss.x1 = 3.0f;
	ss.y0 = 0.00f;
	ss.y1 = 0.25f;
	ss.intensity = 5.0f;
	ss.heat = 1700.0f;
	ss.vel = Vec2F(0.0f, 0.0f);

	s.sources = { ss };
	s.width = 4.f;
	s.height = 4.f;
	return s;
}

SmokeSetup SmokeSetup::Fireplace() {
	SmokeSetup s;
	s.circles = {};
	ColorRGBA wall(0.2f, 0.8f, 0.8f, 1.0f);
	s.rects = {
		rectObj{0.1f, 0.8f, 0.0f, 3.0f, wall},
		rectObj{0.8f, 1.9f, 1.25f, 3.0f, wall},
		rectObj{2.1f, 3.2f, 1.25f, 3.0f, wall},
		rectObj{3.2f, 3.9f, 0.0f, 3.0f, wall},
		rectObj{0.1f, 3.9f, 0.0f, 0.1f, wall}
	};
	s.wind = Vec2F(0.0f, 0.0f);
	smokeSource ss;
	ss.x0 = 1.5f;
	ss.x1 = 2.5f;
	ss.y0 = 0.1f;
	ss.y1 = 0.35f;
	ss.intensity = 5.0f;
	ss.heat = 1700.0f;
	ss.vel = Vec2F(0.0f, 0.0f);

	s.sources = { ss };
	s.width = 4.f;
	s.height = 4.f;
	return s;
}

SmokeSetup SmokeSetup::BallDrag() {
	SmokeSetup s;
	s.circles = { circleObj{1.0f, 1.0f, 0.2f, ColorRGBA(0.1f, 0.6f, 0.1f, 1.0f)} };
	s.rects = {};
	s.wind = Vec2F(0, 0);
	smokeSource ss;
	ss.x0 = 0.7f;
	ss.x1 = 1.3f;
	ss.y0 = 0.0f;
	ss.y1 = 0.1f;
	ss.intensity = 10.0f;
	ss.heat = 0.0f;
	ss.vel = Vec2F(0.0f, 4.0f);
	s.sources = { ss };
	s.width = 2.0f;
	s.height = 6.0f;
	return s;
}