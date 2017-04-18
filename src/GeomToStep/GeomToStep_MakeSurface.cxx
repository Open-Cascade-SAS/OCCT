// Created on: 1993-06-22
// Created by: Martine LANGLOIS
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


#include <Geom_BoundedSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SweptSurface.hxx>
#include <GeomToStep_MakeBoundedSurface.hxx>
#include <GeomToStep_MakeElementarySurface.hxx>
#include <GeomToStep_MakeSurface.hxx>
#include <GeomToStep_MakeSweptSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Logical.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_ElementarySurface.hxx>
#include <StepGeom_OffsetSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SweptSurface.hxx>
#include <TCollection_HAsciiString.hxx>
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
#ifdef OCCT_DEBUG
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
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeSurface::Value() - no result");
  return theSurface;
}
