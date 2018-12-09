#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

int WIDTH = 1280;
int HEIGHT = 720;
bool lookingUp = false;
bool dont_touch_this_variable_mariam_was_desperate_while_writing_this = false;
float rotation = 0.0;
float displacement = 0;

GLuint tex;

GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

// Textures
GLTexture tex_ground;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f &v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f &v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;
	Vector3f tempEye, tempCenter, tempUp;

	Camera(float eyeX = 0.0f, float eyeY = 0.6f, float eyeZ = 1.2f, float centerX = 0.0f, float centerY = 0.6f, float centerZ = 0.2f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
		setupTemps();
	}

	void setupTemps() { //mariam was desperate while writing this too, if you see mariam get her a chocolate
		tempEye = Vector3f(eye.x, eye.y, eye.z);
		tempCenter = Vector3f(center.x, center.y, center.z);
		tempUp = Vector3f(up.x, up.y, up.z);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
		setupTemps();
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
		setupTemps();
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
		displacement -= d;
		setupTemps();
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
		setupTemps();
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
		rotation += a;
		printf("%f \n", right.x);
		setupTemps();
	}

	void look() {
		if (lookingUp && !dont_touch_this_variable_mariam_was_desperate_while_writing_this) {
			dont_touch_this_variable_mariam_was_desperate_while_writing_this = true;
			tempEye = Vector3f(eye.x, eye.y, eye.z);
			eye = Vector3f(0.000000, 3.260000, -0.060000);
			tempCenter = Vector3f(center.x, center.y, center.z);
			center = Vector3f(0.000000, 2.260000, -0.060000);
			tempUp = Vector3f(up.x, up.y, up.z);
			up = Vector3f(0.000000, 0.000000, -1.000000);
		}
		else if(!lookingUp) {
			dont_touch_this_variable_mariam_was_desperate_while_writing_this = false;
			eye = Vector3f(tempEye.x, tempEye.y, tempEye.z);
			center = Vector3f(tempCenter.x, tempCenter.y, tempCenter.z);
			up = Vector3f(tempUp.x, tempUp.y, tempUp.z);
		}
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-20, 0, -20);
	glTexCoord2f(5, 0);
	glVertex3f(20, 0, -20);
	glTexCoord2f(5, 5);
	glVertex3f(20, 0, 20);
	glTexCoord2f(0, 5);
	glVertex3f(-20, 0, 20);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

void drawWall(double thickness) {
	glPushMatrix();
	glColor3f(1, 0.9, 0.8);
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glColor3f(1, 1, 1);
	glPopMatrix();
}
void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawTable(double topWid, double topThick, double legThick, double legLen) {
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glColor3f(0.5, 0.3, 0.3);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95*topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glColor3f(0.5, 0.3, 0.3);
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glColor3f(1, 1, 1);
	glPopMatrix();
}

