// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPAlgo_Builder_HeaderFile
#define _BOPAlgo_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_PPaveFiller.hxx>
#include <BOPAlgo_BuilderShape.hxx>
#include <BOPAlgo_GlueEnum.hxx>
#include <BOPDS_PDS.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class IntTools_Context;
class TopoDS_Shape;
class BOPAlgo_PaveFiller;

//!
//! The class is a General Fuse algorithm - base algorithm for the
//! algorithms in the Boolean Component. Its main purpose is to build
//! the split parts of the argument shapes from which the result of
//! the operations is combined.<br>
//! The result of the General Fuse algorithm itself is a compound
//! containing all split parts of the arguments. <br>
//!
//! Additionally to the options of the base classes, the algorithm has
//! the following options:<br>
//! - *Safe processing mode* - allows to avoid modification of the input
//!                            shapes during the operation (by default it is off);<br>
//! - *Gluing options* - allows to speed up the calculation of the intersections
//!                      on the special cases, in which some sub-shapes are coinciding.<br>
//!
//! The algorithm returns the following Error statuses:
//! - *BOPAlgo_AlertTooFewArguments* - in case there are no enough arguments to perform the operation;
//! - *BOPAlgo_AlertNoFiller* - in case the intersection tool has not been created;
//! - *BOPAlgo_AlertIntersectionFailed* - in case the intersection of the arguments has failed;
//! - *BOPAlgo_AlertBuilderFailed* - in case building splits of arguments has failed with some unexpected error.
//!
class BOPAlgo_Builder  : public BOPAlgo_BuilderShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_Builder();
Standard_EXPORT virtual ~BOPAlgo_Builder();
  
  Standard_EXPORT BOPAlgo_Builder(const Handle(NCollection_BaseAllocator)& theAllocator);
  
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  Standard_EXPORT BOPAlgo_PPaveFiller PPaveFiller();
  
  Standard_EXPORT BOPDS_PDS PDS();
  
  Standard_EXPORT virtual void AddArgument (const TopoDS_Shape& theShape);
  
  Standard_EXPORT virtual void SetArguments (const TopTools_ListOfShape& theLS);
  
  Standard_EXPORT const TopTools_ListOfShape& Arguments() const;

  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PerformWithFiller (const BOPAlgo_PaveFiller& theFiller);
  
  //! Returns the  list of shapes generated from the
  //! shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns the list of shapes modified from the shape
  //! theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns true if the shape theS has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Images() const;
  
  Standard_EXPORT Standard_Boolean IsInterferred (const TopoDS_Shape& theS) const;
  
  //! Returns myOrigins.
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Origins() const;
  
  //! Returns myShapesSD.
  Standard_EXPORT const TopTools_DataMapOfShapeShape& ShapesSD() const;
  
  //! Returns mySplits.
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Splits() const;
  
  
  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  //! This flag is taken into account if internal PaveFiller is used only.
  //! In the case of calling PerformWithFiller the corresponding flag of that PaveFiller
  //! is in force.
  Standard_EXPORT void SetNonDestructive(const Standard_Boolean theFlag);

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT Standard_Boolean NonDestructive() const;

  //! Sets the glue option for the algorithm
  Standard_EXPORT void SetGlue(const BOPAlgo_GlueEnum theGlue);
  
  //! Returns the glue option of the algorithm
  Standard_EXPORT BOPAlgo_GlueEnum Glue() const;

protected:

  //! Prepare information for history support
  Standard_EXPORT virtual void PrepareHistory() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PerformInternal (const BOPAlgo_PaveFiller& thePF);
  
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF);
  
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  //! Checks if the intersection algorithm has Errors/Warnings
  Standard_EXPORT void CheckFiller();
  
  Standard_EXPORT virtual void Prepare();
  
  Standard_EXPORT void FillImagesVertices();
  
  Standard_EXPORT void FillImagesEdges();
  
  Standard_EXPORT virtual void BuildResult (const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void FillImagesContainers (const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void FillImagesCompounds();
  
  Standard_EXPORT void FillImagesContainer (const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType);
  
  Standard_EXPORT void FillImagesCompound (const TopoDS_Shape& theS, TopTools_MapOfShape& theMF);
  
  Standard_EXPORT void FillImagesFaces();
  
  Standard_EXPORT virtual void BuildSplitFaces();
  
  Standard_EXPORT void FillSameDomainFaces();
  
  Standard_EXPORT void FillImagesFaces1();
  
  Standard_EXPORT void FillImagesSolids();
  
  Standard_EXPORT void BuildDraftSolid (const TopoDS_Shape& theSolid, TopoDS_Shape& theDraftSolid, TopTools_ListOfShape& theLIF);
  
  Standard_EXPORT virtual void FillIn3DParts (TopTools_DataMapOfShapeListOfShape& theInParts, TopTools_DataMapOfShapeShape& theDraftSolids, const Handle(NCollection_BaseAllocator)& theAllocator);
  
  Standard_EXPORT void BuildSplitSolids (TopTools_DataMapOfShapeListOfShape& theInParts, TopTools_DataMapOfShapeShape& theDraftSolids, const Handle(NCollection_BaseAllocator)& theAllocator);
  
  Standard_EXPORT void FillInternalShapes();
  
  Standard_EXPORT virtual void PostTreat();


  TopTools_ListOfShape myArguments;
  TopTools_MapOfShape myMapFence;
  BOPAlgo_PPaveFiller myPaveFiller;
  BOPDS_PDS myDS;
  Handle(IntTools_Context) myContext;
  Standard_Integer myEntryPoint;
  TopTools_DataMapOfShapeListOfShape myImages;
  TopTools_DataMapOfShapeShape myShapesSD;
  TopTools_DataMapOfShapeListOfShape mySplits;
  TopTools_DataMapOfShapeListOfShape myOrigins;
  Standard_Boolean myNonDestructive;
  BOPAlgo_GlueEnum myGlue;

private:

};


#endif // _BOPAlgo_Builder_HeaderFile
