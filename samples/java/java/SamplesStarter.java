
//Title:        OpenCASCADE Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;


public class SamplesStarter extends JPanel
                            implements ChangeListener
{
  // The Frame
  public static Frame myFrame;

  // The width and height of the frame
  public static int WIDTH = 900;
  public static int HEIGHT = 550;
  public static int INITIAL_WIDTH = 400;
  public static int INITIAL_HEIGHT = 200;

  // The Status Line
  private static JLabel myStatusBar = null;
  private Component myCurrentPage = null;

  // Track progress
  public static int totalPanels = 10;
  public static int currentProgressValue;
  public static JLabel progressLabel = null;
  public static JProgressBar progressBar = null;


//=======================================================================//
// Constructor
//=======================================================================//
  public SamplesStarter()
  {
    //-----------------------------------------------------------//
    // Localization
    //-----------------------------------------------------------//
    ResourceBundle AppRes = ResourceBundle.getBundle("properties.AppRes");
    Locale aLocale = new Locale(AppRes.getString("language"), "");

    Locale.setDefault(aLocale);

    setLayout(new BorderLayout());

    //-----------------------------------------------------------//
    // Create a tab pane
    //-----------------------------------------------------------//
    JTabbedPane tabbedPane = new JTabbedPane();
    tabbedPane.addChangeListener(this);
    add(tabbedPane, BorderLayout.CENTER);

    // About panel
    try {
	progressLabel.setText("Loading Title page");
	tabbedPane.addTab("Open CASCADE Technology", new AboutPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);
	  

    // The Geometry sample
    try {
	progressLabel.setText("Loading the Geometry sample");
	tabbedPane.addTab("Geometry", new SampleGeometryPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The Topology samples
    try {
	progressLabel.setText("Loading the Topology samples");
	tabbedPane.addTab("Topology", new SamplesTopologyPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The Viewer3D sample
    try {
	progressLabel.setText("Loading the Viewer3D sample");
	tabbedPane.addTab("Viewer3D", new SampleViewer3DPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The AIS Basic sample
    try {
	progressLabel.setText("Loading the AIS Basic sample");
	tabbedPane.addTab("AIS Basic", new SampleAISBasicPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The AIS DisplayMode sample
    try {
	progressLabel.setText("Loading the AIS DisplayMode sample");
	tabbedPane.addTab("AIS DisplayMode", new SampleAISDisplayModePanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The AIS Select sample
    try {
	progressLabel.setText("Loading the AIS Select sample");
	tabbedPane.addTab("AIS Select", new SampleAISSelectPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The Display Animation sample
    try {
	progressLabel.setText("Loading the Display Animation sample");
	tabbedPane.addTab("DisplayAnimation", new SampleDisplayAnimationPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    // The ImportExport sample
    try {
	progressLabel.setText("Loading the ImportExport sample");
	tabbedPane.addTab("Import/Export", new SampleImportExportPanel());
	progressBar.setValue(++currentProgressValue);
    }
    catch (Exception e) {
	e.printStackTrace();
    }

    // The HLR sample
    try {
	progressLabel.setText("Loading the HLR sample");
	tabbedPane.addTab("HLR", new SampleHLRPanel());
    }
    catch (Exception e) {
	e.printStackTrace();
    }
    progressBar.setValue(++currentProgressValue);

    tabbedPane.setSelectedIndex(0);
    myCurrentPage = tabbedPane.getComponentAt(0);

    //-----------------------------------------------------------//
    // Create a status line
    //-----------------------------------------------------------//
    myStatusBar = new JLabel("");
    myStatusBar.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
    add(myStatusBar, BorderLayout.SOUTH);
    put_info("");
  }

//=======================================================================//
// Start
//=======================================================================//
  public static void main(String[] args)
  {
    try
    {
      UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
//      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    }
    catch(Exception e)
    {
    }

    javax.swing.ToolTipManager.sharedInstance().setLightWeightPopupEnabled(false);

    WindowListener l = new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {System.exit(0);}
    };

    ResourceBundle resIcons =
         ResourceBundle.getBundle("properties.DesktopIcon");
    ImageIcon imageIcon = new ImageIcon(resIcons.getString("MF_MATRALOGO"));

    myFrame = new Frame("Open CASCADE Technology Samples");
    myFrame.setIconImage(imageIcon.getImage());
    myFrame.addWindowListener(l);


    JPanel progressPanel = new JPanel() {
	public Insets getInsets() {
	  return new Insets(40,30,20,30);
	}
    };
    progressPanel.setLayout(new BoxLayout(progressPanel, BoxLayout.Y_AXIS));
    myFrame.add(progressPanel, BorderLayout.CENTER);

    Dimension d = new Dimension(400, 20);
    SamplesStarter.progressLabel = new JLabel("Loading, please wait...");
    SamplesStarter.progressLabel.setAlignmentX(CENTER_ALIGNMENT);
    SamplesStarter.progressLabel.setMaximumSize(d);
    SamplesStarter.progressLabel.setPreferredSize(d);
    progressPanel.add(SamplesStarter.progressLabel);
    progressPanel.add(Box.createRigidArea(new Dimension(1,20)));

    SamplesStarter.progressBar = new JProgressBar(0, SamplesStarter.totalPanels);
    SamplesStarter.progressBar.setStringPainted(true);
    SamplesStarter.progressLabel.setLabelFor(progressBar);
    SamplesStarter.progressBar.setAlignmentX(CENTER_ALIGNMENT);
    progressPanel.add(SamplesStarter.progressBar);

    // show the frame
    myFrame.setSize(INITIAL_WIDTH, INITIAL_HEIGHT);
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    myFrame.setLocation(screenSize.width/2 - INITIAL_WIDTH/2,
			screenSize.height/2 - INITIAL_HEIGHT/2);
    myFrame.show();

    myFrame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

    // Samples creation
    SamplesStarter samplesStarter = new SamplesStarter();

    myFrame.removeAll();
    myFrame.setLayout(new BorderLayout());
    myFrame.add(samplesStarter, BorderLayout.CENTER);
    myFrame.setSize(WIDTH, HEIGHT);
    myFrame.setLocation(screenSize.width/2 - WIDTH/2,
			screenSize.height/2 - HEIGHT/2);

    myFrame.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));

    myFrame.validate();
    //    myFrame.repaint();
    samplesStarter.requestDefaultFocus();
  }

  public static void put_info(String message)
  {
    if (myStatusBar != null)
      myStatusBar.setText(". " + message);
  }

  public static Frame getFrame()
  {
    return myFrame;
  }

//=======================================================================//
//                          Change Listener                              //
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    SamplesStarter.put_info("");

    JTabbedPane tab = (JTabbedPane) event.getSource();
    int index = tab.getSelectedIndex();
    Component currentPage = tab.getComponentAt(index);

    if (myCurrentPage != null)
    {
      myCurrentPage.setVisible(false);
      myCurrentPage.setEnabled(false);
    }
    currentPage.setVisible(true);
    currentPage.setEnabled(true);

    myCurrentPage = currentPage;
  }

}
