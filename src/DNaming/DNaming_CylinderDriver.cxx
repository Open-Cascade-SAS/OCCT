// Created on: 2009-05-04
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


#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <DNaming.hxx>
#include <DNaming_CylinderDriver.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Lin.hxx>
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
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_CylinderDriver,TFunction_Driver)

//=======================================================================
//function : DNaming_CylinderDriver
//purpose  : Constructor
//=======================================================================
DNaming_CylinderDriver::DNaming_CylinderDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <log>.
//=======================================================================
void DNaming_CylinderDriver::Validate(Handle(TFunction_Logbook)&) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_CylinderDriver::MustExecute(const Handle(TFunction_Logbook)&) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
Standard_Integer DNaming_CylinderDriver::Execute(Handle(TFunction_Logbook)& theLog) const
{
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(),aFunction);
  if(aFunction.IsNull()) return -1;



  Standard_Real aRadius = DNaming::GetReal(aFunction,CYL_RADIUS)->Get();
  Standard_Real aHeight = DNaming::GetReal(aFunction,CYL_HEIGHT)->Get();
  Handle(TDataStd_UAttribute) anObject = DNaming::GetObjectArg(aFunction,CYL_AXIS);
  Handle(TNaming_NamedShape) aNSAxis = DNaming::GetObjectValue(anObject);
  if (aNSAxis->IsEmpty()) {
#ifdef OCCT_DEBUG
    cout<<"CylinderDriver:: Axis is empty"<<endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  TopoDS_Shape aTopoDSAxis = aNSAxis->Get();
  if (aTopoDSAxis.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"CylinderDriver:: Axis is null"<<endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  // Creation of gp axis (gp_Ax2):
  if (aTopoDSAxis.ShapeType() != TopAbs_EDGE && aTopoDSAxis.ShapeType() != TopAbs_WIRE) {
#ifdef OCCT_DEBUG
    cout<<"CylinderDriver:: Wrong axis, ShapeType = " << aTopoDSAxis.ShapeType() <<endl;
#endif    
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }

  gp_Ax2 anAxis;
  if (aTopoDSAxis.ShapeType() == TopAbs_WIRE) {
    TopExp_Explorer anExplorer(aTopoDSAxis, TopAbs_EDGE);
    aTopoDSAxis = anExplorer.Current();
  }

  BRepAdaptor_Curve aCurveAda(TopoDS::Edge(aTopoDSAxis));
  if (aCurveAda.GetType() == GeomAbs_Line) {
    gp_Lin aLin = aCurveAda.Line();
    anAxis = gp_Ax2(aLin.Location(), aLin.Direction());
    if(!aTopoDSAxis.Infinite()) {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Edge(aTopoDSAxis), V1, V2);
      gp_Pnt aP1 = BRep_Tool::Pnt(V1);
      anAxis.SetLocation(aP1);
    }
  } else {
#ifdef OCCT_DEBUG
    cout<<"CylinderDriver:: I don't support wires for a while"<<endl;
#endif    
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  
  Handle(TNaming_NamedShape) aPrevCyl = DNaming::GetFunctionResult(aFunction);
// Save location
  TopLoc_Location aLocation;
  if (!aPrevCyl.IsNull() && !aPrevCyl->IsEmpty()) {
    aLocation = aPrevCyl->Get().Location();
  }

  BRepPrimAPI_MakeCylinder aMakeCylinder(anAxis, aRadius, aHeight);  
  aMakeCylinder.Build();
  if (!aMakeCylinder.IsDone()) {
    aFunction->SetFailure(ALGO_FAILED);
    return -1;
  }

  TopoDS_Shape aResult = aMakeCylinder.Solid();
  BRepCheck_Analyzer aCheck(aResult);
  if (!aCheck.IsValid(aResult)) {
    aFunction->SetFailure(RESULT_NOT_VALID);
    return -1;
  }

  // Naming
  LoadNamingDS(RESPOSITION(aFunction), aMakeCylinder);

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
void DNaming_CylinderDriver::LoadNamingDS (const TDF_Label& theResultLabel, 
					   BRepPrimAPI_MakeCylinder& MS) const 
{
  TNaming_Builder Builder (theResultLabel);
  Builder.Generated (MS.Solid());

  BRepPrim_Cylinder& S = MS.Cylinder();

  //Load faces of the Cyl :
  if (S.HasBottom()) {
    TopoDS_Face BottomFace = S.BottomFace ();
    TNaming_Builder BOF (theResultLabel.FindChild(1,Standard_True)); 
    BOF.Generated (BottomFace);
  }

  if (S.HasTop()) { 
    TopoDS_Face TopFace = S.TopFace ();
    TNaming_Builder TOF (theResultLabel.FindChild(2,Standard_True)); 
    TOF.Generated (TopFace);
  }

  TopoDS_Face LateralFace = S.LateralFace();
  TNaming_Builder LOF (theResultLabel.FindChild(3,Standard_True)); 
  LOF.Generated(LateralFace); 

  if (S.HasSides()) {
    TopoDS_Face StartFace = S.StartFace();
    TNaming_Builder SF(theResultLabel.FindChild(4,Standard_True)); 
    SF.Generated(StartFace); 
    TopoDS_Face EndFace = S.EndFace();
    TNaming_Builder EF(theResultLabel.FindChild(5,Standard_True)); 
    EF.Generated(EndFace); 
  }

}
