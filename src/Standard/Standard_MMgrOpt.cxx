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

#ifdef _WIN32
#include <windows.h>
#endif

#include <Standard_MMgrOpt.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_Assert.hxx>

#include <stdio.h>
#include <errno.h>

#ifndef _WIN32
# include <sys/mman.h>    /* mmap() */
#endif

#include <fcntl.h>
//
#if defined (__sun) || defined(SOLARIS)
extern "C" int getpagesize() ;
#endif

#ifdef _WIN32
#include <Strsafe.h>
#endif
//======================================================================
// Assumptions
//======================================================================

// This implementation makes a number of assumptions regarding size of 
// types:
//
// sizeof(Standard_Size) == sizeof(Standard_Address==void*)
//
// On WNT, sizeof(HANDLE) is equal of multiple of sizeof(Standard_Size)

//======================================================================
// Naming conventions
//======================================================================

// For clarity of implementation, the following conventions are used
// for naming variables:
// 
// ...Size: size in bytes
//
// RoundSize, RSize etc.: size in bytes, rounded according to allocation granularity
//
// ...SizeN: size counted in number of items of sizeof(Standard_Size) bytes each
//
// ...Storage: address of the user area of the memory block (Standard_Address)
//
// ...Block: address of the hole memory block (header) (Standard_Size*)

//======================================================================
// Macro definitions
//======================================================================

//     
// MMAP_BASE_ADDRESS,  MMAP_FLAGS
#if defined (__hpux) || defined(HPUX)
#define MMAP_BASE_ADDRESS 0x80000000
#define MMAP_FLAGS (MAP_ANONYMOUS | MAP_PRIVATE | MAP_VARIABLE)
#elif defined (__osf__) || defined(DECOSF1)
#define MMAP_BASE_ADDRESS 0x1000000000
#define MMAP_FLAGS (MAP_ANONYMOUS | MAP_PRIVATE | MAP_VARIABLE)
#elif defined(_AIX)
#define MMAP_BASE_ADDRESS  0x80000000
#define MMAP_FLAGS (MAP_ANONYMOUS | MAP_PRIVATE | MAP_VARIABLE)
#elif defined(__APPLE__)
#define MMAP_BASE_ADDRESS  0x80000000
#define MMAP_FLAGS (MAP_ANON | MAP_PRIVATE)
#elif defined(__linux__)
#define MMAP_BASE_ADDRESS 0x20000000
#define MMAP_FLAGS (MAP_PRIVATE)
#elif defined(_WIN32)
//static HANDLE myhMap;
#else
#define MMAP_BASE_ADDRESS 0x60000000
#define MMAP_FLAGS (MAP_PRIVATE)
#endif

// Round size up to the specified page size
#define PAGE_ALIGN(size,thePageSize)                            \
  (((size) + (thePageSize) - 1) &  ~((thePageSize) - 1))

// Round size up to 4, 8, or 16 bytes
// Note that 0 yields 0
#define ROUNDUP16(size)                (((size) + 0xf) & ~(Standard_Size)0xf)
#define ROUNDUP8(size)                 (((size) + 0x7) & ~(Standard_Size)0x7)
#define ROUNDUP4(size)                 (((size) + 0x3) & ~(Standard_Size)0x3)
#define ROUNDDOWN8(size)               ((size) & ~(Standard_Size)0x7)

// The following two macros define granularity of memory allocation,
// by rounding size to the size of the allocation cell,
// and obtaining cell index from rounded size.
// Note that granularity shall be not less than sizeof(Standard_Size)

// Traditional implementation: granularity 16 bytes
//#define ROUNDUP_CELL(size)             ROUNDUP16(size)
//#define INDEX_CELL(rsize)              ((rsize) >> 4)

// Reduced granularity: 8 bytes
#define ROUNDUP_CELL(size)             ROUNDUP8(size)
#define ROUNDDOWN_CELL(size)           ROUNDDOWN8(size)
#define INDEX_CELL(rsize)              ((rsize) >> 3)

/* In the allocated block, first bytes are used for storing of memory manager's data.
   (size of block). The minimal size of these data is sizeof(int).
   The memory allocated in system usually alligned by 16 bytes.Tthe aligment of the 
   data area in the memory block is shfted on BLOCK_SHIFT*sizeof(Standard_Size) 
   bytes.
   It is OK for WNT, SUN and Linux systems, but on SGI aligment should be 8 bytes.
   So, BLOCK_SHIFT is formed as macro for support on other possible platforms.
*/

