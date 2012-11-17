#include "Ogre_Building.h"
#include "Ogre_Mesh.h"

void Ogre_Building::init_mesh()
{
	_mesh = new Ogre_Mesh(_sceneMgr);
	_mesh_flat = new Ogre_Mesh(_sceneMgr);
}

Ogre_Building::~Ogre_Building()
{
	delete _mesh;
	delete _mesh_flat;
}

void Ogre_Building::deinit_mesh()
{
}

void Ogre_Building::Render()
{
	_mesh->Render();
}

void Ogre_Building::RenderFlat(bool colored)
{
	_mesh_flat->Render();
}