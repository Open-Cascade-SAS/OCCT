// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_RemoveFeatures_HeaderFile
#define _BOPAlgo_RemoveFeatures_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_BuilderShape.hxx>
#include <BRepTools_History.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//! The RemoveFeatures algorithm is intended for reconstruction of
//! the shape by removal of the unwanted parts from it. These parts can
//! be holes, protrusions, spikes, fillets etc.
//! The shape itself is not modified, the new shape is built in
//! the result.
//!
//! Currently, only the shapes of type SOLID, COMPSOLID, and
//! COMPOUND of Solids are supported. And only the FACEs can be
//! removed from the shape.
//!
//! On the input the algorithm accepts the shape itself and the
//! faces which have to be removed. It does not matter how the faces
//! are given. It could be the separate faces or the collections of faces.
//! The faces should belong to the initial shape, and those that
//! do not belong will be ignored.
//! Before reconstructing the shape, the algorithm will sort all
//! the given faces on the connected blocks (features).
//!
//! The features will be removed from the shape one by one.
//! It will allow removing all possible features even if there
//! were problems with the removal of some of them.
//!
//! The removed feature is filled by the extension of the faces adjacent
//! to the feature. In general, the algorithm of removing of the single
//! feature from the shape looks as follows:
//! - Find the faces adjacent to the feature;
//! - Extend the adjacent faces to cover the feature;
//! - Trim the extended faces by the bounds of original face
//!   (except for bounds common with the feature), so it will cover
//!   the feature only;
//! - Rebuild the solids with reconstructed adjacent faces
//!   avoiding the faces from the feature.
//!
//! If the removal is successful, the result is overwritten with the
//! new shape and the next feature is treated. Otherwise, the warning
//! will be given.
//!
//! The algorithm has the following options:
//! - History support;
//!
//! and the options available from base class:
//! - Error/Warning reporting system;
//! - Parallel processing mode.
//!
//! Please note that the other options of the base class are not supported
//! here and will have no effect.
//!
//! <b>History support</b> allows tracking modification of the input shape
//! in terms of Modified, IsDeleted and Generated. The history is
//! available through the methods of the history tool *BRepTools_History*,
//! which can be accessed here through the method *History()*.
//! By default, the history is collected, but it is possible to disable it
//! using the method *SetToFillHistory(false)*;
//!
//! <b>Error/Warning reporting system</b> - allows obtaining the extended overview
//! of the Errors/Warnings occurred during the operation. As soon as any error
//! appears the algorithm stops working. The warnings allow continuing the job,
//! informing the user that something went wrong.
//! The algorithm returns the following errors/warnings:
//! - *BOPAlgo_AlertTooFewArguments* - the error alert is given if the input
//!     shape does not contain any solids;
//! - *BOPAlgo_AlertUnsupportedType* - the warning alert is given if the input
//!     shape contains not only solids, but also other shapes;
//! - *BOPAlgo_AlertNoFacesToRemove* - the error alert is given in case
//!     there are no faces to remove from the shape (nothing to do);
//! - *BOPAlgo_AlertUnableToRemoveTheFeature* - the warning alert is given to
//!     inform the user the removal of the feature is not possible. The algorithm
//!     will still try to remove the other features;
//! - *BOPAlgo_AlertRemoveFeaturesFailed* - the error alert is given in case if
//!     the operation was aborted by the unknown reason.
//!
//! <b>Parallel processing mode</b> - allows running the algorithm in parallel mode
//! obtaining the result faster.
//!
//! The algorithm has certain limitations:
//! - Intersection of the connected faces adjacent to the feature should not be empty.
//!   It means, that such faces should not be tangent to each other.
//!   If the intersection of the adjacent faces will be empty, the algorithm will
//!   be unable to trim the faces correctly and, most likely, the feature will not be removed.
//! - The algorithm does not process the INTERNAL parts of the solids, they are simply
//!   removed during reconstruction.
//!
//! Note that for successful removal of the feature, the extended faces adjacent
//! to the feature should cover the feature completely, otherwise the solids will
//! not be rebuild.
//!
//! Here is the example of usage of the algorithm:
//! ~~~~
//! TopoDS_Shape aSolid = ...;              // Input shape to remove the features from
//! TopTools_ListOfShape aFaces = ...;      // Faces to remove from the shape
//! Standard_Boolean bRunParallel = ...;    // Parallel processing mode
//! Standard_Boolean isHistoryNeeded = ...; // History support
//!
//! BOPAlgo_RemoveFeatures aRF;             // Feature removal algorithm
//! aRF.SetShape(aSolid);                   // Set the shape
//! aRF.AddFacesToRemove(aFaces);           // Add faces to remove
//! aRF.SetRunParallel(bRunParallel);       // Define the processing mode (parallel or single)
//! aRF.SetToFillHistory(isHistoryNeeded);  // Define whether to track the shapes modifications
//! aRF.Perform();                          // Perform the operation
//! if (aRF.HasErrors())                    // Check for the errors
//! {
//!   // error treatment
//!   return;
//! }
//! if (aRF.HasWarnings())                  // Check for the warnings
//! {
//!   // warnings treatment
//! }
//! const TopoDS_Shape& aResult = aRF.Shape(); // Result shape
//! ~~~~
//!
//! The algorithm preserves the type of the input shape in the result shape. Thus,
//! if the input shape is a COMPSOLID, the resulting solids will also be put into a COMPSOLID.
//!
//! When all possible features are removed, the shape is simplified by
//! removing extra edges and vertices, created during operation, from the result shape.
//!
class BOPAlgo_RemoveFeatures : public BOPAlgo_BuilderShape
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructors
  //! Empty constructor
  BOPAlgo_RemoveFeatures()
      : BOPAlgo_BuilderShape()
  {
  }

