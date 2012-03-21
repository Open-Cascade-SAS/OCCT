// Created on: 2001-06-26
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#ifndef LDOM_MemManager_HeaderFile
#define LDOM_MemManager_HeaderFile

#include <MMgt_TShared.hxx>
#include <Standard_DefineHandle.hxx>
#include <LDOM_Document.hxx>

class LDOM_BasicElement;

//  Class LDOM_MemManager (underlying structure of LDOM_Document)
//

class LDOM_MemManager : public MMgt_TShared
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

  LDOM_Document           Doc           () const
                                { return LDOM_Document (* this); }

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
//    Standard_Integer            myMask;
    LDOM_MemManager&            myManager;
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
  DEFINE_STANDARD_RTTI (LDOM_MemManager)
};

#include <Handle_LDOM_MemManager.hxx>

#endif
