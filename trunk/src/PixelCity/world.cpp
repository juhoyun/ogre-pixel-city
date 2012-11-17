#include <OgreSceneManager.h>
#include <string.h>
#include <cstdlib>

#include "world.h"
#include "Ogre_Building.h"
#include "Random.h"
#include "Macro.h"
#include "glTypes.h"

//#include <OgreSceneManager.h>


struct plot
{
	int             x;
	int             z;
	int             width;
	int             depth;
};

struct HSL
{
	float     hue;
	float     sat;
	float     lum;
};

static HSL            light_colors[] = 
{ 
	0.04f,  0.9f,  0.93f,   //Amber / pink
	0.055f, 0.95f, 0.93f,   //Slightly brighter amber 
	0.08f,  0.7f,  0.93f,   //Very pale amber
	0.07f,  0.9f,  0.93f,   //Very pale orange
	0.1f,   0.9f,  0.85f,   //Peach
	0.13f,  0.9f,  0.93f,   //Pale Yellow
	0.15f,  0.9f,  0.93f,   //Yellow
	0.17f,  1.0f,  0.85f,   //Saturated Yellow
	0.55f,  0.9f,  0.93f,   //Cyan
	0.55f,  0.9f,  0.93f,   //Cyan - pale, almost white
	0.6f,   0.9f,  0.93f,   //Pale blue
	0.65f,  0.9f,  0.93f,   //Pale Blue II, The Palening
	0.65f,  0.4f,  0.99f,   //Pure white. Bo-ring.
	0.65f,  0.0f,  0.8f,    //Dimmer white.
	0.65f,  0.0f,  0.6f,    //Dimmest white.
};

static char           world[WORLD_SIZE][WORLD_SIZE];
static int            modern_count;
static int            tower_count;
static int            blocky_count;
static int            skyscrapers;
static GLbbox         hot_zone;

#define HUE_COUNT         (sizeof(hue_list)/sizeof(float))
#define LIGHT_COLOR_COUNT (sizeof(light_colors)/sizeof(HSL))

/*-----------------------------------------------------------------------------

  This will return a random color which is suitible for light sources, taken
  from a narrow group of hues. (Yellows, oranges, blues.)

-----------------------------------------------------------------------------*/

