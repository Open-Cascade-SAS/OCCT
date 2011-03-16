
//Title:        Geological editor
//Version:
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;


public class IntegerSpin extends SpinBox
{
  int myStep = 1;
  int myVal = 0;

  int minVal = 0;
  int maxVal = 0;

  boolean isWrong = false;  // true if in the strBuf was putted any string
  String strBuf;
//*********************************************************************
  public IntegerSpin()
  {
    super();
    txtFld.setText("" + myVal);
  }

  public IntegerSpin(int aStartVal, int aStep)
  {
    super();
    myVal = aStartVal;
    txtFld.setText("" + myVal);
    myStep = aStep;
  }

//*********************************************************************
  public int getStep()
  {
    return myStep;
  }

//*********************************************************************
  public void setStep(int aStep)
  {
    myStep = aStep;
  }

//*********************************************************************
  public int getValue()
  {
    return myVal;
  }

//*********************************************************************
  public void setValue(int aVal)
  {
    myVal = aVal;
    if ((maxVal - minVal) > 0)
    {
      if (myVal > maxVal) myVal = maxVal;
      if (myVal < minVal) myVal = minVal;
    }
    txtFld.setText("" + myVal);
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  public void setValue(String aStr)
  {
    if (aStr.equals("") || aStr.equals("-"))
      setValue(0);
    else
      setValue((new Integer(aStr)).intValue());
  }

//*********************************************************************
  protected void IncreaseValue()
  {
    myVal += myStep;
    if ((maxVal - minVal) > 0)
      if (myVal > maxVal) myVal = maxVal;
    txtFld.setText("" + myVal);
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  protected void DecreaseValue()
  {
    myVal -= myStep;
    if ((maxVal - minVal) > 0)
      if (myVal < minVal) myVal = minVal;
    txtFld.setText("" + myVal);
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  public void setMaxValue(int aMax)
  {
    maxVal = aMax;
  }

//*********************************************************************
  public void setMinValue(int aMin)
  {
    minVal = aMin;
  }

//*********************************************************************
//                   KeyListener
//*********************************************************************
  public void keyPressed(KeyEvent e)
  {
    int aKod = e.getKeyCode();

    if (aKod == e.VK_MINUS)
    {
      String aStr = txtFld.getText();
      int aPos = aStr.indexOf("-");

      if (aPos == -1) // Minus not present
      {
        if ((txtFld.getCaretPosition()) != 0)
          if (!isWrong)
          {
            isWrong = true;
            strBuf = txtFld.getText();
          }
      }
      else if (!isWrong)
      {
        isWrong = true;
        strBuf = txtFld.getText();
      }
    }
    else if (aKod == e.VK_UP)
    {
      setValue(txtFld.getText());
      IncreaseValue();
    }
    else if (aKod == e.VK_DOWN)
    {
      setValue(txtFld.getText());
      DecreaseValue();
    }
    else if (!e.isActionKey() && aKod != e.VK_BACK_SPACE)
    {
      if (!Character.isDigit(e.getKeyChar()))
        if (!isWrong)
        {
          isWrong = true;
          strBuf = txtFld.getText();
        }
    }
  }

//*********************************************************************
  public void keyReleased(KeyEvent e)
  {
    if (isWrong)
    {
      txtFld.setText(strBuf);
      strBuf = "";
      isWrong = false;
    }
  }

//*********************************************************************
//                   FocusListener
//*********************************************************************
  public void focusLost(FocusEvent e)
  {
    setValue(txtFld.getText());
//    txtFld.setCaretPosition(0);
  }
}