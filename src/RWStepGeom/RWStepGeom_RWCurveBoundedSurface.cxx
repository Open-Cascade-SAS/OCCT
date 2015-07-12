// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepGeom_RWCurveBoundedSurface.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CurveBoundedSurface.hxx>
#include <StepGeom_HArray1OfSurfaceBoundary.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceBoundary.hxx>

//=======================================================================
//function : RWStepGeom_RWCurveBoundedSurface
//purpose  : 
//=======================================================================
RWStepGeom_RWCurveBoundedSurface::RWStepGeom_RWCurveBoundedSurface ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepGeom_RWCurveBoundedSurface::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                 const Standard_Integer num,
                                                 Handle(Interface_Check)& ach,
                                                 const Handle(StepGeom_CurveBoundedSurface) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"curve_bounded_surface") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of CurveBoundedSurface

  Handle(StepGeom_Surface) aBasisSurface;
  data->ReadEntity (num, 2, "basis_surface", ach, STANDARD_TYPE(StepGeom_Surface), aBasisSurface);

  Handle(StepGeom_HArray1OfSurfaceBoundary) aBoundaries;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "boundaries", ach, sub3) ) {
    Standard_Integer num2 = sub3;
    Standard_Integer nb0 = data->NbParams(num2);
    aBoundaries = new StepGeom_HArray1OfSurfaceBoundary (1, nb0);
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      StepGeom_SurfaceBoundary anIt0;
      data->ReadEntity (num2, i0, "boundaries", ach, anIt0);
      aBoundaries->SetValue(i0, anIt0);
    }
  }

  Standard_Boolean aImplicitOuter;
  data->ReadBoolean (num, 4, "implicit_outer", ach, aImplicitOuter);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aBasisSurface,
            aBoundaries,
            aImplicitOuter);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepGeom_RWCurveBoundedSurface::WriteStep (StepData_StepWriter& SW,
                                                  const Handle(StepGeom_CurveBoundedSurface) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of CurveBoundedSurface

  SW.Send (ent->BasisSurface());

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Boundaries()->Length(); i2++ ) {
    StepGeom_SurfaceBoundary Var0 = ent->Boundaries()->Value(i2);
    SW.Send (Var0.Value());
  }
  SW.CloseSub();

  SW.SendBoolean (ent->ImplicitOuter());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepGeom_RWCurveBoundedSurface::Share (const Handle(StepGeom_CurveBoundedSurface) &ent,
                                              Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of CurveBoundedSurface

  iter.AddItem (ent->BasisSurface());

  for (Standard_Integer i2=1; i2 <= ent->Boundaries()->Length(); i2++ ) {
    StepGeom_SurfaceBoundary Var0 = ent->Boundaries()->Value(i2);
    iter.AddItem (Var0.Value());
  }
}
