// Created on: 2013-11-12
// Created by: Maxim YAKUNIN (myn)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#ifndef NCollection_AccAllocator_HeaderFile
#define NCollection_AccAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>

//!
//! Class  NCollection_AccAllocator  -  accumulating  memory  allocator.  This
//! class  allocates  memory on request returning the pointer to the allocated
//! space.  The  allocation  units  are  grouped  in blocks requested from the
//! system  as  required.  This  memory  is  returned  to  the system when all
//! allocations in a block are freed.
//! 
//! By comparison with  the standard new() and malloc()  calls, this method is
//! faster and consumes very small additional memory to maintain the heap.
//! 
//! By comparison with NCollection_IncAllocator,  this class requires some more
//! additional memory  and a little more time for allocation and deallocation.
//! Memory overhead for NCollection_IncAllocator is 12 bytes per block;
//! average memory overhead for NCollection_AccAllocator is 28 bytes per block.
//! 
//! All pointers  returned by Allocate() are aligned to 4 byte boundaries.
//! To  define  the size  of  memory  blocks  requested  from the OS,  use the
//! parameter of the constructor (measured in bytes).

class NCollection_AccAllocator : public NCollection_BaseAllocator
{
// --------- PUBLIC CONSTANTS ---------
public:
  //! Alignment of all allocated objects: 4 bytes
  static const Standard_Size    Align            = 4;

  //! Default block size
  static const Standard_Size    DefaultBlockSize = 24600;

  //! Number of last blocks to check for free space
  static const Standard_Integer MaxLookupBlocks  = 16;

// ---------- PUBLIC METHODS ----------
public:
  //! Constructor
  Standard_EXPORT NCollection_AccAllocator(const size_t
                                           theBlockSize = DefaultBlockSize);

  //! Destructor
  Standard_EXPORT ~NCollection_AccAllocator();

  //! Allocate memory with given size
  Standard_EXPORT virtual void* Allocate  (const size_t theSize) Standard_OVERRIDE;

  //! Free a previously allocated memory;
  //! memory is returned to the OS when all allocations in some block are freed
  Standard_EXPORT virtual void  Free      (void* theAddress) Standard_OVERRIDE;

// --------- PROTECTED TYPES ---------
protected:
  //! Size value aligned to a 4 byte boundary
  class AlignedSize
  {
    Standard_Size myValue;
  public:
    AlignedSize(){}
    AlignedSize(const Standard_Size theValue)
      : myValue((theValue + Align - 1) & ~(Align - 1)) {}
    operator Standard_Size() const {return myValue;}
  };

  //! A pointer aligned to a 4 byte boundary
  class AlignedPtr
  {
    Standard_Byte* myValue;
  public:
    AlignedPtr(){}
    AlignedPtr(const Standard_Address theValue)
      : myValue((Standard_Byte*)((Standard_Size)theValue & ~(Align - 1))) {}
    operator Standard_Address       () const {return myValue;}
    operator Standard_Byte*         () const {return myValue;}
    AlignedPtr operator -(const AlignedSize theValue) const
      {return myValue - theValue;}
    AlignedPtr operator +(const AlignedSize theValue) const
      {return myValue + theValue;}
    AlignedPtr operator -=(const AlignedSize theValue)
      {return myValue -= theValue;}
    AlignedPtr operator +=(const AlignedSize theValue)
      {return myValue += theValue;}
  };

  //! A key for the map of blocks
  struct Key {Standard_Size Value;};

  //! Key hasher
  class Hasher
  {
  public:
    static Standard_Integer HashCode(const Key theKey, const Standard_Integer theUpper)
    { return theKey.Value % theUpper + 1; }

    static Standard_Boolean IsEqual(const Key theOne, const Key theTwo)
    { return theOne.Value == theTwo.Value; }
  };
  
  //! Descriptor of a block
  struct Block
  {
    Standard_Address address;
    AlignedPtr       allocStart;
    Block*           prevBlock;
    Standard_Integer allocCount;

    Block(const Standard_Address theAddress,
          const Standard_Size    theSize,
          Block*                 thePrevBlock = 0L)
      : address(theAddress), prevBlock(thePrevBlock), allocCount(0)
      {SetFreeSize (theSize);}

    void SetFreeSize(const Standard_Size theSize)
      {allocStart = (Standard_Byte*)address + theSize;}

    Standard_Size FreeSize() const
      {return (Standard_Byte*)allocStart - (Standard_Byte*)address;}

    AlignedPtr Allocate(const AlignedSize theSize)
      {allocCount++; return allocStart -= theSize;}

    void Free()
      {allocCount--;}

    Standard_Boolean IsEmpty() const
      {return allocCount == 0;}
  };

// --------- PROTECTED METHODS ---------
protected:
  //! Calculate a key for the data map basing on the given address
  inline Key getKey(const Standard_Address theAddress) const
  {
    Key aKey = {(Standard_Size)theAddress / myBlockSize};
    return aKey;
  }

  //! Find a block that the given allocation unit belongs to
  Standard_EXPORT Block* findBlock(const Standard_Address theAddress, Key& theKey);

  //! Allocate a new block and return a pointer to it
  Standard_EXPORT Block* allocateNewBlock(const Standard_Size theSize);

// --------- PROHIBITED METHODS ---------
private:
  NCollection_AccAllocator (const NCollection_AccAllocator&);
  NCollection_AccAllocator& operator = (const NCollection_AccAllocator&);

// --------- PROTECTED DATA ---------
protected:
  AlignedSize myBlockSize;
  Block*      mypLastBlock;
  NCollection_DataMap<Key, Block, Hasher> myBlocks;

// Declaration of CASCADE RTTI
public:
  DEFINE_STANDARD_RTTIEXT(NCollection_AccAllocator,NCollection_BaseAllocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NCollection_AccAllocator, NCollection_BaseAllocator)


#endif
