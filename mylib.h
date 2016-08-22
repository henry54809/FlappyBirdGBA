typedef unsigned short u16;
#define SCANLINECOUNTER *(volatile unsigned short*) 0x4000006
//Function prototypes
void setPixel(int r, int c, u16 color);
void drawRect(int r, int c, int width, int height, u16 color);
void drawHollowRect(int r, int c, int width, int height, u16 color);
void plotLine(int x0, int y0, int x1, int y1, u16 color);
void waitForVBlank();

typedef struct {
    int r;
    int c;
} Point;

typedef struct {
    int openingR;
    int width;
    int height;
    int screenC;
    int passed;
} Wall;

typedef struct {
    Point origin;
    Point b;
    Point w1;
    Point w2;
    Point wb;
    Point h;
    Point e;
    Point n1;
    Point n2;
    Point nt;
    int up;
    int offsetR;
    int offsetC;
} Bird;

