
//Title:        Geological editor
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util;

import java.awt.*;
import java.math.BigDecimal;
import java.awt.event.*;
import javax.swing.*;

public class RealSpin extends SpinBox
{
  BigDecimal myStep = new BigDecimal("1");
  BigDecimal myVal = new BigDecimal("0");

  BigDecimal maxValue = new BigDecimal("0");
  BigDecimal minValue = new BigDecimal("0");

  boolean isWrong = false;  // true if in the strBuf was putted any string
  String strBuf;

//*********************************************************************
  public RealSpin()
  {
    super();
    txtFld.setText(myVal.toString());
  }

  public RealSpin(double aStartVal, double aStep, double aMin, double aMax)
  {
    this("" + aStartVal, "" + aStep, "" + aMin, "" + aMax);
  }

  public RealSpin(double aStartVal, double aStep)
  {
    this("" + aStartVal, "" + aStep);
  }

  public RealSpin(String aStartVal, String aStep)
  {
    this(aStartVal, aStep, "0.0", "0.0");
  }

  public RealSpin(String aStartVal, String aStep, String aMin, String aMax)
  {
    super();
    myVal = new BigDecimal(aStartVal);
    txtFld.setText(myVal.toString());

    myStep = new BigDecimal(aStep);
    maxValue = new BigDecimal(aMax);
    minValue = new BigDecimal(aMin);
  }

//*********************************************************************
  public double getStep()
  {
    return myStep.doubleValue();
  }

//*********************************************************************
  public void setStep(double aStep)
  {
    myStep = new BigDecimal(aStep);
  }

//*********************************************************************
  public double getValue()
  {
    return myVal.doubleValue();
  }

//*********************************************************************
  public void setValue(double aVal)
  {
    setValue("" + aVal);
  }

//*********************************************************************
  public void setValue(String aStr)
  {
    if (aStr.equals("") || aStr.equals("-"))
      myVal = new BigDecimal(0.0);
    else
    {
      myVal = new BigDecimal(aStr);
      if (maxValue.subtract(minValue).doubleValue() > 0)
      {
        if (myVal.doubleValue() > maxValue.doubleValue())
          myVal = new BigDecimal(maxValue.doubleValue());
        else if (myVal.doubleValue() < minValue.doubleValue())
          myVal = new BigDecimal(minValue.doubleValue());
      }
    }

    txtFld.setText("" + myVal.toString());
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  protected void IncreaseValue()
  {
    myVal = myVal.add(myStep);
    if (maxValue.subtract(minValue).doubleValue() > 0)
      if (myVal.doubleValue() > maxValue.doubleValue())
        myVal = new BigDecimal(maxValue.doubleValue());

    txtFld.setText("" + myVal.toString());
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  protected void DecreaseValue()
  {
    myVal = myVal.subtract(myStep);
    if (maxValue.subtract(minValue).doubleValue() > 0)
      if (myVal.doubleValue() < minValue.doubleValue())
        myVal = new BigDecimal(minValue.doubleValue());

    txtFld.setText("" + myVal.toString());
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  public void setMaxValue(String aMax)
  {
    maxValue = new BigDecimal(aMax);
  }

//*********************************************************************
  public void setMaxValue(double aMax)
  {
    setMaxValue("" + aMax);
  }

//*********************************************************************
  public void setMinValue(String aMin)
  {
    maxValue = new BigDecimal(aMin);
  }

//*********************************************************************
  public void setMinValue(double aMin)
  {
    setMinValue("" + aMin);
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
    else if ((aKod == e.VK_DECIMAL) || (aKod == e.VK_PERIOD))
    {
      String aStr = txtFld.getText();
      int aPos = aStr.indexOf(".");
      if ((aPos != -1) && (!isWrong)) // the point is present in the string
        if (!isWrong)
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
