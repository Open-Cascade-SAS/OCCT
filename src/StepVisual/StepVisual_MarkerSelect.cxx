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
#include <StepData_SelectMember.hxx>
#include <StepVisual_MarkerMember.hxx>
#include <StepVisual_MarkerSelect.hxx>

StepVisual_MarkerSelect::StepVisual_MarkerSelect () {  }

Standard_Integer StepVisual_MarkerSelect::CaseNum(const Handle(Standard_Transient)& /*ent*/) const
{
	return 0;
}

Handle(StepData_SelectMember)  StepVisual_MarkerSelect::NewMember () const
{  return new StepVisual_MarkerMember;  }

Standard_Integer  StepVisual_MarkerSelect::CaseMem
  (const Handle(StepData_SelectMember)& ent) const
{
  if (ent.IsNull()) return 0;
  Interface_ParamType type = ent->ParamType();
//  Void : on admet "non defini" (en principe, on ne devrait pas)
  if (type != Interface_ParamVoid && type != Interface_ParamEnum) return 0;
  if (ent->Matches("MARKER_TYPE")) return 1;
  return 0;
}

Handle(StepVisual_MarkerMember)  StepVisual_MarkerSelect::MarkerMember () const
{  return GetCasted(StepVisual_MarkerMember,Value());  }
