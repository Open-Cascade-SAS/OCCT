// Created on: 2015-07-07
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

#ifndef _StepDimTol_GeometricToleranceWithModifiers_HeaderFile
#define _StepDimTol_GeometricToleranceWithModifiers_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_HArray1OfGeometricToleranceModifier.hxx>

class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;
class StepDimTol_GeometricToleranceTarget;

class StepDimTol_GeometricToleranceWithModifiers;
DEFINE_STANDARD_HANDLE(StepDimTol_GeometricToleranceWithModifiers, StepDimTol_GeometricTolerance)

//! Representation of STEP entity GeometricToleranceWithModifiers
class StepDimTol_GeometricToleranceWithModifiers : public StepDimTol_GeometricTolerance
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithModifiers();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                       theName,
    const Handle(TCollection_HAsciiString)&                       theDescription,
    const Handle(StepBasic_MeasureWithUnit)&                      theMagnitude,
    const StepDimTol_GeometricToleranceTarget&                    theTolerancedShapeAspect,
    const Handle(StepDimTol_HArray1OfGeometricToleranceModifier)& theModifiers);

  //! Returns field Modifiers
  inline Handle(StepDimTol_HArray1OfGeometricToleranceModifier) Modifiers() const
  {
    return myModifiers;
  }

  //! Set field Modifiers
  inline void SetModifiers(
    const Handle(StepDimTol_HArray1OfGeometricToleranceModifier)& theModifiers)
  {
    myModifiers = theModifiers;
  }

  //! Returns number of modifiers
  inline Standard_Integer NbModifiers() const
  {
    return (myModifiers.IsNull() ? 0 : myModifiers->Length());
  }

  //! Returns modifier with the given number
  inline StepDimTol_GeometricToleranceModifier ModifierValue(const Standard_Integer theNum) const
  {
    return myModifiers->Value(theNum);
  }

  //! Sets modifier with given number
  inline void SetModifierValue(const Standard_Integer                      theNum,
                               const StepDimTol_GeometricToleranceModifier theItem)
  {
    myModifiers->SetValue(theNum, theItem);
  }
  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithModifiers, StepDimTol_GeometricTolerance)

private:
  Handle(StepDimTol_HArray1OfGeometricToleranceModifier) myModifiers;
};
#endif // _StepDimTol_GeometricToleranceWithModifiers_HeaderFile
