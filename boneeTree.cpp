
/*
 *
 *	W programie wykorzystano wybrane rozwiazania opisane w publikacji 
 *  (1) Real Time Design and Animation of Fractal Plants and Trees
 *	Petera Oppenheimera, (2) Structural Simulation of Tree Growth and
 *	response autorstwa Johna Harta i Brenta Bekera oraz (3) w
 *	dokumentacji biblioteki GLUT
 *
 *											Karol Bonenberg, 
*/

#include <windows.h> 
#include "gl\glut.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum { TREE, STEM, LEAF, WHOLETREE, GROUND } DisplayLists;

float lightness=2.0, deltaLight=0.0;
float angle=1.5,deltaAngle = 0.0,height=1.25,deltaHeight = 0.0,ratio;
float x=0.0f,y=1.5f,z=6.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f;
GLint tree_display_list;
int deltaMove = 0;
float red=1.0, blue=1.0, green=1.0;
float a=0.0, b=0.0, r=6.0;
float tmp;
int Level=0;
float Scale=0.5;
int currLevel;
unsigned long k;

// Zmienne w drzewie
float li; //dlugosc galezi
float Ri, ri; // grubosc poczatku / konca galezi
float vi; //objetosc galezi
float Mi, mi; // masa trzymana przez galaz/ masa galezi
float ei; // limit masy
float Gi, gi; // szybkosc wzrostu dla drzewa i odrostow
float Ji; //indeks dziecka
unsigned TreeSeed;   //Przechowujemy seed zeby kazdy poziom nalezal do tego samego drzewa
unsigned oneSeed= time( NULL );

int random() 
{
	return ((float) rand()/(float) 25000);
}

void make_seed()
{
	srand(oneSeed);
}



void changeSize(int w, int h)
	{

	if(h == 0)
		h = 1;

	ratio = 1.0f * w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);

	gluPerspective(45,ratio,1,110);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      0.0f , 1.25f, 0.0f,
			  0.0f,1.0f,0.0f);
	}


 /*
  * Rekursywne rysowanie drzewa w oparciu o algorytm Oppenheiera (1)
  */

void genTree(int level)
{
  currLevel = level;
  long savedseed;  
  if ((level >= Level)) {
      glPushMatrix();
        glRotatef(random()*180, 0, 1, 0);
        glCallList(WHOLETREE);
      glPopMatrix();
  } else {
		glCallList(STEM);
		glPushMatrix();

			glRotatef(random()*180, 0, 1, 0);
			glTranslatef(0, 0.9, 0);
			glScalef(0.8, 0.8, 0.8);
			// Trzy galezie!
		
			make_seed();
			glPushMatrix();    
				glRotatef(110 + random()*40, 0, 1, 0);
				glRotatef(30 + random()*20, 0, 0, 1);
				genTree(level + 1);
			glPopMatrix();
		
			make_seed();
			savedseed = (long) random()*ULONG_MAX;
			glPushMatrix();
				glRotatef(-130 + random()*40, 0, 1, 0);
				glRotatef(30 + random()*20, 0, 0, 1);
				genTree(level + 1);
			glPopMatrix();

			make_seed();
			glPushMatrix();
				glRotatef(-20 + random()*40, 0, 1, 0);
				glRotatef(30 + random()*20, 0, 0, 1);
				genTree(level + 1);
			glPopMatrix();

		glPopMatrix(); }
}

  /*
   * Wg (2) i (3) tworzymy pien, liscie, oraz polaczona strukture
   */
void genLists(void)
{
  GLUquadricObj *cylquad = gluNewQuadric();
  int i;

  glNewList(STEM, GL_COMPILE);
  glColor3f(0.2, 0.1, 0.05f);

  glPushMatrix();
    glRotatef(-90, 1, 0, 0);
	//glNormal3f(1,2,3);
    gluCylinder(cylquad, 0.1, 0.07, 1, 10, 2 );
  glPopMatrix();
  glEndList();

  glNewList(LEAF, GL_COMPILE);  
  glColor3f(0.5, 0.1, 0.0f);
  for (int i=0; i<currLevel; i++) glScalef(1.25, 1.25, 1.25);
  	glScalef(0.5,0.5,0.5);
    glBegin(GL_TRIANGLES);
     // glNormal3f(-0.1, 0, 0.25);  
      glVertex3f(0, 0, 0);
      glVertex3f(0.25, 0.25, 0.1);
      glVertex3f(0, 0.5, 0);

      //glNormal3f(0.1, 0, 0.25);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0.5, 0);
      glVertex3f(-0.25, 0.25, 0.1);
    glEnd();
  glEndList();

  glNewList(WHOLETREE, GL_COMPILE);
  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
    glCallList(STEM);
	if (currLevel>0) {
    for(i = 0; i < 3; i++) {
      glTranslatef(0, 0.333, 0);
      glRotatef(90, 0, 1, 0);
      glPushMatrix();
        glRotatef(0, 0, 1, 0);
        glRotatef(50, 1, 0, 0);
        glCallList(LEAF);
      glPopMatrix();
      glPushMatrix();
        glRotatef(180, 0, 1, 0);
        glRotatef(60, 1, 0, 0);
        glCallList(LEAF);
      glPopMatrix();
	}}
  glPopAttrib();
  glPopMatrix();
  glEndList();
}

 /*
  * Lista gdybysmy chcieli zrobic caly las
  */
