#pragma once

#include <iostream>
#include <cmath>

#include "Vector3D.h"
#include "Plane.h"

#include <list>

#include "RectTex.h"
#include "LightSource.h"
#include "PhotonMap.h"
#include "Photon.h"
#include "Bspline.h"
#include "Refraction.h"
#include "Portal.h"

using namespace std;

class Ray
{
public:
	Vector3D org;
	Vector3D dir;

	Ray(const Vector3D& org, const Vector3D& dir): org(org), dir(dir)
	{
		this->dir.normalize();
	}

	bool trace(Vector3D &color, const PhotonMap &pm, const list<RectTex> &textures, 
		const list<LightSource> &light, const list<GaussBspline> &gb, const Ignore &ig, float intensity, const list<Teleport> &tp) const
	{
		if (intensity < 0.1f) {
			color = Vector3D(0.0f);
			return true;
		}
		float t;
		Vector3D point;
		float nearest = 1000000.0f;
		bool stopped = false;
		bool continued = false;
		bool teleported = false;
		float intens = -1.0f;		//if intens < 0.0f, take intensity from photon map, else take it from intens
		Vector3D c, p;
		for (list<RectTex>::const_iterator i = textures.begin(); i != textures.end(); ++i) {
			if (i->intersectByRayPixel(org, dir, t, c, p)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					color = c;
					point = p;
					stopped = true;
					continued = false;
					teleported = false;
				}
			}
		}
		Vector3D normal;
		Ignore igout;
		for (list<GaussBspline>::const_iterator i = gb.begin(); i != gb.end(); ++i) {
			if (i->intersectByRay(org, dir, t, p, normal, ig, igout)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					point = p;
					stopped = true;
					continued = true;
					teleported = false;
					//cout << "trace: found Bspline!" << endl;
				}
			}
		}
		for (list<LightSource>::const_iterator i = light.begin(); i != light.end(); ++i) {
			if (i->intersectByRay(org, dir, t)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					color = Vector3D(1.0f, 1.0f, 1.0f);
					intens = 0.99f / intensity;
					point = p;
					stopped = true;
					continued = false;
					teleported = false;
					//cout << "trace: found LightSource!" << endl;
				}
			}
		}
		list<Teleport>::const_iterator port;
		for (list<Teleport>::const_iterator i = tp.begin(); i != tp.end(); ++i) {
			if (i->in.intersectByRay(org, dir, t, p)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					port = i;
					stopped = true;
					continued = true;
					teleported = true;
				}
			}
		}

		if (stopped  && !continued) {
			if (intens < 0.0f) {
				color *= clamp(pm.retIntens(point), 0.0f, 1.0f);
			}
			else {
				color *= intens;
			}
		}
		if (continued) {
			if (!teleported) {
				Vector3D refrac;
				color = Vector3D(0.0f);
				Vector3D color1 = Vector3D(0.7843137255f);
				float refrac_intens, reflec_intens = 0.9f;
				if (refraction(refrac, dir, normal, 1.5f)) {
					refrac_intens = 0.7f;
					reflec_intens = 0.2f;
					Ray refrac_ray(point, refrac);
					refrac_ray.trace(color1, pm, textures, light, gb, igout, refrac_intens * intensity, tp);
					color += refrac_intens * color1;
				}
				color1 = Vector3D(0.7843137255f);
				Ray reflec_ray(point, reflect(dir, normal));
				reflec_ray.trace(color1, pm, textures, light, gb, igout, reflec_intens * intensity, tp);
				color += reflec_intens * color1;
			} else {
				Vector3D org_out, dir_out;
				port->teleport(org, dir, org_out, dir_out);
				Ray ported_ray(org_out, dir_out);
				ported_ray.trace(color, pm, textures, light, gb, Ignore(index2(-1, -1), true), intensity, tp);
			}
		}
		return stopped;
	}




	bool tracePm(float eachIntens, float firstIntens, PhotonMap &pm, const list<RectTex> &textures, 
		const list<GaussBspline> &gb, const Ignore &ig, const list<Teleport> &tp) const
	{
		if (eachIntens / firstIntens < 0.1f) {
			return true;
		}
		float t;
		Vector3D point;
		float nearest = 1000000.0f;
		bool stopped = false;
		bool continued = false;
		bool teleported = false;
		Vector3D c, p;
		for (list<RectTex>::const_iterator i = textures.begin(); i != textures.end(); ++i) {
			if (i->intersectByRay(org, dir, t, p)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					point = p;
					stopped = true;
					continued = false;
					teleported = false;
				}
			}
		}
		Vector3D normal;
		Ignore igout;
		for (list<GaussBspline>::const_iterator i = gb.begin(); i != gb.end(); ++i) {
			if (i->intersectByRay(org, dir, t, p, normal, ig, igout)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					point = p;
					stopped = true;
					continued = true;
					teleported = false;
					//cout << "tracePm: found Bspline!" << endl;
				}
			}
		}
		list<Teleport>::const_iterator port;
		for (list<Teleport>::const_iterator i = tp.begin(); i != tp.end(); ++i) {
			if (i->in.intersectByRay(org, dir, t, p)) {
				if (t > 0.0f && t < nearest) {
					nearest = t;
					port = i;
					stopped = true;
					continued = true;
					teleported = true;
				}
			}
		}

		if (stopped && !continued) {
			pm.addPhoton(point, eachIntens);
		}
		if (continued) {
			if (!teleported) {
				Vector3D refrac;
				float refrac_intens, reflec_intens = 0.9f;
				if (refraction(refrac, dir, normal, 1.5f)) {
					refrac_intens = 0.7f;
					reflec_intens = 0.2f;
					Ray refrac_ray(point, refrac);
					refrac_ray.tracePm(refrac_intens * eachIntens, firstIntens, pm, textures, gb, igout, tp);
				}
				Ray reflec_ray(point, reflect(dir, normal));
				reflec_ray.tracePm(reflec_intens * eachIntens, firstIntens, pm, textures, gb, igout, tp);
			} else {
				Vector3D org_out, dir_out;
				port->teleport(org, dir, org_out, dir_out);
				Ray ported_ray(org_out, dir_out);
				ported_ray.tracePm(eachIntens, firstIntens, pm, textures, gb, Ignore(index2(-1, -1), true), tp);
			}
		}
		return stopped;
	}
};