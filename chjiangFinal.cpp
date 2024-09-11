/******************************************
*
* Official Name:  Chengyi Jiang
*
* Call me: Chengyi
*
* E-mail:  chjiang@syr.edu
*
* Assignment:  Final Assignment
*
* Environment/Compiler:  Xcode 14.2
*
* Date submitted:  May 2, 2023
*
* References:  NONE.
*
* Interactions:
*                 Use array keys to move forward / back / turn left / turn right
*                 Click mouse on the door / window to open / close it.
*                 Use page up / down to look up / down
*                 Click keyboard key 'w' to open / close blinds
*                 Click 'd' to open / close the door1 (bedroom)
*                 Click 'D' to open / close the door2 (bathroom)
*                 Click 's' to open / close shower
*                 Click 'r' to open / close drawer
*                 Click 'l to turn on / off the room light
*                 Click 'f' to turn on / off the fan, press '+' / '-' to control the speed
*                 Click 'b' to show a power line for the computer
*                 Mouse click some buttons can also control the animation
*                 Right click mouse can open menu
*
*******************************************/

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
#  include <GL/glext.h>
#endif

#define ONE_BY_ROOT_THREE 0.57735
#define PI 3.14159
#define MAX_NUM_PARTICLES 50
#define INITIAL_NUM_PARTICLES 20
#define INITIAL_POINT_SIZE 5.0
#define INITIAL_SPEED 1.0
#define TRUE 1
#define FALSE 0

using namespace std;

// Begin globals.
static unsigned int aSide; // List index.
static int step = 0; // Steps in open/closing bedroom door
static int step3 = 0; // Steps in open/closing bathroom door
static float tWater = 2.0; // the height of water in the tolit
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
static float meX=0, meY=9,meZ=40;
//static float meX=0, meY=9, meZ=10;
static float drawerZ = 0;
static float bh1=0, bh2=0.79, bh3=1.62, bh4=2.46, bh5=3.3, bh6=4.14, bh7=4.98, bh8=5.82, bh9=6.66, bh10=7.5;
static float bangle = 70;
static float angle=0;  //angle facing
static float stepsize=1.0;  //step size
static float turnsize=10.0; //degrees to turn
static float fanangle = 0.0; //fan rotate variable
static bool door1 = false; //for the bedroom door open/close
static bool door2 = false; //for bathroom door open/close
static int animationPeriod = 100; // Time interval for door.
static int fanPeriod = 100; // Time interval for fan.
static int waterPeriod = 25; // Time interval for water lines
static bool roomlight = true; //the light in the room on/off
static bool hidendoor = false;
static bool window = false;
static bool tolit = false; //control the water in the tolit
static bool fan = false;
static bool drawer = false;
static bool shower = false;
static bool powerline = false;
int height, width;
bool selecting = false;
int xClick,yClick;  //coordinates of mouseclick
float viewAngle = 0; //angle face up/down
bool dark = false; //this depends the objects' color when there's no light
float outlight = 0.5;
static unsigned int texture[2]; // Array of texture indices.
static int Cid = -1;
static float wX1=-3, wY1=10, wX2=-3.5, wY2=10, wX3=-3.3, wY3=10.8, wX4=-3.6, wY4=11, wX5=-3, wY5=11;
static float controlPoints[6][3] =
{
    { -27.0, 47.0, 0.0}, { -19.0, 24.0, 0.0}, { -30.0, 6.0, 0.0},
    {-5.0, -17.0, 0.0}, {39.0, -20.0, 0.0}, { 48.0, -43.0, 0.0}
};

void myDisplay();
void myIdle();
void myReshape(int, int);
void main_menu(int);
void collision(int);
float forces(int, int);
void myinit();


/* particle struct */

typedef struct particle
{
     int color;
     float position[3];
     float velocity[3];
     float mass;
} particle;

particle particles[MAX_NUM_PARTICLES]; /* particle system */

/* initial state of particle system */

int present_time;
int last_time;
int num_particles = INITIAL_NUM_PARTICLES;
float point_size = INITIAL_POINT_SIZE;
float speed = INITIAL_SPEED;
bool gravity = FALSE; /* gravity off */
bool elastic = FALSE; /* restitution off */
bool repulsion = FALSE; /* repulsion off */
float coef = 1.0; /* perfectly elastic collisions */
float d2[MAX_NUM_PARTICLES][MAX_NUM_PARTICLES]; /* array for interparticle distances */

GLsizei wh = 500, ww = 500; /* initial window size */

GLfloat colors[8][3]={{1, 1, 1}, {0.0, 0.0, 0.8},{0.0, 1.0, 0.7},
    {0.0, 0.0, 0.6}, {0.2, 0.2, 1.0}, {0.0, 0.0, 0.5}, {0.3, 0.3, 0.7},
    {0.3, 0.3, 0.8}};

void myinit()
{
        int  i, j;

 /* set up particles with random locations and velocities */

        for(i=0; i<num_particles; i++)
        {
            particles[i].color = i%8;
            if(particles[i].color==1) //red particles have more mass
              particles[i].mass = 10.0;
            else
              particles[i].mass = 1.0;
            
            for(j=0; j<3; j++)
            {
                particles[i].position[j] = 2.0*((float) rand()/RAND_MAX)-1.0;
                particles[i].velocity[j] = speed*((float) rand()/RAND_MAX)-1.0;
            }
        }
        glPointSize(point_size);
}

void myIdle()
{
    int i, j, k;
    float dt;
    present_time = glutGet(GLUT_ELAPSED_TIME);
    dt = 0.001*(present_time -  last_time);
    for(i=0; i<num_particles; i++)
    {
       for(j=0; j<3; j++)
       {
           particles[i].position[j]+=dt*particles[i].velocity[j];
           particles[i].velocity[j]+=dt*forces(i,j)/particles[i].mass;
       }
        collision(i);
    }
    if(repulsion) for(i=0;i<num_particles;i++) for(k=0;k<i;k++)
    {
            d2[i][k] = 0.0;
            for(j=0;j<3;j++) d2[i][k]+= (particles[i].position[j]-
              particles[k].position[j])*(particles[i].position[j]-
              particles[k].position[j]);
            d2[k][i]=d2[i][k];
    }
    last_time = present_time;
    glutPostRedisplay();
}

float forces(int i, int j)
{
   int k;
   float force = 0.0;
   if(gravity&&j==1) force = -1.0; /* simple gravity */
   if(repulsion) for(k=0; k<num_particles; k++)  /* repulsive force */
   {
      if(k!=i) force+= 0.001*(particles[i].position[j]-particles[k].position[j])/(0.001+d2[i][k]);
   }
   return(force);
}

void collision(int n)

/* tests for collisions against cube and reflect particles if necessary */

{
     int i;
     for (i=0; i<3; i++)
     {
           if(particles[n].position[i]>=1.0)
           {
                particles[n].velocity[i] = -coef*particles[n].velocity[i];
                particles[n].position[i] = 1.0-coef*(particles[n].position[i]-1.0);
           }
           if(particles[n].position[i]<=-1.0)
           {
                particles[n].velocity[i] = -coef*particles[n].velocity[i];
                particles[n].position[i] = -1.0-coef*(particles[n].position[i]+1.0);
           }
     }
}


// Struct of bitmap file.
struct BitMapFile
{
   int sizeX;
   int sizeY;
   unsigned char *data;
};

// Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile *getBMPData(string filename)
{
   BitMapFile *bmp = new BitMapFile;
   unsigned int size, offset, headerSize;
  
   // Read input file name.
   ifstream infile(filename.c_str(), ios::binary);
 
   // Get the starting point of the image data.
   infile.seekg(10);
   infile.read((char *) &offset, 4);
   
   // Get the header size of the bitmap.
   infile.read((char *) &headerSize,4);

   // Get width and height values in the bitmap header.
   infile.seekg(18);
   infile.read( (char *) &bmp->sizeX, 4);
   infile.read( (char *) &bmp->sizeY, 4);

   // Allocate buffer for the image.
   size = bmp->sizeX * bmp->sizeY * 24;
   bmp->data = new unsigned char[size];

   // Read bitmap data.
   infile.seekg(offset);
   infile.read((char *) bmp->data , size);
   
   // Reverse color from bgr to rgb.
   int temp;
   for (int i = 0; i < size; i += 3)
   {
      temp = bmp->data[i];
      bmp->data[i] = bmp->data[i+2];
      bmp->data[i+2] = temp;
   }

   return bmp;
}

// Load external textures.
void loadExternalTextures()
{
   // Local storage for bmp image data.
   BitMapFile  *image[2];
   
   // Load the textures.
    image[0] = getBMPData("./grass.bmp");
    image[1] = getBMPData("./launch.bmp");
    
   // Bind grass image to texture index[0].
   glBindTexture(GL_TEXTURE_2D, texture[0]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
                GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
    
    // Activate texture index texture[1].
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);
}

// Box vertex co-ordinate vectors. 
static float vertices[] = 
{
	1.0, -1.0, 1.0, 
	1.0, 1.0, 1.0, 
	1.0, 1.0, -1.0, 
	1.0, -1.0, -1.0, 
	-1.0, -1.0, 1.0, 
	-1.0, 1.0, 1.0, 
	-1.0, 1.0, -1.0, 
	-1.0, -1.0, -1.0
};

// Vertex indices of each box side, 6 groups of 4.
static unsigned char quadIndices[] = 
{
    3, 2, 1, 0, 
    7, 6, 2, 3,
    4, 5, 6, 7,
	0, 1, 5, 4,
	4, 7, 3, 0,
	6, 5, 1, 2
};

// Box vertex normal vectors = normalized unit vector pointing from origin to vertex.
static float normals[] = 
{
	ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, 
	ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, 
	ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, 
	ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, 
	-ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, 
	-ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, 
	-ONE_BY_ROOT_THREE, ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, 
	-ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE, -ONE_BY_ROOT_THREE
};
// End globals.

//function will look at a pixel and based on its color
//set the value of itemID.
//x and y are the screen coordinates of point clicked.
void getID(int x, int y)
{
    unsigned char pixel[3];
    glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    //printed only for demonstration
    cout << "R: " << (int)pixel[0] << endl;
    cout << "G: " << (int)pixel[1] << endl;
    cout << "B: " << (int)pixel[2] << endl;
    cout << endl;
    
    if ((int)pixel[0]==0&&(int)pixel[1]==255&&(int)pixel[2]==0)
    {
        door1 = !door1;
    }
    if ((int)pixel[0]==128&&(int)pixel[1]==0&&(int)pixel[2]==0)
    {
        door2 = !door2;
    }
    if ((int)pixel[0]==204&&(int)pixel[1]==204&&(int)pixel[2]==178)
    {
        door2 = !door2;
    }
    if ((int)pixel[0]==178&&(int)pixel[1]==0&&(int)pixel[2]==0)
    {
        roomlight = !roomlight;
    }
    if ((int)pixel[0]==77&&(int)pixel[1]==0&&(int)pixel[2]==26)
    {
        roomlight = !roomlight;
    }
    if ((int)pixel[0]==255&&(int)pixel[1]==0&&(int)pixel[2]==0)
    {
        window = !window;
    }
    if ((int)pixel[0]==0&&(int)pixel[1]==128&&(int)pixel[2]==77)
    {
        tolit = !tolit;
    }
    if ((int)pixel[0]==0&&(int)pixel[1]==0&&(int)pixel[2]==178)
    {
        fan = !fan;
    }
    if ((int)pixel[0]==51&&(int)pixel[1]==26&&(int)pixel[2]==0)
    {
        drawer = !drawer;
    }
    if ((int)pixel[0]==77&&(int)pixel[1]==77&&(int)pixel[2]==178)
    {
        shower = !shower;
    }
    
    selecting = false;
    glutPostRedisplay();
}

void animate(int value)
{
    if(door1){
        if (step < 90) step += 5;
    }
    else{
        if (step > 0) step -= 5;
    }
    
    if(door2){
        if (step3 < 90) step3 += 5;
    }
    else{
        if (step3 > 0) step3 -= 5;
    }
    
    if(window != true){
        if(bangle < 70) bangle += 5;
    }
    else{
        if (bangle > 0) bangle -= 5;
    }
    
    if(tolit){
        if(tWater > 1.0) tWater -= 0.1;
        else tolit = false;
    }
    if(tolit == false){
        if(tWater<2.0) tWater += 0.1;
    }
    if(window){
        if (outlight < 1) outlight += 0.05;
    }
    if(window != true){
        if (outlight > 0.5) outlight -= 0.05;
    }
    if(drawer){
        if(drawerZ < 2) drawerZ += 0.5;
    }
    if(drawer == false){
        if(drawerZ > 0) drawerZ -= 0.5;
    }
    
    glutTimerFunc(animationPeriod, animate, 1);
   glutPostRedisplay();
}

void animate2(int value)
{
    if(fan){
        fanangle += 5;
        if(fanangle == 360) fanangle = 0;
    }

    glutTimerFunc(fanPeriod, animate2, 1);
   glutPostRedisplay();
}

void animate3(int value)
{
    if(shower){
        //1
        wX1 += 0.3;
        wY1 -= 0.3;
        if(wX1>3) wX1 = -3, wY1 = 10;
        //2
        wX2 += 0.25;
        wY2 -= 0.25;
        if(wX2>3) wX2 = -3, wY2 = 9.5;
        //3
        wX3 += 0.3;
        wY3 -= 0.3;
        if(wX3>3) wX3 = -3.3, wY3 = 10.8;
        //4
        wX4 += 0.25;
        wY4 -= 0.25;
        if(wX4>3) wX4 = -3.6, wY4 = 11;
        //5
        wX5 += 0.35;
        wY5 -= 0.35;
        if(wX5>3) wX5 = -3, wY5 = 11;
    }
    glutTimerFunc(waterPeriod, animate3, 1);
    glutPostRedisplay();
}

// Initialization routine.
void setup(void)
{
   aSide = glGenLists (1); // Return a list index.

   // A side of the box in a display list.
   glNewList(aSide, GL_COMPILE);
   glBegin(GL_QUADS);
      glNormal3f(0.0, -0.8, 0.0);
      glVertex3f(2.0, -2.0, 2.0);
	  glVertex3f(-2.0, -2.0, 2.0);
	  glVertex3f(-2.0, -2.0, -2.0);
	  glVertex3f(2.0, -2.0, -2.0);
   glEnd();
   glEndList();

   glClearColor(0.7, 0.8, 0.9, 0.0); //blue sky
   glEnable(GL_DEPTH_TEST); // Enable depth testing.

   // Turn on OpenGL lighting.
   glEnable(GL_LIGHTING);

   // roomlight property vectors.
   float lightAmb0[] = { 0.9, 0.9, 0.5, 1.0 };
   float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
   float lightPos0[] = { 7.0, 19, -3.0, 1.0 }; //light position
   float globAmb0[] = { 0.3, 0.3, 0.3, 1.0 };
   //float globAmb0[] = { 0, 0, 0, 1.0 };

    //sun
    float lightAmb1[] = {0, 0, 0, 1.0};
    float lightDifAndSpec1[] = { 1.0, 1.0, 1.0, 1.0 };
    float lightPos1[] = { 10.0, 100, -10.0, 1.0 }; //light position
    float globAmb1[] = { 0.4, 0.4, 0.4, 1.0 };
    
    
   // Light properties.
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
   glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

   glEnable(GL_LIGHT0); // Enable particular light source.
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb0); // Global ambient light.
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);// Enable two-sided lighting.
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec1);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec1);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
    
    glEnable(GL_LIGHT1); // Enable particular light source.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb1); // Global ambient light.
    
    
   // Enable two vertex arrays: position and normal.
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   // Specify locations for the position and normal arrays.
   glVertexPointer(3, GL_FLOAT, 0, vertices);
   glNormalPointer(GL_FLOAT, 0, normals);

   glColor3f(0.0, 0.0, 0.0);
    
    // Create texture index array and load external textures.
    glGenTextures(1, texture);
    loadExternalTextures();

    // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//draw the objects
