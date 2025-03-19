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

#ifndef _BOPAlgo_PaveFiller_HeaderFile
#define _BOPAlgo_PaveFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Algo.hxx>
#include <BOPAlgo_GlueEnum.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_IndexedDataMapOfPaveBlockListOfInteger.hxx>
#include <BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks.hxx>
#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_PIterator.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPTools_BoxTree.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerReal.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class IntTools_Context;
class BOPDS_PaveBlock;
class gp_Pnt;
class BOPDS_Curve;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Face;

//!
//! The class represents the Intersection phase of the
//! Boolean Operations algorithm.<br>
//! It performs the pairwise intersection of the sub-shapes of
//! the arguments in the following order:<br>
//! 1. Vertex/Vertex;<br>
//! 2. Vertex/Edge;<br>
//! 3. Edge/Edge;<br>
//! 4. Vertex/Face;<br>
//! 5. Edge/Face;<br>
//! 6. Face/Face.<br>
//!
//! The results of intersection are stored into the Data Structure
//! of the algorithm.<br>
//!
//! Additionally to the options provided by the parent class,
//! the algorithm has the following options:<br>
//! - *Section attributes* - allows to customize the intersection of the faces
//!                          (avoid approximation or building 2d curves);<br>
//! - *Safe processing mode* - allows to avoid modification of the input
//!                            shapes during the operation (by default it is off);<br>
//! - *Gluing options* - allows to speed up the calculation on the special
//!                      cases, in which some sub-shapes are coincide.<br>
//!
//! The algorithm returns the following Warning statuses:
//! - *BOPAlgo_AlertSelfInterferingShape* - in case some of the argument shapes are self-interfering
//! shapes;
//! - *BOPAlgo_AlertTooSmallEdge* - in case some edges of the input shapes have no valid range;
//! - *BOPAlgo_AlertNotSplittableEdge* - in case some edges of the input shapes has such a small
//!                                      valid range so it cannot be split;
//! - *BOPAlgo_AlertBadPositioning* - in case the positioning of the input shapes leads to creation
//!                                   of small edges;
//! - *BOPAlgo_AlertIntersectionOfPairOfShapesFailed* - in case intersection of some of the
//!                                                     sub-shapes has failed;
//! - *BOPAlgo_AlertAcquiredSelfIntersection* - in case some sub-shapes of the argument become
//! connected
//!                                             through other shapes;
//! - *BOPAlgo_AlertBuildingPCurveFailed* - in case building 2D curve for some of the edges
//!                                         on the faces has failed.
//!
//! The algorithm returns the following Error alerts:
//! - *BOPAlgo_AlertTooFewArguments* - in case there are no enough arguments to
//!                      perform the operation;<br>
//! - *BOPAlgo_AlertIntersectionFailed* - in case some unexpected error occurred;<br>
//! - *BOPAlgo_AlertNullInputShapes* - in case some of the arguments are null shapes.<br>
//!
class BOPAlgo_PaveFiller : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BOPAlgo_PaveFiller();

  Standard_EXPORT virtual ~BOPAlgo_PaveFiller();

  Standard_EXPORT BOPAlgo_PaveFiller(const Handle(NCollection_BaseAllocator)& theAllocator);

  Standard_EXPORT const BOPDS_DS& DS();

  Standard_EXPORT BOPDS_PDS PDS();

  Standard_EXPORT const BOPDS_PIterator& Iterator();

  //! Sets the arguments for operation
  void SetArguments(const TopTools_ListOfShape& theLS) { myArguments = theLS; }

  //! Adds the argument for operation
  void AddArgument(const TopoDS_Shape& theShape) { myArguments.Append(theShape); }

  //! Returns the list of arguments
  const TopTools_ListOfShape& Arguments() const { return myArguments; }

  Standard_EXPORT const Handle(IntTools_Context)& Context();

  Standard_EXPORT void SetSectionAttribute(const BOPAlgo_SectionAttribute& theSecAttr);

  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT void SetNonDestructive(const Standard_Boolean theFlag);

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT Standard_Boolean NonDestructive() const;

  Standard_EXPORT virtual void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Sets the glue option for the algorithm
  Standard_EXPORT void SetGlue(const BOPAlgo_GlueEnum theGlue);

  //! Returns the glue option of the algorithm
  Standard_EXPORT BOPAlgo_GlueEnum Glue() const;

  //! Sets the flag to avoid building of p-curves of edges on faces
  void SetAvoidBuildPCurve(const Standard_Boolean theValue) { myAvoidBuildPCurve = theValue; }

  //! Returns the flag to avoid building of p-curves of edges on faces
  Standard_Boolean IsAvoidBuildPCurve() const { return myAvoidBuildPCurve; }

