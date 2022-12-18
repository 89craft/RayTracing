#pragma once

#include <glm/glm.hpp>

#include <vector>

struct Sphere
{
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Material
{
	glm::vec3 Albedo{ 1.0f };
};

struct RenderScene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};