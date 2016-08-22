#include "mylib.h"
#include <math.h>
u16 *videoBuffer = ( u16 * ) 0x6000000;


//Functions
// A function to set pixel (r, c) to the color passed in
void setPixel(int r, int c, u16 color){
    videoBuffer[240*r + c] = color;
}

void drawRect(int r, int c, int width, int height, u16 color){
    int i, j; // column index , row index
    for ( i=c; i< (c + width); i++ ){
        for ( j=r; j< (r + height); j++ ){
            if ( j < 160 && i < 240 ) 
                setPixel( j, i , color );
        }
    }  
}

void drawHollowRect(int r, int c, int width, int height, u16 color){ 
    int i; 
    for ( i=c; i< ( c+ width); i++ ){ 
       if ( i < 240 && r < 160 && i > 0 && r > 0 ){ 
            setPixel( r, i, color );   
       } 
 
       if ( (r + height) < 160 && i < 240 && i > 0 && ( r+ height ) > 0 ){ 
            setPixel( r + height - 1, i, color ); 
       } 
    } 
 
    for ( i=r; i< ( r+ height ); i++ ){ 
       if ( i < 160 && c < 240 && i > 0 && c > 0 ){ 
            setPixel( i, c, color );   
       } 
 
       if ( (c + width) < 240 && i < 160 && i > 0 && ( c + width ) > 0 ){ 
            setPixel( i, c + width - 1, color ); 
       } 
    } 
 
} 
 
void plotLine(int x0, int y0, int x1, int y1, u16 color){ 
 
    int dx = fabs( x1 - x0 );
    int signX = x0 < x1 ? 1 : -1;
    int dy = fabs( y1 - y0 );
    int signY = y0 < y1 ? 1 : -1; 
    int err = ( (dx > dy ? dx : -dy) >>1 );

    while(1){
        
        setPixel(x0,y0, color);
        
        if (x0==x1 && y0 == y1) 
            break;
        int e2 = err;
        
        if (e2 >-dx) { 
            err -= dy; 
            x0 += signX; 
        }
        
        if (e2 < dy) { 
            err += dx; 
            y0 += signY; 
        }
    }
}

void waitForVBlank()
{
    while(SCANLINECOUNTER>160);
    while(SCANLINECOUNTER<160);
}
