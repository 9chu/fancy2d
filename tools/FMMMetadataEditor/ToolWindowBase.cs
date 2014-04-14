using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class ToolWindowBase :
        WeifenLuo.WinFormsUI.Docking.DockContent
    {
        TreeNode _ParentNode = null;
        fancyModelMesh.Block _Block = null;

        protected TreeNode ParentNode
        {
            get
            {
                return _ParentNode;
            }
        }

        public fancyModelMesh.Block Block
        {
            get
            {
                return _Block;
            }
        }

        public ToolWindowBase()
        {
            InitializeComponent();
        }

        public ToolWindowBase(TreeNode ParentNode, fancyModelMesh.Block Block)
        {
            InitializeComponent();

            _ParentNode = ParentNode;
            _Block = Block;
        }

        private void ToolWindowBase_Load(object sender, EventArgs e)
        {

        }
    }
}
