// Created on: 2002-04-12
// Created by: Alexander GRIGORIEV
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

#include <NCollection_IncAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_OutOfMemory.hxx>
#include <stdio.h>
#include <fstream>
#include <iomanip>


IMPLEMENT_STANDARD_RTTIEXT(NCollection_IncAllocator,NCollection_BaseAllocator)

namespace
{

  inline size_t IMEM_SIZE (const size_t theSize)
  {
    return (theSize - 1) / sizeof(NCollection_IncAllocator::aligned_t) + 1;
  }

  inline size_t IMEM_ALIGN (const void* theAddress)
  {
    return sizeof(NCollection_IncAllocator::aligned_t) * IMEM_SIZE (size_t(theAddress));
  }

  #define IMEM_FREE(p_bl) (size_t(p_bl->p_end_block - p_bl->p_free_space))

#ifdef OCCT_DEBUG
  // auxiliary dummy function used to get a place where break point can be set
  inline void place_for_breakpoint() {}
#endif
}

#define MaxLookup 16

static Standard_Boolean IS_DEBUG = Standard_False;

//=======================================================================
/**
 * Static data map (address -> AllocatorID)
 */
//=======================================================================
static NCollection_DataMap<Standard_Address, Standard_Size>& StorageIDMap()
{
  static NCollection_DataMap<Standard_Address, Standard_Size> TheMap;
  return TheMap;
}

//=======================================================================
/**
 * Static map (AllocatorID)
 */
//=======================================================================
static NCollection_Map<Standard_Size>& StorageIDSet()
{
  static NCollection_Map<Standard_Size> TheMap;
  return TheMap;
}

//=======================================================================
//function : IncAllocator_SetDebugFlag
//purpose  : Turn on/off debugging of memory allocation
//=======================================================================

Standard_EXPORT void IncAllocator_SetDebugFlag(const Standard_Boolean theDebug)
{
  IS_DEBUG = theDebug;
}

#ifdef OCCT_DEBUG

//=======================================================================
/**
 * Static value of the current allocation ID. It provides unique
 * numbering of allocators.
 */
//=======================================================================
static Standard_Size CurrentID = 0;
static Standard_Size CATCH_ID = 0;

//=======================================================================
//function : Debug_Create
//purpose  : Store the allocator address in the internal maps
//=======================================================================

static void Debug_Create(Standard_Address theAlloc)
{
  static Standard_Mutex aMutex;
  aMutex.Lock();
  StorageIDMap().Bind(theAlloc, ++CurrentID);
  StorageIDSet().Add(CurrentID);
  if (CurrentID == CATCH_ID)
    place_for_breakpoint();
  aMutex.Unlock();
}

//=======================================================================
//function : Debug_Destroy
//purpose  : Forget the allocator address from the internal maps
//=======================================================================

static void Debug_Destroy(Standard_Address theAlloc)
{
  static Standard_Mutex aMutex;
  aMutex.Lock();
  if (StorageIDMap().IsBound(theAlloc))
  {
    Standard_Size anID = StorageIDMap()(theAlloc);
    StorageIDSet().Remove(anID);
    StorageIDMap().UnBind(theAlloc);
  }
  aMutex.Unlock();
}

#endif /* OCCT_DEBUG */

//=======================================================================
//function : IncAllocator_PrintAlive
//purpose  : Outputs the alive numbers to the file inc_alive.d
//=======================================================================

Standard_EXPORT void IncAllocator_PrintAlive()
{
  if (StorageIDSet().IsEmpty())
  {
    return;
  }

  std::ofstream aFileOut ("inc_alive.d", std::ios_base::trunc | std::ios_base::out);
  if (!aFileOut.is_open())
  {
    std::cout << "failure writing file inc_alive.d" << std::endl;
    return;
  }
  aFileOut.imbue (std::locale ("C"));
  aFileOut << std::fixed << std::setprecision(1);

  aFileOut << "Alive IncAllocators (number, size in Kb)\n";
  Standard_Size    aTotSize = 0;
  Standard_Integer nbAlloc  = 0;
  for (NCollection_DataMap<Standard_Address, Standard_Size>::Iterator itMap (StorageIDMap());
       itMap.More(); itMap.Next())
  {
    const NCollection_IncAllocator* anAlloc = static_cast<NCollection_IncAllocator*>(itMap.Key());
    Standard_Size anID  = itMap.Value();
    Standard_Size aSize = anAlloc->GetMemSize();
    aTotSize += aSize;
    nbAlloc++;
    aFileOut << std::setw(20) << anID << ' '
             << std::setw(20) << (double(aSize) / 1024.0)
             << '\n';
  }
  aFileOut << "Total:\n"
           << std::setw(20) << nbAlloc << ' '
           << std::setw(20) << (double(aTotSize) / 1024.0)
           << '\n';
  aFileOut.close();
}

