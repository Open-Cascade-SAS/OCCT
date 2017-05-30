using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Input;
using System.Drawing;

namespace IE_WPF_D3D
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

  public enum DisplayMode
  {
    Wireframe,
    Shading
  }

  public class OCCViewer
  {
    public event EventHandler ZoomingFinished;
    protected void RaiseZoomingFinished ()
    {
      if (ZoomingFinished != null)
      {
        ZoomingFinished (this, EventArgs.Empty);
      }
    }

    public event EventHandler AvaliabiltyOfOperationsChanged;
    protected void RaiseAvaliabiltyOfOperationsChanged ()
    {
      if (AvaliabiltyOfOperationsChanged != null)
      {
        AvaliabiltyOfOperationsChanged (this, EventArgs.Empty);
      }
    }

    public OCCTProxyD3D View { get; private set; }
    public CurrentAction3d CurrentMode { get; private set; }
    private bool IsRectVisible { get; set; }
    public bool DegenerateMode { get; private set; }

    public bool IsWireframeEnabled { get; private set; }
    public bool IsShadingEnabled { get; private set; }
    public bool IsTransparencyEnabled { get; private set; }
    public bool IsColorEnabled { get; private set; }
    public bool IsMaterialEnabled { get; private set; }
    public bool IsDeleteEnabled { get; private set; }

    private float myCurZoom;
    private int myXmin;
    private int myYmin;
    private int myXmax;
    private int myYmax;
    private int myButtonDownX;
    private int myButtonDownY;
    public OCCViewer()
    {
      View = new OCCTProxyD3D ();
      View.InitOCCTProxy ();
      CurrentMode = CurrentAction3d.CurAction3d_Nothing;
      IsRectVisible = false;
      DegenerateMode = true;
    }

    public bool InitViewer()
    {
      return View.InitViewer();
    }

    public void ImportModel (ModelFormat theFormat)
    {
      int aFormat = 10;
      OpenFileDialog anOpenDialog = new OpenFileDialog ();
      string aDataDir = Environment.GetEnvironmentVariable ("CSF_OCCTDataPath");
      string aFilter = "";

      switch (theFormat)
      {
        case ModelFormat.BREP:
          anOpenDialog.InitialDirectory = (aDataDir + "\\occ");
          aFormat = 0;
          aFilter = "BREP Files (*.brep *.rle)|*.brep; *.rle";
          break;
        case ModelFormat.STEP:
          anOpenDialog.InitialDirectory = (aDataDir + "\\step");
          aFormat = 1;
          aFilter = "STEP Files (*.stp *.step)|*.stp; *.step";
          break;
        case ModelFormat.IGES:
          anOpenDialog.InitialDirectory = (aDataDir + "\\iges");
          aFormat = 2;
          aFilter = "IGES Files (*.igs *.iges)|*.igs; *.iges";
          break;
        default:
          break;
      }

      anOpenDialog.Filter = aFilter + "|All files (*.*)|*.*";
      if (anOpenDialog.ShowDialog () == DialogResult.OK)
      {
        string aFileName = anOpenDialog.FileName;
        if (aFileName == "")
        {
          return;
        }

        if (!View.TranslateModel (aFileName, aFormat, true))
        {
          MessageBox.Show ("Cann't read this file", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
      }
      View.ZoomAllView ();
    }

    public void ExportModel (ModelFormat theFormat)
    {
      int aFormat = 10;
      SaveFileDialog saveDialog = new SaveFileDialog ();
      string aDataDir = Environment.GetEnvironmentVariable ("CSF_OCCTDataPath");
      string aFilter = "";

      switch (theFormat)
      {
        case ModelFormat.BREP:
          saveDialog.InitialDirectory = (aDataDir + "\\occ");
          aFormat = 0;
          aFilter = "BREP Files (*.brep *.rle)|*.brep; *.rle";
          break;
        case ModelFormat.STEP:
          saveDialog.InitialDirectory = (aDataDir + "\\step");
          aFormat = 1;
          aFilter = "STEP Files (*.stp *.step)|*.step; *.stp";
          break;
        case ModelFormat.IGES:
          saveDialog.InitialDirectory = (aDataDir + "\\iges");
          aFormat = 2;
          aFilter = "IGES Files (*.igs *.iges)| *.iges; *.igs";
          break;
        case ModelFormat.VRML:
          saveDialog.InitialDirectory = (aDataDir + "\\vrml");
          aFormat = 3;
          aFilter = "VRML Files (*.vrml)|*.vrml";
          break;
        case ModelFormat.STL:
          saveDialog.InitialDirectory = (aDataDir + "\\stl");
          aFormat = 4;
          aFilter = "STL Files (*.stl)|*.stl";
          break;
        case ModelFormat.IMAGE:
          saveDialog.InitialDirectory = (aDataDir + "\\images");
          aFormat = 5;
          aFilter = "Images Files (*.bmp)|*.bmp";
          break;
        default:
          break;
      }

      saveDialog.Filter = aFilter;
      if (saveDialog.ShowDialog () == DialogResult.OK)
      {
        string aFileName = saveDialog.FileName;
        if (aFileName == "")
        {
          return;
        }

        if (!View.TranslateModel (aFileName, aFormat, false))
        {
          MessageBox.Show ("Can not write this file", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
      }
    }

    public void FitAll ()
    {
      View.ZoomAllView ();
    }

    public void ZoomWindow ()
    {
      CurrentMode = CurrentAction3d.CurAction3d_WindowZooming;
    }

    public void DynamicZooming ()
    {
      CurrentMode = CurrentAction3d.CurAction3d_DynamicZooming;
    }

    public void DynamicPanning ()
    {
      CurrentMode = CurrentAction3d.CurAction3d_DynamicPanning;
    }

    public void GlobalPanning ()
    {
      myCurZoom = View.Scale ();
      CurrentMode = CurrentAction3d.CurAction3d_GlobalPanning;
    }

    public void AxoView ()
    {
      View.AxoView ();
    }

    public void FrontView ()
    {
      View.FrontView ();
    }

    public void TopView ()
    {
      View.TopView ();
    }

    public void LeftView ()
    {
      View.LeftView ();
    }

    public void BackView ()
    {
      View.BackView ();
    }

    public void RightView ()
    {
      View.RightView ();
    }

    public void Reset ()
    {
      View.Reset ();
    }

    public void BottomView ()
    {
      View.BottomView ();
    }

    public void HiddenOff ()
    {
      View.SetDegenerateModeOff ();
      DegenerateMode = false;
    }

    public void HiddenOn ()
    {
      View.SetDegenerateModeOn ();
      DegenerateMode = true;
    }

    public void DynamicRotation ()
    {
      CurrentMode = CurrentAction3d.CurAction3d_DynamicRotation;
    }

    public void SelectionChanged ()
    {
      switch (View.DisplayMode ())
      {
        case -1:
          IsShadingEnabled = false;
          IsWireframeEnabled = false;
          break;
        case 0:
          IsWireframeEnabled = false;
          IsShadingEnabled = true;
          IsTransparencyEnabled = false;
          break;
        case 1:
          IsWireframeEnabled = true;
          IsShadingEnabled = false;
          IsTransparencyEnabled = true;
          break;
        case 10:
          IsWireframeEnabled = true;
          IsShadingEnabled = true;
          IsTransparencyEnabled = true;
          break;
        default:
          break;
      }

      if (View.IsObjectSelected ())
      {
        IsColorEnabled = true;
        IsMaterialEnabled = true;
        IsDeleteEnabled = true;
      }
      else
      {
        IsColorEnabled = false;
        IsMaterialEnabled = false;
        IsTransparencyEnabled = false;
        IsDeleteEnabled = false;
      }

      RaiseAvaliabiltyOfOperationsChanged ();
    }

    public void ChangeColor (bool IsObjectColor)
    {
      int r, g, b;
      if (IsObjectColor)
      {
        r = View.GetObjColR ();
        g = View.GetObjColG ();
        b = View.GetObjColB ();
      }
      else
      {
        r = View.GetBGColR ();
        g = View.GetBGColG ();
        b = View.GetBGColB ();
      }
      System.Windows.Forms.ColorDialog ColDlg = new System.Windows.Forms.ColorDialog ();
      ColDlg.Color = System.Drawing.Color.FromArgb (r, g, b);
      if (ColDlg.ShowDialog () == System.Windows.Forms.DialogResult.OK)
      {
        System.Drawing.Color c = ColDlg.Color;
        r = c.R;
        g = c.G;
        b = c.B;
        if (IsObjectColor)
        {
          View.SetColor (r, g, b);
        }
        else
        {
          View.SetBackgroundColor (r, g, b);
        }
      }
      View.UpdateCurrentViewer ();
    }

    public void Wireframe ()
    {
      View.SetDisplayMode ((int)DisplayMode.Wireframe);
      View.UpdateCurrentViewer ();

      SelectionChanged ();
      RaiseZoomingFinished ();
    }

    public void Shading ()
    {
      View.SetDisplayMode ((int)DisplayMode.Shading);
      View.UpdateCurrentViewer ();

      SelectionChanged ();
      RaiseZoomingFinished ();
    }

    public void Color ()
    {
      ChangeColor (true);
    }

    public void Background ()
    {
      ChangeColor (false);
    }

    public void Material ()
    {
      MaterialDlg aDlg = new MaterialDlg (View);
      aDlg.ShowDialog ();
    }

    public void Transparency ()
    {
      TransparencyDialog dlg = new TransparencyDialog ();
      dlg.View = View;
      dlg.ShowDialog ();
    }

    public void Delete ()
    {
      View.EraseObjects ();
      SelectionChanged ();
    }

    protected void MultiDragEvent (int x, int y, int theState)
    {
      if (theState == -1) //mouse is down
      {
        myButtonDownX = x;
        myButtonDownY = y;
      }
      else if (theState == 1) //mouse is up
      {
        View.ShiftSelect (Math.Min (myButtonDownX, x), Math.Min (myButtonDownY, y),
                          Math.Max (myButtonDownX, x), Math.Max (myButtonDownY, y));
      }
    }

    protected void DragEvent (int x, int y, int theState)
    {
      if (theState == -1) //mouse is down
      {
        myButtonDownX = x;
        myButtonDownY = y;
      }
      else if (theState == 1) //mouse is up
      {
        View.Select (Math.Min (myButtonDownX, x), Math.Min (myButtonDownY, y),
                     Math.Max (myButtonDownX, x), Math.Max (myButtonDownY, y));
      }
    }

    public void OnMouseDown (System.Windows.IInputElement sender, MouseButtonEventArgs e)
    {
      System.Windows.Controls.TabControl aTabControl = sender as System.Windows.Controls.TabControl;
      System.Windows.Controls.Grid aGrid = aTabControl.SelectedContent as System.Windows.Controls.Grid;

      Point p = new Point((int)e.GetPosition(aGrid).X, (int)e.GetPosition(aGrid).Y);

      // to avoid the context menu opening
      aTabControl.ContextMenu.Visibility = System.Windows.Visibility.Collapsed;
      aTabControl.ContextMenu.IsOpen = false;

      if (e.LeftButton == MouseButtonState.Pressed)
      {
        myXmin = p.X;
        myXmax = p.X;
        myYmin = p.Y;
        myYmax = p.Y;

        if (Keyboard.IsKeyDown (Key.LeftCtrl) || Keyboard.IsKeyDown (Key.RightCtrl))
        {
          // start the dinamic zooming....
          CurrentMode = CurrentAction3d.CurAction3d_DynamicZooming;
        }
        else
        {
          switch (CurrentMode)
          {
            case CurrentAction3d.CurAction3d_Nothing:
              if (Keyboard.IsKeyDown (Key.LeftShift) || Keyboard.IsKeyDown (Key.RightShift))
              {
                MultiDragEvent (myXmax, myYmax, -1);
              }
              else
              {
                DragEvent (myXmax, myYmax, -1);
              }
              break;
            case CurrentAction3d.CurAction3d_DynamicRotation:
              if (!DegenerateMode)
              {
                View.SetDegenerateModeOn ();
              }
              View.StartRotation (p.X, p.Y);
              break;
            default:
              break;
          }
        }
      }
      else if (e.RightButton == MouseButtonState.Pressed)
      {
        if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
        {
          if (!DegenerateMode)
          {
            View.SetDegenerateModeOn();
          }
          View.StartRotation(p.X, p.Y);
        }
        else
        {
          // show context menu only in this case
          aTabControl.ContextMenu.Visibility = System.Windows.Visibility.Visible;
        }
      }
    }

    public void OnMouseUp(System.Windows.IInputElement sender, MouseButtonEventArgs e)
    {
      Point p = new Point((int)e.GetPosition(sender).X, (int)e.GetPosition(sender).Y);

      if (e.ChangedButton == MouseButton.Left)
      {
        if (Keyboard.IsKeyDown (Key.LeftCtrl) || Keyboard.IsKeyDown (Key.RightCtrl))
        {
          CurrentMode = CurrentAction3d.CurAction3d_Nothing;
          return;
        }
        switch (CurrentMode)
        {
          case CurrentAction3d.CurAction3d_Nothing:
            if (p.X == myXmin && p.Y == myYmin)
            {
              myXmax = p.X;
              myYmax = p.Y;
              if (Keyboard.IsKeyDown (Key.LeftShift) || Keyboard.IsKeyDown (Key.RightShift))
              {
                View.ShiftSelect ();
              }
              else
              {
                View.Select ();
              }
            }
            else
            {
              myXmax = p.X;
              myYmax = p.Y;
              if (Keyboard.IsKeyDown (Key.LeftShift) || Keyboard.IsKeyDown (Key.RightShift))
              {
                MultiDragEvent (myXmax, myYmax, 1);
              }
              else
              {
                DragEvent (myXmax, myYmax, 1);
              }
            }
            break;
          case CurrentAction3d.CurAction3d_DynamicZooming:
            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            break;
          case CurrentAction3d.CurAction3d_WindowZooming:
            myXmax = p.X;
            myYmax = p.Y;
            int ValZWMin = 1;
            if (Math.Abs (myXmax - myXmin) > ValZWMin &&
                 Math.Abs (myXmax - myYmax) > ValZWMin)
            {
              View.WindowFitAll (myXmin, myYmin, myXmax, myYmax);
            }
            RaiseZoomingFinished ();
            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            break;
          case CurrentAction3d.CurAction3d_DynamicPanning:
            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            break;
          case CurrentAction3d.CurAction3d_GlobalPanning:
            View.Place (p.X, p.Y, myCurZoom);
            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            break;
          case CurrentAction3d.CurAction3d_DynamicRotation:
            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            if (!DegenerateMode)
            {
              View.SetDegenerateModeOff ();
            }
            else
            {
              View.SetDegenerateModeOn ();
            }
            break;
          default:
            break;
        }
      }
      else if (e.ChangedButton == MouseButton.Right)
      {
        if (!DegenerateMode)
        {
          View.SetDegenerateModeOff ();
        }
        else
        {
          View.SetDegenerateModeOn ();
        }
      }

      SelectionChanged ();
    }

    public void OnMouseMove (System.Windows.IInputElement sender, System.Windows.Input.MouseEventArgs e)
    {
      Point p = new Point ((int)e.GetPosition (sender).X, (int)e.GetPosition (sender).Y);

      if (e.LeftButton == MouseButtonState.Pressed) //left button is pressed
      {
        if (Keyboard.IsKeyDown (Key.LeftCtrl) || Keyboard.IsKeyDown (Key.RightCtrl))
        {
          View.Zoom (myXmax, myYmax, p.X, p.Y);
          myXmax = p.X;
          myYmax = p.Y;
        }
        else
        {
          switch (CurrentMode)
          {
            case CurrentAction3d.CurAction3d_Nothing:
              myXmax = p.X;
              myYmax = p.Y;
              break;
            case CurrentAction3d.CurAction3d_DynamicZooming:
              View.Zoom (myXmax, myYmax, p.X, p.Y);
              myXmax = p.X;
              myYmax = p.Y;
              break;
            case CurrentAction3d.CurAction3d_WindowZooming:
              myXmax = p.X;
              myYmax = p.Y;
              break;
            case CurrentAction3d.CurAction3d_DynamicPanning:
              View.Pan (p.X - myXmax, myYmax - p.Y);
              myXmax = p.X;
              myYmax = p.Y;
              break;
            case CurrentAction3d.CurAction3d_GlobalPanning:
              break;
            case CurrentAction3d.CurAction3d_DynamicRotation:
              View.Rotation (p.X, p.Y);
              View.RedrawView ();
              break;
            default:
              break;
          }
        }
      }
      else if (e.MiddleButton == MouseButtonState.Pressed) //middle button is pressed
      {
        if (Keyboard.IsKeyDown (Key.LeftCtrl) || Keyboard.IsKeyDown (Key.RightCtrl))
        {
          View.Pan (p.X - myXmax, myYmax - p.Y);
          myXmax = p.X;
          myYmax = p.Y;
        }
      }
      else if (e.RightButton == MouseButtonState.Pressed) //right button is pressed
      {
        if (Keyboard.IsKeyDown (Key.LeftCtrl) || Keyboard.IsKeyDown (Key.RightCtrl))
        {
          View.Rotation (p.X, p.Y);
        }
      }
      else // no buttons are pressed
      {
        myXmax = p.X;
        myYmax = p.Y;
        View.MoveTo (p.X, p.Y);
      }
    }
  }
}
