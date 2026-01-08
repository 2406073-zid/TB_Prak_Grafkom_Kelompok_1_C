#include <GL/glut.h>

float rotationAngle = 0.0;
bool hidden = false;

void myKeyboard(unsigned char key, int x, int y);
void init3DProjection();
void window();
void hiddenCarte();

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GL_DEPTH);
	glutInitWindowSize(720,720);
	glutInitWindowPosition(200,0);
	init3DProjection();
	glutKeyboardFunc(myKeyboard);
	glutMainLoop();
	return 0;
}

void init3DProjection()
{
	glutCreateWindow("Code 2 - Manual Solid Cube");
	glutDisplayFunc(window);
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0,5.0, -5.0,5.0);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glRotatef(-15.0,0,1,1);
}

void cartecius ()
{
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex3f(-50.0,0.0,0.0);
	glVertex3f(50.0,0.0,0.0);
	
	glVertex3f(0.0, -50.0, 0.0);
	glVertex3f(0.0, 50.0,0.0);
	
	glVertex3f(0.0,0.0,-50.0);
	glVertex3f(0.0,0.0,50.0);
	glEnd();
}

void window ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0,0.0,1.0);
	
	glPushMatrix();
	float x_positive = 1.0;
	float x_negative = -1.0;
	float y_positive = 1.0;
	float y_negative = -1.0;
	float z_positive = 1.0;
	float z_negative = -1.0;
	
	glRotatef(rotationAngle, 0,1,1);
	hiddenCarte();
	glColor3ub(254,236,55);
	glBegin(GL_POLYGON);
	glVertex3f(x_negative, y_negative, z_positive);
	glVertex3f(x_positive, y_negative, z_positive);
	glVertex3f(x_positive, y_positive, z_positive);
	glVertex3f(x_negative, y_positive, z_positive);
	glEnd();
	
	//Belakang
	glColor3ub(255,119,183);
	glBegin(GL_POLYGON);
	glVertex3f(x_negative, y_negative, z_positive);
	glVertex3f(x_positive, y_negative, z_positive);
	glVertex3f(x_positive, y_positive, z_positive);
	glVertex3f(x_negative, y_positive, z_positive);
	glEnd();
	
	//Kiri
	glColor3ub(76,201,254);
	glBegin(GL_POLYGON);
	glVertex3f(x_negative, y_negative, z_negative);
	glVertex3f(x_negative, y_negative, z_positive);
	glVertex3f(x_negative, y_positive, z_positive);
	glVertex3f(x_negative, y_positive, z_negative);
	glEnd();
	
	//Kanan
	glColor3ub(217,22,86);
	glBegin(GL_POLYGON);
	glVertex3f(x_positive, y_negative, z_negative);
	glVertex3f(x_positive, y_positive, z_negative);
	glVertex3f(x_positive, y_positive, z_positive);
	glVertex3f(x_positive, y_negative, z_positive);
	glEnd();
	
	//Bawah
	glColor3ub(192,235,166);
	glBegin(GL_POLYGON);
	glVertex3f(x_positive, y_negative, z_positive);
	glVertex3f(x_negative, y_negative, z_positive);
	glVertex3f(x_negative, y_negative, z_negative);
	glVertex3f(x_positive, y_negative, z_negative);
	glEnd();
	
	//Atas
	glColor3ub(255,102,0);
	glBegin(GL_POLYGON);
	glVertex3f(x_negative, y_positive, z_positive);
	glVertex3f(x_positive, y_positive, z_positive);
	glVertex3f(x_positive, y_positive, z_negative);
	glVertex3f(x_negative, y_positive, z_negative);
	glEnd();
	glPopMatrix();
	
	glutSwapBuffers();
	glutPostRedisplay();
	rotationAngle += 1;
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'c':
			hidden = !hidden;
			break;
	}
	glutPostRedisplay;
}

void hiddenCarte()
{
	if (hidden)
	{
		cartecius();
	}
}



