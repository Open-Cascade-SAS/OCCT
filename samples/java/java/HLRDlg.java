
//Title:        Open CASCADE Technology Samples
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       Natalia Kopnova
//Company:      Matra Datavision (Nizhny Novgorod branch)
//Description:


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import java.util.*;
import CASCADESamplesJni.*;
import SampleHLRJni.*;
import util.*;

public class HLRDlg extends JDialog
                    implements ActionListener,
                               FocusListener
{
  private V3d_Viewer myViewer;
  private AIS_InteractiveContext myAISContext;


  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static private ResourceBundle res = ResourceBundle.getBundle("properties.ViewerIcon");

  // Projection
  static private ImageIcon imgTop = new ImageIcon(res.getString("IC_TOP"));
  static private ImageIcon imgBottom = new ImageIcon(res.getString("IC_BOTTOM"));
  static private ImageIcon imgLeft = new ImageIcon(res.getString("IC_LEFT"));
  static private ImageIcon imgRight = new ImageIcon(res.getString("IC_RIGHT"));
  static private ImageIcon imgFront = new ImageIcon(res.getString("IC_FRONT"));
  static private ImageIcon imgBack = new ImageIcon(res.getString("IC_BACK"));
  static private ImageIcon imgAxo = new ImageIcon(res.getString("IC_AXO"));
  
  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  CASCADEView3d viewPort;
  JRadioButton rbDefault;
  JRadioButton rbVisibleSharpEdges, rbVisibleSmoothEdges,
               rbVisibleSewingEdges, rbVisibleApparentContour;
  JCheckBox chkDrawHidden, chkDegenerateMode;
  JRadioButton rbHiddenSharpEdges, rbHiddenSmoothEdges, rbHiddenSewingEdges,
               rbHiddenApparentContour;
  JRadioButton rbVisibleIsoParam, rbHiddenIsoParam;
  JRadioButton rbPolyAlgo, rbAlgo;
  IntegerField txtNbIsos;
  JButton btnUpdate;

  boolean drawHidden = true;
  boolean algoMode = false;
  int displayMode = 0;
  boolean isShape = false;
  

//=======================================================================//
// Construction
//=======================================================================//
  public HLRDlg(Frame frame)
  {
    super(frame, "Choose Shapes and Projector", true);
    try
    {
      jbInit();
      pack();
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    getContentPane().setLayout(new BorderLayout(5, 5));

    ButtonGroup group;
    JPanel pane;

    group = new ButtonGroup();
    rbDefault = new JRadioButton("Default", true);
    rbDefault.setMargin(new Insets(0, 0, 10, 10));
    rbDefault.setActionCommand("Default");
    rbDefault.addActionListener(this);
    group.add(rbDefault);
    getContentPane().add(rbDefault, BorderLayout.NORTH);

    JPanel mainPane = new JPanel(new GridBagLayout());
    pane = new JPanel(new GridLayout(0, 1, 5, 0));
    pane.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                    "  Visible  "));

    rbVisibleSharpEdges = new JRadioButton("Sharp Edges", false);
    rbVisibleSharpEdges.setMargin(new Insets(0, 5, 0, 0));
    rbVisibleSharpEdges.setActionCommand("VisibleSharpEdges");
    rbVisibleSharpEdges.addActionListener(this);
    group.add(rbVisibleSharpEdges);
    pane.add(rbVisibleSharpEdges);

    rbVisibleSmoothEdges = new JRadioButton("Smooth Edges", false);
    rbVisibleSmoothEdges.setMargin(new Insets(0, 5, 0, 0));
    rbVisibleSmoothEdges.setActionCommand("VisibleSmoothEdges");
    rbVisibleSmoothEdges.addActionListener(this);
    group.add(rbVisibleSmoothEdges);
    pane.add(rbVisibleSmoothEdges);

    rbVisibleSewingEdges = new JRadioButton("Sewing Edges", false);
    rbVisibleSewingEdges.setMargin(new Insets(0, 5, 0, 0));
    rbVisibleSewingEdges.setActionCommand("VisibleSewingEdges");
    rbVisibleSewingEdges.addActionListener(this);
    group.add(rbVisibleSewingEdges);
    pane.add(rbVisibleSewingEdges);

    rbVisibleApparentContour = new JRadioButton("Apparent Contour", false);
    rbVisibleApparentContour.setMargin(new Insets(0, 5, 0, 0));
    rbVisibleApparentContour.setActionCommand("VisibleApparentContour");
    rbVisibleApparentContour.addActionListener(this);
    group.add(rbVisibleApparentContour);
    pane.add(rbVisibleApparentContour);

    rbVisibleIsoParam = new JRadioButton("Iso parametrics", false);
    rbVisibleIsoParam.setMargin(new Insets(0, 5, 5, 0));
    rbVisibleIsoParam.setEnabled(false);
    rbVisibleIsoParam.setActionCommand("VisibleIsoParam");
    rbVisibleIsoParam.addActionListener(this);
    group.add(rbVisibleIsoParam);
    pane.add(rbVisibleIsoParam);
    mainPane.add(pane, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.5,
            GridBagConstraints.NORTH, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    chkDrawHidden = new JCheckBox("Draw Hidden Lines", drawHidden);
    chkDrawHidden.setActionCommand("DrawHiddenLines");
    chkDrawHidden.addActionListener(this);
    mainPane.add(chkDrawHidden, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.SOUTHWEST, GridBagConstraints.HORIZONTAL,
            new Insets(5, 10, 5, 5), 0, 0));

    pane = new JPanel(new GridLayout(0, 1, 5, 0));
    pane.setEnabled(false);
    pane.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                    "  Hidden  "));

    rbHiddenSharpEdges = new JRadioButton("Sharp Edges", false);
    rbHiddenSharpEdges.setMargin(new Insets(0, 5, 0, 0));
    rbHiddenSharpEdges.setActionCommand("HiddenSharpEdges");
    rbHiddenSharpEdges.addActionListener(this);
    group.add(rbHiddenSharpEdges);
    pane.add(rbHiddenSharpEdges);

    rbHiddenSmoothEdges = new JRadioButton("Smooth Edges", false);
    rbHiddenSmoothEdges.setMargin(new Insets(0, 5, 0, 0));
    rbHiddenSmoothEdges.setActionCommand("HiddenSmoothEdges");
    rbHiddenSmoothEdges.addActionListener(this);
    group.add(rbHiddenSmoothEdges);
    pane.add(rbHiddenSmoothEdges);

    rbHiddenSewingEdges = new JRadioButton("Sewing Edges", false);
    rbHiddenSewingEdges.setMargin(new Insets(0, 5, 0, 0));
    rbHiddenSewingEdges.setActionCommand("HiddenSewingEdges");
    rbHiddenSewingEdges.addActionListener(this);
    group.add(rbHiddenSewingEdges);
    pane.add(rbHiddenSewingEdges);

    rbHiddenApparentContour = new JRadioButton("Apparent Contour", false);
    rbHiddenApparentContour.setMargin(new Insets(0, 5, 0, 0));
    rbHiddenApparentContour.setActionCommand("HiddenApparentContour");
    rbHiddenApparentContour.addActionListener(this);
    group.add(rbHiddenApparentContour);
    pane.add(rbHiddenApparentContour);

    rbHiddenIsoParam = new JRadioButton("Iso parametrics", false);
    rbHiddenIsoParam.setMargin(new Insets(0, 5, 5, 0));
    rbHiddenIsoParam.setEnabled(false);
    rbHiddenIsoParam.setActionCommand("HiddenIsoParam");
    rbHiddenIsoParam.addActionListener(this);
    group.add(rbHiddenIsoParam);
    pane.add(rbHiddenIsoParam);
    mainPane.add(pane, new GridBagConstraints(0, 2, 1, 1, 0.0, 0.5,
            GridBagConstraints.NORTH, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    JPanel viewPane = new JPanel(new GridBagLayout());
    viewPane.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                    "  Choose a projector  "));

    JButton button = new JButton("Get Shapes");
    button.setActionCommand("GetShapes");
    button.addActionListener(this);
    viewPane.add(button, new GridBagConstraints(0, 0, 1, 1, 0.5, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    btnUpdate = new JButton("Update 2D");
    btnUpdate.setActionCommand("Update2D");
    btnUpdate.addActionListener(this);
    viewPane.add(btnUpdate, new GridBagConstraints(1, 0, 1, 1, 0.5, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    pane = new JPanel(new FlowLayout(FlowLayout.CENTER, 5, 5));
    group = new ButtonGroup();
    Insets margin = new Insets(1, 1, 1, 1);
    JToggleButton tbutton = new JToggleButton(imgTop);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Top");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    tbutton = new JToggleButton(imgBottom);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Bottom");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    tbutton = new JToggleButton(imgLeft);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Left");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    tbutton = new JToggleButton(imgRight);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Right");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    tbutton = new JToggleButton(imgFront);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Front");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    tbutton = new JToggleButton(imgBack);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Back");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    tbutton = new JToggleButton(imgAxo);
    tbutton.setMargin(margin);
    tbutton.setActionCommand("Axo");
    tbutton.addActionListener(this);
    group.add(tbutton);
    pane.add(tbutton);
    viewPane.add(pane, new GridBagConstraints(0, 1, 2, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 5, 5, 5), 0, 0));

    // Create a CASCADE view
    myViewer = SampleHLRPackage.CreateViewer3d("HLR");
    myViewer.SetDefaultLights();
    myViewer.SetLightOn();
    viewPort = new CASCADEView3d(myViewer) {
                  public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                    SampleHLRPackage.SetWindow3d(view, hiwin, lowin);
                  }
               };
    myAISContext = new AIS_InteractiveContext(myViewer);
    SampleHLRPackage.DisplayTrihedron(myAISContext);
    
    viewPane.add(viewPort, new GridBagConstraints(0, 2, 2, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    chkDegenerateMode = new JCheckBox("Degenerate Mode", true);
    chkDegenerateMode.setActionCommand("DegenerateMode");
    chkDegenerateMode.addActionListener(this);
    viewPane.add(chkDegenerateMode, new GridBagConstraints(0, 3, 2, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 5, 5, 5), 0, 0));
    mainPane.add(viewPane, new GridBagConstraints(1, 0, 1, 3, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));
    getContentPane().add(mainPane, BorderLayout.CENTER);

    JPanel dpane = new JPanel(new GridBagLayout());
    group = new ButtonGroup();
    rbPolyAlgo = new JRadioButton("Poly Algo", !algoMode);
    rbPolyAlgo.setActionCommand("PolyAlgo");
    rbPolyAlgo.addActionListener(this);
    group.add(rbPolyAlgo);
    dpane.add(rbPolyAlgo, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(0, 10, 0, 5), 0, 0));
    rbAlgo = new JRadioButton("Algo", algoMode);
    rbAlgo.setActionCommand("Algo");
    rbAlgo.addActionListener(this);
    group.add(rbAlgo);
    dpane.add(rbAlgo, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(0, 10, 0, 5), 0, 0));

    pane = new JPanel(new FlowLayout(FlowLayout.CENTER, 5, 5));
    pane.add(new JLabel("Nb Isos"));
    txtNbIsos = new IntegerField("2", 6);
    txtNbIsos.setEnabled(false);
    txtNbIsos.setActionCommand("NbIsos");
    txtNbIsos.addActionListener(this);
    txtNbIsos.addFocusListener(this);
    pane.add(txtNbIsos);
    dpane.add(pane, new GridBagConstraints(1, 0, 1, 2, 0.0, 0.0,
            GridBagConstraints.SOUTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    button = new JButton("Close");
    button.setActionCommand("Close");
    button.addActionListener(this);
    dpane.add(button, new GridBagConstraints(2, 0, 1, 2, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 5, 5, 5), 0, 0));
    getContentPane().add(dpane, BorderLayout.SOUTH);

    showButtons(false);
  }


