#include "terrain.h"
#include "heightgenerator.h"
#include "preamble.glsl"
#include "stb_image.h"
#include <iostream>


void Terrain::GenerateHeights() {
	this->heights = new float*[VERTEX_COUNT];
	for (int i = 0; i < VERTEX_COUNT; i++) {
		this->heights[i] = new float[VERTEX_COUNT];
	}
	for (int i = 0; i < VERTEX_COUNT; i++)
		for (int j = 0; j < VERTEX_COUNT; j++)
			heights[j][i] = hg->generate_height(j + this->offset_x, i + this->offset_z);
}

void Terrain::GenerateTerrain(Scene* scene)
{
	int count = VERTEX_COUNT*VERTEX_COUNT;
	std::vector<float> verts(count * 3);
	std::vector<float> normals(count * 3);
	std::vector<float> tex_coords(count * 2);
	std::vector<int> indices(6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1));

	int vp = 0;
	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			verts[vp * 3] = float(j) / float(VERTEX_COUNT - 1) * SIZE;
			verts[vp * 3 + 1] = heights[j][i];
			verts[vp * 3 + 2] = float(i) / float(VERTEX_COUNT - 1) * SIZE;
			glm::vec3 normal = CalculateNormal(j + this->offset_x, i + this->offset_z);
			normals[vp * 3] = normal.x;
			normals[vp * 3 + 1] = normal.y;
			normals[vp * 3 + 2] = normal.z;
			tex_coords[vp * 2] = float(j) / float(VERTEX_COUNT - 1) ;
			tex_coords[vp * 2 + 1] = float(i) / float(VERTEX_COUNT - 1);
			vp++;
		}
	}
	int p = 0;
	for (int z = 0; z < VERTEX_COUNT - 1; z++) {
		for (int x = 0; x < VERTEX_COUNT - 1; x++) {
			int top_left = (z * VERTEX_COUNT) + x;
			int top_right = top_left + 1;
			int bottom_left = ((z + 1)*VERTEX_COUNT) + x;
			int bottom_right = bottom_left + 1;
			indices.at(p++) = top_left;
			indices.at(p++) = bottom_left;
			indices.at(p++) = top_right;
			indices.at(p++) = top_right;
			indices.at(p++) = bottom_left;
			indices.at(p++) = bottom_right;
		}
	}
	CreateMesh(verts, normals, tex_coords, indices, scene);
}
void Terrain::CreateMesh(std::vector<float>& verts, std::vector<float>& normals, std::vector<float>& tex_coords, std::vector<int>& indices, Scene* scene)
{
	Mesh newMesh;

	newMesh.Name = "terrain";
	newMesh.IndexCount = (GLuint)indices.size();
	newMesh.VertexCount = (GLuint)verts.size() / 3;

	if (verts.empty()) {
		newMesh.PositionBO = 0;
	}
	else {
		GLuint newPositionBO;
		glGenBuffers(1, &newPositionBO);
		glBindBuffer(GL_ARRAY_BUFFER, newPositionBO);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), verts.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		newMesh.PositionBO = newPositionBO;
	}

	if (tex_coords.empty())
	{
		newMesh.TexCoordBO = 0;
	}
	else
	{
		GLuint newTexCoordBO;
		glGenBuffers(1, &newTexCoordBO);
		glBindBuffer(GL_ARRAY_BUFFER, newTexCoordBO);
		glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(tex_coords[0]), tex_coords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		newMesh.TexCoordBO = newTexCoordBO;
	}

	if (normals.empty())
	{
		newMesh.NormalBO = 0;
	}
	else
	{
		GLuint newNormalBO;
		glGenBuffers(1, &newNormalBO);
		glBindBuffer(GL_ARRAY_BUFFER, newNormalBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		newMesh.NormalBO = newNormalBO;
	}

	if (indices.empty())
	{
		// should never happen
		newMesh.IndexBO = 0;
	}
	else
	{
		GLuint newIndexBO;
		glGenBuffers(1, &newIndexBO);
		// Why not bind to GL_ELEMENT_ARRAY_BUFFER?
		// Because binding to GL_ELEMENT_ARRAY_BUFFER attaches the EBO to the currently bound VAO, which might stomp somebody else's state.
		glBindBuffer(GL_ARRAY_BUFFER, newIndexBO);
		glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		newMesh.IndexBO = newIndexBO;
	}

	// Hook up VAO
	{
		GLuint newMeshVAO;
		glGenVertexArrays(1, &newMeshVAO);

		glBindVertexArray(newMeshVAO);

		if (newMesh.PositionBO)
		{
			glBindBuffer(GL_ARRAY_BUFFER, newMesh.PositionBO);
			glVertexAttribPointer(SCENE_POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SCENE_POSITION_ATTRIB_LOCATION);
		}

		if (newMesh.TexCoordBO)
		{
			glBindBuffer(GL_ARRAY_BUFFER, newMesh.TexCoordBO);
			glVertexAttribPointer(SCENE_TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SCENE_TEXCOORD_ATTRIB_LOCATION);
		}

		if (newMesh.NormalBO)
		{
			glBindBuffer(GL_ARRAY_BUFFER, newMesh.NormalBO);
			glVertexAttribPointer(SCENE_NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SCENE_NORMAL_ATTRIB_LOCATION);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.IndexBO);

		glBindVertexArray(0);

		newMesh.MeshVAO = newMeshVAO;
	}
	Material newMaterial;
	float dummy = 0.0f; //just want shit on the screen (not literally)
	newMaterial.Ambient[0] = 1.0f;
	newMaterial.Ambient[1] = 1.0f;
	newMaterial.Ambient[2] = 1.0f;
	newMaterial.Diffuse[0] = 1.0f;
	newMaterial.Diffuse[1] = 1.0f;
	newMaterial.Diffuse[2] = 1.0f;
	newMaterial.Specular[0] = dummy;
	newMaterial.Specular[1] = dummy;
	newMaterial.Specular[2] = dummy;
	newMaterial.Shininess = dummy;
	newMaterial.tiled = true;
	newMaterial.multi_sample = true;

	int size_x, size_y, comp;
	TextureData td = this->texture_data;
	if (td.grass_pixels != NULL) {
		uint32_t newDiffuseMapID;
		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		GLuint newDiffuseMapTO;
		glGenTextures(1, &newDiffuseMapTO);
		glBindTexture(GL_TEXTURE_2D, newDiffuseMapTO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, td.grass_x, td.grass_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->texture_data.grass_pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		DiffuseMap newDiffuseMap;
		newDiffuseMap.DiffuseMapTO = newDiffuseMapTO;

		newDiffuseMapID = scene->DiffuseMaps.insert(newDiffuseMap);
		newMaterial.DiffuseMapID = newDiffuseMapID;
	}
	if (td.rock_pixels != NULL) {
		uint32_t newDiffuseMapID;
		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		GLuint newDiffuseMapTO;
		glGenTextures(1, &newDiffuseMapTO);
		glBindTexture(GL_TEXTURE_2D, newDiffuseMapTO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, td.rock_x, td.rock_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->texture_data.rock_pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		DiffuseMap newDiffuseMap;
		newDiffuseMap.DiffuseMapTO = newDiffuseMapTO;

		newDiffuseMapID = scene->DiffuseMaps.insert(newDiffuseMap);
		newMaterial.DiffuseMapID2 = newDiffuseMapID;
	}
	uint32_t newMaterialID = scene->Materials.insert(newMaterial);

	GLDrawElementsIndirectCommand currDrawCommand;
	currDrawCommand.count = newMesh.IndexCount * 3;
	currDrawCommand.primCount = 1;
	currDrawCommand.firstIndex = 0;
	currDrawCommand.baseVertex = 0;
	currDrawCommand.baseInstance = 0;

	newMesh.DrawCommands.push_back(currDrawCommand);
	newMesh.MaterialIDs.push_back(newMaterialID);

	uint32_t newMeshID = scene->Meshes.insert(newMesh);
	this->mesh_id = newMeshID;
}

glm::vec3 Terrain::CalculateNormal(int x, int z)
{
	/*
	glm::vec3 v_center(x, hg.generate_height(x, z), z);
	glm::vec3 v_left(x - 1, hg.generate_height(x - 1, z), z);
	glm::vec3 v_right(x + 1, hg.generate_height(x + 1, z), z);
	glm::vec3 v_up(x, hg.generate_height(x, z - 1), z - 1);
	glm::vec3 v_down(x, hg.generate_height(x, z + 1), z + 1);

	glm::vec3 n1 = glm::cross(v_left - v_center, v_down - v_center);
	glm::vec3 n2 = glm::cross(v_right - v_center, v_up - v_center);
	glm::vec3 normal = n1 + n2;
	*/

	//Finite difference method (or something)
	//Only kinda, sorta works
	float left = hg->generate_height(x - 1, z);
	float right = hg->generate_height(x + 1, z);
	float up = hg->generate_height(x, z - 1);
	float down = hg->generate_height(x, z + 1);
	glm::vec3 normal(left - right, 15.0f, up - down);

	glm::normalize(normal);
	return normal;
}

uint32_t Terrain::MeshID()
{
	return this->mesh_id;
}

int Terrain::GridX()
{
	return this->tile_x;
}

int Terrain::GridZ()
{
	return this->tile_z;
}

int Terrain::size()
{
	return this->SIZE;
}

int Terrain::VertexCount()
{
	return this->VERTEX_COUNT;
}

Terrain::Terrain()
{
}

Terrain::Terrain(int grid_x, int grid_z, HeightGenerator* hg)
{
	this->hg = hg;
	this->SIZE = 100;
	this->tile_x = grid_x;
	this->tile_z = grid_z;
	this->x = float(grid_x * SIZE);
	this->z = float(grid_z * SIZE);
	std::cout << "generating at: " << this->x << " " << this->z << std::endl;
	this->VERTEX_COUNT = 32;
	this->offset_x = grid_x * VERTEX_COUNT;
	this->offset_z = grid_z * VERTEX_COUNT;
	std::cout << "offsets: " << this->offset_x << " " << this->offset_z << std::endl;
	int comp;
	this->texture_data.grass_pixels = stbi_load("assets/terrain/grass.png", &this->texture_data.grass_x, &this->texture_data.grass_y, &comp, 4);
	this->texture_data.rock_pixels = stbi_load("assets/terrain/rock.tga", &this->texture_data.rock_x, &this->texture_data.rock_y, &comp, 4);
	GenerateHeights();
}


Terrain::~Terrain()
{
}
