#include <vector>

#define SCENE_OBJECT_BOX 0 
#define SCENE_OBJECT_SPHERE 1

struct scene;
struct sceneObject;
struct geometry;
struct transform;

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