#if defined(IRIX) || defined(SOLARIS)
#define BLOCK_SHIFT 2
#else
#define BLOCK_SHIFT 1
#endif

// Get address of user area from block address, and vice-versa
#define GET_USER(block)    (((Standard_Size*)(block)) + BLOCK_SHIFT)
#define GET_BLOCK(storage) (((Standard_Size*)(storage))-BLOCK_SHIFT)

//=======================================================================
//function : Standard_MMgr
//purpose  : 
//=======================================================================

Standard_MMgrOpt::Standard_MMgrOpt(const Standard_Boolean aClear,
                                   const Standard_Boolean aMMap,
                                   const Standard_Size aCellSize,
                                   const Standard_Integer aNbPages,
                                   const Standard_Size aThreshold)
{
  // check basic assumption
  Standard_STATIC_ASSERT(sizeof(Standard_Size) == sizeof(Standard_Address));

  // clear buffer fields
  myFreeListMax = 0;
  myFreeList = NULL;
  myPageSize = 0;
  myAllocList = NULL;
  myNextAddr = NULL;
  myEndBlock = NULL;

  // initialize parameters
  myClear = aClear;
  myMMap = (Standard_Integer)aMMap;
  myCellSize = aCellSize;
  myNbPages = aNbPages;
  myThreshold = aThreshold;
  
  // initialize 
  Initialize();
}

//=======================================================================
//function : ~Standard_MMgrOpt
//purpose  : 
//=======================================================================

Standard_MMgrOpt::~Standard_MMgrOpt()
{
  Purge(Standard_True);
  free(myFreeList);
  
  // NOTE: freeing pools may be dangerous if not all memory taken by 
  //       this instance of the memory manager has been freed 
  FreePools();
}

// interface level

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Standard_MMgrOpt::Initialize()
{
  // check number of pages in small blocks pools 
  if ( myNbPages < 100 ) 
    myNbPages = 1000;
  
  // get system-dependent page size
#ifndef _WIN32
  myPageSize = getpagesize();
  if ( ! myPageSize )
    myMMap = 0;
#else
  SYSTEM_INFO SystemInfo;
  GetSystemInfo (&SystemInfo);
  myPageSize = SystemInfo.dwPageSize;
#endif

  // initialize memory mapped files
  if(myMMap) {
#if defined (__sgi) || defined(IRIX)
    /* Probleme de conflit en la zone des malloc et la zone des mmap sur SGI */
    /* Ce probleme a ete identifie en IRIX 5.3 jusqu'en  IRIX 6.2. Le probleme */
    /* ne semble pas apparaitre en IRIX 6.4 */
    /* Les malloc successifs donnent des adresses croissantes (a partir de 0x0x10000000) */
    /* ce que l'on appelle le pointeur de BREAK */
    /* Le premier mmap est force a l'addresse MMAP_BASE_ADDRESS (soit 0x60000000 sur SGI) */
    /* mais les mmap suivants sont decides par le systeme (flag MAP_VARIABLE). Malheureusement */
    /* il renvoie une addresse la plus basse possible dans la zone des malloc juste au dessus */
    /* du BREAK soit 0x18640000 ce qui donne un espace d'allocation d'environ 140 Mo pour les */
    /* malloc. Sur des gros modeles on peut avoir des pointes a 680 Mo en Rev6 pour une maquette */
    /* de 2 000 000 de points. En Rev7, la meme maquette n'excedera pas 286 Mo (voir vision.for) */
    /* Pour palier ce comportement, la solution adoptee est la suivante :                        */
    /*   Lorsque l'on entre dans alloc_startup (ici), on n'a pas encore fait de mmap.            */
    /*   On fait alors un malloc (d'environ 700Mo) que l'on libere de suite. Cela a pour         */
    /*  consequence de deplacer le BREAK tres haut. Le BREAK ne redescend jamais meme lors du free */
    /*  Le mmap donnant une adresse (environ 100 Mo au dessus du BREAK) on se retrouve alors avec */
    /* le partage des zones de memoire suivant :                                                  */
    /*   700 Mo pour les malloc  - 500 Mo (1,2Go - 700Mo )  pour les mmap. Avec un CLD_SD_SIZE  */
    /* de 2 000 000 on atteind jamais 500 Mo de mmap, meme en chargeant des applications (qui   */
    /* utilisent la zone de mmap                                                                    */
    /* Ce partage des zones memoire pourra eventuellemt etre regle par une variable d'environnement */
    /* CLD_HIGH_SBRK                                                                                */
    char *var;
    Standard_Size high_sbrk;
    
    high_sbrk = 700*1024*1024;
    if ( (var=getenv("CLD_HIGH_SBRK")) != NULL ) {
      high_sbrk = atoi(var);
    }

    var = (char*)malloc(high_sbrk); // 700 Mb
    if ( var )
      free(var);
    else
      perror("ERR_MEMRY_FAIL");
#endif
    
#if defined(IRIX) || defined(__sgi) || defined(SOLARIS) || defined(__sun) || defined(__linux__) || defined(__FreeBSD__) || defined(__ANDROID__)
    if ((myMMap = open ("/dev/zero", O_RDWR)) < 0) {
      if ((myMMap = open ("/dev/null", O_RDWR)) < 0){
        myMMap = 0;
      }
    }
    if (!myMMap)
      perror("ERR_MMAP_FAIL");
#else
    myMMap = -1;
#endif
  }
  
  // initialize free lists
  myFreeListMax = INDEX_CELL(ROUNDUP_CELL(myThreshold-BLOCK_SHIFT)); // all blocks less than myThreshold are to be recycled
  myFreeList = (Standard_Size **) calloc (myFreeListMax+1, sizeof(Standard_Size *));
  myCellSize = ROUNDUP16(myCellSize);
}

