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

#ifndef _BVH_BoxSet_Header
#define _BVH_BoxSet_Header

#include <BVH_PrimitiveSet.hxx>

//! Implements easy to use interfaces for adding the elements into
//! BVH tree and its following construction.
//! To make it more effective it is better to set the number of elements
//! that are going to be added into BVH tree.
//! For better efficiency on heavy data types it is recommended to use
//! either BHV_IndexedBoxSet which uses indirect indexing for accessing
//! the elements and their boxes or set the element to be an index
//! of the real element in the application's internal data structures.
//!
//! \tparam NumType Numeric data type
//! \tparam Dimension Vector dimension
//! \tparam DataType Type of elements on which the boxes are built
template <class NumType, int Dimension, class DataType = Standard_Integer>
class BVH_BoxSet : public BVH_PrimitiveSet <NumType, Dimension>
{
public: //! @name Constructors

  //! Empty constructor for use the default BVH_Builder
  BVH_BoxSet()
    : BVH_PrimitiveSet <NumType, Dimension>()
  {
  }
  
  //! Constructor for usage the custom BVH builder
  BVH_BoxSet (const opencascade::handle <BVH_Builder <NumType, Dimension> >& theBuilder)
    : BVH_PrimitiveSet <NumType, Dimension> (theBuilder)
  {
  }

public: //! @name Setting expected size of the BVH

  //! Sets the expected size of BVH tree
  virtual void SetSize (const Standard_Size theSize)
  {
    myElements.reserve (theSize);
    myBoxes.reserve (theSize);
  }

public: //! @name Adding elements in BVH

  //! Adds the element into BVH
  virtual void Add (const DataType& theElement, const BVH_Box<NumType, Dimension>& theBox)
  {
    myElements.push_back (theElement);
    myBoxes.push_back (theBox);
    BVH_Object<NumType, Dimension>::myIsDirty = Standard_True;
  }

public: //! @name BVH construction

  //! BVH construction
  void Build()
  {
    BVH_PrimitiveSet <NumType, Dimension>::Update();
  }

public: //! @name Clearing the elements and boxes

  //! Clears the vectors of elements and boxes
  virtual void Clear()
  {
    myElements.clear();
    myBoxes.clear();
    BVH_Object<NumType, Dimension>::myIsDirty = Standard_True;
  }

public: //! @name Necessary overrides for BVH construction

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_PrimitiveSet <NumType, Dimension>::Box;

  //! Returns the bounding box with the given index.
  virtual BVH_Box <NumType, Dimension> Box (const Standard_Integer theIndex) const Standard_OVERRIDE
  {
    return myBoxes[theIndex];
  }

  //! Returns centroid position along specified axis.
  virtual Standard_Real Center (const Standard_Integer theIndex,
                                const Standard_Integer theAxis) const Standard_OVERRIDE
  {
    return Box (theIndex).Center (theAxis);
  }

  //! Returns the number of boxes.
  virtual Standard_Integer Size() const Standard_OVERRIDE
  {
    return static_cast<Standard_Integer> (myBoxes.size());
  }

  //! Swaps indices of two specified boxes.
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2) Standard_OVERRIDE
  {
    std::swap (myElements[theIndex1], myElements[theIndex2]);
    std::swap (myBoxes   [theIndex1], myBoxes   [theIndex2]);
  }

  //! Returns the Element with the index theIndex.
  virtual DataType Element (const Standard_Integer theIndex) const
  {
    return myElements[theIndex];
  }

protected: //! @name Fields

  std::vector <DataType> myElements;                   //!< Elements
  std::vector <BVH_Box <NumType, Dimension> > myBoxes; //!< Boxes for the elements

};

#endif // _BVH_BoxSet_Header
