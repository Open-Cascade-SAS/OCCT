using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace IE_WinForms
{
  /// <summary>
  /// Summary description for AboutDialog.
  /// </summary>
  public class AboutDialog : System.Windows.Forms.Form
  {
    private System.Windows.Forms.PictureBox pictureBox1;
    private System.Windows.Forms.Button button1;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label myVersion;
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    public AboutDialog()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();
      //
      // Create OCCT proxy object and get OCCT version
      //
      OCCTProxy t = new OCCTProxy();
      t.InitOCCTProxy();
      float version = t.GetOCCVersion();
      this.myVersion.Text = this.myVersion.Text + version;
    }

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutDialog));
      this.pictureBox1 = new System.Windows.Forms.PictureBox();
      this.button1 = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.myVersion = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.label4 = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
      this.SuspendLayout();
      // 
      // pictureBox1
      // 
      this.pictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
      this.pictureBox1.Location = new System.Drawing.Point(59, 64);
      this.pictureBox1.Name = "pictureBox1";
      this.pictureBox1.Size = new System.Drawing.Size(196, 102);
      this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
      this.pictureBox1.TabIndex = 0;
      this.pictureBox1.TabStop = false;
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(96, 248);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(128, 24);
      this.button1.TabIndex = 1;
      this.button1.Text = "OK";
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(16, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(288, 24);
      this.label1.TabIndex = 2;
      this.label1.Text = "Import/Export Sample,";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // myVersion
      // 
      this.myVersion.Location = new System.Drawing.Point(16, 32);
      this.myVersion.Name = "myVersion";
      this.myVersion.Size = new System.Drawing.Size(288, 16);
      this.myVersion.TabIndex = 3;
      this.myVersion.Text = "Open CASCADE Technology ";
      this.myVersion.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(24, 168);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(280, 23);
      this.label3.TabIndex = 4;
      this.label3.Text = "Copyright (C) 2004-2013, Open CASCADE S.A.S";
      this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // label4
      // 
      this.label4.Location = new System.Drawing.Point(8, 200);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(296, 24);
      this.label4.TabIndex = 5;
      this.label4.Text = "http://www.opencascade.com";
      this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
      // 
      // AboutDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(312, 285);
      this.ControlBox = false;
      this.Controls.Add(this.label4);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.myVersion);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.button1);
      this.Controls.Add(this.pictureBox1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "AboutDialog";
      this.Text = "About Import/Export Sample";
      ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
    #endregion

    private void button1_Click(object sender, System.EventArgs e)
    {
      this.Close();
    }
  }
}
