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

#include <algorithm>
#include <cstdint>
#include <cstring>

//! Defines an array of values of configurable size.
//! For instance, this class allows defining an array of 32-bit or 64-bit integer values with
//! bitness determined in runtime. The element size in bytes (stride) should be specified at
//! construction time. Indexation starts from 0 index. As actual type of element varies at runtime,
//! element accessors are defined as templates. Memory for array is allocated with the given
//! alignment (template parameter).
template <int MyAlignSize = 16>
class NCollection_AliasedArray
{
public:
  DEFINE_STANDARD_ALLOC
public:
  //! Empty constructor.
  NCollection_AliasedArray(size_t theStride)
      : myData(nullptr),
        mySize(0),
        myStride(checkedStride(theStride)),
        myDeletable(false)
  {
  }

  //! Constructor allocating an array with the specified element size and number of elements.
  NCollection_AliasedArray(size_t theStride, size_t theLength)
      : myData(nullptr),
        mySize(theLength),
        myStride(checkedStride(theStride)),
        myDeletable(true)
  {
    if (theLength == 0)
    {
      throw Standard_RangeError("NCollection_AliasedArray, array size is out of supported range");
    }
    myData = (uint8_t*)Standard::AllocateAligned(SizeBytes(), MyAlignSize);
    if (myData == nullptr)
    {
      throw Standard_OutOfMemory("NCollection_AliasedArray, allocation failed");
    }
  }

  //! Legacy integer-length constructor.
  NCollection_AliasedArray(size_t theStride, int theLength)
      : NCollection_AliasedArray(theStride, checkedSize(theLength))
  {
  }

  //! Copy constructor
  NCollection_AliasedArray(const NCollection_AliasedArray& theOther)
      : myData(nullptr),
        mySize(theOther.mySize),
        myStride(theOther.myStride),
        myDeletable(false)
  {
    if (mySize != 0)
    {
      myDeletable = true;
      myData      = (uint8_t*)Standard::AllocateAligned(SizeBytes(), MyAlignSize);
      if (myData == nullptr)
      {
        throw Standard_OutOfMemory("NCollection_AliasedArray, allocation failed");
      }
      memcpy(myData, theOther.myData, SizeBytes());
    }
  }

  //! Move constructor
  NCollection_AliasedArray(NCollection_AliasedArray&& theOther) noexcept
      : myData(theOther.myData),
        mySize(theOther.mySize),
        myStride(theOther.myStride),
        myDeletable(theOther.myDeletable)
  {
    theOther.myDeletable = false;
  }

  //! Constructor wrapping pre-allocated C-array of values without copying them.
  //! @param theBegin pointer to the first array element
  //! @param theLength number of array elements
  template <typename Type_t>
  NCollection_AliasedArray(const Type_t* theBegin, size_t theLength)
      : myData(reinterpret_cast<uint8_t*>(const_cast<Type_t*>(theBegin))),
        mySize(theLength),
        myStride(checkedStride(sizeof(Type_t))),
        myDeletable(false)
  {
    if (theBegin == nullptr || theLength == 0)
    {
      throw Standard_RangeError(
        "NCollection_AliasedArray, array pointer and length should be valid");
    }
  }

  //! Legacy integer-length constructor wrapping a pre-allocated C-array.
  template <typename Type_t>
  NCollection_AliasedArray(const Type_t* theBegin, int theLength)
      : NCollection_AliasedArray(theBegin, checkedSize(theLength))
  {
  }

  //! Returns an element size in bytes.
  uint32_t Stride() const noexcept { return myStride; }

  //! Return the number of elements in the array.
  size_t Size() const noexcept { return mySize; }

  //! Return the number of elements as the legacy integer length.
  int Length() const noexcept { return static_cast<int>(mySize); }

  //! Return TRUE if array has zero length.
  bool IsEmpty() const noexcept { return mySize == 0; }

  //! Lower bound
  int Lower() const { return 0; }

  //! Upper bound
  int Upper() const { return static_cast<int>(mySize) - 1; }

  //! myDeletable flag
  bool IsDeletable() const { return myDeletable; }

  //! IsAllocated flag - for naming compatibility
  bool IsAllocated() const { return myDeletable; }

  //! Return buffer size in bytes.
  size_t SizeBytes() const { return myStride * mySize; }

  //! Copies data of theOther array to this.
  //! This array should be pre-allocated and have the same length as theOther;
  //! otherwise exception Standard_DimensionMismatch is thrown.
  NCollection_AliasedArray& Assign(const NCollection_AliasedArray& theOther)
  {
    if (&theOther != this)
    {
      if (myStride != theOther.myStride || mySize != theOther.mySize)
      {
        throw Standard_DimensionMismatch(
          "NCollection_AliasedArray::Assign(), arrays have different size");
      }
      if (myData != nullptr)
      {
        memcpy(myData, theOther.myData, SizeBytes());
      }
    }
    return *this;
  }

