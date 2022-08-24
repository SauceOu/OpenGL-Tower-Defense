#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include <windows.h>

#define PI 3.14159265

const int vWidth = 800;    // Viewport width in pixels
const int vHeight = 600;    // Viewport height in pixels

int window;

// Mouse button
int currentButton;

// Robot Size
float robotHeadWidth = 3.0;
float robotHeadDepth = 3.0;
float robotHeadHeight = 9.0;

// Angles
float robotAngleOne = 0.0;
float robotAngleTwo = 0.0;
float wheelAngle = 0.0;
float gunAngleOne = 0.0;
float gunAngleTwo = 0.0;

// Robot Direction
float directionXOne = 0.0; 
float directionZOne = 0.0; 
float directionXTwo = 0.0;
float directionZTwo = 0.0;

// Robot Gun Direction
float gunDirectionXOne = 0.0;
float gunDirectionZOne = 0.0;
float gunDirectionXTwo = 0.0;
float gunDirectionZTwo = 0.0;

// Bullet Direction
float bulletDirectionXOne = 4.96;
float bulletDirectionZOne = 21.98;
float bulletDirectionXTwo = -2.98;
float bulletDirectionZTwo = 23.98;
float bulletAngleOne = 0.0;
float bulletAngleTwo = 0.0;

// Robot Initial Position
float yShiftOne = 1.7;
float xShiftOne = -5.0;
float zShiftOne = -10.0;
float yShiftTwo = 1.7;
float xShiftTwo = 3.0;
float zShiftTwo = -12.0;

// Bullet Pos
float bulletZOne = -12.0;
float bulletXOne = -5.0;
float bulletZTwo = -20.0;
float bulletXTwo = 3.0;
float bulletZPlayer = 11.5;
float bulletXPlayer = 0.0;

// For collision
float bulletOnePos = bulletZOne + zShiftOne;
float bulletTwoPos = bulletZTwo + zShiftTwo;

// Bullet size scale
float size = 1.0;

// cannon pos
float cannonShift = 0.0;
float cannonZ = 12.0;

// For animation
bool stop = false;
int currentNodeOne = 0;
int nextNodeOne = 1;
int currentNodeTwo = 0;
int nextNodeTwo = 1;

float tempShiftX = 0.0;
float tempShiftZ = 0.0;
bool shoot = false;
bool fpv = false;
bool showRobotOne = true;
bool showRobotTwo = true;
bool brokenCannon = false;

typedef struct CurvePoint
{
	float z;
	float x;
};

// path points
CurvePoint curvePointsOne[] = { {10.0, -5.0}, {5.0, 5.0}, {2.0, 3.0}, {-2.0, 0.0}, {-5.0, -4.0}, {-8.0, -5.0}, {-12.0, 1.0}, {-16.0, 3.0} };
CurvePoint curvePointsTwo[] = { {12.0, 4.0}, {7.0, -6.0}, {4.0, -4.0}, {0.0, 1.0}, {-3.0, 5.0}, {-6.0, 4.0}, {-10.0, -3.0}, {-16.0, -5.0} };

// Texture
GLfloat textureMap[64][64][3];
GLuint tex[2];


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Gun Mat
GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

// Robot Mat
GLfloat robotHead_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotHead_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat robotHead_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat robotHead_mat_shininess[] = { 76.8F };

// Ground Mesh material
GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };

// Eye Location
GLdouble eyeX = 0.0, eyeY = 6.0, eyeZ = 22.0;
GLdouble lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void specialKeyHandler(int key, int x, int y);
void animationHandler(int param);
void drawRobot(float x, float y, float z, float robotAngle, float gunAngle);
void drawHead(float gunAngle);
void drawBody();
void drawConnection();
void drawBarGroup();
void drawBar();
void drawWheel();
void drawGround();
void drawBullet(float z, float size, float angle);
void drawCannon();
void drawBrokenCannon();
bool collided(float bulletX, float bulletZ, float objectX, float objectZ);

void assignColor(GLfloat col[3], GLfloat r, GLfloat g, GLfloat b) {
	col[0] = r;
	col[1] = g;
	col[2] = b;
}

int isEven(int x) {
	if (x % 2 == 0)
		return 1;
	else
		return 0;
}

void makeTextureMap()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++) {
			if (isEven(i / 8 + j / 8))
				assignColor(textureMap[i][j], 1.0, 1.0, 1.0);
			else
				assignColor(textureMap[i][j], 0.5, 0.6, 1.2);
		}
}

