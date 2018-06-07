#pragma once

#include <iostream>
#include <cmath>

#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>

#include "Vector3D.h"
#include "Plane.h"

#include "Clamp.h"
//#include "Ray.h"
//#include "PhotonMap.h"

//class Ray;

using namespace std;

class RectTex
{
public:
	Vector3D ul; //upper left
	Vector3D right;
	Vector3D down;
	Plane pla;

	ILuint id;
	ILubyte *bytes;
	int width;
	int height;
	int bpp;

	RectTex(const wchar_t *name)
	{
		ilGenImages(1, &id);
		ilBindImage(id);
		ilLoadImage(name);

		bytes = ilGetData();

		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	}

	RectTex(const wchar_t *name, const Vector3D &ul, const Vector3D &right, const Vector3D &down) :ul(ul), right(right), down(down)
	{
		pla = Plane(down ^ right, ul);

		ilGenImages(1, &id);
		ilBindImage(id);
		ilLoadImage(name);

		bytes = ilGetData();

		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	}

	Vector3D pixel(int i, int j) const //xyz = rgb
	{
		if (i < 0 || i >= height || j < 0 || j >= width) {
			cout << "error in ReturnColor function: i or j out of bounds" << endl;
			return Vector3D(-1.0f, -1.0f, -1.0f);
		} else {
			return Vector3D((float)bytes[(i * width + j) * bpp] / 255.0f,
				(float)bytes[(i * width + j) * bpp + 1] / 255.0f,
				(float)bytes[(i * width + j) * bpp + 2] / 255.0f);
		}
	}

	void setPixel(const Vector3D &color, int i, int j) //xyz = rgb
	{
		if (bytes == nullptr) {
			cout << "error in setPixel function: bytes == nullptr" << endl;
			return;
		}
		if (i < 0 || i >= height || j < 0 || j >= width) {
			cout << "error in setPixel function: i or j out of bounds" << endl;
			return;
		}
		Vector3D color_res(color);
		if (color.x < 0.0f || color.x > 1.0f || color.y < 0.0f || color.y > 1.0f || color.z < 0.0f || color.z > 1.0f) {
			//cout << "error in setPixel function: color out of bounds" << endl;
			color_res.x = clamp(color_res.x, 0.0f, 1.0f);
			color_res.y = clamp(color_res.y, 0.0f, 1.0f);
			color_res.z = clamp(color_res.z, 0.0f, 1.0f);
		}
		bytes[(i * width + j) * bpp] = roundf(color_res.x * 255.0f);
		bytes[(i * width + j) * bpp + 1] = roundf(color_res.y * 255.0f);
		bytes[(i * width + j) * bpp + 2] = roundf(color_res.z * 255.0f);
	}

	bool intersectByRayPixel(const Vector3D& org, const Vector3D& dir, float &t, Vector3D &color, Vector3D &point) const
	{
		if ((dir & pla.n) > 0.0f) {
			return false;
		}
		if (pla.intersectByRay(org, dir, t)) {
			point = org + t * dir;
			float offsetX = (right & (point - ul)) / right.lengthSq();
			float offsetZ = (down & (point - ul)) / down.lengthSq();
			if (offsetX >= 0.0f && offsetX <= 1.0f && offsetZ >= 0.0f && offsetZ <= 1.0f) {
				color = pixel(roundf(offsetZ * (height - 1)), roundf(offsetX * (width - 1)));
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool intersectByRay(const Vector3D& org, const Vector3D& dir, float &t, Vector3D &point) const
	{
		if ((dir & pla.n) > 0.0f) {
			return false;
		}
		if (pla.intersectByRay(org, dir, t)) {
			point = org + t * dir;
			float offsetX = (right & (point - ul)) / right.lengthSq();
			float offsetZ = (down & (point - ul)) / down.lengthSq();
			if (offsetX >= 0.0f && offsetX <= 1.0f && offsetZ >= 0.0f && offsetZ <= 1.0f) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	void writeOutput(const wchar_t *name)
	{
		ilBindImage(id);
		ilEnable(IL_FILE_OVERWRITE);
		ilSave(IL_PNG, name);
	}
};