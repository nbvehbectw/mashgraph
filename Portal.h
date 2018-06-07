#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"

class Portal
{
public:
	Vector3D center;
	Vector3D normal, up, right;
	float radius, radSq;
	Plane pla;

	Portal(const Vector3D &c, const Vector3D &n, const Vector3D &u, float r) :center(c), normal(n), up(u), radius(r)
	{
		normal.normalize();
		up.normalize();
		right = (normal ^ up).normalize();
		radSq = radius * radius;
		pla = Plane(normal, center);
	}

	bool intersectByRay(const Vector3D &org, const Vector3D &dir, float &t, Vector3D &point) const
	{
		if (!pla.intersectByRay(org, dir, t)) {
			return false;
		}
		if (t < 0.0f) {
			return false;
		}
		point = org + t * dir;
		if ((point - center).lengthSq() < radSq) {
			return true;
		} else {
			return false;
		}
	}
};

class Teleport
{
public:
	Portal in, out;
	float radrat;

	Teleport(const Portal &in, const Portal &out): in(in), out(out) 
	{
		radrat = out.radius / in.radius;
	}

	bool teleport(const Vector3D &org_in, const Vector3D &dir_in, Vector3D &org_out, Vector3D &dir_out) const 
	{
		float t;
		Vector3D point;
		if (!in.intersectByRay(org_in, dir_in, t, point)) {
			return false;
		}
		dir_out = (dir_in & in.normal) * out.normal + (dir_in & in.up) * out.up + (dir_in & in.right) * out.right;
		point -= in.center;
		org_out = out.center + (point & in.up) * radrat * out.up + (point & in.right) * radrat * out.right;
		return true;
	}
};