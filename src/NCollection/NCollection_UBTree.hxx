// Created on: 2002-07-30
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

#ifndef NCollection_UBTree_HeaderFile
#define NCollection_UBTree_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

/**
 * The algorithm of unbalanced binary tree of overlapped bounding boxes.
 *
 * Once the tree of boxes  of geometric objects is constructed, the algorithm
 * is capable of fast geometric selection of objects.  The tree can be easily
 * updated by adding to it a new object with bounding box.
 * 
 * The time of adding to the tree  of one object is O(log(N)), where N is the
 * total number of  objects, so the time  of building a tree of  N objects is
 * O(N(log(N)). The search time of one object is O(log(N)).
 * 
 * Defining  various classes  inheriting NCollection_UBTree::Selector  we can
 * perform various kinds of selection over the same b-tree object
 * 
 * The object  may be of any  type allowing copying. Among  the best suitable
 * solutions there can  be a pointer to an object,  handled object or integer
 * index of object inside some  collection.  The bounding object may have any
 * dimension  and  geometry. The  minimal  interface  of TheBndType  (besides
 * public empty and copy constructor and operator =) used in UBTree algorithm
 * is as the following:
 * @code
 *   class MyBndType
 *   {
 *    public:
 *     inline void                   Add (const MyBndType& other);
 *     // Updates me with other bounding
 * 
 *     inline Standard_Boolean       IsOut (const MyBndType& other) const;
 *     // Classifies other bounding relatively me
 * 
 *     inline Standard_Real          SquareExtent() const;
 *     // Computes the squared maximal linear extent of me.
 *     // (For box it is the squared diagonal of box)
 *   };
 * @endcode
 * To select objects you need to define a class derived from UBTree::Selector
 * that  should  redefine  the  necessary  virtual methods  to  maintain  the
 * selection condition.  The object  of this class  is also used  to retrieve
 * selected objects after search.
 */

template <class TheObjType, class TheBndType> class NCollection_UBTree
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

