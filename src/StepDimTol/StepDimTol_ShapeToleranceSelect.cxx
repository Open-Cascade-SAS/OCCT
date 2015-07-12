// Created on: 2003-06-04
// Created by: Galina KULIKOVA
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
#include <StepDimTol_GeometricTolerance.hxx>
#include <StepDimTol_ShapeToleranceSelect.hxx>
#include <StepShape_PlusMinusTolerance.hxx>

//=======================================================================
//function : StepDimTol_ShapeToleranceSelect
//purpose  : 
//=======================================================================
StepDimTol_ShapeToleranceSelect::StepDimTol_ShapeToleranceSelect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepDimTol_ShapeToleranceSelect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_PlusMinusTolerance))) return 2;
  return 0;
}

//=======================================================================
//function : GeometricTolerance
//purpose  : 
//=======================================================================

Handle(StepDimTol_GeometricTolerance) StepDimTol_ShapeToleranceSelect::GeometricTolerance () const
{
  return Handle(StepDimTol_GeometricTolerance)::DownCast(Value());
}

//=======================================================================
//function : PlusMinusTolerance
//purpose  : 
//=======================================================================

Handle(StepShape_PlusMinusTolerance) StepDimTol_ShapeToleranceSelect::PlusMinusTolerance () const
{
  return Handle(StepShape_PlusMinusTolerance)::DownCast(Value());
}
