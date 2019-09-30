#include <iostream>
#include <stdlib.h>
#include <math.h>
#include<stdio.h>
#include<stdbool.h>
#include <GL/glut.h>
using namespace std;
static int window;
static int value=0;
static int menuid;
int loop;

#define MAX_PARTICLES 1000
float slowdown = 2.0;
float velocity = 0.0;
float zoom = -39.0;

bool snow=false, rain=false;
typedef struct {
  // Life
  bool alive;	// is the particle alive?
  float life;	// particle lifespan
  float fade; // decay
  // color
  float red;
  float green;
  float blue;
  // Position/direction
  float xpos;
  float ypos;
  float zpos;
  // Velocity/Direction, only goes down in y dir
  float vel;
  // Gravity
  float gravity;
}particles;

// Paticle System
particles par_sys[MAX_PARTICLES];

GLfloat pos[] = {0, 0, 0, 1}, //light position
emission[] = {0, 0, 0, 1},
emission_default[] = {0, 0, 0, 1},
amb[] = {.4, .4, .4, 1.0}; //ambient intensity
GLfloat front_amb_diff[] = {.7, .5, .1, 1.0}; //front side property
//GLfloat back_amb_diff[] = {.4, .7, .1, 1.0}; //back side property
GLfloat back_amb_diff[] = {0.5, 0.5, 0.5, 1.0}; //back side property
GLfloat spe[] = {.2, .2, .2, 1.0}; //property for front and back
GLfloat dr = 0, moonHorizontal = 0, moonVertical = 0, snowmanMove = 0;
//dr=rotation angle for big snowman, moonHorizontal=tansition value for moon horizontally,
//moonVertical=transition value for moon vertically, snowmanMove = rotation value for small snowman idle annimation
bool rotateRight = false, rotateLeft = false, movement = true,
goDown = false, moon = false, snowmanMovement = true, sLeft = true,
sRight = false;


