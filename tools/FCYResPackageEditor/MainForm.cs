using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Microsoft.Win32;
using FancyLib;

namespace FCYResPackageEditor
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void associateFile()
        {
            /*
            // --- 记录程序 ---
            string tProgPath = "\"" + Application.ExecutablePath + "\" \"%1\"";
            RegistryKey tRoot = Registry.ClassesRoot;
            RegistryKey tV2PFile = tRoot.CreateSubKey("v2pfile");

            // 设置注册表程序打开的项与值,其中SetValue("", path)代表设置默认值
            tV2PFile.CreateSubKey("shell").CreateSubKey("Open").CreateSubKey("Command").SetValue("", tProgPath);

            // 设置文件图标
            string tIcoPath = Application.ExecutablePath + ",0";
            tV2PFile.CreateSubKey("DefaultIcon").SetValue("", tIcoPath, RegistryValueKind.ExpandString);

            // --- 关联文件 ---
            tRoot.CreateSubKey(".v2p").SetValue("", "v2pfile");
             */ 
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            // 关联文件
            associateFile();

            if (Environment.GetCommandLineArgs().Length > 1)
                openFile(Environment.GetCommandLineArgs()[1]);
            else 
                toolStripButton_New_Click(sender, e);
            splitContainer1_Panel1_SizeChanged(sender, e);
            splitContainer1_Panel2_SizeChanged(sender, e);
        }

        private void showErr(string Text, string Title = "错误")
        {
            MessageBox.Show(Text, Title, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        // --- 新建 ---
        private void toolStripButton_New_Click(object sender, EventArgs e)
        {
            if (treeView_Main.Tag != null)
            {
                if (MessageBox.Show("确认要创建新文件？", "创建", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                    return;
            }

            updateDetail(null);

            FCYResPackage tPackage = new FCYResPackage();

            // 清空
            treeView_Main.Nodes.Clear();

            // 创建根节点
            TreeNode tRootNode = new TreeNode("根节点",0,0);
            tRootNode.Tag = tPackage.RootNode;
            treeView_Main.Nodes.Add(tRootNode);

            // 绑定数据
            treeView_Main.Tag = tPackage;
            GC.Collect();
        }

        // --- 打开 ---
        private TreeNode readNodes(FCYResPackageNode Node)
        {
            TreeNode tRet = new TreeNode(Node.Name);
            tRet.Tag = Node;

            if (Node.IsFloder())
            {
                tRet.ImageIndex = 0;
                tRet.SelectedImageIndex = 0;

                foreach(FCYResPackageNode SubNode in ((FCYResPackageFloderNode)Node).NodeList)
                {
                    tRet.Nodes.Add(readNodes(SubNode));
                }
            }
            else
            {
                tRet.ImageIndex = 1;
                tRet.SelectedImageIndex = 1;
            }

            return tRet;
        }

        private void openFile(string Path)
        {
            FCYResPackage tPackage = null;

            try
            {
                tPackage = new FCYResPackage(Path);
            }
            catch (FCYExcpOpenErr expt)
            {
                showErr("打开文件失败。该文件可能已被占用。\n\n详细信息：\n" + expt.Message, "打开失败");
                return;
            }

            // 清空
            treeView_Main.Nodes.Clear();

            updateDetail(null);

            // 创建根节点
            TreeNode tRootNode = new TreeNode("根节点", 0, 0);
            tRootNode.Tag = tPackage.RootNode;

            // 绑定数据
            treeView_Main.Tag = tPackage;
            GC.Collect();

            // 读取整个流
            foreach (FCYResPackageNode x in tPackage.RootNode.NodeList)
            {
                tRootNode.Nodes.Add(readNodes(x));
            }

            // 加入根节点
            treeView_Main.Nodes.Add(tRootNode);

            treeView_Main.ExpandAll();
        }

        private void toolStripButton_Open_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() != DialogResult.Cancel)
            {
                openFile(openFileDialog1.FileName);
            }
        }

        // --- 保存 ---
        private void toolStripButton_Save_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() != DialogResult.Cancel)
            {
                try
                {
                    ((FCYResPackage)treeView_Main.Tag).Save(saveFileDialog1.FileName);
                }
                catch(FCYExcpSaveErr expt)
                {
                    showErr("当前文件可能正在被使用，请选择新文件进行保存。\n\n详细信息：\n" + expt.Message, "保存失败");
                    return;
                }
            }
        }

        // --- 创建文件夹 ---
        private void ToolStripMenuItem_CreateFloder_Click(object sender, EventArgs e)
        {
            // 获得父节点
            TreeNode tParent = treeView_Main.SelectedNode;
            if (tParent != null)
            {
                // 检查父节点
                if (!((FCYResPackageNode)tParent.Tag).IsFloder())
                {
                    showErr("请选择一个文件夹节点。");
                    return;
                }

                // 获得父节点
                FCYResPackageFloderNode tParentFloder = (FCYResPackageFloderNode)tParent.Tag;

                // 获得新节点名称
                string tNodeName = "";
                if (!CreateFloderForm.OpenCreateFloderForm(treeView_Main.SelectedNode.FullPath, ref tNodeName))
                    return;

                if (tNodeName == "")
                {
                    showErr("无效的名称。");
                    return;
                }

                // 创建节点
                FCYResPackageFloderNode tNewSubNode = new FCYResPackageFloderNode(tParentFloder, tNodeName);

                // 尝试加入
                try
                {
                    tParentFloder.Add(tNewSubNode);
                }
                catch
                {
                    showErr("节点名已存在或者无效。");
                    return;
                }

                // 创建树节点
                TreeNode tTreeNode = new TreeNode(tNodeName, 0, 0);
                tTreeNode.Tag = tNewSubNode;

                // 加入
                tParent.Nodes.Add(tTreeNode);
                tParent.Expand();
            }
        }

        // --- 创建流 ---
        private void ToolStripMenuItem_CreateStream_Click(object sender, EventArgs e)
        {
            // 获得父节点
            TreeNode tParent = treeView_Main.SelectedNode;
            if (tParent != null)
            {
                // 检查父节点
                if (!((FCYResPackageNode)tParent.Tag).IsFloder())
                {
                    showErr("请选择一个文件夹节点。");
                    return;
                }

                // 获得父节点
                FCYResPackageFloderNode tParentFloder = (FCYResPackageFloderNode)tParent.Tag;

                // 获得新节点名称
                string tNodeName = "";
                string tFileName = "";
                string tAddition = "";
                bool tCompress = false;

                if (!CreateStreamForm.OpenCreateStreamForm(treeView_Main.SelectedNode.FullPath, ref tNodeName, ref tFileName, ref tAddition, ref tCompress))
                    return;

                if (tNodeName == "")
                {
                    showErr("无效的名称。");
                    return;
                }

                // 创建节点
                FCYResPackageDataNode tNewSubNode = new FCYResPackageDataNode((FCYResPackage)treeView_Main.Tag, tParentFloder, tNodeName, tFileName);

                // 尝试加入
                try
                {
                    tParentFloder.Add(tNewSubNode);
                }
                catch
                {
                    showErr("节点名已存在或者无效。");
                    return;
                }

                // 设置属性
                tNewSubNode.IsCompressed = tCompress;
                tNewSubNode.AdditionData = tAddition;

                // 创建树节点
                TreeNode tTreeNode = new TreeNode(tNodeName, 1, 1);
                tTreeNode.Tag = tNewSubNode;

                // 加入
                tParent.Nodes.Add(tTreeNode);
                tParent.Expand();
            }
        }

        // --- 删除节点 ---
        private void toolStripButton_Remove_Click(object sender, EventArgs e)
        {
            // 获得节点
            TreeNode tNode = treeView_Main.SelectedNode;
            if (tNode != null)
            {
                if (tNode.Parent == null)
                    return;

                if (MessageBox.Show("确认删除节点？", "删除确认", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    FCYResPackageFloderNode tParentFloder = (FCYResPackageFloderNode)tNode.Parent.Tag;

                    // 移除节点
                    tParentFloder.NodeList.Remove((FCYResPackageNode)tNode.Tag);

                    tNode.Parent.Nodes.Remove(tNode);
                }
            }
        }

        // --- 选择流 ---
        private void updateDetail(TreeNode pNode)
        {
            tabControl_Data.Tag = pNode;

            if(pNode != null)
                toolStripTextBox_Path.Text = pNode.FullPath;
            else
                toolStripTextBox_Path.Text = "";
            
            textBox1.Text = "";
            textBox2.Text = "";
            textBox3.Text = "";
            textBox4.Text = "";
            checkBox1.Checked = false;

            if (pNode != null)
            {
                if (((FCYResPackageNode)pNode.Tag).IsFloder())
                {
                    FCYResPackageFloderNode tFolderNode = (FCYResPackageFloderNode)pNode.Tag;

                    textBox1.Text = tFolderNode.Name;
                }
                else
                {
                    FCYResPackageDataNode tDataNode = (FCYResPackageDataNode)pNode.Tag;

                    textBox1.Text = tDataNode.Name;
                    textBox2.Text = tDataNode.DataRealSize.ToString();
                    textBox3.Text = tDataNode.DataCompressedSize.ToString();
                    textBox4.Text = tDataNode.AdditionData;
                    checkBox1.Checked = tDataNode.IsCompressed;
                }
            }
        }

        private void treeView_Main_AfterSelect(object sender, TreeViewEventArgs e)
        {
            updateDetail(e.Node);
        }

        // --- 导出流 ---
        private void toolStripButton_Export_Click(object sender, EventArgs e)
        {
            if (tabControl_Data.Tag != null)
            {
                FCYResPackageNode tTempNode = ((FCYResPackageNode)((TreeNode)tabControl_Data.Tag).Tag);
                if (tTempNode.IsFloder())
                    return;

                FCYResPackageDataNode tData = (FCYResPackageDataNode)tTempNode;

                if (tData.DataRealSize == 0)
                {
                    showErr("节点无数据。");
                    return;
                }
                saveFileDialog2.FileName = tData.Name;
                if (saveFileDialog2.ShowDialog() != DialogResult.Cancel)
                {
                    FileStream tOut = new FileStream(saveFileDialog2.FileName, FileMode.Create);

                    tData.ExtractStreamTo(tOut);
                }
            }
        }

        // --- 导入流 ---
        private void toolStripButton_Import_Click(object sender, EventArgs e)
        {
            if (tabControl_Data.Tag != null)
            {
                FCYResPackageNode tTempNode = ((FCYResPackageNode)((TreeNode)tabControl_Data.Tag).Tag);
                if (tTempNode.IsFloder())
                    return;

                FCYResPackageDataNode tData = (FCYResPackageDataNode)tTempNode;

                openFileDialog2.FileName = tData.Name;
                if (openFileDialog2.ShowDialog() != DialogResult.Cancel)
                {
                    FileStream tOut = new FileStream(openFileDialog2.FileName, FileMode.Open, FileAccess.Read, FileShare.Read);

                    tData.ReplaceStream(tOut);
                }
            }
        }

        // --- 属性编辑 ---
        private void toolStripButton_Prop_Click(object sender, EventArgs e)
        {
            if (tabControl_Data.Tag == treeView_Main.Nodes[0])
                return;

            if (tabControl_Data.Tag != null)
            {
                FCYResPackageNode tTempNode = ((FCYResPackageNode)((TreeNode)tabControl_Data.Tag).Tag);
                if (tTempNode.IsFloder())
                {
                    PropEditor.OpenPropEditorForm((FCYResPackageFloderNode)tTempNode);   
                }
                else
                {
                    PropEditor.OpenPropEditorForm((FCYResPackageDataNode)tTempNode);   
                }
                ((TreeNode)tabControl_Data.Tag).Text = tTempNode.Name;
            }

            updateDetail(treeView_Main.SelectedNode);
        }

        // --- 导入文件夹 ---
        /// <summary>
        /// 将文件夹导入
        /// </summary>
        /// <param name="Dir">当前目录信息</param>
        /// <param name="Node">当前目录的节点</param>
        private void importFloder(DirectoryInfo Dir, TreeNode Node)
        {
            // 获得节点
            FCYResPackageFloderNode tFloder = (FCYResPackageFloderNode)Node.Tag;
            // 处理所有文件夹
            foreach (DirectoryInfo x in Dir.GetDirectories())
            {
                TreeNode tNewNode = new TreeNode(Path.GetFileName(x.FullName), 0, 0);
                tNewNode.Tag = new FCYResPackageFloderNode(tFloder, tNewNode.Text);

                try
                {
                    tFloder.Add((FCYResPackageNode)tNewNode.Tag);
                }
                catch(FCYExcpNameErr)
                {
                    showErr("添加节点时遭遇错误，该节点可能已经存在。\n\n文件：" + x.FullName);

                    continue;
                }

                importFloder(x, tNewNode);

                Node.Nodes.Add(tNewNode);
            }

            // 处理所有文件
            foreach (FileInfo x in Dir.GetFiles())
            {
                TreeNode tNewNode = new TreeNode(Path.GetFileName(x.FullName), 1, 1);
                tNewNode.Tag = new FCYResPackageDataNode((FCYResPackage)treeView_Main.Tag, tFloder, tNewNode.Text, x.FullName);

                try
                {
                    tFloder.Add((FCYResPackageNode)tNewNode.Tag);
                }
                catch (FCYExcpNameErr)
                {
                    showErr("添加节点时遭遇错误，该节点可能已经存在。\n\n文件：" + x.FullName);

                    continue;
                }

                Node.Nodes.Add(tNewNode);
            }

            GC.Collect();
        }

        private void toolStripButton_ImportFloder_Click(object sender, EventArgs e)
        {
            TreeNode tSelect = treeView_Main.SelectedNode;
            if(tSelect == null || !((FCYResPackageNode)tSelect.Tag).IsFloder())
            {
                showErr("请选择一个文件夹节点。");
                return;
            }

            string tPath = "";

            {
                FolderBrowserDialog tDiag = new FolderBrowserDialog();
                tDiag.Description = "选择要导入的文件夹";

                if (tDiag.ShowDialog() == DialogResult.Cancel)
                {
                    tDiag.Dispose();
                    return;
                }

                tPath = tDiag.SelectedPath;
                tDiag.Dispose();
            }

            importFloder(new DirectoryInfo(tPath + "\\"), tSelect);
        }

        // --- 导出文件夹 ---
        /// <summary>
        /// 将节点数据解包到文件夹
        /// </summary>
        /// <param name="Dir">目的文件夹</param>
        /// <param name="Node">节点</param>
        private void exportFloder(DirectoryInfo Dir, TreeNode Node)
        {
            FCYResPackageNode tNode = (FCYResPackageNode)Node.Tag;

            // 文件夹节点
            if (tNode.IsFloder())
            {
                // 创建文件夹
                DirectoryInfo tOut = Directory.CreateDirectory(Dir.FullName + "\\" + tNode.Name);
                // 遍历
                foreach(TreeNode x in Node.Nodes)
                {
                    exportFloder(tOut, x);
                }
            }
            else // 文件节点
            {
                // 准备文件
                FileStream tOutput = new FileStream(Dir.FullName + "\\" + tNode.Name, FileMode.Create, FileAccess.ReadWrite, FileShare.Read);

                FCYResPackageDataNode tDataNode = (FCYResPackageDataNode)tNode;

                // 直接写出
                tDataNode.ExtractStreamTo(tOutput);

                // 关闭
                tOutput.Close();
            }
        }

        private void toolStripButton_ExportFloder_Click(object sender, EventArgs e)
        {
            TreeNode tSelect = treeView_Main.SelectedNode;
            if (tSelect == null || !((FCYResPackageNode)tSelect.Tag).IsFloder())
            {
                showErr("请选择一个文件夹节点。");
                return;
            }

            string tPath = "";

            {
                FolderBrowserDialog tDiag = new FolderBrowserDialog();
                tDiag.Description = "选择要导出的目的文件夹";

                if (tDiag.ShowDialog() == DialogResult.Cancel)
                {
                    tDiag.Dispose();
                    return;
                }

                tPath = tDiag.SelectedPath;
                tDiag.Dispose();
            }

            // 创建一个文件夹
            DirectoryInfo tInfo = null;
            string tDirName = tPath + "\\";
            if(((FCYResPackageFloderNode)tSelect.Tag).Name == "")
            {
                tDirName += "根节点";
            }
            else
            {
                tDirName += ((FCYResPackageFloderNode)tSelect.Tag).Name;
            }

            if (!Directory.Exists(tDirName))
            {
                try
                {
                    tInfo = Directory.CreateDirectory(tDirName);
                }
                catch
                {
                    showErr("创建导出文件夹失败。\n\n文件：" + tDirName);
                    return;
                }
            }
            else
            {
                if (MessageBox.Show("导出目录已存在，是否覆盖？", "确认导出", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                {
                    return;
                }
                else
                    tInfo = new DirectoryInfo(tDirName);
            }

            exportFloder(tInfo, tSelect);
        }

        // --- 控件外观 ---
        private void splitContainer1_Panel1_SizeChanged(object sender, EventArgs e)
        {
            treeView_Main.Left = 0;
            treeView_Main.Top = toolStrip_Main.Height;
            treeView_Main.Height = splitContainer1.Panel1.Height - toolStrip_Main.Height;
            treeView_Main.Width = splitContainer1.Panel1.Width;
        }

        private void splitContainer1_Panel2_SizeChanged(object sender, EventArgs e)
        {
            tabControl_Data.Left = 0;
            tabControl_Data.Top = toolStrip_Data.Height;
            tabControl_Data.Height = splitContainer1.Panel2.Height - toolStrip_Data.Height;
            tabControl_Data.Width = splitContainer1.Panel2.Width;
        }
    }
}
