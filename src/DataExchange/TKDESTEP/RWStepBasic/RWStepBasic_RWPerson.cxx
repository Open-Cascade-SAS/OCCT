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

#include <Interface_Check.hxx>
#include "RWStepBasic_RWPerson.pxx"
#include <StepBasic_Person.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

RWStepBasic_RWPerson::RWStepBasic_RWPerson() = default;

void RWStepBasic_RWPerson::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                    const int                                   num,
                                    occ::handle<Interface_Check>&               ach,
                                    const occ::handle<StepBasic_Person>&        ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 6, ach, "person"))
    return;

  // --- own field : id ---

  occ::handle<TCollection_HAsciiString> aId;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "id", ach, aId);

  // --- own field : lastName ---

  occ::handle<TCollection_HAsciiString> aLastName;
  bool                                  hasAlastName = true;
  if (data->IsParamDefined(num, 2))
  {
    // szv#4:S4163:12Mar99 `bool stat2 =` not needed
    data->ReadString(num, 2, "last_name", ach, aLastName);
  }
  else
  {
    hasAlastName = false;
    aLastName.Nullify();
  }

  // --- own field : firstName ---

  occ::handle<TCollection_HAsciiString> aFirstName;
  bool                                  hasAfirstName = true;
  if (data->IsParamDefined(num, 3))
  {
    // szv#4:S4163:12Mar99 `bool stat3 =` not needed
    data->ReadString(num, 3, "first_name", ach, aFirstName);
  }
  else
  {
    hasAfirstName = false;
    aFirstName.Nullify();
  }

  // --- own field : middleNames ---

  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> aMiddleNames;
  bool                                                                    hasAmiddleNames = true;
  if (data->IsParamDefined(num, 4))
  {
    occ::handle<TCollection_HAsciiString> aMiddleNamesItem;
    int                                   nsub4;
    if (data->ReadSubList(num, 4, "middle_names", ach, nsub4))
    {
      int nb4      = data->NbParams(nsub4);
      aMiddleNames = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nb4);
      for (int i4 = 1; i4 <= nb4; i4++)
      {
        // szv#4:S4163:12Mar99 `bool stat4 =` not needed
        if (data->ReadString(nsub4, i4, "middle_names", ach, aMiddleNamesItem))
          aMiddleNames->SetValue(i4, aMiddleNamesItem);
      }
    }
  }
  else
  {
    hasAmiddleNames = false;
    aMiddleNames.Nullify();
  }

  // --- own field : prefixTitles ---

  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> aPrefixTitles;
  bool                                                                    hasAprefixTitles = true;
  if (data->IsParamDefined(num, 5))
  {
    occ::handle<TCollection_HAsciiString> aPrefixTitlesItem;
    int                                   nsub5;
    if (data->ReadSubList(num, 5, "prefix_titles", ach, nsub5))
    {
      int nb5       = data->NbParams(nsub5);
      aPrefixTitles = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nb5);
      for (int i5 = 1; i5 <= nb5; i5++)
      {
        // szv#4:S4163:12Mar99 `bool stat5 =` not needed
        if (data->ReadString(nsub5, i5, "prefix_titles", ach, aPrefixTitlesItem))
          aPrefixTitles->SetValue(i5, aPrefixTitlesItem);
      }
    }
  }
  else
  {
    hasAprefixTitles = false;
    aPrefixTitles.Nullify();
  }

  // --- own field : suffixTitles ---

  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> aSuffixTitles;
  bool                                                                    hasAsuffixTitles = true;
  if (data->IsParamDefined(num, 6))
  {
    occ::handle<TCollection_HAsciiString> aSuffixTitlesItem;
    int                                   nsub6;
    if (data->ReadSubList(num, 6, "suffix_titles", ach, nsub6))
    {
      int nb6       = data->NbParams(nsub6);
      aSuffixTitles = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, nb6);
      for (int i6 = 1; i6 <= nb6; i6++)
      {
        // szv#4:S4163:12Mar99 `bool stat6 =` not needed
        if (data->ReadString(nsub6, i6, "suffix_titles", ach, aSuffixTitlesItem))
          aSuffixTitles->SetValue(i6, aSuffixTitlesItem);
      }
    }
  }
  else
  {
    hasAsuffixTitles = false;
    aSuffixTitles.Nullify();
  }

  //--- Initialisation of the read entity ---

  ent->Init(aId,
            hasAlastName,
            aLastName,
            hasAfirstName,
            aFirstName,
            hasAmiddleNames,
            aMiddleNames,
            hasAprefixTitles,
            aPrefixTitles,
            hasAsuffixTitles,
            aSuffixTitles);
}

void RWStepBasic_RWPerson::WriteStep(StepData_StepWriter&                 SW,
                                     const occ::handle<StepBasic_Person>& ent) const
{

  // --- own field : id ---

  SW.Send(ent->Id());

  // --- own field : lastName ---

  bool hasAlastName = ent->HasLastName();
  if (hasAlastName)
  {
    SW.Send(ent->LastName());
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : firstName ---

  bool hasAfirstName = ent->HasFirstName();
  if (hasAfirstName)
  {
    SW.Send(ent->FirstName());
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : middleNames ---

  bool hasAmiddleNames = ent->HasMiddleNames();
  if (hasAmiddleNames)
  {
    SW.OpenSub();
    for (int i4 = 1; i4 <= ent->NbMiddleNames(); i4++)
    {
      SW.Send(ent->MiddleNamesValue(i4));
    }
    SW.CloseSub();
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : prefixTitles ---

  bool hasAprefixTitles = ent->HasPrefixTitles();
  if (hasAprefixTitles)
  {
    SW.OpenSub();
    for (int i5 = 1; i5 <= ent->NbPrefixTitles(); i5++)
    {
      SW.Send(ent->PrefixTitlesValue(i5));
    }
    SW.CloseSub();
  }
  else
  {
    SW.SendUndef();
  }

  // --- own field : suffixTitles ---

  bool hasAsuffixTitles = ent->HasSuffixTitles();
  if (hasAsuffixTitles)
  {
    SW.OpenSub();
    for (int i6 = 1; i6 <= ent->NbSuffixTitles(); i6++)
    {
      SW.Send(ent->SuffixTitlesValue(i6));
    }
    SW.CloseSub();
  }
  else
  {
    SW.SendUndef();
  }
}
