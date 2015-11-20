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

#include <TopoDS_Shape.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <BOPAlgo_Builder.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_DataMapOfIntegerListOfShape.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>

//!
//! The algorithm is based on the General Fuse algorithm (GFA). The result of 
//! GFA is all split parts of the Arguments.
//! 
//! The purpose of this algorithm is to provide the result with the content of:
//! 1. Cells (parts) defined by the user;
//! 2. Internal boundaries defined by the user.
//! 
//! In other words the algorithm should provide the possibility for the user
//! to add or remove any part to (from) result and remove any internal boundaries
//! between parts.
//! 
//! Requirements for the Data:
//! All the requirements of GFA for the DATA are inherited in this algorithm.
//! Plus all the arguments should have the same dimension.
//!
//! Results:
//! The result of the algorithm is compound containing selected parts of 
//! the basic type (VERTEX, EDGE, FACE or SOLID). The default result
//! is empty compound. It is possible to add any split part to the result
//! by using the methods AddToRessult() and AddAllToResult().
//! It is also possible to remove any part from the result by using methods 
//! RemoveFromResult() and RemoveAllFromResult().
//! The method RemoveAllFromResult() is also suitable for clearing the result.
//!
//! To remove Internal boundaries it is necessary to set the same material to the 
//! parts between which the boundaries should be removed and call the method 
//! RemoveInternalBoundaries(). The material should not be equal to 0, as this is 
//! default material value. The boundaries between parts with this value 
//! will not be removed.
//! One part cannot be added with the different materials.
//! It is also possible to remove the boundaries during combining the result.
//! To do this it is necessary to set the material for parts (not equal to 0)
//! and set the flag bUpdate to TRUE. 
//! BUT for the arguments of the types FACE or EDGE it is recommended
//! to remove the boundaries in the end when the result is completely built.
//! It will help to avoid self-intersections in the result.
//!
//! It is possible to create typed Containers from the parts added to result by using
//! method MakeContainers(). The type of the containers will depend on the type of 
//! the arguments: WIRES for EEDGE, SHELLS for FACES and COMPSOLIDS for SOLIDS.
//! The result will be compound containing containers.
//! Adding of the parts to such result will not update containers. The result 
//! compound will contain the containers and new added parts (of basic type).
//! Removing of the parts from such result may affect some containers if the 
//! the parts that should be removed is in container. In this case this container
//! will be rebuilt without that part.
//!
//! History:
//! The algorithm supports history information for basic types of the shapes -
//! VERTEX, EDGE, FACE. This information available through the methods 
//! IsDeleted() and Modified(). In DRAW Test Harness it is available through the same 
//! commands as for Boolean Operations (bmodified, bgenerated and bisdeleted).
//! There could be Generated shapes only after removing of the internal boundaries
//! between faces and edges, i.e. after using ShapeUpgrade_UnifySameDomain tool.
//! 
//! Examples:
//! 1. API
//! BOPAlgo_CellsBuilder aCBuilder;
//! BOPCol_ListOfShape aLS = ...; // arguments
//! /* parallel or single mode (the default value is FALSE)*/
//! Standard_Boolean bRunParallel = Standard_False; 
//! /* fuzzy option (default value is 0)*/
//! Standard_Real aTol = 0.0; 
//! //
//! aCBuilder.SetArguments(aLS);
//! aCBuilder.SetRunParallel(bRunParallel);
//! aCBuilder.SetFuzzyValue(aTol);
//! //
//! aCBuilder.Perform();
//! if (aCBuilder.ErrorStatus()) { // check error status
//!   return;
//! }
//! /* empty compound, as nothing has been added yet */
//! const TopoDS_Shape& aRes = aCBuilder.Shape(); 
//! /* all split parts */
//! const TopoDS_Shape& aRes = aCBuilder.GetAllParts();
//! //
//! BOPCol_ListOfShape aLSToTake = ...; // parts of these arguments will be taken into result
//! BOPCol_ListOfShape aLSToAvoid = ...; // parts of these arguments will not be taken into result
//! //
//! /* defines the material common for the cells, i.e. 
//!    the boundaries between cells with the same material
//!    will be removed. 
//!    By default it is set to 0. Thus, to remove some boundary 
//!    the value of this variable should not be equal to 0 */
//! Standard_Integer iMaterial = ...; 
//! /* defines whether to update the result right now or not */
//! Standard_Boolean bUpdate = ...;
//! // adding to result
//! aCBuilder.AddToResult(aLSToTake, aLSToAvoid, iMaterial, bUpdate);
//! aR = aCBuilder.Shape(); // the result
//! // removing of the boundaries
//! aCBuilder.RemoveInternalBoundaries();
//! 
//! // removing from result
//! aCBuilder.AddAllToResult();
//! aCBuilder.RemoveFromResult(aLSToTake, aLSToAvoid);
//! aR = aCBuilder.Shape(); // the result
//! 
//! 
//! 2. DRAW Test Harness
//! psphere s1 15
//! psphere s2 15
//! psphere s3 15
//! ttranslate s1 0 0 10
//! ttranslate s2 20 0 10
//! ttranslate s3 10 0 0 
//! 
//! bclearobjects; bcleartools
//! baddobjects s1 s2 s3
//! bfillds
//! # rx will contain all split parts
//! bcbuild rx 
//! # add to result the part that is common for all three spheres
//! bcadd res s1 1 s2 1 s3 1 -m 1
//! # add to result the part that is common only for first and third shperes
//! bcadd res s1 1 s2 0 s3 1 -m 1
//! # remove internal boundaries
//! bcremoveint res
//!


