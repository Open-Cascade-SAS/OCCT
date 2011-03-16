
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
import jcas.Standard_Integer;
import CASCADESamplesJni.*;
import SampleAISDisplayModeJni.*;
import util.*;

public class SampleAISDisplayModePanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleAISDisplayMode");

  static private ImageIcon imgBox = new ImageIcon(resGui.getString("Icon-Box"));
  static private ImageIcon imgSphere = new ImageIcon(resGui.getString("Icon-Sphere"));
  static private ImageIcon imgCylinder = new ImageIcon(resGui.getString("Icon-Cylinder"));
  static private ImageIcon imgWireframe = new ImageIcon(resGui.getString("Icon-Wireframe"));
  static private ImageIcon imgShading = new ImageIcon(resGui.getString("Icon-Shading"));
  static private ImageIcon imgIsos = new ImageIcon(resGui.getString("Icon-Isos"));
  static private ImageIcon imgEraseAll = new ImageIcon(resGui.getString("Icon-EraseAll"));
  static private ImageIcon imgLight = new ImageIcon(resGui.getString("Icon-Light"));

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
  // Light Creation
  //-----------------------------------------------------------//
  private static final int CurAction3d_Nothing = 0;
  private static final int CurAction3d_FirstPointLight = 1;
  private static final int CurAction3d_SecondPointLight = 2;

  private int myCurrentMode = CurAction3d_Nothing;
  private V3d_View myView = null;


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
  public SampleAISDisplayModePanel()
  {
    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    SampleAISDisplayModePackage.InitContext(myAISContext);
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));


    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleAISDisplayModePackage.CreateViewer("AISDisplayMode");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleAISDisplayModePanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SampleAISDisplayModePackage.SetWindow(view, hiwin, lowin);
              }
          };
      	}
    };
   
    myView3d.addMouseListener(this);
    myView3d.addMouseMotionListener(this);


    // Attributes
    //------------------------------------------
    myAISContext = new AIS_InteractiveContext(myViewer3d);
      
    AttributesPanel attrib = new AttributesPanel(this, myAISContext);


    // Layout
    //------------------------------------------
    mainPanel.add(attrib, new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(0, 0, 0, 0), 0, 0));

    mainPanel.add(myView3d, new GridBagConstraints(1, 0, 1, 1, 1.0, 1.0,
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

    button = new HeavyButton(imgBox);
    button.setToolTipText(resGui.getString("Help-Box"));
    button.setActionCommand("Box");
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

    tools.addSeparator();;

    button = new HeavyButton(imgWireframe);
    button.setToolTipText(resGui.getString("Help-Wireframe"));
    button.setActionCommand("Wireframe");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgShading);
    button.setToolTipText(resGui.getString("Help-Shading"));
    button.setActionCommand("Shading");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgIsos);
    button.setToolTipText(resGui.getString("Help-Isos"));
    button.setActionCommand("Isos");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgEraseAll);
    button.setToolTipText(resGui.getString("Help-EraseAll"));
    button.setActionCommand("EraseAll");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgLight);
    button.setToolTipText(resGui.getString("Help-Light"));
    button.setActionCommand("Light");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    return tools;
  }


//=======================================================================//
//                              Actions                                  //
//=======================================================================//
  public void onBox()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.DisplayBox(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Box"));
  }

//=======================================================================//
  public void onSphere()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.DisplaySphere(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Sphere"));
  }

//=======================================================================//
  public void onCylinder()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.DisplayCylinder(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Cylinder"));
  }

//=======================================================================//
  public void onEraseAll()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.EraseAll(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-EraseAll"));
  }

//=======================================================================//
  public void onWireframe()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetDisplayMode(myAISContext, (short)0, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Wireframe"));
  }

//=======================================================================//
  public void onShading()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetDisplayMode(myAISContext, (short)1, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Shading"));
  }

//=======================================================================//
  public void onIsos()
  {
    Standard_Integer u = new Standard_Integer();
    Standard_Integer v = new Standard_Integer();
    SampleAISDisplayModePackage.GetIsosNumber(myAISContext,u,v);

    IsosDlg aDlg = new IsosDlg(SamplesStarter.getFrame(), u.GetValue(), v.GetValue());
    Position.centerWindow(aDlg);
    aDlg.show();

    if (aDlg.isOK())
    {
      TCollection_AsciiString message = new TCollection_AsciiString();
      SampleAISDisplayModePackage.SetIsosNumber(myAISContext, 
                          aDlg.getUValue(), aDlg.getVValue(), message);

      traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Shading"));
    }
  }

//=======================================================================//
  public void onLight()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    SampleAISDisplayModePackage.CreateLight(myAISContext);

    myCurrentMode = CurAction3d_FirstPointLight;
    SamplesStarter.put_info("Pick a first point");
  }

//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Box")) onBox();
    else if (command.equals("Sphere")) onSphere();
    else if (command.equals("Cylinder")) onCylinder();
    else if (command.equals("Wireframe")) onWireframe();
    else if (command.equals("Shading")) onShading();
    else if (command.equals("Isos")) onIsos();
    else if (command.equals("EraseAll")) onEraseAll();
    else if (command.equals("Light")) onLight();
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

      switch (myCurrentMode)
      {
        case CurAction3d_FirstPointLight:
	  SampleAISDisplayModePackage.SetFirstPointOfLight(myAISContext, myView,
							   e.getX(), e.getY());
      	  myCurrentMode = CurAction3d_SecondPointLight;
          SamplesStarter.put_info("Pick the second point");
      	  break;

        case CurAction3d_SecondPointLight:
      	  SampleAISDisplayModePackage.SetSecondPointOfLight(myAISContext);
      	  myCurrentMode = CurAction3d_Nothing;
          SamplesStarter.put_info("Ready");
      	  break;

        case CurAction3d_Nothing:
          if (src == myView3d)
          {
            V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
            if (view3d != null)
              myAISContext.MoveTo(startX, startY, view3d);
          }
      	  break;
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
    if (SwingUtilities.isLeftMouseButton(e) &&
       (myCurrentMode == CurAction3d_Nothing))
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
       (myCurrentMode == CurAction3d_Nothing) &&
       (e.getSource() == myView3d))
      Dragged = true;
  }

  public void mouseMoved(MouseEvent e)
  {
    switch (myCurrentMode)
    {
      case CurAction3d_SecondPointLight:
    	  SampleAISDisplayModePackage.MoveSecondPointOfLight(myAISContext, myView,
      							     e.getX(), e.getY());
    	  break;

      case CurAction3d_Nothing:
        if (e.getSource() == myView3d)
        {
          V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
          if (myAISContext != null && view3d != null)
            myAISContext.MoveTo(e.getX(), e.getY(), view3d);
        }
    }
  }


}

