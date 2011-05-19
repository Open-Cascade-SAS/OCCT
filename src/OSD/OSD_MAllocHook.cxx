// File:	OSD_MAllocHook.cxx
// Created:	04.02.2011
// Author:	Mikhail SAZONOV
// Copyright:	Open CASCADE S.A.S. 2011

#include <OSD_MAllocHook.hxx>

#ifndef WNT
#if !defined __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#endif

#include <set>
#include <map>

#ifndef SIZE_MAX
#define SIZE_MAX UINT_MAX
#endif

#define MAX_STR 80

static OSD_MAllocHook::Callback* MypCurrentCallback = NULL;

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

#ifdef WNT
#include <crtdbg.h>

static long getRequestNum(void* pvData, long lRequest, size_t& theSize)
{
  if (_CrtIsValidHeapPointer(pvData))
  {
#if _MSC_VER == 1500   // VS 2008
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
#endif
  }
  return lRequest;
}

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

#else // ! WNT

// Not yet implemented for non-WNT platform

void OSD_MAllocHook::SetCallback(Callback* theCB)
{
  MypCurrentCallback = theCB;
}

#endif // WNT

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// LogFileHandler handler methods
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//=======================================================================
//function : LogFileHandler::LogFileHandler
//purpose  :
//=======================================================================

OSD_MAllocHook::LogFileHandler::LogFileHandler()
: myLogFile(NULL),
  myBreakSize(0)
{
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
  myLogFile = fopen(theFileName, "w");
  if (myLogFile != NULL)
  {
    fputs("Operation type; Request Number; Block Size\n", myLogFile);
    fputs("------------------------------------------\n", myLogFile);
  }
  return myLogFile != NULL;
}

//=======================================================================
//function : LogFileHandler::Close
//purpose  :
//=======================================================================

void OSD_MAllocHook::LogFileHandler::Close()
{
  if (myLogFile != NULL)
  {
    fclose(myLogFile);
    myLogFile = NULL;
  }
}

//=======================================================================
//function : LogFileHandler::MakeReport
//purpose  :
//=======================================================================

struct StorageInfo
{
  Standard_Size size;
  int nbAlloc;
  int nbFree;
  int nbLeftPeak;
  std::set<unsigned long>* alive;
  StorageInfo()
    : size(0), nbAlloc(0), nbFree(0), nbLeftPeak(0), alive(NULL) {}
  StorageInfo(Standard_Size theSize)
    : size(theSize), nbAlloc(0), nbFree(0), nbLeftPeak(0), alive(NULL) {}
  ~StorageInfo()
  {
    if (alive)
      delete alive;
  }
  std::set<unsigned long>& Alive()
  {
    if (!alive)
      alive = new std::set<unsigned long>;
    return *alive;
  }
  const std::set<unsigned long>& Alive() const
  {
    return *alive;
  }
};

