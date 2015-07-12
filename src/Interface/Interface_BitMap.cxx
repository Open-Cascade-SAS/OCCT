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
#include <Standard_NotImplemented.hxx>
#include <TCollection_AsciiString.hxx>

Interface_BitMap::Interface_BitMap()
{
  Initialize(0);
}


Interface_BitMap::Interface_BitMap
(const Standard_Integer nbitems, const Standard_Integer resflags)
{
  Initialize(nbitems,resflags);
}

void Interface_BitMap::Initialize(const Standard_Integer nbitems, const Standard_Integer resflags)
{
  thenbitems = nbitems;
  thenbwords = nbitems/32 + 1;
  thenbflags = 0;
  if(nbitems)
  {
    theflags   = new TColStd_HArray1OfInteger (0,thenbwords*(resflags+1));  
    theflags->Init(0);
  }
}

Interface_BitMap::Interface_BitMap
(const Interface_BitMap& other, const Standard_Boolean copied)
{
 
  Initialize(other,copied);
}

void Interface_BitMap::Initialize(const Interface_BitMap& other,
                                  const Standard_Boolean copied)
{
  other.Internals (thenbitems,thenbwords,thenbflags,theflags,thenames);
  if (!copied) return;
  Standard_Integer nb = theflags->Upper ();
  Handle(TColStd_HArray1OfInteger) flags = new TColStd_HArray1OfInteger(0,nb);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i <= nb; i ++)
    flags->SetValue (i,theflags->Value(i));
  theflags = flags;
  if (thenames.IsNull()) return;
  nb = thenames->Length();
  Handle(TColStd_HSequenceOfAsciiString) names = new TColStd_HSequenceOfAsciiString();
  for (i = 1; i <= nb; i ++)
    names->Append ( TCollection_AsciiString(thenames->Value(i)) );
  thenames = names;
}

void  Interface_BitMap::Internals
(Standard_Integer& nbitems, Standard_Integer& nbwords,
 Standard_Integer& nbflags,
 Handle(TColStd_HArray1OfInteger)& flags,
 Handle(TColStd_HSequenceOfAsciiString)& names) const
{
  nbitems = thenbitems;  nbwords = thenbwords;  nbflags = thenbflags;
  flags = theflags;  names = thenames;
}


void  Interface_BitMap::Reservate (const Standard_Integer moreflags)
{
  Standard_Integer nb = theflags->Upper ();
  Standard_Integer nbflags = nb / thenbwords - 1;    // flag 0 non compte ...
  if (nbflags >= thenbflags + moreflags) return;
  Standard_Integer nbw = thenbwords * (thenbflags+moreflags+2);
  Handle(TColStd_HArray1OfInteger) flags = new TColStd_HArray1OfInteger(0,nbw);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 0; i <= nb; i ++)
    flags->SetValue (i,theflags->Value(i));
  for (i = nb+1; i <= nbw; i ++) flags->SetValue (i,0);
  theflags = flags;
}


void  Interface_BitMap::SetLength (const Standard_Integer nbitems)
{
  Standard_Integer nbw = nbitems/32 + 1;
  if (nbw == thenbwords) return;
  Handle(TColStd_HArray1OfInteger) flags =
    new TColStd_HArray1OfInteger(0,nbw*(thenbflags+1));
  if (nbw > thenbwords) flags->Init(0);
  Standard_Integer nbmots = (nbw > thenbwords ? thenbwords : nbw);
  Standard_Integer i0 = 0, i1 = 0;
  for (Standard_Integer nf = 0; nf <= thenbflags; nf ++) {
    for (Standard_Integer i = 0; i < nbmots; i ++)
      flags->SetValue (i1+i,theflags->Value(i0+i));
    i0 += thenbwords;  i1 += nbw;
  }
  theflags   = flags;
  thenbitems = nbitems;
  thenbwords = nbw;
}


Standard_Integer  Interface_BitMap::AddFlag (const Standard_CString name)
{
  Reservate(1);
  Standard_Integer deja = 0;
  if (thenames.IsNull()) thenames = new TColStd_HSequenceOfAsciiString();
  else {
    Standard_Integer i, nb = thenames->Length();
    for (i = 1; i <= nb; i ++) {
      if (thenames->Value(i).IsEqual("."))
      {  thenames->ChangeValue(i).AssignCat(name);  deja = i;  }
    }
  }
  if (!deja) thenames->Append (TCollection_AsciiString(name));
  thenbflags ++;
  return (deja ? deja : thenbflags);
}

Standard_Integer  Interface_BitMap::AddSomeFlags
(const Standard_Integer more)
{
  Reservate(more);
  if (thenames.IsNull()) thenames = new TColStd_HSequenceOfAsciiString();
  for (Standard_Integer i = 1; i <= more; i ++)
    thenames->Append (TCollection_AsciiString(""));
  thenbflags += more;
  return thenbflags;
}

