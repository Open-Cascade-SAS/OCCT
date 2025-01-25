// Created on: 1993-07-13
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepAdaptor_Curve2d, Geom2dAdaptor_Curve)

//=================================================================================================

BRepAdaptor_Curve2d::BRepAdaptor_Curve2d() {}

//=================================================================================================

BRepAdaptor_Curve2d::BRepAdaptor_Curve2d(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  Initialize(E, F);
}

//=================================================================================================

Handle(Adaptor2d_Curve2d) BRepAdaptor_Curve2d::ShallowCopy() const
{
  Handle(BRepAdaptor_Curve2d) aCopy = new BRepAdaptor_Curve2d();

  aCopy->myCurve        = myCurve;
  aCopy->myTypeCurve    = myTypeCurve;
  aCopy->myFirst        = myFirst;
  aCopy->myLast         = myLast;
  aCopy->myBSplineCurve = myBSplineCurve;
  if (!myNestedEvaluator.IsNull())
  {
    aCopy->myNestedEvaluator = myNestedEvaluator->ShallowCopy();
  }

  return aCopy;
}

//=================================================================================================

void BRepAdaptor_Curve2d::Initialize(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  myEdge = E;
  myFace = F;
  Standard_Real              pf, pl;
  const Handle(Geom2d_Curve) PC = BRep_Tool::CurveOnSurface(E, F, pf, pl);
  Geom2dAdaptor_Curve::Load(PC, pf, pl);
}

//=================================================================================================

const TopoDS_Edge& BRepAdaptor_Curve2d::Edge() const
{
  return myEdge;
}

//=================================================================================================

const TopoDS_Face& BRepAdaptor_Curve2d::Face() const
{
  return myFace;
}
