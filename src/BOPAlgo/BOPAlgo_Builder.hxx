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
//! - *Disabling the check for inverted solids* - Disables/Enables the check of the input solids
//!                          for inverted status (holes in the space). The default value is TRUE,
//!                          i.e. the check is performed. Setting this flag to FALSE for inverted solids,
//!                          most likely will lead to incorrect results.
//!
//! The algorithm returns the following warnings:
//! - *BOPAlgo_AlertUnableToOrientTheShape* - in case the check on the orientation of the split shape
//!                                           to match the orientation of the original shape has failed.
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

  //! Empty constructor.
  Standard_EXPORT BOPAlgo_Builder();
  Standard_EXPORT virtual ~BOPAlgo_Builder();

  Standard_EXPORT BOPAlgo_Builder(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Clears the content of the algorithm.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Returns the PaveFiller, algorithm for sub-shapes intersection.
  BOPAlgo_PPaveFiller PPaveFiller()
  {
    return myPaveFiller;
  }

  //! Returns the Data Structure, holder of intersection information.
  BOPDS_PDS PDS()
  {
    return myDS;
  }

  //! Returns the Context, tool for cashing heavy algorithms.
  Handle(IntTools_Context) Context() const
  {
    return myContext;
  }


public: //! @name Arguments

  //! Adds the argument to the operation.
  Standard_EXPORT virtual void AddArgument (const TopoDS_Shape& theShape);

  //! Sets the list of arguments for the operation.
  Standard_EXPORT virtual void SetArguments (const TopTools_ListOfShape& theLS);

  //! Returns the list of arguments.
  const TopTools_ListOfShape& Arguments() const
  {
    return myArguments;
  }

public: //! @name Options

  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  //! This flag is taken into account if internal PaveFiller is used only.
  //! In the case of calling PerformWithFiller the corresponding flag of that PaveFiller
  //! is in force.
  void SetNonDestructive(const Standard_Boolean theFlag)
  {
    myNonDestructive = theFlag;
  }

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_Boolean NonDestructive() const
  {
    return myNonDestructive;
  }

  //! Sets the glue option for the algorithm
  void SetGlue(const BOPAlgo_GlueEnum theGlue)
  {
    myGlue = theGlue;
  }

  //! Returns the glue option of the algorithm
  BOPAlgo_GlueEnum Glue() const
  {
    return myGlue;
  }

  //! Enables/Disables the check of the input solids for inverted status
  void SetCheckInverted(const Standard_Boolean theCheck)
  {
    myCheckInverted = theCheck;
  }

  //! Returns the flag defining whether the check for input solids on inverted status
  //! should be performed or not.
  Standard_Boolean CheckInverted() const
  {
    return myCheckInverted;
  }


public: //! @name Performing the operation

  //! Performs the operation.
  //! The intersection will be performed also.
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;

  //! Performs the operation with the prepared filler.
  //! The intersection will not be performed in this case.
  Standard_EXPORT virtual void PerformWithFiller (const BOPAlgo_PaveFiller& theFiller);


public: //! @name History methods

  //! Returns the  list of shapes generated from the shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& theS) Standard_OVERRIDE;

  //! Returns the list of shapes modified from the shape theS.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& theS) Standard_OVERRIDE;

  //! Returns true if the shape theS has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& theS) Standard_OVERRIDE;

protected: //! @name History methods

  //! Prepare information for history support.
  Standard_EXPORT virtual void PrepareHistory() Standard_OVERRIDE;

  //! Prepare history information for the input shapes taking into account possible
  //! operation-specific modifications.
  //! For instance, in the CellsBuilder operation, additionally to splitting input shapes
  //! the splits of the shapes (or the shapes themselves) may be unified during removal of internal
  //! boundaries. In this case each split should be linked to the unified shape.
  //!
  //! To have correct history information, the method should be redefined in each operation
  //! where such additional modification is possible. The input shape <theS> should be the one from arguments,
  //! and the returning list should contain all final elements to which the input shape has evolved,
  //! including those not contained in the result shape.
  //!
  //! The method returns pointer to the list of modified elements.
  //! NULL pointer means that the shape has not been modified at all.
  //!
  //! The General Fuse operation does not perform any other modification than splitting the input
  //! shapes basing on their intersection information. This information is contained in myImages map.
  //! Thus, here the method returns only splits (if any) contained in this map.
  Standard_EXPORT virtual const TopTools_ListOfShape* LocModified(const TopoDS_Shape& theS);


public: //! @name Images/Origins

  //! Returns the map of images.
  const TopTools_DataMapOfShapeListOfShape& Images() const
  {
    return myImages;
  }

  //! Returns the map of origins.
  const TopTools_DataMapOfShapeListOfShape& Origins() const
  {
    return myOrigins;
  }

  //! Returns the map of Same Domain (SD) shapes - coinciding shapes
  //! from different arguments.
  const TopTools_DataMapOfShapeShape& ShapesSD() const
  {
    return myShapesSD;
  }


protected: //! @name Methods for building the result

  //! Performs the building of the result.
  //! The method calls the PerfromInternal1() method surrounded by a try-catch block.
  Standard_EXPORT virtual void PerformInternal (const BOPAlgo_PaveFiller& thePF);

  //! Performs the building of the result.
  //! To build the result of any other operation
  //! it will be necessary to override this method.
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF);

  //! Builds the result of operation.
  //! The method is called for each of the arguments type and
  //! adds into the result the splits of the arguments of that type.
  Standard_EXPORT virtual void BuildResult (const TopAbs_ShapeEnum theType);