public:
  // ---------- PUBLIC TYPES ----------

  /**
   * Class defining the minimal interface of selector.
   */
  class Selector
  {
  public:
    /**
     * Constructor
     */
    Selector () : myStop(Standard_False) {}

    /**
     * Rejection base on the bounding type.
     * @return
     *   True if the bounding box does not conform to some selection conditions
     */
    virtual Standard_Boolean Reject (const TheBndType&) const = 0;

    /**
     * Confirm the object while making necessary tests on it. This method is
     * called when the bounding box of the object conforms to the conditions
     * (see Reject()). It is also supposed to keep record of accepted objects.
     * @return
     *   True if the object is accepted
     */
    virtual Standard_Boolean Accept (const TheObjType&) = 0;

    /**
     * This condition is checked after each call to Accept().
     * @return
     *   True signals that the selection process is stopped
     */
    Standard_Boolean Stop () const { return myStop; }

    /**
     * Destructor
     */
    virtual ~Selector () {}

  protected:
    /**
     * The method Accept() should set this flag if the selection process
     * is to be stopped
     */
    Standard_Boolean myStop;
  };

  /**
   * Class describing the node of the tree.
   * Initially the tree consists of one leaf. A node can grow to a branch
   * holding two childs:
   * - one correspondent to initial node
   * - the new one with a new object and bounding box
   */
  class TreeNode
  {
  public:
    DEFINE_STANDARD_ALLOC
    DEFINE_NCOLLECTION_ALLOC

  public:
    TreeNode (const TheObjType& theObj, const TheBndType& theBnd)
      : myBnd(theBnd), myObject(theObj), myChildren(0), myParent(0) {}

    Standard_Boolean       IsLeaf       () const { return !myChildren; }
    Standard_Boolean       IsRoot       () const { return !myParent; }
    const TheBndType&      Bnd          () const { return myBnd; }
    TheBndType&            ChangeBnd    ()       { return myBnd; }
    const TheObjType&      Object       () const { return myObject; }
    const TreeNode&        Child        (const Standard_Integer i) const
                                                 { return myChildren[i]; }
    TreeNode&              ChangeChild  (const Standard_Integer i)
                                                 { return myChildren[i]; }
    const TreeNode&        Parent       () const { return *myParent; }
    TreeNode&              ChangeParent ()       { return *myParent; }

    /**
     * Forces *this node being gemmated such a way that it becomes
     * a branch holding the previous content of *this node at the 
     * first child and theObj at the second child.
     * @param TheNewBnd
     *   new bounding box comprizing both child nodes.
     * @param theObj
     *   added object.
     * @param theBnd
     *   bounding box of theObj.
     * @theAlloc
     *   allocator providing memory to the new child nodes, provided by the
     *   calling Tree instance.
     */
    void Gemmate            (const TheBndType& theNewBnd,
                             const TheObjType& theObj,
                             const TheBndType& theBnd,
                             const Handle(NCollection_BaseAllocator)& theAlloc)
    {
      //TreeNode *children = new TreeNode [2];
      TreeNode *children = (TreeNode *) theAlloc->Allocate (2*sizeof(TreeNode));
      new (&children[0]) TreeNode;
      new (&children[1]) TreeNode;
      children[0] = *this;
      children[1].myObject = theObj;
      children[1].myBnd = theBnd;
      children[0].myParent = children[1].myParent = this;
      if (!IsLeaf()) {
        myChildren[0].myParent = children;
        myChildren[1].myParent = children;
      }
      myChildren = children;
      myBnd = theNewBnd;
      myObject = TheObjType();      // nullify myObject
    }

    /**
     * Kills the i-th child, and *this accepts the content of another child
     */
    void Kill               (const Standard_Integer i,
                             const Handle(NCollection_BaseAllocator)& theAlloc)
    {
      if (!IsLeaf()) {
        TreeNode *oldChildren = myChildren;
        const Standard_Integer iopp = 1 - i;
        myBnd      = oldChildren[iopp].myBnd;
        myObject   = oldChildren[iopp].myObject;
        myChildren = oldChildren[iopp].myChildren;
        if (!IsLeaf()) {
          myChildren[0].myParent = this;
          myChildren[1].myParent = this;
        }
        // oldChildren[0].myChildren = oldChildren[1].myChildren = 0L;
        // delete [] oldChildren;
        oldChildren[iopp].~TreeNode();
        delNode(&oldChildren[i], theAlloc); // remove the whole branch
        theAlloc->Free(oldChildren);
      }
    }

//  ~TreeNode () { if (myChildren) delete [] myChildren; }
    ~TreeNode () { myChildren = 0L; }

    /**
     * Deleter of tree node. The whole hierarchy of its children also deleted.
     * This method should be used instead of operator delete.
     */ 
    static void delNode (TreeNode * theNode,
                         const Handle(NCollection_BaseAllocator)& theAlloc)
    {
      if (theNode) {
        if (theNode -> myChildren) {
          delNode (&theNode -> myChildren[0], theAlloc);
          delNode (&theNode -> myChildren[1], theAlloc);
          theAlloc->Free(theNode -> myChildren);
        }
        theNode->~TreeNode();
      }
    }

  private:
    TreeNode () : myChildren(0L), myParent(0L) {}

    TheBndType  myBnd;          ///< bounding geometry
    TheObjType  myObject;       ///< the object
    TreeNode   *myChildren;     ///< 2 children forming a b-tree
    TreeNode   *myParent;       ///< the pointer to a parent node
  };

  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  NCollection_UBTree() : myRoot(0L), myLastNode(0L), myAlloc (NCollection_BaseAllocator::CommonBaseAllocator()) {}

  /**
   * Constructor.
   */
  explicit NCollection_UBTree (const Handle(NCollection_BaseAllocator)& theAllocator)
  : myRoot(0L), myLastNode(0L), myAlloc (!theAllocator.IsNull() ? theAllocator : NCollection_BaseAllocator::CommonBaseAllocator()) {}

  /**
   * Update the tree with a new object and its bounding box.
   * @param theObj
   *   added object
   * @param theBnd
   *   bounding box of the object.
   * @return
   *   always True
   */
  virtual Standard_Boolean Add (const TheObjType& theObj, const TheBndType& theBnd);

  /**
   * Searches in the tree all objects conforming to the given selector.
   * return
   *   Number of objects accepted
   */
  virtual Standard_Integer Select (Selector& theSelector) const
        { return (IsEmpty() ? 0 : Select (Root(), theSelector)); }

  /**
   * Clears the contents of the tree.
   * @param aNewAlloc
   *   Optional:   a new allocator that will be used when the tree is rebuilt
   *   anew. This makes sense if the memory allocator needs re-initialisation
   *   (like NCollection_IncAllocator).  By default the previous allocator is
   *   kept.
   */
  virtual void Clear (const Handle(NCollection_BaseAllocator)& aNewAlloc = 0L)
