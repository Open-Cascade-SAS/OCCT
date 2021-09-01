// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _BOPAlgo_CellsBuilder_HeaderFile
#define _BOPAlgo_CellsBuilder_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_OStream.hxx>

#include <TopoDS_Shape.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BOPAlgo_Builder.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

//! The algorithm is based on the General Fuse algorithm (GFA).
//! The result of GFA is all split parts of the Arguments.
//!
//! The purpose of this algorithm is to provide the result with the content of:
//! 1. Cells (parts) defined by the user;
//! 2. Internal boundaries defined by the user.
//!
//! In other words the algorithm should provide the possibility for the user to add
//! or remove any part to (from) result and remove any internal boundaries between parts.
//!
//! All the requirements of GFA for the DATA are inherited in this algorithm.
//! The arguments could be of any type (dimension) and should be valid
//! in terms of BRepCheck_Analyzer and BOPAlgo_ArgumentAnalyzer.
//!
//! Results:
//!
//! The result of the algorithm is compound containing selected parts of the basic types (VERTEX, EDGE, FACE or SOLID).
//! The default result is empty compound.
//! It is possible to add any split part to the result by using the methods AddToRessult() and AddAllToResult().
//! It is also possible to remove any part from the result by using methods RemoveFromResult() and RemoveAllFromResult().
//! The method RemoveAllFromResult() is also suitable for clearing the result.
//!
//! To remove Internal boundaries it is necessary to set the same material to the
//! parts between which the boundaries should be removed and call the method RemoveInternalBoundaries().
//! The material should not be equal to 0, as this is default material value.
//! The boundaries between parts with this value will not be removed.
//! One part cannot be added with the different materials.
//! It is also possible to remove the boundaries during combining the result.
//! To do this it is necessary to set the material for parts (not equal to 0) and set the flag bUpdate to TRUE.
//! For the arguments of the types FACE or EDGE it is recommended
//! to remove the boundaries in the end when the result is completely built.
//! It will help to avoid self-intersections in the result.
//!
//! Note, that if the result contains the parts with same material but of different
//! dimension the boundaries between such parts will not be removed.
//! Currently, the removal of the internal boundaries between multi-dimensional shapes is not supported.
//!
//! It is possible to create typed Containers from the parts added to result by using method MakeContainers().
//! The type of the containers will depend on the type of the arguments:
//! WIRES for EEDGE, SHELLS for FACES and COMPSOLIDS for SOLIDS.
//! The result will be compound containing containers.
//! Adding of the parts to such result will not update containers.
//! The result compound will contain the containers and new added parts (of basic type).
//! Removing of the parts from such result may affect some containers if
//! the parts that should be removed is in container.
//! In this case this container will be rebuilt without that part.
//!
//! History:
//!
//! The algorithm supports history information for basic types of the shapes - VERTEX, EDGE, FACE.
//! This information available through the methods IsDeleted() and Modified().
//!
//! In DRAW Test Harness it is available through the same commands
//! as for Boolean Operations (bmodified, bgenerated and bisdeleted).
//!
//! The algorithm can return the following Error Statuses:
//! - Error status acquired in the General Fuse algorithm.
//! The Error status can be checked with HasErrors() method.
//! If the Error status is not equal to zero, the result cannot be trustworthy.
//!
//! The algorithm can set the following Warning Statuses:
//! - Warning status acquired in the General Fuse algorithm;
//! - BOPAlgo_AlertRemovalOfIBForMDimShapes
//! - BOPAlgo_AlertRemovalOfIBForFacesFailed
//! - BOPAlgo_AlertRemovalOfIBForEdgesFailed
//! - BOPAlgo_AlertRemovalOfIBForSolidsFailed
//!
//! The Warning status can be checked with HasWarnings() method or printed with the DumpWarnings() method.
//! If warnings are recorded, the result may be not as expected.
//!
//! Examples:
//!
//! 1. API
//! @code
//! BOPAlgo_CellsBuilder aCBuilder;
//! TopTools_ListOfShape aLS = ...; // arguments
//! // parallel or single mode (the default value is FALSE)
//! bool toRunParallel = false;
//! // fuzzy option (default value is 0)
//! Standard_Real aTol = 0.0;
//! //
//! aCBuilder.SetArguments (aLS);
//! aCBuilder.SetRunParallel (toRunParallel);
//! aCBuilder.SetFuzzyValue (aTol);
//! //
//! aCBuilder.Perform();
//! if (aCBuilder.HasErrors()) // check error status
//! {
//!   return;
//! }
//! // empty compound, as nothing has been added yet
//! const TopoDS_Shape& aRes = aCBuilder.Shape();
//! // all split parts
//! const TopoDS_Shape& aRes = aCBuilder.GetAllParts();
//! //
//! TopTools_ListOfShape aLSToTake  = ...; // parts of these arguments will be taken into result
//! TopTools_ListOfShape aLSToAvoid = ...; // parts of these arguments will not be taken into result
//! //
//! // defines the material common for the cells,
//! // i.e. the boundaries between cells with the same material will be removed.
//! // By default it is set to 0.
//! // Thus, to remove some boundary the value of this variable should not be equal to 0.
//! Standard_Integer iMaterial = ...;
//! // defines whether to update the result right now or not
//! bool toUpdate = ...;
//! // adding to result
//! aCBuilder.AddToResult (aLSToTake, aLSToAvoid, iMaterial, toUpdate);
//! aR = aCBuilder.Shape(); // the result
//! // removing of the boundaries (should be called only if toUpdate is false)
//! aCBuilder.RemoveInternalBoundaries();
//! //
//! // removing from result
//! aCBuilder.AddAllToResult();
//! aCBuilder.RemoveFromResult (aLSToTake, aLSToAvoid);
//! aR = aCBuilder.Shape(); // the result
//! @endcode
//!
//! 2. DRAW Test Harness
//! @code
//! psphere s1 15
//! psphere s2 15
//! psphere s3 15
//! ttranslate s1 0 0 10
//! ttranslate s2 20 0 10
//! ttranslate s3 10 0 0
//! # adding arguments
//! bclearobjects; bcleartools
//! baddobjects s1 s2 s3
//! # intersection
//! bfillds
//! # rx will contain all split parts
//! bcbuild rx
//! # add to result the part that is common for all three spheres
//! bcadd res s1 1 s2 1 s3 1 -m 1
//! # add to result the part that is common only for first and third spheres
//! bcadd res s1 1 s2 0 s3 1 -m 1
//! # remove internal boundaries
//! bcremoveint res
//! @endcode
class BOPAlgo_CellsBuilder : public BOPAlgo_Builder
{
 public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BOPAlgo_CellsBuilder();

