// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _NCollection_AliasedArray_HeaderFile
#define _NCollection_AliasedArray_HeaderFile

#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_Macro.hxx>

//! Defines an array of values of configurable size.
//! For instance, this class allows defining an array of 32-bit or 64-bit integer values with bitness determined in runtime.
//! The element size in bytes (stride) should be specified at construction time.
//! Indexation starts from 0 index.
//! As actual type of element varies at runtime, element accessors are defined as templates.
//! Memory for array is allocated with the given alignment (template parameter).
template<int MyAlignSize = 16>
class NCollection_AliasedArray
{
public:
  DEFINE_STANDARD_ALLOC
public:

  //! Empty constructor.
  NCollection_AliasedArray (Standard_Integer theStride)
  : myData (NULL), myStride (theStride), mySize (0), myDeletable (false)
  {
    if (theStride <= 0) { throw Standard_RangeError ("NCollection_AliasedArray, stride should be positive"); }
  }

  //! Constructor
  NCollection_AliasedArray (Standard_Integer theStride,
                            Standard_Integer theLength)
  : myData (NULL), myStride (theStride), mySize (theLength), myDeletable (true)
  {
    if (theLength <= 0 || myStride <= 0) { throw Standard_RangeError ("NCollection_AliasedArray, stride and length should be positive"); }
    myData = (Standard_Byte* )Standard::AllocateAligned (SizeBytes(), MyAlignSize);
    if (myData == NULL) { throw Standard_OutOfMemory ("NCollection_AliasedArray, allocation failed"); }
  }

  //! Copy constructor 
  NCollection_AliasedArray (const NCollection_AliasedArray& theOther)
  : myData (NULL), myStride (theOther.myStride), mySize (theOther.mySize), myDeletable (false)
  {
    if (mySize != 0)
    {
      myDeletable = true;
      myData = (Standard_Byte* )Standard::AllocateAligned (SizeBytes(), MyAlignSize);
      if (myData == NULL) { throw Standard_OutOfMemory ("NCollection_AliasedArray, allocation failed"); }
      Assign (theOther);
    }
  }

  //! Move constructor
  NCollection_AliasedArray (NCollection_AliasedArray&& theOther) Standard_Noexcept
  : myData (theOther.myData), myStride (theOther.myStride), mySize (theOther.mySize), myDeletable (theOther.myDeletable)
  {
    theOther.myDeletable = false;
  }

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  template<typename Type_t>
  NCollection_AliasedArray (const Type_t& theBegin,
                            Standard_Integer theLength)
  : myData ((Standard_Byte* )&theBegin), myStride ((int )sizeof(Type_t)), mySize (theLength), myDeletable (false)
  {
    if (theLength <= 0) { throw Standard_RangeError ("NCollection_AliasedArray, length should be positive"); }
  }

  //! Returns an element size in bytes.
  Standard_Integer Stride() const { return myStride; }

  //! Size query
  Standard_Integer Size() const { return mySize; }

  //! Length query (the same as Size())
  Standard_Integer Length() const { return mySize; }

  //! Return TRUE if array has zero length.
  Standard_Boolean IsEmpty() const { return mySize == 0; }

  //! Lower bound
  Standard_Integer Lower() const { return 0; }

  //! Upper bound
  Standard_Integer Upper() const { return mySize - 1; }

  //! myDeletable flag
  Standard_Boolean IsDeletable() const { return myDeletable; }

  //! IsAllocated flag - for naming compatibility
  Standard_Boolean IsAllocated() const { return myDeletable; }

  //! Return buffer size in bytes.
  Standard_Size SizeBytes() const { return size_t(myStride) * size_t(mySize); }

  //! Copies data of theOther array to this.
  //! This array should be pre-allocated and have the same length as theOther;
  //! otherwise exception Standard_DimensionMismatch is thrown.
  NCollection_AliasedArray& Assign (const NCollection_AliasedArray& theOther)
  {
    if (&theOther != this)
    {
      if (myStride != theOther.myStride || mySize != theOther.mySize)
      {
        throw Standard_DimensionMismatch ("NCollection_AliasedArray::Assign(), arrays have different size");
      }
      if (myData != NULL)
      {
        memcpy (myData, theOther.myData, SizeBytes());
      }
    }
    return *this;
  }

  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will keep pointer to the memory buffer and
  //! range, but it will not free the buffer on destruction.
  NCollection_AliasedArray& Move (NCollection_AliasedArray& theOther)
  {
    if (&theOther != this)
    {
      if (myDeletable)
      {
        Standard::FreeAligned (myData);
      }
      myStride    = theOther.myStride;
      mySize      = theOther.mySize;
      myDeletable = theOther.myDeletable;
      myData      = theOther.myData;
      theOther.myDeletable = false;
    }
    return *this;
  }

  //! Assignment operator; @sa Assign()
  NCollection_AliasedArray& operator= (const NCollection_AliasedArray& theOther)
  { 
    return Assign (theOther);
  }

  //! Move assignment operator; @sa Move()
  NCollection_AliasedArray& operator= (NCollection_AliasedArray&& theOther)
  {
    return Move (theOther);
  }

