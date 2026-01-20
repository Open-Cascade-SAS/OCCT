// Created on: 1994-06-17
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StepShape_FacetedBrepAndBrepWithVoids_HeaderFile
#define _StepShape_FacetedBrepAndBrepWithVoids_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_ManifoldSolidBrep.hxx>
#include <StepShape_OrientedClosedShell.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class StepShape_FacetedBrep;
class StepShape_BrepWithVoids;
class TCollection_HAsciiString;
class StepShape_ClosedShell;
class StepShape_OrientedClosedShell;

class StepShape_FacetedBrepAndBrepWithVoids : public StepShape_ManifoldSolidBrep
{

public:
  //! Returns a FacetedBrepAndBrepWithVoids
  Standard_EXPORT StepShape_FacetedBrepAndBrepWithVoids();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepShape_ClosedShell>&    aOuter,
                            const occ::handle<StepShape_FacetedBrep>&    aFacetedBrep,
                            const occ::handle<StepShape_BrepWithVoids>&  aBrepWithVoids);

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&               aName,
                            const occ::handle<StepShape_ClosedShell>&                  aOuter,
                            const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>>& aVoids);

  Standard_EXPORT void SetFacetedBrep(const occ::handle<StepShape_FacetedBrep>& aFacetedBrep);

  Standard_EXPORT occ::handle<StepShape_FacetedBrep> FacetedBrep() const;

  Standard_EXPORT void SetBrepWithVoids(const occ::handle<StepShape_BrepWithVoids>& aBrepWithVoids);

  Standard_EXPORT occ::handle<StepShape_BrepWithVoids> BrepWithVoids() const;

  Standard_EXPORT void SetVoids(const occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>>& aVoids);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedClosedShell>>> Voids() const;

  Standard_EXPORT occ::handle<StepShape_OrientedClosedShell> VoidsValue(
    const int num) const;

  Standard_EXPORT int NbVoids() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_FacetedBrepAndBrepWithVoids, StepShape_ManifoldSolidBrep)

private:
  occ::handle<StepShape_FacetedBrep>   facetedBrep;
  occ::handle<StepShape_BrepWithVoids> brepWithVoids;
};

#endif // _StepShape_FacetedBrepAndBrepWithVoids_HeaderFile
