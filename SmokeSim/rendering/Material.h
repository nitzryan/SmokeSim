#pragma once

#include "ColorRGBA.h"

struct Material {
	ColorRGBA color;
	float ka, kd, ks, specPower;
	int textureID;

	Material() : color(0,0,0,1){
		ka = 0;
		kd = 0;
		ks = 0;
		specPower = 10;
		textureID = -1;
	}

	Material(const ColorRGBA& color, float ka, float kd, float ks, float specPower, int textureId) :
	color(color), ka(ka), kd(kd), ks(ks), specPower(specPower), textureID(textureId)
	{
		
	}
};