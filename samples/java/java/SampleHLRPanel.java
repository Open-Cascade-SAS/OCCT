
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
import java.io.File;
import javax.swing.filechooser.FileFilter; 
import jcas.Standard_CString;
import jcas.Standard_Real;
import CASCADESamplesJni.*;
import SampleHLRJni.*;
import util.*;

public class SampleHLRPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleHLR");

  static protected ImageIcon imgImportBrep = new ImageIcon(resGui.getString("Icon-ImportBrep"));
  static protected ImageIcon imgExportBrep = new ImageIcon(resGui.getString("Icon-ExportBrep"));
  static protected ImageIcon imgExport2d = new ImageIcon(resGui.getString("Icon-Export2d"));
  static protected ImageIcon imgExport3d = new ImageIcon(resGui.getString("Icon-Export3d"));
  static protected ImageIcon imgHLR = new ImageIcon(resGui.getString("Icon-HLR"));
  static protected ImageIcon imgProperties = new ImageIcon(resGui.getString("Icon-Properties"));
  static protected ImageIcon imgWireframe = new ImageIcon(resGui.getString("Icon-Wireframe"));
  static protected ImageIcon imgShading = new ImageIcon(resGui.getString("Icon-Shading"));
  static protected ImageIcon imgColor = new ImageIcon(resGui.getString("Icon-Color"));
  static protected ImageIcon imgMaterial = new ImageIcon(resGui.getString("Icon-Material"));
  static protected ImageIcon imgTransparency = new ImageIcon(resGui.getString("Icon-Transparency"));
  static protected ImageIcon imgErase = new ImageIcon(resGui.getString("Icon-Erase"));


  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private static ViewPanel myView2d;
  private static ViewPanel myView3d;

  private static V2d_Viewer myViewer2d = null;
  private static V3d_Viewer myViewer3d = null;

  private static ISession2D_InteractiveContext myInteractiveContext = null;
  private static AIS_InteractiveContext myAISContext = null;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;

  /** Directory path selected by user for the last time. */
  private File curPath = new File(System.getProperty("user.dir") + "/../../../../data");

  private File curPathOcc = new File(curPath + "/occ");
  private File curPathImage = new File(curPath + "/images");

  /** CASCADE properties dialog */
  private PropertiesDlg myPropDlg = null;

  /** HLR dialog */
  private HLRDlg myHLRDlg = null;

  /** Object(s) popup menu */
  private PopupMenu myPopup;

  JButton btnWireframe;
  JButton btnShading;
  JButton btnColor;
  JButton btnMaterial;
  JButton btnTransparency;
  JButton btnErase;


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
  
  public static CASCADEView2d getView2d()
  {
    if (myView2d == null)
      return null;
    return (CASCADEView2d) myView2d.getViewPort();
  }

  public static CASCADEView3d getView3d()
  {
    if (myView3d == null)
      return null;
    return (CASCADEView3d) myView3d.getViewPort();
  }


