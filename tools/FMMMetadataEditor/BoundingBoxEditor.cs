using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class BoundingBoxEditor : ToolWindowBase
    {
        private TreeNode _TreeNode_MinCoord = new TreeNode();
        private TreeNode _TreeNode_MaxCoord = new TreeNode();

        fancyModelMesh.BoundingBoxBlock _Temp = null;

        private void updateInfo()
        {
            ParentNode.Text = "包围盒数据块";

            _TreeNode_MinCoord.Text = String.Format("坐标下界：{0},{1},{2}", _Temp.MinX, _Temp.MinY, _Temp.MinZ);
            _TreeNode_MaxCoord.Text = String.Format("坐标上界：{0},{1},{2}", _Temp.MaxX, _Temp.MaxY, _Temp.MaxZ);
        }

        public BoundingBoxEditor()
        {
            InitializeComponent();
        }

        public BoundingBoxEditor(TreeNode Parent, fancyModelMesh.Block Block)
            : base(Parent, Block)
        {
            InitializeComponent();

            _Temp = ((fancyModelMesh.BoundingBoxBlock)Block).Clone();

            ParentNode.Nodes.Add(_TreeNode_MinCoord);
            ParentNode.Nodes.Add(_TreeNode_MaxCoord);

            updateInfo();

            propertyGrid_main.SelectedObject = _Temp;
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
            fancyModelMesh.BoundingBoxBlock tBlock = (fancyModelMesh.BoundingBoxBlock)Block;

            tBlock.MinX = _Temp.MinX;
            tBlock.MinY = _Temp.MinY;
            tBlock.MinZ = _Temp.MinZ;
            tBlock.MaxX = _Temp.MaxX;
            tBlock.MaxY = _Temp.MaxY;
            tBlock.MaxZ = _Temp.MaxZ;

            toolStrip_main.Visible = false;

            updateInfo();
        }

        private void toolStripButton_cancel_Click(object sender, EventArgs e)
        {
            toolStrip_main.Visible = false;

            _Temp = ((fancyModelMesh.BoundingBoxBlock)Block).Clone();
            propertyGrid_main.SelectedObject = _Temp;
        }
    }
}