//=======================================================================
//function : SetMMgrOptCallBack
//purpose  : Sets a callback function to be called on each alloc/free
//=======================================================================

static Standard_MMgrOpt::TPCallBackFunc MyPCallBackFunc = NULL;

Standard_EXPORT void Standard_MMgrOpt::SetCallBackFunction(TPCallBackFunc pFunc)
{
  MyPCallBackFunc = pFunc;
}

inline void callBack(const Standard_Boolean isAlloc,
                     const Standard_Address aStorage,
                     const Standard_Size aRoundSize,
                     const Standard_Size aSize)
{
  if (MyPCallBackFunc)
    (*MyPCallBackFunc)(isAlloc, aStorage, aRoundSize, aSize);
}

//=======================================================================
//function : Allocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrOpt::Allocate(const Standard_Size aSize)
{
  Standard_Size * aStorage = NULL;
  
  // round up size according to allocation granularity
  // The keyword 'volatile' is only used here for GCC 64-bit compilations
  // otherwise this method would crash in runtime in optimized build.
  volatile Standard_Size RoundSize = ROUNDUP_CELL(aSize);
  const Standard_Size Index = INDEX_CELL(RoundSize);

  // blocks of small and medium size are recyclable
  if ( Index <= myFreeListMax ) {
    const Standard_Size RoundSizeN = RoundSize / sizeof(Standard_Size);

    // Lock access to critical data (myFreeList and other fields) by mutex.
    // Note that we do not lock fields that do not change during the 
    // object life (such as myThreshold), and assume that calls to functions 
    // of standard library are already protected by their implementation.
    // The unlock is called as soon as possible, for every treatment case.
    // We also do not use Sentry, since in case if OCC signal or exception is
    // caused by this block we will have deadlock anyway...
    myMutex.Lock();
    
    // if free block of the requested size is available, return it
    if ( myFreeList[Index] ) {
      // the address of the next free block is stored in the header
      // of the memory block; use it to update list pointer
      // to point to next free block
      Standard_Size* aBlock = myFreeList[Index];
      myFreeList[Index] = *(Standard_Size**)aBlock;

      // unlock the mutex
      myMutex.Unlock();

      // record size of the allocated block in the block header and
      // shift the pointer to the beginning of the user part of block
      aBlock[0] = RoundSize;
      aStorage = GET_USER(aBlock);

      // clear block if requested
      if (myClear)
        memset (aStorage, 0, RoundSize);
    }
    // else if block size is small allocate it in pools
    else if ( RoundSize <= myCellSize ) {
      // unlock the mutex for free lists 
      myMutex.Unlock();

      // and lock the specific mutex used to protect access to small blocks pools;
      // note that this is done by sentry class so as to ensure unlocking in case of 
      // possible exception that may be thrown from AllocMemory()
      Standard_Mutex::Sentry aSentry (myMutexPools);

      // check for availability of requested space in the current pool
      Standard_Size *aBlock = myNextAddr;
      if ( &aBlock[ BLOCK_SHIFT+RoundSizeN] > myEndBlock ) {
        // otherwise, allocate new memory pool with page-aligned size
        Standard_Size Size = myPageSize * myNbPages;
        aBlock = AllocMemory(Size); // note that size may be aligned by this call

        if (myEndBlock > myNextAddr) {
          // put the remaining piece to the free lists
          const Standard_Size aPSize = (myEndBlock - GET_USER(myNextAddr))
            * sizeof(Standard_Size);
          const Standard_Size aRPSize = ROUNDDOWN_CELL(aPSize);
          const Standard_Size aPIndex = INDEX_CELL(aRPSize);
          if ( aPIndex > 0 && aPIndex <= myFreeListMax ) {
            myMutex.Lock();
            *(Standard_Size**)myNextAddr = myFreeList[aPIndex];
            myFreeList[aPIndex] = myNextAddr;
            myMutex.Unlock();
          }
        }

        // set end pointer to the end of the new pool
        myEndBlock = aBlock + Size / sizeof(Standard_Size);
        // record in the first bytes of the pool the address of the previous one
        *(Standard_Size**)aBlock = myAllocList;
        // and make new pool current (last)
        // and get pointer to the first memory block in the pool
        myAllocList = aBlock;
	aBlock+=BLOCK_SHIFT;
      }

      // initialize header of the new block by its size
      // and get the pointer to the user part of block
      aBlock[0] = RoundSize;
      aStorage = GET_USER(aBlock);

      // and advance pool pointer to the next free piece of pool
      myNextAddr = &aStorage[RoundSizeN];
    }
    // blocks of medium size are allocated directly
    else {
      // unlock the mutex immediately, as we do not need further to access any field
      myMutex.Unlock();

      // we use operator ?: instead of if() since it is faster
      Standard_Size *aBlock = (Standard_Size*) (myClear ? calloc( RoundSizeN+BLOCK_SHIFT,   sizeof(Standard_Size)) :
                                                          malloc((RoundSizeN+BLOCK_SHIFT) * sizeof(Standard_Size)) );

      // if allocation failed, try to free some memory by purging free lists, and retry
      if ( ! aBlock ) {
        if ( Purge (Standard_False) )
          aBlock = (Standard_Size*)calloc(RoundSizeN+BLOCK_SHIFT, sizeof(Standard_Size));
        // if still not succeeded, raise exception
        if ( ! aBlock )
          throw Standard_OutOfMemory("Standard_MMgrOpt::Allocate(): malloc failed");
      }

      // initialize new block header by its size
      // and get the pointer to the user part of block
      aBlock[0] = RoundSize;
      aStorage = GET_USER(aBlock);
    }
  }
  // blocks of big size may be allocated as memory mapped files
  else {
    // Compute size of the block to be allocated, including header,
    // Note that we use rounded size, even if this block will not be stored in 
    // the free list, for consistency of calls to AllocMemory() / FreeMemory()
    // and calculation of index in the free list
    Standard_Size AllocSize = RoundSize + sizeof(Standard_Size);

    // allocate memory
    Standard_Size* aBlock = AllocMemory(AllocSize);

    // initialize new block header by its size
    // and get the pointer to the user part of block.
    aBlock[0] = RoundSize;
    aStorage = GET_USER(aBlock);
  }

  callBack(Standard_True, aStorage, RoundSize, aSize);

  return aStorage;
}

