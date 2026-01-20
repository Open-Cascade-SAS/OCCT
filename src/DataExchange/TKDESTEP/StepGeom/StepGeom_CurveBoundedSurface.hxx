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

#ifndef _StepGeom_CurveBoundedSurface_HeaderFile
#define _StepGeom_CurveBoundedSurface_HeaderFile

#include <Standard.hxx>

#include <StepGeom_SurfaceBoundary.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_BoundedSurface.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;

//! Representation of STEP entity CurveBoundedSurface
class StepGeom_CurveBoundedSurface : public StepGeom_BoundedSurface
{

public:
  //! Empty constructor
  Standard_EXPORT StepGeom_CurveBoundedSurface();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aRepresentationItem_Name,
                            const occ::handle<StepGeom_Surface>&         aBasisSurface,
                            const occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>>& aBoundaries,
                            const bool                           aImplicitOuter);

  //! Returns field BasisSurface
  Standard_EXPORT occ::handle<StepGeom_Surface> BasisSurface() const;

  //! Set field BasisSurface
  Standard_EXPORT void SetBasisSurface(const occ::handle<StepGeom_Surface>& BasisSurface);

  //! Returns field Boundaries
  Standard_EXPORT occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>> Boundaries() const;

  //! Set field Boundaries
  Standard_EXPORT void SetBoundaries(const occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>>& Boundaries);

  //! Returns field ImplicitOuter
  Standard_EXPORT bool ImplicitOuter() const;

  //! Set field ImplicitOuter
  Standard_EXPORT void SetImplicitOuter(const bool ImplicitOuter);

  DEFINE_STANDARD_RTTIEXT(StepGeom_CurveBoundedSurface, StepGeom_BoundedSurface)

private:
  occ::handle<StepGeom_Surface>                  theBasisSurface;
  occ::handle<NCollection_HArray1<StepGeom_SurfaceBoundary>> theBoundaries;
  bool                          theImplicitOuter;
};

#endif // _StepGeom_CurveBoundedSurface_HeaderFile
