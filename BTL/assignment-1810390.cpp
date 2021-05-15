#include <iostream>
#include <GL/glut.h>

using namespace std;

void printUsage()
{
	cout << "1   : Xoay ban quay nguoc chieu kim dong ho" << endl;
	cout << "2	: Xoay ban quay cung chieu kim dong ho" << endl;
	cout << "3, 4  :  Dieu chinh vi tri cua chot 2" << endl;
	cout << "W, w  :  Chuyen doi qua lai giua che do khung day va to mau" << endl;
	cout << "V, v: Chuyen doi qua lai giua hai che do nhin khac nhau" << endl;
	cout << "DD, d: Bat/tat nguon sang thu hai" << endl;
	cout << "+   : Tang khoang cach camera" << endl;
	cout << "-   : Giam khoang cach camera" << endl;
	cout << "up arrow  : Tang chieu cao camera" << endl;
	cout << "down arrow: Giam chieu cao camera" << endl;
	cout << "<-        : Quay camera theo chieu kim dong ho" << endl;
	cout << "->        : Quay camera nguoc chieu kim dong ho" << endl;
}

void displayMe(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
	glVertex3f(0.5, 0.0, 0.5);
	glVertex3f(0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.5, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glEnd();
	glFlush();
}

int main(int argc, char **argv)
{
	printUsage();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(400, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Nguyen Tri Nhan - 1810390");
	glutDisplayFunc(displayMe);
	glutMainLoop();
	return 0;
}