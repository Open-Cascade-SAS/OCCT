// Created on: 2002-12-14
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

#ifndef _StepFEA_FreedomsList_HeaderFile
#define _StepFEA_FreedomsList_HeaderFile

#include <Standard.hxx>

#include <StepFEA_HArray1OfDegreeOfFreedom.hxx>
#include <Standard_Transient.hxx>

class StepFEA_FreedomsList;
DEFINE_STANDARD_HANDLE(StepFEA_FreedomsList, Standard_Transient)

//! Representation of STEP entity FreedomsList
class StepFEA_FreedomsList : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_FreedomsList();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepFEA_HArray1OfDegreeOfFreedom)& aFreedoms);

  //! Returns field Freedoms
  Standard_EXPORT Handle(StepFEA_HArray1OfDegreeOfFreedom) Freedoms() const;

  //! Set field Freedoms
  Standard_EXPORT void SetFreedoms(const Handle(StepFEA_HArray1OfDegreeOfFreedom)& Freedoms);

  DEFINE_STANDARD_RTTIEXT(StepFEA_FreedomsList, Standard_Transient)

protected:
private:
  Handle(StepFEA_HArray1OfDegreeOfFreedom) theFreedoms;
};

#endif // _StepFEA_FreedomsList_HeaderFile