Standard_Boolean  Interface_BitMap::RemoveFlag
(const Standard_Integer num)
{
  if (num < 1 || num > thenames->Length()) return Standard_False;
  if (num == thenames->Length()) thenames->Remove (thenames->Length());
  else  thenames->ChangeValue(num).AssignCat(".");
  thenbflags --;
  return Standard_True;
}

Standard_Boolean  Interface_BitMap::SetFlagName
(const Standard_Integer num, const Standard_CString name)
{
  if (num < 1 || num > thenames->Length()) return Standard_False;
  Standard_Integer deja = (name[0] == '\0' ? 0 : FlagNumber (name) );
  if (deja != 0 && deja != num) return Standard_False;
  thenames->ChangeValue(num).AssignCat(name);
  return Standard_True;
}

Standard_Integer  Interface_BitMap::NbFlags () const
{  return thenbflags;  }

Standard_Integer  Interface_BitMap::Length () const
{  return thenbitems;  }

Standard_CString  Interface_BitMap::FlagName
(const Standard_Integer num) const
{
  if (theflags.IsNull()) return "";
  if (num < 1 || num > thenames->Length()) return "";
  return thenames->Value(num).ToCString();
}

Standard_Integer  Interface_BitMap::FlagNumber
(const Standard_CString name) const
{
  if (name[0] == '\0') return 0;
  if (thenames.IsNull()) return 0;
  Standard_Integer i, nb = thenames->Length();
  for (i = 1; i <= nb; i ++)
    if (thenames->Value(i).IsEqual(name)) return i;
  return 0;
}


//  Les valeurs ...

Standard_Boolean  Interface_BitMap::Value
(const Standard_Integer item, const Standard_Integer flag) const
{
  Standard_Integer numw = (thenbwords * flag) + (item >> 5);
  const Standard_Integer& val  = theflags->Value (numw);
  if (val ==   0 ) return Standard_False;
  if (val == ~(0)) return Standard_True;
  Standard_Integer numb = item & 31;
  return ( ((1 << numb) & val) != 0);
}

void  Interface_BitMap::SetValue
(const Standard_Integer item, const Standard_Boolean val,
 const Standard_Integer flag) const
{
  if (val) SetTrue  (item,flag);
  else     SetFalse (item,flag);
}

void  Interface_BitMap::SetTrue
(const Standard_Integer item, const Standard_Integer flag) const
{
  Standard_Integer numw = (thenbwords * flag) + (item >> 5);
  Standard_Integer numb = item & 31;
  theflags->ChangeValue (numw) |=   (1 << numb);
}

void  Interface_BitMap::SetFalse
(const Standard_Integer item, const Standard_Integer flag) const
{
  Standard_Integer numw = (thenbwords * flag) + (item >> 5);
  Standard_Integer& val = theflags->ChangeValue (numw);
  if (val == 0) return;
  Standard_Integer numb = item & 31;
  theflags->ChangeValue (numw) &= ~(1 << numb);
}

Standard_Boolean  Interface_BitMap::CTrue
(const Standard_Integer item, const Standard_Integer flag) const
{
  Standard_Integer numw = (thenbwords * flag) + (item >> 5);
  Standard_Integer numb = item & 31;
  Standard_Integer& val = theflags->ChangeValue (numw);
  Standard_Integer  res, mot = (1 << numb);

  if (val == 0)  {  val = mot;  return Standard_False;  }
  else           {  res = val & mot;  val |= mot;  }
  return (res != 0);
}

Standard_Boolean  Interface_BitMap::CFalse
(const Standard_Integer item, const Standard_Integer flag) const
{
  Standard_Integer numw = (thenbwords * flag) + (item >> 5);
  Standard_Integer numb = item & 31;
  Standard_Integer& val = theflags->ChangeValue (numw);
  Standard_Integer  res, mot = ~(1 << numb);

  if (val == ~(0))  {  val = mot;  return Standard_False;  }
  else              {  res = val | mot;  val &= mot;  }
  return (res != 0);
}


void  Interface_BitMap::Init
(const Standard_Boolean val, const Standard_Integer flag) const
{
  Standard_Integer i, ii = thenbwords, i1 = thenbwords *flag;
  if (flag < 0)  {  i1 = 0;  ii = thenbwords*(thenbflags+1);  }
  if (val)  for (i = 0; i < ii; i ++) theflags->SetValue (i1+i,~(0));
  else      for (i = 0; i < ii; i ++) theflags->SetValue (i1+i,  0 );
}

void Interface_BitMap::Clear()
{
  theflags.Nullify();
  Initialize(0);
}