//=======================================================================
//function : Free
//purpose  : 
//=======================================================================

void Standard_MMgrOpt::Free(Standard_Address theStorage)
{
  // safely return if attempt to free null pointer
  if ( ! theStorage )
    return;

  // get the pointer to the memory block header
  Standard_Size* aBlock = GET_BLOCK(theStorage);
  
  // and get the allocated size of the block
  Standard_Size RoundSize = aBlock[0];
  
  callBack(Standard_False, theStorage, RoundSize, 0);
  
  // check whether blocks with that size are recyclable
  const Standard_Size Index = INDEX_CELL(RoundSize);
  if ( Index <= myFreeListMax ) {
    // Lock access to critical data (myFreeList and other) by mutex
    // Note that we do not lock fields that do not change during the 
    // object life (such as myThreshold), and assume that calls to functions 
    // of standard library are already protected by their implementation.
    // We also do not use Sentry, since in case if OCC signal or exception is
    // caused by this block we will have deadlock anyway...
    myMutex.Lock();
    
    // in the memory block header, record address of the next free block
    *(Standard_Size**)aBlock = myFreeList[Index];
    // add new block to be first in the list
    myFreeList[Index] = aBlock;

    myMutex.Unlock();
  }
  // otherwise, we have block of big size which shall be simply released
  else 
    FreeMemory (aBlock, RoundSize);
}

