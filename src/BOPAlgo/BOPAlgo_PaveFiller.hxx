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

#include <BOPCol_ListOfShape.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_PIterator.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <Standard_Real.hxx>
#include <BOPAlgo_Algo.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
#include <BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks.hxx>
#include <Standard_Boolean.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPCol_DataMapOfIntegerReal.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPCol_DataMapOfIntegerInteger.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <IntSurf_ListOfPntOn2S.hxx>
#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_IndexedDataMapOfPaveBlockListOfInteger.hxx>
#include <BOPCol_IndexedDataMapOfShapeInteger.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPAlgo_GlueEnum.hxx>
#include <IntTools_ShrunkRange.hxx>
class IntTools_Context;
class BOPDS_DS;
class BOPAlgo_SectionAttribute;
class BOPDS_PaveBlock;
class BOPDS_CommonBlock;
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
//! The algorithm returns the following Warning statuses:<br>
//! - *BOPAlgo_AlertSelfInterferingShape* - in case some of the argument shapes are self-interfering shapes;<br>
//! - *BOPAlgo_AlertTooSmallEdge* - in case some edges of the input shapes have no valid range;<br>
//! - *BOPAlgo_AlertNotSplittableEdge* - in case some edges of the input shapes has such a small
//!                         valid range so it cannot be split;<br>
//! - *BOPAlgo_AlertBadPositioning* - in case the positioning of the input shapes leads to creation
//!                      of small edges.<br>
//!
//! The algorithm returns the following Error alerts:
//! - *BOPAlgo_AlertTooFewArguments* - in case there are no enough arguments to
//!                      perform the operation;<br>
//! - *BOPAlgo_AlertIntersectionFailed* - in case some unexpected error occurred;<br>
//! - *BOPAlgo_AlertNullInputShapes* - in case some of the arguments are null shapes.<br>
//!
class BOPAlgo_PaveFiller  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_PaveFiller();

  Standard_EXPORT virtual ~BOPAlgo_PaveFiller();
  
  Standard_EXPORT BOPAlgo_PaveFiller(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT const BOPDS_DS& DS();
  
  Standard_EXPORT BOPDS_PDS PDS();
  
  Standard_EXPORT const BOPDS_PIterator& Iterator();
  
  Standard_EXPORT void SetArguments (const BOPCol_ListOfShape& theLS);
  
  Standard_EXPORT const BOPCol_ListOfShape& Arguments() const;
  
  Standard_EXPORT const Handle(IntTools_Context)& Context();
  
  Standard_EXPORT void SetSectionAttribute (const BOPAlgo_SectionAttribute& theSecAttr);
  
  //! Sets the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT void SetNonDestructive(const Standard_Boolean theFlag);
  
  //! Returns the flag that defines the mode of treatment.
  //! In non-destructive mode the argument shapes are not modified. Instead
  //! a copy of a sub-shape is created in the result if it is needed to be updated.
  Standard_EXPORT Standard_Boolean NonDestructive() const;

  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  

  //! Sets the glue option for the algorithm
  Standard_EXPORT void SetGlue(const BOPAlgo_GlueEnum theGlue);
  
  //! Returns the glue option of the algorithm
  Standard_EXPORT BOPAlgo_GlueEnum Glue() const;

  //! Sets the flag to avoid building of p-curves of edges on faces
  void SetAvoidBuildPCurve(const Standard_Boolean theValue)
  {
    myAvoidBuildPCurve = theValue;
  }

  //! Returns the flag to avoid building of p-curves of edges on faces
  Standard_Boolean IsAvoidBuildPCurve() const
  {
    return myAvoidBuildPCurve;
  }

protected:

  typedef NCollection_DataMap
            <Handle(BOPDS_PaveBlock),
             Bnd_Box,
             TColStd_MapTransientHasher> BOPAlgo_DataMapOfPaveBlockBndBox;


  //! Sets non-destructive mode automatically if an argument 
  //! contains a locked sub-shape (see TopoDS_Shape::Locked()).
  Standard_EXPORT void SetNonDestructive();
     
  Standard_EXPORT void SetIsPrimary(const Standard_Boolean theFlag);
   
  Standard_EXPORT Standard_Boolean IsPrimary() const;

  Standard_EXPORT virtual void PerformInternal();
  
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Init();
  
  Standard_EXPORT void Prepare();
  
  Standard_EXPORT virtual void PerformVV();
  
  Standard_EXPORT virtual void PerformVE();

  //! Performs the intersection of the vertices with edges.
  Standard_EXPORT void IntersectVE(const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theVEPairs,
                                   const Standard_Boolean bAddInterfs = Standard_True);

  //! Splits the Pave Blocks of the given edges with the extra paves.<br>
  //! The method also builds the shrunk data for the new pave blocks and
  //! in case there is no valid range on the pave block, the vertices of
  //! this pave block will be united making SD vertex.<br>
  //! Parameter <theAddInterfs> defines whether this interference will be added
  //! into common table of interferences or not.<br>
  //! If some of the Pave Blocks are forming the Common Blocks, the splits
  //! of the Pave Blocks will also form a Common Block.
  Standard_EXPORT void SplitPaveBlocks(const BOPCol_MapOfInteger& theMEdges,
                                       const Standard_Boolean theAddInterfs);
  
  Standard_EXPORT virtual void PerformVF();
  
  Standard_EXPORT virtual void PerformEE();
  
  Standard_EXPORT virtual void PerformEF();
  
  Standard_EXPORT virtual void PerformFF();
  
  Standard_EXPORT void TreatVerticesEE();
  
  Standard_EXPORT void MakeSDVerticesFF(const BOPCol_DataMapOfIntegerListOfInteger& aDMVLV,
                                        BOPCol_DataMapOfIntegerInteger& theDMNewSD);

  Standard_EXPORT void MakeSplitEdges();
  
  Standard_EXPORT void MakeBlocks();
  
  Standard_EXPORT void MakePCurves();

  Standard_EXPORT Standard_Integer MakeSDVertices(const BOPCol_ListOfInteger& theVertIndices,
                                                  const Standard_Boolean theAddInterfs = 1);
  
  Standard_EXPORT void ProcessDE();
  
  Standard_EXPORT void FillShrunkData (Handle(BOPDS_PaveBlock)& thePB);
  
  Standard_EXPORT void FillShrunkData (const TopAbs_ShapeEnum theType1, const TopAbs_ShapeEnum theType2);

  //! Analyzes the results of computation of the valid range for the
  //! pave block and in case of error adds the warning status, otherwise
  //! saves the valid range in the pave block.
  Standard_EXPORT void AnalyzeShrunkData(const Handle(BOPDS_PaveBlock)& thePB,
                                         const IntTools_ShrunkRange& theSR);

  //! Performs intersection of new vertices, obtained in E/E and E/F intersections
  Standard_EXPORT void PerformNewVertices(BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB,
                                          const BOPCol_BaseAllocator& theAllocator,
                                          const Standard_Boolean theIsEEIntersection = Standard_True);
  
  Standard_EXPORT Standard_Boolean CheckFacePaves (const TopoDS_Vertex& theVnew, const BOPCol_MapOfInteger& theMIF);
  
  Standard_EXPORT static Standard_Boolean CheckFacePaves (const Standard_Integer theN, const BOPCol_MapOfInteger& theMIFOn, const BOPCol_MapOfInteger& theMIFIn);
  
  Standard_EXPORT Standard_Boolean IsExistingVertex (const gp_Pnt& theP, const Standard_Real theTol, const BOPCol_MapOfInteger& theMVOn) const;
  

  //! Checks and puts paves from <theMVOn> on the curve <theNC>.
  Standard_EXPORT void PutPavesOnCurve (const BOPCol_MapOfInteger& theMVOn, 
                                BOPDS_Curve& theNC, 
                                const Standard_Integer nF1, 
                                const Standard_Integer nF2, 
                                const BOPCol_MapOfInteger& theMI, 
                                const BOPCol_MapOfInteger& theMVEF, 
                                BOPCol_DataMapOfIntegerReal& theMVTol,
                                BOPCol_DataMapOfIntegerListOfInteger& aDMVLV);

  Standard_EXPORT void FilterPavesOnCurves(const BOPDS_VectorOfCurve& theVNC);

  //! Depending on the parameter aType it checks whether
  //! the vertex nV was created in EE or EF intersections.
  //! If so, it increases aTolVExt from tolerance value of vertex to
  //! the max distance from vertex nV to the ends of the range of common part.
  //! Possible values of aType:
  //! 1 - checks only EE;
  //! 2 - checks only EF;
  //! other - checks both types of intersections.
  Standard_EXPORT Standard_Boolean ExtendedTolerance (const Standard_Integer nV, const BOPCol_MapOfInteger& aMI, Standard_Real& aTolVExt, const Standard_Integer aType = 0);
  
  Standard_EXPORT void PutBoundPaveOnCurve(const TopoDS_Face& theF1,
                                           const TopoDS_Face& theF2,
                                           BOPDS_Curve& theNC,
                                           BOPCol_ListOfInteger& theLBV);
  
  Standard_EXPORT Standard_Boolean IsExistingPaveBlock
    (const Handle(BOPDS_PaveBlock)& thePB, const BOPDS_Curve& theNC,
     const Standard_Real theTolR3D, const BOPDS_IndexedMapOfPaveBlock& theMPB,
     const BOPDS_MapOfPaveBlock& theMPBCommon,
     Handle(BOPDS_PaveBlock)& thePBOut, Standard_Real& theTolNew);
  
  Standard_EXPORT Standard_Boolean IsExistingPaveBlock (const Handle(BOPDS_PaveBlock)& thePB, const BOPDS_Curve& theNC, const BOPCol_ListOfInteger& theLSE);
  

  //! Treatment of section edges.
  Standard_EXPORT void PostTreatFF (BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB,
                                    BOPDS_DataMapOfPaveBlockListOfPaveBlock& theDMExEdges,
                                    BOPCol_DataMapOfIntegerInteger& theDMNewSD,
                                    const BOPCol_IndexedMapOfShape& theMicroEdges,
                                    const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT void FindPaveBlocks (const Standard_Integer theV, const Standard_Integer theF, BOPDS_ListOfPaveBlock& theLPB);
  
  Standard_EXPORT void FillPaves (const Standard_Integer theV, const Standard_Integer theE, const Standard_Integer theF, const BOPDS_ListOfPaveBlock& theLPB, const Handle(BOPDS_PaveBlock)& thePB);
  
  Standard_EXPORT void MakeSplitEdge (const Standard_Integer theV, const Standard_Integer theF);
  
  Standard_EXPORT void GetEFPnts (const Standard_Integer nF1, const Standard_Integer nF2, IntSurf_ListOfPntOn2S& aListOfPnts);
  

  //! Checks and puts paves created in EF intersections on the curve <theNC>.
  Standard_EXPORT void PutEFPavesOnCurve (BOPDS_Curve& theNC, 
                                const BOPCol_MapOfInteger& theMI, 
                                const BOPCol_MapOfInteger& theMVEF, 
                                BOPCol_DataMapOfIntegerReal& theMVTol,
                                BOPCol_DataMapOfIntegerListOfInteger& aDMVLV);
  

  //! Puts stick paves on the curve <theNC>
  Standard_EXPORT void PutStickPavesOnCurve (const TopoDS_Face& aF1, 
                                const TopoDS_Face& aF2, 
                                const BOPCol_MapOfInteger& theMI, 
                                BOPDS_Curve& theNC, 
                                const BOPCol_MapOfInteger& theMVStick, 
                                BOPCol_DataMapOfIntegerReal& theMVTol,
                                BOPCol_DataMapOfIntegerListOfInteger& aDMVLV);
  

  //! Collects indices of vertices created in all intersections between
  //! two faces (<nF1> and <nF2>) to the map <theMVStick>.
  //! Also, it collects indices of EF vertices to the <theMVEF> map
  //! and indices of all subshapes of these two faces to the <theMI> map.
  Standard_EXPORT void GetStickVertices (const Standard_Integer nF1, const Standard_Integer nF2, BOPCol_MapOfInteger& theMVStick, BOPCol_MapOfInteger& theMVEF, BOPCol_MapOfInteger& theMI);
  

  //! Collects index nF and indices of all subshapes of the shape with index <nF>
  //! to the map <theMI>.
  Standard_EXPORT void GetFullShapeMap (const Standard_Integer nF, BOPCol_MapOfInteger& theMI);
  

  //! Removes indices of vertices that are already on the
  //! curve <theNC> from the map <theMV>.
  //! It is used in PutEFPavesOnCurve and PutStickPavesOnCurve methods.
  Standard_EXPORT void RemoveUsedVertices (BOPDS_Curve& theNC, BOPCol_MapOfInteger& theMV);
  

  //! Puts the pave nV on the curve theNC.
  //! Parameter aType defines whether to check the pave with
  //! extended tolerance:
  //! 0 - do not perform the check;
  //! other - perform the check (aType goes to ExtendedTolerance).
  Standard_EXPORT void PutPaveOnCurve (const Standard_Integer nV, 
                                const Standard_Real theTolR3D, 
                                const BOPDS_Curve& theNC, 
                                const BOPCol_MapOfInteger& theMI, 
                                BOPCol_DataMapOfIntegerReal& theMVTol,
                                BOPCol_DataMapOfIntegerListOfInteger& aDMVLV,
                                const Standard_Integer aType = 0);
  

  //! Adds the existing edges from the map <theMPBOnIn> which interfere
  //! with the vertices from <theMVB> map to the post treatment of section edges.
  Standard_EXPORT void ProcessExistingPaveBlocks (const Standard_Integer theInt, const BOPDS_IndexedMapOfPaveBlock& theMPBOnIn, const BOPCol_DataMapOfIntegerListOfInteger& theDMBV, BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMSCPB, BOPCol_DataMapOfShapeInteger& theMVI, BOPDS_MapOfPaveBlock& theMPB);
  

  //! Replaces existing pave block <thePB> with new pave blocks <theLPB>.
  //! The list <theLPB> contains images of <thePB> which were created in
  //! the post treatment of section edges.
  Standard_EXPORT void UpdateExistingPaveBlocks (const Handle(BOPDS_PaveBlock)& thePB, BOPDS_ListOfPaveBlock& theLPB, const Standard_Integer nF1, const Standard_Integer nF2);
  

  //! Treatment of vertices that were created in EE intersections.
  Standard_EXPORT void TreatNewVertices(const BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& theMVCPB, BOPCol_IndexedDataMapOfShapeListOfShape& theImages);
  

  //! Put paves on the curve <aBC> in case when <aBC>
  //! is closed 3D-curve
  Standard_EXPORT void PutClosingPaveOnCurve (BOPDS_Curve& aNC);
  

  //! Keeps data for post treatment
  Standard_EXPORT void PreparePostTreatFF (const Standard_Integer aInt, const Standard_Integer aCur, const Handle(BOPDS_PaveBlock)& aPB, BOPDS_IndexedDataMapOfShapeCoupleOfPaveBlocks& aMSCPB, BOPCol_DataMapOfShapeInteger& aMVI, BOPDS_ListOfPaveBlock& aLPB);

  //! Updates the information about faces
  Standard_EXPORT void UpdateFaceInfo (BOPDS_DataMapOfPaveBlockListOfPaveBlock& theDME, const BOPCol_DataMapOfIntegerInteger& theDMV);
  

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with edge
  Standard_EXPORT void ForceInterfVE(const Standard_Integer nV,
                                     Handle(BOPDS_PaveBlock)& aPB,
                                     BOPCol_MapOfInteger& theMEdges);

  //! Updates tolerance of vertex with index <nV>
  //! to make it interfere with face with index <nF>
  Standard_EXPORT Standard_Boolean ForceInterfVF (const Standard_Integer nV, const Standard_Integer nF);
  

  //! Checks if there are any common or intersecting sub shapes
  //! between two planar faces.
  Standard_EXPORT Standard_Boolean CheckPlanes (const Standard_Integer nF1, const Standard_Integer nF2) const;
  

  //! Creates new edge from the edge nE with vertices nV1 and nV2
  //! and returns the index of that new edge in the DS.
  Standard_EXPORT Standard_Integer SplitEdge (const Standard_Integer nE, const Standard_Integer nV1, const Standard_Real aT1, const Standard_Integer nV2, const Standard_Real aT2);
  

  //! Updates pave blocks which have the paves with indices contained
  //! in the map <aDMNewSD>.
  Standard_EXPORT void UpdatePaveBlocks(const BOPCol_DataMapOfIntegerInteger& aDMNewSD);

  //! Updates tolerance vertex nV due to V/E interference.
  //! It always creates new vertex if nV is from arguments.
  //! @return  DS index of updated vertex.
  Standard_EXPORT Standard_Integer UpdateVertex(const Standard_Integer nV,
                                                const Standard_Real aTolNew);
   
  Standard_EXPORT void UpdatePaveBlocksWithSDVertices();

  Standard_EXPORT void UpdateCommonBlocksWithSDVertices();
   
  Standard_EXPORT void UpdateBlocksWithSharedVertices();

  Standard_EXPORT void UpdateInterfsWithSDVertices();

  Standard_EXPORT Standard_Boolean EstimatePaveOnCurve(const Standard_Integer nV,
                                                       const BOPDS_Curve& theNC,
                                                       const Standard_Real theTolR3D);
      
  Standard_EXPORT void UpdateEdgeTolerance(const Standard_Integer nE,
                                           const Standard_Real aTolNew);

  Standard_EXPORT void RemovePaveBlocks(const BOPCol_MapOfInteger theEdges);

  Standard_EXPORT void CorrectToleranceOfSE();

  //! Reduce the intersection range using the common ranges of
  //! Edge/Edge interferences to avoid creation of close
  //! intersection vertices
  Standard_EXPORT void ReduceIntersectionRange(const Standard_Integer theV1,
                                               const Standard_Integer theV2,
                                               const Standard_Integer theE,
                                               const Standard_Integer theF,
                                               Standard_Real& theTS1,
                                               Standard_Real& theTS2);

  //! Gets the bounding box for the given Pave Block.
  //! If Pave Block has shrunk data it will be used to get the box,
  //! and the Shrunk Range (<theSFirst>, <theSLast>).
  //! Otherwise the box will be computed using BndLib_Add3dCurve method,
  //! and the Shrunk Range will be equal to the PB's range.
  //! To avoid re-computation of the bounding box for the same Pave Block
  //! it will be saved in the map <thePBBox>.
  //! Returns FALSE in case the PB's range is less than the
  //! Precision::PConfusion(), otherwise returns TRUE.
  Standard_EXPORT Standard_Boolean GetPBBox(const TopoDS_Edge& theE,
                                            const Handle(BOPDS_PaveBlock)& thePB,
                                            BOPAlgo_DataMapOfPaveBlockBndBox& thePBBox,
                                            Standard_Real& theFirst,
                                            Standard_Real& theLast,
                                            Standard_Real& theSFirst,
                                            Standard_Real& theSLast,
                                            Bnd_Box& theBox);

  //! Treatment of the possible common zones, not detected by the
  //! Face/Face intersection algorithm, by intersection of each section edge
  //! with all faces not participated in creation of that section edge.
  //! If the intersection says that the section edge is lying on the face
  //! it will be added into FaceInfo structure of the face as IN edge
  //! and will be used for splitting.
  Standard_EXPORT void PutSEInOtherFaces();

  //! Analyzes the results of interferences of sub-shapes of the shapes
  //! looking for self-interfering entities by the following rules:<br>
  //! 1. The Faces of the same shape considered interfering in case they:<br>
  //!    - Interfere with the other shapes in the same place (in the same vertex) or;<br>
  //!    - Included in the same common block.
  //! 2. The Faces of the same shape considered interfering in case they
  //!    share the IN or SECTION edges.<br>
  //! In case self-interference is found the warning is added.
  Standard_EXPORT void CheckSelfInterference();


  BOPCol_ListOfShape myArguments;
  BOPDS_PDS myDS;
  BOPDS_PIterator myIterator;
  Handle(IntTools_Context) myContext;
  BOPAlgo_SectionAttribute mySectionAttribute;
  Standard_Boolean myNonDestructive;
  Standard_Boolean myIsPrimary;
  Standard_Boolean myAvoidBuildPCurve;
  BOPAlgo_GlueEnum myGlue;


private:

};

#endif // _BOPAlgo_PaveFiller_HeaderFile
