
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

public class SampleLocalOperationsPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleLocalOperations");

  static protected ImageIcon imgPrism = new ImageIcon(resGui.getString("Icon-Prism"));
  static protected ImageIcon imgDPrism = new ImageIcon(resGui.getString("Icon-DPrism"));
  static protected ImageIcon imgRevol = new ImageIcon(resGui.getString("Icon-Revol"));
  static protected ImageIcon imgPipe = new ImageIcon(resGui.getString("Icon-Pipe"));
  static protected ImageIcon imgRib = new ImageIcon(resGui.getString("Icon-Rib"));
  static protected ImageIcon imgGlue = new ImageIcon(resGui.getString("Icon-Glue"));
  static protected ImageIcon imgSplit = new ImageIcon(resGui.getString("Icon-Split"));
  static protected ImageIcon imgThick = new ImageIcon(resGui.getString("Icon-Thick"));
  static protected ImageIcon imgOffset = new ImageIcon(resGui.getString("Icon-Offset"));


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
  public SampleLocalOperationsPanel()
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
    myViewer3d = SamplesTopologyPackage.CreateViewer("LocalOperations");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleLocalOperationsPanel.getViewer3d()) {
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

    button = new HeavyToggleButton(imgPrism, false);
    button.setToolTipText(resGui.getString("TT-Prism"));
    button.setActionCommand("Prism");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgDPrism, false);
    button.setToolTipText(resGui.getString("TT-DPrism"));
    button.setActionCommand("DPrism");
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

    button = new HeavyToggleButton(imgRib, false);
    button.setToolTipText(resGui.getString("TT-Rib"));
    button.setActionCommand("Rib");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgGlue, false);
    button.setToolTipText(resGui.getString("TT-Glue"));
    button.setActionCommand("Glue");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgSplit, false);
    button.setToolTipText(resGui.getString("TT-Split"));
    button.setActionCommand("Split");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgThick, false);
    button.setToolTipText(resGui.getString("TT-Thick"));
    button.setActionCommand("Thick");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgOffset, false);
    button.setToolTipText(resGui.getString("TT-Offset"));
    button.setActionCommand("Offset");
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
  public void onPrism()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.LocalPrism(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Prism"));
  }


//=======================================================================//
  public void onDPrism()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.LocalDPrism(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-DPrism"));
  }


//=======================================================================//
  public void onRevol()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.LocalRevol(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Revol"));
  }


//=======================================================================//
  public void onPipe()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.LocalPipe(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Pipe"));
  }


//=======================================================================//
  public void onRib()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Rib(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Rib"));
  }


//=======================================================================//
  public void onGlue()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Glue(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Glue"));
  }


//=======================================================================//
  public void onSplit()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Split(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Split"));
  }


//=======================================================================//
  public void onThick()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Thick(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Thick"));
  }


//=======================================================================//
  public void onOffset()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Offset(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Offset"));
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Prism")) onPrism();
    else if (command.equals("DPrism")) onDPrism();
    else if (command.equals("Revol")) onRevol();
    else if (command.equals("Pipe")) onPipe();
    else if (command.equals("Rib")) onRib();
    else if (command.equals("Glue")) onGlue();
    else if (command.equals("Split")) onSplit();
    else if (command.equals("Thick")) onThick();
    else if (command.equals("Offset")) onOffset();
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

