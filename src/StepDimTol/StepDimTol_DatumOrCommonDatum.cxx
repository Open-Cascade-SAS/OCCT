// Created on: 2015-07-16
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

#include <StepDimTol_DatumOrCommonDatum.hxx>

#include <Interface_Macros.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_HArray1OfDatumReferenceElement.hxx>

//=======================================================================
//function : StepDimTol_DatumOrCommonDatum
//purpose  : 
//=======================================================================

StepDimTol_DatumOrCommonDatum::StepDimTol_DatumOrCommonDatum () {  }

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepDimTol_DatumOrCommonDatum::CaseNum(const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_Datum))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_HArray1OfDatumReferenceElement))) return 2;
  return 0;
}

Handle(StepDimTol_Datum) StepDimTol_DatumOrCommonDatum::Datum() const
{  return GetCasted(StepDimTol_Datum,Value());  }

Handle(StepDimTol_HArray1OfDatumReferenceElement) StepDimTol_DatumOrCommonDatum::CommonDatumList() const
{  return GetCasted(StepDimTol_HArray1OfDatumReferenceElement,Value());  }