//=======================================================================//
// Commands
//=======================================================================//
  private void showButtons(boolean b)
  {
    rbDefault.setEnabled(b);
    rbVisibleSharpEdges.setEnabled(b);
    rbVisibleSmoothEdges.setEnabled(b);
    rbVisibleSewingEdges.setEnabled(b);
    rbVisibleApparentContour.setEnabled(b);
    chkDrawHidden.setEnabled(b);
    rbPolyAlgo.setEnabled(b);
    rbAlgo.setEnabled(b);
    btnUpdate.setEnabled(b);
    if (b)
    {
      drawHiddenChanged();
      algoModeChanged();
    }
    else
    {
      rbVisibleIsoParam.setEnabled(b);
      rbHiddenSharpEdges.setEnabled(b);
      rbHiddenSmoothEdges.setEnabled(b);
      rbHiddenSewingEdges.setEnabled(b);
      rbHiddenApparentContour.setEnabled(b);
      rbHiddenIsoParam.setEnabled(b);
      txtNbIsos.setEnabled(b);
    }
  }

  private void drawHiddenChanged()
  {
    rbHiddenSharpEdges.setEnabled(drawHidden);
    rbHiddenSmoothEdges.setEnabled(drawHidden);
    rbHiddenSewingEdges.setEnabled(drawHidden);
    rbHiddenApparentContour.setEnabled(drawHidden);
    if (algoMode) rbHiddenIsoParam.setEnabled(drawHidden);
    
    if (!drawHidden && (rbHiddenSharpEdges.isSelected() ||
                        rbHiddenSmoothEdges.isSelected() ||
                        rbHiddenSewingEdges.isSelected() ||
                        rbHiddenApparentContour.isSelected() ||
                        rbHiddenIsoParam.isSelected()))
      rbDefault.setSelected(true);
  }

  private void algoModeChanged()
  {
    rbVisibleIsoParam.setEnabled(algoMode);
    if (drawHidden) rbHiddenIsoParam.setEnabled(algoMode);
    txtNbIsos.setEnabled(algoMode);
    if (!algoMode && (rbVisibleIsoParam.isSelected() ||
                      rbHiddenIsoParam.isSelected()))
      rbDefault.setSelected(true);
  }

  public void getShapes()
  {
    SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
    isShape = SampleHLRPackage.GetShapes(SampleHLRPanel.getAISContext(),
                                         myAISContext);
    viewPort.FitAll();
    SampleHLRPackage.SetNbIsos(txtNbIsos.getValue());
    update2D();
    SampleHLRPanel.getView2d().FitAll();
    showButtons(isShape);
    SamplesStarter.getFrame().setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
  }

  public void update2D()
  {
    SampleHLRPackage.UpdateProjector(viewPort.getView());
    display2D();
  }

  public void display2D()
  {
    int mode = displayMode;
    if (algoMode) mode += 100;
    if (!drawHidden) mode += 1000;
    SampleHLRPackage.Apply(SampleHLRPanel.getInteractiveContext(), mode);
  }

  public void changeNbIsos()
  {
    if (txtNbIsos.getValue() < 1) txtNbIsos.setText("1");
    if (isShape) {
      SampleHLRPackage.SetNbIsos(txtNbIsos.getValue());
      display2D();
    }
  }

  private void close()
  {
    hide();
  }

  
