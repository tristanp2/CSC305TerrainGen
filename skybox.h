#pragma once
#include "scene.h"

//Note: This is not currently functioning
class SkyBox
{
	int SIZE;
	std::vector<float> positions;
	uint32_t mesh_id;
	void CreateMesh();
	Scene* scene;
public:
	SkyBox();
	uint32_t MeshID();
	SkyBox(Scene*);
	~SkyBox();
};

