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

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_Transient.hxx>
#include <StepData_Field.hxx>
#include <StepData_SelectInt.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_SelectReal.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//  The kind encodes the data type, access mode (direct or via Select),
//  and arity (simple, list, square array)
//  Values for Kind: 0 = Clear/Undefined
//  KindInteger KindBoolean KindLogical KindEnum KindReal KindString KindEntity
//  + KindSelect which substitutes and can combine with them
//  + KindList and KindList2 which can combine with them
//  (on KindArity mask and ShiftArity offset)
#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum 4
#define KindReal 5
#define KindString 6
#define KindEntity 7
#define KindAny 8
#define KindDerived 9

#define KindType 15
#define KindSelect 16
#define KindArity 192
#define KindList 64
#define KindList2 128
#define ShiftArity 6

static int TrueKind(const int kind)
{
  return (kind & KindType);
}

StepData_Field::StepData_Field()
{
  Clear();
}

StepData_Field::StepData_Field(const StepData_Field& other, const bool copy)
{
  if (copy)
  {
    CopyFrom(other);
    return;
  }
  thekind = other.Kind(false);
  theint  = other.Int();
  thereal = other.Real();
  theany  = other.Transient();
}

void StepData_Field::CopyFrom(const StepData_Field& other)
{
  thekind = other.Kind(false);
  theint  = other.Int();
  thereal = other.Real();
  theany  = other.Transient();
  if (thekind == KindString || thekind == KindEnum)
  {
    DeclareAndCast(TCollection_HAsciiString, str, theany);
    if (!str.IsNull())
      theany = new TCollection_HAsciiString(str->ToCString());
    return;
  }
  if (thekind == KindSelect)
  {
    //  Differents cas
    DeclareAndCast(StepData_SelectReal, sr, theany);
    if (!sr.IsNull())
    {
      double val = sr->Real();
      sr                = new StepData_SelectReal;
      sr->SetReal(val);
      theany = sr;
      return;
    }
    DeclareAndCast(StepData_SelectInt, si, theany);
    if (!si.IsNull())
    {
      int ival = si->Int(), ik = si->Kind();
      si = new StepData_SelectInt;
      si->SetKind(ik);
      si->SetInt(ival);
      theany = si;
      return;
    }
    DeclareAndCast(StepData_SelectNamed, sn, theany);
    if (!sn.IsNull())
    {
      occ::handle<StepData_SelectNamed> sn2 = new StepData_SelectNamed;
      if (sn->HasName())
        sn2->SetName(sn2->Name());
      sn2->CField().CopyFrom(*this);
      theany = sn2;
      return;
    }
  }
  //    Les listes ...
  if ((thekind & KindArity) == KindList)
  {
    int i, low, up;
    DeclareAndCast(NCollection_HArray1<int>, hi, theany);
    if (!hi.IsNull())
    {
      low                                  = hi->Lower();
      up                                   = hi->Upper();
      occ::handle<NCollection_HArray1<int>> hi2 = new NCollection_HArray1<int>(low, up);
      for (i = low; i <= up; i++)
        hi2->SetValue(i, hi->Value(i));
      return;
    }
    DeclareAndCast(NCollection_HArray1<double>, hr, theany);
    if (!hr.IsNull())
    {
      low                               = hr->Lower();
      up                                = hr->Upper();
      occ::handle<NCollection_HArray1<double>> hr2 = new NCollection_HArray1<double>(low, up);
      for (i = low; i <= up; i++)
        hr2->SetValue(i, hr->Value(i));
      return;
    }
    DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
    if (!hs.IsNull())
    {
      low                                         = hs->Lower();
      up                                          = hs->Upper();
      occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> hs2 = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(low, up);
      for (i = low; i <= up; i++)
        hs2->SetValue(i, new TCollection_HAsciiString(hs->Value(i)));
      return;
    }
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (!ht.IsNull())
    {
      low                                    = ht->Lower();
      up                                     = ht->Upper();
      occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> ht2 = new NCollection_HArray1<occ::handle<Standard_Transient>>(low, up);
      //  Should handle SelectMember cases...
      for (i = low; i <= up; i++)
        ht2->SetValue(i, ht->Value(i));
      return;
    }
  }
  //    Remains the 2D list...
  //  if ((thekind & KindArity) == KindList2) {
  //    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>,ht,theany);
  //  }
}

void StepData_Field::Clear(const int kind)
{
  thekind = kind;
  theint  = 0;
  thereal = 0.;
  theany.Nullify();
}

void StepData_Field::SetDerived()
{
  Clear(KindDerived);
}

