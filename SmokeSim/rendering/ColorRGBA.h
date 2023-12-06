#pragma once

struct ColorRGBA {
	float r;
	float g;
	float b;
	float a;

	ColorRGBA(float red, float green, float blue, float alpha) {
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	void Add(const ColorRGBA& c) {
		r += c.r;
		g += c.g;
		b += c.b;
		if (r > 1)
			r = 1;
		if (g > 1)
			g = 1;
		if (b > 1)
			b = 1;
	}
};
