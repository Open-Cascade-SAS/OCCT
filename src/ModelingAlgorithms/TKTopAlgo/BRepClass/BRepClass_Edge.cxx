// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <BRepClass_Edge.hxx>
#include <Precision.hxx>

//=================================================================================================

BRepClass_Edge::BRepClass_Edge()
    : myFirstParameter(0.0),
      myLastParameter(0.0),
      myMaxTolerance(Precision::Infinite()),
      myBoundingBoxState(BndBoxState::NotBuilt),
      myUseBndBox(false)
{
}

//=================================================================================================

void BRepClass_Edge::SetEdge(const TopoDS_Edge& theEdge)
{
  invalidateDerivedData();
  myEdge = theEdge;
}

//=================================================================================================

void BRepClass_Edge::SetFace(const TopoDS_Face& theFace)
{
  invalidateDerivedData();
  myFace = theFace;
}

//=================================================================================================

BRepClass_Edge::BRepClass_Edge(const TopoDS_Edge& E, const TopoDS_Face& F)
    : myEdge(E),
      myFace(F),
      myFirstParameter(0.0),
      myLastParameter(0.0),
      myMaxTolerance(Precision::Infinite()),
      myBoundingBoxState(BndBoxState::NotBuilt),
      myUseBndBox(false)
{
}

//=================================================================================================

void BRepClass_Edge::SetGeometry(const occ::handle<Geom2d_Curve>& theCurve,
                                 const double                     theFirst,
                                 const double                     theLast)
{
  myCurve          = theCurve;
  myFirstParameter = theFirst;
  myLastParameter  = theLast;
  myBoundingBox.SetVoid();
  myBoundingBoxState = BndBoxState::NotBuilt;
}

//=================================================================================================

void BRepClass_Edge::SetBoundingBox(const Bnd_Box2d& theBox)
{
  if (theBox.IsVoid())
  {
    SetBoundingBoxUnavailable();
    return;
  }
  myBoundingBox      = theBox;
  myBoundingBoxState = BndBoxState::Ready;
}

//=================================================================================================

void BRepClass_Edge::SetBoundingBoxUnavailable()
{
  myBoundingBox.SetVoid();
  myBoundingBoxState = BndBoxState::Unavailable;
}

//=================================================================================================

void BRepClass_Edge::invalidateDerivedData()
{
  myNextEdge.Nullify();
  myCurve.Nullify();
  myBoundingBox.SetVoid();
  myFirstParameter   = 0.0;
  myLastParameter    = 0.0;
  myBoundingBoxState = BndBoxState::NotBuilt;
}
