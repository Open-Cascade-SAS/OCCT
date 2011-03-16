
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
import SamplesTopologyJni.*;
import util.*;

public class SampleTopologyPrimitivesPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleTopologyPrimitives");

  static protected ImageIcon imgBox = new ImageIcon(resGui.getString("Icon-Box"));
  static protected ImageIcon imgCylinder = new ImageIcon(resGui.getString("Icon-Cylinder"));
  static protected ImageIcon imgCone = new ImageIcon(resGui.getString("Icon-Cone"));
  static protected ImageIcon imgSphere = new ImageIcon(resGui.getString("Icon-Sphere"));
  static protected ImageIcon imgTorus = new ImageIcon(resGui.getString("Icon-Torus"));
  static protected ImageIcon imgWedge = new ImageIcon(resGui.getString("Icon-Wedge"));
  static protected ImageIcon imgPrism = new ImageIcon(resGui.getString("Icon-Prism"));
  static protected ImageIcon imgRevol = new ImageIcon(resGui.getString("Icon-Revol"));
  static protected ImageIcon imgPipe = new ImageIcon(resGui.getString("Icon-Pipe"));
  static protected ImageIcon imgThru = new ImageIcon(resGui.getString("Icon-Thru"));
  static protected ImageIcon imgEvolved = new ImageIcon(resGui.getString("Icon-Evolved"));
  static protected ImageIcon imgDraft = new ImageIcon(resGui.getString("Icon-Draft"));


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
  public SampleTopologyPrimitivesPanel()
  {
    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    if (myAISContext == null)
    {
      myAISContext = new AIS_InteractiveContext(myViewer3d);
      myAISContext.SetDisplayMode(AIS_DisplayMode.AIS_Shaded, false);
    }
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));

    // Viewer 3D
    //------------------------------------------
    myViewer3d = SamplesTopologyPackage.CreateViewer("TopologyPrimitives");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleTopologyPrimitivesPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SamplesTopologyPackage.SetWindow(view, hiwin, lowin);
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

    ButtonGroup group = new ButtonGroup();
    Insets margin = new Insets(1, 1, 1, 1);
    JToggleButton button;

    button = new HeavyToggleButton(imgBox, false);
    button.setToolTipText(resGui.getString("TT-Box"));
    button.setActionCommand("Box");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgCylinder, false);
    button.setToolTipText(resGui.getString("TT-Cylinder"));
    button.setActionCommand("Cylinder");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgCone, false);
    button.setToolTipText(resGui.getString("TT-Cone"));
    button.setActionCommand("Cone");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgSphere, false);
    button.setToolTipText(resGui.getString("TT-Sphere"));
    button.setActionCommand("Sphere");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTorus, false);
    button.setToolTipText(resGui.getString("TT-Torus"));
    button.setActionCommand("Torus");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgWedge, false);
    button.setToolTipText(resGui.getString("TT-Wedge"));
    button.setActionCommand("Wedge");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyToggleButton(imgPrism, false);
    button.setToolTipText(resGui.getString("TT-Prism"));
    button.setActionCommand("Prism");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgRevol, false);
    button.setToolTipText(resGui.getString("TT-Revol"));
    button.setActionCommand("Revol");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgPipe, false);
    button.setToolTipText(resGui.getString("TT-Pipe"));
    button.setActionCommand("Pipe");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgThru, false);
    button.setToolTipText(resGui.getString("TT-Thru"));
    button.setActionCommand("Thru");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgEvolved, false);
    button.setToolTipText(resGui.getString("TT-Evolved"));
    button.setActionCommand("Evolved");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyToggleButton(imgDraft, false);
    button.setToolTipText(resGui.getString("TT-Draft"));
    button.setActionCommand("Draft");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    return tools;
  }


//=======================================================================//
//                              Actions                                  //
//=======================================================================//
  public void postProcess(TCollection_AsciiString message, String title)
  {
    myView3d.getViewPort().FitAll();

    String text = message.ToCString().GetValue();
    text += "\n-------------------- END ----------------------\n";

    traceMessage(text, title);
  }

//=======================================================================//
// Tests
//=======================================================================//
  public void onBox()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeBox(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Box"));
  }


//=======================================================================//
  public void onCylinder()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeCylinder(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Cylinder"));
  }


//=======================================================================//
  public void onCone()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeCone(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Cone"));
  }


//=======================================================================//
  public void onSphere()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeSphere(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Sphere"));
  }


//=======================================================================//
  public void onTorus()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeTorus(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Torus"));
  }


//=======================================================================//
  public void onWedge()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeWedge(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Wedge"));
  }


//=======================================================================//
  public void onPrism()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakePrism(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Prism"));
  }


//=======================================================================//
  public void onRevol()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeRevol(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Revol"));
  }


//=======================================================================//
  public void onPipe()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakePipe(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Pipe"));
  }


//=======================================================================//
  public void onThru()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeThru(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Thru"));
  }

  
//=======================================================================//
  public void onEvolved()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeEvolved(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Evolved"));
  }


//=======================================================================//
  public void onDraft()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.MakeDraft(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Draft"));
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Box")) onBox();
    else if (command.equals("Cylinder")) onCylinder();
    else if (command.equals("Cone")) onCone();
    else if (command.equals("Sphere")) onSphere();
    else if (command.equals("Torus")) onTorus();
    else if (command.equals("Wedge")) onWedge();
    else if (command.equals("Prism")) onPrism();
    else if (command.equals("Revol")) onRevol();
    else if (command.equals("Pipe")) onPipe();
    else if (command.equals("Thru")) onThru();
    else if (command.equals("Evolved")) onEvolved();
    else if (command.equals("Draft")) onDraft();
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

