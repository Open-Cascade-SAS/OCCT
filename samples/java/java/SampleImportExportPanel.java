
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
import SampleImportExportJni.*;
import util.*;

public class SampleImportExportPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleImportExport");

  static protected ImageIcon imgImportBrep = new ImageIcon(resGui.getString("Icon-ImportBREP"));
  static protected ImageIcon imgExportBrep = new ImageIcon(resGui.getString("Icon-ExportBREP"));
  static protected ImageIcon imgImportCSFDB = new ImageIcon(resGui.getString("Icon-ImportCSFDB"));
  static protected ImageIcon imgExportCSFDB = new ImageIcon(resGui.getString("Icon-ExportCSFDB"));
  static protected ImageIcon imgImportSTEP = new ImageIcon(resGui.getString("Icon-ImportSTEP"));
  static protected ImageIcon imgExportSTEP = new ImageIcon(resGui.getString("Icon-ExportSTEP"));
  static protected ImageIcon imgImportIGES = new ImageIcon(resGui.getString("Icon-ImportIGES"));
  static protected ImageIcon imgExportIGES = new ImageIcon(resGui.getString("Icon-ExportIGES"));
  static protected ImageIcon imgExportImage = new ImageIcon(resGui.getString("Icon-ExportImage"));
  static protected ImageIcon imgWireframe = new ImageIcon(resGui.getString("Icon-Wireframe"));
  static protected ImageIcon imgShading = new ImageIcon(resGui.getString("Icon-Shading"));
  static protected ImageIcon imgColor = new ImageIcon(resGui.getString("Icon-Color"));
  static protected ImageIcon imgMaterial = new ImageIcon(resGui.getString("Icon-Material"));
  static protected ImageIcon imgTransparency = new ImageIcon(resGui.getString("Icon-Transparency"));
  static protected ImageIcon imgErase = new ImageIcon(resGui.getString("Icon-Erase"));


  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private static ViewPanel myView3d;

  private static V3d_Viewer myViewer3d = null;
  private static AIS_InteractiveContext myAISContext = null;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;

  /** Directory path selected by user for the last time. */
  private File curPath = new File(System.getProperty("user.dir") + "/../../../../data");
  private File curPathOcc = new File(curPath + "/occ");
  private File curPathCsfdb = new File(curPath + "/csfdb");
  private File curPathIges = new File(curPath + "/iges");
  private File curPathStep = new File(curPath + "/step");
  private File curPathImage = new File(curPath + "/images");

  /** Object(s) popup menu */
  private PopupMenu myPopup;

  /** Toolbar buttons */
  private JButton btnWireframe;
  private JButton btnShading;
  private JButton btnColor;
  private JButton btnMaterial;
  private JButton btnTransparency;
  private JButton btnErase;


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
  public SampleImportExportPanel()
  {
    super(false);

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


    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleImportExportPackage.CreateViewer3d("ImportExport");

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort()
      	{
      	  return new CASCADEView3d(SampleImportExportPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                SampleImportExportPackage.SetWindow3d(view, hiwin, lowin);
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

    button = new HeavyButton(imgImportBrep);
    button.setToolTipText(resGui.getString("Help-ImportBREP"));
    button.setActionCommand("ImportBREP");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgImportCSFDB);
    button.setToolTipText(resGui.getString("Help-ImportCSFDB"));
    button.setActionCommand("ImportCSFDB");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgImportSTEP);
    button.setToolTipText(resGui.getString("Help-ImportSTEP"));
    button.setActionCommand("ImportSTEP");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgImportIGES);
    button.setToolTipText(resGui.getString("Help-ImportIGES"));
    button.setActionCommand("ImportIGES");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    tools.addSeparator();

    button = new HeavyButton(imgExportBrep);
    button.setToolTipText(resGui.getString("Help-ExportBREP"));
    button.setActionCommand("ExportBREP");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgExportCSFDB);
    button.setToolTipText(resGui.getString("Help-ExportCSFDB"));
    button.setActionCommand("ExportCSFDB");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgExportSTEP);
    button.setToolTipText(resGui.getString("Help-ExportSTEP"));
    button.setActionCommand("ExportSTEP");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgExportIGES);
    button.setToolTipText(resGui.getString("Help-ExportIGES"));
    button.setActionCommand("ExportIGES");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgExportImage);
    button.setToolTipText(resGui.getString("Help-ExportImage"));
    button.setActionCommand("ExportImage");
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

//=======================================================================//
  public void onImportBREP()
  {
    // Select file to open
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"brep", "rle"};
    ExtensionFileFilter flt = new ExtensionFileFilter(fltExt, "BREP files");
    aDlg.setFileFilter(flt);

    System.out.println("Current path = " + curPath);

    System.out.println("DATADIR path = " + curPathOcc);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathOcc);
    aDlg.setDialogTitle("Import BREP");
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
      boolean result = SampleImportExportPackage.ReadBREP(new Standard_CString(name),
                                                          myAISContext);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      if (!result)
        JOptionPane.showMessageDialog(this, "Error loading file " + name,
                "CASCADE", JOptionPane.ERROR_MESSAGE);
    }
  }

