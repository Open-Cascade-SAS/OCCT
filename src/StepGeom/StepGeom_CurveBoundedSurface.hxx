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

#include <StepGeom_HArray1OfSurfaceBoundary.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_BoundedSurface.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;


class StepGeom_CurveBoundedSurface;
DEFINE_STANDARD_HANDLE(StepGeom_CurveBoundedSurface, StepGeom_BoundedSurface)

//! Representation of STEP entity CurveBoundedSurface
class StepGeom_CurveBoundedSurface : public StepGeom_BoundedSurface
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepGeom_CurveBoundedSurface();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentationItem_Name, const Handle(StepGeom_Surface)& aBasisSurface, const Handle(StepGeom_HArray1OfSurfaceBoundary)& aBoundaries, const Standard_Boolean aImplicitOuter);
  
  //! Returns field BasisSurface
  Standard_EXPORT Handle(StepGeom_Surface) BasisSurface() const;
  
  //! Set field BasisSurface
  Standard_EXPORT void SetBasisSurface (const Handle(StepGeom_Surface)& BasisSurface);
  
  //! Returns field Boundaries
  Standard_EXPORT Handle(StepGeom_HArray1OfSurfaceBoundary) Boundaries() const;
  
  //! Set field Boundaries
  Standard_EXPORT void SetBoundaries (const Handle(StepGeom_HArray1OfSurfaceBoundary)& Boundaries);
  
  //! Returns field ImplicitOuter
  Standard_EXPORT Standard_Boolean ImplicitOuter() const;
  
  //! Set field ImplicitOuter
  Standard_EXPORT void SetImplicitOuter (const Standard_Boolean ImplicitOuter);




  DEFINE_STANDARD_RTTIEXT(StepGeom_CurveBoundedSurface,StepGeom_BoundedSurface)

protected:




private:


  Handle(StepGeom_Surface) theBasisSurface;
  Handle(StepGeom_HArray1OfSurfaceBoundary) theBoundaries;
  Standard_Boolean theImplicitOuter;


};







#endif // _StepGeom_CurveBoundedSurface_HeaderFile
