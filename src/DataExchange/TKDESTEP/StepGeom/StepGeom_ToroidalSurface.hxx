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

#ifndef _StepGeom_ToroidalSurface_HeaderFile
#define _StepGeom_ToroidalSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_ElementarySurface.hxx>
class TCollection_HAsciiString;
class StepGeom_Axis2Placement3d;

class StepGeom_ToroidalSurface : public StepGeom_ElementarySurface
{

public:
  //! Returns a ToroidalSurface
  Standard_EXPORT StepGeom_ToroidalSurface();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&  aName,
                            const occ::handle<StepGeom_Axis2Placement3d>& aPosition,
                            const double                      aMajorRadius,
                            const double                      aMinorRadius);

  Standard_EXPORT void SetMajorRadius(const double aMajorRadius);

  Standard_EXPORT double MajorRadius() const;

  Standard_EXPORT void SetMinorRadius(const double aMinorRadius);

  Standard_EXPORT double MinorRadius() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_ToroidalSurface, StepGeom_ElementarySurface)

private:
  double majorRadius;
  double minorRadius;
};

#endif // _StepGeom_ToroidalSurface_HeaderFile
