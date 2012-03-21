// Created on: 1993-07-05
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

// sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created (StepToGeom_MakeSurfaceOfLinearExtrusion(...) function)

#include <StepToGeom_MakeSurfaceOfLinearExtrusion.ixx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <StepToGeom_MakeVectorWithMagnitude.hxx>
#include <Geom_Curve.hxx>
#include <StepToGeom_MakeCurve.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

//=============================================================================
// Creation d' une SurfaceOfLinearExtrusion de Geom a partir d' une
// SurfaceOfLinearExtrusion de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeSurfaceOfLinearExtrusion::Convert (const Handle(StepGeom_SurfaceOfLinearExtrusion)& SS,
                                                                   Handle(Geom_SurfaceOfLinearExtrusion)& CS)
{
  Handle(Geom_Curve) C;
  if (StepToGeom_MakeCurve::Convert(SS->SweptCurve(),C))
  {
    // sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created
    Handle(Geom_VectorWithMagnitude) V;
    if (StepToGeom_MakeVectorWithMagnitude::Convert(SS->ExtrusionAxis(),V))
    {
      const gp_Dir D(V->Vec());
      CS = new Geom_SurfaceOfLinearExtrusion(C,D);
      return Standard_True;
    }
  }
  return Standard_False;
}
