// Created on: 2001-10-01
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <LDOM_OSStream.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Standard_Assert.hxx>

#include <string.h>

//=======================================================================
//function : LDOM_StringElem()
//purpose  : Constructor
//=======================================================================
LDOM_SBuffer::LDOM_StringElem::LDOM_StringElem
  ( const int theLength, const Handle(NCollection_BaseAllocator)& theAlloc )
: buf (reinterpret_cast<char*>(theAlloc->Allocate (theLength))),
  len (0),
  next(0)
{
}

//=======================================================================
//function : LDOM_SBuffer()
//purpose  : 
//=======================================================================
LDOM_SBuffer::LDOM_SBuffer (const Standard_Integer theMaxBuf)
     : myMaxBuf (theMaxBuf), myLength(0),
       myAlloc (new NCollection_IncAllocator)
{
  myFirstString = new (myAlloc) LDOM_StringElem (theMaxBuf, myAlloc);
  myCurString   = myFirstString;
}

//=======================================================================
//function : ~LDOM_SBuffer()
//purpose  : 
//=======================================================================
LDOM_SBuffer::~LDOM_SBuffer ()
{
  //no destruction is required as IncAllocator is used
}

//=======================================================================
//function : Clear()
//purpose  : 
//=======================================================================
void LDOM_SBuffer::Clear ()
{
  myAlloc       = new NCollection_IncAllocator;
  myFirstString = new (myAlloc) LDOM_StringElem (myMaxBuf, myAlloc);
  myLength      = 0;
  myCurString   = myFirstString;
}

//=======================================================================
//function : str()
//purpose  : 
//=======================================================================
Standard_CString LDOM_SBuffer::str () const
{
  char* aRetStr = new char [myLength + 1];

  LDOM_StringElem* aCurElem = myFirstString;
  int aCurLen = 0;
  while (aCurElem)
  {
    strncpy(aRetStr + aCurLen, aCurElem->buf, aCurElem->len);
    aCurLen += aCurElem->len;
    aCurElem = aCurElem->next;
  }
  *(aRetStr + myLength) = '\0';

  return aRetStr;
}

//=======================================================================
//function : overflow()
//purpose  : redefined virtual
//=======================================================================
int LDOM_SBuffer::overflow(int c)
{
  char cc = (char)c;
  xsputn(&cc,1);
  return c;
}

//=======================================================================
//function : underflow
//purpose  : redefined virtual
//=======================================================================

int LDOM_SBuffer::underflow()
{
  return EOF;
}

//int LDOM_SBuffer::uflow()
//{ return EOF; }

//=======================================================================
//function : xsputn()
//purpose  : redefined virtual
//=======================================================================
std::streamsize LDOM_SBuffer::xsputn (const char* aStr, std::streamsize n)
{
  Standard_ASSERT_RAISE (n < IntegerLast(), "LDOM_SBuffer cannot work with strings greater than 2 Gb");

  Standard_Integer aLen = static_cast<int>(n) + 1;
  Standard_Integer freeLen = myMaxBuf - myCurString->len - 1;
  if (freeLen >= n)
  {
    strncpy(myCurString->buf + myCurString->len, aStr, aLen);
  }
  else if (freeLen <= 0)
  {
    LDOM_StringElem* aNextElem = new (myAlloc) LDOM_StringElem(Max(aLen, myMaxBuf), myAlloc);
    myCurString->next = aNextElem;
    myCurString = aNextElem;
    strncpy(myCurString->buf + myCurString->len, aStr, aLen);
  }
  else // 0 < freeLen < n
  {
    // copy string by parts
    strncpy(myCurString->buf + myCurString->len, aStr, freeLen);
    myCurString->len += freeLen;
    *(myCurString->buf + myCurString->len) = '\0';
    aLen -= freeLen;
    LDOM_StringElem* aNextElem = new (myAlloc) LDOM_StringElem(Max(aLen, myMaxBuf), myAlloc);
    myCurString->next = aNextElem;
    myCurString = aNextElem;
    strncpy(myCurString->buf + myCurString->len, aStr + freeLen, aLen);
  }
  myCurString->len += aLen - 1;
  *(myCurString->buf + myCurString->len) = '\0';

  myLength += static_cast<int>(n);
  return n;
}

//streamsize LDOM_SBuffer::xsgetn(char* s, streamsize n)
//{ return _IO_default_xsgetn(this, s, n); }

//=======================================================================
//function : LDOM_OSStream()
//purpose  : Constructor
//=======================================================================
LDOM_OSStream::LDOM_OSStream (const Standard_Integer theMaxBuf)
     : Standard_OStream (&myBuffer), myBuffer (theMaxBuf)
{
  init(&myBuffer);
}
