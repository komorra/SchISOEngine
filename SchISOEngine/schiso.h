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
struct transform;


struct vector3
{
	float x;
	float y;
	float z;
};

struct scene
{
	std::vector<sceneObject*> sceneObjects;
};


struct sceneObject
{	
	std::vector<geometry*> geometriesUnion;
};

struct transform
{
	float m[16];
};

struct geometry
{
	int objectType;
	float x1, y1, z1;
	float x2, y2, z2;
	transform trans;
	int operationType;
	geometry *operand;
};


void project(const vector3 &src, vector3 &result)
{
	const float xd = 0.86;
	const float yd = 0.5;
	const float zd = 0.5;
	result.x = src.x * xd + src.y * yd;
	result.y = src.x * xd + src.z * zd;
}



