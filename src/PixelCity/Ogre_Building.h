#ifndef _OGRE_BUILDING_H
#define _OGRE_BUILDING_H

#include "Building.h"

namespace Ogre
{
	class SceneManager;
}

class Ogre_Building : public CBuilding
{
private:
	Ogre::SceneManager *_sceneMgr;
protected:
	virtual void            init_mesh() override;
	virtual void            deinit_mesh() override;
public:
	Ogre_Building(Ogre::SceneManager *sm, int type, int x, int y, int height, int width, int depth, int seed, GLrgba color) 
		: CBuilding(type, x, y, height, width, depth, seed, color)
	{
		_sceneMgr = sm;
	}

	virtual ~Ogre_Building();

	virtual void			Init() override;
	virtual void            Render() override;
	virtual void            RenderFlat(bool colored) override;
};

#endif