//method to make ground have snow
void snowland(void){
    //need to disable lighting and depth test
    //so creation is not affected
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
       glColor3f(.9, .9, .9);
        glVertex2f(-2.3, -.88);
        glVertex2f(2.3, -.88);
        glVertex2f(2.3, -2.3);
        glVertex2f(-2.3, -2.3);
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void land(float Rland, float Gland, float Bland)
{
 glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
       glColor3f(Rland, Gland, Bland);
        glVertex2f(-2.3, -.88);
        glVertex2f(2.3, -.881);
        glVertex2f(2.3, -2.33);
        glVertex2f(-2.3, -2.3);
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}


//method to create a house
void house(void){
    glRotated(-20, 0, 1, 0);
    //roof
    glPushMatrix();
    glColor3f(.388, .2, .0039);
    glScaled(.5, .5, .5);
    glRotated(45, 0, 1, 0);
    glutSolidOctahedron();
    glPopMatrix();
    //house
    glColor3f(.871, .612, .416);
    glTranslated(0, -.38, 0);
    glutSolidCube(.73);
    //windows
    glColor3f(0, 0, 0);
    glTranslated(-.2, .13, .32);
    glutSolidCube(.12);
    glTranslated(.4, 0, 0);
    glutSolidCube(.12);
    //door
    glTranslated(-.2, -.355, .046);
    glScaled(.5, 1.1, 0);
    glutSolidCube(.23);
}

//method to create a tree
void tree(float Rtree, float Gtree, float Btree ){
    //trunk
    glPushMatrix();
    glColor3f(.388, .2, .0039);
    
    GLUquadric* qobj = gluNewQuadric(); //cylinder trunk
    glRotated(90, 1, 0, 0);
    gluCylinder(qobj, .05, .05, .4, 30, 30);
    glPopMatrix();
    //tree leaves
    //glColor3f(0, .415, .0156);
    glColor3f(Rtree, Gtree, Btree);
    glTranslated(0, -.23, 0);
    glRotated(-90, 1, 0, 0);
    glutSolidCone(.3, .3, 40, 40);
    glTranslated(0, 0, .1);
    glutSolidCone(.25, .3, 40, 40);
    glTranslated(0, 0, .1);
    glutSolidCone(.2, .3, 40, 40);
}

//method to create moon/ sun
//the moon/sun is the light source
void moonOrSun(void){
    glTranslated(.05, 0, 0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glutSolidSphere(.4, 40, 40);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission_default);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

//method that will create an entire snowman
void snowman(void){
    //hat
    glTranslated(-.05, 0, 0);
    glPushMatrix();
    glColor3f(1, 1,  1);
    glTranslated(0, .7, 0);
    glutSolidSphere(.04, 40, 40);
    glColor3f(1, 0, 0);
    glTranslated(0, -.30, 0);
    glRotated(-90, 1, 0, 0);
    glutSolidCone(.1, .3, 40, 40);
    glColor3f(1, 1, 1);
    glTranslated(0, 0, -.015);
    glutSolidTorus(.05, .07, 10, 10);
    glPopMatrix();
    //body
    glPushMatrix();
    glColor3f(1, 1,  1);
    glTranslated(0, .2, 0);
    glutSolidSphere(.19, 40, 40);
    glTranslated(0.0, -.45, 0);
    glutSolidSphere(.3, 40, 40);
    glTranslated(0.0, -.6, 0);
    glutSolidSphere(.4, 40, 40);
    glPopMatrix();
    //eyes
    glPushMatrix(); //feature
    glPushMatrix(); //face
    glTranslated(-.07, .3, .15);
    glColor3f(0, 0, 0);
    glutSolidSphere(.02, 40, 40);
    glTranslated(.13, 0, 0);
    glutSolidSphere(.02, 40, 40);
    glColor3f(.8, .3, 0);
    //nose
    glTranslated(-.065, -.06, .028);
    glutSolidCone(.03, .2, 40, 40);
    //mouth
    glColor3f(0, 0, 0);
    glTranslated(-.06, -.08, 0);
    glutSolidSphere(.014, 40, 40);
    glTranslated(.03, -.015, 0);
    glutSolidSphere(.014, 40, 40);
    glTranslated(.03, -.001, 0);
    glutSolidSphere(.014, 40, 40);
    glTranslated(.03, .001, 0);
    glutSolidSphere(.014, 40, 40);
    glTranslated(.03, .015, 0);
    glutSolidSphere(.014, 40, 40);
    glPopMatrix(); //pop face objects
    //body buttons
    glColor3f(0, 0, 0);
    glTranslated(.03, -.09, .26);
    glutSolidSphere(.02, 40, 40);
    glTranslated(0, -.09, .03);
    glutSolidSphere(.02, 40, 40);
    glTranslated(0, -.09, .01);
    glutSolidSphere(.02, 40, 40);
    glPopMatrix(); //pop features objects
    //arms
    glPushMatrix();
    glColor3f(.388, .2, .0039);
    GLUquadric* qobj = gluNewQuadric(); //create cylinder object
    glRotated(45, 0, 0, 1);
    glRotated(90, 0, 1, 0);
    glTranslated(-.04, -.3, 0);
    gluCylinder(qobj, .02, .02, .3, 30, 30);
    glRotated(-90, 1, 0, 0);
    glTranslated(0, .3, .3);
    gluCylinder(qobj, .02, .02, .3, 30, 30);
    glPopMatrix();
}

void initParticles(int i)
 {

    par_sys[i].alive = true;
    par_sys[i].life = 1.0;
    par_sys[i].fade = float(rand()%100)/1000.0f+0.003f;

    par_sys[i].xpos = (float)( (rand() % 350) - 150)/10;
    par_sys[i].ypos = (float)( (rand() % 201) - 80)/100;    
    par_sys[i].zpos = (float) ((rand() % 201) - 5)/10;

    par_sys[i].red = 0.5;
    par_sys[i].green = 0.5;
    par_sys[i].blue = 1.0;

    par_sys[i].vel = velocity;
    par_sys[i].gravity = -0.8;//-0.8;

}


// For Rain
void drawRain() 
{

  float x, y, z;
  
  //glClearColor(rSky, gSky, bSky, 1.0f);
  glLineWidth(3.0);
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+1) 
  {
  
    if (par_sys[loop].alive == true)
    {
     
      x = par_sys[loop].xpos/10;
      y = par_sys[loop].ypos+0.5;
      z = (par_sys[loop].zpos + zoom)+0.5;
	 
     
      glColor3f(0.0, 0.0, 0.8);

      glBegin(GL_LINES);
        glVertex3f(x, y-0.3, z+39);
        glVertex3f(x, y-0.35, z+39);
        
      glEnd();
      y=y*-1;
      
      glBegin(GL_LINES);
        glVertex3f(x-0.05, y-0.9, z+39);
        glVertex3f(x-0.05, y-0.95, z+39);
      glEnd();

      // Update values
      //Move
      // Adjust slowdown for speed!
      
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*0.1);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= 0.2)
       {
        par_sys[loop].life = -1.0;
      }
      //Revive
      if (par_sys[loop].life < 0.0)
       {
        initParticles(loop);
      }
      
    }
  }
}

