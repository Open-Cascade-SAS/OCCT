
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


public class ThreadDlg extends StandardDlg
		                   implements InputMethodListener,
                                  KeyListener
{
    JTextField txtThread;

    private boolean consume = false;
    private String strThread = new String("");

    private int myValue = 1;
    private boolean isOK = false;

  //**********************************************************************
    public ThreadDlg(Frame parent, int value)
    {
      super(parent, "Change the animation thread", true, true, false, true);
      myValue = value;

      InitDlg();

      strThread = String.valueOf(myValue);
      txtThread.setText(strThread);

      pack();
    }

//**********************************************************************
    private void InitDlg()
    {
      JPanel aPane = new JPanel(new GridLayout(1, 2, 4, 4));

      aPane.add(new JLabel("Angle Increment"));

      txtThread = new JTextField(6);
      txtThread.addInputMethodListener(this);
      txtThread.addKeyListener(this);
      txtThread.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
      aPane.add(txtThread);

      aPane.setBorder(BorderFactory.createEmptyBorder(5, 7, 5, 7));
      ControlsPanel.setLayout(new BorderLayout());
      ControlsPanel.add(aPane, BorderLayout.CENTER);
      pack();
      setResizable(false);
    }

//**********************************************************************
    public void OkAction()
    {
      Integer value;
      String newValue;

      newValue = txtThread.getText();
      value = new Integer(newValue.equals("")? "0" : newValue);
      myValue = value.intValue();

      isOK = true;
      dispose();
    }

//**********************************************************************
    public void CancelAction()
    {
      isOK = false;
      dispose();
    }


//**********************************************************************
    public int getValue()
    {
      return myValue;
    }

//**********************************************************************
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

    if (field.equals(txtThread))
    {
      if (!newValue.equals(strThread))
      {
      	if (consume)
      	{
      	  txtThread.setText(strThread);
      	  consume = false;
      	}
      	else
      	{
      	  strThread = newValue;
      	  Integer value = new Integer(newValue.equals("")? "0" : newValue);
      	  if (value.intValue()<0)
      	  {
      	    txtThread.selectAll();
            JOptionPane.showMessageDialog(this, "Please enter a positive value",
                                          "Warning!!!", JOptionPane.WARNING_MESSAGE);
      	    requestFocus();
      	  }
          else if (value.intValue()>60)
          {
      	    txtThread.selectAll();
            JOptionPane.showMessageDialog(this, "Please enter an integer between 1 and 60",
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
