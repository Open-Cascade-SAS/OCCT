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

#ifndef NCollection_EBTree_HeaderFile
#define NCollection_EBTree_HeaderFile

#include <NCollection_UBTree.hxx>
#include <NCollection_DataMap.hxx>

/**
 * The algorithm of unbalanced binary  tree of overlapped bounding boxes with
 * the possibility of deleting objects from the tree.
 *
 * In addition to  the requirements to the object type  defined in the parent
 * class this  class requires that the  object can be hashed  and compared to
 * another object (functions HashCode and  IsEqual are defined for it), since
 * the class NCollection_DataMap  is used where the object  plays the role of
 * the key.
 */
template <class TheObjType, class TheBndType>
class NCollection_EBTree : public NCollection_UBTree<TheObjType, TheBndType>
{
public:
  typedef NCollection_UBTree<TheObjType, TheBndType> UBTree;
  typedef typename UBTree::TreeNode                  TreeNode;

  // ---------- PUBLIC METHODS ----------

  /**
   * Constructor.
   */
  NCollection_EBTree(const occ::handle<NCollection_BaseAllocator>& theAllocator = nullptr)
      : UBTree(theAllocator)
  {
  }

  NCollection_EBTree(NCollection_EBTree&& theOther) noexcept
      : UBTree(std::move(theOther)),
        myObjNodeMap(std::move(theOther.myObjNodeMap))
  {
  }

  NCollection_EBTree& operator=(NCollection_EBTree&& theOther) noexcept
  {
    if (this != &theOther)
    {
      UBTree::operator=(std::move(theOther));
      myObjNodeMap = std::move(theOther.myObjNodeMap);
    }
    return *this;
  }

  /**
   * Updates the tree with a new object and its bounding box.
   * Extends the functionality of the parent method by maintaining
   * the map myObjNodeMap. Redefined virtual method.
   * @return
   *   False if the tree already contains theObj.
   */
  bool Add(const TheObjType& theObj, const TheBndType& theBnd) override
  {
    bool result = false;
    if (!Contains(theObj))
    {
      // Add object in the tree using parent method
      UBTree::Add(theObj, theBnd);

      // Update the map
      TreeNode& aNewNode = this->ChangeLastNode();
      myObjNodeMap.Bind(theObj, &aNewNode);
      // If the new node is not the root (has a parent) check the neighbour node.
      // Gemmate moved old content to child(0), so the map entry points to stale address.
      if (!aNewNode.IsRoot())
      {
        TreeNode& aNeiNode = aNewNode.ChangeParent().ChangeChild(0);
        if (aNeiNode.IsLeaf())
        {
          TreeNode** aNeiPtr = myObjNodeMap.ChangeSeek(aNeiNode.Object());
          if (aNeiPtr)
            *aNeiPtr = &aNeiNode;
        }
      }
      result = true;
    }
    return result;
  }

  /**
   * Removes the given object and updates the tree.
   * @return
   *   False if the tree does not contain theObj
   */
  bool Remove(const TheObjType& theObj);

  /**
   * @return
   *   True if the tree contains the object.
   */
  bool Contains(const TheObjType& theObj) const { return myObjNodeMap.IsBound(theObj); }

  /**
   * @return
   *   The leaf node containing the object.
   */
  const TreeNode& FindNode(const TheObjType& theObj) const { return *myObjNodeMap.Find(theObj); }

  /**
   * Clears the contents of the tree. Redefined virtual method
   */
  void Clear(const occ::handle<NCollection_BaseAllocator>& aNewAlloc = nullptr) override
  {
    myObjNodeMap.Clear();
    UBTree::Clear(aNewAlloc);
  }

private:
  // ---------- PRIVATE METHODS ----------

  /// Copy constructor (prohibited).
  NCollection_EBTree(const NCollection_EBTree&) = delete;

  /// Assignment operator (prohibited).
  NCollection_EBTree& operator=(const NCollection_EBTree&) = delete;

  // ---------- PRIVATE FIELDS ----------

  NCollection_DataMap<TheObjType, TreeNode*> myObjNodeMap; ///< map of object to node pointer
};

//==================================================================================================

template <class TheObjType, class TheBndType>
bool NCollection_EBTree<TheObjType, TheBndType>::Remove(const TheObjType& theObj)
{
  // Single lookup using ChangeSeek instead of Contains() + operator()()
  TreeNode** pNodePtr = myObjNodeMap.ChangeSeek(theObj);
  if (!pNodePtr)
    return false;

  TreeNode* pNode = *pNodePtr;
  if (pNode->IsRoot())
  {
    // it is the root, so clear all the tree
    Clear();
  }
  else
  {
    // it is a child of some parent,
    // so kill the child that contains theObj
    // and update bounding boxes of all ancestors
    myObjNodeMap.UnBind(theObj);
    TreeNode* pParent = &pNode->ChangeParent();
    pParent->Kill((pNode == &pParent->Child(0) ? 0 : 1), this->Allocator());
    if (pParent->IsLeaf())
    {
      // The parent node became a leaf (absorbed surviving child),
      // so update the map entry to point to the new address.
      TreeNode** aParentPtr = myObjNodeMap.ChangeSeek(pParent->Object());
      if (aParentPtr)
        *aParentPtr = pParent;
    }
    while (!pParent->IsRoot())
    {
      pParent              = &pParent->ChangeParent();
      pParent->ChangeBnd() = pParent->Child(0).Bnd();
      pParent->ChangeBnd().Add(pParent->Child(1).Bnd());
    }
  }
  return true;
}

#endif
