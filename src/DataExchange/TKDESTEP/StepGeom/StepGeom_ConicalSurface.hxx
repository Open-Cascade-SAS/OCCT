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

#ifndef _StepGeom_ConicalSurface_HeaderFile
#define _StepGeom_ConicalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_ElementarySurface.hxx>
class TCollection_HAsciiString;
class StepGeom_Axis2Placement3d;

class StepGeom_ConicalSurface;
DEFINE_STANDARD_HANDLE(StepGeom_ConicalSurface, StepGeom_ElementarySurface)

class StepGeom_ConicalSurface : public StepGeom_ElementarySurface
{

public:
  //! Returns a ConicalSurface
  Standard_EXPORT StepGeom_ConicalSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&  aName,
                            const Handle(StepGeom_Axis2Placement3d)& aPosition,
                            const Standard_Real                      aRadius,
                            const Standard_Real                      aSemiAngle);

  Standard_EXPORT void SetRadius(const Standard_Real aRadius);

  Standard_EXPORT Standard_Real Radius() const;

  Standard_EXPORT void SetSemiAngle(const Standard_Real aSemiAngle);

  Standard_EXPORT Standard_Real SemiAngle() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_ConicalSurface, StepGeom_ElementarySurface)

protected:
private:
  Standard_Real radius;
  Standard_Real semiAngle;
};

#endif // _StepGeom_ConicalSurface_HeaderFile
