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

#ifndef _BVH_PairDistance_Header
#define _BVH_PairDistance_Header

#include <BVH_Traverse.hxx>
#include <BVH_Tools.hxx>

//! Abstract class for computation of the min distance between
//! elements of two BVH trees.
//! To use this class it is required to define only the method
//! *Accept* to compute the distance between elements of the trees.
//!
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam BVHSetType Type of the set on which BVH is built
template <class NumType, int Dimension, class BVHSetType>
class BVH_PairDistance : public BVH_PairTraverse<NumType, Dimension, BVHSetType, NumType>
{
public:
  typedef typename BVH_Tools<NumType, Dimension>::BVH_VecNt BVH_VecNt;

public: //! @name Constructor

  //! Constructor
  BVH_PairDistance()
    : BVH_PairTraverse <NumType, Dimension, BVHSetType, NumType>(),
      myDistance (std::numeric_limits<NumType>::max()),
      myIsDone(Standard_False)
  {
  }

public: //! @name Compute the distance

  //! Computes the distance between two BVH trees
  NumType ComputeDistance ()
  {
    myIsDone = this->Select() > 0;
    return myDistance;
  }

public: //! @name Accessing the results

  //! Returns IsDone flag
  Standard_Boolean IsDone () const { return myIsDone; }

  //! Returns the computed distance
  NumType Distance() const { return myDistance; }

public: //! @name Definition of the rules for tree descend

  //! Compares the two metrics and chooses the best one
  virtual Standard_Boolean IsMetricBetter (const NumType& theLeft,
                                           const NumType& theRight) const Standard_OVERRIDE
  {
    return theLeft < theRight;
  }

  //! Computes the distance between boxes of the nodes
  virtual Standard_Boolean RejectNode (const BVH_VecNt& theCornerMin1,
                                       const BVH_VecNt& theCornerMax1,
                                       const BVH_VecNt& theCornerMin2,
                                       const BVH_VecNt& theCornerMax2,
                                       NumType& theMetric) const Standard_OVERRIDE
  {
    theMetric = BVH_Tools<NumType, Dimension>::BoxBoxSquareDistance (theCornerMin1, theCornerMax1,
                                                                     theCornerMin2, theCornerMax2);
    return theMetric > myDistance;
  }

  //! Rejects the branch by the metric
  virtual Standard_Boolean RejectMetric (const NumType& theMetric) const Standard_OVERRIDE
  {
    return theMetric > myDistance;
  }

  //! Returns the flag controlling the tree descend
  virtual Standard_Boolean Stop() const Standard_OVERRIDE
  {
    return myDistance == static_cast<NumType>(0);
  }

protected: //! @name Fields

  NumType myDistance;      //!< Square distance
  Standard_Boolean myIsDone; //!< State of the algorithm

};

#endif // _BVH_Distance_Header
