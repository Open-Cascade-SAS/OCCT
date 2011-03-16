
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
import SampleViewer3DJni.*;
import util.*;

public class SampleViewer3DPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleViewer3D");

  static private ImageIcon imgBox = new ImageIcon(resGui.getString("Icon-Box"));
  static private ImageIcon imgCylinder = new ImageIcon(resGui.getString("Icon-Cylinder"));
  static private ImageIcon imgSphere = new ImageIcon(resGui.getString("Icon-Sphere"));
  static private ImageIcon imgEraseAll = new ImageIcon(resGui.getString("Icon-EraseAll"));
  static private ImageIcon imgSpotLight = new ImageIcon(resGui.getString("Icon-SpotLight"));
  static private ImageIcon imgPositionalLight = new ImageIcon(resGui.getString("Icon-PositionalLight"));
  static private ImageIcon imgDirectionalLight = new ImageIcon(resGui.getString("Icon-DirectionalLight"));
  static private ImageIcon imgAmbientLight = new ImageIcon(resGui.getString("Icon-AmbientLight"));
  static private ImageIcon imgClearLights = new ImageIcon(resGui.getString("Icon-ClearLights"));
  static private ImageIcon imgZClipping = new ImageIcon(resGui.getString("Icon-ZClipping"));
  static private ImageIcon imgZCueing = new ImageIcon(resGui.getString("Icon-ZCueing"));
  static private ImageIcon imgShadingModel = new ImageIcon(resGui.getString("Icon-ShadingModel"));
  static private ImageIcon imgAntialiasing = new ImageIcon(resGui.getString("Icon-Antialiasing"));
  static private ImageIcon imgModelClipping = new ImageIcon(resGui.getString("Icon-ModelClipping"));


  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private ViewPanel myView3d;

  private static Graphic3d_GraphicDriver myGraphicDriver = null;
  private static V3d_Viewer myViewer3d = null;
  private static AIS_InteractiveContext myAISContext = null;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;

  
  //-----------------------------------------------------------//
  // Actions
  //-----------------------------------------------------------//
  private static final int CurAction3d_Nothing = 0;
  private static final int CurAction3d_BeginSpotLight = 1;
  private static final int CurAction3d_TargetSpotLight = 2;
  private static final int CurAction3d_EndSpotLight = 3;
  private static final int CurAction3d_BeginPositionalLight = 4;
  private static final int CurAction3d_BeginDirectionalLight = 5;
  private static final int CurAction3d_EndDirectionalLight = 6;

  private int myCurrentMode = CurAction3d_Nothing;
  private V3d_View myView = null;
  private int NbActiveLights = 2;  // There are 2 default active lights


  //-----------------------------------------------------------//
  // External access
  //-----------------------------------------------------------//
  public static Graphic3d_GraphicDriver getGraphicDriver()
  {
    return myGraphicDriver;
  }

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
  public SampleViewer3DPanel()
  {
    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    if (myAISContext == null)
      myAISContext = new AIS_InteractiveContext(myViewer3d);
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));

    // Viewer 3D
    //------------------------------------------
    myGraphicDriver = SampleViewer3DPackage.CreateGraphicDriver();
    myViewer3d = SampleViewer3DPackage.CreateViewer("Viewer3D");

    myView3d = new ViewPanel() {
    	public ViewCanvas createViewPort()
    	{
    	  return new CASCADEView3d(SampleViewer3DPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SampleViewer3DPackage.SetWindow(view, hiwin, lowin);
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
    JButton button;

    button = new HeavyButton(imgBox);
    button.setToolTipText(resGui.getString("Help-Box"));
    button.setActionCommand("Box");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgCylinder);
    button.setToolTipText(resGui.getString("Help-Cylinder"));
    button.setActionCommand("Cylinder");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgSphere);
    button.setToolTipText(resGui.getString("Help-Sphere"));
    button.setActionCommand("Sphere");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgEraseAll);
    button.setToolTipText(resGui.getString("Help-EraseAll"));
    button.setActionCommand("EraseAll");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgSpotLight);
    button.setToolTipText(resGui.getString("Help-SpotLight"));
    button.setActionCommand("SpotLight");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgPositionalLight);
    button.setToolTipText(resGui.getString("Help-PositionalLight"));
    button.setActionCommand("PositionalLight");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgDirectionalLight);
    button.setToolTipText(resGui.getString("Help-DirectionalLight"));
    button.setActionCommand("DirectionalLight");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgAmbientLight);
    button.setToolTipText(resGui.getString("Help-AmbientLight"));
    button.setActionCommand("AmbientLight");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgClearLights);
    button.setToolTipText(resGui.getString("Help-ClearLights"));
    button.setActionCommand("ClearLights");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgZClipping);
    button.setToolTipText(resGui.getString("Help-ZClipping"));
    button.setActionCommand("ZClipping");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgZCueing);
    button.setToolTipText(resGui.getString("Help-ZCueing"));
    button.setActionCommand("ZCueing");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgShadingModel);
    button.setToolTipText(resGui.getString("Help-ShadingModel"));
    button.setActionCommand("ShadingModel");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgAntialiasing);
    button.setToolTipText(resGui.getString("Help-Antialiasing"));
    button.setActionCommand("Antialiasing");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgModelClipping);
    button.setToolTipText(resGui.getString("Help-ModelClipping"));
    button.setActionCommand("ModelClipping");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    return tools;
  }


