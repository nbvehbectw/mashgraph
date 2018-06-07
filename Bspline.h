#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"

#include "Gauss.h"
#include "Index.h"
#include "Clamp.h"

using namespace std;

const float eps = 0.0001f;
const int Cp = 7, Sp = 3;

class Bspline 
{
public:
	int N = Cp; //number of control points
	int p = Sp; //spline power
	int Tnum = Cp + Sp + 1;
	//float *T = nullptr; //uzlovoy vector
	float T[Cp + Sp + 1];

	Bspline(/*int N, int p*/)// : N(N), p(p)
	{
		/*Tnum = N + p + 1;
		T = new float[Tnum];*/
		for (int i = 0; i <= p; i++) {
			T[i] = 0.0f;
			T[i + N] = 1.0f;
		}
		float den = N - p;
		for (int i = p + 1; i < N; i++) {
			T[i] = (float)(i - p) / den;
		}
	}

	~Bspline()
	{
		//delete T;
	}

	float B(float t, int i, int k) const
	{
		if (t < 0.0f || t > 1.0f) {
			//cout << "error in B function: t out of bounds, t = " << t << endl;
			t = clamp(t, 0.0f, 1.0f);
		}
		if (k > p) {
			cout << "error in B function: k > p" << endl;
		}
		if (i > Tnum - k - 2) {
			cout << "error in B function: i > N+p-k-1" << endl;
		}
		if (k == 0) {
			return i == findIndex(t) ? 1.0f : 0.0f;
		} else {
			float ret = 0.0f;
			if (T[i + k] > T[i]) {
				ret += (t - T[i]) * B(t, i, k - 1) / (T[i + k] - T[i]);
			}
			if (T[i + k + 1] > T[i + 1]) {
				ret += (T[i + k + 1] - t) * B(t, i + 1, k - 1) / (T[i + k + 1] - T[i + 1]);
			}
			return ret;
		}
	}

	int findIndex(float t) const
	{
		int min = p, max = N;
		while (min < max - 1) {
			int mid = (min + max) / 2;
			if (t < T[mid]) {
				max = mid;
			} else {
				min = mid;
			}
		}
		return min;
	}

	float B(float t, int i) const
	{
		return B(t, i, p);
	}
};




class TriPoint
{
public:
	Vector3D vec;
	float u, v;

	TriPoint() {}
	TriPoint(const Vector3D &vec, float u, float v) : vec(vec), u(u), v(v) {}
};

class Ignore
{
public:
	index2 ind;
	bool upper;

	Ignore() {}
	Ignore(const index2 &ind, bool u): ind(ind), upper(u) {}
};

class GaussBspline
{
public:
	int N;
	Bspline bs;
	//Vector3D **P = nullptr; // control points
	Vector3D P[Cp][Cp];

	int trinum;
	TriPoint **triang = nullptr;
	Vector3D corner, dir1, dir2, up;

	GaussBspline()/*(int N, int p)*/ : N(Cp), bs(/*Cp, Sp*/)
	{
		/*P = new Vector3D*[N];
		for (int i = 0; i < N; i++) {
			P[i] = new Vector3D[N];
		}*/
	}

	~GaussBspline()
	{
		/*for (int i = 0; i < N; i++) {
			delete[] P[i];
		}*/
	}

	void gaussInit(const Vector3D &crnr, const Vector3D &d1, const Vector3D &d2, float xmul, float gmul)
	{
		corner = crnr;// -0.05f * (dir1 + dir2);
		dir1 = d1;// *1.1f;
		dir2 = d2;// *1.1f;
		up = (dir1 ^ dir2).normalize() * gmul;

		Vector3D h1 = dir1 / (N - 1);
		Vector3D h2 = dir2 / (N - 1);
		Vector3D iter = corner;
		float x = -0.5f * xmul;
		float h = xmul / (N - 1);
		for (int i = 0; i < N; i++) {
			Vector3D iter = corner + i * h1;
			float y = -0.5f * xmul;
			for (int j = 0; j < N; j++) {
				P[i][j] = iter + up * gauss(x, y);
				//cout << x << ' ' << y << endl;
				//cout << P[i][j].x << ' ' << P[i][j].y << ' ' << P[i][j].z << endl;
				y += h;
				iter += h2;
			}
			x += h;
		}
	}

