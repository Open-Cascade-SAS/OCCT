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
#include <NCollection_LocalArray.hxx>

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
 *     inline bool       IsOut (const MyBndType& other) const;
 *     // Classifies other bounding relatively me
 *
 *     inline double          SquareExtent() const;
 *     // Computes the squared maximal linear extent of me.
 *     // (For box it is the squared diagonal of box)
 *   };
 * @endcode
 * To select objects you need to define a class derived from UBTree::Selector
 * that  should  redefine  the  necessary  virtual methods  to  maintain  the
 * selection condition.  The object  of this class  is also used  to retrieve
 * selected objects after search.
 */

template <class TheObjType, class TheBndType>
class NCollection_UBTree
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
    Selector()
        : myStop(false)
    {
    }

    /**
     * Rejection base on the bounding type.
     * @return
     *   True if the bounding box does not conform to some selection conditions
     */
    virtual bool Reject(const TheBndType&) const = 0;

    /**
     * Confirm the object while making necessary tests on it. This method is
     * called when the bounding box of the object conforms to the conditions
     * (see Reject()). It is also supposed to keep record of accepted objects.
     * @return
     *   True if the object is accepted
     */
    virtual bool Accept(const TheObjType&) = 0;

    /**
     * This condition is checked after each call to Accept().
     * @return
     *   True signals that the selection process is stopped
     */
    bool Stop() const noexcept { return myStop; }

    /**
     * Destructor
     */
    virtual ~Selector() = default;

  protected:
    /**
     * The method Accept() should set this flag if the selection process
     * is to be stopped
     */
    bool myStop;
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
    TreeNode(const TheObjType& theObj, const TheBndType& theBnd)
        : myBnd(theBnd),
          myObject(theObj),
          myChildren(nullptr),
          myParent(nullptr)
    {
    }

    bool IsLeaf() const noexcept { return !myChildren; }

    bool IsRoot() const noexcept { return !myParent; }

    const TheBndType& Bnd() const noexcept { return myBnd; }

    TheBndType& ChangeBnd() noexcept { return myBnd; }

    const TheObjType& Object() const noexcept { return myObject; }

    const TreeNode& Child(const int i) const noexcept { return myChildren[i]; }

    TreeNode& ChangeChild(const int i) noexcept { return myChildren[i]; }

    const TreeNode& Parent() const noexcept { return *myParent; }

    TreeNode& ChangeParent() noexcept { return *myParent; }

    /**
     * Forces *this node being gemmated such a way that it becomes
     * a branch holding the previous content of *this node at the
     * first child and theObj at the second child.
     * @param theNewBnd
     *   new bounding box comprizing both child nodes.
     * @param theObj
     *   added object.
     * @param theBnd
     *   bounding box of theObj.
     * @param theAlloc
     *   allocator providing memory to the new child nodes, provided by the
     *   calling Tree instance.
     */
    void Gemmate(const TheBndType&                             theNewBnd,
                 const TheObjType&                             theObj,
                 const TheBndType&                             theBnd,
                 const occ::handle<NCollection_BaseAllocator>& theAlloc)
    {
      TreeNode* children = (TreeNode*)theAlloc->Allocate(2 * sizeof(TreeNode));
      new (&children[0]) TreeNode;
      new (&children[1]) TreeNode;
      children[0]          = *this;
      children[1].myObject = theObj;
      children[1].myBnd    = theBnd;
      children[0].myParent = children[1].myParent = this;
      if (!IsLeaf())
      {
        myChildren[0].myParent = children;
        myChildren[1].myParent = children;
      }
      myChildren = children;
      myBnd      = theNewBnd;
      myObject   = TheObjType(); // nullify myObject
    }

    /**
     * Kills the i-th child, and *this accepts the content of another child
     */
    void Kill(const int i, const occ::handle<NCollection_BaseAllocator>& theAlloc)
    {
      if (!IsLeaf())
      {
        TreeNode* oldChildren = myChildren;
        const int iopp        = 1 - i;
        myBnd                 = oldChildren[iopp].myBnd;
        myObject              = oldChildren[iopp].myObject;
        myChildren            = oldChildren[iopp].myChildren;
        if (!IsLeaf())
        {
          myChildren[0].myParent = this;
          myChildren[1].myParent = this;
        }
        oldChildren[iopp].~TreeNode();
        delNode(&oldChildren[i], theAlloc); // remove the whole branch
        theAlloc->Free(oldChildren);
      }
    }

    ~TreeNode() { myChildren = nullptr; }

    //! Deleter of tree node. The whole hierarchy of its children is also deleted.
    //! This method should be used instead of operator delete.
    //! Uses iterative traversal to avoid stack overflow on deeply unbalanced trees.
    static void delNode(TreeNode* theNode, const occ::handle<NCollection_BaseAllocator>& theAlloc)
    {
      if (!theNode)
        return;

      // Collect children arrays during pre-order traversal, free them after.
      NCollection_LocalArray<TreeNode*, 64> aChildArrays(64);
      NCollection_LocalArray<TreeNode*, 64> aStack(64);
      int                                   aNumArrays = 0;
      int                                   aTop       = 0;

      aStack[aTop++] = theNode;

      while (aTop > 0)
      {
        TreeNode* aNode = aStack[--aTop];
        if (aNode->myChildren)
        {
          // Record children array for later freeing
          if (aNumArrays >= static_cast<int>(aChildArrays.Size()))
            aChildArrays.Reallocate(aChildArrays.Size() * 2, true);
          aChildArrays[aNumArrays++] = aNode->myChildren;

          // Push both children for traversal
          if (aTop + 2 > static_cast<int>(aStack.Size()))
            aStack.Reallocate(aStack.Size() * 2, true);
          aStack[aTop++] = &aNode->myChildren[1];
          aStack[aTop++] = &aNode->myChildren[0];
        }
        aNode->~TreeNode();
      }

      // Free all collected children arrays
      for (int i = 0; i < aNumArrays; ++i)
        theAlloc->Free(aChildArrays[i]);
    }

  private:
    TreeNode()
        : myChildren(nullptr),
          myParent(nullptr)
    {
    }

    TheBndType myBnd;      ///< bounding geometry
    TheObjType myObject;   ///< the object
    TreeNode*  myChildren; ///< 2 children forming a b-tree
    TreeNode*  myParent;   ///< the pointer to a parent node
  };

  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  NCollection_UBTree()
      : myRoot(nullptr),
        myLastNode(nullptr),
        myAlloc(NCollection_BaseAllocator::CommonBaseAllocator())
  {
  }

  /**
   * Constructor.
   */
  explicit NCollection_UBTree(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myRoot(nullptr),
        myLastNode(nullptr),
        myAlloc(!theAllocator.IsNull() ? theAllocator
                                       : NCollection_BaseAllocator::CommonBaseAllocator())
  {
  }

  NCollection_UBTree(NCollection_UBTree&& theOther) noexcept
      : myRoot(theOther.myRoot),
        myLastNode(theOther.myLastNode),
        myAlloc(std::move(theOther.myAlloc))
  {
    theOther.myRoot = nullptr;
    theOther.myLastNode = nullptr;
  }

  NCollection_UBTree& operator=(NCollection_UBTree&& theOther) noexcept
  {
    if (this != &theOther)
    {
      Clear();
      myRoot = theOther.myRoot;
      myLastNode = theOther.myLastNode;
      myAlloc = std::move(theOther.myAlloc);
      theOther.myRoot = nullptr;
      theOther.myLastNode = nullptr;
    }
    return *this;
  }

  /**
   * Update the tree with a new object and its bounding box.
   * @param theObj
   *   added object
   * @param theBnd
   *   bounding box of the object.
   * @return
   *   always True
   */
  virtual bool Add(const TheObjType& theObj, const TheBndType& theBnd);

  /**
   * Searches in the tree all objects conforming to the given selector.
   * @return
   *   Number of objects accepted
   */
  virtual int Select(Selector& theSelector) const
  {
    return (IsEmpty() ? 0 : Select(Root(), theSelector));
  }

  /**
   * Clears the contents of the tree.
   * @param aNewAlloc
   *   Optional:   a new allocator that will be used when the tree is rebuilt
   *   anew. This makes sense if the memory allocator needs re-initialisation
   *   (like NCollection_IncAllocator).  By default the previous allocator is
   *   kept.
   */
  virtual void Clear(const occ::handle<NCollection_BaseAllocator>& aNewAlloc = nullptr)
  {
    if (myRoot)
    {
      TreeNode::delNode(myRoot, this->myAlloc);
      this->myAlloc->Free(myRoot);
      myRoot = nullptr;
    }
    if (!aNewAlloc.IsNull())
      myAlloc = aNewAlloc;
  }

  bool IsEmpty() const noexcept { return !myRoot; }

  /**
   * @return
   *   the root node of the tree
   */
  const TreeNode& Root() const noexcept { return *myRoot; }

  /**
   * Destructor.
   */
  virtual ~NCollection_UBTree() { Clear(); }

  /**
   * Recommended to be used only in sub-classes.
   * @return
   *   Allocator object used in this instance of UBTree.
   */
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept { return myAlloc; }

protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * @return
   *   the last added node
   */
  TreeNode& ChangeLastNode() noexcept { return *myLastNode; }

  /**
   * Searches in the branch all objects conforming to the given selector.
   * @return
   *   the number of objects accepted
   */
  int Select(const TreeNode& theBranch, Selector& theSelector) const;

private:
  // ---------- PRIVATE METHODS ----------

  /// Copy constructor (prohibited).
  NCollection_UBTree(const NCollection_UBTree&) = delete;

  /// Assignment operator (prohibited).
  NCollection_UBTree& operator=(const NCollection_UBTree&) = delete;

  // ---------- PRIVATE FIELDS ----------

  TreeNode*                              myRoot;     ///< root of the tree
  TreeNode*                              myLastNode; ///< the last added node
  occ::handle<NCollection_BaseAllocator> myAlloc;    ///< Allocator for TreeNode
};

