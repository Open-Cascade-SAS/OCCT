
//Title:      Setup program
//Version:    
//Copyright:  Copyright (c) 1999
//Author:     User Interface group
//Company:    MatraDatavision
//Description:Sets graphics configuration.

import javax.swing.UIManager;
import java.awt.*;
import java.util.*;

public class setup
{
  boolean packFrame = false;

  //Construct the application
  public setup()
  {
    ResourceBundle AppRes = ResourceBundle.getBundle("properties.AppRes");
    Locale aLocale = new Locale(AppRes.getString("language"), "");

    Locale.setDefault(aLocale);

    SetupFrame frame = new SetupFrame();
    //Validate frames that have preset sizes
    //Pack frames that have useful preferred size info, e.g. from their layout
    if (packFrame)
      frame.pack();
    else
      frame.validate();
    //Center the window
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = frame.getSize();
    if (frameSize.height > screenSize.height)
      frameSize.height = screenSize.height;
    if (frameSize.width > screenSize.width)
      frameSize.width = screenSize.width;
    frame.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
    frame.setVisible(true);
  }

  //Main method
  public static void main(String[] args)
  {
    try 
    {
      UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
//      UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
    }
    catch(Exception e)
    {
    }
    new setup();
  }
}
 