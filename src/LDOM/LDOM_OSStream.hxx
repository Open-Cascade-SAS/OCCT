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

#ifndef LDOM_OSStream_HeaderFile
#define LDOM_OSStream_HeaderFile

#include <NCollection_DefineAlloc.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>

#include <stdlib.h>
#include <stdio.h> /* EOF */

//! Class LDOM_SBuffer inherits streambuf and
//! redefines some virtual methods of it (overflow() and xsputn()).
//! This class contains pointers on first and current element 
//! of sequence, also it has methods for the sequence management.
class LDOM_SBuffer : public std::streambuf
{
  //! One element of sequence.
  //! Can only be allocated by the allocator and assumes
  //! it is IncAllocator, so destructor isn't required.
  struct LDOM_StringElem
  {
    char*            buf;  //!< pointer on data string
    int              len;  //!< quantity of really written data
    LDOM_StringElem* next; //!< pointer on the next element of a sequence

    DEFINE_NCOLLECTION_ALLOC

    LDOM_StringElem(const int, const Handle(NCollection_BaseAllocator)&);
    ~LDOM_StringElem();

  private:
    LDOM_StringElem (const LDOM_StringElem&);
    LDOM_StringElem& operator= (const LDOM_StringElem&);
  };

public:
  //! Constructor. Sets a default value for the
  //!              length of each sequence element.
  Standard_EXPORT LDOM_SBuffer (const Standard_Integer theMaxBuf);

  //! Concatenates strings of all sequence elements
  //! into one string. Space for output string is allocated
  //! with operator new.
  //! Caller of this function is responsible
  //! for memory release after the string usage.
  Standard_EXPORT Standard_CString str () const;

  //! Returns full length of data contained
  Standard_Integer Length () const {return myLength;}

  //! Clears first element of sequence and removes all others
  Standard_EXPORT void Clear ();

  // Methods of streambuf

  Standard_EXPORT virtual int overflow(int c = EOF) Standard_OVERRIDE;
  Standard_EXPORT virtual int underflow() Standard_OVERRIDE;
  //virtual int uflow();

  Standard_EXPORT virtual std::streamsize xsputn(const char* s, std::streamsize n) Standard_OVERRIDE;
  //virtual int xsgetn(char* s, int n);
  //virtual int sync();

  Standard_EXPORT ~LDOM_SBuffer ();
  // Destructor

private:

  Standard_Integer      myMaxBuf; // default length of one element
  Standard_Integer      myLength; // full length of contained data
  LDOM_StringElem* myFirstString; // the head of the sequence
  LDOM_StringElem* myCurString;   // current element of the sequence
  Handle(NCollection_BaseAllocator) myAlloc; //allocator for chunks
};

//! Subclass if std::ostream allowing to increase performance
//! of outputting data into a string avoiding reallocation of buffer.
//! Class LDOM_OSStream implements output into a sequence of
//! strings and getting the result as a string.
//! It inherits Standard_OStream (ostream).
//! Beside methods of ostream, it also has additional
//! useful methods: str(), Length() and Clear().
class LDOM_OSStream : public Standard_OStream
{
public:
  //! Constructor
  Standard_EXPORT LDOM_OSStream(const Standard_Integer theMaxBuf);

  Standard_CString str () const {return myBuffer.str();}

  Standard_Integer Length () const { return myBuffer.Length(); }

  void Clear () { myBuffer.Clear(); }

 private:
  LDOM_SBuffer myBuffer;
};

#endif
