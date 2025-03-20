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

#ifndef _StepDimTol_DatumSystem_HeaderFile
#define _StepDimTol_DatumSystem_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <StepDimTol_HArray1OfDatumReferenceCompartment.hxx>

class StepDimTol_DatumSystem;
DEFINE_STANDARD_HANDLE(StepDimTol_DatumSystem, StepRepr_ShapeAspect)

//! Representation of STEP entity DatumSystem
class StepDimTol_DatumSystem : public StepRepr_ShapeAspect
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_DatumSystem();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&                      theName,
    const Handle(TCollection_HAsciiString)&                      theDescription,
    const Handle(StepRepr_ProductDefinitionShape)&               theOfShape,
    const StepData_Logical                                       theProductDefinitional,
    const Handle(StepDimTol_HArray1OfDatumReferenceCompartment)& theConstituents);

  //! Returns field Constituents
  inline Handle(StepDimTol_HArray1OfDatumReferenceCompartment) Constituents()
  {
    return myConstituents;
  }

  //! Set field Constituents
  inline void SetConstituents(
    const Handle(StepDimTol_HArray1OfDatumReferenceCompartment)& theConstituents)
  {
    myConstituents = theConstituents;
  }

  //! Returns number of Constituents
  inline Standard_Integer NbConstituents() const
  {
    return (myConstituents.IsNull() ? 0 : myConstituents->Length());
  }

  //! Returns Constituents with the given number
  inline Handle(StepDimTol_DatumReferenceCompartment) ConstituentsValue(
    const Standard_Integer num) const
  {
    return myConstituents->Value(num);
  }

  //! Sets Constituents with given number
  inline void ConstituentsValue(const Standard_Integer                              num,
                                const Handle(StepDimTol_DatumReferenceCompartment)& theItem)
  {
    myConstituents->SetValue(num, theItem);
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_DatumSystem, StepRepr_ShapeAspect)

private:
  Handle(StepDimTol_HArray1OfDatumReferenceCompartment) myConstituents;
};
#endif // _StepDimTol_DatumSystem_HeaderFile