void object(int shadow){
    //make a tree
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(10, 0, 0);
    //trunk
    glPushMatrix();
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.5, 0.3, 0.3); //brown
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(70.0, 0.0, -3.0);
    glScalef(2,2,4);
    glutSolidCube(3);
    glPopMatrix();
    //leaf
    glPushMatrix();
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.2, 0.7, 0.2); //green leaf
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(70.0, 0.0, -8.0);
    glRotatef(180, 1, 0, 0);
    glScalef(2,2,4);
    glutSolidCone(5, 6, 10, 10);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
    //make a tree2
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(-10, 0, 0);
    //trunk
    glPushMatrix();
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.5, 0.3, 0.3); //brown
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(60.0, -10.0, -3.0);
    glScalef(2,2,4);
    glutSolidCube(3);
    glPopMatrix();
    //leaf
    glPushMatrix();
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.2, 0.6, 0.2); //green leaf
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(60.0, -10.0, -9.0);
    glRotatef(180, 1, 0, 0);
    glScalef(2,2,4);
    glutSolidCone(5, 6, 10, 10);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glPopMatrix();
        
    
    //draw inside objects don't need lights
    glDisable(GL_LIGHTING);
        
    //draw a computer
    glPushMatrix();
    glTranslatef(0, -11, -16);
    glScalef(3, 3, 3);
    //bottom
        //right
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.2, 0.2);
    glTranslatef(0.85, 5.5, 0.25);
    glRotatef(45, 0, 1, 0);
    glScalef(0.3, 0.1, 0.7);
    glTranslatef(1, 0, 0);
    glTranslatef(-1, 0, 0);
    glutSolidCube(1);
    glPopMatrix();
        //left
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.2, 0.2);
    glTranslatef(0.15, 5.5, 0.25);
    glRotatef(45, 0, -1, 0);
    glScalef(0.3, 0.1, 0.7);
    glTranslatef(1, 0, 0);
    glTranslatef(-1, 0, 0);
    glutSolidCube(1);
    glPopMatrix();
        //mid
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.15, 0.15, 0.15);
    glTranslatef(0.5, 6, 0);
    glRotatef(45, 0, -1, 0);
    glScalef(0.3, 1, 0.3);
    glTranslatef(1, 0, 0);
    glTranslatef(-1, 0, 0);
    glutSolidCube(1);
    glPopMatrix();
        //screen
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.25, 0.25, 0.3);
    glTranslatef(0.5, 6.5, 0.2);
    glScalef(2, 1, 0.1);
    glTranslatef(1, 0, 0);
    glTranslatef(-1, 0, 0);
    glutSolidCube(1);
    glPopMatrix();
    
    glPopMatrix();//end computer
    
    //fan (5 blades)
    glPushMatrix();
    glTranslatef(7, 19, -6);
    
    //blade1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.6);
    glTranslatef(0, 0, 3);
    glRotatef(fanangle, 0, 1, 0);
    glTranslatef(0, 0, -3);
    glScalef(0.5, 0.1, 2);
    glutSolidCube(2);
    glPopMatrix();
    //blade2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.6);
    glTranslatef(0, 0, 3);
    glRotatef(fanangle+72, 0, 1, 0);
    glTranslatef(0, 0, -3);
    glScalef(0.5, 0.1, 2);
    glutSolidCube(2);
    glPopMatrix();
    //blade3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.6);
    glTranslatef(0, 0, 3);
    glRotatef(fanangle+72*2, 0, 1, 0);
    glTranslatef(0, 0, -3);
    glScalef(0.5, 0.1, 2);
    glutSolidCube(2);
    glPopMatrix();
    //blade4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.6);
    glTranslatef(0, 0, 3);
    glRotatef(fanangle+72*3, 0, 1, 0);
    glTranslatef(0, 0, -3);
    glScalef(0.5, 0.1, 2);
    glutSolidCube(2);
    glPopMatrix();
    //blade5
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.6);
    glTranslatef(0, 0, 3);
    glRotatef(fanangle+72*4, 0, 1, 0);
    glTranslatef(0, 0, -3);
    glScalef(0.5, 0.1, 2);
    glutSolidCube(2);
    glPopMatrix();
    
    glPopMatrix();//end fan
    
    //make a table (smaller one)
    //1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.25, 0.25, 0.25);
    glTranslatef(19.1, 2.1, -8);
    glScalef(0.1,1.5,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.25, 0.25, 0.25);
    glTranslatef(23.0, 2.1, -8);
    glScalef(0.1,1.5,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.25, 0.25, 0.25);
    glTranslatef(19.1, 2.1, -1);
    glScalef(0.1,1.5,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.25, 0.25, 0.25);
    glTranslatef(23.0, 2.1, -1);
    glScalef(0.1,1.5,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //5 top
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.8, 0.8, 0.8);
    glTranslatef(21, 4.5, -4.5);
    glScalef(1.4,0.1,2.4);
    glutSolidCube(3);
    glPopMatrix();
    //6 bottom
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.25, 0.25, 0.25);
    glTranslatef(21, 4.2, -4.5);
    glScalef(1.4,0.08,2.4);
    glutSolidCube(3);
    glPopMatrix();
    //end drawing
    
    //draw a window deck
    glPushMatrix(); //top
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else {
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.7, 0.7, 0.7);
    }
    glTranslatef(24, 8, -2.0);
    glScalef(0.8,0.1,6);
    glutSolidCube(3);
    glPopMatrix();
    glPushMatrix(); //bottom
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else {
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.7, 0.7, 0.7);
    }
    glTranslatef(24.2, 7.8, -2.0);
    glScalef(0.6,0.2,6);
    glutSolidCube(3);
    glPopMatrix();
    
    //draw a drawer
    glPushMatrix();
    glTranslatef(13, 0, 9.8);
    //frame1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(0, 0.15, 0);
    glScalef(6, 0.1, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(0, 6, 0);
    glScalef(6, 0.1, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(0, 0.15, -3);
    glScalef(6, 0.1, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(0, 6, -3);
    glScalef(6, 0.1, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame5
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(3, 3, -3);
    glScalef(0.1, 6, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame6
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(3, 3, 0);
    glScalef(0.1, 6, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame7
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-3, 3, -3);
    glScalef(0.1, 6, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame8
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-3, 3, 0);
    glScalef(0.1, 6, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //frame9
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(3, 0.15, -1.5);
    glScalef(0.1, 0.1, 3);
    glutSolidCube(1);
    glPopMatrix();
    //frame10
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-3, 0.15, -1.5);
    glScalef(0.1, 0.1, 3);
    glutSolidCube(1);
    glPopMatrix();
    //frame11
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(3, 6, -1.5);
    glScalef(0.1, 0.1, 3);
    glutSolidCube(1);
    glPopMatrix();
    //frame12
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-3, 6, -1.5);
    glScalef(0.1, 0.1, 3);
    glutSolidCube(1);
    glPopMatrix();
    //fixed box(inner)
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.1, 0.0, 0.0);
    glTranslatef(0, 1, -1.5);
    glScalef(1.95, 2, 0.8);
    glutSolidCube(3);
    glPopMatrix();
    //fixed box1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.1, 0.0);
    glTranslatef(0, 0.95, -1.47);
    glScalef(1.955, 0.5, 1);
    glutSolidCube(3);
    glPopMatrix();
    //fixed box2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.1, 0.0);
    glTranslatef(0, 2.9, -1.47);
    glScalef(1.955, 0.5, 1);
    glutSolidCube(3);
    glPopMatrix();
    //side1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.1, 0.0);
    glTranslatef(3, 3.1, -1.5);
    glScalef(0.05, 1.9, 0.95);
    glutSolidCube(3);
    glPopMatrix();
    //side2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.1, 0.0);
    glTranslatef(-3, 3.1, -1.5);
    glScalef(0.05, 1.9, 0.95);
    glutSolidCube(3);
    glPopMatrix();
    //side top
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.1, 0.0);
    glTranslatef(0, 6, -1.5);
    glScalef(1.95, 0.05, 1);
    glutSolidCube(3);
    glPopMatrix();
    //moveable box
    glPushMatrix();
    glTranslatef(0, 0, -drawerZ);
        //back
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.0);
    glTranslatef(0, 4.8, -0.5);
    glScalef(1.955, 0.6, 0.1);
    glutSolidCube(3);
    glPopMatrix();
        //bottom
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.6);
    glTranslatef(0, 4.3, -1.5);
    glScalef(1.9, 0.05, 0.9);
    glutSolidCube(3);
    glPopMatrix();
        //side1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.0);
    glTranslatef(2.8, 4.8, -1.5);
    glScalef(0.045, 0.4, 0.8);
    glutSolidCube(3);
    glPopMatrix();
        //side2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.5, 0.5, 0.0);
    glTranslatef(-2.8, 4.8, -1.5);
    glScalef(0.045, 0.4, 0.8);
    glutSolidCube(3);
    glPopMatrix();
        //front
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.1, 0.0);
    glTranslatef(0, 4.95, -2.8);
    glScalef(1.955, 0.6, 0.1);
    glutSolidCube(3);
    glPopMatrix();
    glPopMatrix(); //end moveable box
    
    //dragonfly from HW3!!!
    glPushMatrix();
    glTranslatef(0, 4.5, -5.5-drawerZ);
    glRotatef(90, 0, -1, 0);
    glScalef(0.2, 0.2, 0.2);
   //draw a dragonfly
   //draw the dragonfly body
   glPushMatrix();
   glTranslatef(0, 2, 0);
   glRotatef(90.0, 1.0, 0.0, 0.0);
   glTranslatef(20.0, 0.0, 0.0);
   if ( shadow ) glColor3f(0.0, 0.0, 0.0);
   else glColor3f(0.5, 0.9, 0.5);
   glScalef(0.3,0.6,0.3);
   glutSolidSphere(2.0, 20, 20);
   glPopMatrix();
    //draw the dragonfly head
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(20.0, 2.0, 0.0);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.5, 0.9, 0.5);
    glScalef(0.4,0.4,0.4);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();
    //draw the dragonfly tail
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(20.0, -1.0, 0.0);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.5, 0.9, 0.5);
    glScalef(0.1,1.0,0.1);
    glutWireSphere(2.0, 20, 20);
    glPopMatrix();
    //draw two eyes
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(19.5, 2.5, 0.0);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0, 0.1, 0); //to distinguish it from the shadow
    glScalef(0.2,0.2,0.2);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glTranslatef(20.5, 2.5, 0.0);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0, 0.1, 0); //to distinguish it from the shadow
    glScalef(0.2,0.2,0.2);
    glutSolidSphere(1.0, 20, 20);
    glPopMatrix();
    //draw wings
    //right front wing
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glTranslatef(20.0, 0.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glRotatef(30, 0.0, 0.0, 1.0);
    glTranslatef(-20.0, 0.0, 0.0);
    glTranslatef(16.0, 0.0, 0.0);
    glTranslatef(6, 0.0, 0.5);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.4, 0.4, 0.4);
    glScalef(0.7,0.1,0.2);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();
    //right behind wing
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glTranslatef(20.0, 0.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glRotatef(30, 0.0, 0.0, 1.0);
    glTranslatef(-20.0, 0.0, 0.0);
    glTranslatef(15.5, 0.0, 0.0);
    glTranslatef(6, 0.0, -0.5);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.4, 0.4, 0.4);
    glScalef(0.5,0.1,0.2);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();
    //left front wing
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glTranslatef(20.0, 0.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glRotatef(150, 0.0, 0.0, 1.0);
    glTranslatef(-20.0, 0.0, 0.0);
    glTranslatef(22.0, 0.0, 0.5);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.4, 0.4, 0.4);
    glScalef(0.7,0.1,0.2);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();
    //left behind wing
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glTranslatef(20.0, 0.0, 0.0);
    glRotatef(0, 0.0, 0.0, 1.0);
    glRotatef(-30, 0.0, 0.0, 1.0);
    glTranslatef(-20.0, 0.0, 0.0);
    glTranslatef(18.5, 0.0, -0.5);
    if ( shadow ) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(0.4, 0.4, 0.4);
    glScalef(0.5,0.1,0.2);
    glutSolidSphere(2.0, 20, 20);
    glPopMatrix();
    
    glPopMatrix();//end of the dragonfly
    
    glPopMatrix();//end drawer
    
    //draw a bed
    //mattress
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.2, 0.4);//dark blue
    glTranslatef(15.5, 5, -14.0);
    glScalef(2.5,0.3,1.8);
    glutSolidCube(6);
    glPopMatrix();
    //quilt
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.1, 0.4, 0.6);//blue
    glTranslatef(14, 6, -13.5);
    glScalef(1.9,0.05,1.4);
    glutSolidCube(6);
    glPopMatrix();
    //pillow
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.5, 0.5);//greenish
    glTranslatef(21, 6, -13.5);
    glScalef(0.4,0.1,0.7);
    glutSolidCube(6);
    glPopMatrix();
    //base
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);//silver
    glTranslatef(15.5, 4, -14.0);
    glScalef(2.5,0.05,1.8);
    glutSolidCube(6);
    glPopMatrix();
    //base feet
    //1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(8.5, 2, -9);
    glScalef(0.1,2,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(8.5, 2, -19);
    glScalef(0.1,2,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(22.5, 2, -9);
    glScalef(0.1,2,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(22.5, 2, -19);
    glScalef(0.1,2,0.1);
    glutSolidCube(3);
    glPopMatrix();
    
    //draw a bigger desk
    //base(bottom)
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);//silver
    glTranslatef(2.2, 5, -16.0);
    glScalef(1.8,0.05,1.1);
    glutSolidCube(6);
    glPopMatrix();
    //base(top)
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.2, 0.0, 0.0);//dark red
    glTranslatef(2.2, 5.2, -16.0);
    glScalef(1.8,0.03,1.1);
    glutSolidCube(6);
    glPopMatrix();
    //base feet
    //1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(-3.05, 2.5, -12.9);
    glScalef(0.1,1.7,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(7.45, 2.5, -12.9);
    glScalef(0.1,1.7,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(-3.05, 2.5, -19);
    glScalef(0.1,1.7,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(7.45, 2.5, -19);
    glScalef(0.1,1.7,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //5
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(-3.05, 3.5, -16);
    glScalef(0.1,0.1,2);
    glutSolidCube(3);
    glPopMatrix();
    //6
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(7.45, 3.5, -16);
    glScalef(0.1,0.1,2);
    glutSolidCube(3);
    glPopMatrix();
    //7
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(-3.05, 1, -16);
    glScalef(0.1,0.1,2);
    glutSolidCube(3);
    glPopMatrix();
    //8
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(7.45, 1, -16);
    glScalef(0.1,0.1,2);
    glutSolidCube(3);
    glPopMatrix();
    //9
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(2.2, 1, -16);
    glScalef(3.5,0.1,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //10
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.41, 0.4, 0.41);
    glTranslatef(2.2, 1, -19);
    glScalef(3.5,0.1,0.1);
    glutSolidCube(3);
    glPopMatrix();
    //11
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.15, 0.0, 0.0);//dark red
    glTranslatef(2.2, 1, -17.5);
    glScalef(1.7,0.05,0.35);
    glutSolidCube(6);
    glPopMatrix();
    
    //objects in bathroom
    //cabinet
        //bottom
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.1, 0.0, 0.0);//dark red
    glTranslatef(-27.5, 2, -5.0);
    glScalef(0.8,1,1.7);
    glutSolidCube(6);
    glPopMatrix();
        //top
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.0);//dark
    glTranslatef(-27.5, 5.2, -5.0);
    glScalef(0.8,0.05,1.7);
    glutSolidCube(6);
    glPopMatrix();
    //hand washing sink and faucet
    glPushMatrix();
    glTranslatef(-27, 3.36, -5);
        //sink
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.6);//dark
    glTranslatef(0, 2, 0);
    glScalef(0.25,0,0.5);
    glRotatef(90, 1, 0, 0);
    glutSolidCone(3, 1, 30, 30);
    glPopMatrix();
        //faucet
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);//dark
    glTranslatef(-1.5, 2, 0);
    glScalef(0.5,0.8,0.5);
    glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.45, 0.45, 0.45);//dark
    glTranslatef(-1, 2.5, 0);
    glScalef(1.4,0.2,0.5);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();
    
    //mirror cube
    if(!roomlight) glEnable(GL_LIGHTING);
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.6);//white
    glTranslatef(-27.5, 9, -5.0);
    glScalef(0.5,1,0.8);
    glutSolidCube(3);
    glPopMatrix();
    glDisable(GL_LIGHTING);
    
    //tolit
    glPushMatrix();
    glTranslatef(-20, 0, 6);
    //bottom
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.6);
    glTranslatef(0, 1, 0);
    glScalef(2,1,3);
    glutSolidCube(1);
    glPopMatrix();
    //back
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.6);
    glTranslatef(0, 3, 2);
    glScalef(2,4,1);
    glutSolidCube(1);
    glPopMatrix();
    //seat
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.6);
    glTranslatef(0, 2.5, 0);
    glRotatef(90, 1, 0, 0);
    glScalef(0.5, 0.75, 0.5);
    glutSolidCone(3, 5, 30, 30);
    glPopMatrix();
    //inner
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.9, 0.9, 0.9);
    glTranslatef(0, 2.5, 0);
    glRotatef(90, 1, 0, 0);
    glScalef(0.5, 0.75, 0.5);
    glutSolidCone(2.5, 4, 30, 30);
    glPopMatrix();
    //water
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 1);
    glTranslatef(0, tWater, 0);
    glRotatef(90, 1, 0, 0);
    glScalef(0.75,0.75,0.75);
    glScalef(0.5, 0.75, 0.5);
    glutSolidCone(2.5, 1, 30, 30);
    glPopMatrix();
    //button
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0, 0.5, 0.3);//green
    glTranslatef(1.1, 4, 2);
    glScalef(0.25, 0.25, 0.5);
    glutSolidCube(1);
    glPopMatrix();
    //lid
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.9, 0.9, 0.9);
    glTranslatef(0, 2.5, 0);
    glScalef(0.5, 0.1, 0.75);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.5, 2.5, 30, 30);
    glPopMatrix();
    glPopMatrix();//tolit end
    
    //tolit paper set
    glPushMatrix();
    glTranslatef(-15.5, 2, 3);
    //base
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(0, 2, 0);
    glScalef(0.1, 1, 1);
    glutSolidCube(1);
    glPopMatrix();
    //1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(-0.4, 2, 0.4);
    glScalef(0.8, 0.1, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(-0.4, 2, -0.4);
    glScalef(0.8, 0.1, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.4, 0.4);
    glTranslatef(-0.75, 2, 0);
    glScalef(0.1, 0.1, 0.8);
    glutSolidCube(1);
    glPopMatrix();
    //paper row
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.9, 0.9, 0.8);
    glTranslatef(-0.75, 2, 0);
    glScalef(0.1, 0.1, 0.25);
    glutSolidTorus(1.5, 3, 30, 30);
    glPopMatrix();
    //paper piece
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.9, 0.9, 0.8);
    glTranslatef(-1.08, 1.7, 0);
    glScalef(0.03, 0.4, 0.8);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();//end tolit paper
    
    //shower cubicle
    glPushMatrix();
    glTranslatef(-17, 0, -15);
    //glTranslatef(2, 0, 0);
    //base
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.8, 0.8, 0.8);
    glTranslatef(0, -4, 0);
    glScalef(1, 1, 0.95);
    glutSolidCube(10);
    glPopMatrix();
    //wall right
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.7, 0.7, 0.7);
    glTranslatef(5.2, 5, 0);
    glScalef(0.05, 1.3, 0.95);
    glutSolidCube(10);
    glPopMatrix();
    //wall left
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.7, 0.7, 0.7);
    glTranslatef(-5.2, 5, 0);
    glScalef(0.05, 1.3, 0.95);
    glutSolidCube(10);
    glPopMatrix();
    //wall back
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.65, 0.65, 0.65);
    glTranslatef(0, 5, -5.1);
    glScalef(1.1, 1.3, 0.05);
    glutSolidCube(10);
    glPopMatrix();
    //wall front (low)
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.8, 0.8, 0.8);
    glTranslatef(0, 0.5, 4.5);
    glScalef(1, 0.2, 0.05);
    glutSolidCube(10);
    glPopMatrix();
    //drain
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(0, 1, 0);
    glScalef(1, 0.01, 1);
    glutSolidSphere(0.5, 40, 40);
    glPopMatrix();
    //button
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.7);
    glTranslatef(-5, 6, 0);
    glScalef(1, 1, 1);
    glutSolidCube(0.5);
    glPopMatrix();
    //shower nozzle
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.9, 0.8, 0.7);
    glTranslatef(-4.5, 12, 0);
    glScalef(0.5, 0.5, 0.5);
    glRotatef(90, 1, 0, 0);
    glRotatef(135, 0, -1, 0);
    glutSolidCone(1, 5, 50, 50);
    glPopMatrix();
    //water lines
    if(shower){
        //1
        glPushMatrix();
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.3, 0.8, 0.8);
        glTranslatef(wX1, wY1, 0);
        glRotatef(45, 0, 0, 1);
        glScalef(0.05, 1, 0.05);
        glutSolidCube(1);
        glPopMatrix();
        //2
        glPushMatrix();
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.3, 0.8, 0.8);
        glTranslatef(wX2, wY2, -0.5);
        glRotatef(45, 0, 0, 1);
        glScalef(0.05, 1, 0.05);
        glutSolidCube(1);
        glPopMatrix();
        //3
        glPushMatrix();
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.3, 0.8, 0.8);
        glTranslatef(wX3, wY3, 0.5);
        glRotatef(45, 0, 0, 1);
        glScalef(0.05, 1, 0.05);
        glutSolidCube(1);
        glPopMatrix();
        //4
        glPushMatrix();
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.3, 0.8, 0.8);
        glTranslatef(wX4, wY4, -0.3);
        glRotatef(45, 0, 0, 1);
        glScalef(0.05, 1, 0.05);
        glutSolidCube(1);
        glPopMatrix();
        //5
        glPushMatrix();
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.3, 0.8, 0.8);
        glTranslatef(wX5, wY5, -0.1);
        glRotatef(45, 0, 0, 1);
        glScalef(0.05, 1, 0.05);
        glutSolidCube(1);
        glPopMatrix();
        
        //particles
        int i;
        glPushMatrix();
        glTranslatef(0, 6.5, 1);
        glScalef(4, 4, 4);
        glBegin(GL_POINTS); /* render all particles */
        for(i=0; i<num_particles; i++)
        {
           glColor3fv(colors[particles[i].color]);
           glVertex3fv(particles[i].position);
        }
        glEnd();
        glPopMatrix();
        myinit();
    }
    
    glPopMatrix(); //end cubicle
    
    //blinds
    glPushMatrix();
    glTranslatef(24.3, 8.5, -2);
    //top
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, 7.9, 0);
    glScalef(0.3, 0.03, 6);
    glutSolidCube(3);
    glPopMatrix();
    //rope1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.3);
    glTranslatef(0, 3.6, 0);
    glScalef(0.01, 2.8, 0.01);
    glutSolidCube(3);
    glPopMatrix();
    //rope2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.3);
    glTranslatef(0, 3.6, 4);
    glScalef(0.01, 2.8, 0.01);
    glutSolidCube(3);
    glPopMatrix();
    //rope3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.3);
    glTranslatef(0, 3.6, -4);
    glScalef(0.01, 2.8, 0.01);
    glutSolidCube(3);
    glPopMatrix();
    //rope4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.3);
    glTranslatef(0, 3.6, 8);
    glScalef(0.01, 2.8, 0.01);
    glutSolidCube(3);
    glPopMatrix();
    //rope5
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.6, 0.6, 0.3);
    glTranslatef(0, 3.6, -8);
    glScalef(0.01, 2.8, 0.01);
    glutSolidCube(3);
    glPopMatrix();
    //(10 pieces)
    //1
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh1, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //2
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh2, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //3
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh3, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //4
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh4, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //5
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh5, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //6
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh6, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //7
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh7, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //8
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh8, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //9
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh9, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //10
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, bh10, 0);
    glRotatef(bangle, 0, 0, -1);
    glScalef(0.3, 0.025, 6);
    glutSolidCube(3);
    glPopMatrix();
    //bottom
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.4, 0.3, 0.3);
    glTranslatef(0, -0.3, 0);
    glScalef(0.3, 0.03, 6);
    glutSolidCube(3);
    glPopMatrix();
    
    glPopMatrix();//end blinds
    
    //closet
    //frame
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.75, 0.75, 0.75);
    glTranslatef(-7.5, 5.4, -11.73);
    glScalef(1, 2.2, 0.3);
    glLineWidth(10);
    glutWireCube(5);
    glLineWidth(1);
    glPopMatrix();
    //left side
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.75, 0.75, 0.75);
    glTranslatef(-8.7, 5.45, -11);
    glScalef(0.46, 2.1, 0.01);
    glutSolidCube(5);
    glPopMatrix();
    //right side
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.75, 0.75, 0.75);
    glTranslatef(-6.3, 5.45, -11);
    glScalef(0.46, 2.1, 0.01);
    glutSolidCube(5);
    glPopMatrix();
    //doorknob right
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-7, 5.45, -11);
    glScalef(0.3, 0.3, 0.3);
    glutSolidCube(1);
    glPopMatrix();
    //doorknob right
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-6.7, 5.45, -10.9);
    glScalef(0.6, 0.15, 0.05);
    glutSolidCube(1);
    glPopMatrix();
    //doorknob left
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-8, 5.45, -11);
    glScalef(0.3, 0.3, 0.3);
    glutSolidCube(1);
    glPopMatrix();
    //doorknob left
    glPushMatrix();
    if(dark) glColor3f(0, 0, 0);
    else glColor3f(0.3, 0.3, 0.3);
    glTranslatef(-8.3, 5.45, -10.9);
    glScalef(0.6, 0.15, 0.05);
    glutSolidCube(1);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    //end drawing
    
}

