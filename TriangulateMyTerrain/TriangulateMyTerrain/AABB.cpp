#include "AABB.h"

namespace FreddieBabord
{

	AABB::AABB(glm::vec3 &corner, glm::vec3 position)
	{
		SetBox(corner, position);
	}

	AABB::AABB(void)
	{
		corner.x = 0; corner.y = 0; corner.z = 0;
		position.x = 0; position.y = 0; position.z = 0;
	}

	AABB::~AABB(){}

	void AABB::SetBox(glm::vec3 &corner, glm::vec3 position)
	{
		this->corner = glm::vec3(corner);

		if (position.x < 0.0)
		{
			position.x = -position.x;
			this->corner.x -= position.x;
		}
		if (position.y < 0.0)
		{
			position.y = -position.y;
			this->corner.y -= position.y;
		}
		if (position.z < 0.0)
		{
			position.z = -position.z;
			this->corner.z -= position.z;
		}

		this->position.x = position.x;
		this->position.y = position.y;
		this->position.z = position.z;
	}

	glm::vec3 AABB::GetVertexPositive(glm::vec3 &normal)
	{
		glm::vec3 result = corner;

		if (normal.x > 0)
			result += position.x;

		if (normal.y > 0)
			result += position.y;

		if (normal.z > 0)
			result += position.z;

		return result;
	}

	glm::vec3 AABB::GetVertexNegative(glm::vec3 &normal)
	{
		glm::vec3 result = corner;

		if (normal.x < 0)
			result += position.x;

		if (normal.y < 0)
			result += position.y;

		if (normal.z < 0)
			result += position.z;

		return result;
	}
}