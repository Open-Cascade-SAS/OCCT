
//Title:        ImportExport sample
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       
//Company:      Matra Datavision
//Description:  Your description

import java.awt.*;
import java.util.*;
import java.math.*;
import javax.swing.*;
import util.*;


public class TransparencyDlg extends StandardDlg
{
    private RealSpin spnValue = new RealSpin("0.0", "0.1");
    private double myValue = 0.0;
    private boolean isOK = false;

  //**********************************************************************
    public TransparencyDlg(Frame parent, double value)
    {
      super(parent, "Transparency", true, true, false, true);
      myValue = value;

      InitDlg();

      BigDecimal BD = new BigDecimal(myValue);
      BD = BD.setScale(2, BigDecimal.ROUND_HALF_UP);
      spnValue.setValue(BD.toString());

      pack();
    }

//**********************************************************************
    private void InitDlg()
    {
      JPanel aPane = new JPanel(new GridLayout(2, 1, 0, 4));

      aPane.add(new JLabel("Choose a value between 0 and 1"));

      spnValue.setMinValue("0.0");
      spnValue.setMaxValue("1.0");
      spnValue.setColumns(5);
      spnValue.setBorder(BorderFactory.createEmptyBorder(0, 15, 0, 15));
      aPane.add(spnValue);

      aPane.setBorder(BorderFactory.createEmptyBorder(5, 7, 5, 7));
      ControlsPanel.setLayout(new BorderLayout());
      ControlsPanel.add(aPane, BorderLayout.CENTER);
      pack();
      setResizable(false);
    }

//**********************************************************************
    public void OkAction()
    {
      myValue = spnValue.getValue();
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
    public double getValue()
    {
      return myValue;
    }

//**********************************************************************
    public boolean isOK()
    {
      return isOK;
    }


}