	Vector3D Q(float u, float v) const
	{
		Vector3D ret(0.0f);
		float bu[Cp], bv[Cp];
		/*bu = new float[N];
		bv = new float[N];*/
		for (int i = 0; i < N; i++) {
			bu[i] = bs.B(u, i);
			bv[i] = bs.B(v, i);
			//cout << bu[i] << ' ' << bv[i] << endl;
		}
		//cout << "inside Q, u = " << u << "v = " << v << endl;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				ret += bu[i] * bv[j] * P[i][j];
			}
		}
		/*delete bu;
		delete bv;*/
		return ret;
	}

	Vector3D normal(float u, float v) const
	{
		return ((Q(u + eps, v) - Q(u - eps, v)).normalize() ^ (Q(u, v + eps) - Q(u, v - eps)).normalize()).normalize();
	}

	void triangInit(int ta)
	{
		trinum = ta;
		triang = new TriPoint*[trinum];
		for (int i = 0; i < trinum; i++) {
			triang[i] = new TriPoint[trinum];
		}
		float u = 0.0f, h = 1.0f / (trinum - 1);
		for (int i = 0; i < trinum; i++) {
			float v = 0.0f;
			for (int j = 0; j < trinum; j++) {
				triang[i][j] = TriPoint(Q(u, v), u, v);
				//cout << u << ' ' << v << endl;
				//cout << triang[i][j].vec.x << ' ' << triang[i][j].vec.y << ' ' << triang[i][j].vec.z << ' ' << endl;
				v += h;
			}
			u += h;
		}
	}

	bool intersectByRay(const Vector3D &org, const Vector3D &dir, 
		float &t, Vector3D &point, Vector3D &normal_, const Ignore &igin, Ignore &igout) const
	{
		if (!boxIntersect(org, dir)) {
			return false;
		}
		float t1;
		float a, b;
		float alpha, beta;
		float nearest = 1000000.0f; 
		Vector3D p;
		index2 res;
		bool upper;
		bool stopped = false;
		for (int i = 0; i < trinum - 1; i++) {
			for (int j = 0; j < trinum - 1; j++) {
				if (igin.ind.v1 != i || igin.ind.v2 != j || igin.upper != true) {
					if (triangInresect(org, dir, triang[i][j].vec, triang[i + 1][j].vec, triang[i + 1][j + 1].vec, t1, p, alpha, beta)) {
						if (t1 < nearest) {
							//cout << "inside GaussBspline::intersectByRay" << endl;
							nearest = t1;
							point = p;
							a = alpha;
							b = beta;
							res = index2(i, j);
							upper = true;
							igout = Ignore(res, upper);
							stopped = true;
						}
					}
				}
				if (igin.ind.v1 != i || igin.ind.v2 != j || igin.upper != false) {
					if (triangInresect(org, dir, triang[i][j].vec, triang[i][j + 1].vec, triang[i + 1][j + 1].vec, t1, p, alpha, beta)) {
						if (t1 < nearest) {
							nearest = t1;
							point = p;
							a = alpha;
							b = beta;
							res = index2(i, j);
							upper = false;
							igout = Ignore(res, upper);
							stopped = true;
						}
					}
				}
			}
		}
		if (stopped) {
			float u, v;
			t = nearest;
			if (upper) {
				u = (1.0f - a) * triang[res.v1][res.v2].u + a * (1.0f - b) * triang[res.v1 + 1][res.v2].u +
					a * b * triang[res.v1 + 1][res.v2 + 1].u;
				v = (1.0f - a) * triang[res.v1][res.v2].v + a * (1.0f - b) * triang[res.v1 + 1][res.v2].v +
					a * b * triang[res.v1 + 1][res.v2 + 1].v;
			}
			else {
				u = (1.0f - a) * triang[res.v1][res.v2].u + a * (1.0f - b) * triang[res.v1][res.v2 + 1].u +
					a * b * triang[res.v1 + 1][res.v2 + 1].u;
				v = (1.0f - a) * triang[res.v1][res.v2].v + a * (1.0f - b) * triang[res.v1][res.v2 + 1].v +
					a * b * triang[res.v1 + 1][res.v2 + 1].v;
			}
			normal_ = normal(u, v);
		}
		return stopped;
	}

	bool boxIntersect(const Vector3D &org, const Vector3D &dir) const
	{
		if (rectIntersect(corner, dir1, dir2, org, dir)) return true;
		if (rectIntersect(corner, dir2, up, org, dir)) return true;
		if (rectIntersect(corner, up, dir1, org, dir)) return true;
		if (rectIntersect(corner + up, dir1, dir2, org, dir)) return true;
		if (rectIntersect(corner + dir1, dir2, up, org, dir)) return true;
		if (rectIntersect(corner + dir2, up, dir1, org, dir)) return true;
		//cout << "inside GaussBspline::boxIntersect" << endl;
		return false;
	}

	bool rectIntersect(const Vector3D &plaorg, const Vector3D &plad1, const Vector3D &plad2, const Vector3D &org, const Vector3D &dir) const
	{
		Plane pla(plad1 ^ plad2, plaorg);
		float t;
		if (pla.intersectByRay(org, dir, t)) {
			Vector3D point = org + t * dir;
			if (t < 0.0f) {
				return false;
			}
			float offsetX = (plad1 & (point - plaorg)) / plad1.lengthSq();
			float offsetZ = (plad2 & (point - plaorg)) / plad2.lengthSq();
			if (offsetX >= 0.0f && offsetX <= 1.0f && offsetZ >= 0.0f && offsetZ <= 1.0f) {
				//cout << "inside GaussBspline::rectIntersect" << endl;
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool triangInresect(const Vector3D &org, const Vector3D &dir, const Vector3D &A, const Vector3D &B, const Vector3D &C, 
		float &t, Vector3D &point, float &alpha, float &beta) const
	{
		//cout << A.x << ' ' << A.y << ' ' << A.z << endl;
		Plane pla(A, B, C);
		//cout << pla.n.x << ' ' << pla.n.y << pla.n.z << pla.dist << endl;
		if (!pla.intersectByRay(org, dir, t)) {
			return false;
		}
		//cout << "inside GaussBspline::triangInresect, t = " << t << endl;
		if (t < 0.0f) {
			return false;
		}
		point = org + t * dir;
		Vector3D CmB = C - B;
		Plane BCpla(B, C, B + (CmB ^ (A - B)));
		Vector3D AF = point - A;
		float AGlen;
		if (!BCpla.intersectByRay(A, AF, AGlen)) {
			return false;
		}
		Vector3D G = A + AGlen * AF;
		beta = ((G - B) & CmB) / CmB.lengthSq();
		if (beta < 0.0f || beta > 1.0f) {
			return false;
		}
		Vector3D GmA = G - A;
		alpha = ((point - A) & GmA) / GmA.lengthSq();
		if (alpha < 0.0f || alpha > 1.0f) {
			return false;
		}
		return true;
	}

	float snug_b = 2.0f;
	float snug_a = 0.5f / atanf(snug_b * 0.5f);
	float snuggle(float x) const
	{
		return snug_a * atan(snug_b * (x - 0.5f)) + 0.5f;
	}
};