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
#include <BOPAlgo_Operation.hxx>
#include <BOPDS_PDS.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
class IntTools_Context;
class TopoDS_Shape;

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
//!                          i.e. the check is performed. Setting this flag to FALSE for inverted
//!                          solids, most likely will lead to incorrect results.
//!
//! The algorithm returns the following warnings:
//! - *BOPAlgo_AlertUnableToOrientTheShape* - in case the check on the orientation of the split
//! shape
//!                                           to match the orientation of the original shape has
//!                                           failed.
//!
//! The algorithm returns the following Error statuses:
//! - *BOPAlgo_AlertTooFewArguments* - in case there are no enough arguments to perform the
//! operation;
//! - *BOPAlgo_AlertNoFiller* - in case the intersection tool has not been created;
//! - *BOPAlgo_AlertIntersectionFailed* - in case the intersection of the arguments has failed;
//! - *BOPAlgo_AlertBuilderFailed* - in case building splits of arguments has failed with some
//! unexpected error.
//!
class BOPAlgo_Builder : public BOPAlgo_BuilderShape
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
  BOPAlgo_PPaveFiller PPaveFiller() { return myPaveFiller; }

  //! Returns the Data Structure, holder of intersection information.
  BOPDS_PDS PDS() { return myDS; }

  //! Returns the Context, tool for cashing heavy algorithms.
  Handle(IntTools_Context) Context() const { return myContext; }

public: //! @name Arguments
  //! Adds the argument to the operation.
  Standard_EXPORT virtual void AddArgument(const TopoDS_Shape& theShape);

  //! Sets the list of arguments for the operation.
  Standard_EXPORT virtual void SetArguments(const TopTools_ListOfShape& theLS);

  //! Returns the list of arguments.
  const TopTools_ListOfShape& Arguments() const { return myArguments; }

public: //! @name Options
  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  //! This flag is taken into account if internal PaveFiller is used only.
  //! In the case of calling PerformWithFiller the corresponding flag of that PaveFiller
  //! is in force.
  void SetNonDestructive(const Standard_Boolean theFlag) { myNonDestructive = theFlag; }

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_Boolean NonDestructive() const { return myNonDestructive; }

  //! Sets the glue option for the algorithm
  void SetGlue(const BOPAlgo_GlueEnum theGlue) { myGlue = theGlue; }

  //! Returns the glue option of the algorithm
  BOPAlgo_GlueEnum Glue() const { return myGlue; }

  //! Enables/Disables the check of the input solids for inverted status
  void SetCheckInverted(const Standard_Boolean theCheck) { myCheckInverted = theCheck; }

  //! Returns the flag defining whether the check for input solids on inverted status
  //! should be performed or not.
  Standard_Boolean CheckInverted() const { return myCheckInverted; }

public: //! @name Performing the operation
  //! Performs the operation.
  //! The intersection will be performed also.
  Standard_EXPORT virtual void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Performs the operation with the prepared filler.
  //! The intersection will not be performed in this case.
  Standard_EXPORT virtual void PerformWithFiller(
    const BOPAlgo_PaveFiller&    theFiller,
    const Message_ProgressRange& theRange = Message_ProgressRange());