void StepData_Field::SetInt(const int val)
{
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
    {
      sm->SetInteger(val);
      return;
    }
  }
  if (thekind == KindInteger || thekind == KindBoolean || thekind == KindLogical
      || thekind == KindEnum)
    theint = val;
  //  else ?
}

void StepData_Field::SetInteger(const int val)
{
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
    {
      sm->SetInteger(val);
      return;
    }
  }
  Clear(KindInteger);
  theint = val;
}

void StepData_Field::SetBoolean(const bool val)
{
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
    {
      sm->SetBoolean(val);
      return;
    }
  }
  Clear(KindBoolean);
  theint = (val ? 1 : 0);
}

void StepData_Field::SetLogical(const StepData_Logical val)
{
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
    {
      sm->SetLogical(val);
      return;
    }
  }
  Clear(KindLogical);
  if (val == StepData_LFalse)
    theint = 0;
  if (val == StepData_LTrue)
    theint = 1;
  if (val == StepData_LUnknown)
    theint = 2;
}

void StepData_Field::SetReal(const double val)
{
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
    {
      sm->SetReal(val);
      return;
    }
  }
  Clear(KindReal);
  thereal = val;
}

void StepData_Field::SetString(const char* const val)
{
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
    {
      sm->SetString(val);
      return;
    }
  }
  if (thekind != KindEnum)
    Clear(KindString);
  theany = new TCollection_HAsciiString(val);
}

void StepData_Field::SetEnum(const int val, const char* const text)
{
  Clear(KindEnum);
  SetInt(val);
  if (text && text[0] != '\0')
    SetString(text);
}

void StepData_Field::SetSelectMember(const occ::handle<StepData_SelectMember>& val)
{
  if (val.IsNull())
    return;
  Clear(KindSelect);
  theany = val;
}

void StepData_Field::SetEntity(const occ::handle<Standard_Transient>& val)
{
  Clear(KindEntity);
  theany = val;
}

void StepData_Field::SetEntity()
{
  occ::handle<Standard_Transient> nulent;
  SetEntity(nulent);
}

void StepData_Field::SetList(const int size, const int first)
{
  //  WARNING: we don't handle expansion...

  theint  = size;
  thereal = 0.0;
  theany.Nullify(); // ?? agrandissement ??
  switch (thekind)
  {
    case KindInteger:
    case KindBoolean:
    case KindLogical:
      theany = new NCollection_HArray1<int>(first, first + size - 1);
      break;
    case KindReal:
      theany = new NCollection_HArray1<double>(first, first + size - 1);
      break;
    case KindEnum:
    case KindString:
      theany = new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(first, first + size - 1);
      break;
      //  default : en particulier si "non specifie" (any)
    default:
      theany = new NCollection_HArray1<occ::handle<Standard_Transient>>(first, first + size - 1);
  }
  if (thekind == 0)
    thekind = KindAny;
  thekind |= KindList;
}

void StepData_Field::SetList2(const int siz1,
                              const int siz2,
                              const int f1,
                              const int f2)
{
  //  WARNING: we don't handle expansion...

  theint  = siz1;
  thereal = double(siz2);
  theany.Nullify();
  int kind = thekind;
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
      kind = sm->Kind();
  }
  switch (kind)
  {
    case KindInteger:
    case KindBoolean:
    case KindLogical:
      theany = new NCollection_HArray2<int>(f1, f1 + siz1 - 1, f2, f2 + siz2 - 1);
      break;
    case KindReal:
      theany = new NCollection_HArray2<double>(f1, f1 + siz1 - 1, f2, f2 + siz2 - 1);
      break;
    case KindEnum:
    case KindString:
      theany = new NCollection_HArray2<occ::handle<Standard_Transient>>(f1, f1 + siz1 - 1, f2, f2 + siz2 - 1);
      break;
      //  default : en particulier si "non specifie" (any)
    default:
      theany = new NCollection_HArray2<occ::handle<Standard_Transient>>(f1, f1 + siz1 - 1, f2, f2 + siz2 - 1);
  }
  if (thekind == 0)
    thekind = KindAny;
  thekind |= KindList2;
}

