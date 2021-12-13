#include "graphics.h"
#include "sprites.h"
#include "globals.h"
Timer waitTime;
/*
In this file put all your graphical functions (don't forget to declare them first
in graphics.h). So when you want to draw something use this file. One cool function
to look at would be uLCD.blit() there are more like filled_rectangle etc... 
https://os.mbed.com/users/4180_1/notebook/ulcd-144-g2-128-by-128-color-lcd/
This website is a good resource.
*/

void draw_player(int u, int v, int key)
{
    uLCD.BLIT(u , v, 11, 11, sprites[8]);
}

#define YELLOW 0xFFFF00
#define BROWN  0xD2691E
#define DIRT   BROWN
#define fireball 1
#define freeze   2
void draw_img(int u, int v, const char* img)
{
    int colors[11*11];
    for (int i = 0; i < 11*11; i++)
    {
        if (img[i] == 'R') colors[i] = RED;
        else if (img[i] == 'Y') colors[i] = YELLOW;
        else if (img[i] == 'G') colors[i] = GREEN;
        else if (img[i] == 'D') colors[i] = DIRT;
        else if (img[i] == '5') colors[i] = LGREY;
        else if (img[i] == '3') colors[i] = DGREY;
        else colors[i] = BLACK;
    }
    uLCD.BLIT(u, v, 11, 11, colors);
    wait_us(250); // Recovery time!
}

void draw_nothing(int u, int v)
{
    // Fill a tile with blackness
    uLCD.filled_rectangle(u, v, u+10, v+10, BLACK);
}

void draw_wall(int u, int v)
{
    uLCD.BLIT(u , v, 11, 11, sprites[13]);
}

void draw_plant(int u, int v)
{
    uLCD.filled_rectangle(u, v, u+10, v+10, GREEN);
}
void draw_npc(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[0]);
}

void draw_cave(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[1]);
}
void draw_dragon(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[2]);
}
void draw_iceSpell(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[3]);
}
void draw_fireSpell(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[4]);
}
void draw_key(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[5]);
}
void draw_noKey(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[18]);
}
void draw_door(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[6]);
}
void draw_cash(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[7]);
}
void draw_tree(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[9]);
}
void draw_bush(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[10]);
}
void draw_treeEnt(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[11]);
}
void draw_treeEntGate(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[12]);
}
void draw_DoorOpenable(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[14]);
}
void draw_largeTree(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[15]);
}
void draw_largeTreeTop(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[16]);
}
void draw_playerBehind(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[19]);
}
void draw_teleMonster(int u, int v)
{
     uLCD.BLIT(u , v, 11, 11, sprites[17]);
}
void draw_upper_status(int x, int y)
{
    // Draw bottom border of status bar
    uLCD.line(0, 9, 127, 9, GREEN);

    // Add other status info drawing code here
    char stringIndex[16];
    sprintf(stringIndex, "Position: %u, %u ", x, y);
    uLCD.text_string(stringIndex, 0, 0, FONT_5X7, WHITE);
}

void draw_lower_status(int hp, int mana)
{
    // Draw top border of status bar
    uLCD.line(0, 118, 127, 118, GREEN);
    
    // Add other status info drawing code here
    char stringIndex[6];
    sprintf(stringIndex, "HP:%u ", hp);
    char stringIndex2[6];
    sprintf(stringIndex2, "MP:%u", mana);
    uLCD.text_string(stringIndex, 1, 15, FONT_5X7, RED);
    uLCD.text_string(stringIndex2, 9, 15, FONT_5X7, BLUE);
}
void draw_spellMenu(int x){
    waitTime.start();
    uLCD.text_bold(TEXTBOLD);
    uLCD.rectangle(2, 79, 128, 119, WHITE);
    uLCD.filled_rectangle(3, 80, 127, 118, BLACK);
    if(x == 1){
    uLCD.text_bold(TEXTBOLD);
    uLCD.text_string("Fireball", 1, 10, FONT_5X7, YELLOW);
    uLCD.text_string("Freeze", 1, 11, FONT_5X7, WHITE);
    }
    if(x == 2){
    uLCD.text_bold(TEXTBOLD);
    uLCD.text_string("Fireball", 1, 10, FONT_5X7, WHITE);
    uLCD.text_string("Freeze", 1, 11, FONT_5X7, YELLOW);
    }
    wait_ms(300);
}
void draw_menu(int x){
    waitTime.start();
    uLCD.text_bold(TEXTBOLD);
    uLCD.rectangle(2, 79, 128, 119, WHITE);
    uLCD.filled_rectangle(3, 80, 127, 118, BLACK);
    if(x == 1){
    uLCD.text_bold(TEXTBOLD);
    uLCD.text_string("Spells", 1, 10, FONT_5X7, YELLOW);
    uLCD.text_string("Inventory", 1, 11, FONT_5X7, WHITE);
    }
    if(x == 2){
    uLCD.text_bold(TEXTBOLD);
    uLCD.text_string("Spells", 1, 10, FONT_5X7, WHITE);
    uLCD.text_string("Inventory", 1, 11, FONT_5X7, YELLOW);
    }
    wait_ms(300);
}
void draw_inventoryMenu(int x){
    waitTime.start();
    uLCD.text_bold(TEXTBOLD);
    uLCD.rectangle(2, 79, 128, 119, WHITE);
    uLCD.filled_rectangle(3, 80, 127, 118, BLACK);
    uLCD.text_bold(TEXTBOLD);
    char stringIndex[16];
    sprintf(stringIndex, "Potions %u ", x);
    uLCD.text_string(stringIndex, 1, 10, FONT_5X7, YELLOW);

    wait_ms(300);
}
void erase_menu()
{
    uLCD.filled_rectangle(3, 80, 127, 118, BLACK);
    draw_border();
}

void draw_gameOver(){
    uLCD.filled_rectangle(3, 15, 126, 113, BLACK);
    uLCD.text_string("You Lose!", 6, 8, FONT_8X8, WHITE);
}
void draw_gameOverWin(){
    uLCD.filled_rectangle(3, 15, 126, 113, BLACK);
    uLCD.text_string("You Win!", 6, 8, FONT_8X8, WHITE);
}
void draw_startScreen(){
    uLCD.filled_rectangle(0, 0, 127, 130, BLACK);
    uLCD.text_string("Curse Hunt", 3, 6, FONT_8X8, RED);
    uLCD.text_string("Press", 6, 8,FONT_8X8, WHITE);
    uLCD.text_string("Start", 6, 9,FONT_8X8, WHITE);
}
void draw_border()
{
    uLCD.filled_rectangle(0,     9, 127,  14, WHITE); // Top
    uLCD.filled_rectangle(0,    13,   2, 114, WHITE); // Left
    uLCD.filled_rectangle(0,   114, 127, 117, WHITE); // Bottom
    uLCD.filled_rectangle(124,  14, 127, 117, WHITE); // Right
}


