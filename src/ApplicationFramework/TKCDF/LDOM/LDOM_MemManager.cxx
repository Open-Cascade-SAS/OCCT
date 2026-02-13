// Created on: 2001-06-26
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <LDOM_MemManager.hxx>
#include <LDOMBasicString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(LDOM_MemManager, Standard_Transient)

#define HASH_MASK 255
#define MINIMAL_ROOM 3

typedef unsigned char LDOM_HashValue; // allocating HASH_MASK integer

inline int convertBlockSize(const int aBlockSize)
{
  return ((aBlockSize - 1) / sizeof(int)) + 1;
}

inline bool compareStrings(char* const str, const char* theString, const int theLength)
{
  // ** This is a bit dangerous (can override the boundary of allocated memory)
  //  return (str[theLength] == '\0' &&
  //          memcmp (str, theString, theLength) == 0);
  // ** This is a more stable (less performant) solution
  if (memcmp(str, theString, theLength))
    return false;
  return (str[theLength] == '\0');
}

//=================================================================================================

inline LDOM_MemManager::MemBlock::MemBlock(const int aSize, LDOM_MemManager::MemBlock* aFirst)
    : myNext(aFirst)
{
  myFreeSpace = myBlock = new int[aSize];
  myEndBlock            = myBlock + aSize;
}

//=================================================================================================

inline void* LDOM_MemManager::MemBlock::Allocate(const int aSize)
{
  void* aResult = nullptr;
  if (aSize <= myEndBlock - myFreeSpace)
  {
    aResult = myFreeSpace;
    myFreeSpace += aSize;
  }
  return aResult;
}

//=================================================================================================

void* LDOM_MemManager::MemBlock::AllocateAndCheck(
  const int                         aSize,
  const LDOM_MemManager::MemBlock*& aFirstWithoutRoom)
{
  void* aResult = nullptr;
  int   aRoom   = (int)(myEndBlock - myFreeSpace);
  if (aSize <= aRoom)
  {
    aResult = myFreeSpace;
    myFreeSpace += aSize;
  }
  if (aRoom < MINIMAL_ROOM)
  {
    if (aFirstWithoutRoom == nullptr)
      aFirstWithoutRoom = this;
  }
  else
    aFirstWithoutRoom = nullptr;
  return aResult;
}

//=================================================================================================

LDOM_MemManager::MemBlock::~MemBlock()
{
  delete[] myBlock;
  MemBlock* aNext = myNext;
  while (aNext)
  {
    MemBlock* aNextNext = aNext->myNext;
    aNext->myNext       = nullptr;
    delete aNext;
    aNext = aNextNext;
  }
}

//=================================================================================================

LDOM_MemManager::HashTable::HashTable(/* const int   aMask, */
                                      LDOM_MemManager& aMemManager)
    : myManager(aMemManager)
{
  int m, nKeys = HASH_MASK + 1;
  /*
    int m     = aMask;
    int nKeys = 1;
    while (m) {
      nKeys *= 2;
      m     /= 2;
    }
    myMask = nKeys - 1;
  */
  myTable = (TableItem*)myManager.Allocate(sizeof(TableItem) * nKeys);
  for (m = 0; m < nKeys; m += 2)
  {
    myTable[m].str      = nullptr;
    myTable[m].next     = nullptr;
    myTable[m + 1].str  = nullptr;
    myTable[m + 1].next = nullptr;
  }
}

//=======================================================================
// function : Hash
// purpose  : CRC-16 hash function
//=======================================================================

int LDOM_MemManager::HashTable::Hash(const char* aString, const int aLen)
{
  static const unsigned int wCRC16a[16] = {
    0000000,
    0140301,
    0140601,
    0000500,
    0141401,
    0001700,
    0001200,
    0141101,
    0143001,
    0003300,
    0003600,
    0143501,
    0002400,
    0142701,
    0142201,
    0002100,
  };

  static const unsigned int wCRC16b[16] = {
    0000000,
    0146001,
    0154001,
    0012000,
    0170001,
    0036000,
    0024000,
    0162001,
    0120001,
    0066000,
    0074000,
    0132001,
    0050000,
    0116001,
    0104001,
    0043000,
  };

  unsigned int         aCRC = 0;
  const unsigned char* aPtr = (const unsigned char*)aString;
  for (int i = aLen; i > 0; i--)
  {
    const unsigned int bTmp = aCRC ^ (unsigned int)(*aPtr++);
    aCRC                    = ((aCRC >> 8) ^ wCRC16a[bTmp & 0x0F]) ^ wCRC16b[(bTmp >> 4) & 0x0F];
  }
  return int(aCRC & HASH_MASK /* myMask */);
}

//=======================================================================
// function : AddString
// purpose  : Add or find a string in the hash table
//=======================================================================

