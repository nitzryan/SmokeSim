#pragma once

#include <vector>
#include "../rendering/ColorRGBA.h"
#include "../geometry/Vec2F.h"

struct circleObj {
	float x0;
	float y0;
	float r;
	ColorRGBA color;
};

struct rectObj {
	float x0;
	float x1;
	float y0;
	float y1;
	ColorRGBA color;
};

struct smokeSource {
	float x0;
	float x1;
	float y0;
	float y1;
	float intensity;
	float heat;
	float fuel;
	Vec2F vel;
};

struct SmokeSetup
{
	static SmokeSetup Campfire();
	static SmokeSetup BallDrag();

	std::vector<circleObj> circles;
	std::vector<rectObj> rects;
	Vec2F wind;
	std::vector<smokeSource> sources;
	float width;
	float height;

	SmokeSetup operator= (const SmokeSetup& ss) {
		circles = ss.circles;
		rects = ss.rects;
		wind = ss.wind;
		sources = ss.sources;
		width = ss.width;
		height = ss.height;
		return *this;
	}
};

