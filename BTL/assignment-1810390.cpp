#include <math.h>
#include <iostream>
// #include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>

using namespace std;
// #define M_PI 3.141592653589793238462643383279502884

#define COLORNUM 14
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define COLOR_YELLOW 3
#define COLOR_PINK 4
#define COLOR_AQUA 5
#define COLOR_DARK_GRAY 6
#define COLOR_GRAY 7
#define COLOR_BLACK 12

float ColorArr[COLORNUM][3] = {
	{1.0, 0.0, 0.0},  // red 0
	{0.0, 1.0, 0.0},  // green 1
	{0.0, 0.0, 1.0},  // blue 2
	{1.0, 1.0, 0.0},  // yellow 3
	{1.0, 0.75, 0.8}, // pink 4
	{0.0, 1.0, 1.0},  // aqua 5
	{0.3, 0.3, 0.3},  // dark gray 6
	{0.5, 0.5, 0.5},  // gray 7
	{0.9, 0.9, 0.9},  // silver 8
	{1.0, 0.5, 0.5},  // kinda pink pastel 9
	{0.5, 1.0, 0.5},  // mint green 10
	{0.5, 0.5, 1.0},  // not purple, more blue 11
	{0.0, 0.0, 0.0},  // black 12
	{1.0, 1.0, 1.0}}; // white 13

float screenRatio = 16 / 9;
int screenWidth = 1024;
int screenHeight = screenWidth / screenRatio;
float lookAtX, lookAtY, lookAtZ;
float cameraDistance_init = 10;
float cameraHeight_init = 3.5;
float cameraAngle_init = -35;
float cameraDistance = cameraDistance_init;
float cameraHeight = cameraHeight_init;
float cameraAngle = cameraAngle_init;

bool bLockCamera = false;
bool bDrawWireFrame = false;
bool bLight1on = true;

#pragma region SupportClass

class Point3
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
	void set(Point3 &p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
	}
	Point3() { x = y = z = 0; }
	Point3(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
};
class Color3
{
public:
	float r, g, b;
	void set(float red, float green, float blue)
	{
		r = red;
		g = green;
		b = blue;
	}
	void set(Color3 &c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
	}
	Color3() { r = g = b = 0; }
	Color3(float red, float green, float blue)
	{
		r = red;
		g = green;
		b = blue;
	}
};
class Point2
{
public:
	Point2() { x = y = 0.0f; } // constructor 1
	Point2(float xx, float yy)
	{
		x = xx;
		y = yy;
	} // constructor 2
	void set(float xx, float yy)
	{
		x = xx;
		y = yy;
	}
	float getX() { return x; }
	float getY() { return y; }
	void draw()
	{
		glBegin(GL_POINTS);
		glVertex2f((GLfloat)x, (GLfloat)y);
		glEnd();
	}

private:
	float x, y;
};
class IntRect
{
public:
	IntRect()
	{
		l = 0;
		r = 100;
		b = 0;
		t = 100;
	} // constructor
	IntRect(int left, int right, int bottom, int top)
	{
		l = left;
		r = right;
		b = bottom;
		t = top;
	}
	void set(int left, int right, int bottom, int top)
	{
		l = left;
		r = right;
		b = bottom;
		t = top;
	}
	void draw()
	{
		glRecti(l, b, r, t);
		glFlush();
	} // draw this rectangle using OpenGL
	int getWidth() { return (r - l); }
	int getHeight() { return (t - b); }

private:
	int l, r, b, t;
};

class RealRect
{
public:
	RealRect()
	{
		l = 0;
		r = 100;
		b = 0;
		t = 100;
	} // constructor
	RealRect(float left, float right, float bottom, float top)
	{
		l = left;
		r = right;
		b = bottom;
		t = top;
	}
	void set(float left, float right, float bottom, float top)
	{
		l = left;
		r = right;
		b = bottom;
		t = top;
	}
	float getWidth() { return (r - l); }
	float getHeight() { return (t - b); }
	void draw()
	{
		glRectf(l, b, r, t);
		glFlush();
	}; // draw this rectangle using OpenGL
private:
	float l, r, b, t;
};

