#include <iostream>
#include <GL/glut.h>

float angle = 0.0;
bool hidden = false;

void hiddenCarte();

void drawCartesius()
{
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	// x Line
	glVertex3f(-10.0,0.0,0.0);
	glVertex3f(10.0, 0.0,0.0);
	
	// y Line
	glVertex3f(0.0,-10.0,0.0);
	glVertex3f(0.0,10.0,0.0);
	
	//z Line
	glVertex3f(0.0,0.0,-10.0);
	glVertex3f(0.0,0.0,10.0 );
	glEnd();
}

void monitor(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.0,0.0,1.0);
	
	glPushMatrix();
	glRotatef(angle, 0, 1, 1);
	hiddenCarte();
	glColor3ub(255, 179, 142);
	glutWireCube(2);
	glPopMatrix();
	
	glutSwapBuffers();
	glutPostRedisplay();
	
	angle += 0.01;
}

void init3DProjection(void)
{
	glutCreateWindow("Code 2 - Wire Cube");
	glutDisplayFunc(monitor);
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,0.1,100.0);
	gluLookAt(0.0,0.0,10.0,0.0,0.0,0.0,0.0,1.0,0.0);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	
	glRotatef(15.0,0,1,1);
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'c':
			hidden = !hidden;
			break;
	}
	glutPostRedisplay();
}

void hiddenCarte()
{
	if (hidden)
	{
		drawCartesius();
	}
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(100,100);
	init3DProjection();
	glutKeyboardFunc(myKeyboard);
	glutMainLoop();
	return 0;
}
