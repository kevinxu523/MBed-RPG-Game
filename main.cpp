// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"
#define START 5
#define STARTDONE 6
#define PREQUEST 1
#define QUEST    2
#define REWARD 3
#define END   4

//speaker 

#define VOLUME 0.01
#define BPM 100.0
void playNote(float frequency, float duration, float volume) {
    speaker.period(1.0/frequency);
    speaker = volume/2.0;
    wait(duration);
    speaker = 0.0;
}

// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();

/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    bool omni; //omni mode on or off
    // You can add other properties for the player here
    bool menu;
    bool visible;
    unsigned int mana;
    int hp;
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 5
#define GO_UP 4
#define GO_DOWN 6
#define OMNIMODE 7
int npc_x = 12;
int npc_y = 12;
//boss hps
static int state = START;
static int go_right = GO_RIGHT;
static int go_up = GO_UP;
static int go_down = GO_DOWN;
static int go_left = GO_LEFT;
static int direction;
static int spellCord[5][2];
static MapItem* monsters[2];
static int monsterCord[2][2];
static unsigned int spell = 1;
static int movementBuffer = 1;
static int range = 7;
static MapItem* spellCount[5];
static int spellRange[5];
static int menuState = 1;
static int menuOption = 1;
static int potions = 1;
int get_action(GameInputs inputs)
{
    // Check for button presses first
    if(!inputs.b1)
        return ACTION_BUTTON;
    else if(!inputs.b2)
        return MENU_BUTTON;
    else if(!inputs.b3)
        return OMNIMODE;
    // If x and y axes are within a certain bound, do not move
    if (inputs.ay >= 0.50) return GO_UP;
    if (inputs.ay < -0.50) return GO_DOWN;
    if (inputs.ax < -0.50)  return GO_LEFT;
    if (inputs.ax >= 0.50) return GO_RIGHT;
    return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
#define MENU      3
#define GAME_OVER_WIN 4

MapItem* proximityCheck(int x, int y, int tile)
{
    MapItem* object = NULL;
    MapItem* here  = get_here(Player.x, Player.y);
    MapItem* left  = get_west(Player.x, Player.y);
    MapItem* right = get_east(Player.x, Player.y);
    MapItem* up    = get_north(Player.x, Player.y); 
    MapItem* down  = get_south(Player.x, Player.y);
     
    if(up->type == tile)         object = up;
    else if(down->type == tile)  object = down;
    else if(right->type == tile) object = right;
    else if(left->type == tile)  object = left;
    return object;
}
void proximityCheckHp(int x, int y, int tile)
{
    MapItem* here  = get_here(Player.x, Player.y);
    MapItem* left  = get_west(Player.x, Player.y);
    MapItem* right = get_east(Player.x, Player.y);
    MapItem* up    = get_north(Player.x, Player.y); 
    MapItem* down  = get_south(Player.x, Player.y);
    MapItem* upright   = get_north(Player.x +1, Player.y);
    MapItem* upleft   = get_north(Player.x -1, Player.y);
    MapItem* botleft   = get_south(Player.x -1, Player.y);
    MapItem* botright   = get_south(Player.x +1, Player.y);
    if(up->type == tile || down->type == tile || right->type == tile || left->type == tile || upright->type == tile || upleft->type == tile || botleft->type == tile || botright->type == tile){
        Player.hp -= 10;
    }
}
//spell cast function, first part initializes item, next loop directs movement
void castSpell(int direction, int spell, int ind){
    //use spellIndex to add x and y to spellCords
    //ADD SPELLINDEX TO ADD TO SPELLCOUNT!!!
    unsigned int spellX;
    unsigned int spellY;
    //up
    if(spellCount[ind] == NULL){
        
    if(direction == GO_UP){
        //fireball or freeze
        if(spell == 1){
            add_fireSpell(Player.x, Player.y-1, &go_up);
            spellX = Player.x;
            spellY = Player.y-1;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_north(Player.x, Player.y);
        }
        else if (spell == 2){
            add_iceSpell(Player.x, Player.y  -1, &go_up);
            spellX = Player.x;
            spellY = Player.y -1;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);
        }
    }
    //down
    else if(direction == GO_DOWN){
        //fireball or freeze
        if(spell == 1){
            add_fireSpell(Player.x, Player.y + 1, &go_down);
            spellX = Player.x;
            spellY = Player.y + 1;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);
        }
        else if (spell == 2){
            add_iceSpell(Player.x, Player.y + 1, &go_down); 
            spellX = Player.x;
            spellY = Player.y + 1;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);
        }
    }
    //right
    else if(direction == GO_RIGHT){
        //fireball or freeze
        if(spell == 1){
            add_fireSpell(Player.x + 1, Player.y, &go_right);
            spellX = Player.x + 1;
            spellY = Player.y;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);
        }
        else if (spell == 2){
            add_iceSpell(Player.x + 1, Player.y , &go_right); 
            spellX = Player.x + 1;
            spellY = Player.y;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);
        }
    }
    //left
    else if(direction == GO_LEFT){
        //fireball or freeze
        if(spell == 1){
            add_fireSpell(Player.x - 1, Player.y, &go_left);
            spellX = Player.x - 1;
            spellY = Player.y;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);;
        }
        else if (spell == 2){
            add_iceSpell(Player.x - 1, Player.y , &go_left); 
            spellX = Player.x - 1;
            spellY = Player.y;
            spellCord[ind][0] = spellX;
            spellCord[ind][1] = spellY;
            spellCount[ind] = get_here(spellX, spellY);
        }
    }
}
}