class Vector3
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
	void set(Vector3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	void flip()
	{
		x = -x;
		y = -y;
		z = -z;
	}
	void normalize()
	{
		float temp = sqrt(x * x + y * y + z * z);
		x = x / temp;
		y = y / temp;
		z = z / temp;
	}
	Vector3() { x = y = z = 0; }
	Vector3(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
	Vector3(Vector3 &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	Vector3 cross(Vector3 b)
	{
		Vector3 c(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
		return c;
	}
	float dot(Vector3 b)
	{
		return x * b.x + y * b.y + z * b.z;
	}
};
#pragma endregion

#pragma region Mesh
class VertexID
{
public:
	int vertIndex;
	int colorIndex;
};

class Face
{
public:
	int nVerts;
	VertexID *vert;
	Vector3 facenorm; // Pháp tuyến của mặt.

	Face()
	{
		nVerts = 0;
		vert = NULL;
	}
	~Face()
	{
		if (vert != NULL)
		{
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};

class Mesh
{

public:
	int numVerts;
	Point3 *pt;

	int numFaces;
	Face *face;

	GLfloat ambient[4] = {0.0, 0.0, 0.0, 1.0};
	GLfloat diffuse[4] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 50.0f;

	float rotateX, rotateY, rotateZ;
	float positionX, positionY, positionZ;
	float distance;
	float thanhLienKetAngle;

public:
	Mesh()
	{
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
		rotateX = 0;
		rotateY = 0;
		rotateZ = 0;
		positionX = 0;
		positionY = 0;
		positionZ = 0;
		distance = 0;
	}
	~Mesh()
	{
		if (pt != NULL)
		{
			delete[] pt;
		}
		if (face != NULL)
		{
			delete[] face;
		}
		numVerts = 0;
		numFaces = 0;
	}

	void CalculateFacesNorm()
	{
		for (int f = 0; f < numFaces; f++)
		{
			float mx = 0, my = 0, mz = 0;
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int iv = face[f].vert[v].vertIndex;
				int next = face[f].vert[(v + 1) % face[f].nVerts].vertIndex;
				mx += (pt[iv].y - pt[next].y) * (pt[iv].z + pt[next].z);
				my += (pt[iv].z - pt[next].z) * (pt[iv].x + pt[next].x);
				mz += (pt[iv].x - pt[next].x) * (pt[iv].y + pt[next].y);
			}
			face[f].facenorm.set(mx, my, mz);
			face[f].facenorm.normalize();
		}
	}

	void SetColor(int colorIdx)
	{
		for (int f = 0; f < numFaces; f++)
		{
			for (int v = 0; v < face[f].nVerts; v++)
			{
				face[f].vert[v].colorIndex = colorIdx;
			}
		}
	}

	void setupMaterial(const int &type)
	{

		if (type >= 0 && type < 13)
		{
			diffuse[0] = ColorArr[type][0];
			diffuse[1] = ColorArr[type][1];
			diffuse[2] = ColorArr[type][2];
		}
		else
		{
			ambient[0] = 1.0f;
			ambient[1] = 1.0f;
			ambient[2] = 1.0f;
			diffuse[0] = 1.0f;
			diffuse[1] = 1.0f;
			diffuse[2] = 1.0f;
		}

		GLfloat ambient5 = 0.5f;
		switch (type)
		{
		case COLOR_RED:
		{
			ambient[0] = ambient5;
		}
		break;
		case COLOR_GREEN:
		{
			ambient[1] = ambient5;
		}
		break;
		case COLOR_BLUE:
		{
			ambient[2] = ambient5;
		}
		break;
		case COLOR_YELLOW:
		{
			ambient[0] = ambient5;
		}
		break;
		case COLOR_PINK:
		{
			ambient[0] = ambient5;
			ambient[2] = ambient5;
		}
		break;
		default:
		{
			// ambient[0] = 1.0f;
			// ambient[1] = 1.0f;
			// ambient[2] = 1.0f;
		}
		break;
		}
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	}

	void DrawWireframe()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int f = 0; f < numFaces; f++)
		{
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int iv = face[f].vert[v].vertIndex;

				glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
			}
			glEnd();
		}
	}

	void DrawColor()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (int f = 0; f < numFaces; f++)
		{
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int iv = face[f].vert[v].vertIndex;
				int ic = face[f].vert[v].colorIndex;

				//            ic = f % COLORNUM;

				glColor3f(ColorArr[ic][0], ColorArr[ic][1], ColorArr[ic][2]);
				glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
			}
			glEnd();
		}
	}

	void Draw()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (int f = 0; f < numFaces; f++)
		{
			glBegin(GL_POLYGON);
			for (int v = 0; v < face[f].nVerts; v++)
			{
				int iv = face[f].vert[v].vertIndex;
				glNormal3f(face[f].facenorm.x, face[f].facenorm.y, face[f].facenorm.z);
				glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
			}
			glEnd();
		}
	}

	void CreateTetrahedron()
	{
		int i;
		numVerts = 4;
		pt = new Point3[numVerts];
		pt[0].set(0, 0, 0);
		pt[1].set(1, 0, 0);
		pt[2].set(0, 1, 0);
		pt[3].set(0, 0, 1);

		numFaces = 4;
		face = new Face[numFaces];

		face[0].nVerts = 3;
		face[0].vert = new VertexID[face[0].nVerts];
		face[0].vert[0].vertIndex = 1;
		face[0].vert[1].vertIndex = 2;
		face[0].vert[2].vertIndex = 3;
		for (i = 0; i < face[0].nVerts; i++)
			face[0].vert[i].colorIndex = 0;

		face[1].nVerts = 3;
		face[1].vert = new VertexID[face[1].nVerts];
		face[1].vert[0].vertIndex = 0;
		face[1].vert[1].vertIndex = 2;
		face[1].vert[2].vertIndex = 1;
		for (i = 0; i < face[1].nVerts; i++)
			face[1].vert[i].colorIndex = 1;

		face[2].nVerts = 3;
		face[2].vert = new VertexID[face[2].nVerts];
		face[2].vert[0].vertIndex = 0;
		face[2].vert[1].vertIndex = 3;
		face[2].vert[2].vertIndex = 2;
		for (i = 0; i < face[2].nVerts; i++)
			face[2].vert[i].colorIndex = 2;

		face[3].nVerts = 3;
		face[3].vert = new VertexID[face[3].nVerts];
		face[3].vert[0].vertIndex = 1;
		face[3].vert[1].vertIndex = 3;
		face[3].vert[2].vertIndex = 0;
		for (i = 0; i < face[3].nVerts; i++)
			face[3].vert[i].colorIndex = 3;
	}

	void CreateCube(float fSize)
	{
		int i;

		numVerts = 8;
		pt = new Point3[numVerts];

		pt[0].set(-fSize, fSize, fSize);
		pt[1].set(fSize, fSize, fSize);
		pt[2].set(fSize, fSize, -fSize);
		pt[3].set(-fSize, fSize, -fSize);
		pt[4].set(-fSize, -fSize, fSize);
		pt[5].set(fSize, -fSize, fSize);
		pt[6].set(fSize, -fSize, -fSize);
		pt[7].set(-fSize, -fSize, -fSize);

		numFaces = 6;
		face = new Face[numFaces];

		//Left face
		face[0].nVerts = 4;
		face[0].vert = new VertexID[face[0].nVerts];
		face[0].vert[0].vertIndex = 1;
		face[0].vert[1].vertIndex = 5;
		face[0].vert[2].vertIndex = 6;
		face[0].vert[3].vertIndex = 2;
		for (i = 0; i < face[0].nVerts; i++)
			face[0].vert[i].colorIndex = 0;

		//Right face
		face[1].nVerts = 4;
		face[1].vert = new VertexID[face[1].nVerts];
		face[1].vert[0].vertIndex = 0;
		face[1].vert[1].vertIndex = 3;
		face[1].vert[2].vertIndex = 7;
		face[1].vert[3].vertIndex = 4;
		for (i = 0; i < face[1].nVerts; i++)
			face[1].vert[i].colorIndex = 1;

		//top face
		face[2].nVerts = 4;
		face[2].vert = new VertexID[face[2].nVerts];
		face[2].vert[0].vertIndex = 0;
		face[2].vert[1].vertIndex = 1;
		face[2].vert[2].vertIndex = 2;
		face[2].vert[3].vertIndex = 3;
		for (i = 0; i < face[2].nVerts; i++)
			face[2].vert[i].colorIndex = 2;

		//bottom face
		face[3].nVerts = 4;
		face[3].vert = new VertexID[face[3].nVerts];
		face[3].vert[0].vertIndex = 7;
		face[3].vert[1].vertIndex = 6;
		face[3].vert[2].vertIndex = 5;
		face[3].vert[3].vertIndex = 4;
		for (i = 0; i < face[3].nVerts; i++)
			face[3].vert[i].colorIndex = 3;

		//near face
		face[4].nVerts = 4;
		face[4].vert = new VertexID[face[4].nVerts];
		face[4].vert[0].vertIndex = 4;
		face[4].vert[1].vertIndex = 5;
		face[4].vert[2].vertIndex = 1;
		face[4].vert[3].vertIndex = 0;
		for (i = 0; i < face[4].nVerts; i++)
			face[4].vert[i].colorIndex = 4;

		//Far face
		face[5].nVerts = 4;
		face[5].vert = new VertexID[face[5].nVerts];
		face[5].vert[0].vertIndex = 3;
		face[5].vert[1].vertIndex = 2;
		face[5].vert[2].vertIndex = 6;
		face[5].vert[3].vertIndex = 7;
		for (i = 0; i < face[5].nVerts; i++)
			face[5].vert[i].colorIndex = 5;
	}

	void CreateCylinder(int nSegment, float fHeight, float fRadius)
	{
		numVerts = nSegment * 2 + 2;
		pt = new Point3[numVerts];

		int i;
		int idx;
		float fAngle = 2 * M_PI / nSegment;
		float x, y, z;

		pt[0].set(0, fHeight, 0);
		for (i = 0; i < nSegment; i++)
		{
			x = fRadius * cos(fAngle * i);
			z = fRadius * sin(fAngle * i);
			y = fHeight;
			pt[i + 1].set(x, y, z);

			y = -fHeight;
			pt[i + 1 + nSegment].set(x, y, z);
		}
		pt[numVerts - 1].set(0, -fHeight, 0);

		numFaces = nSegment * 3;
		face = new Face[numFaces];

		idx = 0;
		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = 0;
			if (i < nSegment - 1)
				face[idx].vert[1].vertIndex = i + 2;
			else
				face[idx].vert[1].vertIndex = 1;
			face[idx].vert[2].vertIndex = i + 1;
			idx++;
		}

		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 4;
			face[idx].vert = new VertexID[face[idx].nVerts];

			face[idx].vert[0].vertIndex = i + 1;
			if (i < nSegment - 1)
				face[idx].vert[1].vertIndex = i + 2;
			else
				face[idx].vert[1].vertIndex = 1;
			face[idx].vert[2].vertIndex = face[idx].vert[1].vertIndex + nSegment;
			face[idx].vert[3].vertIndex = face[idx].vert[0].vertIndex + nSegment;

			idx++;
		}

		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = numVerts - 1;
			if (i < nSegment - 1)
				face[idx].vert[2].vertIndex = i + 2 + nSegment;
			else
				face[idx].vert[2].vertIndex = 1 + nSegment;
			face[idx].vert[1].vertIndex = i + 1 + nSegment;
			idx++;
		}
	}

	void CreateRectangular(float fLength, float fWidth, float fHeight)
	{
		int numVerts = 8;
		pt = new Point3[numVerts];
		pt[0].set(-fLength, fWidth, fHeight);
		pt[1].set(fLength, fWidth, fHeight);
		pt[2].set(fLength, fWidth, -fHeight);
		pt[3].set(-fLength, fWidth, -fHeight);
		pt[4].set(-fLength, -fWidth, fHeight);
		pt[5].set(fLength, -fWidth, fHeight);
		pt[6].set(fLength, -fWidth, -fHeight);
		pt[7].set(-fLength, -fWidth, -fHeight);

		numFaces = 6;
		int i;
		face = new Face[numFaces];

		//Left face
		face[0].nVerts = 4;
		face[0].vert = new VertexID[face[0].nVerts];
		face[0].vert[0].vertIndex = 1;
		face[0].vert[1].vertIndex = 5;
		face[0].vert[2].vertIndex = 6;
		face[0].vert[3].vertIndex = 2;

		for (i = 0; i < face[0].nVerts; i++)
			face[0].vert[i].colorIndex = 0;

		//Right face
		face[1].nVerts = 4;
		face[1].vert = new VertexID[face[1].nVerts];
		face[1].vert[0].vertIndex = 0;
		face[1].vert[1].vertIndex = 3;
		face[1].vert[2].vertIndex = 7;
		face[1].vert[3].vertIndex = 4;
		for (i = 0; i < face[1].nVerts; i++)
			face[1].vert[i].colorIndex = 1;

		//top face
		face[2].nVerts = 4;
		face[2].vert = new VertexID[face[2].nVerts];
		face[2].vert[0].vertIndex = 0;
		face[2].vert[1].vertIndex = 1;
		face[2].vert[2].vertIndex = 2;
		face[2].vert[3].vertIndex = 3;
		for (i = 0; i < face[2].nVerts; i++)
			face[2].vert[i].colorIndex = 2;

		//bottom face
		face[3].nVerts = 4;
		face[3].vert = new VertexID[face[3].nVerts];
		face[3].vert[0].vertIndex = 7;
		face[3].vert[1].vertIndex = 6;
		face[3].vert[2].vertIndex = 5;
		face[3].vert[3].vertIndex = 4;
		for (i = 0; i < face[3].nVerts; i++)
			face[3].vert[i].colorIndex = 3;

		//near face
		face[4].nVerts = 4;
		face[4].vert = new VertexID[face[4].nVerts];
		face[4].vert[0].vertIndex = 4;
		face[4].vert[1].vertIndex = 5;
		face[4].vert[2].vertIndex = 1;
		face[4].vert[3].vertIndex = 0;
		for (i = 0; i < face[4].nVerts; i++)
			face[4].vert[i].colorIndex = 4;

		//Far face
		face[5].nVerts = 4;
		face[5].vert = new VertexID[face[5].nVerts];
		face[5].vert[0].vertIndex = 3;
		face[5].vert[1].vertIndex = 2;
		face[5].vert[2].vertIndex = 6;
		face[5].vert[3].vertIndex = 7;
		for (i = 0; i < face[5].nVerts; i++)
			face[5].vert[i].colorIndex = 5;
	}

	void CreateTayQuay(int nSegment, float length, float radius, float height)
	{
		GLfloat alpha = M_PI / nSegment;
		int i = 0;
		numVerts = 4 * nSegment + 8;
		pt = new Point3[numVerts];
		height = height * 2;

		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(-length / 2 - radius * sin(j * alpha), height, radius * cos(j * alpha));
		}
		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(-length / 2 - radius * sin(j * alpha), 0, radius * cos(j * alpha));
		}
		/*=============================================================================*/

		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(length / 2 + radius * sin(j * alpha), height, radius * cos(j * alpha));
		}
		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(length / 2 + radius * sin(j * alpha), 0, radius * cos(j * alpha));
		}

		/*=============================================================================*/

		numFaces = 2 * nSegment + 10;
		face = new Face[numFaces];
		i = 0;
		int color = 0;
		/*=============================================================================*/

		for (int j = 0; j < nSegment - 1; j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 0 + j;
			face[i].vert[1].vertIndex = 0 + j + 1;
			face[i].vert[2].vertIndex = nSegment + 1 + j;
			face[i].vert[3].vertIndex = nSegment + j;
			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = color;
			}
			color++;
			i++;
		}

		/*=============================================================================*/

		for (int j = 0; j < nSegment - 1; j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 2 * nSegment + j;
			face[i].vert[1].vertIndex = 2 * nSegment + j + 1;
			face[i].vert[2].vertIndex = 2 * nSegment + nSegment + 1 + j;
			face[i].vert[3].vertIndex = 2 * nSegment + nSegment + j;
			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = color;
			}
			color++;
			i++;
		}

		/*=============================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = 2 * nSegment;
		face[i].vert[2].vertIndex = 3 * nSegment;
		face[i].vert[3].vertIndex = nSegment;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = nSegment - 1;
		face[i].vert[1].vertIndex = 3 * nSegment - 1;
		face[i].vert[2].vertIndex = 4 * nSegment - 1;
		face[i].vert[3].vertIndex = 2 * nSegment - 1;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = nSegment + nSegment;
		face[i].vert = new VertexID[face[i].nVerts];
		for (int j = 0; j < face[i].nVerts; j++)
		{
			if (j < nSegment)
			{
				face[i].vert[j].vertIndex = j;
			}
			else
			{
				face[i].vert[j].vertIndex = 4 * nSegment - 1 - j;
			}
		}
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = nSegment + nSegment;
		face[i].vert = new VertexID[face[i].nVerts];
		for (int j = 0; j < face[i].nVerts; j++)
		{
			if (j < nSegment)
			{
				face[i].vert[j].vertIndex = nSegment + j;
			}
			else
			{
				face[i].vert[j].vertIndex = 5 * nSegment - 1 - j;
			}
		}
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
	}

	void CreateThanhU(float length, float height, float size)
	{
		int i = 0;
		numVerts = 16;
		height = height * 2;
		pt = new Point3[numVerts];
		pt[i++].set(-length / 2 - size, 0, size);  //0
		pt[i++].set(-length / 2 - size, 0, -size); //1
		pt[i++].set(-length / 2, 0, -size);		   //2
		pt[i++].set(-length / 2, 0, size);		   //3

		pt[i++].set(-length / 2 - size, height, size);					//4
		pt[i++].set(-length / 2 - size, height, -size);					//5
		pt[i++].set(-length / 2, height - size * sin(M_PI / 4), -size); //6
		pt[i++].set(-length / 2, height - size * sin(M_PI / 4), size);	//7

		pt[i++].set(length / 2 + size, height, size);				   //8
		pt[i++].set(length / 2 + size, height, -size);				   //9
		pt[i++].set(length / 2, height - size * sin(M_PI / 4), -size); //10
		pt[i++].set(length / 2, height - size * sin(M_PI / 4), size);  //11

		pt[i++].set(length / 2, 0, size);		  //12
		pt[i++].set(length / 2, 0, -size);		  //13
		pt[i++].set(length / 2 + size, 0, -size); //14
		pt[i++].set(length / 2 + size, 0, size);  //15

		/*===========================================================================*/
		numFaces = 14;
		face = new Face[numFaces];
		i = 0;
		int color = 0;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = 1;
		face[i].vert[2].vertIndex = 2;
		face[i].vert[3].vertIndex = 3;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4;
		face[i].vert[1].vertIndex = 7;
		face[i].vert[2].vertIndex = 3;
		face[i].vert[3].vertIndex = 0;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 7;
		face[i].vert[1].vertIndex = 6;
		face[i].vert[2].vertIndex = 2;
		face[i].vert[3].vertIndex = 3;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 5;
		face[i].vert[1].vertIndex = 6;
		face[i].vert[2].vertIndex = 2;
		face[i].vert[3].vertIndex = 1;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4;
		face[i].vert[1].vertIndex = 5;
		face[i].vert[2].vertIndex = 1;
		face[i].vert[3].vertIndex = 0;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4;
		face[i].vert[1].vertIndex = 7;
		face[i].vert[2].vertIndex = 11;
		face[i].vert[3].vertIndex = 8;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4;
		face[i].vert[1].vertIndex = 5;
		face[i].vert[2].vertIndex = 9;
		face[i].vert[3].vertIndex = 8;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 5;
		face[i].vert[1].vertIndex = 9;
		face[i].vert[2].vertIndex = 10;
		face[i].vert[3].vertIndex = 6;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 7;
		face[i].vert[1].vertIndex = 6;
		face[i].vert[2].vertIndex = 10;
		face[i].vert[3].vertIndex = 11;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 11;
		face[i].vert[1].vertIndex = 8;
		face[i].vert[2].vertIndex = 15;
		face[i].vert[3].vertIndex = 12;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 8;
		face[i].vert[1].vertIndex = 9;
		face[i].vert[2].vertIndex = 14;
		face[i].vert[3].vertIndex = 15;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 9;
		face[i].vert[1].vertIndex = 10;
		face[i].vert[2].vertIndex = 13;
		face[i].vert[3].vertIndex = 14;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*===========================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 11;
		face[i].vert[1].vertIndex = 10;
		face[i].vert[2].vertIndex = 13;
		face[i].vert[3].vertIndex = 12;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
	}

	void CreateThanhLienKet(int nHalfSegment, float fLengthOuter, float fLengthInner, float fRadius, float fHeight, float fWidthInner)
	{
		thanhLienKetAngle = 90;

		GLfloat angle = M_PI / nHalfSegment;
		int i = 0;
		numVerts = 4 * nHalfSegment + 8 + 8 + 4 * nHalfSegment;
		pt = new Point3[numVerts];
		fHeight = fHeight * 2;
		fRadius = fRadius * 2;

		for (int j = 0; j < nHalfSegment; j++)
		{
			pt[i++].set(-fLengthOuter - fRadius * sin(j * angle), fHeight, fRadius * cos(j * angle));
		}
		for (int j = 0; j < nHalfSegment; j++)
		{
			pt[i++].set(-fLengthOuter - fRadius * sin(j * angle), 0, fRadius * cos(j * angle));
		}
		/*=============================================================================*/

		for (int j = 0; j < nHalfSegment; j++)
		{
			pt[i++].set(fLengthOuter + fRadius * sin(j * angle), fHeight, fRadius * cos(j * angle));
		}
		for (int j = 0; j < nHalfSegment; j++)
		{
			pt[i++].set(fLengthOuter + fRadius * sin(j * angle), 0, fRadius * cos(j * angle));
		}

		/*=============================================================================*/
		// 4*nHalfSegment
		pt[i++].set(-fLengthInner, fHeight, fWidthInner);
		// 4*nHalfSegment + 1
		pt[i++].set(-fLengthInner, fHeight, -fWidthInner);
		// 4*nHalfSegment + 2
		pt[i++].set(fLengthInner, fHeight, -fWidthInner);
		// 4*nHalfSegment + 3
		pt[i++].set(fLengthInner, fHeight, fWidthInner);

		//    =============================================================================
		// 4*nHalfSegment + 4
		pt[i++].set(-fLengthInner, 0, fWidthInner);
		// 4*nHalfSegment + 5
		pt[i++].set(-fLengthInner, 0, -fWidthInner);
		// 4*nHalfSegment + 6
		pt[i++].set(fLengthInner, 0, -fWidthInner);
		// 4*nHalfSegment + 7
		pt[i++].set(fLengthInner, 0, fWidthInner);

		// 4*nHalfSegment + 8
		pt[i++].set(-fLengthInner, fHeight, fRadius);
		// 4*nHalfSegment + 9
		pt[i++].set(-fLengthInner, fHeight, -fRadius);
		// 4*nHalfSegment + 10
		pt[i++].set(fLengthInner, fHeight, -fRadius);
		// 4*nHalfSegment + 11
		pt[i++].set(fLengthInner, fHeight, fRadius);

		// 4*nHalfSegment + 12
		pt[i++].set(-fLengthInner, 0, fRadius);
		// 4*nHalfSegment + 13
		pt[i++].set(-fLengthInner, 0, -fRadius);
		// 4*nHalfSegment + 14
		pt[i++].set(fLengthInner, 0, -fRadius);
		// 4*nHalfSegment + 15
		pt[i++].set(fLengthInner, 0, fRadius);

		/*=============================================================================*/

		numFaces = 2 * nHalfSegment + 10 + nHalfSegment;
		face = new Face[numFaces];
		i = 0;
		int color = 0;
		/*=============================================================================*/

		for (int j = 0; j < nHalfSegment - 1; j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 0 + j;
			face[i].vert[1].vertIndex = 0 + j + 1;
			face[i].vert[2].vertIndex = nHalfSegment + 1 + j;
			face[i].vert[3].vertIndex = nHalfSegment + j;
			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = color;
			}
			color++;
			i++;
		}

		/*=============================================================================*/

		for (int j = 0; j < nHalfSegment - 1; j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 2 * nHalfSegment + j;
			face[i].vert[1].vertIndex = 2 * nHalfSegment + j + 1;
			face[i].vert[2].vertIndex = 2 * nHalfSegment + nHalfSegment + 1 + j;
			face[i].vert[3].vertIndex = 2 * nHalfSegment + nHalfSegment + j;
			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = color;
			}
			color++;
			i++;
		}

		/*=============================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = 2 * nHalfSegment;
		face[i].vert[2].vertIndex = 3 * nHalfSegment;
		face[i].vert[3].vertIndex = nHalfSegment;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = nHalfSegment - 1;
		face[i].vert[1].vertIndex = 3 * nHalfSegment - 1;
		face[i].vert[2].vertIndex = 4 * nHalfSegment - 1;
		face[i].vert[3].vertIndex = 2 * nHalfSegment - 1;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 1;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 5;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 4;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment + 1;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 2;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 6;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 5;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment + 2;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 3;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 7;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 6;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment + 3;
		face[i].vert[1].vertIndex = 4 * nHalfSegment;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 4;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 7;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 8;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 11;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 3;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment + 1;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 9;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 10;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 2;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment + 12;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 4;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 7;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 15;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/

		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4 * nHalfSegment + 5;
		face[i].vert[1].vertIndex = 4 * nHalfSegment + 13;
		face[i].vert[2].vertIndex = 4 * nHalfSegment + 14;
		face[i].vert[3].vertIndex = 4 * nHalfSegment + 6;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = nHalfSegment + 2;
		face[i].vert = new VertexID[face[i].nVerts];
		for (int j = 0; j < nHalfSegment; j++)
		{
			face[i].vert[j].vertIndex = j;
		}
		face[i].vert[nHalfSegment].vertIndex = 4 * nHalfSegment + 9;
		face[i].vert[nHalfSegment + 1].vertIndex = 4 * nHalfSegment + 8;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = nHalfSegment + 2;
		face[i].vert = new VertexID[face[i].nVerts];
		for (int j = 0; j < nHalfSegment; j++)
		{
			face[i].vert[j].vertIndex = 2 * nHalfSegment + j;
		}
		face[i].vert[nHalfSegment].vertIndex = 4 * nHalfSegment + 10;
		face[i].vert[nHalfSegment + 1].vertIndex = 4 * nHalfSegment + 11;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = nHalfSegment + 2;
		face[i].vert = new VertexID[face[i].nVerts];
		for (int j = 0; j < nHalfSegment; j++)
		{
			face[i].vert[j].vertIndex = nHalfSegment + j;
		}
		face[i].vert[nHalfSegment].vertIndex = 4 * nHalfSegment + 13;
		face[i].vert[nHalfSegment + 1].vertIndex = 4 * nHalfSegment + 12;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;

		/*=============================================================================*/
		face[i].nVerts = nHalfSegment + 2;
		face[i].vert = new VertexID[face[i].nVerts];
		for (int j = 0; j < nHalfSegment; j++)
		{
			face[i].vert[j].vertIndex = 3 * nHalfSegment + j;
		}
		face[i].vert[nHalfSegment].vertIndex = 4 * nHalfSegment + 14;
		face[i].vert[nHalfSegment + 1].vertIndex = 4 * nHalfSegment + 15;
		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = color;
		}
		color++;
		i++;
	}

	void CreateBearing(float fHeight, float fWidth, int nUpperSegment, float outerRadius, float innerRadius)
	{

		// nUpperSegment: so diem tren mot nua hinh tron
		int M = (nUpperSegment - 1) * 2; // M: So diem tren mot hinh tron
		GLfloat angle = 2 * M_PI / M;

		int i = 0;
		numVerts = M * 2 + nUpperSegment * 2 + nUpperSegment * 2 + 4;
		pt = new Point3[numVerts];

		// Vong tron trong ben trai: [0, M - 1]
		for (int j = 0; j < M; j++)
		{
			pt[i].set(innerRadius * cos((double)j * angle), fHeight + innerRadius * sin((double)j * angle), fWidth);
			i++;
		}
		// Vong tron trong ben phai: [M, 2M - 1]
		for (int j = 0; j < M; j++)
		{
			pt[i].set(innerRadius * cos((double)j * angle), fHeight + innerRadius * sin((double)j * angle), -fWidth);
			i++;
		}
		// Nua vong tron ngoai ben trai: [2M, 2M + nUpperSegment - 1]
		for (int j = 0; j < nUpperSegment; j++)
		{
			pt[i].set(outerRadius * cos((double)j * angle), fHeight + outerRadius * sin((double)j * angle), fWidth);
			i++;
		}
		// Nua vong tron ngoai ben phai: [2M + nUpperSegment, 2M + 2N - 1]
		for (int j = 0; j < nUpperSegment; j++)
		{
			pt[i].set(outerRadius * cos((double)j * angle), fHeight + outerRadius * sin((double)j * angle), -fWidth);
			i++;
		}
		// Canh trai: [2M + 2N, 2M + 3N - 1]
		for (int j = 0; j < nUpperSegment; j++)
		{
			pt[i].set(innerRadius * cos((double)j * angle), 0, fWidth);
			i++;
		}
		// Canh phai: [2M + 3N, 2M + 4N - 1]
		for (int j = 0; j < nUpperSegment; j++)
		{
			pt[i].set(innerRadius * cos((double)j * angle), 0, -fWidth);
			i++;
		}
		// 4 goc: [2M + 4N, 2M + 4N + 3]
		pt[i].set(outerRadius, 0, fWidth);
		i++;
		pt[i].set(outerRadius, 0, -fWidth);
		i++;
		pt[i].set(-outerRadius, 0, -fWidth);
		i++;
		pt[i].set(-outerRadius, 0, fWidth);
		i++;

		/******************************************************************************************************************/
		numFaces = M + nUpperSegment - 1 + nUpperSegment - 1 + nUpperSegment - 1 + nUpperSegment - 1 + nUpperSegment - 1 + 1 + 1 + 1 + 1 + 1 + 1;
		face = new Face[numFaces];

		int mau = 0;
		i = 0;

		// Mat trong hinh tron
		for (int j = 0; j < (M - 1); j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];

			face[i].vert[0].vertIndex = j;
			face[i].vert[1].vertIndex = j + 1;
			face[i].vert[2].vertIndex = j + M + 1;
			face[i].vert[3].vertIndex = j + M;

			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = mau;
			}
			mau++;
			i++;
		}
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = (M - 1);
		face[i].vert[1].vertIndex = 0;
		face[i].vert[2].vertIndex = M;
		face[i].vert[3].vertIndex = 2 * M - 1;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		// Mat ben trai, canh phia tren hinh tron
		for (int j = 0; j < (nUpperSegment - 1); j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];

			face[i].vert[0].vertIndex = j + 2 * M;
			face[i].vert[1].vertIndex = j + 2 * M + 1;
			face[i].vert[2].vertIndex = j + 1;
			face[i].vert[3].vertIndex = j;

			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = mau;
			}
			mau++;
			i++;
		}
		// Mat ngoai nua tren hinh tron
		for (int j = 0; j < (nUpperSegment - 1); j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];

			face[i].vert[0].vertIndex = j + 2 * M + nUpperSegment;
			face[i].vert[1].vertIndex = j + 2 * M + nUpperSegment + 1;
			face[i].vert[2].vertIndex = j + 2 * M + 1;
			face[i].vert[3].vertIndex = j + 2 * M;

			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = mau;
			}
			mau++;
			i++;
		}
		// Mat ben phai, nua tren hinh tron
		for (int j = 0; j < (nUpperSegment - 1); j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];

			face[i].vert[0].vertIndex = j + 2 * M + nUpperSegment + 1;
			face[i].vert[1].vertIndex = j + 2 * M + nUpperSegment;
			face[i].vert[2].vertIndex = j + M;
			face[i].vert[3].vertIndex = j + M + 1;

			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = mau;
			}
			mau++;
			i++;
		}
		// Mat ben trai, nua duoi hinh tron
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = (M - 1);
		face[i].vert[2].vertIndex = 2 * M + 2 * nUpperSegment + 1;
		face[i].vert[3].vertIndex = 2 * M + 2 * nUpperSegment;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		for (int j = 1; j < (nUpperSegment - 1); j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];

			face[i].vert[0].vertIndex = M - j;
			face[i].vert[1].vertIndex = M - j - 1;
			face[i].vert[2].vertIndex = j + 2 * M + 2 * nUpperSegment + 1;
			face[i].vert[3].vertIndex = j + 2 * M + 2 * nUpperSegment;

			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = mau;
			}
			mau++;
			i++;
		}
		// Mat ben phai, nua duoi hinh tron
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = M;
		face[i].vert[1].vertIndex = 2 * M - 1;
		face[i].vert[2].vertIndex = 2 * M + 3 * nUpperSegment + 1;
		face[i].vert[3].vertIndex = 2 * M + 3 * nUpperSegment;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		for (int j = 1; j < (nUpperSegment - 1); j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];

			face[i].vert[0].vertIndex = 2 * M - j;
			face[i].vert[1].vertIndex = 2 * M - j - 1;
			face[i].vert[2].vertIndex = j + 2 * M + 3 * nUpperSegment + 1;
			face[i].vert[3].vertIndex = j + 2 * M + 3 * nUpperSegment;

			for (int k = 0; k < face[i].nVerts; k++)
			{
				face[i].vert[k].colorIndex = mau;
			}
			mau++;
			i++;
		}
		// Mat cua goc: x > 0, z > 0;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = 2 * M;
		face[i].vert[1].vertIndex = 0;
		face[i].vert[2].vertIndex = 2 * M + 2 * nUpperSegment;
		face[i].vert[3].vertIndex = 2 * M + 4 * nUpperSegment;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		// Mat cua goc: x > 0, z < 0;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = M;
		face[i].vert[1].vertIndex = 2 * M + nUpperSegment;
		face[i].vert[2].vertIndex = 2 * M + 4 * nUpperSegment + 1;
		face[i].vert[3].vertIndex = 2 * M + 3 * nUpperSegment;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		// Mat cua goc: x < 0, z < 0;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = 2 * M + 2 * nUpperSegment - 1;
		face[i].vert[1].vertIndex = M + (nUpperSegment - 1);
		face[i].vert[2].vertIndex = 2 * M + 4 * nUpperSegment - 1;
		face[i].vert[3].vertIndex = 2 * M + 4 * nUpperSegment + 2;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		// Mat cua goc: x < 0, z > 0;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = 0 + (nUpperSegment - 1);
		face[i].vert[1].vertIndex = 2 * M + nUpperSegment - 1;
		face[i].vert[2].vertIndex = 2 * M + 4 * nUpperSegment + 3;
		face[i].vert[3].vertIndex = 2 * M + 3 * nUpperSegment - 1;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		// Mat x > 0
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = 2 * M + nUpperSegment;
		face[i].vert[1].vertIndex = 2 * M;
		face[i].vert[2].vertIndex = 2 * M + 4 * nUpperSegment;
		face[i].vert[3].vertIndex = 2 * M + 4 * nUpperSegment + 1;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
		// Mat x < 0
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];

		face[i].vert[0].vertIndex = 2 * M + nUpperSegment - 1;
		face[i].vert[1].vertIndex = 2 * M + 2 * nUpperSegment - 1;
		face[i].vert[2].vertIndex = 2 * M + 4 * nUpperSegment + 2;
		face[i].vert[3].vertIndex = 2 * M + 4 * nUpperSegment + 3;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
};

