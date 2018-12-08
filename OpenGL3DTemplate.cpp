#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

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

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

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
	glColor3f(0.5 , 0.3 , 0.3 );
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95*topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glColor3f(0.5 , 0.3 , 0.3 );
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

	glPushMatrix();
	glTranslated(-1.24, 0, 0.25);
	drawBedroom();
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

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1500, 1000);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("3D PACMAN");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
