
/*********
   CTIS164 - Template Source Program
----------
STUDENT : Sajad Abdolvandi
SECTION : 1
HOMEWORK: 4
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
 
 
 
 
 Sea with random sinusodial Waves
 Realistic sunrise effect from sun(the impact on sea)
 sons flare effect around sun
 Diamond Shooter with angle tracking (Tracking the suns position not the sun's vector's angle )
 changing colors for diamonds
 changing color when your inside or outside of the sea
 
 
*********/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GLUT/GLUT.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "vec.h"
#include <stdbool.h>
#include <stdarg.h>
#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 600
#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define SIZE 100
#define D2R 0.0174532
/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
typedef struct {
   vec_t pos;
   vec_t N;
} vertex_t;
#define FOV  30
typedef struct {
   float r, g, b;
} color_t;
typedef struct{
    vec_t pos;
    vec_t Vel;
    float angle;
    color_t color;
} light_t;
typedef struct {
    vec_t pos;
    vec_t vel;
    bool Shot;
    bool  visibility;
    int angle;
} attacker_t;
int movingX,movingY;

int generalTimer = 0;
attacker_t enemy[SIZE];
#define ENEMY_SPEED 20
int enemCounter = 0;
light_t Sun = {{0,0},{0,0},0,0.988, 0.49, 0.02};
void colorFunc(float r,float g,float b,float opac){
    glColor4f(r/255, g/255, b/255,opac);
}
float A = 10;
vec_t Xaxis = {600,0};
float xC ;
float yC ;
color_t mulColor(float k, color_t c) {
   color_t tmp = { k * c.r, k * c.g, k * c.b };
   return tmp;
}
double distanceImpact(double d) {
   return (-4.0 / 2000.0) * d + 2.2;
}
color_t calculateColor(light_t source, vertex_t v) {
   vec_t L = subV(source.pos, v.pos);
   vec_t uL = unitV(L);
    float  factor = dotP(uL, v.N) * distanceImpact(magV(L));
    color_t result = mulColor(factor, source.color);
    return result;
}
color_t calculateColor2(light_t source, vertex_t v) {
   vec_t L = subV(source.pos, v.pos);
   vec_t uL = unitV(L);
    float  factor = dotP(uL, v.N) *5;
    color_t result = mulColor(factor, source.color);
    return result;
}
color_t addColor(color_t c1, color_t c2) {
   color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    if (c1.r + c2.r >1) {
        tmp.r = 1;
    }
    if (c1.g + c2.g >1) {
        tmp.g = 1;
    }
    if (c1.b + c2.b >1) {
        tmp.b = 1;
    }
   return tmp;
}

void circle( int x, int y, int r )
{
#define PI 3.1415
   float angle ;
   glBegin( GL_POLYGON ) ;
   for ( int i = 0 ; i < 100 ; i++ )
   {
      angle = 2*PI*i/100;
      glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
   }
   glEnd();
}
void circle_wire(int x, int y, int r)
{
#define PI 3.1415
   float angle;

   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++)
   {
      angle = 2 * PI*i / 100;
      glVertex2f(x + r*cos(angle), y + r*sin(angle));
   }
   glEnd();
}
void print(int x, int y, const char *string, void *font )
{
   int len, i ;

   glRasterPos2f( x, y );
   len = (int) strlen( string );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, string[i]);
   }
}
// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string , ...)
{
   va_list ap;
   va_start ( ap, string );
   char str[1024] ;
   vsprintf( str, string, ap ) ;
   va_end(ap) ;
   
   int len, i ;
   glRasterPos2f( x, y );
   len = (int) strlen( str );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, str[i]);
   }
}
// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
   va_list ap;
   va_start(ap, string);
   char str[1024];
   vsprintf(str, string, ap);
   va_end(ap);
   glPushMatrix();
      glTranslatef(x, y, 0);
      glScalef(size, size, 1);
      
      int len, i;
      len = (int)strlen(str);
      for (i = 0; i<len; i++)
      {
         glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
      }
   glPopMatrix();
}
void LightrSource(void){
    glBegin( GL_LINES ) ;
    for ( float i = 0 ; i < 1000 ; i+=0.1 )
    {
        float angle = 2*PI*i/1000;
        vertex_t P = { {  Sun.pos.x+150*cos(angle), Sun.pos.y+150*sin(angle) }, { 0, -1 } };
        colorFunc(235, 235, 10,0.8);
        glVertex2f(Sun.pos.x, Sun.pos.y);
        glColor3f(1,1,1);
       glVertex2f( P.pos.x, P.pos.y) ;
    }
    glEnd();
    colorFunc(252, 127, 3,0.3);
    circle(Sun.pos.x , Sun.pos.y, 30);

    
}
bool shooer = false;
float freq = 1;
float stage = 0;
float sinfunc(float x){
    return (freq*sin((stage+x*D2R)))-100;
}
void Sea(void){
    glBegin(GL_LINES);
    for (float x = -600; x <= 600; x+=0.1) {
        vertex_t P = { { static_cast<double>(x), sinfunc(x) }, { 0, 1 } };
            color_t res = {0,0,0};
             res =  calculateColor(Sun, P);
            glBegin(GL_LINES);
            glColor3f(res.r, res.g, res.b);
            glVertex2f(x, sinfunc(x));
            //colorFunc(70, 148, 189 ,1);
            colorFunc(131, 218, 255 ,1);
            glVertex2f(x, -300);
            glEnd();
    }
   
}

