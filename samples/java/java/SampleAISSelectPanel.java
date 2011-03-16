
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
import SampleAISSelectJni.*;
import util.*;

public class SampleAISSelectPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleAISSelect");

  static private ImageIcon imgBox = new ImageIcon(resGui.getString("Icon-Box"));
  static private ImageIcon imgCylinder = new ImageIcon(resGui.getString("Icon-Cylinder"));
  static private ImageIcon imgVertices = new ImageIcon(resGui.getString("Icon-Vertices"));
  static private ImageIcon imgEdges = new ImageIcon(resGui.getString("Icon-Edges"));
  static private ImageIcon imgFaces = new ImageIcon(resGui.getString("Icon-Faces"));
  static private ImageIcon imgNeutral = new ImageIcon(resGui.getString("Icon-Neutral"));
  static private ImageIcon imgFillet = new ImageIcon(resGui.getString("Icon-Fillet"));

  private HTMLFrame myHtmlFrame = null;
  private PopupMenu myPopup;

  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private ViewPanel myView3d;

  private static V3d_Viewer myViewer3d = null;

  private static AIS_InteractiveContext myAISContext = null;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;

  
  //-----------------------------------------------------------//
  // Changing face color
  //-----------------------------------------------------------//
  private static final int CurAction3d_Default = 0;
  private static final int CurAction3d_SelectFace = 1;

  private int myCurrentMode = CurAction3d_Default;


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
  public SampleAISSelectPanel()
  {
    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    if (myAISContext == null)
      myAISContext = new AIS_InteractiveContext(myViewer3d);


    // Creation popup
    //------------------------------------------
    myPopup = new PopupMenu("User cylinder");

    MenuItem menuItem = new MenuItem("Change face color...");
    menuItem.setActionCommand("ChangeFaceColor");
    menuItem.addActionListener(this);
    myPopup.add(menuItem);
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));


    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleAISSelectPackage.CreateViewer("AISSelect");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleAISSelectPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SampleAISSelectPackage.SetWindow(view, hiwin, lowin);
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

    tools.addSeparator();;

    button = new HeavyButton(imgVertices);
    button.setToolTipText(resGui.getString("Help-Vertices"));
    button.setActionCommand("Vertices");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgEdges);
    button.setToolTipText(resGui.getString("Help-Edges"));
    button.setActionCommand("Edges");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgFaces);
    button.setToolTipText(resGui.getString("Help-Faces"));
    button.setActionCommand("Faces");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgNeutral);
    button.setToolTipText(resGui.getString("Help-Neutral"));
    button.setActionCommand("Neutral");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();;

    button = new HeavyButton(imgFillet);
    button.setToolTipText(resGui.getString("Help-Fillet"));
    button.setActionCommand("Fillet");
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
    SampleAISSelectPackage.DisplayBox(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Box"));
  }

//=======================================================================//
  public void onCylinder()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISSelectPackage.DisplayCylinder(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Cylinder"));
  }

//=======================================================================//
  public void onVertices()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISSelectPackage.SelectVertices(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Vertices"));
  }

//=======================================================================//
  public void onEdges()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISSelectPackage.SelectEdges(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Edges"));
  }

//=======================================================================//
  public void onFaces()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISSelectPackage.SelectFaces(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Faces"));
  }

//=======================================================================//
  public void onNeutral()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISSelectPackage.SelectNeutral(myAISContext, message);

    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Neutral"));
  }

//=======================================================================//
  public void onFillet()
  {
    if (!myAISContext.HasOpenedContext())
    {
      JOptionPane.showMessageDialog(this,
           "It is necessary to activate the edges selection mode\n" +
           "and select edges on an object before\n" +
           "running this function",
           "Warning!!!", JOptionPane.WARNING_MESSAGE);
      return;
    }

    myAISContext.InitSelected();
    if (myAISContext.MoreSelected())
    {
      RadiusDlg aDlg = new RadiusDlg(SamplesStarter.getFrame(), 10.);
      Position.centerWindow(aDlg);
      aDlg.show();

      if (aDlg.isOK())
      {
      	TCollection_AsciiString message = new TCollection_AsciiString();
      	int result = SampleAISSelectPackage.MakeFillet(myAISContext, aDlg.getValue(),
                                        				       message);
      	if (result == 2)
          JOptionPane.showMessageDialog(this,
               "It is necessary to activate the edges selection mode\n" +
               "and select edges on an object before\n" +
               "running this function",
               "Warning!!!", JOptionPane.WARNING_MESSAGE);
      	else if (result == 1)
          JOptionPane.showMessageDialog(this, "Error during fillet computation",
                                        "Warning!!!", JOptionPane.WARNING_MESSAGE);
      	else if (result == 0)
      	  traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Fillet"));
      }
    }
    else
      JOptionPane.showMessageDialog(this,
           "It is necessary to activate the edges selection mode\n" +
           "and select edges on an object before\n" +
           "running this function",
           "Warning!!!", JOptionPane.WARNING_MESSAGE);
  }

//=======================================================================//
  public void onChangeFaceColor()
  {
    SampleAISSelectPackage.StartSelectFace(myAISContext);
    myCurrentMode = CurAction3d_SelectFace;
    SamplesStarter.put_info("Select the face");
  }

//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Box")) onBox();
    else if (command.equals("Cylinder")) onCylinder();
    else if (command.equals("Vertices")) onVertices();
    else if (command.equals("Edges")) onEdges();
    else if (command.equals("Faces")) onFaces();
    else if (command.equals("Neutral")) onNeutral();
    else if (command.equals("Fillet")) onFillet();
    else if (command.equals("ChangeFaceColor")) onChangeFaceColor();
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
      Object src = e.getSource();
      if (src == myView3d)
      {
      	if (SampleAISSelectPackage.IsCylinderSelected(myAISContext))
      	{
      	  myView3d.add(myPopup);
      	  myPopup.show(myView3d, e.getX(), e.getY() + 30);
      	}
      	else
      	{
      	  PopupMenu defPopup = myView3d.getDefaultPopup();
      	  myView3d.add(defPopup);
      	  defPopup.show(myView3d, e.getX(), e.getY() + 30);
      	}
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

      // Change the face color
      if (myCurrentMode == CurAction3d_SelectFace)
      {
      	myAISContext.InitSelected();
      	if (myAISContext.MoreSelected())
      	{
      	  Quantity_Color aColor = SampleAISSelectPackage.GetFaceColor(myAISContext);
      	  int red = (int) (aColor.Red()*255);
      	  int green = (int) (aColor.Green()*255);
      	  int blue = (int) (aColor.Blue()*255);
      	  Color theColor = new Color(red, green, blue);

      	  Color theNewColor = JColorChooser.showDialog(SamplesStarter.getFrame(),
						       "Choose the color", theColor);
	  
      	  if (theNewColor != null)
      	  {
      	    Quantity_Color aNewColor = new Quantity_Color(theNewColor.getRed()/255.,
							  theNewColor.getGreen()/255.,
							  theNewColor.getBlue()/255.,
							  Quantity_TypeOfColor.Quantity_TOC_RGB);
      	    TCollection_AsciiString message = new TCollection_AsciiString();
      	    SampleAISSelectPackage.SetFaceColor(myAISContext, aNewColor, message);

      	    traceMessage(message.ToCString().GetValue(), resGui.getString("Dlg-Color"));
      	  }

      	  SampleAISSelectPackage.EndSelectFace(myAISContext);
      	  myCurrentMode = CurAction3d_Default;
      	  SamplesStarter.put_info("");
      	}
      }
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

