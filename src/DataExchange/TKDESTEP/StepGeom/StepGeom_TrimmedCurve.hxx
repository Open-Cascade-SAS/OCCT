// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepGeom_TrimmedCurve_HeaderFile
#define _StepGeom_TrimmedCurve_HeaderFile

#include <Standard.hxx>

#include <StepGeom_TrimmingSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Boolean.hxx>
#include <StepGeom_TrimmingPreference.hxx>
#include <StepGeom_BoundedCurve.hxx>
#include <Standard_Integer.hxx>
class StepGeom_Curve;
class TCollection_HAsciiString;
class StepGeom_TrimmingSelect;

class StepGeom_TrimmedCurve : public StepGeom_BoundedCurve
{

public:
  //! Returns a TrimmedCurve
  Standard_EXPORT StepGeom_TrimmedCurve();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepGeom_Curve>&           aBasisCurve,
                            const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim1,
                            const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim2,
                            const bool                        aSenseAgreement,
                            const StepGeom_TrimmingPreference aMasterRepresentation);

  Standard_EXPORT void SetBasisCurve(const occ::handle<StepGeom_Curve>& aBasisCurve);

  Standard_EXPORT occ::handle<StepGeom_Curve> BasisCurve() const;

  Standard_EXPORT void SetTrim1(
    const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim1);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> Trim1() const;

  Standard_EXPORT StepGeom_TrimmingSelect Trim1Value(const int num) const;

  Standard_EXPORT int NbTrim1() const;

  Standard_EXPORT void SetTrim2(
    const occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>>& aTrim2);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> Trim2() const;

  Standard_EXPORT StepGeom_TrimmingSelect Trim2Value(const int num) const;

  Standard_EXPORT int NbTrim2() const;

  Standard_EXPORT void SetSenseAgreement(const bool aSenseAgreement);

  Standard_EXPORT bool SenseAgreement() const;

  Standard_EXPORT void SetMasterRepresentation(
    const StepGeom_TrimmingPreference aMasterRepresentation);

  Standard_EXPORT StepGeom_TrimmingPreference MasterRepresentation() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_TrimmedCurve, StepGeom_BoundedCurve)

private:
  occ::handle<StepGeom_Curve>                               basisCurve;
  occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> trim1;
  occ::handle<NCollection_HArray1<StepGeom_TrimmingSelect>> trim2;
  bool                                                      senseAgreement;
  StepGeom_TrimmingPreference                               masterRepresentation;
};

#endif // _StepGeom_TrimmedCurve_HeaderFile