GLrgba WorldLightColor (unsigned index)
{
	index %= LIGHT_COLOR_COUNT;
	return glRgbaFromHsl (light_colors[index].hue, light_colors[index].sat, light_colors[index].lum);
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static void claim (int x, int y, int width, int depth, int val)
{
	int     xx, yy;

	for (xx = x; xx < (x + width); xx++) {
		for (yy = y; yy < (y + depth); yy++) {
			world[CLAMP (xx,0,WORLD_SIZE - 1)][CLAMP (yy,0,WORLD_SIZE - 1)] |= val;
		}
	}
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static bool claimed (int x, int y, int width, int depth)
{
	int     xx, yy;

	for (xx = x; xx < x + width; xx++) {
		for (yy = y; yy < y + depth; yy++) {
			if (world[CLAMP (xx,0,WORLD_SIZE - 1)][CLAMP (yy,0,WORLD_SIZE - 1)])
				return true;
		}
	}
	return false;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static void build_road (int x1, int y1, int width, int depth)
{
	int       lanes;
	int       divider;
	int       sidewalk;

	//the given rectangle defines a street and its sidewalk. See which way it goes.
	if (width > depth) 
		lanes = depth;
	else
		lanes = width;
	//if we dont have room for both lanes and sidewalk, abort
	if (lanes < 4)
		return;
	//if we have an odd number of lanes, give the extra to a divider.
	if (lanes % 2) {
		lanes--;
		divider = 1;
	} else
		divider = 0;
	//no more than 10 traffic lanes, give the rest to sidewalks
	sidewalk = MAX (2, (lanes - 10));
	lanes -= sidewalk;
	sidewalk /= 2;
	//take the remaining space and give half to each direction
	lanes /= 2;
	//Mark the entire rectangle as used
	claim (x1, y1, width, depth, CLAIM_WALK);
	//now place the directional roads
	if (width > depth) {
		claim (x1, y1 + sidewalk, width, lanes, CLAIM_ROAD | MAP_ROAD_WEST);
		claim (x1, y1 + sidewalk + lanes + divider, width, lanes, CLAIM_ROAD | MAP_ROAD_EAST);
	} else {
		claim (x1 + sidewalk, y1, lanes, depth, CLAIM_ROAD | MAP_ROAD_SOUTH);
		claim (x1 + sidewalk + lanes + divider, y1, lanes, depth, CLAIM_ROAD | MAP_ROAD_NORTH);
	}
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static plot find_plot (int x, int z)
{
	plot      p;
	int       x1, x2, z1, z2;

	//We've been given the location of an open bit of land, but we have no 
	//idea how big it is. Find the boundary.
	x1 = x2 = x;
	while (!claimed (x1 - 1, z, 1, 1) && x1 > 0)
		x1--;
	while (!claimed (x2 + 1, z, 1, 1) && x2 < WORLD_SIZE)
		x2++;
	z1 = z2 = z;
	while (!claimed (x, z1 - 1, 1, 1) && z1 > 0)
		z1--;
	while (!claimed (x, z2 + 1, 1, 1) && z2 < WORLD_SIZE)
		z2++;
	p.width = (x2 - x1);
	p.depth = (z2 - z1);
	p.x = x1;
	p.z = z1;
	return p;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static plot make_plot (int x, int z, int width, int depth)
{
	plot      p = {x, z, width, depth};
	return p;
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static void do_building (plot p, Ogre::SceneManager *sm)
{
	int     height;
	int     seed;
	int     area;
	int     type;
	GLrgba  color;
	bool    square;

	//now we know how big the rectangle plot is. 
	area = p.width * p.depth;
	color = WorldLightColor (RandomVal ());
	seed = RandomVal ();
	//Make sure the plot is big enough for a building
	if (p.width < 10 || p.depth < 10)
		return;
	//If the area is too big for one building, sub-divide it.

	if (area > 800) {
		if (COIN_FLIP) {
			p.width /= 2;
			if (COIN_FLIP)
				do_building (make_plot (p.x, p.z, p.width, p.depth), sm);
			else
				do_building (make_plot (p.x + p.width, p.z, p.width, p.depth), sm);
			return;
		} else {
			p.depth /= 2;
			if (COIN_FLIP)
				do_building (make_plot (p.x, p.z, p.width, p.depth), sm);
			else
				do_building (make_plot (p.x, p.z + p.depth, p.width, p.depth), sm);
			return;
		}
	}
	if (area < 100)
		return;
	//The plot is "square" if width & depth are close
	square = std::abs (p.width - p.depth) < 10;
	//mark the land as used so other buildings don't appear here, even if we don't use it all.
	claim (p.x, p.z, p.width, p.depth, CLAIM_BUILDING);

	//The roundy mod buildings look best on square plots.
	if (square && p.width > 20) {
		height = 45 + RandomVal (10);
		modern_count++;
		skyscrapers++;
		CBuilding *building = new Ogre_Building(sm, BUILDING_MODERN, p.x - WORLD_HALF, p.z - WORLD_HALF, height, p.width, p.depth, seed, color);
		building->Init();
		return;
	}
	/*
	//Rectangular plots are a good place for Blocky style buildsing to sprawl blockily.
	if (p.width > p.depth * 2 || p.depth > p.width * 2 && area > 800) {
	height = 20 + RandomVal (10);
	blocky_count++;
	skyscrapers++;
	new Ogre_Building(sm, BUILDING_BLOCKY, p.x, p.z, height, p.width, p.depth, seed, color);
	return;
	}
	*/
	//tower_count = -1;
	//This spot isn't ideal for any particular building, but try to keep a good mix
	if (tower_count < modern_count && tower_count < blocky_count) {
		type = BUILDING_TOWER;
		tower_count++;
	} else if (blocky_count < modern_count) {
		type = BUILDING_BLOCKY;
		blocky_count++;
	} else {
		type = BUILDING_MODERN;
		modern_count++;
	}
	height = 45 + RandomVal (10);
	CBuilding *building = new Ogre_Building(sm, type, p.x - WORLD_HALF, p.z - WORLD_HALF, height, p.width, p.depth, seed, color);
	building->Init();
	skyscrapers++;
}

static void do_reset (Ogre::SceneManager *sm)
{
	int       x, y;
	int       width, depth, height;
	int       attempts;
	bool      broadway_done;
	GLrgba    building_color;
	float     west_street, north_street, east_street, south_street;

	//RandomInit (6);
	broadway_done = false;
	skyscrapers = 0;
	tower_count = blocky_count = modern_count = 0;
	hot_zone = glBboxClear ();
	memset(world, 0, WORLD_SIZE * WORLD_SIZE);

	for (y = WORLD_EDGE; y < WORLD_SIZE - WORLD_EDGE; y += RandomVal (25) + 25) {
		if (!broadway_done && y > WORLD_HALF - 20) {
			build_road (0, y, WORLD_SIZE, 19);
			y += 20;
			broadway_done = true;
		} else {
			depth = 6 + RandomVal (6);
			if (y < WORLD_HALF / 2)
				north_street = (float)(y + depth / 2);
			if (y < (WORLD_SIZE - WORLD_HALF / 2))
				south_street = (float)(y + depth / 2);
			build_road (0, y, WORLD_SIZE, depth);
		}
	}

	broadway_done = false;
	for (x = WORLD_EDGE; x < WORLD_SIZE - WORLD_EDGE; x += RandomVal (25) + 25) {
		if (!broadway_done && x > WORLD_HALF - 20) {
			build_road (x, 0, 19, WORLD_SIZE);
			x += 20;
			broadway_done = true;
		} else {
			width = 6 + RandomVal (6);
			if (x <= WORLD_HALF / 2)
				west_street = (float)(x + width / 2);
			if (x <= WORLD_HALF + WORLD_HALF / 2)
				east_street = (float)(x + width / 2);
			build_road (x, 0, width, WORLD_SIZE);
		}
	}

	//We kept track of the positions of streets that will outline the high-detail hot zone 
	//in the middle of the world.  Save this in a bounding box so that later we can 
	//have the camera fly around without clipping through buildings.
	hot_zone = glBboxContainPoint (hot_zone, glVector (west_street, 0.0f, north_street)); 
	hot_zone = glBboxContainPoint (hot_zone, glVector (east_street, 0.0f, south_street));

	//Scan over the center area of the map and place the big buildings 
	attempts = 0;
	while (skyscrapers < 50 && attempts < 350) {
		x = (WORLD_HALF / 2) + (RandomVal () % WORLD_HALF);
		y = (WORLD_HALF / 2) + (RandomVal () % WORLD_HALF);
		if (!claimed (x, y, 1, 1)) {
			do_building (find_plot (x, y), sm);
			skyscrapers++;
		}
		attempts++;
	}

	//now blanket the rest of the world with lesser buildings
	for (x = 0; x < WORLD_SIZE; x ++) {
		for (y = 0; y < WORLD_SIZE; y ++) {
			if (world[CLAMP (x,0,WORLD_SIZE)][CLAMP (y,0,WORLD_SIZE)])
				continue;
			width = 12 + RandomVal (20);
			depth = 12 + RandomVal (20);
			height = MIN (width, depth);
			if (x < 30 || y < 30 || x > WORLD_SIZE - 30 || y > WORLD_SIZE - 30)
				height = RandomVal (15) + 20;
			else if (x < WORLD_HALF / 2)
				height /= 2;
			while (width > 8 && depth > 8) {
				if (!claimed (x, y, width, depth)) {
					claim (x, y, width, depth, CLAIM_BUILDING);
					//building_color = WorldLightColor (RandomVal ());
					building_color = glRgba(1, 1, 1);
					//if we're out of the hot zone, use simple buildings
					if (x < hot_zone.min.x || x > hot_zone.max.x || y < hot_zone.min.z || y > hot_zone.max.z) {
						height = 5 + RandomVal (height) + RandomVal (height);
						CBuilding *building = new Ogre_Building(sm, BUILDING_SIMPLE, x + 1 - WORLD_HALF, y + 1 - WORLD_HALF, height, width - 2, depth - 2, RandomVal (), building_color);
						building->Init();
					} else { //use fancy buildings.
						height = 15 + RandomVal (15);
						width -=2;
						depth -=2;
						CBuilding *building;
						if (COIN_FLIP) 
							building = new Ogre_Building(sm, BUILDING_TOWER, x + 1 - WORLD_HALF, y + 1 - WORLD_HALF, height, width, depth, RandomVal (), building_color);
						else
							building = new Ogre_Building(sm, BUILDING_BLOCKY, x + 1 - WORLD_HALF, y + 1 - WORLD_HALF, height, width, depth, RandomVal (), building_color);
						building->Init();
					}
					break;
				}
				width--;
				depth--;
			}
			//leave big gaps near the edge of the map, no need to pack detail there.
			if (y < WORLD_EDGE || y > WORLD_SIZE - WORLD_EDGE) 
				y += 32;
		}
		//leave big gaps near the edge of the map
		if (x < WORLD_EDGE || x > WORLD_SIZE - WORLD_EDGE) 
			x += 28;
	}
}

void WorldReset (Ogre::SceneManager* sm)
{
	do_reset(sm);
}