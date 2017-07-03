// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Standard_ArrayStreamBuffer.hxx>

// =======================================================================
// function : Standard_ArrayStreamBuffer
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::Standard_ArrayStreamBuffer (const char*  theBegin,
                                                        const size_t theSize)
: myBegin  (theBegin),
  myEnd    (theBegin + theSize),
  myCurrent(theBegin)
{
  //
}

// =======================================================================
// function : ~Standard_ArrayStreamBuffer
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::~Standard_ArrayStreamBuffer()
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void Standard_ArrayStreamBuffer::Init (const char*  theBegin,
                                       const size_t theSize)
{
  myBegin   = theBegin;
  myEnd     = theBegin + theSize;
  myCurrent = theBegin;
}

// =======================================================================
// function : underflow
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::int_type Standard_ArrayStreamBuffer::underflow()
{
  if (myCurrent == myEnd)
  {
    return traits_type::eof();
  }

  return traits_type::to_int_type(*myCurrent);
}

// =======================================================================
// function : uflow
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::int_type Standard_ArrayStreamBuffer::uflow()
{
  if (myCurrent == myEnd)
  {
    return traits_type::eof();
  }

  return traits_type::to_int_type(*myCurrent++);
}

// =======================================================================
// function : pbackfail
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::int_type Standard_ArrayStreamBuffer::pbackfail (int_type ch)
{
  if (myCurrent == myBegin
    || (ch != traits_type::eof()
    && ch != myCurrent[-1]))
  {
    return traits_type::eof();
  }

  return traits_type::to_int_type(*--myCurrent);
}

// =======================================================================
// function : showmanyc
// purpose  :
// =======================================================================
std::streamsize Standard_ArrayStreamBuffer::showmanyc()
{
  if (myCurrent > myEnd)
  {
    // assert
  }
  return myEnd - myCurrent;
}

// =======================================================================
// function : seekoff
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::pos_type Standard_ArrayStreamBuffer::seekoff (off_type theOff,
                                                                          std::ios_base::seekdir theWay,
                                                                          std::ios_base::openmode theWhich)
{
  switch (theWay)
  {
    case std::ios_base::beg:
    {
      myCurrent = myBegin + theOff;
      if (myCurrent >= myEnd)
      {
        myCurrent = myEnd;
      }
      break;
    }
    case std::ios_base::cur:
    {
      myCurrent += theOff;
      if (myCurrent >= myEnd)
      {
        myCurrent = myEnd;
      }
      break;
    }
    case std::ios_base::end:
    {
      myCurrent = myEnd - theOff;
      if (myCurrent < myBegin)
      {
        myCurrent = myBegin;
      }
      break;
    }
    default:
    {
      break;
    }
  }
  (void )theWhich;
  return myCurrent - myBegin;
}

// =======================================================================
// function : seekpos
// purpose  :
// =======================================================================
Standard_ArrayStreamBuffer::pos_type Standard_ArrayStreamBuffer::seekpos (pos_type thePosition,
                                                                          std::ios_base::openmode theWhich)
{
  return seekoff (off_type(thePosition), std::ios_base::beg, theWhich);
}

// =======================================================================
// function : xsgetn
// purpose  :
// =======================================================================
std::streamsize Standard_ArrayStreamBuffer::xsgetn (char* thePtr,
                                                    std::streamsize theCount)
{
  const char* aCurrent = myCurrent + theCount;
  if (aCurrent >= myEnd)
  {
    aCurrent = myEnd;
  }
  size_t aCopied = aCurrent - myCurrent;
  if (aCopied == 0)
  {
    return 0;
  }
  memcpy (thePtr, myCurrent, aCopied);
  myCurrent = aCurrent;
  return aCopied;
}
