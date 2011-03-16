
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

public class SampleTopologyBuildingPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleTopologyBuilding");

  static protected ImageIcon imgVertex = new ImageIcon(resGui.getString("Icon-Vertex"));
  static protected ImageIcon imgEdge = new ImageIcon(resGui.getString("Icon-Edge"));
  static protected ImageIcon imgWire = new ImageIcon(resGui.getString("Icon-Wire"));
  static protected ImageIcon imgFace = new ImageIcon(resGui.getString("Icon-Face"));
  static protected ImageIcon imgShell = new ImageIcon(resGui.getString("Icon-Shell"));
  static protected ImageIcon imgCompound = new ImageIcon(resGui.getString("Icon-Compound"));
  static protected ImageIcon imgSewing = new ImageIcon(resGui.getString("Icon-Sewing"));
  static protected ImageIcon imgBuilder = new ImageIcon(resGui.getString("Icon-Builder"));
  static protected ImageIcon imgGeometry = new ImageIcon(resGui.getString("Icon-Geometry"));
  static protected ImageIcon imgExplorer = new ImageIcon(resGui.getString("Icon-Explorer"));


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
  public SampleTopologyBuildingPanel()
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
    myViewer3d = SamplesTopologyPackage.CreateViewer("TopologyBuilding");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleTopologyBuildingPanel.getViewer3d()) {
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

    button = new HeavyToggleButton(imgVertex, false);
    button.setToolTipText(resGui.getString("TT-Vertex"));
    button.setActionCommand("Vertex");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgEdge, false);
    button.setToolTipText(resGui.getString("TT-Edge"));
    button.setActionCommand("Edge");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgWire, false);
    button.setToolTipText(resGui.getString("TT-Wire"));
    button.setActionCommand("Wire");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgFace, false);
    button.setToolTipText(resGui.getString("TT-Face"));
    button.setActionCommand("Face");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgShell, false);
    button.setToolTipText(resGui.getString("TT-Shell"));
    button.setActionCommand("Shell");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgCompound, false);
    button.setToolTipText(resGui.getString("TT-Compound"));
    button.setActionCommand("Compound");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyToggleButton(imgSewing, false);
    button.setToolTipText(resGui.getString("TT-Sewing"));
    button.setActionCommand("Sewing");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgBuilder, false);
    button.setToolTipText(resGui.getString("TT-Builder"));
    button.setActionCommand("Builder");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgGeometry, false);
    button.setToolTipText(resGui.getString("TT-Geometry"));
    button.setActionCommand("Geometry");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgExplorer, false);
    button.setToolTipText(resGui.getString("TT-Explorer"));
    button.setActionCommand("Explorer");
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
  public void onVertex()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Vertex(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Vertex"));
  }


//=======================================================================//
  public void onEdge()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Edge(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Edge"));
  }


//=======================================================================//
  public void onWire()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Wire(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Wire"));
  }


//=======================================================================//
  public void onFace()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Face(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Face"));
  }


//=======================================================================//
  public void onShell()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Shell(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Shell"));
  }


//=======================================================================//
  public void onCompound()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Compound(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Compound"));
  }


//=======================================================================//
  public void onSewing()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Sewing(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Sewing"));
  }


//=======================================================================//
  public void onBuilder()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Builder(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Builder"));
  }


//=======================================================================//
  public void onGeometry()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Geometry(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Geometry"));
  }


//=======================================================================//
  public void onExplorer()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Explorer(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Explorer"));
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Vertex")) onVertex();
    else if (command.equals("Edge")) onEdge();
    else if (command.equals("Wire")) onWire();
    else if (command.equals("Face")) onFace();
    else if (command.equals("Shell")) onShell();
    else if (command.equals("Compound")) onCompound();
    else if (command.equals("Sewing")) onSewing();
    else if (command.equals("Builder")) onBuilder();
    else if (command.equals("Geometry")) onGeometry();
    else if (command.equals("Explorer")) onExplorer();
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

