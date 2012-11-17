#include "Ogre_Mesh.h"
#include "Ogre.h"

using namespace Ogre;

static unsigned int meshIndex = 0;

Ogre_Mesh::Ogre_Mesh(Ogre::SceneManager *sm)
{
	String meshName = "OgreMesh" + StringConverter::toString(meshIndex++);
	_ogreMesh = MeshManager::getSingleton().createManual(meshName, "General");
	_sceneMgr = sm;
}

Ogre_Mesh::~Ogre_Mesh()
{
	MeshManager::getSingleton().remove(_ogreMesh->getHandle());
}

void Ogre_Mesh::Render()
{
}

void Ogre_Mesh::Compile()
{
	HardwareBufferManager &hbm = HardwareBufferManager::getSingleton();
	size_t vertexCnt = _cube.size() * 24;

	for (std::vector<quad_strip>::iterator qsi = _quad_strip.begin(); qsi < _quad_strip.end(); ++qsi)
	{
		size_t nFaces = qsi->index_list.size() - 2;
		vertexCnt += nFaces * 3;
	}

	for (std::vector<fan>::iterator fi = _fan.begin(); fi < _fan.end(); ++fi)
	{
		vertexCnt += fi->index_list.size();
	}

	_ogreMesh->sharedVertexData = new VertexData();
	//_ogreMesh->sharedVertexData->vertexCount = _vertex.size();
	_ogreMesh->sharedVertexData->vertexCount = vertexCnt;

	VertexDeclaration* decl = _ogreMesh->sharedVertexData->vertexDeclaration;
	size_t offset = 0;
	const size_t vertex_size = 8;

	decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
	offset += VertexElement::getTypeSize(VET_FLOAT2);

	std::vector<GLvertex>::iterator vi = _vertex.begin();
	
	float minx, miny, minz;
	float maxx, maxy, maxz;
	minx = maxx = vi->position.x;
	miny = maxy = vi->position.y;
	minz = maxz = vi->position.z;

	while(vi != _vertex.end())
	{
		//memcpy(p, &vi->position.x, sizeof(float) * 5);
		if (minx > vi->position.x)
			minx = vi->position.x;
		if (miny > vi->position.y)
			miny = vi->position.y;
		if (minz > vi->position.z)
			minz = vi->position.z;
		if (maxx < vi->position.x)
			maxx = vi->position.x;
		if (maxy < vi->position.y)
			maxy = vi->position.y;
		if (maxz < vi->position.z)
			maxz = vi->position.z;
		//p += 5;
		++vi;
	}

	HardwareVertexBufferSharedPtr vbuf = hbm.createVertexBuffer(
		offset, vertexCnt, HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	float *vp = static_cast<float *>(vbuf->lock(HardwareBuffer::HBL_NORMAL));
	offset = 0;

	// process quad strips
	for (std::vector<quad_strip>::iterator qsi = _quad_strip.begin(); qsi < _quad_strip.end(); ++qsi)
	{
		size_t base_offset = offset/vertex_size;
		size_t nFaces = qsi->index_list.size() - 2;

		for(size_t face = 0; face < nFaces; face++)
		{
			Ogre::Vector3 v[3];
			v[0] = Ogre::Vector3(&_vertex[qsi->index_list[face+0]].position.x);
			if (face & 1)
			{
				v[1] = Ogre::Vector3(&_vertex[qsi->index_list[face+2]].position.x);
				v[2] = Ogre::Vector3(&_vertex[qsi->index_list[face+1]].position.x);
			}
			else
			{
				v[1] = Ogre::Vector3(&_vertex[qsi->index_list[face+1]].position.x);
				v[2] = Ogre::Vector3(&_vertex[qsi->index_list[face+2]].position.x);
			}
			
			Ogre::Vector3 vn = (v[1]-v[0]).crossProduct(v[2]-v[0]).normalisedCopy();

			for(size_t vi = 0; vi < 3; vi++, offset += vertex_size)
			{
				// position
				vp[offset+0] = v[vi].x;
				vp[offset+1] = v[vi].y;
				vp[offset+2] = v[vi].z;
				// normal
				vp[offset+3] = vn.x;
				vp[offset+4] = vn.y;
				vp[offset+5] = vn.z;
				// uv - not process yet
				vp[offset+6] = 0;
				vp[offset+7] = 0;
			}
		}

		size_t ibufCount = nFaces * 3;

		HardwareIndexBufferSharedPtr ibuf = hbm.createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT, 
			ibufCount, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		ushort *ip = static_cast<ushort *>(ibuf->lock(HardwareBuffer::HBL_NORMAL));

		for(size_t j = 0; j < ibufCount; j += 3, base_offset += 3)
		{
			ip[j+0]  = (ushort)base_offset + 0;
			ip[j+1]  = (ushort)base_offset + 1;
			ip[j+2]  = (ushort)base_offset + 2;
		}

		ibuf->unlock();

		/// Set parameters of the submesh
		SubMesh* sub = _ogreMesh->createSubMesh();
		sub->useSharedVertices = true;
		sub->operationType = RenderOperation::OT_TRIANGLE_LIST;
		sub->indexData->indexBuffer = ibuf;
		sub->indexData->indexCount = ibufCount;
		sub->indexData->indexStart = 0;
	}

	for (std::vector<cube>::iterator ci = _cube.begin(); ci < _cube.end(); ++ci)
	{
		size_t base_offset = offset/vertex_size;
		Ogre::Vector3 v[4];
		Ogre::Vector3 vn;

		// side walls
		for(size_t j = 0; j < 4; j++)
		{
			v[0] = Ogre::Vector3(&_vertex[ci->index_list[j*2+0]].position.x);
			v[1] = Ogre::Vector3(&_vertex[ci->index_list[j*2+1]].position.x);
			v[2] = Ogre::Vector3(&_vertex[ci->index_list[j*2+2]].position.x);
			v[3] = Ogre::Vector3(&_vertex[ci->index_list[j*2+3]].position.x);
			vn = (v[1]-v[0]).crossProduct(v[2]-v[0]).normalisedCopy();

			for(size_t k = 0; k < 4; k++, offset += vertex_size)
			{
				// position
				vp[offset+0] = v[k].x;
				vp[offset+1] = v[k].y;
				vp[offset+2] = v[k].z;
				// normal
				vp[offset+3] = vn.x;
				vp[offset+4] = vn.y;
				vp[offset+5] = vn.z;
				// uv - not process yet
				vp[offset+6] = 0;
				vp[offset+7] = 0;
			}
		}
		// bottom
		v[0] = Ogre::Vector3(&_vertex[ci->index_list[0]].position.x);
		v[1] = Ogre::Vector3(&_vertex[ci->index_list[2]].position.x);
		v[2] = Ogre::Vector3(&_vertex[ci->index_list[6]].position.x);
		v[3] = Ogre::Vector3(&_vertex[ci->index_list[4]].position.x);
		vn = (v[1]-v[0]).crossProduct(v[2]-v[0]).normalisedCopy();
		for(size_t k = 0; k < 4; k++, offset += vertex_size)
		{
			// position
			vp[offset+0] = v[k].x;
			vp[offset+1] = v[k].y;
			vp[offset+2] = v[k].z;
			// normal
			vp[offset+3] = vn.x;
			vp[offset+4] = vn.y;
			vp[offset+5] = vn.z;
			// uv - not process yet
			vp[offset+6] = 0;
			vp[offset+7] = 0;
		}
		// top
		v[0] = Ogre::Vector3(&_vertex[ci->index_list[1]].position.x);
		v[1] = Ogre::Vector3(&_vertex[ci->index_list[7]].position.x);
		v[2] = Ogre::Vector3(&_vertex[ci->index_list[3]].position.x);
		v[3] = Ogre::Vector3(&_vertex[ci->index_list[5]].position.x);
		vn = (v[1]-v[0]).crossProduct(v[2]-v[0]).normalisedCopy();
		for(size_t k = 0; k < 4; k++, offset += vertex_size)
		{
			// position
			vp[offset+0] = v[k].x;
			vp[offset+1] = v[k].y;
			vp[offset+2] = v[k].z;
			// normal
			vp[offset+3] = vn.x;
			vp[offset+4] = vn.y;
			vp[offset+5] = vn.z;
			// uv - not process yet
			vp[offset+6] = 0;
			vp[offset+7] = 0;
		}

		size_t ibufCount = (ci->index_list.size() + 2) * 3;

		HardwareIndexBufferSharedPtr ibuf = hbm.createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT, 
			ibufCount, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		ushort *ip = static_cast<ushort *>(ibuf->lock(HardwareBuffer::HBL_NORMAL));

		for(size_t j = 0; j < ibufCount; j += 6, base_offset += 4)
		{
			ip[j+0]  = (ushort)base_offset + 0;
			ip[j+1]  = (ushort)base_offset + 1;
			ip[j+2]  = (ushort)base_offset + 3;

			ip[j+3]  = (ushort)base_offset + 0;
			ip[j+4]  = (ushort)base_offset + 3;
			ip[j+5]  = (ushort)base_offset + 2;
		}

		ibuf->unlock();

		/// Set parameters of the submesh
		SubMesh* sub = _ogreMesh->createSubMesh();
		sub->useSharedVertices = true;
		sub->operationType = RenderOperation::OT_TRIANGLE_LIST;
		sub->indexData->indexBuffer = ibuf;
		sub->indexData->indexCount = ibufCount;
		sub->indexData->indexStart = 0;
	}

	for (std::vector<fan>::iterator fi = _fan.begin(); fi < _fan.end(); ++fi)
	{
		size_t base_offset = offset/vertex_size;
		Ogre::Vector3 v[3];
		Ogre::Vector3 vn;
		size_t vCnt = fi->index_list.size();

		// assuming the fan has the same normal
		v[0] = Ogre::Vector3(&_vertex[fi->index_list[0]].position.x);
		v[1] = Ogre::Vector3(&_vertex[fi->index_list[1]].position.x);
		v[2] = Ogre::Vector3(&_vertex[fi->index_list[2]].position.x);
		vn = (v[1]-v[0]).crossProduct(v[2]-v[0]).normalisedCopy();

		for(size_t j = 0; j < vCnt; j++, offset += vertex_size)
		{
			// position
			const GLvector3 &pos = _vertex[fi->index_list[j]].position;
			vp[offset+0] = pos.x;
			vp[offset+1] = pos.y;
			vp[offset+2] = pos.z;
			// normal
			vp[offset+3] = vn.x;
			vp[offset+4] = vn.y;
			vp[offset+5] = vn.z;
			// uv - not process yet
			vp[offset+6] = 0;
			vp[offset+7] = 0;
		}

		size_t ibufCount = vCnt;

		HardwareIndexBufferSharedPtr ibuf = hbm.createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT, 
			ibufCount, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		ushort *ip = static_cast<ushort *>(ibuf->lock(HardwareBuffer::HBL_NORMAL));
		for(size_t j = 0; j < vCnt; j++)
		{
			*ip++ = (ushort)base_offset + j;
		}
		ibuf->unlock();

		/// Set parameters of the submesh
		SubMesh* sub = _ogreMesh->createSubMesh();
		sub->useSharedVertices = true;
		sub->operationType = RenderOperation::OT_TRIANGLE_FAN;
		sub->indexData->indexBuffer = ibuf;
		sub->indexData->indexCount = ibufCount;
		sub->indexData->indexStart = 0;
	}
	vbuf->unlock();

	VertexBufferBinding* bind = _ogreMesh->sharedVertexData->vertexBufferBinding; 
	bind->setBinding(0, vbuf);

	/// Set bounding information (for culling)
	_ogreMesh->_setBounds(AxisAlignedBox(minx, miny, minz, maxx, maxy, maxz));
	_ogreMesh->_setBoundingSphereRadius(Math::Sqrt((maxx-minx)*(maxx-minx) + (maxy-miny)*(maxy-miny) + (maxz-minz)*(maxz-minz)));

	/// Notify Mesh object that it has been loaded
	_ogreMesh->load();

	Entity *ent = _sceneMgr->createEntity(_ogreMesh->getName());
	ent->setMaterialName("Building_Material");
	_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
}
