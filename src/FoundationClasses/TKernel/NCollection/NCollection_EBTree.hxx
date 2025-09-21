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
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <TColStd_SequenceOfInteger.hxx>
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
  NCollection_EBTree(const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
      : UBTree(theAllocator)
  {
  }

  /**
   * Updates the tree with a new object and its bounding box.
   * Extends the functionality of the parent method by maintaining
   * the map myObjNodeMap. Redefined virtual method.
   * @return
   *   False if the tree already contains theObj.
   */
  Standard_Boolean Add(const TheObjType& theObj, const TheBndType& theBnd) Standard_OVERRIDE
  {
    Standard_Boolean result = Standard_False;
    if (!Contains(theObj))
    {
      // Add object in the tree using parent method
      UBTree::Add(theObj, theBnd);

      // Update the map
      TreeNode& aNewNode = this->ChangeLastNode();
      myObjNodeMap.Bind(theObj, &aNewNode);
      // If the new node is not the root (has a parent) check the neighbour node
      if (!aNewNode.IsRoot())
      {
        TreeNode& aNeiNode = aNewNode.ChangeParent().ChangeChild(0);
        if (aNeiNode.IsLeaf())
        {
          myObjNodeMap.UnBind(aNeiNode.Object());
          myObjNodeMap.Bind(aNeiNode.Object(), &aNeiNode);
        }
      }
      result = Standard_True;
    }
    return result;
  }

  /**
   * Removes the given object and updates the tree.
   * @return
   *   False if the tree does not contain theObj
   */
  Standard_Boolean Remove(const TheObjType& theObj);

  /**
   * @return
   *   True if the tree contains the object.
   */
  Standard_Boolean Contains(const TheObjType& theObj) const { return myObjNodeMap.IsBound(theObj); }

  /**
   * @return
   *   The leaf node containing the object.
   */
  const TreeNode& FindNode(const TheObjType& theObj) const { return *myObjNodeMap.Find(theObj); }

  /**
   * Clears the contents of the tree. Redefined virtual method
   */
  void Clear(const Handle(NCollection_BaseAllocator)& aNewAlloc = 0L) Standard_OVERRIDE
  {
    myObjNodeMap.Clear();
    UBTree::Clear(aNewAlloc);
  }

private:
  // ---------- PRIVATE METHODS ----------

  /// Copy constructor (prohibited).
  NCollection_EBTree(const NCollection_EBTree&);

  /// Assignment operator (prohibited).
  NCollection_EBTree& operator=(const NCollection_EBTree&);

  // ---------- PRIVATE FIELDS ----------

  NCollection_DataMap<TheObjType, TreeNode*> myObjNodeMap; ///< map of object to node pointer
};

// ================== METHODS TEMPLATES =====================

//=======================================================================
// function : Remove
// purpose  : Removes the given object and updates the tree.
//           Returns false if the tree does not contain theObj.
//=======================================================================

template <class TheObjType, class TheBndType>
Standard_Boolean NCollection_EBTree<TheObjType, TheBndType>::Remove(const TheObjType& theObj)
{
  Standard_Boolean result = Standard_False;
  if (Contains(theObj))
  {
    TreeNode* pNode = myObjNodeMap(theObj);
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
        // the parent node became a leaf, so update the map
        myObjNodeMap.UnBind(pParent->Object());
        myObjNodeMap.Bind(pParent->Object(), pParent);
      }
      while (!pParent->IsRoot())
      {
        pParent              = &pParent->ChangeParent();
        pParent->ChangeBnd() = pParent->Child(0).Bnd();
        pParent->ChangeBnd().Add(pParent->Child(1).Bnd());
      }
    }
    result = Standard_True;
  }
  return result;
}

// ======================================================================
// Declaration of handled version of NCollection_EBTree.
// In the macros below the arguments are:
// _HEBTREE      - the desired name of handled class
// _OBJTYPE      - the name of the object type
// _BNDTYPE      - the name of the bounding box type
// _HUBTREE      - the name of parent class
//                 (defined using macro DEFINE_HUBTREE)

#define DEFINE_HEBTREE(_HEBTREE, _OBJTYPE, _BNDTYPE, _HUBTREE)                                     \
  class _HEBTREE : public _HUBTREE                                                                 \
  {                                                                                                \
  public:                                                                                          \
    typedef NCollection_UBTree<_OBJTYPE, _BNDTYPE> UBTree;                                         \
    typedef NCollection_EBTree<_OBJTYPE, _BNDTYPE> EBTree;                                         \
                                                                                                   \
    _HEBTREE()                                                                                     \
        : _HUBTREE(new EBTree)                                                                     \
    {                                                                                              \
    }                                                                                              \
    /* Empty constructor */                                                                        \
                                                                                                   \
    /* Access to the methods of EBTree */                                                          \
                                                                                                   \
    Standard_Boolean Remove(const _OBJTYPE& theObj)                                                \
    {                                                                                              \
      return ChangeETree().Remove(theObj);                                                         \
    }                                                                                              \
                                                                                                   \
    Standard_Boolean Contains(const _OBJTYPE& theObj) const                                        \
    {                                                                                              \
      return ETree().Contains(theObj);                                                             \
    }                                                                                              \
                                                                                                   \
    const UBTree::TreeNode& FindNode(const _OBJTYPE& theObj) const                                 \
    {                                                                                              \
      return ETree().FindNode(theObj);                                                             \
    }                                                                                              \
                                                                                                   \
    /* Access to the extended tree algorithm */                                                    \
                                                                                                   \
    const EBTree& ETree() const                                                                    \
    {                                                                                              \
      return (const EBTree&)Tree();                                                                \
    }                                                                                              \
    EBTree& ChangeETree()                                                                          \
    {                                                                                              \
      return (EBTree&)ChangeTree();                                                                \
    }                                                                                              \
                                                                                                   \
    DEFINE_STANDARD_RTTI_INLINE(_HEBTREE, _HUBTREE)                                                \
    /* Type management */                                                                          \
  };                                                                                               \
  DEFINE_STANDARD_HANDLE(_HEBTREE, _HUBTREE)

#define IMPLEMENT_HEBTREE(_HEBTREE, _HUBTREE)

#endif
