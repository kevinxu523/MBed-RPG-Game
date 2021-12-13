#include "map.h"

#include "globals.h"
#include "graphics.h"
#define mapHeight 50
#define mapWidth 50
#define dungeonHeight 10
#define dungeonWidth 10
#define groveHeight 10
#define groveWidth 10
/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 * In this file you need to define how the map will be structured. IE how will
 * you put values into the map, pull them from the map. Remember a "Map" variable
 * is a hashtable plus two ints (see below) 
 * You will have more than one map variable, one will be the main map with it's own hashtable.
 * Then you'll have a second map with another hashtable
 * You should store objects into the hashtable with different properties (spells
 * etc)
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map;
static Map dungeon;
static Map grove;
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    unsigned xy_key = Y* map_width() + X;
    return xy_key;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    // TODO: Fix me!
    unsigned hashValue =key% map_width();
    return hashValue;
}

void maps_init()
{
    // TODO: Implement!    
    // Initialize hash table
    // Set width & height
    map.items = createHashTable(map_hash, mapHeight);
    map.w = mapWidth;
    map.h = mapHeight;
    dungeon.items = createHashTable(map_hash, dungeonHeight);
    dungeon.w = dungeonWidth;
    dungeon.h = dungeonHeight;
    grove.items = createHashTable(map_hash, groveHeight);
    grove.w = groveWidth;
    grove.h = groveHeight;
        
}

Map* get_active_map()
{
    if(active_map == 0)
        return &map;
        
    else if(active_map == 1)
        return &dungeon;
    else if(active_map == 2)
        return &grove;
    else
        return &map;
}

Map* set_active_map(int m)
{
    active_map = m;
    
    if(active_map == 1)
        return &dungeon;
    else if(active_map == 2)
        return &grove;
    else if(active_map == 0)
        return &map;
    
        
    return NULL;
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    return get_active_map()->w;
}

int map_height()
{
    return get_active_map()->h;
}

int map_area()
{
    return map_width() * map_height();
}

MapItem* get_north(int x, int y)
{
    if(y<=0){
        return NULL;
        }
    return (MapItem*)getItem(get_active_map()->items,XY_KEY(x,y-1));
}

MapItem* get_south(int x, int y)
{
    if(y>=get_active_map()->h){
        return NULL;
        }
    return (MapItem*)getItem(get_active_map()-> items, XY_KEY(x,y+1));
}

MapItem* get_east(int x, int y)
{
    if(x>=get_active_map()->w){
        return NULL;
        }
    return (MapItem*)getItem(get_active_map()->items, XY_KEY(x+1,y));
}

MapItem* get_west(int x, int y)
{
    if(x<=0){
        return NULL;
        }
    return (MapItem*)getItem(get_active_map()->items, XY_KEY(x-1,y));
}

MapItem* get_here(int x, int y)
{
    return (MapItem*)getItem(get_active_map()->items, XY_KEY(x,y));
}


void map_erase(int x, int y)
{
    deleteItem(get_active_map()->items,XY_KEY(x,y));
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = PLANT;
        w1->draw = draw_plant;
        w1->walkable = true;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}
void add_npc(int x, int y, int* state)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_npc;
    w1->walkable = false;
    w1->data = state;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_cave(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = cave;
    w1->draw = draw_cave;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_dragon(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = dragon;
    w1->draw = draw_dragon;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_iceSpell(int x, int y, int* direction)
{

    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = iceSpell;
    w1->draw = draw_iceSpell;
    w1->walkable = true;
    w1->data = direction;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_fireSpell(int x, int y, int* direction)
{

    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = fireSpell;
    w1->draw = draw_fireSpell;
    w1->walkable = true;
    w1->data = direction;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_key(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = game_key;
    w1->draw = draw_key;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_door(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = door;
    w1->draw = draw_door;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_cash(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = cash;
    w1->draw = draw_cash;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_nothing(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NULL;
    w1->draw = draw_nothing;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_tree(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = tree;
    w1->draw = draw_tree;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_bush(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = bush;
    w1->draw = draw_bush;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_treeEnt(int x, int y, int* hp)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = treeEnt;
    w1->draw = draw_treeEnt;
    w1->walkable = false;
    w1->data = hp;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_treeEntGate(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = treeEntGate;
    w1->draw = draw_treeEntGate;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_DoorOpenable(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = DoorOpenable;
    w1->draw = draw_DoorOpenable;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_largeTree(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = largeTree;
    w1->draw = draw_largeTree;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_largeTreeTop(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = largeTreeTop;
    w1->draw = draw_largeTreeTop;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_teleMonster(int x, int y)
{
    int array[2] = {x,y};
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = teleMonster;
    w1->draw = draw_teleMonster;
    w1->walkable = false;
    w1->data = array;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_noKey(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = game_noKey;
    w1->draw = draw_noKey;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}