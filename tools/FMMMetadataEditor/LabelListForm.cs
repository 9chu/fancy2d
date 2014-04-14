using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class LabelListForm : WeifenLuo.WinFormsUI.Docking.DockContent
    {
        private fancyModelMesh.MeshFile _File = null;

        public LabelListForm()
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

                // 清空已有节点
                foreach (TreeNode p in LabelTreeView.Nodes)
                {
                    ToolWindowBase tForm = p.Tag as ToolWindowBase;
                    if (tForm != null)
                    {
                        tForm.Close();
                    }
                }

                // 装载所有节点
                LabelTreeView.Nodes.Clear();
                foreach (fancyModelMesh.Block i in _File.BlockList)
                {
                    AddNode(i);
                }
            }
        }

        public TreeView LabelTreeView
        {
            get
            {
                return treeView_labels;
            }
        }

        private void AddNode(fancyModelMesh.Block Block)
        {
            TreeNode tNode = new TreeNode();
            ToolWindowBase tBase = null;

            if (Block is fancyModelMesh.VertexBlock)
                tBase = new VertexEditor(tNode, Block);
            else if (Block is fancyModelMesh.IndexBlock)
                tBase = new IndexEditor(tNode, Block);
            if (Block is fancyModelMesh.SubsetBlock)
                tBase = new SubsetEditor(tNode, Block);
            else if (Block is fancyModelMesh.MaterialBlock)
                tBase = new MaterialEditor(tNode, Block);
            else if (Block is fancyModelMesh.UnknownBlock)
                tBase = new UserDataEditor(tNode, Block);

            tNode.Tag = tBase;
            LabelTreeView.Nodes.Add(tNode);
        }

        private void LabelListForm_Load(object sender, EventArgs e)
        {

        }

        private void treeView_labels_DoubleClick(object sender, EventArgs e)
        {
            if (treeView_labels.SelectedNode != null)
            {
                ToolWindowBase tForm = treeView_labels.SelectedNode.Tag as ToolWindowBase;
                if (tForm != null && !(tForm is VertexEditor) && !(tForm is IndexEditor))
                {
                    if (tForm.Visible == false)
                    {
                        tForm.Show(DockPanel, WeifenLuo.WinFormsUI.Docking.DockState.Document);
                    }
                    else
                    {
                        tForm.Focus();
                    }
                }
            }
        }

        private void ToolStripMenuItem_newVertex_Click(object sender, EventArgs e)
        {
            if (_File != null)
            {
                fancyModelMesh.Block tBlock = new fancyModelMesh.VertexBlock();
                _File.BlockList.Add(tBlock);
                AddNode(tBlock);
            }
        }

        private void ToolStripMenuItem_newIndex_Click(object sender, EventArgs e)
        {
            if (_File != null)
            {
                fancyModelMesh.Block tBlock = new fancyModelMesh.IndexBlock();
                _File.BlockList.Add(tBlock);
                AddNode(tBlock);
            }
        }

        private void ToolStripMenuItem_newSubset_Click(object sender, EventArgs e)
        {
            if (_File != null)
            {
                fancyModelMesh.Block tBlock = new fancyModelMesh.SubsetBlock();
                _File.BlockList.Add(tBlock);
                AddNode(tBlock);
            }
        }

        private void ToolStripMenuItem_newMat_Click(object sender, EventArgs e)
        {
            if (_File != null)
            {
                fancyModelMesh.Block tBlock = new fancyModelMesh.MaterialBlock();
                _File.BlockList.Add(tBlock);
                AddNode(tBlock);
            }
        }

        private void ToolStripMenuItem_userData_Click(object sender, EventArgs e)
        {
            if (_File != null)
            {
                fancyModelMesh.Block tBlock = new fancyModelMesh.UnknownBlock("_USER");
                _File.BlockList.Add(tBlock);
                AddNode(tBlock);
            }
        }

        private void toolStripButton_remove_Click(object sender, EventArgs e)
        {
            if (treeView_labels.SelectedNode != null && treeView_labels.SelectedNode.Tag is ToolWindowBase)
            {
                TreeNode p = treeView_labels.SelectedNode;
                ToolWindowBase tForm = p.Tag as ToolWindowBase;

                if (DialogResult.Yes == MessageBox.Show(
                    String.Format("确认删除数据块 {0} ？\n\n该操作不可逆！", tForm.Block.BlockName), 
                    "删除数据块", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
                {
                    if (tForm != null)
                    {
                        tForm.Close();
                    }

                    p.Remove();
                }
            }
        }
    }
}
