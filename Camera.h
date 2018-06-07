#pragma once

#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>

#include "Vector3D.h"
#include "Plane.h"

#include <list>

#include "RectTex.h"
#include "Ray.h"
#include "PhotonMap.h"
#include "Clamp.h"
#include "LightSource.h"
#include "Portal.h"

using namespace std;

class Camera
{
public:
	Vector3D center;
	Vector3D direction;
	float h;
	//for antialiasing:
	float h_2;
	int n_aa;
	float h_aa;
	float h_aa2;
	float n_aasq;
	float shift_aa;

	Camera() {}
	Camera(const Vector3D &center, const Vector3D &direction, float h, int n_aa) : center(center), direction(direction), h(h), n_aa(n_aa)
	{
		this->direction.normalize();
		h_2 = h * 0.5f;
		h_aa = h / n_aa;
		h_aa2 = h_aa * 0.5f;
		n_aasq = n_aa * n_aa;
		shift_aa = -h_2 + h_aa2;
	}

	void camIterate(const list<RectTex> &textures, const list<LightSource> &light, 
		const list<GaussBspline> &gb, const PhotonMap &pm, RectTex output, const list<Teleport> &tp)
	{
		Vector3D vert;
		Plane pla(direction, center);
		pla.closestPoint(Vector3D(0.0f, 0.0f, 1.0f) + center, vert);
		vert -= center;
		vert.normalize();
		Vector3D hor = direction ^ vert;
		hor.normalize();

		for (int i = 0; i < output.height; i++) {
			cout << i << ' ';
			for (int j = 0; j < output.width; j++) {
				Vector3D napr;
				napr = direction - ((i - output.height / 2) * h) * vert + ((j - output.width / 2) * h) * hor;
				Vector3D result_color(0.0f);
				Vector3D napr_aa;
				for (int i_aa = 0; i_aa < n_aa; i_aa++) {
					for (int j_aa = 0; j_aa < n_aa; j_aa++) {
						napr_aa = napr + (shift_aa + i_aa * h_aa) * vert + (shift_aa + j_aa * h_aa) * hor;
						Ray ray(center, napr_aa);
						Vector3D point;
						Vector3D result_color_aa = Vector3D(0.7843137255f);
						ray.trace(result_color_aa, pm, textures, light, gb, Ignore(index2(-1, -1), true), 1.0f, tp);
						result_color += result_color_aa;
					}
				}
				result_color /= n_aasq;
				output.setPixel(result_color, i, j);
			}
		}		
	}

	
};