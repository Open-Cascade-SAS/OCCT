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

#ifndef _StepShape_RevolvedAreaSolid_HeaderFile
#define _StepShape_RevolvedAreaSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_SweptAreaSolid.hxx>
class StepGeom_Axis1Placement;
class TCollection_HAsciiString;
class StepGeom_CurveBoundedSurface;

class StepShape_RevolvedAreaSolid : public StepShape_SweptAreaSolid
{

public:
  //! Returns a RevolvedAreaSolid
  Standard_EXPORT StepShape_RevolvedAreaSolid();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&     aName,
                            const occ::handle<StepGeom_CurveBoundedSurface>& aSweptArea,
                            const occ::handle<StepGeom_Axis1Placement>&      aAxis,
                            const double                                     aAngle);

  Standard_EXPORT void SetAxis(const occ::handle<StepGeom_Axis1Placement>& aAxis);

  Standard_EXPORT occ::handle<StepGeom_Axis1Placement> Axis() const;

  Standard_EXPORT void SetAngle(const double aAngle);

  Standard_EXPORT double Angle() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_RevolvedAreaSolid, StepShape_SweptAreaSolid)

private:
  occ::handle<StepGeom_Axis1Placement> axis;
  double                               angle;
};

#endif // _StepShape_RevolvedAreaSolid_HeaderFile
