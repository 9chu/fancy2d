namespace FMMMetadataEditor
{
    partial class SubsetEditor
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
            this.toolStrip_main = new System.Windows.Forms.ToolStrip();
            this.toolStripButton_ok = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_cancel = new System.Windows.Forms.ToolStripButton();
            this.propertyGrid_main = new System.Windows.Forms.PropertyGrid();
            this.toolStrip_main.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip_main
            // 
            this.toolStrip_main.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_ok,
            this.toolStripButton_cancel});
            this.toolStrip_main.Location = new System.Drawing.Point(0, 0);
            this.toolStrip_main.Name = "toolStrip_main";
            this.toolStrip_main.Size = new System.Drawing.Size(269, 25);
            this.toolStrip_main.TabIndex = 0;
            this.toolStrip_main.Text = "toolStrip1";
            this.toolStrip_main.Visible = false;
            // 
            // toolStripButton_ok
            // 
            this.toolStripButton_ok.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_ok.Image = global::FMMMetadataEditor.Properties.Resources.ICON_OK;
            this.toolStripButton_ok.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_ok.Name = "toolStripButton_ok";
            this.toolStripButton_ok.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_ok.Text = "保存修改";
            this.toolStripButton_ok.Click += new System.EventHandler(this.toolStripButton_ok_Click);
            // 
            // toolStripButton_cancel
            // 
            this.toolStripButton_cancel.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_cancel.Image = global::FMMMetadataEditor.Properties.Resources.ICON_CANCEL;
            this.toolStripButton_cancel.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_cancel.Name = "toolStripButton_cancel";
            this.toolStripButton_cancel.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_cancel.Text = "取消修改";
            this.toolStripButton_cancel.Click += new System.EventHandler(this.toolStripButton_cancel_Click);
            // 
            // propertyGrid_main
            // 
            this.propertyGrid_main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid_main.Location = new System.Drawing.Point(0, 25);
            this.propertyGrid_main.Name = "propertyGrid_main";
            this.propertyGrid_main.Size = new System.Drawing.Size(269, 256);
            this.propertyGrid_main.TabIndex = 1;
            this.propertyGrid_main.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid_main_PropertyValueChanged);
            // 
            // SubsetEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(269, 281);
            this.Controls.Add(this.propertyGrid_main);
            this.Controls.Add(this.toolStrip_main);
            this.Name = "SubsetEditor";
            this.Text = "子集属性编辑器";
            this.Load += new System.EventHandler(this.SubsetEditor_Load);
            this.toolStrip_main.ResumeLayout(false);
            this.toolStrip_main.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip_main;
        private System.Windows.Forms.ToolStripButton toolStripButton_ok;
        private System.Windows.Forms.ToolStripButton toolStripButton_cancel;
        private System.Windows.Forms.PropertyGrid propertyGrid_main;

    }
}