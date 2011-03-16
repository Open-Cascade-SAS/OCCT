
//Title:        OpenCASCADE Samples
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:  

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import jcas.*;
import CASCADESamplesJni.*;
import util.*;

public class CASCADEView3d extends ViewCanvas
{
  private V3d_Viewer myViewer = null;
  private V3d_View myView = null;

  private double currScale = 1.0;
  private boolean degenerated = true;

//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public CASCADEView3d(V3d_Viewer viewer)
  {
    myViewer = viewer;
    myView = myViewer.CreateView();
  }

//=======================================================================//
// Redefined operations
//=======================================================================//
  /** Sets a handle of native window to CASCADE view */
//   public void setWindow3d(V3d_View view, int hiwin, int lowin)
//   {
//   }

//=======================================================================//
//   public void SetWindow()
//   {
//     int windowHandle = getWinData();
//     short hi, lo;
//     lo = (short) windowHandle;
//     hi = (short) (windowHandle >> 16);
//     setWindow3d(myView, hi, lo);
//   }

//=======================================================================//
  public void FitAll()
  {
    if (myView != null)
    {
      myView.FitAll(0.01, true);
      myView.ZFitAll(0.0);
    }
  }

//=======================================================================//
  public void WindowFit(Rectangle rect)
  {
    if (myView != null)
      myView.WindowFit(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
  }

//=======================================================================//
  public void Zoom(int x0, int y0, int x, int y)
  {
    if (myView != null)
      myView.Zoom(x0, y0, x, y);
  }

//=======================================================================//
  public void ActivateGlobalPanning()
  {
    super.ActivateGlobalPanning();
    if (myView != null)
    {
      currScale = myView.Scale();
      myView.FitAll(0.01, false);
    }
  }

//=======================================================================//
  public void Pan(int dx, int dy)
  {
    if (myView != null)
      myView.Pan(dx, dy, 1.0);
  }

//=======================================================================//
  public void SetCenter(int x, int y)
  {
    if (myView != null)
      myView.Place(x, y, currScale);
  }

//=======================================================================//
  public void StartRotation(int x0, int y0)
  {
    if (myView != null)
    {
      degenerated = myView.DegenerateModeIsOn();
      myView.SetDegenerateModeOn();
      myView.StartRotation(x0, y0, 0.0);
    }
  }

//=======================================================================//
  public void Rotate(int x, int y)
  {
    if (myView != null)
      myView.Rotation(x, y);
  }

//=======================================================================//
  public void EndRotation()
  {
    if (myView != null)
      if (!degenerated)
        myView.SetDegenerateModeOff();
  }

//=======================================================================//
  public void SetProjection(String type)
  {
    if (myView != null)
    {
      if (type.equals("Axo"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_XposYnegZpos);
      else if (type.equals("Front"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_Yneg);
      else if (type.equals("Top"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_Zpos);
      else if (type.equals("Left"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_Xneg);
      else if (type.equals("Back"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_Ypos);
      else if (type.equals("Right"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_Xpos);
      else if (type.equals("Bottom"))
        myView.SetProj(V3d_TypeOfOrientation.V3d_Zneg);
    }
  }

//=======================================================================//
  public void SetDegenerateMode(boolean isOn)
  {
    if (myView != null)
    {
      if (isOn)
        myView.SetDegenerateModeOn();
      else
        myView.SetDegenerateModeOff();
    }
  }

//=======================================================================//
  public void ResetView()
  {
    if (myView != null)
      myView.Reset();
  }

//=======================================================================//
  public Color GetBackgroundColor()
  {
    if (myView != null)
    {
      Standard_Real aRed = new Standard_Real();
      Standard_Real aGreen = new Standard_Real();
      Standard_Real aBlue = new Standard_Real();

      myView.BackgroundColor(Quantity_TypeOfColor.Quantity_TOC_RGB, aRed, aGreen, aBlue);

      int red = (int) (aRed.GetValue()*255);
      int green = (int) (aGreen.GetValue()*255);
      int blue = (int) (aBlue.GetValue()*255);

      return new Color(red, green, blue);
    }
    else
      return super.GetBackgroundColor();
  }

//=======================================================================//
  public void SetBackgroundColor(Color color)
  {
    if (myView != null)
    {
      myView.SetBackgroundColor(Quantity_TypeOfColor.Quantity_TOC_RGB,
                                color.getRed()/255., color.getGreen()/255.,
                                color.getBlue()/255.);
      myView.Update();
    }
  }

//=======================================================================//

  public native void paint (Graphics g);
// commented
//   public void paint(Graphics g)
//   {
//     if (myView != null)
//     {
//       if (needResize)
//       {
//         myView.MustBeResized();
//         needResize = false;
//       }
//       myView.Redraw();
//     }
//     super.paint(g);
//   }

//=======================================================================//
  public V3d_View getView()
  {
    return myView;
  }
}