void drawSnow() 
{

  float x, y, z;
  
  //glClearColor(rSky, gSky, bSky, 1.0f);
  glLineWidth(3.0);
  for (loop = 0; loop < MAX_PARTICLES; loop=loop+1) 
  {
  
    if (par_sys[loop].alive == true)
    {
     
      x = par_sys[loop].xpos/10;
      y = par_sys[loop].ypos+0.5;
      z = (par_sys[loop].zpos + zoom)+0.5;	 
	 
     
      glColor3f(1.0, 1.0, 1.0);

      glBegin(GL_LINES);
        glVertex3f(x, y-0.3, z+39);
        glVertex3f(x, y-0.31, z+39);
        
      glEnd();
      y=y*-1;

      
      glBegin(GL_LINES);
        glVertex3f(x-0.05, y-0.9, z+39);
        glVertex3f(x-0.05, y-0.91, z+39);
      glEnd();

      // Update values
      //Move
      // Adjust slowdown for speed!
      
      par_sys[loop].ypos += par_sys[loop].vel / (slowdown*0.001);
      par_sys[loop].vel += par_sys[loop].gravity;
      // Decay
      par_sys[loop].life -= par_sys[loop].fade;

      if (par_sys[loop].ypos <= 0.2)
       {
        par_sys[loop].life = -1.0;
      }
      //Revive
      if (par_sys[loop].life < 0.0)
       {
        initParticles(loop);
      }
      
    }
  }
}


void scene2(bool snowmanflag,float Rtree,float Gtree, float Btree, float Rland, float Gland, float Bland)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.00, 0.8, 0.816, 0.0);
    land(Rland, Gland,Bland);     
    
    if(!snowmanflag)
    {
    	//tree creation
    glPushMatrix();
    glScaled(2.5, 4, 2.5);
    glTranslated(-0.65, 0, -.3);
    tree(Rtree, Gtree, Btree);
    glPopMatrix();
    
    glPushMatrix();
    glScaled(3, 4, 3);
    glTranslated(-0.5, 0, -.9);
    tree(Rtree, Gtree, Btree);
    glPopMatrix();
    }
    if(snowmanflag)
    {
    //big snow man creation
		glPushMatrix();
		glScaled(1.5, 1.5, 1.5);
		glTranslated(-.8, -.1, -.4);
		glRotated(-15, 0, 1, 0);
		//key command animations
		if(rotateRight){
		    glRotated(dr, 0, 1, 0);
		}
		else if(rotateLeft){
		    glRotated(dr, 0, 1, 0);
		}
		snowman();
		glPopMatrix();
		
    }
    //tree creation
    glPushMatrix();
    glScaled(2.5, 4, 2.5);
    glTranslated(.85, 0, -.3);
    tree(Rtree, Gtree, Btree);
    glPopMatrix();
    
    glPushMatrix();
    glScaled(3, 4, 3);
    glTranslated(.7, 0, -.9);
    tree(Rtree, Gtree, Btree);
    glPopMatrix();
    
    //house creation
    glPushMatrix();
    glTranslated(.45, -.3, -1.5);
    glScaled(1.9, 1.5, 1.4);
    house();
    glPopMatrix();
    
    //moon creation
    glPushMatrix();
    //idle animation for moon
    if(moon==true){
        //glColor3f(.696, .696, .670);
        glColor3f(1.0,1.0,1.0);
    }
    else{
        glColor3f(0.98, 0.3, 0.0);
    }
    glTranslated(-2.7, 1.5, -.5);
    if(movement){
        glTranslated(moonHorizontal, moonVertical, 0);
    }
    moonOrSun();
    glPopMatrix();
    
    

  //glClearColor(rSky, gSky, bSky, 1.0f);
  
    if(rain)
    {
       	for (loop = 0; loop < MAX_PARTICLES; loop++)
       	 {       	 
          initParticles(loop);
   	 }
       	drawRain();       	
    } 
    if(snow)
    {
       	for (loop = 0; loop < MAX_PARTICLES; loop++)
       	 {       	 
         initParticles(loop);
   	 }
       	drawSnow();       	
    }        
    glFlush();
    glutSwapBuffers();
    
}

void drawtext(float x, float y, char *s)
{

	int q;
	glRasterPos2f(x,y);
	for(q=0;s[q]!='\0';q++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,s[q]);
}


