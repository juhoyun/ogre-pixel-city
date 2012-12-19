#ifndef _PFXSSAO_H_
#define _PFXSSAO_H_

/*
Copyright (C) 2012 Ilija Boshkov

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* 	
	Based on the SSAO Demo by nullsquared.
	Original Thread: http://www.ogre3d.org/forums/viewtopic.php?t=42350 
*/

#include <Ogre.h>

class PFXSSAO : public Ogre::CompositorInstance::Listener, public Ogre::SceneManager::Listener
{
public:
	Ogre::SceneManager *mSceneMgr;
	Ogre::Camera *mCamera;
	Ogre::Viewport *mViewport;
	Ogre::RenderWindow *mWindow;
	Ogre::CompositorInstance *mCompositor;
	PFXSSAO(Ogre::RenderWindow* wnd, Ogre::Camera* cam)
		: mSceneMgr(0)
		, mCamera(0)
		, mViewport(0)
		, mWindow(0)
		, mCompositor(0)
	{
		mWindow = wnd;
		mCamera = cam;
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		mSceneMgr = mCamera->getSceneManager();
		mViewport = mCamera->getViewport();
		initShadows();
		initSSAO();
	}

	void initSSAO()
	{
		mCompositor = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "ssao");
		mCompositor->setEnabled(true);
		mCompositor->addListener(this);
	}

	void stopSSAO()
	{
	}

	void setEnabled(bool state)
	{
		mCompositor->setEnabled(state);
	}

	void toggle()
	{
		mCompositor->setEnabled(!mCompositor->getEnabled());
	}

	void initShadows()
	{
		mSceneMgr->setShadowTextureSelfShadow(true);
		mSceneMgr->setShadowTextureCasterMaterial("shadow_caster");
		mSceneMgr->setShadowTextureCount(4);

		mSceneMgr->setShadowTextureSize(256);
		mSceneMgr->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_RGB);
		mSceneMgr->setShadowCasterRenderBackFaces(false);

		const unsigned numShadowRTTs = mSceneMgr->getShadowTextureCount();
		for (unsigned i = 0; i < numShadowRTTs; ++i)
		{
			Ogre::TexturePtr tex = mSceneMgr->getShadowTexture(i);
			Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
			vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
			vp->setClearEveryFrame(true);
		}
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		mSceneMgr->addListener(this);
	}

	void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{
		if (pass_id != 42)
			return;

		Ogre::Vector3 farCorner = mCamera->getViewMatrix(true) * mCamera->getWorldSpaceCorners()[4];
		Ogre::Pass *pass = mat->getBestTechnique()->getPass(0);
		Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
		if (params->_findNamedConstantDefinition("farCorner"))
			params->setNamedConstant("farCorner", farCorner);

		params = pass->getFragmentProgramParameters();
		static const Ogre::Matrix4 CLIP_SPACE_TO_IMAGE_SPACE(
			0.5,    0,    0,  0.5,
			0,   -0.5,    0,  0.5,
			0,      0,    1,    0,
			0,      0,    0,    1);
		if (params->_findNamedConstantDefinition("ptMat"))
			params->setNamedConstant("ptMat", CLIP_SPACE_TO_IMAGE_SPACE * mCamera->getProjectionMatrixWithRSDepth());
		if (params->_findNamedConstantDefinition("far"))
			params->setNamedConstant("far", mCamera->getFarClipDistance());
	}

	void shadowTextureCasterPreViewProj(Ogre::Light *light, Ogre::Camera *cam, size_t)
	{
		float range = light->getAttenuationRange();
		cam->setNearClipDistance((Ogre::Real)0.01);
		cam->setFarClipDistance(99990);
	}
	void shadowTexturesUpdated(size_t) {}
	void shadowTextureReceiverPreViewProj(Ogre::Light*, Ogre::Frustum*) {}
	void preFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
	void postFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
};
#endif