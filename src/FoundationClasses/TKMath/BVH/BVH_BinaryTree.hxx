// Created on: 2016-06-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _BVH_BinaryTree_Header
#define _BVH_BinaryTree_Header

#include <BVH_QuadTree.hxx>

#include <deque>
#include <tuple>

//! Specialization of binary BVH tree.
template <class T, int N>
class BVH_Tree<T, N, BVH_BinaryTree> : public BVH_TreeBase<T, N>
{
public: //! @name custom data types
  typedef typename BVH_TreeBase<T, N>::BVH_VecNt BVH_VecNt;

public: //! @name methods for accessing individual nodes
  //! Creates new empty BVH tree.
  BVH_Tree()
      : BVH_TreeBase<T, N>()
  {
  }

  //! Sets node type to 'outer'.
  void SetOuter(const int theNodeIndex)
  {
    BVH::Array<int, 4>::ChangeValue(this->myNodeInfoBuffer, theNodeIndex).x() = 1;
  }

  //! Sets node type to 'inner'.
  void SetInner(const int theNodeIndex)
  {
    BVH::Array<int, 4>::ChangeValue(this->myNodeInfoBuffer, theNodeIndex).x() = 0;
  }

  //! Returns index of the K-th child of the given inner node.
  //! \tparam K the index of node child (0 or 1)
  template <int K>
  int Child(const int theNodeIndex) const
  {
    return BVH::Array<int, 4>::Value(this->myNodeInfoBuffer, theNodeIndex)[K + 1];
  }

  //! Returns index of the K-th child of the given inner node.
  //! \tparam K the index of node child (0 or 1)
  template <int K>
  int& ChangeChild(const int theNodeIndex)
  {
    return BVH::Array<int, 4>::ChangeValue(this->myNodeInfoBuffer, theNodeIndex)[K + 1];
  }

  //! Returns index of the K-th child of the given inner node.
  //! \tparam K the index of node child (0 or 1)
  template <int K>
  int& Child(const int theNodeIndex)
  {
    return BVH::Array<int, 4>::ChangeValue(this->myNodeInfoBuffer, theNodeIndex)[K + 1];
  }

public: //! @name methods for adding/removing tree nodes
  //! Removes all nodes from the tree.
  void Clear()
  {
    this->myDepth = 0;
    BVH::Array<T, N>::Clear(this->myMinPointBuffer);
    BVH::Array<T, N>::Clear(this->myMaxPointBuffer);
    BVH::Array<int, 4>::Clear(this->myNodeInfoBuffer);
  }

  //! Reserves internal BVH storage, so that it
  //! can contain the given number of BVH nodes.
  void Reserve(const int theNbNodes)
  {
    BVH::Array<T, N>::Reserve(this->myMinPointBuffer, theNbNodes);
    BVH::Array<T, N>::Reserve(this->myMaxPointBuffer, theNbNodes);
    BVH::Array<int, 4>::Reserve(this->myNodeInfoBuffer, theNbNodes);
  }

  //! Adds new leaf node to the BVH.
  int AddLeafNode(const BVH_VecNt& theMinPoint,
                  const BVH_VecNt& theMaxPoint,
                  const int        theBegElem,
                  const int        theEndElem)
  {
    BVH::Array<T, N>::Append(this->myMinPointBuffer, theMinPoint);
    BVH::Array<T, N>::Append(this->myMaxPointBuffer, theMaxPoint);
    BVH::Array<int, 4>::Append(this->myNodeInfoBuffer, BVH_Vec4i(1, theBegElem, theEndElem, 0));
    return BVH::Array<int, 4>::Size(this->myNodeInfoBuffer) - 1;
  }

  //! Adds new inner node to the BVH.
  int AddInnerNode(const BVH_VecNt& theMinPoint,
                   const BVH_VecNt& theMaxPoint,
                   const int        theLftChild,
                   const int        theRghChild)
  {
    BVH::Array<T, N>::Append(this->myMinPointBuffer, theMinPoint);
    BVH::Array<T, N>::Append(this->myMaxPointBuffer, theMaxPoint);
    BVH::Array<int, 4>::Append(this->myNodeInfoBuffer, BVH_Vec4i(0, theLftChild, theRghChild, 0));
    return BVH::Array<int, 4>::Size(this->myNodeInfoBuffer) - 1;
  }

  //! Adds new leaf node to the BVH.
  int AddLeafNode(const BVH_Box<T, N>& theAABB, const int theBegElem, const int theEndElem)
  {
    return AddLeafNode(theAABB.CornerMin(), theAABB.CornerMax(), theBegElem, theEndElem);
  }

  //! Adds new inner node to the BVH.
  int AddInnerNode(const BVH_Box<T, N>& theAABB, const int theLftChild, const int theRghChild)
  {
    return AddInnerNode(theAABB.CornerMin(), theAABB.CornerMax(), theLftChild, theRghChild);
  }

  //! Adds new leaf node to the BVH with UNINITIALIZED bounds.
  int AddLeafNode(const int theBegElem, const int theEndElem)
  {
    BVH::Array<int, 4>::Append(this->myNodeInfoBuffer, BVH_Vec4i(1, theBegElem, theEndElem, 0));
    return BVH::Array<int, 4>::Size(this->myNodeInfoBuffer) - 1;
  }

