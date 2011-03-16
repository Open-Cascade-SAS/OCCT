
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
import SampleGeometryJni.*;
import util.*;

public class SampleGeometryPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleGeometry");

  static protected ImageIcon imgTest1 = new ImageIcon(resGui.getString("Icon-Test1"));
  static protected ImageIcon imgTest2 = new ImageIcon(resGui.getString("Icon-Test2"));
  static protected ImageIcon imgTest3 = new ImageIcon(resGui.getString("Icon-Test3"));
  static protected ImageIcon imgTest4 = new ImageIcon(resGui.getString("Icon-Test4"));
  static protected ImageIcon imgTest5 = new ImageIcon(resGui.getString("Icon-Test5"));
  static protected ImageIcon imgTest6 = new ImageIcon(resGui.getString("Icon-Test6"));
  static protected ImageIcon imgTest7 = new ImageIcon(resGui.getString("Icon-Test7"));
  static protected ImageIcon imgTest8 = new ImageIcon(resGui.getString("Icon-Test8"));
  static protected ImageIcon imgTest9 = new ImageIcon(resGui.getString("Icon-Test9"));
  static protected ImageIcon imgTest10 = new ImageIcon(resGui.getString("Icon-Test10"));
  static protected ImageIcon imgTest11 = new ImageIcon(resGui.getString("Icon-Test11"));
  static protected ImageIcon imgTest12 = new ImageIcon(resGui.getString("Icon-Test12"));
  static protected ImageIcon imgTest13 = new ImageIcon(resGui.getString("Icon-Test13"));
  static protected ImageIcon imgTest14 = new ImageIcon(resGui.getString("Icon-Test14"));
  static protected ImageIcon imgTest15 = new ImageIcon(resGui.getString("Icon-Test15"));
  static protected ImageIcon imgTest16 = new ImageIcon(resGui.getString("Icon-Test16"));
  static protected ImageIcon imgTest17 = new ImageIcon(resGui.getString("Icon-Test17"));
  static protected ImageIcon imgTest18 = new ImageIcon(resGui.getString("Icon-Test18"));
  static protected ImageIcon imgTest19 = new ImageIcon(resGui.getString("Icon-Test19"));
  static protected ImageIcon imgTest20 = new ImageIcon(resGui.getString("Icon-Test20"));
  static protected ImageIcon imgTest21 = new ImageIcon(resGui.getString("Icon-Test21"));
  static protected ImageIcon imgTest22 = new ImageIcon(resGui.getString("Icon-Test22"));
  static protected ImageIcon imgTest23 = new ImageIcon(resGui.getString("Icon-Test23"));
  static protected ImageIcon imgTest24 = new ImageIcon(resGui.getString("Icon-Test24"));
  static protected ImageIcon imgTest25 = new ImageIcon(resGui.getString("Icon-Test25"));
  static protected ImageIcon imgTest26 = new ImageIcon(resGui.getString("Icon-Test26"));
  static protected ImageIcon imgTest27 = new ImageIcon(resGui.getString("Icon-Test27"));
  static protected ImageIcon imgTest28 = new ImageIcon(resGui.getString("Icon-Test28"));
  static protected ImageIcon imgTest29 = new ImageIcon(resGui.getString("Icon-Test29"));
  static protected ImageIcon imgTest30 = new ImageIcon(resGui.getString("Icon-Test30"));
  static protected ImageIcon imgTest31 = new ImageIcon(resGui.getString("Icon-Test31"));
  static protected ImageIcon imgTest32 = new ImageIcon(resGui.getString("Icon-Test32"));
  static protected ImageIcon imgTest33 = new ImageIcon(resGui.getString("Icon-Test33"));
  static protected ImageIcon imgTest34 = new ImageIcon(resGui.getString("Icon-Test34"));
  static protected ImageIcon imgTest35 = new ImageIcon(resGui.getString("Icon-Test35"));
  static protected ImageIcon imgTest36 = new ImageIcon(resGui.getString("Icon-Test36"));
  static protected ImageIcon imgTest37 = new ImageIcon(resGui.getString("Icon-Test37"));
  static protected ImageIcon imgTest38 = new ImageIcon(resGui.getString("Icon-Test38"));
  static protected ImageIcon imgTest39 = new ImageIcon(resGui.getString("Icon-Test39"));
  static protected ImageIcon imgTest40 = new ImageIcon(resGui.getString("Icon-Test40"));
  static protected ImageIcon imgTest41 = new ImageIcon(resGui.getString("Icon-Test41"));
  static protected ImageIcon imgTest42 = new ImageIcon(resGui.getString("Icon-Test42"));
  static protected ImageIcon imgTest43 = new ImageIcon(resGui.getString("Icon-Test43"));
  static protected ImageIcon imgTest44 = new ImageIcon(resGui.getString("Icon-Test44"));
  static protected ImageIcon imgTest45 = new ImageIcon(resGui.getString("Icon-Test45"));
  static protected ImageIcon imgTest46 = new ImageIcon(resGui.getString("Icon-Test46"));
  static protected ImageIcon imgTest47 = new ImageIcon(resGui.getString("Icon-Test47"));
  static protected ImageIcon imgTest48 = new ImageIcon(resGui.getString("Icon-Test48"));
  static protected ImageIcon imgTest49 = new ImageIcon(resGui.getString("Icon-Test49"));
  static protected ImageIcon imgTest50 = new ImageIcon(resGui.getString("Icon-Test50"));


  static final int ONLY_VIEW3D = 0;
  static final int ONLY_VIEW2D = 1;
  static final int BOTH_VIEW = 2;
  

  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private ViewPanel myView2d;
  private ViewPanel myView3d;

  private static V2d_Viewer myViewer2d = null;
  private static V3d_Viewer myViewer3d = null;
  
  private static ISession2D_InteractiveContext myInteractiveContext = null;
  private static AIS_InteractiveContext myAISContext = null;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;


  //-----------------------------------------------------------//
  // External access
  //-----------------------------------------------------------//
  public static V2d_Viewer getViewer2d()
  {
    return myViewer2d;
  }

  public static V3d_Viewer getViewer3d()
  {
    return myViewer3d;
  }

  public static ISession2D_InteractiveContext getInteractiveContext()
  {
    return myInteractiveContext;
  }

  public static AIS_InteractiveContext getAISContext()
  {
    return myAISContext;
  }


