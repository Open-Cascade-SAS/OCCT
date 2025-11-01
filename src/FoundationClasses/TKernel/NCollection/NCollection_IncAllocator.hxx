// Copyright (c) 2002-2023 OPEN CASCADE SAS
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

#ifndef NCollection_IncAllocator_HeaderFile
#define NCollection_IncAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_OccAllocator.hxx>
#include <NCollection_Allocator.hxx>

#include <utility>

class Standard_Mutex;

/**
 *  Class NCollection_IncAllocator - incremental memory  allocator. This class
 *  allocates  memory  on  request  returning  the  pointer  to  an  allocated
 *  block. This memory is never returned  to the system until the allocator is
 *  destroyed.
 *
 *  By comparison with  the standard new() and malloc()  calls, this method is
 *  faster and consumes very small additional memory to maintain the heap.
 *
 *  All pointers  returned by Allocate() are  aligned to the size  of the data
 *  type "aligned_t". To  modify the size of memory  blocks requested from the
 *  OS,  use the parameter  of the  constructor (measured  in bytes);  if this
 *  parameter is  smaller than  25 bytes on  32bit or  49 bytes on  64bit, the
 *  block size will be the default 12 kbytes.
 *
 *  It is not recommended  to use memory blocks  larger than 16KB  on  Windows
 *  platform  for the repeated operations  because  Low Fragmentation Heap  is
 *  not going to be  used  for  these  allocations  which  may lead  to memory
 *  fragmentation and the general performance slow down.
 *
 *  Note that this allocator is most suitable for single-threaded algorithms
 *  (consider creating dedicated allocators per working thread),
 *  and thread-safety of allocations is DISABLED by default (see SetThreadSafe()).
 */
class NCollection_IncAllocator : public NCollection_BaseAllocator
{
public:
  //! Constructor.
  //! Note that this constructor does NOT setup mutex for using allocator concurrently from
  //! different threads, see SetThreadSafe() method.
  //!
  //! The default size of the memory blocks is 12KB.
  //! It is not recommended to use memory blocks larger than 16KB on Windows
  //! platform for the repeated operations (and thus multiple allocations)
  //! because Low Fragmentation Heap is not going to be used for these allocations,
  //! leading to memory fragmentation and eventual performance slow down.
  Standard_EXPORT NCollection_IncAllocator(const size_t theBlockSize = THE_DEFAULT_BLOCK_SIZE);

  //! Setup mutex for thread-safe allocations.
  Standard_EXPORT void SetThreadSafe(const bool theIsThreadSafe = true);

  //! Allocate memory with given size. Returns NULL on failure
  Standard_EXPORT void* Allocate(const size_t size) Standard_OVERRIDE;

  //! Allocate memory with given size. Returns NULL on failure
  Standard_EXPORT void* AllocateOptimal(const size_t size) Standard_OVERRIDE;

  //! Free a previously allocated memory. Does nothing
  void Free(void*) Standard_OVERRIDE
  {
    // Do nothing
  }

  //! Destructor (calls Clean() internally)
  Standard_EXPORT ~NCollection_IncAllocator();

  //! Re-initialize the allocator so that the next Allocate call should
  //! start allocating in the very beginning as though the allocator is just
  //! constructed. Warning: make sure that all previously allocated data are
  //! no more used in your code!
  //! @param theReleaseMemory
  //!   True - release all previously allocated memory, False - preserve it
  //!   for future allocations.
  Standard_EXPORT void Reset(const bool theReleaseMemory = false);

private:
  // Prohibited methods
  NCollection_IncAllocator(const NCollection_IncAllocator&)            = delete;
  NCollection_IncAllocator& operator=(const NCollection_IncAllocator&) = delete;

public:
  //! Forward list to keep multi-time allocated pointers.
  //! On Reset operation objects will be reused.
  struct IBlock
  {
    IBlock(void* thePointer, const size_t theSize);

    char*   CurPointer;
    size_t  AvailableSize;
    IBlock* NextBlock        = nullptr; //! Pointer to next sorted block
    IBlock* NextOrderedBlock = nullptr; //! Pointer to next ordered block
  };

  //! Description ability to next growing size each 5-th new block
  enum class IBlockSizeLevel : unsigned short
  {
    Min = 0, // x8 growing
    Small,   // x4 growing
    Medium,  // x2 growing
    Large,   // x1.5 growing
    Max      // no growing
  };

protected:
  //! Increases size according current block size level
  void increaseBlockSize();

  //! Resets available size and CurPointer field
  void resetBlock(IBlock* theBlock) const;

  //! Flush all previously allocated data. All pointers returned by
  //! Allocate() become invalid -- be very careful with this
  void clean();

public:
  static constexpr size_t THE_DEFAULT_BLOCK_SIZE = 1024 * 12;

  static constexpr size_t THE_MINIMUM_BLOCK_SIZE = 1024 * 2;

private:
  unsigned int    myBlockSize;                //!< Block size to incremental allocations
  unsigned int    myBlockCount     = 0;       //!< Count of created blocks
  Standard_Mutex* myMutex          = nullptr; //!< Thread-safety mutex
  IBlock*         myAllocationHeap = nullptr; //!< Sorted list for allocations
  IBlock*         myUsedHeap       = nullptr; //!< Sorted list for store empty blocks
  IBlock*         myOrderedBlocks  = nullptr; //!< Ordered list for store growing size blocks

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(NCollection_IncAllocator, NCollection_BaseAllocator)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(NCollection_IncAllocator, NCollection_BaseAllocator)

#endif
