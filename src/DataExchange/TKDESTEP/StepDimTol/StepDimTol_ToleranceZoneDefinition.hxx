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
#include <StepRepr_ShapeAspect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepDimTol_ToleranceZone.hxx>

//! Representation of STEP entity ToleranceZoneDefinition
class StepDimTol_ToleranceZoneDefinition : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_ToleranceZoneDefinition();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepDimTol_ToleranceZone>&                               theZone,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>>& theBoundaries);

  //! Returns field Boundaries
  inline occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>> Boundaries() const
  {
    return myBoundaries;
  }

  //! Set field Boundaries
  inline void SetBoundaries(
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>>& theBoundaries)
  {
    myBoundaries = theBoundaries;
  }

  //! Returns number of Boundaries
  inline int NbBoundaries() const { return (myBoundaries.IsNull() ? 0 : myBoundaries->Length()); }

  //! Returns Boundaries with the given number
  inline occ::handle<StepRepr_ShapeAspect> BoundariesValue(const int theNum) const
  {
    return myBoundaries->Value(theNum);
  }

  //! Sets Boundaries with given number
  inline void SetBoundariesValue(const int theNum, const occ::handle<StepRepr_ShapeAspect>& theItem)
  {
    myBoundaries->SetValue(theNum, theItem);
  }

  //! Returns field Zone
  inline occ::handle<StepDimTol_ToleranceZone> Zone() { return myZone; }

  //! Set field Zone
  inline void SetZone(const occ::handle<StepDimTol_ToleranceZone>& theZone) { myZone = theZone; }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_ToleranceZoneDefinition, Standard_Transient)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_ShapeAspect>>> myBoundaries;
  occ::handle<StepDimTol_ToleranceZone>                               myZone;
};
#endif // _StepDimTol_ToleranceZoneDefinition_HeaderFile
