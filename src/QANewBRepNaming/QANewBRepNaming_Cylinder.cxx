// Created on: 2000-10-31
// Created by: Vladislav ROMASHKO
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BRepPrimAPI_MakeCylinder.hxx>
#include <QANewBRepNaming_Cylinder.hxx>
#include <Standard_NullObject.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS_Solid.hxx>

#ifdef OCCT_DEBUG
#include <TDataStd_Name.hxx>
#endif

//=======================================================================
//function : QANewBRepNaming_Cylinder
//purpose  : 
//=======================================================================

QANewBRepNaming_Cylinder::QANewBRepNaming_Cylinder() {}

//=======================================================================
//function : QANewBRepNaming_Cylinder
//purpose  : 
//=======================================================================

QANewBRepNaming_Cylinder::QANewBRepNaming_Cylinder(const TDF_Label& ResultLabel):
       QANewBRepNaming_TopNaming(ResultLabel) {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_Cylinder::Init(const TDF_Label& ResultLabel) {
  if(ResultLabel.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_Cylinder::Init The Result label is Null ..."); 
  myResultLabel = ResultLabel;
}    

//=======================================================================
//function : Bottom
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Cylinder::Bottom() const {
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(ResultLabel().FindChild(1, Standard_True), "Bottom");
#endif
  return ResultLabel().FindChild(1, Standard_True);
}

//=======================================================================
//function : Top
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Cylinder::Top() const {
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(ResultLabel().FindChild(2, Standard_True), "Top");
#endif
  return ResultLabel().FindChild(2, Standard_True);
}

//=======================================================================
//function : Lateral
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Cylinder::Lateral() const {
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(ResultLabel().FindChild(3, Standard_True), "Lateral");
#endif
  return ResultLabel().FindChild(3, Standard_True);
}

//=======================================================================
//function : StartSide
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Cylinder::StartSide() const {
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(ResultLabel().FindChild(4, Standard_True), "StartSide");
#endif
  return ResultLabel().FindChild(4, Standard_True);
}

//=======================================================================
//function : EndSide
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Cylinder::EndSide() const {
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(ResultLabel().FindChild(5, Standard_True), "EndSide");
#endif
  return ResultLabel().FindChild(5, Standard_True);
}

//=======================================================================
//function : Load (Cylinder)
//purpose  : 
//=======================================================================

void QANewBRepNaming_Cylinder::Load (BRepPrimAPI_MakeCylinder& mkCylinder,
				const QANewBRepNaming_TypeOfPrimitive3D Type) const
{
  BRepPrim_Cylinder& S = mkCylinder.Cylinder();

  if (S.HasBottom()) {
    TopoDS_Face BottomFace = S.BottomFace();
    TNaming_Builder BottomFaceIns(Bottom()); 
    BottomFaceIns.Generated(BottomFace); 
  }

  if (S.HasTop()) {
    TopoDS_Face TopFace = S.TopFace();
    TNaming_Builder TopFaceIns(Top()); 
    TopFaceIns.Generated(TopFace); 
  }

  TopoDS_Face LateralFace = S.LateralFace();
  TNaming_Builder LateralFaceIns(Lateral()); 
  LateralFaceIns.Generated(LateralFace); 

  if (S.HasSides()) {
    TopoDS_Face StartFace = S.StartFace();
    TNaming_Builder StartFaceIns(StartSide()); 
    StartFaceIns.Generated(StartFace); 
    TopoDS_Face EndFace = S.EndFace();
    TNaming_Builder EndFaceIns(EndSide()); 
    EndFaceIns.Generated(EndFace); 
  }

  TNaming_Builder Builder (ResultLabel());
  if (Type == QANewBRepNaming_SOLID) Builder.Generated (mkCylinder.Solid());
  else if (Type == QANewBRepNaming_SHELL) Builder.Generated (mkCylinder.Shell());
  else {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Cylinder::Load(): Unexpected type of result"<<endl;
    Builder.Generated (mkCylinder.Shape());
#endif
  }
}

