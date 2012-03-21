// Created on: 2007-03-07
// Created by: msv@EUCLIDEX
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef MeshVS_Buffer_HeaderFile
#define MeshVS_Buffer_HeaderFile

#include <Standard.hxx>

/**
 * General purpose buffer that is allocated on the stack with a
 * constant size MeshVS_BufSize, or is allocated dynamically if the requested
 * size exceeds the standard one.
 * It is useful when an allocation of an array of unknown size is needed,
 * and most often the array is small enough to allocate as automatic C array.
 */

//! define the constant to the size of 10 points
#define MeshVS_BufSize 10*3*sizeof(double)

class MeshVS_Buffer 
{
public:
  //! Constructor of the buffer of the requested size
  MeshVS_Buffer (const Standard_Size theSize)
    : myDynData (0)
  {
    if (theSize > MeshVS_BufSize)
      myDynData = Standard::Allocate (theSize);
  }

  //! Destructor
  ~MeshVS_Buffer()
  {
    if (myDynData)
    {
      Standard::Free (myDynData);
      myDynData = 0;
    }
  }

  //! Cast the buffer to the void pointer
  operator void* ()
  {
    return myDynData ? myDynData : (void*) myAutoData;
  }

  //! Interpret the buffer as a reference to double
  operator Standard_Real& ()
  {
    return * (myDynData ? (Standard_Real*) myDynData : (Standard_Real*) myAutoData);
  }

  //! Interpret the buffer as a reference to int
  operator Standard_Integer& ()
  {
    return * (myDynData ? (Standard_Integer*) myDynData : (Standard_Integer*) myAutoData);
  }

private:
  //! Deprecate copy constructor
  MeshVS_Buffer(const MeshVS_Buffer&) {}

  //! Deprecate copy operation
  MeshVS_Buffer& operator=(const MeshVS_Buffer&) {return *this;}

  char  myAutoData[ MeshVS_BufSize ];
  void* myDynData;
};

#endif
