
//Title:        Viewer3D Sample
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:  


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import util.*;
import CASCADESamplesJni.*;
import SampleAISDisplayModeJni.*;
// import SampleAISDisplayModeJni.SampleAISDisplayModePackage;
import jcas.Standard_Real;


public class AttributesPanel extends JPanel
                             implements ActionListener
{
  SamplePanel myDocument = null;
  AIS_InteractiveContext myAISContext = null;

//=======================================================================//
// Construction
//=======================================================================//
  public AttributesPanel(SamplePanel aDoc, AIS_InteractiveContext aContext)
  {
    myDocument = aDoc;
    myAISContext = aContext;

    try
    {
      jbInit();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  void jbInit() throws Exception
  {
    setLayout(new GridLayout(0, 1));
    setBounds(10, 10, 10, 10);

    JButton button;

    button = new JButton("WireFrame");
    button.addActionListener(this);
    button.setActionCommand("WireFrame");
    add(button);

    button = new JButton("Shading");
    button.addActionListener(this);
    button.setActionCommand("Shading");
    add(button);

    button = new JButton("Color...");
    button.addActionListener(this);
    button.setActionCommand("Color");
    add(button);

    button = new JButton("Brass");
    button.addActionListener(this);
    button.setActionCommand("Brass");
    add(button);

    button = new JButton("Bronze");
    button.addActionListener(this);
    button.setActionCommand("Bronze");
    add(button);

    button = new JButton("Copper");
    button.addActionListener(this);
    button.setActionCommand("Copper");
    add(button);

    button = new JButton("Gold");
    button.addActionListener(this);
    button.setActionCommand("Gold");
    add(button);

    button = new JButton("Pewter");
    button.addActionListener(this);
    button.setActionCommand("Pewter");
    add(button);

    button = new JButton("Plaster");
    button.addActionListener(this);
    button.setActionCommand("Plaster");
    add(button);

    button = new JButton("Plastic");
    button.addActionListener(this);
    button.setActionCommand("Plastic");
    add(button);

    button = new JButton("Silver");
    button.addActionListener(this);
    button.setActionCommand("Silver");
    add(button);

    button = new JButton("Steel");
    button.addActionListener(this);
    button.setActionCommand("Steel");
    add(button);

    button = new JButton("Stone");
    button.addActionListener(this);
    button.setActionCommand("Stone");
    add(button);

    button = new JButton("Shiny Plastic");
    button.addActionListener(this);
    button.setActionCommand("ShinyPlastic");
    add(button);

    button = new JButton("Satin");
    button.addActionListener(this);
    button.setActionCommand("Satin");
    add(button);

    button = new JButton("Transparency...");
    button.addActionListener(this);
    button.setActionCommand("Transparency");
    add(button);
  }


//=======================================================================//
// Commands
//=======================================================================//
  private void onWireFrame()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectDisplayMode(myAISContext, (short)0, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Wireframe");
  }

//=======================================================================//
  private void onShading()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectDisplayMode(myAISContext, (short)1, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Shading");
  }

//=======================================================================//
  private void onColor()
  {
    Quantity_Color aColor = SampleAISDisplayModePackage.GetObjectColor(myAISContext);
    int red = (int) (aColor.Red()*255);
    int green = (int) (aColor.Green()*255);
    int blue = (int) (aColor.Blue()*255);
    Color theColor = new Color(red, green, blue);
    
    Color theNewColor = JColorChooser.showDialog(SamplesStarter.getFrame(),
                                                 "Choose the color", theColor);

    if (theNewColor != null)
    {
      Quantity_Color aNewColor = new Quantity_Color(theNewColor.getRed()/255.,
						    theNewColor.getGreen()/255.,
						    theNewColor.getBlue()/255.,
						    Quantity_TypeOfColor.Quantity_TOC_RGB);
      TCollection_AsciiString message = new TCollection_AsciiString();
      SampleAISDisplayModePackage.SetObjectColor(myAISContext, aNewColor, message);

      myDocument.traceMessage(message.ToCString().GetValue(), "Setting Color");
    }
  }

//=======================================================================//
  private void onBrass()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_BRASS, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onBronze()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_BRONZE, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onCopper()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_COPPER, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onGold()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_GOLD, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onPewter()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_PEWTER, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onPlaster()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_PLASTER, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onPlastic()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_PLASTIC, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onSilver()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_SILVER, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onSteel()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_STEEL, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onStone()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_STONE, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onShinyPlastic()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_SHINY_PLASTIC, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onSatin()
  {
    TCollection_AsciiString message = new TCollection_AsciiString();
    SampleAISDisplayModePackage.SetObjectMaterial(myAISContext,
		      Graphic3d_NameOfMaterial.Graphic3d_NOM_SATIN, message);

    myDocument.traceMessage(message.ToCString().GetValue(), "Setting Material");
  }

//=======================================================================//
  private void onTransparency()
  {
    double aValue = SampleAISDisplayModePackage.GetObjectTransparency(myAISContext);

    TransparencyDlg aDlg = new TransparencyDlg(SamplesStarter.getFrame(), aValue);
    Position.centerWindow(aDlg);
    aDlg.show();

    if (aDlg.isOK())
    {
    	TCollection_AsciiString message = new TCollection_AsciiString();
      SampleAISDisplayModePackage.SetObjectTransparency(myAISContext, aDlg.getValue(), message);

      myDocument.traceMessage(message.ToCString().GetValue(), "Setting Transparency");
    }
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();

    if (myAISContext.NbCurrents() > 0)
    {
    	if (nameAction.equals("WireFrame")) onWireFrame();
    	else if (nameAction.equals("Shading")) onShading();
    	else if (nameAction.equals("Color")) onColor();
    	else if (nameAction.equals("Brass")) onBrass();
    	else if (nameAction.equals("Bronze")) onBronze();
    	else if (nameAction.equals("Copper")) onCopper();
    	else if (nameAction.equals("Gold")) onGold();
    	else if (nameAction.equals("Pewter")) onPewter();
    	else if (nameAction.equals("Plaster")) onPlaster();
    	else if (nameAction.equals("Plastic")) onPlastic();
    	else if (nameAction.equals("Silver")) onSilver();
    	else if (nameAction.equals("Steel")) onSteel();
    	else if (nameAction.equals("Stone")) onStone();
    	else if (nameAction.equals("ShinyPlastic")) onShinyPlastic();
    	else if (nameAction.equals("Satin")) onSatin();
    	else if (nameAction.equals("Transparency")) onTransparency();
    }
  }

}
