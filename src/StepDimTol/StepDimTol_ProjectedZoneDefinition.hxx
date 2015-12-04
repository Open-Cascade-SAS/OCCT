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
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_ToleranceZoneDefinition.hxx>
#include <StepRepr_ShapeAspect.hxx>

class StepDimTol_ProjectedZoneDefinition;
DEFINE_STANDARD_HANDLE(StepDimTol_ProjectedZoneDefinition, StepDimTol_ToleranceZoneDefinition)
//! Representation of STEP entity ProjectedZoneDefinition
class StepDimTol_ProjectedZoneDefinition : public StepDimTol_ToleranceZoneDefinition
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_ProjectedZoneDefinition();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init (const Handle(StepDimTol_ToleranceZone)& theZone,
                               const Handle(StepRepr_HArray1OfShapeAspect)& theBoundaries,
                               const Handle(StepRepr_ShapeAspect)& theProjectionEnd,
                               const Handle(StepBasic_LengthMeasureWithUnit)& theProjectionLength);
  
  //! Returns field ProjectionEnd
  inline Handle(StepRepr_ShapeAspect) ProjectionEnd () const
  {
    return myProjectionEnd;
  }
  
  //! Set field ProjectionEnd
  inline void SetProjectionEnd (const Handle(StepRepr_ShapeAspect) &theProjectionEnd)
  {
    myProjectionEnd = theProjectionEnd;
  }
  
  //! Returns field ProjectionLength
  inline Handle(StepBasic_LengthMeasureWithUnit) ProjectionLength()
  {
    return myProjectionLength;
  }
  
  //! Set field ProjectionLength
  inline void SetProjectionLength(const Handle(StepBasic_LengthMeasureWithUnit)& theProjectionLength)
  {
    myProjectionLength = theProjectionLength;
  }
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_ProjectedZoneDefinition,StepDimTol_ToleranceZoneDefinition)

private: 
  Handle(StepRepr_ShapeAspect) myProjectionEnd;
  Handle(StepBasic_LengthMeasureWithUnit) myProjectionLength;
};
#endif // _StepDimTol_ProjectionZoneDefinition_HeaderFile
