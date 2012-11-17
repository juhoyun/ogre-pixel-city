#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

//Used in orienting roads and cars
enum
{
  NORTH,
  EAST,
  SOUTH,
  WEST
};

#define LOADING_SCREEN      1

//Controls the ammount of space available for buildings.  
//Other code is wrtten assuming this will be a power of two.
#define WORLD_SIZE          1024
#define WORLD_HALF          (WORLD_SIZE / 2)

#define GRID_RESOLUTION   32
#define GRID_CELL         (GRID_RESOLUTION / 2)
#define GRID_SIZE         (WORLD_SIZE / GRID_RESOLUTION)
#define WORLD_TO_GRID(x)  (int)(x / GRID_RESOLUTION)
#define GRID_TO_WORLD(x)  ((float)x * GRID_RESOLUTION)


#endif