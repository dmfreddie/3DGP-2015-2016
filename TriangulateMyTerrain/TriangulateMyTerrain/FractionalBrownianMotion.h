#pragma once

#include "Mesh.h"
#include "PerlinNoise.h"
#include "SimplexNoise.h"


void ApplyNoise(float octaves, Mesh& mesh, float multiplyer, SimplexNoise sn, float outputScale = 3.5f)
{	
	

	for (int i = 0; i < mesh.GetVertexCount(); ++i)
	{
		auto& p = mesh.verticies[i].position;
		auto& u = mesh.verticies[i].uv;
		auto& n = mesh.verticies[i].normal;
		float x = u.x * multiplyer;
		float y = u.y * multiplyer;
		p += n * outputScale * sn.fractal((size_t)octaves, x , y);
	}

	mesh.RecalculateNormals();
}