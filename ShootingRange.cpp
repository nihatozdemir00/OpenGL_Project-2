
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 640

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532


/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

bool flag = false, activefire = false, endofgame = false;
bool success1 = false, success2 = false, success3 = false, success4 = false;

bool active1 = true, active2 = true, active3 = true, active4 = true;

bool freegun = false;

typedef struct {
    double x, y;
} point_t;


typedef struct {
    point_t pos;
    double angle;
}gun_t;

gun_t p = { {-300, 0}, 0 };


typedef struct {
    float x, y;
} pos_t;

pos_t p1, p2;


float A = 50,
fq = 1,
C = 0,
B = 10;
float angle = 0;

typedef struct {
    point_t pos2;
    double angle;
}fire_t;

fire_t fire = { {0,0}, 0};


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
    double angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 5)
    {
        angle = i * D2R;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
    float angle;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i += 5)
    {
        angle = i * D2R;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}


void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void vertex(point_t A, point_t B, double angle) {
    double xp = (A.x * cos(angle) - A.y * sin(angle)) + B.x;
    double yp = (A.x * sin(angle) + A.y * cos(angle)) + B.y;
    glVertex2d(xp, yp);
}

// function to move the box
float f(float x) {
    return 40 * sin((2 * x + C) * D2R) + B;
}

// to draw gun
void drawGun(gun_t p)
{
    double angle = p.angle * D2R;

    glColor3f(0.2, 0.3, 0.3);
    glBegin(GL_QUADS);
    vertex({ -40, 8 }, p.pos, angle);
    vertex({ 60, 8 }, p.pos, angle);
    vertex({ 60, -8 }, p.pos, angle);
    vertex({ -40, -8 }, p.pos, angle);
    glEnd();
    glLineWidth(5);
    glBegin(GL_LINE_STRIP);
    vertex({ -55, 16 }, p.pos, angle);
    vertex({ -40, 0 }, p.pos, angle);
    vertex({ -55, -16 }, p.pos, angle);
    glEnd();
   

    glColor3f(0, 0, 0.2);
    circle(p.pos.x, p.pos.y, 18);
    glColor3f(1, 1, 1);
    vprint(p.pos.x - 7, p.pos.y - 5, GLUT_BITMAP_HELVETICA_10, "%.0f", p.angle);
    glLineWidth(2);
    glColor3f(0.9, 0, 0);
    circle_wire(p.pos.x, p.pos.y, 18);
}

// to draw bullet
void drawFire(fire_t fire)
{
    double angle = fire.angle * D2R;
    if (activefire)
    {
        glColor3f(0.9, 0, 0);
        
        glBegin(GL_POLYGON);
        vertex({ 65, 4 }, fire.pos2, angle);
        vertex({ 85, 4 }, fire.pos2, angle);
        vertex({ 95, 0 }, fire.pos2, angle);
        vertex({ 85, -4 }, fire.pos2, angle);
        vertex({ 65, -4 }, fire.pos2, angle);
        glEnd();
    } 
}

// to draw orange 
void drawCircularIcon()
{
    if (active1)
    {
        float r = fabs(A);

        glColor3f(1, 0.6, 0.1);
        circle(r * cos(angle * D2R) + 250, r * sin(angle * D2R) + 245, 18);

        glColor3f(0, 0, 0);
        glLineWidth(1);
        circle_wire(r * cos(angle * D2R) + 250, r * sin(angle * D2R) + 245, 18);

        glColor3f(0.2, 0.5, 0.2);
        glBegin(GL_QUADS);
        glVertex2f(r * cos(angle * D2R) + 250, r * sin(angle * D2R) + 265);
        glVertex2f(r * cos(angle * D2R) + 250, r * sin(angle * D2R) + 255);
        glVertex2f(r * cos(angle * D2R) + 260, r * sin(angle * D2R) + 255);
        glVertex2f(r * cos(angle * D2R) + 270, r * sin(angle * D2R) + 270);
        glEnd();
    }
}

