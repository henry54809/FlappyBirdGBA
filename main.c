#define REG_DISPCNT  *(unsigned short *) 0x4000000
#define COLOR(r,g,b) ((r) + ((g) << 5) + ((b) << 10))
#define MODE3 3
#define BG2_ENABLE (1<<10)
//Color
#define BLACK   COLOR(0,0,0)
#define WHITE   COLOR(31, 31, 31)
#define RED     COLOR(31,0,0)
#define GREEN   COLOR(0, 31, 0)
#define BLUE    COLOR(0,0,31)
#define YELLOW  COLOR(31,31,0)
#define CYAN    COLOR(0,31,31)
#define MAGENTA COLOR(31,0,31)
#define GRAY    COLOR(25, 25, 25)
//Button
#define BUTTON_ADDRESS *(volatile unsigned short*) 0x4000130
#define BUTTON_DOWN_NOW(key) (~BUTTON_ADDRESS & (key))

#define BUTTON_A 1
#define BUTTON_B (1<<1)
#define BUTTON_SELECT (1<<2)
#define BUTTON_START (1<<3)
#define BUTTON_RIGHT (1<<4)
#define BUTTON_LEFT (1<<5)
#define BUTTON_UP (1<<6)
#define BUTTON_DOWN (1<<7)
#define BUTTON_R (1<<8)
#define BUTTON_L (1<<9) 
#include "mylib.h"
#include "text.h"
#include <stdlib.h>
#include <string.h>

void updateBird( Bird *bird, Point *newOrigin );
void drawBird( Bird *bird, u16 color );
void drawWall( Wall *myWall, u16 color );
void initializeWalls();
void updateWalls();
int testCollision();
void checkScore( Bird *bird, int *score );
//Only keep 2 walls at a time
static Wall walls[2];

int main(){
    REG_DISPCNT = MODE3 | BG2_ENABLE;
    int r = 70;
    int paused = 0;
    Bird bird;
    bird.up = 1;
    bird.offsetR = 0;
    bird.offsetC = 0;
    u16 color = GREEN;
    Point origin = { .r = r, .c = 100 };
    updateBird( &bird, &origin ); 
    drawBird( &bird, RED );
    initializeWalls();
    drawWall( &walls[0], MAGENTA );
    drawWall( &walls[1], MAGENTA );
    
    int score = 0;
    while(1){
        if (BUTTON_DOWN_NOW(BUTTON_SELECT)){
            switch ( rand() % 10 ){
                case 0:
                  color = BLACK;
                  break;
                case 1:   
                  color = WHITE;
                  break;
                case 2:
                  color = RED;    
                  break;
                case 3:
                  color = GREEN;
                  break;
                case 4:
                  color = BLUE;
                  break;     
                case 5:
                  color = YELLOW;
                  break;
                case 6:
                  color = CYAN;
                  break;   
                case 7:
                  color = MAGENTA;
                  break;   
                case 8:
                  color = GRAY;
                  break;  
            }
        }    
        if(BUTTON_DOWN_NOW(BUTTON_DOWN)) {
            r++;
        }
        if(BUTTON_DOWN_NOW(BUTTON_UP)) {
            r--;
        }
        

        waitForVBlank();
        if ( r < 0 ){
            r = 0;
        }
        if ( r > 134 ){
            r = 134;
        }
        
        if ( !paused ){
            drawBird( &bird, BLACK );
            drawWall( &walls[0], BLACK );
            drawWall( &walls[1], BLACK );
            Point newOrigin = { .r = r, .c = origin.c };
            
            updateBird( &bird, &newOrigin ); 
            
            updateWalls();
            drawWall( &walls[0], MAGENTA );
            drawWall( &walls[1], MAGENTA );
            drawBird( &bird, color );
            
            if( testCollision( &bird )){
                paused = 1;
                drawRect(0, 0, 240, 160, BLACK );
                char gameover[] = "Game Over";
                drawString( 60, 40, gameover, RED );
            }
            checkScore( &bird, &score );
        }
        
        char livestext[] = "Score: ";
        char lives_amt = (char)(score + 48); // the ascii code for representing the current lives left
        char *lives_str = strcat(livestext, &lives_amt); // concatenate the str and the amount
        drawString(150, 165, lives_str, RED);
    }
}

int testCollision( Bird *bird ){
    if ( 
            (bird->nt).c >= walls[0].screenC 
         && (bird->b).c < ( walls[0].screenC + walls[0].width )
       ) {
        if ( 
             ( (bird->b).r + 10 ) >= ( walls[0].openingR + walls[0].height )
          || (bird->h).r <= ( walls[0].openingR ) 
            ){
            return 1;
        }
    } else if ( 
                    (bird->nt).c >= walls[1].screenC 
                 && (bird->b).c < ( walls[1].screenC + walls[0].width )
               ) {
                    if ( 
                         ( (bird->b).r + 10 ) >= ( walls[1].openingR + walls[0].height )
                      || (bird->h).r <= ( walls[1].openingR ) 
                        ){
                        return 1;
                    }
    }

    return 0;
}