//=======================================================================//
// Focus listener interface
//=======================================================================//
  public void focusGained(FocusEvent event)
  {
  }

  public void focusLost(FocusEvent event)
  {
    changeNbIsos();
  }

//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    
    if (nameAction.equals("Close")) close();

    else if (nameAction.equals("DrawHiddenLines")) {
      drawHidden = chkDrawHidden.isSelected();
      drawHiddenChanged();
      display2D();
    }
    else if (nameAction.equals("PolyAlgo")) {
      algoMode = false;
      algoModeChanged();
      display2D();
    }
    else if (nameAction.equals("Algo")) {
      algoMode = true;
      algoModeChanged();
      display2D();
    }
    else if (nameAction.equals("NbIsos")) changeNbIsos();
    else if (nameAction.equals("GetShapes")) getShapes();
    else if (nameAction.equals("Update2D")) update2D();
    
    // Projection
    else if (nameAction.equals("Front") || nameAction.equals("Back") ||
             nameAction.equals("Top") || nameAction.equals("Bottom") ||
             nameAction.equals("Left") || nameAction.equals("Right") ||
             nameAction.equals("Axo"))
      viewPort.SetProjection(nameAction);

    else if (nameAction.equals("DegenerateMode"))
      viewPort.SetDegenerateMode(chkDegenerateMode.isSelected());

    // Changing displayMode
    else if (nameAction.equals("Default")) {
      displayMode = 0;
      display2D();
    }
    else if (nameAction.equals("VisibleSharpEdges")) {
      displayMode = 1;
      display2D();
    }
    else if (nameAction.equals("VisibleSmoothEdges")) {
      displayMode = 2;
      display2D();
    }
    else if (nameAction.equals("VisibleSewingEdges")) {
      displayMode = 3;
      display2D();
    }
    else if (nameAction.equals("VisibleApparentContour")) {
      displayMode = 4;
      display2D();
    }
    else if (nameAction.equals("VisibleIsoParam")) {
      displayMode = 5;
      display2D();
    }
    else if (nameAction.equals("HiddenSharpEdges")) {
      displayMode = 6;
      display2D();
    }
    else if (nameAction.equals("HiddenSmoothEdges")) {
      displayMode = 7;
      display2D();
    }
    else if (nameAction.equals("HiddenSewingEdges")) {
      displayMode = 8;
      display2D();
    }
    else if (nameAction.equals("HiddenApparentContour")) {
      displayMode = 9;
      display2D();
    }
    else if (nameAction.equals("HiddenIsoParam")) {
      displayMode = 10;
      display2D();
    }
  }

}
