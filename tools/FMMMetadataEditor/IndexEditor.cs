using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class IndexEditor : ToolWindowBase
    {
        private TreeNode _TreeNode_Use32BitIndex = new TreeNode();
        private TreeNode _TreeNode_IndexCount = new TreeNode();

        private void updateInfo()
        {
            fancyModelMesh.IndexBlock tBlock = (fancyModelMesh.IndexBlock)Block;

            _TreeNode_Use32BitIndex.Text = String.Format("是否启用32位索引：{0}", tBlock.UseInt32Index);
            _TreeNode_IndexCount.Text = String.Format("索引数量：{0}", tBlock.IndexList.Count);
        }

        public IndexEditor()
        {
            InitializeComponent();
        }

        public IndexEditor(TreeNode Parent, fancyModelMesh.Block Block)
            : base(Parent, Block)
        {
            InitializeComponent();

            ParentNode.Text = "索引数据块";
            ParentNode.Nodes.Add(_TreeNode_Use32BitIndex);
            ParentNode.Nodes.Add(_TreeNode_IndexCount);

            updateInfo();
        }

        private void IndexEditor_Load(object sender, EventArgs e)
        {

        }
    }
}
