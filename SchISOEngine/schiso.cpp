#include "schiso.h"

extern "C" __declspec(dllexport) void __stdcall Render(
	float camx, float camy, float camz,
	float srcx, float srcy, float srcw, float srch,
	int dstw, int dsth, int dstPitch,
	int* dstBuf)
{
	for (int lb = 0; lb < dsth; lb++)
	{
		for (int la = 0; la < dstw; la++)
		{
			dstBuf[la + lb * dstPitch / 4] = 0xff000000;
		}
	}
}


extern "C" __declspec(dllexport) scene* __stdcall CreateScene()
{
	scene* scn = new scene();

	return scn;
}

extern "C" __declspec(dllexport) void __stdcall DestroyScene(scene* scn)
{
	for (auto it = scn->sceneObjects.begin(); it != scn->sceneObjects.end(); it++)
	{
		for (auto g = (*it)->geometriesUnion.begin(); g != (*it)->geometriesUnion.end(); g++)
		{
			auto inners = std::vector<geometry*>();

			auto gptr = *g;
			do
			{
				inners.push_back(gptr);
				gptr = gptr->operand;
			} while (gptr != 0);

			for (auto i = inners.begin(); i != inners.end(); i++)
			{
				delete *i;
			}
		}
		delete *it;
	}
	delete scn;
}

extern "C" __declspec(dllexport) sceneObject* __stdcall AddNewSceneObject(
	scene* scn,
	int objectType,
	float xmin, float ymin, float zmin,
	float xmax, float ymax, float zmax)
{
	auto so = new sceneObject();
	auto g = new geometry();

	g->operand = 0;
	matrixIdentity(g->trans);
	g->x1 = xmin;
	g->y1 = ymin;
	g->z1 = zmin;
	g->x2 = xmax;
	g->y2 = ymax;
	g->z2 = zmax;
	g->objectType = objectType;

	so->geometriesUnion.push_back(g);
	scn->sceneObjects.push_back(so);
	return so;
}

extern "C" __declspec(dllexport) geometry* __stdcall GetGeometry(sceneObject* obj, int index)
{
	return obj->geometriesUnion[index];
}

extern "C" __declspec(dllexport) geometry* __stdcall AddGeometryToObject(sceneObject* obj,
	int objectType,
	float xmin, float ymin, float zmin,
	float xmax, float ymax, float zmax)
{
	auto g = new geometry();
	g->operand = 0;
	matrixIdentity(g->trans);
	g->x1 = xmin;
	g->y1 = ymin;
	g->z1 = zmin;
	g->x2 = xmax;
	g->y2 = ymax;
	g->z2 = zmax;
	g->objectType = objectType;

	obj->geometriesUnion.push_back(g);

	return g;
}

extern "C" __declspec(dllexport) geometry* __stdcall AddGeometryToGeometry(geometry* g,
	int operationType,
	int objectType,
	float xmin, float ymin, float zmin,
	float xmax, float ymax, float zmax)
{
	geometry* added = 0;
	if (g->operand == 0)
	{
		added = new geometry();
		added->operand = 0;
		matrixIdentity(added->trans);
		added->x1 = xmin;
		added->y1 = ymin;
		added->z1 = zmin;
		added->x2 = xmax;
		added->y2 = ymax;
		added->z2 = zmax;
		added->objectType = objectType;
	}
	g->operationType = operationType;
	return added;
}