class BOPAlgo_CellsBuilder : public BOPAlgo_Builder
{
 public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BOPAlgo_CellsBuilder();

  Standard_EXPORT BOPAlgo_CellsBuilder(const Handle(NCollection_BaseAllocator)& theAllocator);

  Standard_EXPORT virtual ~BOPAlgo_CellsBuilder();

  //! Redefined method Clear - clears the contents.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Adding the parts to result.
  //! The parts are defined by two lists of shapes.
  //! <theLSToTake> defines the arguments which parts should be taken into result;
  //! <theLSToAvoid> defines the arguments which parts should not be taken into result;
  //! To be taken into result the part must be IN for all shapes from the list
  //! <theLSToTake> and must be OUT of all shapes from the list <theLSToAvoid>. 
  //! 
  //! To remove internal boundaries between any cells in the result 
  //! <theMaterial> variable should be used. The boundaries between 
  //! cells with the same material will be removed. Default value is 0. 
  //! Thus, to remove any boundary the value of this variable should not be equal to 0.
  //! <theUpdate> parameter defines whether to remove boundaries now or not
  Standard_EXPORT void AddToResult(const BOPCol_ListOfShape& theLSToTake,
                                   const BOPCol_ListOfShape& theLSToAvoid,
                                   const Standard_Integer theMaterial = 0,
                                   const Standard_Boolean theUpdate = Standard_False);

  //! Add all split parts to result
  //! <theMaterial> defines the removal of internal boundaries;
  //! <theUpdate> parameter defines whether to remove boundaries now or not.
  Standard_EXPORT void AddAllToResult(const Standard_Integer theMaterial = 0,
                                      const Standard_Boolean theUpdate = Standard_False);

  //! Removing the parts from result.
  //! The parts are defined by two lists of shapes.
  //! <theLSToTake> defines the arguments which parts should be removed from result;
  //! <theLSToAvoid> defines the arguments which parts should not be removed from result.
  //! To be removed from the result the part must be IN for all shapes from the list
  //! <theLSToTake> and must be OUT of all shapes from the list <theLSToAvoid>.
  Standard_EXPORT void RemoveFromResult(const BOPCol_ListOfShape& theLSToTake,
                                        const BOPCol_ListOfShape& theLSToAvoid);

  //! Remove all parts from result.
  Standard_EXPORT void RemoveAllFromResult();

  //! Removes internal boundaries between cells with the same material.
  Standard_EXPORT void RemoveInternalBoundaries();

  //! Get all split parts.
  Standard_EXPORT const TopoDS_Shape& GetAllParts() const;

  //! Makes the Containers of proper type from the parts added to result.
  Standard_EXPORT void MakeContainers();

  //! Returns the list of shapes generated from the shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated(const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns true if the shape theS has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
 protected:

  //! Redefined method Prepare - no need to prepare history 
  //! information on the default result as it is empty compound.
  Standard_EXPORT virtual void Prepare() Standard_OVERRIDE;

  //! Redefined method CheckData() - additional check for the arguments 
  //! to be of the same dimension.
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  //! Redefined method PerformInternal1 - makes all split parts, 
  //! nullifies the result <myShape>, and index all parts.
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF) Standard_OVERRIDE;

  //! Saves all split parts in myAllParts.
  Standard_EXPORT void TakeAllParts();

  //! Indexes the parts for quick access to the arguments.
  Standard_EXPORT void IndexParts();

  //! Looking for the parts defined by two lists.
  Standard_EXPORT void FindParts(const BOPCol_ListOfShape& theLSToTake,
                                 const BOPCol_ListOfShape& theLSToAvoid,
                                 BOPCol_ListOfShape& theParts);

  //! Removes internal boundaries between cells with the same material.
  Standard_EXPORT Standard_Integer RemoveInternals(const BOPCol_ListOfShape& theLS,
                                                   BOPCol_ListOfShape& theLSNew);

  // fields
  TopAbs_ShapeEnum myType;
  TopoDS_Shape myAllParts;
  BOPCol_IndexedDataMapOfShapeListOfShape myIndex;
  BOPCol_DataMapOfIntegerListOfShape myMaterials;
  BOPCol_DataMapOfShapeInteger myShapeMaterial;
  BOPCol_DataMapOfShapeShape myMapGenerated;

 private:

};

#endif //_BOPAlgo_CellsBuilder_HeaderFile
