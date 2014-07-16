#include "schiso.h"

extern "C" __declspec(dllexport) void __stdcall Render(
	scene *scn,
	float camx, float camy, float camz,
	float srcx, float srcy, float srcw, float srch,
	int dstw, int dsth, int dstPitch,
	int* dstBuf)
{
	vector3 normal;
	float depth;
	int color = 0xff000000;
	vector3 xy;

	for (int lb = 0; lb < dsth; lb++)
	{
		for (int la = 0; la < dstw; la++)
		{			
			dstBuf[la + lb * dstPitch / 4] = 0xff000000;
			xy.v[0] = (la - 160) / 50.0;
			xy.v[1] = (lb - 120) / 50.0;
			sampleScene(scn, xy, color, normal, depth);
			dstBuf[la + lb * dstPitch / 4] = color;
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
		csgOperation* op = (*it)->operation;
		destroyOperation(op);
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
	
	matrixIdentity(g->trans);
	g->x1 = xmin;
	g->y1 = ymin;
	g->z1 = zmin;
	g->x2 = xmax;
	g->y2 = ymax;
	g->z2 = zmax;
	g->objectType = objectType;

	so->operation = new csgOperation();
	so->operation->operationType = OPERATION_UNION;
	so->operation->geomA = g;
	so->operation->geomB = 0;
	so->operation->operA = 0;
	so->operation->operB = 0;
	scn->sceneObjects.push_back(so);
	return so;
}

extern "C" __declspec(dllexport) csgOperation* __stdcall GetSceneObjectOperation(sceneObject* obj)
{
	return obj->operation;
}

extern "C" __declspec(dllexport) csgOperation* __stdcall GetSubOperation(csgOperation* oper, int index)
{
	if(index == 0) return oper->operA;
	if(index == 1) return oper->operB;
	return 0;
}

extern "C" __declspec(dllexport) geometry* __stdcall GetSubGeometry(csgOperation* oper, int index)
{
	if(index == 0) return oper->geomA;
	if(index == 1) return oper->geomB;
	return 0;
}

extern "C" __declspec(dllexport) geometry* __stdcall CreateGeometry(csgOperation* oper, int index,
	int objectType,
	float xmin, float ymin, float zmin,
	float xmax, float ymax, float zmax)
{
	auto g = new geometry();
	
	matrixIdentity(g->trans);
	g->x1 = xmin;
	g->y1 = ymin;
	g->z1 = zmin;
	g->x2 = xmax;
	g->y2 = ymax;
	g->z2 = zmax;
	g->objectType = objectType;

	if(index == 0 && oper->geomA == 0) oper->geomA = g;
	if(index == 1 && oper->geomB == 0) oper->geomB = g;

	return g;
}

extern "C" __declspec(dllexport) void __stdcall UpdateOperation(csgOperation* oper, int operationType)
{
	oper->operationType = operationType;
}

extern "C" __declspec(dllexport) void __stdcall SetGeometryTransform(geometry* g, float* transform)
{
	for(int la=0;la<16;la++)
	{
		g->trans.m[la] = transform[la];
	}
}