#if !defined(_QUADTREE_H_)
#define _QUADREE_H_

#include <glm/glm.hpp>
#include <vector>

namespace FreddieBabord
{
	struct Object
	{
		Object(glm::vec3 position, float width_ = 2, float hieght_ = 2, float depth_ = 2) :width(width_), height(hieght_), depth(depth_){ x = position.x; y = position.y; z = position.z; };
		glm::vec3 Position() const { return glm::vec3(x, z, -y); };

		float x, y, z;
		float width, height, depth;
	};

	class Quadtree {
	public:
		Quadtree(float x, float y, float width, float height, int level, int maxLevel);
		~Quadtree();

		void			AddObject(Object *object);
		std::vector<Object*>	GetObjectsAt(float x, float y);
		void			Clear();

	private:
		float           x;
		float           y;
		float           width;
		float           height;
		int	            level;
		int             maxLevel;
		std::vector<Object*>	objects;

		Quadtree *      parent;
		Quadtree *      NW;
		Quadtree *      NE;
		Quadtree *      SW;
		Quadtree *      SE;

		bool            contains(Quadtree *child, Object *object);
	};
}

#endif