void drawBedroom() {
	// the bedroom
	glPushMatrix(); //startbedroom

					// the bed
	glPushMatrix();
	glPushMatrix(); // mattress
	glTranslated(0.15, 0.1, 0.33);
	glScaled(0.8, 0.4, 1.6);
	glutSolidCube(0.4);
	glTranslated(0, 0.03, 0.065);// sheets
	glScaled(1.05, 1, 0.7);
	glColor3f(1, 0.6, 0);
	glutSolidCube(0.4);
	glPopMatrix();
	glPushMatrix(); // pillow
	glTranslated(0.15, 0.19, 0.03);
	glRotated(90, 0, 1, 0);
	glScaled(0.2, 0.2, 1.0);
	glutSolidSphere(0.15, 15, 15);
	glPopMatrix();
	glPushMatrix(); // back of the bed
	glColor3f(0.7, 0.3, 0.2);
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(0, -0.02, 0);
	glScaled(0.31, 1, 0.3);
	glTranslated(0.5, 0.5 * 0.02, 0.5);
	glScaled(1.0, 0.02, 1.0);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	// comode
	glPushMatrix();
	glPushMatrix();
	glColor3f(0.7, 0.3, 0.2);
	glTranslated(0.4, 0.17, 0.112);
	glScaled(0.5, 1, 0.7);
	glutSolidCube(0.3);
	glColor3f(1, 0.6, 0);
	glTranslated(0, 0.07, 0.03);
	glScaled(0.95, 0.4, 1);
	glutSolidCube(0.3);
	glPushMatrix();
	glColor3f(0.7, 0.3, 0.2);
	glTranslated(0, 0, 0.16);
	glutSolidSphere(0.03, 20, 20);
	glPopMatrix();
	glColor3f(1, 0.6, 0);
	glTranslated(0, -0.35, 0);
	glutSolidCube(0.3);
	glColor3f(0.7, 0.3, 0.2);
	glTranslated(0, 0, 0.16);
	glutSolidSphere(0.03, 20, 20);
	glPopMatrix();
	glPopMatrix();

	//the lamb
	glPushMatrix();
	glTranslated(0.03, 0.03, 0);
	glScaled(0.9, 0.9, 0.9);
	glColor3f(0.9, 0.8, 0.6);
	glTranslated(0.4, 0.34, 0.112);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.025, 0.04, 7, 5);
	glPopMatrix();
	GLUquadric* qobj1 = gluNewQuadric();
	gluQuadricNormals(qobj1, GLU_SMOOTH);
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	gluCylinder(qobj1, 0.02, 0.035, .2, 20, 20);
	glPopMatrix();
	glColor3f(1, 1, 0.7);
	glTranslated(0, 0.2, 0);
	glRotated(-90, 1, 0, 0);
	glutSolidCone(0.125, 0.13, 5, 20);
	glTranslated(0, 0, 0.12);
	glutSolidSphere(0.02, 20, 20);
	glPopMatrix();

	glPushMatrix();// wordrobe
	glTranslated(0.75, 0.36, 0.1);
	glPushMatrix();
	glColor3f(0.7, 0.3, 0.2);
	glScaled(1.2, 2.3, 0.7);
	glutSolidCube(0.3);
	glPushMatrix();
	glTranslated(0, -0.1, 0.02);
	glColor3f(1, 0.6, 0);
	glScaled(0.93, 0.2, 1);
	glutSolidCube(0.3);
	glPushMatrix();
	glColor3f(0.7, 0.3, 0.2);
	glTranslated(0, 0, 0.16);
	glutSolidSphere(0.03, 20, 20);
	glPopMatrix();
	glPopMatrix();
	glColor3f(0.7, 0.3, 0.2);
	glTranslated(0, 0.141, 0.01);
	glScaled(0.4, 0.3, 0.37);
	glRotated(-90, 1, 0, 0);
	glRotated(45, 0, 0, 1);
	glutSolidCone(0.6, 0.3, 4, 16);
	glPopMatrix();
	glColor3f(0.8, 0.9, 1);
	glTranslated(-0.08, 0.07, 0.07);
	glScaled(0.5, 1.3, 0.3);
	glutSolidCube(0.3);
	glTranslated(0.34, 0, 0);
	glutSolidCube(0.3);
	glPopMatrix();

	glPushMatrix(); //the wall decoration
	glTranslated(0.2, 0.7, 0);
	glPushMatrix();
	glColor3f(1, 0.6, 0);
	glScaled(1, 1.5, 0.6);
	glutSolidCube(0.1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.7, 0.3, 0.2);
	glTranslated(0, 0, 0.02);
	glScaled(0.08, 0.1, 0.03);
	glutSolidOctahedron();
	glPopMatrix();
	glPushMatrix();
	glutSolidTorus(0.015, 0.095, 20, 20);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
}

void drawCircle3(int x, int y, float r) {
	glPushMatrix();

	glTranslatef(x, y, 0);
	GLUquadric *quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();

}

void drawframe() {
	glPushMatrix();
	glTranslated(1.5, 0.7, 0.001);
	glScaled(0.345, 0.45, 0.01);
	glColor3f(0.7, 0.5, 0.0);
	glutSolidCube(0.8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.5, 0.7, 0.003);
	glScaled(0.9, 0.9, 1.0);
	glScaled(0.345, 0.45, 0.01);
	glColor3f(0, 0, 0.5);
	glutSolidCube(0.8);
	glPopMatrix();


	glPushMatrix();
	glTranslated(1.29, 0.515, 0.01);
	glBegin(GL_TRIANGLES);
	glColor3f(0, 1, 0);
	glVertex3f(0.2f, 0.25f, 0.0f);
	glColor3f(0, 1, 0);
	glVertex3f(0.1f, 0.1f, 0.0f);
	glColor3f(0, 1, 0);
	glVertex3f(0.3f, 0.1f, 0.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.57, 0.78, 0.03);
	glColor3f(1, 1, 0);
	drawCircle3(1.43, 0.6, 0.03);
	glPopMatrix();

}

void drawlamp() {
	//3mod
	glPushMatrix();
	glTranslated(1.09, 0.2, 0.15);
	glScaled(0.05, 1.45, 0.05);
	glColor3f(0.1, 0.3, 0);
	glutSolidCube(0.8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.09, 0.7, 0.15);
	glColor3f(3.0, 3.0, 3.0);
	glutSolidSphere(0.1, 15, 15);
	glPopMatrix();
}
void drawtv() {
	glPushMatrix();
	glTranslated(1.7, 0.4, 0.2);
	glScaled(0.345, 0.345, 0.2);
	glColor3f(0, 0, 0);
	glutSolidCube(0.8);
	glPopMatrix();

	//ariel
	glPushMatrix();
	glTranslated(1.7, 0.54, 0.2);
	glRotated(40.0, 0, 0, 1);
	glScaled(0.01, 0.5, 0.01);
	glColor3f(0.6, 0.6, 0.5);
	glutSolidCube(0.8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.7, 0.54, 0.2);
	glRotated(-40.0, 0, 0, 1);
	glScaled(0.01, 0.5, 0.01);
	glColor3f(0.6, 0.6, 0.5);
	glutSolidCube(0.8);
	glPopMatrix();
	//tvtable
	glPushMatrix();
	glTranslated(1.7, 0.0, 0.2);

	glScaled(0.55, 0.85, 0.4);
	glColor3f(1.0, 0.6, 0.0);
	drawTable(0.6, 0.02, 0.04, 0.3);
	glPopMatrix();

}

