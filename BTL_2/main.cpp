// ===========================================================================
// ||					Bai tap lon Do Hoa May Tinh							||
// ||					Ten: 	Nguyen Tri Nhan								||
// ||					MSSV:	1810390										||
// ===========================================================================

#include <math.h>
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>

using namespace std;
#define M_PI 3.141592653589793238462643383279502884

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

#define DRAW_WIREFRAME 0
#define DRAW_COLOR_2D 1
#define DRAW_COLOR_3D 2

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
float cameraHeight_init = 5;
float cameraAngle_init = -35;
float cameraDistance = cameraDistance_init;
float cameraHeight = cameraHeight_init;
float cameraAngle = cameraAngle_init;

float gachOpacity = 0.6;

int drawMode = DRAW_COLOR_3D;
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

		/* ================================================== */
		/* =									 SETUP VERTS									= */
		/* ================================================== */
		pt[0].set(-fLength, fWidth, fHeight);
		pt[1].set(fLength, fWidth, fHeight);
		pt[2].set(fLength, fWidth, -fHeight);
		pt[3].set(-fLength, fWidth, -fHeight);
		pt[4].set(-fLength, -fWidth, fHeight);
		pt[5].set(fLength, -fWidth, fHeight);
		pt[6].set(fLength, -fWidth, -fHeight);
		pt[7].set(-fLength, -fWidth, -fHeight);

		/* ================================================== */
		/* =									 DRAW FACES										= */
		/* ================================================== */
		numFaces = 6;
		int i;
		face = new Face[numFaces];

		int sideColor = 0;
		int botTopColor = 1;

		//Left face
		face[0].nVerts = 4;
		face[0].vert = new VertexID[face[0].nVerts];
		face[0].vert[0].vertIndex = 1;
		face[0].vert[1].vertIndex = 5;
		face[0].vert[2].vertIndex = 6;
		face[0].vert[3].vertIndex = 2;

		for (i = 0; i < face[0].nVerts; i++)
			face[0].vert[i].colorIndex = sideColor;

		//Right face
		face[1].nVerts = 4;
		face[1].vert = new VertexID[face[1].nVerts];
		face[1].vert[0].vertIndex = 0;
		face[1].vert[1].vertIndex = 3;
		face[1].vert[2].vertIndex = 7;
		face[1].vert[3].vertIndex = 4;
		for (i = 0; i < face[1].nVerts; i++)
			face[1].vert[i].colorIndex = sideColor;

		//top face
		face[2].nVerts = 4;
		face[2].vert = new VertexID[face[2].nVerts];
		face[2].vert[0].vertIndex = 0;
		face[2].vert[1].vertIndex = 1;
		face[2].vert[2].vertIndex = 2;
		face[2].vert[3].vertIndex = 3;
		for (i = 0; i < face[2].nVerts; i++)
			face[2].vert[i].colorIndex = botTopColor;

		//bottom face
		face[3].nVerts = 4;
		face[3].vert = new VertexID[face[3].nVerts];
		face[3].vert[0].vertIndex = 7;
		face[3].vert[1].vertIndex = 6;
		face[3].vert[2].vertIndex = 5;
		face[3].vert[3].vertIndex = 4;
		for (i = 0; i < face[3].nVerts; i++)
			face[3].vert[i].colorIndex = botTopColor;

		//near face
		face[4].nVerts = 4;
		face[4].vert = new VertexID[face[4].nVerts];
		face[4].vert[0].vertIndex = 4;
		face[4].vert[1].vertIndex = 5;
		face[4].vert[2].vertIndex = 1;
		face[4].vert[3].vertIndex = 0;
		for (i = 0; i < face[4].nVerts; i++)
			face[4].vert[i].colorIndex = sideColor;

		//Far face
		face[5].nVerts = 4;
		face[5].vert = new VertexID[face[5].nVerts];
		face[5].vert[0].vertIndex = 3;
		face[5].vert[1].vertIndex = 2;
		face[5].vert[2].vertIndex = 6;
		face[5].vert[3].vertIndex = 7;
		for (i = 0; i < face[5].nVerts; i++)
			face[5].vert[i].colorIndex = sideColor;
	}

	void CreateCylinder(int nSegment, float fHeight, float fRadius)
	{
		numVerts = nSegment * 2 + 2;
		pt = new Point3[numVerts];

		int i;
		int idx;
		float fAngle = 2 * M_PI / nSegment;
		float x, y, z;

		/* ================================================== */
		/* =									 SETUP VERTS									= */
		/* ================================================== */
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

		/* ================================================== */
		/* =									 DRAW FACES										= */
		/* ================================================== */
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

		/* ================================================== */
		/* =									 SETUP VERTS									= */
		/* ================================================== */
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

		/* ================================================== */
		/* =									 DRAW FACES										= */
		/* ================================================== */
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

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = 0;
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

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = 1;
			idx++;
		}

		for (i = nSegment + 2; i < numVerts - 2; i++)
		{
			face[idx].nVerts = 3;
			face[idx].vert = new VertexID[face[idx].nVerts];
			face[idx].vert[0].vertIndex = numVerts - 1;
			face[idx].vert[1].vertIndex = i;
			face[idx].vert[2].vertIndex = i + 1;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = 2;
			idx++;
		}

		// Draw last face
		face[idx].nVerts = 4;
		face[idx].vert = new VertexID[face[idx].nVerts];
		face[idx].vert[0].vertIndex = 1;
		face[idx].vert[1].vertIndex = nSegment + 1;
		face[idx].vert[2].vertIndex = numVerts - 2;
		face[idx].vert[3].vertIndex = nSegment + 2;
		for (int k = 0; k < face[4].nVerts; k++)
			face[4].vert[k].colorIndex = 3;
	}

	void CreateCylinderAQuater(int nSegment, float fHeight, float fRadius)
	{
		numVerts = (nSegment + 1) * 2 + 2;
		pt = new Point3[numVerts];

		int i;
		int idx;
		float fAngle = (M_PI / 2) / nSegment;
		float x, y, z;

		/* ================================================== */
		/* =									 SETUP VERTS									= */
		/* ================================================== */
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

		/* ================================================== */
		/* =									 DRAW FACES										= */
		/* ================================================== */
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

	void CreateBloodyObj(float fRadius, int nSegment)
	{
		float fLength_RecSmall = 1.3 * 2 * fRadius;
		float fLength_RecBig = 2 * 2 * fRadius;
		float fHeight = 0.6 * fRadius;
		float fWidth = 2 * fRadius;
		int i = 0;
		numVerts = 8 + 1 + (nSegment + 1) * 4 + 1 + 6;

		pt = new Point3[numVerts];
		pt[i++].set(0, fHeight, 0);
		pt[i++].set(-fWidth, fHeight, 0);
		pt[i++].set(-fWidth, fHeight, fLength_RecBig);
		pt[i++].set(0, fHeight, fLength_RecBig);
		pt[i++].set(0, -fHeight, 0);
		pt[i++].set(-fWidth, -fHeight, 0);
		pt[i++].set(-fWidth, -fHeight, fLength_RecBig);
		pt[i++].set(0, -fHeight, fLength_RecBig);

		/* ========== Cylinder Half - setup Verts ========== */
		pt[i++].set(-fRadius, fHeight, fLength_RecBig); // Top center
		float fAngle = M_PI / nSegment;
		for (int j = 0; j <= nSegment; j++) // Top verts
		{
			float x = fRadius * cos(fAngle * j) - fRadius;
			float y = fHeight;
			float z = fRadius * sin(fAngle * j) + fLength_RecBig;

			pt[i++].set(x, y, z);
		}
		for (int j = 0; j <= nSegment; j++) // Bottom verts
		{
			float x = fRadius * cos(fAngle * j) - fRadius;
			float y = -fHeight;
			float z = fRadius * sin(fAngle * j) + fLength_RecBig;

			pt[i++].set(x, y, z);
		}
		pt[i++].set(-fRadius, -fHeight, fLength_RecBig); // Bottom center

		/* ========== Cylinder A Quater - setup Verts ========== */
		fAngle = M_PI / (2 * nSegment);
		for (int j = 0; j <= nSegment; j++) // Top verts
		{
			float x = -fWidth * sin(fAngle * j);
			float y = fHeight;
			float z = -fWidth * cos(fAngle * j);

			pt[i++].set(x, y, z);
		}
		for (int j = 0; j <= nSegment; j++) // Bottom verts
		{
			float x = -fWidth * sin(fAngle * j);
			float y = -fHeight;
			float z = -fWidth * cos(fAngle * j);

			pt[i++].set(x, y, z);
		}

		/* ========== Rec Small - setup Verts ========== */
		pt[i++].set(0, fHeight, -fWidth);
		pt[i++].set(fLength_RecSmall, fHeight, -fWidth);
		pt[i++].set(fLength_RecSmall, fHeight, 0);
		pt[i++].set(0, -fHeight, -fWidth);
		pt[i++].set(fLength_RecSmall, -fHeight, -fWidth);
		pt[i++].set(fLength_RecSmall, -fHeight, 0);

		/* ================================================== */
		/* =									 DRAW FACES										= */
		/* ================================================== */
		numFaces = 4 + 6 * nSegment + 5;
		face = new Face[numFaces];
		int starterId;
		int colorTop = COLOR_BLUE;
		int colorSide = COLOR_GREEN;
		int colorBottom = COLOR_RED;
		i = 0;

		/* ========== Rec Big - top face - i = 0 ========== */
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[0].nVerts];
		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = 1;
		face[i].vert[2].vertIndex = 2;
		face[i].vert[3].vertIndex = 3;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorTop;
		i++;

		/* ========== Rec Big - side faces ========== */
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[0].nVerts];
		face[i].vert[0].vertIndex = 1;
		face[i].vert[1].vertIndex = 2;
		face[i].vert[2].vertIndex = 6;
		face[i].vert[3].vertIndex = 5;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorSide;
		i++;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[0].nVerts];
		face[i].vert[0].vertIndex = 3;
		face[i].vert[1].vertIndex = 0;
		face[i].vert[2].vertIndex = 4;
		face[i].vert[3].vertIndex = 7;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorSide;
		i++;

		/* ========== Rec Big - bottom face ========== */
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[0].nVerts];
		face[i].vert[0].vertIndex = 6;
		face[i].vert[1].vertIndex = 5;
		face[i].vert[2].vertIndex = 4;
		face[i].vert[3].vertIndex = 7;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorBottom;
		i++;

		/* ========== Cylinder Half - top face ========== */
		for (int j = 0; j < nSegment; j++)
		{
			face[i].nVerts = 3;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 8;
			face[i].vert[1].vertIndex = 8 + j + 1;
			face[i].vert[2].vertIndex = 8 + j + 2;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = colorTop;
			i++;
		}

		/* ========== Cylinder Half - side face ========== */
		for (int j = 0; j < nSegment; j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 8 + j + 1;
			face[i].vert[1].vertIndex = 8 + j + 2;
			face[i].vert[2].vertIndex = 8 + j + 2 + nSegment + 1;
			face[i].vert[3].vertIndex = 8 + j + 1 + nSegment + 1;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = colorSide;
			i++;
		}

		/* ========== Cylinder Half - bottom face ========== */
		for (int j = 0; j < nSegment; j++)
		{
			face[i].nVerts = 3;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 2 * nSegment + 11;
			face[i].vert[1].vertIndex = 10 + nSegment + j;
			face[i].vert[2].vertIndex = 10 + nSegment + 1 + j;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = colorBottom;
			i++;
		}

		/* ========== Cylinder A Quater - top face ========== */
		for (int j = 0; j < nSegment; j++)
		{
			face[i].nVerts = 3;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 0;
			face[i].vert[1].vertIndex = 12 + 2 * nSegment + j;
			face[i].vert[2].vertIndex = 12 + 2 * nSegment + j + 1;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = colorTop;
			i++;
		}

		/* ========== Cylinder A Quater - side face ========== */
		for (int j = 0; j < nSegment; j++)
		{
			face[i].nVerts = 4;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 12 + 2 * nSegment + j;
			face[i].vert[1].vertIndex = 12 + 2 * nSegment + j + 1;
			face[i].vert[2].vertIndex = 13 + 3 * nSegment + 1 + j;
			face[i].vert[3].vertIndex = 13 + 3 * nSegment + j;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = colorSide;
			i++;
		}

		/* ========== Cylinder A Quater - bottom face ========== */
		for (int j = 0; j < nSegment; j++)
		{
			face[i].nVerts = 3;
			face[i].vert = new VertexID[face[i].nVerts];
			face[i].vert[0].vertIndex = 4;
			face[i].vert[1].vertIndex = 13 + 3 * nSegment + j;
			face[i].vert[2].vertIndex = 13 + 3 * nSegment + j + 1;

			for (int k = 0; k < face[i].nVerts; k++)
				face[i].vert[k].colorIndex = colorBottom;
			i++;
		}

		/* ========== Rectangular Small - top face ========== */
		starterId = 4 * nSegment + 14;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = starterId;
		face[i].vert[2].vertIndex = starterId + 1;
		face[i].vert[3].vertIndex = starterId + 2;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorTop;
		i++;

		/* ========== Rectangular Small - side faces ========== */
		starterId = 4 * nSegment;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 0;
		face[i].vert[1].vertIndex = 16 + starterId;
		face[i].vert[2].vertIndex = 19 + starterId;
		face[i].vert[3].vertIndex = 4;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorSide;
		i++;

		starterId = 4 * nSegment + 14;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = starterId;
		face[i].vert[1].vertIndex = starterId + 1;
		face[i].vert[2].vertIndex = starterId + 4;
		face[i].vert[3].vertIndex = starterId + 3;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorSide;
		i++;

		starterId = 15 + 4 * nSegment;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = starterId;
		face[i].vert[1].vertIndex = starterId + 1;
		face[i].vert[2].vertIndex = starterId + 4;
		face[i].vert[3].vertIndex = starterId + 3;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorSide;
		i++;

		/* ========== Rectangular Small - bottom faces ========== */
		starterId = 17 + 4 * nSegment;
		face[i].nVerts = 4;
		face[i].vert = new VertexID[face[i].nVerts];
		face[i].vert[0].vertIndex = 4;
		face[i].vert[1].vertIndex = starterId;
		face[i].vert[2].vertIndex = starterId + 1;
		face[i].vert[3].vertIndex = starterId + 2;
		for (int k = 0; k < face[i].nVerts; k++)
			face[i].vert[k].colorIndex = colorBottom;
	}

	void CreateGachNen(float fWidth)
	{
		int i;
		numVerts = 4;
		pt = new Point3[numVerts];

		/* ================================================== */
		/* =									 SETUP VERTS									= */
		/* ================================================== */
		pt[0].set(-fWidth, 0, fWidth);
		pt[1].set(fWidth, 0, fWidth);
		pt[2].set(fWidth, 0, -fWidth);
		pt[3].set(-fWidth, 0, -fWidth);

		/* ================================================== */
		/* =									 DRAW FACES										= */
		/* ================================================== */
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

float baseMeasure = 0.5;
float baseScale = 1;
float baseScaleMin = 0.75;
float baseScaleMax = 2.5;
float baseScaleDelta = 0.025;

float baseWidth = baseMeasure;
float baseHeight = baseWidth * 0.6;

float baseRotateY = 0;
float baseRotateX = 0;
float baseRotateZ = 0;
float baseRotateDelta = 5;

Mesh bloodyObj;
float bloodyObjRadius = baseMeasure;
int bloodyObjNSegments = 32;

Mesh rectangular0;
float rectangular0Width = baseWidth;
float rectangular0Length = rectangular0Width * 2;
float rectangular0Height = baseHeight;

Mesh rectangular1;
float rectangular1Width = baseWidth;
float rectangular1Length = rectangular1Width * 1.3;
float rectangular1Height = baseHeight;

Mesh cylinderHalf;
float cylinderHalfSegments = 32;
float cylinderHalfHeight = baseHeight;
float cylinderHalfRadius = baseWidth;

Mesh cylinderAQuater;
float cylinderAQuaterSegments = 32;
float cylinderAQuaterHeight = baseHeight;
float cylinderAQuaterRadius = baseWidth * 2;

float basePositionY = 2;
float basePositionX = -rectangular0Length;
float basePositionZ = -rectangular0Width;
float basePositionYMax = 5;
float basePositionYMin = 1;
float basePositionDelta = 0.1;

void createObject()
{
	bloodyObj.CreateBloodyObj(bloodyObjRadius, bloodyObjNSegments);
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

void drawGach(float x, float y, float z, float width, float alpha)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	float r = 211, g = 211, b = 209;
	glColor4f(r / 255, g / 255, b / 255, alpha);
	glNormal3f(0.0f, 1.0f, 0.0f);
	width = (width / 2) * 0.97;
	glVertex3f(x + width, y, z - width);
	glVertex3f(x + width, y, z + width);
	glVertex3f(x - width, y, z + width);
	glVertex3f(x - width, y, z - width);
	glEnd();
}

void drawNen(float alpha)
{
	float y = 0;
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	int horizontalLimit = 20;
	float width = 1;
	int i = 0;
	for (float x = -horizontalLimit; x < horizontalLimit; x += width)
	{
		for (float z = -horizontalLimit; z < horizontalLimit; z += width)
			drawGach(x, y, z, width, alpha);
		i++;
	}
	glEnable(GL_LIGHTING);
}

void drawPart(Mesh &part)
{
	switch (drawMode)
	{
	case DRAW_WIREFRAME:
		part.DrawWireframe();
		break;
	case DRAW_COLOR_2D:
		glDisable(GL_LIGHTING);
		part.DrawColor();
		glEnable(GL_LIGHTING);
		break;
	case DRAW_COLOR_3D:
	default:
		part.Draw();
	}
}

void drawBloodyObj()
{
	glPushMatrix();
	glTranslated(0, basePositionY, 0);

	// printf("Rotation rec0: %f, %f, %f\n", baseRotateY, baseRotateX, baseRotateZ);
	glRotatef(baseRotateY, 0, 1, 0);
	glRotatef(baseRotateX, 1, 0, 0);
	glRotatef(baseRotateZ, 0, 0, 1);

	bloodyObj.setupMaterial(COLOR_RED);
	// bloodyObj.SetColor(COLOR_RED);
	bloodyObj.CalculateFacesNorm();
	drawPart(bloodyObj);

	glPopMatrix();
}

void drawRectangular0()
{
	glPushMatrix();
	glTranslated(0, basePositionY, 0);

	// printf("Rotation rec0: %f, %f, %f\n", baseRotateY, baseRotateX, baseRotateZ);
	glRotatef(baseRotateY, 0, 1, 0);
	glRotatef(baseRotateX, 0, 0, 1);
	glRotatef(baseRotateZ, 1, 0, 0);

	glTranslated(basePositionX, 0, basePositionZ); // move the center to the end edge of box

	glTranslated(rectangular0.positionX, 0, rectangular0.positionZ);

	rectangular0.setupMaterial(COLOR_RED);
	// rectangular0.SetColor(COLOR_RED);
	rectangular0.CalculateFacesNorm();
	drawPart(rectangular0);

	glPopMatrix();
}

void drawRectangular1()
{
	/*
	Its always rotate around its center
	=> rotate it before changing the coordinate
	 */
	glPushMatrix();
	glTranslated(0, basePositionY, 0);

	glRotated(baseRotateY + 90, 0, 1, 0);
	glRotatef(-baseRotateX, 1, 0, 0);
	glRotatef(baseRotateZ, 0, 0, 1);

	rectangular1.positionX = -rectangular1Length;
	rectangular1.positionZ = rectangular1Width;
	glTranslated(rectangular1.positionX, 0, rectangular1.positionZ);

	rectangular1.setupMaterial(COLOR_RED);
	// rectangular1.SetColor(COLOR_RED);
	rectangular1.CalculateFacesNorm();
	drawPart(rectangular1);

	glPopMatrix();
}

void drawCylinderHalf()
{
	/*
	Its always rotate around its center
	=> rotate it before changing the coordinate
	 */
	glPushMatrix();
	glTranslated(0, basePositionY, 0);

	glRotatef(baseRotateY - 90, 0, 1, 0);
	glRotatef(baseRotateX, 1, 0, 0);
	glRotatef(-baseRotateZ, 0, 0, 1);

	glTranslated(basePositionX, 0, basePositionZ);

	cylinderHalf.positionX = cylinderHalfRadius;
	cylinderHalf.positionZ = cylinderHalfRadius + rectangular0Length * 2;
	glTranslated(cylinderHalf.positionX, 0, cylinderHalf.positionZ);

	cylinderHalf.setupMaterial(COLOR_RED);
	// cylinderHalf.SetColor(COLOR_RED);
	cylinderHalf.CalculateFacesNorm();
	drawPart(cylinderHalf);

	glPopMatrix();
}

void drawCylinderAQuater()
{
	/*
	Its always rotate around its center
	=> rotate it before changing the coordinate
	 */
	glPushMatrix();
	glTranslated(0, basePositionY, 0);

	glRotated(baseRotateY + 90, 0, 1, 0);
	glRotatef(-baseRotateX, 1, 0, 0);
	glRotatef(baseRotateZ, 0, 0, 1);

	glTranslated(basePositionX, 0, basePositionZ);

	cylinderAQuater.positionX = cylinderAQuaterRadius;
	cylinderAQuater.positionZ = cylinderAQuaterRadius / 2;
	glTranslated(cylinderAQuater.positionX, 0, cylinderAQuater.positionZ);

	cylinderAQuater.setupMaterial(COLOR_RED);
	// cylinderAQuater.SetColor(COLOR_RED);
	cylinderAQuater.CalculateFacesNorm();
	drawPart(cylinderAQuater);

	glPopMatrix();
}

void drawAllObject()
{
	drawBloodyObj();
	// drawRectangular0();
	// drawCylinderHalf();
	// drawCylinderAQuater();
	// drawRectangular1();
}

void switchCameraView()
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
	drawAxis();

	// Clear the stencil buffers
	glClearStencil(0);
	// Clear depth
	glClearDepth(1.0f);

	glPushMatrix();
	glScalef(baseScale, baseScale, baseScale); // Scaling object up and down
	drawAllObject();
	glPopMatrix();

	// Draw nen & disable update on depth
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawNen(1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	/* Now, only render where stencil is set to 1. */
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Draw reflection
	glPushMatrix();
	glScalef(baseScale, -baseScale, baseScale);
	drawAllObject();
	glPopMatrix();
	glDisable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawNen(gachOpacity);
	glDisable(GL_BLEND);

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
		if (basePositionY + basePositionDelta > basePositionYMax)
		{
			break;
		}
		basePositionY += basePositionDelta;
		break;
	}
	case 'D': // Move obj down
	case 'd':
	{
		if (basePositionY - basePositionDelta < basePositionYMin)
		{
			break;
		}
		basePositionY -= basePositionDelta;
		break;
	}
	case 'I': // Increase obj size
	case 'i':
	{
		if (baseScale + baseScaleDelta > baseScaleMax)
		{
			break;
		}
		baseScale += baseScaleDelta;
		break;
	}
	case 'O': // Decrease obj size
	case 'o':
	{
		if (baseScale - baseScaleDelta < baseScaleMin)
		{
			break;
		}
		baseScale -= baseScaleDelta;
		break;
	}
	case 'X': // Rotate around the x axis
	case 'x':
	{
		baseRotateX += baseRotateDelta;
		if (baseRotateX == 360)
		{
			baseRotateX = 0;
		}
		break;
	}
	case 'Y': // Rotate around the y axis
	case 'y':
	{
		baseRotateY += baseRotateDelta;
		if (baseRotateY == 360)
		{
			baseRotateY = 0;
		}
		break;
	}
	case 'Z': // Rotate around the z axis
	case 'z':
	{
		baseRotateZ += baseRotateDelta;
		if (baseRotateZ == 360)
		{
			baseRotateZ = 0;
		}
		break;
	}
	case '1': // Draw wireframe
	{
		drawMode = DRAW_WIREFRAME;
		break;
	}
	case '2': // Draw 2d color
	{
		drawMode = DRAW_COLOR_2D;
		break;
	}
	case '3': // Draw 3d color
	{
		drawMode = DRAW_COLOR_3D;
		break;
	}
	case '+': // Increase camera distance
		cameraDistance += 0.25;
		break;
	case '-': // Decrease camera distance
		cameraDistance -= 0.25;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void printUsage()
{
	cout << "1	: Che do hien thi khung day" << endl;
	cout << "2	: Che do hien thi to mau 2D" << endl;
	cout << "3	: Che do hien thi to mau 3D" << endl;
	cout << "U, u	: Di chuyen len tren" << endl;
	cout << "D, d	: Di chuyen xuong duoi" << endl;
	cout << "I, i	: Phong to" << endl;
	cout << "O, o	: Thu nho" << endl;
	cout << "X, x	: Xoay quanh truc X" << endl;
	cout << "Y, y	: Xoay quanh truc Y" << endl;
	cout << "Z, z	: Xoay quanh truc Z" << endl;
	cout << "+	: Tang khoang cach camera" << endl;
	cout << "-	: Giam khoang cach camera" << endl;
	cout << "up arrow	: Tang chieu cao camera" << endl;
	cout << "down arrow	: Giam chieu cao camera" << endl;
	cout << "<-	: Quay camera theo chieu kim dong ho" << endl;
	cout << "->	: Quay camera nguoc chieu kim dong ho" << endl;
}

int main(int argc, char **argv)
{
	printUsage();

	glutInit(&argc, (char **)argv);														//initialize the tool kit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //set the display mode
	glutInitWindowSize(screenWidth, screenHeight);						//set window size
	glutInitWindowPosition(800, 100);													// set window position on screen
	glutCreateWindow("Nguyen Tri Nhan - 1810390");

	glutSpecialFunc(mySpecialKeyBoard);
	glutKeyboardFunc(myKeyBoard);
	createObject();
	myInit();
	glutDisplayFunc(displayMe);

	glutMainLoop();
	return 0;
}
