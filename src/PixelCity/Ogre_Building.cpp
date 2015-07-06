#include "Ogre_Building.h"
#include "Ogre_Mesh.h"

void Ogre_Building::Init()
{
	CBuilding::Init();

	// Save the data into .obj
	static int _n_building = 0;
	char filename[200];

	if (_n_building > 5)
		return;

	sprintf(filename, "Building_%d.obj", _n_building++);
	FILE* fp = fopen(filename, "w");
	if (!fp)
		return;

	Ogre_Mesh* ogreMesh = static_cast<Ogre_Mesh *>(_mesh);
	Ogre_Mesh* ogreMeshFlat = static_cast<Ogre_Mesh *>(_mesh_flat);

	const GLbbox& aabb = ogreMesh->GetAABB();
	const GLbbox& aabb_flat = ogreMeshFlat->GetAABB();
	GLbbox new_aabb;
	new_aabb.max.x = fmaxf(aabb.max.x, aabb_flat.max.x);
	new_aabb.max.y = fmaxf(aabb.max.y, aabb_flat.max.y);
	new_aabb.max.z = fmaxf(aabb.max.z, aabb_flat.max.z);
	new_aabb.min.x = fminf(aabb.min.x, aabb_flat.min.x);
	new_aabb.min.y = fminf(aabb.min.y, aabb_flat.min.y);
	new_aabb.min.z = fminf(aabb.min.z, aabb_flat.min.z);

	// merge 2 mesh info
	fprintf(fp, "# Mesh Info - %d vertices\n", (int)ogreMesh->GetOgreVertexCount());
	ogreMesh->SaveObj(fp, 0, new_aabb);
	fprintf(fp, "# Mesh Flat Info - %d vertices\n", (int)ogreMeshFlat->GetOgreVertexCount());
	ogreMeshFlat->SaveObj(fp, ogreMesh->GetOgreVertexCount(), new_aabb);

	fclose(fp);
}

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