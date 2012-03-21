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


#include <GeomToStep_MakeSurface.ixx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_Surface.hxx>
#include <GeomToStep_MakeSurface.hxx>
#include <Geom_BoundedSurface.hxx>
#include <GeomToStep_MakeBoundedSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <GeomToStep_MakeElementarySurface.hxx>
#include <Geom_SweptSurface.hxx>
#include <GeomToStep_MakeSweptSurface.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Geom_OffsetSurface.hxx>
#include <StepGeom_OffsetSurface.hxx>
#include <StepData_Logical.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' une Surface de prostep a partir d' une Surface de Geom
//=============================================================================

GeomToStep_MakeSurface::GeomToStep_MakeSurface ( const Handle(Geom_Surface)& S)
{
  done = Standard_True;
  if (S->IsKind(STANDARD_TYPE(Geom_BoundedSurface))) {
    Handle(Geom_BoundedSurface) S1 = 
      Handle(Geom_BoundedSurface)::DownCast(S);
    GeomToStep_MakeBoundedSurface MkBoundedS(S1);
    theSurface = MkBoundedS.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface))) {
    Handle(Geom_ElementarySurface) S1 = 
      Handle(Geom_ElementarySurface)::DownCast(S);
    GeomToStep_MakeElementarySurface MkElementaryS(S1);
    theSurface = MkElementaryS.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_SweptSurface))) {
    Handle(Geom_SweptSurface) S1 = 
      Handle(Geom_SweptSurface)::DownCast(S);
    GeomToStep_MakeSweptSurface MkSwept(S1);
    theSurface = MkSwept.Value();
  }
  else if (S->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) S1 =
      Handle(Geom_OffsetSurface)::DownCast(S);
    GeomToStep_MakeSurface MkBasis(S1->BasisSurface());
    done = MkBasis.IsDone();
    if (!done) return;
    Handle(StepGeom_OffsetSurface) Surf = new StepGeom_OffsetSurface;
    Surf->Init (new TCollection_HAsciiString(""),
		     MkBasis.Value(),S1->Offset()/UnitsMethods::LengthFactor(),StepData_LFalse);
    theSurface = Surf;
  }
  else {
    done = Standard_False;
#ifdef DEB
    cout << " unknown type " << S->DynamicType()->Name() << endl;
#endif
  }
}


//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_Surface) &
      GeomToStep_MakeSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theSurface;
}