//=======================================================================//
// Actions
//=======================================================================//
  public void onBox()
  {
    SampleViewer3DPackage.DisplayBox(myAISContext);
  }

  public void onCylinder()
  {
    SampleViewer3DPackage.DisplayCylinder(myAISContext);
  }

  public void onSphere()
  {
    SampleViewer3DPackage.DisplaySphere(myAISContext);
  }

  public void onEraseAll()
  {
    SampleViewer3DPackage.EraseAll(myAISContext);
  }

//=======================================================================//
  public void onSpotLight()
  {
    if (NbActiveLights >= myGraphicDriver.InquireLightLimit())
    {
      String tmp = new String("You have reach the limit number of active lights ");
      tmp += myGraphicDriver.InquireLightLimit();
      tmp += ".\n";
      tmp += "Clear lights to create new ones.";
      JOptionPane.showMessageDialog(this, tmp);
      return;
    }

    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.CreateSpotLight(myAISContext, message);

    NbActiveLights++;

    myCurrentMode = CurAction3d_BeginSpotLight;
    SamplesStarter.put_info("Pick the light position");

    traceMessage(message.ToCString().GetValue(), "V3d_SpotLight");
  }

//=======================================================================//
  public void onPositionalLight()
  {
    if (NbActiveLights >= myGraphicDriver.InquireLightLimit())
    {
      String tmp = new String("You have reach the limit number of active lights ");
      tmp += myGraphicDriver.InquireLightLimit();
      tmp += ".\n";
      tmp += "Clear lights to create new ones.";
      JOptionPane.showMessageDialog(this, tmp);
      return;
    }

    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.CreatePositionalLight(myAISContext, myView, message);

    NbActiveLights++;

    myCurrentMode = CurAction3d_BeginPositionalLight;
    SamplesStarter.put_info("Pick the light position");

    traceMessage(message.ToCString().GetValue(), "V3d_PositionalLight");
  }

//=======================================================================//
  public void onDirectionalLight()
  {
    if (NbActiveLights >= myGraphicDriver.InquireLightLimit())
    {
      String tmp = new String("You have reach the limit number of active lights ");
      tmp += myGraphicDriver.InquireLightLimit();
      tmp += ".\n";
      tmp += "Clear lights to create new ones.";
      JOptionPane.showMessageDialog(this, tmp);
      return;
    }

    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.CreateDirectionalLight(myAISContext, message);

    NbActiveLights++;

    myCurrentMode = CurAction3d_BeginDirectionalLight;
    SamplesStarter.put_info("Pick a first point");

    traceMessage(message.ToCString().GetValue(), "V3d_DirectionalLight");
  }

//=======================================================================//
  public void onAmbientLight()
  {
    if (NbActiveLights >= myGraphicDriver.InquireLightLimit())
    {
      String tmp = new String("You have reach the limit number of active lights ");
      tmp += myGraphicDriver.InquireLightLimit();
      tmp += ".\n";
      tmp += "Clear lights to create new ones.";
      JOptionPane.showMessageDialog(this, tmp);
      return;
    }

    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.CreateAmbientLight(myAISContext, myView, message);

    NbActiveLights++;

    traceMessage(message.ToCString().GetValue(), "V3d_AmbientLight");
  }

//=======================================================================//
  public void onClearLights()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ClearLights(myView, message);

    NbActiveLights = 2; // There are 2 default active lights

    traceMessage(message.ToCString().GetValue(), "SetLightOff");
  }

//=======================================================================//
  public void onZClipping()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    ZClippingDlg theDlg = new ZClippingDlg(SamplesStarter.getFrame(), this, myView);
    Position.centerWindow(theDlg);
    theDlg.show();
  }

//=======================================================================//
  public void onZCueing()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    ZCueingDlg theDlg = new ZCueingDlg(SamplesStarter.getFrame(), this, myView);
    Position.centerWindow(theDlg);
    theDlg.show();
  }

