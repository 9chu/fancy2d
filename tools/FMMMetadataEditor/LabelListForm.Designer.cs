namespace FMMMetadataEditor
{
    partial class LabelListForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.treeView_labels = new System.Windows.Forms.TreeView();
            this.toolStrip_labels = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripDropDownButton_new = new System.Windows.Forms.ToolStripDropDownButton();
            this.ToolStripMenuItem_newVertex = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_newIndex = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_newSubset = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_newMat = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_userData = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripButton_remove = new System.Windows.Forms.ToolStripButton();
            this.toolStrip_labels.SuspendLayout();
            this.SuspendLayout();
            // 
            // treeView_labels
            // 
            this.treeView_labels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView_labels.Location = new System.Drawing.Point(0, 25);
            this.treeView_labels.Name = "treeView_labels";
            this.treeView_labels.Size = new System.Drawing.Size(284, 237);
            this.treeView_labels.TabIndex = 6;
            this.treeView_labels.DoubleClick += new System.EventHandler(this.treeView_labels_DoubleClick);
            // 
            // toolStrip_labels
            // 
            this.toolStrip_labels.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.toolStripDropDownButton_new,
            this.toolStripButton_remove});
            this.toolStrip_labels.Location = new System.Drawing.Point(0, 0);
            this.toolStrip_labels.Name = "toolStrip_labels";
            this.toolStrip_labels.Size = new System.Drawing.Size(284, 25);
            this.toolStrip_labels.TabIndex = 5;
            this.toolStrip_labels.Text = "toolStrip1";
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(56, 22);
            this.toolStripLabel1.Text = "标签列表";
            // 
            // toolStripDropDownButton_new
            // 
            this.toolStripDropDownButton_new.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripDropDownButton_new.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_newVertex,
            this.ToolStripMenuItem_newIndex,
            this.ToolStripMenuItem_newSubset,
            this.ToolStripMenuItem_newMat,
            this.ToolStripMenuItem_userData});
            this.toolStripDropDownButton_new.Image = global::FMMMetadataEditor.Properties.Resources.ICON_ADD;
            this.toolStripDropDownButton_new.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripDropDownButton_new.Name = "toolStripDropDownButton_new";
            this.toolStripDropDownButton_new.Size = new System.Drawing.Size(29, 22);
            this.toolStripDropDownButton_new.Text = "新建标签";
            // 
            // ToolStripMenuItem_newVertex
            // 
            this.ToolStripMenuItem_newVertex.Name = "ToolStripMenuItem_newVertex";
            this.ToolStripMenuItem_newVertex.Size = new System.Drawing.Size(152, 22);
            this.ToolStripMenuItem_newVertex.Text = "顶点数据块";
            this.ToolStripMenuItem_newVertex.Visible = false;
            this.ToolStripMenuItem_newVertex.Click += new System.EventHandler(this.ToolStripMenuItem_newVertex_Click);
            // 
            // ToolStripMenuItem_newIndex
            // 
            this.ToolStripMenuItem_newIndex.Name = "ToolStripMenuItem_newIndex";
            this.ToolStripMenuItem_newIndex.Size = new System.Drawing.Size(152, 22);
            this.ToolStripMenuItem_newIndex.Text = "索引数据块";
            this.ToolStripMenuItem_newIndex.Visible = false;
            this.ToolStripMenuItem_newIndex.Click += new System.EventHandler(this.ToolStripMenuItem_newIndex_Click);
            // 
            // ToolStripMenuItem_newSubset
            // 
            this.ToolStripMenuItem_newSubset.Name = "ToolStripMenuItem_newSubset";
            this.ToolStripMenuItem_newSubset.Size = new System.Drawing.Size(152, 22);
            this.ToolStripMenuItem_newSubset.Text = "子集数据块";
            this.ToolStripMenuItem_newSubset.Click += new System.EventHandler(this.ToolStripMenuItem_newSubset_Click);
            // 
            // ToolStripMenuItem_newMat
            // 
            this.ToolStripMenuItem_newMat.Name = "ToolStripMenuItem_newMat";
            this.ToolStripMenuItem_newMat.Size = new System.Drawing.Size(152, 22);
            this.ToolStripMenuItem_newMat.Text = "材质数据块";
            this.ToolStripMenuItem_newMat.Click += new System.EventHandler(this.ToolStripMenuItem_newMat_Click);
            // 
            // ToolStripMenuItem_userData
            // 
            this.ToolStripMenuItem_userData.Name = "ToolStripMenuItem_userData";
            this.ToolStripMenuItem_userData.Size = new System.Drawing.Size(152, 22);
            this.ToolStripMenuItem_userData.Text = "自定义数据";
            this.ToolStripMenuItem_userData.Click += new System.EventHandler(this.ToolStripMenuItem_userData_Click);
            // 
            // toolStripButton_remove
            // 
            this.toolStripButton_remove.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_remove.Image = global::FMMMetadataEditor.Properties.Resources.ICON_DEC;
            this.toolStripButton_remove.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_remove.Name = "toolStripButton_remove";
            this.toolStripButton_remove.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_remove.Text = "删除标签";
            this.toolStripButton_remove.Click += new System.EventHandler(this.toolStripButton_remove_Click);
            // 
            // LabelListForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.CloseButton = false;
            this.CloseButtonVisible = false;
            this.Controls.Add(this.treeView_labels);
            this.Controls.Add(this.toolStrip_labels);
            this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft)));
            this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.HideOnClose = true;
            this.Name = "LabelListForm";
            this.Text = "标签列表";
            this.Load += new System.EventHandler(this.LabelListForm_Load);
            this.toolStrip_labels.ResumeLayout(false);
            this.toolStrip_labels.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView treeView_labels;
        private System.Windows.Forms.ToolStrip toolStrip_labels;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripDropDownButton toolStripDropDownButton_new;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_newVertex;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_newIndex;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_newSubset;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_newMat;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_userData;
        private System.Windows.Forms.ToolStripButton toolStripButton_remove;
    }
}