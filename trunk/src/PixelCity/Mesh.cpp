/*-----------------------------------------------------------------------------

  Mesh.cpp

  2009 Shamus Young

-------------------------------------------------------------------------------

  This class is used to make constructing objects easier. It handles
  allocating vertex lists, polygon lists, and suchlike. 

  If you were going to implement vertex buffers, this would be the place to 
  do it.  Take away the _vertex member variable and store verts for ALL meshes
  in a common list, which could then be unloaded onto the good 'ol GPU.

-----------------------------------------------------------------------------*/

//#include <windows.h>

#include <vector>
#include "glTypes.h"
#include "Mesh.h"

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CMesh::CMesh ()
{
  _compiled = false;
  _polycount = 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CMesh::~CMesh ()
{
  _vertex.clear ();
  _fan.clear ();
  _quad_strip.clear ();
  _cube.clear ();
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::VertexAdd (const GLvertex& v)
{

  _vertex.push_back(v);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::CubeAdd (const cube& c)
{

  _cube.push_back(c);
  _polycount += 5;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::QuadStripAdd (const quad_strip& qs)
{

  _quad_strip.push_back(qs);
  _polycount += (qs.index_list.size() - 2) / 2;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CMesh::FanAdd (const fan& f)
{

  _fan.push_back(f);
  _polycount += f.index_list.size() - 2;

}