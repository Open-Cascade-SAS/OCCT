
//Title:        Cas.Cade Object Library - samples of using
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      MatraDatavision
//Description:  

import java.awt.*;
import java.awt.event.*;
import java.math.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import util.*;
import CASCADESamplesJni.*;
// import CASCADESamplesJni.Aspect_TypeOfLine;
// import CASCADESamplesJni.TShort_Array1OfShortReal;
// import CASCADESamplesJni.V2d_Viewer;
// import CASCADESamplesJni.Quantity_Color;
// import CASCADESamplesJni.Quantity_NameOfColor;
// import CASCADESamplesJni.Quantity_TypeOfColor;
// import CASCADESamplesJni.Aspect_TypeMap;
// import CASCADESamplesJni.Aspect_FontMap;
// import CASCADESamplesJni.Aspect_MarkMap;
// import CASCADESamplesJni.Aspect_FontStyle;
// import CASCADESamplesJni.Aspect_LineStyle;
// import CASCADESamplesJni.Aspect_WidthMap;
// import CASCADESamplesJni.Aspect_ColorMap;
// import CASCADESamplesJni.Aspect_WidthOfLine;

import jcas.Standard_CString;
import SampleHLRJni.*;

class PropertiesDlg extends JDialog
                    implements ActionListener
{
  private V2d_Viewer myViewer;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  WidthMapPane myWidthPane;
  ColorMapPane myColorPane;
  FontMapPane myFontPane;
  TypeMapPane myTypePane;
  MarkMapPane myMarkPane;
  JButton btnApply;


//=======================================================================//
// Construction
//=======================================================================//
  public PropertiesDlg(Frame frame, V2d_Viewer viewer)
  {
    super(frame, "View Map Properties", true);
    myViewer = viewer;
    try
    {
      jbInit();
//    setSize(500, 500);
      pack();
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    getContentPane().setLayout(new BorderLayout());

    JTabbedPane myTabbedPane = new JTabbedPane();
    myTabbedPane.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

    myWidthPane = new WidthMapPane(myViewer, this);
    myTabbedPane.addTab("Width Map", myWidthPane);

    myColorPane = new ColorMapPane(myViewer, this);
    myTabbedPane.addTab("Color Map", myColorPane);

    myFontPane = new FontMapPane(myViewer, this);
    myTabbedPane.addTab("Font Map", myFontPane);

    myTypePane = new TypeMapPane(myViewer, this);
    myTabbedPane.addTab("Type Map", myTypePane);

    myMarkPane = new MarkMapPane(myViewer, this);
    myTabbedPane.addTab("Mark Map", myMarkPane);

    getContentPane().add(myTabbedPane, BorderLayout.CENTER);

    JPanel controlPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 5));

    JButton btnOK = new JButton("OK");
    btnOK.addActionListener(this);
    btnOK.setActionCommand("OK");
    controlPanel.add(btnOK);

    JButton btnCancel = new JButton("Cancel");
    btnCancel.addActionListener(this);
    btnCancel.setActionCommand("Cancel");
    controlPanel.add(btnCancel);

    btnApply = new JButton("Apply");
    btnApply.addActionListener(this);
    btnApply.setActionCommand("Apply");
    btnApply.setEnabled(false);
    controlPanel.add(btnApply);
    getContentPane().add(controlPanel, BorderLayout.SOUTH);
  }


//=======================================================================//
// Commands
//=======================================================================//
  public void setModified()
  {
    btnApply.setEnabled(true);
  }

//=======================================================================//
  public String format(double value)
  {
    return format(value, 6);
  }

  public String format(double value, int prec)
  {
    BigDecimal aValue = new BigDecimal(value);
    return aValue.setScale(prec, BigDecimal.ROUND_HALF_UP).toString();
  }

//=======================================================================//
  private void apply()
  {
    myWidthPane.apply();
    myColorPane.apply();
    myFontPane.apply();
    myTypePane.apply();
    myMarkPane.apply();
    
    myViewer.Update();
    
    btnApply.setEnabled(false);
  }

//=======================================================================//
  private void close(boolean isOK)
  {
    if (isOK) apply();
    dispose();
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Apply")) apply();
    else if (nameAction.equals("OK")) close(true);
    else if (nameAction.equals("Cancel")) close(false);
  }

}