void Vertex(float x ,float y,float ang,int ORIGINX,int ORIGINY)
{
    float s,c,xnew,ynew;
    s= sin(((ang-90)*D2R));
    c = cos(((ang-90)*D2R));
    x -= ORIGINX;
    y -= ORIGINY;
    xnew = x * c - y * s;
    ynew = x * s + y * c;
    x = xnew + ORIGINX;
    y = ynew + ORIGINY;
    glVertex2f(x, y);
    
}


void Attacker(){
    colorFunc(0, 0, 0, 1);
    glPointSize(10);

    for (int i = 0; i<enemCounter; i++) {
        if (enemy[i].Shot == true) {
        
            glBegin(GL_POLYGON);
            glColor3f(0.2, 0.2, 0.2);
            Vertex(-10+enemy[i].pos.x, 0+enemy[i].pos.y,enemy[i].angle, enemy[i].pos.x, enemy[i].pos.y);
            Vertex(0+enemy[i].pos.x, 15+enemy[i].pos.y,enemy[i].angle, enemy[i].pos.x, enemy[i].pos.y);
            Vertex(10+enemy[i].pos.x, 0+enemy[i].pos.y,enemy[i].angle, enemy[i].pos.x, enemy[i].pos.y);
            glEnd();
            glBegin(GL_POLYGON);
            float c = 1 * sin(0.7 * Sun.angle * D2R) + 1;
            glColor3f(c, c*c, 1-c);
            Vertex(-10+enemy[i].pos.x, 0+enemy[i].pos.y,enemy[i].angle, enemy[i].pos.x, enemy[i].pos.y);
            Vertex(0+enemy[i].pos.x, -30+enemy[i].pos.y,enemy[i].angle, enemy[i].pos.x, enemy[i].pos.y);
            Vertex(10+enemy[i].pos.x, 0+enemy[i].pos.y,enemy[i].angle, enemy[i].pos.x, enemy[i].pos.y);
            glEnd();
            
        }
     
    }
}
float PointAvg(float n1,float n2){
    return (n1+n2)/2;
}

float delta_x ;
float delta_y;
float myAng ;
float theta;
void dispShooter(){
    if (movingY < sinfunc(-150)) {
        colorFunc(255, 255, 255,1);
    }else{
        colorFunc(1, 1, 1, 1);

    }
   circle(movingX, movingY, 25);
   
    if (movingY < sinfunc(-150)) {
        colorFunc(237, 138, 24,1);
    }else{
        colorFunc(240, 240, 246, 1);

    }
   circle(movingX, movingY, 21);
   // big outside quad
    if (movingY < sinfunc(-150)) {
        colorFunc(250, 240, 240, 1);
        
    }else{
        colorFunc(1, 1,1, 1);

    }
  
   glBegin(GL_QUADS);
   Vertex(movingX-15,movingY+20,theta,movingX,movingY);
   Vertex(movingX-8,movingY+20+30,theta,movingX,movingY);
   Vertex(movingX+8,movingY+20+30,theta,movingX,movingY);
   Vertex(movingX+15, movingY+20,theta,movingX,movingY);
   glEnd();
    // inside quad
   if (movingY < sinfunc(-150)) {
       colorFunc(0, 0, 0, 1);
       shooer = true;
   }else{
       colorFunc(131, 218, 255 ,1);
       shooer = false;

   }

   glBegin(GL_QUADS);
   Vertex(movingX-11 , movingY+20,theta,movingX,movingY);
   Vertex(movingX-5, movingY+20+30,theta,movingX,movingY);
   Vertex(movingX+5, movingY+20+30,theta,movingX,movingY);
   Vertex(movingX+11, movingY+20,theta,movingX,movingY);
   glEnd();
    
    
    
    
//    glBegin(GL_LINES);
//    for (float x = -600; x <= 600; x+=0.1) {
//        vertex_t P = { { static_cast<double>(x), sinfunc(x) }, { 0, 1 } };
//            color_t res = {0,0,0};
//             res =  calculateColor(Sun, P);
//            glBegin(GL_LINES);
//            glColor3f(res.r, res.g, res.b);
//            glVertex2f(x, sinfunc(x));
//            colorFunc(131, 218, 255 ,1);
//            glVertex2f(x, -300);
//            glEnd();
//    }
   
    
    
    
}