// to draw box
void drawSinIcon()
{
    if (active2)
    {
        glColor3f(0.7, 0.4, 0.1);
        glBegin(GL_QUADS);
        glVertex2f(angle - 100, f(angle) + 50);
        glVertex2f(angle - 50, f(angle) + 50);
        glVertex2f(angle - 50, f(angle) + 100);
        glVertex2f(angle - 100, f(angle) + 100);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(angle - 100, f(angle) + 100);
        glVertex2f(angle - 50, f(angle) + 100);
        glVertex2f(angle - 10, f(angle) + 130);
        glVertex2f(angle - 60, f(angle) + 130);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(angle - 10, f(angle) + 130);
        glVertex2f(angle - 50, f(angle) + 100);
        glVertex2f(angle - 50, f(angle) + 50);
        glVertex2f(angle - 10, f(angle) + 80);
        glEnd();

        glColor3f(0, 0, 0);
        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
        glVertex2f(angle - 100, f(angle) + 50);
        glVertex2f(angle - 50, f(angle) + 50);
        glVertex2f(angle - 50, f(angle) + 100);
        glVertex2f(angle - 100, f(angle) + 100);
        glEnd();
        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
        glVertex2f(angle - 100, f(angle) + 100);
        glVertex2f(angle - 50, f(angle) + 100);
        glVertex2f(angle - 10, f(angle) + 130);
        glVertex2f(angle - 60, f(angle) + 130);
        glEnd();
        glLineWidth(3);
        glBegin(GL_LINE_LOOP);
        glVertex2f(angle - 10, f(angle) + 130);
        glVertex2f(angle - 50, f(angle) + 100);
        glVertex2f(angle - 50, f(angle) + 50);
        glVertex2f(angle - 10, f(angle) + 80);
        glEnd();
    }
}

// to draw bottle
void drawHorizontalIcon()
{
    if (active3)
    {
        glColor3f(0.3, 0.5, 0);
        glBegin(GL_POLYGON);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p1.x + 20, p1.y);
        glVertex2f(p1.x + 20, p1.y + 40);
        glVertex2f(p1.x + 13, p1.y + 60);
        glVertex2f(p1.x + 13, p1.y + 70);
        glVertex2f(p1.x + 7, p1.y + 70);
        glVertex2f(p1.x + 7, p1.y + 60);
        glVertex2f(p1.x, p1.y + 40);
        glEnd();

        glColor3f(0.6, 0.4, 0.1);
        glBegin(GL_POLYGON);
        glVertex2f(p1.x + 13, p1.y + 75);
        glVertex2f(p1.x + 7, p1.y + 75);
        glVertex2f(p1.x + 7, p1.y + 70);
        glVertex2f(p1.x + 13, p1.y + 70);
        glEnd();

        glColor3f(0.9, 0.5, 0);
        glBegin(GL_POLYGON);
        glVertex2f(p1.x, p1.y + 30);
        glVertex2f(p1.x, p1.y + 10);
        glVertex2f(p1.x + 20, p1.y + 10);
        glVertex2f(p1.x + 20, p1.y + 30);
        glEnd();

        glColor3f(0, 0, 0);
        vprint(p1.x + 1, p1.y + 15, GLUT_BITMAP_HELVETICA_10, "164");
    }
}

// to draw target board
void drawIcon()
{
    if (active4)
    {
        glColor3f(1, 1, 1);
        glBegin(GL_POLYGON);
        glVertex2f(p2.x, p2.y - 55);
        glVertex2f(p2.x + 50, p2.y - 55);
        glVertex2f(p2.x + 55, p2.y + 45);
        glVertex2f(p2.x + 5, p2.y + 45);
        glEnd();

        glColor3f(0, 0, 0);
        glLineWidth(1);
        circle_wire(p2.x + 27, p2.y - 20, 20);
        circle_wire(p2.x + 27, p2.y - 20, 10);
        circle_wire(p2.x + 27, p2.y - 20, 1);
        glColor3f(0.9, 0, 0);
        glLineWidth(1);
        circle_wire(p2.x + 27, p2.y - 20, 15);
        circle_wire(p2.x + 27, p2.y - 20, 5);

        glColor3f(0, 0, 0);
        vprint(p2.x + 10, p2.y + 25, GLUT_BITMAP_HELVETICA_10, "TARGET");
        vprint(p2.x + 10, p2.y + 10, GLUT_BITMAP_HELVETICA_10, "BOARD");
    }
}