  //! Move assignment.
  //! This array will borrow all the data from theOther.
  //! The moved object will keep pointer to the memory buffer and
  //! range, but it will not free the buffer on destruction.
  NCollection_AliasedArray& Move(NCollection_AliasedArray& theOther)
  {
    if (&theOther != this)
    {
      if (myDeletable)
      {
        Standard::FreeAligned(myData);
      }
      myStride             = theOther.myStride;
      mySize               = theOther.mySize;
      myDeletable          = theOther.myDeletable;
      myData               = theOther.myData;
      theOther.myDeletable = false;
    }
    return *this;
  }

  //! Assignment operator; @sa Assign()
  NCollection_AliasedArray& operator=(const NCollection_AliasedArray& theOther)
  {
    return Assign(theOther);
  }

  //! Move assignment operator; @sa Move()
  NCollection_AliasedArray& operator=(NCollection_AliasedArray&& theOther)
  {
    return Move(theOther);
  }

  //! Resizes the array to specified bounds.
  //! No re-allocation will be done if length of array does not change,
  //! but existing values will not be discarded if theToCopyData set to FALSE.
  //! @param theLength new length of array
  //! @param theToCopyData flag to copy existing data into new array
  void Resize(size_t theLength, bool theToCopyData)
  {
    if (theLength == 0)
    {
      throw Standard_RangeError("NCollection_AliasedArray::Resize, size is out of supported range");
    }
    if (mySize == theLength)
    {
      return;
    }

    const size_t anOldLen  = mySize;
    uint8_t*     anOldData = myData;
    const size_t aNewBytes = myStride * theLength;

    if (!theToCopyData && myDeletable)
    {
      // Discarded owned data can be released before allocation to avoid a temporary memory peak.
      Standard::FreeAligned(myData);
      myData      = nullptr;
      mySize      = 0;
      myDeletable = false;
    }

    uint8_t* aNewData = (uint8_t*)Standard::AllocateAligned(aNewBytes, MyAlignSize);
    if (aNewData == nullptr)
    {
      throw Standard_OutOfMemory("NCollection_AliasedArray, allocation failed");
    }

    if (theToCopyData)
    {
      const size_t aLenCopy = (std::min)(anOldLen, theLength) * myStride;
      if (aLenCopy > 0)
      {
        memcpy(aNewData, anOldData, aLenCopy);
      }
    }
    if (myDeletable)
    {
      Standard::FreeAligned(anOldData);
    }
    myData      = aNewData;
    mySize      = theLength;
    myDeletable = true;
  }

  //! Legacy integer-length resize.
  void Resize(int theLength, bool theToCopyData) { Resize(checkedSize(theLength), theToCopyData); }

  //! Destructor - releases the memory
  ~NCollection_AliasedArray()
  {
    if (myDeletable)
    {
      Standard::FreeAligned(myData);
    }
  }

public:
  //! Return typed pointer to the first element.
  //! This method requires size of a type matching stride value.
  template <typename Type_t>
  const Type_t* Data() const
  {
    Standard_TypeMismatch_Raise_if(myStride != sizeof(Type_t),
                                   "NCollection_AliasedArray::Data(), wrong type");
    return reinterpret_cast<const Type_t*>(myData);
  }

  //! Return mutable typed pointer to the first element.
  //! This method requires size of a type matching stride value.
  template <typename Type_t>
  Type_t* ChangeData()
  {
    Standard_TypeMismatch_Raise_if(myStride != sizeof(Type_t),
                                   "NCollection_AliasedArray::ChangeData(), wrong type");
    return reinterpret_cast<Type_t*>(myData);
  }