//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public SampleGeometryPanel()
  {
    // 2D Initialization
    //------------------------------------------
    myViewer2d.SetCircularGridValues(0.0, 0.0, 1.0, 8, 0.0);
    myViewer2d.SetRectangularGridValues(0.0, 0.0, 1.0, 1.0, 0.0);

    if (myInteractiveContext == null)
      myInteractiveContext = new ISession2D_InteractiveContext(myViewer2d);


    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    if (myAISContext == null)
    {
      myAISContext = new AIS_InteractiveContext(myViewer3d);
      myAISContext.SetIsoNumber(11, AIS_TypeOfIso.AIS_TOI_Both);
    }
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));


    // Viewer 2D
    //------------------------------------------
    if (myViewer2d == null){
      System.out.println("Info: Create Viewer");
      myViewer2d = SampleGeometryPackage.CreateViewer2d("Geometry2d");
      System.out.println("Info: Done creation of Viewer2d");
    }

    myView2d = new ViewPanel(ViewPanel.VIEW_2D) {
	public ViewCanvas createViewPort()
	{
	  return new CASCADEView2d(SampleGeometryPanel.getViewer2d()) {
	      public V2d_View createView2d(V2d_Viewer viewer, int hiwin, int lowin) {
		return SampleGeometryPackage.CreateView2d(viewer, hiwin, lowin);
	      }
	  };
	}
    };

    myView2d.addMouseListener(this);
    myView2d.addMouseMotionListener(this);


    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleGeometryPackage.CreateViewer3d("Geometry3d");

    myView3d = new ViewPanel() {
	public ViewCanvas createViewPort()
	{
	  return new CASCADEView3d(SampleGeometryPanel.getViewer3d()) {
	      public void setWindow3d(V3d_View view, int hiwin, int lowin) {
		SampleGeometryPackage.SetWindow3d(view, hiwin, lowin);
	      }
	  };
	}
    };
   
    myView3d.addMouseListener(this);
    myView3d.addMouseMotionListener(this);


    // Layout
    //------------------------------------------
    mainPanel.add(myView2d, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.5,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));
    mainPanel.add(myView3d, new GridBagConstraints(0, 1, 1, 1, 1.0, 0.5,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));

    showView3d();
            
    return mainPanel;
  }

