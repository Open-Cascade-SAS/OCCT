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


#ifndef _NCollection_LocalArray_HeaderFile
#define _NCollection_LocalArray_HeaderFile

#include <Standard.hxx>
#include <Standard_TypeDef.hxx>

//! Auxiliary class optimizing creation of array buffer 
//! (using stack allocation for small arrays).
template<class theItem> class NCollection_LocalArray
{
public:

  // 1K * sizeof (theItem)
  static const size_t MAX_ARRAY_SIZE = 1024;

  NCollection_LocalArray (const size_t theSize)
  : myPtr (myBuffer)
  {
    Allocate(theSize);
  }

  NCollection_LocalArray ()
  : myPtr (myBuffer) {}

  virtual ~NCollection_LocalArray()
  {
    Deallocate();
  }

  void Allocate (const size_t theSize)
  {
    Deallocate();
    if (theSize > MAX_ARRAY_SIZE)
      myPtr = (theItem*)Standard::Allocate (theSize * sizeof(theItem));
    else
      myPtr = myBuffer;
  }

  operator theItem*() const
  {
    return myPtr;
  }

private:

  NCollection_LocalArray (const NCollection_LocalArray& );
  NCollection_LocalArray& operator= (const NCollection_LocalArray& );

protected:

  void Deallocate()
  {
    if (myPtr != myBuffer)
      Standard::Free (*(Standard_Address*)&myPtr);
  }

protected:

  theItem  myBuffer[MAX_ARRAY_SIZE];
  theItem* myPtr;

};

#endif // _NCollection_LocalArray_HeaderFile
