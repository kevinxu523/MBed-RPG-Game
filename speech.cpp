#include "speech.h"
#include "graphics.h"
#include "globals.h"
#include "hardware.h"

/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.rectangle(0, 93, 127, 110, WHITE);
    uLCD.filled_rectangle(1, 94, 126, 109, BLACK);
}

void erase_speech_bubble()
{
    uLCD.filled_rectangle(0, 93, 127, 110, BLACK);
    draw_border();
}

void draw_speech_line(const char* line, int which)
{
    uLCD.text_bold(TEXTBOLD);
    uLCD.text_string((char*) line, 1, 12 + which, FONT_8X8, 0xFFFF00);
}

void speech_bubble_wait()
{
     GameInputs in;

    Timer c; c.start();

    
    do {
        Timer t; t.start();

        int wait = c.read_ms();
        if (wait > 400) {
            uLCD.filled_circle(120, 114, 3, BLUE);
            c.reset();
        }
        else if (wait > 800)
            uLCD.filled_circle(120, 114, 3, BLUE);

        in = read_inputs();

        t.stop();
        int time = t.read_ms();
        if (time < 100) wait_ms(400 - time);
    }while(in.b1);
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}


