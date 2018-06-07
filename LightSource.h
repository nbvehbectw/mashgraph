#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"
#include "Math3D.h"

#include "Ray.h"

using namespace std;

//class Ray;

class LightSource
{
public:
	Vector3D center;
	float intens;
	float radius;

	LightSource(const Vector3D &center, float intens, float radius): center(center), intens(intens), radius(radius) {}

	bool intersectByRay(const Vector3D& org, const Vector3D& dir, float &t) const
	{
		if ((org - center).lengthSq() < radius * radius) {
			return true;
			cout << "inside LightSource::intersectByRay" << endl;
		}
		t = intersectSphereByRay(center, radius, org, dir);
		if (t > 0.0f) {
			//cout << "inside LightSource::intersectByRay" << endl;
			return true;
		} else {
			return false;
		}
	}
};