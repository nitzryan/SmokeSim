#include "SmokeSetup.h"

SmokeSetup SmokeSetup::Campfire() {
	SmokeSetup s;
	s.circles = {};
	s.rects = {};
	s.wind = Vec2F(0.1f, 0.1f);
	smokeSource ss;
	ss.x0 = 1.9f;
	ss.x1 = 2.1f;
	ss.y0 = 0.0f;
	ss.y1 = 0.1f;
	ss.intensity = 0.0f;
	ss.heat = 10000.0f;
	ss.fuel = 1.0f;
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
	ss.fuel = 0.0f;
	ss.vel = Vec2F(0.0f, 4.0f);
	s.sources = { ss };
	s.width = 2.0f;
	s.height = 6.0f;
	return s;
}