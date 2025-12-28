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

#ifndef _StepShape_RightCircularCylinder_HeaderFile
#define _StepShape_RightCircularCylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Axis1Placement;
class TCollection_HAsciiString;

class StepShape_RightCircularCylinder : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a RightCircularCylinder
  Standard_EXPORT StepShape_RightCircularCylinder();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepGeom_Axis1Placement>&  aPosition,
                            const double                                 aHeight,
                            const double                                 aRadius);

  Standard_EXPORT void SetPosition(const occ::handle<StepGeom_Axis1Placement>& aPosition);

  Standard_EXPORT occ::handle<StepGeom_Axis1Placement> Position() const;

  Standard_EXPORT void SetHeight(const double aHeight);

  Standard_EXPORT double Height() const;

  Standard_EXPORT void SetRadius(const double aRadius);

  Standard_EXPORT double Radius() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_RightCircularCylinder, StepGeom_GeometricRepresentationItem)

private:
  occ::handle<StepGeom_Axis1Placement> position;
  double                               height;
  double                               radius;
};

#endif // _StepShape_RightCircularCylinder_HeaderFile
