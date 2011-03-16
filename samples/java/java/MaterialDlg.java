
//Title:        Open CASCADE Samples
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       Natalia Kopnova
//Company:      Matra Datavision (Nizhny Novgorod branch)
//Description:  


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import CASCADESamplesJni.*;

class MaterialDlg extends JDialog
                  implements ActionListener
{
  private AIS_InteractiveContext myAISContext;


//=======================================================================//
// Construction
//=======================================================================//
  public MaterialDlg(Frame frame, AIS_InteractiveContext aContext)
  {
    super(frame, "Material", false);
    myAISContext = aContext;

    try
    {
      jbInit();
      pack();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  void jbInit() throws Exception
  {
    getContentPane().setLayout(new GridLayout(0, 1));
    getContentPane().setBounds(10, 10, 10, 10);

    ButtonGroup group = new ButtonGroup();
    JToggleButton button;

    button = new JToggleButton("Brass", false);
    button.addActionListener(this);
    button.setActionCommand("Brass");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Bronze", false);
    button.addActionListener(this);
    button.setActionCommand("Bronze");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Copper", false);
    button.addActionListener(this);
    button.setActionCommand("Copper");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Gold", false);
    button.addActionListener(this);
    button.setActionCommand("Gold");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Pewter", false);
    button.addActionListener(this);
    button.setActionCommand("Pewter");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Plaster", false);
    button.addActionListener(this);
    button.setActionCommand("Plaster");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Plastic", false);
    button.addActionListener(this);
    button.setActionCommand("Plastic");
    group.add(button);
    getContentPane().add(button);

    button = new JToggleButton("Silver", false);
    button.addActionListener(this);
    button.setActionCommand("Silver");
    group.add(button);
    getContentPane().add(button);
  }

//=======================================================================//
// Commands
//=======================================================================//
  private void setMaterial(int material)
  {
    if (myAISContext != null)
    {
      for (myAISContext.InitCurrent(); myAISContext.MoreCurrent();
           myAISContext.NextCurrent())
        myAISContext.SetMaterial(myAISContext.Current(), (short)material, true);
    }
  }



//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Brass"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_BRASS);
    else if (nameAction.equals("Broze"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_BRONZE);
    else if (nameAction.equals("Copper"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_COPPER);
    else if (nameAction.equals("Gold"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_GOLD);
    else if (nameAction.equals("Pewter"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_PEWTER);
    else if (nameAction.equals("Plaster"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_PLASTER);
    else if (nameAction.equals("Plastic"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_PLASTIC);
    else if (nameAction.equals("Silver"))
      setMaterial(Graphic3d_NameOfMaterial.Graphic3d_NOM_SILVER);
  }

}