void drawShapes()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    gluLookAt(meX, meY, meZ, meX-sin(angle*PI/180), meY+viewAngle, meZ-cos(angle*PI/180), 0.0, 1.0, 0.0);
    
    if (roomlight == true && window == false) dark = true;
    else dark = false;
    
    //color picking hiden object for door1
    glPushMatrix();
    glTranslatef(0,5,10);
    glScalef(1.7, 3.4, 0);
    if (selecting)
    {
        glColor3f(0.0, 1.0, 0.0);
        glutSolidCube(3); // Solid Green Box.
    }
    else
    {
        glColor3f(0.9, 0.9, 0.9);
        glutWireCube(3); // white Wire Box.
    }
    glPopMatrix();
    
    //color picking hiden object for door2
    glPushMatrix();
    glTranslatef(-11.5,5,-5);
    glScalef(0.1, 3.4, 1.7);
    if (selecting)
    {
        glColor3f(0.5, 0.0, 0.0);
        glutSolidCube(3); // Solid Green Box.
    }
    else
    {
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.9, 0.9, 0.9);
        glutWireCube(3); // White door.
    }
    glPopMatrix();
    
    //color picking hiden object for bathroom light
    glPushMatrix();
    glTranslatef(-29,11.5,-5);
    glScalef(0.2, 0.2, 2);
    if (selecting)
    {
        glColor3f(0.3, 0.0, 0.1);
        glutSolidCube(3);
    }
    else
    {
        glColor3f(1, 1, 1);
        if(roomlight) glutWireCube(3);
        else glutSolidCube(3);
    }
    glPopMatrix();
    
    //color picking hiden object for window
    glPushMatrix();
    glTranslatef(24.2,12,-2);
    glScalef(0.7,3,6);
    if (selecting)
    {
        glColor3f(1.0, 0.0, 0.0);
        glutSolidCube(3); // Solid red Box.
    }
