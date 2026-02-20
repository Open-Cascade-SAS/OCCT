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
#include <NCollection_DataMap.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_PIterator.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPTools_BoxTree.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntTools_ShrunkRange.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>

#include <utility>

class IntTools_Context;
class BOPDS_PaveBlock;
class gp_Pnt;
class BOPDS_Curve;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Face;

//!
//! The class represents the Intersection phase of the
//! Boolean Operations algorithm.
//! It performs the pairwise intersection of the sub-shapes of
//! the arguments in the following order:
//! 1. Vertex/Vertex;
//! 2. Vertex/Edge;
//! 3. Edge/Edge;
//! 4. Vertex/Face;
//! 5. Edge/Face;
//! 6. Face/Face.
//!
//! The results of intersection are stored into the Data Structure
//! of the algorithm.
//!
//! Additionally to the options provided by the parent class,
//! the algorithm has the following options:
//! - *Section attributes* - allows to customize the intersection of the faces
//!                          (avoid approximation or building 2d curves);
//! - *Safe processing mode* - allows to avoid modification of the input
//!                            shapes during the operation (by default it is off);
//! - *Gluing options* - allows to speed up the calculation on the special
//!                      cases, in which some sub-shapes are coincide.
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
//!                      perform the operation;
//! - *BOPAlgo_AlertIntersectionFailed* - in case some unexpected error occurred;
//! - *BOPAlgo_AlertNullInputShapes* - in case some of the arguments are null shapes.
//!
class BOPAlgo_PaveFiller : public BOPAlgo_Algo
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BOPAlgo_PaveFiller();

  Standard_EXPORT ~BOPAlgo_PaveFiller() override;

  Standard_EXPORT BOPAlgo_PaveFiller(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  Standard_EXPORT const BOPDS_DS& DS();

  Standard_EXPORT BOPDS_PDS PDS();

  Standard_EXPORT const BOPDS_PIterator& Iterator();

  //! Sets the arguments for operation
  void SetArguments(const NCollection_List<TopoDS_Shape>& theLS) { myArguments = theLS; }

  //! Sets the arguments for operation (move semantics)
  void SetArguments(NCollection_List<TopoDS_Shape>&& theLS) { myArguments = std::move(theLS); }

  //! Adds the argument for operation
  void AddArgument(const TopoDS_Shape& theShape) { myArguments.Append(theShape); }

  //! Returns the list of arguments
  const NCollection_List<TopoDS_Shape>& Arguments() const { return myArguments; }

  Standard_EXPORT const occ::handle<IntTools_Context>& Context();

  Standard_EXPORT void SetSectionAttribute(const BOPAlgo_SectionAttribute& theSecAttr);

  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT void SetNonDestructive(const bool theFlag);

  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT bool NonDestructive() const;

  Standard_EXPORT void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) override;

  //! Sets the glue option for the algorithm
  Standard_EXPORT void SetGlue(const BOPAlgo_GlueEnum theGlue);

  //! Returns the glue option of the algorithm
  Standard_EXPORT BOPAlgo_GlueEnum Glue() const;

  //! Sets the flag to avoid building of p-curves of edges on faces
  void SetAvoidBuildPCurve(const bool theValue) { myAvoidBuildPCurve = theValue; }

  //! Returns the flag to avoid building of p-curves of edges on faces
  bool IsAvoidBuildPCurve() const { return myAvoidBuildPCurve; }