void StepData_Field::Set(const occ::handle<Standard_Transient>& val)
{
  int kind = thekind;
  Clear();
  theany = val;
  if (val.IsNull())
    return;
  if (val->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
  {
    thekind = KindString;
    return;
  }
  DeclareAndCast(StepData_SelectMember, sm, val);
  if (!sm.IsNull())
  {
    thekind = KindSelect;
    return;
  }
  DeclareAndCast(NCollection_HArray1<int>, hi, val);
  if (!hi.IsNull())
  {
    if (kind == 0)
      kind = KindInteger;
    thekind = kind | KindList;
    theint  = hi->Length();
    return;
  }
  DeclareAndCast(NCollection_HArray1<double>, hr, val);
  if (!hr.IsNull())
  {
    thekind = KindReal | KindList;
    theint  = hr->Length();
    return;
  }
  DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, val);
  if (!hs.IsNull())
  {
    thekind = KindString | KindList;
    theint  = hs->Length();
    return;
  }
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, val);
  if (!ht.IsNull())
  {
    if (kind == 0)
      kind = KindAny;
    thekind = kind | KindList;
    theint  = ht->Length();
    return;
  }
  DeclareAndCast(NCollection_HArray2<int>, hi2, val);
  if (!hi2.IsNull())
  {
    if (kind == 0)
      kind = KindInteger;
    thekind = kind | KindList2;
    theint  = hi2->ColLength();
    thereal = double(hi2->RowLength());
    return;
  }
  DeclareAndCast(NCollection_HArray2<double>, hr2, val);
  if (!hr2.IsNull())
  {
    thekind = KindInteger | KindList2;
    theint  = hr2->ColLength();
    thereal = double(hi2->RowLength());
    return;
  }
  DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht2, val);
  if (!ht2.IsNull())
  {
    if (kind == 0)
      kind = KindAny;
    thekind = kind | KindList2;
    theint  = ht2->ColLength();
    thereal = double(hi2->RowLength());
    return;
  }
}

void StepData_Field::ClearItem(const int num)
{
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
  if (!ht.IsNull())
    ht->ChangeValue(num).Nullify();
  DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
  if (!hs.IsNull())
    hs->ChangeValue(num).Nullify();
}

void StepData_Field::SetInt(const int num,
                            const int val,
                            const int kind)
{
  DeclareAndCast(NCollection_HArray1<int>, hi, theany);
  if (!hi.IsNull())
  {
    hi->SetValue(num, val);
    return;
  }
  //   If already started with something else, change and put selects
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
  if (ht.IsNull())
    return; // yena erreur, ou alors OfReal
  thekind = KindAny | KindList;
  DeclareAndCast(StepData_SelectMember, sm, ht->Value(num));
  if (sm.IsNull())
  {
    sm = new StepData_SelectInt;
    ht->SetValue(num, sm);
  }
  sm->SetKind(kind);
  sm->SetInt(val);
}

void StepData_Field::SetInteger(const int num, const int val)
{
  SetInt(num, val, KindInteger);
}

void StepData_Field::SetBoolean(const int num, const bool val)
{
  SetInt(num, (val ? 1 : 0), KindBoolean);
}

void StepData_Field::SetLogical(const int num, const StepData_Logical val)
{
  if (val == StepData_LFalse)
    SetInt(num, 0, KindLogical);
  if (val == StepData_LTrue)
    SetInt(num, 1, KindLogical);
  if (val == StepData_LUnknown)
    SetInt(num, 2, KindLogical);
}

void StepData_Field::SetEnum(const int num,
                             const int val,
                             const char* const text)
{
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
  if (ht.IsNull())
  {
    SetInteger(num, val);
    return;
  }
  DeclareAndCast(StepData_SelectMember, sm, ht->Value(num));
  thekind = KindAny | KindList;
  if (sm.IsNull())
  {
    sm = new StepData_SelectNamed;
    ht->SetValue(num, sm);
  }
  sm->SetEnum(val, text);
}

void StepData_Field::SetReal(const int num, const double val)
{
  DeclareAndCast(NCollection_HArray1<double>, hr, theany);
  if (!hr.IsNull())
  {
    hr->SetValue(num, val);
    return;
  }
  //   If already started with something else, change and put selects
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
  if (ht.IsNull())
    return; // yena erreur, ou alors OfInteger
  thekind = KindAny | KindList;
  DeclareAndCast(StepData_SelectMember, sm, ht->Value(num));
  if (sm.IsNull())
  {
    sm = new StepData_SelectReal;
    ht->SetValue(num, sm);
  }
  sm->SetReal(val);
}

void StepData_Field::SetString(const int num, const char* const val)
{
  DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
  if (!hs.IsNull())
  {
    hs->SetValue(num, new TCollection_HAsciiString(val));
    return;
  }
  //    et si OfInteger ou OfReal ?
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
  if (ht.IsNull())
    return;
  thekind = KindAny | KindList;
  ht->SetValue(num, new TCollection_HAsciiString(val));
}