inline bool operator < (const StorageInfo& one, const StorageInfo& two)
{
  return one.size < two.size;
}

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
      aInfo.Alive().insert(aReqNum);
    }
    else
    {
      std::set<unsigned long>::iterator aFoundReqNum =
        aInfo.Alive().find(aReqNum);
      if (aFoundReqNum == aInfo.Alive().end())
        // freeing non-registered block, skip it
        continue;
      aTotalLeftSize -= aSize;
      aInfo.Alive().erase(aFoundReqNum);
      if (aInfo.nbAlloc + 1 > 0)
        aInfo.nbFree++;
    }
  }
  fclose(aLogFile);

  // print the report
  FILE* aRepFile = fopen(theOutFile, "w");
  if (aRepFile == NULL)
    return Standard_False;
  fprintf(aRepFile, "%10s %10s %10s %10s %10s %10s %10s\n",
          "BlockSize", "NbAlloc", "NbLeft", "NbLeftPeak",
          "AllocSize", "LeftSize", "PeakSize");
  Standard_Size aTotAlloc = 0;
  for (std::set<StorageInfo>::const_iterator it = aStMap.begin();
       it != aStMap.end(); ++it)
  {
    const StorageInfo& aInfo = *it;
    Standard_Integer nbLeft = aInfo.nbAlloc - aInfo.nbFree;
    Standard_Size aSizeAlloc = aInfo.nbAlloc * aInfo.size;
    Standard_Size aSizeLeft = nbLeft * aInfo.size;
    Standard_Size aSizePeak = aInfo.nbLeftPeak * aInfo.size;
    fprintf(aRepFile, "%10d %10d %10d %10d %10Iu %10Iu %10Iu\n", aInfo.size,
            aInfo.nbAlloc, nbLeft, aInfo.nbLeftPeak,
            aSizeAlloc, aSizeLeft, aSizePeak);
    if (aTotAlloc + aSizeAlloc < aTotAlloc) // overflow ?
      aTotAlloc = SIZE_MAX;
    else
      aTotAlloc += aSizeAlloc;
    if (theIncludeAlive && !aInfo.Alive().empty())
    {
      for (std::set<unsigned long>::const_iterator it1 = aInfo.alive->begin();
           it1 != aInfo.alive->end(); ++it1)
        fprintf(aRepFile, "%10lu\n", *it1);
    }
  }
  fprintf(aRepFile, "%10s %10s %10s %10s%c%10Iu %10Iu %10Iu\n", "Total:",
          "", "", "", (aTotAlloc == SIZE_MAX ? '>' : ' '), aTotAlloc,
          aTotalLeftSize, aTotalPeakSize);
  fclose(aRepFile);
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
  if (myLogFile != NULL)
  {
    myMutex.Lock();
    fprintf(myLogFile, "alloc %10lu %10u\n", theRequestNum, theSize);
    myMutex.Unlock();
    if (myBreakSize == theSize)
    {
      int a = 1;
    }
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
  if (myLogFile != NULL)
  {
    myMutex.Lock();
    fprintf(myLogFile, "free  %10lu %10u\n", theRequestNum, theSize);
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
  myBreakSize(0)
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

#define MAX_ALLOC_SIZE 2000000u

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
  FILE* aRepFile = fopen(theOutFile, "w");
  if (aRepFile == NULL)
    return Standard_False;
  fprintf(aRepFile, "%10s %10s %10s %10s %10s %10s %10s\n",
          "BlockSize", "NbAlloc", "NbLeft", "NbLeftPeak",
          "AllocSize", "LeftSize", "PeakSize");
  Standard_Size aTotAlloc = 0;
  for (int i = 0; i < MAX_ALLOC_SIZE; i++)
  {
    if (myArray[i].nbAlloc > 0)
    {
      Standard_Integer nbLeft = myArray[i].nbAlloc - myArray[i].nbFree;
      if (nbLeft < 0)
        nbLeft = 0;
      int aSize = i + 1;
      Standard_Size aSizeAlloc = myArray[i].nbAlloc * aSize;
      Standard_Size aSizeLeft = nbLeft * aSize;
      Standard_Size aSizePeak = myArray[i].nbLeftPeak * aSize;
      fprintf(aRepFile, "%10d %10d %10d %10d %10Iu %10Iu %10Iu\n", aSize,
              myArray[i].nbAlloc, nbLeft, myArray[i].nbLeftPeak,
              aSizeAlloc, aSizeLeft, aSizePeak);
      if (aTotAlloc + aSizeAlloc < aTotAlloc) // overflow ?
        aTotAlloc = SIZE_MAX;
      else
        aTotAlloc += aSizeAlloc;
    }
  }
  fprintf(aRepFile, "%10s %10s %10s %10s%c%10Iu %10Iu %10Iu\n", "Total:",
          "", "", "", (aTotAlloc == SIZE_MAX ? '>' : ' '), aTotAlloc,
          myTotalLeftSize, myTotalPeakSize);
  fclose(aRepFile);
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
  {
    int a = 1;
  }
  if (theSize > 0)
  {
    myMutex.Lock();
    int ind = (theSize > MAX_ALLOC_SIZE ? MAX_ALLOC_SIZE-1 : (int)(theSize-1));
    if (myArray[ind].nbAlloc + 1 > 0)
      myArray[ind].nbAlloc++;
    myTotalLeftSize += theSize;
    int nbLeft = myArray[ind].nbAlloc - myArray[ind].nbFree;
    if (nbLeft > myArray[ind].nbLeftPeak)
      myArray[ind].nbLeftPeak = nbLeft;
    if (myTotalLeftSize > myTotalPeakSize)
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
  if (theSize > 0 && myTotalLeftSize >= theSize)
  {
    myMutex.Lock();
    int ind = (theSize > MAX_ALLOC_SIZE ? MAX_ALLOC_SIZE-1 : (int)(theSize-1));
    if (myArray[ind].nbFree + 1 > 0)
      myArray[ind].nbFree++;
    myTotalLeftSize -= theSize;
    myMutex.Unlock();
  }
}
