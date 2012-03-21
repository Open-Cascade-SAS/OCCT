// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepData_FreeFormEntity.ixx>
#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <Interface_Macros.hxx>



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
  Handle(Dico_DictionaryOfTransient) dic = new Dico_DictionaryOfTransient;
  while (!e1.IsNull()) {
    dic->SetItem (e1->StepType(), e1);
    e1 = e1->Next();
  }
//  d abord effacer les next en cours ...
  Dico_IteratorOfDictionaryOfTransient iter(dic);
  for (iter.Start(); iter.More(); iter.Next()) {
    e1 = GetCasted(StepData_FreeFormEntity,iter.Value());
    if (!e1.IsNull()) e1->SetNext(e2);
  }
//  ... puis les remettre dans l ordre
  e1.Nullify();
  for (iter.Start(); iter.More(); iter.Next()) {
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