//-----------------------------------------------------------------------//
  public Component createToolbar()
  {
    JToolBar tools = (JToolBar) super.createToolbar();
    tools.setLayout(new GridLayout(2,0,1,1));

    ButtonGroup group = new ButtonGroup();
    Insets margin = new Insets(1, 1, 1, 1);
    JToggleButton button;

    button = new HeavyToggleButton(imgTest1, false);
    button.setToolTipText(resGui.getString("TT-Test1"));
    button.setActionCommand("Test1");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest2, false);
    button.setToolTipText(resGui.getString("TT-Test2"));
    button.setActionCommand("Test2");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest3, false);
    button.setToolTipText(resGui.getString("TT-Test3"));
    button.setActionCommand("Test3");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest4, false);
    button.setToolTipText(resGui.getString("TT-Test4"));
    button.setActionCommand("Test4");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest5, false);
    button.setToolTipText(resGui.getString("TT-Test5"));
    button.setActionCommand("Test5");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest6, false);
    button.setToolTipText(resGui.getString("TT-Test6"));
    button.setActionCommand("Test6");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest7, false);
    button.setToolTipText(resGui.getString("TT-Test7"));
    button.setActionCommand("Test7");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest8, false);
    button.setToolTipText(resGui.getString("TT-Test8"));
    button.setActionCommand("Test8");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest9, false);
    button.setToolTipText(resGui.getString("TT-Test9"));
    button.setActionCommand("Test9");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest10, false);
    button.setToolTipText(resGui.getString("TT-Test10"));
    button.setActionCommand("Test10");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest11, false);
    button.setToolTipText(resGui.getString("TT-Test11"));
    button.setActionCommand("Test11");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest12, false);
    button.setToolTipText(resGui.getString("TT-Test12"));
    button.setActionCommand("Test12");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest13, false);
    button.setToolTipText(resGui.getString("TT-Test13"));
    button.setActionCommand("Test13");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest14, false);
    button.setToolTipText(resGui.getString("TT-Test14"));
    button.setActionCommand("Test14");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest15, false);
    button.setToolTipText(resGui.getString("TT-Test15"));
    button.setActionCommand("Test15");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest16, false);
    button.setToolTipText(resGui.getString("TT-Test16"));
    button.setActionCommand("Test16");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest17, false);
    button.setToolTipText(resGui.getString("TT-Test17"));
    button.setActionCommand("Test17");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest18, false);
    button.setToolTipText(resGui.getString("TT-Test18"));
    button.setActionCommand("Test18");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest19, false);
    button.setToolTipText(resGui.getString("TT-Test19"));
    button.setActionCommand("Test19");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest20, false);
    button.setToolTipText(resGui.getString("TT-Test20"));
    button.setActionCommand("Test20");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest21, false);
    button.setToolTipText(resGui.getString("TT-Test21"));
    button.setActionCommand("Test21");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest22, false);
    button.setToolTipText(resGui.getString("TT-Test22"));
    button.setActionCommand("Test22");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest23, false);
    button.setToolTipText(resGui.getString("TT-Test23"));
    button.setActionCommand("Test23");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest24, false);
    button.setToolTipText(resGui.getString("TT-Test24"));
    button.setActionCommand("Test24");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest25, false);
    button.setToolTipText(resGui.getString("TT-Test25"));
    button.setActionCommand("Test25");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest26, false);
    button.setToolTipText(resGui.getString("TT-Test26"));
    button.setActionCommand("Test26");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest27, false);
    button.setToolTipText(resGui.getString("TT-Test27"));
    button.setActionCommand("Test27");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest28, false);
    button.setToolTipText(resGui.getString("TT-Test28"));
    button.setActionCommand("Test28");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest29, false);
    button.setToolTipText(resGui.getString("TT-Test29"));
    button.setActionCommand("Test29");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest30, false);
    button.setToolTipText(resGui.getString("TT-Test30"));
    button.setActionCommand("Test30");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest31, false);
    button.setToolTipText(resGui.getString("TT-Test31"));
    button.setActionCommand("Test31");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest32, false);
    button.setToolTipText(resGui.getString("TT-Test32"));
    button.setActionCommand("Test32");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest33, false);
    button.setToolTipText(resGui.getString("TT-Test33"));
    button.setActionCommand("Test33");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest34, false);
    button.setToolTipText(resGui.getString("TT-Test34"));
    button.setActionCommand("Test34");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest35, false);
    button.setToolTipText(resGui.getString("TT-Test35"));
    button.setActionCommand("Test35");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest36, false);
    button.setToolTipText(resGui.getString("TT-Test36"));
    button.setActionCommand("Test36");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest37, false);
    button.setToolTipText(resGui.getString("TT-Test37"));
    button.setActionCommand("Test37");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest38, false);
    button.setToolTipText(resGui.getString("TT-Test38"));
    button.setActionCommand("Test38");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest39, false);
    button.setToolTipText(resGui.getString("TT-Test39"));
    button.setActionCommand("Test39");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest40, false);
    button.setToolTipText(resGui.getString("TT-Test40"));
    button.setActionCommand("Test40");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest41, false);
    button.setToolTipText(resGui.getString("TT-Test41"));
    button.setActionCommand("Test41");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest42, false);
    button.setToolTipText(resGui.getString("TT-Test42"));
    button.setActionCommand("Test42");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest43, false);
    button.setToolTipText(resGui.getString("TT-Test43"));
    button.setActionCommand("Test43");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest44, false);
    button.setToolTipText(resGui.getString("TT-Test44"));
    button.setActionCommand("Test44");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest45, false);
    button.setToolTipText(resGui.getString("TT-Test45"));
    button.setActionCommand("Test45");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest46, false);
    button.setToolTipText(resGui.getString("TT-Test46"));
    button.setActionCommand("Test46");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest47, false);
    button.setToolTipText(resGui.getString("TT-Test47"));
    button.setActionCommand("Test47");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest48, false);
    button.setToolTipText(resGui.getString("TT-Test48"));
    button.setActionCommand("Test48");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest49, false);
    button.setToolTipText(resGui.getString("TT-Test49"));
    button.setActionCommand("Test49");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    button = new HeavyToggleButton(imgTest50, false);
    button.setToolTipText(resGui.getString("TT-Test50"));
    button.setActionCommand("Test50");
    button.addActionListener(this);
    button.setMargin(margin);
    group.add(button);
    tools.add(button);

    return tools;
  }


