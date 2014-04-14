namespace FMMMetadataEditor
{
    partial class UserDataEditor
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
            this.button_export = new System.Windows.Forms.Button();
            this.button_import = new System.Windows.Forms.Button();
            this.openFileDialog_userData = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog_userData = new System.Windows.Forms.SaveFileDialog();
            this.SuspendLayout();
            // 
            // button_export
            // 
            this.button_export.Location = new System.Drawing.Point(12, 12);
            this.button_export.Name = "button_export";
            this.button_export.Size = new System.Drawing.Size(98, 23);
            this.button_export.TabIndex = 0;
            this.button_export.Text = "导出用户数据";
            this.button_export.UseVisualStyleBackColor = true;
            this.button_export.Click += new System.EventHandler(this.button_export_Click);
            // 
            // button_import
            // 
            this.button_import.Location = new System.Drawing.Point(116, 12);
            this.button_import.Name = "button_import";
            this.button_import.Size = new System.Drawing.Size(98, 23);
            this.button_import.TabIndex = 1;
            this.button_import.Text = "导入用户数据";
            this.button_import.UseVisualStyleBackColor = true;
            this.button_import.Click += new System.EventHandler(this.button_import_Click);
            // 
            // openFileDialog_userData
            // 
            this.openFileDialog_userData.Filter = "所有文件(*.*)|*.*";
            this.openFileDialog_userData.Title = "导入用户数据";
            // 
            // saveFileDialog_userData
            // 
            this.saveFileDialog_userData.Filter = "所有文件(*.*)|*.*";
            this.saveFileDialog_userData.Title = "导出用户数据";
            // 
            // UserDataEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(261, 93);
            this.Controls.Add(this.button_import);
            this.Controls.Add(this.button_export);
            this.Name = "UserDataEditor";
            this.Text = "用户数据编辑器";
            this.Load += new System.EventHandler(this.UserDataEditor_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button_export;
        private System.Windows.Forms.Button button_import;
        private System.Windows.Forms.OpenFileDialog openFileDialog_userData;
        private System.Windows.Forms.SaveFileDialog saveFileDialog_userData;
    }
}