//      { if (myRoot) delete myRoot; myRoot = 0L; }
  {
    if (myRoot) {
      TreeNode::delNode (myRoot, this->myAlloc);
      this->myAlloc->Free (myRoot);
      myRoot = 0L;
    }
    if (aNewAlloc.IsNull() == Standard_False)
      myAlloc = aNewAlloc;
  }

  Standard_Boolean IsEmpty () const { return !myRoot; }

  /**
   * @return
   *   the root node of the tree
   */
  const TreeNode& Root () const { return *myRoot; }

  /**
   * Destructor.
   */
  virtual ~NCollection_UBTree () { Clear(); }

  /**
   * Recommended to be used only in sub-classes.
   * @return
   *   Allocator object used in this instance of UBTree.
   */
  const Handle(NCollection_BaseAllocator)& Allocator () const
  { return myAlloc; }

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * @return
   *   the last added node
   */
  TreeNode& ChangeLastNode () { return *myLastNode; }

  /**
   * Searches in the branch all objects conforming to the given selector.
   * @return
   *   the number of objects accepted
   */
  Standard_Integer Select (const TreeNode& theBranch, Selector& theSelector) const;

 private:
  // ---------- PRIVATE METHODS ----------

  /// Copy constructor (prohibited).
  NCollection_UBTree (const NCollection_UBTree&);

  /// Assignment operator (prohibited).
  NCollection_UBTree& operator = (const NCollection_UBTree&);

  // ---------- PRIVATE FIELDS ----------

  TreeNode                            *myRoot;    ///< root of the tree
  TreeNode                            *myLastNode;///< the last added node
  Handle(NCollection_BaseAllocator)    myAlloc;   ///< Allocator for TreeNode
};

// ================== METHODS TEMPLATES =====================
//=======================================================================
//function : Add
//purpose  : Updates the tree with a new object and its bounding box
//=======================================================================

template <class TheObjType, class TheBndType>
Standard_Boolean NCollection_UBTree<TheObjType,TheBndType>::Add
                        (const TheObjType& theObj,
                         const TheBndType& theBnd)
{
  if (IsEmpty()) {
    // Accepting first object
    myRoot = new (this->myAlloc) TreeNode (theObj, theBnd);
    myLastNode = myRoot;
    return Standard_True;
  }

  TreeNode *pBranch = myRoot;
  Standard_Boolean isOutOfBranch = pBranch->Bnd().IsOut (theBnd);

  for(;;) {
    // condition of stopping the search
    if (isOutOfBranch || pBranch->IsLeaf()) {
      TheBndType aNewBnd = theBnd;
      aNewBnd.Add (pBranch->Bnd());
      // put the new leaf aside on the level of pBranch
      pBranch->Gemmate (aNewBnd, theObj, theBnd, this->myAlloc);
      myLastNode = &pBranch->ChangeChild(1);
      break;
    }

    // Update the bounding box of the branch
    pBranch->ChangeBnd().Add (theBnd);

    // Select the best child branch to accept the object:
    // 1. First check if one branch is out and another one is not.
    // 2. Else select the child having the least union with theBnd
    Standard_Integer iBest = 0;
    Standard_Boolean isOut[] = { pBranch->Child(0).Bnd().IsOut (theBnd),
                                 pBranch->Child(1).Bnd().IsOut (theBnd) };
    if (isOut[0] != isOut[1])
      iBest = (isOut[0] ? 1 : 0);
    else {
      TheBndType aUnion[] = { theBnd, theBnd };
      aUnion[0].Add (pBranch->Child(0).Bnd());
      aUnion[1].Add (pBranch->Child(1).Bnd());
      const Standard_Real d1 = aUnion[0].SquareExtent();
      const Standard_Real d2 = aUnion[1].SquareExtent();
      if (d1 > d2)
        iBest = 1;
    }

    // Continue with the selected branch
    isOutOfBranch = isOut[iBest];
    pBranch = &pBranch->ChangeChild(iBest);
  }
  return Standard_True;
}

