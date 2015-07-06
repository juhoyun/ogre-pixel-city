#ifndef _OGRE_MESH_H_
#define _OGRE_MESH_H_

#include "Mesh.h"
#include "OgreMesh.h"

class Ogre::SceneManager;

class Ogre_Mesh : public CMesh
{
private:
	Ogre::MeshPtr _ogreMesh;
	Ogre::SceneManager *_sceneMgr;
public:
	Ogre_Mesh(Ogre::SceneManager *sm);
	~Ogre_Mesh();

	virtual void Render() override;
	virtual void Compile() override;
};

#endif