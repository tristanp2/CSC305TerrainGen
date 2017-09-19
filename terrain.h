#pragma once
#include "scene.h"
#include "heightgenerator.h"
#include "stb_image.h"

struct TextureData {
	stbi_uc* grass_pixels;
	int grass_x;
	int grass_y;
	stbi_uc* rock_pixels;
	int rock_x;
	int rock_y;
};
//Defines a tile of terrain
class Terrain
{
	friend class TerrainContainer;

	//the following are currently not consts because changing the assignment value
	//somehow was not updating the value on a new build
	int SIZE;
	int VERTEX_COUNT;
	//offsets to allow terrains to generate heights based on world location
	int offset_x;
	int offset_z;
	int tile_x;
	int tile_z;
	HeightGenerator* hg;
	TextureData texture_data;
	float** heights;
	uint32_t mesh_id;
	void GenerateHeights();
	void GenerateTerrain(Scene* scene);
	void CreateMesh(std::vector<float>& verts, std::vector<float>& normals, std::vector<float>& tex_coords, std::vector<int>& indices, Scene* scene);
	glm::vec3 CalculateNormal(int x, int z);
public:
	float x;
	float z;
	uint32_t MeshID();
	int GridX();
	int GridZ();
	int size();
	int VertexCount();
	Terrain();
	Terrain(int grid_x, int grid_z, HeightGenerator* hg);
	~Terrain();
};

