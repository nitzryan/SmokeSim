#pragma once

#include "Material.h"

struct RenderingData {
	Material material;
	unsigned int count;
	unsigned int offset;
};