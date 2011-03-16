
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

public class SampleTopologyTransformationsPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleTopologyTransformations");

  static protected ImageIcon imgMirror = new ImageIcon(resGui.getString("Icon-Mirror"));
  static protected ImageIcon imgMirroraxis = new ImageIcon(resGui.getString("Icon-Mirroraxis"));
  static protected ImageIcon imgRotate = new ImageIcon(resGui.getString("Icon-Rotate"));
  static protected ImageIcon imgScale = new ImageIcon(resGui.getString("Icon-Scale"));
  static protected ImageIcon imgTranslation = new ImageIcon(resGui.getString("Icon-Translation"));
  static protected ImageIcon imgDisplacement = new ImageIcon(resGui.getString("Icon-Displacement"));
  static protected ImageIcon imgDeform = new ImageIcon(resGui.getString("Icon-Deform"));


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
  public SampleTopologyTransformationsPanel()
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
    myViewer3d = SamplesTopologyPackage.CreateViewer("TopologyTransformations");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleTopologyTransformationsPanel.getViewer3d()) {
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

    button = new HeavyToggleButton(imgMirror, false);
    button.setToolTipText(resGui.getString("TT-Mirror"));
    button.setActionCommand("Mirror");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgMirroraxis, false);
    button.setToolTipText(resGui.getString("TT-Mirroraxis"));
    button.setActionCommand("Mirroraxis");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgRotate, false);
    button.setToolTipText(resGui.getString("TT-Rotate"));
    button.setActionCommand("Rotate");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgScale, false);
    button.setToolTipText(resGui.getString("TT-Scale"));
    button.setActionCommand("Scale");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTranslation, false);
    button.setToolTipText(resGui.getString("TT-Translation"));
    button.setActionCommand("Translation");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgDisplacement, false);
    button.setToolTipText(resGui.getString("TT-Displacement"));
    button.setActionCommand("Displacement");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgDeform, false);
    button.setToolTipText(resGui.getString("TT-Deform"));
    button.setActionCommand("Deform");
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
  public void onMirror()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Mirror(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Mirror"));
  }

//=======================================================================//
  public void onMirroraxis()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Mirroraxis(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Mirroraxis"));
  }

//=======================================================================//
  public void onRotate()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Rotate(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Rotate"));
  }


//=======================================================================//
  public void onScale()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Scale(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Scale"));
  }


//=======================================================================//
  public void onTranslation()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Translation(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Translation"));
  }


//=======================================================================//
  public void onDisplacement()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Displacement(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Displacement"));
  }


//=======================================================================//
  public void onDeform()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Deform(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Deform"));
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Mirror")) onMirror();
    else if (command.equals("Mirroraxis")) onMirroraxis();
    else if (command.equals("Rotate")) onRotate();
    else if (command.equals("Scale")) onScale();
    else if (command.equals("Translation")) onTranslation();
    else if (command.equals("Displacement")) onDisplacement();
    else if (command.equals("Deform")) onDeform();
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

