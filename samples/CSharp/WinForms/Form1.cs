using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace IE_WinForms
{
  /// <summary>
  /// Summary description for Form1.
  /// </summary>
  public class Form1 : System.Windows.Forms.Form
  {
    private System.Windows.Forms.MainMenu mainMenu1;
    private System.Windows.Forms.MenuItem FileNew;
    private System.Windows.Forms.MenuItem FileClose;
    private System.Windows.Forms.MenuItem FileExport;
    private System.Windows.Forms.MenuItem FileImport;
    private System.Windows.Forms.MenuItem ImportBRep;
    private System.Windows.Forms.MenuItem ExportImage;
    private System.Windows.Forms.MenuItem File;
    private System.Windows.Forms.MenuItem Window;
    private System.Windows.Forms.ToolBarButton New;
    private System.Windows.Forms.ImageList imageList1;
    private System.Windows.Forms.ToolBarButton About;
    private System.Windows.Forms.StatusBar myStatusBar;
    private System.Windows.Forms.MenuItem ImportIges;
    private System.Windows.Forms.MenuItem ImportStep;
    private System.Windows.Forms.MenuItem ExportBRep;
    private System.Windows.Forms.MenuItem ExportIges;
    private System.Windows.Forms.MenuItem ExportStep;
    private System.Windows.Forms.MenuItem ExportStl;
    private System.Windows.Forms.MenuItem ExportVrml;
    private System.ComponentModel.IContainer components;
    private System.Windows.Forms.MenuItem menuItem1;
    private System.Windows.Forms.MenuItem menuItem2;
    private System.Windows.Forms.MenuItem menuItem3;
    private System.Windows.Forms.MenuItem View;
    private System.Windows.Forms.MenuItem Help;
    private System.Windows.Forms.MenuItem HelpAbout;
    private System.Windows.Forms.MenuItem ViewToolbar;
    private System.Windows.Forms.MenuItem ViewStatusBar;
    private System.Windows.Forms.MenuItem menuItem4;
    private System.Windows.Forms.MenuItem WindowCascade;
    private System.Windows.Forms.MenuItem WindowTile;

    protected IE_WinForms.ModelFormat myModelFormat;
    private System.Windows.Forms.ToolBarButton wireframe;
    private System.Windows.Forms.ToolBarButton shading;
    private System.Windows.Forms.ToolBarButton toolBarButton1;
    private System.Windows.Forms.ToolBarButton color;
    private System.Windows.Forms.ToolBarButton transparency;
    private System.Windows.Forms.ToolBarButton delete;
    private System.Windows.Forms.ToolBarButton material;
    private System.Windows.Forms.ToolBarButton ZoomAll;
    private System.Windows.Forms.ToolBarButton ZoomWin;
    private System.Windows.Forms.ToolBarButton ZoomProg;
    private System.Windows.Forms.ToolBarButton Pan;
    private System.Windows.Forms.ToolBarButton PanGlo;
    private System.Windows.Forms.ToolBarButton Front;
    private System.Windows.Forms.ToolBarButton Back;
    private System.Windows.Forms.ToolBarButton TOP;
    private System.Windows.Forms.ToolBarButton BOTTOM;
    private System.Windows.Forms.ToolBarButton RIGHT;
    private System.Windows.Forms.ToolBarButton LEFT;
    private System.Windows.Forms.ToolBarButton Axo;
    private System.Windows.Forms.ToolBarButton Rot;
    private System.Windows.Forms.ToolBarButton Reset;
    private System.Windows.Forms.ToolBarButton HlrOn;
    private System.Windows.Forms.ToolBarButton HlrOff;
    private System.Windows.Forms.ToolBar toolBarTool;
    private System.Windows.Forms.ToolBar toolBarView;
    protected static int myNbOfChildren;

    public Form1()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      //
      IE_WinForms.Form1.myNbOfChildren = 0;
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
      this.components = new System.ComponentModel.Container();
      System.Configuration.AppSettingsReader configurationAppSettings = new System.Configuration.AppSettingsReader();
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Form1));
      this.mainMenu1 = new System.Windows.Forms.MainMenu();
      this.File = new System.Windows.Forms.MenuItem();
      this.FileNew = new System.Windows.Forms.MenuItem();
      this.FileExport = new System.Windows.Forms.MenuItem();
      this.ImportBRep = new System.Windows.Forms.MenuItem();
      this.ImportIges = new System.Windows.Forms.MenuItem();
      this.ImportStep = new System.Windows.Forms.MenuItem();
      this.FileImport = new System.Windows.Forms.MenuItem();
      this.ExportBRep = new System.Windows.Forms.MenuItem();
      this.ExportIges = new System.Windows.Forms.MenuItem();
      this.ExportStep = new System.Windows.Forms.MenuItem();
      this.ExportStl = new System.Windows.Forms.MenuItem();
      this.ExportVrml = new System.Windows.Forms.MenuItem();
      this.menuItem3 = new System.Windows.Forms.MenuItem();
      this.ExportImage = new System.Windows.Forms.MenuItem();
      this.FileClose = new System.Windows.Forms.MenuItem();
      this.menuItem2 = new System.Windows.Forms.MenuItem();
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.View = new System.Windows.Forms.MenuItem();
      this.ViewToolbar = new System.Windows.Forms.MenuItem();
      this.ViewStatusBar = new System.Windows.Forms.MenuItem();
      this.Window = new System.Windows.Forms.MenuItem();
      this.menuItem4 = new System.Windows.Forms.MenuItem();
      this.WindowCascade = new System.Windows.Forms.MenuItem();
      this.WindowTile = new System.Windows.Forms.MenuItem();
      this.Help = new System.Windows.Forms.MenuItem();
      this.HelpAbout = new System.Windows.Forms.MenuItem();
      this.toolBarTool = new System.Windows.Forms.ToolBar();
      this.New = new System.Windows.Forms.ToolBarButton();
      this.About = new System.Windows.Forms.ToolBarButton();
      this.toolBarButton1 = new System.Windows.Forms.ToolBarButton();
      this.wireframe = new System.Windows.Forms.ToolBarButton();
      this.shading = new System.Windows.Forms.ToolBarButton();
      this.color = new System.Windows.Forms.ToolBarButton();
      this.material = new System.Windows.Forms.ToolBarButton();
      this.transparency = new System.Windows.Forms.ToolBarButton();
      this.delete = new System.Windows.Forms.ToolBarButton();
      this.imageList1 = new System.Windows.Forms.ImageList(this.components);
      this.myStatusBar = new System.Windows.Forms.StatusBar();
      this.toolBarView = new System.Windows.Forms.ToolBar();
      this.ZoomAll = new System.Windows.Forms.ToolBarButton();
      this.ZoomWin = new System.Windows.Forms.ToolBarButton();
      this.ZoomProg = new System.Windows.Forms.ToolBarButton();
      this.Pan = new System.Windows.Forms.ToolBarButton();
      this.PanGlo = new System.Windows.Forms.ToolBarButton();
      this.Front = new System.Windows.Forms.ToolBarButton();
      this.Back = new System.Windows.Forms.ToolBarButton();
      this.TOP = new System.Windows.Forms.ToolBarButton();
      this.BOTTOM = new System.Windows.Forms.ToolBarButton();
      this.LEFT = new System.Windows.Forms.ToolBarButton();
      this.RIGHT = new System.Windows.Forms.ToolBarButton();
      this.Axo = new System.Windows.Forms.ToolBarButton();
      this.Rot = new System.Windows.Forms.ToolBarButton();
      this.Reset = new System.Windows.Forms.ToolBarButton();
      this.HlrOn = new System.Windows.Forms.ToolBarButton();
      this.HlrOff = new System.Windows.Forms.ToolBarButton();
      this.SuspendLayout();
      // 
      // mainMenu1
      // 
      this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
           this.File,
           this.View,
           this.Window,
           this.Help});
      // 
      // File
      // 
      this.File.Index = 0;
      this.File.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
           this.FileNew,
           this.FileExport,
           this.FileImport,
           this.FileClose,
           this.menuItem2,
           this.menuItem1});
      this.File.Text = "&File";
      this.File.Popup += new System.EventHandler(this.File_Popup);
      // 
      // FileNew
      // 
      this.FileNew.Index = 0;
      this.FileNew.Text = "&New";
      this.FileNew.Click += new System.EventHandler(this.menuItem2_Click);
      // 
      // FileExport
      // 
      this.FileExport.Index = 1;
      this.FileExport.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
	             this.ImportBRep,
	             this.ImportIges,
	             this.ImportStep});
      this.FileExport.Text = "&Import";
      this.FileExport.Visible = false;
      // 
      // ImportBRep
      // 
      this.ImportBRep.Index = 0;
      this.ImportBRep.Text = "&BRep ...";
      this.ImportBRep.Click += new System.EventHandler(this.ImportBRep_Click);
      // 
      // ImportIges
      // 
      this.ImportIges.Index = 1;
      this.ImportIges.Text = "&Iges ...";
      this.ImportIges.Click += new System.EventHandler(this.ImportIges_Click);
      // 
      // ImportStep
      // 
      this.ImportStep.Index = 2;
      this.ImportStep.Text = "&Step ...";
      this.ImportStep.Click += new System.EventHandler(this.ImportStep_Click);
      // 
      // FileImport
      // 
      this.FileImport.Index = 2;
      this.FileImport.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
	             this.ExportBRep,
	             this.ExportIges,
	             this.ExportStep,
	             this.ExportStl,
	             this.ExportVrml,
	             this.menuItem3,
	             this.ExportImage});
      this.FileImport.Text = "&Export";
      this.FileImport.Visible = false;
      this.FileImport.Popup += new System.EventHandler(this.FileImport_Popup);
      // 
      // ExportBRep
      // 
      this.ExportBRep.Enabled = false;
      this.ExportBRep.Index = 0;
      this.ExportBRep.Text = "&BRep ...";
      this.ExportBRep.Click += new System.EventHandler(this.ExportBRep_Click);
      // 
      // ExportIges
      // 
      this.ExportIges.Enabled = false;
      this.ExportIges.Index = 1;
      this.ExportIges.Text = "&Iges ...";
      this.ExportIges.Click += new System.EventHandler(this.ExportIges_Click);
      // 
      // ExportStep
      // 
      this.ExportStep.Enabled = false;
      this.ExportStep.Index = 2;
      this.ExportStep.Text = "&Step ...";
      this.ExportStep.Click += new System.EventHandler(this.ExportStep_Click);
      // 
      // ExportStl
      // 
      this.ExportStl.Enabled = false;
      this.ExportStl.Index = 3;
      this.ExportStl.Text = "&Stl ...";
      this.ExportStl.Click += new System.EventHandler(this.ExportStl_Click);
      // 
      // ExportVrml
      // 
      this.ExportVrml.Enabled = false;
      this.ExportVrml.Index = 4;
      this.ExportVrml.Text = "&Vrml ...";
      this.ExportVrml.Click += new System.EventHandler(this.ExportVrml_Click);
      // 
      // menuItem3
      // 
      this.menuItem3.Index = 5;
      this.menuItem3.Text = "-";
      // 
      // ExportImage
      // 
      this.ExportImage.Index = 6;
      this.ExportImage.Text = "Image ...";
      this.ExportImage.Click += new System.EventHandler(this.ExportImage_Click);
      // 
      // FileClose
      // 
      this.FileClose.Enabled = false;
      this.FileClose.Index = 3;
      this.FileClose.Text = "&Close";
      this.FileClose.Click += new System.EventHandler(this.menuItem3_Click);
      // 
      // menuItem2
      // 
      this.menuItem2.Index = 4;
      this.menuItem2.Text = "-";
      // 
      // menuItem1
      // 
      this.menuItem1.Index = 5;
      this.menuItem1.Text = "&Quit";
      this.menuItem1.Click += new System.EventHandler(this.menuItem1_Click);
      // 
      // View
      // 
      this.View.Index = 1;
      this.View.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
           this.ViewToolbar,
           this.ViewStatusBar});
      this.View.Text = "&View";
      // 
      // ViewToolbar
      // 
      this.ViewToolbar.Checked = true;
      this.ViewToolbar.Index = 0;
      this.ViewToolbar.Text = "&Toolbar";
      this.ViewToolbar.Click += new System.EventHandler(this.ViewToolbar_Click);
      // 
      // ViewStatusBar
      // 
      this.ViewStatusBar.Checked = true;
      this.ViewStatusBar.Index = 1;
      this.ViewStatusBar.Text = "&Statusbar";
      this.ViewStatusBar.Click += new System.EventHandler(this.ViewStatusBar_Click);
      // 
      // Window
      // 
      this.Window.Index = 2;
      this.Window.MdiList = true;
      this.Window.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
             this.menuItem4,
             this.WindowCascade,
             this.WindowTile});
      this.Window.Text = "&Window";
      this.Window.Visible = false;
      // 
      // menuItem4
      // 
      this.menuItem4.Index = 0;
      this.menuItem4.Text = "&New 3d View";
      this.menuItem4.Click += new System.EventHandler(this.menuItem4_Click);
      // 
      // WindowCascade
      // 
      this.WindowCascade.Index = 1;
      this.WindowCascade.Text = "&Cascade";
      this.WindowCascade.Click += new System.EventHandler(this.WindowCascade_Click);
      // 
      // WindowTile
      // 
      this.WindowTile.Index = 2;
      this.WindowTile.Text = "&Tile";
      this.WindowTile.Click += new System.EventHandler(this.WindowTile_Click);
      // 
      // Help
      // 
      this.Help.Index = 3;
      this.Help.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
           this.HelpAbout});
      this.Help.Text = "&Help";
      // 
      // HelpAbout
      // 
      this.HelpAbout.Index = 0;
      this.HelpAbout.Shortcut = System.Windows.Forms.Shortcut.F1;
      this.HelpAbout.Text = "&About";
      this.HelpAbout.Click += new System.EventHandler(this.HelpAbout_Click);
      // 
      // toolBarTool
      // 
      this.toolBarTool.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.toolBarTool.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
		             this.New,
		             this.About,
		             this.toolBarButton1,
		             this.wireframe,
		             this.shading,
		             this.color,
		             this.material,
		             this.transparency,
		             this.delete});
      this.toolBarTool.DropDownArrows = true;
      this.toolBarTool.ImageList = this.imageList1;
      this.toolBarTool.Location = new System.Drawing.Point(0, 0);
      this.toolBarTool.Name = "toolBarTool";
      this.toolBarTool.ShowToolTips = true;
      this.toolBarTool.Size = new System.Drawing.Size(560, 28);
      this.toolBarTool.TabIndex = 1;
      this.toolBarTool.Enter += new System.EventHandler(this.menuItem3_Click);
      this.toolBarTool.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.toolBar1_ButtonClick);
      this.toolBarTool.MouseHover += new System.EventHandler(this.toolBar1_MouseHover);
      this.toolBarTool.MouseLeave += new System.EventHandler(this.toolBar1_MouseLeave);
      // 
      // New
      // 
      this.New.ImageIndex = 0;
      this.New.ToolTipText = "New";
      this.New.Visible = ((bool)(configurationAppSettings.GetValue("New.Visible", typeof(bool))));
      // 
      // About
      // 
      this.About.ImageIndex = 1;
      this.About.ToolTipText = "About(F1)";
      this.About.Visible = ((bool)(configurationAppSettings.GetValue("About.Visible", typeof(bool))));
      // 
      // toolBarButton1
      // 
      this.toolBarButton1.Style = System.Windows.Forms.ToolBarButtonStyle.Separator;
      // 
      // wireframe
      // 
      this.wireframe.Enabled = ((bool)(configurationAppSettings.GetValue("wireframe.Enabled", typeof(bool))));
      this.wireframe.ImageIndex = 2;
      this.wireframe.Pushed = ((bool)(configurationAppSettings.GetValue("wireframe.Pushed", typeof(bool))));
      this.wireframe.ToolTipText = "Wireframe";
      this.wireframe.Visible = ((bool)(configurationAppSettings.GetValue("wireframe.Visible", typeof(bool))));
      // 
      // shading
      // 
      this.shading.Enabled = ((bool)(configurationAppSettings.GetValue("shading.Enabled", typeof(bool))));
      this.shading.ImageIndex = 3;
      this.shading.Pushed = ((bool)(configurationAppSettings.GetValue("shading.Pushed", typeof(bool))));
      this.shading.ToolTipText = "Shading";
      this.shading.Visible = ((bool)(configurationAppSettings.GetValue("shading.Visible", typeof(bool))));
      // 
      // color
      // 
      this.color.Enabled = ((bool)(configurationAppSettings.GetValue("color.Enabled", typeof(bool))));
      this.color.ImageIndex = 4;
      this.color.ToolTipText = "Color";
      this.color.Visible = ((bool)(configurationAppSettings.GetValue("color.Visible", typeof(bool))));
      // 
      // material
      // 
      this.material.Enabled = ((bool)(configurationAppSettings.GetValue("material.Enabled", typeof(bool))));
      this.material.ImageIndex = 5;
      this.material.ToolTipText = "Material";
      this.material.Visible = ((bool)(configurationAppSettings.GetValue("material.Visible", typeof(bool))));
      // 
      // transparency
      // 
      this.transparency.Enabled = ((bool)(configurationAppSettings.GetValue("transparency.Enabled", typeof(bool))));
      this.transparency.ImageIndex = 6;
      this.transparency.ToolTipText = "Transparency";
      this.transparency.Visible = ((bool)(configurationAppSettings.GetValue("transparency.Visible", typeof(bool))));
      // 
      // delete
      // 
      this.delete.Enabled = ((bool)(configurationAppSettings.GetValue("delete.Enabled", typeof(bool))));
      this.delete.ImageIndex = 7;
      this.delete.ToolTipText = "Delete";
      this.delete.Visible = ((bool)(configurationAppSettings.GetValue("delete.Visible", typeof(bool))));
      // 
      // imageList1
      // 
      this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
      this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
      this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
      // 
      // myStatusBar
      // 
      this.myStatusBar.AccessibleRole = System.Windows.Forms.AccessibleRole.StatusBar;
      this.myStatusBar.Location = new System.Drawing.Point(0, 363);
      this.myStatusBar.Name = "myStatusBar";
      this.myStatusBar.Size = new System.Drawing.Size(560, 22);
      this.myStatusBar.TabIndex = 3;
      // 
      // toolBarView
      // 
      this.toolBarView.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
      this.toolBarView.Buttons.AddRange(new System.Windows.Forms.ToolBarButton[] {
		             this.ZoomAll,
		             this.ZoomWin,
		             this.ZoomProg,
		             this.Pan,
		             this.PanGlo,
		             this.Front,
		             this.Back,
		             this.TOP,
		             this.BOTTOM,
		             this.LEFT,
		             this.RIGHT,
		             this.Axo,
		             this.Rot,
		             this.Reset,
		             this.HlrOn,
		             this.HlrOff});
      this.toolBarView.DropDownArrows = true;
      this.toolBarView.ImageList = this.imageList1;
      this.toolBarView.Location = new System.Drawing.Point(0, 28);
      this.toolBarView.Name = "toolBarView";
      this.toolBarView.ShowToolTips = true;
      this.toolBarView.Size = new System.Drawing.Size(560, 28);
      this.toolBarView.TabIndex = 5;
      this.toolBarView.Visible = false;
      this.toolBarView.Wrappable = false;
      this.toolBarView.ButtonClick += new System.Windows.Forms.ToolBarButtonClickEventHandler(this.toolBarView_ButtonClick);
      this.toolBarView.MouseHover += new System.EventHandler(this.toolBarView_MouseHover);
      this.toolBarView.MouseLeave += new System.EventHandler(this.toolBarView_MouseLeave);
      // 
      // ZoomAll
      // 
      this.ZoomAll.ImageIndex = 8;
      this.ZoomAll.ToolTipText = "FitAll";
      // 
      // ZoomWin
      // 
      this.ZoomWin.ImageIndex = 9;
      this.ZoomWin.ToolTipText = "Zoom Window";
      // 
      // ZoomProg
      // 
      this.ZoomProg.ImageIndex = 10;
      this.ZoomProg.ToolTipText = "Dynamic Zooming";
      // 
      // Pan
      // 
      this.Pan.ImageIndex = 11;
      this.Pan.ToolTipText = "Dynamic Panning";
      // 
      // PanGlo
      // 
      this.PanGlo.ImageIndex = 12;
      this.PanGlo.ToolTipText = "GlobalPanning";
      // 
      // Front
      // 
      this.Front.ImageIndex = 13;
      this.Front.ToolTipText = "Front";
      // 
      // Back
      // 
      this.Back.ImageIndex = 14;
      this.Back.ToolTipText = "Back";
      // 
      // TOP
      // 
      this.TOP.ImageIndex = 15;
      this.TOP.ToolTipText = "Top";
      // 
      // BOTTOM
      // 
      this.BOTTOM.ImageIndex = 16;
      this.BOTTOM.ToolTipText = "Bottom";
      // 
      // LEFT
      // 
      this.LEFT.ImageIndex = 17;
      this.LEFT.ToolTipText = "Left";
      // 
      // RIGHT
      // 
      this.RIGHT.ImageIndex = 18;
      this.RIGHT.ToolTipText = "Right";
      // 
      // Axo
      // 
      this.Axo.ImageIndex = 19;
      this.Axo.ToolTipText = "Axo";
      // 
      // Rot
      // 
      this.Rot.ImageIndex = 20;
      this.Rot.ToolTipText = "Dynamic Rotation";
      // 
      // Reset
      // 
      this.Reset.ImageIndex = 21;
      this.Reset.ToolTipText = "Reset";
      // 
      // HlrOn
      // 
      this.HlrOn.ImageIndex = 22;
      this.HlrOn.Pushed = true;
      this.HlrOn.ToolTipText = "Hidden On";
      // 
      // HlrOff
      // 
      this.HlrOff.ImageIndex = 23;
      this.HlrOff.ToolTipText = "Hidden Off";
      // 
      // Form1
      // 
      this.AccessibleRole = System.Windows.Forms.AccessibleRole.Application;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(560, 385);
      this.Controls.Add(this.toolBarView);
      this.Controls.Add(this.myStatusBar);
      this.Controls.Add(this.toolBarTool);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.IsMdiContainer = true;
      this.Menu = this.mainMenu1;
      this.Name = "Form1";
      this.Text = "Sample Import / Export";
      this.Activated += new System.EventHandler(this.Form1_Activated);
      this.ResumeLayout(false);
    }
    #endregion

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      Application.Run(new Form1());
    }

    private void menuItem2_Click(object sender, System.EventArgs e)
    {

      this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
      this.OnNewFile();
    }

    private void menuItem3_Click(object sender, System.EventArgs e)
    {
      if (this.MdiChildren.Length > 0)
        this.ActiveMdiChild.Close();

    }

    private void ImportBRep_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = ModelFormat.BREP;
      curForm.ImportModel(this.myModelFormat);
    }

    private void toolBar1_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      switch (toolBarTool.Buttons.IndexOf(e.Button))
      {
        case 0:
          this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
          this.OnNewFile();
          break;
        case 1:
          AboutDialog myDlg = new AboutDialog();
          myDlg.ShowDialog(this);
          break;

        case 2: //just separator
          break;
        case 3:
          if (curForm == null)
            return;
          curForm.SetDisplayMode(0); //wireframe
          this.wireframe.Enabled = false;
          this.shading.Enabled = true;
          this.transparency.Enabled = false;
          break;
        case 4:
          if (curForm == null)
            return;
          curForm.SetDisplayMode(1); //shading
          this.shading.Enabled = false;
          this.wireframe.Enabled = true;
          this.transparency.Enabled = true;
          break;
        case 5:
          if (curForm == null)
            return;
          curForm.ChangeColor(true);
          break;
        case 6:
          if (curForm == null)
            return;
          MaterialDialog m = new MaterialDialog();
          m.View = curForm.View;
          m.ShowDialog(curForm);
          break;
        case 7:
          if (curForm == null)
            return;
          IE_WinForms.TransparencyDialog t = new TransparencyDialog();
          t.View = curForm.View;
          t.ShowDialog(curForm);
          break;
        case 8:
          if (curForm == null)
            return;
          curForm.DeleteObjects();
          break;
        default:
          break;
      }
    }

    private void toolBar1_MouseHover(object sender, System.EventArgs e)
    {
      this.myStatusBar.Text = "Document toolbar";
    }

    private void toolBar1_MouseLeave(object sender, System.EventArgs e)
    {
      this.myStatusBar.Text = "";
    }

    private void ImportIges_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.IGES;
      curForm.ImportModel(this.myModelFormat);
    }

    private void ImportStep_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.STEP;
      curForm.ImportModel(this.myModelFormat);
    }

    private void ExportBRep_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.BREP;
      curForm.ExportModel(this.myModelFormat);
    }

    private void ExportIges_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.IGES;
      curForm.ExportModel(this.myModelFormat);
    }

    private void ExportStep_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.STEP;
      curForm.ExportModel(this.myModelFormat);
    }

    private void ExportStl_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.STL;
      curForm.ExportModel(this.myModelFormat);
    }

    private void ExportVrml_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.VRML;
      curForm.ExportModel(this.myModelFormat);
    }

    private void ExportImage_Click(object sender, System.EventArgs e)
    {
      Form2 curForm = (Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      this.myModelFormat = IE_WinForms.ModelFormat.IMAGE;
      curForm.ExportModel(this.myModelFormat);
    }

    private void HelpAbout_Click(object sender, System.EventArgs e)
    {
      AboutDialog myDlg = new AboutDialog();
      myDlg.ShowDialog(this);
    }

    private void ViewToolbar_Click(object sender, System.EventArgs e)
    {

      if (this.ViewToolbar.Checked)
      {
        if (this.MdiChildren.Length > 0)
        {
          this.New.Visible = false;
          this.About.Visible = false;
        }
        else
          this.toolBarTool.Hide();
        this.ViewToolbar.Checked = false;
      }
      else
      {
        if (this.MdiChildren.Length > 0)
        {
          this.New.Visible = true;
          this.About.Visible = true;
        }
        else
          this.toolBarTool.Show();
        this.ViewToolbar.Checked = true;
      }

    }

    private void ViewStatusBar_Click(object sender, System.EventArgs e)
    {
      if (this.ViewStatusBar.Checked)
      {
        this.myStatusBar.Hide();
        this.ViewStatusBar.Checked = false;
      }
      else
      {
        this.myStatusBar.Show();
        this.ViewStatusBar.Checked = true;
      }
    }

    public void OnNewFile()
    {
      Form2 newForm = new Form2();
      newForm.MdiParent = this;
      IE_WinForms.Form1.myNbOfChildren = IE_WinForms.Form1.myNbOfChildren + 1;
      newForm.SetIndex (IE_WinForms.Form1.myNbOfChildren, 1);
      newForm.Show();
      newForm.InitView();
      newForm.InitV3D();
      this.FileExport.Visible = true;
      this.FileImport.Visible = true;
      this.Window.Visible = true;
      this.wireframe.Visible = true;
      this.shading.Visible = true;
      this.color.Visible = true;
      this.material.Visible = true;
      this.transparency.Visible = true;
      this.delete.Visible = true;
      this.Cursor = System.Windows.Forms.Cursors.Default;
      this.toolBarView.Visible = true;
    }

    private void FileImport_Popup(object sender, System.EventArgs e)
    {
      IE_WinForms.Form2 curForm = (IE_WinForms.Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      if (curForm.View.IsObjectSelected())
      {
        this.ExportBRep.Enabled = true;
        this.ExportIges.Enabled = true;
        this.ExportStep.Enabled = true;
        this.ExportVrml.Enabled = true;
        this.ExportStl.Enabled = true;
      }

    }

    private void menuItem1_Click(object sender, System.EventArgs e)
    {
      this.Close();
    }

    private void File_Popup(object sender, System.EventArgs e)
    {
      if (this.MdiChildren.Length > 0)
        this.FileClose.Enabled = true;
      else
      {
        this.FileClose.Enabled = false;
        this.FileExport.Visible = false;
        this.FileImport.Visible = false;
      }
    }

    private void menuItem4_Click(object sender, System.EventArgs e)
    {
      IE_WinForms.Form2 curForm = (IE_WinForms.Form2)this.ActiveMdiChild;
      IE_WinForms.Form2 newView = new Form2();
      newView.MdiParent = this;
      newView.Show();
      newView.InitView();
      newView.SetContext(curForm.View);
      newView.View.CreateNewView(newView.Handle);
      newView.SetNextIndex(curForm);
    }

    private void WindowCascade_Click(object sender, System.EventArgs e)
    {
      this.LayoutMdi(System.Windows.Forms.MdiLayout.Cascade);
    }

    private void WindowTile_Click(object sender, System.EventArgs e)
    {
      this.LayoutMdi(System.Windows.Forms.MdiLayout.TileVertical);
    }

    public void SelectionChanged()
    {
      if (this.MdiChildren.Length == 0)
        return;
      IE_WinForms.Form2 curForm = (IE_WinForms.Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      switch (curForm.View.DisplayMode())
      {
        case -1:
          this.shading.Enabled = false;
          this.wireframe.Enabled = false;
          break;
        case 0:
          this.wireframe.Enabled = false;
          this.shading.Enabled = true;
          this.transparency.Enabled = false;
          break;
        case 1:
          this.wireframe.Enabled = true;
          this.shading.Enabled = false;
          this.transparency.Enabled = true;
          break;
        case 10:
          this.wireframe.Enabled = true;
          this.shading.Enabled = true;
          this.transparency.Enabled = true;
          break;
        default:
          break;
      }
      bool IsSelected = curForm.View.IsObjectSelected();
      if (IsSelected)
      {
        this.color.Enabled = true;
        this.material.Enabled = true;
        this.delete.Enabled = true;
      }
      else
      {
        this.color.Enabled = false;
        this.material.Enabled = false;
        this.transparency.Enabled = false;
        this.delete.Enabled = false;
      }
      if (curForm.DegenerateMode)
      {
        this.HlrOff.Pushed = false;
        this.HlrOn.Pushed = true;
      }
      else
      {
        this.HlrOff.Pushed = true;
        this.HlrOn.Pushed = false;
      }
      if (curForm.Mode == IE_WinForms.CurrentAction3d.CurAction3d_WindowZooming)
        this.ZoomWin.Pushed = false;

    }

    public StatusBar StatusBar
    {
      get
      {
        return this.myStatusBar;
      }
    }

    public void OnFileClose()
    {
      if (this.MdiChildren.Length <= 1)
      {
        this.FileClose.Enabled = false;
        this.Window.Visible = false;
        this.wireframe.Visible = false;
        this.shading.Visible = false;
        this.color.Visible = false;
        this.material.Visible = false;
        this.transparency.Visible = false;
        this.delete.Visible = false;
        this.toolBarView.Visible = false;
      }
    }

    private void toolBarView_ButtonClick(object sender, System.Windows.Forms.ToolBarButtonClickEventArgs e)
    {
      IE_WinForms.Form2 curForm = (IE_WinForms.Form2)this.ActiveMdiChild;
      if (curForm == null)
        return;
      switch (toolBarView.Buttons.IndexOf(e.Button))
      {
        case 0:
          curForm.View.ZoomAllView();
          break;
        case 1:
          curForm.Mode = CurrentAction3d.CurAction3d_WindowZooming;
          this.ZoomWin.Pushed = true;
          break;
        case 2:
          curForm.Mode = CurrentAction3d.CurAction3d_DynamicZooming;
          break;
        case 3:
          curForm.Mode = CurrentAction3d.CurAction3d_DynamicPanning;
          break;
        case 4:
          curForm.Zoom = curForm.View.Scale();
          curForm.Mode = CurrentAction3d.CurAction3d_GlobalPanning;
          break;
        case 5:
          curForm.View.FrontView();
          break;
        case 6:
          curForm.View.BackView();
          break;
        case 7:
          curForm.View.TopView();
          break;
        case 8:
          curForm.View.BottomView();
          break;
        case 9:
          curForm.View.LeftView();
          break;
        case 10:
          curForm.View.RightView();
          break;
        case 11:
          curForm.View.AxoView();
          break;
        case 12:
          curForm.Mode = CurrentAction3d.CurAction3d_DynamicRotation;
          break;
        case 13:
          curForm.View.Reset();
          break;
        case 14:
          curForm.View.SetDegenerateModeOn();
          curForm.DegenerateMode = true;
          this.HlrOff.Pushed = false;
          this.HlrOn.Pushed = true;
          break;
        case 15:
          curForm.View.SetDegenerateModeOff();
          curForm.DegenerateMode = false;
          this.HlrOn.Pushed = false;
          this.HlrOff.Pushed = true;
          break;
        default:
          break;
      }
    }

    private void Form1_Activated(object sender, System.EventArgs e)
    {
      if (this.toolBarView.Visible)
        this.SelectionChanged();
    }

    private void toolBarView_MouseHover(object sender, System.EventArgs e)
    {
      this.myStatusBar.Text = "View toolbar";
    }

    private void toolBarView_MouseLeave(object sender, System.EventArgs e)
    {
      this.myStatusBar.Text = "";
    }

  }

}