#include "sphereCalculation.h"
#include "vector3Calculation.h"

bool SphereHitSphere(Sphere& sphere1, Sphere& sphere2) {
	float distance = Length(Subtract(sphere1.center, sphere2.center));
	if (distance <= sphere1.radius + sphere2.radius) {
		// 中心間が半径＋半径より短かったらTrue
		return true;
	}
	return false;
}