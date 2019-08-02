// Created on: 2002-10-18
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef NCollection_UBTreeFiller_HeaderFile
#define NCollection_UBTreeFiller_HeaderFile

#include <NCollection_UBTree.hxx>
#include <NCollection_Vector.hxx>

#include <random>

/**
 * This class is used to fill an UBTree in a random order.
 * The quality of a tree is much better (from the point of view of
 * the search time) if objects are added to it in a random order to
 * avoid adding a chain of neerby objects one following each other.
 *
 * This class collects objects to be added, and then add them to the tree
 * in a random order.
 */
template <class TheObjType, class TheBndType> class NCollection_UBTreeFiller
{
 public:
  // ---------- PUBLIC TYPES ----------

  //! Structure of pair (object, bnd box)
  struct ObjBnd
  {
    TheObjType  myObj;
    TheBndType  myBnd;
    ObjBnd (const TheObjType& theObj, const TheBndType& theBnd)
      : myObj(theObj), myBnd(theBnd) {}
    ObjBnd ()
      : myObj(TheObjType()), myBnd(TheBndType()) {}
  };

  //! UBTree algorithm
  typedef NCollection_UBTree<TheObjType, TheBndType>    UBTree;
  typedef TYPENAME UBTree::TreeNode                     UBTreeNode;


  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   * @param theTree
   *   Tree instance that is to be filled.
   * @param theAlloc
   *   Allocator for the Filler data.
   * @param isFullRandom
   *   Takes effect when the number of items is large (order of 50,000). When
   *   it is True, the code uses the maximal randomization allowing a better
   *   balanced tree. If False, the randomization/tree balance are worse but
   *   the tree filling is faster due to better utilisation of CPU L1/L2 cache.
   */ 
  NCollection_UBTreeFiller (UBTree& theTree,
                            const Handle(NCollection_BaseAllocator)& theAlloc=0L,
                            const Standard_Boolean isFullRandom = Standard_True)
    : myTree(theTree), mySeqPtr(256, theAlloc),
      myRandGen (5489u /* == std::mt19937::default_seed, not defined in older environments, e.g, on Debian 6.0 with GCC 4.4.5 */),
      myIsFullRandom (isFullRandom) {}

  //! Adds a pair (theObj, theBnd) to my sequence
  void Add (const TheObjType& theObj, const TheBndType& theBnd)
  { mySeqPtr.Append (ObjBnd (theObj, theBnd)); }

  /**
   * Fills the tree with the objects from my sequence. This method clears
   * the internal buffer of added items making sure that no item would be added
   * twice.
   * @return
   *   the number of objects added to the tree.
   */
  Standard_Integer Fill ();

  /**
   * Remove all data from Filler, partculary if the Tree no more needed
   * so the destructor of this Filler should not populate the useless Tree.
   */
  void                             Reset()      { mySeqPtr.Clear(); }

  /**
   * Check the filled tree for the total number of items and the balance
   * outputting these results to std::ostream.
   * @return
   *   the tree size (the same value is returned by method Fill()).
   */ 
  Standard_Integer CheckTree (Standard_OStream& theStream);

  /**
   * Destructor. Fills the tree with accumulated items if they have not been
   * passed by a previous call of method Fill().
   */
  ~NCollection_UBTreeFiller ()
  {
    if (mySeqPtr.Length() > 0)
#ifdef OCCT_DEBUG_UBTREE
      std::cout << "~NCollection_UBTreeFiller: " << Fill()
           << " objects added to the tree" << std::endl;
#else
      Fill();
#endif
  }

 private:

  // Assignment operator is made empty and private in order to
  // avoid warning on MSVC (C4512)
  void operator = (const NCollection_UBTreeFiller&) {}
  
  static Standard_Real    checkNode     (const UBTreeNode&      theNode,
                                         const Standard_Integer theLength,
                                         Standard_Integer&      theNumber);


 private:
  // ---------- PRIVATE FIELDS ----------

  UBTree&                               myTree;
  NCollection_Vector<ObjBnd>            mySeqPtr;
  opencascade::std::mt19937             myRandGen;      //!< random number generator
  Standard_Boolean                      myIsFullRandom;
};

//=======================================================================
//function : Fill
//purpose  : 
//=======================================================================

template <class TheObjType, class TheBndType>
Standard_Integer NCollection_UBTreeFiller<TheObjType,TheBndType>::Fill ()
{
  Standard_Integer i, nbAdd = mySeqPtr.Length();
  // Fisher-Yates randomization
  if (myIsFullRandom)
    for (i = nbAdd; i > 0; i--) { 
      unsigned int ind = myRandGen();
      ind = ind % i;
      const ObjBnd& aObjBnd = mySeqPtr(ind);
      myTree.Add (aObjBnd.myObj, aObjBnd.myBnd);
      mySeqPtr(ind) = mySeqPtr(i-1);
    }
  else
    for (i = nbAdd; i > 0; i--) { 
      unsigned int ind = myRandGen();
      ind = i - (ind % i) - 1;
      const ObjBnd& aObjBnd = mySeqPtr(ind);
      myTree.Add (aObjBnd.myObj, aObjBnd.myBnd);
      mySeqPtr(ind) = mySeqPtr(i-1);
    }
  mySeqPtr.Clear();
  return nbAdd;
}

//=======================================================================
//function : CheckTree
//purpose  : 
//=======================================================================

template <class TheObjType, class TheBndType>
Standard_Integer NCollection_UBTreeFiller<TheObjType,TheBndType>::CheckTree
                                        (Standard_OStream& theStream)
{
  Standard_Integer aNumber(0);
  const Standard_Real aLen = checkNode (myTree.Root(), 0, aNumber);
  const Standard_Real num = (double) aNumber;
  const Standard_Real aLen1 = sqrt (aLen / num);
  const Standard_Real aLen0 = log(num) / log(2.);
  char buf[128];
  sprintf (buf,  "Checking UBTree:%8d leaves, balance =%7.2f",
           aNumber, aLen1 / aLen0);
  theStream << buf << std::endl;
  return aNumber;
}

//=======================================================================
//function : checkNode
//purpose  : 
//=======================================================================

template <class TheObjType, class TheBndType>
Standard_Real NCollection_UBTreeFiller<TheObjType,TheBndType>::checkNode
  (const TYPENAME NCollection_UBTree<TheObjType, TheBndType>::TreeNode& theNode,
   const Standard_Integer theLength,
   Standard_Integer&      theNumber)
{
  Standard_Real aLength;
  if (!theNode.IsLeaf())
    aLength = (checkNode (theNode.Child(0), theLength+1, theNumber) +
               checkNode (theNode.Child(1), theLength+1, theNumber));
  else {
    theNumber++;
    aLength = theLength * theLength;
  }
  return aLength;
}

#endif
