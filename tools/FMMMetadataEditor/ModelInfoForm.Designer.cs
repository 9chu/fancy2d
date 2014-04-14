namespace FMMMetadataEditor
{
    partial class ModelInfoForm
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
            this.propertyGrid_main = new System.Windows.Forms.PropertyGrid();
            this.SuspendLayout();
            // 
            // propertyGrid_main
            // 
            this.propertyGrid_main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid_main.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid_main.Name = "propertyGrid_main";
            this.propertyGrid_main.Size = new System.Drawing.Size(284, 262);
            this.propertyGrid_main.TabIndex = 0;
            // 
            // ModelInfoForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.propertyGrid_main);
            this.Name = "ModelInfoForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "模型属性";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ModelInfoForm_FormClosing);
            this.Load += new System.EventHandler(this.ModelInfoForm_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid_main;
    }
}