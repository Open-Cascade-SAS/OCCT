// Created on: 1993-06-17
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


#include <GeomToStep_MakeSurfaceOfLinearExtrusion.ixx>
#include <GeomToStep_MakeVector.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <StdFail_NotDone.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Vec.hxx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_Vector.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une surface_of_linear_extrusion de prostep a partir d' une
// SurfaceOfLinearExtrusion de Geom
//=============================================================================

GeomToStep_MakeSurfaceOfLinearExtrusion::GeomToStep_MakeSurfaceOfLinearExtrusion
  ( const Handle(Geom_SurfaceOfLinearExtrusion)& S )
	
{
  Handle(StepGeom_SurfaceOfLinearExtrusion) Surf;
  Handle(StepGeom_Curve) aSweptCurve;
  Handle(StepGeom_Vector) aExtrusionAxis;
  
  GeomToStep_MakeCurve MkCurve(S->BasisCurve());
  GeomToStep_MakeVector MkVector(gp_Vec(S->Direction()));

  aSweptCurve = MkCurve.Value();
  aExtrusionAxis = MkVector.Value();

  Surf = new StepGeom_SurfaceOfLinearExtrusion;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  Surf->Init(name, aSweptCurve, aExtrusionAxis);
  theSurfaceOfLinearExtrusion = Surf;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SurfaceOfLinearExtrusion) &
      GeomToStep_MakeSurfaceOfLinearExtrusion::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSurfaceOfLinearExtrusion;
}
