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

#include <Standard_Type.hxx>
#include <StepShape_BrepWithVoids.hxx>
#include <StepShape_ClosedShell.hxx>
#include <StepShape_FacetedBrep.hxx>
#include <StepShape_FacetedBrepAndBrepWithVoids.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_FacetedBrepAndBrepWithVoids, StepShape_ManifoldSolidBrep)

StepShape_FacetedBrepAndBrepWithVoids::StepShape_FacetedBrepAndBrepWithVoids() = default;

void StepShape_FacetedBrepAndBrepWithVoids::Init(
  const occ::handle<TCollection_HAsciiString>& aName,
  const occ::handle<StepShape_ClosedShell>&    aOuter,
  const occ::handle<StepShape_FacetedBrep>&    aFacetedBrep,
  const occ::handle<StepShape_BrepWithVoids>&  aBrepWithVoids)
{
  // --- class own fields ---
  facetedBrep   = aFacetedBrep;
  brepWithVoids = aBrepWithVoids;
  // --- class inherited fields ---
  StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}

void StepShape_FacetedBrepAndBrepWithVoids::Init(
  const occ::handle<TCollection_HAsciiString>&                                        aName,
  const occ::handle<StepShape_ClosedShell>&                                           aOuter,
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>>& aVoids)
{
  // --- class inherited fields ---

  StepShape_ManifoldSolidBrep::Init(aName, aOuter);

  // --- ANDOR component fields ---

  brepWithVoids = new StepShape_BrepWithVoids();
  brepWithVoids->Init(aName, aOuter, aVoids);

  // --- ANDOR component fields ---

  facetedBrep = new StepShape_FacetedBrep();
  facetedBrep->Init(aName, aOuter);
}

void StepShape_FacetedBrepAndBrepWithVoids::SetFacetedBrep(
  const occ::handle<StepShape_FacetedBrep>& aFacetedBrep)
{
  facetedBrep = aFacetedBrep;
}

occ::handle<StepShape_FacetedBrep> StepShape_FacetedBrepAndBrepWithVoids::FacetedBrep() const
{
  return facetedBrep;
}

void StepShape_FacetedBrepAndBrepWithVoids::SetBrepWithVoids(
  const occ::handle<StepShape_BrepWithVoids>& aBrepWithVoids)
{
  brepWithVoids = aBrepWithVoids;
}

occ::handle<StepShape_BrepWithVoids> StepShape_FacetedBrepAndBrepWithVoids::BrepWithVoids() const
{
  return brepWithVoids;
}

//--- Specific Methods for AND class field access ---

void StepShape_FacetedBrepAndBrepWithVoids::SetVoids(
  const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>>& aVoids)
{
  brepWithVoids->SetVoids(aVoids);
}

occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>>
  StepShape_FacetedBrepAndBrepWithVoids::Voids() const
{
  return brepWithVoids->Voids();
}

occ::handle<StepShape_OrientedClosedShell> StepShape_FacetedBrepAndBrepWithVoids::VoidsValue(
  const int num) const
{
  return brepWithVoids->VoidsValue(num);
}

int StepShape_FacetedBrepAndBrepWithVoids::NbVoids() const
{
  return brepWithVoids->NbVoids();
}

//--- Specific Methods for AND class field access ---
