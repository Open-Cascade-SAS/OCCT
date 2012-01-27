// File:        PLib_LocalArray.hxx
// Created:     Wed Sep 23 2009
// Copyright:   Open CASCADE 2009

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
