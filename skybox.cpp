#include "skybox.h"
#include "preamble.glsl"
#include "stb_image.h"



GLuint LoadCubeMap() {
	std::string files[] = { "right", "left", "top", "bottom", "back", "front" };
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	glActiveTexture(GL_TEXTURE0 + SKYBOX_CUBE_MAP_TEXTURE_BINDING);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
	int size_x, size_y, comp;
	for (int i = 0; i < 6; i++) {
		stbi_uc* pixels = stbi_load((std::string("assets/skybox/") + files[i] + std::string(".png")).c_str(), &size_x, &size_y, &comp, 4);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8_ALPHA8, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return tex_id;
}
void SkyBox::CreateMesh()
{
	Mesh newMesh;

	newMesh.Name = "sky";
	newMesh.VertexCount = (GLuint)positions.size() / 3;

	if (positions.empty()) {
		newMesh.PositionBO = 0;
	}
	else {
		GLuint newPositionBO;
		glGenBuffers(1, &newPositionBO);
		glBindBuffer(GL_ARRAY_BUFFER, newPositionBO);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(positions[0]), positions.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		newMesh.PositionBO = newPositionBO;
	}
	// Hook up VAO
	{
		GLuint newMeshVAO;
		glGenVertexArrays(1, &newMeshVAO);

		glBindVertexArray(newMeshVAO);

		if (newMesh.PositionBO)
		{
			glBindBuffer(GL_ARRAY_BUFFER, newMesh.PositionBO);
			glVertexAttribPointer(SKYBOX_POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SKYBOX_POSITION_ATTRIB_LOCATION);
		}

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
	newMaterial.tiled = false;
	newMaterial.multi_sample = false;

	DiffuseMap newDiffuseMap;
	newDiffuseMap.DiffuseMapTO = LoadCubeMap();
	uint32_t newDiffuseMapID = scene->DiffuseMaps.insert(newDiffuseMap);
	newMaterial.DiffuseMapID = newDiffuseMapID;
	uint32_t newMaterialID = scene->Materials.insert(newMaterial);

	GLDrawElementsIndirectCommand currDrawCommand;
	currDrawCommand.count = newMesh.VertexCount * 3;
	currDrawCommand.primCount = 1;
	currDrawCommand.firstIndex = 0;
	currDrawCommand.baseVertex = 0;
	currDrawCommand.baseInstance = 0;

	newMesh.DrawCommands.push_back(currDrawCommand);
	newMesh.MaterialIDs.push_back(newMaterialID);

	uint32_t newMeshID = scene->Meshes.insert(newMesh);
	this->mesh_id = newMeshID;
}

SkyBox::SkyBox()
{
}

uint32_t SkyBox::MeshID()
{
	return this->mesh_id;
}

SkyBox::SkyBox(Scene* scene)
{
	this->scene = scene;
	this->SIZE = 300;
	float positions[] = {
		-SIZE, SIZE, -SIZE,
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, SIZE, -SIZE,
		-SIZE, SIZE, -SIZE,

		-SIZE, -SIZE, SIZE,
		-SIZE, -SIZE, -SIZE,
		-SIZE, SIZE, -SIZE,
		-SIZE, SIZE, -SIZE,
		-SIZE, SIZE, SIZE,
		-SIZE, -SIZE, SIZE,

		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, SIZE,
		SIZE, SIZE, SIZE,
		SIZE, SIZE, SIZE,
		SIZE, SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,

		-SIZE, -SIZE, SIZE,
		-SIZE, SIZE, SIZE,
		SIZE, SIZE, SIZE,
		SIZE, SIZE, SIZE,
		SIZE, -SIZE, SIZE,
		-SIZE, -SIZE, SIZE,

		-SIZE, SIZE, -SIZE,
		SIZE, SIZE, -SIZE,
		SIZE, SIZE, SIZE,
		SIZE, SIZE, SIZE,
		-SIZE, SIZE, SIZE,
		-SIZE, SIZE, -SIZE,

		-SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE, SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE, SIZE,
		SIZE, -SIZE, SIZE
	};

	int num_indices = 6 * 6 * 3;
	this->positions = std::vector<float>(positions, positions + num_indices);
	CreateMesh();
}


SkyBox::~SkyBox()
{
}
