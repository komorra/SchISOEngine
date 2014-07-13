#include <vector>

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
