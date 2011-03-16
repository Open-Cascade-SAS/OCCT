
//Title:        Viewer3D Sample
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:  


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import CASCADESamplesJni.*;
import SampleViewer3DJni.*;
import jcas.Standard_Real;


public class ZClippingDlg extends JDialog
                          implements ActionListener,
                                     InputMethodListener,
                                     ChangeListener,
                                     KeyListener
{
  private SamplePanel myDocument;
  private V3d_View myView;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JComboBox cmbType;
  JSlider sldDepth;
  JSlider sldWidth;
  JTextField txtDepth;
  JTextField txtWidth;

  private boolean consume = false;
  private String strDepth = new String("");
  private String strWidth = new String("");
  private boolean userDepthChanged = false;
  private boolean userWidthChanged = false;
  

//=======================================================================//
// Construction
//=======================================================================//
  public ZClippingDlg(Frame frame, SamplePanel aDoc, V3d_View aView)
  {
    super(frame, "ZClipping", false);
    myDocument = aDoc;
    myView = aView;

    try
    {
      jbInit();
      initValues();
      pack();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  void jbInit() throws Exception
  {
    getContentPane().setLayout(new BorderLayout());

    JPanel mainPanel = new JPanel(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createRaisedBevelBorder());

    JLabel lblDepth = new JLabel("Depth");
    mainPanel.add(lblDepth, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(10, 10, 5, 5), 0, 0));

    sldDepth = new JSlider(-1500, 1500, 0);
    sldDepth.addChangeListener(this);
    mainPanel.add(sldDepth, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(10, 5, 5, 5), 0, 0));

    txtDepth = new JTextField(6);
    txtDepth.addInputMethodListener(this);
    txtDepth.addKeyListener(this);
    mainPanel.add(txtDepth, new GridBagConstraints(2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(10, 5, 5, 10), 0, 0));

    JLabel lblWidth = new JLabel("Width");
    mainPanel.add(lblWidth, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(5, 10, 10, 5), 0, 0));

    sldWidth = new JSlider(0, 1500, 0);
    sldWidth.addChangeListener(this);
    mainPanel.add(sldWidth, new GridBagConstraints(1, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 5, 10, 5), 0, 0));

    txtWidth = new JTextField(6);
    txtWidth.addInputMethodListener(this);
    txtWidth.addKeyListener(this);
    mainPanel.add(txtWidth, new GridBagConstraints(2, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 5, 10, 10), 0, 0));

    JLabel lblType = new JLabel("Type");
    mainPanel.add(lblType, new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(10, 10, 10, 5), 0, 0));

    String[] items = {"OFF", "BACK", "FRONT", "SLICE"};
    cmbType = new JComboBox(items);
    cmbType.setSelectedIndex(0);
    cmbType.setEditable(false);
    cmbType.addActionListener(this);
    cmbType.setActionCommand("Type");
    mainPanel.add(cmbType, new GridBagConstraints(1, 2, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(10, 10, 10, 10), 0, 0));
    getContentPane().add(mainPanel, BorderLayout.CENTER);

    JPanel controlPanel = new JPanel();

    JButton btnOK = new JButton("OK");
    btnOK.addActionListener(this);
    btnOK.setActionCommand("OK");
    controlPanel.add(btnOK);

    JButton btnCancel = new JButton("Cancel");
    btnCancel.addActionListener(this);
    btnCancel.setActionCommand("Cancel");
    controlPanel.add(btnCancel);
    getContentPane().add(controlPanel, BorderLayout.SOUTH);
  }

  private void initValues()
  {
    Standard_Real depth = new Standard_Real();
    Standard_Real width = new Standard_Real();
    int type = myView.ZClipping(depth, width);

/*
    if (type == V3d_TypeOfZclipping.V3d_OFF)
      cmbType.setSelectedIndex(0);
    else if (type == V3d_TypeOfZclipping.V3d_BACK)
      cmbType.setSelectedIndex(1);
    else if (type == V3d_TypeOfZclipping.V3d_FRONT)
      cmbType.setSelectedIndex(2);
    else if (type == V3d_TypeOfZclipping.V3d_SLICE)
      cmbType.setSelectedIndex(3);
*/
    cmbType.setSelectedIndex(type);

    sldDepth.setValue((int) Math.round(depth.GetValue()));
    txtDepth.setText(String.valueOf(depth.GetValue()));

    sldWidth.setValue((int) Math.round(width.GetValue()));
    txtWidth.setText(String.valueOf(width.GetValue()));
  }