void welcome()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.00, 0.8, 0.816, 0.0);
	
	glColor4f(1.0,1.0,1.0,0.5);
    drawtext(-1.13, 1.83,"Bangalore Institute of Technology");
    drawtext(-.603, 1.63,"CGV Mini Project");
    drawtext(-.813, 1.43,"Human nature Interaction");
    drawtext(-.703, 1.23,"Simulation of Seasons");
    
    
    drawtext(-0.503, .43,"Project by:");
    drawtext(-0.903, .23,"Shruthi B 					1BI16CS146");
    drawtext(-.903, .03,"Swathi R 					 1BI16CS165");
    drawtext(-1.903, -1.03,"To switch to day time, press: d");
    drawtext(-1.903, -1.23,"To switch to night time, press: n");
    drawtext(-1.903, -1.43,"In winter, to make the snowman rotate clockwise, press: l ");
    drawtext(-1.903, -1.63,"In winter, to make the snowman rotate anticlockwise, press: k");

	
	glFlush();
    glutSwapBuffers();
		
}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	switch(value)
	{
	case 0:welcome();
			break;			
	case 1:scene2(true, 0.58,0.61,0.22, 0.9,0.9,0.9);	//winter
			snow=true;
			rain=false;
			break;
			
	case 2:scene2(false, 0.3,0.61,0.22, 0.4,0.76,0.22);	//summer
			rain=false;
			snow=false;
			break;
			
	case 3:scene2(false, 0.918,0.516,0.0,0.82,0.35,0.07);	//autumn
			
			rain=false;
			snow=false;
			break;
			
	case 4:scene2(false, 0.195,0.507,0.34, 0.295,0.537,0.34);	//no snowman, green trees, green land	//rainy
			rain=true;
			snow=false;
			break;
	}
    
}

void menu(int num)
{
	if(num==5)
	{
	glutDestroyWindow(window);
	exit(0);
	}
	else value=num;
}

void createmenu(void)
{
	menuid=glutCreateMenu(menu);
	glutAddMenuEntry("Winter",1);
	glutAddMenuEntry("Summer",2);
	glutAddMenuEntry("Autumn",3);
	glutAddMenuEntry("Rainy",4);
	glutAddMenuEntry("Quit",5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void idle(void){
    //rotation for big snowman
    if(rotateRight){
        dr = dr + 2;
    }
    if(rotateLeft){
        dr = dr - 2;
    }
    if((rotateRight==false) && (rotateLeft==false)) dr = 0;
    
    //movement of moon/sun
    moonHorizontal = moonHorizontal + .002;
    if(goDown == false) moonVertical = moonVertical + .0004;
    if(moonHorizontal > 5.5){
       moonHorizontal = 0;
       moonVertical = 0;
        goDown = false;
    }
    if(moonVertical > .45 || goDown == true){
        goDown = true;
        moonVertical = moonVertical - .0004;
    }
    
    //movement for smaller snowmen
    if(sLeft){
        snowmanMove = snowmanMove + 1.5;
        if(snowmanMove>=20){
            sLeft = false;
            sRight = true;
        }
    }
    if(sRight){
     snowmanMove = snowmanMove - 1.5;
        if(snowmanMove<=-20){
            sLeft = true;
            sRight = false;
        }
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
    switch (key){
        case 'k':
            //rotate big snowman counter-clockwise
            rotateRight = true;
            rotateLeft = false;
            break;
        case 'l':
            //rotate big snowman clockwise
            rotateLeft = true;
            rotateRight = false;
            break;
        case 'm':
            //return big snowman to default
            rotateLeft = false;
            rotateRight = false;
            break;
        case 'n':
            //night time
            moon = true;
            //glClearColor(0.03, 0.02, 0.7, 0.0);
            glClearColor(0.28, 0.203, 0.457, 0.0);
            break;
        case 'd':
            //day time
            moon = false;
            //glClearColor(.165, .553, .812, 0);
            //glClearColor(0.148, 0.77, 0.968, 0.0);
            glClearColor(0.00, 0.8, 0.816, 0.0);
            break;
    }
    glutPostRedisplay();
}

void commandInfo(void)
{

    cout<<"To switch to day time, press: d \n";
    cout<<"To switch back to night time, press: n \n";
    cout<<"To make the big snowman rotate clockwise, press: l \n";
    cout<<"To make the big snowman rotate counter-clockwise, press: k \n";
    cout<<"To make the big snowman return to his default animation, press: m \n";
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 600);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Simulation of Seasons");
    glClearColor(0.125, 0.695, 0.664, 0.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1.0, 2, 8);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, front_amb_diff);
    glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, back_amb_diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0, 0, -5); //translate object (& light) into view volume
    glEnable(GL_COLOR_MATERIAL); //to change the colors of 3d generated objects
    glEnable (GL_NORMALIZE); //for scaling down objects
    commandInfo(); //display key command information
    glutKeyboardFunc(keyboard);
    createmenu();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();
}