#pragma endregion

float baseMeasure = 2;

Mesh baseFoot;
float baseFootLength = baseMeasure;
float baseFootWidth = baseFootLength * 0.37;
float baseFootHeight = baseFootLength * 0.05;

Mesh baseBody; //Cột // reactangular
float baseBodyLength = baseFootLength * 0.19;
float baseBodyWidth = baseFootHeight;
float baseBodyHeight = baseFootLength * 0.82;

Mesh baseHead;
float baseHeadLength = baseFootLength;
float baseHeadWidth = baseBodyWidth;
float baseHeadHeight = baseHeadLength * 0.35;

Mesh bearingLeft;
Mesh bearingLeftBox;
Mesh bearingRight;
Mesh bearingRightBox;
float bearingBoxLength = baseHeadHeight * 0.55;
float bearingBoxHeight = baseHeadWidth * 0.6;
float bearingBoxWidth = bearingBoxLength * 0.26;
int bearingNUpperSegment = 32;
float bearingWidth = bearingBoxWidth;
float bearingOuterRadius = bearingBoxLength * 0.75;
float bearingInnerRadius = bearingOuterRadius * 0.6;
float bearingHeight = baseHeadHeight * 1.2 - bearingBoxHeight * 2 - bearingOuterRadius;

Mesh rotationDisk;
float rotationDiskSegment = 64;
float rotationDiskHeight = baseBodyWidth;
float rotationDiskRadius = baseBodyLength * 1.5;
float rotationSpeed = 5;

