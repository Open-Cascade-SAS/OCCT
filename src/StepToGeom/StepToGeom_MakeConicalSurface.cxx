// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StepToGeom_MakeConicalSurface.ixx>
#include <Geom_ConicalSurface.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <Precision.hxx>//#2(K3-3) rln
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une ConicalSurface de Geom a partir d' une ConicalSurface de
// Step
//=============================================================================

Standard_Boolean StepToGeom_MakeConicalSurface::Convert (const Handle(StepGeom_ConicalSurface)& SS, Handle(Geom_ConicalSurface)& CS)
{
  Handle(Geom_Axis2Placement) A;
  if (StepToGeom_MakeAxis2Placement::Convert(SS->Position(),A))
  {
    const Standard_Real R = SS->Radius() * UnitsMethods::LengthFactor();
    const Standard_Real Ang = SS->SemiAngle() * UnitsMethods::PlaneAngleFactor();
    //#2(K3-3) rln 12/02/98 ProSTEP ct_turbine-A.stp entity #518, #3571 (gp::Resolution() is too little)
    CS = new Geom_ConicalSurface(A->Ax2(), Max(Ang, Precision::Angular()), R);
    return Standard_True;
  }
  return Standard_False;
}
