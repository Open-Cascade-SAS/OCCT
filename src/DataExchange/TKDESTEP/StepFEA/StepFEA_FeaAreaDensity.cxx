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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_FeaAreaDensity.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FeaAreaDensity, StepFEA_FeaMaterialPropertyRepresentationItem)

//=================================================================================================

StepFEA_FeaAreaDensity::StepFEA_FeaAreaDensity() {}

//=================================================================================================

void StepFEA_FeaAreaDensity::Init(const Handle(TCollection_HAsciiString)& aRepresentationItem_Name,
                                  const Standard_Real                     aFeaConstant)
{
  StepFEA_FeaMaterialPropertyRepresentationItem::Init(aRepresentationItem_Name);

  theFeaConstant = aFeaConstant;
}

//=================================================================================================

Standard_Real StepFEA_FeaAreaDensity::FeaConstant() const
{
  return theFeaConstant;
}

//=================================================================================================

void StepFEA_FeaAreaDensity::SetFeaConstant(const Standard_Real aFeaConstant)
{
  theFeaConstant = aFeaConstant;
}