//=======================================================================//
//                              Actions                                  //
//=======================================================================//
  public void showView2d()
  {
    myView3d.setVisible(false);
    myView2d.setVisible(true);
    validate();
  }

//-----------------------------------------------------------------------//
  public void showView3d()
  {
    myView2d.setVisible(false);
    myView3d.setVisible(true);
    validate();
  }

//-----------------------------------------------------------------------//
  public void showBothView()
  {
    myView2d.setVisible(true);
    myView3d.setVisible(true);
    validate();
  }


//-----------------------------------------------------------------------//
  public void preProcess(int type)
  {
    SamplesStarter.put_info("");
    switch (type)
    {
      case BOTH_VIEW:
        showBothView();
        break;
      case ONLY_VIEW2D:
        showView2d();
        break;
      default:
        showView3d();
        break;
    }
  }

//-----------------------------------------------------------------------//
  public String getTitleString(int type)
  {
    String mess = new String("Results are ");
    switch (type)
    {
      case BOTH_VIEW:
        mess += "in both 2D and 3D\n";
        break;
      case ONLY_VIEW2D:
        mess += "only in 2D\n";
        break;
      default:
        mess += "only in 3D\n";
        break;
    }
    mess = mess + "====================================\n\n";
    return mess;
  }

//-----------------------------------------------------------------------//
  public void postProcess(int type, TCollection_AsciiString message, String title)
  {
    postProcess(type, message, title, -1.0);
  }

  public void postProcess(int type, TCollection_AsciiString message, String title, double zoom)
  {
    String text = getTitleString(type);
    text += message.ToCString().GetValue();

    switch (type)
    {
      case BOTH_VIEW:
        myView3d.getViewPort().FitAll();
        if (zoom > -1)
        {
          V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
          if (view3d != null)
            view3d.SetZoom(zoom, true);
        }

        myView2d.getViewPort().FitAll();
	myView2d.getViewPort().repaint();
	myView3d.getViewPort().repaint();
        traceMessage(text, title);
        break;

      case ONLY_VIEW2D:
        myView2d.getViewPort().FitAll();
        traceMessage(text, title);
	myView2d.getViewPort().repaint();
        break;
        
      default:
        myView3d.getViewPort().FitAll();
        if (zoom > -1)
        {
          V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
          if (view3d != null)
            view3d.SetZoom(zoom, true);
        }
        traceMessage(text, title);
	myView3d.getViewPort().repaint();
	break;
    }
  }

