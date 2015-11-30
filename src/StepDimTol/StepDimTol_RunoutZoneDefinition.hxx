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

#ifndef _StepDimTol_RunoutZoneDefinition_HeaderFile
#define _StepDimTol_RunoutZoneDefinition_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepDimTol_RunoutZoneOrientation.hxx>
#include <StepDimTol_ToleranceZoneDefinition.hxx>
#include <Standard_Integer.hxx>

class StepRepr_HArray1OfShapeAspect;

class StepDimTol_RunoutZoneDefinition;
DEFINE_STANDARD_HANDLE(StepDimTol_RunoutZoneDefinition, StepDimTol_ToleranceZoneDefinition)

//! Representation of STEP entity ToleranceZoneDefinition
class StepDimTol_RunoutZoneDefinition : public StepDimTol_ToleranceZoneDefinition
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_RunoutZoneDefinition();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init (const Handle(StepDimTol_ToleranceZone)& theZone,
                               const Handle(StepRepr_HArray1OfShapeAspect)& theBoundaries,
                               const Handle(StepDimTol_RunoutZoneOrientation)& theOrientation);
  
  //! Returns field Orientation
  inline Handle(StepDimTol_RunoutZoneOrientation) Orientation () const
  {
    return myOrientation;
  }
  
  //! Set field Orientation
  inline void SetOrientation (const Handle(StepDimTol_RunoutZoneOrientation) &theOrientation)
  {
    myOrientation = theOrientation;
  }
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_RunoutZoneDefinition,StepDimTol_ToleranceZoneDefinition)

private: 
  Handle(StepDimTol_RunoutZoneOrientation) myOrientation;
};
#endif // _StepDimTol_RunoutToleranceZone_HeaderFile
