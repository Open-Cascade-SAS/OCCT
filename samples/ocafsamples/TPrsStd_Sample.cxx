// Created on: 1999-12-27
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <V3d_Viewer.hxx>
#include <Quantity_NameOfColor.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>
#include <TPrsStd_PlaneDriver.hxx>
#include <TDataXtd_Plane.hxx>

// ====================================================================================
// This sample contains template for typical actions with OCAF visualization attributes
// ====================================================================================

#ifdef DEB
static void Sample()
{
  // Starting with data framework 
  Handle(TDF_Data) DF = new TDF_Data();
  TDF_Label aLabel = DF->Root();

  //----------------------------------- TPrsStd_AISViewer ----------------------------------------
  //==============================================================================================

  // Setting the TPrsStd_AISViewer in the framework

  Handle(V3d_Viewer) aViewer;

  //... Initialization of aViewer

  //Creating the new AIS_InteractiveContext 
  Handle(AIS_InteractiveContext) ctx = new AIS_InteractiveContext(aViewer);

  //Creating the new TPrsStd_AISViewer attribute initialized with AIS_InteractiveContext
  Handle(TPrsStd_AISViewer) aisviewer;

  if( !TPrsStd_AISViewer::Has(aLabel) ) { //Check if there has already been set TPrsStd_AISViewer in the framework
    aisviewer = TPrsStd_AISViewer::New(aLabel, ctx);
  }

  //Finding TPrsStd_AISViewer attribute in the DataFramework 
  if( TPrsStd_AISViewer::Find(aLabel, aisviewer) ) {
    aisviewer->Update(); //Update the viewer associated with this attribute
  }

  //Getting AIS_InteractiveContext from TPrsStd_AISViewer may be done in two ways: 

  //1. If we have already gotten TPrsStd_AISViewer attribute (strored in a variable <aisviewer>)
  Handle(AIS_InteractiveContext) context1 = aisviewer->GetInteractiveContext();
 
  //2. Getting AIS_InteractiveContext directly
  Handle(AIS_InteractiveContext) context2;
  if( TPrsStd_AISViewer::Find(aLabel, context2) ) {
    //do something...
  }

  //----------------------------------- TPrsStd_Driver and TPrsStd_DriverTable -------------------
  //==============================================================================================

  // All work for building AIS_InteractiveObject to be presented by TPrsStd_AISPresentation is done
  // by drivers which are descendants of deferred class TPrsStd_Driver

  // There is a map of drivers with Standard_GUID as a key. 

  // Adding driver to the map of drivers

  Handle(TPrsStd_NamedShapeDriver) NSDriver = new TPrsStd_NamedShapeDriver();
 
  Handle(TPrsStd_DriverTable) table  = TPrsStd_DriverTable::Get();
 
  Standard_GUID guid = TNaming_NamedShape::GetID();

  table->AddDriver(guid, NSDriver);  

  // When the first time called TPrsStd_DriverTable loads standard drivers defined in TPrsStd package

  // Getting driver from the map of drivers

  Standard_GUID driverguid = TNaming_NamedShape::GetID();
  
  Handle(TPrsStd_NamedShapeDriver) driver;

  if( table->FindDriver(driverguid,  driver) ) 
    cout << "Driver was found " << endl;
  else 
    cout << "Driver wasn't found" << endl;

  // Driver can be used to build AIS_InteractiveObject for presenting the given label

  Handle(TPrsStd_PlaneDriver) planedriver;

  if( table->FindDriver(TDataXtd_Plane::GetID(),  planedriver) ) {
    
    TDF_Label planelabel;

    // Finding planelabel ... 

    Handle(AIS_InteractiveObject) aisobject;

    planedriver->Update(planelabel, aisobject);

    if( !aisobject.IsNull() ) {

      // Do something with aisobject ...

    }
  } 

  //----------------------------------- TPrsStd_AISPresentation ----------------------------------
  //==============================================================================================


  TDF_Label ShapeLabel;
  
  // ... Setting TNaming_NamedShape to <ShapeLabel>

  // Setting the new  TPrsStd_AISPresentation to <ShapeLabel>
  // It can be done in two different ways:

  Handle(TPrsStd_AISPresentation) Presenation;
  //  1. By giving to TPrsStd_AISPresentation attribute Standard_GUID of an attribute  to be displayed:
  //  This GUID will be used to find driver for building AIS_InteractiveObject in the map of drivers
        
  Presenation = TPrsStd_AISPresentation::Set( ShapeLabel, TNaming_NamedShape::GetID() );
 
  //  2. Or by giving the attribute itself to TPrsStd_AISPresentation attribute:
  //  An ID of attribute  will be used to find driver for building AIS_InteractiveObject in the map of drivers

  Handle(TNaming_NamedShape) NS;
  if( ShapeLabel.FindAttribute( TNaming_NamedShape::GetID(), NS) ) {
    Presenation = TPrsStd_AISPresentation::Set( NS );
  }


  // Displaying (recomputation of presentation of attribute is done only if presentation is null)

  Handle(TPrsStd_AISPresentation) PRS;
  
  if( ShapeLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), PRS) ) PRS->Display();
  //After call of the method PRS->Display() the presentation of the attribute is marked as displayed in 
  //AIS_InteractiveContext  but not in viewer, in order to draw the object in viewer last has to be updated

  TPrsStd_AISViewer::Update(ShapeLabel);  //Update presentation of the attribute in a viewer's window

  // Erasing
  
  if( ShapeLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), PRS) ) PRS->Erase();
  // The method Erase() marks presentation of attribute as erased in AIS_InteractiveContext; 
  // in order to make changes visible in a viewer's window viewer has to be updated  
  TPrsStd_AISViewer::Update(ShapeLabel);  //Update viewer to erase presenation of the attribute in a viewer's window
                                          //Presentation of the attribute is erased from viewer but
                                          // stays in AIS_InteractiveContext
 
  if( ShapeLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), PRS) ) PRS->Erase(Standard_True); 
  TPrsStd_AISViewer::Update(ShapeLabel);
                                        //Presentation of the attribute is erased
                                        //from viewer and removed from AIS_InteractiveContext

  Handle(TPrsStd_AISPresentation) P;
  if( ShapeLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), P) ) {

    // Updating and displaying presentation of the attribute to be displayed 

    P->Display(Standard_True); 
    TPrsStd_AISViewer::Update(ShapeLabel);  //Update presenation of the attribute in a viewer's window

    //Getting Standard_GUID of attribute with which TPrsStd_AISPresentation attribute is associeted

    Standard_GUID guid = P->GetDriverGUID();

    //Setting a color to the displayd attribute

    P->SetColor(Quantity_NOC_RED);
    TPrsStd_AISViewer::Update(ShapeLabel);  //Update viewer to make changes visible to user

    //Getting transparency the displayd attribute

    Standard_Real transparency = P->Transparency();

    //Getting AIS_InteractiveObject built and stored in the AIS_Presentation attribute

    Handle(AIS_InteractiveObject) AISObject = P->GetAIS();
  }

  // ... Attribute is modified  


  //Updating presentation of the attribute in viewer

  if( ShapeLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), PRS) )
  PRS->Update(); //Updates presentation of attribute in AIS_InteractiveContext
  TPrsStd_AISViewer::Update(ShapeLabel); //Updates presentation in viewer

  return; 
}

#endif
