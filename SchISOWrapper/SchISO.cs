using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using SharpDX;

namespace SchISOWrapper
{
    public static class SchISO
    {
        public const int ObjectTypeBox = 0;
        public const int ObjectTypeSphere = 1;

        public const int OperationUnion = 0;
        public const int OperationIntersection = 1;
        public const int OperationAMinusB = 2;
        public const int OperationBMinusA = 3;

#if DEBUG
        public const string LibPath = "../../../Debug/SchISOEngine.dll";
#else
        public const string LibPath = "../../../Release/SchISOEngine.dll";
#endif
        [DllImport(LibPath)]
        public static extern void Render(IntPtr scene,
            float camx, float camy, float camz,
            float srcx, float srcy, float srcw, float srch,
            int dstw, int dsth, int dstPitch,
            IntPtr dstBuf);
        

        [DllImport(LibPath)]
        public static extern IntPtr CreateScene();

        [DllImport(LibPath)]
        public static extern void DestroyScene(IntPtr scenePtr);

        [DllImport(LibPath)]
        public static extern IntPtr AddNewSceneObject(IntPtr scenePtr,
            int objectType,
            float xmin, float ymin, float zmin,
            float xmax, float ymax, float zmax);        

        [DllImport(LibPath)]
        public static extern void SetGeometryTransform(IntPtr geometry, ref Matrix transform);

        [DllImport(LibPath)]
        public static extern IntPtr GetSceneObjectOperation(IntPtr sceneObj);

        [DllImport(LibPath)]
        public static extern IntPtr GetSubOperation(IntPtr operation, int index);

        [DllImport(LibPath)]
        public static extern IntPtr GetSubGeometry(IntPtr operation, int index);

        [DllImport(LibPath)]
        public static extern IntPtr CreateGeometry(IntPtr operation, int index,
            int objectType,
            float xmin, float ymin, float zmin,
            float xmax, float ymax, float zmax);

        [DllImport(LibPath)]
        public static extern void UpdateOperation(IntPtr operation, int operationType);        
    }
}
