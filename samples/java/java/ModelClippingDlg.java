
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
import jcas.Standard_Boolean;


public class ModelClippingDlg extends JDialog
                              implements ActionListener,
                                         InputMethodListener,
                                         ChangeListener,
                                         KeyListener,
                                         ItemListener
{
  private SamplePanel myDocument;
  private V3d_View myView;

  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JCheckBox chkOnOff;
  JSlider sldZ;
  JTextField txtZ;

  private boolean consume = false;
  private String strZ = new String("");
  private boolean userZChanged = false;


//=======================================================================//
// Construction
//=======================================================================//
  public ModelClippingDlg(Frame frame, SamplePanel aDoc, V3d_View aView)
  {
    super(frame, "ModelClipping", false);
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

  private void jbInit() throws Exception
  {
    getContentPane().setLayout(new BorderLayout());

    JPanel mainPanel = new JPanel(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createRaisedBevelBorder());

    JLabel lblZ = new JLabel("Z");
    mainPanel.add(lblZ, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(10, 10, 5, 5), 0, 0));

    sldZ = new JSlider(-750, 750, 0);
    sldZ.addChangeListener(this);
    mainPanel.add(sldZ, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(10, 5, 5, 5), 0, 0));

    txtZ = new JTextField(6);
    txtZ.addInputMethodListener(this);
    txtZ.addKeyListener(this);
    mainPanel.add(txtZ, new GridBagConstraints(2, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(10, 5, 5, 10), 0, 0));

    chkOnOff = new JCheckBox("Model clipping ON/OFF", false);
    chkOnOff.addItemListener(this);
    mainPanel.add(chkOnOff, new GridBagConstraints(0, 1, 2, 1, 0.0, 0.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
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
    Standard_Real Z = new Standard_Real();
    Standard_Boolean isModelClippingOn = new Standard_Boolean();
    SampleViewer3DPackage.DisplayClippingPlane(SampleViewer3DPanel.getAISContext(), myView, 
					       Z, isModelClippingOn);

    sldZ.setValue((int) Math.round(Z.GetValue()));
    txtZ.setText(String.valueOf(Z.GetValue()));

    chkOnOff.setSelected(isModelClippingOn.GetValue());
  }

//=======================================================================//
// Commands
//=======================================================================//
  private void onModelClippingZChanged()
  {
    String newValue = txtZ.getText();
    Double value = new Double((newValue.equals("") || newValue.equals("-"))?
                        "0." : newValue);
    if (value.doubleValue() > 750. || value.doubleValue() < -750.)
      return;

    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ChangeModelClippingZ(SampleViewer3DPanel.getAISContext(), myView,
					       value.doubleValue(), chkOnOff.isSelected(),
					       message);

    myDocument.traceMessage(message.ToCString().GetValue(), "SetPlane");
  }

//=======================================================================//
  private void onModelClippingOnOff()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleViewer3DPackage.ChangeModelClippingOnOff(SampleViewer3DPanel.getAISContext(), myView,
						   chkOnOff.isSelected(), message);

    myDocument.traceMessage(message.ToCString().GetValue(), "SetPlaneOn/SetPlaneOff");
  }

//=======================================================================//
  private void close(boolean isOK)
  {
    if (!isOK)
      SampleViewer3DPackage.ClearClippingPlane(SampleViewer3DPanel.getAISContext(), myView,
					       chkOnOff.isSelected());
    else
      SampleViewer3DPackage.ClearClippingPlane(SampleViewer3DPanel.getAISContext(), myView, 
					       false);

    dispose();
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("OK"))
      close(true);
    else if (nameAction.equals("Cancel"))
      close(false);
  }


//=======================================================================//
// Item listener interface
//=======================================================================//
  public void itemStateChanged(ItemEvent event)
  {
    if (event.getSource().equals(chkOnOff))
      onModelClippingOnOff();
  }

//=======================================================================//
// Change listener interface
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    JSlider slider = (JSlider) event.getSource();
    if (slider.equals(sldZ))
    {
      if (userZChanged)
      {
        txtZ.setText(String.valueOf(slider.getValue()));
        onModelClippingZChanged();
      }
      else
        userZChanged = true;
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

    if (field.equals(txtZ))
    {
      if (!newValue.equals(strZ))
      {
        strZ = newValue;
        if (value.doubleValue() < -750. || value.doubleValue() > 750.)
        {
          txtZ.selectAll();
          JOptionPane.showMessageDialog(this, "Please enter a value between -750 and 750",
                                        "Warning!!!", JOptionPane.WARNING_MESSAGE);
          requestFocus();
        }
        else
        {
          userZChanged = false;
          sldZ.setValue((int) Math.round(value.doubleValue()));
          onModelClippingZChanged();
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