//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public SampleHLRPanel()
  {
    super(false);
    
    // 2D Initialization
    //------------------------------------------
    myViewer2d.SetCircularGridValues(0.0, 0.0, 10.0, 8, 0.0);
    myViewer2d.SetRectangularGridValues(0.0, 0.0, 10.0, 10.0, 0.0);

    SampleHLRPackage.InitMaps(myViewer2d);

    if (myInteractiveContext == null)
      myInteractiveContext = new ISession2D_InteractiveContext(myViewer2d);


    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

    if (myAISContext == null)
      myAISContext = new AIS_InteractiveContext(myViewer3d);

    myPopup = createObjectPopup();
    updateButtons();
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));


    // Viewer 2D
    //------------------------------------------
    if (myViewer2d == null) {
//       JOptionPane.showMessageDialog(null, "Call SampleHLRPackage.CreateViewer2d");
//       System.out.println("SampleHLRPanel.java: Press enter: ");
//       try{
//         new java.io.DataInputStream(System.in).readLine();
//         System.out.println("...");
//       }catch (Exception e) {
//         System.out.println("Error: SampleHLRPanel.createViewPanel:"+ e);
//         System.out.println("Error", "SampleHLRPanel.createViewPanel:", e);
//       }
      myViewer2d = SampleHLRPackage.CreateViewer2d("HLR2d");
    }

    myView2d = new ViewPanel(ViewPanel.VIEW_2D) {
        public ViewCanvas createViewPort() {
          return new CASCADEView2d(SampleHLRPanel.getViewer2d()) {
                  public V2d_View createView2d(V2d_Viewer viewer,
                                               int hiwin, int lowin) {
                    return SampleHLRPackage.CreateView2d(viewer, hiwin, lowin);
                  }
                  public Color GetBackgroundColor() {
//                     Aspect_WindowDriver aDriver = new Aspect_WindowDriver(myView.Driver());
                    Aspect_WindowDriver aDriver = myView.Driver();
                    Aspect_Window aWindow = aDriver.Window();
                    Aspect_Background aBack = aWindow.Background();
                    Quantity_Color aColor = aBack.Color();
                    int red = (int) (aColor.Red()*255);
                    int green = (int) (aColor.Green()*255);
                    int blue = (int) (aColor.Blue()*255);
                    return new Color(red, green, blue);
                  }
                  public void SetBackgroundColor(Color color) {
//                     SampleHLRJni.Aspect_WindowDriver aDriver = new SampleHLRJni.Aspect_WindowDriver(myView.Driver());
//                     Aspect_WindowDriver aDriver = new Aspect_WindowDriver(myView.Driver());
                    Aspect_WindowDriver aDriver = myView.Driver();

                    Aspect_Window aWindow = aDriver.Window();
                    Aspect_Background aBack = aWindow.Background();
                    Quantity_Color theColor = new Quantity_Color(color.getRed()/255.,
                        color.getGreen()/255., color.getBlue()/255.,
                        Quantity_TypeOfColor.Quantity_TOC_RGB);
                    aBack.SetColor(theColor);
                    aWindow.SetBackground(aBack);
                    myView.Update();
                  }
          };
      	}
    };

    myView2d.addMouseListener(this);
    myView2d.addMouseMotionListener(this);


    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleHLRPackage.CreateViewer3d("HLR3d");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleHLRPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SampleHLRPackage.SetWindow3d(view, hiwin, lowin);
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
    mainPanel.add(myView2d, new GridBagConstraints(1, 0, 1, 1, 1.0, 1.0,
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

    button = new HeavyButton(imgImportBrep);
    button.setToolTipText(resGui.getString("Help-ImportBrep"));
    button.setActionCommand("ImportBrep");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();

    button = new HeavyButton(imgExportBrep);
    button.setToolTipText(resGui.getString("Help-ExportBrep"));
    button.setActionCommand("ExportBrep");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgExport2d);
    button.setToolTipText(resGui.getString("Help-Export2d"));
    button.setActionCommand("Export2d");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgExport3d);
    button.setToolTipText(resGui.getString("Help-Export3d"));
    button.setActionCommand("Export3d");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();

    button = new HeavyButton(imgHLR);
    button.setToolTipText(resGui.getString("Help-HLR"));
    button.setActionCommand("HLR");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();

    button = new HeavyButton(imgProperties);
    button.setToolTipText(resGui.getString("Help-Properties"));
    button.setActionCommand("Properties");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();

    btnWireframe = new HeavyButton(imgWireframe);
    btnWireframe.setToolTipText(resGui.getString("Help-Wireframe"));
    btnWireframe.setActionCommand("Wireframe");
    btnWireframe.addActionListener(this);
    btnWireframe.setMargin(margin);
    tools.add(btnWireframe);

    btnShading = new HeavyButton(imgShading);
    btnShading.setToolTipText(resGui.getString("Help-Shading"));
    btnShading.setActionCommand("Shading");
    btnShading.addActionListener(this);
    btnShading.setMargin(margin);
    tools.add(btnShading);

    tools.addSeparator();

    btnColor = new HeavyButton(imgColor);
    btnColor.setToolTipText(resGui.getString("Help-Color"));
    btnColor.setActionCommand("Color");
    btnColor.addActionListener(this);
    btnColor.setMargin(margin);
    tools.add(btnColor);

    btnMaterial = new HeavyButton(imgMaterial);
    btnMaterial.setToolTipText(resGui.getString("Help-Material"));
    btnMaterial.setActionCommand("Material");
    btnMaterial.addActionListener(this);
    btnMaterial.setMargin(margin);
    tools.add(btnMaterial);

    btnTransparency = new HeavyButton(imgTransparency);
    btnTransparency.setToolTipText(resGui.getString("Help-Transparency"));
    btnTransparency.setActionCommand("Transparency");
    btnTransparency.addActionListener(this);
    btnTransparency.setMargin(margin);
    tools.add(btnTransparency);

    tools.addSeparator();

    btnErase = new HeavyButton(imgErase);
    btnErase.setToolTipText(resGui.getString("Help-Erase"));
    btnErase.setActionCommand("Erase");
    btnErase.addActionListener(this);
    btnErase.setMargin(margin);
    tools.add(btnErase);

    return tools;
  }

