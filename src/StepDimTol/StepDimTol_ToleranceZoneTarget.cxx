// Created on: 2015-07-13
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

#include <StepDimTol_ToleranceZoneTarget.hxx>
#include <Interface_Macros.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <StepDimTol_GeneralDatumReference.hxx>
#include <StepDimTol_GeometricTolerance.hxx>

//=======================================================================
//function : StepDimTol_ToleranceZoneTarget
//purpose  : 
//=======================================================================

StepDimTol_ToleranceZoneTarget::StepDimTol_ToleranceZoneTarget () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepDimTol_ToleranceZoneTarget::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 2;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_GeometricTolerance))) return 3;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_GeneralDatumReference))) return 4;
  return 0;
}

Handle(StepShape_DimensionalLocation) StepDimTol_ToleranceZoneTarget::DimensionalLocation() const
{  return GetCasted(StepShape_DimensionalLocation,Value());  }

Handle(StepShape_DimensionalSize) StepDimTol_ToleranceZoneTarget::DimensionalSize() const
{  return GetCasted(StepShape_DimensionalSize,Value());  }

Handle(StepDimTol_GeometricTolerance) StepDimTol_ToleranceZoneTarget::GeometricTolerance() const
{  return GetCasted(StepDimTol_GeometricTolerance,Value());  }

Handle(StepDimTol_GeneralDatumReference) StepDimTol_ToleranceZoneTarget::GeneralDatumReference() const
{  return GetCasted(StepDimTol_GeneralDatumReference,Value());  }
