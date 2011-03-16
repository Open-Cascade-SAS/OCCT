
//Title:        Jad version for OCC
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface team
//Company:      Matra Datavision
//Description:  Jad version with java classes 
//distribution according to OCC
//packaging


import javax.swing.*;

public class HeavyToggleButton extends JToggleButton
{
  public HeavyToggleButton()
  {
    super(null, null);
  }

  public HeavyToggleButton(Icon icon, boolean theState)
  {
//     super(null, icon);
    super(icon, theState);
  }

  public HeavyToggleButton(String text)
  {
    super(text, null);
  }

//   public HeavyToggleButton(Action a)
//   {
//     super(a);
//   }

  public HeavyToggleButton(Icon icon, String text)
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
