
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

public class SampleTopologicalOperationsPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleTopologicalOperations");

  static protected ImageIcon imgCut = new ImageIcon(resGui.getString("Icon-Cut"));
  static protected ImageIcon imgFuse = new ImageIcon(resGui.getString("Icon-Fuse"));
  static protected ImageIcon imgCommon = new ImageIcon(resGui.getString("Icon-Common"));
  static protected ImageIcon imgSection = new ImageIcon(resGui.getString("Icon-Section"));
  static protected ImageIcon imgPSection = new ImageIcon(resGui.getString("Icon-PSection"));
  static protected ImageIcon imgBlend = new ImageIcon(resGui.getString("Icon-Blend"));
  static protected ImageIcon imgEvolvedBlend = new ImageIcon(resGui.getString("Icon-EvolvedBlend"));
  static protected ImageIcon imgChamfer = new ImageIcon(resGui.getString("Icon-Chamfer"));


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
  public SampleTopologicalOperationsPanel()
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
    myViewer3d = SamplesTopologyPackage.CreateViewer("TopologicalOperations");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleTopologicalOperationsPanel.getViewer3d()) {
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

    button = new HeavyToggleButton(imgCut, false);
    button.setToolTipText(resGui.getString("TT-Cut"));
    button.setActionCommand("Cut");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgFuse, false);
    button.setToolTipText(resGui.getString("TT-Fuse"));
    button.setActionCommand("Fuse");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgCommon, false);
    button.setToolTipText(resGui.getString("TT-Common"));
    button.setActionCommand("Common");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgSection, false);
    button.setToolTipText(resGui.getString("TT-Section"));
    button.setActionCommand("Section");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgPSection, false);
    button.setToolTipText(resGui.getString("TT-PSection"));
    button.setActionCommand("PSection");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyToggleButton(imgBlend, false);
    button.setToolTipText(resGui.getString("TT-Blend"));
    button.setActionCommand("Blend");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgEvolvedBlend, false);
    button.setToolTipText(resGui.getString("TT-EvolvedBlend"));
    button.setActionCommand("EvolvedBlend");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgChamfer, false);
    button.setToolTipText(resGui.getString("TT-Chamfer"));
    button.setActionCommand("Chamfer");
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
  public void onCut()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Cut(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Cut"));
  }


//=======================================================================//
  public void onFuse()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Fuse(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Fuse"));
  }


//=======================================================================//
  public void onCommon()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Common(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Common"));
  }


//=======================================================================//
  public void onSection()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Section(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Section"));
  }


//=======================================================================//
  public void onPSection()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.PSection(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-PSection"));
  }


//=======================================================================//
  public void onBlend()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Blend(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Blend"));
  }


//=======================================================================//
  public void onEvolvedBlend()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.EvolvedBlend(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-EvolvedBlend"));
  }


//=======================================================================//
  public void onChamfer()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SamplesTopologyPackage.Chamfer(myAISContext, message);

    postProcess(message, resGui.getString("Dlg-Chamfer"));
  }

  
//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Cut")) onCut();
    else if (command.equals("Fuse")) onFuse();
    else if (command.equals("Common")) onCommon();
    else if (command.equals("Section")) onSection();
    else if (command.equals("PSection")) onPSection();
    else if (command.equals("Blend")) onBlend();
    else if (command.equals("EvolvedBlend")) onEvolvedBlend();
    else if (command.equals("Chamfer")) onChamfer();
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