Mesh tayQuay;
float tayQuaySegments = 20;
float tayQuayLength = rotationDiskRadius * 0.88;
float tayQuayHeight = rotationDiskHeight * 0.7;
float tayQuayRadius = rotationDiskHeight * 0.8;

Mesh cottar3;
int cottar3Segment = 32;
float cottar3Height = (baseFootWidth - baseBodyWidth) / 2;
float cottar3Radius = baseBodyWidth;

Mesh cottar2OuterCube;
float cottar2OuterCubeSize = tayQuayHeight;

Mesh cottar2;
int cottar2Segment = 6;
float cottar2Height = cottar2OuterCubeSize;
float cottar2Radius = cottar2OuterCubeSize * 0.7;

Mesh cottar1OuterCube;
float cottar1OuterCubeSize = tayQuayHeight;

Mesh cottar1;
int cottar1Segment = 6;
float cottar1Height = cottar2OuterCubeSize;
float cottar1Radius = cottar2OuterCubeSize * 0.7;

Mesh thanhLienKet;
float thanhLienKetnHalfSegment = 36;
float thanhLienKetWidthInner = cottar2OuterCubeSize;
float thanhLienKetLengthOuter = baseBodyHeight * 0.8;
float thanhLienKetLengthInner = thanhLienKetLengthOuter * 0.63;
float thanhLienKetRadius = thanhLienKetWidthInner * 1.5;
float thanhLienKetHeight = cottar2OuterCubeSize;

