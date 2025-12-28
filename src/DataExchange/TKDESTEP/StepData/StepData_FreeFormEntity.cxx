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
#include <Standard_Type.hxx>
#include <StepData_FreeFormEntity.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_FreeFormEntity, Standard_Transient)

void StepData_FreeFormEntity::SetStepType(const char* typenam)
{
  // Set the STEP entity type name for this free-form entity
  thetype.Clear();
  thetype.AssignCat(typenam);
}

const char* StepData_FreeFormEntity::StepType() const
{
  return thetype.ToCString();
}

void StepData_FreeFormEntity::SetNext(const occ::handle<StepData_FreeFormEntity>& next,
                                      const bool                 last)
{
  if (next.IsNull())
    thenext.Nullify();
  else if (thenext.IsNull())
    thenext = next;
  else if (last)
    thenext->SetNext(next);
  else
  {
    next->SetNext(thenext, last);
    thenext = next;
  }
}

occ::handle<StepData_FreeFormEntity> StepData_FreeFormEntity::Next() const
{
  return thenext;
}

bool StepData_FreeFormEntity::IsComplex() const
{
  // A complex entity is one that has additional entity parts linked via 'next'
  return (!thenext.IsNull());
}

occ::handle<StepData_FreeFormEntity> StepData_FreeFormEntity::Typed(const char* typenam) const
{
  occ::handle<StepData_FreeFormEntity> res;
  if (thetype.IsEqual(typenam))
    return this;
  if (thenext.IsNull())
    return res;
  return thenext->Typed(typenam);
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> StepData_FreeFormEntity::TypeList() const
{
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> li = new NCollection_HSequence<TCollection_AsciiString>();
  li->Append(thetype);
  occ::handle<StepData_FreeFormEntity> next = thenext;
  while (!next.IsNull())
  {
    li->Append(TCollection_AsciiString(next->StepType()));
    next = next->Next();
  }
  return li;
}

bool StepData_FreeFormEntity::Reorder(occ::handle<StepData_FreeFormEntity>& ent)
{
  // Reorder complex entities to ensure alphabetical sorting of entity types
  if (ent.IsNull())
    return false;
  if (!ent->IsComplex())
    return false;
  bool                afr = false; // flag: any reordering needed
  occ::handle<StepData_FreeFormEntity> e1  = ent;
  occ::handle<StepData_FreeFormEntity> e2  = ent->Next();
  // Check if entities are already in alphabetical order
  while (!e2.IsNull())
  {
    if (strcmp(e1->StepType(), e2->StepType()) > 0)
    {
      afr = true; // Found out-of-order pair
      break;
    }
    e1 = e2;
    e2 = e1->Next();
  }
  if (!afr)
    return afr;
  //  Reordering using a dictionary (map) to sort entity types alphabetically
  e1 = ent;
  e2.Nullify();
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>> dic;
  while (!e1.IsNull())
  {
    dic.Bind(e1->StepType(), e1);
    e1 = e1->Next();
  }
  //  First clear the current 'next' links to break the chain...
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator iter(dic);
       iter.More();
       iter.Next())
  {
    e1 = GetCasted(StepData_FreeFormEntity, iter.Value());
    if (!e1.IsNull())
      e1->SetNext(e2);
  }
  //  ... then rebuild the chain in alphabetical order
  e1.Nullify();
  for (NCollection_DataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator iter(dic);
       iter.More();
       iter.Next())
  {
    e2 = GetCasted(StepData_FreeFormEntity, iter.Value());
    if (!e1.IsNull())
      e1->SetNext(e2);
    e1 = e2;
  }

  ent = e1;
  return afr;
}

void StepData_FreeFormEntity::SetNbFields(const int nb)
{
  // Initialize the array of fields for this entity
  if (nb <= 0)
    thefields.Nullify();
  else
    thefields = new NCollection_HArray1<StepData_Field>(1, nb);
}

int StepData_FreeFormEntity::NbFields() const
{
  return (thefields.IsNull() ? 0 : thefields->Length());
}

const StepData_Field& StepData_FreeFormEntity::Field(const int num) const
{
  return thefields->Value(num);
}

StepData_Field& StepData_FreeFormEntity::CField(const int num)
{
  return thefields->ChangeValue(num);
}
