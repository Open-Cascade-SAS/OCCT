
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


public class CASCADEView2d extends ViewCanvas
                           implements FocusListener
{
  V2d_Viewer myViewer = null;
  V2d_View myView = null;
  short myBackColor = Quantity_NameOfColor.Quantity_NOC_AQUAMARINE4;

  private double currScale = 1.0;

  //-----------------------------------------------------------//
  // 2d Grid management
  //-----------------------------------------------------------//
  private int gridState = 0;  // 0 - no active grid
                              // 1 - active rectangular grid
                              // 2 - active circular grid

  // Rectangular grid
  private RealSpin ctlXOrigin = new RealSpin();
  private RealSpin ctlYOrigin = new RealSpin();
  private RealSpin ctlXStep = new RealSpin();
  private RealSpin ctlYStep = new RealSpin();
  private RealSpin ctlAngle = new RealSpin();
  private JPanel RectGridPane = new JPanel(new GridLayout(5, 2, 0, 4));

  // Circular grid
  private RealSpin ctlXOrigC = new RealSpin();
  private RealSpin ctlYOrigC = new RealSpin();
  private RealSpin ctlRadStep = new RealSpin();
  private IntegerSpin ctlDivision = new IntegerSpin();
  private RealSpin ctlAngleRot = new RealSpin();
  private JPanel CircGridPane = new JPanel(new GridLayout(5, 2, 0, 4));

  // Buttons
  private JButton btnClose = new JButton(resGui.getString("BT_CLOSE"));
  private JButton btnHelp = new JButton(resGui.getString("BT_HELP"));
  private JPanel ButtonsPane = new JPanel();
  private JDialog RectGridDlg= null;
  private JDialog CircGridDlg = null;


//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public CASCADEView2d(V2d_Viewer viewer)
  {
    myViewer = viewer;
    createDialogs();
  }

  private void createDialogs()
  {
    // Rectangular
    RectGridPane.setBorder(BorderFactory.createEmptyBorder(2, 8, 2, 8));
    RectGridPane.add(new JLabel(resGui.getString("TXT_XORIGIN")));
    ctlXOrigin.addFocusListener(this);
    RectGridPane.add(ctlXOrigin);

    RectGridPane.add(new JLabel(resGui.getString("TXT_YORIGIN")));
    ctlYOrigin.addFocusListener(this);
    RectGridPane.add(ctlYOrigin);

    RectGridPane.add(new JLabel(resGui.getString("TXT_XSTEP")));
    ctlXStep.addFocusListener(this);
    RectGridPane.add(ctlXStep);

    RectGridPane.add(new JLabel(resGui.getString("TXT_YSTEP")));
    ctlYStep.addFocusListener(this);
    RectGridPane.add(ctlYStep);

    RectGridPane.add(new JLabel(resGui.getString("TXT_ANGLE")));
    ctlAngle.addFocusListener(this);
    RectGridPane.add(ctlAngle);

    // Circular
    CircGridPane.setBorder(BorderFactory.createEmptyBorder(2, 8, 2, 8));
    CircGridPane.add(new JLabel(resGui.getString("TXT_XORIGIN")));
    ctlXOrigC.addFocusListener(this);
    CircGridPane.add(ctlXOrigC);

    CircGridPane.add(new JLabel(resGui.getString("TXT_YORIGIN")));
    ctlYOrigC.addFocusListener(this);
    CircGridPane.add(ctlYOrigC);

    CircGridPane.add(new JLabel(resGui.getString("TXT_RADSTEP")));
    ctlRadStep.addFocusListener(this);
    CircGridPane.add(ctlRadStep);

    CircGridPane.add(new JLabel(resGui.getString("TXT_DIVISION")));
    ctlDivision.addFocusListener(this);
    CircGridPane.add(ctlDivision);

    CircGridPane.add(new JLabel(resGui.getString("TXT_ANGLE")));
    ctlAngleRot.addFocusListener(this);
    CircGridPane.add(ctlAngleRot);

    // Buttons
    ButtonsPane.setBorder(BorderFactory.createCompoundBorder(
         BorderFactory.createMatteBorder(1,0,0,0,SystemColor.controlShadow),
         BorderFactory.createMatteBorder(1,0,0,0,SystemColor.controlLtHighlight)));
    btnClose.setActionCommand("CloseGridDlg");
    btnClose.addActionListener(this);
    ButtonsPane.add(btnClose);
    btnHelp.setActionCommand("HelpGridDlg");
    btnHelp.addActionListener(this);
    ButtonsPane.add(btnHelp);
  }


//=======================================================================//
// Redefined operations
//=======================================================================//
  /**
   * Creates a new CASCADE V2d_View object and
   * sets a handle of native window to CASCADE view.
   */
  public V2d_View createView2d(V2d_Viewer viewer, int hiwin, int lowin)
  {
    return null;
  }

//=======================================================================//
//   public void SetWindow()
//   {
//     int windowHandle = getWinData();
//     short hi, lo;
//     lo = (short) windowHandle;
//     hi = (short) (windowHandle >> 16);

//     myView = createView2d(myViewer, hi, lo);
//     if (myView != null)
//       myView.Fitall();
//   }

//=======================================================================//
  public void FitAll()
  {
    if (myView != null)
      myView.Fitall();
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
      myView.Zoom(x0, y0, x, y, 0.005);
  }

//=======================================================================//
  public void ActivateGlobalPanning()
  {
    super.ActivateGlobalPanning();
    if (myView != null)
    {
      currScale = myView.Zoom();
      myView.Fitall();
    }
  }

//=======================================================================//
  public void Pan(int dx, int dy)
  {
    if (myView != null)
      myView.Pan(dx, dy);
  }

//=======================================================================//
  public void SetCenter(int x, int y)
  {
    if (myView != null)
      myView.Place(x, y, currScale);
  }

//=======================================================================//
  public void ResetView()
  {
    if (myView != null)
      myView.Reset();
  }

//=======================================================================//
  public void EditGrid()
  {
    if (RectGridDlg != null) return;
    if (CircGridDlg != null) return;

    if (gridState == 1)
    {
      Standard_Real XOrigin = new Standard_Real(),
                    YOrigin = new Standard_Real(),
                    XStep = new Standard_Real(),
                    YStep = new Standard_Real(),
                    aAngle = new Standard_Real();
      myViewer.RectangularGridValues(XOrigin, YOrigin, XStep, YStep, aAngle);

      ctlXOrigin.setValue(XOrigin.GetValue());
      ctlYOrigin.setValue(YOrigin.GetValue());
      ctlXStep.setValue(XStep.GetValue());
      ctlYStep.setValue(YStep.GetValue());
      ctlAngle.setValue(aAngle.GetValue());

      Frame mainFrame = SamplesStarter.getFrame();
      RectGridDlg = new JDialog(mainFrame, resGui.getString("DLG_RECTEDIT"), true);
      RectGridDlg.setSize(180, 200);
      RectGridDlg.getContentPane().setLayout(new BorderLayout());
      RectGridDlg.getContentPane().add(RectGridPane, BorderLayout.CENTER);
      RectGridDlg.getContentPane().add(ButtonsPane, BorderLayout.SOUTH);

      Point aLoc = mainFrame.getLocation();
      RectGridDlg.setLocation(aLoc.x + mainFrame.getWidth()/2 - RectGridDlg.getWidth()/2,
                              aLoc.y + mainFrame.getHeight()/2 - RectGridDlg.getHeight()/2);
      RectGridDlg.show();
    }
    else if (gridState == 2)
    {
      Standard_Real XOrigin = new Standard_Real(),
                    YOrigin = new Standard_Real(),
                    RadStep = new Standard_Real(),
                    aAngle = new Standard_Real();
      Standard_Integer DivN = new Standard_Integer();
      myViewer.CircularGridValues(XOrigin, YOrigin, RadStep, DivN, aAngle);

      ctlXOrigC.setValue(XOrigin.GetValue());
      ctlYOrigC.setValue(YOrigin.GetValue());
      ctlRadStep.setValue(RadStep.GetValue());
      ctlDivision.setValue(DivN.GetValue());
      ctlAngleRot.setValue(aAngle.GetValue());

      Frame mainFrame = SamplesStarter.getFrame();
      CircGridDlg = new JDialog(mainFrame, resGui.getString("DLG_CIRCEDIT"), true);
      CircGridDlg.setSize(180, 200);
      CircGridDlg.getContentPane().setLayout(new BorderLayout());
      CircGridDlg.getContentPane().add(CircGridPane, BorderLayout.CENTER);
      CircGridDlg.getContentPane().add(ButtonsPane, BorderLayout.SOUTH);
      
      Point aLoc = mainFrame.getLocation();
      CircGridDlg.setLocation(aLoc.x + mainFrame.getWidth()/2 - CircGridDlg.getWidth()/2,
                              aLoc.y + mainFrame.getHeight()/2 - CircGridDlg.getHeight()/2);
      CircGridDlg.show();
    }
  }

//=======================================================================//
  public void EraseGrid()
  {
    myViewer.DeactivateGrid();
    gridState = 0;
  }

//=======================================================================//
  public void ActivateGrid(String type)
  {
    if (type.equals("RectLine"))
    {
      myViewer.ActivateGrid(Aspect_GridType.Aspect_GT_Rectangular,
                            Aspect_GridDrawMode.Aspect_GDM_Lines);
      gridState = 1;
    }
    else if (type.equals("RectPoint"))
    {
      myViewer.ActivateGrid(Aspect_GridType.Aspect_GT_Rectangular,
                            Aspect_GridDrawMode.Aspect_GDM_Points);
      gridState = 1;
    }
    else if (type.equals("CircLine"))
    {
      myViewer.ActivateGrid(Aspect_GridType.Aspect_GT_Circular,
                            Aspect_GridDrawMode.Aspect_GDM_Lines);
      gridState = 2;
    }
    else if (type.equals("CircPoint"))
    {
      myViewer.ActivateGrid(Aspect_GridType.Aspect_GT_Circular,
                            Aspect_GridDrawMode.Aspect_GDM_Points);
      gridState = 2;
    }
  }

//=======================================================================//
//   public void paint(Graphics g)
//   {
//     if (myView != null)
//     {
//       if (needResize)
//       {
//         myView.MustBeResized(V2d_TypeOfWindowResizingEffect.V2d_TOWRE_ENLARGE_OBJECTS);
//         needResize = false;
//       }
//       myView.Update();
//     }
//     super.paint(g);
//   }
  public native void paint(Graphics g);

//=======================================================================//
  public V2d_View getView()
  {
    return myView;
  }


//=======================================================================//
//                             ActionListener                            //
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String nameAction = e.getActionCommand();
    if (nameAction.equals("CloseGridDlg"))
    {
      if (gridState == 1)
      {
        RectGridDlg.dispose();
        RectGridDlg = null;
      }
      else if (gridState == 2)
      {
        CircGridDlg.dispose();
        CircGridDlg = null;
      }
    }
    else
      super.actionPerformed(e);
  }

//=======================================================================//
//                             FocusListener                            //
//=======================================================================//
  public void focusGained(FocusEvent e)
  {
  }

  public void focusLost(FocusEvent e)
  {
    if (RectGridDlg != null)
      myViewer.SetRectangularGridValues(ctlXOrigin.getValue(),
                                        ctlYOrigin.getValue(),
                                        ctlXStep.getValue(),
                                        ctlYStep.getValue(),
                                        ctlAngle.getValue());
    if (CircGridDlg != null)
      myViewer.SetCircularGridValues(ctlXOrigC.getValue(),
                                     ctlYOrigC.getValue(),
                                     ctlRadStep.getValue(),
                                     ctlDivision.getValue(),
                                     ctlAngleRot.getValue());
  }


} 
