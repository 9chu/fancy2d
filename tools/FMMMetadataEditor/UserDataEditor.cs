using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace FMMMetadataEditor
{
    public partial class UserDataEditor : ToolWindowBase
    {
        private TreeNode _TreeNode_Size = new TreeNode();

        private void updateInfo()
        {
            fancyModelMesh.UnknownBlock tBlock = (fancyModelMesh.UnknownBlock)Block;

            ParentNode.Text = "用户数据块：" + tBlock.BlockName;
            Text = "用户数据编辑器 - " + tBlock.BlockName;

            _TreeNode_Size.Text = String.Format("数据大小：{0}", tBlock.BlockData.Length);
        }

        public UserDataEditor()
        {
            InitializeComponent();
        }

        public UserDataEditor(TreeNode ParentNode, fancyModelMesh.Block Block)
            : base(ParentNode, Block)
        {
            InitializeComponent();
        }

        private void UserDataEditor_Load(object sender, EventArgs e)
        {

        }

        private void button_export_Click(object sender, EventArgs e)
        {
            if (saveFileDialog_userData.ShowDialog() != System.Windows.Forms.DialogResult.Cancel)
            {
                try
                {
                    fancyModelMesh.UnknownBlock tBlock = (fancyModelMesh.UnknownBlock)Block;

                    using (FileStream tFile = new FileStream(saveFileDialog_userData.FileName, FileMode.Create, FileAccess.Write))
                    {
                        tFile.Write(tBlock.BlockData, 0, tBlock.BlockData.Length);
                        tFile.Close();
                    }
                }
                catch (Exception expt)
                {
                    MessageBox.Show("保存文件失败！\n\n" + expt.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void button_import_Click(object sender, EventArgs e)
        {
            if (openFileDialog_userData.ShowDialog() != System.Windows.Forms.DialogResult.Cancel)
            {
                try
                {
                    using (FileStream tFile = new FileStream(openFileDialog_userData.FileName, FileMode.Open, FileAccess.Read))
                    {
                        byte[] tData = new byte[tFile.Length];
                        tFile.Read(tData, 0, (int)tFile.Length);
                        tFile.Close();
                    }

                    updateInfo();
                }
                catch (Exception expt)
                {
                    MessageBox.Show("打开文件失败！\n\n" + expt.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }
    }
}
