// Created on: 2002-01-04
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepGeom_OrientedSurface_HeaderFile
#define _StepGeom_OrientedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepGeom_Surface.hxx>
class TCollection_HAsciiString;

class StepGeom_OrientedSurface;
DEFINE_STANDARD_HANDLE(StepGeom_OrientedSurface, StepGeom_Surface)

//! Representation of STEP entity OrientedSurface
class StepGeom_OrientedSurface : public StepGeom_Surface
{

public:
  //! Empty constructor
  Standard_EXPORT StepGeom_OrientedSurface();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aRepresentationItem_Name,
                            const Standard_Boolean                  aOrientation);

  //! Returns field Orientation
  Standard_EXPORT Standard_Boolean Orientation() const;

  //! Set field Orientation
  Standard_EXPORT void SetOrientation(const Standard_Boolean Orientation);

  DEFINE_STANDARD_RTTIEXT(StepGeom_OrientedSurface, StepGeom_Surface)

protected:
private:
  Standard_Boolean theOrientation;
};

#endif // _StepGeom_OrientedSurface_HeaderFile
