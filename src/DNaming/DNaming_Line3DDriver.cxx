// Created on: 2010-02-26
// Created by: Sergey ZARITCHNY
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <DNaming.hxx>
#include <DNaming_Line3DDriver.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <ModelDefinitions.hxx>
#include <Precision.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_Label.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_Line3DDriver,TFunction_Driver)

//=======================================================================
//function : DNaming_Line3DDriver
//purpose  : Constructor
//=======================================================================
DNaming_Line3DDriver::DNaming_Line3DDriver()
{}

//=======================================================================
//function : Validate
//purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_Line3DDriver::Validate(Handle(TFunction_Logbook)&) const
{}

//=======================================================================
//function : MustExecute
//purpose  : Analyse in <theLog> if the loaded function must be executed
//=======================================================================
Standard_Boolean DNaming_Line3DDriver::MustExecute(const Handle(TFunction_Logbook)&) const
{
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  : Execute the function
//=======================================================================
Standard_Integer DNaming_Line3DDriver::Execute(Handle(TFunction_Logbook)& theLog) const
{
  Handle(TFunction_Function) aFunction;
  Label().FindAttribute(TFunction_Function::GetID(),aFunction);
  if(aFunction.IsNull()) return -1;

  aFunction->SetFailure(NOTDONE);  
  Handle(TNaming_NamedShape) aPrevILine3D = DNaming::GetFunctionResult(aFunction);
// Save location
  TopLoc_Location aLocation;
  if (!aPrevILine3D.IsNull() && !aPrevILine3D->IsEmpty()) {
    aLocation = aPrevILine3D->Get().Location();
  }

  const Standard_Integer aType = DNaming::GetInteger(aFunction, LINE3D_TYPE)->Get();
  Standard_Boolean isClosed = (aType != 0);
  Standard_Integer aCounter(0), aLength  = DNaming::GetInteger(aFunction, LINE3D_PNTNB)->Get();
  if(aLength < 2) {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  Handle(TNaming_NamedShape) aNS1, aNS2;
  BRepBuilderAPI_MakeWire aMakeWire;
  TopoDS_Wire aWire;
  TopoDS_Shape aShape1, aShape2;
  
  TopTools_Array1OfShape anArV(1,aLength); //aLength - number of points
  for(aCounter = 1; aCounter <= aLength-1; aCounter++) {       
    Handle(TDataStd_UAttribute) aRefP1 = DNaming::GetObjectArg(aFunction, (LINE3D_TYPE + aCounter));
    aNS1 = DNaming::GetObjectValue(aRefP1);
    Handle(TDataStd_UAttribute) aRefP2 = DNaming::GetObjectArg(aFunction, (LINE3D_TYPE + aCounter +1));
    aNS2 =  DNaming::GetObjectValue(aRefP2);
#ifdef OCCT_DEBUG
    if(!aNS1->IsEmpty()) {
      aShape1 = aNS1->Get();
      const gp_Pnt aDebPoint = BRep_Tool::Pnt(TopoDS::Vertex(aShape1));
      (void )aDebPoint;
      //	cout << aCounter << " X = " <<  aDebPoint.X() << " Y = " <<  aDebPoint.Y() << " Z = " <<  aDebPoint.Z() << endl;
    } else
      cout << " Line3DDriver:: NS1 is empty" << endl;
    if(!aNS2->IsEmpty()) {
      aShape2 = aNS2->Get();
      const gp_Pnt aDebPoint = BRep_Tool::Pnt(TopoDS::Vertex(aShape2));
      (void )aDebPoint;
      //      cout << aCounter+1 << " X = " <<  aDebPoint.X() << " Y = " <<  aDebPoint.Y() << " Z = " <<  aDebPoint.Z() << endl;
    } else
      cout << " Line3DDriver:: NS2 is empty" << endl;
#endif
    if(aNS1->IsEmpty() || aNS2->IsEmpty() || 
       aNS1->Get().IsNull() || aNS2->Get().IsNull()) {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    aShape1 = aNS1->Get();
    aShape2 = aNS2->Get();
#ifdef OCCT_DEBUG
    const gp_Pnt aDebPoint1 = BRep_Tool::Pnt(TopoDS::Vertex(aShape1));
    const gp_Pnt aDebPoint2 = BRep_Tool::Pnt(TopoDS::Vertex(aShape2));
    //      cout << aCounter   << " X = " <<  aDebPoint1.X() << " Y = " <<  aDebPoint1.Y() << " Z = " <<  aDebPoint1.Z() << endl;
    //      cout << aCounter+1 << " X = " <<  aDebPoint2.X() << " Y = " <<  aDebPoint2.Y() << " Z = " <<  aDebPoint2.Z() << endl;
    (void )aDebPoint1;
    (void )aDebPoint2;
#endif 
    if(aShape1.ShapeType() != TopAbs_VERTEX || aShape2.ShapeType() != TopAbs_VERTEX) {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    BRepBuilderAPI_MakeEdge aMakeEdge(TopoDS::Vertex(aShape1), TopoDS::Vertex(aShape2));
    if(aMakeEdge.IsDone()) {
      anArV.SetValue(aCounter,   aShape1);
      anArV.SetValue(aCounter+1, aShape2);
      aMakeWire.Add(aMakeEdge.Edge());
    }
  }
//  } else // closed
  if(isClosed) { 
    Handle(TDataStd_UAttribute) aRefP1 = DNaming::GetObjectArg(aFunction, (LINE3D_TYPE + 1));
    aNS1 = DNaming::GetObjectValue(aRefP1);
    aShape1 = aNS1->Get();
    BRepBuilderAPI_MakeEdge aMakeEdge(TopoDS::Vertex(aShape2), TopoDS::Vertex(aShape1));
    if(aMakeEdge.IsDone()) aMakeWire.Add(aMakeEdge.Edge());
  }
  if(aMakeWire.IsDone()) aWire = aMakeWire.Wire(); 
  

  if(aWire.IsNull()) {
    aFunction->SetFailure(ALGO_FAILED);
    return -1;
  }

  if (!BRepAlgo::IsValid(aWire)) {
    aFunction->SetFailure(RESULT_NOT_VALID);
    return -1;
  }

  TDF_Label aResultLabel = RESPOSITION(aFunction); //aFunction->GetResult()->Label();
  try {  
    LoadNamingDS(aResultLabel, aWire, anArV, isClosed);

  } catch (Standard_Failure) {
    aFunction->SetFailure(NAMING_FAILED);
    return -1;
  }

  // restore location
  if(!aLocation.IsIdentity())
    TNaming::Displace(aResultLabel, aLocation, Standard_True);

  theLog->SetValid(aResultLabel, Standard_True);  

  aFunction->SetFailure(DONE);
  return 0;
}

//=======================================================================
//function : LoadNamingDS
//purpose  : Loads a Line3D in a data framework
//=======================================================================
void DNaming_Line3DDriver::LoadNamingDS (const TDF_Label& theResultLabel, 
					 const TopoDS_Wire& theWire, 
					 const TopTools_Array1OfShape& theArV,
					 const Standard_Boolean isClosed) const
{
  if(theWire.IsNull()) return;
//Wire
  TNaming_Builder aWBuilder(theResultLabel);
  aWBuilder.Generated(theWire);
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(theResultLabel, "Line3DCurve");
#endif
  Standard_Integer aLength = theArV.Length();
  if(aLength < 2) return;
  TopoDS_Shape aShape;
  TopTools_Array1OfShape anArE(1,aLength);
  TopoDS_Vertex aFirst, aLast;
  for(Standard_Integer i = 1;i < aLength;i++) {
    gp_Pnt aP1  = BRep_Tool::Pnt(TopoDS::Vertex(theArV.Value(i)));
    gp_Pnt aP2  = BRep_Tool::Pnt(TopoDS::Vertex(theArV.Value(i+1)));
    Standard_Boolean aFound = Standard_False;
    TopExp_Explorer anExp(theWire,TopAbs_EDGE);
    for(;anExp.More();anExp.Next()) {
      const TopoDS_Edge& anE = TopoDS::Edge(anExp.Current());
      TopoDS_Vertex aV, aV1, aV2;
      TopExp::Vertices(anE, aV1, aV2);
      gp_Pnt aPE1 = BRep_Tool::Pnt(aV1);
      gp_Pnt aPE2 = BRep_Tool::Pnt(aV2);
      if(aP1.IsEqual(aPE1, Precision::Confusion()) && aP2.IsEqual(aPE2, Precision::Confusion())) {
        anArE.SetValue(i, anE);
        aFound = Standard_True;
        break;
      }
    }
    if(!aFound)
      anArE.SetValue(i, aShape);
    else aFound = Standard_False;
  }
  if(isClosed) {
    Standard_Boolean aFound = Standard_False;
    gp_Pnt aP1  = BRep_Tool::Pnt(TopoDS::Vertex(theArV.Value(aLength)));
    gp_Pnt aP2  = BRep_Tool::Pnt(TopoDS::Vertex(theArV.Value(1)));
    TopExp_Explorer anExp(theWire,TopAbs_EDGE);
    for(;anExp.More();anExp.Next()) {
      const TopoDS_Edge& anE = TopoDS::Edge(anExp.Current());
      TopoDS_Vertex aV, aV1, aV2;
      TopExp::Vertices(anE, aV1, aV2);
      gp_Pnt aPE1 = BRep_Tool::Pnt(aV1);
      gp_Pnt aPE2 = BRep_Tool::Pnt(aV2);
      if(aP1.IsEqual(aPE1, Precision::Confusion()) && aP2.IsEqual(aPE2, Precision::Confusion())) {
        anArE.SetValue(aLength, anE);
        aFound = Standard_True;
     // closed case
        aFirst = aV2;
        aLast  = aV1;
        break;
      }
    }
    if(!aFound)
      anArE.SetValue(aLength, aShape);
  } else {
    // open
    anArE.SetValue(aLength, aShape); // last edge
    TopExp::Vertices(theWire, aFirst, aLast);
  }

  // put edges
  // from 1:aLength - edges
  for(Standard_Integer i1 = 1;i1 <= aLength ;i1++) {
    TDF_Label aLab = theResultLabel.FindChild(i1);
    if(!anArE.Value(i1).IsNull()) {
      TNaming_Builder aBuilder(aLab);
      aBuilder.Generated(anArE.Value(i1));
    } else {
      Handle(TNaming_NamedShape) aNS;
      if(aLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
        TNaming_Builder aB(aLab);
    }
  }
  //put vertexes
  // aLength +1 - first vertex
  // aLength +2 - last vertex
  TDF_Label aLab1 = theResultLabel.FindChild(aLength+1);
  TDF_Label aLab2 = theResultLabel.FindChild(aLength+2);
  if(!aFirst.IsNull()) {
    TNaming_Builder aBuilder(aLab1);
    aBuilder.Generated(aFirst);
  } else {
    Handle(TNaming_NamedShape) aNS;
    if(aLab1.FindAttribute(TNaming_NamedShape::GetID(), aNS))
      TNaming_Builder aB(aLab1);
  }
  if(!aLast.IsNull()) {
    TNaming_Builder aBuilder(aLab2);
    aBuilder.Generated(aLast);
  } else {
    Handle(TNaming_NamedShape) aNS;
    if(aLab2.FindAttribute(TNaming_NamedShape::GetID(), aNS))
      TNaming_Builder aB(aLab2);
  }
}
