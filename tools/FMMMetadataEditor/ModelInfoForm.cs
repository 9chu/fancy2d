using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class ModelInfoForm : Form
    {
        fancyModelMesh.MeshFile _File = null;

        public ModelInfoForm()
        {
            InitializeComponent();
        }

        public fancyModelMesh.MeshFile File
        {
            get
            {
                return _File;
            }
            set
            {
                _File = value;
                propertyGrid_main.SelectedObject = _File;
            }
        }

        private void ModelInfoForm_Load(object sender, EventArgs e)
        {

        }

        private void ModelInfoForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                Hide();
            }
        }
    }
}
