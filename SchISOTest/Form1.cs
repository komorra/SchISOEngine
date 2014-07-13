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

namespace SchISOTest
{
    public partial class Form1 : Form
    {
        Bitmap bmp = new Bitmap(512, 512);
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Stopwatch watch = new Stopwatch();
            watch.Start();

            var bd = bmp.LockBits(new Rectangle(0,0,bmp.Width,bmp.Height), System.Drawing.Imaging.ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            var scene = SchISO.CreateScene();
            var box = SchISO.AddNewSceneObject(scene, SchISO.ObjectTypeBox, 0, 0, 0, 5, 5, 5);
            SchISO.Render(scene, 0, 0, 0, -100, -100, 200, 200, bmp.Width, bmp.Height, bd.Stride, bd.Scan0);
            SchISO.DestroyScene(scene);

            bmp.UnlockBits(bd);
            pictureBox1.Image = bmp;

            watch.Stop();
            Text = watch.Elapsed.ToString();
        }
    }
}
