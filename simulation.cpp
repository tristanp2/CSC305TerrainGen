#include "simulation.h"

#include "scene.h"

#include "terraincontainer.h"
#include "terrain.h"
#include "skybox.h"

#include "imgui.h"

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "flythrough_camera.h"

#include <glm/gtc/type_ptr.hpp>

#include <SDL.h>

void Simulation::Init(Scene* scene)
{
    mScene = scene;

    std::vector<uint32_t> loadedMeshIDs;
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/cube/cube.obj", &loadedMeshIDs);
	uint32_t childCubeID;
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
		{
			uint32_t newInstanceID;
			AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
			// scale up the cube
			uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
			scene->Transforms[newTransformID].Scale = glm::vec3(2.0f);
		}
		{
			//Add instance of cube to be made child of teapot
			AddMeshInstance(mScene, loadedMeshID, &childCubeID);
			uint32_t newTransformID = scene->Instances[childCubeID].TransformID;
			scene->Transforms[newTransformID].Translation = glm::vec3(3.0f, 1.0f, 0);
			scene->Transforms[newTransformID].RotationOrigin = -scene->Transforms[newTransformID].Translation;
			scene->Rotating.push_back(childCubeID);
		}
    }

    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/teapot/teapot.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
        // place a teapot on top of the cube
        {
            uint32_t newInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
            uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
            scene->Transforms[newTransformID].Translation += glm::vec3(0.0f, 2.0f, 0.0f);
        }

        // place a teapot on the side
        {
            uint32_t newInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
            uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
            scene->Transforms[newTransformID].Translation += glm::vec3(3.0f, 1.0f, 4.0f);
        }

        // place another teapot on the side
        {
            uint32_t newInstanceID;
            AddMeshInstance(mScene, loadedMeshID, &newInstanceID);
            uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
			uint32_t childTransformID = scene->Instances[childCubeID].TransformID;
            scene->Transforms[newTransformID].Translation += glm::vec3(3.0f, 1.0f, -4.0f);
			scene->Transforms[childTransformID].ParentID = newTransformID;
			scene->Transforms[newTransformID].RotationOrigin = -scene->Transforms[newTransformID].Translation;
			scene->Rotating.push_back(newInstanceID);
        }
    }
	{
		int start_x = -1, start_z = -1, end_x = 1, end_z = 1;
		TerrainContainer tc(start_x, start_z, end_x, end_z, scene);
		for (int i = start_x; i < end_x; i++) {
			for (int j = start_x; j < end_z; j++) {
				uint32_t newInstanceID;
				Terrain* t = tc.TerrainAt(i, j);
				uint32_t meshId = t->MeshID();
				AddMeshInstance(scene, meshId, &newInstanceID);
				uint32_t newTransformID = scene->Instances[newInstanceID].TransformID;
				scene->Transforms[newTransformID].Translation = glm::vec3(t->x, -20.0f, t->z);
			}
		}
	}
	/*/
    loadedMeshIDs.clear();
    LoadMeshesFromFile(mScene, "assets/floor/floor.obj", &loadedMeshIDs);
    for (uint32_t loadedMeshID : loadedMeshIDs)
    {
        AddMeshInstance(mScene, loadedMeshID, nullptr);
    }*/

    Camera mainCamera;
    mainCamera.Eye = glm::vec3(5.0f);
    glm::vec3 target = glm::vec3(0.0f);
    mainCamera.Look = normalize(target - mainCamera.Eye);
    mainCamera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
    mainCamera.FovY = glm::radians(70.0f);
    mScene->MainCamera = mainCamera;

	Light mainLight;
	mainLight.Position = glm::vec3(0.0f, 15.0f, -0.0f);
	mainLight.Direction = normalize(target - mainLight.Position);	//just using same target as camera for now
	mainLight.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	mainLight.FovY = glm::radians(70.0f);
	mScene->MainLight = mainLight;
}

void Simulation::HandleEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_MOUSEMOTION)
    {
        mDeltaMouseX += ev.motion.xrel;
        mDeltaMouseY += ev.motion.yrel;
    }
}

void Simulation::Update(float deltaTime)
{
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    
    int mx, my;
    Uint32 mouse = SDL_GetMouseState(&mx, &my);
	//the following was added because the mouse controls don't work well with my touchpad
	Uint8 key_check = keyboard[SDL_SCANCODE_W] | keyboard[SDL_SCANCODE_A] | keyboard[SDL_SCANCODE_S] | keyboard[SDL_SCANCODE_D]
						| keyboard[SDL_SCANCODE_SPACE] | keyboard[SDL_SCANCODE_LCTRL];
    if (((mouse & (SDL_BUTTON(SDL_BUTTON_RIGHT))) != 0) || (key_check != 0))
    {
        flythrough_camera_update(
            value_ptr(mScene->MainCamera.Eye),
            value_ptr(mScene->MainCamera.Look),
            value_ptr(mScene->MainCamera.Up),
            NULL,
            deltaTime,
            5.0f * (keyboard[SDL_SCANCODE_LSHIFT]? 3.0f: 1.0f), // eye_speed
            0.1f, // degrees_per_cursor_move
            80.0f, // max_pitch_rotation_degrees
            mDeltaMouseX, mDeltaMouseY,
            keyboard[SDL_SCANCODE_W], keyboard[SDL_SCANCODE_A], keyboard[SDL_SCANCODE_S], keyboard[SDL_SCANCODE_D],
            keyboard[SDL_SCANCODE_SPACE], keyboard[SDL_SCANCODE_LCTRL],
            0);
    }

    mDeltaMouseX = 0;
    mDeltaMouseY = 0;
	
	//update real-time rotations
	for (uint32_t currentInstanceID : mScene->Rotating) {
		glm::vec3 rot_axis(0, 1, 0);
		float rot_speed = float(M_PI) / 4;
		uint32_t transformID = mScene->Instances[currentInstanceID].TransformID;
		float angle = fmodf(2*acos(mScene->Transforms[transformID].Rotation.w) + rot_speed * deltaTime, float(2*M_PI));
		float half_angle = angle / 2.0f;
		glm::quat new_rot(cos(half_angle), rot_axis.x * sin(half_angle), rot_axis.y * sin(half_angle), rot_axis.z * sin(half_angle));

		mScene->Transforms[transformID].Rotation = new_rot;
	}
    if (ImGui::Begin("Example GUI Window"))
    {
		Light& light = mScene->MainLight;
        ImGui::Text("Mouse Pos: (%d, %d)", mx, my);
		ImGui::SliderFloat3("Light Position", value_ptr(light.Position), -40.0f, 40.0f);
    }
    ImGui::End();
}

void* Simulation::operator new(size_t sz)
{
    // zero out the memory initially, for convenience.
    void* mem = ::operator new(sz);
    memset(mem, 0, sz);
    return mem;
}
    