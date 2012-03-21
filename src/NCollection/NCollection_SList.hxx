// Created on: 2002-04-17
// Created by: Alexander Kartomin (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#ifndef NCollection_SList_HeaderFile
#define NCollection_SList_HeaderFile

#include <NCollection_BaseCollection.hxx>

#if !defined No_Exception && !defined No_Standard_NoSuchObject
#include <Standard_NoSuchObject.hxx>
#endif

/**
 * Purpose:     An SList is a LISP like list of Items.
 *              An SList is :
 *                . Empty.
 *                . Or it has a Value and a  Tail  which is an other SList. 
 *              
 *              The Tail of an empty list is an empty list.
 *              SList are  shared.  It  means   that they  can  be
 *              modified through other lists.
 *              SList may  be used  as Iterators. They  have Next,
 *              More, and value methods. To iterate on the content
 *              of the list S just do.
 *              
 *              SList Iterator;
 *              for (Iterator = S; Iterator.More(); Iterator.Next())
 *                X = Iterator.Value();
 *            
 *              Memory usage  is  automatically managed for  SLists
 *              (using reference counts).
 *             
 * Example:
 *              If S1 and S2 are SLists :
 *              if S1.Value() is X.
 *              
 *              And the following is done :
 *              S2 = S1;
 *              S2.SetValue(Y);
 *            
 *              S1.Value() becomes also Y.   So SList must be used
 *              with   care.  Mainly  the SetValue()    method  is
 *              dangerous. 
 */            
template <class TheItemType> class NCollection_SList
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseCollection<TheItemType>::Iterator
{
 public:
  //!   The node of SList
  class SListNode
  {
  private:
    //! Constructor
    SListNode (const TheItemType& theItem,
               const NCollection_SList& theTail) :
                 myCount(1),
                 myValue(theItem) 
    { myTail = new (theTail.myAllocator) NCollection_SList(theTail); }
    //! Tail
    NCollection_SList& Tail (void)
    { return (*myTail); }
    //! Value
    TheItemType& Value (void)         
    { return myValue; }
    //! Clear
    void Clear (void)
    {
      myTail->Clear();
      myTail->myAllocator->Free(myTail);
    }

    DEFINE_STANDARD_ALLOC
    DEFINE_NCOLLECTION_ALLOC

  private:
    // ---------- PRIVATE FIELDS ------------
    Standard_Integer    myCount; //!< Reference count
    NCollection_SList * myTail;  //!< The tail
    TheItemType         myValue; //!< Datum

    // Everything above is private. Only SList has an access
    friend class NCollection_SList<TheItemType>;
  }; // End of nested class SListNode

 public:
  // ---------- PUBLIC METHODS ------------

  //! Empty constructor
  NCollection_SList(const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheItemType>(theAllocator),
    myNode(NULL) {}

  //! Constructor
  NCollection_SList(const TheItemType&       theItem,
                    const NCollection_SList& theTail) :
    NCollection_BaseCollection<TheItemType>(theTail.myAllocator)
  { myNode = new (theTail.myAllocator) SListNode(theItem,theTail); }

  //! Copy constructor
  NCollection_SList (const NCollection_SList& theOther) :
    NCollection_BaseCollection<TheItemType>(theOther.myAllocator)
  { 
    myNode = theOther.myNode; 
    if (myNode)
      myNode->myCount++;
  }

  //! Clear the items out
  void Clear (void)
  {
    if (!myNode)
      return;
    myNode->myCount--; 
    if (myNode->myCount < 1)
    {
      myNode->Clear();
      this->myAllocator->Free(myNode);
    }
    myNode = NULL;
  }

  //! Make this list identical to theOther
  NCollection_SList& operator= (const NCollection_SList& theOther)
  { 
    if (myNode != theOther.myNode) 
    {
      if (theOther.myNode) 
        theOther.myNode->myCount++;
      Clear();
      this->myAllocator = theOther.myAllocator;
      myNode = theOther.myNode;
    }
    return *this;
  }

  //! Replace this list by the items of theOther collection
  virtual void Assign (const NCollection_BaseCollection<TheItemType>& theOther)
  {
    if (this == &theOther) 
      return;
    Clear();
    TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& anIter = 
      theOther.CreateIterator();
    if (!anIter.More())
      return;
    SListNode *aNode, *aPrevNode=NULL;
    for (; anIter.More(); anIter.Next())
    {
      aNode = new (this->myAllocator) SListNode 
        (anIter.Value(), NCollection_SList(this->myAllocator));
      if (IsEmpty())
        myNode = aNode;
      else
        aPrevNode->Tail().myNode = aNode;
      aPrevNode = aNode;
    }
  }

  //! IsEmpty query
  Standard_Boolean IsEmpty (void) const
  { return (myNode==NULL); }

  //! Value - constant access
  virtual const TheItemType& Value (void) const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_SList::Value");
#endif
    return myNode->Value();
  }

  //! ChangeValue - variable access
  virtual TheItemType& ChangeValue (void) const
  { 
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_SList::ChangeValue");
#endif
    return myNode->Value();
  }

  //! SetValue
  void SetValue (const TheItemType& theItem)
  { 
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (IsEmpty())
      Standard_NoSuchObject::Raise ("NCollection_SList::SetValue");
#endif
    myNode->Value() = theItem;
  }

  //! Tail
  const NCollection_SList& Tail (void) const
  { 
    if (!IsEmpty()) 
      return  myNode->Tail();
    else
      return *this;
  }

  //! ChangeTail
  NCollection_SList& ChangeTail (void)
  { 
    if (!IsEmpty()) 
      return myNode->Tail();
    else
      return *this;
  }

  //! SetTail
  void SetTail (NCollection_SList& theList)
  { 
    if (!IsEmpty())
      myNode->Tail() = theList;
    else
      *this = theList;
  }

  //! Construct
  void Construct(const TheItemType& theItem)
  { *this = NCollection_SList (theItem, *this); }

  //! Constructed
  NCollection_SList Constructed(const TheItemType& theItem) const
  { return NCollection_SList (theItem, *this); }

  //! ToTail
  void ToTail (void)
  { *this = Tail(); }

  //! Initialize (~Assign)
  void Initialize (const NCollection_SList& theOther)
  { *this = theOther; }

  //! Init (virtual method of base iterator)
  void Init (const NCollection_SList& theOther)
  { *this = theOther; }

  //! More (~!IsEmpty)
  virtual Standard_Boolean More (void) const
  { return !IsEmpty(); }

  //! Next (~ToTail)
  virtual void Next (void) 
  { ToTail(); }

  //! Size - Number of items
  virtual Standard_Integer Size (void) const
  { return (IsEmpty() ? 0 : 1+myNode->Tail().Size()); }

  //! Destructor - clears the SList
  ~NCollection_SList (void)
  { Clear(); }


 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) NCollection_SList(*this)); }

 private:
  // ---------- PRIVATE FIELDS ------------
  SListNode*    myNode;

  friend class SListNode;
};

#endif
