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


#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepData_Field.hxx>
#include <StepData_SelectInt.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_SelectReal.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_HArray2OfTransient.hxx>

//  Le kind code le type de donnee, le mode d acces (direct ou via Select),
//  l arite (simple, liste, carre)
//  Valeurs pour Kind : 0 = Clear/Undefined
//  KindInteger KindBoolean KindLogical KindEnum KindReal KindString KindEntity
//  + KindSelect qui s y substitue et peut s y combiner
//  + KindList et KindList2  qui peuvent s y combiner
//  (sur masque KindArity et decalage ShiftArity)
#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum    4
#define KindReal    5
#define KindString  6
#define KindEntity  7
#define KindAny     8
#define KindDerived 9

#define KindType    15
#define KindSelect  16
#define KindArity   192
#define KindList    64
#define KindList2   128
#define ShiftArity  6

static Standard_Integer TrueKind (const Standard_Integer kind)
{  return (kind & KindType);  }


    StepData_Field::StepData_Field ()    {  Clear();  }

    StepData_Field::StepData_Field
  (const StepData_Field& other, const Standard_Boolean copy)
{
  if (copy)  {  CopyFrom(other);  return;  }
  thekind = other.Kind(Standard_False);  theint = other.Int();
  thereal = other.Real();  theany = other.Transient();
}

    void  StepData_Field::CopyFrom (const StepData_Field& other)
{
  thekind = other.Kind(Standard_False);  theint = other.Int();
  thereal = other.Real();  theany = other.Transient();
  if (thekind == KindString || thekind == KindEnum) {
    DeclareAndCast(TCollection_HAsciiString,str,theany);
    if (!str.IsNull()) theany = new TCollection_HAsciiString (str->ToCString());
    return;
  }
  if (thekind == KindSelect) {
//  Differents cas
    DeclareAndCast(StepData_SelectReal,sr,theany);
    if (!sr.IsNull()) {
      Standard_Real val = sr->Real();
      sr = new StepData_SelectReal;  sr->SetReal(val);
      theany = sr;  return;
    }
    DeclareAndCast(StepData_SelectInt,si,theany);
    if (!si.IsNull()) {
      Standard_Integer ival = si->Int(), ik = si->Kind();
      si = new StepData_SelectInt;  si->SetKind(ik);  si->SetInt(ival);
      theany = si;  return;
    }
    DeclareAndCast(StepData_SelectNamed,sn,theany);
    if (!sn.IsNull()) {
      Handle(StepData_SelectNamed) sn2 = new StepData_SelectNamed;
      if (sn->HasName()) sn2->SetName(sn2->Name());
      sn2->CField().CopyFrom(*this);
      theany = sn2;  return;
    }
  }
//    Les listes ...
  if ((thekind & KindArity) == KindList)  {
    Standard_Integer i, low, up;
    DeclareAndCast(TColStd_HArray1OfInteger,hi,theany);
    if (!hi.IsNull()) {
      low = hi->Lower();  up = hi->Upper();
      Handle(TColStd_HArray1OfInteger) hi2 = new TColStd_HArray1OfInteger (low,up);
      for (i = low; i <= up; i ++) hi2->SetValue (i,hi->Value(i));
      return;
    }
    DeclareAndCast(TColStd_HArray1OfReal,hr,theany);
    if (!hr.IsNull()) {
      low = hr->Lower();  up = hr->Upper();
      Handle(TColStd_HArray1OfReal) hr2 = new TColStd_HArray1OfReal (low,up);
      for (i = low; i <= up; i ++) hr2->SetValue (i,hr->Value(i));
      return;
    }
    DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
    if (!hs.IsNull()) {
      low = hs->Lower();  up = hs->Upper();
      Handle(Interface_HArray1OfHAsciiString) hs2 = new Interface_HArray1OfHAsciiString (low,up);
      for (i = low; i <= up; i ++) hs2->SetValue (i,new TCollection_HAsciiString(hs->Value(i)));
      return;
    }
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (!ht.IsNull()) {
      low = ht->Lower();  up = ht->Upper();
      Handle(TColStd_HArray1OfTransient) ht2 = new TColStd_HArray1OfTransient (low,up);
//  faudrait reprendre les cas SelectMember ...
      for (i = low; i <= up; i ++) ht2->SetValue (i,ht->Value(i));
      return;
    }
  }
//    Reste la liste 2 ...
//  if ((thekind & KindArity) == KindList2) {
//    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
//  }
}


    void  StepData_Field::Clear (const Standard_Integer kind)
{
  thekind = kind;
  theint = 0; thereal = 0.;  theany.Nullify();
}

    void  StepData_Field::SetDerived ()
      {  Clear(KindDerived);  }

    void  StepData_Field::SetInt (const Standard_Integer val)
{
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  {  sm->SetInteger(val);  return;  }
  }
  if (thekind == KindInteger || thekind == KindBoolean ||
      thekind == KindLogical || thekind == KindEnum)  theint = val;
