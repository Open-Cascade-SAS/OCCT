// Created on: 2009-04-29
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <DNaming_BoxDriver.ixx>
#include <TFunction_Function.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_NamedShape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Solid.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Real.hxx>
#include <TDF_Label.hxx>
#include <ModelDefinitions.hxx>
#include <DNaming.hxx>
#include <TNaming_Builder.hxx>
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
void DNaming_BoxDriver::Validate(TFunction_Logbook& theLog) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_BoxDriver::MustExecute(const TFunction_Logbook& theLog) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
Standard_Integer DNaming_BoxDriver::Execute(TFunction_Logbook& theLog) const
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

  theLog.SetValid(RESPOSITION(aFunction), Standard_True);  

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
