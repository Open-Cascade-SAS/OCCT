// Created on : Sat May 02 12:41:14 2020
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

#ifndef _StepGeom_SuParameters_HeaderFile_
#define _StepGeom_SuParameters_HeaderFile_

#include <Standard.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>

#include <TCollection_HAsciiString.hxx>

//! Representation of STEP entity SuParameters
class StepGeom_SuParameters : public StepGeom_GeometricRepresentationItem
{
public:
  //! default constructor
  Standard_EXPORT StepGeom_SuParameters();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theRepresentationItem_Name,
                            const double                     theA,
                            const double                     theAlpha,
                            const double                     theB,
                            const double                     theBeta,
                            const double                     theC,
                            const double                     theGamma);

  //! Returns field A
  Standard_EXPORT double A() const;
  //! Sets field A
  Standard_EXPORT void SetA(const double theA);

  //! Returns field Alpha
  Standard_EXPORT double Alpha() const;
  //! Sets field Alpha
  Standard_EXPORT void SetAlpha(const double theAlpha);

  //! Returns field B
  Standard_EXPORT double B() const;
  //! Sets field B
  Standard_EXPORT void SetB(const double theB);

  //! Returns field Beta
  Standard_EXPORT double Beta() const;
  //! Sets field Beta
  Standard_EXPORT void SetBeta(const double theBeta);

  //! Returns field C
  Standard_EXPORT double C() const;
  //! Sets field C
  Standard_EXPORT void SetC(const double theC);

  //! Returns field Gamma
  Standard_EXPORT double Gamma() const;
  //! Sets field Gamma
  Standard_EXPORT void SetGamma(const double theGamma);

  DEFINE_STANDARD_RTTIEXT(StepGeom_SuParameters, StepGeom_GeometricRepresentationItem)

private:
  double myA;
  double myAlpha;
  double myB;
  double myBeta;
  double myC;
  double myGamma;
};
#endif // _StepGeom_SuParameters_HeaderFile_
