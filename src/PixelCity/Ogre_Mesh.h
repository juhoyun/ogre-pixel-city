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
	GLbbox _bbox;
	size_t _ogreVertexCount;
public:
	Ogre_Mesh(Ogre::SceneManager *sm);
	~Ogre_Mesh();

	virtual void Render() override;
	virtual void Compile() override;

	const GLbbox & GetAABB()
	{
		return _bbox;
	}

	size_t GetOgreVertexCount() { return _ogreVertexCount; }

	void SaveObj(FILE* fp, size_t index_offset, const GLbbox& aabb);
};

#endif