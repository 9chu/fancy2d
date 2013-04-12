using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace FCYResPackageEditor
{
    public partial class CreateStreamForm : Form
    {
        static public bool OpenCreateStreamForm(string Path, ref string NodeName, ref string FileName, ref string Addition, ref bool Compress)
        {
            CreateStreamForm tForm = new CreateStreamForm(Path);

            if (tForm.ShowDialog() == DialogResult.Cancel)
            {
                tForm.Dispose();
                return false;
            }
            else
            {
                NodeName = tForm.textBox2.Text;
                FileName = tForm.textBox3.Text;
                Addition = tForm.textBox4.Text;
                Compress = tForm.checkBox1.Checked;
                tForm.Dispose();
                return true;
            }
        }

        public CreateStreamForm(string Path)
        {
            InitializeComponent();

            textBox1.Text = Path;
        }

        private void CreateStreamForm_Load(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() != DialogResult.Cancel)
            {
                textBox3.Text = openFileDialog1.FileName;

                if(textBox2.Text == "")
                    textBox2.Text = Path.GetFileName(openFileDialog1.FileName);
            }
        }
    }
}
