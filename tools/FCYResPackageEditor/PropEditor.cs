using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FancyLib;

namespace FCYResPackageEditor
{
    public partial class PropEditor : Form
    {
        static public bool OpenPropEditorForm(FCYResPackageDataNode tNode)
        {
            PropEditor tForm = new PropEditor(tNode.IsCompressed, tNode.AdditionData, tNode.Name);

            if (tForm.ShowDialog() == DialogResult.Cancel)
            {
                tForm.Dispose();
                return false;
            }
            else
            {
                tNode.IsCompressed = tForm.checkBox1.Checked;
                tNode.AdditionData = tForm.textBox1.Text;
                try
                {
                    if(tNode.Name != tForm.textBox2.Text)
                        tNode.Name = tForm.textBox2.Text;
                }
                catch(FCYExcpNameErr)
                {
                    MessageBox.Show("修改节点名称失败。可能存在同名节点或者节点名称不合法。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

                tForm.Dispose();
                return true;
            }
        }

        static public bool OpenPropEditorForm(FCYResPackageFloderNode tNode)
        {
            PropEditor tForm = new PropEditor(tNode.Name);

            if (tForm.ShowDialog() == DialogResult.Cancel)
            {
                tForm.Dispose();
                return false;
            }
            else
            {
                try
                {
                    if (tNode.Name != tForm.textBox2.Text)
                        tNode.Name = tForm.textBox2.Text;
                }
                catch (FCYExcpNameErr)
                {
                    MessageBox.Show("修改节点名称失败。可能存在同名节点或者节点名称不合法。", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }

                tForm.Dispose();
                return true;
            }
        }

        public PropEditor(bool Compress, string Content, string NodeName)
        {
            InitializeComponent();

            checkBox1.Checked = Compress;
            textBox1.Text = Content;
            textBox2.Text = NodeName;
        }

        public PropEditor(string NodeName)
        {
            InitializeComponent();

            checkBox1.Enabled = false;
            textBox1.Enabled = false;
            textBox2.Text = NodeName;
        }

        private void PropEditor_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Close();
        }
    }
}
