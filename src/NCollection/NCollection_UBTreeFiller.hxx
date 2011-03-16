// File:      NCollection_UBTreeFiller.hxx
// Created:   18.10.02 10:34:57
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#ifndef NCollection_UBTreeFiller_HeaderFile
#define NCollection_UBTreeFiller_HeaderFile

#include <NCollection_UBTree.hxx>
#include <NCollection_Vector.hxx>
#include <stdlib.h>
#include <stdio.h>

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
   * @param isFullRandom
   *   Takes effect when the number of items is large (order of 50,000). When
   *   it is True, the code uses the maximal randomization allowing a better
   *   balanced tree. If False, the randomization/tree balance are worse but
   *   the tree filling is faster due to better utilisation of CPU L1/L2 cache.
   */ 
  NCollection_UBTreeFiller (UBTree& theTree,
                            const Standard_Boolean isFullRandom = Standard_True)
    : myTree(theTree), mySeed(1), myIsFullRandom (isFullRandom)
    {
#ifndef _REENTRANT
      // We use srand/rand for a single threaded application
      // and rand_r for a multi threaded one.
      // _REENTRANT must be defined for a multi threaded application.
      srand (mySeed);
#endif
    }

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
  Standard_EXPORT Standard_Integer Fill ();

  /**
   * Check the filled tree for the total number of items and the balance
   * outputting these results to ostream.
   * @return
   *   the tree size (the same value is returned by method Fill()).
   */ 
  Standard_EXPORT Standard_Integer CheckTree (Standard_OStream& theStream);

  /**
   * Destructor. Fills the tree with accumulated items if they have not been
   * passed by a previous call of method Fill().
   */
  ~NCollection_UBTreeFiller ()
  {
    if (mySeqPtr.Length() > 0)
#ifdef DEB_UBTREE
      cout << "~NCollection_UBTreeFiller: " << Fill()
           << " objects added to the tree" << endl;
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
  int                                   mySeed;         //!< seed for rand
  Standard_Boolean                      myIsFullRandom;
};

#ifdef _REENTRANT
inline int take_random (int * theSeed)
{
  return rand_r ((unsigned *) theSeed);
}
#else
inline int take_random (int *)
{
  return rand();
}
#endif

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
      unsigned int ind = take_random(&mySeed);
      if (i > RAND_MAX) {
        const unsigned int ind1 = take_random(&mySeed);
        ind += (ind1 << 15);
      }
      ind = ind % i;
      const ObjBnd& aObjBnd = mySeqPtr(ind);
      myTree.Add (aObjBnd.myObj, aObjBnd.myBnd);
      mySeqPtr(ind) = mySeqPtr(i-1);
    }
  else
    for (i = nbAdd; i > 0; i--) { 
      unsigned int ind = take_random(&mySeed);
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
  theStream << buf << endl;
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