void checkScore( Bird *bird, int *score ){
    if ( !(walls[0].passed) ){
        if ( (bird->b).c >= (walls[0].screenC + walls[0].width) ){
            *score = *score + 1;
            walls[0].passed = 1;
        }
    } else if ( !(walls[1].passed) ){
        if ( (bird->b).c >= (walls[1].screenC + walls[1].width) ){
            *score = *score + 1;
            walls[1].passed = 1;
        }
    }
}

void updateBird( Bird *bird, Point *newOrigin ){
    bird->offsetC = rand() % 2;
    static int downwards = 0;
    if ( bird->offsetR >= 10 ){
        downwards = 1;
    } else if ( bird->offsetR <= -10 ){
        downwards = 0;
    }
    
    if ( downwards ){
        bird->offsetR = bird->offsetR - 1;
    } else {
        bird->offsetR = bird->offsetR + 1;
    }

    bird->origin = *newOrigin;
     
    //Point B
    (bird->b).r = newOrigin->r + 10 + bird->offsetR;
    (bird->b).c = newOrigin->c + bird->offsetC;
    
    //Point W1
    (bird->w1).r = newOrigin->r + 5 + bird->offsetR;
    (bird->w1).c = newOrigin->c + 2 + bird->offsetC;
    
    //Point W2
    (bird->w2).r = newOrigin->r + 25 + bird->offsetR;
    (bird->w2).c = newOrigin->c + 2 + bird->offsetC;

    //Point Wb
    (bird->wb).r = newOrigin->r + 15 + bird->offsetR;
    (bird->wb).c = newOrigin->c + 10 + bird->offsetC;

    //Point H
    (bird->h).r = newOrigin->r + bird->offsetR;
    (bird->h).c = newOrigin->c + 10 + bird->offsetC;

    //Point E
    (bird->e).r = newOrigin->r + 4 + bird->offsetR;
    (bird->e).c = newOrigin->c + 16 + bird->offsetC;

    //Point N1
    (bird->n1).r = newOrigin->r + bird->offsetR;
    (bird->n1).c = newOrigin->c + 20 + bird->offsetC;
    
    //Point N2
    (bird->n2).r = newOrigin->r + 10 + bird->offsetR;
    (bird->n2).c = newOrigin->c + 20 + bird->offsetC;

    //Point Nt 
    (bird->nt).r = newOrigin->r + 5 + bird->offsetR;
    (bird->nt).c = newOrigin->c + 30 + bird->offsetC;
    
    if ( bird->up ){
        bird->up = 0;
    } else {
        bird->up = 1;
    }
}

void drawBird( Bird *bird, u16 color ){
    //Draw Head
    drawHollowRect( (bird->h).r, (bird->h).c, 10, 10, color );
    
    //Draw Eye
    drawRect( (bird->e).r, (bird->e).c, 2, 2, color );
    
    //Draw Body 
    drawHollowRect( (bird->b).r, (bird->b).c, 20, 10, color );

    //Draw Wing
    if ( bird->up ){
       plotLine( (bird->w1).r, (bird->w1).c, (bird->wb).r, (bird->wb).c, color ); 
    } else {
       plotLine( (bird->w2).r, (bird->w2).c, (bird->wb).r, (bird->wb).c, color ); 
    }

    //Draw Peak
    plotLine( (bird->n1).r, (bird->n1).c, (bird->nt).r, (bird->nt).c, color ); 
    plotLine( (bird->n2).r, (bird->n2).c, (bird->nt).r, (bird->nt).c, color ); 
}

void initializeWalls(){
    Wall wall1 = { 
                    .openingR = 20 + rand()%100,  
                    .width = 40 + rand()%20,
                    .height = 50,
                    .screenC = 240, 
                    .passed = 0
                 };
    
    Wall wall2 = { 
                    .openingR = 20 + rand()%100,  
                    .width = 40 + rand()%20,
                    .height = 50,
                    .screenC = wall1.screenC + wall1.width + 90 + rand()%30, 
                    .passed = 0
                 };

    walls[0] = wall1;
    walls[1] = wall2;
}

void updateWalls(){
    
    walls[0].screenC = walls[0].screenC - 1;
    walls[1].screenC = walls[1].screenC - 1;
    
    if ( ( walls[0].screenC + walls[0].width ) <= 0 ){
        Wall newWall = { 
                        .openingR = 20 + rand()%100,  
                        .width = 40 + rand()%20,
                        .height = 50,
                        .screenC = walls[1].screenC + walls[1].width + 90 + rand()%30, 
                        .passed = 0
                   };
        walls[0] = walls[1];
        walls[1] = newWall;
    }
}

void drawWall(Wall *myWall, u16 color){
    drawHollowRect( 0, myWall->screenC, myWall->width, myWall->openingR, color );
    drawHollowRect( myWall->openingR + myWall->height, myWall->screenC, myWall->width, 160 - ( myWall->openingR + myWall->height ), color );
}
