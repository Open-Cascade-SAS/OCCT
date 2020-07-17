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

#ifndef _BVH_Distance_Header
#define _BVH_Distance_Header

#include <BVH_Traverse.hxx>

//! Abstract class for computation of the min distance between some
//! Object and elements of BVH tree.
//! To use this class it is required to define two methods:
//! - *RejectNode* to compute distance from the object to bounding box
//! - *Accept* to compute distance from the object to the element of tree
//!
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam ObjectType Type of the object to which the distance is required
//! \tparam BVHSetType Type of the set on which BVH is built
template <class NumType, int Dimension, class ObjectType, class BVHSetType>
class BVH_Distance : public BVH_Traverse<NumType, Dimension, BVHSetType, NumType>
{
public: //! @name Constructor

  //! Constructor
  BVH_Distance()
    : BVH_Traverse <NumType, Dimension, BVHSetType, NumType>(),
      myDistance (std::numeric_limits<NumType>::max()),
      myIsDone(Standard_False)
  {
  }

public: //! @name Setting object for distance computation

  //! Sets the object to which the distance is required
  void SetObject (const ObjectType& theObject)
  {
    myObject = theObject;
  }

public: //! @name Compute the distance

  //! Computes the distance between object and BVH tree
  NumType ComputeDistance()
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

  NumType myDistance;        //!< Distance
  Standard_Boolean myIsDone; //!< State of the algorithm
  ObjectType myObject;       //!< Object to compute the distance to

};

#endif // _BVH_Distance_Header
