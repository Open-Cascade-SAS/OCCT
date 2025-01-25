// Created on: 1998-06-30
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _StepBasic_ProductDefinitionEffectivity_HeaderFile
#define _StepBasic_ProductDefinitionEffectivity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_Effectivity.hxx>
class StepBasic_ProductDefinitionRelationship;
class TCollection_HAsciiString;

class StepBasic_ProductDefinitionEffectivity;
DEFINE_STANDARD_HANDLE(StepBasic_ProductDefinitionEffectivity, StepBasic_Effectivity)

class StepBasic_ProductDefinitionEffectivity : public StepBasic_Effectivity
{

public:
  Standard_EXPORT StepBasic_ProductDefinitionEffectivity();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&                aId,
                            const Handle(StepBasic_ProductDefinitionRelationship)& aUsage);

  Standard_EXPORT Handle(StepBasic_ProductDefinitionRelationship) Usage() const;

  Standard_EXPORT void SetUsage(const Handle(StepBasic_ProductDefinitionRelationship)& aUsage);

  DEFINE_STANDARD_RTTIEXT(StepBasic_ProductDefinitionEffectivity, StepBasic_Effectivity)

protected:
private:
  Handle(StepBasic_ProductDefinitionRelationship) theUsage;
};

#endif // _StepBasic_ProductDefinitionEffectivity_HeaderFile
