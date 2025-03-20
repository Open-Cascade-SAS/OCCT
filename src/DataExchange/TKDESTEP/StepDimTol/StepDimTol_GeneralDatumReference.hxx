// Created on: 2015-07-16
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

#ifndef _StepDimTol_GeneralDatumReference_HeaderFile
#define _StepDimTol_GeneralDatumReference_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepDimTol_DatumOrCommonDatum.hxx>
#include <StepDimTol_HArray1OfDatumReferenceModifier.hxx>

class StepDimTol_GeneralDatumReference;
DEFINE_STANDARD_HANDLE(StepDimTol_GeneralDatumReference, StepRepr_ShapeAspect)

//! Representation of STEP entity GeneralDatumReference
class StepDimTol_GeneralDatumReference : public StepRepr_ShapeAspect
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_GeneralDatumReference();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&        theName,
                            const Handle(TCollection_HAsciiString)&        theDescription,
                            const Handle(StepRepr_ProductDefinitionShape)& theOfShape,
                            const StepData_Logical                         theProductDefinitional,
                            const StepDimTol_DatumOrCommonDatum&           theBase,
                            const Standard_Boolean                         theHasModifiers,
                            const Handle(StepDimTol_HArray1OfDatumReferenceModifier)& theModifiers);

  //! Returns field Base
  inline StepDimTol_DatumOrCommonDatum Base() { return myBase; }

  //! Set field Base
  inline void SetBase(const StepDimTol_DatumOrCommonDatum& theBase) { myBase = theBase; }

  //! Indicates is field Modifiers exist
  inline Standard_Boolean HasModifiers() const
  {
    return !(myModifiers.IsNull() || myModifiers->Length() == 0);
  }

  //! Returns field Modifiers
  inline Handle(StepDimTol_HArray1OfDatumReferenceModifier) Modifiers() { return myModifiers; }

  //! Set field Modifiers
  inline void SetModifiers(const Handle(StepDimTol_HArray1OfDatumReferenceModifier)& theModifiers)
  {
    myModifiers = theModifiers;
  }

  //! Returns number of Modifiers
  inline Standard_Integer NbModifiers() const
  {
    return (myModifiers.IsNull() ? 0 : myModifiers->Length());
  }

  //! Returns Modifiers with the given number
  inline StepDimTol_DatumReferenceModifier ModifiersValue(const Standard_Integer theNum) const
  {
    return myModifiers->Value(theNum);
  }

  //! Sets Modifiers with given number
  inline void ModifiersValue(const Standard_Integer                   theNum,
                             const StepDimTol_DatumReferenceModifier& theItem)
  {
    myModifiers->SetValue(theNum, theItem);
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_GeneralDatumReference, StepRepr_ShapeAspect)

private:
  StepDimTol_DatumOrCommonDatum                      myBase;
  Handle(StepDimTol_HArray1OfDatumReferenceModifier) myModifiers;
};
#endif // _StepDimTol_GeneralDatumReference_HeaderFile