//-----------------------------------------------------------------------//
  public PopupMenu createObjectPopup()
  {
    PopupMenu popup = new PopupMenu("Object(s)");

    MenuItem menuItem = new MenuItem("Wireframe");
    menuItem.setActionCommand("Wireframe");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    menuItem = new MenuItem("Shading");
    menuItem.setActionCommand("Shading");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    menuItem = new MenuItem("Color...");
    menuItem.setActionCommand("Color");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    Menu menu = new Menu("Material");

    menuItem = new MenuItem("Brass");
    menuItem.setActionCommand("MaterialBrass");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Bronze");
    menuItem.setActionCommand("MaterialBronze");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Copper");
    menuItem.setActionCommand("MaterialCopper");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Gold");
    menuItem.setActionCommand("MaterialGold");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Pewter");
    menuItem.setActionCommand("MaterialPewter");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Plaster");
    menuItem.setActionCommand("MaterialPlaster");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Plastic");
    menuItem.setActionCommand("MaterialPlastic");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    menuItem = new MenuItem("Silver");
    menuItem.setActionCommand("MaterialSilver");
    menuItem.addActionListener(this);
    menu.add(menuItem);

    popup.add(menu);

    menuItem = new MenuItem("Transparency...");
    menuItem.setActionCommand("Transparency");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    menuItem = new MenuItem("Erase");
    menuItem.setActionCommand("Erase");
    menuItem.addActionListener(this);
    popup.add(menuItem);

    return popup;
  }


