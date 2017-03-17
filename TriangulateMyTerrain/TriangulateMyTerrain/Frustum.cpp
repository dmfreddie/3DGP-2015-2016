#include "Frustum.h"

namespace FreddieBabord
{

#define ANG2RAD 3.14159265358979323846/180.0

	void Frustum::setCamInternals(float angle, float ratio, float nearD, float farD) {

		this->ratio = ratio;
		this->angle = angle;
		this->nearD = nearD;
		this->farD = farD;

		tang = (float)tan(angle* ANG2RAD * 0.5);
		nh = nearD * tang;
		nw = nh * ratio;
		fh = farD  * tang;
		fw = fh * ratio;


	}


	void Frustum::setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u) 
	{
		// direction // nearclip // farclip
		glm::vec3 dir, nc, fc, X, Y, Z;

		Z = p - l;
		Z = glm::normalize(Z);

		X = u * Z;
		X= glm::normalize(X);

		Y = Z * X;

		nc = p - Z * nearD;
		fc = p - Z * farD;

		ntl = nc + Y * nh - X * nw;
		ntr = nc + Y * nh + X * nw;
		nbl = nc - Y * nh - X * nw;
		nbr = nc - Y * nh + X * nw;

		ftl = fc + Y * fh - X * fw;
		ftr = fc + Y * fh + X * fw;
		fbl = fc - Y * fh - X * fw;
		fbr = fc - Y * fh + X * fw;

		planes[Top].set3Points(ntr, ntl, ftl);
		planes[Bottom].set3Points(nbl, nbr, fbr);
		planes[Left].set3Points(ntl, nbl, fbl);
		planes[Right].set3Points(nbr, ntr, fbr);
		planes[Near].set3Points(ntl, ntr, nbr);
		planes[Far].set3Points(ftr, ftl, fbl);
	}


	int Frustum::pointInFrustum(glm::vec3 &p) {

		int result = Inside;
		for (int i = 0; i < 6; i++) {

			if (planes[i].distance(p) < 0)
				return Outside;
		}
		return(result);

	}


	int Frustum::boxInFrustum(AABB &b) {

		int result = Inside;
		for (int i = 0; i < 6; i++) {

			if (planes[i].distance(b.GetVertexPositive(planes[i].normal)) < 0)
				result = Outside;
			else if (planes[i].distance(b.GetVertexNegative(planes[i].normal)) < 0)
				result = Intersect;
		}
		return(result);

	}


}