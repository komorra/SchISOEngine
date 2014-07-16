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
struct csgOperation;

void normalizeVector(vector3& v);


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
	csgOperation* operation;
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
};

struct csgOperation
{
	int operationType;
	geometry* geomA;
	geometry* geomB;
	csgOperation* operA;
	csgOperation* operB;
};

void v3copy(const vector3& a, vector3& b)
{
	for(int la=0;la<3;la++)
	{
		b.v[la] = a.v[la];
	}
}

int normalToColor(const vector3& nrm)
{
	int r = (nrm.v[0] + 1.0) / 2.0 * 255.0;
	int g = (nrm.v[1] + 1.0) / 2.0 * 255.0;
	int b = (nrm.v[2] + 1.0) / 2.0 * 255.0;

	g <<= 8;
	b <<= 16;

	return 0xff000000 | (r & 0x000000ff) | (g & 0x0000ff00) | (b & 0x00ff0000);
}

float dot(const vector3& a, const vector3& b)
{
	float d = 0;
	for(int la=0;la<3;la++)
	{
		d += a.v[la] * b.v[la];
	}
	return d;
}

int normalToShade(const vector3& nrm)
{
	vector3 sun;
	for(int la=0;la<3;la++)
	{
		sun.v[la] = -la-1;
	}
	sun.v[2]*=-1.0;
	normalizeVector(sun);
	float d = dot(nrm,sun);
	int sh = (d+1.0)/2.0*255.0;
	return 0xff000000 | (sh) | (sh << 8) | (sh << 16);
}

void project(const vector3 &src, vector3 &result)
{
	const float xd = 0.86;
	const float yd = 0.5;
	const float zd = 0.5;
	result.v[0] = src.v[0] * xd + src.v[1] * yd;
	result.v[1] = src.v[0] * xd + src.v[2] * zd;
}

void transformCoordinate(const matrix& transform, vector3& src)
{
	const float m[4] = { src.v[0], src.v[1], src.v[2], 1 };
	float res[4] = { 0, 0, 0, 0 };	

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{			
			res[col] += transform.m[col + row * 4] * m[row];
		}
	}

	src.v[0] = res[0];
	src.v[1] = res[1];
	src.v[2] = res[2];
}

void transformNormal(const matrix& transform, vector3& src)
{
	const float m[4] = { src.v[0], src.v[1], src.v[2], 0 };
	float res[4] = { 0, 0, 0, 0 };

	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			res[col] += transform.m[col + row * 4] * m[row];
		}
	}

	src.v[0] = res[0];
	src.v[1] = res[1];
	src.v[2] = res[2];
}

float length(const vector3& v)
{
	return sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
}

