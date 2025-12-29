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

#include <MoniTool_Macros.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepDimTol_DatumReferenceElement.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

StepDimTol_DatumOrCommonDatum::StepDimTol_DatumOrCommonDatum() = default;

//=================================================================================================

int StepDimTol_DatumOrCommonDatum::CaseNum(const occ::handle<Standard_Transient>& ent) const
{
  if (ent.IsNull())
    return 0;
  if (ent->IsKind(STANDARD_TYPE(StepDimTol_Datum)))
    return 1;
  if (ent->IsKind(
        STANDARD_TYPE(NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceElement>>)))
    return 2;
  return 0;
}

occ::handle<StepDimTol_Datum> StepDimTol_DatumOrCommonDatum::Datum() const
{
  return GetCasted(StepDimTol_Datum, Value());
}

occ::handle<NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceElement>>>
  StepDimTol_DatumOrCommonDatum::CommonDatumList() const
{
  return GetCasted(NCollection_HArray1<occ::handle<StepDimTol_DatumReferenceElement>>, Value());
}
