#ifndef _WORLD_H_
#define _WORLD_H_

//The "dead zone" along the edge of the world, with super-low detail.
#define WORLD_EDGE          200
//Controls the ammount of space available for buildings.  
//Other code is wrtten assuming this will be a power of two.
#define WORLD_SIZE          1024
#define WORLD_HALF          (WORLD_SIZE / 2)


//Bitflags used to track how world space is being used.
#define CLAIM_ROAD          1
#define CLAIM_WALK          2
#define CLAIM_BUILDING      4
#define MAP_ROAD_NORTH      8
#define MAP_ROAD_SOUTH      16
#define MAP_ROAD_EAST       32
#define MAP_ROAD_WEST       64

namespace Ogre
{
	class SceneManager;
}

void      WorldReset (Ogre::SceneManager *);

#endif