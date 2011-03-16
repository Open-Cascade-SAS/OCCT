
//Title:        Open CASCADE Technology Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       Natalia Kopnova
//Company:      Matra Datavision (Nizhny Novgorod branch)
//Description:

package util;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

public class RealField extends JTextField
		                   implements InputMethodListener,
                                  KeyListener
{
  private boolean consume = false;
  private String str = new String("");

//=======================================================================//
// Construction
//=======================================================================//
  public RealField()
  {
    addInputMethodListener(this);
    addKeyListener(this);
  }

  public RealField(int columns)
  {
    super(columns);
    addInputMethodListener(this);
    addKeyListener(this);
  }

  public RealField(String text)
  {
    super(text);
    addInputMethodListener(this);
    addKeyListener(this);
  }

  public RealField(String text, int columns)
  {
    super(text, columns);
    addInputMethodListener(this);
    addKeyListener(this);
  }

//=======================================================================//
  /** Returns integer contents of this component */
  public double getValue()
  {
    Double value;
    String newValue;

    newValue = getText();
    value = new Double((newValue.equals("") || newValue.equals("-")) ?
                       "0." : newValue);
                       
    return value.doubleValue();
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
    int aKod = event.getKeyCode();
    if (aKod == event.VK_MINUS)
    {
      String aStr = getText();
      int aPos = aStr.indexOf("-");

      if (aPos == -1) // Minus is not present
      {
        if ((getCaretPosition()) != 0)
          consume = true;
      }
      else
        consume = true;
    }
    else if ((aKod == event.VK_DECIMAL) || (aKod == event.VK_PERIOD))
    {
      String aStr = getText();
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
    String newValue = getText();

    if (!newValue.equals(str))
    {
    	if (consume)
    	{
     	  setText(str);
     	  consume = false;
     	}
      else
        str = newValue;
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