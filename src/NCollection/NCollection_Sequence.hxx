// Created on: 2002-03-28
// Created by: Alexander GRIGORIEV
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


#ifndef NCollection_Sequence_HeaderFile
#define NCollection_Sequence_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseSequence.hxx>

#ifndef No_Exception
#include <Standard_OutOfRange.hxx>
#include <Standard_NoSuchObject.hxx>
#endif

/**
 * Purpose:     Definition of a sequence of elements indexed by
 *              an Integer in range of 1..n
 */              
template <class TheItemType> class NCollection_Sequence
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseSequence
{

 public:
  //!   Class defining sequence node - for internal use by Sequence
  class Node : public NCollection_SeqNode
  {
  public:
    //! Constructor
    Node (const TheItemType& theItem) :
      NCollection_SeqNode ()
      { myValue = theItem; }
    //! Constant value access
    const TheItemType& Value () const { return myValue; }
    //! Variable value access
    TheItemType&       ChangeValue () { return myValue; }
    //! Memory allocation
    DEFINE_STANDARD_ALLOC
    DEFINE_NCOLLECTION_ALLOC

  private:
    TheItemType    myValue;
  }; // End of nested class Node

 public:
  //!   Implementation of the Iterator interface.
  class Iterator : public NCollection_BaseCollection<TheItemType>::Iterator,
                   public NCollection_BaseSequence::Iterator
  {
  public:
    //! Empty constructor - for later Init
    Iterator  (void) {}
    //! Constructor with initialisation
    Iterator  (const NCollection_Sequence& theSeq,
               const Standard_Boolean      isStart = Standard_True)
      : NCollection_BaseSequence::Iterator (theSeq, isStart) {}
    //! Assignment
    Iterator& operator= (const Iterator& theIt)
    {
      NCollection_BaseSequence::Iterator& me = * this;
      me.operator= (theIt);
      return * this;
    }
    //! Check end
    virtual Standard_Boolean More (void) const
    { return (myCurrent!=NULL); }
    //! Make step
    virtual void Next (void)         
    { if (myCurrent) myCurrent = (NCollection_SeqNode *) myCurrent->Next(); }
    //! Constant value access
    virtual const TheItemType& Value (void) const
    { return ((const Node *)myCurrent)->Value(); }
    //! Variable value access
    virtual TheItemType& ChangeValue (void) const
    { return ((Node *)myCurrent)->ChangeValue(); }
  }; // End of nested class Iterator

 public:
  // ---------- PUBLIC METHODS ------------

  //! Constructor
  NCollection_Sequence(const Handle(NCollection_BaseAllocator)& theAllocator=0L)
    : NCollection_BaseCollection<TheItemType>(theAllocator),
      NCollection_BaseSequence() {}

  //! Copy constructor
  NCollection_Sequence (const NCollection_Sequence& theOther) :
    NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
    NCollection_BaseSequence()
  { *this = theOther; }

  //! Number of items
  virtual Standard_Integer Size (void) const
  { return mySize; }

  //! Number of items
  Standard_Integer Length (void) const
  { return mySize; }

  //! Empty query
  Standard_Boolean IsEmpty (void) const
  { return (mySize==0); }

  //! Reverse sequence
  void Reverse (void)
  { PReverse(); }

  //! Exchange two members
  void Exchange (const Standard_Integer I,
                 const Standard_Integer J )
  { PExchange(I, J); }

  //! Static deleter to be passed to BaseSequence
  static void delNode (NCollection_SeqNode * theNode, 
                       Handle(NCollection_BaseAllocator)& theAl)
  {
    ((Node *) theNode)->~Node();
    theAl->Free(theNode);
  }

  //! Clear the items out, take a new allocator if non null
  void Clear (const Handle(NCollection_BaseAllocator)& theAllocator=0L)
  {
    ClearSeq (delNode, this->myAllocator);
    if (!theAllocator.IsNull())
      this->myAllocator = theAllocator;
  }
  
  //! Replace this sequence by the items of theOther
  NCollection_Sequence& operator= (const NCollection_Sequence& theOther)
  { 
    if (this == &theOther) 
      return *this;
    Clear (theOther.myAllocator);
    Node * pCur = (Node *) theOther.myFirstItem;
    while (pCur) {
      Node* pNew = new (this->myAllocator) Node (pCur->Value());
      PAppend (pNew);
      pCur = (Node *) pCur->Next();
    }
    return * this;
  }

  //! Replace this sequence by the items of theOther collection
  virtual void Assign (const NCollection_BaseCollection<TheItemType>& theOther)
  {
    if (this == &theOther)
      return;
    Clear ();
    TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& anIter = 
      theOther.CreateIterator();
    for (; anIter.More(); anIter.Next())
      Append(anIter.Value());
  }

  //! Remove one item
  void Remove (Iterator& thePosition)
  { RemoveSeq (thePosition, delNode, this->myAllocator); }

  //! Remove one item
  void Remove (const Standard_Integer theIndex)
  { RemoveSeq (theIndex, delNode, this->myAllocator); }

  //! Remove range of items
  void Remove (const Standard_Integer theFromIndex,
               const Standard_Integer theToIndex)
  { RemoveSeq (theFromIndex, theToIndex, delNode, this->myAllocator); }

  //! Append one item
  void Append (const TheItemType& theItem)
  { PAppend (new (this->myAllocator) Node (theItem)); }

  //! Append another sequence (making it empty)
  void Append (NCollection_Sequence& theSeq)
  {
    if (myFirstItem == theSeq.myFirstItem) Assign (theSeq);
    PAppend (theSeq);
  }

  //! Prepend one item
  void Prepend (const TheItemType& theItem)
  { PPrepend (new (this->myAllocator) Node (theItem)); }

  //! Prepend another sequence (making it empty)
  void Prepend (NCollection_Sequence& theSeq)
  {
    if (myFirstItem == theSeq.myFirstItem) Assign (theSeq);
    PPrepend (theSeq);
  }

  //! InsertBefore theIndex theItem
  void InsertBefore (const Standard_Integer theIndex, 
                     const TheItemType&     theItem)
  { InsertAfter (theIndex-1, theItem); }

  //! InsertBefore theIndex another sequence
  void InsertBefore (const Standard_Integer theIndex,
                     NCollection_Sequence&  theSeq)
  { InsertAfter (theIndex-1, theSeq); }
  
  //! InsertAfter the position of iterator
  void InsertAfter  (Iterator&              thePosition,
                     const TheItemType&     theItem)
  { PInsertAfter (thePosition, new (this->myAllocator) Node (theItem)); }

  //! InsertAfter theIndex theItem
  void InsertAfter  (const Standard_Integer theIndex,
                     NCollection_Sequence&  theSeq)
  { PInsertAfter (theIndex, theSeq); }

  //! InsertAfter theIndex another sequence
  void InsertAfter (const Standard_Integer  theIndex, 
                    const TheItemType&      theItem)
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theIndex < 0 || theIndex > mySize)
      Standard_OutOfRange::Raise ("NCollection_Sequence::InsertAfter");
