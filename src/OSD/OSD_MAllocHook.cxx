// Created on: 2011-02-04
// Created by: Mikhail SAZONOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OSD_MAllocHook.hxx>

#ifndef _MSC_VER
#if !defined __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#endif

#include <set>
#include <map>
#include <cstdlib>
#include <cstring>
#include <iomanip>

#ifndef SIZE_MAX
#define SIZE_MAX UINT_MAX
#endif

#define MAX_STR 80

static OSD_MAllocHook::Callback* MypCurrentCallback = NULL;

namespace {
  // dummy function to call at place where break point might be needed
  static unsigned debug_counter = 0;
  inline void place_for_breakpoint () {
      // this statement is just to have any instruction in object code,
      // otherwise compiler does not leave a place for break point
      debug_counter++;
  }
}

//=======================================================================
//function : GetCallback
//purpose  :
//=======================================================================

OSD_MAllocHook::Callback* OSD_MAllocHook::GetCallback()
{
  return MypCurrentCallback;
}

//=======================================================================
//function : GetLogFileHandler
//purpose  :
//=======================================================================

OSD_MAllocHook::LogFileHandler* OSD_MAllocHook::GetLogFileHandler()
{
  static LogFileHandler MyHandler;
  return &MyHandler;
}

//=======================================================================
//function : GetCollectBySize
//purpose  :
//=======================================================================

