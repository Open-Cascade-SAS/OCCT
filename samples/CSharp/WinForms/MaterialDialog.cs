using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace IE_WinForms
{
  /// <summary>
  /// Summary description for MaterialDialog.
  /// </summary>
  public class MaterialDialog : System.Windows.Forms.Form
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    public MaterialDialog()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      //
      // TODO: Add any constructor code after InitializeComponent call
      //
      this.myView = null;
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
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MaterialDialog));
      this.BTN_PLASTER = new System.Windows.Forms.Button();
      this.BTN_BRASS = new System.Windows.Forms.Button();
      this.BTN_BRONZE = new System.Windows.Forms.Button();
      this.BTN_COPPER = new System.Windows.Forms.Button();
      this.BTN_GOLD = new System.Windows.Forms.Button();
      this.BTN_PEWTER = new System.Windows.Forms.Button();
      this.BTN_PLASTIC = new System.Windows.Forms.Button();
      this.BTN_SILVER = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // BTN_PLASTER
      // 
      this.BTN_PLASTER.Location = new System.Drawing.Point(16, 8);
      this.BTN_PLASTER.Name = "BTN_PLASTER";
      this.BTN_PLASTER.Size = new System.Drawing.Size(80, 24);
      this.BTN_PLASTER.TabIndex = 0;
      this.BTN_PLASTER.Text = "Plaster";
      this.BTN_PLASTER.Click += new System.EventHandler(this.button1_Click);
      // 
      // BTN_BRASS
      // 
      this.BTN_BRASS.Location = new System.Drawing.Point(16, 40);
      this.BTN_BRASS.Name = "BTN_BRASS";
      this.BTN_BRASS.Size = new System.Drawing.Size(80, 24);
      this.BTN_BRASS.TabIndex = 1;
      this.BTN_BRASS.Text = "Brass";
      this.BTN_BRASS.Click += new System.EventHandler(this.BTN_BRASS_Click);
      // 
      // BTN_BRONZE
      // 
      this.BTN_BRONZE.Location = new System.Drawing.Point(16, 72);
      this.BTN_BRONZE.Name = "BTN_BRONZE";
      this.BTN_BRONZE.Size = new System.Drawing.Size(80, 24);
      this.BTN_BRONZE.TabIndex = 2;
      this.BTN_BRONZE.Text = "Bronze";
      this.BTN_BRONZE.Click += new System.EventHandler(this.BTN_BRONZE_Click);
      // 
      // BTN_COPPER
      // 
      this.BTN_COPPER.Location = new System.Drawing.Point(16, 104);
      this.BTN_COPPER.Name = "BTN_COPPER";
      this.BTN_COPPER.Size = new System.Drawing.Size(80, 24);
      this.BTN_COPPER.TabIndex = 3;
      this.BTN_COPPER.Text = "Copper";
      this.BTN_COPPER.Click += new System.EventHandler(this.BTN_COPPER_Click);
      // 
      // BTN_GOLD
      // 
      this.BTN_GOLD.Location = new System.Drawing.Point(16, 136);
      this.BTN_GOLD.Name = "BTN_GOLD";
      this.BTN_GOLD.Size = new System.Drawing.Size(80, 24);
      this.BTN_GOLD.TabIndex = 4;
      this.BTN_GOLD.Text = "Gold";
      this.BTN_GOLD.Click += new System.EventHandler(this.BTN_GOLD_Click);
      // 
      // BTN_PEWTER
      // 
      this.BTN_PEWTER.Location = new System.Drawing.Point(16, 168);
      this.BTN_PEWTER.Name = "BTN_PEWTER";
      this.BTN_PEWTER.Size = new System.Drawing.Size(80, 24);
      this.BTN_PEWTER.TabIndex = 5;
      this.BTN_PEWTER.Text = "Pewter";
      this.BTN_PEWTER.Click += new System.EventHandler(this.BTN_PEWTER_Click);
      // 
      // BTN_PLASTIC
      // 
      this.BTN_PLASTIC.Location = new System.Drawing.Point(16, 200);
      this.BTN_PLASTIC.Name = "BTN_PLASTIC";
      this.BTN_PLASTIC.Size = new System.Drawing.Size(80, 24);
      this.BTN_PLASTIC.TabIndex = 6;
      this.BTN_PLASTIC.Text = "Plastic";
      this.BTN_PLASTIC.Click += new System.EventHandler(this.BTN_PLASTIC_Click);
      // 
      // BTN_SILVER
      // 
      this.BTN_SILVER.Location = new System.Drawing.Point(16, 232);
      this.BTN_SILVER.Name = "BTN_SILVER";
      this.BTN_SILVER.Size = new System.Drawing.Size(80, 24);
      this.BTN_SILVER.TabIndex = 7;
      this.BTN_SILVER.Text = "Silver";
      this.BTN_SILVER.Click += new System.EventHandler(this.BTN_SILVER_Click);
      // 
      // MaterialDialog
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(112, 273);
      this.Controls.Add(this.BTN_SILVER);
      this.Controls.Add(this.BTN_PLASTIC);
      this.Controls.Add(this.BTN_PEWTER);
      this.Controls.Add(this.BTN_GOLD);
      this.Controls.Add(this.BTN_COPPER);
      this.Controls.Add(this.BTN_BRONZE);
      this.Controls.Add(this.BTN_BRASS);
      this.Controls.Add(this.BTN_PLASTER);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "MaterialDialog";
      this.Text = "MaterialDialog";
      this.ResumeLayout(false);

    }
    #endregion

    private System.Windows.Forms.Button BTN_PLASTER;
    private System.Windows.Forms.Button BTN_BRASS;
    private System.Windows.Forms.Button BTN_BRONZE;
    private System.Windows.Forms.Button BTN_COPPER;
    private System.Windows.Forms.Button BTN_GOLD;
    private System.Windows.Forms.Button BTN_PEWTER;
    private System.Windows.Forms.Button BTN_PLASTIC;
    private System.Windows.Forms.Button BTN_SILVER;

    private int myMaterial;
    private OCCTProxy myView;

    private void button1_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 5;
      ChangeMaterial();
    }

    private void BTN_BRASS_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 0;
      ChangeMaterial();
    }

    private void BTN_BRONZE_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 1;
      ChangeMaterial();
    }

    private void BTN_COPPER_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 2;
      ChangeMaterial();
    }

    private void BTN_GOLD_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 3;
      ChangeMaterial();
    }

    private void BTN_PEWTER_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 4;
      ChangeMaterial();
    }

    private void BTN_PLASTIC_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 6;
      ChangeMaterial();
    }

    private void BTN_SILVER_Click(object sender, System.EventArgs e)
    {
      this.myMaterial = 7;
      ChangeMaterial();
    }

    public int Material
    {
      get
      {
        return this.myMaterial;
      }
      set
      {
        this.myMaterial = value;
      }
    }

    public void ChangeMaterial()
    {
      if (myView == null)
        return;
      myView.SetMaterial(this.myMaterial);

    }

    public OCCTProxy View
    {
      set
      {
        this.myView = value;
      }
    }

  }
}
