// Created on: 1993-12-15
// Created by: Christophe MARION
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

#include <BRepExtrema_ExtPC.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>

//=================================================================================================

BRepExtrema_ExtPC::BRepExtrema_ExtPC(const TopoDS_Vertex& V, const TopoDS_Edge& E)
{
  Initialize(E);
  Perform(V);
}

//=================================================================================================

void BRepExtrema_ExtPC::Initialize(const TopoDS_Edge& E)
{
  if (!BRep_Tool::IsGeometric(E))
    return; // protect against non-geometric type (e.g. polygon)
  Standard_Real U1, U2;
  myHC              = new BRepAdaptor_Curve(E);
  Standard_Real Tol = Min(BRep_Tool::Tolerance(E), Precision::Confusion());
  Tol               = Max(myHC->Resolution(Tol), Precision::PConfusion());
  BRep_Tool::Range(E, U1, U2);
  myExtPC.Initialize(*myHC, U1, U2, Tol);
}

//=================================================================================================

void BRepExtrema_ExtPC::Perform(const TopoDS_Vertex& V)
{
  if (!myHC.IsNull())
  {
    gp_Pnt P = BRep_Tool::Pnt(V);
    myExtPC.Perform(P);
  }
}