void StepData_Field::SetEntity(const int num, const occ::handle<Standard_Transient>& val)
{
  DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, aHt, theany);
  if (!aHt.IsNull())
  {
    aHt->SetValue(num, val);
    return;
  }
  DeclareAndCast(NCollection_HArray1<int>, hi, theany);
  if (!hi.IsNull())
  {
    int                   low = hi->Lower(), up = hi->Upper();
    occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> ht = new NCollection_HArray1<occ::handle<Standard_Transient>>(low, up);
    occ::handle<StepData_SelectMember>      sm;
    int                   kind = Kind();
    for (int i = low; i <= up; i++)
    {
      if (i == num)
        ht->SetValue(i, val);
      else
      {
        sm = new StepData_SelectInt;
        sm->SetKind(kind);
        sm->SetInt(hi->Value(i));
        ht->SetValue(i, sm);
      }
    }
    thekind = KindAny | KindList;
    return;
  }
  DeclareAndCast(NCollection_HArray1<double>, hr, theany);
  if (!hr.IsNull())
  {
    int                   low = hr->Lower(), up = hr->Upper();
    occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> ht = new NCollection_HArray1<occ::handle<Standard_Transient>>(low, up);
    occ::handle<StepData_SelectMember>      sm;
    for (int i = low; i <= up; i++)
    {
      if (i == num)
        ht->SetValue(i, val);
      else
      {
        sm = new StepData_SelectReal;
        sm->SetReal(hr->Value(i));
        ht->SetValue(i, sm);
      }
    }
    thekind = KindAny | KindList;
    return;
  }
  DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
  if (!hs.IsNull())
  {
    int                   low = hs->Lower(), up = hs->Upper();
    occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> ht = new NCollection_HArray1<occ::handle<Standard_Transient>>(low, up);
    for (int i = low; i <= up; i++)
    {
      if (i == num)
        ht->SetValue(i, val);
      else
        ht->SetValue(i, hs->Value(i));
    }
    thekind = KindAny | KindList;
    return;
  }
}

//     QUERIES

bool StepData_Field::IsSet(const int n1, const int n2) const
{
  if (thekind == 0)
    return false;
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (sm.IsNull())
      return false;
    return (sm->Kind() != 0);
  }
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (!ht.IsNull())
      return (!ht->Value(n1).IsNull());
    DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
    if (!hs.IsNull())
      return (!hs->Value(n1).IsNull());
  }
  if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (!ht.IsNull())
      return (!ht->Value(n1, n2).IsNull());
  }
  return true;
}

int StepData_Field::ItemKind(const int n1,
                                          const int n2) const
{
  if ((thekind & KindArity) == 0)
    return Kind(true);
  int kind = TrueKind(thekind); // si Any, evaluer ...
  if (kind != KindAny)
    return kind;
  //  Otherwise, look for a Transient
  occ::handle<Standard_Transient> item;
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (!ht.IsNull())
      return kind;
    item = ht->Value(n1);
  }
  else if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (!ht.IsNull())
      return kind;
    item = ht->Value(n1, n2);
  }
  if (item.IsNull())
    return 0;
  if (item->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
    return KindString;
  DeclareAndCast(StepData_SelectMember, sm, item);
  if (sm.IsNull())
    return KindEntity;
  return sm->Kind();
}

int StepData_Field::Kind(const bool type) const
{
  if (!type)
    return thekind;
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
      return TrueKind(sm->Kind());
  }
  return TrueKind(thekind);
}

int StepData_Field::Arity() const
{
  return (thekind & KindArity) >> ShiftArity;
}

int StepData_Field::Length(const int index) const
{
  if ((thekind & KindArity) == KindList)
    return theint;
  if ((thekind & KindArity) == KindList2)
  {
    if (index == 2)
      return int(thereal);
    else
      return theint;
  }
  return 0;
}

int StepData_Field::Lower(const int index) const
{
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<int>, hi, theany);
    if (!hi.IsNull())
      return hi->Lower();
    DeclareAndCast(NCollection_HArray1<double>, hr, theany);
    if (!hr.IsNull())
      return hr->Lower();
    DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
    if (!hs.IsNull())
      return hs->Lower();
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (!ht.IsNull())
      return ht->Lower();
  }
  if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return 0;
    if (index == 1)
      return ht->LowerCol();
    if (index == 2)
      return ht->LowerRow();
  }
  return 0;
}

int StepData_Field::Int() const
{
  return theint;
}

