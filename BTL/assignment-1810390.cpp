#include <iostream>
#include <math.h>
#include <GL/glut.h>

using namespace std;

#define PI 3.1415926
#define COLORNUM 14
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define COLOR_YELLOW 3
#define COLOR_PINK 4
#define COLOR_DARK_GRAY 6
#define COLOR_BLACK 12

float ColorArr[COLORNUM][3] = {
	{1.0, 0.0, 0.0},  // red 0
	{0.0, 1.0, 0.0},  // green 1
	{0.0, 0.0, 1.0},  // blue 2
	{1.0, 1.0, 0.0},  // yellow 3
	{1.0, 0.0, 1.0},  // pink 4
	{0.0, 1.0, 1.0},  // aqua 5
	{0.3, 0.3, 0.3},  // dark gray 6
	{0.5, 0.5, 0.5},  // gray 7
	{0.9, 0.9, 0.9},  // silver 8
	{1.0, 0.5, 0.5},  // kinda pink pastel 9
	{0.5, 1.0, 0.5},  // mint green 10
	{0.5, 0.5, 1.0},  // not purple, more blue 11
	{0.0, 0.0, 0.0},  // black 12
	{1.0, 1.0, 1.0}}; // white 13

int windowWidth = 720;
int windowHeight = 720;

#pragma region SupportClass
// Define a point in 3D space
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

// Define a vector in 3D space
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
#pragma endregion

#pragma region Mesh
class Mesh
{
public:
	int numVerts;
	Point3 *pt;

	int numFaces;
	Face *face;

	float rotateX, rotateY, rotateZ;

public:
	Mesh()
	{
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
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

	void SetMaterialColor(const int &type)
	{
		for (int f = 0; f < numFaces; f++)
		{
			for (int v = 0; v < face[f].nVerts; v++)
			{
				face[f].vert[v].colorIndex = type;
			}
		}
	}

	// Draw wireframe of the mesh
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
	// Add color to the mesh
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
				// ic = f % COLORNUM;
				glColor3f(ColorArr[ic][0], ColorArr[ic][1], ColorArr[ic][2]);
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
		float fAngle = 2 * PI / nSegment;
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
	void CreateRectangle(float fLength, float fWidth, float fHeight)
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
	void CreateBearing(float fHeight, float fWidth, int nUpperSegment, float outerRadius, float innerRadius)
	{

		// nUpperSegment: so diem tren mot nua hinh tron
		int M = (nUpperSegment - 1) * 2; // M: So diem tren mot hinh tron
		GLfloat angle = 2 * PI / M;

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
	void CreateCrank(int nSegment, float fLength, float fRadius, float fHeight)
	{
		GLfloat anpha = PI / nSegment;
		int i = 0;
		numVerts = 4 * nSegment + 8;
		pt = new Point3[numVerts];

		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(-fLength / 2 - fRadius * sin(j * anpha), fHeight * 2, fRadius * cos(j * anpha));
		}
		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(-fLength / 2 - fRadius * sin(j * anpha), 0, fRadius * cos(j * anpha));
		}
		/*=============================================================================*/

		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(fLength / 2 + fRadius * sin(j * anpha), fHeight * 2, fRadius * cos(j * anpha));
		}
		for (int j = 0; j < nSegment; j++)
		{
			pt[i++].set(fLength / 2 + fRadius * sin(j * anpha), 0, fRadius * cos(j * anpha));
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
};
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
#pragma endregion

#pragma region camera
float cameraDistance = 5;
float cameraHeight = 2.5;
float cameraAngle = -30;
float cameraX, cameraY, cameraZ;
float lookAtX, lookAtY, lookAtZ;

void arrowKeyController(int key, int x, int y)
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

void otherKeyController(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+':
		cameraDistance += 0.5;
		break;
	case '-':
		cameraDistance -= 0.5;
		break;
	}
	glutPostRedisplay();
}

#pragma endregion

#pragma region MeshObjects

float baseMeasure = 3.0f;

Mesh baseFoot;
float baseFootLength = baseMeasure;
float baseFootWidth = baseFootLength * 0.37;
float baseFootHeight = baseFootLength * 0.05;

Mesh baseBody;
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

Mesh rotationDisk; // Bàn quay
float rotationDisknSegment = 48;
float rotationDiskHeight = baseBodyWidth;
float rotationDiskRadius = baseBodyLength * 1.5;

