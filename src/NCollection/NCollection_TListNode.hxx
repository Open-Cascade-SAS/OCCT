// File:        NCollection_TListNode.hxx
// Created:     Tue Apr 23 17:30:38 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@opencascade.com>

#ifndef NCollection_TListNode_HeaderFile
#define NCollection_TListNode_HeaderFile

#include <NCollection_ListNode.hxx>
#include <NCollection_BaseAllocator.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

/**
 * Purpose:     Abstract list node class. Used by BaseList
 * Remark:      Internal class
 */              
template <class TheItemType> class NCollection_TListNode 
  : public NCollection_ListNode
{
 public:
  //! Constructor
  NCollection_TListNode (const TheItemType& theItem,
                         NCollection_ListNode* theNext=NULL) :
    NCollection_ListNode  (theNext) { myValue = theItem; }
  //! Constant value access
  const TheItemType& Value () const { return myValue; }
  //! Variable value access
  TheItemType& ChangeValue () { return myValue; }
  //! Memory allocation
  void* operator new(size_t theSize,
                     const Handle(NCollection_BaseAllocator)& theAllocator) 
  { return theAllocator->Allocate(theSize); }
  //! Static deleter to be passed to BaseList
  static void delNode (NCollection_ListNode * theNode, 
                       Handle(NCollection_BaseAllocator)& theAl)
  {
    ((NCollection_TListNode *) theNode)->myValue.~TheItemType();
    theAl->Free(theNode);
  }

  
 protected:
  TheItemType    myValue; //!< The item stored in the node
  
};

#ifdef WNT
#pragma warning (pop)
#endif

#endif
