// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_Curve3dElementProperty_HeaderFile
#define _StepFEA_Curve3dElementProperty_HeaderFile

#include <Standard.hxx>

#include <StepFEA_CurveElementInterval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepFEA_CurveElementEndOffset.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepFEA_CurveElementEndRelease.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

//! Representation of STEP entity Curve3dElementProperty
class StepFEA_Curve3dElementProperty : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_Curve3dElementProperty();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&                aPropertyId,
    const occ::handle<TCollection_HAsciiString>&                aDescription,
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementInterval>>>&   aIntervalDefinitions,
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndOffset>>>&  aEndOffsets,
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndRelease>>>& aEndReleases);

  //! Returns field PropertyId
  Standard_EXPORT occ::handle<TCollection_HAsciiString> PropertyId() const;

  //! Set field PropertyId
  Standard_EXPORT void SetPropertyId(const occ::handle<TCollection_HAsciiString>& PropertyId);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  //! Returns field IntervalDefinitions
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementInterval>>> IntervalDefinitions() const;

  //! Set field IntervalDefinitions
  Standard_EXPORT void SetIntervalDefinitions(
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementInterval>>>& IntervalDefinitions);

  //! Returns field EndOffsets
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndOffset>>> EndOffsets() const;

  //! Set field EndOffsets
  Standard_EXPORT void SetEndOffsets(
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndOffset>>>& EndOffsets);

  //! Returns field EndReleases
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndRelease>>> EndReleases() const;

  //! Set field EndReleases
  Standard_EXPORT void SetEndReleases(
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndRelease>>>& EndReleases);

  DEFINE_STANDARD_RTTIEXT(StepFEA_Curve3dElementProperty, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString>                thePropertyId;
  occ::handle<TCollection_HAsciiString>                theDescription;
  occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementInterval>>>   theIntervalDefinitions;
  occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndOffset>>>  theEndOffsets;
  occ::handle<NCollection_HArray1<occ::handle<StepFEA_CurveElementEndRelease>>> theEndReleases;
};

#endif // _StepFEA_Curve3dElementProperty_HeaderFile
