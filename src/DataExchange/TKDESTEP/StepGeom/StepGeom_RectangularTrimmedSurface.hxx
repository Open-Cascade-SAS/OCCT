// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepGeom_RectangularTrimmedSurface_HeaderFile
#define _StepGeom_RectangularTrimmedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_BoundedSurface.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;

class StepGeom_RectangularTrimmedSurface;
DEFINE_STANDARD_HANDLE(StepGeom_RectangularTrimmedSurface, StepGeom_BoundedSurface)

class StepGeom_RectangularTrimmedSurface : public StepGeom_BoundedSurface
{

public:
  //! Returns a RectangularTrimmedSurface
  Standard_EXPORT StepGeom_RectangularTrimmedSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepGeom_Surface)&         aBasisSurface,
                            const Standard_Real                     aU1,
                            const Standard_Real                     aU2,
                            const Standard_Real                     aV1,
                            const Standard_Real                     aV2,
                            const Standard_Boolean                  aUsense,
                            const Standard_Boolean                  aVsense);

  Standard_EXPORT void SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface);

  Standard_EXPORT Handle(StepGeom_Surface) BasisSurface() const;

  Standard_EXPORT void SetU1(const Standard_Real aU1);

  Standard_EXPORT Standard_Real U1() const;

  Standard_EXPORT void SetU2(const Standard_Real aU2);

  Standard_EXPORT Standard_Real U2() const;

  Standard_EXPORT void SetV1(const Standard_Real aV1);

  Standard_EXPORT Standard_Real V1() const;

  Standard_EXPORT void SetV2(const Standard_Real aV2);

  Standard_EXPORT Standard_Real V2() const;

  Standard_EXPORT void SetUsense(const Standard_Boolean aUsense);

  Standard_EXPORT Standard_Boolean Usense() const;

  Standard_EXPORT void SetVsense(const Standard_Boolean aVsense);

  Standard_EXPORT Standard_Boolean Vsense() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_RectangularTrimmedSurface, StepGeom_BoundedSurface)

protected:
private:
  Handle(StepGeom_Surface) basisSurface;
  Standard_Real            u1;
  Standard_Real            u2;
  Standard_Real            v1;
  Standard_Real            v2;
  Standard_Boolean         usense;
  Standard_Boolean         vsense;
};

#endif // _StepGeom_RectangularTrimmedSurface_HeaderFile