//=======================================================================
//function : NCollection_IncAllocator()
//purpose  : Constructor
//=======================================================================

NCollection_IncAllocator::NCollection_IncAllocator (const size_t theBlockSize)
{
#ifdef ALLOC_TRACK_USAGE
  printf ("\n..NCollection_IncAllocator: Created (%x)\n",this);
#endif
#ifdef OCCT_DEBUG
  if (IS_DEBUG)
    Debug_Create(this);
#endif
  const size_t aDefault = DefaultBlockSize;
  const size_t aSize = IMEM_SIZE(sizeof(IBlock)) +
      IMEM_SIZE((theBlockSize > 2*sizeof(IBlock)) ? theBlockSize : aDefault);
  IBlock * const aBlock = (IBlock *) malloc (aSize * sizeof(aligned_t));
  myFirstBlock = aBlock;
  mySize = aSize - IMEM_SIZE(sizeof(IBlock));
  myMemSize = aSize * sizeof(aligned_t);
  if (aBlock == NULL)
    throw Standard_OutOfMemory("NCollection_IncAllocator: out of memory");
  aBlock -> p_free_space = (aligned_t *) IMEM_ALIGN (&aBlock[1]);
  aBlock -> p_end_block  = ((aligned_t *) aBlock) + aSize;
  aBlock -> p_next       = NULL;
}

//=======================================================================
//function : ~NCollection_IncAllocator
//purpose  : Destructor
//=======================================================================

NCollection_IncAllocator::~NCollection_IncAllocator ()
{
#ifdef OCCT_DEBUG
  if (IS_DEBUG)
    Debug_Destroy(this);
#endif
  Clean();
  free (myFirstBlock);
}

//=======================================================================
//function : Allocate
//purpose  : allocate a memory
//remark   : returns NULL if allocation fails
//=======================================================================