//  else ?
}

    void  StepData_Field::SetInteger (const Standard_Integer val)
{
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  {  sm->SetInteger(val);  return;  }
  }
  Clear(KindInteger);
  theint = val;
}

    void  StepData_Field::SetBoolean (const Standard_Boolean val)
{
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  {  sm->SetBoolean(val);  return;  }
  }
  Clear(KindBoolean);
  theint = (val ? 1 : 0);
}

    void  StepData_Field::SetLogical (const StepData_Logical val)
{
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  {  sm->SetLogical(val);  return;  }
  }
  Clear(KindLogical);
  if (val == StepData_LFalse)   theint = 0;
  if (val == StepData_LTrue)    theint = 1;
  if (val == StepData_LUnknown) theint = 2;
}

    void  StepData_Field::SetReal (const Standard_Real val)
{
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  {  sm->SetReal(val);  return;  }
  }
  Clear(KindReal);
  thereal = val;
}

    void  StepData_Field::SetString (const Standard_CString val)
{
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  {  sm->SetString (val);  return;  }
  }
  if (thekind != KindEnum) Clear(KindString);
  theany = new TCollection_HAsciiString(val);
}

    void  StepData_Field::SetEnum
  (const Standard_Integer val, const Standard_CString text)
{
  Clear(KindEnum);
  SetInt(val);
  if (text && text[0] != '\0') SetString(text);
}

    void  StepData_Field::SetSelectMember
  (const Handle(StepData_SelectMember)& val)
{
  if (val.IsNull()) return;
  Clear (KindSelect);
  theany = val;
}

    void  StepData_Field::SetEntity (const Handle(Standard_Transient)& val)
      {  Clear(KindEntity);  theany = val;  }

    void  StepData_Field::SetEntity ()
      {  Handle(Standard_Transient) nulent;  SetEntity(nulent);  }

    void  StepData_Field::SetList
  (const Standard_Integer size, const Standard_Integer first)
{
//  ATTENTION, on ne traite pas l agrandissement ...

  theint = size;  thereal = 0.0;  theany.Nullify();  // ?? agrandissement ??
  switch (thekind) {
  case KindInteger :
  case KindBoolean :
  case KindLogical : theany = new TColStd_HArray1OfInteger(first,first+size-1);
		     break;
  case KindReal    : theany = new TColStd_HArray1OfReal (first,first+size-1);
		     break;
  case KindEnum    :
  case KindString  : theany = new Interface_HArray1OfHAsciiString (first,first+size-1);
		     break;
//  default : en particulier si "non specifie" (any)
  default          : theany = new TColStd_HArray1OfTransient(first,first+size-1);
  }
  if (thekind == 0) thekind = KindAny;
  thekind |= KindList;
}

    void  StepData_Field::SetList2
  (const Standard_Integer siz1, const Standard_Integer siz2,
   const Standard_Integer f1,   const Standard_Integer f2)
{
//  ATTENTION, on ne traite pas l agrandissement ...

  theint = siz1;  thereal = Standard_Real(siz2);  theany.Nullify();
  Standard_Integer kind = thekind;
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  kind = sm->Kind();
  }
  switch (kind) {
  case KindInteger :
  case KindBoolean :
  case KindLogical : theany = new TColStd_HArray2OfInteger(f1,f1+siz1-1,f2,f2+siz2-1);
		     break;
  case KindReal    : theany = new TColStd_HArray2OfReal (f1,f1+siz1-1,f2,f2+siz2-1);
		     break;
  case KindEnum    :
  case KindString  : theany = new TColStd_HArray2OfTransient (f1,f1+siz1-1,f2,f2+siz2-1);
		     break;
//  default : en particulier si "non specifie" (any)
  default          : theany = new TColStd_HArray2OfTransient(f1,f1+siz1-1,f2,f2+siz2-1);
  }
  if (thekind == 0) thekind = KindAny;
  thekind |= KindList2;
}


    void  StepData_Field::Set (const Handle(Standard_Transient)& val)
{
  Standard_Integer kind = thekind;
  Clear();  theany = val;
  if (val.IsNull())  return;
  if (val->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
    {  thekind = KindString;  return;  }
  DeclareAndCast(StepData_SelectMember,sm,val);
  if (!sm.IsNull())  {  thekind = KindSelect;  return;  }
  DeclareAndCast(TColStd_HArray1OfInteger,hi,val);
  if (!hi.IsNull())
    {  if (kind == 0) kind = KindInteger;
       thekind = kind        | KindList;  theint = hi->Length();  return;  }
  DeclareAndCast(TColStd_HArray1OfReal,hr,val);
  if (!hr.IsNull())
    {  thekind = KindReal    | KindList;  theint = hr->Length();  return;  }
  DeclareAndCast(Interface_HArray1OfHAsciiString,hs,val);
  if (!hs.IsNull())
    {  thekind = KindString  | KindList;  theint = hs->Length();  return;  }
  DeclareAndCast(TColStd_HArray1OfTransient,ht,val);
  if (!ht.IsNull())
    {  if (kind == 0) kind = KindAny;
       thekind = kind        | KindList;  theint = ht->Length();  return;  }
  DeclareAndCast(TColStd_HArray2OfInteger,hi2,val);
  if (!hi2.IsNull())
    {  if (kind == 0) kind = KindInteger;
       thekind = kind        | KindList2;  theint = hi2->ColLength();
       thereal = Standard_Real(hi2->RowLength());  return;  }
  DeclareAndCast(TColStd_HArray2OfReal,hr2,val);
  if (!hr2.IsNull())
    {  thekind = KindInteger | KindList2;  theint = hr2->ColLength();
       thereal = Standard_Real(hi2->RowLength());  return;  }
  DeclareAndCast(TColStd_HArray2OfTransient,ht2,val);
  if (!ht2.IsNull())
    {  if (kind == 0) kind = KindAny;
       thekind = kind        | KindList2;  theint = ht2->ColLength();
       thereal = Standard_Real(hi2->RowLength());  return;  }
}


    void  StepData_Field::ClearItem (const Standard_Integer num)
{
  DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
  if (!ht.IsNull()) ht->ChangeValue(num).Nullify();
  DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
  if (!hs.IsNull()) hs->ChangeValue(num).Nullify();
}

    void  StepData_Field::SetInt
  (const Standard_Integer num, const Standard_Integer val, const Standard_Integer kind)
{
  DeclareAndCast(TColStd_HArray1OfInteger,hi,theany);
  if (!hi.IsNull())  {  hi->SetValue(num,val);  return;  }
//   Si deja commence sur autre chose, changer et mettre des select
  DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
  if (ht.IsNull()) return;  // yena erreur, ou alors OfReal
  thekind = KindAny | KindList;
  DeclareAndCast(StepData_SelectMember,sm,ht->Value(num));
  if (sm.IsNull())  {  sm = new StepData_SelectInt;  ht->SetValue(num,sm);  }
  sm->SetKind(kind);  sm->SetInt (val);
}

    void  StepData_Field::SetInteger
  (const Standard_Integer num, const Standard_Integer val)
      {  SetInt (num,val,KindInteger);  }

    void  StepData_Field::SetBoolean
  (const Standard_Integer num, const Standard_Boolean val)
      {  SetInt (num, (val ? 1 : 0),KindBoolean);  }

    void  StepData_Field::SetLogical
  (const Standard_Integer num, const StepData_Logical val)
{
  if (val == StepData_LFalse)   SetInt (num, 0,KindLogical);
  if (val == StepData_LTrue)    SetInt (num, 1,KindLogical);
  if (val == StepData_LUnknown) SetInt (num, 2,KindLogical);
}

    void  StepData_Field::SetEnum
  (const Standard_Integer num, const Standard_Integer val, const Standard_CString text)
{
  DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
  if (ht.IsNull()) { SetInteger (num,val); return; }
  DeclareAndCast(StepData_SelectMember,sm,ht->Value(num));
  thekind = KindAny | KindList;
  if (sm.IsNull()) {  sm = new StepData_SelectNamed;  ht->SetValue(num,sm);  }
  sm->SetEnum (val,text);
}

    void  StepData_Field::SetReal
  (const Standard_Integer num, const Standard_Real val)
{
  DeclareAndCast(TColStd_HArray1OfReal,hr,theany);
  if (!hr.IsNull())  {  hr->SetValue(num,val);  return;  }
//   Si deja commence sur autre chose, changer et mettre des select
  DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
  if (ht.IsNull()) return;  // yena erreur, ou alors OfInteger
  thekind = KindAny | KindList;
  DeclareAndCast(StepData_SelectMember,sm,ht->Value(num));
  if (sm.IsNull())  {  sm = new StepData_SelectReal;  ht->SetValue(num,sm);  }
  sm->SetReal (val);
}


    void  StepData_Field::SetString
  (const Standard_Integer num, const Standard_CString val)
{
  DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
  if (!hs.IsNull()) { hs->SetValue (num,new TCollection_HAsciiString(val)); return; }
//    et si OfInteger ou OfReal ?
  DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
  if ( ht.IsNull()) return;
  thekind = KindAny | KindList;
  ht->SetValue (num,new TCollection_HAsciiString(val));
}


    void  StepData_Field::SetEntity
  (const Standard_Integer num, const Handle(Standard_Transient)& val)
{
  DeclareAndCast(TColStd_HArray1OfTransient,aHt,theany);
  if (!aHt.IsNull()) { aHt->SetValue (num,val); return; }
  DeclareAndCast(TColStd_HArray1OfInteger,hi,theany);
  if (!hi.IsNull()) {
    Standard_Integer low = hi->Lower(), up = hi->Upper();
    Handle(TColStd_HArray1OfTransient) ht = new TColStd_HArray1OfTransient(low,up);
    Handle(StepData_SelectMember) sm;
    Standard_Integer kind = Kind();
    for (Standard_Integer i = low; i <= up; i ++) {
      if (i == num) ht->SetValue(i,val);
      else {
	sm = new StepData_SelectInt;
	sm->SetKind(kind); sm->SetInt(hi->Value(i));
	ht->SetValue(i,sm);
      }
    }
    thekind = KindAny | KindList;
    return;
  }
  DeclareAndCast(TColStd_HArray1OfReal,hr,theany);
  if (!hr.IsNull()) {
    Standard_Integer low = hr->Lower(), up = hr->Upper();
    Handle(TColStd_HArray1OfTransient) ht = new TColStd_HArray1OfTransient(low,up);
    Handle(StepData_SelectMember) sm;
    for (Standard_Integer i = low; i <= up; i ++) {
      if (i == num) ht->SetValue(i,val);
      else {
	sm = new StepData_SelectReal;
	sm->SetReal(hr->Value(i));
	ht->SetValue(i,sm);
      }
    }
    thekind = KindAny | KindList;
    return;
  }
  DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
  if (!hs.IsNull()) {
    Standard_Integer low = hs->Lower(), up = hs->Upper();
    Handle(TColStd_HArray1OfTransient) ht = new TColStd_HArray1OfTransient(low,up);
    for (Standard_Integer i = low; i <= up; i ++) {
      if (i == num) ht->SetValue(i,val);
      else ht->SetValue(i,hs->Value(i));
    }
    thekind = KindAny | KindList;
    return;
  }
}


//     QUERIES

    Standard_Boolean  StepData_Field::IsSet
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  if (thekind == 0) return Standard_False;
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (sm.IsNull()) return Standard_False;
    return (sm->Kind() != 0);
  }
  if ((thekind & KindArity) == KindList) {
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (!ht.IsNull()) return (!ht->Value(n1).IsNull());
    DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
    if (!hs.IsNull()) return (!hs->Value(n1).IsNull());
  }
  if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if (!ht.IsNull()) return (!ht->Value(n1,n2).IsNull());
  }
  return Standard_True;
}


    Standard_Integer  StepData_Field::ItemKind
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  if ((thekind & KindArity) == 0) return Kind(Standard_True);
  Standard_Integer kind = TrueKind(thekind);    // si Any, evaluer ...
  if (kind != KindAny) return kind;
