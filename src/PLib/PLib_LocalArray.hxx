// Created on: 2009-09-23
// Copyright (c) 2009-2012 OPEN CASCADE SAS
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


#ifndef _PLib_LocalArray_HeaderFile
#define _PLib_LocalArray_HeaderFile

#include <Standard.hxx>
#include <Standard_TypeDef.hxx>

//! Auxiliary class optimizing creation of array buffer for
//! evaluation of bspline (using stack allocation for small arrays)
class PLib_LocalArray
{
public:

  // 1K * sizeof (double) = 8K
  static const size_t MAX_ARRAY_SIZE = 1024;

  PLib_LocalArray (const size_t theSize)
  : myPtr (myBuffer)
  {
    Allocate(theSize);
  }

  PLib_LocalArray()
  : myPtr (myBuffer) {}

  virtual ~PLib_LocalArray()
  {
    Deallocate();
  }

  void Allocate (const size_t theSize)
  {
    Deallocate();
    if (theSize > MAX_ARRAY_SIZE)
      myPtr = (Standard_Real*)Standard::Allocate (theSize * sizeof(Standard_Real));
    else
      myPtr = myBuffer;
  }

  operator Standard_Real*() const
  {
    return myPtr;
  }

private:

  PLib_LocalArray (const PLib_LocalArray& );
  PLib_LocalArray& operator= (const PLib_LocalArray& );

protected:

  void Deallocate()
  {
    if (myPtr != myBuffer)
      Standard::Free (*(Standard_Address*)&myPtr);
  }

protected:

  Standard_Real  myBuffer[MAX_ARRAY_SIZE];
  Standard_Real* myPtr;

};

#endif
