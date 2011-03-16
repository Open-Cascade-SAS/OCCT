
//Title:        OpenCASCADE Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      Matra Datavision
//Description:


import java.awt.*;
import javax.swing.*;
import java.util.*;

public class AboutPanel extends JPanel
{
  static protected ResourceBundle resIcons =
         ResourceBundle.getBundle("properties.DesktopIcon");

  public AboutPanel()
  {
    try
    {
      jbInit();
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

  private void jbInit() throws Exception
  {
    setLayout(new GridBagLayout());

    ImageIcon imageIcon = new ImageIcon(resIcons.getString("AD_MATRA"));
    JLabel imageLabel = new JLabel(imageIcon);
    add(imageLabel, new GridBagConstraints(0, 0, 1, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(10, 10, 5, 10), 0, 0));

    add(new JLabel("Open CASCADE Technology Samples, Open CASCADE Technology 6.4"),
        new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 10, 5, 10), 0, 0));

    add(new JLabel("Copyright (C) 2001-2010, Open CASCADE S.A.S"),
        new GridBagConstraints(0, 2, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 10, 10, 10), 0, 0));

    add(new JLabel("http://www.opencascade.com"),
        new GridBagConstraints(0, 3, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 10, 10, 10), 0, 0));
  }

}

