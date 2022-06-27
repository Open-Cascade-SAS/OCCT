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

#ifndef _StepDimTol_ToleranceZoneDefinition_HeaderFile
#define _StepDimTol_ToleranceZoneDefinition_HeaderFile

#include <Standard.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <StepRepr_HArray1OfShapeAspect.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepDimTol_ToleranceZone.hxx>


class StepDimTol_ToleranceZoneDefinition;
DEFINE_STANDARD_HANDLE(StepDimTol_ToleranceZoneDefinition, Standard_Transient)
//! Representation of STEP entity ToleranceZoneDefinition
class StepDimTol_ToleranceZoneDefinition : public Standard_Transient
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_ToleranceZoneDefinition();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init (const Handle(StepDimTol_ToleranceZone)& theZone,
                               const Handle(StepRepr_HArray1OfShapeAspect)& theBoundaries);
  
  //! Returns field Boundaries
  inline Handle(StepRepr_HArray1OfShapeAspect) Boundaries () const
  {
    return myBoundaries;
  }
  
  //! Set field Boundaries
  inline void SetBoundaries (const Handle(StepRepr_HArray1OfShapeAspect) &theBoundaries)
  {
    myBoundaries = theBoundaries;
  }
  
  //! Returns number of Boundaries
  inline Standard_Integer NbBoundaries () const
  {  
    return (myBoundaries.IsNull() ? 0 : myBoundaries->Length());
  }
  
  //! Returns Boundaries with the given number
  inline Handle(StepRepr_ShapeAspect) BoundariesValue(const Standard_Integer theNum) const
  {  
    return myBoundaries->Value(theNum);
  }
  
  //! Sets Boundaries with given number
  inline void SetBoundariesValue(const Standard_Integer theNum, const Handle(StepRepr_ShapeAspect)& theItem)
  {  
    myBoundaries->SetValue (theNum, theItem);
  }
  
  //! Returns field Zone
  inline Handle(StepDimTol_ToleranceZone) Zone()
  {
    return myZone;
  }
  
  //! Set field Zone
  inline void SetZone(const Handle(StepDimTol_ToleranceZone)& theZone)
  {
    myZone = theZone;
  }
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_ToleranceZoneDefinition,Standard_Transient)

private: 
  Handle(StepRepr_HArray1OfShapeAspect) myBoundaries;
  Handle(StepDimTol_ToleranceZone) myZone;
};
#endif // _StepDimTol_ToleranceZoneDefinition_HeaderFile
