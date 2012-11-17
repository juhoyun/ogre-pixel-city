#include "ExampleApplication.h"
#include "Random.h"
#include "Ogre_Building.h"
#include "world.h"

using namespace Ogre;

class TestListener : public ExampleFrameListener
{
protected:

public:
    TestListener(RenderWindow* win, Camera* cam)
        : ExampleFrameListener(win, cam)
    {
	}
};

class TestApplication : public ExampleApplication
{
public:
	TestApplication(LPSTR strCmdLine)
	{
	}

	~TestApplication()
	{
		delete mPlane;
	}

protected:

	MovablePlane* mPlane;

	void AppInit()
	{
		RandomInit ((unsigned long)time(NULL));
		WorldReset(mSceneMgr);
	}

	void createFrameListener(void)
    {
		// This is where we instantiate our own frame listener
        mFrameListener = new TestListener(mWindow, mCamera);
        mRoot->addFrameListener(mFrameListener);
    }

	void createScene()
	{
		Light* gLight = mSceneMgr->createLight( "GlobalLight" );
		gLight->setType(Light::LT_DIRECTIONAL);
		gLight->setDirection(Vector3(-0.5f, -1, 0));

		// Floor plane 
		mPlane = new MovablePlane("Ground Plane");
        mPlane->normal = Vector3::UNIT_Y;
        mPlane->d = 0;
        MeshManager::getSingleton().createPlane("Myplane",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *mPlane,
            1050,1050,1,1,true,1,1,1,Vector3::UNIT_Z);
        Entity* ent = mSceneMgr->createEntity( "plane", "Myplane" );
        ent->setMaterialName("Ground_Material");
        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

		AppInit();

		mCamera->setPosition(0, 100, 100);
		mCamera->lookAt(0, 0, 0);
	}
};


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    TestApplication app(strCmdLine);

    try {
        app.go();
    } catch( Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
    }

    return 0;
}

#ifdef __cplusplus
}
#endif