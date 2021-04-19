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

#ifndef _Standard_ArrayStreamBuffer_HeaderFile
#define _Standard_ArrayStreamBuffer_HeaderFile

#include <Standard_Type.hxx>

#include <fstream>

// Suppress VC9 warning on xsputn() function
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

//! Custom buffer object implementing STL interface std::streambuf for streamed reading from allocated memory block.
//! Implements minimal sub-set of methods for passing buffer to std::istream, including seek support.
//!
//! This class can be used for creating a seekable input stream in cases,
//! when the source data does not satisfies Reader requirements (non-seekable stream, compressed data)
//! or represents an in-memory resource.
//!
//! The memory itself is NOT managed by this class - it is up to the caller to ensure that passed memory pointer
//! is not released during Standard_ArrayStreamBuffer lifetime.
//!
//! Usage example:
//! @code
//!   const char*  theBuffer;
//!   const size_t theBufferLength;
//!   Standard_ArrayStreamBuffer aStreamBuffer (theBuffer, theBufferLength);
//!   std::istream aStream (&aStreamBuffer);
//!   TopoDS_Shape aShape;
//!   BRep_Builder aBuilder;
//!   BRepTools::Read (aShape, aStream, aBuilder);
//! @endcode
class Standard_ArrayStreamBuffer : public std::streambuf
{
public:

  //! Main constructor.
  //! Passed pointer is stored as is (memory is NOT copied nor released with destructor).
  //! @param theBegin pointer to the beginning of pre-allocated buffer
  //! @param theSize  length of pre-allocated buffer
  Standard_EXPORT Standard_ArrayStreamBuffer (const char*  theBegin,
                                              const size_t theSize);

  //! Destructor.
  Standard_EXPORT virtual ~Standard_ArrayStreamBuffer();

  //! (Re)-initialize the stream.
  //! Passed pointer is stored as is (memory is NOT copied nor released with destructor).
  //! @param theBegin pointer to the beginning of pre-allocated buffer
  //! @param theSize  length of pre-allocated buffer
  Standard_EXPORT virtual void Init (const char*  theBegin,
                                     const size_t theSize);

protected:

  //! Get character on underflow.
  //! Virtual function called by other member functions to get the current character
  //! in the controlled input sequence without changing the current position.
  Standard_EXPORT virtual int_type underflow() Standard_OVERRIDE;

  //! Get character on underflow and advance position.
  //! Virtual function called by other member functions to get the current character
  //! in the controlled input sequence and then advance the position indicator to the next character.
  Standard_EXPORT virtual int_type uflow() Standard_OVERRIDE;

  //! Put character back in the case of backup underflow.
  //! Virtual function called by other member functions to put a character back
  //! into the controlled input sequence and decrease the position indicator.
  Standard_EXPORT virtual int_type pbackfail (int_type ch) Standard_OVERRIDE;

  //! Get number of characters available.
  //! Virtual function (to be read s-how-many-c) called by other member functions
  //! to get an estimate on the number of characters available in the associated input sequence.
  Standard_EXPORT virtual std::streamsize showmanyc() Standard_OVERRIDE;

  //! Seek to specified position.
  Standard_EXPORT virtual pos_type seekoff (off_type theOff,
                                            std::ios_base::seekdir theWay,
                                            std::ios_base::openmode theWhich) Standard_OVERRIDE;

  //! Change to specified position, according to mode.
  Standard_EXPORT virtual pos_type seekpos (pos_type thePosition,
                                            std::ios_base::openmode theWhich) Standard_OVERRIDE;

public:

  //! Read a bunch of bytes at once.
  Standard_EXPORT virtual std::streamsize xsgetn (char* thePtr,
                                                  std::streamsize theCount) Standard_OVERRIDE;

private:

  // copying is not allowed
  Standard_ArrayStreamBuffer            (const Standard_ArrayStreamBuffer& );
  Standard_ArrayStreamBuffer& operator= (const Standard_ArrayStreamBuffer& );

protected:

  const char* myBegin;
  const char* myEnd;
  const char* myCurrent;

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // _Standard_ArrayStreamBuffer_HeaderFile
