using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;


namespace IE_WinForms
{
  public enum CurrentAction3d
  {
    CurAction3d_Nothing,
    CurAction3d_DynamicZooming,
    CurAction3d_WindowZooming,
    CurAction3d_DynamicPanning,
    CurAction3d_GlobalPanning,
    CurAction3d_DynamicRotation
  }
  public enum CurrentPressedKey
  {
    CurPressedKey_Nothing,
    CurPressedKey_Ctrl,
    CurPressedKey_Shift
  }
  public enum ModelFormat
  {
    BREP,
    STEP,
    IGES,
    VRML,
    STL,
    IMAGE
  }
  /// <summary>
  /// Summary description for Form2.
  /// </summary>
  public class Form2 : System.Windows.Forms.Form
  {
    private System.ComponentModel.IContainer components;

    public Form2()
    {
      //
      // Required for Windows Form Designer support
      //
      InitializeComponent();

      //
      // Create OCCT proxy object
      //
      myOCCTProxy = new OCCTProxy();
      myCurrentMode = CurrentAction3d.CurAction3d_Nothing;
      myCurrentPressedKey = CurrentPressedKey.CurPressedKey_Nothing;
      myDegenerateModeIsOn = true;
      IsRectVisible = false;
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
      System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Form2));
      this.imageList1 = new System.Windows.Forms.ImageList(this.components);
      this.myPopup = new System.Windows.Forms.ContextMenu();
      this.menuItem1 = new System.Windows.Forms.MenuItem();
      this.myPopupObject = new System.Windows.Forms.ContextMenu();
      this.ContextWireframe = new System.Windows.Forms.MenuItem();
      this.ContextShading = new System.Windows.Forms.MenuItem();
      this.ContextColor = new System.Windows.Forms.MenuItem();
      this.ContextMaterial = new System.Windows.Forms.MenuItem();
      this.ContMatBrass = new System.Windows.Forms.MenuItem();
      this.ContMenBronze = new System.Windows.Forms.MenuItem();
      this.ContMenCopper = new System.Windows.Forms.MenuItem();
      this.ContMenGold = new System.Windows.Forms.MenuItem();
      this.ContMenPewt = new System.Windows.Forms.MenuItem();
      this.ContMenPlaster = new System.Windows.Forms.MenuItem();
      this.ContMenPlastic = new System.Windows.Forms.MenuItem();
      this.ContMenSilver = new System.Windows.Forms.MenuItem();
      this.ContMenTranc = new System.Windows.Forms.MenuItem();
      this.ContMenDelete = new System.Windows.Forms.MenuItem();
      // 
      // imageList1
      // 
      this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
      this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
      this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
      // 
      // myPopup
      // 
      this.myPopup.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.menuItem1});
      // 
      // menuItem1
      // 
      this.menuItem1.Index = 0;
      this.menuItem1.Text = "Change &Background";
      this.menuItem1.Click += new System.EventHandler(this.menuItem1_Click);
      // 
      // myPopupObject
      // 
      this.myPopupObject.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
           this.ContextWireframe,
	   this.ContextShading,
	   this.ContextColor,
	   this.ContextMaterial,
	   this.ContMenTranc,
	   this.ContMenDelete});
      this.myPopupObject.Popup += new System.EventHandler(this.myPopupObject_Popup);
      // 
      // ContextWireframe
      // 
      this.ContextWireframe.Index = 0;
      this.ContextWireframe.Text = "Wireframe";
      this.ContextWireframe.Click += new System.EventHandler(this.ContextWireframe_Click);
      // 
      // ContextShading
      // 
      this.ContextShading.Index = 1;
      this.ContextShading.Text = "Shading";
      this.ContextShading.Click += new System.EventHandler(this.ContextShading_Click);
      // 
      // ContextColor
      // 
      this.ContextColor.Index = 2;
      this.ContextColor.Text = "Color";
      this.ContextColor.Click += new System.EventHandler(this.ContextColor_Click);
      // 
      // ContextMaterial
      // 
      this.ContextMaterial.Index = 3;
      this.ContextMaterial.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
           this.ContMatBrass,
           this.ContMenBronze,
           this.ContMenCopper,
           this.ContMenGold,
           this.ContMenPewt,
           this.ContMenPlaster,
           this.ContMenPlastic,
           this.ContMenSilver});
      this.ContextMaterial.Text = "Material";
      // 
      // ContMatBrass
      // 
      this.ContMatBrass.Index = 0;
      this.ContMatBrass.Text = "&Brass";
      this.ContMatBrass.Click += new System.EventHandler(this.ContMatBrass_Click);
      // 
      // ContMenBronze
      // 
      this.ContMenBronze.Index = 1;
      this.ContMenBronze.Text = "&Bronze";
      this.ContMenBronze.Click += new System.EventHandler(this.ContMenBronze_Click);
      // 
      // ContMenCopper
      // 
      this.ContMenCopper.Index = 2;
      this.ContMenCopper.Text = "&Copper";
      this.ContMenCopper.Click += new System.EventHandler(this.ContMenCopper_Click);
      // 
      // ContMenGold
      // 
      this.ContMenGold.Index = 3;
      this.ContMenGold.Text = "&Gold";
      this.ContMenGold.Click += new System.EventHandler(this.ContMenGold_Click);
      // 
      // ContMenPewt
      // 
      this.ContMenPewt.Index = 4;
      this.ContMenPewt.Text = "&Pewter";
      this.ContMenPewt.Click += new System.EventHandler(this.ContMenPewt_Click);
      // 
      // ContMenPlaster
      // 
      this.ContMenPlaster.Index = 5;
      this.ContMenPlaster.Text = "&Plaster";
      this.ContMenPlaster.Click += new System.EventHandler(this.ContMenPlaster_Click);
      // 
      // ContMenPlastic
      // 
      this.ContMenPlastic.Index = 6;
      this.ContMenPlastic.Text = "&Plastic";
      this.ContMenPlastic.Click += new System.EventHandler(this.ContMenPlastic_Click);
      // 
      // ContMenSilver
      // 
      this.ContMenSilver.Index = 7;
      this.ContMenSilver.Text = "&Silver";
      this.ContMenSilver.Click += new System.EventHandler(this.ContMenSilver_Click);
      // 
      // ContMenTranc
      // 
      this.ContMenTranc.Index = 4;
      this.ContMenTranc.Text = "&Trancparency";
      this.ContMenTranc.Click += new System.EventHandler(this.ContMenTranc_Click);
      // 
      // ContMenDelete
      // 
      this.ContMenDelete.Index = 5;
      this.ContMenDelete.Text = "&Delete";
      this.ContMenDelete.Click += new System.EventHandler(this.ContMenDelete_Click);
      // 
      // Form2
      // 
      this.AccessibleRole = System.Windows.Forms.AccessibleRole.Window;
      this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
      this.ClientSize = new System.Drawing.Size(320, 261);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.ImeMode = System.Windows.Forms.ImeMode.NoControl;
      this.Name = "Form2";
      this.Text = "Document";
      this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form2_KeyDown);
      this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Form2_MouseDown);
      this.SizeChanged += new System.EventHandler(this.Form2_SizeChanged);
      this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Form2_MouseUp);
      this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Form2_KeyUp);
      this.Closed += new System.EventHandler(this.Form2_Closed);
      this.Paint += new System.Windows.Forms.PaintEventHandler(this.Form2_Paint);
      this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form2_MouseMove);
    }
    #endregion

    private System.Windows.Forms.ImageList imageList1;
    private System.Windows.Forms.ContextMenu myPopup;
    private System.Windows.Forms.ContextMenu myPopupObject;
    private System.Windows.Forms.MenuItem ContextWireframe;
    private System.Windows.Forms.MenuItem ContextShading;
    private System.Windows.Forms.MenuItem ContextColor;
    private System.Windows.Forms.MenuItem ContextMaterial;
    private System.Windows.Forms.MenuItem ContMatBrass;
    private System.Windows.Forms.MenuItem ContMenBronze;
    private System.Windows.Forms.MenuItem ContMenCopper;
    private System.Windows.Forms.MenuItem ContMenGold;
    private System.Windows.Forms.MenuItem ContMenPewt;
    private System.Windows.Forms.MenuItem ContMenPlaster;
    private System.Windows.Forms.MenuItem ContMenPlastic;
    private System.Windows.Forms.MenuItem ContMenSilver;
    private System.Windows.Forms.MenuItem ContMenTranc;
    private System.Windows.Forms.MenuItem ContMenDelete;
    private System.Windows.Forms.MenuItem menuItem1;

    private OCCTProxy myOCCTProxy;
    private int myDocumentIndex, myViewIndex;

    public void InitV3D()
    {
      if (!myOCCTProxy.InitViewer(this.Handle))
        MessageBox.Show("Fatal Error during the graphic initialisation", "Error!",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
    }

    public bool ImportBRep(System.String filename)
    {
      return myOCCTProxy.ImportBrep(filename);
    }

    private void Form2_SizeChanged(object sender, System.EventArgs e)
    {
      myOCCTProxy.UpdateView();
    }

    private void Form2_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
    {
      myOCCTProxy.RedrawView();
      myOCCTProxy.UpdateView();
    }

    protected CurrentAction3d myCurrentMode;
    protected CurrentPressedKey myCurrentPressedKey;
    protected float myCurZoom;
    protected bool myDegenerateModeIsOn;
    protected int myXmin;
    protected int myYmin;
    protected int myXmax;
    protected int myYmax;
    protected int theButtonDownX;
    protected int theButtonDownY;
    // for erasing of rectangle
    protected int theRectDownX;
    protected int theRectDownY;
    protected bool IsRectVisible;

    private void Form2_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      switch (e.Button)
      {
        case MouseButtons.Left:
          myXmin = e.X; myYmin = e.Y;
          myXmax = e.X; myYmax = e.Y;
          if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
            // start the dinamic zooming....
            myCurrentMode = CurrentAction3d.CurAction3d_DynamicZooming;
          else
          {
            switch (myCurrentMode)
            {
              case CurrentAction3d.CurAction3d_Nothing:
                if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift)
                  MultiDragEvent(myXmax, myYmax, -1);
                else
                  DragEvent(myXmax, myYmax, -1);
                break;
              case CurrentAction3d.CurAction3d_DynamicRotation:
                if (!myDegenerateModeIsOn)
                  myOCCTProxy.SetDegenerateModeOn();
                //start the rotation
                myOCCTProxy.StartRotation(e.X, e.Y);
                break;
              case IE_WinForms.CurrentAction3d.CurAction3d_WindowZooming:
                this.Cursor = System.Windows.Forms.Cursors.Hand;
                break;
              default:
                break;
            }
          }
          break;
        case MouseButtons.Right:
          //MessageBox.Show("right mouse button is down");
          if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
          {
            if (!myDegenerateModeIsOn)
              myOCCTProxy.SetDegenerateModeOn();
            myOCCTProxy.StartRotation(e.X, e.Y);
          }
          else
            Popup(e.X, e.Y);
          break;
        default:
          break;
      }
    }

    private void Form2_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
    {
      if (e.Shift)
        myCurrentPressedKey = CurrentPressedKey.CurPressedKey_Shift;
      else if (e.Control)
        myCurrentPressedKey = CurrentPressedKey.CurPressedKey_Ctrl;
    }

    private void Form2_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
    {
      myCurrentPressedKey = CurrentPressedKey.CurPressedKey_Nothing;
    }

    protected void MultiDragEvent(int x, int y, int theState)
    {
      if (theState == -1)
      {
        theButtonDownX = x;
        theButtonDownY = y;
      }
      else if (theState == 1)
        myOCCTProxy.ShiftSelect(Math.Min(theButtonDownX, x), Math.Min(theButtonDownY, y),
                Math.Max(theButtonDownX, x), Math.Max(theButtonDownY, y));
    }

    protected void DragEvent(int x, int y, int theState)
    {
      if (theState == -1) //mouse is down
      {
        theButtonDownX = x;
        theButtonDownY = y;
      }
      else if (theState == 1) //mouse is up
      {
        myOCCTProxy.Select(Math.Min(theButtonDownX, x), Math.Min(theButtonDownY, y),
                Math.Max(theButtonDownX, x), Math.Max(theButtonDownY, y));
      }
    }

    protected void Popup(int x, int y)
    {
      System.Drawing.Point p = new Point(x, y);
      if (this.myOCCTProxy.IsObjectSelected())
        this.myPopupObject.Show(this, p);
      else
        this.myPopup.Show(this, p);
    }

    private void Form2_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      switch (e.Button)
      {
        case MouseButtons.Left:
          if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
            return;
          switch (myCurrentMode)
          {
            case CurrentAction3d.CurAction3d_Nothing:
              if (e.X == myXmin && e.Y == myYmin)
              {
                myXmax = e.X; myYmax = e.Y;
                if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift)
                  MultiInputEvent(myXmax, myYmax);
                else
                  InputEvent(myXmax, myYmax);
              }
              else
              {
                myXmax = e.X; myYmax = e.Y;
                DrawRectangle(false);
                if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift)
                  MultiDragEvent(myXmax, myYmax, 1);
                else
                  DragEvent(myXmax, myYmax, 1);
              }
              break;
            case CurrentAction3d.CurAction3d_DynamicZooming:
              myCurrentMode = CurrentAction3d.CurAction3d_Nothing;
              break;
            case CurrentAction3d.CurAction3d_WindowZooming:
              myXmax = e.X; myYmax = e.Y;
              DrawRectangle(false);
              int ValZWMin = 1;
              if (Math.Abs(myXmax - myXmin) > ValZWMin && Math.Abs(myXmax - myYmax) > ValZWMin)
                myOCCTProxy.WindowFitAll(myXmin, myYmin, myXmax, myYmax);
              this.Cursor = System.Windows.Forms.Cursors.Default;
              IE_WinForms.Form1 f = (IE_WinForms.Form1)this.ParentForm;
              f.SelectionChanged();
              myCurrentMode = CurrentAction3d.CurAction3d_Nothing;
              break;
            case CurrentAction3d.CurAction3d_DynamicPanning:
              myCurrentMode = CurrentAction3d.CurAction3d_Nothing;
              break;
            case CurrentAction3d.CurAction3d_GlobalPanning:
              myOCCTProxy.Place(e.X, e.Y, myCurZoom);
              myCurrentMode = CurrentAction3d.CurAction3d_Nothing;
              break;
            case CurrentAction3d.CurAction3d_DynamicRotation:
              myCurrentMode = CurrentAction3d.CurAction3d_Nothing;
              if (!myDegenerateModeIsOn)
              {
                myOCCTProxy.SetDegenerateModeOff();
                myDegenerateModeIsOn = false;
              }
              else
              {
                myOCCTProxy.SetDegenerateModeOn();
                myDegenerateModeIsOn = true;
              }
              break;
            default:
              break;

          }
          break;
        case MouseButtons.Right:
          if (!myDegenerateModeIsOn)
          {
            myOCCTProxy.SetDegenerateModeOff();
            myDegenerateModeIsOn = false;
          }
          else
          {
            myOCCTProxy.SetDegenerateModeOn();
            myDegenerateModeIsOn = true;
          }
          break;
        default:
          break;
      }

      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.SelectionChanged();
    }

    protected void MultiInputEvent(int x, int y)
    {
      myOCCTProxy.ShiftSelect();
    }

    protected void InputEvent(int x, int y)
    {
      myOCCTProxy.Select();
    }

    private void DrawRectangle(bool draw)
    {
      Graphics gr = Graphics.FromHwnd(this.Handle);
      System.Drawing.Pen p = null;
      if (this.IsRectVisible || (!draw))//erase the rect
      {
        int r = myOCCTProxy.GetBGColR();
        int g = myOCCTProxy.GetBGColG();
        int b = myOCCTProxy.GetBGColB();
        p = new Pen(System.Drawing.Color.FromArgb(r, g, b));
        this.IsRectVisible = false;
        this.myOCCTProxy.UpdateView();
      }
      else if (draw)
      {
        p = new Pen(System.Drawing.Color.White);
        this.IsRectVisible = true;
      }
      if (p == null)
        return;
      int x = Math.Min(this.myXmin, this.myXmax);
      int y = Math.Min(this.myYmin, this.myYmax);
      gr.DrawRectangle(p, x, y, Math.Abs(myXmax - myXmin), Math.Abs(myYmax - myYmin));
      this.theRectDownX = Math.Max(this.myXmin, this.myXmax);
      this.theRectDownY = Math.Max(this.myYmin, this.myYmax);
    }

    private void Form2_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      if (e.Button == MouseButtons.Left) //left button is pressed
      {
        if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
        {
          myOCCTProxy.Zoom(myXmax, myYmax, e.X, e.Y);
          myXmax = e.X; myYmax = e.Y;
        }
        else
        {
          switch (myCurrentMode)
          {
            case CurrentAction3d.CurAction3d_Nothing:
              DrawRectangle(false);
              myXmax = e.X; myYmax = e.Y;
              DrawRectangle(true);
              break;
            case CurrentAction3d.CurAction3d_DynamicZooming:
              myOCCTProxy.Zoom(myXmax, myYmax, e.X, e.Y);
              myXmax = e.X; myYmax = e.Y;
              break;
            case CurrentAction3d.CurAction3d_WindowZooming:
              DrawRectangle(false);
              myXmax = e.X; myYmax = e.Y;
              DrawRectangle(true);//add brush here
              break;
            case CurrentAction3d.CurAction3d_DynamicPanning:
              myOCCTProxy.Pan(e.X - myXmax, myYmax - e.Y);
              myXmax = e.X; myYmax = e.Y;
              break;
            case CurrentAction3d.CurAction3d_GlobalPanning:
              break;
            case CurrentAction3d.CurAction3d_DynamicRotation:
              myOCCTProxy.Rotation(e.X, e.Y);
              myOCCTProxy.RedrawView();
              break;
            default:
              break;
          }
        }
      } // e.Button == MouseButtons.Left
      else if (e.Button == MouseButtons.Middle)
      {
        if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
        {
          myOCCTProxy.Pan(e.X - myXmax, myYmax - e.Y);
          myXmax = e.X; myYmax = e.Y;
        }
      }//e.Button=MouseButtons.Middle
      else if (e.Button == MouseButtons.Right) //right button is pressed
      {
        if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl)
          myOCCTProxy.Rotation(e.X, e.Y);
      }
      else // no buttons are pressed
      {
        myXmax = e.X; myYmax = e.Y;
        if (myCurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift)
          MultiMoveEvent(e.X, e.Y);
        else
          MoveEvent(e.X, e.Y);
      }


    }

    protected void MultiMoveEvent(int x, int y)
    {
      myOCCTProxy.MoveTo(x, y);
    }

    protected void MoveEvent(int x, int y)
    {
      myOCCTProxy.MoveTo(x, y);
    }

    public void SetDisplayMode(int aMode)
    {
      myOCCTProxy.SetDisplayMode(aMode);
    }

    public void ChangeColor(bool IsObjectColor)
    {
      int r, g, b;
      if (IsObjectColor)
      {
        r = myOCCTProxy.GetObjColR();
        g = myOCCTProxy.GetObjColG();
        b = myOCCTProxy.GetObjColB();
      }
      else
      {
        r = myOCCTProxy.GetBGColR();
        g = myOCCTProxy.GetBGColG();
        b = myOCCTProxy.GetBGColB();
      }
      System.Windows.Forms.ColorDialog ColDlg = new ColorDialog();
      ColDlg.Color = System.Drawing.Color.FromArgb(r, g, b);
      if (ColDlg.ShowDialog() == DialogResult.OK)
      {
        Color c = ColDlg.Color;
        r = c.R;
        g = c.G;
        b = c.B;
        if (IsObjectColor)
          myOCCTProxy.SetColor(r, g, b);
        else
          myOCCTProxy.SetBackgroundColor(r, g, b);
      }
      this.myOCCTProxy.UpdateCurrentViewer();

    }

    public void DeleteObjects()
    {
      myOCCTProxy.EraseObjects();
      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.SelectionChanged();
    }
    public void ImportModel(IE_WinForms.ModelFormat format)
    {
      int theformat = 10;
      System.Windows.Forms.OpenFileDialog openDialog = new OpenFileDialog();

      string DataDir = Environment.GetEnvironmentVariable("CSF_OCCTDataPath");

      string filter = "";

      switch (format)
      {
        case ModelFormat.BREP:
          openDialog.InitialDirectory = (DataDir + "\\occ");
          theformat = 0;
          filter = "BREP Files (*.brep *.rle)|*.brep; *.rle";
          break;
        case IE_WinForms.ModelFormat.STEP:
          openDialog.InitialDirectory = (DataDir + "\\step");
          theformat = 1;
          filter = "STEP Files (*.stp *.step)|*.stp; *.step";
          break;
        case IE_WinForms.ModelFormat.IGES:
          openDialog.InitialDirectory = (DataDir + "\\iges");
          theformat = 2;
          filter = "IGES Files (*.igs *.iges)|*.igs; *.iges";
          break;
        default:
          break;
      }
      openDialog.Filter = filter + "|All files (*.*)|*.*";
      if (openDialog.ShowDialog() == DialogResult.OK)
      {
        string filename = openDialog.FileName;
        if (filename == "")
          return;
        this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
        if (!myOCCTProxy.TranslateModel(filename, theformat, true))
          MessageBox.Show("Cann't read this file", "Error!",
                  MessageBoxButtons.OK, MessageBoxIcon.Warning);
        this.Cursor = System.Windows.Forms.Cursors.Default;
      }
      this.myOCCTProxy.ZoomAllView();
    }

    public void ExportModel(ModelFormat format)
    {
      int theformat = 10;
      System.Windows.Forms.SaveFileDialog saveDialog = new SaveFileDialog();
      string DataDir = Environment.GetEnvironmentVariable("CSF_OCCTDataPath");
      string filter = "";
      switch (format)
      {
        case IE_WinForms.ModelFormat.BREP:
          saveDialog.InitialDirectory = (DataDir + "\\occ");
          theformat = 0;
          filter = "BREP Files (*.brep *.rle)|*.brep; *.rle";
          break;
        case IE_WinForms.ModelFormat.STEP:
          saveDialog.InitialDirectory = (DataDir + "\\step");
          theformat = 1;
          filter = "STEP Files (*.stp *.step)|*.step; *.stp";
          break;
        case IE_WinForms.ModelFormat.IGES:
          saveDialog.InitialDirectory = (DataDir + "\\iges");
          theformat = 2;
          filter = "IGES Files (*.igs *.iges)| *.iges; *.igs";
          break;
        case IE_WinForms.ModelFormat.VRML:
          saveDialog.InitialDirectory = (DataDir + "\\vrml");
          theformat = 3;
          filter = "VRML Files (*.vrml)|*.vrml";
          break;
        case IE_WinForms.ModelFormat.STL:
          saveDialog.InitialDirectory = (DataDir + "\\stl");
          theformat = 4;
          filter = "STL Files (*.stl)|*.stl";
          break;
        case IE_WinForms.ModelFormat.IMAGE:
          saveDialog.InitialDirectory = (DataDir + "\\images");
          theformat = 5;
          filter = "Images Files (*.bmp *.gif)| *.bmp; *.gif";
          break;
        default:
          break;
      }
      saveDialog.Filter = filter;
      if (saveDialog.ShowDialog() == DialogResult.OK)
      {
        string filename = saveDialog.FileName;
        if (filename == "")
          return;
        this.Cursor = System.Windows.Forms.Cursors.WaitCursor;
        if (!myOCCTProxy.TranslateModel(filename, theformat, false))
          MessageBox.Show("Cann't write this file", "Error!",
                  MessageBoxButtons.OK, MessageBoxIcon.Warning);
        this.Cursor = System.Windows.Forms.Cursors.Default;
      }
    }

    private void ContextColor_Click(object sender, System.EventArgs e)
    {
      this.ChangeColor(true);
    }

    private void menuItem1_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.ChangeColor(false);
    }

    private void ContextWireframe_Click(object sender, System.EventArgs e)
    {
      this.SetDisplayMode(0);
      this.myOCCTProxy.UpdateCurrentViewer();
      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.SelectionChanged();
    }

    private void ContextShading_Click(object sender, System.EventArgs e)
    {
      this.SetDisplayMode(1);
      this.myOCCTProxy.UpdateCurrentViewer();
      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.SelectionChanged();
    }

    private void ContMenTranc_Click(object sender, System.EventArgs e)
    {
      IE_WinForms.TransparencyDialog dlg = new TransparencyDialog();
      dlg.View = this.myOCCTProxy;
      dlg.ShowDialog(this);
    }

    private void ContMenDelete_Click(object sender, System.EventArgs e)
    {
      this.DeleteObjects();
    }

    private void ContMatBrass_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(0);
    }

    private void ContMenBronze_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(1);
    }

    private void ContMenCopper_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(2);
    }

    private void ContMenGold_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(3);
    }

    private void ContMenPewt_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(4);
    }

    private void ContMenPlaster_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(5);
    }

    private void ContMenPlastic_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(6);
    }

    private void ContMenSilver_Click(object sender, System.EventArgs e)
    {
      this.myOCCTProxy.UpdateCurrentViewer();
      this.myOCCTProxy.SetMaterial(7);
    }

    private void toolBar1_MouseHover(object sender, System.EventArgs e)
    {
      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.StatusBar.Text = "View toolbar";

    }

    private void toolBar1_MouseLeave(object sender, System.EventArgs e)
    {
      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.StatusBar.Text = "";
    }

    private void myPopupObject_Popup(object sender, System.EventArgs e)
    {
      int mode = this.myOCCTProxy.DisplayMode();
      switch (mode)
      {
        case -1:
          break;
        case 0:
          this.ContextWireframe.Enabled = false;
          this.ContextShading.Enabled = true;
          this.ContMenTranc.Enabled = false;
          break;
        case 1:
          this.ContextShading.Enabled = false;
          this.ContextWireframe.Enabled = true;
          this.ContMenTranc.Enabled = true;
          break;
        case 10:
          this.ContextShading.Enabled = true; ;
          this.ContextWireframe.Enabled = true;
          this.ContMenTranc.Enabled = true;
          break;
        default:
          break;

      }
    }

    public OCCTProxy View
    {
      get
      {
        return this.myOCCTProxy;
      }
      set
      {
        this.myOCCTProxy = value;
      }
    }

    public void InitView()
    {
      this.myOCCTProxy.InitOCCTProxy();
    }

    public void SetIndex(int documentIndex, int viewIndex)
    {
      this.myDocumentIndex = documentIndex;
      this.myViewIndex = viewIndex;
      this.Text = System.String.Format("Document {0}:{1}", documentIndex, viewIndex);
    }

    public void SetNextIndex(Form2 other)
    {
      SetIndex(other.myDocumentIndex, other.myViewIndex + 1);
    }

    public void SetContext(OCCTProxy View)
    {
      this.myOCCTProxy.SetAISContext(View);
    }

    private void Form2_Closed(object sender, System.EventArgs e)
    {
      IE_WinForms.Form1 parent = (IE_WinForms.Form1)this.ParentForm;
      parent.OnFileClose();
    }

    public CurrentAction3d Mode
    {
      get
      {
        return this.myCurrentMode;
      }
      set
      {
        this.myCurrentMode = value;
      }
    }

    public float Zoom
    {
      set
      {
        this.myCurZoom = value;
      }
    }

    public bool DegenerateMode
    {
      get
      {
        return this.myDegenerateModeIsOn;
      }
      set
      {
        this.myDegenerateModeIsOn = value;
      }
    }

  }

}