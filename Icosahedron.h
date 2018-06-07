#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"

#include "Index.h"

using namespace std;

class Icosahedron
{
public:
	Vector3D vertex[12];

	Icosahedron()
	{
		vertex[0] = Vector3D(0.0f, 0.0f, sqrtf(5.0f) / 2.0f);
		vertex[11] = Vector3D(0.0f, 0.0f, -sqrtf(5.0f) / 2.0f);
		for (int j = 0; j < 5; j++) {
			vertex[j + 1] = Vector3D(cosf(j * M_PI * 0.4f), sinf(j * M_PI * 0.4f), 0.5f);
			vertex[j + 6] = Vector3D(cosf((0.5f + j) * M_PI * 0.4f), sinf((0.5f + j) * M_PI * 0.4f), -0.5f);
		}
		for (int j = 0; j < 12; j++) {
			vertex[j].normalize();
		}
	}

	index3 verticesByFace(int i) {
		if (i == 0) return index3(0, 1, 2);
		if (i == 1) return index3(0, 2, 3);
		if (i == 2) return index3(0, 3, 4);
		if (i == 3) return index3(0, 4, 5);
		if (i == 4) return index3(0, 5, 1);

		if (i == 5) return index3(1, 2, 6);
		if (i == 6) return index3(2, 3, 7);
		if (i == 7) return index3(3, 4, 8);
		if (i == 8) return index3(4, 5, 9);
		if (i == 9) return index3(5, 1, 10);

		if (i == 10) return index3(2, 6, 7);
		if (i == 11) return index3(3, 7, 8);
		if (i == 12) return index3(4, 8, 9);
		if (i == 13) return index3(5, 9, 10);
		if (i == 14) return index3(1, 10, 6);

		if (i == 15) return index3(6, 7, 11);
		if (i == 16) return index3(7, 8, 11);
		if (i == 17) return index3(8, 9, 11);
		if (i == 18) return index3(9, 10, 11);
		if (i == 19) return index3(10, 6, 11);
		
		cout << "error in verticesByFace function" << endl;
		return index3(-1, -1, -1);
	}

	index2 verticesByEdge(int i) {
		if (i == 0) return index2(0, 1);
		if (i == 1) return index2(0, 2);
		if (i == 2) return index2(0, 3);
		if (i == 3) return index2(0, 4);
		if (i == 4) return index2(0, 5);

		if (i == 5) return index2(1, 2);
		if (i == 6) return index2(2, 3);
		if (i == 7) return index2(3, 4);
		if (i == 8) return index2(4, 5);
		if (i == 9) return index2(5, 1);

		if (i == 10) return index2(1, 6);
		if (i == 11) return index2(6, 2);
		if (i == 12) return index2(2, 7);
		if (i == 13) return index2(7, 3);
		if (i == 14) return index2(3, 8);

		if (i == 15) return index2(8, 4);
		if (i == 16) return index2(4, 9);
		if (i == 17) return index2(9, 5);
		if (i == 18) return index2(5, 10);
		if (i == 19) return index2(10, 1);

		if (i == 20) return index2(6, 7);
		if (i == 21) return index2(7, 8);
		if (i == 22) return index2(8, 9);
		if (i == 23) return index2(9, 10);
		if (i == 24) return index2(10, 6);

		if (i == 25) return index2(6, 11);
		if (i == 26) return index2(7, 11);
		if (i == 27) return index2(8, 11);
		if (i == 28) return index2(9, 11);
		if (i == 29) return index2(10, 11);

		cout << "error in verticesByEdge function" << endl;
		return index2(-1, -1);
	}
};

