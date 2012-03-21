// Created on: 2002-04-12
// Created by: Alexander GRIGORIEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#include <NCollection_IncAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_OutOfMemory.hxx>
#include <stdio.h>

IMPLEMENT_STANDARD_HANDLE  (NCollection_IncAllocator,NCollection_BaseAllocator)
IMPLEMENT_STANDARD_RTTIEXT (NCollection_IncAllocator,NCollection_BaseAllocator)

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

};

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
  Standard_Boolean isReentrant = Standard::IsReentrant();
  if (isReentrant)
    aMutex.Lock();
  StorageIDMap().Bind(theAlloc, ++CurrentID);
  StorageIDSet().Add(CurrentID);
  if (isReentrant)
    aMutex.Unlock();
  if (CurrentID == CATCH_ID)
  {
    // Place for break point for creation of investigated allocator
    int a = 1;
  }
}

//=======================================================================
//function : Debug_Destroy
//purpose  : Forget the allocator address from the internal maps
//=======================================================================

static void Debug_Destroy(Standard_Address theAlloc)
{
  static Standard_Mutex aMutex;
  Standard_Boolean isReentrant = Standard::IsReentrant();
  if (isReentrant)
    aMutex.Lock();
  if (StorageIDMap().IsBound(theAlloc))
  {
    Standard_Size anID = StorageIDMap()(theAlloc);
    StorageIDSet().Remove(anID);
    StorageIDMap().UnBind(theAlloc);
  }
  if (isReentrant)
    aMutex.Unlock();
}

//=======================================================================
//function : IncAllocator_PrintAlive
//purpose  : Outputs the alive numbers to the file inc_alive.d
//=======================================================================

Standard_EXPORT void IncAllocator_PrintAlive()
{
  if (!StorageIDSet().IsEmpty())
  {
    FILE * ff = fopen("inc_alive.d", "wt");
    if (ff == NULL)
    {
      cout << "failure writing file inc_alive.d" << endl;
    }
    else
    {
      fprintf(ff, "Alive IncAllocators (number, size in Kb)\n");
      NCollection_DataMap<Standard_Address, Standard_Size>::Iterator
        itMap(StorageIDMap());
      Standard_Size aTotSize = 0;
      Standard_Integer nbAlloc = 0;
      for (; itMap.More(); itMap.Next())
      {
        NCollection_IncAllocator* anAlloc =
          static_cast<NCollection_IncAllocator*>(itMap.Key());
        Standard_Size anID = itMap.Value();
        Standard_Size aSize = anAlloc->GetMemSize();
        aTotSize += aSize;
        nbAlloc++;
        fprintf(ff, "%-8d %8.1f\n", anID, double(aSize)/1024);
      }
      fprintf(ff, "Total:\n%-8d %8.1f\n", nbAlloc, double(aTotSize)/1024);
      fclose(ff);
    }
  }
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
#ifdef DEB
  if (IS_DEBUG)
    Debug_Create(this);
#endif
  const size_t aSize = IMEM_SIZE(sizeof(IBlock)) +
      IMEM_SIZE((theBlockSize > 2*sizeof(IBlock)) ? theBlockSize : 24600);
  IBlock * const aBlock = (IBlock *) malloc (aSize * sizeof(aligned_t));
  myFirstBlock = aBlock;
  mySize = aSize;
  myMemSize = aSize * sizeof(aligned_t);
  if (aBlock == NULL)
    Standard_OutOfMemory::Raise("NCollection_IncAllocator: out of memory");
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
#ifdef DEB
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
  aligned_t * aResult = (aligned_t *) allocateNewBlock (mySize);
  if (aResult) {
    myFirstBlock -> p_free_space = aResult + cNewSize;
    for (unsigned i = 0; i < cOldSize; i++)
      aResult[i] = anAddress[i];
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
  else
    Standard_OutOfMemory::Raise("NCollection_IncAllocator: out of memory");
  return aResult;
}
