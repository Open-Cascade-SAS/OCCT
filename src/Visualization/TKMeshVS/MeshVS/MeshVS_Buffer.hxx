// Created on: 2007-03-07
// Created by: msv@EUCLIDEX
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef MeshVS_Buffer_HeaderFile
#define MeshVS_Buffer_HeaderFile

#include <Standard.hxx>
#include <gp_Pnt.hxx>

/**
 * General purpose buffer that is allocated on the stack with a
 * constant size MeshVS_BufSize, or is allocated dynamically if the requested
 * size exceeds the standard one.
 * It is useful when an allocation of an array of unknown size is needed,
 * and most often the array is small enough to allocate as automatic C array.
 */

//! define the constant to the size of 10 points
#define MeshVS_BufSize 10 * 3 * sizeof(double)

class MeshVS_Buffer
{
public:
  //! Constructor of the buffer of the requested size
  MeshVS_Buffer(const Standard_Size theSize)
      : myDynData(0)
  {
    if (theSize > MeshVS_BufSize)
      myDynData = Standard::Allocate(theSize);
  }

  //! Destructor
  ~MeshVS_Buffer()
  {
    if (myDynData)
    {
      Standard::Free(myDynData);
      myDynData = 0;
    }
  }

  //! Cast the buffer to the void pointer
  operator void*() { return myDynData ? myDynData : (void*)myAutoData; }

  //! Interpret the buffer as a reference to double
  operator Standard_Real&()
  {
    return *(myDynData ? (Standard_Real*)myDynData : (Standard_Real*)myAutoData);
  }

  //! Interpret the buffer as a reference to int
  operator Standard_Integer&()
  {
    return *(myDynData ? (Standard_Integer*)myDynData : (Standard_Integer*)myAutoData);
  }

  //! Interpret the buffer as a reference to gp_Pnt
  operator gp_Pnt&() { return *(myDynData ? (gp_Pnt*)myDynData : (gp_Pnt*)myAutoData); }

private:
  //! Deprecate copy constructor
  MeshVS_Buffer(const MeshVS_Buffer&) {}

  //! Deprecate copy operation
  MeshVS_Buffer& operator=(const MeshVS_Buffer&) { return *this; }

  char  myAutoData[MeshVS_BufSize];
  void* myDynData;
};

#endif