// to draw shooting range background
void backGround()
{
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_QUADS);
    glVertex2f(-400, 320);
    glVertex2f(-400, -280);
    glVertex2f(400, -280);
    glVertex2f(400, 320);
    glEnd();

    glColor3f(0, 0.2, 0.6);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(-220, 315);
    glVertex2f(-220, -278);
    glEnd();

    glColor3f(0, 0, 0);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(-400, 315);
    glVertex2f(400, 315);
    glEnd();

    glColor3f(0, 0, 0);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(-400, -280);
    glVertex2f(400, -280);
    glEnd();

    glColor3f(0, 0.2, 0.6);;
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(-370, 315);
    glVertex2f(-370, -278);
    glEnd();

    glColor3f(1, 1, 0.1);
    for(int y= -130; y <= 170; y += 150)
        for (int x = -200; x <= 350; x += 60)
        {
            glBegin(GL_LINES);
            glVertex2f(x,y);
            glVertex2f(x + 50, y);
            glEnd();
        }

    glLineWidth(10);
    glColor3f(0.9, 0, 0);
    vprint(-390, 160,  GLUT_BITMAP_8_BY_13, "S");
    vprint(-390, 140, GLUT_BITMAP_8_BY_13, "H");
    vprint(-390, 120, GLUT_BITMAP_8_BY_13, "O");
    vprint(-390, 100, GLUT_BITMAP_8_BY_13, "O");
    vprint(-390, 80, GLUT_BITMAP_8_BY_13, "T");
    vprint(-390, 60, GLUT_BITMAP_8_BY_13, "I");
    vprint(-390, 40, GLUT_BITMAP_8_BY_13, "N");
    vprint(-390, 20, GLUT_BITMAP_8_BY_13, "G");
    vprint(-390, -20, GLUT_BITMAP_8_BY_13, "R");
    vprint(-390, -40, GLUT_BITMAP_8_BY_13, "A");
    vprint(-390, -60, GLUT_BITMAP_8_BY_13, "N");
    vprint(-390, -80, GLUT_BITMAP_8_BY_13, "G");
    vprint(-390, -100, GLUT_BITMAP_8_BY_13, "E");
    
    // information about how to play
    glColor3f(1, 1, 1);
    glLineWidth(2);
    vprint(-150, -300, GLUT_BITMAP_8_BY_13, "<SPACE BAR> Fire, <F1> Pause/Restart, <F2> Move Gun Freely");

    // current location of gun 
    glLineWidth(4);
    vprint(-390, -300, GLUT_BITMAP_8_BY_13, "[%f , %f]", p.pos.x, p.pos.y);
}

// to draw end of the game window
void drawwindowEnd()
{
    if(endofgame)
    {
        glColor3f(1, 1, 1);
        glBegin(GL_POLYGON);
        glVertex2f(-100, -50);
        glVertex2f(-100, 50);
        glVertex2f(165, 50);
        glVertex2f(165, -50);
        glEnd();
        
        glColor3f(0.9, 0, 0);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-100, -50);
        glVertex2f(-100, 50);
        glVertex2f(165, 50);
        glVertex2f(165, -50);
        glEnd();

        glLineWidth(2);
        glColor3f(0, 0, 0);
        vprint(-90, 10, GLUT_BITMAP_8_BY_13, "THE END    THE END     THE END");
        vprint(-70, -30, GLUT_BITMAP_8_BY_13, "PRESS <F1> TO START AGAIN");
    }
}


//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    backGround();

    drawGun(p);

    drawCircularIcon();
    drawSinIcon();
    drawHorizontalIcon();
    drawIcon();

    drawFire(fire);

    drawwindowEnd();
    

    glutSwapBuffers();
}


