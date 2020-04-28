#ifndef _tri_
#define _tri_

#include <vector>

using namespace std;

class Vertex
{
public:
	int index;
	double x;
	double y;
	double z;
};

class TriangularFace
{
public:
	int vIndex1;
	int vIndex2;
	int vIndex3;
	Vertex vn1;  /*��ķ�����*/
	Vertex vn2;
	Vertex vn3;
	double fn1;  /*��ķ�����*/
	double fn2;
	double fn3;
};

class Edgeline
{
public:
	int vindex1;
	int vindex2;
	TriangularFace edgeface;
};

class TriangularMesh
{
public:
	vector<TriangularFace> triangles;
	vector<Edgeline> edge;
	vector<vector<int>> borderVerts;
};

#endif