void makeTextures()
{
	GLfloat planes[] = { 0.0, 0.0, 0.3, 0.0 };
	GLfloat planet[] = { 0.0, 0.3, 0.0, 0.0 };

	glGenTextures(2, tex);

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap);

	/************************************ */
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	window = glutCreateWindow("Assignment 3");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyHandler);

	glutMainLoop();

	return 0;
}

void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_TEXTURE_2D);
	makeTextureMap();
	makeTextures();

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);

	drawGround();
	//draw3DSubdivisionCurve();
	
	if (collided(bulletXPlayer, bulletZPlayer, xShiftOne, zShiftOne)) { showRobotOne = false; }
	if (showRobotOne && zShiftOne < 15.0) {	drawRobot(xShiftOne, yShiftOne, zShiftOne, robotAngleOne, gunAngleOne); }
	glPushMatrix();
	glTranslatef(bulletXOne, 1.2, 0.0);
	if (showRobotOne && bulletZOne < 15.0) { drawBullet(bulletZOne, 0.1, bulletAngleOne); }
	glPopMatrix();

	
	if (collided(bulletXPlayer, bulletZPlayer, xShiftTwo, zShiftTwo)) { showRobotTwo = false; }
	if (showRobotTwo && zShiftTwo < 15.0) { drawRobot(xShiftTwo, yShiftTwo, zShiftTwo, robotAngleTwo, gunAngleTwo); }
	glPushMatrix();
	glTranslatef(bulletXTwo, 1.2, 0.0);
	if (showRobotTwo && bulletZTwo < 15.0) { drawBullet(bulletZTwo, 0.1, bulletAngleTwo); }
	glPopMatrix();


	if ((showRobotOne && collided(bulletXOne, bulletZOne, cannonShift, cannonZ)) || (showRobotTwo && collided(bulletXTwo, bulletZTwo, cannonShift, cannonZ))) {
		brokenCannon = true; 
		stop = true;
	}
	if (brokenCannon) { drawBrokenCannon(); }
	else { drawCannon(); }
	glPushMatrix();
	glTranslatef(bulletXPlayer, 1.2, 0.0);
	if (brokenCannon == false) { drawBullet(bulletZPlayer, 0.1, 0.0); }
	glPopMatrix();
	
	glutSwapBuffers();
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.1, 40.0);
	//gluPerspective(fov,aspect,zNear,zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void drawRobot(float x, float y, float z, float robotAngle, float gunAngle)
{
	glPushMatrix();
	// Forward/backward movement
	glTranslatef(x, y, z);
	glScalef(0.15, 0.15, 0.15);
	glPushMatrix();
	glRotatef(gunAngle, 0.0, 1.0, 0.0);
	drawHead(gunAngle);
	glPopMatrix();
	// mech bot turning 
	glRotatef(robotAngle, 0.0, 1.0, 0.0);
	drawBody();
	drawWheel();
	glPopMatrix();
}