Mesh thanhU;
float thanhULength = cottar1Height * 2;
float thanhUHeight = baseHeadHeight * 1.05;
float thanhUSize = cottar1Height;

Mesh thanhTruot;
int thanhTruotSegment = 48;
float thanhTruotHeight = baseHeadLength * 1.75;
float thanhTruotRadius = bearingInnerRadius;

void createObject()
{
	baseFoot.CreateRectangular(baseFootLength, baseFootHeight, baseFootWidth);

	baseBody.CreateRectangular(baseBodyLength, baseBodyHeight, baseBodyWidth);

	baseHead.CreateRectangular(baseHeadLength, baseHeadHeight, baseHeadWidth);

	cottar3.CreateCylinder(cottar3Segment, cottar3Height, cottar3Radius);

	// Create thanhLienKet, disk, tayQuay before cottar 2 to fix motion bug
	cottar2OuterCube.CreateCube(cottar2OuterCubeSize);

	cottar2.CreateCylinder(cottar2Segment, cottar2Height, cottar2Radius);
	cottar2.distance = 0.25;

	tayQuay.CreateTayQuay(tayQuaySegments, tayQuayLength, tayQuayRadius, tayQuayHeight);

	thanhLienKet.CreateThanhLienKet(thanhLienKetnHalfSegment, thanhLienKetLengthOuter, thanhLienKetLengthInner, thanhLienKetRadius, thanhLienKetHeight, thanhLienKetWidthInner);

	rotationDisk.CreateCylinder(rotationDiskSegment, rotationDiskHeight, rotationDiskRadius);

	cottar1OuterCube.CreateCube(cottar1OuterCubeSize);

	cottar1.CreateCylinder(cottar1Segment, cottar1Height, cottar1Radius);

	thanhU.CreateThanhU(thanhULength, thanhUHeight, thanhUSize);

	bearingRightBox.CreateRectangular(bearingBoxLength, bearingBoxHeight, bearingBoxWidth);

	bearingRight.CreateBearing(bearingHeight, bearingWidth, bearingNUpperSegment, bearingOuterRadius, bearingInnerRadius);

	bearingLeftBox.CreateRectangular(bearingBoxLength, bearingBoxHeight, bearingBoxWidth);

	bearingLeft.CreateBearing(bearingHeight, bearingWidth, bearingNUpperSegment, bearingOuterRadius, bearingInnerRadius);

	thanhTruot.CreateCylinder(thanhTruotSegment, thanhTruotHeight, thanhTruotRadius);
}

