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
#include <StepShape_HArray1OfOrientedClosedShell.hxx>
#include <Standard_Integer.hxx>
class StepShape_FacetedBrep;
class StepShape_BrepWithVoids;
class TCollection_HAsciiString;
class StepShape_ClosedShell;
class StepShape_OrientedClosedShell;

class StepShape_FacetedBrepAndBrepWithVoids;
DEFINE_STANDARD_HANDLE(StepShape_FacetedBrepAndBrepWithVoids, StepShape_ManifoldSolidBrep)

class StepShape_FacetedBrepAndBrepWithVoids : public StepShape_ManifoldSolidBrep
{

public:
  //! Returns a FacetedBrepAndBrepWithVoids
  Standard_EXPORT StepShape_FacetedBrepAndBrepWithVoids();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_ClosedShell)&    aOuter,
                            const Handle(StepShape_FacetedBrep)&    aFacetedBrep,
                            const Handle(StepShape_BrepWithVoids)&  aBrepWithVoids);

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&               aName,
                            const Handle(StepShape_ClosedShell)&                  aOuter,
                            const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids);

  Standard_EXPORT void SetFacetedBrep(const Handle(StepShape_FacetedBrep)& aFacetedBrep);

  Standard_EXPORT Handle(StepShape_FacetedBrep) FacetedBrep() const;

  Standard_EXPORT void SetBrepWithVoids(const Handle(StepShape_BrepWithVoids)& aBrepWithVoids);

  Standard_EXPORT Handle(StepShape_BrepWithVoids) BrepWithVoids() const;

  Standard_EXPORT void SetVoids(const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids);

  Standard_EXPORT Handle(StepShape_HArray1OfOrientedClosedShell) Voids() const;

  Standard_EXPORT Handle(StepShape_OrientedClosedShell) VoidsValue(
    const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbVoids() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_FacetedBrepAndBrepWithVoids, StepShape_ManifoldSolidBrep)

protected:
private:
  Handle(StepShape_FacetedBrep)   facetedBrep;
  Handle(StepShape_BrepWithVoids) brepWithVoids;
};

#endif // _StepShape_FacetedBrepAndBrepWithVoids_HeaderFile
