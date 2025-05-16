// Created on: 1992-11-24
// Created by: Didier PIFFAULT
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Bnd_BoundSortBox_HeaderFile
#define _Bnd_BoundSortBox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_ListOfInteger.hxx>

#include <array>
#include <vector>

class gp_Pln;
class Bnd_VoxelGrid;

//! A tool to compare a bounding box or a plane with a set of
//! bounding boxes. It sorts the set of bounding boxes to give
//! the list of boxes which intersect the element being compared.
//! The boxes being sorted generally bound a set of shapes,
//! while the box being compared bounds a shape to be
//! compared. The resulting list of intersecting boxes therefore
//! gives the list of items which potentially intersect the shape to be compared.
//! How to use this class:
//! - Create an instance of this class.
//! - Initialize it with the set of boxes to be sorted using one of the
//!   Initialize() methods.
//! - Call the Compare() method with the box or plane to be compared.
//!   Compare() will return the list of indices of the boxes which intersect
//!   the box or plane passed as argument.
class Bnd_BoundSortBox
{
public:
  DEFINE_STANDARD_ALLOC

private:
  using VectorInt = std::vector<Standard_Integer, NCollection_Allocator<Standard_Integer>>;

public:
  //! Constructs an empty comparison algorithm for bounding boxes.
  //! The bounding boxes are then defined using the Initialize function.
  Standard_EXPORT Bnd_BoundSortBox();

  //! Initializes this comparison algorithm with the set of boxes.
  //! @param theSetOfBoxes The set of bounding boxes to be used by this algorithm.
  Standard_EXPORT void Initialize(const Handle(Bnd_HArray1OfBox)& theSetOfBoxes);

  //! Initializes this comparison algorithm with the set of boxes and the bounding box
  //! that encloses all those boxes. This version of initialization can be used if complete
  //! box is known in advance to avoid calculating it again inside the algorithm.
  //! @param theEnclosingBox The bounding box that contains all the boxes in @p theSetOfBoxes.
  //! @param theSetOfBoxes The set of bounding boxes to be used by this algorithm.
  Standard_EXPORT void Initialize(const Bnd_Box&                  theEnclosingBox,
                                  const Handle(Bnd_HArray1OfBox)& theSetOfBoxes);

  //! Initializes this comparison algorithm with the bounding box that encloses all the boxes
  //! that will be used by this algorithm. and the expected number of those boxes.
  //! Boxes to be considered can then be added using the Add() method.
  //! @param theEnclosingBox The bounding box that contains all the boxes to be sorted.
  //! @param theNbComponents The number of components to be added.
  Standard_EXPORT void Initialize(const Bnd_Box&         theEnclosingBox,
                                  const Standard_Integer theNbBoxes);

  //! Adds the bounding box theBox at position boxIndex in the internal array of boxes
  //! to be sorted by this comparison algorithm. This function is used only in
  //! conjunction with the Initialize(const Bnd_Box&, const Standard_Integer) method.
  //! Exceptions:
  //! - Standard_OutOfRange if boxIndex is not in the range [ 1,nbComponents ] where
  //!   nbComponents is the maximum number of bounding boxes declared for this algorithm at
  //!   initialization.
  //! - Standard_MultiplyDefined if a box already exists at position @p theIndex in the
  //!   internal array of boxes.
  //! @param theBox The bounding box to be added.
  //! @param theIndex The index of the bounding box in the internal array where the box
  //!        will be added. The index is 1-based.
  Standard_EXPORT void Add(const Bnd_Box& theBox, const Standard_Integer theIndex);

  //! Compares the bounding box theBox, with the set of bounding boxes provided to this
  //! algorithm at initialization, and returns the list of indices of bounding boxes
  //! that intersect the @p theBox or are inside it.
  //! The indices correspond to the indices of the bounding boxes in the array provided
  //! to this algorithm at initialization.
  //! @param theBox The bounding box to be compared.
  //! @return The list of indices of bounding boxes that intersect the bounding box theBox
  //!         or are inside it.
  Standard_EXPORT const TColStd_ListOfInteger& Compare(const Bnd_Box& theBox);

  //! Compares the plane @p thePlane with the set of bounding boxes provided to this
  //! algorithm at initialization, and returns the list of indices of bounding boxes
  //! that intersect the @p thePlane.
  //! The indices correspond to the indices of the bounding boxes in the array provided
  //! to this algorithm at initialization.
  //! @param thePlane The plane to be compared.
  //! @return The list of indices of bounding boxes that intersect the plane thePlane.
  Standard_EXPORT const TColStd_ListOfInteger& Compare(const gp_Pln& thePlane);

private:
  //! Precalculates the coefficients for the voxel grid based on the enclosing box dimensions.
  //! The coefficients will be used to map the box coordinates to the voxel grid.
  void calculateCoefficients();

  //! Resets the voxel grid and clears the list of large boxes.
  void resetVoxelGrid();

  //! Performs the sorting of the boxes in the voxel grid.
  //! This method is called after the boxes have been added to the voxel grid.
  void sortBoxes();

  //! Returns indices of voxels that contain minimum and maximum points of the box.
  //! @param theBox The bounding box to be compared.
  //! @return The indices of the voxels that contain the minimum and maximum points of the box
  //!         in the order: [minX, minY, minZ, maxX, maxY, maxZ].
  std::array<Standard_Integer, 6> getBoundingVoxels(const Bnd_Box& theBox) const;

  //! Adds the box stored in myBoxes to the voxel map.
  //! @param theBox The bounding box to be added.
  //! @param theIndex The index of the bounding box in myBoxes.
  void addBox(const Bnd_Box& theBox, const Standard_Integer theIndex);

  Bnd_Box myEnclosingBox;            //!< The bounding box that contains all the boxes to be sorted.
  Handle(Bnd_HArray1OfBox) myBoxes;  //!< The set of bounding boxes to be sorted.
  Standard_Real            myCoeffX; //!< Coefficient for X direction.
  Standard_Real            myCoeffY; //!< Coefficient for Y direction.
  Standard_Real            myCoeffZ; //!< Coefficient for Z direction.
  Standard_Integer         myResolution; //!< The number of voxels in each direction.
  TColStd_ListOfInteger    myLastResult; //!< The last result of the Compare() method.
  VectorInt                myLargeBoxes; //!< The list of large boxes.
  Handle(Bnd_VoxelGrid)    myVoxelGrid;  //!< The voxel grid used for sorting the boxes.
};

#endif // _Bnd_BoundSortBox_HeaderFile