//=======================================================================
//function : Purge
//purpose  : Frees all free lists except small blocks (less than CellSize)
//=======================================================================

Standard_Integer Standard_MMgrOpt::Purge(Standard_Boolean )
{
  // Lock access to critical data by mutex
  Standard_Mutex::Sentry aSentry (myMutex);

  // TODO: implement support for isDeleted = True
  
  // free memory blocks contained in free lists
  // whose sizes are greater than cellsize
  Standard_Integer nbFreed = 0;
  Standard_Size i = INDEX_CELL(ROUNDUP_CELL(myCellSize+BLOCK_SHIFT));
  for (; i <= myFreeListMax; i++ ) {
    Standard_Size * aFree = myFreeList[i];      
    while(aFree) {
      Standard_Size * anOther = aFree;
      aFree = * (Standard_Size **) aFree;
      free(anOther); 
      nbFreed++;
    }
    myFreeList[i] = NULL;
  }

  // Lock access to critical data by mutex
  Standard_Mutex::Sentry aSentry1 (myMutexPools);

  // release memory pools containing no busy memory;
  // for that for each pool count the summary size of blocks
  // got from the free lists allocated from this pool
#ifndef _WIN32
  const Standard_Size PoolSize = myPageSize * myNbPages;
#else
  const Standard_Size PoolSize =
    PAGE_ALIGN(myPageSize * myNbPages + sizeof(HANDLE), myPageSize) -
    sizeof(HANDLE);
#endif
  const Standard_Size RPoolSize = ROUNDDOWN_CELL(PoolSize);
  const Standard_Size PoolSizeN = RPoolSize / sizeof(Standard_Size);

  // declare the table of pools;
  // (we map free blocks onto a number of pools simultaneously)
  static const Standard_Integer NB_POOLS_WIN = 512;
  static Standard_Size* aPools[NB_POOLS_WIN];
  static Standard_Size aFreeSize[NB_POOLS_WIN];
  static Standard_Integer aFreePools[NB_POOLS_WIN];

  Standard_Size * aNextPool = myAllocList;
  Standard_Size * aPrevPool = NULL;
  const Standard_Size nCells = INDEX_CELL(myCellSize);
  Standard_Integer nPool = 0, nPoolFreed = 0;

  while (aNextPool) {
    // fill the table of pools
    Standard_Integer iPool;
    for (iPool = 0; aNextPool && iPool < NB_POOLS_WIN; iPool++) {
      aPools[iPool] = aNextPool;
      aFreeSize[iPool] = 0;
      aNextPool = * (Standard_Size **) aNextPool; // get next pool
    }
    const Standard_Integer iLast = iPool - 1;
    nPool += iPool;

    // scan free blocks, find corresponding pools and increment
    // counters
    for (i = 0; i <= nCells; i++ ) {
      Standard_Size * aFree = myFreeList[i];
      Standard_Size aSize = BLOCK_SHIFT * sizeof(Standard_Size) +
        ROUNDUP_CELL(1) * i;
      while(aFree) {
        for (iPool = 0; iPool <= iLast; iPool++) {
          if (aFree >= aPools[iPool] && aFree < aPools[iPool] + PoolSizeN) {
            aFreeSize[iPool] += aSize;
            break;
          }
        }
        aFree = * (Standard_Size **) aFree; // get next free block
      }
    }

    // scan the table and make the list of free pools
    Standard_Integer iLastFree = -1;
    for (iPool = 0; iPool <= iLast; iPool++) {
      aFreeSize[iPool] = ROUNDUP_CELL(aFreeSize[iPool]);
      if (aFreeSize[iPool] == RPoolSize)
        aFreePools[++iLastFree] = iPool;
    }
    if (iLastFree == -1) {
      // no free pools found in this table
      aPrevPool = aPools[iLast];
      continue;
    }

    // scan free blocks again, and remove those of them
    // that belong to free pools
    Standard_Integer j;
    for (i = 0; i <= nCells; i++ ) {
      Standard_Size * aFree = myFreeList[i];
      Standard_Size * aPrevFree = NULL;
      while(aFree) {
        for (j = 0; j <= iLastFree; j++) {
          iPool = aFreePools[j];
          if (aFree >= aPools[iPool] && aFree < aPools[iPool] + PoolSizeN)
            break;
        }
        if (j <= iLastFree)
        {
          // remove
          aFree = * (Standard_Size **) aFree;
          if (aPrevFree)
            * (Standard_Size **) aPrevFree = aFree; // link to previous
          else
            myFreeList[i] = aFree;
          nbFreed++;
        }
        else {
          // skip
          aPrevFree = aFree;
          aFree = * (Standard_Size **) aFree;
        }
      }
    }

    // release free pools, and reconnect remaining pools
    // in the linked list
    Standard_Size * aPrev = (aFreePools[0] == 0
                             ? aPrevPool
                             : aPools[aFreePools[0] - 1]);
    for (j = 0; j <= iLastFree; j++) {
      iPool = aFreePools[j];
      if (j > 0) {
        // update the pointer to the previous non-free pool
        if (iPool - aFreePools[j - 1] > 1)
          aPrev = aPools[iPool - 1];
      }
      if (j == iLastFree || aFreePools[j + 1] - iPool > 1) {
        // get next non-free pool
        Standard_Size * aNext =
          (j == iLastFree && aFreePools[j] == iLast)
          ? aNextPool
          : aPools[iPool + 1];
        // and connect it to the list of pools that have been processed
        // and remain non-free
        if (aPrev)
          * (Standard_Size **) aPrev = aNext;
        else
          myAllocList = aNext;
      }
      FreeMemory(aPools[iPool], PoolSize);
    }
    // update the pointer to the previous non-free pool
    aPrevPool = (aFreePools[iLastFree] == iLast
                 ? aPrev
                 : aPools[iLast]);
    nPoolFreed += iLastFree + 1;
  }

  return nbFreed;
}

