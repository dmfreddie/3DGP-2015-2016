#pragma once

#include <glm/glm.hpp>
#include <vector>

// Bezier curve from: http://www.dreamincode.net/forums/topic/320703-bezier-curves-part-1-linear-algebra-series/

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh
{
public:

	static Mesh GenerateIndexedGrid(int gapX = 5, int gapY = 5, int sizeX = 5, int sizeY = 5);
	glm::vec3 BezierSurface(std::vector<std::vector<std::vector<glm::vec3>>> cps, float u, float v);
	glm::vec3 BezierCurve(std::vector<glm::vec3>& pts, double t);

	int GetVertexCount() const { return (int)verticies.size(); };

	void ApplyBezier(std::vector<std::vector<std::vector<glm::vec3>>> cps);
	
	std::vector<Vertex> verticies;
	std::vector<int> elements;
	void RecalculateNormals();

	int Height()const{ return height; };
	int Width()const{ return width; };

private:
	glm::vec2 GetVertexCoordinate(int index);
	glm::vec3 GetVertexPosition(int index);
	glm::vec3 GetVertexNormal(int index);
	void SetVertexPosition(int index, glm::vec3 newPosition);
	void SetVertexNormal(int index, glm::vec3 newNormal);
	void AddVertexNormal(int index, glm::vec3 newNormal);
	float choose(float a, float b);
	float factorial(float num);

	int width, height;
};