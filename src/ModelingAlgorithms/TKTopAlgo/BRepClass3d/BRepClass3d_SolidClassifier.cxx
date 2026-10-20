// Created on: 1994-03-30
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepClass3d_SolidClassifier.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>

BRepClass3d_SolidClassifier::BRepClass3d_SolidClassifier()
{
  aSolidLoaded = false;
}

void BRepClass3d_SolidClassifier::Load(const TopoDS_Shape& S)
{
  if (aSolidLoaded)
  {
    explorer.Destroy();
  }
  explorer.InitShape(S);
  aSolidLoaded = true;
}

BRepClass3d_SolidClassifier::BRepClass3d_SolidClassifier(const TopoDS_Shape& S)
    : aSolidLoaded(true),
      explorer(S)
{
}

BRepClass3d_SolidClassifier::BRepClass3d_SolidClassifier(const TopoDS_Shape& S,
                                                         const gp_Pnt&       P,
                                                         const double        Tol)
    : explorer(S)
{
  aSolidLoaded = true;
  Perform(P, Tol);
}

void BRepClass3d_SolidClassifier::Perform(const gp_Pnt& P, const double Tol)
{
  BRepClass3d_SClassifier::Perform(explorer, P, Tol);
}

void BRepClass3d_SolidClassifier::PerformInfinitePoint(const double Tol)
{
  if (aSolidLoaded)
  {
    BRepClass3d_SClassifier::PerformInfinitePoint(explorer, Tol);
  }
}

void BRepClass3d_SolidClassifier::Destroy()
{
  if (aSolidLoaded)
  {
    explorer.Destroy();
    aSolidLoaded = false;
  }
}
