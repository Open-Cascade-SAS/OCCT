// Created on: 2015-07-13
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepDimTol_ProjectedZoneDefinition_HeaderFile
#define _StepDimTol_ProjectedZoneDefinition_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_ToleranceZoneDefinition.hxx>
#include <StepRepr_ShapeAspect.hxx>

//! Representation of STEP entity ProjectedZoneDefinition
class StepDimTol_ProjectedZoneDefinition : public StepDimTol_ToleranceZoneDefinition
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_ProjectedZoneDefinition();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepDimTol_ToleranceZone>&                               theZone,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>>& theBoundaries,
    const occ::handle<StepRepr_ShapeAspect>&                                   theProjectionEnd,
    const occ::handle<StepBasic_LengthMeasureWithUnit>&                        theProjectionLength);

  //! Returns field ProjectionEnd
  inline occ::handle<StepRepr_ShapeAspect> ProjectionEnd() const { return myProjectionEnd; }

  //! Set field ProjectionEnd
  inline void SetProjectionEnd(const occ::handle<StepRepr_ShapeAspect>& theProjectionEnd)
  {
    myProjectionEnd = theProjectionEnd;
  }

  //! Returns field ProjectionLength
  inline occ::handle<StepBasic_LengthMeasureWithUnit> ProjectionLength()
  {
    return myProjectionLength;
  }

  //! Set field ProjectionLength
  inline void SetProjectionLength(
    const occ::handle<StepBasic_LengthMeasureWithUnit>& theProjectionLength)
  {
    myProjectionLength = theProjectionLength;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_ProjectedZoneDefinition, StepDimTol_ToleranceZoneDefinition)

private:
  occ::handle<StepRepr_ShapeAspect>            myProjectionEnd;
  occ::handle<StepBasic_LengthMeasureWithUnit> myProjectionLength;
};
#endif // _StepDimTol_ProjectionZoneDefinition_HeaderFile
