// Created by: Eugeny MALTCHIKOV
// Created on: 2019-04-17
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _BVH_Traverse_Header
#define _BVH_Traverse_Header

#include <BVH_Box.hxx>
#include <BVH_Tree.hxx>

//! The classes implement the traverse of the BVH tree.
//!
//! There are two traverse methods implemented:
//! - Traverse of the single tree
//! - Parallel traverse of two trees
//!
//! To perform Selection of the elements from BVH_Tree using
//! the traverse methods implemented here it is
//! required to define Acceptance/Rejection rules in the
//! following methods:
//! - *RejectNode* - Node rejection by its bounding box.
//!   It is applied to both inner and outer nodes of the BVH tree.
//!   Optionally, the method should compute the metric for the node
//!   which will allow performing traverse faster by descending by the
//!   best branches.
//! - *Accept* - Element acceptance. It takes the index of the element
//!   of BVH tree. The access to the element itself should be performed
//!   through the set on which BVH is built.
//!   The *Accept* method implements the leaf node operation and usually
//!   defines the logic of the whole operation.
//! - *IsMetricBetter* - Compares the metrics of the nodes and returns
//!   true if the left metric is better than the right one.
//! - *RejectMetric* - Node rejection by the metric. It should compare
//!   the metric of the node with the global one and return true
//!   if the global metric is better than the given one.
//! - *Stop* - implements conditions to stop the tree descend if the necessary
//!   elements are already found.
//!
//! The selector of a single tree has an extra method which allows
//! accepting the whole branches without any further checks
//! (e.g. full inclusion test):
//! - *AcceptMetric* - basing on the metric of the node decides if the
//!   node may be accepted without any further checks.
//!
//! Two ways of selection are possible:
//! 1. Set the BVH set containing the tree and use the method Select()
//!    which allows using common interface for setting the BVH Set for accessing
//!    the BVH tree and elements in the Accept method.
//! 2. Keep the BVHSetType void, do not set the BVH set and use the
//!    method Select (const BVH_Tree<>&) which allows performing selection
//!    on the arbitrary BVH tree.
//!
//! Here is the example of usage of the traverse to find the point-triangulation
//! minimal distance.
//! ~~~~
//! // Structure to contain points of the triangle
//! struct Triangle
//! {
//!   Triangle() {}
//!   Triangle(const BVH_Vec3d& theNode1,
//!            const BVH_Vec3d& theNode2,
//!            const BVH_Vec3d& theNode3)
//!     : Node1 (theNode1), Node2 (theNode2), Node3 (theNode3)
//!   {}
//!
//!   BVH_Vec3d Node1;
//!   BVH_Vec3d Node2;
//!   BVH_Vec3d Node3;
//! };
//!
//! // Selector for min point-triangulation distance
//! class BVH_PointTriangulationSqDist :
//!   public BVH_Distance<Standard_Real, 3, BVH_Vec3d, BVH_BoxSet<Standard_Real, 3, Triangle>>
//! {
//! public:
//!
//!   // Computes the distance from the point to bounding box
//!   virtual Standard_Boolean RejectNode (const BVH_Vec3d& theCMin,
//!                                        const BVH_Vec3d& theCMax,
//!                                        Standard_Real& theDistance) const Standard_OVERRIDE
//!   {
//!     theDistance = BVH_Tools<Standard_Real, 3>::PointBoxSquareDistance (myObject, theCMin,
//!     theCMax); return RejectMetric (theDistance);
//!   }
//!
//!   // Computes the distance from the point to triangle
//!   virtual Standard_Boolean Accept (const Standard_Integer theIndex,
//!                                    const Standard_Real&) Standard_OVERRIDE
//!   {
//!     const Triangle& aTri = myBVHSet->Element (theIndex);
//!     Standard_Real aDist = BVH_Tools<Standard_Real, 3>::PointTriangleSquareDistance (myObject,
//!     aTri.Node1, aTri.Node2, aTri.Node3); if (aDist < myDistance)
//!     {
//!       myDistance = aDist;
//!       return Standard_True;
//!     }
//!     return Standard_False;
//!   }
//! };
//!
//! // Point to which the distance is required
//! BVH_Vec3d aPoint = ...;
//! // BVH Set containing triangulation
//! opencascade::handle<BVH_BoxSet<Standard_Real, 3, Triangle>> aTriangulationSet = ...;
//!
//! BVH_PointTriangulationSqDist aDistTool;
//! aDistTool.SetObject (aPoint);
//! aDistTool.SetBVHSet (aTriangulationSet.get());
//! aDistTool.ComputeDistance();
//! if (aDistTool.IsDone())
//! {
//!   Standard_Real aPointTriSqDist = aDistTool.Distance();
//! }
//!
//! ~~~~
//!

