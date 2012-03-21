// Created on: 2009-05-04
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




#include <DNaming_CylinderDriver.ixx>
#include <TFunction_Function.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_NamedShape.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Solid.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Real.hxx>
#include <TDF_Label.hxx>
#include <ModelDefinitions.hxx>

#include <DNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopoDS.hxx>
#include <gp_Lin.hxx>
#include <Geom_TrimmedCurve.hxx> 
#include <Geom_Line.hxx> 
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
void DNaming_CylinderDriver::Validate(TFunction_Logbook& theLog) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_CylinderDriver::MustExecute(const TFunction_Logbook& theLog) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
Standard_Integer DNaming_CylinderDriver::Execute(TFunction_Logbook& theLog) const
{
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(),aFunction);
  if(aFunction.IsNull()) return -1;



  Standard_Real aRadius = DNaming::GetReal(aFunction,CYL_RADIUS)->Get();
  Standard_Real aHeight = DNaming::GetReal(aFunction,CYL_HEIGHT)->Get();
  Handle(TDataStd_UAttribute) anObject = DNaming::GetObjectArg(aFunction,CYL_AXIS);
  Handle(TNaming_NamedShape) aNSAxis = DNaming::GetObjectValue(anObject);
  if (aNSAxis->IsEmpty()) {
#ifdef DEB
    cout<<"CylinderDriver:: Axis is empty"<<endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  TopoDS_Shape aTopoDSAxis = aNSAxis->Get();
  if (aTopoDSAxis.IsNull()) {
#ifdef DEB
    cout<<"CylinderDriver:: Axis is null"<<endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  // Creation of gp axis (gp_Ax2):
  if (aTopoDSAxis.ShapeType() != TopAbs_EDGE && aTopoDSAxis.ShapeType() != TopAbs_WIRE) {
#ifdef DEB
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
#ifdef DEB
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


  theLog.SetValid(RESPOSITION(aFunction), Standard_True);  
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
