// Created on : Sat May 02 12:41:15 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#ifndef _StepKinematics_LowOrderKinematicPairValue_HeaderFile_
#define _StepKinematics_LowOrderKinematicPairValue_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_PairValue.hxx>

#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>

//! Representation of STEP entity LowOrderKinematicPairValue
class StepKinematics_LowOrderKinematicPairValue : public StepKinematics_PairValue
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_LowOrderKinematicPairValue();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&     theRepresentationItem_Name,
                            const occ::handle<StepKinematics_KinematicPair>& thePairValue_AppliesToPair,
                            const double                         theActualTranslationX,
                            const double                         theActualTranslationY,
                            const double                         theActualTranslationZ,
                            const double                         theActualRotationX,
                            const double                         theActualRotationY,
                            const double                         theActualRotationZ);

  //! Returns field ActualTranslationX
  Standard_EXPORT double ActualTranslationX() const;
  //! Sets field ActualTranslationX
  Standard_EXPORT void SetActualTranslationX(const double theActualTranslationX);

  //! Returns field ActualTranslationY
  Standard_EXPORT double ActualTranslationY() const;
  //! Sets field ActualTranslationY
  Standard_EXPORT void SetActualTranslationY(const double theActualTranslationY);

  //! Returns field ActualTranslationZ
  Standard_EXPORT double ActualTranslationZ() const;
  //! Sets field ActualTranslationZ
  Standard_EXPORT void SetActualTranslationZ(const double theActualTranslationZ);

  //! Returns field ActualRotationX
  Standard_EXPORT double ActualRotationX() const;
  //! Sets field ActualRotationX
  Standard_EXPORT void SetActualRotationX(const double theActualRotationX);

  //! Returns field ActualRotationY
  Standard_EXPORT double ActualRotationY() const;
  //! Sets field ActualRotationY
  Standard_EXPORT void SetActualRotationY(const double theActualRotationY);

  //! Returns field ActualRotationZ
  Standard_EXPORT double ActualRotationZ() const;
  //! Sets field ActualRotationZ
  Standard_EXPORT void SetActualRotationZ(const double theActualRotationZ);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_LowOrderKinematicPairValue, StepKinematics_PairValue)

private:
  double myActualTranslationX;
  double myActualTranslationY;
  double myActualTranslationZ;
  double myActualRotationX;
  double myActualRotationY;
  double myActualRotationZ;
};
#endif // _StepKinematics_LowOrderKinematicPairValue_HeaderFile_