const char* LDOM_MemManager::HashTable::AddString(const char* theString,
                                                  const int   theLen,
                                                  int&        theHashIndex)
{
  const char* aResult = nullptr;
  if (theString == nullptr)
    return nullptr;
  int        aHashIndex = Hash(theString, theLen);
  TableItem* aNode      = &myTable[aHashIndex];
  if (aNode->str == nullptr)
  {
    LDOM_HashValue* anAlloc =
      (LDOM_HashValue*)myManager.Allocate(theLen + 1 + sizeof(LDOM_HashValue));
    anAlloc[0] = LDOM_HashValue(aHashIndex);
    aNode->str = (char*)&anAlloc[1];
    memcpy(aNode->str, theString, theLen);
    aNode->str[theLen] = '\0';
    aResult            = aNode->str;
  }
  else
  {
    if (compareStrings(aNode->str, theString, theLen))
      aResult = aNode->str;
    else
      while (aNode->next)
      {
        aNode = aNode->next;
        if (compareStrings(aNode->str, theString, theLen))
        {
          aResult = aNode->str;
          break;
        }
      }
    if (aResult == nullptr)
    {
      // Attention!!! We can make this allocation in a separate pool
      //              improving performance
      aNode->next = (TableItem*)myManager.Allocate(sizeof(TableItem));
      aNode       = aNode->next;
      LDOM_HashValue* anAlloc =
        (LDOM_HashValue*)myManager.Allocate(theLen + 1 + sizeof(LDOM_HashValue));
      anAlloc[0] = LDOM_HashValue(aHashIndex);
      aNode->str = (char*)&anAlloc[1];
      memcpy(aNode->str, theString, theLen);
      aNode->str[theLen] = '\0';
      aResult            = aNode->str;
      aNode->next        = nullptr;
    }
  }
  theHashIndex = aHashIndex;
  return aResult;
}

//=================================================================================================

LDOM_MemManager::LDOM_MemManager(const int aBlockSize)
    : myRootElement(nullptr),
      myFirstBlock(nullptr),
      myFirstWithoutRoom(nullptr),
      myBlockSize(convertBlockSize(aBlockSize)),
      myHashTable(nullptr)
{
}

//=================================================================================================

LDOM_MemManager::~LDOM_MemManager()
{
  delete myFirstBlock;
  delete myHashTable;
}

//=================================================================================================

void* LDOM_MemManager::Allocate(const int theSize)
{
  void* aResult = nullptr;
  int   aSize   = convertBlockSize(theSize);

  if (aSize >= myBlockSize)
  {
    myFirstBlock = new MemBlock(aSize, myFirstBlock);
    aResult      = myFirstBlock->Allocate(aSize);
  }
  else
  {
    MemBlock* aBlock = myFirstBlock;
    if (aBlock == nullptr)
    {
      myFirstBlock = new MemBlock(myBlockSize, myFirstBlock);
      return myFirstBlock->Allocate(aSize);
    }
    aResult = aBlock->Allocate(aSize);
    if (aResult)
      return aResult;
    aBlock                            = aBlock->Next();
    const MemBlock* aFirstWithoutRoom = nullptr;
    while (aBlock != myFirstWithoutRoom)
    {
      aResult = aBlock->AllocateAndCheck(aSize, aFirstWithoutRoom);
      if (aResult)
        break;
      aBlock = aBlock->Next();
    }
    myFirstWithoutRoom = (MemBlock*&)aFirstWithoutRoom;
    if (aResult == nullptr)
    {
      myFirstBlock = new MemBlock(myBlockSize, myFirstBlock);
      aResult      = myFirstBlock->Allocate(aSize);
    }
  }
  return aResult;
}

//=======================================================================
// function : HashedAllocate
// purpose  : Memory allocation with access via hash table. No new allocation
//           if already present
//=======================================================================

const char* LDOM_MemManager::HashedAllocate(const char* theString, const int theLen, int& theHash)
{
  if (myHashTable == nullptr)
    myHashTable = new HashTable(*this);
  return myHashTable->AddString(theString, theLen, theHash);
}

//=======================================================================
// function : HashedAllocate
// purpose  : Memory allocation with access via hash table. No new allocation
//           if already present
//=======================================================================

void LDOM_MemManager::HashedAllocate(const char*      aString,
                                     const int        theLen,
                                     LDOMBasicString& theResult)
{
  theResult.myType = LDOMBasicString::LDOM_AsciiHashed;
  int         aDummy;
  const char* aHashedString = HashedAllocate(aString, theLen, aDummy);
  if (aHashedString != nullptr)
    theResult.myVal.ptr = (void*)aHashedString;
}

//=================================================================================================

bool LDOM_MemManager::CompareStrings(const char* theString,
                                     const int   theHashValue,
                                     const char* theHashedStr)
{
  if (((LDOM_HashValue*)theHashedStr)[-1] == LDOM_HashValue(theHashValue))
    if (!strcmp(theString, theHashedStr))
      return true;
  return false;
}