//  Sinon, chercher un Transient
  Handle(Standard_Transient) item;
  if ((thekind & KindArity) == KindList) {
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (!ht.IsNull()) return kind;
    item = ht->Value(n1);
  } else if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if (!ht.IsNull()) return kind;
    item = ht->Value(n1,n2);
  }
  if (item.IsNull()) return 0;
  if (item->IsKind(STANDARD_TYPE(TCollection_HAsciiString))) return KindString;
  DeclareAndCast(StepData_SelectMember,sm,item);
  if (sm.IsNull()) return KindEntity;
  return sm->Kind();
}

    Standard_Integer  StepData_Field::Kind (const Standard_Boolean type) const
{
  if (!type) return thekind;
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull())  return TrueKind(sm->Kind());
  }
  return TrueKind (thekind);
}


    Standard_Integer  StepData_Field::Arity () const
      {  return (thekind & KindArity) >> ShiftArity;  }

    Standard_Integer  StepData_Field::Length (const Standard_Integer index) const
{
  if ((thekind & KindArity) == KindList)  return theint;
  if ((thekind & KindArity) == KindList2) {
    if (index == 2) return Standard_Integer (thereal);
    else return theint;
  }
  return 0;
}

    Standard_Integer  StepData_Field::Lower (const Standard_Integer index) const
{
  if ((thekind & KindArity) == KindList)  {
    DeclareAndCast(TColStd_HArray1OfInteger,hi,theany);
    if (!hi.IsNull()) return hi->Lower();
    DeclareAndCast(TColStd_HArray1OfReal,hr,theany);
    if (!hr.IsNull()) return hr->Lower();
    DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
    if (!hs.IsNull()) return hs->Lower();
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (!ht.IsNull()) return ht->Lower();
  }
  if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if ( ht.IsNull()) return 0;
    if (index == 1) return ht->LowerCol();
    if (index == 2) return ht->LowerRow();
  }
  return 0;
}

    Standard_Integer  StepData_Field::Int () const
      {  return theint;  }

    Standard_Integer  StepData_Field::Integer
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  if ((thekind & KindArity) == 0) {
    if (thekind == KindSelect) {
      DeclareAndCast(StepData_SelectMember,sm,theany);
      if (!sm.IsNull()) return sm->Int();
    }
    return theint;
  }
  if ((thekind & KindArity) == KindList) {
    DeclareAndCast(TColStd_HArray1OfInteger,hi,theany);
    if (!hi.IsNull()) return hi->Value(n1);
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (ht.IsNull()) return 0;
    DeclareAndCast(StepData_SelectMember,sm,ht->Value(n1));
    if (!sm.IsNull()) return sm->Int();
  }
  if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if (ht.IsNull()) return 0;
    DeclareAndCast(StepData_SelectMember,sm,ht->Value(n1,n2));
    if (!sm.IsNull()) return sm->Int();
  }
  return 0;
}

    Standard_Boolean  StepData_Field::Boolean
  (const Standard_Integer n1, const Standard_Integer n2) const
      {  return (Integer(n1,n2) > 0);  }

    StepData_Logical  StepData_Field::Logical
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  Standard_Integer ival = Integer(n1,n2);
  if (ival == 0) return StepData_LFalse;
  if (ival == 1) return StepData_LTrue;
  return StepData_LUnknown;
}

    Standard_Real     StepData_Field::Real
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  if ((thekind & KindArity) == 0) {
    if (thekind == KindSelect) {
      DeclareAndCast(StepData_SelectMember,sm,theany);
      if (!sm.IsNull()) return sm->Real();
    }
    return thereal;
  }
  if ((thekind & KindArity) == KindList) {
    DeclareAndCast(TColStd_HArray1OfReal,hr,theany);
    if (!hr.IsNull()) return hr->Value(n1);
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (ht.IsNull()) return 0;
    DeclareAndCast(StepData_SelectMember,sm,ht->Value(n1));
    if (!sm.IsNull()) return sm->Real();
  }
  if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfReal,hr,theany);
    if (!hr.IsNull()) return hr->Value(n1,n2);
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if (ht.IsNull()) return 0;
    DeclareAndCast(StepData_SelectMember,sm,ht->Value(n1,n2));
    if (!sm.IsNull()) return sm->Int();
  }
  return 0.0;
}

    Standard_CString  StepData_Field::String
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  if (thekind == KindString || thekind == KindEnum) {
    DeclareAndCast(TCollection_HAsciiString,str,theany);
    if (!str.IsNull()) return str->ToCString();
    else return "";
  }
  if (thekind == KindSelect) {
    DeclareAndCast(StepData_SelectMember,sm,theany);
    if (!sm.IsNull()) return sm->String();
  }
  if ((thekind & KindArity) == KindList) {
    DeclareAndCast(Interface_HArray1OfHAsciiString,hs,theany);
    if (!hs.IsNull()) {
      if (hs->Value(n1).IsNull()) return "";
      else return hs->Value(n1)->ToCString();
    }
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (ht.IsNull()) return "";
    DeclareAndCast(TCollection_HAsciiString,str,ht->Value(n1));
    if (!str.IsNull()) return str->ToCString();
    DeclareAndCast(StepData_SelectMember,sm,ht->Value(n1));
    if (!sm.IsNull()) return sm->String();
  }
  if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if (ht.IsNull()) return "";
    DeclareAndCast(TCollection_HAsciiString,str,ht->Value(n1,n2));
    if (!str.IsNull()) return str->ToCString();
    DeclareAndCast(StepData_SelectMember,sm,ht->Value(n1,n2));
    if (!sm.IsNull()) return sm->String();
  }
  return "";
}


    Standard_Integer  StepData_Field::Enum
  (const Standard_Integer n1, const Standard_Integer n2) const
      {  return Integer(n1,n2);  }

    Standard_CString  StepData_Field::EnumText
  (const Standard_Integer n1, const Standard_Integer n2) const
      {  return String (n1,n2);  }

    Handle(Standard_Transient)  StepData_Field::Entity
  (const Standard_Integer n1, const Standard_Integer n2) const
{
  Handle(Standard_Transient) nulval;  // null handle
  if ((thekind & KindArity) == 0) {
    if (thekind == KindEntity) return theany;
    return nulval;
  }
  if ((thekind & KindArity) == KindList) {
    DeclareAndCast(TColStd_HArray1OfTransient,ht,theany);
    if (ht.IsNull()) return nulval;
    nulval = ht->Value(n1);
    if (nulval.IsNull()) return nulval;
    if (nulval->IsKind(STANDARD_TYPE(StepData_SelectMember)) ||
	nulval->IsKind(STANDARD_TYPE(TCollection_HAsciiString)) )
      nulval.Nullify();
    return nulval;
  }
  if ((thekind & KindArity) == KindList2) {
    DeclareAndCast(TColStd_HArray2OfTransient,ht,theany);
    if (ht.IsNull()) return nulval;
    nulval = ht->Value(n1,n2);
    if (nulval.IsNull()) return nulval;
    if (nulval->IsKind(STANDARD_TYPE(StepData_SelectMember))
	|| nulval->IsKind(STANDARD_TYPE(TCollection_HAsciiString)) )
      nulval.Nullify();
    return nulval;
  }
  return nulval;
}

    Handle(Standard_Transient)  StepData_Field::Transient () const
      {  return theany;  }
