
//Title:        OpenCASCADE Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      Matra Datavision
//Description:


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import jcas.Standard_CString;
import CASCADESamplesJni.*;
import SampleAISBasicJni.*;
import util.*;

public class SampleAISBasicPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleAISBasic");

  static private ImageIcon imgCircle = new ImageIcon(resGui.getString("Icon-Circle"));
  static private ImageIcon imgLine = new ImageIcon(resGui.getString("Icon-Line"));
  static private ImageIcon imgSphere = new ImageIcon(resGui.getString("Icon-Sphere"));
  static private ImageIcon imgCylinder = new ImageIcon(resGui.getString("Icon-Cylinder"));

  private HTMLFrame myHtmlFrame = null;

  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private ViewPanel myView3d;

  private static V3d_Viewer myViewer3d = null;

  private static AIS_InteractiveContext myAISContext = null;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;


  //-----------------------------------------------------------//
  // External access
  //-----------------------------------------------------------//
  public static V3d_Viewer getViewer3d()
  {
    return myViewer3d;
  }

  public static AIS_InteractiveContext getAISContext()
  {
    return myAISContext;
  }


//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public SampleAISBasicPanel()
  {
    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    if (myAISContext == null)
      myAISContext = new AIS_InteractiveContext(myViewer3d);

    SampleAISBasicPackage.DisplayTrihedron(myAISContext);
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));

    // Set the local system units
    //------------------------------------------
    UnitsAPI.SetLocalSystem(UnitsAPI_SystemUnits.UnitsAPI_MDTV);
    

    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleAISBasicPackage.CreateViewer("AISBasic");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort() {
      	  return new CASCADEView3d(SampleAISBasicPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SampleAISBasicPackage.SetWindow(view, hiwin, lowin);
              }
          };
      	}
    };
   
    myView3d.addMouseListener(this);
    myView3d.addMouseMotionListener(this);


    // Layout
    //------------------------------------------
    mainPanel.add(myView3d, new GridBagConstraints(0, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));

    return mainPanel;
  }

//-----------------------------------------------------------------------//
  public Component createToolbar()
  {
    JToolBar tools = (JToolBar) super.createToolbar();

    Insets margin = new Insets(1, 1, 1, 1);
    JButton button;

    button = new HeavyButton(imgCircle);
    button.setToolTipText(resGui.getString("Help-Circle"));
    button.setActionCommand("Circle");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgLine);
    button.setToolTipText(resGui.getString("Help-Line"));
    button.setActionCommand("Line");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgSphere);
    button.setToolTipText(resGui.getString("Help-Sphere"));
    button.setActionCommand("Sphere");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgCylinder);
    button.setToolTipText(resGui.getString("Help-Cylinder"));
    button.setActionCommand("Cylinder");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    return tools;
  }


//=======================================================================//
//                              Actions                                  //
//=======================================================================//
  public void onCircle()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISBasicPackage.DisplayCircle(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Circle"));
  }

//=======================================================================//
  public void onLine()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISBasicPackage.DisplayLine(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Line"));
  }

//=======================================================================//
  public void onSphere()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISBasicPackage.DisplaySphere(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Sphere"));
  }

//=======================================================================//
  public void onCylinder()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISBasicPackage.DisplayCylinder(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Cylinder"));
  }

//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Circle")) onCircle();
    else if (command.equals("Line")) onLine();
    else if (command.equals("Sphere")) onSphere();
    else if (command.equals("Cylinder")) onCylinder();
  }

//=======================================================================//
// MouseListener interface
//=======================================================================//
  public void mouseClicked(MouseEvent e)
  {
  }

  public void mousePressed(MouseEvent e)
  {
    if (SwingUtilities.isLeftMouseButton(e))
    {
      startX = e.getX();
      startY = e.getY();
      Object src = e.getSource();

      if (src == myView3d)
      {
        V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
        if (view3d != null)
          myAISContext.MoveTo(startX, startY, view3d);
      }
    }
    else if (SwingUtilities.isRightMouseButton(e))
    {
      if (e.getSource() == myView3d)
      {
      	PopupMenu defPopup = myView3d.getDefaultPopup();
	myView3d.add(defPopup);
	defPopup.show(myView3d, e.getX(), e.getY() + 30);
      }
    }
  }

  public void mouseReleased(MouseEvent e)
  {
    if (SwingUtilities.isLeftMouseButton(e))
    {
      Object src = e.getSource();

      if (Dragged)
      {
        if (src == myView3d)
        {
          V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
          if (view3d != null)
          {
            if (e.isShiftDown())
              myAISContext.ShiftSelect(startX, startY, e.getX(), e.getY(), view3d, true);
            else
              myAISContext.Select(startX, startY, e.getX(), e.getY(), view3d, true);
          }
        }
      }
      else
      {
        if (src == myView3d)
        {
          if (e.isShiftDown())
            myAISContext.ShiftSelect(true);
          else
            myAISContext.Select(true);
        }
      }
      Dragged = false;
    }
  }

  public void mouseEntered(MouseEvent e)
  {
  }

  public void mouseExited(MouseEvent e)
  {
  }

//=======================================================================//
// MouseMotionListener interface
//=======================================================================//
  public void mouseDragged(MouseEvent e)
  {
    if (SwingUtilities.isLeftMouseButton(e) &&
        e.getSource() == myView3d)
      Dragged = true;
  }

  public void mouseMoved(MouseEvent e)
  {
    Object src = e.getSource();

    if (src == myView3d)
    {
      V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
      if (myAISContext != null && view3d != null)
        myAISContext.MoveTo(e.getX(), e.getY(), view3d);
    }
  }


}