public: //! @name BOPs on open solids
  //! Builds the result shape according to the given states for the objects
  //! and tools. These states can be unambiguously converted into the Boolean operation type.
  //! Thus, it performs the Boolean operation on the given groups of shapes.
  //!
  //! The result is built basing on the result of Builder operation (GF or any other).
  //! The only condition for the Builder is that the splits of faces should be created
  //! and classified relatively solids.
  //!
  //! The method uses classification approach for choosing the faces which will
  //! participate in building the result shape:
  //! - All faces from each group having the given state for the opposite group
  //!   will be taken into result.
  //!
  //! Such approach shows better results (in comparison with BOPAlgo_BuilderSolid approach)
  //! when working with open solids. However, the result may not be always
  //! correct on such data (at least, not as expected) as the correct classification
  //! of the faces relatively open solids is not always possible and may vary
  //! depending on the chosen classification point on the face.
  //!
  //! History is not created for the solids in this method.
  //!
  //! To avoid pollution of the report of Builder algorithm, there is a possibility to pass
  //! the different report to collect the alerts of the method only. But, if the new report
  //! is not given, the Builder report will be used.
  //! So, even if Builder passed without any errors, but some error has been stored into its report
  //! in this method, for the following calls the Builder report must be cleared.
  //!
  //! The method may set the following errors:
  //! - BOPAlgo_AlertBuilderFailed - Building operation has not been performed yet or failed;
  //! - BOPAlgo_AlertBOPNotSet - invalid BOP type is given (COMMON/FUSE/CUT/CUT21 are supported);
  //! - BOPAlgo_AlertTooFewArguments - arguments are not given;
  //! - BOPAlgo_AlertUnknownShape - the shape is unknown for the operation.
  //!
  //! Parameters:
  //! @param theObjects     - The group of Objects for BOP;
  //! @param theObjState    - State for objects faces to pass into result;
  //! @param theTools       - The group of Tools for BOP;
  //! @param theToolsState  - State for tools faces to pass into result;
  //! @param theReport      - The alternative report to avoid pollution of the main one.
  Standard_EXPORT virtual void BuildBOP(const TopTools_ListOfShape&  theObjects,
                                        const TopAbs_State           theObjState,
                                        const TopTools_ListOfShape&  theTools,
                                        const TopAbs_State           theToolsState,
                                        const Message_ProgressRange& theRange,
                                        Handle(Message_Report)       theReport = NULL);

  //! Builds the result of Boolean operation of given type
  //! basing on the result of Builder operation (GF or any other).
  //!
  //! The method converts the given type of operation into the states
  //! for the objects and tools required for their face to pass into result
  //! and performs the call to the same method, but with states instead
  //! of operation type.
  //!
  //! The conversion looks as follows:
  //! - COMMON is built from the faces of objects located IN any of the tools
  //!          and vice versa.
  //! - FUSE   is built from the faces OUT of all given shapes;
  //! - CUT    is built from the faces of the objects OUT of the tools and
  //!          faces of the tools located IN solids of the objects.
  //!
  //! @param theObjects   - The group of Objects for BOP;
  //! @param theTools     - The group of Tools for BOP;
  //! @param theOperation - The BOP type;
  //! @param theRange     - The parameter to progressIndicator
  //! @param theReport    - The alternative report to avoid pollution of the global one.
  void BuildBOP(const TopTools_ListOfShape&  theObjects,
                const TopTools_ListOfShape&  theTools,
                const BOPAlgo_Operation      theOperation,
                const Message_ProgressRange& theRange,
                Handle(Message_Report)       theReport = NULL)
  {
    TopAbs_State anObjState, aToolsState;
    switch (theOperation)
    {
      case BOPAlgo_COMMON: {
        anObjState  = TopAbs_IN;
        aToolsState = TopAbs_IN;
        break;
      }
      case BOPAlgo_FUSE: {
        anObjState  = TopAbs_OUT;
        aToolsState = TopAbs_OUT;
        break;
      }
      case BOPAlgo_CUT: {
        anObjState  = TopAbs_OUT;
        aToolsState = TopAbs_IN;
        break;
      }
      case BOPAlgo_CUT21: {
        anObjState  = TopAbs_IN;
        aToolsState = TopAbs_OUT;
        break;
      }
      default: {
        anObjState  = TopAbs_UNKNOWN;
        aToolsState = TopAbs_UNKNOWN;
        break;
      }
    }
    BuildBOP(theObjects, anObjState, theTools, aToolsState, theRange, theReport);
  }

protected: //! @name History methods
  //! Prepare information for history support.
  Standard_EXPORT void PrepareHistory(const Message_ProgressRange& theRange);

  //! Prepare history information for the input shapes taking into account possible
  //! operation-specific modifications.
  //! For instance, in the CellsBuilder operation, additionally to splitting input shapes
  //! the splits of the shapes (or the shapes themselves) may be unified during removal of internal
  //! boundaries. In this case each split should be linked to the unified shape.
  //!
  //! To have correct history information, the method should be redefined in each operation
  //! where such additional modification is possible. The input shape <theS> should be the one from
  //! arguments, and the returning list should contain all final elements to which the input shape
  //! has evolved, including those not contained in the result shape.
  //!
  //! The method returns pointer to the list of modified elements.
  //! NULL pointer means that the shape has not been modified at all.
  //!
  //! The General Fuse operation does not perform any other modification than splitting the input
  //! shapes basing on their intersection information. This information is contained in myImages
  //! map. Thus, here the method returns only splits (if any) contained in this map.
  Standard_EXPORT virtual const TopTools_ListOfShape* LocModified(const TopoDS_Shape& theS);

  //! Returns the list of shapes generated from the shape theS.
  //! Similarly to *LocModified* must be redefined for specific operations,
  //! obtaining Generated elements differently.
  Standard_EXPORT virtual const TopTools_ListOfShape& LocGenerated(const TopoDS_Shape& theS);

public: //! @name Images/Origins
  //! Returns the map of images.
  const TopTools_DataMapOfShapeListOfShape& Images() const { return myImages; }

  //! Returns the map of origins.
  const TopTools_DataMapOfShapeListOfShape& Origins() const { return myOrigins; }

  //! Returns the map of Same Domain (SD) shapes - coinciding shapes
  //! from different arguments.
  const TopTools_DataMapOfShapeShape& ShapesSD() const { return myShapesSD; }