//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
    {
        activefire = true;
        fire.pos2 = p.pos;
        fire.angle = p.angle;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.

    // to move targets
    if (key == GLUT_KEY_F1)
        flag = !flag;

    // to move gun freely
    if (key == GLUT_KEY_F2)
        freegun = !freegun;

    // to start game again
    if (key == GLUT_KEY_F1 && !active1 && !active2 && !active3 && !active4)
    {
        active1 = !active1;
        active2 = !active2;
        active3 = !active3;
        active4 = !active4;
        endofgame = !endofgame;
    }

    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
   glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

// function to move gun
void moveGunFree(gun_t* p, double speed) 
{
    double dx = speed * cos(p->angle * D2R);
    double dy = speed * sin(p->angle * D2R);
    point_t oldPos = p->pos;
    p->pos = { p->pos.x + dx, p->pos.y + dy };
   
    // check rectangular boundary
    if (p->pos.x > (-240) || p->pos.x < (-320) || p->pos.y >(280) || p->pos.y < (-240))
    {
        p->pos = oldPos;
    }
}

void moveGunUp(gun_t* p, double speed)
{
    double dy = 5;
    point_t oldPos = p->pos;
    p->pos = { p->pos.x , p->pos.y + dy };

    // check rectangular boundary
    if (p->pos.y >(280) || p->pos.y < (-240))
    {
        p->pos = oldPos;
    }
}

void moveGunDown(gun_t* p, double speed)
{
    double dy = 5;
    point_t oldPos = p->pos;
    p->pos = { p->pos.x , p->pos.y - dy };

    // check rectangular boundary
    if (p->pos.y > (280) || p->pos.y < (-240))
    {
        p->pos = oldPos;
    }
}






// function to change gun's angel
void turnGun(gun_t* p, double turn) {
    p->angle += turn;
    if (p->angle < 0) p->angle += 360;
    if (p->angle >= 360) p->angle -= 360;
}

// to check whether the orange is hitted  
bool successfulShot1(fire_t* fire, float A, float angle)
{
    if(angle < 360)
    angle += 2;
    if (angle > 360)
        angle -= 360;
  
    float r = fabs(A);
    if ((fire->pos2.x >= (r * cos(angle * D2R) + 250 - 18) && fire->pos2.x <= (r * cos(angle * D2R) + 250 + 18) && fire->pos2.y >= (r * sin(angle * D2R) + 245 - 18) && fire->pos2.y <= (r * sin(angle * D2R) + 245 + 18)) == 1)
    {
        success1 = true;
    }
   
    return(success1);
}

// to check whether the box is hitted
bool successfulShot2(fire_t* fire, float angle)
{
    angle = 0;

    if (((fire->pos2.x >= (angle - 100)) && (fire->pos2.x <= (angle - 10))) && ((fire->pos2.y >= (f(angle) + 80)) && (fire->pos2.y <= (f(angle) + 130))) == 1)
    {
        success2 = true;
    }
   
    return(success2);
}

// to check whether the bottle is hitted  
bool successfulShot3(fire_t* fire, pos_t p1, float angle)
{
    p1.y = -100;
    p1.x = 200 * sin(angle * D2R) + 100;

    if (((fire->pos2.x >= p1.x ) && (fire->pos2.x <= p1.x + 20)) && ((fire->pos2.y >= p1.y) && (fire->pos2.y <= p1.y + 120)) == 1)
    {
        success3 = true;
    }

    return(success3);
}

// to check whether the board is hitted 
bool successfulShot4(fire_t* fire, pos_t p2)
{
    p2.y = -200;
    if (p2.x + 5 > -180)
        p2.x -= 2;
    else
        p2.x = 400;

    if (((fire->pos2.x >= p2.x) && (fire->pos2.x <= p2.x + 50)) && ((fire->pos2.y >= p2.y - 55) && (fire->pos2.y <= p2.y + 45)) == 1)
    {
        success4 = true;
    }

    return(success4);
}



#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    // to move bullet
    if (activefire)
    {
        fire.angle = p.angle;

        if (fire.pos2.y > -280 && fire.pos2.y < 320) 
        {
            fire.pos2.y = fire.pos2.y + (sin(fire.angle * D2R) * 10);
        }
        if (fire.pos2.x > -400 && fire.pos2.x < 400) 
        {
            fire.pos2.x = fire.pos2.x + (cos(fire.angle * D2R) * 10);
        }
    }

    // to move target icons when the user press F1 
    if (flag)
    {
        angle += 2;
        if (angle > 360)
            angle -= 360;

        p1.y = -100;
        p1.x = 200 * sin(angle * D2R) + 100;

        p2.y = -200;
        if (p2.x + 5 > -180)
            p2.x -= 2;
        else
            p2.x = 400;

        // to destroy targets when the shot is succesfull
        if (successfulShot1(&fire, 50, 0))
        {
            active1 = !activefire;
        }

        if (successfulShot2(&fire, angle))
        {
            active2 = !activefire;
        }

        if (successfulShot3(&fire, p1, 50))
        {
            active3 = !activefire;
        }

        if (successfulShot4(&fire, p2))
        {
            active4 = !activefire;
        }

        // to activate bool which makes game start again
        if ((success1 == true && success2 == true && success3 == true && success4 == true) == 1)
            endofgame = !endofgame;
    }

    // turn the gun clockwise direction
    if (right) {
        turnGun(&p, -4);
    }

     // turn the gun counter-clockwise direction
    if (left) 
    {
        turnGun(&p, 4);

    }

    // move forward
    if (up)
    {
        
        moveGunUp(&p, 5);

    }

     // move backward
    if (down)
    {
       
        moveGunDown(&p, -5);

    }

    if (freegun)
    {
        if (up)
            moveGunFree(&p, 5);
        if (down)
            moveGunFree(&p, -5);
    }
    

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

    }
#endif


void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("CTI164 HW3 - Shooting Range Game by Nihat Özdemir(21802080)");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    //Init();

    glutMainLoop();
}
