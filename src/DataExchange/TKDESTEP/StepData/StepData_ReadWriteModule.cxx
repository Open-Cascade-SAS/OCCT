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

#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_ReadWriteModule, Interface_ReaderModule)

int StepData_ReadWriteModule::CaseNum(const occ::handle<Interface_FileReaderData>& data,
                                      const int                                    num) const
{
  DeclareAndCast(StepData_StepReaderData, stepdat, data);
  if (stepdat.IsNull())
    return 0;
  if (stepdat->IsComplex(num))
  {
    NCollection_Sequence<TCollection_AsciiString> types;
    stepdat->ComplexType(num, types);
    if (types.IsEmpty())
      return 0;
    if (types.Length() == 1)
      return CaseStep(types.Value(1));
    else
      return CaseStep(types);
  }
  return CaseStep(stepdat->RecordType(num));
}

int StepData_ReadWriteModule::CaseStep(const NCollection_Sequence<TCollection_AsciiString>&) const
{
  return 0;
} // default

bool StepData_ReadWriteModule::IsComplex(const int) const
{
  return false;
} // default

TCollection_AsciiString StepData_ReadWriteModule::ShortType(const int) const
{
  return TCollection_AsciiString("");
} // default empty

bool StepData_ReadWriteModule::ComplexType(const int,
                                           NCollection_Sequence<TCollection_AsciiString>&) const
{
  return false;
}

//=================================================================================================

void StepData_ReadWriteModule::Read(const int                                    CN,
                                    const occ::handle<Interface_FileReaderData>& data,
                                    const int                                    num,
                                    occ::handle<Interface_Check>&                ach,
                                    const occ::handle<Standard_Transient>&       ent) const
{
  DeclareAndCast(StepData_StepReaderData, stepdat, data);
  if (stepdat.IsNull())
    return;
  ReadStep(CN, stepdat, num, ach, ent);
}