  //! Access raw bytes of specified element.
  const uint8_t* value(size_t theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize,
                                 "NCollection_AliasedArray::value(), out of range index");
    return myData + myStride * theIndex;
  }

  //! Access raw bytes at a legacy integer index.
  const uint8_t* value(int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::value(), out of range index");
    return value(static_cast<size_t>(theIndex));
  }

  //! Access raw bytes of specified element.
  uint8_t* changeValue(size_t theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex >= mySize,
                                 "NCollection_AliasedArray::changeValue(), out of range index");
    return myData + myStride * theIndex;
  }

  //! Access raw bytes at a legacy integer index.
  uint8_t* changeValue(int theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::changeValue(), out of range index");
    return changeValue(static_cast<size_t>(theIndex));
  }

  //! Initialize the items with theValue
  template <typename Type_t>
  void Init(const Type_t& theValue)
  {
    std::fill_n(ChangeData<Type_t>(), mySize, theValue);
  }

  //! Access element with specified position and type.
  //! This method requires size of a type matching stride value.
  template <typename Type_t>
  const Type_t& Value(size_t theIndex) const
  {
    Standard_TypeMismatch_Raise_if(myStride != sizeof(Type_t),
                                   "NCollection_AliasedArray::Value(), wrong type");
    return *reinterpret_cast<const Type_t*>(value(theIndex));
  }

  //! Access element at a legacy integer index.
  template <typename Type_t>
  const Type_t& Value(int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::Value(), out of range index");
    return Value<Type_t>(static_cast<size_t>(theIndex));
  }

  //! Access element with specified position and type.
  //! This method requires size of a type matching stride value.
  template <typename Type_t>
  void Value(size_t theIndex, Type_t& theValue) const
  {
    Standard_TypeMismatch_Raise_if(myStride != sizeof(Type_t),
                                   "NCollection_AliasedArray::Value(), wrong type");
    theValue = *reinterpret_cast<const Type_t*>(value(theIndex));
  }

  //! Access element at a legacy integer index.
  template <typename Type_t>
  void Value(int theIndex, Type_t& theValue) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::Value(), out of range index");
    Value<Type_t>(static_cast<size_t>(theIndex), theValue);
  }

  //! Access element with specified position and type.
  //! This method requires size of a type matching stride value.
  template <typename Type_t>
  Type_t& ChangeValue(size_t theIndex)
  {
    Standard_TypeMismatch_Raise_if(myStride != sizeof(Type_t),
                                   "NCollection_AliasedArray::ChangeValue(), wrong type");
    return *reinterpret_cast<Type_t*>(changeValue(theIndex));
  }

  //! Access element at a legacy integer index.
  template <typename Type_t>
  Type_t& ChangeValue(int theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::ChangeValue(), out of range index");
    return ChangeValue<Type_t>(static_cast<size_t>(theIndex));
  }

  //! Access element with specified position and type.
  //! This method allows wrapping element into smaller type (e.g. to alias 2-components within
  //! 3-component vector).
  template <typename Type_t>
  const Type_t& Value2(size_t theIndex) const
  {
    Standard_TypeMismatch_Raise_if(myStride < sizeof(Type_t),
                                   "NCollection_AliasedArray::Value2(), wrong type");
    return *reinterpret_cast<const Type_t*>(value(theIndex));
  }

  //! Access element at a legacy integer index using a type not larger than the stride.
  template <typename Type_t>
  const Type_t& Value2(int theIndex) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::Value2(), out of range index");
    return Value2<Type_t>(static_cast<size_t>(theIndex));
  }

  //! Access element with specified position and type.
  //! This method allows wrapping element into smaller type (e.g. to alias 2-components within
  //! 3-component vector).
  template <typename Type_t>
  void Value2(size_t theIndex, Type_t& theValue) const
  {
    Standard_TypeMismatch_Raise_if(myStride < sizeof(Type_t),
                                   "NCollection_AliasedArray::Value2(), wrong type");
    theValue = *reinterpret_cast<const Type_t*>(value(theIndex));
  }

  //! Access element at a legacy integer index using a type not larger than the stride.
  template <typename Type_t>
  void Value2(int theIndex, Type_t& theValue) const
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::Value2(), out of range index");
    Value2<Type_t>(static_cast<size_t>(theIndex), theValue);
  }

  //! Access element with specified position and type.
  //! This method allows wrapping element into smaller type (e.g. to alias 2-components within
  //! 3-component vector).
  template <typename Type_t>
  Type_t& ChangeValue2(size_t theIndex)
  {
    Standard_TypeMismatch_Raise_if(myStride < sizeof(Type_t),
                                   "NCollection_AliasedArray::ChangeValue2(), wrong type");
    return *reinterpret_cast<Type_t*>(changeValue(theIndex));
  }

  //! Access element at a legacy integer index using a type not larger than the stride.
  template <typename Type_t>
  Type_t& ChangeValue2(int theIndex)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_AliasedArray::ChangeValue2(), out of range index");
    return ChangeValue2<Type_t>(static_cast<size_t>(theIndex));
  }

  //! Return first element
  template <typename Type_t>
  const Type_t& First() const
  {
    return Value<Type_t>(0);
  }

  //! Return first element
  template <typename Type_t>
  Type_t& ChangeFirst()
  {
    return ChangeValue<Type_t>(0);
  }

  //! Return last element
  template <typename Type_t>
  const Type_t& Last() const
  {
    return Value<Type_t>(mySize - 1);
  }

  //! Return last element
  template <typename Type_t>
  Type_t& ChangeLast()
  {
    return ChangeValue<Type_t>(mySize - 1);
  }

protected:
  //! Convert a legacy integer size to the canonical size type.
  static size_t checkedSize(int theSize)
  {
    if (theSize <= 0)
    {
      throw Standard_RangeError("NCollection_AliasedArray, length should be positive");
    }
    return static_cast<size_t>(theSize);
  }

  //! Validate and compactly store an element stride.
  static uint32_t checkedStride(size_t theStride)
  {
    if (theStride == 0)
    {
      throw Standard_RangeError("NCollection_AliasedArray, stride should be positive");
    }
    return static_cast<uint32_t>(theStride);
  }

protected:
  uint8_t* myData;      //!< data pointer
  size_t   mySize;      //!< number of elements
  uint32_t myStride;    //!< element size
  bool     myDeletable; //!< flag showing who allocated the array
};

#endif // _NCollection_AliasedArray_HeaderFile
