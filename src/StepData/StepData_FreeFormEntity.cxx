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


#include <Interface_Macros.hxx>
#include <Standard_Type.hxx>
#include <StepData_Field.hxx>
#include <StepData_FreeFormEntity.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_FreeFormEntity,Standard_Transient)

void StepData_FreeFormEntity::SetStepType (const Standard_CString typenam)
      {  thetype.Clear();  thetype.AssignCat (typenam);  }

    Standard_CString  StepData_FreeFormEntity::StepType () const
      {  return thetype.ToCString();  }

    void  StepData_FreeFormEntity::SetNext
  (const Handle(StepData_FreeFormEntity)& next, const Standard_Boolean last)
{
  if (next.IsNull()) thenext.Nullify();
  else if (thenext.IsNull()) thenext = next;
  else if (last) thenext->SetNext(next);
  else {
    next->SetNext(thenext,last);
    thenext = next;
  }
}

    Handle(StepData_FreeFormEntity)  StepData_FreeFormEntity::Next () const
      {  return thenext;  }

    Standard_Boolean  StepData_FreeFormEntity::IsComplex () const
      {  return (!thenext.IsNull());  }

    Handle(StepData_FreeFormEntity)  StepData_FreeFormEntity::Typed
  (const Standard_CString typenam) const
{
  Handle(StepData_FreeFormEntity) res;
  if (thetype.IsEqual (typenam)) return this;
  if (thenext.IsNull()) return res;
  return thenext->Typed (typenam);
}

    Handle(TColStd_HSequenceOfAsciiString)  StepData_FreeFormEntity::TypeList
  () const
{
  Handle(TColStd_HSequenceOfAsciiString) li = new TColStd_HSequenceOfAsciiString();
  li->Append (thetype);
  Handle(StepData_FreeFormEntity) next = thenext;
  while (!next.IsNull()) {
    li->Append (TCollection_AsciiString (next->StepType()) );
    next = next->Next();
  }
  return li;
}

    Standard_Boolean  StepData_FreeFormEntity::Reorder
  (Handle(StepData_FreeFormEntity)& ent)
{
  if (ent.IsNull()) return Standard_False;
  if (!ent->IsComplex()) return Standard_False;
  Standard_Boolean afr = Standard_False;
  Handle(StepData_FreeFormEntity) e1 = ent;  Handle(StepData_FreeFormEntity) e2 = ent->Next();
  while (!e2.IsNull()) {
    if (strcmp (e1->StepType(), e2->StepType()) > 0) { afr = Standard_True; break; }
    e1 = e2;  e2 = e1->Next();
  }
  if (!afr) return afr;
//  remise en ordre avec un dictionnaire
  e1 = ent;  e2.Nullify();
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> dic;
  while (!e1.IsNull()) {
    dic.Bind(e1->StepType(), e1);
    e1 = e1->Next();
  }
//  d abord effacer les next en cours ...
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>::Iterator iter(dic);
  for (; iter.More(); iter.Next()) {
    e1 = GetCasted(StepData_FreeFormEntity,iter.Value());
    if (!e1.IsNull()) e1->SetNext(e2);
  }
//  ... puis les remettre dans l ordre
  e1.Nullify();
  for (iter.Reset(); iter.More(); iter.Next()) {
    e2 = GetCasted(StepData_FreeFormEntity,iter.Value());
    if (!e1.IsNull()) e1->SetNext(e2);
    e1 = e2;
  }

  ent = e1;
  return afr;
}


    void  StepData_FreeFormEntity::SetNbFields (const Standard_Integer nb)
{
  if (nb <= 0) thefields.Nullify();
  else thefields = new StepData_HArray1OfField (1,nb);
}

    Standard_Integer  StepData_FreeFormEntity::NbFields () const
      {  return  (thefields.IsNull() ? 0 : thefields->Length());  }

    const StepData_Field&  StepData_FreeFormEntity::Field
  (const Standard_Integer num) const
      {  return thefields->Value(num);  }

    StepData_Field&  StepData_FreeFormEntity::CField
  (const Standard_Integer num)
      {  return thefields->ChangeValue(num);  }
