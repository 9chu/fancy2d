namespace FCYResPackageEditor
{
    partial class MainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.toolStrip_Main = new System.Windows.Forms.ToolStrip();
            this.toolStripButton_New = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_Open = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_Save = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripDropDownButton_Add = new System.Windows.Forms.ToolStripDropDownButton();
            this.ToolStripMenuItem_CreateStream = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_CreateFloder = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripButton_Remove = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButton_ImportFloder = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_ExportFloder = new System.Windows.Forms.ToolStripButton();
            this.treeView_Main = new System.Windows.Forms.TreeView();
            this.imageList_Main = new System.Windows.Forms.ImageList(this.components);
            this.tabControl_Data = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.toolStrip_Data = new System.Windows.Forms.ToolStrip();
            this.toolStripButton_Import = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_Export = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_Prop = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripTextBox_Path = new System.Windows.Forms.ToolStripTextBox();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.openFileDialog2 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog2 = new System.Windows.Forms.SaveFileDialog();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.toolStrip_Main.SuspendLayout();
            this.tabControl_Data.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.toolStrip_Data.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.toolStrip_Main);
            this.splitContainer1.Panel1.Controls.Add(this.treeView_Main);
            this.splitContainer1.Panel1.SizeChanged += new System.EventHandler(this.splitContainer1_Panel1_SizeChanged);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tabControl_Data);
            this.splitContainer1.Panel2.Controls.Add(this.toolStrip_Data);
            this.splitContainer1.Panel2.SizeChanged += new System.EventHandler(this.splitContainer1_Panel2_SizeChanged);
            this.splitContainer1.Size = new System.Drawing.Size(541, 268);
            this.splitContainer1.SplitterDistance = 215;
            this.splitContainer1.TabIndex = 0;
            // 
            // toolStrip_Main
            // 
            this.toolStrip_Main.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_New,
            this.toolStripButton_Open,
            this.toolStripButton_Save,
            this.toolStripSeparator1,
            this.toolStripDropDownButton_Add,
            this.toolStripButton_Remove,
            this.toolStripSeparator3,
            this.toolStripButton_ImportFloder,
            this.toolStripButton_ExportFloder});
            this.toolStrip_Main.Location = new System.Drawing.Point(0, 0);
            this.toolStrip_Main.Name = "toolStrip_Main";
            this.toolStrip_Main.Size = new System.Drawing.Size(215, 25);
            this.toolStrip_Main.TabIndex = 2;
            this.toolStrip_Main.Text = "toolStrip1";
            // 
            // toolStripButton_New
            // 
            this.toolStripButton_New.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_New.Image = global::FCYResPackageEditor.Properties.Resources.ICON_NEW;
            this.toolStripButton_New.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_New.Name = "toolStripButton_New";
            this.toolStripButton_New.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_New.Text = "新建";
            this.toolStripButton_New.Click += new System.EventHandler(this.toolStripButton_New_Click);
            // 
            // toolStripButton_Open
            // 
            this.toolStripButton_Open.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_Open.Image = global::FCYResPackageEditor.Properties.Resources.ICON_DIR;
            this.toolStripButton_Open.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_Open.Name = "toolStripButton_Open";
            this.toolStripButton_Open.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_Open.Text = "打开";
            this.toolStripButton_Open.Click += new System.EventHandler(this.toolStripButton_Open_Click);
            // 
            // toolStripButton_Save
            // 
            this.toolStripButton_Save.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_Save.Image = global::FCYResPackageEditor.Properties.Resources.ICON_SAVE;
            this.toolStripButton_Save.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_Save.Name = "toolStripButton_Save";
            this.toolStripButton_Save.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_Save.Text = "保存";
            this.toolStripButton_Save.Click += new System.EventHandler(this.toolStripButton_Save_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripDropDownButton_Add
            // 
            this.toolStripDropDownButton_Add.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripDropDownButton_Add.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_CreateStream,
            this.ToolStripMenuItem_CreateFloder});
            this.toolStripDropDownButton_Add.Image = global::FCYResPackageEditor.Properties.Resources.ICON_ADD;
            this.toolStripDropDownButton_Add.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton_Add.Name = "toolStripDropDownButton_Add";
            this.toolStripDropDownButton_Add.Size = new System.Drawing.Size(29, 22);
            this.toolStripDropDownButton_Add.Text = "添加节点";
            // 
            // ToolStripMenuItem_CreateStream
            // 
            this.ToolStripMenuItem_CreateStream.Name = "ToolStripMenuItem_CreateStream";
            this.ToolStripMenuItem_CreateStream.Size = new System.Drawing.Size(136, 22);
            this.ToolStripMenuItem_CreateStream.Text = "创建流";
            this.ToolStripMenuItem_CreateStream.Click += new System.EventHandler(this.ToolStripMenuItem_CreateStream_Click);
            // 
            // ToolStripMenuItem_CreateFloder
            // 
            this.ToolStripMenuItem_CreateFloder.Name = "ToolStripMenuItem_CreateFloder";
            this.ToolStripMenuItem_CreateFloder.Size = new System.Drawing.Size(136, 22);
            this.ToolStripMenuItem_CreateFloder.Text = "创建档案夹";
            this.ToolStripMenuItem_CreateFloder.Click += new System.EventHandler(this.ToolStripMenuItem_CreateFloder_Click);
            // 
            // toolStripButton_Remove
            // 
            this.toolStripButton_Remove.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_Remove.Image = global::FCYResPackageEditor.Properties.Resources.ICON_DEC;
            this.toolStripButton_Remove.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_Remove.Name = "toolStripButton_Remove";
            this.toolStripButton_Remove.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_Remove.Text = "删除节点";
            this.toolStripButton_Remove.Click += new System.EventHandler(this.toolStripButton_Remove_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripButton_ImportFloder
            // 
            this.toolStripButton_ImportFloder.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_ImportFloder.Image = global::FCYResPackageEditor.Properties.Resources.ICON_DIR_IMPORT;
            this.toolStripButton_ImportFloder.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_ImportFloder.Name = "toolStripButton_ImportFloder";
            this.toolStripButton_ImportFloder.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_ImportFloder.Text = "导入文件夹";
            this.toolStripButton_ImportFloder.Click += new System.EventHandler(this.toolStripButton_ImportFloder_Click);
            // 
            // toolStripButton_ExportFloder
            // 
            this.toolStripButton_ExportFloder.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_ExportFloder.Image = global::FCYResPackageEditor.Properties.Resources.ICON_DIR_EXPORT;
            this.toolStripButton_ExportFloder.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_ExportFloder.Name = "toolStripButton_ExportFloder";
            this.toolStripButton_ExportFloder.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_ExportFloder.Text = "导出文件夹";
            this.toolStripButton_ExportFloder.Click += new System.EventHandler(this.toolStripButton_ExportFloder_Click);
            // 
            // treeView_Main
            // 
            this.treeView_Main.ImageIndex = 0;
            this.treeView_Main.ImageList = this.imageList_Main;
            this.treeView_Main.Location = new System.Drawing.Point(21, 48);
            this.treeView_Main.Name = "treeView_Main";
            this.treeView_Main.SelectedImageIndex = 0;
            this.treeView_Main.Size = new System.Drawing.Size(174, 165);
            this.treeView_Main.TabIndex = 1;
            this.treeView_Main.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_Main_AfterSelect);
            // 
            // imageList_Main
            // 
            this.imageList_Main.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList_Main.ImageStream")));
            this.imageList_Main.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList_Main.Images.SetKeyName(0, "ICON_DIR.png");
            this.imageList_Main.Images.SetKeyName(1, "ICON_FILE.png");
            // 
            // tabControl_Data
            // 
            this.tabControl_Data.Controls.Add(this.tabPage1);
            this.tabControl_Data.Controls.Add(this.tabPage2);
            this.tabControl_Data.Location = new System.Drawing.Point(3, 40);
            this.tabControl_Data.Name = "tabControl_Data";
            this.tabControl_Data.SelectedIndex = 0;
            this.tabControl_Data.Size = new System.Drawing.Size(316, 216);
            this.tabControl_Data.TabIndex = 1;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.textBox3);
            this.tabPage1.Controls.Add(this.textBox2);
            this.tabPage1.Controls.Add(this.textBox1);
            this.tabPage1.Controls.Add(this.checkBox1);
            this.tabPage1.Controls.Add(this.label3);
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(308, 190);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "属性";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(85, 67);
            this.textBox3.Name = "textBox3";
            this.textBox3.ReadOnly = true;
            this.textBox3.Size = new System.Drawing.Size(217, 21);
            this.textBox3.TabIndex = 6;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(85, 40);
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.Size = new System.Drawing.Size(217, 21);
            this.textBox2.TabIndex = 5;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(85, 13);
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(217, 21);
            this.textBox1.TabIndex = 4;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Enabled = false;
            this.checkBox1.Location = new System.Drawing.Point(17, 98);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(72, 16);
            this.checkBox1.TabIndex = 3;
            this.checkBox1.Text = "压缩数据";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 71);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "压缩后大小";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(15, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "原始大小";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "节点名";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.textBox4);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(308, 190);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "附加数据";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // textBox4
            // 
            this.textBox4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox4.Location = new System.Drawing.Point(3, 3);
            this.textBox4.Multiline = true;
            this.textBox4.Name = "textBox4";
            this.textBox4.ReadOnly = true;
            this.textBox4.Size = new System.Drawing.Size(302, 184);
            this.textBox4.TabIndex = 0;
            // 
            // toolStrip_Data
            // 
            this.toolStrip_Data.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_Import,
            this.toolStripButton_Export,
            this.toolStripButton_Prop,
            this.toolStripSeparator2,
            this.toolStripTextBox_Path});
            this.toolStrip_Data.Location = new System.Drawing.Point(0, 0);
            this.toolStrip_Data.Name = "toolStrip_Data";
            this.toolStrip_Data.Size = new System.Drawing.Size(322, 25);
            this.toolStrip_Data.TabIndex = 0;
            this.toolStrip_Data.Text = "toolStrip1";
            // 
            // toolStripButton_Import
            // 
            this.toolStripButton_Import.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_Import.Image = global::FCYResPackageEditor.Properties.Resources.ICON_IMPORT;
            this.toolStripButton_Import.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_Import.Name = "toolStripButton_Import";
            this.toolStripButton_Import.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_Import.Text = "导入";
            this.toolStripButton_Import.Click += new System.EventHandler(this.toolStripButton_Import_Click);
            // 
            // toolStripButton_Export
            // 
            this.toolStripButton_Export.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_Export.Image = global::FCYResPackageEditor.Properties.Resources.ICON_EXPORT;
            this.toolStripButton_Export.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_Export.Name = "toolStripButton_Export";
            this.toolStripButton_Export.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_Export.Text = "导出";
            this.toolStripButton_Export.Click += new System.EventHandler(this.toolStripButton_Export_Click);
            // 
            // toolStripButton_Prop
            // 
            this.toolStripButton_Prop.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_Prop.Image = global::FCYResPackageEditor.Properties.Resources.ICON_PROPERTY;
            this.toolStripButton_Prop.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_Prop.Name = "toolStripButton_Prop";
            this.toolStripButton_Prop.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_Prop.Text = "属性";
            this.toolStripButton_Prop.Click += new System.EventHandler(this.toolStripButton_Prop_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripTextBox_Path
            // 
            this.toolStripTextBox_Path.Name = "toolStripTextBox_Path";
            this.toolStripTextBox_Path.ReadOnly = true;
            this.toolStripTextBox_Path.Size = new System.Drawing.Size(200, 25);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "*.frp";
            this.openFileDialog1.Filter = "fancy资源包(*.frp)|*.frp";
            this.openFileDialog1.Title = "打开资源包";
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.Filter = "fancy资源包(*.frp)|*.frp";
            this.saveFileDialog1.Title = "保存资源包";
            // 
            // openFileDialog2
            // 
            this.openFileDialog2.FileName = "*.*";
            this.openFileDialog2.Filter = "所有文件|*.*";
            this.openFileDialog2.Title = "打开文件";
            // 
            // saveFileDialog2
            // 
            this.saveFileDialog2.Filter = "所有文件|*.*";
            this.saveFileDialog2.Title = "保存文件";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(541, 268);
            this.Controls.Add(this.splitContainer1);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "fancy资源包编辑器";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            this.splitContainer1.ResumeLayout(false);
            this.toolStrip_Main.ResumeLayout(false);
            this.toolStrip_Main.PerformLayout();
            this.tabControl_Data.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.toolStrip_Data.ResumeLayout(false);
            this.toolStrip_Data.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ToolStrip toolStrip_Main;
        private System.Windows.Forms.TreeView treeView_Main;
        private System.Windows.Forms.ToolStripButton toolStripButton_New;
        private System.Windows.Forms.ToolStripButton toolStripButton_Open;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripButton_Remove;
        private System.Windows.Forms.ImageList imageList_Main;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton_Add;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_CreateStream;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_CreateFloder;
        private System.Windows.Forms.TabControl tabControl_Data;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.ToolStrip toolStrip_Data;
        private System.Windows.Forms.ToolStripButton toolStripButton_Import;
        private System.Windows.Forms.ToolStripButton toolStripButton_Export;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox_Path;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        private System.Windows.Forms.ToolStripButton toolStripButton_Save;
        private System.Windows.Forms.ToolStripButton toolStripButton_Prop;
        private System.Windows.Forms.OpenFileDialog openFileDialog2;
        private System.Windows.Forms.SaveFileDialog saveFileDialog2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolStripButton_ImportFloder;
        private System.Windows.Forms.ToolStripButton toolStripButton_ExportFloder;

    }
}

