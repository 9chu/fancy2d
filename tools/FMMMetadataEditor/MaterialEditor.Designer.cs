namespace FMMMetadataEditor
{
    partial class MaterialEditor
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
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripTextBox_name = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripButton_ok = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_cancel = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButton_change = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_restore = new System.Windows.Forms.ToolStripButton();
            this.textBox_content = new System.Windows.Forms.TextBox();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripLabel1,
            this.toolStripTextBox_name,
            this.toolStripButton_ok,
            this.toolStripButton_cancel,
            this.toolStripSeparator1,
            this.toolStripButton_change,
            this.toolStripButton_restore});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(284, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(56, 22);
            this.toolStripLabel1.Text = "材质名称";
            // 
            // toolStripTextBox_name
            // 
            this.toolStripTextBox_name.Name = "toolStripTextBox_name";
            this.toolStripTextBox_name.Size = new System.Drawing.Size(100, 25);
            this.toolStripTextBox_name.KeyDown += new System.Windows.Forms.KeyEventHandler(this.toolStripTextBox_name_KeyDown);
            this.toolStripTextBox_name.TextChanged += new System.EventHandler(this.toolStripTextBox_name_TextChanged);
            // 
            // toolStripButton_ok
            // 
            this.toolStripButton_ok.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_ok.Image = global::FMMMetadataEditor.Properties.Resources.ICON_OK;
            this.toolStripButton_ok.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_ok.Name = "toolStripButton_ok";
            this.toolStripButton_ok.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_ok.Text = "是";
            this.toolStripButton_ok.Visible = false;
            this.toolStripButton_ok.Click += new System.EventHandler(this.toolStripButton_ok_Click);
            // 
            // toolStripButton_cancel
            // 
            this.toolStripButton_cancel.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_cancel.Image = global::FMMMetadataEditor.Properties.Resources.ICON_CANCEL;
            this.toolStripButton_cancel.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_cancel.Name = "toolStripButton_cancel";
            this.toolStripButton_cancel.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_cancel.Text = "否";
            this.toolStripButton_cancel.Visible = false;
            this.toolStripButton_cancel.Click += new System.EventHandler(this.toolStripButton_cancel_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripButton_change
            // 
            this.toolStripButton_change.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_change.Image = global::FMMMetadataEditor.Properties.Resources.ICON_OK;
            this.toolStripButton_change.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_change.Name = "toolStripButton_change";
            this.toolStripButton_change.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_change.Text = "保存修改";
            this.toolStripButton_change.Visible = false;
            this.toolStripButton_change.Click += new System.EventHandler(this.toolStripButton_change_Click);
            // 
            // toolStripButton_restore
            // 
            this.toolStripButton_restore.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_restore.Image = global::FMMMetadataEditor.Properties.Resources.ICON_CANCEL;
            this.toolStripButton_restore.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_restore.Name = "toolStripButton_restore";
            this.toolStripButton_restore.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_restore.Text = "恢复修改";
            this.toolStripButton_restore.Visible = false;
            this.toolStripButton_restore.Click += new System.EventHandler(this.toolStripButton_restore_Click);
            // 
            // textBox_content
            // 
            this.textBox_content.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox_content.Location = new System.Drawing.Point(0, 25);
            this.textBox_content.Multiline = true;
            this.textBox_content.Name = "textBox_content";
            this.textBox_content.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_content.Size = new System.Drawing.Size(284, 237);
            this.textBox_content.TabIndex = 1;
            this.textBox_content.TextChanged += new System.EventHandler(this.textBox_content_TextChanged);
            // 
            // MaterialEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.textBox_content);
            this.Controls.Add(this.toolStrip1);
            this.Name = "MaterialEditor";
            this.Text = "材质属性编辑器";
            this.Load += new System.EventHandler(this.MaterialEditor_Load);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox_name;
        private System.Windows.Forms.ToolStripButton toolStripButton_ok;
        private System.Windows.Forms.ToolStripButton toolStripButton_cancel;
        private System.Windows.Forms.TextBox textBox_content;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripButton_change;
        private System.Windows.Forms.ToolStripButton toolStripButton_restore;

    }
}