//=======================================================================//
  public void onExportBREP()
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
    aDlg.setDialogTitle("Export BREP");
    aDlg.setApproveButtonText("Export");
    aDlg.setApproveButtonMnemonic('e');
    aDlg.setApproveButtonToolTipText("Export shape into selected file");

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
      boolean result = SampleImportExportPackage.SaveBREP(new Standard_CString(name),
                                                          myAISContext);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      if (!result)
        JOptionPane.showMessageDialog(this, "The shape or shapes were not saved",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
    }
  }

//=======================================================================//
  public void onImportCSFDB()
  {
    // Select file to open
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"csfdb", "csf"};
    ExtensionFileFilter flt = new ExtensionFileFilter(fltExt, "CSFDB files");
    aDlg.setFileFilter(flt);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathCsfdb);
    aDlg.setDialogTitle("Import CSFDB");
    aDlg.setApproveButtonText("Import");
    aDlg.setApproveButtonMnemonic('i');
    aDlg.setApproveButtonToolTipText("Import CSFDB from the selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Import");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathCsfdb = aDlg.getCurrentDirectory();

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
      TCollection_AsciiString aMessage = new TCollection_AsciiString();
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
      boolean result = SampleImportExportPackage.ReadCSFDB(new Standard_CString(name),
                                                           myAISContext, aMessage);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      if (result)
        JOptionPane.showMessageDialog(this, aMessage.ToCString().GetValue(),
                "CASCADE", JOptionPane.INFORMATION_MESSAGE);
      else
        JOptionPane.showMessageDialog(this, aMessage.ToCString().GetValue(),
                "CASCADE", JOptionPane.ERROR_MESSAGE);
    }
  }

//=======================================================================//
  public void onExportCSFDB()
  {
    // Get file name
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"csfdb", "csf"};
    ExtensionFileFilter fltBrep = new ExtensionFileFilter(fltExt, "CSFDB files");
    aDlg.setFileFilter(fltBrep);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathCsfdb);
    aDlg.setDialogTitle("Export CSFDB");
    aDlg.setApproveButtonText("Export");
    aDlg.setApproveButtonMnemonic('e');
    aDlg.setApproveButtonToolTipText("Export shape into selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Export");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathCsfdb = aDlg.getCurrentDirectory();

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
      TCollection_AsciiString aMessage = new TCollection_AsciiString();
      boolean result = SampleImportExportPackage.SaveCSFDB(new Standard_CString(name),
              myAISContext, aMessage, MgtBRep_TriangleMode.MgtBRep_WithTriangle);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      if (result)
        JOptionPane.showMessageDialog(this, aMessage.ToCString().GetValue(),
                "CASCADE", JOptionPane.INFORMATION_MESSAGE);
      else
        JOptionPane.showMessageDialog(this, aMessage.ToCString().GetValue(),
                "CASCADE", JOptionPane.ERROR_MESSAGE);
    }
  }

//=======================================================================//
  public void onImportSTEP()
  {
    // Select file to open
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"step", "stp"};
    ExtensionFileFilter flt = new ExtensionFileFilter(fltExt, "STEP files");
    aDlg.setFileFilter(flt);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathStep);
    aDlg.setDialogTitle("Import STEP");
    aDlg.setApproveButtonText("Import");
    aDlg.setApproveButtonMnemonic('i');
    aDlg.setApproveButtonToolTipText("Import STEP from the selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Import");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathStep = aDlg.getCurrentDirectory();

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
      int result = SampleImportExportPackage.ReadSTEP(new Standard_CString(name),
                                                      myAISContext);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      switch(result)
      {
        case IFSelect_ReturnStatus.IFSelect_RetError:
          JOptionPane.showMessageDialog(this, "Not a valid Step file",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
          break;
        case IFSelect_ReturnStatus.IFSelect_RetFail:
          JOptionPane.showMessageDialog(this, "Reading has failed",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
          break;
        case IFSelect_ReturnStatus.IFSelect_RetVoid:
          JOptionPane.showMessageDialog(this, "Nothing to transfer",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
          break;
      }
    }
  }

//=======================================================================//
  public void onExportSTEP()
  {
    // Get file name
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"step", "stp"};
    ExtensionFileFilter fltBrep = new ExtensionFileFilter(fltExt, "STEP files");
    aDlg.setFileFilter(fltBrep);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathStep);
    aDlg.setDialogTitle("Export STEP");
    aDlg.setApproveButtonText("Export");
    aDlg.setApproveButtonMnemonic('e');
    aDlg.setApproveButtonToolTipText("Export shape into selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Export");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathStep = aDlg.getCurrentDirectory();

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
      int result = SampleImportExportPackage.SaveSTEP(new Standard_CString(name),
              myAISContext, STEPControl_StepModelType.STEPControl_AsIs);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      switch (result)
      {
        case IFSelect_ReturnStatus.IFSelect_RetError:
          JOptionPane.showMessageDialog(this, "Incorrect Data",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
          break;
        case IFSelect_ReturnStatus.IFSelect_RetFail:
          JOptionPane.showMessageDialog(this, "Writing has failed",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
          break;
        case IFSelect_ReturnStatus.IFSelect_RetVoid:
          JOptionPane.showMessageDialog(this, "Nothing to transfer",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
          break;
      }
    }
  }

//=======================================================================//
  public void onImportIGES()
  {
    // Select file to open
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"iges", "igs"};
    ExtensionFileFilter flt = new ExtensionFileFilter(fltExt, "IGES files");
    aDlg.setFileFilter(flt);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathIges);
    aDlg.setDialogTitle("Import IGES");
    aDlg.setApproveButtonText("Import");
    aDlg.setApproveButtonMnemonic('i');
    aDlg.setApproveButtonToolTipText("Import IGES from the selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Import");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathIges = aDlg.getCurrentDirectory();

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
      int result = SampleImportExportPackage.ReadIGES(new Standard_CString(name),
                                                      myAISContext);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      if (result != IFSelect_ReturnStatus.IFSelect_RetDone)
        JOptionPane.showMessageDialog(this, "Error loading file " + name,
                "CASCADE", JOptionPane.ERROR_MESSAGE);
    }
  }

