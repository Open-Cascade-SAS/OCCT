
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


public class DeviationDlg extends StandardDlg
{
  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JSlider sldDeviation;

  private int myValue = 1;
  private boolean isOK = false;

//=======================================================================//
// Construction
//=======================================================================//
  public DeviationDlg(Frame parent, int value)
  {
    super(parent, "Deviation Coefficient", true, true, false, true);
    myValue = value;

    InitDlg();

    sldDeviation.setValue(myValue);

    pack();
  }

//**********************************************************************
  private void InitDlg()
  {
    JPanel aPane = new JPanel(new GridBagLayout());

    sldDeviation = new JSlider(0, 100, 26);
    aPane.add(sldDeviation, new GridBagConstraints(0, 0, 3, 1, 0.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 0, 5, 0), 0, 0));

    aPane.add(new JLabel("Fine"), new GridBagConstraints(0, 1, 1, 1, 1.0, 1.0,
            GridBagConstraints.WEST, GridBagConstraints.NONE,
            new Insets(5, 5, 5, 5), 0, 0));

    aPane.add(new JLabel("Default"), new GridBagConstraints(1, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.NONE,
            new Insets(5, 5, 5, 5), 0, 0));

    aPane.add(new JLabel("Unpolished"), new GridBagConstraints(2, 1, 1, 1, 1.0, 0.0,
            GridBagConstraints.EAST, GridBagConstraints.NONE,
            new Insets(5, 5, 5, 5), 0, 0));

    aPane.setBorder(BorderFactory.createEmptyBorder(5, 7, 5, 7));
    ControlsPanel.setLayout(new BorderLayout());
    ControlsPanel.add(aPane, BorderLayout.CENTER);
    pack();
    setResizable(false);
  }

//**********************************************************************
  public void OkAction()
  {
    myValue = sldDeviation.getValue();

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

}