Mesh tayQuay;
float tayQuaynSegment = 20;
float tayQuayLength = rotationDiskRadius * 0.88;
float tayQuayHeight = rotationDiskHeight * 0.75;
float tayQuayRadius = rotationDiskHeight * 0.8;

// Chốt 3, 2, 1
Mesh cottar3;
float cottar3nSegment = 16;
float cottar3Height = baseBodyWidth * 3.75;
float cottar3Radius = baseBodyWidth;

Mesh cottar2OuterCube;
float cottar2OuterCubeSize = tayQuayHeight;

Mesh cottar2;
float cottar2nSegment = 6;
float cottar2Height = cottar2OuterCubeSize;
float cottar2Radius = cottar2OuterCubeSize * 0.7;

Mesh cottar1;

Mesh thanhLienKet;
Mesh thanhChuU;
Mesh thanhTruot;

void drawBaseFoot()
{
	glPushMatrix();
	glTranslated(0, baseFootHeight, 0);
	baseFoot.DrawWireframe();
	baseFoot.SetMaterialColor(COLOR_RED);
	baseFoot.DrawColor();
	glPopMatrix();
}

void drawBaseBody()
{
	glPushMatrix();
	glTranslated(0, 2 * baseFootHeight + baseBodyHeight, 0);
	baseBody.DrawWireframe();
	baseBody.SetMaterialColor(COLOR_GREEN);
	baseBody.DrawColor();
	glPopMatrix();
}

void drawBaseHead()
{
	glPushMatrix();
	float positionY = 2 * baseFootHeight + 2 * baseBodyHeight + baseHeadHeight;
	glTranslated(0, positionY, 0);
	baseHead.DrawWireframe();
	baseHead.SetMaterialColor(COLOR_BLUE);
	baseHead.DrawColor();
	glPopMatrix();
}

void drawBearings()
{
	glPushMatrix();
	float positionX = baseHeadLength - bearingBoxWidth * 2;
	float positionY = 2 * baseFootHeight + 2 * baseBodyHeight + baseHeadHeight;
	float positionZ = baseHeadWidth + bearingBoxHeight;
	glTranslated(positionX, positionY, positionZ);
	bearingRightBox.SetMaterialColor(COLOR_RED);
	glRotated(90, 1, 0, 0);
	glRotated(90, 0, 1, 0);
	bearingRightBox.DrawWireframe();
	bearingRightBox.DrawColor();

	glTranslated(0, bearingBoxHeight, 0);
	bearingRight.SetMaterialColor(COLOR_RED);
	bearingRight.DrawColor();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-positionX, positionY, positionZ);
	bearingLeftBox.SetMaterialColor(COLOR_RED);
	glRotated(90, 1, 0, 0);
	glRotated(90, 0, 1, 0);
	bearingLeftBox.DrawWireframe();
	bearingLeftBox.DrawColor();

	glTranslated(0, bearingBoxHeight, 0);
	bearingLeft.SetMaterialColor(COLOR_RED);
	bearingLeft.DrawColor();
	glPopMatrix();
}

