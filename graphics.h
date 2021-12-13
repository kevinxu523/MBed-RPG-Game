#ifndef GRAPHICS_H
#define GRAPHICS_H


/**
 * Draws the player. This depends on the player state, so it is not a DrawFunc.
 */
void draw_player(int u, int v, int key);

/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);
void draw_mysprite();
/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
void draw_wall(int u, int v);
void draw_plant(int u, int v);

/**
 * Draw the upper status bar.
 */
void draw_upper_status(int x, int y);

/**
 * Draw the lower status bar.
 */ 
void draw_lower_status(int hp, int mana);
void draw_gameOver();
void draw_gameOverWin();
void draw_startScreen();
/**
 * Draw the border for the map.
 */
void draw_border();
void draw_npc(int u, int v);
void draw_cave(int u, int v);
void draw_dragon(int u, int v);
void draw_iceSpell(int u, int v);
void draw_fireSpell(int u , int v);
void draw_key(int u, int v);
void draw_door(int u, int v);
void draw_cash(int u, int v);
void draw_menu(int u);
void draw_spellMenu(int u);
void draw_inventoryMenu(int u);
void erase_menu();
void draw_tree(int u, int v);
void draw_bush(int u, int v);
void draw_treeEnt(int u, int v);
void draw_treeEntGate(int u, int v);
void draw_DoorOpenable(int u, int v);
void draw_largeTree(int u, int v);
void draw_largeTreeTop(int u, int v);
void draw_teleMonster(int u, int v);
void draw_noKey(int u , int v);
void draw_playerBehind(int u, int v);
#endif // GRAPHICS_H