//=======================================================================//
// Tests
//=======================================================================//
  public void gpTest1()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest1(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test1"));
  }

//=======================================================================//
  public void gpTest2()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest2(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test2"), 0.002);
  }

//=======================================================================//
  public void gpTest3()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest3(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test3"), 0.02);
  }

//=======================================================================//
  public void gpTest4()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest4(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test4"), 0.02);
  }

//=======================================================================//
  public void gpTest5()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest5(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test5"));
  }

//=======================================================================//
  public void gpTest6()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest6(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test6"));
  }

//=======================================================================//
  public void gpTest7()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest7(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test7"));
  }

//=======================================================================//
  public void gpTest8()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest8(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test8"));
  }

//=======================================================================//
  public void gpTest9()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest9(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test9"));
  }

//=======================================================================//
  public void gpTest10()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest10(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test10"));
  }

//=======================================================================//
  public void gpTest11()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest11(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test11"));
  }

//=======================================================================//
  public void gpTest12()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest12(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test12"));
  }

//=======================================================================//
  public void gpTest13()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest13(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test13"));
  }

//=======================================================================//
  public void gpTest14()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest14(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test14"));
  }

//=======================================================================//
  public void gpTest15()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest15(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test15"));
  }

//=======================================================================//
  public void gpTest16()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest16(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test16"));
  }

//=======================================================================//
  public void gpTest17()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest17(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test17"));
  }

//=======================================================================//
  public void gpTest18()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest18(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test18"));
  }

//=======================================================================//
  public void gpTest19()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest19(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test19"));
  }

//=======================================================================//
  public void gpTest20()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest20(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test20"));
  }

//=======================================================================//
  public void gpTest21()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest21(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test21"));
  }

//=======================================================================//
  public void gpTest22()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest22(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test22"));
  }

//=======================================================================//
  public void gpTest23()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest23(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test23"));
  }

