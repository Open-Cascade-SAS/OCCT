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

#ifndef _StepGeom_CylindricalSurface_HeaderFile
#define _StepGeom_CylindricalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_ElementarySurface.hxx>
class TCollection_HAsciiString;
class StepGeom_Axis2Placement3d;

class StepGeom_CylindricalSurface;
DEFINE_STANDARD_HANDLE(StepGeom_CylindricalSurface, StepGeom_ElementarySurface)

class StepGeom_CylindricalSurface : public StepGeom_ElementarySurface
{

public:
  //! Returns a CylindricalSurface
  Standard_EXPORT StepGeom_CylindricalSurface();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&  aName,
                            const Handle(StepGeom_Axis2Placement3d)& aPosition,
                            const Standard_Real                      aRadius);

  Standard_EXPORT void SetRadius(const Standard_Real aRadius);

  Standard_EXPORT Standard_Real Radius() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CylindricalSurface, StepGeom_ElementarySurface)

protected:
private:
  Standard_Real radius;
};

#endif // _StepGeom_CylindricalSurface_HeaderFile
