#if !defined (_AABB_)
#define _AABB_

#include <glm/glm.hpp>

namespace FreddieBabord
{

	class AABB
	{
	public:
		glm::vec3 corner;
		glm::vec3 position;

		AABB(glm::vec3 &corner, glm::vec3 position);
		AABB(void);
		~AABB();

		void SetBox(glm::vec3 &corner, glm::vec3 position);

		glm::vec3 GetVertexPositive(glm::vec3 &normal);
		glm::vec3 GetVertexNegative(glm::vec3 &normal);
	};

}

#endif