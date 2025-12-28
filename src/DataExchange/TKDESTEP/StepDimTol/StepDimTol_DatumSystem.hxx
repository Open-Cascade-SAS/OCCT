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
#include <StepDimTol_DatumReferenceCompartment.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! Representation of STEP entity DatumSystem
class StepDimTol_DatumSystem : public StepRepr_ShapeAspect
{

public:
  //! Empty constructor
  Standard_EXPORT StepDimTol_DatumSystem();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                      theName,
    const occ::handle<TCollection_HAsciiString>&                      theDescription,
    const occ::handle<StepRepr_ProductDefinitionShape>&               theOfShape,
    const StepData_Logical                                       theProductDefinitional,
    const occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>>& theConstituents);

  //! Returns field Constituents
  inline occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>> Constituents()
  {
    return myConstituents;
  }

  //! Set field Constituents
  inline void SetConstituents(
    const occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>>& theConstituents)
  {
    myConstituents = theConstituents;
  }

  //! Returns number of Constituents
  inline int NbConstituents() const
  {
    return (myConstituents.IsNull() ? 0 : myConstituents->Length());
  }

  //! Returns Constituents with the given number
  inline occ::handle<StepDimTol_DatumReferenceCompartment> ConstituentsValue(
    const int num) const
  {
    return myConstituents->Value(num);
  }

  //! Sets Constituents with given number
  inline void ConstituentsValue(const int                              num,
                                const occ::handle<StepDimTol_DatumReferenceCompartment>& theItem)
  {
    myConstituents->SetValue(num, theItem);
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_DatumSystem, StepRepr_ShapeAspect)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceCompartment>>> myConstituents;
};
#endif // _StepDimTol_DatumSystem_HeaderFile
