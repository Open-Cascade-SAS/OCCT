using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace IE_WPF_D3D
{
  /// <summary>
  /// Summary description for TransparencyDialog.
  /// </summary>
  public class TransparencyDialog : System.Windows.Forms.Form
  {
    private System.Windows.Forms.NumericUpDown MyTransparency;
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;
    private OCCTProxyD3D myView;

    public TransparencyDialog()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      //
      // TODO: Add any constructor code after InitializeComponent call
      //
      myView = null;
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
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(TransparencyDialog));
      this.MyTransparency = new System.Windows.Forms.NumericUpDown();
      ((System.ComponentModel.ISupportInitialize)(this.MyTransparency)).BeginInit();
      this.SuspendLayout();
      // 
      // MyTransparency
      // 
      this.MyTransparency.Location = new System.Drawing.Point(16, 16);
      this.MyTransparency.Maximum = new System.Decimal(new int[] {
																		   10,
																		   0,
																		   0,
																		   0});
      this.MyTransparency.Name = "MyTransparency";
      this.MyTransparency.Size = new System.Drawing.Size(96, 20);
      this.MyTransparency.TabIndex = 0;
      this.MyTransparency.ValueChanged += new System.EventHandler(this.MyTransparency_ValueChanged);
      // 
      // TransparencyDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(128, 53);
      this.Controls.Add(this.MyTransparency);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "TransparencyDialog";
      this.Text = "TransparencyDialog";
      ((System.ComponentModel.ISupportInitialize)(this.MyTransparency)).EndInit();
      this.ResumeLayout(false);

    }
    #endregion

    private void MyTransparency_ValueChanged(object sender, System.EventArgs e)
    {
      if (this.myView == null)
        return;
      int transp = (int)this.MyTransparency.Value;
      this.myView.SetTransparency(transp);
    }

    public OCCTProxyD3D View
    {
      set
      {
        this.myView = value;
      }
    }

  }
}