//=======================================================================//
  public void gpTest24()
  {
    int type = BOTH_VIEW;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest24(myAISContext, myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test24"));
  }

//=======================================================================//
  public void gpTest25()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest25(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test25"));
  }

//=======================================================================//
  public void gpTest26()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest26(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test26"));
  }

//=======================================================================//
  public void gpTest27()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest27(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test27"));
  }

//=======================================================================//
  public void gpTest28()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest28(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test28"));
  }

//=======================================================================//
  public void gpTest29()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest29(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test29"));
  }

//=======================================================================//
  public void gpTest30()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest30(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test30"));
  }

//=======================================================================//
  public void gpTest31()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest31(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test31"));
  }

//=======================================================================//
  public void gpTest32()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest32(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test32"));
  }

//=======================================================================//
  public void gpTest33()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest33(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test33"));
  }

//=======================================================================//
  public void gpTest34()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest34(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test34"));
  }

//=======================================================================//
  public void gpTest35()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest35(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test35"));
  }

//=======================================================================//
  public void gpTest36()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest36(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test36"));
  }

//=======================================================================//
  public void gpTest37()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest37(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test37"));
  }

//=======================================================================//
  public void gpTest38()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest38(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test38"));
  }

//=======================================================================//
  public void gpTest39()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest39(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test39"));
  }

//=======================================================================//
  public void gpTest40()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest40(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test40"));
  }

//=======================================================================//
  public void gpTest41()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest41(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test41"));
  }

//=======================================================================//
  public void gpTest42()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest42(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test42"));
  }

//=======================================================================//
  public void gpTest43()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest43(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test43"));
  }

//=======================================================================//
  public void gpTest44()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest44(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test44"));
  }

//=======================================================================//
  public void gpTest45()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest45(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test45"));
  }

//=======================================================================//
  public void gpTest46()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest46(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test46"));
  }

//=======================================================================//
  public void gpTest47()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest47(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test47"));
  }

//=======================================================================//
  public void gpTest48()
  {
    int type = ONLY_VIEW2D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest48(myInteractiveContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test48"));
  }

//=======================================================================//
  public void gpTest49()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest49(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test49"));
  }

//=======================================================================//
  public void gpTest50()
  {
    int type = ONLY_VIEW3D;
    preProcess(type);

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleGeometryPackage.gpTest50(myAISContext, message);

    postProcess(type, message, resGui.getString("Dlg-Test50"));
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("Test1")) gpTest1();
    else if (command.equals("Test2")) gpTest2();
    else if (command.equals("Test3")) gpTest3();
    else if (command.equals("Test4")) gpTest4();
    else if (command.equals("Test5")) gpTest5();
    else if (command.equals("Test6")) gpTest6();
    else if (command.equals("Test7")) gpTest7();
    else if (command.equals("Test8")) gpTest8();
    else if (command.equals("Test9")) gpTest9();
    else if (command.equals("Test10")) gpTest10();
    else if (command.equals("Test11")) gpTest11();
    else if (command.equals("Test12")) gpTest12();
    else if (command.equals("Test13")) gpTest13();
    else if (command.equals("Test14")) gpTest14();
    else if (command.equals("Test15")) gpTest15();
    else if (command.equals("Test16")) gpTest16();
    else if (command.equals("Test17")) gpTest17();
    else if (command.equals("Test18")) gpTest18();
    else if (command.equals("Test19")) gpTest19();
    else if (command.equals("Test20")) gpTest20();
    else if (command.equals("Test21")) gpTest21();
    else if (command.equals("Test22")) gpTest22();
    else if (command.equals("Test23")) gpTest23();
    else if (command.equals("Test24")) gpTest24();
    else if (command.equals("Test25")) gpTest25();
    else if (command.equals("Test26")) gpTest26();
    else if (command.equals("Test27")) gpTest27();
    else if (command.equals("Test28")) gpTest28();
    else if (command.equals("Test29")) gpTest29();
    else if (command.equals("Test30")) gpTest30();
    else if (command.equals("Test31")) gpTest31();
    else if (command.equals("Test32")) gpTest32();
    else if (command.equals("Test33")) gpTest33();
    else if (command.equals("Test34")) gpTest34();
    else if (command.equals("Test35")) gpTest35();
    else if (command.equals("Test36")) gpTest36();
    else if (command.equals("Test37")) gpTest37();
    else if (command.equals("Test38")) gpTest38();
    else if (command.equals("Test39")) gpTest39();
    else if (command.equals("Test40")) gpTest40();
    else if (command.equals("Test41")) gpTest41();
    else if (command.equals("Test42")) gpTest42();
    else if (command.equals("Test43")) gpTest43();
    else if (command.equals("Test44")) gpTest44();
    else if (command.equals("Test45")) gpTest45();
    else if (command.equals("Test46")) gpTest46();
    else if (command.equals("Test47")) gpTest47();
    else if (command.equals("Test48")) gpTest48();
    else if (command.equals("Test49")) gpTest49();
    else if (command.equals("Test50")) gpTest50();
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
      if (src == myView2d)
      {
        V2d_View view2d = ((CASCADEView2d) myView2d.getViewPort()).getView();
        if (view2d != null)
          myInteractiveContext.Move(startX, startY, view2d);
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
        if (src == myView3d)
        {
          V2d_View view2d = ((CASCADEView2d) myView2d.getViewPort()).getView();
          if (view2d != null)
          {
            myInteractiveContext.Move(startX, startY, e.getX(), e.getY(), view2d);
            myInteractiveContext.Pick(e.isShiftDown());
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
        if (src == myView2d)
        {
          myInteractiveContext.Pick(e.isShiftDown());
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
    if (SwingUtilities.isLeftMouseButton(e))
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
    if (src == myView2d)
    {
      V2d_View view2d = ((CASCADEView2d) myView2d.getViewPort()).getView();
      if (myInteractiveContext != null && view2d != null)
        myInteractiveContext.Move(e.getX(), e.getY(), view2d);
    }
  }


}

