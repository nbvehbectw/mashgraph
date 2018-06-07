#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"
#include "Math3D.h"

#include "Ray.h"

class Photon
{
public:
	Vector3D loc;
	float intens;

	Photon(const Vector3D &loc, float intens): loc(loc), intens(intens) {}
};