void drawCottar3()
{
	glPushMatrix();
	float positionY = 2 * baseFootHeight + baseBodyHeight * 0.25;
	float positionZ = baseBodyWidth + cottar3Height;
	glTranslated(0, positionY, positionZ);
	glRotated(90, 1, 0, 0);
	cottar3.SetMaterialColor(COLOR_DARK_GRAY);
	cottar3.DrawWireframe();
	cottar3.DrawColor();
	glPopMatrix();
}
void drawCottar2()
{
	glPushMatrix();
	float positionX = 0;
	float positionY = 2 * baseFootHeight + baseBodyHeight + tayQuayLength / 2;
	float positionZ = baseBodyWidth + rotationDiskHeight + tayQuayHeight * 2 + cottar2Height * 2;
	glTranslated(positionX, positionY, positionZ);
	glRotated(tayQuay.rotateZ + 90, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	cottar2.SetMaterialColor(COLOR_DARK_GRAY);
	// cottar2.DrawWireframe();
	cottar2.DrawColor();
	cottar2OuterCube.SetMaterialColor(COLOR_RED);
	cottar2OuterCube.DrawWireframe();
	cottar2OuterCube.DrawColor();
	glPopMatrix();
}
void drawCottar1() {}
void drawRotationDisk()
{
	glPushMatrix();
	float positionY = 2 * baseFootHeight + baseBodyHeight;
	float positionZ = baseBodyWidth + rotationDiskHeight;
	glTranslated(0, positionY, positionZ);
	glRotated(90, 1, 0, 0);
	rotationDisk.SetMaterialColor(COLOR_YELLOW);
	rotationDisk.DrawWireframe();
	rotationDisk.DrawColor();
	glPopMatrix();
}
void drawTayQuay()
{
	glPushMatrix();
	float positionY = 2 * baseFootHeight + baseBodyHeight + tayQuayLength / 2;
	float positionZ = baseBodyWidth + rotationDiskHeight + tayQuayHeight;
	glTranslated(0, positionY, positionZ);
	glRotated(tayQuay.rotateZ + 90, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	tayQuay.SetMaterialColor(COLOR_PINK);
	tayQuay.DrawWireframe();
	tayQuay.DrawColor();
	glPopMatrix();
}
void drawThanhLienKet() {}
void drawThanhChuU() {}
void drawThanhTruot() {}
void drawAxis()
{
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(4, 0, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 4, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 4);
	glEnd();
}
#pragma endregion

#pragma region MainSection
void drawMyObject()
{
	drawBaseFoot();
	drawBaseBody();
	drawBaseHead();
	drawBearings();
	drawCottar3();
	drawCottar2();
	drawCottar1();
	drawRotationDisk();
	drawTayQuay();
	drawThanhLienKet();
	drawThanhChuU();
	drawThanhTruot();
}

void displayMe(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cameraX = cameraDistance * sin(cameraAngle * PI / 180);
	cameraY = cameraHeight;
	cameraZ = cameraDistance * cos(cameraAngle * PI / 180);

	if (cameraDistance == 0)
	{
		gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, sinf(cameraAngle * PI / 180), 0, cosf(cameraAngle * PI / 180));
	}
	else
	{
		gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0, 1, 0);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);

	drawAxis();
	drawMyObject();

	glFlush();
	glutSwapBuffers();
}

void myInit()
{
	cameraAngle = -30;	  // Góc quay camera xung quanh trục Oy
	cameraHeight = 5.5;	  // Chiều cao camera so với mặt phẳng xOz
	cameraDistance = 6.5; // Khoảng cách đến trục Oy
	lookAtX = 0;
	lookAtY = 1;
	lookAtZ = 0;

	float fHalfSize = 4;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-fHalfSize, fHalfSize, -fHalfSize, fHalfSize, -1000, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void createObj()
{
	baseFoot.CreateRectangle(baseFootLength, baseFootHeight, baseFootWidth);
	baseBody.CreateRectangle(baseBodyLength, baseBodyHeight, baseBodyWidth);
	baseHead.CreateRectangle(baseHeadLength, baseHeadHeight, baseHeadWidth);

	bearingRightBox.CreateRectangle(bearingBoxLength, bearingBoxHeight, bearingBoxWidth);
	bearingRight.CreateBearing(bearingHeight, bearingWidth, bearingNUpperSegment, bearingOuterRadius, bearingInnerRadius);
	bearingLeftBox.CreateRectangle(bearingBoxLength, bearingBoxHeight, bearingBoxWidth);
	bearingLeft.CreateBearing(bearingHeight, bearingWidth, bearingNUpperSegment, bearingOuterRadius, bearingInnerRadius);

	cottar3.CreateCylinder(cottar3nSegment, cottar3Height, cottar3Radius);
	cottar2OuterCube.CreateCube(cottar2OuterCubeSize);
	cottar2.CreateCylinder(cottar2nSegment, cottar2Height, cottar2Radius);
	// cottar1;
	rotationDisk.CreateCylinder(rotationDisknSegment, rotationDiskHeight, rotationDiskRadius);
	tayQuay.CreateCrank(tayQuaynSegment, tayQuayLength, tayQuayRadius, tayQuayHeight);
	// thanhLienKet;
	// thanhChuU;
	// thanhTruot;
}

int main(int argc, char **argv)
{
	printUsage();

	glutInit(&argc, (char **)argv);							  //initialize the tool kit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //set the display mode
	glutInitWindowSize(windowWidth, windowHeight);			  //set window size
	glutInitWindowPosition(100, 100);						  // set window position on screen
	glutCreateWindow("Nguyen Tri Nhan - 1810390");

	createObj();
	// tmp.CreateBearing(bearingLength, bearingWidth, )

	myInit();
	glutKeyboardFunc(otherKeyController);
	glutSpecialFunc(arrowKeyController);
	glutDisplayFunc(displayMe);

	glutMainLoop();
	return 0;
}
#pragma endregion
