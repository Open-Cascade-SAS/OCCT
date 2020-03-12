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
#include <Standard_Type.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>

#include <TCollection_HAsciiString.hxx>

DEFINE_STANDARD_HANDLE(StepGeom_SuParameters, StepGeom_GeometricRepresentationItem)

//! Representation of STEP entity SuParameters
class StepGeom_SuParameters : public StepGeom_GeometricRepresentationItem
{
public :

  //! default constructor
  Standard_EXPORT StepGeom_SuParameters();

  //! Initialize all fields (own and inherited)
 Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                           const Standard_Real theA,
                           const Standard_Real theAlpha,
                           const Standard_Real theB,
                           const Standard_Real theBeta,
                           const Standard_Real theC,
                           const Standard_Real theGamma);

  //! Returns field A
  Standard_EXPORT Standard_Real A() const;
  //! Sets field A
  Standard_EXPORT void SetA (const Standard_Real theA);

  //! Returns field Alpha
  Standard_EXPORT Standard_Real Alpha() const;
  //! Sets field Alpha
  Standard_EXPORT void SetAlpha (const Standard_Real theAlpha);

  //! Returns field B
  Standard_EXPORT Standard_Real B() const;
  //! Sets field B
  Standard_EXPORT void SetB (const Standard_Real theB);

  //! Returns field Beta
  Standard_EXPORT Standard_Real Beta() const;
  //! Sets field Beta
  Standard_EXPORT void SetBeta (const Standard_Real theBeta);

  //! Returns field C
  Standard_EXPORT Standard_Real C() const;
  //! Sets field C
  Standard_EXPORT void SetC (const Standard_Real theC);

  //! Returns field Gamma
  Standard_EXPORT Standard_Real Gamma() const;
  //! Sets field Gamma
  Standard_EXPORT void SetGamma (const Standard_Real theGamma);

DEFINE_STANDARD_RTTIEXT(StepGeom_SuParameters, StepGeom_GeometricRepresentationItem)

private:
  Standard_Real myA;
  Standard_Real myAlpha;
  Standard_Real myB;
  Standard_Real myBeta;
  Standard_Real myC;
  Standard_Real myGamma;

};
#endif // _StepGeom_SuParameters_HeaderFile_