  Standard_EXPORT BOPAlgo_CellsBuilder(const Handle(NCollection_BaseAllocator)& theAllocator);

  Standard_EXPORT virtual ~BOPAlgo_CellsBuilder();

  //! Redefined method Clear - clears the contents.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Adding the parts to result.<br>
  //! The parts are defined by two lists of shapes:<br>
  //! <theLSToTake> defines the arguments which parts should be taken into result;<br>
  //! <theLSToAvoid> defines the arguments which parts should not be taken into result;<br>
  //! To be taken into result the part must be IN for all shapes from the list
  //! <theLSToTake> and must be OUT of all shapes from the list <theLSToAvoid>.<br>
  //! 
  //! To remove internal boundaries between any cells in the result
  //! <theMaterial> variable should be used. The boundaries between
  //! cells with the same material will be removed. Default value is 0.<br>
  //! Thus, to remove any boundary the value of this variable should not be equal to 0.<br>
  //! <theUpdate> parameter defines whether to remove boundaries now or not.
  Standard_EXPORT void AddToResult(const TopTools_ListOfShape& theLSToTake,
                                   const TopTools_ListOfShape& theLSToAvoid,
                                   const Standard_Integer theMaterial = 0,
                                   const Standard_Boolean theUpdate = Standard_False);

  //! Add all split parts to result.<br>
  //! <theMaterial> defines the removal of internal boundaries;<br>
  //! <theUpdate> parameter defines whether to remove boundaries now or not.
  Standard_EXPORT void AddAllToResult(const Standard_Integer theMaterial = 0,
                                      const Standard_Boolean theUpdate = Standard_False);

  //! Removing the parts from result.<br>
  //! The parts are defined by two lists of shapes:<br>
  //! <theLSToTake> defines the arguments which parts should be removed from result;<br>
  //! <theLSToAvoid> defines the arguments which parts should not be removed from result.<br>
  //! To be removed from the result the part must be IN for all shapes from the list
  //! <theLSToTake> and must be OUT of all shapes from the list <theLSToAvoid>.
  Standard_EXPORT void RemoveFromResult(const TopTools_ListOfShape& theLSToTake,
                                        const TopTools_ListOfShape& theLSToAvoid);

  //! Remove all parts from result.
  Standard_EXPORT void RemoveAllFromResult();

  //! Removes internal boundaries between cells with the same material.<br>
  //! If the result contains the cells with same material but of different dimension
  //! the removal of internal boundaries between these cells will not be performed.<br>
  //! In case of some errors during the removal the method will set the appropriate warning 
  //! status - use GetReport() to access them.
  Standard_EXPORT void RemoveInternalBoundaries();

  //! Get all split parts.
  Standard_EXPORT const TopoDS_Shape& GetAllParts() const;

  //! Makes the Containers of proper type from the parts added to result.
  Standard_EXPORT void MakeContainers();

 protected:

  //! Prepare information for history support taking into account
  //! local modification map of unified elements - myMapModified.
  Standard_EXPORT virtual const TopTools_ListOfShape* LocModified(const TopoDS_Shape& theS) Standard_OVERRIDE;

  //! Redefined method PerformInternal1 - makes all split parts,
  //! nullifies the result <myShape>, and index all parts.
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF, const Message_ProgressRange& theRange) Standard_OVERRIDE;

  //! Indexes the parts for quick access to the arguments.
  Standard_EXPORT void IndexParts();

  //! Looking for the parts defined by two lists.
  Standard_EXPORT void FindParts(const TopTools_ListOfShape& theLSToTake,
                                 const TopTools_ListOfShape& theLSToAvoid,
                                 TopTools_ListOfShape& theParts);

  //! Removes internal boundaries between cells with the same material.<br>
  //! Returns TRUE if any internal boundaries have been removed.
  Standard_EXPORT Standard_Boolean RemoveInternals(const TopTools_ListOfShape& theLS,
                                                   TopTools_ListOfShape& theLSNew,
                                                   const TopTools_MapOfShape& theMapKeepBnd = TopTools_MapOfShape());

  // fields
  TopoDS_Shape myAllParts;                           //!< All split parts of the arguments
  TopTools_IndexedDataMapOfShapeListOfShape myIndex; //!< Connection map from all splits parts to the argument shapes from which they were created
  TopTools_DataMapOfIntegerListOfShape myMaterials;  //!< Map of assigned materials (material -> list of shape)
  TopTools_DataMapOfShapeInteger myShapeMaterial;    //!< Map of assigned materials (shape -> material)
  TopTools_DataMapOfShapeShape myMapModified;        //!< Local modification map to track unification of the splits
};

#endif //_BOPAlgo_CellsBuilder_HeaderFile
