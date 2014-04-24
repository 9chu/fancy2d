using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FMMMetadataEditor
{
    public partial class MainForm : Form
    {
        private LabelListForm _LabelForm = new LabelListForm();
        private ModelInfoForm _InfoForm = new ModelInfoForm();

        private fancyModelMesh.MeshFile _MeshFile = new fancyModelMesh.MeshFile();
        private string _MeshFilePath = String.Empty;

        private void create()
        {
           // 新建模型
            _MeshFile.New();
            _MeshFilePath = String.Empty;

            _LabelForm.File = _MeshFile;
            _InfoForm.File = _MeshFile; 
        }

        private void load(string Path)
        {
            // 加载模型文件
            try
            {
                fancyModelMesh.MeshFile tFile = fancyModelMesh.MeshFile.FromFile(Path);

                _MeshFile = tFile;
                _MeshFilePath = Path;

                _LabelForm.File = _MeshFile;
                _InfoForm.File = _MeshFile; 
            }
            catch(Exception e)
            {
                MessageBox.Show(String.Format("加载模型失败，错误：{0}", e.Message), "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void save()
        {
        }

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            _LabelForm.Show(dockPanel_main, WeifenLuo.WinFormsUI.Docking.DockState.DockLeft);

            /*
#if DEBUG
            // for DEBUG
            load(@"C:\Users\CHU\Desktop\鸟居\torii.fmm");
#endif
            */
        }

        private void ToolStripMenuItem_new_Click(object sender, EventArgs e)
        {
            if (DialogResult.Yes == MessageBox.Show("确认创建新文件么？\n\n所有未保存的更改将全部丢失！", "退出", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
            {
                create();
            }
        }

        private void ToolStripMenuItem_open_Click(object sender, EventArgs e)
        {
            if (openFileDialog_main.ShowDialog() != DialogResult.Cancel)
            {
                load(openFileDialog_main.FileName);
            }
        }

        private void ToolStripMenuItem_save_Click(object sender, EventArgs e)
        {
            if (String.IsNullOrEmpty(_MeshFilePath))
            {
                ToolStripMenuItem_saveas_Click(sender, e);
            }
            else
            {
                try
                {
                    _MeshFile.Save(_MeshFilePath);
                }
                catch (Exception expt)
                {
                    MessageBox.Show("保存模型文件时失败！\n\n" + expt.Message, "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void ToolStripMenuItem_saveas_Click(object sender, EventArgs e)
        {
            if (saveFileDialog_main.ShowDialog() != DialogResult.Cancel)
            {
                _MeshFilePath = saveFileDialog_main.FileName;

                ToolStripMenuItem_save_Click(sender, e);
            }
        }

        private void ToolStripMenuItem_exit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void ToolStripMenuItem_prop_Click(object sender, EventArgs e)
        {
            _InfoForm.Show();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (DialogResult.No == MessageBox.Show("确认退出程序么？\n\n所有未保存的更改将全部丢失！", "退出", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
            {
                e.Cancel = true;
            }
        }
    }
}
