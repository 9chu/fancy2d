using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class SubsetEditor : ToolWindowBase
    {
        private TreeNode _TreeNode_MatName = new TreeNode();
        private TreeNode _TreeNode_PrimType = new TreeNode();
        private TreeNode _TreeNode_StartIndex = new TreeNode();
        private TreeNode _TreeNode_PrimCount = new TreeNode();

        fancyModelMesh.SubsetBlock _Temp = null;

        private void updateInfo()
        {
            ParentNode.Text = "子集数据块：" + _Temp.SubsetName;
            Text = "子集数据编辑器 - " + _Temp.SubsetName;

            _TreeNode_MatName.Text = String.Format("材质名称：{0}", _Temp.MaterialName);
            _TreeNode_PrimType.Text = String.Format("图元类型：{0}", _Temp.SubsetPrimitiveType);
            _TreeNode_StartIndex.Text = String.Format("开始索引：{0}", _Temp.StartIndex);
            _TreeNode_PrimCount.Text = String.Format("图元数量：{0}", _Temp.PrimitiveCount);
        }

        public SubsetEditor()
        {
            InitializeComponent();
        }

        public SubsetEditor(TreeNode Parent, fancyModelMesh.Block Block)
            : base(Parent, Block)
        {
            InitializeComponent();

            _Temp = ((fancyModelMesh.SubsetBlock)Block).Clone();

            ParentNode.Nodes.Add(_TreeNode_MatName);
            ParentNode.Nodes.Add(_TreeNode_PrimType);
            ParentNode.Nodes.Add(_TreeNode_StartIndex);
            ParentNode.Nodes.Add(_TreeNode_PrimCount);

            updateInfo();

            propertyGrid_main.SelectedObject = _Temp;
        }

        private void SubsetEditor_Load(object sender, EventArgs e)
        {

        }

        private void propertyGrid_main_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (toolStrip_main.Visible == false)
            {
                toolStrip_main.Visible = true;
            }
        }

        private void toolStripButton_ok_Click(object sender, EventArgs e)
        {
            fancyModelMesh.SubsetBlock tBlock = (fancyModelMesh.SubsetBlock)Block;

            tBlock.MaterialName = _Temp.MaterialName;
            tBlock.PrimitiveCount = _Temp.PrimitiveCount;
            tBlock.StartIndex = _Temp.StartIndex;
            tBlock.SubsetName = _Temp.SubsetName;
            tBlock.SubsetPrimitiveType = _Temp.SubsetPrimitiveType;

            toolStrip_main.Visible = false;

            updateInfo();
        }

        private void toolStripButton_cancel_Click(object sender, EventArgs e)
        {
            toolStrip_main.Visible = false;

            _Temp = ((fancyModelMesh.SubsetBlock)Block).Clone();
            propertyGrid_main.SelectedObject = _Temp;
        }
    }
}
