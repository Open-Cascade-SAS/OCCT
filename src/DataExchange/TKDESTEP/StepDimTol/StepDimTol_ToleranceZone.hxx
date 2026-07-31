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

#ifndef _StepDimTol_ToleranceZone_HeaderFile
#define _StepDimTol_ToleranceZone_HeaderFile

#include <Standard.hxx>

#include <StepRepr_ShapeAspect.hxx>
#include <Standard_Integer.hxx>
#include <StepDimTol_ToleranceZoneTarget.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepDimTol_ToleranceZoneForm.hxx>

class TCollection_HAsciiString;

// Avoid possible conflict with SetForm macro defined by windows.h
#ifdef SetForm
  #undef SetForm
#endif

//! Representation of STEP entity ToleranceZone
class StepDimTol_ToleranceZone : public StepRepr_ShapeAspect
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_ToleranceZone();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                            theName,
    const occ::handle<TCollection_HAsciiString>&                            theDescription,
    const occ::handle<StepRepr_ProductDefinitionShape>&                     theOfShape,
    const StepData_Logical                                                  theProductDefinitional,
    const occ::handle<NCollection_HArray1<StepDimTol_ToleranceZoneTarget>>& theDefiningTolerance,
    const occ::handle<StepDimTol_ToleranceZoneForm>&                        theForm);

  //! Returns field DefiningTolerance
  inline occ::handle<NCollection_HArray1<StepDimTol_ToleranceZoneTarget>> DefiningTolerance() const
  {
    return myDefiningTolerance;
  }

  //! Set field DefiningTolerance
  inline void SetDefiningTolerance(
    const occ::handle<NCollection_HArray1<StepDimTol_ToleranceZoneTarget>>& theDefiningTolerance)
  {
    myDefiningTolerance = theDefiningTolerance;
  }

  //! Returns number of Defining Tolerances
  inline int NbDefiningTolerances() const
  {
    return (myDefiningTolerance.IsNull() ? 0 : myDefiningTolerance->Length());
  }

  //! Returns Defining Tolerance with the given number
  inline StepDimTol_ToleranceZoneTarget DefiningToleranceValue(const int theNum) const
  {
    return myDefiningTolerance->Value(theNum);
  }

  //! Sets Defining Tolerance with given number
  inline void SetDefiningToleranceValue(const int                             theNum,
                                        const StepDimTol_ToleranceZoneTarget& theItem)
  {
    myDefiningTolerance->SetValue(theNum, theItem);
  }

  //! Returns field Form
  inline occ::handle<StepDimTol_ToleranceZoneForm> Form() { return myForm; }

  //! Set field Form
  inline void SetForm(const occ::handle<StepDimTol_ToleranceZoneForm>& theForm)
  {
    myForm = theForm;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_ToleranceZone, StepRepr_ShapeAspect)

private:
  occ::handle<NCollection_HArray1<StepDimTol_ToleranceZoneTarget>> myDefiningTolerance;
  occ::handle<StepDimTol_ToleranceZoneForm>                        myForm;
};
#endif // _StepDimTol_ToleranceZone_HeaderFile
