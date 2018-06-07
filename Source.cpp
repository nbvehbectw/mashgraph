#include <iostream>
#include <cstdlib>
#include <cmath>

#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>

#include "Vector3D.h"
#include "Plane.h"
#include "Math3D.h"

#include <list>

#include "Camera.h"
#include "Icosahedron.h"
#include "RectTex.h"
#include "Ray.h"
#include "LightSource.h"
#include "Photon.h"
#include "PhotonMap.h"
#include "PhotonMapCreation.h"
#include "Bspline.h"
#include "Output.h"
#include "Portal.h"

using namespace std;

int main()
{
	ilInit();
	iluInit();
	ilutInit();

	list<RectTex> textures;
	//textures.push_back(RectTex(L"D:\\rd.png", Vector3D(0, 0, 20), Vector3D(10, 0, 20), Vector3D(0, 0, 0)));
	textures.push_back(RectTex(L"fs.png", Vector3D(10, 10, 20), Vector3D(0, -20, 0), Vector3D(0, 0, -20)));
	textures.push_back(RectTex(L"aj.png", Vector3D(-10, 10, 0), Vector3D(20, 0, 0), Vector3D(0, -20, 0)));
	textures.push_back(RectTex(L"rd.png", Vector3D(-10, 10, 20), Vector3D(20, 0, 0), Vector3D(0, 0, -20)));
	textures.push_back(RectTex(L"ts.png", Vector3D(-10, -10, 20), Vector3D(0, 20, 0), Vector3D(0, 0, -20)));
	textures.push_back(RectTex(L"rr.png", Vector3D(10, -10, 20), Vector3D(-20, 0, 0), Vector3D(0, 0, -20)));
	textures.push_back(RectTex(L"pp.png", Vector3D(10, 10, 20), Vector3D(-20, 0, 0), Vector3D(0, -20, 0)));

	RectTex output(L"framesize1.png");

	int N = 15;
	for (int i = 0; i < N; i++) {
		cout << "\nFrame number " << i << endl;

		float gmul = 2.7f * (1.0f + 0.9f * sinf(2.0f * M_PI * (float)i / (float)N));
		float xmul = 5.0f - cosf(2.0f * M_PI * (float)i / (float)N);

		cout << "GaussBspline creation" << endl;
		list<GaussBspline> gb;
		gb.push_back(GaussBspline());
		cout << "GaussBspline initialization" << endl;
		gb.begin()->gaussInit(Vector3D(-10, -10, 10), Vector3D(20, 0, 0), Vector3D(0, 20, 0), xmul, gmul);
		cout << "Triangulation creation" << endl;
		gb.begin()->triangInit(15);

		list<LightSource> ls;
		ls.push_back(LightSource(Vector3D(0, 0, 18), 60.0f, 0.5f));

		Portal pt1(Vector3D(5, 0, 17), Vector3D(1, 0, 0), Vector3D(0, 0, 1), 3);
		Portal pt2(Vector3D(-7, 7, 4), Vector3D(0, 0, -1), Vector3D(1, 0, 0), 3);
		list<Teleport> tp;
		tp.push_back(Teleport(pt1, pt2));
		tp.push_back(Teleport(pt2, pt1));

		cout << "PhotonMap creation" << endl;

		PhotonMap pm(10);// , Vector3D(-1, -11, -1), 22);
		create(pm, ls, textures, gb, tp);

		cout << "Camera iteration" << endl;

		Camera cam(Vector3D(-30, -40, 25), Vector3D(1, 1.5f, -0.6f), 0.001f / 1.5f, 3);
		cam.camIterate(textures, ls, gb, pm, output, tp);

		//output.writeOutput(L"out.png");
		outnum(output, i);
	}

	//system("pause");

	return 0;
}