protected:
  typedef NCollection_DataMap<Handle(BOPDS_PaveBlock), Bnd_Box> BOPAlgo_DataMapOfPaveBlockBndBox;

  typedef NCollection_DataMap<Handle(BOPDS_PaveBlock), TColStd_ListOfInteger>
    BOPAlgo_DataMapOfPaveBlockListOfInteger;

  typedef NCollection_DataMap<Standard_Integer, BOPDS_MapOfPaveBlock>
    BOPAlgo_DataMapOfIntegerMapOfPaveBlock;

  //! Sets non-destructive mode automatically if an argument
  //! contains a locked sub-shape (see TopoDS_Shape::Locked()).
  Standard_EXPORT void SetNonDestructive();

  Standard_EXPORT void SetIsPrimary(const Standard_Boolean theFlag);

  Standard_EXPORT Standard_Boolean IsPrimary() const;

  Standard_EXPORT virtual void PerformInternal(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  Standard_EXPORT virtual void Init(const Message_ProgressRange& theRange);

  Standard_EXPORT void Prepare(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformVV(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformVE(const Message_ProgressRange& theRange);

  //! Performs the intersection of the vertices with edges.
  Standard_EXPORT void IntersectVE(const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theVEPairs,
                                   const Message_ProgressRange&                        theRange,
                                   const Standard_Boolean bAddInterfs = Standard_True);

  //! Splits the Pave Blocks of the given edges with the extra paves.<br>
  //! The method also builds the shrunk data for the new pave blocks and
  //! in case there is no valid range on the pave block, the vertices of
  //! this pave block will be united making SD vertex.<br>
  //! Parameter <theAddInterfs> defines whether this interference will be added
  //! into common table of interferences or not.<br>
  //! If some of the Pave Blocks are forming the Common Blocks, the splits
  //! of the Pave Blocks will also form a Common Block.
  Standard_EXPORT void SplitPaveBlocks(const TColStd_MapOfInteger& theMEdges,
                                       const Standard_Boolean      theAddInterfs);

  Standard_EXPORT virtual void PerformVF(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformEE(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformEF(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformFF(const Message_ProgressRange& theRange);

  Standard_EXPORT void TreatVerticesEE();

  Standard_EXPORT void MakeSDVerticesFF(const TColStd_DataMapOfIntegerListOfInteger& aDMVLV,
                                        TColStd_DataMapOfIntegerInteger&             theDMNewSD);

  Standard_EXPORT void MakeSplitEdges(const Message_ProgressRange& theRange);

  Standard_EXPORT void MakeBlocks(const Message_ProgressRange& theRange);

  Standard_EXPORT void MakePCurves(const Message_ProgressRange& theRange);

  Standard_EXPORT Standard_Integer MakeSDVertices(const TColStd_ListOfInteger& theVertIndices,
                                                  const Standard_Boolean       theAddInterfs = 1);

  Standard_EXPORT void ProcessDE(const Message_ProgressRange& theRange);

  Standard_EXPORT void FillShrunkData(Handle(BOPDS_PaveBlock)& thePB);

  Standard_EXPORT void FillShrunkData(const TopAbs_ShapeEnum theType1,
                                      const TopAbs_ShapeEnum theType2);

  //! Analyzes the results of computation of the valid range for the
  //! pave block and in case of error adds the warning status, otherwise
  //! saves the valid range in the pave block.
  Standard_EXPORT void AnalyzeShrunkData(const Handle(BOPDS_PaveBlock)& thePB,
                                         const IntTools_ShrunkRange&    theSR);

  //! Performs intersection of new vertices, obtained in E/E and E/F intersections
  Standard_EXPORT void PerformNewVertices(
    BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
    const Handle(NCollection_BaseAllocator)&       theAllocator,
    const Message_ProgressRange&                   theRange,
    const Standard_Boolean                         theIsEEIntersection = Standard_True);

  Standard_EXPORT Standard_Boolean CheckFacePaves(const TopoDS_Vertex&        theVnew,
                                                  const TColStd_MapOfInteger& theMIF);

  Standard_EXPORT static Standard_Boolean CheckFacePaves(const Standard_Integer      theN,
                                                         const TColStd_MapOfInteger& theMIFOn,
                                                         const TColStd_MapOfInteger& theMIFIn);

  Standard_EXPORT Standard_Boolean IsExistingVertex(const gp_Pnt&               theP,
                                                    const Standard_Real         theTol,
                                                    const TColStd_MapOfInteger& theMVOn) const;

  //! Checks and puts paves from <theMVOnIn> on the curve <theNC>.
  //! At that, common (from theMVCommon) and not common vertices
  //! are processed differently.
  Standard_EXPORT void PutPavesOnCurve(const TColStd_MapOfInteger&            theMVOnIn,
                                       const TColStd_MapOfInteger&            theMVCommon,
                                       BOPDS_Curve&                           theNC,
                                       const TColStd_MapOfInteger&            theMI,
                                       const TColStd_MapOfInteger&            theMVEF,
                                       TColStd_DataMapOfIntegerReal&          theMVTol,
                                       TColStd_DataMapOfIntegerListOfInteger& theDMVLV);

  Standard_EXPORT void FilterPavesOnCurves(const BOPDS_VectorOfCurve&    theVNC,
                                           TColStd_DataMapOfIntegerReal& theMVTol);

  //! Depending on the parameter aType it checks whether
  //! the vertex nV was created in EE or EF intersections.
  //! If so, it increases aTolVExt from tolerance value of vertex to
  //! the max distance from vertex nV to the ends of the range of common part.
  //! Possible values of aType:
  //! 1 - checks only EE;
  //! 2 - checks only EF;
  //! other - checks both types of intersections.
  Standard_EXPORT Standard_Boolean ExtendedTolerance(const Standard_Integer      nV,
                                                     const TColStd_MapOfInteger& aMI,
                                                     Standard_Real&              aTolVExt,
                                                     const Standard_Integer      aType = 0);

  Standard_EXPORT void PutBoundPaveOnCurve(const TopoDS_Face&     theF1,
                                           const TopoDS_Face&     theF2,
                                           BOPDS_Curve&           theNC,
                                           TColStd_ListOfInteger& theLBV);

  //! Checks if the given pave block (created on section curve)
  //! coincides with any of the pave blocks of the faces
  //! created the section curve.
  Standard_EXPORT Standard_Boolean IsExistingPaveBlock(const Handle(BOPDS_PaveBlock)&     thePB,
                                                       const BOPDS_Curve&                 theNC,
                                                       const Standard_Real                theTolR3D,
                                                       const BOPDS_IndexedMapOfPaveBlock& theMPB,
                                                       BOPTools_BoxTree&                  thePBTree,
                                                       const BOPDS_MapOfPaveBlock& theMPBCommon,
                                                       Handle(BOPDS_PaveBlock)&    thePBOut,
                                                       Standard_Real&              theTolNew);

  //! Checks if the given pave block (created on section curve)
  //! coincides with any of the edges shared between the faces
  //! created the section curve.
  Standard_EXPORT Standard_Boolean IsExistingPaveBlock(const Handle(BOPDS_PaveBlock)& thePB,
                                                       const BOPDS_Curve&             theNC,
                                                       const TColStd_ListOfInteger&   theLSE,
                                                       Standard_Integer&              theNEOut,
                                                       Standard_Real&                 theTolNew);

  //! Treatment of section edges.
  Standard_EXPORT void PostTreatFF(BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
                                   BOPDS_DataMapOfPaveBlockListOfPaveBlock&       theDMExEdges,
                                   TColStd_DataMapOfIntegerInteger&               theDMNewSD,
                                   const BOPDS_IndexedMapOfPaveBlock&             theMicroPB,
                                   const TopTools_IndexedMapOfShape&        theVertsOnRejectedPB,
                                   const Handle(NCollection_BaseAllocator)& theAllocator,
                                   const Message_ProgressRange&             theRange);

  Standard_EXPORT void FindPaveBlocks(const Standard_Integer theV,
                                      const Standard_Integer theF,
                                      BOPDS_ListOfPaveBlock& theLPB);

  Standard_EXPORT void FillPaves(const Standard_Integer         theV,
                                 const Standard_Integer         theE,
                                 const Standard_Integer         theF,
                                 const BOPDS_ListOfPaveBlock&   theLPB,
                                 const Handle(BOPDS_PaveBlock)& thePB);

  Standard_EXPORT void MakeSplitEdge(const Standard_Integer theV, const Standard_Integer theF);

  Standard_EXPORT void GetEFPnts(const Standard_Integer nF1,
                                 const Standard_Integer nF2,
                                 IntSurf_ListOfPntOn2S& aListOfPnts);

  //! Checks and puts paves created in EF intersections on the curve <theNC>.
  Standard_EXPORT void PutEFPavesOnCurve(const BOPDS_VectorOfCurve&             theVC,
                                         const Standard_Integer                 theIndex,
                                         const TColStd_MapOfInteger&            theMI,
                                         const TColStd_MapOfInteger&            theMVEF,
                                         TColStd_DataMapOfIntegerReal&          theMVTol,
                                         TColStd_DataMapOfIntegerListOfInteger& aDMVLV);

  //! Puts stick paves on the curve <theNC>
  Standard_EXPORT void PutStickPavesOnCurve(const TopoDS_Face&                     aF1,
                                            const TopoDS_Face&                     aF2,
                                            const TColStd_MapOfInteger&            theMI,
                                            const BOPDS_VectorOfCurve&             theVC,
                                            const Standard_Integer                 theIndex,
                                            const TColStd_MapOfInteger&            theMVStick,
                                            TColStd_DataMapOfIntegerReal&          theMVTol,
                                            TColStd_DataMapOfIntegerListOfInteger& aDMVLV);

  //! Collects indices of vertices created in all intersections between
  //! two faces (<nF1> and <nF2>) to the map <theMVStick>.
  //! Also, it collects indices of EF vertices to the <theMVEF> map
  //! and indices of all subshapes of these two faces to the <theMI> map.
  Standard_EXPORT void GetStickVertices(const Standard_Integer nF1,
                                        const Standard_Integer nF2,
                                        TColStd_MapOfInteger&  theMVStick,
                                        TColStd_MapOfInteger&  theMVEF,
                                        TColStd_MapOfInteger&  theMI);

  //! Collects index nF and indices of all subshapes of the shape with index <nF>
  //! to the map <theMI>.
  Standard_EXPORT void GetFullShapeMap(const Standard_Integer nF, TColStd_MapOfInteger& theMI);

  //! Removes indices of vertices that are already on the
  //! curve <theNC> from the map <theMV>.
  //! It is used in PutEFPavesOnCurve and PutStickPavesOnCurve methods.
  Standard_EXPORT void RemoveUsedVertices(const BOPDS_VectorOfCurve& theVC,
                                          TColStd_MapOfInteger&      theMV);

  //! Puts the pave nV on the curve theNC.
  //! Parameter aType defines whether to check the pave with
  //! extended tolerance:
  //! 0 - do not perform the check;
  //! other - perform the check (aType goes to ExtendedTolerance).
  Standard_EXPORT void PutPaveOnCurve(const Standard_Integer                 nV,
                                      const Standard_Real                    theTolR3D,
                                      const BOPDS_Curve&                     theNC,
                                      const TColStd_MapOfInteger&            theMI,
                                      TColStd_DataMapOfIntegerReal&          theMVTol,
                                      TColStd_DataMapOfIntegerListOfInteger& aDMVLV,
                                      const Standard_Integer                 aType = 0);

  //! Adds the existing edges for intersection with section edges
  //! by checking the possible intersection with the faces comparing
  //! pre-saved E-F distances with new tolerances.
  Standard_EXPORT void ProcessExistingPaveBlocks(
    const Standard_Integer                         theInt,
    const Standard_Integer                         theCur,
    const Standard_Integer                         nF1,
    const Standard_Integer                         nF2,
    const TopoDS_Edge&                             theES,
    const BOPDS_IndexedMapOfPaveBlock&             theMPBOnIn,
    BOPTools_BoxTree&                              thePBTree,
    BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
    TopTools_DataMapOfShapeInteger&                theMVI,
    BOPDS_ListOfPaveBlock&                         theLPBC,
    BOPAlgo_DataMapOfPaveBlockListOfInteger&       thePBFacesMap,
    BOPDS_MapOfPaveBlock&                          theMPB);

  //! Adds the existing edges from the map <theMPBOnIn> which interfere
  //! with the vertices from <theMVB> map to the post treatment of section edges.
  Standard_EXPORT void ProcessExistingPaveBlocks(
    const Standard_Integer                         theInt,
    const Standard_Integer                         nF1,
    const Standard_Integer                         nF2,
    const BOPDS_IndexedMapOfPaveBlock&             theMPBOnIn,
    BOPTools_BoxTree&                              thePBTree,
    const TColStd_DataMapOfIntegerListOfInteger&   theDMBV,
    BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
    TopTools_DataMapOfShapeInteger&                theMVI,
    BOPAlgo_DataMapOfPaveBlockListOfInteger&       thePBFacesMap,
    BOPDS_MapOfPaveBlock&                          theMPB);

  //! Replaces existing pave block <thePB> with new pave blocks <theLPB>.
  //! The list <theLPB> contains images of <thePB> which were created in
  //! the post treatment of section edges.
  //! Tries to project the new edges on the faces contained in the <thePBFacesMap>.
  Standard_EXPORT void UpdateExistingPaveBlocks(
    const Handle(BOPDS_PaveBlock)&                 thePB,
    BOPDS_ListOfPaveBlock&                         theLPB,
    const BOPAlgo_DataMapOfPaveBlockListOfInteger& thePBFacesMap);

  //! Treatment of vertices that were created in EE intersections.
  Standard_EXPORT void TreatNewVertices(
    const BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
    TopTools_IndexedDataMapOfShapeListOfShape&           theImages);

  //! Put paves on the curve <aBC> in case when <aBC>
  //! is closed 3D-curve
  Standard_EXPORT void PutClosingPaveOnCurve(BOPDS_Curve& aNC);

  //! Keeps data for post treatment
  Standard_EXPORT void PreparePostTreatFF(const Standard_Integer                         aInt,
                                          const Standard_Integer                         aCur,
                                          const Handle(BOPDS_PaveBlock)&                 aPB,
                                          BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB,
                                          TopTools_DataMapOfShapeInteger&                aMVI,
                                          BOPDS_ListOfPaveBlock&                         aLPB);

  //! Updates the information about faces
  Standard_EXPORT void UpdateFaceInfo(BOPDS_DataMapOfPaveBlockListOfPaveBlock&       theDME,
                                      const TColStd_DataMapOfIntegerInteger&         theDMV,
                                      const BOPAlgo_DataMapOfPaveBlockListOfInteger& thePBFacesMap);

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with edge.
  //! Returns TRUE if intersection happened.
  Standard_EXPORT Standard_Boolean ForceInterfVE(const Standard_Integer   nV,
                                                 Handle(BOPDS_PaveBlock)& aPB,
                                                 TColStd_MapOfInteger&    theMEdges);

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with face with index <nF>
  Standard_EXPORT Standard_Boolean ForceInterfVF(const Standard_Integer nV,
                                                 const Standard_Integer nF);

  //! Checks if there are any common or intersecting sub shapes
  //! between two planar faces.
  Standard_EXPORT Standard_Boolean CheckPlanes(const Standard_Integer nF1,
                                               const Standard_Integer nF2) const;

  //! Creates new edge from the edge nE with vertices nV1 and nV2
  //! and returns the index of that new edge in the DS.
  Standard_EXPORT Standard_Integer SplitEdge(const Standard_Integer nE,
                                             const Standard_Integer nV1,
                                             const Standard_Real    aT1,
                                             const Standard_Integer nV2,
                                             const Standard_Real    aT2);

  //! Updates pave blocks which have the paves with indices contained
  //! in the map <aDMNewSD>.
  Standard_EXPORT void UpdatePaveBlocks(const TColStd_DataMapOfIntegerInteger& aDMNewSD);

  //! Updates tolerance vertex nV due to V/E interference.
  //! It always creates new vertex if nV is from arguments.
  //! @return  DS index of updated vertex.
  Standard_EXPORT Standard_Integer UpdateVertex(const Standard_Integer nV,
                                                const Standard_Real    aTolNew);

  Standard_EXPORT void UpdatePaveBlocksWithSDVertices();

  Standard_EXPORT void UpdateCommonBlocksWithSDVertices();

  Standard_EXPORT void UpdateBlocksWithSharedVertices();

  Standard_EXPORT void UpdateInterfsWithSDVertices();

  Standard_EXPORT Standard_Boolean EstimatePaveOnCurve(const Standard_Integer nV,
                                                       const BOPDS_Curve&     theNC,
                                                       const Standard_Real    theTolR3D);

  Standard_EXPORT void UpdateEdgeTolerance(const Standard_Integer nE, const Standard_Real aTolNew);

  Standard_EXPORT void RemovePaveBlocks(const TColStd_MapOfInteger& theEdges);

  Standard_EXPORT void CorrectToleranceOfSE();

  //! Reduce the intersection range using the common ranges of
  //! Edge/Edge interferences to avoid creation of close
  //! intersection vertices
  Standard_EXPORT void ReduceIntersectionRange(const Standard_Integer theV1,
                                               const Standard_Integer theV2,
                                               const Standard_Integer theE,
                                               const Standard_Integer theF,
                                               Standard_Real&         theTS1,
                                               Standard_Real&         theTS2);

  //! Gets the bounding box for the given Pave Block.
  //! If Pave Block has shrunk data it will be used to get the box,
  //! and the Shrunk Range (<theSFirst>, <theSLast>).
  //! Otherwise the box will be computed using BndLib_Add3dCurve method,
  //! and the Shrunk Range will be equal to the PB's range.
  //! To avoid re-computation of the bounding box for the same Pave Block
  //! it will be saved in the map <thePBBox>.
  //! Returns FALSE in case the PB's range is less than the
  //! Precision::PConfusion(), otherwise returns TRUE.
  Standard_EXPORT Standard_Boolean GetPBBox(const TopoDS_Edge&                theE,
                                            const Handle(BOPDS_PaveBlock)&    thePB,
                                            BOPAlgo_DataMapOfPaveBlockBndBox& thePBBox,
                                            Standard_Real&                    theFirst,
                                            Standard_Real&                    theLast,
                                            Standard_Real&                    theSFirst,
                                            Standard_Real&                    theSLast,
                                            Bnd_Box&                          theBox);

  //! Treatment of the possible common zones, not detected by the
  //! Face/Face intersection algorithm, by intersection of each section edge
  //! with all faces not participated in creation of that section edge.
  //! If the intersection says that the section edge is lying on the face
  //! it will be added into FaceInfo structure of the face as IN edge
  //! and will be used for splitting.
  Standard_EXPORT void PutSEInOtherFaces(const Message_ProgressRange& theRange);

  //! Analyzes the results of interferences of sub-shapes of the shapes
  //! looking for self-interfering entities by the following rules:<br>
  //! 1. The Faces of the same shape considered interfering in case they:<br>
  //!    - Interfere with the other shapes in the same place (in the same vertex) or;<br>
  //!    - Included in the same common block.
  //! 2. The Faces of the same shape considered interfering in case they
  //!    share the IN or SECTION edges.<br>
  //! In case self-interference is found the warning is added.
  Standard_EXPORT void CheckSelfInterference();

  //! Adds the warning about failed intersection of pair of sub-shapes
  Standard_EXPORT void AddIntersectionFailedWarning(const TopoDS_Shape& theS1,
                                                    const TopoDS_Shape& theS2);

  //! Repeat intersection of sub-shapes with increased vertices.
  Standard_EXPORT void RepeatIntersection(const Message_ProgressRange& theRange);

  //! Updates vertices of CommonBlocks with real tolerance of CB.
  Standard_EXPORT void UpdateVerticesOfCB();

  //! The method looks for the additional common blocks among pairs of edges
  //! with the same bounding vertices.
  Standard_EXPORT void ForceInterfEE(const Message_ProgressRange& theRange);

  //! The method looks for the additional edge/face common blocks
  //! among pairs of edge/face having the same vertices.
  Standard_EXPORT void ForceInterfEF(const Message_ProgressRange& theRange);

  //! Performs intersection of given pave blocks
  //! with all faces from arguments.
  Standard_EXPORT void ForceInterfEF(const BOPDS_IndexedMapOfPaveBlock& theMPB,
                                     const Message_ProgressRange&       theRange,
                                     const Standard_Boolean             theAddInterf);

  //! When all section edges are created and no increase of the tolerance
  //! of vertices put on the section edges is expected, make sure that
  //! the created sections have valid range.
  //! If any of the section edges do not have valid range, remove them
  //! from Face/Face intersection info and from the input <theMSCPB> map.
  //! Put such edges into <MicroPB> map for further unification of their
  //! vertices in the PostTreatFF method.
  //!
  //! All these section edges have already been checked to have valid range.
  //! Current check is necessary for the edges whose vertices have also
  //! been put on other section edges with greater tolerance, which has caused
  //! increase of the tolerance value of the vertices.
  Standard_EXPORT void RemoveMicroSectionEdges(
    BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
    BOPDS_IndexedMapOfPaveBlock&                   theMicroPB);

  //! Check all edges on the micro status and remove the positive ones
  Standard_EXPORT void RemoveMicroEdges();

  //! Auxiliary structure to hold the edge distance to the face
  struct EdgeRangeDistance
  {
    Standard_Real First;
    Standard_Real Last;
    Standard_Real Distance;

    EdgeRangeDistance(const Standard_Real theFirst    = 0.0,
                      const Standard_Real theLast     = 0.0,
                      const Standard_Real theDistance = RealLast())
        : First(theFirst),
          Last(theLast),
          Distance(theDistance)
    {
    }
  };

protected: //! Analyzing Progress steps
  //! Filling steps for constant operations
  Standard_EXPORT void fillPIConstants(const Standard_Real theWhole,
                                       BOPAlgo_PISteps&    theSteps) const Standard_OVERRIDE;
  //! Filling steps for all other operations
  Standard_EXPORT void fillPISteps(BOPAlgo_PISteps& theSteps) const Standard_OVERRIDE;

protected: //! Fields
  TopTools_ListOfShape     myArguments;
  BOPDS_PDS                myDS;
  BOPDS_PIterator          myIterator;
  Handle(IntTools_Context) myContext;
  BOPAlgo_SectionAttribute mySectionAttribute;
  Standard_Boolean         myNonDestructive;
  Standard_Boolean         myIsPrimary;
  Standard_Boolean         myAvoidBuildPCurve;
  BOPAlgo_GlueEnum         myGlue;

  // clang-format off
  BOPAlgo_DataMapOfIntegerMapOfPaveBlock myFPBDone; //!< Fence map of intersected faces and pave blocks
  TColStd_MapOfInteger myIncreasedSS; //!< Sub-shapes with increased tolerance during the operation
  TColStd_MapOfInteger myVertsToAvoidExtension; //!< Vertices located close to E/E or E/F intersection points
                                                //! which has already been extended to cover the real intersection
                                                //! points, and should not be extended any longer to be put
                                                //! on a section curve.
  
  NCollection_DataMap <BOPDS_Pair,
                       NCollection_List<EdgeRangeDistance>>
                                            myDistances; //!< Map to store minimal distances between shapes
                                                         //!  which have no real intersections
  // clang-format on
};

#endif // _BOPAlgo_PaveFiller_HeaderFile