//=======================================================================//
//                                                                       //
//                              WidthMapPane                             //
//                                                                       //
//=======================================================================//
class WidthMapPane extends JPanel implements ChangeListener,
                                             ActionListener
{
  private V2d_Viewer myViewer;
  private Aspect_WidthMap myWidthMap;
  private PropertiesDlg myDlg;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JLabel lblMapSize;
  TitledBorder brdEntries;
  JSlider sldEntries;
  JLabel lblType;
  JLabel lblWidth;
  JComboBox cmbType;
  RealField txtWidth;
  JButton btnUpdate;


//=======================================================================//
// Construction
//=======================================================================//
  public WidthMapPane(V2d_Viewer viewer, PropertiesDlg parent)
  {
    myViewer = viewer;
//     myWidthMap = new Aspect_WidthMap(myViewer.WidthMap());
    myWidthMap = myViewer.WidthMap();
    myDlg = parent;

    try
    {
      jbInit();
      initValues();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    setLayout(new GridBagLayout());

    JPanel labelPanel = new JPanel();
    labelPanel.add(new JLabel("Size of the width map: "));
    lblMapSize = new JLabel();
    labelPanel.add(lblMapSize);
    add(labelPanel, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(5, 10, 5, 10), 0, 0));

    // Entries pane
    JPanel entriesPanel = new JPanel(new GridBagLayout());
    brdEntries = new TitledBorder(BorderFactory.createEtchedBorder(), "Entries:");
    entriesPanel.setBorder(brdEntries);

    sldEntries = new JSlider();
    sldEntries.setMinimum(0);
    sldEntries.setMinorTickSpacing(1);
    sldEntries.setPaintLabels(true);
    sldEntries.setPaintTicks(true);
    sldEntries.setPaintTrack(true);
    sldEntries.addChangeListener(this);
    entriesPanel.add(sldEntries, new GridBagConstraints(0, 0, 2, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Type: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblType = new JLabel();
    entriesPanel.add(lblType, new GridBagConstraints(1, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Width: "), new GridBagConstraints(0, 2, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblWidth = new JLabel();
    entriesPanel.add(lblWidth, new GridBagConstraints(1, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    add(entriesPanel, new GridBagConstraints(0, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    // New entry pane
    JPanel entryPanel = new JPanel(new GridBagLayout());
    entryPanel.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                         "  New entry:  "));

    entryPanel.add(new JLabel("Type: "), new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    String[] items = {"THIN", "MEDIUM", "THICK", "VERYTHICK", "USERDEFINED"};
    cmbType = new JComboBox(items);
    cmbType.setEditable(false);
    cmbType.addActionListener(this);
    cmbType.setActionCommand("Type");
    entryPanel.add(cmbType, new GridBagConstraints(1, 0, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entryPanel.add(new JLabel("Width: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtWidth = new RealField(10);
    entryPanel.add(txtWidth, new GridBagConstraints(1, 1, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    btnUpdate = new JButton("Update current");
    btnUpdate.addActionListener(this);
    btnUpdate.setActionCommand("Update");
    entryPanel.add(btnUpdate, new GridBagConstraints(2, 0, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    JButton button = new JButton("Add");
    button.addActionListener(this);
    button.setActionCommand("Add");
    entryPanel.add(button, new GridBagConstraints(2, 1, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    add(entryPanel, new GridBagConstraints(0, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
  }

  private void initValues()
  {
    updateEntries(0);
  }


//=======================================================================//
// Commands
//=======================================================================//
  public void updateEntries(int index)
  {
    int mapSize = myWidthMap.Size();
    lblMapSize.setText("" + mapSize);
    if (index == -1) index = mapSize - 1;

    sldEntries.setMaximum(mapSize - 1);
    sldEntries.setValue(index);

    if (sldEntries.getMaximum() <= 15)
    {
      sldEntries.setMajorTickSpacing(1);
      sldEntries.setSnapToTicks(true);
    }
    else if (sldEntries.getMaximum() <= 75)
    {
      sldEntries.setMajorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }
    else if (sldEntries.getMaximum() <= 150)
    {
      sldEntries.setMajorTickSpacing(10);
      sldEntries.setSnapToTicks(false);
    }
    else
    {
      sldEntries.setMajorTickSpacing(25);
      sldEntries.setMinorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }

    updateEntryPane(index);
  }

//=======================================================================//
  public void updateEntryPane(int index)
  {
    brdEntries.setTitle("  Entries:   " + index + "  ");
    repaint();

    Aspect_WidthMapEntry anEntry = myWidthMap.Entry(index + 1);

    switch (anEntry.Type())
    {
      case Aspect_WidthOfLine.Aspect_WOL_THIN:
        lblType.setText("Aspect_WOL_THIN");
        break;
      case Aspect_WidthOfLine.Aspect_WOL_MEDIUM:
        lblType.setText("Aspect_WOL_MEDIUM");
        break;
      case Aspect_WidthOfLine.Aspect_WOL_THICK:
        lblType.setText("Aspect_WOL_THICK");
        break;
      case Aspect_WidthOfLine.Aspect_WOL_VERYTHICK:
        lblType.setText("Aspect_WOL_VERYTHICK");
        break;
      case Aspect_WidthOfLine.Aspect_WOL_USERDEFINED:
        lblType.setText("Aspect_WOL_USERDEFINED");
        break;
      default:
        lblType.setText("Unknown");
    }
    
    lblWidth.setText(myDlg.format(anEntry.Width()));
    cmbType.setSelectedIndex(anEntry.Type());
    txtWidth.setText(myDlg.format(anEntry.Width()));

    if (index == 0)
      btnUpdate.setEnabled(false);
    else
      btnUpdate.setEnabled(true);
  }

//=======================================================================//
  public void onTypeChanged()
  {
    if (cmbType.getSelectedIndex() == 4)  // USERDEFINED
      txtWidth.setEnabled(true);
    else
    {
      txtWidth.setEnabled(false);
      Aspect_WidthMapEntry aTmpEntry =
          new Aspect_WidthMapEntry(99, cmbType.getSelectedIndex());
      txtWidth.setText(myDlg.format(aTmpEntry.Width()));
    }
  }

//=======================================================================//
  public void updateCurrentEntry()
  {
    Aspect_WidthMapEntry anEntry = myWidthMap.Entry(sldEntries.getValue() + 1);
    if (cmbType.getSelectedIndex() == Aspect_WidthOfLine.Aspect_WOL_USERDEFINED)
      anEntry.SetWidth(txtWidth.getValue());
    else
      anEntry.SetType((short)cmbType.getSelectedIndex());
    myWidthMap.AddEntry(anEntry);
    myDlg.setModified();
    updateEntryPane(sldEntries.getValue());
  }

//=======================================================================//
  public void addNewEntry()
  {
    int newEntryIndex;
    if (cmbType.getSelectedIndex() == Aspect_WidthOfLine.Aspect_WOL_USERDEFINED)
      newEntryIndex = myWidthMap.AddEntry(txtWidth.getValue());
    else
      newEntryIndex = myWidthMap.AddEntry(cmbType.getSelectedIndex());
    myDlg.setModified();
    updateEntries(newEntryIndex);
  }

//=======================================================================//
  public void apply()
  {
//     myViewer.SetWidthMap(myWidthMap.getAspectWidthMap());
    myViewer.SetWidthMap(myWidthMap);
  }

//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldEntries))
      updateEntryPane(sldEntries.getValue());
  }

//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Type")) onTypeChanged();
    else if (nameAction.equals("Update")) updateCurrentEntry();
    else if (nameAction.equals("Add")) addNewEntry();
  }

}



//=======================================================================//
//                                                                       //
//                              ColorMapPane                             //
//                                                                       //
//=======================================================================//
class ColorMapPane extends JPanel implements ChangeListener,
                                             ActionListener,
                                             FocusListener
{
  private V2d_Viewer myViewer;
  private Aspect_ColorMap myColorMap;
  private PropertiesDlg myDlg;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JLabel lblMapSize, lblMapType;
  TitledBorder brdEntries;
  JSlider sldEntries;
  JLabel lblRed, lblGreen, lblBlue;
  JLabel lblName;
  JPanel entryPanel;
  RealField txtRed, txtGreen, txtBlue;
  JComboBox cmbName;
  JLabel lblNotAvailable;
  JButton btnUpdate;


//=======================================================================//
// Construction
//=======================================================================//
  public ColorMapPane(V2d_Viewer viewer, PropertiesDlg parent)
  {
    myViewer = viewer;
//     myColorMap = new Aspect_ColorMap(myViewer.ColorMap());
    myColorMap = myViewer.ColorMap();
    myDlg = parent;

    try
    {
      jbInit();
      initValues();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    setLayout(new GridBagLayout());

    JPanel infoPanel = new JPanel(new GridBagLayout());

    JPanel labelPanel = new JPanel(new GridBagLayout());
    labelPanel.add(new JLabel("Type of the color map: "),
            new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblMapType = new JLabel();
    labelPanel.add(lblMapType, new GridBagConstraints(1, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    labelPanel.add(new JLabel("Size of the color map: "),
            new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblMapSize = new JLabel();
    labelPanel.add(lblMapSize, new GridBagConstraints(1, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    infoPanel.add(labelPanel, new GridBagConstraints(0, 0, 1, 1, 0.7, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    JPanel buttonsPanel = new JPanel(new GridLayout(0, 1, 5, 5));
    JButton button = new JButton("New generic color map");
    button.addActionListener(this);
    button.setActionCommand("NewGeneric");
    buttonsPanel.add(button);
    button = new JButton("New color cube color map");
    button.addActionListener(this);
    button.setActionCommand("NewColorCube");
    buttonsPanel.add(button);
    button = new JButton("New color ramp color map");
    button.addActionListener(this);
    button.setActionCommand("NewColorRamp");
    buttonsPanel.add(button);
    infoPanel.add(buttonsPanel, new GridBagConstraints(1, 0, 1, 1, 0.3, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    add(infoPanel, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    // Entries pane
    JPanel entriesPanel = new JPanel(new GridBagLayout());
    brdEntries = new TitledBorder(BorderFactory.createEtchedBorder(), "Entries:");
    entriesPanel.setBorder(brdEntries);

    sldEntries = new JSlider();
    sldEntries.setMinimum(0);
    sldEntries.setMinorTickSpacing(1);
    sldEntries.setPaintLabels(true);
    sldEntries.setPaintTicks(true);
    sldEntries.setPaintTrack(true);
    sldEntries.addChangeListener(this);
    entriesPanel.add(sldEntries, new GridBagConstraints(0, 0, 5, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Color: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblRed = new JLabel();
    entriesPanel.add(lblRed, new GridBagConstraints(1, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblGreen = new JLabel();
    entriesPanel.add(lblGreen, new GridBagConstraints(2, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblBlue = new JLabel();
    entriesPanel.add(lblBlue, new GridBagConstraints(3, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblName = new JLabel();
    entriesPanel.add(lblName, new GridBagConstraints(4, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    add(entriesPanel, new GridBagConstraints(0, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    // New entry pane
    entryPanel = new JPanel(new GridLayout(0, 1, 5, 5));
    entryPanel.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                           "  New entry:  "));

    JPanel colorPanel = new JPanel(new GridBagLayout());
    colorPanel.add(new JLabel("Color: "), new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtRed = new RealField(6);
    txtRed.addFocusListener(this);
    colorPanel.add(txtRed, new GridBagConstraints(1, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtGreen = new RealField(6);
    txtGreen.addFocusListener(this);
    colorPanel.add(txtGreen, new GridBagConstraints(2, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtBlue = new RealField(6);
    txtBlue.addFocusListener(this);
    colorPanel.add(txtBlue, new GridBagConstraints(3, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    String[] items = new String[517];
    for (int i = 0; i < 517; i++)
      items[i] = Quantity_Color.StringName((short)i).GetValue();
    cmbName = new JComboBox(items);
    cmbName.setEditable(false);
    cmbName.addActionListener(this);
    cmbName.setActionCommand("ColorName");
    colorPanel.add(cmbName, new GridBagConstraints(4, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    entryPanel.add(colorPanel);

    buttonsPanel = new JPanel(new GridLayout(1, 0, 10, 20));
    buttonsPanel.setBorder(BorderFactory.createEmptyBorder(0, 10, 10, 10));
    btnUpdate = new JButton("Update current");
    btnUpdate.addActionListener(this);
    btnUpdate.setActionCommand("Update");
    buttonsPanel.add(btnUpdate);
    button = new JButton("Add");
    button.addActionListener(this);
    button.setActionCommand("Add");
    buttonsPanel.add(button);
    button = new JButton("Edit...");
    button.addActionListener(this);
    button.setActionCommand("Edit");
    buttonsPanel.add(button);
    entryPanel.add(buttonsPanel);
    add(entryPanel, new GridBagConstraints(0, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    lblNotAvailable = new JLabel("Not available for this type of Color map");
    lblNotAvailable.setHorizontalAlignment(SwingConstants.CENTER);
    lblNotAvailable.setVerticalAlignment(SwingConstants.CENTER);
    lblNotAvailable.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                           "  New entry:  "));
    add(lblNotAvailable, new GridBagConstraints(0, 3, 1, 1, 1.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
  }

  private void initValues()
  {
    updateEntries(0);
  }


//=======================================================================//
// Commands
//=======================================================================//
  public void updateEntries(int index)
  {
    switch (myColorMap.Type())
    {
      case Aspect_TypeOfColorMap.Aspect_TOC_Generic:
        lblMapType.setText("Generic");
        break;
      case Aspect_TypeOfColorMap.Aspect_TOC_ColorCube:
        lblMapType.setText("ColorCube");
        break;
      case Aspect_TypeOfColorMap.Aspect_TOC_ColorRamp:
        lblMapType.setText("ColorRamp");
        break;
      default:
        lblMapType.setText("Unknown");
    }

    if (myColorMap.Type() == Aspect_TypeOfColorMap.Aspect_TOC_Generic)
    {
      lblNotAvailable.setVisible(false);
      entryPanel.setVisible(true);
    }
    else
    {
      lblNotAvailable.setVisible(true);
      entryPanel.setVisible(false);
    }

    int mapSize = myColorMap.Size();
    lblMapSize.setText("" + mapSize);
    if (index == -1) index = mapSize - 1;

    sldEntries.setMaximum(mapSize - 1);
    sldEntries.setValue(index);

    if (sldEntries.getMaximum() <= 15)
    {
      sldEntries.setMajorTickSpacing(1);
      sldEntries.setMinorTickSpacing(1);
      sldEntries.setSnapToTicks(true);
      sldEntries.setPaintLabels(true);
    }
    else if (sldEntries.getMaximum() <= 75)
    {
      sldEntries.setMajorTickSpacing(5);
      sldEntries.setMinorTickSpacing(1);
      sldEntries.setSnapToTicks(false);
      sldEntries.setPaintLabels(true);
    }
    else if (sldEntries.getMaximum() <= 150)
    {
      sldEntries.setMajorTickSpacing(10);
      sldEntries.setMinorTickSpacing(2);
      sldEntries.setSnapToTicks(false);
      sldEntries.setPaintLabels(true);
    }
    else
    {
      sldEntries.setMajorTickSpacing(25);
      sldEntries.setMinorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
      sldEntries.setPaintLabels(false);
    }

    updateEntryPane(index);
  }

  public void updateEntryPane(int index)
  {
    brdEntries.setTitle("  Entries:   " + index + "  ");
    repaint();

    Aspect_ColorMapEntry anEntry = myColorMap.FindEntry(index);

    if (anEntry.IsAllocated())
    {
      Quantity_Color aColor = anEntry.Color();
      double red = aColor.Red();
      double green = aColor.Green();
      double blue = aColor.Blue();
      int name = aColor.Name();

      lblRed.setText(myDlg.format(red));
      lblGreen.setText(myDlg.format(green));
      lblBlue.setText(myDlg.format(blue));
      lblName.setText(Quantity_Color.StringName((short)name).toString());

      txtRed.setText(myDlg.format(red));
      txtGreen.setText(myDlg.format(green));
      txtBlue.setText(myDlg.format(blue));
      cmbName.setSelectedIndex(name);
    }
    else
    {
      lblRed.setText("");
      lblGreen.setText("");
      lblBlue.setText("");
      lblName.setText("Not allocated");

      txtRed.setText("");
      txtGreen.setText("");
      txtBlue.setText("");
      cmbName.setSelectedIndex(0);
    }

    if (index == 0)
      btnUpdate.setEnabled(false);
    else
      btnUpdate.setEnabled(true);
  }

  public void newGenericMap()
  {
    myColorMap = V2d_DefaultMap.ColorMap();
    myDlg.setModified();
    updateEntries(0);
  }

  public void newColorCubeMap()
  {
    ColorCubeDlg aDlg = new ColorCubeDlg(myDlg);
    Position.centerWindow(aDlg);
    aDlg.show();
    if (aDlg.isOK())
    {
      myColorMap = aDlg.getColorMap();
      myDlg.setModified();
      updateEntries(0);
    }
  }

  public void newColorRampMap()
  {
    ColorRampDlg aDlg = new ColorRampDlg(myDlg);
    Position.centerWindow(aDlg);
    aDlg.show();
    if (aDlg.isOK())
    {
      myColorMap = aDlg.getColorMap();
      myDlg.setModified();
      updateEntries(0);
    }
  }

  public void onColorNameChanged()
  {
    Quantity_Color aColor = new Quantity_Color((short)cmbName.getSelectedIndex());
    txtRed.setText(myDlg.format(aColor.Red()));
    txtGreen.setText(myDlg.format(aColor.Green()));
    txtBlue.setText(myDlg.format(aColor.Blue()));
  }

  public void onColorEdit()
  {
    Color newColor = JColorChooser.showDialog(myDlg, "Select Color",
            new Color((float) txtRed.getValue(), (float) txtGreen.getValue(),
                      (float) txtBlue.getValue()));
    if (newColor != null)
    {
      double red = newColor.getRed()/255.;
      double green = newColor.getGreen()/255.;
      double blue = newColor.getBlue()/255.;
      txtRed.setText(myDlg.format(red));
      txtGreen.setText(myDlg.format(green));
      txtBlue.setText(myDlg.format(blue));
      cmbName.setSelectedIndex(Quantity_Color.Name(red, green, blue));
    }
  }

  public void updateCurrentEntry()
  {
    Aspect_GenericColorMap aMap = null;
    if (myColorMap instanceof Aspect_GenericColorMap)
      aMap = (Aspect_GenericColorMap) myColorMap;
    else if (myColorMap.Type() == Aspect_TypeOfColorMap.Aspect_TOC_Generic)
      aMap = (Aspect_GenericColorMap) myColorMap.DownCast(
              new Standard_CString("Aspect_GenericColorMap"),
              new Standard_CString("SampleHLRJni"));

    if (aMap != null)
    {
      Aspect_ColorMapEntry anEntry = myColorMap.FindEntry(sldEntries.getValue());
      Quantity_Color aColor = new Quantity_Color(txtRed.getValue(),
              txtGreen.getValue(), txtBlue.getValue(),
              Quantity_TypeOfColor.Quantity_TOC_RGB);
      anEntry.SetColor(aColor);
      aMap.AddEntry(anEntry);
      myDlg.setModified();
      updateEntryPane(sldEntries.getValue());
    }
  }

  public void addNewEntry()
  {
    Aspect_GenericColorMap aMap = null;
    if (myColorMap instanceof Aspect_GenericColorMap)
      aMap = (Aspect_GenericColorMap) myColorMap;
    else if (myColorMap.Type() == Aspect_TypeOfColorMap.Aspect_TOC_Generic)
      aMap = (Aspect_GenericColorMap) myColorMap.DownCast(
              new Standard_CString("Aspect_GenericColorMap"),
              new Standard_CString("SampleHLRJni"));

    if (aMap != null)
    {
      Quantity_Color aColor = new Quantity_Color(txtRed.getValue(),
              txtGreen.getValue(), txtBlue.getValue(),
              Quantity_TypeOfColor.Quantity_TOC_RGB);
      aMap.AddEntry(aColor);
      myDlg.setModified();
      updateEntries(-1);
    }
  }

  public void apply()
  {
//     myViewer.SetColorMap(myColorMap.getAspectColorMap());
    if(myColorMap instanceof Aspect_GenericColorMap)
      myViewer.SetColorMap((Aspect_GenericColorMap)myColorMap); // AVO: cast inserted
    else{
      System.out.println("Error: !!!!!!!!! Not the generic color map!!!");
    }
  }

//=======================================================================//
// Focus listener interface
//=======================================================================//
  public void focusGained(FocusEvent event)
  {
  }

  public void focusLost(FocusEvent event)
  {
    RealField field = (RealField) event.getSource();
    if (field.getValue() < 0) field.setText("0");
    if (field.getValue() > 1) field.setText("1");
    cmbName.setSelectedIndex(Quantity_Color.Name(txtRed.getValue(),
                             txtGreen.getValue(), txtBlue.getValue()));
  }

//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldEntries))
      updateEntryPane(sldEntries.getValue());
  }

//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("ColorName")) onColorNameChanged();
    else if (nameAction.equals("Edit")) onColorEdit();
    else if (nameAction.equals("Update")) updateCurrentEntry();
    else if (nameAction.equals("Add")) addNewEntry();
    else if (nameAction.equals("NewGeneric")) newGenericMap();
    else if (nameAction.equals("NewColorCube")) newColorCubeMap();
    else if (nameAction.equals("NewColorRamp")) newColorRampMap();
  }

}



//=======================================================================//
//                                                                       //
//                               FontMapPane                             //
//                                                                       //
//=======================================================================//
class FontMapPane extends JPanel implements ChangeListener,
                                            ActionListener
{
  private V2d_Viewer myViewer;
  private Aspect_FontMap myFontMap;
  private PropertiesDlg myDlg;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JLabel lblMapSize;
  TitledBorder brdEntries;
  JSlider sldEntries;
  JLabel lblType, lblValue, lblSize, lblSlant;
  JComboBox cmbType;
  JTextField txtValue;
  IntegerField txtSize, txtSlant;
  JButton btnEdit, btnUpdate, btnAdd;


//=======================================================================//
// Construction
//=======================================================================//
  public FontMapPane(V2d_Viewer viewer, PropertiesDlg parent)
  {
    myViewer = viewer;
//     myFontMap = new Aspect_FontMap(myViewer.FontMap());
    myFontMap = myViewer.FontMap();
    myDlg = parent;

    try
    {
      jbInit();
      initValues();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    setLayout(new GridBagLayout());

    JPanel labelPanel = new JPanel();
    labelPanel.add(new JLabel("Size of the font map: "));
    lblMapSize = new JLabel();
    labelPanel.add(lblMapSize);
    add(labelPanel, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(5, 10, 5, 10), 0, 0));

    // Entries pane
    JPanel entriesPanel = new JPanel(new GridBagLayout());
    brdEntries = new TitledBorder(BorderFactory.createEtchedBorder(), "Entries:");
    entriesPanel.setBorder(brdEntries);

    sldEntries = new JSlider();
    sldEntries.setMinimum(0);
    sldEntries.setMinorTickSpacing(1);
    sldEntries.setPaintLabels(true);
    sldEntries.setPaintTicks(true);
    sldEntries.setPaintTrack(true);
    sldEntries.addChangeListener(this);
    entriesPanel.add(sldEntries, new GridBagConstraints(0, 0, 2, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Type: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblType = new JLabel();
    entriesPanel.add(lblType, new GridBagConstraints(1, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Value: "), new GridBagConstraints(0, 2, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblValue = new JLabel();
    entriesPanel.add(lblValue, new GridBagConstraints(1, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Size: "), new GridBagConstraints(0, 3, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblSize = new JLabel();
    entriesPanel.add(lblSize, new GridBagConstraints(1, 3, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Slant: "), new GridBagConstraints(0, 4, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblSlant = new JLabel();
    entriesPanel.add(lblSlant, new GridBagConstraints(1, 4, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    add(entriesPanel, new GridBagConstraints(0, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    // New entry pane
    JPanel entryPanel = new JPanel(new GridBagLayout());
    entryPanel.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                         "  New entry:  "));

    entryPanel.add(new JLabel("Type: "), new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    String[] items = {"DEFAULT", "COURIER", "HELVETICA", "TIMES", "USERDEFINED"};
    cmbType = new JComboBox(items);
    cmbType.setEditable(false);
    cmbType.addActionListener(this);
    cmbType.setActionCommand("Type");
    entryPanel.add(cmbType, new GridBagConstraints(1, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    btnEdit = new JButton("Edit...");
    btnEdit.addActionListener(this);
    btnEdit.setActionCommand("Edit");
    entryPanel.add(btnEdit, new GridBagConstraints(2, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entryPanel.add(new JLabel("Value: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtValue = new JTextField(20);
    txtValue.setEditable(false);
    entryPanel.add(txtValue, new GridBagConstraints(1, 1, 2, 1, 1.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entryPanel.add(new JLabel("Size: "), new GridBagConstraints(0, 2, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtSize = new IntegerField(6);
    entryPanel.add(txtSize, new GridBagConstraints(1, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    btnUpdate = new JButton("Update current");
    btnUpdate.addActionListener(this);
    btnUpdate.setActionCommand("Update");
    entryPanel.add(btnUpdate, new GridBagConstraints(2, 2, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entryPanel.add(new JLabel("Slant: "), new GridBagConstraints(0, 3, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtSlant = new IntegerField(6);
    entryPanel.add(txtSlant, new GridBagConstraints(1, 3, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    btnAdd = new JButton("Add");
    btnAdd.addActionListener(this);
    btnAdd.setActionCommand("Add");
    entryPanel.add(btnAdd, new GridBagConstraints(2, 3, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    add(entryPanel, new GridBagConstraints(0, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
  }

  private void initValues()
  {
    updateEntries(0);
  }


//=======================================================================//
// Commands
//=======================================================================//
  public void updateEntries(int index)
  {
    int mapSize = myFontMap.Size();
    lblMapSize.setText("" + mapSize);
    if (index == -1) index = mapSize - 1;

    sldEntries.setMaximum(mapSize - 1);
    sldEntries.setValue(index);

    if (sldEntries.getMaximum() <= 15)
    {
      sldEntries.setMajorTickSpacing(1);
      sldEntries.setSnapToTicks(true);
    }
    else if (sldEntries.getMaximum() <= 75)
    {
      sldEntries.setMajorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }
    else if (sldEntries.getMaximum() <= 150)
    {
      sldEntries.setMajorTickSpacing(10);
      sldEntries.setSnapToTicks(false);
    }
    else
    {
      sldEntries.setMajorTickSpacing(25);
      sldEntries.setMinorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }

    updateEntryPane(index);
  }

  public void updateEntryPane(int index)
  {
    brdEntries.setTitle("  Entries:   " + index + "  ");
    repaint();

    Aspect_FontMapEntry anEntry = myFontMap.Entry(index + 1);
    Aspect_FontStyle aStyle = anEntry.Type();

    switch (aStyle.Style())
    {
      case Aspect_TypeOfFont.Aspect_TOF_DEFAULT:
        lblType.setText("Aspect_TOF_DEFAULT");
        break;
      case Aspect_TypeOfFont.Aspect_TOF_COURIER:
        lblType.setText("Aspect_TOF_COURIER");
        break;
      case Aspect_TypeOfFont.Aspect_TOF_HELVETICA:
        lblType.setText("Aspect_TOF_HELVETICA");
        break;
      case Aspect_TypeOfFont.Aspect_TOF_TIMES:
        lblType.setText("Aspect_TOF_TIMES");
        break;
      case Aspect_TypeOfFont.Aspect_TOF_USERDEFINED:
        lblType.setText("Aspect_TOF_USERDEFINED");
        break;
      default:
        lblType.setText("Unknown");
    }

    lblValue.setText(aStyle.Value().GetValue());
    lblSize.setText(myDlg.format(aStyle.Size(), 1));
    lblSlant.setText(myDlg.format(aStyle.Slant(), 1));
    
    cmbType.setSelectedIndex(aStyle.Style());
    txtValue.setText(aStyle.Value().GetValue());
    txtSize.setText(myDlg.format(aStyle.Size(), 1));
    txtSlant.setText(myDlg.format(aStyle.Slant(), 1));
  }

  public void onTypeChanged()
  {
    if (cmbType.getSelectedIndex() == Aspect_TypeOfFont.Aspect_TOF_USERDEFINED)
    {
      txtValue.setText("Please, select the font");
      txtSize.setText("");
      txtSlant.setText("");
      btnEdit.setEnabled(true);
      btnUpdate.setEnabled(false);
      btnAdd.setEnabled(false);
      txtSize.setEnabled(false);
      txtSlant.setEnabled(false);
    }
    else
    {
      Aspect_FontStyle aStyle = new Aspect_FontStyle((short)cmbType.getSelectedIndex(),
                                                     0.3, 0.0, false);
      txtValue.setText(aStyle.Value().GetValue());
      txtSize.setText(myDlg.format(aStyle.Size(), 1));
      txtSlant.setText(myDlg.format(aStyle.Slant(), 1));
      btnEdit.setEnabled(false);
      btnUpdate.setEnabled(true);
      btnAdd.setEnabled(true);
      txtSize.setEnabled(true);
      txtSlant.setEnabled(true);
    }
  }

  public void onFontEdit()
  {
    Font defFont = new Font("Helvetica", Font.BOLD, 10); 
    JFontChooser aDlg = new JFontChooser(defFont);
    if (aDlg.showDialog(myDlg, "Choose a font") == JFontChooser.ACCEPT_OPTION)
    {
      Font newFont = aDlg.getSelectedFont();
      Standard_CString name =
          new Standard_CString(newFont.getFamily() + (newFont.isBold() ?
                  (newFont.isItalic() ? "-BoldItalic" : "-Bold") :
                  (newFont.isItalic() ? "-Italic" : "")));
      Aspect_FontStyle aStyle =
          new Aspect_FontStyle(name, newFont.getSize()*25.4/72., 0.0, false);
      txtValue.setText(aStyle.Value().GetValue());
      txtSize.setText(myDlg.format(aStyle.Size(), 1));
      txtSlant.setText("");
      btnUpdate.setEnabled(true);
      btnAdd.setEnabled(true);
    }
  }

  public void updateCurrentEntry()
  {
    Aspect_FontMapEntry anEntry = myFontMap.Entry(sldEntries.getValue() + 1);
    if (cmbType.getSelectedIndex() == Aspect_TypeOfFont.Aspect_TOF_USERDEFINED)
    {
      Aspect_FontStyle aStyle =
          new Aspect_FontStyle(new Standard_CString(txtValue.getText()));
      anEntry.SetType(aStyle);
    }
    else
    {
      Aspect_FontStyle aStyle =
          new Aspect_FontStyle((short)cmbType.getSelectedIndex(), (double)txtSize.getValue(),
                               (double)txtSlant.getValue(), false);
      anEntry.SetType(aStyle);
    }
    myFontMap.AddEntry(anEntry);
    myDlg.setModified();
    updateEntryPane(sldEntries.getValue());
  }

  public void addNewEntry()
  {
    Aspect_FontStyle aStyle;
    if (cmbType.getSelectedIndex() == Aspect_TypeOfFont.Aspect_TOF_USERDEFINED)
      aStyle = new Aspect_FontStyle(new Standard_CString(txtValue.getText()));
    else
      aStyle = new Aspect_FontStyle((short)cmbType.getSelectedIndex(), (double)txtSize.getValue(), (double)txtSlant.getValue(), false);
    myFontMap.AddEntry(aStyle);
    myDlg.setModified();
    updateEntries(-1);
  }

  public void apply()
  {
//     myViewer.SetFontMap(myFontMap.getAspectFontMap(), true);
    myViewer.SetFontMap(myFontMap, true);
  }

//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldEntries))
      updateEntryPane(sldEntries.getValue());
  }

//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Type")) onTypeChanged();
    else if (nameAction.equals("Edit")) onFontEdit();
    else if (nameAction.equals("Update")) updateCurrentEntry();
    else if (nameAction.equals("Add")) addNewEntry();
  }
}



//=======================================================================//
//                                                                       //
//                               TypeMapPane                             //
//                                                                       //
//=======================================================================//
class TypeMapPane extends JPanel implements ChangeListener,
                                            ActionListener
{
  private V2d_Viewer myViewer;
  private Aspect_TypeMap myTypeMap;
  private PropertiesDlg myDlg;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JLabel lblMapSize;
  TitledBorder brdEntries;
  JSlider sldEntries;
  JLabel lblStyle;
  JLabel lblValue;
  JComboBox cmbStyle;
  JTextField txtValue;
  JButton btnUpdate;


//=======================================================================//
// Construction
//=======================================================================//
  public TypeMapPane(V2d_Viewer viewer, PropertiesDlg parent)
  {
    myViewer = viewer;
//     myTypeMap = new Aspect_TypeMap(myViewer.TypeMap());
    myTypeMap = myViewer.TypeMap();
    myDlg = parent;

    try
    {
      jbInit();
      initValues();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    setLayout(new GridBagLayout());

    JPanel labelPanel = new JPanel();
    labelPanel.add(new JLabel("Size of the type map: "));
    lblMapSize = new JLabel();
    labelPanel.add(lblMapSize);
    add(labelPanel, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(5, 10, 5, 10), 0, 0));

    // Entries pane
    JPanel entriesPanel = new JPanel(new GridBagLayout());
    brdEntries = new TitledBorder(BorderFactory.createEtchedBorder(), "Entries:");
    entriesPanel.setBorder(brdEntries);

    sldEntries = new JSlider();
    sldEntries.setMinimum(0);
    sldEntries.setMinorTickSpacing(1);
    sldEntries.setPaintLabels(true);
    sldEntries.setPaintTicks(true);
    sldEntries.setPaintTrack(true);
    sldEntries.addChangeListener(this);
    entriesPanel.add(sldEntries, new GridBagConstraints(0, 0, 2, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Style: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblStyle = new JLabel();
    entriesPanel.add(lblStyle, new GridBagConstraints(1, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Value: "), new GridBagConstraints(0, 2, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblValue = new JLabel();
    entriesPanel.add(lblValue, new GridBagConstraints(1, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    add(entriesPanel, new GridBagConstraints(0, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(5, 5, 5, 5), 0, 0));

    // New entry pane
    JPanel entryPanel = new JPanel(new GridBagLayout());
    entryPanel.setBorder(new TitledBorder(BorderFactory.createEtchedBorder(),
                         "  New entry:  "));

    entryPanel.add(new JLabel("Style: "), new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    String[] items = {"SOLID", "DASH", "DOT", "DOTDASH", "USERDEFINED"};
    cmbStyle = new JComboBox(items);
    cmbStyle.setEditable(false);
    cmbStyle.addActionListener(this);
    cmbStyle.setActionCommand("Style");
    entryPanel.add(cmbStyle, new GridBagConstraints(1, 0, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entryPanel.add(new JLabel("Value: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    txtValue = new JTextField(20);
    entryPanel.add(txtValue, new GridBagConstraints(1, 1, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    btnUpdate = new JButton("Update current");
    btnUpdate.addActionListener(this);
    btnUpdate.setActionCommand("Update");
    entryPanel.add(btnUpdate, new GridBagConstraints(2, 0, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    JButton button = new JButton("Add");
    button.addActionListener(this);
    button.setActionCommand("Add");
    entryPanel.add(button, new GridBagConstraints(2, 1, 1, 1, 0.5, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    add(entryPanel, new GridBagConstraints(0, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
  }

  private void initValues()
  {
    updateEntries(0);
  }


//=======================================================================//
// Commands
//=======================================================================//
  public void updateEntries(int index)
  {
    int mapSize = myTypeMap.Size();
    lblMapSize.setText("" + mapSize);
    if (index == -1) index = mapSize - 1;

    sldEntries.setMaximum(mapSize - 1);
    sldEntries.setValue(index);

    if (sldEntries.getMaximum() <= 15)
    {
      sldEntries.setMajorTickSpacing(1);
      sldEntries.setSnapToTicks(true);
    }
    else if (sldEntries.getMaximum() <= 75)
    {
      sldEntries.setMajorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }
    else if (sldEntries.getMaximum() <= 150)
    {
      sldEntries.setMajorTickSpacing(10);
      sldEntries.setSnapToTicks(false);
    }
    else
    {
      sldEntries.setMajorTickSpacing(25);
      sldEntries.setMinorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }

    updateEntryPane(index);
  }

//=======================================================================//
  public void updateEntryPane(int index)
  {
    brdEntries.setTitle("  Entries:   " + index + "  ");
    repaint();

    Aspect_TypeMapEntry anEntry = myTypeMap.Entry(index + 1);
    if (anEntry.IsAllocated())
    {
      Aspect_LineStyle aStyle = anEntry.Type();

      switch (aStyle.Style())
      {
        case Aspect_TypeOfLine.Aspect_TOL_SOLID:
          lblStyle.setText("SOLID");
          break;
        case Aspect_TypeOfLine.Aspect_TOL_DASH:
          lblStyle.setText("DASH");
          break;
        case Aspect_TypeOfLine.Aspect_TOL_DOT:
          lblStyle.setText("DOT");
          break;
        case Aspect_TypeOfLine.Aspect_TOL_DOTDASH:
          lblStyle.setText("DOTDASH");
          break;
        case Aspect_TypeOfLine.Aspect_TOL_USERDEFINED:
          lblStyle.setText("USERDEFINED");
          break;
        default:
          lblStyle.setText("Unknown");
      }

      lblValue.setText(buildValuesString(aStyle.Values()));
      cmbStyle.setSelectedIndex(aStyle.Style());
      txtValue.setText(buildValuesString(aStyle.Values()));
    }
    else
    {
      lblStyle.setText("Not allocated");
      lblValue.setText("");
      cmbStyle.setSelectedIndex(4);
      txtValue.setText("");
    }
  }

//=======================================================================//
  public void onStyleChanged()
  {
    if (cmbStyle.getSelectedIndex() == Aspect_TypeOfLine.Aspect_TOL_USERDEFINED)
      txtValue.setEnabled(true);
    else
    {
      txtValue.setEnabled(false);

      Aspect_TypeMapEntry aTmpEntry = new Aspect_TypeMapEntry(99, new Aspect_LineStyle((short)cmbStyle.getSelectedIndex()));
      Aspect_LineStyle aStyle = aTmpEntry.Type();
      txtValue.setText(buildValuesString(aStyle.Values()));
    }
  }

//=======================================================================//
  public void updateCurrentEntry()
  {
    Aspect_TypeMapEntry anEntry = myTypeMap.Entry(sldEntries.getValue() + 1);
    
    if (cmbStyle.getSelectedIndex() == Aspect_TypeOfLine.Aspect_TOL_USERDEFINED)
    {
      TColQuantity_Array1OfLength aValues = extractValues(txtValue.getText());
      if (aValues != null)
      {
        Aspect_LineStyle aStyle = new Aspect_LineStyle(aValues);
        anEntry.SetType(aStyle);
      }
      else
      {
        JOptionPane.showMessageDialog(myDlg, "The string isn't valid",
            "Error", JOptionPane.ERROR_MESSAGE);
        return;
      }
    }
    else
    {
//       Aspect_LineStyle aStyle = new Aspect_LineStyle(cmbStyle.getSelectedIndex());
      Aspect_LineStyle aStyle = new Aspect_LineStyle((short)cmbStyle.getSelectedIndex());
      anEntry.SetType(aStyle);
    }
    myTypeMap.AddEntry(anEntry);
    myDlg.setModified();
    updateEntryPane(sldEntries.getValue());
  }

//=======================================================================//
  public void addNewEntry()
  {
    int newEntryIndex;
    if (cmbStyle.getSelectedIndex() == Aspect_TypeOfLine.Aspect_TOL_USERDEFINED)
    {
      TColQuantity_Array1OfLength aValues = extractValues(txtValue.getText());
      if (aValues != null)
      {
        Aspect_LineStyle aStyle = new Aspect_LineStyle(aValues);
        newEntryIndex = myTypeMap.AddEntry(aStyle);
      }
      else
      {
        JOptionPane.showMessageDialog(myDlg, "The string isn't valid",
            "Error", JOptionPane.ERROR_MESSAGE);
        return;
      }
    }
    else
    {
      Aspect_LineStyle aStyle = new Aspect_LineStyle((short)cmbStyle.getSelectedIndex());
      newEntryIndex = myTypeMap.AddEntry(aStyle);
    }
    myDlg.setModified();
    updateEntries(newEntryIndex);
  }

//=======================================================================//
  public void apply()
  {
//     myViewer.SetTypeMap(myTypeMap.getAspectTypeMap());
    myViewer.SetTypeMap(myTypeMap);
  }

//=======================================================================//
  private TColQuantity_Array1OfLength extractValues(String aValues)
  {
    if (aValues.equals(""))
      return null;
      
    Vector v = new Vector(5);
    int pos = 0;
    boolean isFinish = false;

    while (!isFinish)
    {
      int k = aValues.indexOf(";", pos);
      if (k == -1)
      {
        try {
          BigDecimal value = new BigDecimal(aValues.substring(pos).trim());
          v.add(value);
        }
        catch (Exception e) {
          return null;
        }
        isFinish = true;
      }
      else
      {
        try {
          BigDecimal value = new BigDecimal(aValues.substring(pos, k).trim());
          v.add(value);
        }
        catch (Exception e) {
          return null;
        }
        pos = k + 1;
      }
    }
    if (v.isEmpty())
      return null;

    int count = v.size();
    TColQuantity_Array1OfLength anArray = new TColQuantity_Array1OfLength(1, count);
    for (int i = 0; i < count; i++)
    {
      BigDecimal value = (BigDecimal) v.get(i);
      anArray.SetValue(i+1, value.doubleValue());
    }
    return anArray;
  }

//=======================================================================//
  private String buildValuesString(TColQuantity_Array1OfLength anArray)
  {
    String aValues = new String("");
    int count = anArray.Length();
    for (int i = 0; i < count; i++)
    {
      if (i > 0) aValues += "; ";
      aValues += myDlg.format(anArray.Value(i+1));
    }
    return aValues;
  }

//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldEntries))
      updateEntryPane(sldEntries.getValue());
  }

//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Style")) onStyleChanged();
    else if (nameAction.equals("Update")) updateCurrentEntry();
    else if (nameAction.equals("Add")) addNewEntry();
  }

}



//=======================================================================//
//                                                                       //
//                               MarkMapPane                             //
//                                                                       //
//=======================================================================//
class MarkMapPane extends JPanel implements ChangeListener
{
  private V2d_Viewer myViewer;
  private Aspect_MarkMap myMarkMap;
  private PropertiesDlg myDlg;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JLabel lblMapSize;
  TitledBorder brdEntries;
  JSlider sldEntries;
  JLabel lblStyle;
  JPanel paneXValues, paneYValues, paneSValues;


//=======================================================================//
// Construction
//=======================================================================//
  public MarkMapPane(V2d_Viewer viewer, PropertiesDlg parent)
  {
    myViewer = viewer;
//     myMarkMap = new Aspect_MarkMap(myViewer.MarkMap());
    myMarkMap = myViewer.MarkMap();
    myDlg = parent;

    try
    {
      jbInit();
      initValues();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    setLayout(new GridBagLayout());

    JPanel labelPanel = new JPanel();
    labelPanel.add(new JLabel("Size of the mark map: "));
    lblMapSize = new JLabel();
    labelPanel.add(lblMapSize);
    add(labelPanel, new GridBagConstraints(0, 0, 1, 1, 1.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(5, 10, 5, 10), 0, 0));

    // Entries pane
    JPanel entriesPanel = new JPanel(new GridBagLayout());
    brdEntries = new TitledBorder(BorderFactory.createEtchedBorder(), "Entries:");
    entriesPanel.setBorder(brdEntries);

    sldEntries = new JSlider();
    sldEntries.setMinimum(0);
    sldEntries.setMinorTickSpacing(1);
    sldEntries.setPaintLabels(true);
    sldEntries.setPaintTicks(true);
    sldEntries.setPaintTrack(true);
    sldEntries.addChangeListener(this);
    entriesPanel.add(sldEntries, new GridBagConstraints(0, 0, 2, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("Style: "), new GridBagConstraints(0, 1, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    lblStyle = new JLabel();
    entriesPanel.add(lblStyle, new GridBagConstraints(1, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("XValues: "), new GridBagConstraints(0, 2, 1, 1, 0.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    paneXValues = new JPanel() {
        public Dimension getPreferredSize() {
          Dimension size = super.getPreferredSize();
          return new Dimension(size.width, Math.min(size.height, 100));
          }
        public Dimension getMinimumSize() {
          Dimension size = super.getMinimumSize();
          return new Dimension(size.width, Math.min(size.height, 100));
          }
        };
    entriesPanel.add(paneXValues, new GridBagConstraints(1, 2, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("YValues: "), new GridBagConstraints(0, 3, 1, 1, 0.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    paneYValues = new JPanel() {
        public Dimension getPreferredSize() {
          Dimension size = super.getPreferredSize();
          return new Dimension(size.width, Math.min(size.height, 100));
          }
        public Dimension getMinimumSize() {
          Dimension size = super.getMinimumSize();
          return new Dimension(size.width, Math.min(size.height, 100));
          }
        };
    entriesPanel.add(paneYValues, new GridBagConstraints(1, 3, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));

    entriesPanel.add(new JLabel("SValues: "), new GridBagConstraints(0, 4, 1, 1, 0.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    paneSValues = new JPanel() {
        public Dimension getPreferredSize() {
          Dimension size = super.getPreferredSize();
          return new Dimension(size.width, Math.min(size.height, 100));
          }
        public Dimension getMinimumSize() {
          Dimension size = super.getMinimumSize();
          return new Dimension(size.width, Math.min(size.height, 100));
          }
        };
    entriesPanel.add(paneSValues, new GridBagConstraints(1, 4, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
    add(entriesPanel, new GridBagConstraints(0, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.NORTH, GridBagConstraints.HORIZONTAL,
            new Insets(5, 5, 5, 5), 0, 0));
  }

  private void initValues()
  {
    updateEntries(0);
  }


//=======================================================================//
// Commands
//=======================================================================//
  public void updateEntries(int index)
  {
    int mapSize = myMarkMap.Size();
    lblMapSize.setText("" + mapSize);
    if (index == -1) index = mapSize - 1;

    sldEntries.setMaximum(mapSize - 1);
    sldEntries.setValue(index);

    if (sldEntries.getMaximum() <= 15)
    {
      sldEntries.setMajorTickSpacing(1);
      sldEntries.setSnapToTicks(true);
    }
    else if (sldEntries.getMaximum() <= 75)
    {
      sldEntries.setMajorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }
    else if (sldEntries.getMaximum() <= 150)
    {
      sldEntries.setMajorTickSpacing(10);
      sldEntries.setSnapToTicks(false);
    }
    else
    {
      sldEntries.setMajorTickSpacing(25);
      sldEntries.setMinorTickSpacing(5);
      sldEntries.setSnapToTicks(false);
    }

    updateEntryPane(index);
  }

//=======================================================================//
  public void updateEntryPane(int index)
  {
    brdEntries.setTitle("  Entries:   " + index + "  ");
    repaint();

    Aspect_MarkMapEntry anEntry = myMarkMap.Entry(index + 1);
    if (anEntry.IsAllocated())
    {
      Aspect_MarkerStyle aStyle = anEntry.Style();

      switch (aStyle.Type())
      {
        case Aspect_TypeOfMarker.Aspect_TOM_POINT:
          lblStyle.setText("Aspect_TOM_POINT");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_PLUS:
          lblStyle.setText("Aspect_TOM_PLUS");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_STAR:
          lblStyle.setText("Aspect_TOM_STAR");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_O:
          lblStyle.setText("Aspect_TOM_O");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_X:
          lblStyle.setText("Aspect_TOM_X");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_O_POINT:
          lblStyle.setText("Aspect_TOM_O_POINT");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_O_PLUS:
          lblStyle.setText("Aspect_TOM_O_PLUS");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_O_STAR:
          lblStyle.setText("Aspect_TOM_O_STAR");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_O_X:
          lblStyle.setText("Aspect_TOM_O_X");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_BALL:
          lblStyle.setText("Aspect_TOM_BALL");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_RING1:
          lblStyle.setText("Aspect_TOM_RING1");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_RING2:
          lblStyle.setText("Aspect_TOM_RING2");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_RING3:
          lblStyle.setText("Aspect_TOM_RING3");
          break;
        case Aspect_TypeOfMarker.Aspect_TOM_USERDEFINED:
          lblStyle.setText("Aspect_TOM_USERDEFINED");
          break;
        default:
          lblStyle.setText("Unknown");
      }

      setValues(paneXValues, getStringValues(aStyle.XValues()));
      setValues(paneYValues, getStringValues(aStyle.YValues()));
      setValues(paneSValues, getStringValues(aStyle.SValues()));
    }
    else
    {
      lblStyle.setText("Not allocated");
      paneXValues.removeAll();
      paneYValues.removeAll();
      paneSValues.removeAll();
    }
  }

//=======================================================================//
  public void apply()
  {
  }

 //=======================================================================//
 private void setValues(JPanel pane, String[] aValues)
  {
    pane.removeAll();
    pane.setLayout(new ToolbarLayout(ToolbarLayout.LEFT, ToolbarLayout.TOP, 10, 0));
    for (int i = 0; i < aValues.length; i++)
      pane.add(new JLabel(aValues[i]));
  }

//=======================================================================//
  private String[] getStringValues(TShort_Array1OfShortReal anArray)
  {
    int count = anArray.Length();
    if (count == 0)
      return new String[1];

    String[] aValues = new String[count];
    for (int i = 0; i < count; i++)
      aValues[i] = myDlg.format(anArray.Value(i+1));
    return aValues;
  }

//=======================================================================//
  private String[] getStringValues(TColStd_Array1OfBoolean anArray)
  {
    int count = anArray.Length();
    String[] aValues = new String[count];
    for (int i = 0; i < count; i++)
      aValues[i] = anArray.Value(i+1) ? "True" : "False";
    return aValues;
  }


//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldEntries))
      updateEntryPane(sldEntries.getValue());
  }

}


//=======================================================================//
//                                                                       //
//                              ColorCubeDlg                             //
//                                                                       //
//=======================================================================//
class ColorCubeDlg extends JDialog implements ActionListener
{
  private IntegerField txtBasePixel = new IntegerField("0");
  private IntegerField txtRedMax = new IntegerField("7");
  private IntegerField txtRedMult = new IntegerField("1");
  private IntegerField txtGreenMax = new IntegerField("7");
  private IntegerField txtGreenMult = new IntegerField("8");
  private IntegerField txtBlueMax = new IntegerField("3");
  private IntegerField txtBlueMult = new IntegerField("64");
  private boolean isOK = false;

//=======================================================================//
// Construction
//=======================================================================//
  public ColorCubeDlg(PropertiesDlg parent)
  {
    super(parent, "New Color Cube", true);
    try
    {
      jbInit();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    getContentPane().setLayout(new BorderLayout(10, 10));

    JPanel infoPane = new JPanel(new GridLayout(0, 2, 10, 10));
    infoPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
    infoPane.add(new JLabel("base_pixel"));
    infoPane.add(txtBasePixel);
    infoPane.add(new JLabel("redmax"));
    infoPane.add(txtRedMax);
    infoPane.add(new JLabel("redmult"));
    infoPane.add(txtRedMult);
    infoPane.add(new JLabel("greenmax"));
    infoPane.add(txtGreenMax);
    infoPane.add(new JLabel("greenmult"));
    infoPane.add(txtGreenMult);
    infoPane.add(new JLabel("bluemax"));
    infoPane.add(txtBlueMax);
    infoPane.add(new JLabel("bluemult"));
    infoPane.add(txtBlueMult);
    getContentPane().add(infoPane, BorderLayout.CENTER);

    JPanel controlPane = new JPanel();
    controlPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
    JPanel pane = new JPanel(new GridLayout(0, 1, 10, 10));
    JButton button = new JButton("OK");
    button.addActionListener(this);
    button.setActionCommand("OK");
    pane.add(button);
    button = new JButton("Cancel");
    button.addActionListener(this);
    button.setActionCommand("Cancel");
    pane.add(button);
    controlPane.add(pane);
    getContentPane().add(controlPane, BorderLayout.EAST);
    pack();
  }

//=======================================================================//
  private void close(boolean b)
  {
    isOK = b;
    dispose();
  }

//=======================================================================//
  public boolean isOK()
  {
    return isOK;
  }

//=======================================================================//
  public Aspect_ColorCubeColorMap getColorMap()
  {
    if (isOK)
    {
      Aspect_ColorCubeColorMap aMap =
          new Aspect_ColorCubeColorMap(txtBasePixel.getValue(),
                                       txtRedMax.getValue(), txtRedMult.getValue(),
                                       txtGreenMax.getValue(), txtGreenMult.getValue(),
                                       txtBlueMax.getValue(), txtBlueMult.getValue());
      return aMap;
    }

    return null; 
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("OK")) close(true);
    else if (nameAction.equals("Cancel")) close(false);
  }

}



//=======================================================================//
//                                                                       //
//                              ColorRampDlg                             //
//                                                                       //
//=======================================================================//
class ColorRampDlg extends JDialog implements ActionListener,
                                              FocusListener
{
  private IntegerField txtBasePixel;
  private IntegerField txtDimension;
  private JComboBox cmbName;
  private RealField txtRed;
  private RealField txtGreen;
  private RealField txtBlue;
  private boolean isOK = false;
  private PropertiesDlg myDlg;

//=======================================================================//
// Construction
//=======================================================================//
  public ColorRampDlg(PropertiesDlg parent)
  {
    super(parent, "New Color Ramp", true);
    myDlg = parent;
    try
    {
      jbInit();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    getContentPane().setLayout(new BorderLayout(10, 10));

    JPanel infoPane = new JPanel(new GridLayout(0, 2, 10, 10));
    infoPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
    infoPane.add(new JLabel("base_pixel"));
    txtBasePixel = new IntegerField("0");
    infoPane.add(txtBasePixel);
    infoPane.add(new JLabel("dimension"));
    txtDimension = new IntegerField("16");
    infoPane.add(txtDimension);
    infoPane.add(new JLabel("Color Name"));

    String[] items = new String[517];
    for (int i = 0; i < 517; i++)
      items[i] = Quantity_Color.StringName((short)i).GetValue();
    cmbName = new JComboBox(items);
    cmbName.setEditable(false);
    cmbName.addActionListener(this);
    cmbName.setActionCommand("ColorName");
    infoPane.add(cmbName);
    getContentPane().add(infoPane, BorderLayout.CENTER);

    JPanel colorPane = new JPanel(new GridLayout(1, 0, 10, 10));
    colorPane.setBorder(BorderFactory.createEmptyBorder(0, 10, 10, 10));
    txtRed = new RealField(8);
    txtRed.addFocusListener(this);
    colorPane.add(txtRed);
    txtGreen = new RealField(8);
    txtGreen.addFocusListener(this);
    colorPane.add(txtGreen);
    txtBlue = new RealField(8);
    txtBlue.addFocusListener(this);
    colorPane.add(txtBlue);
    JButton button = new JButton("Edit...");
    button.addActionListener(this);
    button.setActionCommand("Edit");
    colorPane.add(button);
    cmbName.setSelectedIndex(Quantity_NameOfColor.Quantity_NOC_RED);
    getContentPane().add(colorPane, BorderLayout.SOUTH);

    JPanel controlPane = new JPanel();
    controlPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
    JPanel pane = new JPanel(new GridLayout(0, 1, 10, 10));
    button = new JButton("OK");
    button.addActionListener(this);
    button.setActionCommand("OK");
    pane.add(button);
    button = new JButton("Cancel");
    button.addActionListener(this);
    button.setActionCommand("Cancel");
    pane.add(button);
    controlPane.add(pane);
    getContentPane().add(controlPane, BorderLayout.EAST);
    pack();
  }

//=======================================================================//
  private void close(boolean b)
  {
    isOK = b;
    dispose();
  }

//=======================================================================//
  private void editColor()
  {
    Color newColor = JColorChooser.showDialog(this, "Select Color",
            new Color((float) txtRed.getValue(), (float) txtGreen.getValue(),
                      (float) txtBlue.getValue()));
    if (newColor != null)
    {
      double red = newColor.getRed()/255.;
      double green = newColor.getGreen()/255.;
      double blue = newColor.getBlue()/255.;
      txtRed.setText(myDlg.format(red));
      txtGreen.setText(myDlg.format(green));
      txtBlue.setText(myDlg.format(blue));
      cmbName.setSelectedIndex(Quantity_Color.Name(red, green, blue));
    }
  }

//=======================================================================//
  private void changeColor()
  {
    Quantity_Color aColor = new Quantity_Color((short)cmbName.getSelectedIndex());
    txtRed.setText(myDlg.format(aColor.Red()));
    txtGreen.setText(myDlg.format(aColor.Green()));
    txtBlue.setText(myDlg.format(aColor.Blue()));
  }

//=======================================================================//
  public boolean isOK()
  {
    return isOK;
  }

//=======================================================================//
  public Aspect_ColorRampColorMap getColorMap()
  {
    if (isOK)
    {
      Quantity_Color aColor =
          new Quantity_Color(txtRed.getValue(), txtGreen.getValue(), txtBlue.getValue(),
                             Quantity_TypeOfColor.Quantity_TOC_RGB);
      Aspect_ColorRampColorMap aMap =
          new Aspect_ColorRampColorMap(txtBasePixel.getValue(), txtDimension.getValue(),
                                       aColor);
      return aMap;
    }
    
    return null; 
  }


//=======================================================================//
// Focus listener interface
//=======================================================================//
  public void focusGained(FocusEvent event)
  {
  }

  public void focusLost(FocusEvent event)
  {
    RealField field = (RealField) event.getSource();
    if (field.getValue() < 0) field.setText("0");
    if (field.getValue() > 1) field.setText("1");
    cmbName.setSelectedIndex(Quantity_Color.Name(txtRed.getValue(),
                             txtGreen.getValue(), txtBlue.getValue()));
  }

//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("OK")) close(true);
    else if (nameAction.equals("Cancel")) close(false);
    else if (nameAction.equals("Edit")) editColor();
    else if (nameAction.equals("ColorName")) changeColor();
  }

}