protected: //! @name Analyze progress of the operation
  //! List of operations to be supported by the Progress Indicator
  enum BOPAlgo_PIOperation
  {
    PIOperation_TreatVertices = 0,
    PIOperation_TreatEdges,
    PIOperation_TreatWires,
    PIOperation_TreatFaces,
    PIOperation_TreatShells,
    PIOperation_TreatSolids,
    PIOperation_TreatCompsolids,
    PIOperation_TreatCompounds,
    PIOperation_FillHistory,
    PIOperation_PostTreat,
    PIOperation_Last
  };

  //! Auxiliary structure to get information about number of shapes
  //! of each type participated in operation.
  class NbShapes
  {
  public:
    NbShapes()
    {
      for (Standard_Integer i = 0; i < 8; ++i)
      {
        myNbShapesArr[i] = 0;
      }
    }

    Standard_Integer NbVertices() const { return myNbShapesArr[0]; }

    Standard_Integer NbEdges() const { return myNbShapesArr[1]; }

    Standard_Integer NbWires() const { return myNbShapesArr[2]; }

    Standard_Integer NbFaces() const { return myNbShapesArr[3]; }

    Standard_Integer NbShells() const { return myNbShapesArr[4]; }

    Standard_Integer NbSolids() const { return myNbShapesArr[5]; }

    Standard_Integer NbCompsolids() const { return myNbShapesArr[6]; }

    Standard_Integer NbCompounds() const { return myNbShapesArr[7]; }

    Standard_Integer& NbVertices() { return myNbShapesArr[0]; }

    Standard_Integer& NbEdges() { return myNbShapesArr[1]; }

    Standard_Integer& NbWires() { return myNbShapesArr[2]; }

    Standard_Integer& NbFaces() { return myNbShapesArr[3]; }

    Standard_Integer& NbShells() { return myNbShapesArr[4]; }

    Standard_Integer& NbSolids() { return myNbShapesArr[5]; }

    Standard_Integer& NbCompsolids() { return myNbShapesArr[6]; }

    Standard_Integer& NbCompounds() { return myNbShapesArr[7]; }

  private:
    Standard_Integer myNbShapesArr[8];
  };

protected:
  //! Compute number of shapes of certain type participating in operation
  Standard_EXPORT NbShapes getNbShapes() const;

  //! Filling steps for constant operations
  Standard_EXPORT void fillPIConstants(const Standard_Real theWhole,
                                       BOPAlgo_PISteps&    theSteps) const Standard_OVERRIDE;

  //! Filling steps for all other operations
  Standard_EXPORT void fillPISteps(BOPAlgo_PISteps& theSteps) const Standard_OVERRIDE;

protected: //! @name Methods for building the result
  //! Performs the building of the result.
  //! The method calls the PerformInternal1() method surrounded by a try-catch block.
  Standard_EXPORT virtual void PerformInternal(const BOPAlgo_PaveFiller&    thePF,
                                               const Message_ProgressRange& theRange);

  //! Performs the building of the result.
  //! To build the result of any other operation
  //! it will be necessary to override this method.
  Standard_EXPORT virtual void PerformInternal1(const BOPAlgo_PaveFiller&    thePF,
                                                const Message_ProgressRange& theRange);

  //! Builds the result of operation.
  //! The method is called for each of the arguments type and
  //! adds into the result the splits of the arguments of that type.
  Standard_EXPORT virtual void BuildResult(const TopAbs_ShapeEnum theType);

protected: //! @name Checking input arguments
  //! Checks the input data.
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

  //! Checks if the intersection algorithm has Errors/Warnings.
  Standard_EXPORT void CheckFiller();

  //! Prepares the result shape by making it empty compound.
  Standard_EXPORT virtual void Prepare();

protected: //! @name Fill Images of VERTICES
  //! Fills the images of vertices.
  Standard_EXPORT void FillImagesVertices(const Message_ProgressRange& theRange);

protected: //! @name Fill Images of EDGES
  //! Fills the images of edges.
  Standard_EXPORT void FillImagesEdges(const Message_ProgressRange& theRange);

protected: //! @name Fill Images of CONTAINERS
  //! Fills the images of containers (WIRES/SHELLS/COMPSOLID).
  Standard_EXPORT void FillImagesContainers(const TopAbs_ShapeEnum       theType,
                                            const Message_ProgressRange& theRange);

  //! Builds the image of the given container using the splits
  //! of its sub-shapes.
  Standard_EXPORT void FillImagesContainer(const TopoDS_Shape&    theS,
                                           const TopAbs_ShapeEnum theType);

