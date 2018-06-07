#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"

#include <list>

bool refraction(Vector3D &res, const Vector3D &dir, const Vector3D& normal, float n)
{
	float cos_a = dir & normal;
	float sin_a = sqrtf(1.0f - cos_a * cos_a);
	if (cos_a > 0.0f) { //from the inside
		float sin_b = sin_a * n;
		if (sin_b > 1.0f) { //total internal reflection
			return false;
		} else {
			float cos_b = sqrtf(1.0f - sin_b * sin_b);
			float tg_b = sin_b / cos_b;
			Vector3D OA = cos_a * normal;
			res = (OA + tg_b * OA.length() * (dir - OA).normalize()).normalize();
			return true;
		}
	} else { //from the outside
		float sin_b = sin_a / n;
		float cos_b = sqrtf(1.0f - sin_b * sin_b);
		float tg_b = sin_b / cos_b;
		Vector3D OA = cos_a * normal;
		res = (OA + tg_b * OA.length() * (dir - OA).normalize()).normalize();
		return true;
	}
}