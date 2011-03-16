
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

public class IntegerField extends JTextField
		                      implements InputMethodListener,
                                     KeyListener
{
  private boolean consume = false;
  private String str = new String("");

//=======================================================================//
// Construction
//=======================================================================//
  public IntegerField()
  {
    addInputMethodListener(this);
    addKeyListener(this);
  }

  public IntegerField(int columns)
  {
    super(columns);
    addInputMethodListener(this);
    addKeyListener(this);
  }

  public IntegerField(String text)
  {
    super(text);
    addInputMethodListener(this);
    addKeyListener(this);
  }

  public IntegerField(String text, int columns)
  {
    super(text, columns);
    addInputMethodListener(this);
    addKeyListener(this);
  }

//=======================================================================//
  /** Returns integer contents of this component */
  public int getValue()
  {
    Integer value;
    String newValue;

    newValue = getText();
    value = new Integer((newValue.equals("") || newValue.equals("-")) ?
                        "0" : newValue);
                        
    return value.intValue();
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

      if (aPos == -1) // Minus not present
      {
        if ((getCaretPosition()) != 0)
          consume = true;
      }
      else
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