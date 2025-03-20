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

#include <Interface_LineBuffer.hxx>
#include <Standard_OutOfRange.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

Interface_LineBuffer::Interface_LineBuffer(const Standard_Integer size)
    : myLine(1, size + 1)
{
  myLine.SetValue(1, '\0');
  myMax  = size;
  myInit = myLen = myGet = myKeep = myFriz = 0;
}

void Interface_LineBuffer::SetMax(const Standard_Integer theMax)
{
  if (theMax > myLine.Length())
  {
    throw Standard_OutOfRange("Interface LineBuffer : SetMax");
  }
  if (theMax <= 0)
  {
    myMax = myLine.Length();
  }
  else
  {
    myMax = theMax;
  }
}

void Interface_LineBuffer::SetInitial(const Standard_Integer theInitial)
{
  if (myFriz > 0)
  {
    return;
  }
  if (theInitial >= myMax)
  {
    throw Standard_OutOfRange("Interface LineBuffer : SetInitial");
  }
  if (theInitial <= 0)
  {
    myInit = 0;
  }
  else
  {
    myInit = theInitial;
  }
}

void Interface_LineBuffer::SetKeep()
{
  myKeep = -myLen;
}

Standard_Boolean Interface_LineBuffer::CanGet(const Standard_Integer theMore)
{
  myGet = theMore;
  if ((myLen + myInit + theMore) <= myMax)
  {
    return Standard_True;
  }
  if (myKeep < 0)
  {
    myKeep = -myKeep;
  }
  return Standard_False;
}

void Interface_LineBuffer::FreezeInitial()
{
  myFriz = myInit + 1;
  myInit = 0;
}

void Interface_LineBuffer::Clear()
{
  myGet = myKeep = myLen = myFriz = 0;
  myLine.SetValue(1, '\0');
}

// ....                        RESULTATS                        ....

void Interface_LineBuffer::Prepare()
{
  //  ATTENTION aux blanx initiaux
  if (myInit > 0)
  {
    if ((myLen + myInit) > myMax)
    {
      return;
    }

    for (Standard_Integer i = myLen + 1; i > 0; --i)
    {
      myLine.SetValue(i + myInit, myLine.Value(i));
    }
    for (Standard_Integer i = 1; i <= myInit; ++i)
    {
      myLine.SetValue(i, ' ');
    }
  }
  //  GERER KEEP : est-il jouable ? sinon, annuler. sioui, noter la jointure
  if (myKeep > 0)
  {
    myKeep += (myInit + 1); // myInit, et +1 car Keep INCLUS
  }
  if (myKeep > 0)
  {
    if ((myLen + myGet + myInit - myKeep) >= myMax)
    {
      myKeep = 0;
    }
  }
  if (myKeep > 0)
  {
    myKept = myLine.Value(myKeep);
    myLine.SetValue(myKeep, '\0');
  }
}

void Interface_LineBuffer::Keep()
{
  //  Si Keep, sauver de myKeep + 1  a  myLen (+1 pour 0 final)
  if (myKeep > 0)
  {
    myLine.SetValue(1, myKept);
    for (Standard_Integer i = myKeep + 1; i <= myLen + myInit + 1; ++i)
    {
      myLine.SetValue(i - myKeep + 1, myLine.Value(i));
    }
    myLen = myLen + myInit - myKeep + 1;
  }
  else
  {
    Clear();
  }
  myGet = myKeep = 0;
  if (myFriz > 0)
  {
    myInit = myFriz - 1;
    myFriz = 0;
  }
}

void Interface_LineBuffer::Move(TCollection_AsciiString& theStr)
{
  Prepare();
  theStr.AssignCat(&myLine.First());
  Keep();
}

void Interface_LineBuffer::Move(const Handle(TCollection_HAsciiString)& theStr)
{
  Prepare();
  theStr->AssignCat(&myLine.First());
  Keep();
}

Handle(TCollection_HAsciiString) Interface_LineBuffer::Moved()
{
  Prepare();
  Handle(TCollection_HAsciiString) R = new TCollection_HAsciiString(&myLine.First());
  Keep();
  return R;
}

// ....                        AJOUTS                        ....

void Interface_LineBuffer::Add(const Standard_CString theText)
{
  Add(theText, (Standard_Integer)strlen(theText));
}

void Interface_LineBuffer::Add(const Standard_CString text, const Standard_Integer lntext)
{
  Standard_Integer lnt = (lntext > (myMax - myLen - myInit) ? (myMax - myLen - myInit) : lntext);
  for (Standard_Integer i = 1; i <= lnt; ++i)
  {
    myLine.SetValue(myLen + i, text[i - 1]);
  }
  myLen += lnt;
  myLine.SetValue(myLen + 1, '\0');
}

void Interface_LineBuffer::Add(const TCollection_AsciiString& theText)
{
  Add(theText.ToCString(), theText.Length());
}

void Interface_LineBuffer::Add(const Standard_Character theText)
{
  myLine.SetValue(myLen + 1, theText);
  ++myLen;
  myLine.SetValue(myLen + 1, '\0');
}
