// File:        NCollection_BaseAllocator.cxx
// Created:     Fri Apr 12 12:53:28 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
// Purpose:     Implementation of the BaseAllocator class

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
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
/**
 * Structure for collecting statistics about blocks of one size
 */
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

//=======================================================================
/**
 * Static data map (block_size -> StorageInfo)
 */
//=======================================================================
static NCollection_DataMap<Standard_Size, StorageInfo>& StorageMap()
{
  static NCollection_IncAllocator TheAlloc;
  static NCollection_DataMap<Standard_Size, StorageInfo>
    TheMap (1, & TheAlloc);
  return TheMap;
}

//=======================================================================
/**
 * Static data map (address -> AllocationID)
 */
//=======================================================================
static NCollection_DataMap<Standard_Address, Standard_Size>& StorageIDMap()
{
  static NCollection_IncAllocator TheAlloc;
  static NCollection_DataMap<Standard_Address, Standard_Size>
    TheMap (1, & TheAlloc);
  return TheMap;
}

//=======================================================================
/**
 * Static map (AllocationID)
 */
//=======================================================================
static NCollection_Map<Standard_Size>& StorageIDSet()
{
  static NCollection_IncAllocator TheAlloc;
  static NCollection_Map<Standard_Size> TheMap (1, & TheAlloc);
  return TheMap;
}

//=======================================================================
/**
 * Exported value to set the block size for which it is required 
 * collecting alive allocation IDs.
 * The method NCollection_BaseAllocator::PrintMemUsageStatistics
 * dumps all alive IDs into the file alive.d in the current directory.
 */
//=======================================================================
Standard_EXPORT Standard_Size& StandardCallBack_CatchSize()
{
  static Standard_Size Value = 0;
  return Value;
}

//=======================================================================
/**
 * Exported value to set the allocation ID for which it is required 
 * to set a breakpoint on the moment of allocation or freeing.
 * See the method NCollection_BaseAllocator::StandardCallBack
 * where the value StandardCallBack_CatchID() is compared to the current ID.
 * There you can place a break point at the stub assignment statement "a =".
 */
//=======================================================================
Standard_EXPORT Standard_Size& StandardCallBack_CatchID()
{
  static Standard_Size Value = 0;
  return Value;
}

//=======================================================================
/**
 * Static value of the current allocation ID. It provides unique
 * numbering of allocation events.
 */
//=======================================================================
static Standard_Size CurrentID = 0;

//=======================================================================
/**
 * Exported function to reset the callback system to the initial state
 */
//=======================================================================
Standard_EXPORT void StandardCallBack_Reset()
{
  StorageMap().Clear();
  StorageIDMap().Clear();
  StorageIDSet().Clear();
  CurrentID = 0;
  StandardCallBack_CatchSize() = 0;
  StandardCallBack_CatchID() = 0;
}

//=======================================================================
//function : StandardCallBack
//purpose  : Callback function to register alloc/free calls
//=======================================================================

void NCollection_BaseAllocator::StandardCallBack
                    (const Standard_Boolean theIsAlloc,
                     const Standard_Address theStorage,
                     const Standard_Size theRoundSize,
                     const Standard_Size /*theSize*/)
{
  static Standard_Mutex aMutex;
  Standard_Boolean isReentrant = Standard::IsReentrant();
  if (isReentrant)
    aMutex.Lock();
  // statistics by storage size
  NCollection_DataMap<Standard_Size, StorageInfo>& aStMap = StorageMap();
  if (!aStMap.IsBound(theRoundSize))
  {
    StorageInfo aEmpty(theRoundSize);
    aStMap.Bind(theRoundSize, aEmpty);
  }
  StorageInfo& aInfo = aStMap(theRoundSize);
  if (theIsAlloc)
    aInfo.nbAlloc++;
  else
    aInfo.nbFree++;

  if (theRoundSize == StandardCallBack_CatchSize())
  {
    // statistics by alive objects
    NCollection_DataMap<Standard_Address, Standard_Size>& aStIDMap = StorageIDMap();
    NCollection_Map<Standard_Size>& aStIDSet = StorageIDSet();
    int a;
    if (theIsAlloc)
    {
      aStIDMap.Bind(theStorage, ++CurrentID);
      aStIDSet.Add(CurrentID);
      if (CurrentID == StandardCallBack_CatchID())
      {
        // Place for break point for allocation of investigated ID
        a = 1;
      }
    }
    else
    {
      if (aStIDMap.IsBound(theStorage))
      {
        Standard_Size anID = aStIDMap(theStorage);
        aStIDSet.Remove(anID);
        if (anID == StandardCallBack_CatchID())
        {
          // Place for break point for freeing of investigated ID
          a = 0;
        }
      }
    }
  }

  if (isReentrant)
    aMutex.Unlock();
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
  NCollection_DataMap<Standard_Size, StorageInfo>::Iterator itMap(StorageMap());
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
  FILE * ff = fopen("memstat.d", "wt");
  if (ff == NULL)
  {
    cout << "failure writing file memstat.d" << endl;
    return;
  }
  fprintf(ff, "%12s %12s %12s %12s %12s\n",
          "BlockSize", "NbAllocated", "NbLeft", "Allocated", "Left");
  for (itLst.Init(aColl); itLst.More(); itLst.Next())
  {
    const StorageInfo& aInfo = itLst.Value();
    Standard_Integer nbLeft = aInfo.nbAlloc - aInfo.nbFree;
    Standard_Size aSizeAlloc = aInfo.nbAlloc * aInfo.roundSize;
    Standard_Size aSizeLeft = nbLeft * aInfo.roundSize;
    fprintf(ff, "%12d %12d %12d %12d %12d\n", aInfo.roundSize,
            aInfo.nbAlloc, nbLeft, aSizeAlloc, aSizeLeft);
    aTotAlloc += aSizeAlloc;
    aTotLeft += aSizeLeft;
  }
  fprintf(ff, "%12s %12s %12s %12d %12d\n", "Total:", "", "",
          aTotAlloc, aTotLeft);

  if (!StorageIDSet().IsEmpty())
  {
    fprintf(ff, "Alive allocation numbers of size=%d\n", StandardCallBack_CatchSize());
    NCollection_Map<Standard_Size>::Iterator itMap1(StorageIDSet());
    for (; itMap1.More(); itMap1.Next())
      fprintf(ff, "%d\n", itMap1.Key());
  }
  fclose(ff);
}
