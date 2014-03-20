// Created on: 2005-03-15
// Created by: Peter KURNEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Standard_MMgrOpt_HeaderFile
#define _Standard_MMgrOpt_HeaderFile

#include <Standard_MMgrRoot.hxx>
#include <Standard_Mutex.hxx>

/**
* @brief Open CASCADE memory manager optimized for speed.
*
* The behaviour is different for memory blocks of different sizes,
* according to specified options provided to constructor:
*
* - Small blocks with size less than or equal to aCellSize are allocated 
*   in big pools of memory. The parameter aNbPages specifies size of 
*   these pools in pages (operating system-dependent). 
*   When freed, small block is not returned to the system but added
*   into free blocks list and reused when block of the same size is 
*   requested.
* 
* - Medium size blocks with size less than aThreshold are allocated 
*   using malloc() or calloc() function but not returned to the system
*   when method Free() is called; instead they are put into free list
*   and reused when block of the same size is requested.
*   Blocks of medium size stored in free lists can be released to the 
*   system (by free()) by calling method Purge().
*
* - Large blocks with size greater than or equal to aThreshold are allocated 
*   and freed directly: either using malloc()/calloc() and free(), or using 
*   memory mapped files (if option aMMap is True)
*   
* Thus the optimization of memory allocation/deallocation is reached 
* for small and medium size blocks using free lists method; 
* note that space allocated for small blocks cannot be (currently) released
* to the system while space for medium size blocks can be released by method Purge().
*
* Note that destructor of that class frees all free lists and memory pools 
* allocated for small blocks.
* 
* Note that size of memory blocks allocated by this memory manager is always 
* rounded up to 16 bytes. In addition, 8 bytes are added at the beginning 
* of the memory block to hold auxiliary information (size of the block when
* in use, or pointer to the next free block when in free list).
* This the expense of speed optimization. At the same time, allocating small 
* blocks is usually less costly than directly by malloc since allocation is made
* once (when allocating a pool) and overheads induced by malloc are minimized.
*/
class Standard_MMgrOpt : public Standard_MMgrRoot
{
 public:
  
  //! Constructor. If aClear is True, the allocated emmory will be 
  //! nullified. For description of other parameters, see description 
  //! of the class above.
  Standard_EXPORT Standard_MMgrOpt
                        (const Standard_Boolean aClear      = Standard_True,
                         const Standard_Boolean aMMap       = Standard_True,
                         const Standard_Size    aCellSize   = 200,
                         const Standard_Integer aNbPages    = 10000,
                         const Standard_Size    aThreshold  = 40000);

  //! Frees all free lists and pools allocated for small blocks 
  Standard_EXPORT virtual ~Standard_MMgrOpt();
  
  //! Allocate aSize bytes; see class description above
  Standard_EXPORT virtual Standard_Address Allocate(const Standard_Size aSize);
  
  //! Reallocate previously allocated aPtr to a new size; new address is returned.
  //! In case that aPtr is null, the function behaves exactly as Allocate.
  Standard_EXPORT virtual Standard_Address Reallocate (Standard_Address thePtr, 
                                                       const Standard_Size theSize);
  
  //! Free previously allocated block.
  //! Note that block can not all blocks are released to the OS by this 
  //! method (see class description)
  Standard_EXPORT virtual void Free (Standard_Address thePtr);
  
  //! Release medium-sized blocks of memory in free lists to the system.
  //! Returns number of actually freed blocks
  Standard_EXPORT virtual Standard_Integer Purge(Standard_Boolean isDestroyed);

  //! Declaration of a type pointer to the callback function that should accept the following arguments:
  //! @param theIsAlloc   true if the data is allocated, false if it is freed
  //! @param theStorage   address of the allocated/freed block
  //! @param theRoundSize the real rounded size of the block
  //! @param theSize      the size of the block that was requested by application (this value is correct only if theIsAlloc is true)
  typedef void (*TPCallBackFunc)(const Standard_Boolean theIsAlloc,
                                 const Standard_Address theStorage,
                                 const Standard_Size theRoundSize,
                                 const Standard_Size theSize);

  //! Set the callback function. You may pass 0 there to turn off the callback.
  //! The callback function, if set, will be automatically called from within
  //! Allocate and Free methods.
  Standard_EXPORT static void SetCallBackFunction(TPCallBackFunc pFunc);

protected:
 
  //! Internal - initialization of buffers
  Standard_EXPORT void Initialize();

  //! Internal - allocation of memory using either malloc or memory mapped files.
  //! The size of the actually allocated block may be greater than requested one
  //! when memory mapping is used, since it is aligned to page size 
  Standard_Size* AllocMemory (Standard_Size &aSize);
  
  //! Internal - deallocation of memory taken by AllocMemory
  void FreeMemory (Standard_Address aPtr, const Standard_Size aSize);
  
  //! Internal - free memory pools allocated for small size blocks
  void FreePools();

 protected:
  Standard_Boolean myClear;         //!< option to clear allocated memory
  
  Standard_Size    myFreeListMax;   //!< last allocated index in the free blocks list
  Standard_Size ** myFreeList;      //!< free blocks list 

  Standard_Size    myCellSize;      //!< small blocks size
  Standard_Integer myNbPages;       //!< size (pages) for small block memory pools
  Standard_Size    myPageSize;      //!< system-dependent memory page size
  Standard_Size *  myAllocList;     //!< list of memory pools for small blocks
  Standard_Size *  myNextAddr;      //!< next free address in the active memory pool
  Standard_Size *  myEndBlock;      //!< end of the active memory pool
  
  Standard_Integer myMMap;          //!< non-null if using memory mapped files for allocation of large blocks
  Standard_Size    myThreshold;     //!< large block size  
  
  Standard_Mutex   myMutex;         //!< Mutex to protect free lists data
  Standard_Mutex   myMutexPools;    //!< Mutex to protect small block pools data
};

#endif
