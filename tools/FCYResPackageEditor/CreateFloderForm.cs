using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FCYResPackageEditor
{
    public partial class CreateFloderForm : Form
    {
        static public bool OpenCreateFloderForm(string Path, ref string NodeName)
        {
            CreateFloderForm tForm = new CreateFloderForm(Path); 

            if (tForm.ShowDialog() == DialogResult.Cancel)
            {
                tForm.Dispose();
                return false;
            }
            else
            {
                NodeName = tForm.textBox2.Text;
                tForm.Dispose();
                return true;
            }
        }

        public CreateFloderForm(string Path)
        {
            InitializeComponent();

            textBox1.Text = Path;
        }

        private void CreateFloderForm_Load(object sender, EventArgs e)
        {
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }
    }
}
