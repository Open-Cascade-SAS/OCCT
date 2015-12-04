// Created on: 2009-04-29
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <DNaming.hxx>
#include <DNaming_BoxDriver.hxx>
#include <ModelDefinitions.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_BoxDriver,TFunction_Driver)

//=======================================================================
//function : DNaming_BoxDriver
//purpose  : Constructor
//=======================================================================
DNaming_BoxDriver::DNaming_BoxDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <log>.
//=======================================================================
void DNaming_BoxDriver::Validate(Handle(TFunction_Logbook)&) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_BoxDriver::MustExecute(const Handle(TFunction_Logbook)&) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
Standard_Integer DNaming_BoxDriver::Execute(Handle(TFunction_Logbook)& theLog) const
{
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(),aFunction);
  if(aFunction.IsNull()) return -1;
  
  
  
// perform calculations

  Standard_Real aDX = DNaming::GetReal(aFunction,BOX_DX)->Get();
  Standard_Real aDY = DNaming::GetReal(aFunction,BOX_DY)->Get();
  Standard_Real aDZ = DNaming::GetReal(aFunction,BOX_DZ)->Get();

  Handle(TNaming_NamedShape) aPrevBox = DNaming::GetFunctionResult(aFunction);
// Save location
  TopLoc_Location aLocation;
  if (!aPrevBox.IsNull() && !aPrevBox->IsEmpty()) {
    aLocation = aPrevBox->Get().Location();
  }
  BRepPrimAPI_MakeBox aMakeBox(aDX, aDY, aDZ);  
  aMakeBox.Build();
  if (!aMakeBox.IsDone())
    {
      aFunction->SetFailure(ALGO_FAILED);
      return -1;
    }

  TopoDS_Shape aResult = aMakeBox.Solid();
  BRepCheck_Analyzer aCheck(aResult);
  if (!aCheck.IsValid (aResult))
    {
      aFunction->SetFailure(RESULT_NOT_VALID);
      return -1;
    }

    // Naming
  LoadNamingDS(RESPOSITION(aFunction),aMakeBox);

// restore location
  if(!aLocation.IsIdentity())
    TNaming::Displace(RESPOSITION(aFunction), aLocation, Standard_True);

  theLog->SetValid(RESPOSITION(aFunction), Standard_True);  

  aFunction->SetFailure(DONE);
  return 0;
}

//=======================================================================
//function : LoadAndName
//purpose  : 
//=======================================================================
void DNaming_BoxDriver::LoadNamingDS (const TDF_Label& theResultLabel, 
				      BRepPrimAPI_MakeBox& MS) const 
{
  TNaming_Builder Builder (theResultLabel);
  Builder.Generated (MS.Solid());

  //Load the faces of the box :
  TopoDS_Face BottomFace = MS.BottomFace ();
  TNaming_Builder BOF (theResultLabel.FindChild(1,Standard_True)); 
  BOF.Generated (BottomFace);
 
  TopoDS_Face TopFace = MS.TopFace ();
  TNaming_Builder TF (theResultLabel.FindChild(2,Standard_True)); 
  TF.Generated (TopFace); 

  TopoDS_Face FrontFace = MS.FrontFace ();
  TNaming_Builder FF (theResultLabel.FindChild(3,Standard_True)); 
  FF.Generated (FrontFace); 

  TopoDS_Face RightFace = MS.RightFace ();
  TNaming_Builder RF (theResultLabel.FindChild(4,Standard_True)); 
  RF.Generated (RightFace); 

  TopoDS_Face BackFace = MS.BackFace ();
  TNaming_Builder BF (theResultLabel.FindChild(5,Standard_True)); 
  BF.Generated (BackFace); 

  TopoDS_Face LeftFace = MS.LeftFace ();
  TNaming_Builder LF (theResultLabel.FindChild(6,Standard_True)); 
  LF.Generated (LeftFace); 
}
