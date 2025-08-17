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

// szv#4 S4163

#include <Interface_IntList.hxx>

//   Data organization :
//   theents value : 0 no reference
//    > 0 : one reference, here is the value; no list
//    < 0 : a list of references; we store <rank>, it starts at <rank>+1
//   the list is in therefs and is thus constituted :
//   list of negative values, ending with a positive value :
//   from <rank>+1 to <rank>+nb , <rank>+1 to <rank>+nb-1 are negative and
//    <rank>+nb is negative
//   a zero means : free space
//   Pre-reservation : <rank> notes the current number, in strict positive
//   It must then be incremented at each addition
//   Contextual usage, you must call SetNumber(num < 0) to exploit this
//   info and Add(ref < 0) to manage it.
//   If it is not present, we switch to current mode
Interface_IntList::Interface_IntList()
{
  thenbe = thenbr = thenum = thecount = therank = 0;
}

Interface_IntList::Interface_IntList(const Standard_Integer nbe)
{
  Initialize(nbe);
}

Interface_IntList::Interface_IntList(const Interface_IntList& other, const Standard_Boolean copied)
{
  thenbe = other.NbEntities();
  thenum = thecount = therank = 0; // szv#4:S4163:12Mar99 initialization needed
  other.Internals(thenbr, theents, therefs);
  if (copied)
  {
    Standard_Integer                 i;
    Handle(TColStd_HArray1OfInteger) ents = new TColStd_HArray1OfInteger(0, thenbe);
    ents->Init(0);
    for (i = 1; i <= thenbe; i++)
      ents->SetValue(i, theents->Value(i));
    Handle(TColStd_HArray1OfInteger) refs = new TColStd_HArray1OfInteger(0, thenbr);
    refs->Init(0);
    for (i = 1; i <= thenbr; i++)
      refs->SetValue(i, therefs->Value(i));
    theents = ents;
    therefs = refs;
  }
  SetNumber(other.Number());
}

void Interface_IntList::Initialize(const Standard_Integer nbe)
{
  thenbe = nbe;
  thenbr = thenum = thecount = therank = 0;
  theents                              = new TColStd_HArray1OfInteger(0, nbe);
  theents->Init(0);
}

void Interface_IntList::Internals(Standard_Integer&                 nbrefs,
                                  Handle(TColStd_HArray1OfInteger)& ents,
                                  Handle(TColStd_HArray1OfInteger)& refs) const
{
  nbrefs = thenbr;
  ents   = theents;
  refs   = therefs;
}

Standard_Integer Interface_IntList::NbEntities() const
{
  return thenbe;
}

void Interface_IntList::SetNbEntities(const Standard_Integer nbe)
{
  if (nbe <= theents->Upper())
    return;
  Standard_Integer                 i;
  Handle(TColStd_HArray1OfInteger) ents = new TColStd_HArray1OfInteger(0, nbe);
  ents->Init(0);
  for (i = 1; i <= thenbe; i++)
    ents->SetValue(i, theents->Value(i));
  theents = ents;
  thenbe  = nbe;
}

void Interface_IntList::SetNumber(const Standard_Integer number)
{
  //  Pre-reservation usage : to be requested specifically ! -> optimization
  //   <preres> verifies that the pre-reservation is valid
  if (number < 0)
  {
    if (thenum == -number || number < -thenbe)
      return;
    Standard_Boolean preres = Standard_True;
    thenum                  = -number;
    Standard_Integer val    = theents->Value(thenum);
    if (val == 0)
    {
      thecount = 0;
      therank  = 0;
    }
    else if (val > 0)
    {
      thecount = 1;
      therank  = -1;
    }
    if (val < -1)
    {
      therank  = -val;
      thecount = therefs->Value(therank);
      if (thecount <= 0)
        preres = Standard_False;
    }
    if (preres)
      return;
  }
  //  Current usage. The following in current usage or if no pre-reservation
  else if (number > 0)
  {
    if (thenum == number || number > thenbe)
      return;
    thenum = number;
  }
  else
    return;

  Standard_Integer val = theents->Value(thenum);
  if (val == 0)
  {
    thecount = 0;
    therank  = 0;
  }
  else if (val > 0)
  {
    thecount = 1;
    therank  = -1;
  }
  else if (val < -1)
  {
    therank  = -val;
    thecount = 0;
    if (therefs->Value(therank + 1) == 0)
      thecount = -therefs->Value(therank);
    else
    {
      for (Standard_Integer j = 1;; j++)
      {
        val = therefs->Value(therank + j);
        if (val >= 0)
          break;
        thecount++;
      }
      if (val > 0)
        thecount++;
    }
  }
  else
  {
    thecount = 0;
    therank  = -1;
  } // val == -1 reste
}

Standard_Integer Interface_IntList::Number() const
{
  return thenum;
}

Interface_IntList Interface_IntList::List(const Standard_Integer number,
                                          const Standard_Boolean copied) const
{
  Interface_IntList alist(*this, copied);
  alist.SetNumber(number);
  return alist;
}

void Interface_IntList::SetRedefined(const Standard_Boolean mode)
{
  if (!NbEntities() || thenum == 0)
    return;

  Standard_Integer val = theents->Value(thenum);
  if (val < -1)
    return;
  else if (mode)
  {
    if (val == 0)
      theents->SetValue(thenum, -1);
    else if (val > 0)
    {
      Reservate(2);
      theents->SetValue(thenum, -thenbr);
      therefs->SetValue(thenbr + 1, val);
      thenbr++;
    }
  }
  else if (!mode)
  {
    if (val == -1)
      theents->SetValue(thenum, 0);
    else if (therefs->Value(therank + 1) >= 0)
    {
      theents->SetValue(thenum, therefs->Value(therank + 1));
      if (thenbr == therank + 1)
        thenbr--;
    }
  }
}

