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

#ifndef _StepDimTol_DatumReferenceModifierWithValue_HeaderFile
#define _StepDimTol_DatumReferenceModifierWithValue_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepDimTol_DatumReferenceModifierType.hxx>

class StepDimTol_DatumReferenceModifierWithValue;
DEFINE_STANDARD_HANDLE(StepDimTol_DatumReferenceModifierWithValue, Standard_Transient)
//! Representation of STEP entity DatumReferenceModifierWithValue
class StepDimTol_DatumReferenceModifierWithValue : public Standard_Transient
{

public:
  
  //! Empty constructor
  Standard_EXPORT StepDimTol_DatumReferenceModifierWithValue();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT   void Init (const StepDimTol_DatumReferenceModifierType& theModifierType,
                               const Handle(StepBasic_LengthMeasureWithUnit)& theModifierValue);
  
  //! Returns field ModifierType
  inline StepDimTol_DatumReferenceModifierType ModifierType () const
  {
    return myModifierType;
  }
  
  //! Set field ModifierType
  inline void SetModifierType (const StepDimTol_DatumReferenceModifierType &theModifierType)
  {
    myModifierType = theModifierType;
  }
  
  //! Returns field ModifierValue
  inline Handle(StepBasic_LengthMeasureWithUnit) ModifierValue()
  {
    return myModifierValue;
  }
  
  //! Set field ModifierValue
  inline void SetModifierValue(const Handle(StepBasic_LengthMeasureWithUnit)& theModifierValue)
  {
    myModifierValue = theModifierValue;
  }
  
  DEFINE_STANDARD_RTTIEXT(StepDimTol_DatumReferenceModifierWithValue,Standard_Transient)

private: 
  StepDimTol_DatumReferenceModifierType myModifierType;
  Handle(StepBasic_LengthMeasureWithUnit) myModifierValue;
};
#endif // _StepDimTol_DatumReferenceModifierWithValue_HeaderFile