//! Abstract class implementing the base Traverse interface
//! required for selection of the elements from BVH tree.
//!
//! \tparam MetricType Type of metric to perform more optimal tree descend
template <class MetricType>
class BVH_BaseTraverse
{
public: //! @name Metrics comparison for choosing the best branch
  //! Compares the two metrics and chooses the best one.
  //! Returns true if the first metric is better than the second,
  //! false otherwise.
  virtual Standard_Boolean IsMetricBetter(const MetricType&, const MetricType&) const
  {
    // Keep the left to right tree descend by default
    return Standard_True;
  }

public: //! @name Rejection of the node by metric
  //! Rejects the node by the metric
  virtual Standard_Boolean RejectMetric(const MetricType&) const
  {
    // Do not reject any nodes by metric by default
    return Standard_False;
  }

public: //! @name Condition to stop the descend
  //! Returns the flag controlling the tree descend.
  //! Returns true if the tree descend should be stopped.
  virtual Standard_Boolean Stop() const
  {
    // Do not stop tree descend by default
    return Standard_False;
  }

protected: //! @name Constructors
  //! Constructor
  BVH_BaseTraverse() {}

  //! Destructor
  virtual ~BVH_BaseTraverse() {}
};

//! Abstract class implementing the traverse of the single binary tree.
//! Selection of the data from the tree is performed by the
//! rules defined in the Accept/Reject methods.
//! See description of the required methods in the comments above.
//!
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam BVHSetType Type of set containing the BVH tree (required to access the elements by the
//! index) \tparam MetricType Type of metric to perform more optimal tree descend
template <class NumType, int Dimension, class BVHSetType = void, class MetricType = NumType>
class BVH_Traverse : public BVH_BaseTraverse<MetricType>
{
public: //! @name public types
  typedef typename BVH_Box<NumType, Dimension>::BVH_VecNt BVH_VecNt;

public: //! @name Constructor
  //! Constructor
  BVH_Traverse()
      : BVH_BaseTraverse<MetricType>(),
        myBVHSet(NULL)
  {
  }

public: //! @name Setting the set to access the elements and BVH tree
  //! Sets the BVH Set containing the BVH tree
  void SetBVHSet(BVHSetType* theBVHSet) { myBVHSet = theBVHSet; }

public: //! @name Rules for Accept/Reject
  //! Basing on the given metric, checks if the whole branch may be
  //! accepted without any further checks.
  //! Returns true if the metric is accepted, false otherwise.
  virtual Standard_Boolean AcceptMetric(const MetricType&) const
  {
    // Do not accept the whole branch by default
    return Standard_False;
  }

  //! Rejection of the node by bounding box.
  //! Metric is computed to choose the best branch.
  //! Returns true if the node should be rejected, false otherwise.
  virtual Standard_Boolean RejectNode(const BVH_VecNt& theCornerMin,
                                      const BVH_VecNt& theCornerMax,
                                      MetricType&      theMetric) const = 0;

  //! Leaf element acceptance.
  //! Metric of the parent leaf-node is passed to avoid the check on the
  //! element and accept it unconditionally.
  //! Returns true if the element has been accepted, false otherwise.
  virtual Standard_Boolean Accept(const Standard_Integer theIndex, const MetricType& theMetric) = 0;

public: //! @name Selection
  //! Selection of the elements from the BVH tree by the
  //! rules defined in Accept/Reject methods.
  //! The method requires the BVHSet containing BVH tree to be set.
  //! Returns the number of accepted elements.
  Standard_Integer Select()
  {
    if (myBVHSet)
    {
      const opencascade::handle<BVH_Tree<NumType, Dimension>>& aBVH = myBVHSet->BVH();
      return Select(aBVH);
    }
    return 0;
  }

  //! Performs selection of the elements from the BVH tree by the
  //! rules defined in Accept/Reject methods.
  //! Returns the number of accepted elements.
  Standard_Integer Select(const opencascade::handle<BVH_Tree<NumType, Dimension>>& theBVH);

protected: //! @name Fields
  BVHSetType* myBVHSet;
};

//! Abstract class implementing the parallel traverse of two binary trees.
//! Selection of the data from the trees is performed by the
//! rules defined in the Accept/Reject methods.
//! See description of the required methods in the comments above.
//!
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam BVHSetType Type of set containing the BVH tree (required to access the elements by the
//! index) \tparam MetricType Type of metric to perform more optimal tree descend
template <class NumType, int Dimension, class BVHSetType = void, class MetricType = NumType>
class BVH_PairTraverse : public BVH_BaseTraverse<MetricType>
{
public: //! @name public types
  typedef typename BVH_Box<NumType, Dimension>::BVH_VecNt BVH_VecNt;

public: //! @name Constructor
  //! Constructor
  BVH_PairTraverse()
      : BVH_BaseTraverse<MetricType>(),
        myBVHSet1(NULL),
        myBVHSet2(NULL)
  {
  }

public: //! @name Setting the sets to access the elements and BVH trees
  //! Sets the BVH Sets containing the BVH trees
  void SetBVHSets(BVHSetType* theBVHSet1, BVHSetType* theBVHSet2)
  {
    myBVHSet1 = theBVHSet1;
    myBVHSet2 = theBVHSet2;
  }

public: //! @name Rules for Accept/Reject
  //! Rejection of the pair of nodes by bounding boxes.
  //! Metric is computed to choose the best branch.
  //! Returns true if the pair of nodes should be rejected, false otherwise.
  virtual Standard_Boolean RejectNode(const BVH_VecNt& theCornerMin1,
                                      const BVH_VecNt& theCornerMax1,
                                      const BVH_VecNt& theCornerMin2,
                                      const BVH_VecNt& theCornerMax2,
                                      MetricType&      theMetric) const = 0;

  //! Leaf element acceptance.
  //! Returns true if the pair of elements is accepted, false otherwise.
  virtual Standard_Boolean Accept(const Standard_Integer theIndex1,
                                  const Standard_Integer theIndex2) = 0;

public: //! @name Selection
  //! Selection of the pairs of elements of two BVH trees by the
  //! rules defined in Accept/Reject methods.
  //! The method requires the BVHSets containing BVH trees to be set.
  //! Returns the number of accepted pairs of elements.
  Standard_Integer Select()
  {
    if (myBVHSet1 && myBVHSet2)
    {
      const opencascade::handle<BVH_Tree<NumType, Dimension>>& aBVH1 = myBVHSet1->BVH();
      const opencascade::handle<BVH_Tree<NumType, Dimension>>& aBVH2 = myBVHSet2->BVH();
      return Select(aBVH1, aBVH2);
    }
    return 0;
  }

  //! Performs selection of the elements from two BVH trees by the
  //! rules defined in Accept/Reject methods.
  //! Returns the number of accepted pairs of elements.
  Standard_Integer Select(const opencascade::handle<BVH_Tree<NumType, Dimension>>& theBVH1,
                          const opencascade::handle<BVH_Tree<NumType, Dimension>>& theBVH2);

protected: //! @name Fields
  BVHSetType* myBVHSet1;
  BVHSetType* myBVHSet2;
};

#include <BVH_Traverse.lxx>

#endif // _BVH_Traverse_Header