protected:
  typedef NCollection_DataMap<occ::handle<BOPDS_PaveBlock>, Bnd_Box>
    BOPAlgo_DataMapOfPaveBlockBndBox;

  typedef NCollection_DataMap<occ::handle<BOPDS_PaveBlock>, NCollection_List<int>>
    BOPAlgo_DataMapOfPaveBlockListOfInteger;

  typedef NCollection_DataMap<int, NCollection_Map<occ::handle<BOPDS_PaveBlock>>>
    BOPAlgo_DataMapOfIntegerMapOfPaveBlock;

  //! Sets non-destructive mode automatically if an argument
  //! contains a locked sub-shape (see TopoDS_Shape::Locked()).
  Standard_EXPORT void SetNonDestructive();

  Standard_EXPORT void SetIsPrimary(const bool theFlag);

  Standard_EXPORT bool IsPrimary() const;

  Standard_EXPORT virtual void PerformInternal(const Message_ProgressRange& theRange);

  Standard_EXPORT void Clear() override;

  Standard_EXPORT virtual void Init(const Message_ProgressRange& theRange);

  Standard_EXPORT void Prepare(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformVV(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformVE(const Message_ProgressRange& theRange);

  //! Performs the intersection of the vertices with edges.
  Standard_EXPORT void IntersectVE(
    const NCollection_IndexedDataMap<occ::handle<BOPDS_PaveBlock>, NCollection_List<int>>&
                                 theVEPairs,
    const Message_ProgressRange& theRange,
    const bool                   bAddInterfs = true);

  //! Splits the Pave Blocks of the given edges with the extra paves.
  //! The method also builds the shrunk data for the new pave blocks and
  //! in case there is no valid range on the pave block, the vertices of
  //! this pave block will be united making SD vertex.
  //! Parameter <theAddInterfs> defines whether this interference will be added
  //! into common table of interferences or not.
  //! If some of the Pave Blocks are forming the Common Blocks, the splits
  //! of the Pave Blocks will also form a Common Block.
  Standard_EXPORT void SplitPaveBlocks(const NCollection_Map<int>& theMEdges,
                                       const bool                  theAddInterfs);

  Standard_EXPORT virtual void PerformVF(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformEE(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformEF(const Message_ProgressRange& theRange);

  Standard_EXPORT virtual void PerformFF(const Message_ProgressRange& theRange);

  Standard_EXPORT void TreatVerticesEE();

  Standard_EXPORT void MakeSDVerticesFF(
    const NCollection_DataMap<int, NCollection_List<int>>& aDMVLV,
    NCollection_DataMap<int, int>&                         theDMNewSD);

  Standard_EXPORT void MakeSplitEdges(const Message_ProgressRange& theRange);

  Standard_EXPORT void MakeBlocks(const Message_ProgressRange& theRange);

  Standard_EXPORT void MakePCurves(const Message_ProgressRange& theRange);

  Standard_EXPORT int MakeSDVertices(const NCollection_List<int>& theVertIndices,
                                     const bool                   theAddInterfs = true);

  Standard_EXPORT void ProcessDE(const Message_ProgressRange& theRange);

  Standard_EXPORT void FillShrunkData(occ::handle<BOPDS_PaveBlock>& thePB);

  Standard_EXPORT void FillShrunkData(const TopAbs_ShapeEnum theType1,
                                      const TopAbs_ShapeEnum theType2);

  //! Analyzes the results of computation of the valid range for the
  //! pave block and in case of error adds the warning status, otherwise
  //! saves the valid range in the pave block.
  Standard_EXPORT void AnalyzeShrunkData(const occ::handle<BOPDS_PaveBlock>& thePB,
                                         const IntTools_ShrunkRange&         theSR);

  //! Performs intersection of new vertices, obtained in E/E and E/F intersections
  Standard_EXPORT void PerformNewVertices(
    NCollection_IndexedDataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>&
                                                  theMVCPB,
    const occ::handle<NCollection_BaseAllocator>& theAllocator,
    const Message_ProgressRange&                  theRange,
    const bool                                    theIsEEIntersection = true);

  Standard_EXPORT bool CheckFacePaves(const TopoDS_Vertex&        theVnew,
                                      const NCollection_Map<int>& theMIF);

  Standard_EXPORT static bool CheckFacePaves(const int                   theN,
                                             const NCollection_Map<int>& theMIFOn,
                                             const NCollection_Map<int>& theMIFIn);

  Standard_EXPORT bool IsExistingVertex(const gp_Pnt&               theP,
                                        const double                theTol,
                                        const NCollection_Map<int>& theMVOn) const;

  //! Checks and puts paves from <theMVOnIn> on the curve <theNC>.
  //! At that, common (from theMVCommon) and not common vertices
  //! are processed differently.
  Standard_EXPORT void PutPavesOnCurve(const NCollection_Map<int>&                      theMVOnIn,
                                       const NCollection_Map<int>&                      theMVCommon,
                                       BOPDS_Curve&                                     theNC,
                                       const NCollection_Map<int>&                      theMI,
                                       const NCollection_Map<int>&                      theMVEF,
                                       NCollection_DataMap<int, double>&                theMVTol,
                                       NCollection_DataMap<int, NCollection_List<int>>& theDMVLV);

  Standard_EXPORT void FilterPavesOnCurves(const NCollection_Vector<BOPDS_Curve>& theVNC,
                                           NCollection_DataMap<int, double>&      theMVTol);

  //! Depending on the parameter aType it checks whether
  //! the vertex nV was created in EE or EF intersections.
  //! If so, it increases aTolVExt from tolerance value of vertex to
  //! the max distance from vertex nV to the ends of the range of common part.
  //! Possible values of aType:
  //! 1 - checks only EE;
  //! 2 - checks only EF;
  //! other - checks both types of intersections.
  Standard_EXPORT bool ExtendedTolerance(const int                   nV,
                                         const NCollection_Map<int>& aMI,
                                         double&                     aTolVExt,
                                         const int                   aType = 0);

  Standard_EXPORT void PutBoundPaveOnCurve(const TopoDS_Face&     theF1,
                                           const TopoDS_Face&     theF2,
                                           BOPDS_Curve&           theNC,
                                           NCollection_List<int>& theLBV);

  //! Checks if the given pave block (created on section curve)
  //! coincides with any of the pave blocks of the faces
  //! created the section curve.
  Standard_EXPORT bool IsExistingPaveBlock(
    const occ::handle<BOPDS_PaveBlock>&                         thePB,
    const BOPDS_Curve&                                          theNC,
    const double                                                theTolR3D,
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPB,
    BOPTools_BoxTree&                                           thePBTree,
    const NCollection_Map<occ::handle<BOPDS_PaveBlock>>&        theMPBCommon,
    occ::handle<BOPDS_PaveBlock>&                               thePBOut,
    double&                                                     theTolNew);

  //! Checks if the given pave block (created on section curve)
  //! coincides with any of the edges shared between the faces
  //! created the section curve.
  Standard_EXPORT bool IsExistingPaveBlock(const occ::handle<BOPDS_PaveBlock>& thePB,
                                           const BOPDS_Curve&                  theNC,
                                           const NCollection_List<int>&        theLSE,
                                           int&                                theNEOut,
                                           double&                             theTolNew);

  //! Treatment of section edges.
  Standard_EXPORT void PostTreatFF(
    NCollection_IndexedDataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>&
                                                                         theMSCPB,
    NCollection_DataMap<occ::handle<BOPDS_PaveBlock>,
                        NCollection_List<occ::handle<BOPDS_PaveBlock>>>& theDMExEdges,
    NCollection_DataMap<int, int>&                                       theDMNewSD,
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>&          theMicroPB,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertsOnRejectedPB,
    const occ::handle<NCollection_BaseAllocator>&                        theAllocator,
    const Message_ProgressRange&                                         theRange);

  Standard_EXPORT void FindPaveBlocks(const int                                       theV,
                                      const int                                       theF,
                                      NCollection_List<occ::handle<BOPDS_PaveBlock>>& theLPB);

  Standard_EXPORT void FillPaves(const int                                             theV,
                                 const int                                             theE,
                                 const int                                             theF,
                                 const NCollection_List<occ::handle<BOPDS_PaveBlock>>& theLPB,
                                 const occ::handle<BOPDS_PaveBlock>&                   thePB);

  Standard_EXPORT void MakeSplitEdge(const int theV, const int theF);

  Standard_EXPORT void GetEFPnts(const int                          nF1,
                                 const int                          nF2,
                                 NCollection_List<IntSurf_PntOn2S>& aListOfPnts);

  //! Checks and puts paves created in EF intersections on the curve <theNC>.
  Standard_EXPORT void PutEFPavesOnCurve(const NCollection_Vector<BOPDS_Curve>&           theVC,
                                         const int                                        theIndex,
                                         const NCollection_Map<int>&                      theMI,
                                         const NCollection_Map<int>&                      theMVEF,
                                         NCollection_DataMap<int, double>&                theMVTol,
                                         NCollection_DataMap<int, NCollection_List<int>>& aDMVLV);

  //! Puts stick paves on the curve <theNC>
  Standard_EXPORT void PutStickPavesOnCurve(
    const TopoDS_Face&                               aF1,
    const TopoDS_Face&                               aF2,
    const NCollection_Map<int>&                      theMI,
    const NCollection_Vector<BOPDS_Curve>&           theVC,
    const int                                        theIndex,
    const NCollection_Map<int>&                      theMVStick,
    NCollection_DataMap<int, double>&                theMVTol,
    NCollection_DataMap<int, NCollection_List<int>>& aDMVLV);

  //! Collects indices of vertices created in all intersections between
  //! two faces (<nF1> and <nF2>) to the map <theMVStick>.
  //! Also, it collects indices of EF vertices to the <theMVEF> map
  //! and indices of all subshapes of these two faces to the <theMI> map.
  Standard_EXPORT void GetStickVertices(const int             nF1,
                                        const int             nF2,
                                        NCollection_Map<int>& theMVStick,
                                        NCollection_Map<int>& theMVEF,
                                        NCollection_Map<int>& theMI);

  //! Collects index nF and indices of all subshapes of the shape with index <nF>
  //! to the map <theMI>.
  Standard_EXPORT void GetFullShapeMap(const int nF, NCollection_Map<int>& theMI);

  //! Removes indices of vertices that are already on the
  //! curve <theNC> from the map <theMV>.
  //! It is used in PutEFPavesOnCurve and PutStickPavesOnCurve methods.
  Standard_EXPORT void RemoveUsedVertices(const NCollection_Vector<BOPDS_Curve>& theVC,
                                          NCollection_Map<int>&                  theMV);

  //! Puts the pave nV on the curve theNC.
  //! Parameter aType defines whether to check the pave with
  //! extended tolerance:
  //! 0 - do not perform the check;
  //! other - perform the check (aType goes to ExtendedTolerance).
  Standard_EXPORT void PutPaveOnCurve(const int                                        nV,
                                      const double                                     theTolR3D,
                                      const BOPDS_Curve&                               theNC,
                                      const NCollection_Map<int>&                      theMI,
                                      NCollection_DataMap<int, double>&                theMVTol,
                                      NCollection_DataMap<int, NCollection_List<int>>& aDMVLV,
                                      const int                                        aType = 0);

  //! Adds the existing edges for intersection with section edges
  //! by checking the possible intersection with the faces comparing
  //! pre-saved E-F distances with new tolerances.
  Standard_EXPORT void ProcessExistingPaveBlocks(
    const int                                                   theInt,
    const int                                                   theCur,
    const int                                                   nF1,
    const int                                                   nF2,
    const TopoDS_Edge&                                          theES,
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPBOnIn,
    BOPTools_BoxTree&                                           thePBTree,
    NCollection_IndexedDataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>&
                                                                     theMSCPB,
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>& theMVI,
    NCollection_List<occ::handle<BOPDS_PaveBlock>>&                  theLPBC,
    BOPAlgo_DataMapOfPaveBlockListOfInteger&                         thePBFacesMap,
    NCollection_Map<occ::handle<BOPDS_PaveBlock>>&                   theMPB);

  //! Adds the existing edges from the map <theMPBOnIn> which interfere
  //! with the vertices from <theMVB> map to the post treatment of section edges.
  Standard_EXPORT void ProcessExistingPaveBlocks(
    const int                                                   theInt,
    const int                                                   nF1,
    const int                                                   nF2,
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPBOnIn,
    BOPTools_BoxTree&                                           thePBTree,
    const NCollection_DataMap<int, NCollection_List<int>>&      theDMBV,
    NCollection_IndexedDataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>&
                                                                     theMSCPB,
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>& theMVI,
    BOPAlgo_DataMapOfPaveBlockListOfInteger&                         thePBFacesMap,
    NCollection_Map<occ::handle<BOPDS_PaveBlock>>&                   theMPB);

  //! Replaces existing pave block <thePB> with new pave blocks <theLPB>.
  //! The list <theLPB> contains images of <thePB> which were created in
  //! the post treatment of section edges.
  //! Tries to project the new edges on the faces contained in the <thePBFacesMap>.
  Standard_EXPORT void UpdateExistingPaveBlocks(
    const occ::handle<BOPDS_PaveBlock>&             thePB,
    NCollection_List<occ::handle<BOPDS_PaveBlock>>& theLPB,
    const BOPAlgo_DataMapOfPaveBlockListOfInteger&  thePBFacesMap);

  //! Treatment of vertices that were created in EE intersections.
  Standard_EXPORT void TreatNewVertices(
    const NCollection_IndexedDataMap<TopoDS_Shape,
                                     BOPDS_CoupleOfPaveBlocks,
                                     TopTools_ShapeMapHasher>& theMVCPB,
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>&       theImages);

  //! Put paves on the curve <aBC> in case when <aBC>
  //! is closed 3D-curve
  Standard_EXPORT void PutClosingPaveOnCurve(BOPDS_Curve& aNC);

  //! Keeps data for post treatment
  Standard_EXPORT void PreparePostTreatFF(
    const int                           aInt,
    const int                           aCur,
    const occ::handle<BOPDS_PaveBlock>& aPB,
    NCollection_IndexedDataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>&
                                                                     aMSCPB,
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>& aMVI,
    NCollection_List<occ::handle<BOPDS_PaveBlock>>&                  aLPB);

  //! Updates the information about faces
  Standard_EXPORT void UpdateFaceInfo(
    NCollection_DataMap<occ::handle<BOPDS_PaveBlock>,
                        NCollection_List<occ::handle<BOPDS_PaveBlock>>>& theDME,
    const NCollection_DataMap<int, int>&                                 theDMV,
    const BOPAlgo_DataMapOfPaveBlockListOfInteger&                       thePBFacesMap);

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with edge.
  //! Returns TRUE if intersection happened.
  Standard_EXPORT bool ForceInterfVE(const int                     nV,
                                     occ::handle<BOPDS_PaveBlock>& aPB,
                                     NCollection_Map<int>&         theMEdges);

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with face with index <nF>
  Standard_EXPORT bool ForceInterfVF(const int nV, const int nF);

  //! Checks if there are any common or intersecting sub shapes
  //! between two planar faces.
  Standard_EXPORT bool CheckPlanes(const int nF1, const int nF2) const;

  //! Creates new edge from the edge nE with vertices nV1 and nV2
  //! and returns the index of that new edge in the DS.
  Standard_EXPORT int SplitEdge(const int    nE,
                                const int    nV1,
                                const double aT1,
                                const int    nV2,
                                const double aT2);

  //! Updates pave blocks which have the paves with indices contained
  //! in the map <aDMNewSD>.
  Standard_EXPORT void UpdatePaveBlocks(const NCollection_DataMap<int, int>& aDMNewSD);

  //! Updates tolerance vertex nV due to V/E interference.
  //! It always creates new vertex if nV is from arguments.
  //! @return  DS index of updated vertex.
  Standard_EXPORT int UpdateVertex(const int nV, const double aTolNew);

  Standard_EXPORT void UpdatePaveBlocksWithSDVertices();

  Standard_EXPORT void UpdateCommonBlocksWithSDVertices();

  Standard_EXPORT void UpdateBlocksWithSharedVertices();

  Standard_EXPORT void UpdateInterfsWithSDVertices();

  Standard_EXPORT bool EstimatePaveOnCurve(const int          nV,
                                           const BOPDS_Curve& theNC,
                                           const double       theTolR3D);

  Standard_EXPORT void UpdateEdgeTolerance(const int nE, const double aTolNew);

  Standard_EXPORT void RemovePaveBlocks(const NCollection_Map<int>& theEdges);

  Standard_EXPORT void CorrectToleranceOfSE();

  //! Reduce the intersection range using the common ranges of
  //! Edge/Edge interferences to avoid creation of close
  //! intersection vertices
  Standard_EXPORT void ReduceIntersectionRange(const int theV1,
                                               const int theV2,
                                               const int theE,
                                               const int theF,
                                               double&   theTS1,
                                               double&   theTS2);

  //! Gets the bounding box for the given Pave Block.
  //! If Pave Block has shrunk data it will be used to get the box,
  //! and the Shrunk Range (<theSFirst>, <theSLast>).
  //! Otherwise the box will be computed using BndLib_Add3dCurve method,
  //! and the Shrunk Range will be equal to the PB's range.
  //! To avoid re-computation of the bounding box for the same Pave Block
  //! it will be saved in the map <thePBBox>.
  //! Returns FALSE in case the PB's range is less than the
  //! Precision::PConfusion(), otherwise returns TRUE.
  Standard_EXPORT bool GetPBBox(const TopoDS_Edge&                  theE,
                                const occ::handle<BOPDS_PaveBlock>& thePB,
                                BOPAlgo_DataMapOfPaveBlockBndBox&   thePBBox,
                                double&                             theFirst,
                                double&                             theLast,
                                double&                             theSFirst,
                                double&                             theSLast,
                                Bnd_Box&                            theBox);

  //! Treatment of the possible common zones, not detected by the
  //! Face/Face intersection algorithm, by intersection of each section edge
  //! with all faces not participated in creation of that section edge.
  //! If the intersection says that the section edge is lying on the face
  //! it will be added into FaceInfo structure of the face as IN edge
  //! and will be used for splitting.
  Standard_EXPORT void PutSEInOtherFaces(const Message_ProgressRange& theRange);

  //! Analyzes the results of interferences of sub-shapes of the shapes
  //! looking for self-interfering entities by the following rules:
  //! 1. The Faces of the same shape considered interfering in case they:
  //!    - Interfere with the other shapes in the same place (in the same vertex) or;
  //!    - Included in the same common block.
  //! 2. The Faces of the same shape considered interfering in case they
  //!    share the IN or SECTION edges.
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
  Standard_EXPORT void ForceInterfEF(
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPB,
    const Message_ProgressRange&                                theRange,
    const bool                                                  theAddInterf);

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
    NCollection_IndexedDataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>&
                                                          theMSCPB,
    NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMicroPB);

  //! Check all edges on the micro status and remove the positive ones
  Standard_EXPORT void RemoveMicroEdges();

  //! Auxiliary structure to hold the edge distance to the face
  struct EdgeRangeDistance
  {
    double First;
    double Last;
    double Distance;

    EdgeRangeDistance(const double theFirst    = 0.0,
                      const double theLast     = 0.0,
                      const double theDistance = RealLast())
        : First(theFirst),
          Last(theLast),
          Distance(theDistance)
    {
    }
  };

protected: //! Analyzing Progress steps
  //! Filling steps for constant operations
  Standard_EXPORT void fillPIConstants(const double     theWhole,
                                       BOPAlgo_PISteps& theSteps) const override;
  //! Filling steps for all other operations
  Standard_EXPORT void fillPISteps(BOPAlgo_PISteps& theSteps) const override;

protected: //! Fields
  NCollection_List<TopoDS_Shape> myArguments;
  BOPDS_PDS                      myDS;
  BOPDS_PIterator                myIterator;
  occ::handle<IntTools_Context>  myContext;
  BOPAlgo_SectionAttribute       mySectionAttribute;
  bool                           myNonDestructive;
  bool                           myIsPrimary;
  bool                           myAvoidBuildPCurve;
  BOPAlgo_GlueEnum               myGlue;

  // clang-format off
  BOPAlgo_DataMapOfIntegerMapOfPaveBlock myFPBDone; //!< Fence map of intersected faces and pave blocks
  NCollection_Map<int> myIncreasedSS; //!< Sub-shapes with increased tolerance during the operation
  NCollection_Map<int> myVertsToAvoidExtension; //!< Vertices located close to E/E or E/F intersection points
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