void recalculateThanhLienKetAngle()
{
	/* 
    calculate góc alpha trong tam giác sau
    với alpha = góc giữa cạnh
    (chốt 2) A --------- B (tâm bàn quay)
               \       |
                \      |
                 \     |
                  \    |
                   \   |
                    \  |
                     \ |
                      \| <-- góc alpha
                       C (chốt 1)
    */
	double disCottar3_2, disCottar3_DiskCenter, disCottar2_DiskCenter;
	disCottar3_2 = sqrt(pow((cottar2.positionX - cottar3.positionX), 2) + pow((cottar2.positionY - cottar3.positionY), 2));
	disCottar2_DiskCenter = sqrt(pow((rotationDisk.positionX - cottar2.positionX), 2) + pow((rotationDisk.positionY - cottar2.positionY), 2));
	disCottar3_DiskCenter = sqrt(pow((rotationDisk.positionX - cottar3.positionX), 2) + pow((rotationDisk.positionY - cottar3.positionY), 2));
	// printf("a = %.2f   ~~~   ", disCottar3_2);
	// printf("b = %.2f   ~~~   ", disCottar3_DiskCenter);
	// printf("c = %.2f\n", disCottar2_DiskCenter);
	double alpha;
	alpha = pow(disCottar3_2, 2) + pow(disCottar3_DiskCenter, 2) - pow(disCottar2_DiskCenter, 2);
	alpha = alpha / (2 * disCottar3_2 * disCottar3_DiskCenter);
	alpha = acos(alpha) * 180 / M_PI;
	alpha += 90; // rotate verticaly
	// printf("x = %.2f; alpha = %.2f\n", cottar2.positionX, alpha);
	// printf("%.5f\n", alpha);
	if (cottar2.positionX > 0)
	{
		alpha = -alpha + 180;
	}
	thanhLienKet.thanhLienKetAngle = alpha;
}

void drawAxis()
{
	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(-4, 0, 0);
	glVertex3f(4, 0, 0);
	glEnd();

	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 4, 0);
	glEnd();

	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 4);
	glEnd();
}

void drawPart_ColorOrWireframe(Mesh &part)
{
	if (bDrawWireFrame)
		part.DrawWireframe();
	else
		part.Draw();
}

void drawBaseFoot()
{
	glPushMatrix();
	baseFoot.positionY = baseFootHeight;
	glTranslated(baseFoot.positionX, baseFoot.positionY, baseFoot.positionZ);

	baseFoot.setupMaterial(COLOR_BLUE);
	baseFoot.SetColor(COLOR_BLUE);
	baseFoot.CalculateFacesNorm();
	drawPart_ColorOrWireframe(baseFoot);

	glPopMatrix();
}

void drawBaseBody()
{
	glPushMatrix();
	baseBody.positionY = baseFoot.positionY * 2 + baseBodyHeight;
	glTranslated(baseBody.positionX, baseBody.positionY, baseBody.positionZ);
	baseBody.setupMaterial(COLOR_BLUE);
	baseBody.SetColor(COLOR_GREEN);
	baseBody.CalculateFacesNorm();

	drawPart_ColorOrWireframe(baseBody);

	glPopMatrix();
}