public: //! @name Setting input data for the algorithm
  //! Sets the shape for processing.
  //! @param[in] theShape  The shape to remove the faces from.
  //!                      It should either be the SOLID, COMPSOLID or COMPOUND of Solids.
  void SetShape(const TopoDS_Shape& theShape) { myInputShape = theShape; }

  //! Returns the input shape
  const TopoDS_Shape& InputShape() const { return myInputShape; }

  //! Adds the face to remove from the input shape.
  //! @param[in] theFace  The shape to extract the faces for removal.
  void AddFaceToRemove(const TopoDS_Shape& theFace) { myFacesToRemove.Append(theFace); }

  //! Adds the faces to remove from the input shape.
  //! @param[in] theFaces  The list of shapes to extract the faces for removal.
  void AddFacesToRemove(const TopTools_ListOfShape& theFaces)
  {
    TopTools_ListIteratorOfListOfShape it(theFaces);
    for (; it.More(); it.Next())
      myFacesToRemove.Append(it.Value());
  }

  //! Returns the list of faces which have been requested for removal
  //! from the input shape.
  const TopTools_ListOfShape& FacesToRemove() const { return myFacesToRemove; }

public: //! @name Performing the operation
  //! Performs the operation
  Standard_EXPORT virtual void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

public: //! @name Clearing the contents of the algorithm
  //! Clears the contents of the algorithm from previous run,
  //! allowing reusing it for following removals.
  virtual void Clear() Standard_OVERRIDE
  {
    BOPAlgo_BuilderShape::Clear();
    myInputShape.Nullify();
    myShape.Nullify();
    myFacesToRemove.Clear();
    myFeatures.Clear();
    myInputsMap.Clear();
  }

protected: //! @name Protected methods performing the removal
  //! Checks the input data on validity for the algorithm:
  //! - The input shape must be either a SOLID, COMPSOLID or COMPOUND of Solids.
  //! If the input shape is not a solid, the method looks for the solids
  //! in <myInputShape> and uses only them. All other shapes are simply removed.
  //! If no solids were found, the Error of unsupported type is returned.
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  //! Prepares the faces to remove:
  //! - Gets only faces contained in the input solids;
  //! - Builds connected blocks of faces creating separate features to remove.
  Standard_EXPORT void PrepareFeatures(const Message_ProgressRange& theRange);

  //! Removes the features and fills the created gaps by extension of the adjacent faces.
  //! Processes each feature separately.
  Standard_EXPORT void RemoveFeatures(const Message_ProgressRange& theRange);

  //! Remove the single feature from the shape.
  //! @param[in] theFeature  The feature to remove;
  //! @param[in] theSolids  The solids to be reconstructed after feature removal;
  //! @param[in] theFeatureFacesMap  The map of feature faces;
  //! @param[in] theHasAdjacentFaces  Shows whether the adjacent faces have been
  //!                                 found for the feature or not;
  //! @param[in] theAdjFaces  The reconstructed adjacent faces covering the feature;
  //! @param[in] theAdjFacesHistory  The history of the adjacent faces reconstruction;
  //! @param[in] theSolidsHistoryNeeded  Defines whether the history of solids
  //!                                    modifications should be tracked or not.
  Standard_EXPORT void RemoveFeature(const TopoDS_Shape&               theFeature,
                                     const TopTools_IndexedMapOfShape& theSolids,
                                     const TopTools_MapOfShape&        theFeatureFacesMap,
                                     const Standard_Boolean            theHasAdjacentFaces,
                                     const TopTools_IndexedDataMapOfShapeListOfShape& theAdjFaces,
                                     const Handle(BRepTools_History)& theAdjFacesHistory,
                                     const Standard_Boolean           theSolidsHistoryNeeded,
                                     const Message_ProgressRange&     theRange);

  //! Updates history with the removed features
  Standard_EXPORT void UpdateHistory(const Message_ProgressRange& theRange);

  //! Simplifies the result by removing extra edges and vertices created
  //! during removal of the features.
  Standard_EXPORT void SimplifyResult(const Message_ProgressRange& theRange);

  //! Post treatment - restore the type of the initial shape
  Standard_EXPORT void PostTreat();

  //! Filling steps for constant operations
  Standard_EXPORT void fillPIConstants(const Standard_Real theWhole,
                                       BOPAlgo_PISteps&    theSteps) const Standard_OVERRIDE;

protected: //! @name Fields
  // Inputs
  TopoDS_Shape         myInputShape;    //!< Input shape
  TopTools_ListOfShape myFacesToRemove; //!< Faces to remove

  // Intermediate
  TopTools_ListOfShape myFeatures;        //!< List of not connected features to remove
                                          //! (each feature is a compound of faces)
  TopTools_IndexedMapOfShape myInputsMap; //!< Map of all sub-shapes of the input shape
};

#endif // _BOPAlgo_RemoveFeatures_HeaderFile