void CreateTree(void)
{
  //make_seed();
  genLists();
  glNewList(TREE, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0, 0, 0);
	//Tutaj mamy sam proces WZROSTU DRZEWA JAKO CALOSCI
	glScalef(Scale*1.1, Scale*1.2, Scale*1.1);
    genTree(0);
    glPopMatrix();
  glEndList();  
}


// Obrot dookola osi oraz ustawianie wysokosci ( uzywamy ukladu wsp cylindrycznych)
void orientMe(float ang, float height) {

	x = r * cos(ang);
	z = r * sin(ang);
	y = height;
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      0.0f , 1.25f, 0.0f ,
			  0.0f,1.0f,0.0f);
}


// Poruszanie sie po scenie (zmiana promienia - uzywamy ukl wsp cylindrycznych)

void moveMeFlat(float ang) {

	x = r * cos(ang);
	z = r * sin(ang);
	
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      0.0f , 1.25f, 0.0f ,
			  0.0f,1.0f,0.0f);
}



void ground_wall() {

	int subdiv=1;
	float back=1;
	float dn=(2.0/3)/subdiv;
	float nx=-1.0/3;
	float ny=-1.0/3;
	float dst=1.0/subdiv;
	float s=0;
	float t=0;
	float x=-back;
	float y=-back;
	float dp=(float)2*back/subdiv;
	glEnable(GL_NORMALIZE);
	glColor3f(0.13f, 0.1f, 0.01f);
	glBegin(GL_QUADS);
		for (int i1=0;i1<subdiv;i1++) {
			for (int i2=0;i2<subdiv;i2++) {
				glNormal3f(nx,ny,-1.0/3);
				glTexCoord2f(s,t);
				glVertex3f(x,y,-back);
				glNormal3f(nx+dn,ny,-1.0/3);
				glTexCoord2f(s+dst,t);
				glVertex3f(x+dp,y,-back);
				glNormal3f(nx+dn,ny+dn,-1.0/3);
				glTexCoord2f(s+dst,t+dst);
				glVertex3f(x+dp,y+dp,-back);
				glNormal3f(nx,ny+dn,-1.0/3);
				glTexCoord2f(s,t+dst);
				glVertex3f(x,y+dp,-back);
				nx+=dn;x+=dp;s+=dst;
			}
			nx=-1.0/3;x=-back;s=0;
			ny+=dn;y+=dp;t+=dst;
		}
		glEnd();
}

void CreateGround(void) {
	glNewList(GROUND, GL_COMPILE);
		glScalef(1.0, 0.1, 1.0);
		glPushMatrix();
			ground_wall();
			glRotatef(90,1,0,0);
			ground_wall();
			glRotatef(90,1,0,0);
			ground_wall();
			glRotatef(90,1,0,0);
			ground_wall();
			glRotatef(90,1,0,0);
			glRotatef(90,0,1,0);
			ground_wall();
			glRotatef(180,0,1,0);
			ground_wall();
		glPopMatrix();
		glScalef(1.0, 10.0, 1.0);
	glEndList(); 
}



