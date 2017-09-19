#pragma once

#include "shaderset.h"

struct SDL_Window;
class Scene;

class Renderer
{
    Scene* mScene;

    // ShaderSet explanation:
    // https://nlguillemot.wordpress.com/2016/07/28/glsl-shader-live-reloading/
    ShaderSet mShaders;

    GLuint* mSceneSP;
	GLuint* mShadowSP;
	GLuint* mDepthVisSP;
	GLuint* mSkySP;

    int mBackbufferWidth;
    int mBackbufferHeight;
	bool mShowDepthVis;
    GLuint mBackbufferFBO;
    GLuint mBackbufferColorTO;
    GLuint mBackbufferDepthTO;
	GLuint mShadowDepthTO;
	GLuint mShadowFBO;
	GLuint mNullVAO;
	GLsizei kShadowMapResolution;
	GLfloat mShadowSlopeScaleBias;
	GLfloat mShadowDepthBias;

public:
    void Init(Scene* scene);
    void Resize(int width, int height);
    void Render();

    void* operator new(size_t sz);
};