void display() {
   glClearColor(1,1,1,1);
   glClear(GL_COLOR_BUFFER_BIT);
    LightrSource();
    Attacker();
    Sea();
    dispShooter();
 
    
    
    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);
    
   // to refresh the window it calls display() function
}

void onKeyUp(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);
    
   // to refresh the window it calls display() function
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown( int key, int x, int y )
{
   // Write your codes here.
   switch (key) {
   case GLUT_KEY_UP: up = true; break;
   case GLUT_KEY_DOWN: down = true; break;
   case GLUT_KEY_LEFT: left = true; break;
   case GLUT_KEY_RIGHT: right = true; break;
   }

   // to refresh the window it calls display() function
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp( int key, int x, int y )
{
   // Write your codes here.
   switch (key) {
   case GLUT_KEY_UP: up = false; break;
   case GLUT_KEY_DOWN: down = false; break;
   case GLUT_KEY_LEFT: left = false; break;
   case GLUT_KEY_RIGHT: right = false; break;
   }

   // to refresh the window it calls display() function
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick( int button, int stat, int x, int y )
{
   // Write your codes here.

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && shooer == false) {
        enemy[enemCounter].pos.x= movingX;
        enemy[enemCounter].pos.y= movingY;
        enemy[enemCounter].Shot = true;
        enemCounter++;
    }
   
   // to refresh the window it calls display() function
}
//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize( int w, int h )
{
   winWidth = w;
   winHeight = h;
   glViewport( 0, 0, w, h ) ;
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
   glMatrixMode( GL_MODELVIEW);
   glLoadIdentity();
   display(); // refresh window.
}
void onMoveDown( int x, int y ) {
   // Write your codes here.


   
   // to refresh the window it calls display() function
}
// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove( int x, int y ) {
   // Write your codes here.

    movingX = x - winWidth / 2;
    movingY = winHeight / 2 - y;
   
   // to refresh the window it calls display() function
}

float dist(vec_t obj1,vec_t obj2){
    float xp = powf(obj1.x-obj2.x, 2);
    
    float yp =powf(obj1.y-obj2.y, 2);
    float res = sqrtf(xp+yp);
    return res;
}
bool toggle = false;
#if TIMER_ON == 1
void onTimer( int v ) {
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   // Write your codes here.
    delta_x = Sun.pos.x - movingX;
    delta_y = Sun.pos.y - movingY;
    myAng = atan2(delta_y,delta_x)*V_R2D;
    theta = myAng < 0 ? myAng + 360 : myAng;
   
    stage+=0.12;
    if (freq >= 17) {
        toggle = !toggle;
    }
        if (toggle == true) {
            freq-=0.14;
        }else{
            freq+=0.14;
        }
    if (freq <= -17 ){
        toggle =!toggle;
    }
    
    xC =A*cos(Sun.angle*D2R);
    yC  = A*sin(Sun.angle*D2R);
    Sun.Vel.x =xC;
    Sun.Vel.y =yC;
    Sun.pos = addV(Sun.pos, Sun.Vel);
    Sun.angle+=1;
    if (Sun.angle== 360) {
        Sun.angle = 0;
    }
    
   
    for (int i = 0; i<enemCounter; i++) {
       
        enemy[i].angle = angleV(enemy[i].pos);
        vec_t V = pol2rec({ 1, Sun.angle });
        vec_t W = unitV(  subV(enemy[i].pos, Sun.pos) );
        enemy[i].visibility = dotP(V, W) > cos( FOV * D2R);

    
        enemy[i].vel = mulV( ENEMY_SPEED, unitV(subV(Sun.pos, enemy[i].pos)));


    

        enemy[i].pos = addV(enemy[i].pos, enemy[i].vel); // chase
    

    // do not allow Enemy to get out of the game arena.
        if (dist(enemy[i].pos, Sun.pos) <=30) {
            enemy[i].Shot = false;
        }
    }
   
   // to refresh the window it calls display() function
   glutPostRedisplay() ; // display()
}
#endif

void Init() {
   
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
}

int main( int argc, char *argv[] ) {
    for (int i = 0; i<SIZE; i++) {
        enemy[i].Shot = false;
        enemy[i].visibility = false;
    }
   glutInit(&argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   //glutInitWindowPosition(100, 100);
   glutCreateWindow("SeaSide Attack - SajadAbdolvandi - HW4");

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
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   #endif

   Init();
   
   glutMainLoop();
}
