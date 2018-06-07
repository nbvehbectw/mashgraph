#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"

#include <list>

#include "RectTex.h"
#include "Ray.h"
#include "PhotonMap.h"
#include "Photon.h"
#include "LightSource.h"
#include "Icosahedron.h"
#include "Index.h"
#include "Clamp.h"
#include "Bspline.h"
#include "Portal.h"

void create(PhotonMap &pm, const list<LightSource> &light, const list<RectTex> &textures, 
	const list<GaussBspline> &gb, const list<Teleport> &tp)
{
	int totalRays = 12 + (20 + 10 * pm.N) * pm.N; //12 + 30 * N + 20 * N * (N - 1) / 2

	Icosahedron ico;
	Vector3D point, color;
	for (list<LightSource>::const_iterator l = light.begin(); l != light.end(); ++l) {
		float eachIntens = l->intens / totalRays;
		for (int i = 0; i < 12; i++) {				//iterate through vertices 
			Ray ray(l->center, ico.vertex[i]);// , eachIntens);
			ray.tracePm(eachIntens, eachIntens, pm, textures, gb, Ignore(index2(-1, -1), true), tp);
		}

		for (int i = 0; i < 30; i++) {				//iterate through edges
			index2 ind = ico.verticesByEdge(i);
			Vector3D step = (ico.vertex[ind.v2] - ico.vertex[ind.v1]) / (pm.N + 1);
			Vector3D dir = ico.vertex[ind.v1];
			for (int j = 0; j < pm.N; j++) {
				dir += step;
				Ray ray(l->center, dir);//, eachIntens);
				ray.tracePm(eachIntens, eachIntens, pm, textures, gb, Ignore(index2(-1, -1), true), tp);
			}
		}

		for (int i = 0; i < 20; i++) {				//iterate through faces
			index3 ind = ico.verticesByFace(i);
			Vector3D step1 = (ico.vertex[ind.v2] - ico.vertex[ind.v1]) / (pm.N + 1);
			Vector3D step2 = (ico.vertex[ind.v3] - ico.vertex[ind.v1]) / (pm.N + 1);
			Vector3D step3 = (ico.vertex[ind.v3] - ico.vertex[ind.v2]) / (pm.N + 1);
			Vector3D intm1 = ico.vertex[ind.v1] + step1;
			Vector3D intm2 = ico.vertex[ind.v1] + step2;
			for (int j = 1; j < pm.N; j++) {
				intm1 += step1;
				intm2 += step2;
				Vector3D dir = intm1;
				for (int k = 0; k < j; k++) {
					dir += step3;
					Ray ray(l->center, dir);//, eachIntens);
					ray.tracePm(eachIntens, eachIntens, pm, textures, gb, Ignore(index2(-1, -1), true), tp);
				}
			}
		}
	}
}