//=======================================================================
//function : FreePools
//purpose  : Frees all memory pools allocated for small blocks
//=======================================================================

void Standard_MMgrOpt::FreePools()
{
  // Lock access to critical data by mutex
  Standard_Mutex::Sentry aSentry (myMutexPools);
    
  // last pool is remembered in myAllocList
  Standard_Size * aFree = myAllocList;
  myAllocList = 0;
  while (aFree) {
    Standard_Size * aBlock = aFree;
    // next pool address is stored in first 8 bytes of each pool
    aFree = * (Standard_Size **) aFree;
    // free pool (note that its size is calculated rather than stored)
    FreeMemory ( aBlock, myPageSize * myNbPages );
  }
}

//=======================================================================
//function : Reallocate
//purpose  : 
//=======================================================================

Standard_Address Standard_MMgrOpt::Reallocate(Standard_Address theStorage,
                                              const Standard_Size theNewSize)
{
  // if theStorage == NULL, just allocate new memory block
  if (!theStorage)
  {
    return Allocate(theNewSize);
  }
  
  Standard_Size * aBlock = GET_BLOCK(theStorage);
  Standard_Address newStorage = NULL;

  // get current size of the memory block from its header
  Standard_Size OldSize = aBlock[0];

  // if new size is less than old one, just do nothing
  if (theNewSize <= OldSize) {
    newStorage = theStorage;
  }
  // otherwise, allocate new block and copy the data to it
  else {
    newStorage = Allocate(theNewSize);
    memcpy (newStorage, theStorage, OldSize);
    Free( theStorage );
    // clear newly added part of the block
    if ( myClear )
      memset(((char*)newStorage) + OldSize, 0, theNewSize-OldSize);
  }
  return newStorage;
}

//=======================================================================
//function : AllocMemory
//purpose  : Allocate a big block of memory using either malloc/calloc
//           or memory mapped file
//=======================================================================