//=======================================================================//
// Commands
//=======================================================================//
  private void onDepthChanged()
  {
    String newValue = txtDepth.getText();
    Double value = new Double((newValue.equals("") || newValue.equals("-"))?
                              "0." : newValue);
    if (value.doubleValue() > 1500. || value.doubleValue() < -1500.)
      return;

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ChangeZClippingDepth(myView, value.doubleValue(), message);

    myDocument.traceMessage(message.ToCString().GetValue(), "SetZClippingDepth");
  }

//=======================================================================//
  private void onWidthChanged()
  {
    String newValue = txtWidth.getText();
    Double value = new Double((newValue.equals("") || newValue.equals("-"))?
                              "0." : newValue);
    if (value.doubleValue() > 1500. || value.doubleValue() <= 0.)
      return;

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ChangeZClippingWidth(myView, value.doubleValue(), message);

    myDocument.traceMessage(message.ToCString().GetValue(), "SetZClippingWidth");
  }

//=======================================================================//
  private void onTypeChanged()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ChangeZClippingType(myView, (short)cmbType.getSelectedIndex(), message);

    myDocument.traceMessage(message.ToCString().GetValue(), "SetZClippingType");
  }

//=======================================================================//
  private void close()
  {
    dispose();
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Type"))
      onTypeChanged();
    else if (nameAction.equals("OK"))
      close();
    else if (nameAction.equals("Cancel"))
      close();
  }


//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldDepth))
    {
      if (userDepthChanged)
      {
        txtDepth.setText(String.valueOf(slider.getValue()));
        onDepthChanged();
      }
      else
        userDepthChanged = true;
    }
    else if (slider.equals(sldWidth))
    {
      if (userWidthChanged)
      {
        txtWidth.setText(String.valueOf(slider.getValue()));
        onWidthChanged();
      }
      else
        userWidthChanged = true;
    }
  }

//=======================================================================//
// Key listener interface
//=======================================================================//
  public void keyTyped(KeyEvent event)
  {
  }

//=======================================================================//
  public void keyPressed(KeyEvent event)
  {
    JTextField field = (JTextField) event.getSource();

    int aKod = event.getKeyCode();
    if (aKod == event.VK_MINUS)
    {
      String aStr = field.getText();
      int aPos = aStr.indexOf("-");

      if (aPos == -1) // Minus not present
      {
        if ((field.getCaretPosition()) != 0)
          consume = true;
      }
      else
        consume = true;
    }
    else if ((aKod == event.VK_DECIMAL) || (aKod == event.VK_PERIOD))
    {
      String aStr = field.getText();
      int aPos = aStr.indexOf(".");
      if (aPos != -1) // the point is present in the string
        consume = true;
    }
    else if (!event.isActionKey() && aKod != event.VK_BACK_SPACE &&
                                     aKod != event.VK_DELETE)
    {
      if (!Character.isDigit(event.getKeyChar()))
        consume = true;
    }
  }

//=======================================================================//
  public void keyReleased(KeyEvent event)
  {
    JTextField field = (JTextField) event.getSource();
    String newValue = field.getText();
    Double value = new Double((newValue.equals("") || newValue.equals("-"))?
                               "0." : newValue);

    if (field.equals(txtDepth))
    {
      if (!newValue.equals(strDepth))
      {
        strDepth = newValue;
        if (value.doubleValue() < -1500. || value.doubleValue() > 1500.)
        {
          txtDepth.selectAll();
          JOptionPane.showMessageDialog(this, "Please enter a value between -1500 and 1500",
                                        "Warning!!!", JOptionPane.WARNING_MESSAGE);
          requestFocus();
        }
        else
        {
          userDepthChanged = false;
          sldDepth.setValue((int) Math.round(value.doubleValue()));
          onDepthChanged();
        }
      }
    }
    else if (field.equals(txtWidth))
    {
      if (!newValue.equals(strWidth))
      {
        strWidth = newValue;
        if (value.doubleValue() <= 0. || value.doubleValue() > 1500.)
        {
          txtWidth.selectAll();
          JOptionPane.showMessageDialog(this, "Please enter a value between 0 and 1500",
                                        "Warning!!!", JOptionPane.WARNING_MESSAGE);
          requestFocus();
        }
        else
        {
          userWidthChanged = false;
          sldWidth.setValue((int) Math.round(value.doubleValue()));
          onWidthChanged();
        }
      }
    }
  }

//=======================================================================//
// InputMethod listener interface
//=======================================================================//
  public void inputMethodTextChanged(InputMethodEvent event)
  {
    if (consume)
    {
      event.consume();
      consume = false;
    }
  }

  public void caretPositionChanged(InputMethodEvent event)
  {
  }
}
