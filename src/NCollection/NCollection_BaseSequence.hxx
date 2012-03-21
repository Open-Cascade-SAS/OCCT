// Created on: 2002-04-10
// Created by: Alexander KARTOMIN (akm)
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



#ifndef NCollection_BaseSequence_HeaderFile
#define NCollection_BaseSequence_HeaderFile

#include <Standard.hxx>
#include <NCollection_BaseAllocator.hxx>

// **************************************** Class SeqNode ********************

class NCollection_SeqNode 
{
 public:
  // Methods PUBLIC
  // 
  NCollection_SeqNode (void)
    : myNext (NULL), myPrevious (NULL)                {}
  const NCollection_SeqNode * Next      () const      { return myNext; }
  const NCollection_SeqNode * Previous  () const      { return myPrevious; }
  void                      SetNext     (const NCollection_SeqNode * theNext)
                                                      { myNext = theNext; }
  void                      SetPrevious (const NCollection_SeqNode * thePrev)
                                                      { myPrevious = thePrev; }
  //~NCollection_SeqNode() {
  //  if (myNext)           myNext -> myPrevious = myPrevious;
  //  if (myPrevious)       myPrevious -> myNext = myNext;
  //}
  
 private:
  // Fields PRIVATE
  //
  const NCollection_SeqNode     * myNext;
  const NCollection_SeqNode     * myPrevious;
};

typedef void (* NCollection_DelSeqNode) 
     (NCollection_SeqNode*, Handle(NCollection_BaseAllocator)& theAl);

/**
 * Purpose:     This  is  a base  class  for  the  Sequence.  It  deals with
 *              an indexed bidirectional list of NCollection_SeqNode's.
 */              
class NCollection_BaseSequence 
{
 public:
  class Iterator
  {
  public:
    //! Empty constructor
    Iterator                    (void) : myCurrent (NULL) {}
    //! Constructor with initialisation
    Iterator                    (const NCollection_BaseSequence& theSeq,
                                 const Standard_Boolean          isStart)
      : myCurrent(isStart ? (NCollection_SeqNode *)theSeq.myFirstItem
                          : (NCollection_SeqNode *)theSeq.myLastItem) {}
    //! Initialisation
    void             Init       (const NCollection_BaseSequence& theSeq,
                                 const Standard_Boolean          isStart
                                                                = Standard_True)
    { myCurrent = isStart ? (NCollection_SeqNode *)theSeq.myFirstItem
                          : (NCollection_SeqNode *)theSeq.myLastItem; }
    //! Assignment
    Iterator& operator = (const Iterator& theOther)
    { myCurrent = theOther.myCurrent; return *this; }
    //! Previous
    void             Previous   ()
    { if (myCurrent)
        myCurrent = (NCollection_SeqNode *)myCurrent -> Previous(); }
      
  protected:
    NCollection_SeqNode * myCurrent; //!< Pointer to the current node
    friend class NCollection_BaseSequence;
  };

 public:
  // Methods PUBLIC
  // 
      Standard_Boolean   IsEmpty     () const {return (mySize == 0);}
      Standard_Integer   Length      () const {return mySize;}

 protected:
  // Methods PROTECTED
  // 
  inline          NCollection_BaseSequence ();
  Standard_EXPORT void   ClearSeq    (NCollection_DelSeqNode fDel,
                                      Handle(NCollection_BaseAllocator)& theAl);
  Standard_EXPORT void   PAppend     (NCollection_SeqNode *);
  Standard_EXPORT void   PAppend     (NCollection_BaseSequence& S);
  Standard_EXPORT void   PPrepend    (NCollection_SeqNode *);
  Standard_EXPORT void   PPrepend    (NCollection_BaseSequence& S);
  Standard_EXPORT void   PInsertAfter(Iterator& thePosition,
                                      NCollection_SeqNode *);
  Standard_EXPORT void   PInsertAfter(const Standard_Integer Index,
                                      NCollection_SeqNode *);
  Standard_EXPORT void   PInsertAfter(const Standard_Integer Index,
                                      NCollection_BaseSequence& S);
  Standard_EXPORT void   PSplit      (const Standard_Integer Index,
                                      NCollection_BaseSequence& Sub);
  Standard_EXPORT void   RemoveSeq   (Iterator& thePosition,
                                      NCollection_DelSeqNode fDel,
                                      Handle(NCollection_BaseAllocator)& theAl);
  Standard_EXPORT void   RemoveSeq   (const Standard_Integer Index,
                                      NCollection_DelSeqNode fDel,
                                      Handle(NCollection_BaseAllocator)& theAl);
  Standard_EXPORT void   RemoveSeq   (const Standard_Integer From,
                                      const Standard_Integer To,
                                      NCollection_DelSeqNode fDel,
                                      Handle(NCollection_BaseAllocator)& theAl);
  Standard_EXPORT void   PReverse    ();
  Standard_EXPORT void   PExchange   (const Standard_Integer I,
                                      const Standard_Integer J) ;
  Standard_EXPORT const NCollection_SeqNode *
                         Find        (const Standard_Integer) const;

 protected:
  // Fields PROTECTED
  //
  const NCollection_SeqNode         * myFirstItem;
  const NCollection_SeqNode         * myLastItem;
  const NCollection_SeqNode         * myCurrentItem;
  Standard_Integer                  myCurrentIndex;
  Standard_Integer                  mySize;

 private: 
  // Methods PRIVATE
  // 
  Standard_EXPORT NCollection_BaseSequence
                           (const NCollection_BaseSequence& Other);
  inline void     Nullify ();
  friend class Iterator;
};

inline NCollection_BaseSequence::NCollection_BaseSequence ()
     : myFirstItem        (NULL),
       myLastItem         (NULL),
       myCurrentItem      (NULL),
       myCurrentIndex     (0),
       mySize             (0) {}

inline void NCollection_BaseSequence::Nullify ()
{
  myFirstItem = myLastItem = myCurrentItem = NULL;
  myCurrentIndex = mySize = 0;
}

#endif
