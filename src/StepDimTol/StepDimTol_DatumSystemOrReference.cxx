// Created on: 2015-07-21
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepDimTol_DatumSystemOrReference.hxx>
#include <Interface_Macros.hxx>
#include <StepDimTol_DatumSystem.hxx>
#include <StepDimTol_DatumReference.hxx>

//=======================================================================
//function : StepDimTol_DatumSystemOrReference
//purpose  : 
//=======================================================================

StepDimTol_DatumSystemOrReference::StepDimTol_DatumSystemOrReference () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepDimTol_DatumSystemOrReference::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_DatumSystem))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_DatumReference))) return 2;
  return 0;
}

Handle(StepDimTol_DatumSystem) StepDimTol_DatumSystemOrReference::DatumSystem() const
{  return GetCasted(StepDimTol_DatumSystem,Value());  }

Handle(StepDimTol_DatumReference) StepDimTol_DatumSystemOrReference::DatumReference() const
{  return GetCasted(StepDimTol_DatumReference,Value());  }
