#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"

#include <list>

#include "RectTex.h"
//#include "Ray.h"
#include "Photon.h"
#include "LightSource.h"
#include "Icosahedron.h"
#include "Index.h"
#include "Clamp.h"
#include "Bspline.h"

using namespace std;

//const int hb = 20;		//hash blocks at each dimension
//const float h = 3.0f;	//max distance where Photons counts

class PhotonMap
{
public:
	list<Photon> hash;// [hb][hb][hb];
	//Vector3D corner;
	//float scale;
	int N; //ray amount at one edge
	//float totalIntens;
	//float blockScale;
	//int maxShift;

	PhotonMap(int N) : N(N)//, totalIntens(totalIntens)//, const Vector3D& corner, float scale) : N(N), totalIntens(totalIntens), corner(corner), scale(scale)
	{
		//blockScale = scale / hb;
		//maxShift = ceilf(h / blockScale);
	}

	void addPhoton(const Vector3D &point, float intens)
	{
		/*Vector3D shift = (point - corner) / blockScale;
		index3 i(roundf(shift.x), roundf(shift.y), roundf(shift.z));
		if (i.v1 < 0 || i.v1 >= hb || i.v2 < 0 || i.v2 >= hb || i.v3 < 0 || i.v3 >= hb) {
			cout << "error in addPhoton function: Photon out of bounds" <<i.v1<<' '<<i.v2<<' '<<i.v3 << endl;
			return;
		}
		hash[i.v1][i.v2][i.v3].push_back(Photon(point, intens));*/
		hash.push_back(Photon(point, intens));
	}

	float retIntens(const Vector3D &point) const
	{
		/*Vector3D shift = (point - corner) / blockScale;
		index3 i(roundf(shift.x), roundf(shift.y), roundf(shift.z));
		if (i.v1 < 0 || i.v1 >= hb || i.v2 < 0 || i.v2 >= hb || i.v3 < 0 || i.v3 >= hb) {
			//cout << "error in retIntens function: point out of bounds" << endl;
			return -1.0f;
		}

		float sumIntens = 0.0f;
		for (int ix = i.v1 - maxShift; ix <= i.v1 + maxShift; ix++) {
			if (ix >= 0 && ix < hb) {
				for (int iy = i.v2 - maxShift; iy <= i.v2 + maxShift; iy++) {
					if (iy >= 0 && iy < hb) {
						for (int iz = i.v3 - maxShift; iz <= i.v3 + maxShift; iz++) {
							if (iz >= 0 && iz < hb) {
								for (list<Photon>::const_iterator j = hash[ix][iy][iz].begin(); j != hash[ix][iy][iz].end(); ++j) {
									sumIntens += fading((point - j->loc).length()) * j->intens;
								}
							}
						}
					}
				}
			}
		}*/
		float sumIntens = 0.0f;
		for (list<Photon>::const_iterator j = hash.begin(); j != hash.end(); ++j) {
			sumIntens += fading((point - j->loc).length()) * j->intens;
		}
		return sumIntens;
	}

	float fading(float d) const
	{
		//return 1.0f / (1.0f + d);
		return clamp(1.0f - 0.2f * d, 0.0f, 1.0f);
		//return clamp(1.2f / (1.0f + d) - 0.2f, 0.0f, 1.0f);
		//return sqrtf(clamp(1.0f - 0.1f * d, 0.0f, 1.0f));
	}
};