Standard_Size * Standard_MMgrOpt::AllocMemory(Standard_Size &Size)
{
  // goto is used as efficient method for a possibility to retry allocation
retry:

  Standard_Size * aBlock = NULL;

  // if MMap option is ON, allocate using memory mapped files
  if (myMMap) {
#ifndef _WIN32

    // align size to page size
    const Standard_Size AlignedSize = PAGE_ALIGN(Size, myPageSize);

    // allocate memory
    // note that on UNIX myMMap is file descriptor for /dev/null
    aBlock = (Standard_Size * )mmap((char*)MMAP_BASE_ADDRESS, AlignedSize,
                                    PROT_READ | PROT_WRITE, MMAP_FLAGS,
                                    myMMap, 0);
    if (aBlock == MAP_FAILED /* -1 */) {
      int errcode = errno;
      // as a last resort, try freeing some memory by calling Purge()
      if ( Purge(Standard_False) )
        goto retry;
      // if nothing helps, raise exception
      throw Standard_OutOfMemory(strerror(errcode));
    }

    // save actually allocated size into argument
    Size = AlignedSize;

#else /* _WIN32 */

    // align size to page size, taking into account additional space needed to
    // store handle to the memory map
    const Standard_Size AlignedSize = PAGE_ALIGN(Size+sizeof(HANDLE), myPageSize);

    // allocate mapped file
    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
                                    PAGE_READWRITE,
                                    DWORD(AlignedSize / 0x80000000),
                                    DWORD(AlignedSize % 0x80000000), NULL); 
    HANDLE * aMBlock = (hMap && GetLastError() != ERROR_ALREADY_EXISTS ? 
                        (HANDLE*)MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0) : NULL);
    // check for error and try allocating address space
    if ( ! aMBlock ) 
    {
      // close handle if allocated
      if ( hMap ) 
        CloseHandle(hMap); 
      hMap = 0;
      // as a last resort, try freeing some memory by calling Purge() and retry
      if ( Purge(Standard_False) )
        goto retry;
      // if nothing helps, make error message and raise exception
      const int BUFSIZE=1024;

      wchar_t message[BUFSIZE];

      if ( FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0,
        message, BUFSIZE-1, 0) <=0 )
        StringCchCopyW(message, _countof(message), L"Standard_MMgrOpt::AllocMemory() failed to mmap");

      char messageA[BUFSIZE];
      WideCharToMultiByte(CP_UTF8, 0, message, -1, messageA, sizeof(messageA), NULL, NULL);
      throw Standard_OutOfMemory(messageA);
    }

    // record map handle in the beginning
    aMBlock[0] = hMap;

    // and shift to the beginning of usable area
    aBlock = (Standard_Size*)(aMBlock+1);

    // save actually allocated size into argument
    Size = AlignedSize - sizeof(HANDLE);
#endif    
  }
  // else just allocate by malloc or calloc
  else {
    aBlock = (Standard_Size *) (myClear ? calloc(Size,sizeof(char)) : malloc(Size));
    // check the result
    if ( ! aBlock ) 
    {
      // as a last resort, try freeing some memory by calling Purge()
      if ( Purge(Standard_False) )
        goto retry;
      // if nothing helps, raise exception
      throw Standard_OutOfMemory("Standard_MMgrOpt::Allocate(): malloc failed");
    }
  }
  // clear whole block if clearing option is set
  if (myClear)
    memset (aBlock, 0, Size);
  return aBlock;
}

//=======================================================================
//function : FreeMemory
//purpose  : 
//=======================================================================

void Standard_MMgrOpt::FreeMemory (Standard_Address aBlock, 
                                   const Standard_Size
#ifndef _WIN32                                   
                                   aSize
#endif
                                  )
{
  // release memory (either free or unmap)
  if ( myMMap ) {
#ifndef _WIN32
    // align size to page size, just the same as in AllocMemory()
    const Standard_Size AlignedSize = PAGE_ALIGN(aSize, myPageSize);
    munmap((char*)aBlock, AlignedSize);
#else
    // recover handle to the memory mapping stored just before the block
    const HANDLE * aMBlock = (const HANDLE *)aBlock;
    HANDLE hMap = *(--aMBlock);
    UnmapViewOfFile((LPCVOID)aMBlock);
    CloseHandle (hMap);
#endif
  }
  else
    free(aBlock);
}
