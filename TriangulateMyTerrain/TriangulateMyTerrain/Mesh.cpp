#include "Mesh.h"

Mesh Mesh::GenerateIndexedGrid(int gapX, int gapY, int sizeX, int sizeY)
{
	const int dimX = gapX;
	const int dimY = gapY;

	const int numberOfVerticies = dimX * dimY;
	const int numberOfQuads = (dimX - 1) * (dimY - 1);
	const int numberOfQuadsX = (dimX - 1);
	const int numberOfQuadsY = (dimY - 1);
	const int numerOfTriangles = 2 * numberOfQuads;
	const int elementCount = 3 * numerOfTriangles;

	Mesh newMesh;

	newMesh.verticies.reserve(numberOfVerticies);
	newMesh.elements.reserve(elementCount);

	newMesh.height = dimY;
	newMesh.width = dimX;

	//Init Verts
	for (int y = 0; y < dimY; y++)
	{
		for (int x = 0; x < dimX; x++)
		{
			Vertex vertex;
			vertex.position = glm::vec3((sizeX / numberOfQuadsX) * x, 0, -(sizeY / numberOfQuadsY) * y);

			vertex.normal = glm::vec3(0, 1, 0);
			vertex.uv = glm::vec2((float)x / numberOfQuadsX, (float)y / numberOfQuadsY);
			newMesh.verticies.push_back(vertex);
		}
	}

	//Init elms
	
	for (int y = 0; y < numberOfQuadsY; ++y)
	{
		for (int x = 0; x < numberOfQuadsX; ++x)
		{
			int k = y*dimX + x;

			if ((x % 2 == 0 && y % 2 == 0) || (x % 2 == 1 && y % 2 == 1))
			{
				newMesh.elements.push_back(k);
				newMesh.elements.push_back(k + 1);
				newMesh.elements.push_back(k + 1 + dimX);

				newMesh.elements.push_back(k);
				newMesh.elements.push_back(k + 1 + dimX);
				newMesh.elements.push_back(k + dimX);
			}
			else
			{
				newMesh.elements.push_back(k);
				newMesh.elements.push_back(k + 1);
				newMesh.elements.push_back(k + dimX);

				newMesh.elements.push_back(k + 1);
				newMesh.elements.push_back(k + 1 + dimX);
				newMesh.elements.push_back(k + dimX);
			}
		}
	}

	return newMesh;
}

void Mesh::RecalculateNormals()
{
	for (int i = 0; i < GetVertexCount() - 3; i += 3)
	{
		verticies[i].normal = glm::vec3(0, 0, 0);
		verticies[i + 1].normal = glm::vec3(0, 0, 0);
		verticies[i + 2].normal = glm::vec3(0, 0, 0);

	}

	for (int i = 0; i < elements.size() - 3; i+=3)
	{
		//Should be vec3
		auto point0 = GetVertexPosition(elements[i]);
		auto point1 = GetVertexPosition(elements[i+1]);
		auto point2 = GetVertexPosition(elements[i+2]);

		glm::vec3 eA = point1 - point0;
		glm::vec3 eB = point2 - point0;
		auto cross = glm::cross(eA, eB);
		auto normal = glm::normalize(cross);
		
		verticies[elements[i]].normal += normal;
		verticies[elements[i+1]].normal += normal;
		verticies[elements[i+2]].normal += normal;
	}

	for (int i = 0; i < GetVertexCount() - 3; i += 3)
	{
		//Should be vec3
		glm::vec3 point0 = glm::normalize(verticies[i].normal);
		glm::vec3 point1 = glm::normalize(verticies[i+1].normal);
		glm::vec3 point2 = glm::normalize(verticies[i+2].normal);

		verticies[i].normal = point0;
		verticies[i + 1].normal = point1;
		verticies[i + 2].normal = point2;
	}

}

glm::vec3 Mesh::BezierSurface(std::vector<std::vector<std::vector<glm::vec3>>> cps, float u, float v)
{
	//Note Cps must be 4 not 8
	std::vector<glm::vec3> curve;
	curve.reserve(cps.size());

	assert(u >= 0);

	if (u <= 0.5f || u == 0.0f)
	{
		for (int i = 0; i < cps.size() / 2; ++i)
		{
			for (int j = 0; j < cps[i].size(); ++j)
			{
				curve.push_back(BezierCurve(cps[i][j], v));
			}

		}
	}
	else
	{
		for (int i = cps.size() / 2; i < cps.size(); ++i)
		{
			for (int j = 0; j < cps[i].size(); ++j)
			{
				curve.push_back(BezierCurve(cps[i][j], v));
			}

		}
	}

	if (u < 0.5f)
	{
		assert(u * 2 <= 1 && u >= 0.f);
		return BezierCurve(curve, u * 2);
	}
	else
	{
		assert((u - 0.5) * 2 <= 1 && u >= 0.f);
		return BezierCurve(curve, (u - 0.5) * 2);
	}
}

glm::vec3 Mesh::BezierCurve(std::vector<glm::vec3>& pts, double t){
	glm::vec3 position;
	std::size_t len = pts.size();
	for (std::size_t i = 0; i < len; ++i){
		position += pts[i] * choose(len - 1, i) * powf(1 - t, len - 1 - i) * powf(t, i);
	}
	return position;
}

void Mesh::ApplyBezier(std::vector<std::vector<std::vector<glm::vec3>>> cps)
{
	for (int i = 0; i < GetVertexCount(); ++i)
	{
		auto t = GetVertexCoordinate(i);
		SetVertexPosition(i, BezierSurface(cps, t.x, t.y));
	}
}

glm::vec2 Mesh::GetVertexCoordinate(int index)
{
	return verticies[index].uv;
}

glm::vec3 Mesh::GetVertexPosition(int index)
{
	return verticies[index].position;
}

glm::vec3 Mesh::GetVertexNormal(int index)
{
	return verticies[index].normal;
}

void Mesh::SetVertexNormal(int index, glm::vec3 newNormal)
{
	verticies[index].normal = newNormal;
}

void Mesh::AddVertexNormal(int index, glm::vec3 newNormal)
{
	verticies[index].normal += newNormal;
}

void Mesh::SetVertexPosition(int index, glm::vec3 newPosition)
{
	verticies[index].position = newPosition;
}

float Mesh::factorial(float num){
	if (num <= 1){
		return 1;
	}
	float fac = 1;
	for (float i = 1; i <= num; i++){
		fac *= i;
	}
	return fac;
}

float Mesh::choose(float a, float b){
	return factorial(a) / (factorial(b) * factorial(a - b));
}