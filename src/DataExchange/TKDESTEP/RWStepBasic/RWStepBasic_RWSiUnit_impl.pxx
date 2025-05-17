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

#include "RWStepBasic_RWSiUnit.pxx"
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepBasic_RWSiPrefix.pxx"
#include "RWStepBasic_RWSiUnitName.pxx"

RWStepBasic_RWSiUnit::RWStepBasic_RWSiUnit() {}

void RWStepBasic_RWSiUnit::ReadStep(const Handle(StepData_StepReaderData)& data,
                                    const Standard_Integer                 num,
                                    Handle(Interface_Check)&               ach,
                                    const Handle(StepBasic_SiUnit)&        ent) const
{
  // --- Number of Parameter Control ---
  if (!data->CheckNbParams(num, 3, ach, "si_unit"))
    return;

  // --- inherited field : dimensions ---
  // --- this field is redefined ---
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->CheckDerived(num, 1, "dimensions", ach, Standard_False);

  // --- own field : prefix ---
  StepBasic_SiPrefix aPrefix    = StepBasic_spExa;
  Standard_Boolean   hasAprefix = Standard_False;
  if (data->IsParamDefined(num, 2))
  {
    if (data->ParamType(num, 2) == Interface_ParamEnum)
    {
      Standard_CString text = data->ParamCValue(num, 2);
      hasAprefix            = RWStepBasic_RWSiPrefix::ConvertToEnum(text, aPrefix);
      if (!hasAprefix)
      {
        ach->AddFail("Enumeration si_prefix has not an allowed value");
      }
    }
    else
    {
      ach->AddFail("Parameter #2 (prefix) is not an enumeration");
    }
  }

  // --- own field : name ---
  StepBasic_SiUnitName aName = StepBasic_sunMetre;
  if (data->ParamType(num, 3) == Interface_ParamEnum)
  {
    Standard_CString text = data->ParamCValue(num, 3);
    if (!RWStepBasic_RWSiUnitName::ConvertToEnum(text, aName))
    {
      ach->AddFail("Enumeration si_unit_name has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #3 (name) is not an enumeration");

  //--- Initialisation of the read entity ---
  ent->Init(hasAprefix, aPrefix, aName);
}

void RWStepBasic_RWSiUnit::WriteStep(StepData_StepWriter&            SW,
                                     const Handle(StepBasic_SiUnit)& ent) const
{

  // --- inherited field dimensions ---
  SW.SendDerived();

  // --- own field : prefix ---
  Standard_Boolean hasAprefix = ent->HasPrefix();
  if (hasAprefix)
    SW.SendEnum(RWStepBasic_RWSiPrefix::ConvertToString(ent->Prefix()));
  else
    SW.SendUndef();

  // --- own field : name ---
  SW.SendEnum(RWStepBasic_RWSiUnitName::ConvertToString(ent->Name()));
}
