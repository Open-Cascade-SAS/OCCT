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

#include <Interface_BitMap.hxx>
#include <TCollection_AsciiString.hxx>

Interface_BitMap::Interface_BitMap()
{
  Initialize(0);
}

Interface_BitMap::Interface_BitMap(const int nbitems, const int resflags)
{
  Initialize(nbitems, resflags);
}

void Interface_BitMap::Initialize(const int nbitems, const int resflags)
{
  thenbitems = nbitems;
  thenbwords = nbitems / 32 + 1;
  thenbflags = 0;
  theflags   = new NCollection_HArray1<int>(0, thenbwords * (resflags + 1), 0);
}

Interface_BitMap::Interface_BitMap(const Interface_BitMap& other, const bool copied)
{

  Initialize(other, copied);
}

void Interface_BitMap::Initialize(const Interface_BitMap& other, const bool copied)
{
  thenbitems = other.thenbitems;
  thenbwords = other.thenbwords;
  thenbflags = other.thenbflags;
  if (!copied)
  {
    theflags = other.theflags;
    thenames = other.thenames;
  }
  else
  {
    theflags = new NCollection_HArray1<int>(other.theflags->Array1());
    if (!other.thenames.IsNull())
    {
      thenames = new NCollection_HSequence<TCollection_AsciiString>(other.thenames->Sequence());
    }
  }
}

void Interface_BitMap::Reservate(const int moreflags)
{
  int nb      = theflags->Upper();
  int nbflags = nb / thenbwords - 1; // flag 0 not counted ...
  if (nbflags >= thenbflags + moreflags)
    return;
  int                                   nbw   = thenbwords * (thenbflags + moreflags + 2);
  occ::handle<NCollection_HArray1<int>> flags = new NCollection_HArray1<int>(0, nbw);
  int                                   i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i <= nb; i++)
    flags->SetValue(i, theflags->Value(i));
  for (i = nb + 1; i <= nbw; i++)
    flags->SetValue(i, 0);
  theflags = flags;
}

void Interface_BitMap::SetLength(const int nbitems)
{
  int nbw = nbitems / 32 + 1;
  if (nbw == thenbwords)
    return;
  occ::handle<NCollection_HArray1<int>> flags =
    new NCollection_HArray1<int>(0, nbw * (thenbflags + 1));
  if (nbw > thenbwords)
    flags->Init(0);
  int nbmots = (nbw > thenbwords ? thenbwords : nbw);
  int i0 = 0, i1 = 0;
  for (int nf = 0; nf <= thenbflags; nf++)
  {
    for (int i = 0; i < nbmots; i++)
      flags->SetValue(i1 + i, theflags->Value(i0 + i));
    i0 += thenbwords;
    i1 += nbw;
  }
  theflags   = flags;
  thenbitems = nbitems;
  thenbwords = nbw;
}

int Interface_BitMap::AddFlag(const char* name)
{
  Reservate(1);
  int deja = 0;
  if (thenames.IsNull())
    thenames = new NCollection_HSequence<TCollection_AsciiString>();
  else
  {
    int i, nb = thenames->Length();
    for (i = 1; i <= nb; i++)
    {
      if (thenames->Value(i).IsEqual("."))
      {
        thenames->ChangeValue(i).AssignCat(name);
        deja = i;
      }
    }
  }
  if (!deja)
    thenames->Append(TCollection_AsciiString(name));
  thenbflags++;
  return (deja ? deja : thenbflags);
}

int Interface_BitMap::AddSomeFlags(const int more)
{
  Reservate(more);
  if (thenames.IsNull())
    thenames = new NCollection_HSequence<TCollection_AsciiString>();
  for (int i = 1; i <= more; i++)
    thenames->Append(TCollection_AsciiString(""));
  thenbflags += more;
  return thenbflags;
}

bool Interface_BitMap::RemoveFlag(const int num)
{
  if (num < 1 || num > thenames->Length())
    return false;
  if (num == thenames->Length())
    thenames->Remove(thenames->Length());
  else
    thenames->ChangeValue(num).AssignCat(".");
  thenbflags--;
  return true;
}

bool Interface_BitMap::SetFlagName(const int num, const char* name)
{
  if (num < 1 || num > thenames->Length())
    return false;
  int deja = (name[0] == '\0' ? 0 : FlagNumber(name));
  if (deja != 0 && deja != num)
    return false;
  thenames->ChangeValue(num).AssignCat(name);
  return true;
}

int Interface_BitMap::NbFlags() const
{
  return thenbflags;
}

int Interface_BitMap::Length() const
{
  return thenbitems;
}

const char* Interface_BitMap::FlagName(const int num) const
{
  if (theflags.IsNull())
    return "";
  if (num < 1 || num > thenames->Length())
    return "";
  return thenames->Value(num).ToCString();
}

int Interface_BitMap::FlagNumber(const char* name) const
{
  if (name[0] == '\0')
    return 0;
  if (thenames.IsNull())
    return 0;
  int i, nb = thenames->Length();
  for (i = 1; i <= nb; i++)
    if (thenames->Value(i).IsEqual(name))
      return i;
  return 0;
}

//  Values ...

bool Interface_BitMap::Value(const int item, const int flag) const
{
  int        numw = (thenbwords * flag) + (item >> 5);
  const int& val  = theflags->Value(numw);
  if (val == 0)
    return false;
  if (val == ~(0))
    return true;
  int numb = item & 31;
  return (((1 << numb) & val) != 0);
}

void Interface_BitMap::SetValue(const int item, const bool val, const int flag) const
{
  if (val)
    SetTrue(item, flag);
  else
    SetFalse(item, flag);
}

void Interface_BitMap::SetTrue(const int item, const int flag) const
{
  int numw = (thenbwords * flag) + (item >> 5);
  int numb = item & 31;
  theflags->ChangeValue(numw) |= (1 << numb);
}

void Interface_BitMap::SetFalse(const int item, const int flag) const
{
  int  numw = (thenbwords * flag) + (item >> 5);
  int& val  = theflags->ChangeValue(numw);
  if (val == 0)
    return;
  int numb = item & 31;
  theflags->ChangeValue(numw) &= ~(1 << numb);
}

bool Interface_BitMap::CTrue(const int item, const int flag) const
{
  int  numw = (thenbwords * flag) + (item >> 5);
  int  numb = item & 31;
  int& val  = theflags->ChangeValue(numw);
  int  res, mot = (1 << numb);

  if (val == 0)
  {
    val = mot;
    return false;
  }
  else
  {
    res = val & mot;
    val |= mot;
  }
  return (res != 0);
}

bool Interface_BitMap::CFalse(const int item, const int flag) const
{
  int  numw = (thenbwords * flag) + (item >> 5);
  int  numb = item & 31;
  int& val  = theflags->ChangeValue(numw);
  int  res, mot = ~(1 << numb);

  if (val == ~(0))
  {
    val = mot;
    return false;
  }
  else
  {
    res = val | mot;
    val &= mot;
  }
  return (res != 0);
}

void Interface_BitMap::Init(const bool val, const int flag) const
{
  int i, ii = thenbwords, i1 = thenbwords * flag;
  if (flag < 0)
  {
    i1 = 0;
    ii = thenbwords * (thenbflags + 1);
  }
  if (val)
    for (i = 0; i < ii; i++)
      theflags->SetValue(i1 + i, ~(0));
  else
    for (i = 0; i < ii; i++)
      theflags->SetValue(i1 + i, 0);
}

void Interface_BitMap::Clear()
{
  theflags.Nullify();
  Initialize(0);
}