  //! Adds new inner node to the BVH with UNINITIALIZED bounds.
  int AddInnerNode(const int theLftChild, const int theRghChild)
  {
    BVH::Array<int, 4>::Append(this->myNodeInfoBuffer, BVH_Vec4i(0, theLftChild, theRghChild, 0));
    return BVH::Array<int, 4>::Size(this->myNodeInfoBuffer) - 1;
  }

public: //! @name methods specific to binary BVH
  //! Returns value of SAH (surface area heuristic).
  //! Allows to compare the quality of BVH trees constructed for
  //! the same sets of geometric objects with different methods.
  T EstimateSAH() const;

  //! Collapses the tree into QBVH an returns it. As a result, each
  //! 2-nd level of current tree is kept and the rest are discarded.
  BVH_Tree<T, N, BVH_QuadTree>* CollapseToQuadTree() const;
};

namespace BVH
{
//! Internal function for recursive calculation of
//! surface area heuristic (SAH) of the given tree.
template <class T, int N>
void EstimateSAH(const BVH_Tree<T, N, BVH_BinaryTree>* theTree,
                 const int                             theNode,
                 T                                     theProb,
                 T&                                    theSAH)
{
  BVH_Box<T, N> aBox(theTree->MinPoint(theNode), theTree->MaxPoint(theNode));

  if (theTree->IsOuter(theNode))
  {
    theSAH += theProb * (theTree->EndPrimitive(theNode) - theTree->BegPrimitive(theNode) + 1);
  }
  else
  {
    theSAH += theProb * static_cast<T>(2.0);

    BVH_Box<T, N> aLftBox(theTree->MinPoint(theTree->template Child<0>(theNode)),
                          theTree->MaxPoint(theTree->template Child<0>(theNode)));

    if (theProb > 0.0)
    {
      EstimateSAH(theTree,
                  theTree->template Child<0>(theNode),
                  theProb * aLftBox.Area() / aBox.Area(),
                  theSAH);
    }

    BVH_Box<T, N> aRghBox(theTree->MinPoint(theTree->template Child<1>(theNode)),
                          theTree->MaxPoint(theTree->template Child<1>(theNode)));

    if (theProb > 0.0)
    {
      EstimateSAH(theTree,
                  theTree->template Child<1>(theNode),
                  theProb * aRghBox.Area() / aBox.Area(),
                  theSAH);
    }
  }
}
} // namespace BVH

// =======================================================================
// function : EstimateSAH
// purpose  :
// =======================================================================
template <class T, int N>
T BVH_Tree<T, N, BVH_BinaryTree>::EstimateSAH() const
{
  T aSAH = static_cast<T>(0.0);
  BVH::EstimateSAH<T, N>(this, 0, static_cast<T>(1.0), aSAH);
  return aSAH;
}

// =======================================================================
// function : CollapseToQuadTree
// purpose  :
// =======================================================================
template <class T, int N>
BVH_Tree<T, N, BVH_QuadTree>* BVH_Tree<T, N, BVH_BinaryTree>::CollapseToQuadTree() const
{
  BVH_Tree<T, N, BVH_QuadTree>* aQBVH = new BVH_Tree<T, N, BVH_QuadTree>;

  if (this->Length() == 0)
  {
    return aQBVH;
  }

  std::deque<std::pair<int, int>> aQueue(1, std::make_pair(0, 0));

  for (int aNbNodes = 1; !aQueue.empty();)
  {
    const std::pair<int, int> aNode = aQueue.front();

    BVH::Array<T, N>::Append(aQBVH->myMinPointBuffer,
                             BVH::Array<T, N>::Value(this->myMinPointBuffer, std::get<0>(aNode)));
    BVH::Array<T, N>::Append(aQBVH->myMaxPointBuffer,
                             BVH::Array<T, N>::Value(this->myMaxPointBuffer, std::get<0>(aNode)));

    BVH_Vec4i aNodeInfo;
    if (this->IsOuter(std::get<0>(aNode))) // is leaf node
    {
      aNodeInfo = BVH_Vec4i(1 /* leaf flag */,
                            this->BegPrimitive(std::get<0>(aNode)),
                            this->EndPrimitive(std::get<0>(aNode)),
                            std::get<1>(aNode) /* level */);
    }
    else
    {
      NCollection_Vector<int> aGrandChildNodes;

      const int aLftChild = Child<0>(std::get<0>(aNode));
      const int aRghChild = Child<1>(std::get<0>(aNode));
      if (this->IsOuter(aLftChild)) // is leaf node
      {
        aGrandChildNodes.Append(aLftChild);
      }
      else
      {
        aGrandChildNodes.Append(Child<0>(aLftChild));
        aGrandChildNodes.Append(Child<1>(aLftChild));
      }

      if (this->IsOuter(aRghChild)) // is leaf node
      {
        aGrandChildNodes.Append(aRghChild);
      }
      else
      {
        aGrandChildNodes.Append(Child<0>(aRghChild));
        aGrandChildNodes.Append(Child<1>(aRghChild));
      }

      for (int aNodeIdx = 0; aNodeIdx < aGrandChildNodes.Size(); ++aNodeIdx)
      {
        aQueue.push_back(std::make_pair(aGrandChildNodes(aNodeIdx), std::get<1>(aNode) + 1));
      }

      aNodeInfo = BVH_Vec4i(0 /* inner flag */,
                            aNbNodes,
                            aGrandChildNodes.Size() - 1,
                            std::get<1>(aNode) /* level */);

      aQBVH->myDepth = Max(aQBVH->myDepth, std::get<1>(aNode) + 1);

      aNbNodes += aGrandChildNodes.Size();
    }

    BVH::Array<int, 4>::Append(aQBVH->myNodeInfoBuffer, aNodeInfo);
    aQueue.pop_front(); // node processing completed
  }

  return aQBVH;
}

#endif // _BVH_BinaryTree_Header
