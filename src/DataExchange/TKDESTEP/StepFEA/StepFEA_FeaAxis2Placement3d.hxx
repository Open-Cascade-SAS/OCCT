// Created on: 2002-12-12
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

#ifndef _StepFEA_FeaAxis2Placement3d_HeaderFile
#define _StepFEA_FeaAxis2Placement3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_CoordinateSystemType.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <Standard_Boolean.hxx>
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;
class StepGeom_Direction;

//! Representation of STEP entity FeaAxis2Placement3d
class StepFEA_FeaAxis2Placement3d : public StepGeom_Axis2Placement3d
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_FeaAxis2Placement3d();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aRepresentationItem_Name,
                            const occ::handle<StepGeom_CartesianPoint>&  aPlacement_Location,
                            const bool                                   hasAxis2Placement3d_Axis,
                            const occ::handle<StepGeom_Direction>&       aAxis2Placement3d_Axis,
                            const bool                             hasAxis2Placement3d_RefDirection,
                            const occ::handle<StepGeom_Direction>& aAxis2Placement3d_RefDirection,
                            const StepFEA_CoordinateSystemType     aSystemType,
                            const occ::handle<TCollection_HAsciiString>& aDescription);

  //! Returns field SystemType
  Standard_EXPORT StepFEA_CoordinateSystemType SystemType() const;

  //! Set field SystemType
  Standard_EXPORT void SetSystemType(const StepFEA_CoordinateSystemType SystemType);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  DEFINE_STANDARD_RTTIEXT(StepFEA_FeaAxis2Placement3d, StepGeom_Axis2Placement3d)

private:
  StepFEA_CoordinateSystemType          theSystemType;
  occ::handle<TCollection_HAsciiString> theDescription;
};

#endif // _StepFEA_FeaAxis2Placement3d_HeaderFile
