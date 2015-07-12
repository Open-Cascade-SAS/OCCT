// Created on: 2003-02-04
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <Standard_Transient.hxx>
#include <StepFEA_ElementGroup.hxx>
#include <StepFEA_ElementOrElementGroup.hxx>
#include <StepFEA_ElementRepresentation.hxx>

//=======================================================================
//function : StepFEA_ElementOrElementGroup
//purpose  : 
//=======================================================================
StepFEA_ElementOrElementGroup::StepFEA_ElementOrElementGroup ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepFEA_ElementOrElementGroup::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_ElementRepresentation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepFEA_ElementGroup))) return 2;
  return 0;
}

//=======================================================================
//function : ElementRepresentation
//purpose  : 
//=======================================================================

Handle(StepFEA_ElementRepresentation) StepFEA_ElementOrElementGroup::ElementRepresentation () const
{
  return Handle(StepFEA_ElementRepresentation)::DownCast(Value());
}

//=======================================================================
//function : ElementGroup
//purpose  : 
//=======================================================================

Handle(StepFEA_ElementGroup) StepFEA_ElementOrElementGroup::ElementGroup () const
{
  return Handle(StepFEA_ElementGroup)::DownCast(Value());
}
