using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class MaterialEditor : ToolWindowBase
    {
        private void updateInfo()
        {
            fancyModelMesh.MaterialBlock tBlock = (fancyModelMesh.MaterialBlock)Block;

            ParentNode.Text = "材质数据块：" + tBlock.MaterialName;
            Text = "材质数据编辑器 - " + tBlock.MaterialName;

            textBox_content.Text = tBlock.DefineXML.Replace("\n","\r\n");
            toolStripTextBox_name.Text = tBlock.MaterialName;

            toolStripButton_change.Visible = false;
            toolStripButton_restore.Visible = false; 
            toolStripButton_ok.Visible = false;
            toolStripButton_cancel.Visible = false;
        }

        public MaterialEditor()
        {
            InitializeComponent();
        }

        public MaterialEditor(TreeNode Parent, fancyModelMesh.Block Block)
            : base(Parent, Block)
        {
            InitializeComponent();

            updateInfo();
        }

        private void MaterialEditor_Load(object sender, EventArgs e)
        {

        }

        private void toolStripTextBox_name_TextChanged(object sender, EventArgs e)
        {
            if (toolStripButton_ok.Visible == false)
            {
                toolStripButton_ok.Visible = true;
                toolStripButton_cancel.Visible = true;
            }
        }

        private void toolStripButton_ok_Click(object sender, EventArgs e)
        {
            fancyModelMesh.MaterialBlock tBlock = (fancyModelMesh.MaterialBlock)Block;

            tBlock.MaterialName = toolStripTextBox_name.Text.Replace("\r\n", "\n");
            updateInfo();
        }

        private void toolStripButton_cancel_Click(object sender, EventArgs e)
        {
            fancyModelMesh.MaterialBlock tBlock = (fancyModelMesh.MaterialBlock)Block;

            toolStripTextBox_name.Text = tBlock.MaterialName;

            toolStripButton_cancel.Visible = false;
            toolStripButton_ok.Visible = false;
        }

        private void toolStripButton_change_Click(object sender, EventArgs e)
        {
            fancyModelMesh.MaterialBlock tBlock = (fancyModelMesh.MaterialBlock)Block;

            tBlock.DefineXML = textBox_content.Text;
            updateInfo();
        }

        private void toolStripButton_restore_Click(object sender, EventArgs e)
        {
            fancyModelMesh.MaterialBlock tBlock = (fancyModelMesh.MaterialBlock)Block;

            textBox_content.Text = tBlock.DefineXML;

            toolStripButton_change.Visible = false;
            toolStripButton_restore.Visible = false;
        }

        private void textBox_content_TextChanged(object sender, EventArgs e)
        {
            if (toolStripButton_change.Visible == false)
            {
                toolStripButton_change.Visible = true;
                toolStripButton_restore.Visible = true;
            }
        }

        private void toolStripTextBox_name_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                toolStripButton_ok_Click(sender, e);
                e.Handled = true;
            }
        }
    }
}
