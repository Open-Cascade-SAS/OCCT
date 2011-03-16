// File:      NCollection_IncAllocator.cxx
// Created:   12.04.02 21:16:26
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2002

#include <NCollection_IncAllocator.hxx>
#include <Standard_OutOfMemory.hxx>
#include <stdio.h>

IMPLEMENT_STANDARD_HANDLE  (NCollection_IncAllocator,NCollection_BaseAllocator)
IMPLEMENT_STANDARD_RTTIEXT (NCollection_IncAllocator,NCollection_BaseAllocator)

#define IMEM_SIZE(_size) ((((_size) - 1)/sizeof(aligned_t)) + 1)
#define IMEM_FREE(p_bl) ((unsigned int)(p_bl->p_end_block - p_bl->p_free_space))
#define IMEM_ALIGN(_addr) (sizeof(aligned_t)* IMEM_SIZE((unsigned long)(_addr)))

#define MaxLookup 16

//=======================================================================
//function : NCollection_IncAllocator()
//purpose  : Constructor
//=======================================================================

NCollection_IncAllocator::NCollection_IncAllocator (const size_t theBlockSize)
{
#ifdef ALLOC_TRACK_USAGE
  printf ("\n..NCollection_IncAllocator: Created (%x)\n",this);
#endif
  const size_t aSize = IMEM_SIZE(sizeof(IBlock)) +
      IMEM_SIZE((theBlockSize > 2*sizeof(IBlock)) ? theBlockSize : 24600);
  IBlock * const aBlock = (IBlock *) malloc (aSize * sizeof(aligned_t));
  myFirstBlock = aBlock;
  mySize = aSize;
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
  size_t aResult = 0;
  IBlock * aBlock = myFirstBlock;
  while (aBlock) {
    aResult += (aBlock -> p_end_block - (aligned_t *) aBlock);
    aBlock = aBlock -> p_next;
  }
  return aResult * sizeof (aligned_t);
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
  }
  else
    Standard_OutOfMemory::Raise("NCollection_IncAllocator: out of memory");
  return aResult;
}
