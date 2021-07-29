#include <math.h>
#include <iostream>
// #include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>

using namespace std;
// #define M_PI 3.141592653589793238462643383279502884

#define COLORNUM 15
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define COLOR_YELLOW 3
#define COLOR_PINK 4
#define COLOR_AQUA 5
#define COLOR_DARK_GRAY 6
#define COLOR_GRAY 7
#define COLOR_PURPLE 12
#define COLOR_BLACK 13

float ColorArr[COLORNUM][3] = {
		{1.0, 0.0, 0.0},			 // red 0
		{0.0, 1.0, 0.0},			 // green 1
		{0.0, 0.0, 1.0},			 // blue 2
		{1.0, 1.0, 0.0},			 // yellow 3
		{1.0, 0.75, 0.8},			 // pink 4
		{0.0, 1.0, 1.0},			 // aqua 5
		{0.3, 0.3, 0.3},			 // dark gray 6
		{0.5, 0.5, 0.5},			 // gray 7
		{0.9, 0.9, 0.9},			 // silver 8
		{1.0, 0.5, 0.5},			 // kinda pink pastel 9
		{0.5, 1.0, 0.5},			 // mint green 10
		{0.5, 0.5, 1.0},			 // not purple, more blue 11
		{0.792, 0.821, 0.984}, // purple 12
		{0.0, 0.0, 0.0},			 // black 13
		{1.0, 1.0, 1.0}};			 // white 14

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

	void CreateRectangular(float fLength, float fWidth, float fHeight)
	{
		numVerts = 8;
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
			pt[i + 1 + nSegment].set(x, -y, z);
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

	void CreateCylinderHalf(int nSegment, float fHeight, float fRadius)
	{
		numVerts = (nSegment + 1) * 2 + 2;
		pt = new Point3[numVerts];

		int i;
		int idx;
		float fAngle = M_PI / nSegment;
		float x, y, z;

		pt[0].set(0, fHeight, 0);
		pt[numVerts - 1].set(0, -fHeight, 0);
		for (i = 0; i <= nSegment; i++)
		{
			x = fRadius * cos(fAngle * i);
			z = fRadius * sin(fAngle * i);
			y = fHeight;
			pt[i + 1].set(x, y, z);
			pt[i + 2 + nSegment].set(x, -y, z);
		}

		numFaces = nSegment * 3 + 1;
		face = new Face[numFaces];

		idx = 0;
		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = 0;
			face[idx].vert[1].vertIndex = i + 1;
			face[idx].vert[2].vertIndex = i + 2;
			idx++;
		}

		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 4;
			face[idx].vert = new VertexID[face[idx].nVerts];

			face[idx].vert[0].vertIndex = i + 1;
			face[idx].vert[1].vertIndex = i + 2;
			face[idx].vert[2].vertIndex = i + 3 + nSegment;
			face[idx].vert[3].vertIndex = i + 2 + nSegment;
			idx++;
		}

		for (i = nSegment + 2; i < numVerts - 2; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = numVerts - 1;
			face[idx].vert[1].vertIndex = i;
			face[idx].vert[2].vertIndex = i + 1;
			idx++;
		}

		// Draw last face
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = 1;
		face[idx].vert[1].vertIndex = nSegment + 1;
		face[idx].vert[2].vertIndex = numVerts - 2;
		face[idx].vert[3].vertIndex = nSegment + 2;
	}

	void CreateCylinderAQuater(int nSegment, float fHeight, float fRadius)
	{
		numVerts = (nSegment + 1) * 2 + 2;
		pt = new Point3[numVerts];

		int i;
		int idx;
		float fAngle = (M_PI / 2) / nSegment;
		float x, y, z;

		pt[0].set(0, fHeight, 0);
		pt[numVerts - 1].set(0, -fHeight, 0);
		for (i = 0; i <= nSegment; i++)
		{
			x = fRadius * cos(fAngle * i);
			z = fRadius * sin(fAngle * i);
			y = fHeight;
			pt[i + 1].set(x, y, z);
			pt[i + 2 + nSegment].set(x, -y, z);
		}

		numFaces = nSegment * 3 + 2;
		face = new Face[numFaces];

		idx = 0;
		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = 0;
			face[idx].vert[1].vertIndex = i + 1;
			face[idx].vert[2].vertIndex = i + 2;
			idx++;
		}

		for (i = 0; i < nSegment; i++)
		{
			face[idx].nVerts = 4;
			face[idx].vert = new VertexID[face[idx].nVerts];

			face[idx].vert[0].vertIndex = i + 1;
			face[idx].vert[1].vertIndex = i + 2;
			face[idx].vert[2].vertIndex = i + 3 + nSegment;
			face[idx].vert[3].vertIndex = i + 2 + nSegment;
			idx++;
		}

		for (i = nSegment + 2; i < numVerts - 2; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = numVerts - 1;
			face[idx].vert[1].vertIndex = i;
			face[idx].vert[2].vertIndex = i + 1;
			idx++;
		}

		// Draw last 2 faces
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = 0;
		face[idx].vert[1].vertIndex = numVerts - 1;
		face[idx].vert[2].vertIndex = nSegment + 2;
		face[idx].vert[3].vertIndex = 1;
		idx++;

		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = 0;
		face[idx].vert[1].vertIndex = numVerts - 1;
		face[idx].vert[2].vertIndex = numVerts - 2;
		face[idx].vert[3].vertIndex = nSegment + 1;
	}

	void CreateGachNen(float fWidth)
	{
		int i;
		numVerts = 4;
		pt = new Point3[numVerts];

		pt[0].set(-fWidth, 0, fWidth);
		pt[1].set(fWidth, 0, fWidth);
		pt[2].set(fWidth, 0, -fWidth);
		pt[3].set(-fWidth, 0, -fWidth);

		numFaces = 1;
		face = new Face[numFaces];
		face[0].nVerts = 4;
		face[0].vert = new VertexID[face[0].nVerts];
		face[0].vert[0].vertIndex = 0;
		face[0].vert[1].vertIndex = 1;
		face[0].vert[2].vertIndex = 2;
		face[0].vert[3].vertIndex = 3;
		for (i = 0; i < face[0].nVerts; i++)
			face[0].vert[i].colorIndex = 0;
	}
};

