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

#ifndef _StepGeom_PointOnSurface_HeaderFile
#define _StepGeom_PointOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepGeom_Point.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;

class StepGeom_PointOnSurface;
DEFINE_STANDARD_HANDLE(StepGeom_PointOnSurface, StepGeom_Point)

class StepGeom_PointOnSurface : public StepGeom_Point
{

public:
  //! Returns a PointOnSurface
  Standard_EXPORT StepGeom_PointOnSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepGeom_Surface)&         aBasisSurface,
                            const Standard_Real                     aPointParameterU,
                            const Standard_Real                     aPointParameterV);

  Standard_EXPORT void SetBasisSurface(const Handle(StepGeom_Surface)& aBasisSurface);

  Standard_EXPORT Handle(StepGeom_Surface) BasisSurface() const;

  Standard_EXPORT void SetPointParameterU(const Standard_Real aPointParameterU);

  Standard_EXPORT Standard_Real PointParameterU() const;

  Standard_EXPORT void SetPointParameterV(const Standard_Real aPointParameterV);

  Standard_EXPORT Standard_Real PointParameterV() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_PointOnSurface, StepGeom_Point)

protected:
private:
  Handle(StepGeom_Surface) basisSurface;
  Standard_Real            pointParameterU;
  Standard_Real            pointParameterV;
};

#endif // _StepGeom_PointOnSurface_HeaderFile