int StepData_Field::Integer(const int n1, const int n2) const
{
  if ((thekind & KindArity) == 0)
  {
    if (thekind == KindSelect)
    {
      DeclareAndCast(StepData_SelectMember, sm, theany);
      if (!sm.IsNull())
        return sm->Int();
    }
    return theint;
  }
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<int>, hi, theany);
    if (!hi.IsNull())
      return hi->Value(n1);
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return 0;
    DeclareAndCast(StepData_SelectMember, sm, ht->Value(n1));
    if (!sm.IsNull())
      return sm->Int();
  }
  if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return 0;
    DeclareAndCast(StepData_SelectMember, sm, ht->Value(n1, n2));
    if (!sm.IsNull())
      return sm->Int();
  }
  return 0;
}

bool StepData_Field::Boolean(const int n1, const int n2) const
{
  return (Integer(n1, n2) > 0);
}

StepData_Logical StepData_Field::Logical(const int n1, const int n2) const
{
  int ival = Integer(n1, n2);
  if (ival == 0)
    return StepData_LFalse;
  if (ival == 1)
    return StepData_LTrue;
  return StepData_LUnknown;
}

double StepData_Field::Real(const int n1, const int n2) const
{
  if ((thekind & KindArity) == 0)
  {
    if (thekind == KindSelect)
    {
      DeclareAndCast(StepData_SelectMember, sm, theany);
      if (!sm.IsNull())
        return sm->Real();
    }
    return thereal;
  }
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<double>, hr, theany);
    if (!hr.IsNull())
      return hr->Value(n1);
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return 0;
    DeclareAndCast(StepData_SelectMember, sm, ht->Value(n1));
    if (!sm.IsNull())
      return sm->Real();
  }
  if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<double>, hr, theany);
    if (!hr.IsNull())
      return hr->Value(n1, n2);
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return 0;
    DeclareAndCast(StepData_SelectMember, sm, ht->Value(n1, n2));
    if (!sm.IsNull())
      return sm->Int();
  }
  return 0.0;
}

const char* StepData_Field::String(const int n1, const int n2) const
{
  if (thekind == KindString || thekind == KindEnum)
  {
    DeclareAndCast(TCollection_HAsciiString, str, theany);
    if (!str.IsNull())
      return str->ToCString();
    else
      return "";
  }
  if (thekind == KindSelect)
  {
    DeclareAndCast(StepData_SelectMember, sm, theany);
    if (!sm.IsNull())
      return sm->String();
  }
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<occ::handle<TCollection_HAsciiString>>, hs, theany);
    if (!hs.IsNull())
    {
      if (hs->Value(n1).IsNull())
        return "";
      else
        return hs->Value(n1)->ToCString();
    }
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return "";
    DeclareAndCast(TCollection_HAsciiString, str, ht->Value(n1));
    if (!str.IsNull())
      return str->ToCString();
    DeclareAndCast(StepData_SelectMember, sm, ht->Value(n1));
    if (!sm.IsNull())
      return sm->String();
  }
  if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return "";
    DeclareAndCast(TCollection_HAsciiString, str, ht->Value(n1, n2));
    if (!str.IsNull())
      return str->ToCString();
    DeclareAndCast(StepData_SelectMember, sm, ht->Value(n1, n2));
    if (!sm.IsNull())
      return sm->String();
  }
  return "";
}

int StepData_Field::Enum(const int n1, const int n2) const
{
  return Integer(n1, n2);
}

const char* StepData_Field::EnumText(const int n1,
                                          const int n2) const
{
  return String(n1, n2);
}

occ::handle<Standard_Transient> StepData_Field::Entity(const int n1,
                                                  const int n2) const
{
  occ::handle<Standard_Transient> nulval; // null handle
  if ((thekind & KindArity) == 0)
  {
    if (thekind == KindEntity)
      return theany;
    return nulval;
  }
  if ((thekind & KindArity) == KindList)
  {
    DeclareAndCast(NCollection_HArray1<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return nulval;
    nulval = ht->Value(n1);
    if (nulval.IsNull())
      return nulval;
    if (nulval->IsKind(STANDARD_TYPE(StepData_SelectMember))
        || nulval->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
      nulval.Nullify();
    return nulval;
  }
  if ((thekind & KindArity) == KindList2)
  {
    DeclareAndCast(NCollection_HArray2<occ::handle<Standard_Transient>>, ht, theany);
    if (ht.IsNull())
      return nulval;
    nulval = ht->Value(n1, n2);
    if (nulval.IsNull())
      return nulval;
    if (nulval->IsKind(STANDARD_TYPE(StepData_SelectMember))
        || nulval->IsKind(STANDARD_TYPE(TCollection_HAsciiString)))
      nulval.Nullify();
    return nulval;
  }
  return nulval;
}

occ::handle<Standard_Transient> StepData_Field::Transient() const
{
  return theany;
}
