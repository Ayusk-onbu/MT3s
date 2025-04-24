#pragma once
#include "structures.h"

float cot(float theta) {
	float ret = 1.0f / std::tanf(theta);
	return ret;
}