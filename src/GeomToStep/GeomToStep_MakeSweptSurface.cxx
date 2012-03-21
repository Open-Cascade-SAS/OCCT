// Created on: 1993-06-22
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


#include <GeomToStep_MakeSweptSurface.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_SweptSurface.hxx>
#include <GeomToStep_MakeSweptSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomToStep_MakeSurfaceOfLinearExtrusion.hxx>
#include <GeomToStep_MakeSurfaceOfRevolution.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une SweptSurface de prostep a partir d' une 
// SweptSurface de Geom
//=============================================================================

GeomToStep_MakeSweptSurface::GeomToStep_MakeSweptSurface
  ( const Handle(Geom_SweptSurface)& S)
{
  done = Standard_True;
  if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) Sur = 
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S);
    GeomToStep_MakeSurfaceOfLinearExtrusion MkLinear(Sur);
    theSweptSurface = MkLinear.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) Sur = 
      Handle(Geom_SurfaceOfRevolution)::DownCast(S);
    GeomToStep_MakeSurfaceOfRevolution MkRevol(Sur);
    theSweptSurface = MkRevol.Value();
  }
  else
    done = Standard_False;
}	 


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_SweptSurface) &
      GeomToStep_MakeSweptSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSweptSurface;
}