#pragma endregion

float baseMeasure = 1;
float baseMeasureDelta = 0.025;

float baseWidth = baseMeasure;
float baseHeight = baseWidth * 0.6;

float basePositionX = 0;
float basePositionZ = 0;
float basePositionY = baseHeight;
float basePositionDelta = 0.1;

Mesh rectangular0;
float rectangular0Width = baseWidth;
float rectangular0Length = rectangular0Width * 2;
float rectangular0Height = baseHeight;

Mesh rectangular1;
float rectangular1Width = baseWidth;
float rectangular1Length = rectangular1Width * 1.3;
float rectangular1Height = baseHeight;

Mesh cylinderHalf;
float cylinderHalfSegments = 9;
float cylinderHalfHeight = baseHeight;
float cylinderHalfRadius = baseWidth;

Mesh cylinderAQuater;
float cylinderAQuaterSegments = 10;
float cylinderAQuaterHeight = baseHeight;
float cylinderAQuaterRadius = baseWidth * 2;

void createObject()
{
	rectangular0.CreateRectangular(rectangular0Length, rectangular0Height, rectangular0Width);

	rectangular1.CreateRectangular(rectangular1Length, rectangular1Height, rectangular1Width);

	cylinderHalf.CreateCylinderHalf(cylinderHalfSegments, cylinderHalfHeight, cylinderHalfRadius);

	cylinderAQuater.CreateCylinderAQuater(cylinderAQuaterSegments, cylinderAQuaterHeight, cylinderAQuaterRadius);
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

void drawRectangular0()
{
	glPushMatrix();
	rectangular0.positionY = basePositionY;
	glTranslated(rectangular0.positionX, rectangular0.positionY, rectangular0.positionZ);

	rectangular0.setupMaterial(COLOR_RED);
	rectangular0.SetColor(COLOR_RED);
	rectangular0.CalculateFacesNorm();
	drawPart_ColorOrWireframe(rectangular0);

	glPopMatrix();
}

void drawRectangular1()
{
	glPushMatrix();
	rectangular1.positionY = basePositionY;
	rectangular1.positionX = rectangular0Length + baseWidth;
	rectangular1.positionZ = baseWidth * 2 + (rectangular1Length - baseWidth);
	glTranslated(rectangular1.positionX, rectangular1.positionY, rectangular1.positionZ);
	glRotated(90, 0, 1, 0);

	rectangular1.setupMaterial(COLOR_RED);
	rectangular1.SetColor(COLOR_RED);
	rectangular1.CalculateFacesNorm();
	drawPart_ColorOrWireframe(rectangular1);

	glPopMatrix();
}

void drawCylinderHalf()
{
	glPushMatrix();
	cylinderHalf.positionY = basePositionY;
	cylinderHalf.positionX = -rectangular0Length;
	glTranslated(cylinderHalf.positionX, cylinderHalf.positionY, cylinderHalf.positionZ);
	glRotated(-90, 0, 1, 0);

	cylinderHalf.setupMaterial(COLOR_RED);
	cylinderHalf.SetColor(COLOR_RED);
	cylinderHalf.CalculateFacesNorm();
	drawPart_ColorOrWireframe(cylinderHalf);

	glPopMatrix();
}

void drawCylinderAQuater()
{
	glPushMatrix();
	cylinderAQuater.positionY = basePositionY;
	cylinderAQuater.positionX = rectangular0Length;
	cylinderAQuater.positionZ = baseWidth;
	glTranslated(cylinderAQuater.positionX, cylinderAQuater.positionY, cylinderAQuater.positionZ);
	glRotated(90, 0, 1, 0);

	cylinderAQuater.setupMaterial(COLOR_RED);
	cylinderAQuater.SetColor(COLOR_RED);
	cylinderAQuater.CalculateFacesNorm();
	drawPart_ColorOrWireframe(cylinderAQuater);

	glPopMatrix();
}

void drawAllObject()
{
	drawRectangular0();
	drawRectangular1();
	drawCylinderHalf();
	drawCylinderAQuater();
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
	case 'U': // Move obj up
	case 'u':
	{
		basePositionY += basePositionDelta;
		break;
	}
	case 'D': // Move obj down
	case 'd':
	{
		basePositionY -= basePositionDelta;
		break;
	}
	// TODO: need fixing
	case 'I': // Increase obj size
	case 'i':
	{
		baseMeasure += baseMeasureDelta;
		createObject();
		drawAllObject();
		cout << "BIGGGGGGGGGGGGGGGGGER" << endl;
		break;
	}
	// TODO: need fixing
	case 'O': // Decrease obj size
	case 'o':
	{
		baseMeasure -= baseMeasureDelta;
		createObject();
		drawAllObject();
		cout << "smaller" << endl;
		break;
	}
	// TODO: Add rotation around X
	case 'X': // Rotate around the x axis
	case 'x':
	{
		cout << "rotate X" << endl;
		break;
	}
	// TODO: Add rotation around Y
	case 'Y': // Rotate around the y axis
	case 'y':
	{
		cout << "rotate Y" << endl;
		break;
	}
	// TODO: Add rotation around Z
	case 'Z': // Rotate around the z axis
	case 'z':
	{
		cout << "rotate Z" << endl;
		break;
	}
	case '1': // Toggle draw wire frame mode
	{
		bDrawWireFrame = !bDrawWireFrame;
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
}

int main(int argc, char **argv)
{
	printUsage();

	glutInit(&argc, (char **)argv);														//initialize the tool kit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //set the display mode
	glutInitWindowSize(screenWidth, screenHeight);						//set window size
	glutInitWindowPosition(800, 100);													// set window position on screen
	glutCreateWindow("Nguyen Tri Nhan - 1810390");

	createObject();
	myInit();
	glutSpecialFunc(mySpecialKeyBoard);
	glutKeyboardFunc(myKeyBoard);
	glutDisplayFunc(displayMe);

	glutMainLoop();
	return 0;
}
