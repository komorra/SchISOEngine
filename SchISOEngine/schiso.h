#include <vector>
#include <cmath>

#define SCENE_OBJECT_BOX 0 
#define SCENE_OBJECT_SPHERE 1

#define OPERATION_UNION 0
#define OPERATION_INTERSECTION 1
#define OPERATION_AMINUSB 2 
#define OPERATION_BMINUSA 3

struct vector3;
struct scene;
struct sceneObject;
struct geometry;
struct matrix;


struct vector3
{
	float v[3];
};

struct ray
{
	vector3 org;
	vector3 dir;
};

struct scene
{
	std::vector<sceneObject*> sceneObjects;
};


struct sceneObject
{	
	std::vector<geometry*> geometriesUnion;
};

struct matrix
{
	float m[16];
};

struct geometry
{
	int objectType;
	float x1, y1, z1;
	float x2, y2, z2;
	matrix trans;
	int operationType;
	geometry *operand;
};


void project(const vector3 &src, vector3 &result)
{
	const float xd = 0.86;
	const float yd = 0.5;
	const float zd = 0.5;
	result.v[0] = src.v[0] * xd + src.v[1] * yd;
	result.v[1] = src.v[0] * xd + src.v[2] * zd;
}

void transformCoordinate(const matrix& transform, const vector3& src, vector3& result)
{
	const float m[4] = { src.v[0], src.v[1], src.v[2], 1 };
	float res[4] = { 0, 0, 0, 0 };	

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{			
			res[col] += transform.m[col + row * 4] * m[col];
		}
	}

	result.v[0] = res[0];
	result.v[1] = res[1];
	result.v[2] = res[2];
}

void transformNormal(const matrix& transform, const vector3& src, vector3& result)
{
	const float m[4] = { src.v[0], src.v[1], src.v[2], 0 };
	float res[4] = { 0, 0, 0, 0 };

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			res[col] += transform.m[col + row * 4] * m[col];
		}
	}

	result.v[0] = res[0];
	result.v[1] = res[1];
	result.v[2] = res[2];
}

float length(const vector3& v)
{
	return sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
}

void normalizeVector(vector3& v)
{	
	float len = length(v);
	for (int la = 0; la < 3; la++)
	{
		v.v[la] /= len;
	}
}

void matrixIdentity(matrix& transform)
{
	for (int la = 0; la < 16; la++)
	{
		transform.m[la] = (la % 5 == 0) ? 1 : 0;
	}
}

void matrixZero(matrix& transform)
{
	for (int la = 0; la < 16; la++)
	{
		transform.m[la] = 0;
	}
}

void matrixInverse(matrix& transform)
{

}

void matrixMul(const matrix& left, const matrix& right, matrix& result)
{
	matrixZero(result);

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			for (int inner = 0; inner < 4; inner++)
			{
				result.m[col + row * 4] += left.m[inner + row * 4] * right.m[col + inner * 4];
			}
		}
	}
}



bool rayRectangleIntersection(const ray& ray, int aabbSide, const vector3& min, const vector3& max, vector3& outPos, vector3& outNrm)
{	
	const float p[6] = { min.v[0], min.v[1], min.v[2], max.v[0], max.v[1], max.v[2] };
	
	int i0 = aabbSide % 3;
	int i1 = (aabbSide + 1) % 3;
	int i2 = (aabbSide + 2) % 3;

	float pcur = p[aabbSide] - ray.org.v[i0];

	float s = pcur / ray.dir.v[i0];
	
	float u = ray.dir.v[i1] * s;
	float v = ray.dir.v[i2] * s;

	u += ray.org.v[i1];
	v += ray.org.v[i2];

	//v += 115;
	//u += 

	outNrm.v[i0] = (aabbSide >= 2) ? 1 : -1;
	outNrm.v[i1] = 0;
	outNrm.v[i2] = 0;

	outPos.v[i0] = p[aabbSide];
	outPos.v[i1] = u;
	outPos.v[i2] = v;

	return ((u >= p[i1]) && (u <= p[i1 + 3]) && (v >= p[i2]) && (v <= p[i2 + 3]));
	
}

bool rayAABBIntersection(const ray& ray, const vector3& min, const vector3& max, vector3& outPos, vector3& outNrm)
{
	for (int la = 0; la < 6; la++)
	{		
		if (rayRectangleIntersection(ray, la, min, max, outPos, outNrm))
		{
			return true;
		}
	}
	return false;
}

void sampleScene(scene* scn, const vector3& xy, int& color, vector3& normal, float& depth)
{
	ray r;
	r.org.v[0] = xy.v[0];
	r.org.v[1] = xy.v[1];
	r.org.v[2] = 0;

	r.dir.v[0] = 0;
	r.dir.v[1] = 0;
	r.dir.v[2] = 1;

	normalizeVector(r.dir);

	vector3 min;
	vector3 max;
	vector3 pos;
	vector3 nrm;
	color = 0xff000000;
	for (auto it = scn->sceneObjects.begin(); it != scn->sceneObjects.end(); it++)
	{
		min.v[0] = (*it)->geometriesUnion[0]->x1;
		min.v[1] = (*it)->geometriesUnion[0]->y1;
		min.v[2] = (*it)->geometriesUnion[0]->z1;

		max.v[0] = (*it)->geometriesUnion[0]->x2;
		max.v[1] = (*it)->geometriesUnion[0]->y2;
		max.v[2] = (*it)->geometriesUnion[0]->z2;

		if (rayAABBIntersection(r, min, max, pos, nrm))
		{
			color = 0xffffffff;
			return;
		}
	}
}