//    else
//    {
//        glColor3f(1.0, 0.5, 0.0);
//        glutWireCube(3); // Orange Wire Box.
//    }
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
    
    //switch base
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);
    glTranslatef(6, 7, 10);
    glScalef(0.8, 0.3, 0.1);
    glutSolidCube(1);
    glPopMatrix();
    //draw a button for roomlight
    glPushMatrix();
    glColor3f(0.7, 0, 0);
    glTranslatef(5.8, 7.0, 9.8);
    glScalef(0.1, 0.1, 0.3);
    glutSolidCube(1);
    glPopMatrix();
    //button base
    glPushMatrix();
    glColor3f(0, 0, 0.7);
    glTranslatef(6.2, 7.0, 9.8);
    glScalef(0.1, 0.1, 0.3);
    glutSolidCube(1);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    
    // Material property vectors.
    float matAmbAndDif1[] = {0.6, 0.7, 0.8, 1.0}; //The color of outside wall
    float matAmbAndDif2[] = {outlight, outlight, outlight, 1.0}; //The color of inside wall
    float matAmbAndDif3[] = {1.0, 1.0, 0.8, 1.0}; //The color of roomlight
    float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float matShine[] = { 50.0 };
    
    // Material properties of the box.
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
     glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
     glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif2);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
     glEnable(GL_NORMALIZE);
     
     
     if (roomlight) glDisable(GL_LIGHT0);
     else glEnable(GL_LIGHT0);
    
     // Unveraged normals for the house
        glPushMatrix();
        glTranslatef(0, 10.1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(90, 0, -1, 0);
        glScalef(5, 5, 5);
     
//Make all the walls
        //north wall
       glPushMatrix();
       glTranslatef(0, -2, 0.5);
       glScalef(1, 1, 2.75);
       glCallList(aSide);
       glPopMatrix();
        
    //east wall
        //outer walls
            //bottom
       glPushMatrix();
       glRotatef(90.0, 1.0, 0.0, 0.0);
       glTranslatef(1.25, -3, 1);
       glScalef(0.4, 1, 1.5);
       glCallList(aSide);
       glPopMatrix();
            //top
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-1.63, -3, 1);
        glScalef(0.18, 1, 1.5);
        glCallList(aSide);
        glPopMatrix();
            //right
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-0.41, -3, -1.7);
        glScalef(0.43, 1, 0.15);
        glCallList(aSide);
        glPopMatrix();
            //left
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-0.41, -3, 3.1);
        glScalef(0.43, 1, 0.45);
        glCallList(aSide);
        glPopMatrix();
        
        //inner walls
            //bottom
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(1.25, -2.7, 1);
        glScalef(0.4, 1, 1.5);
        glCallList(aSide);
        glPopMatrix();
            //top
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-1.63, -2.7, 1);
        glScalef(0.18, 1, 1.5);
        glCallList(aSide);
        glPopMatrix();
            //right
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-0.41, -2.7, -1.7);
        glScalef(0.43, 1, 0.15);
        glCallList(aSide);
        glPopMatrix();
            //left
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-0.41, -2.7, 3.1);
        glScalef(0.43, 1, 0.45);
        glCallList(aSide);
        glPopMatrix();
    
        //window walls
            //left
        glPushMatrix();
        glTranslatef(-0.5, -0.2, -4.85);
        glScalef(0.45, 1, 0.075);
        glCallList(aSide);
        glPopMatrix();
            //right
        glPushMatrix();
        glRotatef(180.0, 1.0, 0.0, 0.0);
        glScalef(0.45, 1, 0.075);
        glTranslatef(-1, 0.6, 64.65);
        glCallList(aSide);
        glPopMatrix();
            //top
        glPushMatrix();
        glRotatef(270.0, 0.0, 0.0, 1.0);
        glTranslatef(0.5, 0.7345, -4.85);
        glScalef(1, 1, 0.075);
        glCallList(aSide);
        glPopMatrix();
    
    //west wall
       //first depth
         //left
       glPushMatrix();
       glRotatef(270.0, 1.0, 0.0, 0.0);
       glScalef(1, 1, 0.5);
       glTranslatef(0, 1.3, 2);
       glCallList(aSide);
       glPopMatrix();
         //right
       glPushMatrix();
       glRotatef(270.0, 1.0, 0.0, 0.0);
       glScalef(1, 1, 0.45);
       glTranslatef(0, 1.3, -6.88);
       glCallList(aSide);
       glPopMatrix();
         //top
       glPushMatrix();
       glRotatef(270.0, 1.0, 0.0, 0.0);
       glScalef(0.33, 1, 0.55);
       glTranslatef(-4, 1.3, -2);
       glCallList(aSide);
       glPopMatrix();
        
       //second depth (aisle)
        //left wall
        glPushMatrix();
        glRotatef(180.0, 1.0, 0.0, 0.0);
        glScalef(1, 1, 0.6);
        glTranslatef(0, 2, -3.1665);
        glCallList(aSide);
        glPopMatrix();
        //right wall
        glPushMatrix();
        glTranslatef(0, -0.2, 1.5);
        glScalef(1, 1, 0.4);
        glCallList(aSide);
        glPopMatrix();
        //ceiling wall
        glPushMatrix();
        glRotatef(270.0, 0.0, 0.0, 1.0);
        glTranslatef(1.0, 1.34, 3.35);
        glScalef(1.5, 1, 1.325);
        glCallList(aSide);
        glPopMatrix();
    
        //third depth (bathroom door wall)
        //bedroom side
        //top
        glPushMatrix();
        glRotatef(270.0, 1.0, 0.0, 0.0);
        glScalef(0.18, 0.5, 0.55);
        glTranslatef(-2.0, -2.55, -2);
        glCallList(aSide);
        glPopMatrix();
        //left side
        glPushMatrix();
        glRotatef(270.0, 1.0, 0.0, 0.0);
        glScalef(0.5, 0.5, 0.5);
        glTranslatef(2, -2.55, 1);
        glCallList(aSide);
        glPopMatrix();
        //right
        glPushMatrix();
        glRotatef(270.0, 1.0, 0.0, 0.0);
        glScalef(0.5, 0.5, 0.5);
        glTranslatef(2, -2.55, -5);
        glCallList(aSide);
        glPopMatrix();
    
        //fourth depth (bathroom)
        //left wall
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(0, 4.3, -0.75);
        glScalef(1, 1, 0.62);
        glCallList(aSide);
        glPopMatrix();
        //right wall
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(0, 4.3, 2.75);
        glScalef(1, 1, 0.625);
        glCallList(aSide);
        glPopMatrix();
        //top wall
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(-1.0, 4.3, 1.0);
        glScalef(0.5, 1, 0.26);
        glCallList(aSide);
        glPopMatrix();
    
    //tolit walls
        //left side
        glPushMatrix();
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glTranslatef(0, 5.095, -1);
        glScalef(1, 1, 0.5);
        glCallList(aSide);
        glPopMatrix();
        //right side1
        glPushMatrix();
        glRotatef(270.0, 1.0, 0.0, 0.0);
        glTranslatef(0, -3, 1);
        glScalef(1, 1, 0.5);
        glCallList(aSide);
        glPopMatrix();
        //right side2
        glPushMatrix();
        glRotatef(180.0, 1.0, 0.0, 0.0);
        glScalef(1, 1, 0.25);
        glTranslatef(0, 2, -22);
        glCallList(aSide);
        glPopMatrix();
    
    //shower cubicle walls
        //left side1
        glPushMatrix();
        glRotatef(270.0, 1.0, 0.0, 0.0);
        glTranslatef(0, -2.5, -3);
        glScalef(1, 1, 0.5);
        glCallList(aSide);
        glPopMatrix();
        //left side2
        glPushMatrix();
        glTranslatef(0, -0.0, 5.25);
        glScalef(1, 1, 0.375);
        glCallList(aSide);
        glPopMatrix();
    
       //deepest wall
       glPushMatrix();
       glRotatef(270.0, 1.0, 0.0, 0.0);
       glTranslatef(0, -4, -1);
       glScalef(1, 1, 1.5);
       glCallList(aSide);
       glPopMatrix();
        
        //floor
       glPushMatrix();
       glRotatef(90.0, 0.0, 0.0, 1.0);
       glTranslatef(-1, 0, 0.5);
       glScalef(1.5, 1, 2.75);
       glCallList(aSide);
       glPopMatrix();
        
        //ceiling
       glPushMatrix();
       glRotatef(270.0, 0.0, 0.0, 1.0);
       glTranslatef(1, 0, 0.5);
       glScalef(1.5, 1, 2.75);
       glCallList(aSide);
       glPopMatrix();

        //door1
       glPushMatrix();
       glTranslatef(0.0, 2, 0.5);
       glRotatef((float)step, -1, 0.0, 0.0);
       glTranslatef(0.0, -2, -0.5);
       glRotatef(180.0, 1.0, 0.0, 0.0);
       glScalef(0.5, 1, 0.25);
       glTranslatef(2, 0, 0);
       glCallList(aSide);
       glPopMatrix();
        
        //door2
        glPushMatrix();
        glDisable(GL_LIGHTING);
        glTranslatef(1, -1, 2.3);
        glTranslatef(0.0, -0.5, 0);
        glRotatef((float)step3, 1, 0.0, 0.0);
        glTranslatef(0.0, 0.5, 0);
        glScalef(2,1,0.05);
        if(dark) glColor3f(0, 0, 0);
        else glColor3f(0.8, 0.8, 0.7);
        glutSolidCube(1);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    
    //south wall
        //middle wall up the door
        glPushMatrix();
        glRotatef(180.0, 1.0, 0.0, 0.0);
         glScalef(0.5, 1, 0.26);
         glTranslatef(-2, 0, 0);
        glCallList(aSide); // front up wall.
        glPopMatrix();
        //left wall
        glPushMatrix();
        glRotatef(180.0, 1.0, 0.0, 0.0);
         glScalef(1, 1, 1.375);
         glTranslatef(0, 0, -2.37);
        glCallList(aSide); // front left wall.
        glPopMatrix();
        //right wall
        glPushMatrix();
        glRotatef(180.0, 1.0, 0.0, 0.0);
         glScalef(1, 1, 1.125);
         glTranslatef(0, 0, 2.445);
        glCallList(aSide); // front right wall.
        glPopMatrix();
        
        glPopMatrix();

     glDisable(GL_LIGHTING);
     
     //draw the shadow of objects
     glPushMatrix();
     glScalef(1.0, 0.001, 1.0);
     object(1); //shadow
     glPopMatrix();
     
     glEnable(GL_LIGHTING);
     
     glEnable(GL_DEPTH_TEST); // Restore depth testing.
     
     // Draw real objects
     object(0);
     
    // Material properties of the roomlight.
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif3);
     // Cull the back faces of the sphere.
     glEnable(GL_CULL_FACE);
     glCullFace(GL_FRONT);
     //roomlight.
     glPushMatrix();
     glTranslatef(7, 19, -3);
     glutSolidSphere(1, 70, 70);
     glPopMatrix();
    glDisable(GL_CULL_FACE);
    
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    // Blend the grass texture onto a rectangle lying along the xz-plane.
    glBlendFunc(GL_ONE, GL_ZERO); // Specify blending parameters to overwrite background.
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_POLYGON);
       glNormal3f(0.0, 1.0, 0.0);
       glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 0.0, 100.0);
       glTexCoord2f(8.0, 0.0); glVertex3f(100.0, 0.0, 100.0);
       glTexCoord2f(8.0, 8.0); glVertex3f(100.0, 0.0, -100.0);
       glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, 0.0, -100.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    if (Cid != -1){
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ZERO); // Specify blending parameters to overwrite background.
        glBindTexture(GL_TEXTURE_2D, texture[Cid]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.5, 7.0, -15.2);
        glTexCoord2f(1.0, 0.0); glVertex3f(4.5, 7.0, -15.2);
        glTexCoord2f(1.0, 1.0); glVertex3f(4.5, 10.0, -15.2);
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.5, 10.0, -15.2);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    glDisable(GL_LIGHT0);
    //end texture
    
    if(powerline){
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        // Specify and enable the Bezier curve.
        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 6, controlPoints[0]);
        glEnable(GL_MAP1_VERTEX_3);
        glPushMatrix();
        glTranslatef(5, 4.5, -17);
        glScalef(0.1, 0.1, 1);
        // Draw the Bezier curve by defining a sample grid and evaluating on it.
        glColor3f(0.0, 0.0, 0.0);
        glMapGrid1f(40, 0.0, 1.0);
        glEvalMesh1(GL_LINE, 10, 30);
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
}


