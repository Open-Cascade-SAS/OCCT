// Created on: 2014-04-01
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _NCollection_Buffer_HeaderFile
#define _NCollection_Buffer_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <Standard_Transient.hxx>

//! Low-level buffer object.
class NCollection_Buffer : public Standard_Transient
{

public:

  //! Default constructor.
  //! When theData is NULL but theSize is not 0 than buffer of specified size will be allocated.
  //! @param theAlloc memory allocator
  //! @param theSize  buffer size
  //! @param theData  buffer data allocated by theAlloc
  NCollection_Buffer (const Handle(NCollection_BaseAllocator)& theAlloc,
                      const Standard_Size                      theSize = 0,
                      Standard_Byte*                           theData = NULL)
  : myData (NULL),
    mySize (0),
    myAllocator (theAlloc)
  {
    if (theData != NULL)
    {
      myData = theData;
      mySize = theSize;
    }
    else
    {
      Allocate (theSize);
    }
  }

  //! Destructor.
  ~NCollection_Buffer()
  {
    Free();
  }

  //! @return buffer data
  const Standard_Byte* Data() const
  {
    return myData;
  }

  //! @return buffer data
  Standard_Byte* ChangeData()
  {
    return myData;
  }

  //! @return true if buffer is not allocated
  bool IsEmpty() const
  {
    return myData == NULL;
  }

  //! Return buffer length in bytes.
  Standard_Size Size() const
  {
    return mySize;
  }

  //! @return buffer allocator
  const Handle(NCollection_BaseAllocator)& Allocator() const
  {
    return myAllocator;
  }

  //! Assign new buffer allocator with de-allocation of buffer.
  void SetAllocator (const Handle(NCollection_BaseAllocator)& theAlloc)
  {
    Free();
    myAllocator = theAlloc;
  }

  //! Allocate the buffer.
  //! @param theSize buffer length in bytes
  bool Allocate (const Standard_Size theSize)
  {
    Free();
    mySize = theSize;
    if (theSize != 0
    || !myAllocator.IsNull())
    {
      myData = (Standard_Byte* )myAllocator->Allocate (theSize);
    }

    if (myData == NULL)
    {
      mySize = 0;
      return false;
    }
    return true;
  }

  //! De-allocate buffer.
  void Free()
  {
    if (!myAllocator.IsNull())
    {
      myAllocator->Free (myData);
    }
    myData = NULL;
    mySize = 0;
  }

protected:

  Standard_Byte*                    myData;      //!< data pointer
  Standard_Size                     mySize;      //!< buffer length in bytes
  Handle(NCollection_BaseAllocator) myAllocator; //!< buffer allocator

public:

  DEFINE_STANDARD_RTTI(NCollection_Buffer) // Type definition

};

DEFINE_STANDARD_HANDLE(NCollection_Buffer, Standard_Transient)

#endif // _NCollection_Buffer_HeaderFile