  //! Resizes the array to specified bounds.
  //! No re-allocation will be done if length of array does not change,
  //! but existing values will not be discarded if theToCopyData set to FALSE.
  //! @param theLength new length of array
  //! @param theToCopyData flag to copy existing data into new array
  void Resize (Standard_Integer theLength,
               Standard_Boolean theToCopyData)
  {
    if (theLength <= 0) { throw Standard_RangeError ("NCollection_AliasedArray::Resize, length should be positive"); }
    if (mySize == theLength)
    {
      return;
    }

    const Standard_Integer anOldLen  = mySize;
    const Standard_Byte*   anOldData = myData;
    mySize = theLength;
    if (!theToCopyData && myDeletable)
    {
      Standard::FreeAligned (myData);
    }
    myData = (Standard_Byte* )Standard::AllocateAligned (SizeBytes(), MyAlignSize);
    if (myData == NULL) { throw Standard_OutOfMemory ("NCollection_AliasedArray, allocation failed"); }
    if (!theToCopyData)
    {
      myDeletable = true;
      return;
    }

    const size_t aLenCopy = size_t(Min (anOldLen, theLength)) * size_t(myStride);
    memcpy (myData, anOldData, aLenCopy);
    if (myDeletable)
    {
      Standard::FreeAligned (anOldData);
    }
    myDeletable = true;
  }

  //! Destructor - releases the memory
  ~NCollection_AliasedArray()
  { 
    if (myDeletable)
    {
      Standard::FreeAligned (myData);
    }
  }

public:

  //! Access raw bytes of specified element.
  const Standard_Byte* value (Standard_Integer theIndex) const
  {
    Standard_OutOfRange_Raise_if (theIndex < 0 || theIndex >= mySize, "NCollection_AliasedArray::value(), out of range index");
    return myData + size_t(myStride) * size_t(theIndex);
  }

  //! Access raw bytes of specified element.
  Standard_Byte* changeValue (Standard_Integer theIndex)
  {
    Standard_OutOfRange_Raise_if (theIndex < 0 || theIndex >= mySize, "NCollection_AliasedArray::changeValue(), out of range index");
    return myData + size_t(myStride) * size_t(theIndex);
  }

  //! Initialize the items with theValue
  template<typename Type_t> void Init (const Type_t& theValue)
  {
    for (Standard_Integer anIter = 0; anIter < mySize; ++anIter)
    {
      ChangeValue<Type_t> (anIter) = theValue;
    }
  }

  //! Access element with specified position and type.
  //! This method requires size of a type matching stride value.
  template<typename Type_t> const Type_t& Value (Standard_Integer theIndex) const
  {
    Standard_TypeMismatch_Raise_if(size_t(myStride) != sizeof(Type_t), "NCollection_AliasedArray::Value(), wrong type");
    return *reinterpret_cast<const Type_t*>(value (theIndex));
  }

  //! Access element with specified position and type.
  //! This method requires size of a type matching stride value.
  template<typename Type_t> void Value (Standard_Integer theIndex, Type_t& theValue) const
  {
    Standard_TypeMismatch_Raise_if(size_t(myStride) != sizeof(Type_t), "NCollection_AliasedArray::Value(), wrong type");
    theValue = *reinterpret_cast<const Type_t*>(value (theIndex));
  }

  //! Access element with specified position and type.
  //! This method requires size of a type matching stride value.
  template<typename Type_t> Type_t& ChangeValue (Standard_Integer theIndex)
  {
    Standard_TypeMismatch_Raise_if(size_t(myStride) != sizeof(Type_t), "NCollection_AliasedArray::ChangeValue(), wrong type");
    return *reinterpret_cast<Type_t* >(changeValue (theIndex));
  }

  //! Access element with specified position and type.
  //! This method allows wrapping element into smaller type (e.g. to alias 2-components within 3-component vector).
  template<typename Type_t> const Type_t& Value2 (Standard_Integer theIndex) const
  {
    Standard_TypeMismatch_Raise_if(size_t(myStride) < sizeof(Type_t), "NCollection_AliasedArray::Value2(), wrong type");
    return *reinterpret_cast<const Type_t*>(value (theIndex));
  }

  //! Access element with specified position and type.
  //! This method allows wrapping element into smaller type (e.g. to alias 2-components within 3-component vector).
  template<typename Type_t> void Value2 (Standard_Integer theIndex, Type_t& theValue) const
  {
    Standard_TypeMismatch_Raise_if(size_t(myStride) < sizeof(Type_t), "NCollection_AliasedArray::Value2(), wrong type");
    theValue = *reinterpret_cast<const Type_t*>(value (theIndex));
  }

  //! Access element with specified position and type.
  //! This method allows wrapping element into smaller type (e.g. to alias 2-components within 3-component vector).
  template<typename Type_t>
  Type_t& ChangeValue2 (Standard_Integer theIndex)
  {
    Standard_TypeMismatch_Raise_if(size_t(myStride) < sizeof(Type_t), "NCollection_AliasedArray::ChangeValue2(), wrong type");
    return *reinterpret_cast<Type_t* >(changeValue (theIndex));
  }

  //! Return first element
  template<typename Type_t> const Type_t& First() const { return Value<Type_t> (0); }

  //! Return first element
  template<typename Type_t> Type_t& ChangeFirst() { return ChangeValue<Type_t> (0); }

  //! Return last element
  template<typename Type_t> const Type_t& Last() const { return Value<Type_t> (mySize - 1); }

  //! Return last element
  template<typename Type_t> Type_t& ChangeLast() { return Value<Type_t> (mySize - 1); }

protected:

  Standard_Byte*   myData;      //!< data pointer
  Standard_Integer myStride;    //!< element size
  Standard_Integer mySize;      //!< number of elements
  Standard_Boolean myDeletable; //!< flag showing who allocated the array

};

#endif // _NCollection_AliasedArray_HeaderFile