void Interface_IntList::Reservate(const Standard_Integer count)
{
  //  Reservate (-count) = Reservate (count) + allocation on current entity + 1
  if (count < 0)
  {
    Reservate(-count - 1);
    if (thenum == 0)
      return;
    thenbr++;
    therefs->SetValue(thenbr, 0); // will contain the number ...
    therank = thenbr;
    theents->SetValue(thenum, -thenbr);
    thenbr -= count;
    return;
  }
  Standard_Integer up, oldup = 0;
  if (thenbr == 0)
  { //  i.e. not yet allocated ...
    up = thenbe / 2 + 1;
    if (up < 2)
      up = 2;
    if (up < count)
      up = count * 3 / 2;
    therefs = new TColStd_HArray1OfInteger(0, up);
    therefs->Init(0);
    thenbr = 2; // we start after (convenience of addressing)
  }
  oldup = therefs->Upper();
  if (thenbr + count < oldup)
    return; // OK
  up = oldup * 3 / 2 + count;
  if (up < 2)
    up = 2;
  Handle(TColStd_HArray1OfInteger) refs = new TColStd_HArray1OfInteger(0, up);
  refs->Init(0);
  for (Standard_Integer i = 1; i <= oldup; i++)
    refs->SetValue(i, therefs->Value(i));
  therefs = refs;
}

void Interface_IntList::Add(const Standard_Integer ref)
{
  if (thenum == 0)
    return;
  //   ref < 0 : pre-reservation
  if (ref < 0)
  {
    Add(-ref);
    if (therank <= 0)
      return;
    if (therefs->Value(therank) >= 0)
      therefs->SetValue(therank, thecount);
    return;
  }

  if (therank == 0)
  {
    theents->SetValue(thenum, ref);
    thecount = 1;
    therank  = -1;
  }
  else if (therank < 0)
  {
    Reservate(2);
    therank              = thenbr;
    Standard_Integer val = theents->Value(thenum);
    theents->SetValue(thenum, -thenbr);
    if (thecount == 1)
    {
      therefs->SetValue(thenbr + 1, -val);
      thenbr++;
    }
    therefs->SetValue(thenbr + 1, ref);
    thenbr++;
    thecount++;
  }
  else if (thenbr == therank + thecount)
  { // free space at end
    therefs->SetValue(thenbr, -therefs->Value(thenbr));
    therefs->SetValue(thenbr + 1, ref);
    thenbr++;
    thecount++;
  }
  else if (therefs->Value(therank + thecount + 1) == 0)
  { // free space after
    therefs->SetValue(therank + thecount, -therefs->Value(therank + thecount));
    therefs->SetValue(therank + thecount + 1, ref);
    thecount++;
  }
  else
  { // copy further !
    Reservate(thecount + 2);
    Standard_Integer rank = therank;
    therank               = thenbr;
    theents->SetValue(thenum, -therank);
    for (Standard_Integer i = 1; i < thecount; i++)
    {
      therefs->SetValue(therank + i, therefs->Value(rank + i));
      therefs->SetValue(rank + i, 0);
    }
    therefs->SetValue(therank + thecount, -therefs->Value(rank + thecount));
    therefs->SetValue(rank + thecount, 0);
    therefs->SetValue(therank + thecount + 1, ref);
    thecount++;
    thenbr = therank + thecount + 1;
  }
}

Standard_Integer Interface_IntList::Length() const
{
  return thecount;
}

Standard_Boolean Interface_IntList::IsRedefined(const Standard_Integer num) const
{
  Standard_Integer n = (num == 0 ? thenum : num);
  if (!NbEntities() || n == 0)
    return Standard_False;
  if (theents->Value(n) < 0)
    return Standard_True;
  return Standard_False;
}

Standard_Integer Interface_IntList::Value(const Standard_Integer num) const
{
  if (thenum == 0)
    return 0;
  if (num <= 0 || num > thecount)
    return 0;
  if (thecount == 0)
    return 0;
  if (therank <= 0)
    return theents->Value(thenum);
  Standard_Integer val = therefs->Value(therank + num);
  if (val < 0)
    return -val;
  return val;
}

Standard_Boolean Interface_IntList::Remove(const Standard_Integer)
{
  return Standard_False; // not yet implemented
}

void Interface_IntList::Clear()
{
  if (thenbr == 0)
    return; // already clear
  Standard_Integer i, low, up;
  low = theents->Lower();
  up  = theents->Upper();
  for (i = low; i <= up; i++)
    theents->SetValue(i, 0);
  thenbr = 0;
  if (therefs.IsNull())
    return;
  low = therefs->Lower();
  up  = therefs->Upper();
  for (i = low; i <= up; i++)
    therefs->SetValue(i, 0);
}

void Interface_IntList::AdjustSize(const Standard_Integer margin)
{
  Standard_Integer i, up = theents->Upper();
  if (up > thenbe)
  {
    Handle(TColStd_HArray1OfInteger) ents = new TColStd_HArray1OfInteger(0, thenbe);
    ents->Init(0);
    for (i = 1; i <= thenbe; i++)
      ents->SetValue(i, theents->Value(i));
    theents = ents;
  }
  if (thenbr == 0)
    Reservate(margin);
  else
  {
    up = therefs->Upper();
    if (up >= thenbr && up <= thenbr + margin)
      return;
    Handle(TColStd_HArray1OfInteger) refs = new TColStd_HArray1OfInteger(0, thenbr + margin);
    refs->Init(0);
    for (i = 1; i <= thenbr; i++)
      refs->SetValue(i, therefs->Value(i));
    therefs = refs;
  }
}
