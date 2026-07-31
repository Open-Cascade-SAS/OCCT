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

#ifndef _StepFEA_CurveElementEndOffset_HeaderFile
#define _StepFEA_CurveElementEndOffset_HeaderFile

#include <Standard.hxx>

#include <StepFEA_CurveElementEndCoordinateSystem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>

//! Representation of STEP entity CurveElementEndOffset
class StepFEA_CurveElementEndOffset : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_CurveElementEndOffset();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const StepFEA_CurveElementEndCoordinateSystem&  aCoordinateSystem,
                            const occ::handle<NCollection_HArray1<double>>& aOffsetVector);

  //! Returns field CoordinateSystem
  Standard_EXPORT StepFEA_CurveElementEndCoordinateSystem CoordinateSystem() const;

  //! Set field CoordinateSystem
  Standard_EXPORT void SetCoordinateSystem(
    const StepFEA_CurveElementEndCoordinateSystem& CoordinateSystem);

  //! Returns field OffsetVector
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> OffsetVector() const;

  //! Set field OffsetVector
  Standard_EXPORT void SetOffsetVector(
    const occ::handle<NCollection_HArray1<double>>& OffsetVector);

  DEFINE_STANDARD_RTTIEXT(StepFEA_CurveElementEndOffset, Standard_Transient)

private:
  StepFEA_CurveElementEndCoordinateSystem  theCoordinateSystem;
  occ::handle<NCollection_HArray1<double>> theOffsetVector;
};

#endif // _StepFEA_CurveElementEndOffset_HeaderFile
