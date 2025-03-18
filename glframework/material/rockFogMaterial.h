#pragma once

#include "material.h"
#include "../texture.h"

class RockFogMaterial :public Material {

public:
	RockFogMaterial();
	~RockFogMaterial();

public:
	Texture* mDiffuse{ nullptr };
	Texture* mHeightMap{ nullptr };
};