//=======================================================================//
  public void onExportIGES()
  {
    // Get file name
    JFileChooser aDlg = new JFileChooser();
		aDlg.setFileSelectionMode(JFileChooser.FILES_ONLY);

    // Setting file filters
    String[] fltExt = {"iges", "igs"};
    ExtensionFileFilter fltBrep = new ExtensionFileFilter(fltExt, "IGES files");
    aDlg.setFileFilter(fltBrep);

    // Setting dialog properties
    aDlg.setCurrentDirectory(curPathIges);
    aDlg.setDialogTitle("Export IGES");
    aDlg.setApproveButtonText("Export");
    aDlg.setApproveButtonMnemonic('e');
    aDlg.setApproveButtonToolTipText("Export shape into selected file");

    // Show the dialog
    int returnVal = aDlg.showDialog(this, "Export");

    if (returnVal == JFileChooser.APPROVE_OPTION)
    {
      String name = aDlg.getSelectedFile().getAbsolutePath();
      curPathIges = aDlg.getCurrentDirectory();

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
      boolean result = SampleImportExportPackage.SaveIGES(new Standard_CString(name),
                                                          myAISContext);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
      if (!result)
        JOptionPane.showMessageDialog(this, "The shape or shapes were not saved",
                "CASCADE", JOptionPane.ERROR_MESSAGE);
    }
  }

//=======================================================================//
  public void onExportImage()
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

      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
      String format = name.substring(name.lastIndexOf(".")+1);
      V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
      SampleImportExportPackage.SaveImage(new Standard_CString(name),
              new Standard_CString(format), view3d);
      SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
    }
  }

//=======================================================================//
  public void onWireframe()
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetDisplayMode(myAISContext.Current(), 0, true);
    }
  }

//=======================================================================//
  public void onShading()
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetDisplayMode(myAISContext.Current(), 1, true);
    }
  }

//=======================================================================//
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

//=======================================================================//
  public void onMaterial()
  {
    MaterialDlg theDlg = new MaterialDlg(SamplesStarter.getFrame(), myAISContext);
    Position.centerWindow(theDlg);
    theDlg.show();
  }

//=======================================================================//
  public void setMaterial(int material)
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetMaterial(myAISContext.Current(), (short)material, true);
    }
  }

//=======================================================================//
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

//=======================================================================//
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

    // Import
    if (command.equals("ImportBREP")) onImportBREP();
    else if (command.equals("ImportCSFDB")) onImportCSFDB();
    else if (command.equals("ImportSTEP")) onImportSTEP();
    else if (command.equals("ImportIGES")) onImportIGES();
    // Export
    else if (command.equals("ExportBREP")) onExportBREP();
    else if (command.equals("ExportCSFDB")) onExportCSFDB();
    else if (command.equals("ExportSTEP")) onExportSTEP();
    else if (command.equals("ExportIGES")) onExportIGES();
    else if (command.equals("ExportImage")) onExportImage();

    // Object properties
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

//-----------------------------------------------------------------------//
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
    }
  }

//-----------------------------------------------------------------------//
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
      }
      Dragged = false;
    }
  }

//-----------------------------------------------------------------------//
  public void mouseEntered(MouseEvent e)
  {
  }

//-----------------------------------------------------------------------//
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

//-----------------------------------------------------------------------//
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