protected: //! @name Checking input arguments

  //! Checks the input data.
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  //! Checks if the intersection algorithm has Errors/Warnings.
  Standard_EXPORT void CheckFiller();

  //! Prepares the result shape by making it empty compound.
  Standard_EXPORT virtual void Prepare();


protected: //! @name Fill Images of VERTICES

  //! Fills the images of vertices.
  Standard_EXPORT void FillImagesVertices();


protected: //! @name Fill Images of EDGES

  //! Fills the images of edges.
  Standard_EXPORT void FillImagesEdges();


protected: //! @name Fill Images of CONTAINERS

  //! Fills the images of containers (WIRES/SHELLS/COMPSOLID).
  Standard_EXPORT void FillImagesContainers (const TopAbs_ShapeEnum theType);

  //! Builds the image of the given container using the splits
  //! of its sub-shapes.
  Standard_EXPORT void FillImagesContainer (const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType);


protected: //! @name Fill Images of FACES

  //! Fills the images of faces.
  //! The method consists of three steps:
  //! 1. Build the splits of faces;
  //! 2. Find SD faces;
  //! 3. Add internal vertices (if any) to faces.
  Standard_EXPORT void FillImagesFaces();

  //! Builds the splits of faces using the information from the
  //! intersection stage stored in Data Structure.
  Standard_EXPORT virtual void BuildSplitFaces();

  //! Looks for the same domain faces among the splits of the faces.
  //! Updates the map of images with SD faces.
  Standard_EXPORT void FillSameDomainFaces();

  //! Classifies the alone vertices on faces relatively its splits
  //! and adds them as INTERNAL into the splits.
  Standard_EXPORT void FillInternalVertices();


protected: //! @name Fill Images of SOLIDS

  //! Fills the images of solids.
  //! The method consists of four steps:
  //! 1. Build the draft solid - just rebuild the solid using the splits of faces;
  //! 2. Find faces from other arguments located inside the solids;
  //! 3. Build splits of solid using the inside faces;
  //! 4. Fill internal shapes for the splits (Wires and vertices).
  Standard_EXPORT void FillImagesSolids();

  //! Builds the draft solid by rebuilding the shells of the solid
  //! with the splits of faces.
  Standard_EXPORT void BuildDraftSolid (const TopoDS_Shape& theSolid,
                                        TopoDS_Shape& theDraftSolid,
                                        TopTools_ListOfShape& theLIF);

  //! Finds faces located inside each solid.
  Standard_EXPORT virtual void FillIn3DParts (TopTools_DataMapOfShapeListOfShape& theInParts,
                                              TopTools_DataMapOfShapeShape& theDraftSolids,
                                              const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Builds the splits of the solids using their draft versions
  //! and faces located inside.
  Standard_EXPORT void BuildSplitSolids (TopTools_DataMapOfShapeListOfShape& theInParts,
                                         TopTools_DataMapOfShapeShape& theDraftSolids,
                                         const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Classifies the vertices and edges from the arguments relatively
  //! splits of solids and makes them INTERNAL for solids.
  Standard_EXPORT void FillInternalShapes();


protected: //! @name Fill Images of COMPOUNDS

  //! Fills the images of compounds.
  Standard_EXPORT void FillImagesCompounds();

  //! Builds the image of the given compound.
  Standard_EXPORT void FillImagesCompound (const TopoDS_Shape& theS,
                                           TopTools_MapOfShape& theMF);

protected: //! @name Post treatment

  //! Post treatment of the result of the operation.
  //! The method checks validity of the sub-shapes of the result
  //! and updates the tolerances to make them valid.
  Standard_EXPORT virtual void PostTreat();


protected: //! @name Fields

  TopTools_ListOfShape myArguments;             //!< Arguments of the operation
  TopTools_MapOfShape myMapFence;               //!< Fence map providing the uniqueness of the shapes in the list of arguments
  BOPAlgo_PPaveFiller myPaveFiller;             //!< Pave Filler - algorithm for sub-shapes intersection
  BOPDS_PDS myDS;                               //!< Data Structure - holder of intersection information
  Handle(IntTools_Context) myContext;           //!< Context - tool for cashing heavy algorithms such as Projectors and Classifiers
  Standard_Integer myEntryPoint;                //!< EntryPoint - controls the deletion of the PaveFiller, which could live longer than the Builder
  TopTools_DataMapOfShapeListOfShape myImages;  //!< Images - map of Images of the sub-shapes of arguments
  TopTools_DataMapOfShapeShape myShapesSD;      //!< ShapesSD - map of SD Shapes
  TopTools_DataMapOfShapeListOfShape myOrigins; //!< Origins - map of Origins, back map of Images
  Standard_Boolean myNonDestructive;            //!< Safe processing option allows avoiding modification of the input shapes
  BOPAlgo_GlueEnum myGlue;                      //!< Gluing option allows speeding up the intersection of the input shapes
  Standard_Boolean myCheckInverted;             //!< Check inverted option allows disabling the check of input solids on inverted status

};

#endif // _BOPAlgo_Builder_HeaderFile
