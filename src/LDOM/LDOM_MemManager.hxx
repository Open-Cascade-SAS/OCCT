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
DEFINE_STANDARD_HANDLE (LDOM_MemManager, Standard_Transient)

//  Class LDOM_MemManager (underlying structure of LDOM_Document)
//

class LDOM_MemManager : public Standard_Transient
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT LDOM_MemManager       (const Standard_Integer aBlockSize);
  // Constructor

  Standard_EXPORT ~LDOM_MemManager      ();
  // Destructor

  Standard_EXPORT void * Allocate       (const Standard_Integer aSize);
  // General Memory allocator

  const char *           HashedAllocate (const char             * aString,
                                         const Standard_Integer theLen,
                                         Standard_Integer&      theHash);
  // Memory allocation with access via hash table. No new allocation
  // if already present 

  void                   HashedAllocate (const char             * aString,
                                         const Standard_Integer theLen,
                                         LDOMBasicString&      theResult);
  // Memory allocation with access via hash table. No new allocation
  // if already present 

  static Standard_Integer Hash          (const char             * theString,
                                         const Standard_Integer theLen)
                                { return HashTable::Hash (theString, theLen); }

  static Standard_Boolean CompareStrings(const char             * theString,
                                         const Standard_Integer theHashValue,
                                         const char             * theHashedStr);

//  LDOM_Document           Doc           () const
//                                { return LDOM_Document (* this); }

  const LDOM_MemManager&  Self          () const
                                { return * this; }

  const LDOM_BasicElement * RootElement   () const
                                { return myRootElement; }

 private:
  friend class LDOM_Document;
  friend class LDOMParser;

  // ---- CLASS MemBlock ----
  class MemBlock {
    friend class LDOM_MemManager;
    inline MemBlock         (const Standard_Integer aSize, MemBlock * aFirst);
    inline void * Allocate  (const Standard_Integer aSize);
    void * AllocateAndCheck (const Standard_Integer aSize, const MemBlock *&);
    ~MemBlock               ();
    MemBlock * Next         ()           { return myNext; }

    Standard_Integer    mySize;
    Standard_Integer    * myBlock;
    Standard_Integer    * myEndBlock;
    Standard_Integer    * myFreeSpace;
    MemBlock            * myNext;
  };

  // ---- CLASS HashTable ----
  class HashTable {
    friend class LDOM_MemManager;
    HashTable                   (/* const Standard_Integer theMask, */
                                 LDOM_MemManager&       theMemManager);
    const char     * AddString  (const char             * theString,
                                 const Standard_Integer theLen,
                                 Standard_Integer&      theHashIndex);
    static Standard_Integer Hash(const char             * theString,
                                 const Standard_Integer theLen);
    struct TableItem {
      char             * str;
      struct TableItem * next;
    }                           * myTable;
    LDOM_MemManager&            myManager;
    void operator= (const HashTable&);
  };

  // ---- PROHIBITED (PRIVATE) METHODS ----
  LDOM_MemManager (const LDOM_MemManager& theOther);
  // Copy constructor

  LDOM_MemManager& operator = (const LDOM_MemManager& theOther);
  // Assignment

  // ---------- PRIVATE FIELDS ----------

  const LDOM_BasicElement * myRootElement;
  MemBlock              * myFirstBlock;
  MemBlock              * myFirstWithoutRoom;
  Standard_Integer      myBlockSize;
  HashTable             * myHashTable;

 public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(LDOM_MemManager,Standard_Transient)
};


#endif