// this function draws the head of the mech bot
// note that the guns are part of the head
void drawHead(float gunAngle)
{
	// Set robot material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotHead_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotHead_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotHead_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotHead_mat_shininess);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	// Head
	glPushMatrix();
	glScalef(0.7 * robotHeadWidth, 0.7 * robotHeadHeight, 0.7 * robotHeadDepth);
	GLUquadricObj* mySphere = gluNewQuadric();
	gluQuadricDrawStyle(mySphere, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(mySphere, GLU_SMOOTH);
	gluSphere(mySphere, 1.0, 32, 32);
	glPopMatrix();

	// Guns
	glPushMatrix();
	glTranslatef(0.0, -(0.35 * robotHeadHeight), 0.0);

	// Left Gun
	glPushMatrix();
	glTranslatef(0.6 * robotHeadWidth, 0.0, 0.0);

	// Left Gun Body
	glPushMatrix();
	glScalef(0.3 * robotHeadWidth, 0.3 * robotHeadWidth, 0.3 * robotHeadDepth);
	gluSphere(mySphere, 1.0, 32, 32);
	glPopMatrix();

	// Left Gun Barrel
	glPushMatrix();
	glTranslatef(0.15 * robotHeadWidth, 0.0, 0.3 * robotHeadDepth);
	glScalef(0.2, 0.2, robotHeadDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();

	// Right Gun
	glPushMatrix();
	glTranslatef(-(0.6 * robotHeadWidth), 0.0, 0.0);

	// Right Gun Body
	glPushMatrix();
	glScalef(0.3 * robotHeadWidth, 0.3 * robotHeadWidth, 0.3 * robotHeadDepth);
	gluSphere(mySphere, 1.0, 32, 32);
	glPopMatrix();

	// Right Gun Barrel
	glPushMatrix();
	glTranslatef(-(0.15 * robotHeadWidth), 0.0, 0.3 * robotHeadDepth);
	glScalef(0.2, 0.2, robotHeadDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

// the body of the robot consists of two symmetrical parts that connect the head and the wheel
void drawBody()
{
	glPushMatrix();
	// Rotate and Translate
	glTranslatef(0.0, -(0.7 * robotHeadHeight), -(2.1 * robotHeadDepth));
	glRotatef(-45.0, 1.0, 0.0, 0.0);

	// Left Part
	glPushMatrix();
	glTranslatef(-(0.75 * robotHeadWidth), 0.0, 0.0);
	drawConnection();
	glPopMatrix();

	// Right Part
	glPushMatrix();
	glTranslatef(0.75 * robotHeadWidth, 0.0, 0.0);
	drawConnection();
	glPopMatrix();

	glPopMatrix();
}

// Part of body that connects the head and the wheel
// Consistes of three spheres and two groups of rectangular bars that interconnect the spheres
void drawConnection()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotHead_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotHead_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotHead_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotHead_mat_shininess);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	GLUquadricObj* mySphere = gluNewQuadric();
	gluQuadricDrawStyle(mySphere, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(mySphere, GLU_SMOOTH);

	// Top Sphere
	glPushMatrix();
	glTranslatef(0.0, 0.0, robotHeadHeight);
	glScalef(0.15 * robotHeadHeight, 0.15 * robotHeadHeight, 0.15 * robotHeadHeight);
	gluSphere(mySphere, 1.0, 32, 32);
	glPopMatrix();

	// Middle Sphere
	glPushMatrix();
	glScalef(0.15 * robotHeadHeight, 0.15 * robotHeadHeight, 0.15 * robotHeadHeight);
	gluSphere(mySphere, 1.0, 32, 32);
	glPopMatrix();

	// Bottom Sphere
	glPushMatrix();
	glTranslatef(0.0, -robotHeadHeight, 0.0);
	glScalef(0.15 * robotHeadHeight, 0.15 * robotHeadHeight, 0.15 * robotHeadHeight);
	gluSphere(mySphere, 1.0, 32, 32);
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	// Upper Bar Group
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.5 * robotHeadHeight);
	drawBarGroup();
	glPopMatrix();

	// Lower Bar Group
	glPushMatrix();
	glTranslatef(0.0, -(0.5 * robotHeadHeight), 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	drawBarGroup();
	glPopMatrix();
}

// group of four rectangular bars that is used to connect the spheres in the connection part
void drawBarGroup()
{
	// Bar1 - Upper Left
	glPushMatrix();
	glTranslatef(-(0.09375 * robotHeadWidth), 0.09375 * robotHeadWidth, 0.0);
	drawBar();
	glPopMatrix();

	// Bar2 - Upper Right
	glPushMatrix();
	glTranslatef(0.09375 * robotHeadWidth, 0.09375 * robotHeadWidth, 0.0);
	drawBar();
	glPopMatrix();

	// Bar3 - Bottom Left
	glPushMatrix();
	glTranslatef(-(0.09375 * robotHeadWidth), -(0.09375 * robotHeadWidth), 0.0);
	drawBar();
	glPopMatrix();

	// Bar4 - Bottom Right
	glPushMatrix();
	glTranslatef(0.09375 * robotHeadWidth, -(0.09375 * robotHeadWidth), 0.0);
	drawBar();
	glPopMatrix();

}

// a thin rectangular bar
void drawBar()
{
	glPushMatrix();
	glScalef(0.4, 0.4, robotHeadHeight);
	glutSolidCube(1.0);
	glPopMatrix();
}

// wheel of the mech bot
// consists of a thicker outer wheel(tire), a thinner inner wheel, and a rectuangular object that shows the wheel rotation.
void drawWheel()
{
	// similiar texture as the gun (black for the tire)
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	glPushMatrix();

	glTranslatef(0.0, -1.4 * robotHeadHeight, 0.0);
	glRotatef(wheelAngle, 1.0, 0.0, 0.0); // Rotate wheel when moving
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glScalef(1.0, 1.0, 1.1);

	// Outer Wheel
	glPushMatrix();
	glScalef(0.9 * robotHeadHeight, 0.9 * robotHeadHeight, 0.9 * robotHeadHeight);
	glutSolidTorus(0.2, 0.5, 32, 32);
	glPopMatrix();

	// Inner Wheel (which is white)
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotHead_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotHead_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotHead_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotHead_mat_shininess);

	glPushMatrix();
	glScalef(0.8 * robotHeadHeight, 0.8 * robotHeadHeight, 0.8 * robotHeadWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// attachment object to show wheel rotation
	glPushMatrix();
	glTranslatef(0.0, 4.5, 0.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glScalef(2 * robotHeadWidth, 0.1 * robotHeadHeight, 0.1 * robotHeadHeight);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawBrokenCannon()
{
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	GLUquadricObj* myCylinder = gluNewQuadric();
	gluQuadricDrawStyle(myCylinder, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(myCylinder, GLU_SMOOTH);

	GLUquadricObj* mySphere = gluNewQuadric();
	gluQuadricDrawStyle(mySphere, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(mySphere, GLU_SMOOTH);

	glPushMatrix();
	glTranslatef(cannonShift, 0.0, cannonZ);
	glScalef(0.6, 0.6, 0.6);
	glPushMatrix();
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	gluCylinder(myCylinder, 1.4, 0.6, 2.0, 32, 32);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-1.3, 0.0, 2.0);
	glRotatef(-110.0, 1.0, 0.0, 0.0);
	glRotatef(-60.0, 0.0, 1.0, 0.0);
	gluSphere(mySphere, 0.7, 32, 32);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1.5);
	gluCylinder(myCylinder, 0.2, 0.2, 2.0, 32, 32);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

void drawCannon()
{
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	GLUquadricObj* myCylinder = gluNewQuadric();
	gluQuadricDrawStyle(myCylinder, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(myCylinder, GLU_SMOOTH);

	GLUquadricObj* mySphere = gluNewQuadric();
	gluQuadricDrawStyle(mySphere, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(mySphere, GLU_SMOOTH);

	glPushMatrix();
	glTranslatef(cannonShift, 0.0, cannonZ);
	glScalef(0.6, 0.6, 0.6);
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(myCylinder, 1.4, 0.6, 2.0, 32, 32);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 2.0, 0.0);
	gluSphere(mySphere, 0.7, 32, 32);
	glPushMatrix();
	glTranslatef(0.0,0.0,-1.5);
	gluCylinder(myCylinder, 0.2, 0.2, 2.0, 32, 32);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

void drawGround() {
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-12.0f, -1.0f, -12.0f);
	glVertex3f(-12.0f, -1.0f, 12.0f);
	glVertex3f(12.0f, -1.0f, 12.0f);
	glVertex3f(12.0f, -1.0f, -12.0f);
	glEnd();
	glPopMatrix();
}

void drawBullet(float z, float size, float angle)
{
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);

	GLUquadricObj* myCylinder = gluNewQuadric();
	gluQuadricDrawStyle(myCylinder, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(myCylinder, GLU_SMOOTH);

	GLUquadricObj* myDisk = gluNewQuadric();
	gluQuadricDrawStyle(myDisk, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(myDisk, GLU_SMOOTH);

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotHead_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotHead_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotHead_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotHead_mat_shininess);

	glPushMatrix();
	glTranslatef(0.0, 0.0, z);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glScalef(size, size, size);
	gluCylinder(myCylinder, 1.0, 1.0, 3.0, 32, 32);
	gluDisk(myDisk, 0.0, 1.0, 32, 32);
	glPushMatrix();
	glTranslatef(0.0, 0.0, 3.0);
	gluDisk(myDisk, 0.0, 1.0, 32, 32);
	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
	case 27:
		// Esc, q, or Q key = Quit 
		exit(0);
		break;
	case 'a':
	case 'A':
		// Add code to create timer and call animation handler
		stop = false;
		glutTimerFunc(5, animationHandler, 0);
		// Use this to set to 3D window and redraw it
		glutSetWindow(window);
		glutPostRedisplay();
		break;
	case 'p':
	case 'P':
		stop = true;
		break;
	case 'r':
	case 'R':
		// reset the game
		stop = true;
		showRobotOne = true;
		showRobotTwo = true;
		brokenCannon = false;
		shoot = false;
		fpv = false;
		eyeX = 0.0;
		eyeY = 6.0;
		eyeZ = 22.0;
		lookAtX = 0.0;
		bulletZOne = -12.0;
		bulletXOne = -5.0;
		bulletZTwo = -20.0;
		bulletXTwo = 3.0;
		bulletZPlayer = 11.5;
		bulletXPlayer = 0.0;
		cannonShift = 0.0;
		xShiftOne = -5.0;
		zShiftOne = -10.0;
		xShiftTwo = 3.0;
		zShiftTwo = -12.0;
		currentNodeOne = 0;
		nextNodeOne = 1;
		currentNodeTwo = 0;
		nextNodeTwo = 1;
		directionXOne = 0.0;
		directionZOne = 0.0;
		directionXTwo = 0.0;
		directionZTwo = 0.0;

		break;
	case 's':
		if (fpv) {
			fpv = false;
			eyeX = 0.0;
			eyeY = 6.0;
			eyeZ = 22.0;
			lookAtX = 0.0;
		}
		else 
		{
			fpv = true;
			eyeX = cannonShift;
			eyeY = 2.3;
			eyeZ = 13.0;
			lookAtX = cannonShift;
		}
		break;
	case ' ':
		if (shoot == false) {
			shoot = true;
			
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void animationHandler(int param)
{
	if (!stop)
	{
		// robot one movement
		if (curvePointsOne[nextNodeOne].x - 1 < xShiftOne && xShiftOne < curvePointsOne[nextNodeOne].x + 1
			&& -(curvePointsOne[nextNodeOne].z) - 1 < zShiftOne && zShiftOne < -(curvePointsOne[nextNodeOne].z) + 1) {
			currentNodeOne = currentNodeOne + 1;
			nextNodeOne = nextNodeOne + 1;
		}
		if (currentNodeOne >= sizeof(curvePointsOne)) { currentNodeOne = 0;  nextNodeOne = 1; }
		directionXOne = curvePointsOne[nextNodeOne].x - curvePointsOne[currentNodeOne].x;
		directionZOne = -(curvePointsOne[nextNodeOne].z - curvePointsOne[currentNodeOne].z);
		robotAngleOne = atan(directionXOne / directionZOne) * 180 / PI;
		xShiftOne += 0.004 * directionXOne;
		zShiftOne += 0.004 * directionZOne;

		// robot two movement
		if (curvePointsTwo[nextNodeTwo].x - 1 < xShiftTwo && xShiftTwo < curvePointsTwo[nextNodeTwo].x + 1
			&& -(curvePointsTwo[nextNodeTwo].z) - 1 < zShiftTwo && zShiftTwo < -(curvePointsTwo[nextNodeTwo].z) + 1) {
			currentNodeTwo = currentNodeTwo + 1;
			nextNodeTwo = nextNodeTwo + 1;
		}
		if (currentNodeTwo >= sizeof(curvePointsTwo)) { currentNodeTwo = 0;  nextNodeTwo = 1; }
		directionXTwo = curvePointsTwo[nextNodeTwo].x - curvePointsTwo[currentNodeTwo].x;
		directionZTwo = -(curvePointsTwo[nextNodeTwo].z - curvePointsTwo[currentNodeTwo].z);
		robotAngleTwo = atan(directionXTwo / directionZTwo) * 180 / PI;
		xShiftTwo += 0.004 * directionXTwo;
		zShiftTwo += 0.004 * directionZTwo;

		gunDirectionXOne = cannonShift - xShiftOne;
		gunDirectionZOne = (cannonZ - zShiftOne);
		gunAngleOne = atan(gunDirectionXOne / gunDirectionZOne) * 180 / PI;

		gunDirectionXTwo = cannonShift - xShiftTwo;
		gunDirectionZTwo = (cannonZ - zShiftTwo);
		gunAngleTwo = atan(gunDirectionXTwo / gunDirectionZTwo) * 180 / PI;
		
		//printf("oneX: %f, OneZ: %f, TwoX: %f, twoZ: %f", gunDirectionXOne, gunDirectionZOne, gunDirectionXTwo, gunDirectionZTwo);
	
		bulletAngleOne = atan(bulletDirectionXOne / bulletDirectionZOne) * 180 / PI;
		bulletAngleTwo = atan(bulletDirectionXTwo / bulletDirectionZTwo) * 180 / PI;

		bulletXOne += 0.01 * bulletDirectionXOne;
		bulletZOne += 0.01 * bulletDirectionZOne;
		bulletXTwo += 0.01 * bulletDirectionXTwo;
		bulletZTwo += 0.01 * bulletDirectionZTwo;

		if (shoot) { bulletZPlayer -= 0.8; }
		if (zShiftOne > 14.0) 
		{ 
			xShiftOne = -5.0;
			zShiftOne = -10.0;
			directionXOne = 0.0;
			directionZOne = 0.0;
			currentNodeOne = 0;
			nextNodeOne = 1; 
		}
		if (zShiftTwo > 14.0) 
		{
			xShiftTwo = 3.0;
			zShiftTwo = -12.0;
			directionXTwo = 0.0;
			directionZTwo = 0.0;
			currentNodeTwo = 0; 
			nextNodeTwo = 1; 
		}
		if (bulletZOne > zShiftOne + 20.0) 
		{ 
			bulletZOne = zShiftOne;
			bulletXOne = xShiftOne; 
			bulletDirectionXOne = gunDirectionXOne;
			bulletDirectionZOne = gunDirectionZOne;
		}
		if (bulletZTwo > zShiftTwo + 20.0) 
		{ 
			bulletZTwo = zShiftTwo; 
			bulletXTwo = xShiftTwo; 
			bulletDirectionXTwo = gunDirectionXTwo;
			bulletDirectionZTwo = gunDirectionZTwo;
		}
		if (bulletZPlayer < -70.0) {
			shoot = false; 
			bulletZPlayer = 11.5;
			bulletXPlayer = cannonShift;
		}
		// wheel rotation
		wheelAngle += 2.0;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}

/*
void animationHandler(int param)
{
	if (!stop)
	{
		if (subcurve.curvePoints[nextNode].x - 0.001 < currentX && currentX < subcurve.curvePoints[nextNode].x + 0.001
			&& -(subcurve.curvePoints[nextNode].y) - 0.001 < currentZ && currentZ < -(subcurve.curvePoints[nextNode].y) + 0.001) {
			currentNode = currentNode + 1;
			nextNode = nextNode + 1;
		}
		if (nextNode >= subcurve.numCurvePoints) { stop = true; }
		directionX = subcurve.curvePoints[nextNode].x - subcurve.curvePoints[currentNode].x;
		directionZ = -(subcurve.curvePoints[nextNode].y - subcurve.curvePoints[currentNode].y);
		robotAngle = atan(directionX / directionZ) * 180 / PI;
		xShift += 0.04 * directionX;
		zShift += 0.04 * directionZ;
		wheelAngle += 2.0;
		currentX = xShift + subcurve.curvePoints[0].x;
		currentZ = zShift - subcurve.curvePoints[0].y;
		//printf("xShift:%f currentX:%f currentZ:%f currentNode:%d nextNode:%d\n", xShift, currentX, currentZ, currentNode, nextNode);
		glutPostRedisplay();
		glutTimerFunc(5, animationHandler, 0);
	}
}
*/

void specialKeyHandler(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_F1:
		printf(" a/A -> start game \n r/R -> reset game \n p/P -> pause game \n left/right arrow keys -> move cannon \n s/S -> switch view point \n space -> shoot");
		break;
	case GLUT_KEY_LEFT:
		// add code here
		if (stop == false)
		{
			cannonShift -= 1.0;
			if (shoot == false) { bulletXPlayer -= 1.0; }
			if (fpv)
			{
				eyeX = cannonShift;
				lookAtX = cannonShift;
			}
		}
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		// add code here;
		if (stop == false)
		{
			cannonShift += 1.0;
			if (shoot == false) { bulletXPlayer += 1.0; }
			if (fpv)
			{
				eyeX = cannonShift;
				lookAtX = cannonShift;
			}
		}
		glutPostRedisplay();
		break;
	}
	glutPostRedisplay();
}

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

bool collided(float bulletX, float bulletZ, float objectX, float objectZ)
{
	//printf("bulletX: %f, bulletZ: %f, ObjectX: %f, ObjectZ: %f", bulletX, bulletZ, objectX, objectZ);
	return (bulletX > objectX - 1 && bulletX < objectX + 1 && bulletZ > objectZ - 1 && bulletZ < objectZ + 1);
}

