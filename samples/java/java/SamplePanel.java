
//Title:        OpenCASCADE Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      Matra Datavision
//Description:


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import util.*;

public class SamplePanel extends JPanel
{
  JPanel myViewPanel;
  TracePanel myTracePanel = new TracePanel();

//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public SamplePanel()
  {
    this(true);
  }

  public SamplePanel(boolean isTracePanel)
  {
    try
    {
      jbInit(isTracePanel);
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

  private void jbInit(boolean isTracePanel) throws Exception
  {
      /*
    setLayout(new BorderLayout());

    myViewPanel = createViewPanel();
    myViewPanel.setVisible(false);
    add(myViewPanel, BorderLayout.CENTER);
    add(createToolbar(), BorderLayout.NORTH);
    add(myTracePanel, BorderLayout.EAST);
      */

    setLayout(new GridBagLayout());
    setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));

    myViewPanel = createViewPanel();
    myViewPanel.setVisible(false);
    add(createToolbar(), new GridBagConstraints(0, 0, 2, 1, 0.0, 0.0,
            GridBagConstraints.NORTHWEST, GridBagConstraints.NONE,
            new Insets(0, 0, 0, 0), 0, 0));
    add(myViewPanel, new GridBagConstraints(0, 1, 1, 1, 6.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));
    if (isTracePanel)
    {
      add(myTracePanel, new GridBagConstraints(1, 1, 1, 1, 4.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));
    }
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    return new ViewPanel();
  }

//-----------------------------------------------------------------------//
  public Component createToolbar()
  {
    JToolBar tools = new JToolBar() {
      public Dimension getMinimumSize() {
        return new Dimension(700, super.getMinimumSize().height);
        }
      };
    tools.setBorder(BorderFactory.createEmptyBorder(3, 3, 3, 3));
    tools.setFloatable(false);

    return tools;
  }

//=======================================================================//
//                                Actions                                //
//=======================================================================//
  public void traceMessage(String text, String title)
  {
    myTracePanel.setTitle(title);
    myTracePanel.setText(text);
  }

//-----------------------------------------------------------------------//
  public void setVisible(boolean b)
  {
    super.setVisible(b);
    myViewPanel.setVisible(b);
    if (b) this.validate();
  }

//=======================================================================//
//                           Action Listener                             //
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
  }

}