//=======================================================================//
//                              Actions                                  //
//=======================================================================//
  private void updateButtons()
  {
    if (myAISContext != null)
    {
      myAISContext.InitCurrent();
      if (myAISContext.MoreCurrent())
      {
        btnWireframe.setEnabled(true);
        btnShading.setEnabled(true);
        btnColor.setEnabled(true);
        btnMaterial.setEnabled(true);
        btnTransparency.setEnabled(true);
        btnErase.setEnabled(true);
      }
      else
      {
        btnWireframe.setEnabled(false);
        btnShading.setEnabled(false);
        btnColor.setEnabled(false);
        btnMaterial.setEnabled(false);
        btnTransparency.setEnabled(false);
        btnErase.setEnabled(false);
      }
    }
  }

  public void onImportBrep()
  {
    // Select file to open
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"brep", "rle"};
    ExtensionFileFilter flt = new ExtensionFileFilter(fltExt, "BREP files");
    aDlg.setFileFilter(flt);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathOcc);
    aDlg.setDialogTitle("Import Brep");
    aDlg.setApproveButtonText("Import");
    aDlg.setApproveButtonMnemonic('i');
    aDlg.setApproveButtonToolTipText("Import BRep from the selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Import");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathOcc = aDlg.getCurrentDirectory();

      // Checking extention
      FileFilter curFlt = aDlg.getFileFilter();
      if (curFlt instanceof ExtensionFileFilter)
      {
        ExtensionFileFilter extFlt = (ExtensionFileFilter) curFlt;
        String ext = extFlt.getExtension(name);
        if (ext == null)
        {
          String[] extentions = extFlt.getExtensions();
          if (extentions != null)
          {
            if (name.endsWith("."))
              name = name + extentions[0];
            else
              name = name + "." + extentions[0];
          }
        }
      }

      // Load selected file
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
      if (!SampleHLRPackage.ReadBRep(new Standard_CString(name), myAISContext))
        SamplesStarter.put_info("Error loading file " + name);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
    }
  }

  public void onExportBrep()
  {
    // Get file name
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"brep", "rle"};
    ExtensionFileFilter fltBrep = new ExtensionFileFilter(fltExt, "BREP files");
    aDlg.setFileFilter(fltBrep);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathOcc);
    aDlg.setDialogTitle("Export BRep");
    aDlg.setApproveButtonText("Export");
    aDlg.setApproveButtonMnemonic('e');
    aDlg.setApproveButtonToolTipText("Export BRep into selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Export");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathOcc = aDlg.getCurrentDirectory();

      // Checking extention
      FileFilter curFlt = aDlg.getFileFilter();
      if (curFlt instanceof ExtensionFileFilter)
      {
        ExtensionFileFilter extFlt = (ExtensionFileFilter) curFlt;
        String ext = extFlt.getExtension(name);
        if (ext == null)
        {
          String[] extentions = extFlt.getExtensions();
          if (extentions != null)
          {
            if (name.endsWith("."))
              name = name + extentions[0];
            else
              name = name + "." + extentions[0];
          }
        }
      }

      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
      SampleHLRPackage.SaveBRep(new Standard_CString(name), myAISContext);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
    }
  }

  public void onExportImage(boolean is2d)
  {
    // Get file name
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    ExtensionFileFilter fltBmp = new ExtensionFileFilter("bmp", "BMP files");
    aDlg.addChoosableFileFilter(fltBmp);
    ExtensionFileFilter fltGif = new ExtensionFileFilter("gif", "GIF files");
    aDlg.addChoosableFileFilter(fltGif);
    ExtensionFileFilter fltXwd = new ExtensionFileFilter("xwd", "XWD files");
    aDlg.addChoosableFileFilter(fltXwd);

    aDlg.setFileFilter(fltBmp);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathImage);
    aDlg.setDialogTitle("Export Image");
    aDlg.setApproveButtonText("Export");
    aDlg.setApproveButtonMnemonic('e');
    aDlg.setApproveButtonToolTipText("Export image into selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Export");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathImage = aDlg.getCurrentDirectory();

      // Checking extention
      FileFilter curFlt = aDlg.getFileFilter();
      if (curFlt instanceof ExtensionFileFilter)
      {
        ExtensionFileFilter extFlt = (ExtensionFileFilter) curFlt;
        String ext = extFlt.getExtension(name);
        if (ext == null)
        {
          String[] extentions = extFlt.getExtensions();
          if (extentions != null)
          {
            if (name.endsWith("."))
              name = name + extentions[0];
            else
              name = name + "." + extentions[0];
          }
        }
      }

      String format = name.substring(name.lastIndexOf(".")+1);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
      if (is2d)
      {
        V2d_View view2d = ((CASCADEView2d) myView2d.getViewPort()).getView();
        SampleHLRPackage.SaveImage(new Standard_CString(name),
                                   new Standard_CString(format), view2d);
      }
      else
      {
        V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
        SampleHLRPackage.SaveImage(new Standard_CString(name),
                                   new Standard_CString(format), view3d);
      }
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
    }
  }

  public void onHLR()
  {
    if (myHLRDlg == null)
    {
      myHLRDlg = new HLRDlg(SamplesStarter.getFrame());
      Position.centerWindow(myHLRDlg);
    }

    myHLRDlg.show();
  }

  public void onProperties()
  {
    if (myPropDlg == null)
    {
      myPropDlg = new PropertiesDlg(SamplesStarter.getFrame(), myViewer2d);
      Position.centerWindow(myPropDlg);
    }

    myPropDlg.show();
  }

  public void onWireframe()
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetDisplayMode(myAISContext.Current(), 0, true);
    }
  }

  public void onShading()
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetDisplayMode(myAISContext.Current(), 1, true);
    }
  }

  public void onColor()
  {
    if (myAISContext != null)
    {
      myAISContext.InitCurrent();
      if (myAISContext.MoreCurrent())
      {
        Color theColor;
        if (myAISContext.Current().HasColor())
        {
          Quantity_Color aColor =
              new Quantity_Color(myAISContext.Color(myAISContext.Current()));
          int red = (int) (aColor.Red()*255);
          int green = (int) (aColor.Green()*255);
          int blue = (int) (aColor.Blue()*255);
          theColor = new Color(red, green, blue);
        }
        else
          theColor = Color.white;

        Color theNewColor = JColorChooser.showDialog(SamplesStarter.getFrame(),
                                                 "Choose the color", theColor);

        if (theNewColor != null)
        {
          Quantity_Color aNewColor = new Quantity_Color(theNewColor.getRed()/255.,
    				    theNewColor.getGreen()/255.,
						    theNewColor.getBlue()/255.,
						    Quantity_TypeOfColor.Quantity_TOC_RGB);
          for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
               myAISContext.NextCurrent())
            myAISContext.SetColor(myAISContext.Current(), aNewColor, true);
        }
      }
    }
  }

  public void onMaterial()
  {
    MaterialDlg theDlg = new MaterialDlg(SamplesStarter.getFrame(), myAISContext);
    Position.centerWindow(theDlg);
    theDlg.show();
  }

  public void setMaterial(int material)
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetMaterial(myAISContext.Current(), (short)material, true);
    }
  }

  public void onTransparency()
  {
    if (myAISContext != null)
    {
      myAISContext.InitCurrent();
      if (myAISContext.MoreCurrent())
      {
        double aValue = myAISContext.Current().Transparency();

        TransparencyDlg aDlg = new TransparencyDlg(SamplesStarter.getFrame(), aValue);
        Position.centerWindow(aDlg);
        aDlg.show();

        if (aDlg.isOK())
        {
          for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
               myAISContext.NextCurrent())
            myAISContext.SetTransparency(myAISContext.Current(), aDlg.getValue(), true);
        }
      }
    }
  }

  public void onErase()
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.Erase(myAISContext.Current(), true, false);
    }
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String command = e.getActionCommand();

    if (command.equals("HLR")) onHLR();
    else if (command.equals("ImportBrep")) onImportBrep();
    else if (command.equals("ExportBrep")) onExportBrep();
    else if (command.equals("Export2d")) onExportImage(true);
    else if (command.equals("Export3d")) onExportImage(false);
    else if (command.equals("Properties")) onProperties();

    // Object(s) properties
    else if (command.equals("Wireframe")) onWireframe();
    else if (command.equals("Shading")) onShading();
    else if (command.equals("Color")) onColor();
    else if (command.equals("Material")) onMaterial();
    else if (command.equals("Transparency")) onTransparency();
    else if (command.equals("Erase")) onErase();
    
    // Setting material (from popup menu)
    else if (command.equals("MaterialBrass"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_BRASS);
    else if (command.equals("MaterialBronze"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_BRONZE);
    else if (command.equals("MaterialCopper"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_COPPER);
    else if (command.equals("MaterialGold"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_GOLD);
    else if (command.equals("MaterialPewter"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_PEWTER);
    else if (command.equals("MaterialPlaster"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_PLASTER);
    else if (command.equals("MaterialPlastic"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_PLASTIC);
    else if (command.equals("MaterialSilver"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_SILVER);
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
        if (myAISContext != null)
        {
          myAISContext.InitCurrent();
          if (myAISContext.MoreCurrent())
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
        else
        {
        	PopupMenu defPopup = myView3d.getDefaultPopup();
        	myView3d.add(defPopup);
        	defPopup.show(myView3d, e.getX(), e.getY() + 30);
        }
      }
      else if (e.getSource() == myView2d)
      {
        PopupMenu defPopup = myView2d.getDefaultPopup();
        myView2d.add(defPopup);
        defPopup.show(myView2d, e.getX(), e.getY() + 30);
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
            updateButtons();
          }
        }
        if (src == myView2d)
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
          updateButtons();
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
    {
      Object src = e.getSource();
      Dragged = true;
    }
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