void drawScene()
{
    //draw everything with special coloring for selecting
    //but don't display on computer screen.
    if(selecting)
    {
        drawShapes();
        getID(xClick,yClick);
    }
    //draw everything
    else
    {
        drawShapes();
        glutSwapBuffers();
    }
}


// OpenGL window reshape routine.
void resize (int w, int h)
{
   glViewport (0, 0, (GLsizei)w, (GLsizei)h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, (float)w/(float)h, 1.0, 300.0);
   glMatrixMode(GL_MODELVIEW);
    height = h;
    width = w;
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch (key) 
   {
      case 27:
         exit(0);
         break;
      case 'd':
         door1 = !door1;
         glutPostRedisplay();
         break;
      case 'D':
         door2 = !door2;
         glutPostRedisplay();
         break;
      case 'l':
         roomlight = !roomlight;
         glutPostRedisplay();
         break;
      case 's':
          shower = !shower;
          glutPostRedisplay();
          break;
      case 'w':
         window = !window;
         glutPostRedisplay();
         break;
      case 't':
         tolit = !tolit;
         glutPostRedisplay();
         break;
      case 'f':
         fan = !fan;
         glutPostRedisplay();
         break;
     case '+':
         if(fanPeriod >5) fanPeriod -= 5;
         glutPostRedisplay();
         break;
    case '-':
        fanPeriod += 5;
        glutPostRedisplay();
        break;
    case 'r':
        drawer = !drawer;
        glutPostRedisplay();
        break;
    case 'b':
        powerline = !powerline;
        glutPostRedisplay();
        break;
           
      default:
         break;
   }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    switch(key){
        case GLUT_KEY_UP: //forward
            meZ -= stepsize*cos(angle*PI/180);
            meX -= stepsize*sin(angle*PI/180);
            break;
        case GLUT_KEY_DOWN: //back
            meZ += stepsize*cos(angle*PI/180);
            
            meX += stepsize*sin(angle*PI/180);
            break;
        case GLUT_KEY_RIGHT: //turn right
            angle -= turnsize;
            break;
        case GLUT_KEY_LEFT: //turn left
            angle += turnsize;
            break;
        case GLUT_KEY_PAGE_DOWN:
            if (viewAngle > -1.0) viewAngle -= 0.1;
            break;
        case GLUT_KEY_PAGE_UP:
            if (viewAngle < 1.0) viewAngle += 0.1;
            break;
    }
   glutPostRedisplay();
}