void renderScene(void) {

	if (deltaMove==1) {
		if (r>4) {
			r=r-0.08;	
			moveMeFlat(angle);
		}
	}
	else if (deltaMove==-1) {
		if (r<75) {
			r=r+0.08;	
			moveMeFlat(angle);		
		}
	}
	if (deltaAngle) {
		angle += deltaAngle;
		orientMe(angle,height);
	}

	if (deltaHeight) {
		height += deltaHeight;
		orientMe(angle,height);
	}

	if (deltaLight) {
		lightness += deltaLight;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

  float LightDiffuse[]   = { lightness, lightness, lightness, 1.0 } ; // kolor œwiat³a rozproszonego
  float LightPosition[]  = { 0.0, 1.0,10.0, 1.0 } ; // pozycja Ÿród³a œwiat³a
  float LightDirection[] = { 0.0, 0.0,-1.0 } ;      // kierunek
  float Material[]       = { 0.5, 0.5, 0.5, 1.0 } ; // rodzaj materia³u obiektu

	glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse ) ;   
    glLightfv( GL_LIGHT0, GL_POSITION, LightPosition ) ; // ustawienie pozycji œwiat³a

    glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 60.0 ) ;   // kat rozwarcia sto¿ka œwiat³a
    glLightf( GL_LIGHT0, GL_SPOT_EXPONENT, 100.0 ) ; //  "gêstoœci" 
    glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, LightDirection ) ; // miejsce w które œwiecimy

	glMaterialfv( GL_FRONT, GL_DIFFUSE, Material ) ;


	glEnable( GL_LIGHT0 ) ;   // aktywujemy nasze œwiat³o
    glEnable( GL_LIGHTING ) ; // w³¹czamy oœwietlenie


	// Tworzymy podstawke do drzewa (rezygnujemy tu z listy)
	/*glColor3f(0.09f, 0.06f, 0.01f);
	glPushMatrix();
		glRotatef(45.0, 0.0, 1.0, 0.0);
		glTranslatef( -1.5, -0.1 , -1.5 ) ;
		glBegin(GL_QUADS);
			glNormal3f( -1, 1, 0 );
			glVertex3d(3,0,0);
			glVertex3d(3,0,3);
			glVertex3d(0,0,3);
			glVertex3d(0,0,0);

			glVertex3d(3,0.2,0);
			glVertex3d(3,0,0);
			glVertex3d(0,0,0);
			glVertex3d(0,0.2,0);

			glVertex3d(0,0,0);
			glVertex3d(0,0.2,0);
			glVertex3d(0,0.2,3);
			glVertex3d(0,0,3);

			glVertex3d(0,0.2,3);
			glVertex3d(0,0,3);
			glVertex3d(3,0,3);
			glVertex3d(3,0.2,3);

			glVertex3d(3,0,3);
			glVertex3d(3,0.2,3);
			glVertex3d(3,0.2,0);
			glVertex3d(3,0,0);

			glVertex3d(3,0.2,0);
			glVertex3d(3,0.2,3);
			glVertex3d(0,0.2,3);
			glVertex3d(0,0.2,0);
		glEnd();
	glPopMatrix();*/

	//Generowanie drzewka
	CreateGround();
	glCallList(GROUND);
	CreateTree();


	//Siatka dookola drzewka 
	GLUquadricObj *quadric = gluNewQuadric() ;
	gluQuadricDrawStyle( quadric, GLU_LINE ) ;
	glColor3f(0.2f, 0.2f, 0.2f);
	glPushMatrix(); 
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		gluCylinder( quadric, 0.28, 0.33, 0.5f, 15, 15 ) ; 
	glPopMatrix(); 

	glDisable( GL_LIGHTING ) ; 
    glDisable( GL_LIGHT0 ) ;
	glutSwapBuffers();
	gluDeleteQuadric( quadric ) ;

}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
	
}

void pressKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : 
			deltaAngle = -0.01f;break;
		case GLUT_KEY_RIGHT : 
			deltaAngle = 0.01f;break;
		case GLUT_KEY_UP : 
			deltaMove = 1;break;
		case GLUT_KEY_DOWN : 
			deltaMove = -1;break;
		case GLUT_KEY_HOME :
			deltaHeight = 0.05f;
			break;
		case GLUT_KEY_END :
			deltaHeight = -0.05f;
			break;
		case GLUT_KEY_PAGE_UP :
			Scale=Scale+0.1;
			Level++; break;
		case GLUT_KEY_PAGE_DOWN :
			Scale=Scale-0.1;
			Level--; break;
		case GLUT_KEY_F1 :
			oneSeed= time( NULL );
			break;
		case GLUT_KEY_F2 :
			deltaLight = 0.02f;
			break;
		case GLUT_KEY_F3 :
			deltaLight = -0.02f;
			break;
	}
}

// Zapewnia plynnosc ruchow, dzieki keyrepeat

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_HOME : 
		case GLUT_KEY_END :
			deltaHeight = 0.0f; break;
		case GLUT_KEY_LEFT : 
		case GLUT_KEY_RIGHT : 
			deltaAngle = 0.0f;break;
		case GLUT_KEY_UP : 
		case GLUT_KEY_DOWN : 
			deltaMove = 0;break;
		case GLUT_KEY_F2 :
		case GLUT_KEY_F3 :
			deltaLight = 0.0f;break;
	}
}




int main(int argc, char **argv)
{
	// Definicje poczatkowe 0.7, 0.08, 0.04
	li=0.7;
	Ri=0.08;
	ri=0.04;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(480,640);
	glutCreateWindow("boneeTree");

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	Level=1;
	glutReshapeFunc(changeSize);
	glEnable(GL_DEPTH_TEST);
	  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
	glutMainLoop();

	return(0);
}

