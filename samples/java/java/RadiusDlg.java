
//Title:        The AISSelect Sample
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       
//Company:      Matra Datavision
//Description:  

import java.awt.*;
import java.util.*;
import java.math.*;
import javax.swing.*;
import util.*;


public class RadiusDlg extends StandardDlg
{
    private RealSpin spnValue = new RealSpin("10.0", "1.0");
    private double myValue = 10.0;
    private boolean isOK = false;

  //**********************************************************************
    public RadiusDlg(Frame parent, double value)
    {
      super(parent, "Radius", true, true, false, true);
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

      aPane.add(new JLabel("Radius fillet"));

      spnValue.setMinValue("-10000.0");
      spnValue.setMaxValue("10000.0");
      spnValue.setColumns(6);
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
