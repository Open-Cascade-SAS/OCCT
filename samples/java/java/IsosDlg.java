
//Title:        AISDisplayMode sample
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       
//Company:      Matra Datavision
//Description:  

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.math.*;
import javax.swing.*;
import javax.swing.event.*;
import util.*;


public class IsosDlg extends StandardDlg
		                 implements InputMethodListener,
                                KeyListener
{
  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
    JTextField txtU;
    JTextField txtV;

    private boolean consume = false;
    private String strU = new String("");
    private String strV = new String("");

    private int myUValue = 1;
    private int myVValue = 1;
    private boolean isOK = false;

//=======================================================================//
// Construction
//=======================================================================//
    public IsosDlg(Frame parent, int u, int v)
    {
      super(parent, "Isos", true, true, false, true);
      myUValue = u;
      myVValue = v;

      InitDlg();

      strU = String.valueOf(myUValue);
      strV = String.valueOf(myVValue);
      txtU.setText(strU);
      txtV.setText(strV);

      pack();
    }

//-----------------------------------------------------------------------//
    private void InitDlg()
    {
      JPanel aPane = new JPanel(new GridLayout(2, 2, 4, 4));

      aPane.add(new JLabel("U Isos Number"));

      txtU = new JTextField(6);
      txtU.addInputMethodListener(this);
      txtU.addKeyListener(this);
      txtU.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
      aPane.add(txtU);

      aPane.add(new JLabel("V Isos Number"));

      txtV = new JTextField(6);
      txtV.addInputMethodListener(this);
      txtV.addKeyListener(this);
      txtV.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
      aPane.add(txtV);

      aPane.setBorder(BorderFactory.createEmptyBorder(5, 7, 5, 7));
      ControlsPanel.setLayout(new BorderLayout());
      ControlsPanel.add(aPane, BorderLayout.CENTER);
      pack();
      setResizable(false);
    }

//=======================================================================//
    public void OkAction()
    {
      Integer value;
      String newValue;

      newValue = txtU.getText();
      value = new Integer(newValue.equals("")? "0" : newValue);
      myUValue = value.intValue();

      newValue = txtV.getText();
      value = new Integer(newValue.equals("")? "0" : newValue);
      myVValue = value.intValue();

      isOK = true;
      dispose();
    }

//-----------------------------------------------------------------------//
    public void CancelAction()
    {
      isOK = false;
      dispose();
    }


//=======================================================================//
    public int getUValue()
    {
      return myUValue;
    }

//-----------------------------------------------------------------------//
    public int getVValue()
    {
      return myVValue;
    }

//-----------------------------------------------------------------------//
    public boolean isOK()
    {
      return isOK;
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
    if (!event.isActionKey() && aKod != event.VK_BACK_SPACE &&
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

    if (field.equals(txtU))
    {
      if (!newValue.equals(strU))
      {
      	if (consume)
      	{
      	  txtU.setText(strU);
      	  consume = false;
      	}
      	else
      	{
      	  strU = newValue;
      	  Integer value = new Integer(newValue.equals("")? "0" : newValue);
      	  if (!(value.intValue()>0))
      	  {
      	    txtU.selectAll();
            JOptionPane.showMessageDialog(this, "Please enter a positive value",
                                          "Warning!!!", JOptionPane.WARNING_MESSAGE);
      	    requestFocus();
      	  }
      	}
      }
    }
    else if (field.equals(txtV))
    {
      if (!newValue.equals(strV))
      {
      	if (consume)
      	{
      	  txtV.setText(strV);
      	  consume = false;
      	}
      	else
      	{
          strV = newValue;
      	  Integer value = new Integer(newValue.equals("")? "0" : newValue);
          if (!(value.intValue()>0))
          {
            txtV.selectAll();
            JOptionPane.showMessageDialog(this, "Please enter a positive value",
                                          "Warning!!!", JOptionPane.WARNING_MESSAGE);
      	    requestFocus();
      	  }
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