void drawCircle(int x, int y, float r) {
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);


	glTranslatef(x, y, 0);
	GLUquadric *quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();

}
void drawchair() {
	glPushMatrix();
	glTranslated(1.17, 0.0, 0.7);

	glScaled(0.35, 1.0, 0.2);
	//glColor3f(0.7, 0.2, 0.3);
	drawTable(0.6, 0.02, 0.04, 0.3);
	glPopMatrix();


	glPushMatrix();
	glTranslated(1.17, 0.3935, 0.645);
	//glRotatef(rotAng, 0, 1, 0);
	glScaled(0.345, 0.35, 0.02);
	//glColor3f(0.7, 0.2, 0.3);
	glutSolidCube(0.6);
	glPopMatrix();
}

void drawcomode() {
	glPushMatrix();
	glTranslated(1.3, 0.2, 0.1);
	glScaled(0.345, 0.45, 0.2);
	glColor3f(0.8, 0.5, 0.2);
	glutSolidCube(0.8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.3, 0.1, 0.181);
	drawCircle(1.3, 0.2, 0.01);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.3, 0.2, 0.181);
	drawCircle(1.3, 0.2, 0.01);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.3, 0.3, 0.181);
	drawCircle(1.3, 0.2, 0.01);
	glPopMatrix();
}

void drawDinnerTable() {
	glPushMatrix();
	glTranslated(0.2, 0.0, -0.3);
	glColor3f(0.2, 0.1, 0.0);
	drawTable(0.6, 0.02, 0.02, 0.3);
	glPopMatrix();

	//chairs 
	glPushMatrix();
	glTranslated(-1.1, 0, -1.3);
	drawchair();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.8, 0, -1.3);
	drawchair();
	glPopMatrix();

	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glTranslated(-0.9, 0, -0.8);
	drawchair();
	glPopMatrix();

	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(-1.32, 0, -0.8);
	drawchair();
	glPopMatrix();

	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(-1.58, 0, -0.8);
	drawchair();
	glPopMatrix();
}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslated(-1.25, 0, -1.25);
	glScaled(2.5, 1, 2.5);
	drawWall(0.01);
	glPopMatrix();

	glPushMatrix();
	glScaled(1, 1, 2.5);
	glTranslated(1.25, 0, -.5);
	glRotated(90, 0, 0, 1);
	drawWall(0.01);
	glTranslated(0, 2.5, 0);
	drawWall(0.01);
	glScaled(1, 2.5, 1);
	glRotated(90, 1, 0, 0);
	drawWall(0.01);
	glTranslated(0, 1, 0);
	drawWall(0.01);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-0.25, 0, 0.25);
	glRotated(90, 0, 0, 1);
	drawWall(0.01);
	glRotated(-90, 1, 0, 0);
	drawWall(0.01);
	glPopMatrix();

	glPushMatrix(); // bedroom
	glTranslated(-1.24, 0, 0.25);
	drawBedroom();
	glPopMatrix();

	///living room
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glTranslated(-2.57, 0.1, -0.24);
	drawframe();
	glPopMatrix();
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glTranslated(-2.3, 0, -0.3);
	drawtv();
	glPopMatrix();
	
	//corner
	glPushMatrix();
	glScaled(1, 1, 2.4);
	glRotated(90, 0, 1, 0);
	glTranslated(-1.1, 0, -1.2);
	drawcomode();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2.2, 0.1, -1.2);
	drawlamp();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.3, 0, -0.3);
	drawDinnerTable();
	glPopMatrix();

	glPushMatrix();
	glTranslated(camera.eye.x, camera.eye.y, camera.eye.z);
	glRotated(rotation, 0, 1, 0);
	glTranslated(0, -0.04, -0.1);
	glutSolidSphere(0.03, 100, 100);
	glPopMatrix();
	glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.07;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;
	float d = 0.05;


	if (!lookingUp) {
		switch (key) {
		case GLUT_KEY_UP:
			camera.moveZ(d);
			break;
		case GLUT_KEY_LEFT:
			camera.rotateY(a);
			break;
		case GLUT_KEY_RIGHT:
			camera.rotateY(-a);
			break;
		}
	}

	glutPostRedisplay();
}

void key(unsigned char k, int x, int y)
{
	if (k == 32) {
		if (lookingUp)
			lookingUp = false;
		else
			lookingUp = true;
		camera.look();
	}

	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1500, 1000);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("3D PACMAN");
	glutDisplayFunc(Display);
	glutKeyboardFunc(key);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	tex_ground.Load("Textures/ground.bmp");

	glutMainLoop();
}