protected: //! @name Fill Images of FACES
  //! Fills the images of faces.
  //! The method consists of three steps:
  //! 1. Build the splits of faces;
  //! 2. Find SD faces;
  //! 3. Add internal vertices (if any) to faces.
  Standard_EXPORT void FillImagesFaces(const Message_ProgressRange& theRange);

  //! Builds the splits of faces using the information from the
  //! intersection stage stored in Data Structure.
  Standard_EXPORT virtual void BuildSplitFaces(const Message_ProgressRange& theRange);

  //! Looks for the same domain faces among the splits of the faces.
  //! Updates the map of images with SD faces.
  Standard_EXPORT void FillSameDomainFaces(const Message_ProgressRange& theRange);

  //! Classifies the alone vertices on faces relatively its splits
  //! and adds them as INTERNAL into the splits.
  Standard_EXPORT void FillInternalVertices(const Message_ProgressRange& theRange);

protected: //! @name Fill Images of SOLIDS
  //! Fills the images of solids.
  //! The method consists of four steps:
  //! 1. Build the draft solid - just rebuild the solid using the splits of faces;
  //! 2. Find faces from other arguments located inside the solids;
  //! 3. Build splits of solid using the inside faces;
  //! 4. Fill internal shapes for the splits (Wires and vertices).
  Standard_EXPORT void FillImagesSolids(const Message_ProgressRange& theRange);

  //! Builds the draft solid by rebuilding the shells of the solid
  //! with the splits of faces.
  Standard_EXPORT void BuildDraftSolid(const TopoDS_Shape&   theSolid,
                                       TopoDS_Shape&         theDraftSolid,
                                       TopTools_ListOfShape& theLIF);

  //! Finds faces located inside each solid.
  Standard_EXPORT virtual void FillIn3DParts(TopTools_DataMapOfShapeShape& theDraftSolids,
                                             const Message_ProgressRange&  theRange);

  //! Builds the splits of the solids using their draft versions
  //! and faces located inside.
  Standard_EXPORT void BuildSplitSolids(TopTools_DataMapOfShapeShape& theDraftSolids,
                                        const Message_ProgressRange&  theRange);

  //! Classifies the vertices and edges from the arguments relatively
  //! splits of solids and makes them INTERNAL for solids.
  Standard_EXPORT void FillInternalShapes(const Message_ProgressRange& theRange);

protected: //! @name Fill Images of COMPOUNDS
  //! Fills the images of compounds.
  Standard_EXPORT void FillImagesCompounds(const Message_ProgressRange& theRange);

  //! Builds the image of the given compound.
  Standard_EXPORT void FillImagesCompound(const TopoDS_Shape& theS, TopTools_MapOfShape& theMF);

protected: //! @name Post treatment
  //! Post treatment of the result of the operation.
  //! The method checks validity of the sub-shapes of the result
  //! and updates the tolerances to make them valid.
  Standard_EXPORT virtual void PostTreat(const Message_ProgressRange& theRange);

protected:                          //! @name Fields
  TopTools_ListOfShape myArguments; //!< Arguments of the operation
  // clang-format off
  TopTools_MapOfShape myMapFence;               //!< Fence map providing the uniqueness of the shapes in the list of arguments
  BOPAlgo_PPaveFiller myPaveFiller;             //!< Pave Filler - algorithm for sub-shapes intersection
  BOPDS_PDS myDS;                               //!< Data Structure - holder of intersection information
  Handle(IntTools_Context) myContext;           //!< Context - tool for cashing heavy algorithms such as Projectors and Classifiers
  Standard_Integer myEntryPoint;                //!< EntryPoint - controls the deletion of the PaveFiller, which could live longer than the Builder
  TopTools_DataMapOfShapeListOfShape myImages;  //!< Images - map of Images of the sub-shapes of arguments
  TopTools_DataMapOfShapeShape myShapesSD;      //!< ShapesSD - map of SD Shapes
  TopTools_DataMapOfShapeListOfShape myOrigins; //!< Origins - map of Origins, back map of Images
  TopTools_DataMapOfShapeListOfShape myInParts; //!< InParts - map of own and acquired IN faces of the arguments solids
  Standard_Boolean myNonDestructive;            //!< Safe processing option allows avoiding modification of the input shapes
  BOPAlgo_GlueEnum myGlue;                      //!< Gluing option allows speeding up the intersection of the input shapes
  Standard_Boolean myCheckInverted;             //!< Check inverted option allows disabling the check of input solids on inverted status
  // clang-format on
};

#endif // _BOPAlgo_Builder_HeaderFile