//==================================================================================================

template <class TheObjType, class TheBndType>
bool NCollection_UBTree<TheObjType, TheBndType>::Add(const TheObjType& theObj,
                                                     const TheBndType& theBnd)
{
  if (IsEmpty())
  {
    // Accepting first object
    myRoot     = new (this->myAlloc) TreeNode(theObj, theBnd);
    myLastNode = myRoot;
    return true;
  }

  TreeNode* pBranch       = myRoot;
  bool      isOutOfBranch = pBranch->Bnd().IsOut(theBnd);

  for (;;)
  {
    // condition of stopping the search
    if (isOutOfBranch || pBranch->IsLeaf())
    {
      TheBndType aNewBnd = theBnd;
      aNewBnd.Add(pBranch->Bnd());
      // put the new leaf aside on the level of pBranch
      pBranch->Gemmate(aNewBnd, theObj, theBnd, this->myAlloc);
      myLastNode = &pBranch->ChangeChild(1);
      break;
    }

    // Update the bounding box of the branch
    pBranch->ChangeBnd().Add(theBnd);

    // Select the best child branch to accept the object:
    // 1. First check if one branch is out and another one is not.
    // 2. Else select the child having the least union with theBnd
    int  iBest   = 0;
    bool isOut[] = {pBranch->Child(0).Bnd().IsOut(theBnd), pBranch->Child(1).Bnd().IsOut(theBnd)};
    if (isOut[0] != isOut[1])
      iBest = (isOut[0] ? 1 : 0);
    else
    {
      TheBndType aUnion[] = {theBnd, theBnd};
      aUnion[0].Add(pBranch->Child(0).Bnd());
      aUnion[1].Add(pBranch->Child(1).Bnd());
      const double d1 = aUnion[0].SquareExtent();
      const double d2 = aUnion[1].SquareExtent();
      if (d1 > d2)
        iBest = 1;
    }

    // Continue with the selected branch
    isOutOfBranch = isOut[iBest];
    pBranch       = &pBranch->ChangeChild(iBest);
  }
  return true;
}

//==================================================================================================

template <class TheObjType, class TheBndType>
int NCollection_UBTree<TheObjType, TheBndType>::Select(const TreeNode& theBranch,
                                                       Selector&       theSelector) const
{
  // Explicit stack for iterative DFS. 64 covers balanced trees up to 2^64 nodes;
  // Reallocate handles deeply unbalanced trees.
  NCollection_LocalArray<const TreeNode*, 64> aStack(64);
  int                                         aTop = 0;
  int                                         nSel = 0;

  aStack[aTop++] = &theBranch;

  while (aTop > 0)
  {
    const TreeNode* aNode = aStack[--aTop];

    if (theSelector.Reject(aNode->Bnd()))
      continue;

    if (aNode->IsLeaf())
    {
      if (theSelector.Accept(aNode->Object()))
        nSel++;
      if (theSelector.Stop())
        break;
    }
    else
    {
      // Ensure stack has space for 2 children
      if (aTop + 2 > static_cast<int>(aStack.Size()))
        aStack.Reallocate(aStack.Size() * 2, true);
      // Push child(1) first so child(0) is processed first (LIFO order)
      aStack[aTop++] = &aNode->Child(1);
      aStack[aTop++] = &aNode->Child(0);
    }
  }
  return nSel;
}

#endif
