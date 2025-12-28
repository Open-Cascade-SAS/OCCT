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
#include <StepDimTol_GeometricToleranceModifier.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class TCollection_HAsciiString;
class StepDimTol_GeometricToleranceTarget;

//! Representation of STEP entity GeometricToleranceWithModifiers
class StepDimTol_GeometricToleranceWithModifiers : public StepDimTol_GeometricTolerance
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeometricToleranceWithModifiers();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                       theName,
    const occ::handle<TCollection_HAsciiString>&                       theDescription,
    const occ::handle<Standard_Transient>&                             theMagnitude,
    const StepDimTol_GeometricToleranceTarget&                    theTolerancedShapeAspect,
    const occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>>& theModifiers);

  //! Returns field Modifiers
  inline occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>> Modifiers() const
  {
    return myModifiers;
  }

  //! Set field Modifiers
  inline void SetModifiers(
    const occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>>& theModifiers)
  {
    myModifiers = theModifiers;
  }

  //! Returns number of modifiers
  inline int NbModifiers() const
  {
    return (myModifiers.IsNull() ? 0 : myModifiers->Length());
  }

  //! Returns modifier with the given number
  inline StepDimTol_GeometricToleranceModifier ModifierValue(const int theNum) const
  {
    return myModifiers->Value(theNum);
  }

  //! Sets modifier with given number
  inline void SetModifierValue(const int                      theNum,
                               const StepDimTol_GeometricToleranceModifier theItem)
  {
    myModifiers->SetValue(theNum, theItem);
  }
  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeometricToleranceWithModifiers, StepDimTol_GeometricTolerance)

private:
  occ::handle<NCollection_HArray1<StepDimTol_GeometricToleranceModifier>> myModifiers;
};
#endif // _StepDimTol_GeometricToleranceWithModifiers_HeaderFile