void drawBaseHead()
{
	glPushMatrix();
	baseHead.positionY = baseBody.positionY + baseBodyHeight + baseHeadHeight;
	glTranslated(baseHead.positionX, baseHead.positionY, baseHead.positionZ);
	baseHead.setupMaterial(COLOR_BLUE);
	baseHead.SetColor(COLOR_BLUE);
	baseHead.CalculateFacesNorm();

	drawPart_ColorOrWireframe(baseHead);

	glPopMatrix();
}

void drawBearingRight()
{
	glPushMatrix();
	bearingRightBox.positionX = baseHeadLength - bearingBoxWidth * 2;
	bearingRightBox.positionY = 2 * baseFootHeight + 2 * baseBodyHeight + baseHeadHeight;
	bearingRightBox.positionZ = baseHead.positionZ + baseHeadWidth + bearingBoxHeight;
	glTranslated(bearingRightBox.positionX, bearingRightBox.positionY, bearingRightBox.positionZ);
	glRotated(90, 1, 0, 0);
	glRotated(90, 0, 1, 0);
	bearingRightBox.setupMaterial(COLOR_RED);
	bearingRightBox.SetColor(COLOR_RED);
	bearingRightBox.CalculateFacesNorm();
	drawPart_ColorOrWireframe(bearingRightBox);

	glTranslated(0, 0, 0);
	glPopMatrix();

	glPushMatrix();
	bearingRight.positionX = bearingRightBox.positionX;
	bearingRight.positionY = bearingRightBox.positionY;
	bearingRight.positionZ = bearingRightBox.positionZ + bearingBoxHeight;
	glTranslated(bearingRight.positionX, bearingRight.positionY, bearingRight.positionZ);
	glRotated(90, 1, 0, 0);
	glRotated(90, 0, 1, 0);

	bearingRight.setupMaterial(COLOR_RED);
	bearingRight.SetColor(COLOR_RED);
	bearingRight.CalculateFacesNorm();
	drawPart_ColorOrWireframe(bearingRight);

	glPopMatrix();
}

void drawBearingLeft()
{
	glPushMatrix();
	bearingLeftBox.positionX = -bearingRightBox.positionX;
	bearingLeftBox.positionY = bearingRightBox.positionY;
	bearingLeftBox.positionZ = bearingRight.positionZ - bearingBoxHeight;
	glTranslated(bearingLeftBox.positionX, bearingLeftBox.positionY, bearingLeftBox.positionZ);
	glRotated(90, 1, 0, 0);
	glRotated(90, 0, 1, 0);
	bearingLeftBox.setupMaterial(COLOR_RED);
	bearingLeftBox.SetColor(COLOR_RED);
	bearingLeftBox.CalculateFacesNorm();

	drawPart_ColorOrWireframe(bearingLeftBox);

	glTranslated(0, 0, 0);
	glPopMatrix();

	glPushMatrix();
	bearingLeft.positionX = bearingLeftBox.positionX;
	bearingLeft.positionY = bearingLeftBox.positionY;
	bearingLeft.positionZ = bearingLeftBox.positionZ + bearingBoxHeight;
	glTranslated(bearingLeft.positionX, bearingLeft.positionY, bearingLeft.positionZ);
	glRotated(90, 1, 0, 0);
	glRotated(90, 0, 1, 0);

	bearingLeft.setupMaterial(COLOR_RED);
	bearingLeft.SetColor(COLOR_RED);
	bearingLeft.CalculateFacesNorm();

	drawPart_ColorOrWireframe(bearingLeft);

	glPopMatrix();
}

void drawBearings()
{
	drawBearingRight();
	drawBearingLeft();
}

void drawCottar3()
{
	glPushMatrix();
	cottar3.positionY = 2 * baseFootHeight + baseBodyHeight * 0.25;
	cottar3.positionZ = baseBodyWidth + cottar3Height;
	glTranslated(cottar3.positionX, cottar3.positionY, cottar3.positionZ);

	glRotatef(90, 1, 0, 0);
	bearingLeft.setupMaterial(COLOR_DARK_GRAY);
	cottar3.SetColor(COLOR_DARK_GRAY);
	cottar3.CalculateFacesNorm();

	drawPart_ColorOrWireframe(cottar3);

	glPopMatrix();
}