//movement function will be placed in main function so it updates every time frame update
MapItem* spellMovement(MapItem* spellItem, int* location, int spellIndex){
    //inputs spell from spellCount, cords from spellcord, and index used to identify them.  erase given the cords, add based on cords.  Update cords back into the spellCord w/ same index.  Update spell into spellCount using same index.
    //add what happens w/ collisions... delete spell from spellCount and cords if it encounters a wall... if it encounters the NPC game over.
    MapItem* item = spellItem;
    int x = location[0];
    int y = location[1];
    spellRange[spellIndex]++;
    if(spellRange[spellIndex] == range){
        add_nothing(x,y);
        spellCount[spellIndex] = NULL;
        spellCord[spellIndex][0] = NULL;
        spellCord[spellIndex][1] = NULL;
        spellRange[spellIndex] = NULL;
        return NULL;
    }
    MapItem* nextItem;
    int direction = *(int*)spellItem->data;
    if(direction == go_up){
        if(spellItem->type == fireSpell){
            nextItem = get_north(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;
                spellRange[spellIndex] = NULL;    
            }
            else{
                y--;
                add_fireSpell(x, y, &go_up);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
        if (item->type == iceSpell ){
            nextItem = get_north(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;
                spellRange[spellIndex] = NULL;    
            }
            else{
                y--;
                add_iceSpell(x, y, &go_up);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
    }
    //down
    else if(direction == GO_DOWN){
        //fireball or freeze
        if(item->type == fireSpell){
            nextItem = get_south(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;
                spellRange[spellIndex] = NULL;    
            }
            else{
                y++;
                add_fireSpell(x, y, &go_down);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
        
        else if (item->type == iceSpell){
            nextItem = get_south(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;
                spellRange[spellIndex] = NULL;    
            }
            else{
                y++;
                add_iceSpell(x, y, &go_down);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
    }
    //right
    else if(direction == go_right){
        //fireball or freeze
        
        if(item->type == fireSpell){
            nextItem = get_east(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;    
            }
            else{
                x++;
                add_fireSpell(x, y, &go_right);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
        else if (item->type == iceSpell){
            nextItem = get_east(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;    
            }
            else{
                x++;
                add_iceSpell(x, y, &go_right);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
    }
    //left
    else if(direction == go_left){
        //fireball or freeze
        if(item->type == fireSpell){
            nextItem = get_west(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;    
            }
            else{
                x--;
                add_fireSpell(x, y, &go_left);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
        else if (item->type == iceSpell){
            nextItem = get_west(x,y);
            add_nothing(x,y);
            if(nextItem->type == WALL || nextItem->type == cave){
                add_nothing(x,y);
                spellCount[spellIndex] = NULL;
                spellCord[spellIndex][0] = NULL;
                spellCord[spellIndex][1] = NULL;    
            }
            else{
                x--;
                add_iceSpell(x, y, &go_left);
                spellCord[spellIndex][0] = x;
                spellCord[spellIndex][1] = y;
                spellCount[spellIndex] = get_here(x,y);
            }
        }
    }
    return item;
}
    
//slash attack
void proximitySlash(int x,int y){
        //if the proximity item is the door, we can destroy it
        MapItem* up = get_north(x, y);
        MapItem* down = get_south(x, y);
        MapItem* right = get_east(x,y);
        MapItem* left = get_west(x,y);
        if(up->type == DoorOpenable ){
            add_nothing(x,y + 1);
        }
        else if(down->type == DoorOpenable ){
            map_erase(x, y-1);
        }
        else if(right->type == DoorOpenable ){
            map_erase(x+1,y);
        }
        else if(left->type == DoorOpenable ){
            map_erase(x-1,y);
        }
}
//funciton to decide monster movement towards player
bool monsterAggro(MapItem* monster, int* cords, int x, int y, int index){
    MapItem* maybe = get_here(cords[0], cords[1]);
    //if there is no monster, we want to set it to NULL in the monster array
    if(!(maybe->type == teleMonster)){
        monsters[index] = NULL;
        return false;    
    }
    //pythagorean way to calculate distance
    int monsterX = cords[0];
    int monsterY = cords[1];
    int xDis = abs(x - monsterX);
    int yDis = abs(y - monsterY);
    double pythag = (xDis * xDis) + (yDis * yDis);
    int distance = sqrt(pythag);
    //if distance is close enough and movementBuffer (slows down monster movement) is 2, move
    if(distance <= 4 && distance >1 && movementBuffer == 2){
        add_nothing(monsterX,monsterY);
        //cases for each direction
        if(xDis >= yDis){
            if(monsterX <= x){
                monsterX++;
                monsterCord[index][0] = monsterX;
                add_teleMonster(monsterX , monsterY);
                monsters[index] = get_here(monsterX, monsterY);
            }
            else{
                monsterX--;
                monsterCord[index][0] = monsterX;
                add_teleMonster(monsterX , monsterY);
                monsters[index] = get_here(monsterX, monsterY);
            }
        }
        else if(xDis < yDis){
            if(monsterY <= y){
                monsterY++;
                monsterCord[index][1] = monsterY;
                add_teleMonster(monsterX, monsterY);
                monsters[index] = get_here(monsterX, monsterY);
            }
            else{
                monsterY--;
                monsterCord[index][1] = monsterY;
                add_teleMonster(monsterX, monsterY);
                monsters[index] = get_here(monsterX, monsterY);
            }
        }
    }
    if(movementBuffer != 2){
        movementBuffer++;    
    }
    else if(movementBuffer == 2){
        movementBuffer = 0;
    }
    return false;
}
int update_game(int action)
{
    // Save player previous location before updating
    if(Player.hp <= 0)return GAME_OVER;
    Player.px = Player.x;
    Player.py = Player.y;
    Player.visible = true;
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    MapItem* nextItem;
    MapItem* item;
    unsigned int map_num;
    
    //update spell movement if there is any, spellindex will already be 0
    for(int i = 0; i<5;i++ ){
        if(spellCount[i] != NULL){
            //spellMovement also updates spellCount and spellCord of index i
            //pray god
            spellMovement(spellCount[i], spellCord[i], i);
            
        }
    }
    //update monster movement if player is in range
    for(int k = 0; k<2;k++){
        if(monsters[k] != NULL){
            monsterAggro(monsters[k], monsterCord[k], Player.x, Player.y, k);    
        }
    }  
    //monster damage based on if it's in the proximity
    proximityCheckHp(Player.x, Player.y, teleMonster);
    switch(action){
        case GO_UP:
        {    
            nextItem = get_north(Player.x,Player.y);
            //spellMenu functionalities and upper bound
            if(Player.menu && menuState == 2 && spell != 1){
                draw_spellMenu(spell-1);
                spell = 1;
            }
            else if(Player.menu && menuState == 2 && spell == 1){
                draw_spellMenu(1);
                spell = 1;
            }
            //main menu
            if(Player.menu && menuState == 1 && menuOption != 1){
                draw_menu(menuOption-1);
                menuOption = 1;
            }
            else if(Player.menu && menuState == 1 && menuOption == 1){
                draw_menu(1);
                menuOption = 1;
            }
            //if omni, nextItem is empty, or next Item is walkable, you can walk this direction
            if(!Player.menu){
                if(Player.omni){
                    Player.y = Player.y - 1; 
                    direction = go_up;
                }
                else if(nextItem == NULL){
                    Player.y = Player.y - 1; 
                    direction = go_up;
                }
                else if(nextItem->walkable){
                    Player.y = Player.y - 1;
                    direction = go_up;
                }
                
    
            } 
            if(Player.menu){
            return MENU;
            }
            break;
        }
        case GO_LEFT:  
        {
            nextItem = get_west(Player.x,Player.y);
            //if omni, nextItem is empty, or next Item is walkable, you can walk this direction
            if(!Player.menu){
                if(Player.omni){
                    Player.x = Player.x - 1;
                    direction = go_left;   
                } 
                else if(nextItem == NULL){
                    Player.x = Player.x - 1;
                    direction = go_left; 
                } 
                else if(nextItem->walkable){
                    Player.x = Player.x - 1; 
                    direction = go_left; 
                }
                
            }
            if(Player.menu){
            return MENU;
            }
            break;  
        }         
        case GO_DOWN:   
        {
            nextItem = get_south(Player.x,Player.y);
            //spellMenu navigation functionalities
            if(Player.menu && menuState == 2 && spell != 2){
                draw_spellMenu(spell+1);
                spell = spell + 1;
            }
            else if(Player.menu && menuState == 2 && spell == 2){
                draw_spellMenu(2);
                spell = 2;
            }
            //main menu
            if(Player.menu && menuState == 1 && menuOption != 2){
                draw_menu(menuOption+1);
                menuOption = menuOption + 1;
            }
            else if(Player.menu && menuState == 1 && menuOption == 2){
                draw_menu(2);
                menuOption = 2;
            }
            //if omni, nextItem is empty, or next Item is walkable, you can walk this direction
            if(!Player.menu){
                if(Player.omni){
                    Player.y = Player.y + 1; 
                    direction = go_down; 
                }
                else if(nextItem == NULL){
                    Player.y = Player.y + 1; 
                    direction = go_down; 
                }    
                else if(nextItem->walkable){
                    Player.y = Player.y + 1;
                    direction = go_down;    
                } 
             
            }
            if(Player.menu){
            return MENU;
            }
            break;
        }
        case GO_RIGHT:  
        {   
            //if omni, nextItem is empty, or next Item is walkable, you can walk this direction
            nextItem = get_east(Player.x,Player.y);
            if(!Player.menu){
                if(Player.omni){
                    Player.x = Player.x + 1; 
                    direction = go_right;    
                }
                else if(nextItem == NULL){
                    Player.x = Player.x + 1;
                    direction = go_right;    
                } 
                else if(nextItem->walkable){
                    Player.x = Player.x + 1;
                    direction = go_right;    
                } 
           
            }
            if(Player.menu){
            return MENU;
            }
            break;   
        }
        case ACTION_BUTTON: 
        {
            //key grab
            if(state == STARTDONE){
                state = PREQUEST;   
                return FULL_DRAW; 
            }
            //menu functionalities and central navigation for spell/inventory submenu
            if(Player.menu){
                if(menuState == 1){
                    if(menuOption == 1){
                        menuState = 2;
                        draw_spellMenu(spell);
                        return MENU;
                    }
                    else if(menuOption == 2){
                        menuState = 3;
                        draw_inventoryMenu(potions);
                        return MENU;
                    }
                }
                if(menuState == 3 && potions >=1){
                    potions--;
                    draw_inventoryMenu(potions);
                    Player.hp = Player.hp + 50;
                    Player.mana = Player.mana + 400;
                }
                Player.menu = false;   
                break;
            }
            //spell 3 currently no use
            if(spell == 3){
                proximitySlash(Player.x, Player.y);    
            }
            //check to see if the key is in reach, if it is take it and replace with empty table
            item = proximityCheck(Player.x, Player.y, game_key);;
            if(item->type == game_key){
                   const char* lines[] = { "yoink",
                                            "Time to go back." }; 
                   speech(lines[0], lines[1]);
                   map_erase(5,2);
                   Player.has_key = true;
                   add_noKey(5,2);
                   item = get_here(5,2);
            }
            
            //map selection
            nextItem = get_here(Player.x,Player.y);
            if(nextItem-> type == cave){
                if(map_width() == 50){
                    map_num = 1;
                }
                else{
                    map_num = 0;    
                }
                set_active_map(map_num);
                if(map_num == 1){
                    Player.x = 8;
                    Player.y = 8;
                }
                else{
                    Player.x = 10;
                    Player.y = 25;
                }
                return FULL_DRAW;
            }
            else if(nextItem-> type == treeEntGate){
                if(map_width() == 50){
                    map_num = 2;
                }
                else{
                    map_num = 0;    
                }
                set_active_map(map_num);
                if(map_num == 2){
                    Player.x = 5;
                    Player.y = 5;
                }
                else{
                    Player.x = 11;
                    Player.y = 14;
                }
                return FULL_DRAW;
            }
                
            //npc interaction for reward state
            item = proximityCheck(Player.x, Player.y, NPC);
            if(Player.has_key == true && item->type == NPC && *((int*)item->data) == REWARD){
                const char* lines[] = { "....",
                                        "..."};
                speech(lines[0], lines[1]);

               
            }
            //after quest completion, set state to reward state
            if(item->type == NPC && Player.has_key == true && *((int*)item->data) == QUEST){
                const char* lines[] = 
                { "Didn't expect to",
                "see you alive.",
                "Go South and",
                "take my treasure"};
                speech(lines[0], lines[1]);
                speech(lines[2], lines[3]);
                state = REWARD;
                item->data = &state;
                
            }
            //quest is in progress
            if(item->type == NPC && *((int*)item->data) == QUEST){
                const char* lines[] = { "Don't",
                                        "Die!"};
            
                speech(lines[0], lines[1]);
            
            }
            //prequest dialogue, also changes quest state
            if(item->type == NPC && *((int*)item->data) == PREQUEST){
                const char* lines[] = 
                {"Go west, kill ",
                "the guards.  ",
                "Go to the cave, ",
                "kill the dragon.  "};
            
                speech(lines[0], lines[1]);
                speech(lines[2], lines[3]);
                state = QUEST;
                item->data =&state;
                
            }
            
            
            //open door
            item = proximityCheck(Player.x, Player.y, door);
            if(item->type == door && Player.has_key == true){
            item->walkable = true;    
            }
            //grab cash game over
            nextItem = get_here(Player.x,Player.y);
            if(nextItem->type == cash){
                map_erase(32,12);
                return GAME_OVER_WIN;
            }
            
            MapItem* npcMaybe = proximityCheck(Player.x, Player.y, NPC);
            
            //casting spell is last priority
            if(!(item) && !npcMaybe &&  get_here(Player.x,Player.y) == NULL && (spellCount[0] == NULL || spellCount[1] == NULL || spellCount[2] == NULL || spellCount[3] == NULL || spellCount[4] == NULL)){
                //if spell is fireball and you have enough mana, cast
                if(Player.mana >= 200 && spell == 1){
                    if(spellCount[0] == NULL){
                        castSpell(direction , spell , 0);
                        Player.mana -= 200;
                        playNote(392,.1,.5);
                    }
                    else if(spellCount[1] == NULL){
                        castSpell(direction , spell , 1);
                        Player.mana -= 200;
                        playNote(392,.1,.5);
                    }
                    else if(spellCount[2] == NULL){
                        castSpell(direction , spell , 2);
                        Player.mana -= 200;
                        playNote(392,.1,.5);
                    }
                    else if(spellCount[3] == NULL){
                        castSpell(direction , spell , 3);
                        Player.mana -= 200;
                        playNote(392,.1,.5);
                    }
                    else if(spellCount[4] == NULL){
                        castSpell(direction , spell , 4);
                        Player.mana -= 200;
                        playNote(392,.1,.5);
                    }
                    
                }
                //conditions to cast freeze spell, need enough mana and must be toggled to freeze spell
                else if(Player.mana >= 100 && spell == 2){
                    if(spellCount[0] == NULL){
                        castSpell(direction , spell , 0);
                        Player.mana -= 100;
                        playNote(293.66,.1,.5);
                    }
                    else if(spellCount[1] == NULL){
                        castSpell(direction , spell , 1);
                        Player.mana -= 100;
                        playNote(293.66,.1,.5);
                    }
                    else if(spellCount[2] == NULL){
                        castSpell(direction , spell , 2);
                        Player.mana -= 100;
                        playNote(293.66,.1,.5);
                    }
                    else if(spellCount[3] == NULL){
                        castSpell(direction , spell , 3);
                        Player.mana -= 100;
                        playNote(293.66,.1,.5);
                    }
                    else if(spellCount[4] == NULL){
                        castSpell(direction , spell , 4);
                        Player.mana -= 100;
                        playNote(293.66,.1,.5);
                    }
                }   
            }
            
            break;
        }
        case MENU_BUTTON: 
        {   
            //pressing menu button toggles menu on/off
            Player.menu = !(Player.menu);
            if(Player.menu){
                menuState = 1;
                draw_menu(menuOption);
            }
            //if Player's menu is true, then return MENU to keep menu screen
            if(Player.menu){
            return MENU;
            }
            break;
        }
        case OMNIMODE: 
        {   
            //toggle omni mode on/off
            Player.omni = !(Player.omni);
            break;
        }
        default:     
        {   //if menu is on, return it, else draw everything else updated
            if(Player.menu){
                return MENU;
            }
            else{
                return FULL_DRAW;    
            }
        }
    }
    return FULL_DRAW;
}


/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init){
    // Draw game border first
    if(init){ 
    draw_border();
    };
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {   
                MapItem* item = get_here(Player.x,Player.y);
                if(item->type == largeTreeTop){
                    draw_largeTreeTop(u,v);
                }
                else if(item->type == PLANT){
                    draw_playerBehind(u,v);
                }
                else{
                    draw_player(u, v, Player.has_key);
                }
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status(Player.x, Player.y);
    draw_lower_status(Player.hp, Player.mana);
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Player.mana = 500;
    Player.hp = 100;
    Map* map = set_active_map(0);
    
    pc.printf("plants\r\n");
        
    pc.printf("Adding walls!\r\n");
    //add stuff to map 1
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    add_wall(1, 19, HORIZONTAL, 20);
    add_wall(1, 29, HORIZONTAL, 20);
    add_door(19,29);
    add_cash(19,35);
    add_DoorOpenable(19,19);;
    add_wall(20,0 , VERTICAL, 20);
    add_wall(20,29 , VERTICAL, 20);

    add_npc(20,25, &state);
    add_largeTree(17,25);
    add_largeTreeTop(17,24);
    add_teleMonster(15,22);
    monsters[0] = get_here(15,22);
    monsterCord[0][0] = 15;
    monsterCord[0][1] = 22;
    add_teleMonster(10,23);
    add_plant(10,26,1 ,1);
    monsters[1] = get_here(10,23);
    monsterCord[1][0] = 10;
    monsterCord[1][1] = 23;
    add_cave(10,25);
    pc.printf("Walls done!\r\n");

    print_map();
    //add stuff to map 2, dungeon
    Map* dungeon = set_active_map(1);
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    add_cave(8,8);
    add_dragon(5,5);
    add_key(5,2);
    

    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");


    // Initialize the maps
    maps_init();
    init_main_map();
    
    // Initialize game state
    set_active_map(0);
    Player.x = 25;
    Player.y = 24;
    Player.has_key = 0;
    GameInputs inputs;
    // Initial drawing
    draw_startScreen();
    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        // Actually do the game update:
        
        // 1. Read inputs        
        inputs = read_inputs();
        // 2. Determine action (get_action)  
        int action = get_action(inputs);      
        // 3. Update game (update_game)
        int updatedGame = update_game(action);
        
        //check start screen
        if(state == START){
            draw_startScreen();
            state = STARTDONE; 
            continue;   
        }
        else if(state == STARTDONE) continue;
        // 3b. Check for game over win/lose
        if(updatedGame == GAME_OVER){
            draw_gameOver();
            return NULL;
        }
        if(updatedGame == GAME_OVER_WIN){
            draw_gameOverWin();
            return NULL;
        }
        else if(updatedGame == MENU){
            continue;
        }
        // 4. Draw frame (draw_game)
        else{
        draw_game(updatedGame);
        }
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }

}
