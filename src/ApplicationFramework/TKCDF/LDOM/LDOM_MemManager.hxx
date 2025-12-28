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

#ifndef LDOM_MemManager_HeaderFile
#define LDOM_MemManager_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

class LDOM_BasicElement;
class LDOM_MemManager;
class LDOMBasicString;

// Define handle class for LDOM_MemManager
//  Class LDOM_MemManager (underlying structure of LDOM_Document)
//

class LDOM_MemManager : public Standard_Transient
{
public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT LDOM_MemManager(const int aBlockSize);
  // Constructor

  Standard_EXPORT ~LDOM_MemManager() override;
  // Destructor

  Standard_EXPORT void* Allocate(const int aSize);
  // General Memory allocator

  const char* HashedAllocate(const char* aString, const int theLen, int& theHash);
  // Memory allocation with access via hash table. No new allocation
  // if already present

  void HashedAllocate(const char* aString, const int theLen, LDOMBasicString& theResult);

  // Memory allocation with access via hash table. No new allocation
  // if already present

  static int Hash(const char* theString, const int theLen)
  {
    return HashTable::Hash(theString, theLen);
  }

  static bool CompareStrings(const char* theString,
                             const int   theHashValue,
                             const char* theHashedStr);

  //  LDOM_Document           Doc           () const
  //                                { return LDOM_Document (* this); }

  const LDOM_MemManager& Self() const { return *this; }

  const LDOM_BasicElement* RootElement() const { return myRootElement; }

private:
  friend class LDOM_Document;
  friend class LDOMParser;

  // ---- CLASS MemBlock ----
  class MemBlock
  {
    friend class LDOM_MemManager;
    inline MemBlock(const int aSize, MemBlock* aFirst);
    inline void* Allocate(const int aSize);
    void*        AllocateAndCheck(const int aSize, const MemBlock*&);
    ~MemBlock();

    MemBlock* Next() { return myNext; }

    int       mySize;
    int*      myBlock;
    int*      myEndBlock;
    int*      myFreeSpace;
    MemBlock* myNext;
  };

  // ---- CLASS HashTable ----
  class HashTable
  {
    friend class LDOM_MemManager;
    HashTable(/* const int theMask, */
              LDOM_MemManager& theMemManager);
    const char* AddString(const char* theString, const int theLen, int& theHashIndex);
    static int  Hash(const char* theString, const int theLen);

    struct TableItem
    {
      char*             str;
      struct TableItem* next;
    }* myTable;

    LDOM_MemManager& myManager;
    void             operator=(const HashTable&) = delete;
  };

  // ---- PROHIBITED (PRIVATE) METHODS ----
  LDOM_MemManager(const LDOM_MemManager& theOther) = delete;
  // Copy constructor

  LDOM_MemManager& operator=(const LDOM_MemManager& theOther) = delete;
  // Assignment

  // ---------- PRIVATE FIELDS ----------

  const LDOM_BasicElement* myRootElement;
  MemBlock*                myFirstBlock;
  MemBlock*                myFirstWithoutRoom;
  int                      myBlockSize;
  HashTable*               myHashTable;

public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(LDOM_MemManager, Standard_Transient)
};

#endif
