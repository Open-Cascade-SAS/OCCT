// File:        NCollection_BaseAllocator.cxx
// Created:     Fri Apr 12 12:53:28 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
// Purpose:     Implementation of the BaseAllocator class

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <Standard_Mutex.hxx>

IMPLEMENT_STANDARD_HANDLE(NCollection_BaseAllocator,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(NCollection_BaseAllocator,MMgt_TShared)

//=======================================================================
//function : Allocate
//purpose  : Standard allocation
//=======================================================================

void* NCollection_BaseAllocator::Allocate(const size_t size)
{ 
  return Standard::Allocate(size);
}

//=======================================================================
//function : Free
//purpose  : Standard deallocation
//=======================================================================

void  NCollection_BaseAllocator::Free(void *anAddress)
{ 
  if (anAddress) Standard::Free((Standard_Address&)anAddress); 
}

//=======================================================================
//function : CommonBaseAllocator
//purpose  : Creates the only one BaseAllocator
//=======================================================================

const Handle(NCollection_BaseAllocator)& 
       NCollection_BaseAllocator::CommonBaseAllocator(void)
{ 
  static Handle(NCollection_BaseAllocator) pAllocator = 
    new NCollection_BaseAllocator;
  return pAllocator;
}

// global variable to ensure that allocator will be created during loading the library
static Handle(NCollection_BaseAllocator) theAllocInit = 
  NCollection_BaseAllocator::CommonBaseAllocator();

//=======================================================================
//function : StandardCallBack
//purpose  : Callback function to register alloc/free calls
//=======================================================================

struct StorageInfo
{
  Standard_Size roundSize;
  int nbAlloc;
  int nbFree;
  StorageInfo()
    : roundSize(0), nbAlloc(0), nbFree(0) {}
  StorageInfo(Standard_Size theSize)
    : roundSize(theSize), nbAlloc(0), nbFree(0) {}
};

static NCollection_DataMap<Standard_Size, StorageInfo> StorageMap;

void NCollection_BaseAllocator::StandardCallBack
                    (const Standard_Boolean theIsAlloc,
                     const Standard_Address /*theStorage*/,
                     const Standard_Size theRoundSize,
                     const Standard_Size /*theSize*/)
{
  static int aLock = 0;
  if (aLock)
    return;
  aLock = 1;
  if (!StorageMap.IsBound(theRoundSize))
  {
    StorageInfo aEmpty(theRoundSize);
    StorageMap.Bind(theRoundSize, aEmpty);
  }
  StorageInfo& aInfo = StorageMap(theRoundSize);
  if (theIsAlloc)
    aInfo.nbAlloc++;
  else
    aInfo.nbFree++;
  aLock = 0;
}

//=======================================================================
//function : PrintMemUsageStatistics
//purpose  : Prints memory usage statistics cumulated by StandardCallBack
//=======================================================================

void NCollection_BaseAllocator::PrintMemUsageStatistics()
{
  // sort by roundsize
  NCollection_List<StorageInfo> aColl;
  NCollection_List<StorageInfo>::Iterator itLst;
  NCollection_DataMap<Standard_Size, StorageInfo>::Iterator itMap(StorageMap);
  for (; itMap.More(); itMap.Next())
  {
    for (itLst.Init(aColl); itLst.More(); itLst.Next())
      if (itMap.Value().roundSize < itLst.Value().roundSize)
        break;
    if (itLst.More())
      aColl.InsertBefore(itMap.Value(), itLst);
    else
      aColl.Append(itMap.Value());
  }
  Standard_Size aTotAlloc = 0;
  Standard_Size aTotLeft = 0;
  // print
  printf("%12s %12s %12s %12s %12s\n",
         "BlockSize", "NbAllocated", "NbLeft", "Allocated", "Left");
  for (itLst.Init(aColl); itLst.More(); itLst.Next())
  {
    const StorageInfo& aInfo = itLst.Value();
    Standard_Integer nbLeft = aInfo.nbAlloc - aInfo.nbFree;
    Standard_Size aSizeAlloc = aInfo.nbAlloc * aInfo.roundSize;
    Standard_Size aSizeLeft = nbLeft * aInfo.roundSize;
    printf("%12d %12d %12d %12d %12d\n", aInfo.roundSize,
           aInfo.nbAlloc, nbLeft, aSizeAlloc, aSizeLeft);
    aTotAlloc += aSizeAlloc;
    aTotLeft += aSizeLeft;
  }
  printf("%12s %12s %12s %12d %12d\n", "Total:", "", "",
         aTotAlloc, aTotLeft);
  fflush(stdout);
}