//=======================================================================
//function : Select
//purpose  : Recursively searches in the branch all objects conforming 
//           to the given selector.
//           Returns the number of objects found.
//=======================================================================

template <class TheObjType, class TheBndType>
Standard_Integer NCollection_UBTree<TheObjType,TheBndType>::Select
                                 (const TreeNode& theBranch,
                                  Selector&       theSelector) const
{
  // Try to reject the branch by bounding box
  if (theSelector.Reject (theBranch.Bnd()))
    return 0;

  Standard_Integer nSel = 0;

  if (theBranch.IsLeaf()) {
    // It is a leaf => try to accept the object
    if (theSelector.Accept (theBranch.Object()))
      nSel++;
  }
  else {
    // It is a branch => select from its children
    nSel += Select (theBranch.Child(0), theSelector);
    if (!theSelector.Stop())
      nSel += Select (theBranch.Child(1), theSelector);
  }

  return nSel;
}

// ======================================================================
/**
 * Declaration of handled version of NCollection_UBTree.
 * In the macros below the arguments are:
 * _HUBTREE      - the desired name of handled class
 * _OBJTYPE      - the name of the object type
 * _BNDTYPE      - the name of the bounding box type
 * _HPARENT      - the name of parent class (usually Standard_Transient)
 */
#define DEFINE_HUBTREE(_HUBTREE, _OBJTYPE, _BNDTYPE, _HPARENT)          \
class _HUBTREE : public _HPARENT                                        \
{                                                                       \
 public:                                                                \
  typedef NCollection_UBTree <_OBJTYPE, _BNDTYPE> UBTree;               \
                                                                        \
  _HUBTREE () : myTree(new UBTree) {}                                   \
  /* Empty constructor */                                               \
  _HUBTREE (const Handle(NCollection_BaseAllocator)& theAlloc)           \
     : myTree(new UBTree(theAlloc)) {}                                  \
  /* Constructor */                                                     \
                                                                        \
  /* Access to the methods of UBTree */                                 \
                                                                        \
  Standard_Boolean Add (const _OBJTYPE& theObj,                         \
                        const _BNDTYPE& theBnd)                         \
        { return ChangeTree().Add (theObj, theBnd); }                   \
                                                                        \
  Standard_Integer Select (UBTree::Selector& theSelector) const         \
        { return Tree().Select (theSelector); }                         \
                                                                        \
  void Clear () { ChangeTree().Clear (); }                              \
                                                                        \
  Standard_Boolean IsEmpty () const { return Tree().IsEmpty(); }        \
                                                                        \
  const UBTree::TreeNode& Root () const { return Tree().Root(); }       \
                                                                        \
                                                                        \
  /* Access to the tree algorithm */                                    \
                                                                        \
  const UBTree& Tree () const { return *myTree; }                       \
  UBTree&       ChangeTree () { return *myTree; }                       \
                                                                        \
  ~_HUBTREE () { delete myTree; }                                       \
  /* Destructor */                                                      \
                                                                        \
  DEFINE_STANDARD_RTTI_INLINE(_HUBTREE,_HPARENT)                                       \
  /* Type management */                                                 \
                                                                        \
 private:                                                               \
  /* Copying and assignment are prohibited  */                          \
  _HUBTREE (UBTree*);                                                   \
  _HUBTREE (const _HUBTREE&);                                           \
  void operator = (const _HUBTREE&);                                    \
                                                                        \
 private:                                                               \
  UBTree       *myTree;        /* pointer to the tree algorithm */      \
};                                                                      \
DEFINE_STANDARD_HANDLE (_HUBTREE, _HPARENT)

#define IMPLEMENT_HUBTREE(_HUBTREE, _HPARENT)                           



#endif
