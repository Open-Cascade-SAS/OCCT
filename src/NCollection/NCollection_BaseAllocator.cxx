// Created on: 2002-04-12
// Created by: Alexander KARTOMIN (akm)
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

// Purpose:     Implementation of the BaseAllocator class

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <Standard_Mutex.hxx>
#include <fstream>
#include <iomanip>


IMPLEMENT_STANDARD_RTTIEXT(NCollection_BaseAllocator,Standard_Transient)

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
  if (anAddress) Standard::Free(anAddress); 
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

namespace {
  // dummy function for break point
  inline void place_for_break_point () {}
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
    if (theIsAlloc)
    {
      aStIDMap.Bind(theStorage, ++CurrentID);
      aStIDSet.Add(CurrentID);
      if (CurrentID == StandardCallBack_CatchID())
      {
        // Place for break point for allocation of investigated ID
        place_for_break_point();
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
          place_for_break_point();
        }
      }
    }
  }

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
  std::ofstream aFileOut ("memstat.d", std::ios_base::trunc | std::ios_base::out);
  if (!aFileOut.is_open())
  {
    std::cout << "failure writing file memstat.d" << std::endl;
    return;
  }
  aFileOut.imbue (std::locale ("C"));

  // header
  aFileOut << std::setw(20) << "BlockSize"   << ' '
           << std::setw(12) << "NbAllocated" << ' '
           << std::setw(12) << "NbLeft"      << ' '
           << std::setw(20) << "Allocated"   << ' '
           << std::setw(20) << "Left"        << '\n';

  // body
  for (itLst.Init(aColl); itLst.More(); itLst.Next())
  {
    const StorageInfo& aInfo = itLst.Value();
    Standard_Integer nbLeft = aInfo.nbAlloc - aInfo.nbFree;
    Standard_Size aSizeAlloc = aInfo.nbAlloc * aInfo.roundSize;
    Standard_Size aSizeLeft = nbLeft * aInfo.roundSize;

    aFileOut << std::setw(20) << aInfo.roundSize << ' '
             << std::setw(12) << aInfo.nbAlloc   << ' '
             << std::setw(12) << nbLeft          << ' '
             << std::setw(20) << aSizeAlloc      << ' '
             << std::setw(20) << aSizeLeft       << '\n';

    aTotAlloc += aSizeAlloc;
    aTotLeft  += aSizeLeft;
  }

  // footer
  aFileOut << std::setw(20) << "Total:"  << ' '
           << std::setw(12) << ""        << ' '
           << std::setw(12) << ""        << ' '
           << std::setw(20) << aTotAlloc << ' '
           << std::setw(20) << aTotLeft  << '\n';

  if (!StorageIDSet().IsEmpty())
  {
    aFileOut << "Alive allocation numbers of size=" << StandardCallBack_CatchSize() << '\n';
    for (NCollection_Map<Standard_Size>::Iterator itMap1(StorageIDSet()); itMap1.More(); itMap1.Next())
    {
      aFileOut << itMap1.Key() << '\n';
    }
  }
  aFileOut.close();
}