void mouseControl(int button, int state, int x, int y)
{
    if(state==GLUT_DOWN && button == GLUT_LEFT)
    {   selecting=true;
        xClick=x;
        yClick=height-y; //for screen vs mouse coordinates
        glutPostRedisplay();
    }
}

// The top menu callback function.
void top_menu(int id)
{
   if (id==1) exit(0);
}

// The sub-menu callback function.
void color_menu(int id)
{
    if (id==2) Cid = 0;
    if (id==3) Cid = 1;
    if (id==4) Cid = -1;
   glutPostRedisplay();
}

// Routine to make the menu.
void makeMenu(void)
{
   // The sub-menu is created first (because it should be visible when the top
   // menu is created): its callback function is registered and menu entries added.
   int sub_menu;
   sub_menu = glutCreateMenu(color_menu);
   glutAddMenuEntry("Grass", 2);
   glutAddMenuEntry("Rocket", 3);
   glutAddMenuEntry("Close Screen", 4);

   // The top menu is created: its callback function is registered and menu entries,
   // including a submenu, added.
   glutCreateMenu(top_menu);
   glutAddSubMenu("Screen", sub_menu);
   glutAddMenuEntry("Quit",1);

   // The menu is attached to a mouse button.
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   cout << "Interaction:" << endl;
   cout << "Press arrow keys to move or turn." << endl
	    << "Press page up/down to look up/down." << endl;
}

// Main routine.
int main(int argc, char **argv) 
{
   printInteraction();
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
   glutInitWindowSize (1000, 800);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Chengyi's Room.cpp");
   setup();
   glutDisplayFunc(drawScene);
   glutReshapeFunc(resize);
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);
   glutMouseFunc(mouseControl);
   glutTimerFunc(5, animate, 1);
   glutTimerFunc(5, animate2, 1);
   glutTimerFunc(5, animate3, 1);
   makeMenu();
    glutIdleFunc(myIdle);
   glutMainLoop();
   
   return 0;
}
