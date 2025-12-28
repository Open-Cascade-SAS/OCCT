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

#include <HeaderSection_FileSchema.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <RWHeaderSection_RWFileSchema.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWHeaderSection_RWFileSchema::RWHeaderSection_RWFileSchema() = default;

void RWHeaderSection_RWFileSchema::ReadStep(const occ::handle<StepData_StepReaderData>&  data,
                                            const int                                    num,
                                            occ::handle<Interface_Check>&                ach,
                                            const occ::handle<HeaderSection_FileSchema>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 1, ach, "file_schema has not 1 parameter(s)"))
    return;

  // --- own field : schemaIdentifiers ---

  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> aSchemaIdentifiers;
  occ::handle<TCollection_HAsciiString>                                   aSchemaIdentifiersItem;
  int                                                                     nsub1;
  nsub1 = data->SubListNumber(num, 1, false);
  if (nsub1 != 0)
  {
    int nb1            = data->NbParams(nsub1);
    aSchemaIdentifiers = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nb1);
    for (int i1 = 1; i1 <= nb1; i1++)
    {
      bool stat1 = data->ReadString(nsub1, i1, "schema_identifiers", ach, aSchemaIdentifiersItem);
      if (stat1)
        aSchemaIdentifiers->SetValue(i1, aSchemaIdentifiersItem);
    }
  }
  else
  {
    ach->AddFail("Parameter #1 (schema_identifiers) is not a LIST");
  }

  //--- Initialisation of the read entity ---

  if (!ach->HasFailed())
    ent->Init(aSchemaIdentifiers);
}

void RWHeaderSection_RWFileSchema::WriteStep(StepData_StepWriter&                         SW,
                                             const occ::handle<HeaderSection_FileSchema>& ent) const
{

  // --- own field : schemaIdentifiers ---

  SW.OpenSub();
  for (int i1 = 1; i1 <= ent->NbSchemaIdentifiers(); i1++)
  {
    SW.Send(ent->SchemaIdentifiersValue(i1));
  }
  SW.CloseSub();
}
