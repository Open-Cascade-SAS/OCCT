// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_MakerVolume_HeaderFile
#define _BOPAlgo_MakerVolume_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Boolean.hxx>

#include <BOPAlgo_Builder.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
class BOPAlgo_PaveFiller;



//! The algorithm is to build solids from set of shapes.
//! It uses the BOPAlgo_Builder algorithm to intersect the given shapes
//! and build the images of faces (if needed) and BOPAlgo_BuilderSolid
//! algorithm to build the solids.
//!
//! Steps of the algorithm:
//! 1. Collect all faces: intersect the shapes if necessary and collect
//! the images of faces, otherwise just collect the faces to the
//! <myFaces> list;
//! All faces on this step added twice, with orientation FORWARD
//! and REVERSED;
//!
//! 2. Create bounding box covering all the faces from <myFaces> and
//! create solid box from corner points of that bounding box
//! (myBBox, mySBox). Add faces from that box to <myFaces>;
//!
//! 3. Build solids from <myFaces> using BOPAlgo_BuilderSolid algorithm;
//!
//! 4. Treat the result: Eliminate solid containing faces from <mySBox>;
//!
//! 5. Fill internal shapes: add internal vertices and edges into
//! created solids;
//!
//! 6. Prepare the history.
//!
//! Fields:
//! <myIntersect> - boolean flag. It defines whether intersect shapes
//! from <myArguments> (if set to TRUE) or not (FALSE).
//! The default value is TRUE. By setting it to FALSE
//! the user should guarantee that shapes in <myArguments>
//! do not interfere with each other, otherwise the result
//! is unpredictable.
//!
//! <myBBox>      - bounding box, covering all faces from <myFaces>.
//!
//! <mySBox>      - Solid box created from the corner points of <myBBox>.
//!
//! <myFaces>     - the list is to keep the "final" faces, that will be
//! given to the BOPAlgo_BuilderSolid algorithm.
//! If the shapes have been interfered it should contain
//! the images of the source shapes, otherwise its just
//! the original faces.
//! It also contains the faces from <mySBox>.
//!
//! Fields inherited from BOPAlgo_Builder:
//!
//! <myArguments> - list of the source shapes. The source shapes can have
//! any type, but each shape must not be self-interfered.
//!
//! <myShape>     - Result shape:
//! - empty compound - if no solids were created;
//! - solid - if created only one solid;
//! - compound of solids - if created more than one solid.
//!
//! Fields inherited from BOPAlgo_Algo:
//!
//! <myRunParallel> - Defines whether the parallel processing is
//! switched on or not.
//! <myReport> - Error status of the operation. Additionally to the
//! errors of the parent algorithm it can have the following values:
//! - *BOPAlgo_AlertSolidBuilderFailed* - BOPAlgo_BuilderSolid algorithm has failed.
//!
//! Example:
//!
//! BOPAlgo_MakerVolume aMV;
//! //
//! aMV.SetArguments(aLS); //source shapes
//! aMV.SetRunParallel(bRunParallel); //parallel or single mode
//! aMV.SetIntersect(bIntersect); //intersect or not the shapes from <aLS>
//! //
//! aMV.Perform(); //perform the operation
//! if (aMV.HasErrors()) { //check error status
//!   return;
//! }
//! //
//! const TopoDS_Shape& aResult = aMV.Shape();  //result of the operation
class BOPAlgo_MakerVolume  : public BOPAlgo_Builder
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  BOPAlgo_MakerVolume();
  virtual ~BOPAlgo_MakerVolume();

  //! Empty constructor.
  BOPAlgo_MakerVolume(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Clears the data.
  virtual void Clear() Standard_OVERRIDE;

  //! Sets the flag myIntersect:
  //! if <bIntersect> is TRUE the shapes from <myArguments> will be intersected.
  //! if <bIntersect> is FALSE no intersection will be done.
  void SetIntersect(const Standard_Boolean bIntersect);

  //! Returns the flag <myIntersect>.
  Standard_Boolean IsIntersect() const;

  //! Returns the solid box <mySBox>.
  const TopoDS_Solid& Box() const;

  //! Returns the processed faces <myFaces>.
  const TopTools_ListOfShape& Faces() const;

  //! Defines the preventing of addition of internal for solid parts into the result.
  //! By default the internal parts are added into result.
  void SetAvoidInternalShapes(const Standard_Boolean theAvoidInternal) {
    myAvoidInternalShapes = theAvoidInternal;
  }

  //! Returns the AvoidInternalShapes flag
  Standard_Boolean IsAvoidInternalShapes() const {
    return myAvoidInternalShapes;
  }

  //! Performs the operation.
  Standard_EXPORT virtual void Perform(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

protected:

  //! Checks the data.
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  //! Performs the operation.
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF, const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Collects all faces.
  Standard_EXPORT void CollectFaces();

  //! Makes solid box.
  Standard_EXPORT void MakeBox (TopTools_MapOfShape& theBoxFaces);

  //! Builds solids.
  Standard_EXPORT void BuildSolids (TopTools_ListOfShape& theLSR,
                                    const Message_ProgressRange& theRange);

  //! Removes the covering box.
  Standard_EXPORT void RemoveBox (TopTools_ListOfShape& theLSR, const TopTools_MapOfShape& theBoxFaces);

  //! Fills the solids with internal shapes.
  Standard_EXPORT void FillInternalShapes (const TopTools_ListOfShape& theLSR);

  //! Builds the result.
  Standard_EXPORT void BuildShape (const TopTools_ListOfShape& theLSR);

protected:
  //! List of operations to be supported by the Progress Indicator.
  //! Enumeration is going to contain some extra operations from base class,
  //! which are not going to be used here. So, the array of steps will also
  //! contain some extra zero values. This is the only extra resource that is
  //! going to be used, but it allows us not to override the methods that use
  //! the values of the enumeration of base class.
  //! Starting the enumeration from the middle of enumeration of base class is
  //! not a good idea as the values in enumeration may be swapped.
  enum BOPAlgo_PIOperation
  {
    PIOperation_BuildSolids = BOPAlgo_Builder::PIOperation_Last,
    PIOperation_Last
  };

  //! Analyze progress steps
  Standard_EXPORT void fillPISteps(BOPAlgo_PISteps& theSteps) const Standard_OVERRIDE;

protected:

  Standard_Boolean myIntersect;
  Bnd_Box myBBox;
  TopoDS_Solid mySBox;
  TopTools_ListOfShape myFaces;
  Standard_Boolean myAvoidInternalShapes;

private:

};

#include <BOPAlgo_MakerVolume.lxx>

#endif // _BOPAlgo_MakerVolume_HeaderFile
