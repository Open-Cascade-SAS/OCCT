// Created on: 1993-07-02
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
