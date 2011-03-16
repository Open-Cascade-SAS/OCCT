
//Title:        Viewer3D Sample
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:  


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import CASCADESamplesJni.*;
import SampleViewer3DJni.*;
import jcas.Standard_Real;


public class ShadingModelDlg extends JDialog
                             implements ActionListener
{
  private V3d_View myView;


//=======================================================================//
// Construction
//=======================================================================//
  public ShadingModelDlg(Frame frame, V3d_View aView)
  {
    super(frame, "ShadingModel", false);
    myView = aView;

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

    JButton button;

    button = new JButton("COLOR");
    button.addActionListener(this);
    button.setActionCommand("Color");
    getContentPane().add(button);

    button = new JButton("FLAT");
    button.addActionListener(this);
    button.setActionCommand("Flat");
    getContentPane().add(button);

    button = new JButton("GOURAUD");
    button.addActionListener(this);
    button.setActionCommand("Gouraud");
    getContentPane().add(button);
  }

//=======================================================================//
// Commands
//=======================================================================//
  private void onShadingModelColor()
  {
    SampleViewer3DPackage.ChangeShadingModel(myView, V3d_TypeOfShadingModel.V3d_COLOR);
  }

//=======================================================================//
  private void onShadingModelFlat()
  {
    SampleViewer3DPackage.ChangeShadingModel(myView, V3d_TypeOfShadingModel.V3d_FLAT);
  }

//=======================================================================//
  private void onShadingModelGouraud()
  {
    SampleViewer3DPackage.ChangeShadingModel(myView, V3d_TypeOfShadingModel.V3d_GOURAUD);
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();
    if (nameAction.equals("Color"))
      onShadingModelColor();
    else if (nameAction.equals("Flat"))
      onShadingModelFlat();
    else if (nameAction.equals("Gouraud"))
      onShadingModelGouraud();
  }

}