//=======================================================================//
  public void onShadingModel()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    String message = new String("\n");
    message += "myView->SetShadingModel(Model);\n";
    message += "myView->Update();\n";
    message += "\n";

    traceMessage(message, "SetShadingModel");

    ShadingModelDlg theDlg = new ShadingModelDlg(SamplesStarter.getFrame(), myView);
    Position.centerWindow(theDlg);
    theDlg.show();
  }

//=======================================================================//
  public void onAntialiasing()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ChangeAntialiasing(myView, message);

    traceMessage(message.ToCString().GetValue(), "SetAntialiasingOn/SetAntialiasingOff");
  }

//=======================================================================//
  public void onModelClipping()
  {
    myView = ((CASCADEView3d) myView3d.getViewPort()).getView();

    SampleViewer3DPackage.CreateClippingPlane(myViewer3d);

    ModelClippingDlg theDlg = new ModelClippingDlg(SamplesStarter.getFrame(), this, myView);
    Position.centerWindow(theDlg);
    theDlg.show();
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Box")) onBox();
    else if (command.equals("Cylinder")) onCylinder();
    else if (command.equals("Sphere")) onSphere();
    else if (command.equals("EraseAll")) onEraseAll();

    else if (command.equals("SpotLight")) onSpotLight();
    else if (command.equals("PositionalLight")) onPositionalLight();
    else if (command.equals("DirectionalLight")) onDirectionalLight();
    else if (command.equals("AmbientLight")) onAmbientLight();
    else if (command.equals("ClearLights")) onClearLights();

    else if (command.equals("ZClipping")) onZClipping();
    else if (command.equals("ZCueing")) onZCueing();

    else if (command.equals("ShadingModel")) onShadingModel();
    else if (command.equals("Antialiasing")) onAntialiasing();

    else if (command.equals("ModelClipping")) onModelClipping();
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
      TCollection_AsciiString message = new TCollection_AsciiString();
      switch (myCurrentMode)
      {
        case CurAction3d_BeginPositionalLight:
      	  SampleViewer3DPackage.DirectPositionalLight(myAISContext, myView,
						      e.getX(), e.getY(), message);
	  
	        myCurrentMode = CurAction3d_Nothing;
          SamplesStarter.put_info("Ready");

          traceMessage(message.ToCString().GetValue(), "SetPosition");
          break;

        case CurAction3d_BeginSpotLight:
	        SampleViewer3DPackage.SetSpotLight(myAISContext, myView,
					     e.getX(), e.getY(), message);

          myCurrentMode = CurAction3d_TargetSpotLight;
          SamplesStarter.put_info("Pick the target point");

          traceMessage(message.ToCString().GetValue(), "SetDirection");
          break;

        case CurAction3d_TargetSpotLight:
	        SampleViewer3DPackage.DirectSpotLight(myView, e.getX(), e.getY(), message);

          myCurrentMode = CurAction3d_EndSpotLight;
          SamplesStarter.put_info("Pick a third point (to define the angle)");

          traceMessage(message.ToCString().GetValue(), "SetAngle");
          break;

        case CurAction3d_EndSpotLight:
	        SampleViewer3DPackage.ExpandSpotLight(myAISContext);

	        myCurrentMode = CurAction3d_Nothing;
          SamplesStarter.put_info("Ready");
          break;

        case CurAction3d_BeginDirectionalLight:
	        SampleViewer3DPackage.SetDirectionalLight(myAISContext, myView,
						    e.getX(), e.getY(), message);

          myCurrentMode = CurAction3d_EndDirectionalLight;
          SamplesStarter.put_info("Pick the target point");

          traceMessage(message.ToCString().GetValue(), "SetDirection");
          break;

        case CurAction3d_EndDirectionalLight:
	        SampleViewer3DPackage.DirectDirectionalLight(myAISContext);

	        myCurrentMode = CurAction3d_Nothing;
          SamplesStarter.put_info("Ready");
          break;

        case CurAction3d_Nothing:
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
      case CurAction3d_BeginPositionalLight:
      	SampleViewer3DPackage.DirectingPositionalLight(myView, e.getX(), e.getY());
        break;

      case CurAction3d_TargetSpotLight:
      	SampleViewer3DPackage.DirectingSpotLight(myAISContext, myView,
						 e.getX(), e.getY());
        break;

      case CurAction3d_EndSpotLight:
      	SampleViewer3DPackage.ExpandingSpotLight(myAISContext, myView,
						 e.getX(), e.getY());
        break;

      case CurAction3d_EndDirectionalLight:
      	SampleViewer3DPackage.DirectingDirectionalLight(myAISContext, myView,
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