#endif
    PInsertAfter (theIndex, new (this->myAllocator) Node (theItem));
  }

  //! Split in two sequences
  void Split (const Standard_Integer theIndex, NCollection_Sequence& theSeq)
  {
    theSeq.Clear (this->myAllocator);
    PSplit (theIndex, theSeq);
  }

  //! First item access
  const TheItemType& First () const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (mySize == 0)
      Standard_NoSuchObject::Raise ("NCollection_Sequence::First");
#endif
    return ((const Node *) myFirstItem) -> Value();
  }

  //! Last item access
  const TheItemType& Last () const
  {
#if !defined No_Exception && !defined No_Standard_NoSuchObject
    if (mySize == 0)
      Standard_NoSuchObject::Raise ("NCollection_Sequence::Last");
#endif
    return ((const Node *) myLastItem) -> Value();
  }

  //! Constant item access by theIndex
  const TheItemType& Value (const Standard_Integer theIndex) const
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theIndex <= 0 || theIndex > mySize)
      Standard_OutOfRange::Raise ("NCollection_Sequence::Value");
#endif
    NCollection_Sequence * const aLocalTHIS = (NCollection_Sequence *) this;
    aLocalTHIS -> myCurrentItem  = Find (theIndex);
    aLocalTHIS -> myCurrentIndex = theIndex;
    return ((const Node *) myCurrentItem) -> Value();
  }

  //! Constant operator()
  const TheItemType& operator() (const Standard_Integer theIndex) const
  { return Value(theIndex); }

  //! Variable item access by theIndex
  TheItemType& ChangeValue (const Standard_Integer theIndex)
  {
#if !defined No_Exception && !defined No_Standard_OutOfRange
    if (theIndex <= 0 || theIndex > mySize)
      Standard_OutOfRange::Raise ("NCollection_Sequence::ChangeValue");
#endif
    myCurrentItem  = Find (theIndex);
    myCurrentIndex = theIndex;
    return ((Node *) myCurrentItem) -> ChangeValue();
  }

  //! Variable operator()
  TheItemType& operator() (const Standard_Integer theIndex)
  { return ChangeValue(theIndex); }

  //! Set item value by theIndex
  void SetValue (const Standard_Integer theIndex, 
                 const TheItemType& theItem)
  { ChangeValue (theIndex) = theItem; }

  // ******** Destructor - clears the Sequence
  ~NCollection_Sequence (void)
  { Clear(); }


 private:
  // ----------- PRIVATE METHODS -----------

  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

  // ---------- FRIEND CLASSES ------------
  friend class Iterator;

};

#endif