void * NCollection_IncAllocator::Allocate (const size_t aSize)
{
  aligned_t * aResult = NULL;
  const size_t cSize = aSize ? IMEM_SIZE(aSize) : 0;

  if (cSize > mySize) {
    /* If the requested size exceeds normal allocation size, allocate
       a separate block and place it as the head of the list              */
    aResult = (aligned_t *) allocateNewBlock (cSize+1);
    if (aResult)
      myFirstBlock -> p_free_space = myFirstBlock -> p_end_block;
    else
      throw Standard_OutOfMemory("NCollection_IncAllocator: out of memory");
  } else
    if (cSize <= IMEM_FREE(myFirstBlock)) {
      /* If the requested size fits into the free space in the 1st block  */
      aResult = myFirstBlock -> allocateInBlock (cSize);
    } else {
      /* Search for a block in the list with enough free space            */
      int aMaxLookup = MaxLookup;   /* limit the number of blocks to query */
      IBlock * aCurrentBlock = myFirstBlock -> p_next;
      while (aCurrentBlock && aMaxLookup--) {
        if (cSize <= IMEM_FREE(aCurrentBlock)) {
          aResult = aCurrentBlock -> allocateInBlock (cSize);
          break;
        }
        aCurrentBlock = aCurrentBlock -> p_next;
      }
      if (aResult == NULL) {
        /* There is no available block with enough free space. Create a new
           one and place it in the head of the list                       */
        aResult = (aligned_t *) allocateNewBlock (mySize);
        if (aResult)
          myFirstBlock -> p_free_space = aResult + cSize;
        else
        {
          const size_t aDefault = IMEM_SIZE(DefaultBlockSize);
          if (cSize > aDefault)
              throw Standard_OutOfMemory("NCollection_IncAllocator: out of memory");
          else
          {            
            aResult = (aligned_t *) allocateNewBlock (aDefault);
            if (aResult)
              myFirstBlock -> p_free_space = aResult + cSize;
            else
              throw Standard_OutOfMemory("NCollection_IncAllocator: out of memory");
          }
        }
      }
    }
  return aResult;
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

void * NCollection_IncAllocator::Reallocate (void         * theAddress,
                                             const size_t oldSize,
                                             const size_t newSize)
{
// Check that the dummy parameters are OK
  if (theAddress == NULL || oldSize == 0)
    return Allocate (newSize);
  const size_t cOldSize = IMEM_SIZE(oldSize);
  const size_t cNewSize = newSize ? IMEM_SIZE(newSize) : 0;
  aligned_t * anAddress = (aligned_t *) theAddress;

// We check only the LAST allocation to do the real extension/contraction
  if (anAddress + cOldSize == myFirstBlock -> p_free_space) {
    myFirstBlock -> p_free_space = anAddress;
// If the new size fits into the memory block => OK
// This also includes any case of contraction
    if (cNewSize <= IMEM_FREE(myFirstBlock)) {
      myFirstBlock -> p_free_space += cNewSize;
      return anAddress;
    }
  }
// In case of contraction of non-terminating allocation, do nothing
  else if (cOldSize >= cNewSize)
    return anAddress;
// Extension of non-terminated allocation if there is enough room in the
// current memory block 
  if (cNewSize <= IMEM_FREE(myFirstBlock)) {
    aligned_t * aResult = myFirstBlock -> allocateInBlock (cNewSize);
    if (aResult)
      for (unsigned i = 0; i < cOldSize; i++)
        aResult[i] = anAddress[i];
    return aResult;
  }

// This is either of the cases:
//   - extension of non-terminating allocation, or
//   - extension of terminating allocation when the new size is too big
// In both cases create a new memory block, allocate memory and copy there
// the reallocated memory.
  size_t cMaxSize = mySize > cNewSize ? mySize : cNewSize;
  aligned_t * aResult = (aligned_t *) allocateNewBlock (cMaxSize);
  if (aResult) {
    myFirstBlock -> p_free_space = aResult + cNewSize;
    for (unsigned i = 0; i < cOldSize; i++)
      aResult[i] = anAddress[i];
  }
  else
  {
    throw Standard_OutOfMemory("NCollection_IncAllocator: out of memory");
  }
  return aResult;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void NCollection_IncAllocator::Free (void *)
{}

//=======================================================================
//function : Clean
//purpose  : 
//=======================================================================

void NCollection_IncAllocator::Clean ()
{
#ifdef ALLOC_TRACK_USAGE
  printf ("\n..NCollection_IncAllocator: Memory size to clean:%8.1f kB (%x)\n",
           double(GetMemSize())/1024, this);
#endif
  IBlock * aBlock = myFirstBlock;
  if (aBlock) {
    aBlock -> p_free_space = (aligned_t *) &aBlock[1];
    aBlock = aBlock -> p_next;
    while (aBlock) {
      IBlock * aNext = aBlock -> p_next;
      free (aBlock);
      aBlock = aNext;
    }
    myFirstBlock -> p_next = NULL;
  }
  myMemSize = 0;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void NCollection_IncAllocator::Reset (const Standard_Boolean doReleaseMem)
{
  if (doReleaseMem)
    Clean();
  else {
    Standard_Integer aBlockCount(0);
    IBlock * aBlock = myFirstBlock;
    while (aBlock)
      if (aBlockCount++ < MaxLookup) {
        aBlock -> p_free_space = (aligned_t *) &aBlock[1];
        if (aBlockCount < MaxLookup)
          aBlock = aBlock -> p_next;
        else {
          IBlock * aNext = aBlock -> p_next;
          aBlock -> p_next = NULL;
          aBlock = aNext;
        }
      } else {
        IBlock * aNext = aBlock -> p_next;
        myMemSize -= (aBlock -> p_end_block - (aligned_t *) aBlock) * sizeof (aligned_t);
        free (aBlock);
        aBlock = aNext;
      }
  }
}

//=======================================================================
//function : GetMemSize
//purpose  : diagnostic utility
//=======================================================================

size_t NCollection_IncAllocator::GetMemSize () const
{
//   size_t aResult = 0;
//   IBlock * aBlock = myFirstBlock;
//   while (aBlock) {
//     aResult += (aBlock -> p_end_block - (aligned_t *) aBlock);
//     aBlock = aBlock -> p_next;
//   }
//   return aResult * sizeof (aligned_t);
  return myMemSize;
}

//=======================================================================
//function : allocateNewBlock
//purpose  : 
//=======================================================================

void * NCollection_IncAllocator::allocateNewBlock (const size_t cSize)
{
  aligned_t * aResult = 0L;
  const size_t aSz = cSize + IMEM_SIZE(sizeof(IBlock));
  IBlock * aBlock = (IBlock *) malloc (aSz * sizeof(aligned_t));
  if (aBlock) {
    aBlock -> p_end_block  = ((aligned_t *)aBlock) + aSz;
    aBlock -> p_next = myFirstBlock;
    myFirstBlock = aBlock;
    aResult = (aligned_t *) IMEM_ALIGN(&aBlock[1]);
    myMemSize += aSz * sizeof(aligned_t);
  }
  return aResult;
}
