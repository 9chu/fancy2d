using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class VertexEditor : ToolWindowBase
    {
        private TreeNode _TreeNode_VertexElementCount = new TreeNode();
        private TreeNode _TreeNode_VertexCount = new TreeNode();
        private TreeNode _TreeNode_PerVertexSize = new TreeNode();
        private TreeNode _TreeNode_VertexSize = new TreeNode();

        private void updateInfo()
        {
            fancyModelMesh.VertexBlock tBlock = (fancyModelMesh.VertexBlock)Block;
            uint tVertexSize = tBlock.GetVertexSize();
            uint tVertexTotalSize = (uint)tBlock.VertexList.Count * tVertexSize;

            _TreeNode_VertexElementCount.Text = String.Format("顶点元素数量：{0}", tBlock.ElementList.Count);
            _TreeNode_VertexCount.Text = String.Format("顶点数量：{0}", tBlock.VertexList.Count);
            _TreeNode_PerVertexSize.Text = String.Format("单位顶点大小：{0}", tVertexSize);
            _TreeNode_VertexSize.Text = String.Format("顶点总大小：{1:N2} KB ({0} 字节)", tVertexTotalSize, tVertexTotalSize / 1024);
        }

        public VertexEditor()
        {
            InitializeComponent();
        }

        public VertexEditor(TreeNode Parent, fancyModelMesh.Block Block)
            : base(Parent, Block)
        {
            InitializeComponent();

            ParentNode.Text = "顶点数据块";
            ParentNode.Nodes.Add(_TreeNode_VertexElementCount);
            ParentNode.Nodes.Add(_TreeNode_VertexCount);
            ParentNode.Nodes.Add(_TreeNode_PerVertexSize);
            ParentNode.Nodes.Add(_TreeNode_VertexSize);

            updateInfo();
        }

        private void VertexEditor_Load(object sender, EventArgs e)
        {
        }
    }
}