float distance(const vector3& a, const vector3& b)
{
	vector3 dif;
	for (int la = 0; la < 3; la++)
	{
		dif.v[la] = a.v[la] - b.v[la];
	}
	return length(dif);
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

void matrixRotation(matrix& transform, int axis, float angle)
{
	matrixIdentity(transform);
	if(axis==0)
	{
		transform.m[1*4 + 1] = cos(angle);
		transform.m[2*4 + 1] = -sin(angle);
		transform.m[1*4 + 2] = sin(angle);
		transform.m[2*4 + 2] = cos(angle);
	}
	else if(axis==1)
	{
		transform.m[0*4 + 0] = cos(angle);
		transform.m[2*4 + 0] = sin(angle);
		transform.m[0*4 + 2] = -sin(angle);
		transform.m[2*4 + 2] = cos(angle);
	}
	else if(axis==2)
	{
		transform.m[0*4 + 0] = cos(angle);
		transform.m[1*4 + 0] = -sin(angle);
		transform.m[0*4 + 1] = sin(angle);
		transform.m[1*4 + 1] = cos(angle);
	}
}

void matrixTranslation(matrix& transform, float x, float y, float z)
{
	matrixIdentity(transform);
	transform.m[0*4 + 3] = x;
	transform.m[1*4 + 3] = y;
	transform.m[2*4 + 3] = z;
}

void matrixInvert(matrix& transform)
{
    double inv[16], det;
    int i;
	float* m = &(*transform.m);

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    /*if (det == 0)
        return false*/;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
		transform.m[i] = inv[i] * det;

    //return true;
}

void matrixScale(matrix& transform, float x, float y, float z)
{
	matrixIdentity(transform);
	transform.m[0*4 + 0] = x;
	transform.m[1*4 + 1] = y;
	transform.m[2*4 + 2] = z;
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
	vector3 tpos;
	vector3 tnrm;
	float dist = 100000000;
	bool ret = false;

	for (int la = 0; la < 6; la++)
	{		
		if (rayRectangleIntersection(ray, la, min, max, tpos, tnrm))
		{
			//return true;
			float d = distance(ray.org, tpos);
			if (d < dist)
			{
				dist = d;
				for (int la = 0; la < 3; la++)
				{
					outNrm.v[la] = tnrm.v[la];
					outPos.v[la] = tpos.v[la];
				}
				ret = true;
			}
		}
	}
	return ret;
}

bool rayAABBIntersectionFar(const ray& ray, const vector3& min, const vector3& max, vector3& outPos, vector3& outNrm)
{
	vector3 tpos;
	vector3 tnrm;
	float dist = 0;
	bool ret = false;

	for (int la = 0; la < 6; la++)
	{		
		if (rayRectangleIntersection(ray, la, min, max, tpos, tnrm))
		{
			//return true;
			float d = distance(ray.org, tpos);
			if (d > dist)
			{
				dist = d;
				for (int la = 0; la < 3; la++)
				{
					outNrm.v[la] = tnrm.v[la];
					outPos.v[la] = tpos.v[la];
				}
				ret = true;
			}
		}
	}
	return ret;
}

bool renderOperation(const csgOperation* oper, const ray& r, vector3& pos, vector3& nrm)
{
	vector3 tposa;
	vector3 tnrma;
	matrix tmata;
	bool intera = false;
	ray trA = ray(r);
	vector3 amin;
	vector3 amax;
	for(int la=0;la<3;la++)
	{
		tposa.v[la] = 10000000000;
	}
	amin.v[0] = oper->geomA->x1;
	amin.v[1] = oper->geomA->y1;
	amin.v[2] = oper->geomA->z1;
	amax.v[0] = oper->geomA->x2;
	amax.v[1] = oper->geomA->y2;
	amax.v[2] = oper->geomA->z2;

	for(int la=0;la<16;la++)tmata.m[la] = oper->geomA->trans.m[la];
	matrixInvert(tmata);
	transformCoordinate(tmata, trA.org);
	transformNormal(tmata, trA.dir);
	intera = rayAABBIntersection(trA, amin, amax, tposa, tnrma);
	
	vector3 tposb;
	vector3 tnrmb;
	matrix tmatb;
	bool interb = false;
	ray trB = ray(r);
	vector3 bmin;
	vector3 bmax;

	for(int la=0;la<3;la++)
	{
		tposb.v[la] = 10000000000;
	}

	if(oper->geomB != 0)
	{
		bmin.v[0] = oper->geomB->x1;
		bmin.v[1] = oper->geomB->y1;
		bmin.v[2] = oper->geomB->z1;
		bmax.v[0] = oper->geomB->x2;
		bmax.v[1] = oper->geomB->y2;
		bmax.v[2] = oper->geomB->z2;

		for(int la=0;la<16;la++)tmatb.m[la] = oper->geomB->trans.m[la];
		matrixInvert(tmatb);
		transformCoordinate(tmatb, trB.org);
		transformNormal(tmatb, trB.dir);
		if(oper->operationType == OPERATION_AMINUSB)
		{
			interb = rayAABBIntersectionFar(trB, bmin, bmax, tposb, tnrmb);
		}
		else
		{
			interb = rayAABBIntersection(trB, bmin, bmax, tposb, tnrmb);
		}
	}

	if(oper->operationType == OPERATION_UNION)
	{
		if(interb == false)
		{
			v3copy(tposa,pos);
			v3copy(tnrma,nrm);
			return intera;
		}
		else
		{
			if((distance(tposa, trA.org) < distance(tposb, trB.org)))
			{
				v3copy(tposa,pos);
				v3copy(tnrma,nrm);
				return intera;
			}
			else
			{
				v3copy(tposb,pos);
				v3copy(tnrmb,nrm);
				return interb;
			}
		}
	}
	else if(oper->operationType == OPERATION_AMINUSB)
	{
		if(interb == false)
		{
			v3copy(tposa,pos);
			v3copy(tnrma,nrm);
			return intera;
		}
		else if(intera)
		{
			if(distance(tposb, trB.org) > distance(tposa, trA.org))
			{
				v3copy(tposb,pos);
				v3copy(tnrmb,nrm);
				for(int la=0;la<3;la++) nrm.v[la] *= -1;
			}
			else
			{
				v3copy(tposa,pos);
				v3copy(tnrma,nrm);
			}
			return true;
		}
		return false;
	}
}

void sampleScene(scene* scn, const vector3& xy, int& color, vector3& normal, float& depth)
{
	ray r;

	//project(xy, r.org);
	r.org.v[0] = xy.v[0];// + 5;
	r.org.v[1] = xy.v[1];// - 5;
	r.org.v[2] = 0;
	//r.org.v[2] = ;// xy.v[0] + xy.v[1];

	r.dir.v[0] = 0;//-0.5;
	r.dir.v[1] = 0;//0.5;
	r.dir.v[2] = 1;

	normalizeVector(r.dir);

	vector3 min;
	vector3 max;
	vector3 pos;
	vector3 nrm;
	color = 0xff000000;
	for (auto it = scn->sceneObjects.begin(); it != scn->sceneObjects.end(); it++)
	{
		if(renderOperation((*it)->operation, r, pos, nrm))
		{
			color = normalToShade(nrm);
			return;
		}
		/*min.v[0] = (*it)->geometriesUnion[0]->x1;
		min.v[1] = (*it)->geometriesUnion[0]->y1;
		min.v[2] = (*it)->geometriesUnion[0]->z1;

		max.v[0] = (*it)->geometriesUnion[0]->x2;
		max.v[1] = (*it)->geometriesUnion[0]->y2;
		max.v[2] = (*it)->geometriesUnion[0]->z2;

		matrix t;
		for(int la=0;la<16;la++)t.m[la] = (*it)->geometriesUnion[0]->trans.m[la];

		matrixInvert(t);
		transformCoordinate(t, r.org);
		transformNormal(t, r.dir);

		if (rayAABBIntersection(r, min, max, pos, nrm))
		{
			color = normalToShade(nrm);
			return;
		}*/
	}
}

void destroyOperation(csgOperation* op)
{
	if(op->geomA!=0) delete op->geomA;
	if(op->geomB!=0) delete op->geomB;
	if(op->operA)
	{
		destroyOperation(op->operA);		
	}
	if(op->operB)
	{
		destroyOperation(op->operB);			
	}
	delete op;
}