void drawCottar2()
{
	float offsetX = cottar2.distance * sin(rotationDisk.rotateZ * M_PI / 180);
	float offsetY = cottar2.distance * cos(rotationDisk.rotateZ * M_PI / 180);
	cottar2.positionX = -offsetX;
	cottar2.positionY = rotationDisk.positionY + offsetY;
	cottar2.positionZ = tayQuay.positionZ + tayQuayHeight * 2 + cottar2Height;
	glPushMatrix();
	glTranslated(cottar2.positionX, cottar2.positionY, cottar2.positionZ);
	glRotated(thanhLienKet.thanhLienKetAngle, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	cottar2.setupMaterial(COLOR_DARK_GRAY);
	cottar2.SetColor(COLOR_DARK_GRAY);
	cottar2.CalculateFacesNorm();

	drawPart_ColorOrWireframe(cottar2);

	glPopMatrix();

	glPushMatrix();
	glTranslated(cottar2.positionX, cottar2.positionY, cottar2.positionZ);
	glRotated(thanhLienKet.thanhLienKetAngle, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	cottar2OuterCube.setupMaterial(COLOR_RED);
	cottar2OuterCube.SetColor(COLOR_RED);
	cottar2OuterCube.CalculateFacesNorm();

	drawPart_ColorOrWireframe(cottar2OuterCube);

	glPopMatrix();

	recalculateThanhLienKetAngle();
}

void drawCottar1()
{

	float rad = thanhLienKet.thanhLienKetAngle * M_PI / 180;
	cottar1.positionX = thanhLienKetLengthOuter * 2 * cos(rad);
	cottar1.positionY = cottar3.positionY + thanhLienKetLengthOuter * 2 * sin(rad) - thanhLienKetRadius / 2;
	cottar1.positionZ = thanhLienKet.positionZ + thanhLienKetHeight * 2 + cottar1Height;

	glPushMatrix();
	glTranslated(cottar1.positionX, cottar1.positionY, cottar1.positionZ);
	glRotated(90, 1, 0, 0);
	cottar1.setupMaterial(COLOR_DARK_GRAY);
	cottar1.SetColor(COLOR_DARK_GRAY);
	cottar1.CalculateFacesNorm();

	drawPart_ColorOrWireframe(cottar1);

	glPopMatrix();
	glPushMatrix();
	glTranslated(cottar1.positionX, cottar1.positionY, cottar1.positionZ);
	glRotated(90, 1, 0, 0);
	cottar1OuterCube.setupMaterial(COLOR_RED);
	cottar1OuterCube.SetColor(COLOR_RED);
	cottar1OuterCube.CalculateFacesNorm();

	drawPart_ColorOrWireframe(cottar1OuterCube);

	glPopMatrix();
}

void drawRotationDisk()
{
	glPushMatrix();
	rotationDisk.positionY = baseBody.positionY;
	// rotationDisk.positionZ = baseBodyWidth + rotationDiskHeight;
	rotationDisk.positionZ = baseBodyWidth + rotationDiskHeight;
	glTranslated(rotationDisk.positionX, rotationDisk.positionY, rotationDisk.positionZ);
	glRotated(rotationDisk.rotateZ, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	rotationDisk.setupMaterial(COLOR_RED);
	rotationDisk.SetColor(COLOR_RED);
	rotationDisk.CalculateFacesNorm();

	drawPart_ColorOrWireframe(rotationDisk);

	glPopMatrix();
}

void drawTayQuay()
{
	glPushMatrix();
	tayQuay.positionY = rotationDisk.positionY;
	tayQuay.positionZ = rotationDisk.positionZ + rotationDiskHeight;
	glTranslated(tayQuay.positionX, tayQuay.positionY, tayQuay.positionZ);
	glRotated(90, 0, 0, 1);
	glRotated(rotationDisk.rotateZ, 0, 0, 1);
	glTranslated(tayQuayLength / 2, 0, 0);
	glRotated(90, 1, 0, 0);
	tayQuay.setupMaterial(COLOR_PINK);
	tayQuay.SetColor(COLOR_PINK);
	tayQuay.CalculateFacesNorm();

	drawPart_ColorOrWireframe(tayQuay);

	glPopMatrix();
}

void drawThanhLienKet()
{
	glPushMatrix();
	// float positionY = 2 * baseFootHeight + baseBodyHeight - thanhLienKetLengthOuter / 2;
	thanhLienKet.positionY = 2 * baseFootHeight + baseBodyHeight * 0.25;
	thanhLienKet.positionZ = tayQuay.positionZ + tayQuayHeight * 2;
	glTranslated(thanhLienKet.positionX, thanhLienKet.positionY, thanhLienKet.positionZ);

	glRotated(thanhLienKet.thanhLienKetAngle, 0, 0, 1);
	glTranslated(thanhLienKetLengthOuter, 0, 0);
	glRotated(90, 1, 0, 0);
	thanhLienKet.setupMaterial(COLOR_GREEN);
	thanhLienKet.SetColor(COLOR_GREEN);
	thanhLienKet.CalculateFacesNorm();

	drawPart_ColorOrWireframe(thanhLienKet);

	glPopMatrix();
}

void drawthanhU()
{
	glPushMatrix();
	thanhU.positionX = cottar1.positionX;
	thanhU.positionY = baseFootHeight + 2 * baseBodyHeight - thanhUHeight - thanhTruotRadius / 2;
	thanhU.positionZ = cottar1.positionZ;
	glTranslated(thanhU.positionX, thanhU.positionY, thanhU.positionZ);

	thanhU.setupMaterial(COLOR_DARK_GRAY);
	thanhU.SetColor(COLOR_GRAY);
	thanhU.CalculateFacesNorm();

	drawPart_ColorOrWireframe(thanhU);

	glPopMatrix();
}

void drawThanhTruot()
{
	glPushMatrix();
	thanhTruot.positionX = thanhU.positionX;
	thanhTruot.positionY = 2 * baseFootHeight + 2 * baseBodyHeight + baseHeadHeight;
	thanhTruot.positionZ = baseHead.positionZ + baseHeadWidth + bearingBoxHeight * 2 + bearingHeight - bearingInnerRadius + thanhTruotRadius;
	glTranslated(thanhTruot.positionX, thanhTruot.positionY, thanhTruot.positionZ);
	glRotated(90, 0, 0, 1);
	thanhTruot.setupMaterial(COLOR_DARK_GRAY);
	thanhTruot.SetColor(COLOR_DARK_GRAY);
	thanhTruot.CalculateFacesNorm();

	drawPart_ColorOrWireframe(thanhTruot);

	glPopMatrix();
}

void drawAllObject()
{
	drawBaseFoot();
	drawBaseBody();
	drawCottar3();
	drawBaseHead();
	drawRotationDisk();
	drawTayQuay();
	drawCottar2();
	drawThanhLienKet();
	drawBearings();
	drawCottar1();
	drawthanhU();
	drawThanhTruot();
}

void switchCameraView()
{
	if (bLockCamera)
	{
		cameraDistance_init = cameraDistance;
		cameraHeight_init = cameraHeight;
		cameraAngle_init = cameraAngle;

		cameraDistance = 0;
		cameraHeight = 20;
		cameraAngle = 180;
		float fHalfSizeHorizontal = 10.0f;
		float fHalfSizeVertical = fHalfSizeHorizontal / screenRatio;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-fHalfSizeHorizontal, fHalfSizeHorizontal, -fHalfSizeVertical, fHalfSizeVertical, -1000, 1000);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else
	{
		cameraDistance = cameraDistance_init;
		cameraHeight = cameraHeight_init;
		cameraAngle = cameraAngle_init;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-screenRatio, screenRatio, -1.0, 1.0, 1.5, 50.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

void switchSecondLight()
{
	if (bLight1on)
	{
		glEnable(GL_LIGHT1);
		GLfloat diffuse1[] = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat specular1[] = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat ambient1[] = {0.4f, 0.4f, 0.4f, 1.0f};
		GLfloat position1[] = {0.0f, 1.0f, -1.0f, 0.0f};

		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
	}
	else
		glDisable(GL_LIGHT1);
}

void displayMe(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	switchSecondLight();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float camera_x = cameraDistance * sin(cameraAngle * M_PI / 180);
	float camera_y = cameraHeight;
	float camera_z = cameraDistance * cos(cameraAngle * M_PI / 180);

	if (cameraDistance == 0)
	{
		gluLookAt(camera_x, camera_y, camera_z, lookAtX, lookAtY, lookAtZ, sinf(cameraAngle * M_PI / 180), 0, cosf(cameraAngle * M_PI / 180));
	}
	else
	{
		gluLookAt(camera_x, camera_y, camera_z, lookAtX, lookAtY, lookAtZ, 0, 1, 0);
	}
	glViewport(0, 0, screenWidth, screenHeight);
	drawAllObject();

	drawAxis();

	glFlush();
	glutSwapBuffers();
}

void myInit()
{
	lookAtX = 0;
	lookAtY = 1;
	lookAtZ = 0;

	float fHalfSize = 4;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// for camera angle view, more 3d look
	switchCameraView();

	GLfloat lmodel_ambient[] = {0.0, 0.0, 0.0, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_DEPTH_TEST);

	GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat light_position1[] = {6.0f, 6.0f, 6.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	// GLfloat shininess = 40.0f;
	// glMateriali(GL_FRONT, GL_SHININESS, shininess);
}

void mySpecialKeyBoard(int key, int x, int y)
{
	if (bLockCamera)
		return;

	switch (key)
	{
	case GLUT_KEY_UP:
		cameraHeight += 0.5;
		break;
	case GLUT_KEY_DOWN:
		cameraHeight -= 0.5;
		break;
	case GLUT_KEY_RIGHT:
		cameraAngle += 5;
		break;
	case GLUT_KEY_LEFT:
		cameraAngle -= 5;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void myKeyBoard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1': // Rotate counter clockwise
	{
		rotationDisk.rotateZ += rotationSpeed;
		if (rotationDisk.rotateZ >= 360)
			rotationDisk.rotateZ = 0;
		break;
	}
	case '2': // Rotate clockwise
	{
		rotationDisk.rotateZ -= rotationSpeed;
		if (rotationDisk.rotateZ <= 0)
			rotationDisk.rotateZ = 360;
		break;
	}
	case '3': // Move cotter 2 out, far from center
	{
		if (cottar2.distance + 0.05 <= tayQuayLength)
		{
			cottar2.distance += 0.05;
		}
		break;
	}
	case '4': // Move cotter 2 in, nearer from center
	{
		if (cottar2.distance >= 0.05)
		{
			cottar2.distance -= 0.05;
		}
		break;
	}
	case 'W': // Toggle draw wire frame mode
	case 'w':
	{
		bDrawWireFrame = !bDrawWireFrame;
		break;
	}
	case 'V': // Toggle View to front and back
	case 'v':
	{
		bLockCamera = !bLockCamera;
		switchCameraView();
		break;
	}
	case 'D': // Toggle second light on or off
	case 'd':
	{
		bLight1on = !bLight1on;
		switchSecondLight();
		break;
	}
	case '+': // Increase camera distance
		if (bLockCamera)
			break;
		cameraDistance += 0.25;
		break;
	case '-': // Decrease camera distance
		if (bLockCamera)
			break;
		cameraDistance -= 0.25;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

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
	cout << "up arrow  : Tang chieu fHeight camera" << endl;
	cout << "down arrow: Giam chieu fHeight camera" << endl;
	cout << "<-        : Quay camera theo chieu kim dong ho" << endl;
	cout << "->        : Quay camera nguoc chieu kim dong ho" << endl;
}

int main(int argc, char **argv)
{
	printUsage();

	glutInit(&argc, (char **)argv);							  //initialize the tool kit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //set the display mode
	glutInitWindowSize(screenWidth, screenHeight);			  //set window size
	glutInitWindowPosition(800, 100);						  // set window position on screen
	glutCreateWindow("Nguyen Tri Nhan - 1810390");

	createObject();
	myInit();
	glutSpecialFunc(mySpecialKeyBoard);
	glutKeyboardFunc(myKeyBoard);
	glutDisplayFunc(displayMe);

	glutMainLoop();
	return 0;
}
