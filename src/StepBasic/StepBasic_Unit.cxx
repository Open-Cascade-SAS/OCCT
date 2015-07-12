// Created on: 1999-09-08
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepBasic_DerivedUnit.hxx>
#include <StepBasic_NamedUnit.hxx>
#include <StepBasic_Unit.hxx>

//=======================================================================
//function : StepBasic_Unit
//purpose  : 
//=======================================================================
StepBasic_Unit::StepBasic_Unit () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepBasic_Unit::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_NamedUnit))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepBasic_DerivedUnit))) return 2;
  return 0;
}

//=======================================================================
//function : NamedUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_NamedUnit) StepBasic_Unit::NamedUnit () const
{
  return GetCasted(StepBasic_NamedUnit,Value());
}

//=======================================================================
//function : DerivedUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_DerivedUnit) StepBasic_Unit::DerivedUnit () const
{
  return GetCasted(StepBasic_DerivedUnit,Value());
}

