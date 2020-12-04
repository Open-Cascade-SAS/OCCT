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

#ifndef _BVH_IndexedBoxSet_Header
#define _BVH_IndexedBoxSet_Header

#include <BVH_BoxSet.hxx>

//! Implements easy to use interfaces for adding the elements into
//! BVH tree and its following construction.
//! To make it more effective it is better to set the number of elements
//! that are going to be added into BVH tree.
//! It uses the indirect indexing for accessing the elements and their boxes
//! which allows using heavy data types as elements with better efficiency
//! during BVH construction and just a bit slower selection time.
//! Due to better BVH tree construction time the class will be more efficient
//! than BVH_BoxSet on the operations where just a few selections from
//! the tree required.
//!
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam DataType Type of elements on which the boxes are built
template <class NumType, int Dimension, class DataType = Standard_Integer>
class BVH_IndexedBoxSet : public BVH_BoxSet <NumType, Dimension, DataType>
{
public: //! @name Constructors

  //! Empty constructor for use the default BVH_Builder
  BVH_IndexedBoxSet()
    : BVH_BoxSet <NumType, Dimension, DataType>()
  {
  }
  
  //! Constructor for usage the custom BVH builder
  BVH_IndexedBoxSet (const opencascade::handle <BVH_Builder <NumType, Dimension> >& theBuilder)
    : BVH_BoxSet <NumType, Dimension, DataType> (theBuilder)
  {
  }

public: //! @name Setting expected size of the BVH

  //! Sets the expected size of BVH tree
  virtual void SetSize (const Standard_Size theSize) Standard_OVERRIDE
  {
    myIndices.reserve (theSize);
    BVH_BoxSet <NumType, Dimension, DataType>::SetSize (theSize);
  }

public: //! @name Adding elements in BVH

  //! Adds the element into BVH
  virtual void Add (const DataType& theElement, const BVH_Box<NumType, Dimension>& theBox) Standard_OVERRIDE
  {
    myIndices.push_back (static_cast<Standard_Integer> (myIndices.size()));
    BVH_BoxSet <NumType, Dimension, DataType>::Add (theElement, theBox);
  }

public: //! @name Clearing the elements and boxes

  //! Clears the vectors of elements and boxes
  virtual void Clear() Standard_OVERRIDE
  {
    myIndices.clear();
    BVH_BoxSet <NumType, Dimension, DataType>::Clear();
  }

public: //! @name Necessary overrides for BVH construction

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_BoxSet <NumType, Dimension, DataType>::Box;

  //! Returns the bounding box with the given index.
  virtual BVH_Box <NumType, Dimension> Box (const Standard_Integer theIndex) const Standard_OVERRIDE
  {
    return this->myBoxes[myIndices[theIndex]];
  }

  //! Swaps indices of two specified boxes.
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    std::swap (myIndices[theIndex1], myIndices[theIndex2]);
  }

  //! Returns the Element with the index theIndex.
  virtual DataType Element (const Standard_Integer theIndex) const Standard_OVERRIDE
  {
    return this->myElements[myIndices[theIndex]];
  }

protected: //! @name Fields

  std::vector <Standard_Integer> myIndices;

};

#endif // _BVH_IndexedBoxSet_Header
