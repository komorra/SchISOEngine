using SchISOWrapper;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SharpDX;
using Rectangle = System.Drawing.Rectangle;

namespace SchISOTest
{
    public partial class Form1 : Form
    {
        Bitmap bmp = new Bitmap(320, 240);
        private IntPtr scene = IntPtr.Zero;
        private IntPtr geom;
        private IntPtr geom2;
        private DateTime start = DateTime.Now;

        public Form1()
        {
            InitializeComponent();
            Application.Idle += Application_Idle;
            
        }

        void Application_Idle(object sender, EventArgs e)
        {
            var elapsed = DateTime.Now - start;
            if (scene != IntPtr.Zero)
            {
                Stopwatch watch = new Stopwatch();
                watch.Start();

                var bd = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), System.Drawing.Imaging.ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                
                Matrix trans = Matrix.RotationY((float) elapsed.TotalSeconds)*Matrix.LookAtLH(-Vector3.One * 10, Vector3.Zero, Vector3.Up);                
                SchISO.SetGeometryTransform(geom, ref trans);

                var mat = Matrix.Translation(1,1,1) * Matrix.RotationY((float) elapsed.TotalSeconds) * Matrix.LookAtLH(-Vector3.One * 10, Vector3.Zero, Vector3.Up);
                SchISO.SetGeometryTransform(geom2, ref mat);

                SchISO.Render(scene, 0, 0, 0, -100, -100, 200, 200, bmp.Width, bmp.Height, bd.Stride, bd.Scan0);
                bmp.UnlockBits(bd);
                pictureBox1.Image = bmp;
                watch.Stop();
                Text = watch.Elapsed.ToString();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {           
            scene = SchISO.CreateScene();
            var box = SchISO.AddNewSceneObject(scene, SchISO.ObjectTypeBox, -1, -1, -1, 1, 1, 1);
            var oper = SchISO.GetSceneObjectOperation(box);
            geom = SchISO.GetSubGeometry(oper, 0);
            geom2 = SchISO.CreateGeometry(oper, 1, SchISO.ObjectTypeBox, -1, -1, -1, 1, 1, 1);            
            SchISO.UpdateOperation(oper, SchISO.OperationAMinusB);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            SchISO.DestroyScene(scene);
        }
    }
}
