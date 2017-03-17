#if !defined (FRUSTRUM)
#define FRUSTRUM

#include<glm/glm.hpp>
#include "Plane.h"
#include "AABB.h"

namespace FreddieBabord
{

	/*
		Attempted frustum culling but doesn't work.....sorry! :/
	*/
	class Frustum
	{
	public:
		static enum {Outside, Intersect, Inside};
		Plane planes[6];

		glm::vec3 ntl, ntr;
		glm::vec3 nbl, nbr;
		glm::vec3 ftl, ftr;
		glm::vec3 fbl, fbr;
		float nearD, farD, ratio, angle, tang;
		float nw, nh, fw, fh;

		void setCamInternals(float angle, float ratio, float nearD, float farD);
		void setCamDef(glm::vec3 &p, glm::vec3 &l, glm::vec3 &u);
		int pointInFrustum(glm::vec3 &p);
		int boxInFrustum(AABB &b);

	private:
		enum 
		{
			Top = 0,
			Bottom,
			Left,
			Right,
			Near,
			Far
		};
	};
}
#endif