OSD_MAllocHook::CollectBySize* OSD_MAllocHook::GetCollectBySize()
{
  static CollectBySize MyHandler;
  return &MyHandler;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Platform-dependent methods
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifdef _MSC_VER
#include <crtdbg.h>

#if _MSC_VER >= 1500  /* VS 2008 */

static long getRequestNum(void* pvData, long lRequest, size_t& theSize)
{
#ifdef _DEBUG /* protect against invalid pointer; in Release, _CrtIsValidHeapPointer is always 1 */
  if (!_CrtIsValidHeapPointer(pvData))
    return lRequest;
#else
  (void)lRequest; // avoid compiler warning on unused arg
#endif

#define nNoMansLandSize 4
  // the header struct is taken from crt/src/dbgint.h
  struct _CrtMemBlockHeader
  {
#ifdef _WIN64
      int                         nBlockUse;
      size_t                      nDataSize;
#else
      size_t                      nDataSize;
      int                         nBlockUse;
#endif
    long                        lRequest;
    unsigned char               gap[nNoMansLandSize];
  };

  _CrtMemBlockHeader* aHeader = ((_CrtMemBlockHeader*)pvData)-1;
  theSize = aHeader->nDataSize;
  return aHeader->lRequest;
}

#else /* _MSC_VER < 1500 */

static long getRequestNum(void* /*pvData*/, long lRequest, size_t& /*theSize*/)
{
  return lRequest;
}

#endif /* _MSC_VER == 1500 */

int __cdecl MyAllocHook(int      nAllocType,
                        void   * pvData,
                        size_t   nSize,
                        int      nBlockUse,
                        long     lRequest,
                        const unsigned char * /*szFileName*/,
                        int      /*nLine*/)
{
  if (nBlockUse == _CRT_BLOCK ||  // Ignore internal C runtime library allocations
      MypCurrentCallback == NULL)
    return(1);

  if (nAllocType == _HOOK_ALLOC)
    MypCurrentCallback->AllocEvent(nSize, lRequest);
  else if (nAllocType == _HOOK_FREE)
  {
    // for free hook, lRequest is not defined,
    // but we can take it from the CRT mem block header
    size_t aSize = 0;
    lRequest = getRequestNum(pvData, lRequest, aSize);
    MypCurrentCallback->FreeEvent(pvData, aSize, lRequest);
  }
  else // _HOOK_REALLOC
  {
    // for realloc hook, lRequest shows the new request,
    // and we should get request number for old block
    size_t anOldSize = 0;
    long anOldRequest = getRequestNum(pvData, 0, anOldSize);
    MypCurrentCallback->FreeEvent(pvData, anOldSize, anOldRequest);
    MypCurrentCallback->AllocEvent(nSize, lRequest);
  }

  return(1);         // Allow the memory operation to proceed
}

//=======================================================================
//function : SetCallback
//purpose  :
//=======================================================================

void OSD_MAllocHook::SetCallback(Callback* theCB)
{
  MypCurrentCallback = theCB;
  if (theCB == NULL)
    _CrtSetAllocHook(NULL);
  else
    _CrtSetAllocHook(MyAllocHook);
}

#else // ! _MSC_VER

// Not yet implemented for non-WNT platform

void OSD_MAllocHook::SetCallback(Callback* theCB)
{
  MypCurrentCallback = theCB;
}

#endif // _MSC_VER

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// LogFileHandler handler methods
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//=======================================================================
//function : LogFileHandler::LogFileHandler
//purpose  :
//=======================================================================

OSD_MAllocHook::LogFileHandler::LogFileHandler()
: myBreakSize(0)
{
  myLogFile.imbue (std::locale ("C"));
}

//=======================================================================
//function : LogFileHandler::~LogFileHandler
//purpose  :
//=======================================================================

OSD_MAllocHook::LogFileHandler::~LogFileHandler()
{
  Close();
}

//=======================================================================
//function : LogFileHandler::Open
//purpose  :
//=======================================================================

Standard_Boolean OSD_MAllocHook::LogFileHandler::Open(const char* theFileName)
{
  Close();
  myLogFile.open (theFileName);
  if (!myLogFile.is_open())
  {
    return Standard_False;
  }

  myLogFile << "Operation type; Request Number; Block Size\n"
               "------------------------------------------\n";
  return Standard_True;
}

//=======================================================================
//function : LogFileHandler::Close
//purpose  :
//=======================================================================

void OSD_MAllocHook::LogFileHandler::Close()
{
  if (myLogFile.is_open())
  {
    myLogFile.close();
  }
}

//=======================================================================
//function : LogFileHandler::MakeReport
//purpose  :
//=======================================================================

struct StorageInfo
{
  Standard_Size           size;
  Standard_Integer        nbAlloc;
  Standard_Integer        nbFree;
  Standard_Integer        nbLeftPeak;
  std::set<unsigned long> alive;

  StorageInfo(Standard_Size theSize = 0)
  : size      (theSize),
    nbAlloc   (0),
    nbFree    (0),
    nbLeftPeak(0),
    alive()
  {
  }

  bool operator < (const StorageInfo& theOther) const
  {
    return size < theOther.size;
  }
};

Standard_Boolean OSD_MAllocHook::LogFileHandler::MakeReport
                   (const char* theLogFile,
                    const char* theOutFile,
                    const Standard_Boolean theIncludeAlive)
{
  // open log file
  FILE* aLogFile = fopen(theLogFile, "r");
  if (aLogFile == NULL)
    return Standard_False;

  // skip 2 header lines
  char aStr[MAX_STR];
  if (fgets(aStr, MAX_STR-1, aLogFile) == NULL)
  {
    fclose(aLogFile);
    return Standard_False;
  }
  if (fgets(aStr, MAX_STR-1, aLogFile) == NULL)
  {
    fclose(aLogFile);
    return Standard_False;
  }

  // scan the log file
  size_t aTotalLeftSize = 0;
  size_t aTotalPeakSize = 0;
  std::set<StorageInfo> aStMap;
  while (fgets(aStr, MAX_STR-1, aLogFile) != NULL)
  {
    // detect operation type, request number and block size
    unsigned long aReqNum, aSize;
    char* aType = aStr;
    char* pStr = aStr;
    //sscanf(aStr, "%5s %lu %lu", aType, &aReqNum, &aSize);
    while (*pStr != ' ' && *pStr) pStr++;
    *pStr++ = '\0';
    while (*pStr == ' ' && *pStr) pStr++;
    aReqNum = atol(pStr);
    while (*pStr != ' ' && *pStr) pStr++;
    while (*pStr == ' ' && *pStr) pStr++;
    aSize = atol(pStr);
    Standard_Boolean isAlloc = Standard_False;
    if (strcmp(aType, "alloc") == 0)
    {
      isAlloc = Standard_True;
    }
    else if (strcmp(aType, "free") != 0)
      continue;

    // collect statistics by storage size
    StorageInfo aSuchInfo(aSize);
    std::set<StorageInfo>::iterator aFound = aStMap.find(aSuchInfo);
    if (aFound == aStMap.end())
      aFound = aStMap.insert(aSuchInfo).first;
    StorageInfo& aInfo = const_cast<StorageInfo&>(*aFound);
    if (isAlloc)
    {
      if (aInfo.nbAlloc + 1 > 0)
        aInfo.nbAlloc++;
      aTotalLeftSize += aSize;
      if (aTotalLeftSize > aTotalPeakSize)
        aTotalPeakSize = aTotalLeftSize;
      int nbLeft = aInfo.nbAlloc - aInfo.nbFree;
      if (nbLeft > aInfo.nbLeftPeak)
        aInfo.nbLeftPeak = nbLeft;
      aInfo.alive.insert(aReqNum);
    }
    else
    {
      std::set<unsigned long>::iterator aFoundReqNum =
        aInfo.alive.find(aReqNum);
      if (aFoundReqNum == aInfo.alive.end())
        // freeing non-registered block, skip it
        continue;
      aTotalLeftSize -= aSize;
      aInfo.alive.erase(aFoundReqNum);
      if (aInfo.nbAlloc + 1 > 0)
        aInfo.nbFree++;
    }
  }
  fclose(aLogFile);

  // print the report
  std::ofstream aRepFile (theOutFile);
  if(!aRepFile.is_open())
  {
    return Standard_False;
  }
  aRepFile.imbue (std::locale ("C"));

  aRepFile << std::setw(20) << "BlockSize "
           << std::setw(10) << "NbAlloc "
           << std::setw(10) << "NbLeft "
           << std::setw(10) << "NbLeftPeak "
           << std::setw(20) << "AllocSize "
           << std::setw(20) << "LeftSize "
           << std::setw(20) << "PeakSize " << std::endl;

  Standard_Size aTotAlloc = 0;
  for (std::set<StorageInfo>::const_iterator it = aStMap.begin();
       it != aStMap.end(); ++it)
  {
    const StorageInfo& aInfo = *it;
    Standard_Integer nbLeft = aInfo.nbAlloc - aInfo.nbFree;
    Standard_Size aSizeAlloc = aInfo.nbAlloc * aInfo.size;
    Standard_Size aSizeLeft = nbLeft * aInfo.size;
    Standard_Size aSizePeak = aInfo.nbLeftPeak * aInfo.size;

    aRepFile << std::setw(20) << aInfo.size << ' '
             << std::setw(10) << aInfo.nbAlloc << ' '
             << std::setw(10) << nbLeft << ' '
             << std::setw(10) << aInfo.nbLeftPeak << ' '
             << std::setw(20) << aSizeAlloc << ' '
             << std::setw(20) << aSizeLeft << ' '
             << std::setw(20) << aSizePeak << std::endl;

    if (aTotAlloc + aSizeAlloc < aTotAlloc) // overflow ?
      aTotAlloc = SIZE_MAX;
    else
      aTotAlloc += aSizeAlloc;
    if (theIncludeAlive && !aInfo.alive.empty())
    {
      for (std::set<unsigned long>::const_iterator it1 = aInfo.alive.begin();
           it1 != aInfo.alive.end(); ++it1)
      aRepFile << std::setw(10) << *it1;
    }
  }
  aRepFile << std::setw(20) << "Total:"
           << std::setw(10) << "" << ' '
           << std::setw(10) << "" << ' '
           << std::setw(10) << "" << ' '
           << (aTotAlloc == SIZE_MAX ? '>' : ' ')
           << std::setw(20) << aTotAlloc << ' '
           << std::setw(20) << aTotalLeftSize << ' '
           << std::setw(20) << aTotalPeakSize << std::endl;

  aRepFile.close();
  return Standard_True;
}

//=======================================================================
//function : LogFileHandler::AllocEvent
//purpose  :
//=======================================================================

void OSD_MAllocHook::LogFileHandler::AllocEvent
                   (size_t      theSize,
                    long        theRequestNum)
{
  if (myLogFile.is_open())
  {
    myMutex.Lock();
    myLogFile << "alloc "<< std::setw(10) << theRequestNum
              << std::setw(20) << theSize << std::endl;
    if (myBreakSize == theSize)
      place_for_breakpoint();
    myMutex.Unlock();
  }
}

//=======================================================================
//function : LogFileHandler::FreeEvent
//purpose  :
//=======================================================================

void OSD_MAllocHook::LogFileHandler::FreeEvent
                   (void*       /*theData*/,
                    size_t      theSize,
                    long        theRequestNum)
{
  if (myLogFile.is_open())
  {
    myMutex.Lock();
    myLogFile << "free " << std::setw(20) << theRequestNum
              << std::setw(20) << theSize << std::endl;
    myMutex.Unlock();
  }
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// CollectBySize handler methods
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//=======================================================================
//function : CollectBySize::CollectBySize
//purpose  :
//=======================================================================

OSD_MAllocHook::CollectBySize::CollectBySize()
: myArray(NULL),
  myTotalLeftSize(0),
  myTotalPeakSize(0),
  myBreakSize(0),
  myBreakPeak(0)
{
  Reset();
}

//=======================================================================
//function : CollectBySize::~CollectBySize
//purpose  :
//=======================================================================

OSD_MAllocHook::CollectBySize::~CollectBySize()
{
  if (myArray != NULL)
    delete [] myArray;
}

//=======================================================================
//function : CollectBySize::Reset
//purpose  :
//=======================================================================

#define MAX_ALLOC_SIZE 2000000
const size_t OSD_MAllocHook::CollectBySize::myMaxAllocSize = MAX_ALLOC_SIZE;

void OSD_MAllocHook::CollectBySize::Reset()
{
  myMutex.Lock();
  if (myArray == NULL)
    myArray = new Numbers[MAX_ALLOC_SIZE];
  else
  {
    for (int i = 0; i < MAX_ALLOC_SIZE; i++)
      myArray[i] = Numbers();
  }
  myTotalLeftSize = 0;
  myTotalPeakSize = 0;
  myMutex.Unlock();
}

//=======================================================================
//function : CollectBySize::MakeReport
//purpose  :
//=======================================================================

Standard_Boolean OSD_MAllocHook::CollectBySize::MakeReport(const char* theOutFile)
{
  // print the report
  std::ofstream aRepFile(theOutFile);
  if (!aRepFile.is_open())
    return Standard_False;
  std::locale aCLoc("C");
  aRepFile.imbue(aCLoc);

  aRepFile << std::setw(10) << "BlockSize "
           << std::setw(10) << "NbAlloc "
           << std::setw(10) << "NbLeft "
           << std::setw(10) << "NbLeftPeak "
           << std::setw(20) << "AllocSize "
           << std::setw(20) << "LeftSize "
           << std::setw(20) << "PeakSize " << std::endl;

  Standard_Size aTotAlloc = 0;
  for (int i = 0; i < MAX_ALLOC_SIZE; i++)
  {
    if (myArray[i].nbAlloc > 0 || myArray[i].nbFree > 0)
    {
      Standard_Integer nbLeft = myArray[i].nbAlloc - myArray[i].nbFree;
      int aSize = i + 1;
      Standard_Size aSizeAlloc = myArray[i].nbAlloc * aSize;
      ptrdiff_t     aSizeLeft = nbLeft * aSize;
      Standard_Size aSizePeak = myArray[i].nbLeftPeak * aSize;

      aRepFile << std::setw(10) << aSize << ' '
               << std::setw(10) << myArray[i].nbAlloc << ' '
               << std::setw(10) << nbLeft << ' '
               << std::setw(10) << myArray[i].nbLeftPeak << ' '
               << std::setw(20) << aSizeAlloc << ' '
               << std::setw(20) << aSizeLeft << ' '
               << std::setw(20) << aSizePeak << std::endl;

      if (aTotAlloc + aSizeAlloc < aTotAlloc) // overflow ?
        aTotAlloc = SIZE_MAX;
      else
        aTotAlloc += aSizeAlloc;
    }
  }
  aRepFile << std::setw(10) << "Total:" << ' '
           << std::setw(10) << "" << ' '
           << std::setw(10) << "" << ' '
           << std::setw(10) << "" << ' '
           << (aTotAlloc == SIZE_MAX ? '>' : ' ')
           << std::setw(20) << aTotAlloc  << ' '
           << std::setw(20) << myTotalLeftSize  << ' '
           << std::setw(20) << myTotalPeakSize << std::endl;
  aRepFile.close();
  return Standard_True;
}

//=======================================================================
//function : CollectBySize::AllocEvent
//purpose  :
//=======================================================================

void OSD_MAllocHook::CollectBySize::AllocEvent
                   (size_t      theSize,
                    long        /*theRequestNum*/)
{
  if (myBreakSize == theSize)
    place_for_breakpoint();
  if (theSize > 0)
  {
    myMutex.Lock();
    int ind = (theSize > MAX_ALLOC_SIZE ? MAX_ALLOC_SIZE-1 : (int)(theSize-1));
    myArray[ind].nbAlloc++;
    myTotalLeftSize += theSize;
    int nbLeft = myArray[ind].nbAlloc - myArray[ind].nbFree;
    if (nbLeft > myArray[ind].nbLeftPeak)
    {
      myArray[ind].nbLeftPeak = nbLeft;
      if (myBreakPeak != 0
       && (myBreakSize == theSize || myBreakSize == 0))
      {
        const Standard_Size aSizePeak = myArray[ind].nbLeftPeak * theSize;
        if (aSizePeak > myBreakPeak)
        {
          place_for_breakpoint();
        }
      }
    }
    if (myTotalLeftSize > (ptrdiff_t)myTotalPeakSize)
      myTotalPeakSize = myTotalLeftSize;
    myMutex.Unlock();
  }
}

//=======================================================================
//function : CollectBySize::FreeEvent
//purpose  :
//=======================================================================

void OSD_MAllocHook::CollectBySize::FreeEvent
                   (void*       /*theData*/,
                    size_t      theSize,
                    long        /*theRequestNum*/)
{
  if (theSize > 0)
  {
    myMutex.Lock();
    int ind = (theSize > MAX_ALLOC_SIZE ? MAX_ALLOC_SIZE-1 : (int)(theSize-1));
    myArray[ind].nbFree++;
    myTotalLeftSize -= theSize;
    myMutex.Unlock();
  }
}
