
//Title:        Jad version for OCC
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface team
//Company:      Matra Datavision
//Description:  Jad version with java classes 
//distribution according to OCC
//packaging


import javax.swing.*;

public class HeavyButton extends JButton
{
  public HeavyButton()
  {
    super(null, null);
  }

  public HeavyButton(Icon icon)
  {
    super(null, icon);
  }

  public HeavyButton(String text)
  {
    super(text, null);
  }

//   public HeavyButton(Action a)
//   {
//     super(a);
//   }

  public HeavyButton(String text, Icon icon)
  {
    super(text, icon);
  }

//=======================================================================//
  public void setToolTipText(String text)
  {
    putClientProperty(TOOL_TIP_TEXT_KEY, text);
    HeavyToolTipManager toolTipManager = HeavyToolTipManager.sharedInstance();
    if (text != null)
      toolTipManager.registerComponent(this);
    else
      toolTipManager.unregisterComponent(this);
  }
}
