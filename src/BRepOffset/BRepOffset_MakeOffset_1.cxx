// Created on: 2016
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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


// This is the implementation of the extension of the 3D offset algorithm
// to work in mode Complete and Join Type Intersection.
// Currently only the Planar cases are supported.


#include <BRepOffset_MakeOffset.hxx>

#include <Precision.hxx>
#include <TopoDS.hxx>

#include <BRepAlgo_AsDes.hxx>
#include <BRepAlgo_Image.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepTools.hxx>

#include <BRepAdaptor_Curve.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_DataMapOfShapeInteger.hxx>

#include <BRepOffset_Tool.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#include <BOPDS_DS.hxx>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_Section.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <BOPAlgo_BuilderFace.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>

#include <IntTools_Context.hxx>
#include <IntTools_ShrunkRange.hxx>

typedef NCollection_DataMap
  <TopoDS_Shape, TopTools_MapOfShape, TopTools_ShapeMapHasher> BRepOffset_DataMapOfShapeMapOfShape;

static
  void IntersectTrimmedEdges(const TopTools_ListOfShape& theLF,
                             const Handle(BRepAlgo_AsDes)& theAsDes,
                             TopTools_DataMapOfShapeListOfShape& theOEImages,
                             TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                             TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                             Handle(IntTools_Context)& theCtx,
                             TopTools_MapOfShape& theNewEdges,
                             TopTools_DataMapOfShapeShape& theETrimEInf);

static
  Standard_Boolean GetEdges(const TopoDS_Face& theFace,
                            const Handle(BRepAlgo_AsDes)& theAsDes,
                            const TopTools_DataMapOfShapeListOfShape& theEImages,
                            const TopTools_MapOfShape& theLastInvEdges,
                            const TopTools_IndexedMapOfShape& theInvEdges,
                            Handle(IntTools_Context)& theCtx,
                            const TopTools_MapOfShape& theModifiedEdges,
                            TopoDS_Shape& theEdges,
                            TopTools_IndexedMapOfShape& theInv);

static
  void BuildSplitsOfTrimmedFace(const TopoDS_Face& theFace,
                                const TopoDS_Shape& theEdges,
                                TopTools_ListOfShape& theLFImages);

static
  void BuildSplitsOfFace(const TopoDS_Face& theFace,
                         const TopoDS_Shape& theEdges,
                         TopTools_DataMapOfShapeShape& theOrigins,
                         TopTools_ListOfShape& theLFImages);

static
  void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                          const TopTools_MapOfShape& theModifiedEdges,
                          const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                          Handle(BRepAlgo_AsDes)& theAsDes,
                          TopTools_DataMapOfShapeShape& theFacesOrigins,
                          TopTools_DataMapOfShapeListOfShape& theOEImages,
                          TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                          TopTools_MapOfShape& theLastInvEdges,
                          TopTools_IndexedMapOfShape& theEdgesToAvoid,
                          TopTools_IndexedMapOfShape& theInvEdges,
                          TopTools_IndexedMapOfShape& theValidEdges,
                          TopTools_MapOfShape& theInvertedEdges,
                          TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                          TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          TopTools_DataMapOfShapeShape& theArtInvFaces,
                          TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                          TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                          TopoDS_Shape& theSolids,
                          TopTools_DataMapOfShapeListOfShape& theSSInterfs);

static 
  void BuildSplitsOfInvFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild, 
                             const TopTools_MapOfShape& theModifiedEdges,
                             TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                             TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                             TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                             TopTools_DataMapOfShapeShape& theFacesOrigins,
                             TopTools_DataMapOfShapeListOfShape& theOEImages,
                             TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                             TopTools_MapOfShape& theLastInvEdges,
                             TopTools_IndexedMapOfShape& theEdgesToAvoid,
                             TopTools_MapOfShape& theVertsToAvoid,
                             TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                             TopTools_IndexedMapOfShape& theValidEdges,
                             TopTools_DataMapOfShapeShape& theETrimEInf,
                             Handle(BRepAlgo_AsDes)& theAsDes);

static 
  Standard_Boolean CheckIfArtificial(const TopoDS_Shape& theF,
                                     const TopTools_ListOfShape& theLFImages,
                                     const TopoDS_Shape& theCE,
                                     const TopTools_IndexedMapOfShape& theMapEInv,
                                     const TopTools_DataMapOfShapeListOfShape& theOEImages,
                                     TopTools_MapOfShape& theMENInv,
                                     Handle(BRepAlgo_AsDes)& theAsDes);

static
  void FindInvalidEdges(const TopoDS_Face& theF,
                        const TopTools_ListOfShape& theLFImages,
                        const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        const TopTools_DataMapOfShapeShape& theFacesOrigins,
                        const TopTools_DataMapOfShapeListOfShape& theOEImages,
                        const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        TopTools_IndexedMapOfShape& theInvEdges,
                        TopTools_IndexedMapOfShape& theValidEdges,
                        TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                        TopTools_DataMapOfShapeListOfShape& theDMFLNE,
                        TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                        TopTools_DataMapOfShapeListOfShape& theDMFLVIE,
                        TopTools_DataMapOfShapeListOfShape& theDMEOrLEIm,
                        TopTools_MapOfShape& theMEInverted,
                        TopTools_MapOfShape& theEdgesInvalidByVertex);

static
  void FindInvalidFaces(TopTools_ListOfShape& theLFImages,
                        const TopTools_IndexedMapOfShape& theInvEdges,
                        const TopTools_IndexedMapOfShape& theValidEdges,
                        const TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                        const TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                        const TopTools_ListOfShape& theLENeutral,
                        const TopTools_ListOfShape& theLEValInverted,
                        const TopTools_MapOfShape& theMEInverted,
                        const TopTools_MapOfShape& theEdgesInvalidByVertex,
                        const TopTools_MapOfShape& theMFHoles,
                        TopTools_IndexedMapOfShape& theMFInvInHole,
                        TopTools_ListOfShape& theInvFaces);

static
  void FindFacesInsideHoleWires(const TopoDS_Face& theFOrigin,
                                const TopoDS_Face& theFOffset,
                                const TopTools_ListOfShape& theLFImages,
                                const TopTools_MapOfShape& theInvertedEdges,
                                const TopTools_DataMapOfShapeListOfShape& theDMEOrLEIm,
                                TopTools_MapOfShape& theMFHoles,
                                TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                                Handle(IntTools_Context)& theContext);

static
  gp_Vec GetAverageTangent(const TopoDS_Shape& theS,
                           const Standard_Integer theNbP);

static
  Standard_Boolean CheckInverted(const TopoDS_Edge& theEIm,
                                 const TopoDS_Face& theFOr,
                                 const TopTools_DataMapOfShapeListOfShape& theOEImages,
                                 const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                 const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                 const TopTools_IndexedDataMapOfShapeListOfShape& theDMVE,
                                 const TopTools_IndexedMapOfShape& theMEdges,
                                 TopTools_MapOfShape& theMEInverted);

static
  Standard_Boolean CheckInvertedBlock(const TopoDS_Shape& theCB,
                                      const TopTools_ListOfShape& theLCBF,
                                      const TopTools_MapOfShape& theMEInverted,
                                      const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                      BRepOffset_DataMapOfShapeMapOfShape& theDMCBVInverted,
                                      BRepOffset_DataMapOfShapeMapOfShape& theDMCBVAll);

static
  void GetVerticesOnEdges(const TopoDS_Shape& theCB,
                          const TopTools_MapOfShape& theEdges,
                          TopTools_MapOfShape& theVerticesOnEdges,
                          TopTools_MapOfShape& theAllVertices);

static
  void RemoveInvalidSplitsByInvertedEdges(const TopTools_MapOfShape& theMEInverted,
                                          const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                          TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                          TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                          TopTools_IndexedMapOfShape& theMERemoved);

static
  void RemoveInvalidSplitsFromValid(const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                    const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                    const TopTools_MapOfShape& theMEInverted,
                                    TopTools_IndexedDataMapOfShapeListOfShape& theFImages);

static
  void RemoveInsideFaces(TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                         TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                         const TopTools_DataMapOfShapeShape& theArtInvFaces,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         const TopTools_IndexedMapOfShape& theMFToCheckInt,
                         const TopTools_IndexedMapOfShape& theMFInvInHole,
                         const TopoDS_Shape& theFHoles,
                         TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                         TopTools_IndexedMapOfShape& theMERemoved,
                         TopTools_IndexedMapOfShape& theMEInside,
                         TopoDS_Shape& theSolids);

static
  void ShapesConnections(const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         const TopTools_DataMapOfShapeShape& theDMFOr,
                         BOPAlgo_Builder& theBuilder,
                         TopTools_DataMapOfShapeListOfShape& theSSInterfs);

static
  void RemoveValidSplits(const TopTools_MapOfShape& theSpRem,
                         TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                         BOPAlgo_Builder& theGF,
                         TopTools_IndexedMapOfShape& theMERemoved);

static
  void RemoveInvalidSplits(const TopTools_MapOfShape& theSpRem,
                           const TopTools_DataMapOfShapeShape& theArtInvFaces,
                           const TopTools_IndexedMapOfShape& theInvEdges,
                           TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                           BOPAlgo_Builder& theGF,
                           TopTools_IndexedMapOfShape& theMERemoved);

static
  void FilterEdgesImages(const TopoDS_Shape& theS,
                         TopTools_DataMapOfShapeListOfShape& theOEImages,
                         TopTools_DataMapOfShapeListOfShape& theOEOrigins);

static
  void FilterInvalidFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                          const TopTools_IndexedDataMapOfShapeListOfShape& theDMEF,
                          const TopTools_IndexedMapOfShape& theMERemoved,
                          TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          TopTools_DataMapOfShapeShape& theArtInvFaces);

static
  void FilterInvalidEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          const TopTools_DataMapOfShapeShape& theArtInvFaces,
                          const TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                          const TopTools_IndexedMapOfShape& theMERemoved,
                          TopTools_IndexedMapOfShape& theInvEdges);

static 
  void FindFacesToRebuild(const TopTools_IndexedDataMapOfShapeListOfShape&  theLFImages,
                          const TopTools_IndexedMapOfShape& theInvEdges,
                          const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                          const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                          TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                          TopTools_MapOfShape& theFSelfRebAvoid);

static
  void RebuildFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                    const TopTools_MapOfShape& theFSelfRebAvoid,
                    const TopoDS_Shape& theSolids,
                    const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                    TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                    TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                    TopTools_DataMapOfShapeShape& theFacesOrigins,
                    TopTools_DataMapOfShapeListOfShape& theOEImages,
                    TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                    TopTools_MapOfShape& theLastInvEdges,
                    TopTools_IndexedMapOfShape& theEdgesToAvoid,
                    TopTools_IndexedMapOfShape& theInvEdges,
                    TopTools_IndexedMapOfShape& theValidEdges,
                    const TopTools_MapOfShape& theInvertedEdges,
                    TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                    TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                    const TopTools_DataMapOfShapeShape& theArtInvFaces,
                    TopTools_MapOfShape& theVertsToAvoid,
                    TopTools_DataMapOfShapeShape& theETrimEInf,
                    Handle(BRepAlgo_AsDes)& theAsDes);

static
  void IntersectFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                      const TopTools_MapOfShape& theFSelfRebAvoid,
                      const TopoDS_Shape& theSolids,
                      const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                      TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                      TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      TopTools_DataMapOfShapeListOfShape& theOEImages,
                      TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                      TopTools_IndexedMapOfShape& theInvEdges,
                      TopTools_IndexedMapOfShape& theValidEdges,
                      const TopTools_MapOfShape& theInvertedEdges,
                      TopTools_IndexedMapOfShape& theEdgesToAvoid,
                      TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                      const TopTools_DataMapOfShapeShape& theArtInvFaces,
                      TopTools_MapOfShape& theVertsToAvoid,
                      TopTools_DataMapOfShapeShape& theETrimEInf,
                      TopTools_MapOfShape& theModifiedEdges,
                      Handle(BRepAlgo_AsDes)& theAsDes);

static
  void PrepareFacesForIntersection(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                                   const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                   const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                   const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                   const Standard_Boolean bLookVertToAvoid,
                                   TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                                   TopTools_DataMapOfShapeListOfShape& theMDone,
                                   TopTools_DataMapOfShapeListOfShape& theDMSF,
                                   TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                                   TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                                   TopTools_DataMapOfShapeShape& theETrimEInf,
                                   TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv);

static
  void FindVerticesToAvoid(const TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv,
                           const TopTools_IndexedMapOfShape& theInvEdges,
                           const TopTools_IndexedMapOfShape& theValidEdges,
                           TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                           TopTools_MapOfShape& theMVRInv);

static
  void FindFacesForIntersection(const TopoDS_Shape& theFInv,
                                const TopTools_IndexedMapOfShape& theME,
                                const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                const TopTools_DataMapOfShapeListOfShape& theDMSF,
                                const TopTools_MapOfShape& theMVInvAll,
                                const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                const Standard_Boolean theArtCase,
                                const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                                TopTools_IndexedMapOfShape& theMFAvoid,
                                TopTools_IndexedMapOfShape& theMFInt,
                                TopTools_IndexedMapOfShape& theMFIntExt,
                                TopTools_ListOfShape& theLFImInt);

static
  void ProcessCommonEdges(const TopTools_ListOfShape& theLEC,
                          const TopTools_IndexedMapOfShape& theInvEdges,
                          const TopTools_IndexedMapOfShape& theValidEdges,
                          const TopTools_IndexedMapOfShape& theME,
                          const TopTools_DataMapOfShapeShape& theETrimEInf,
                          const TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                          const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                          const TopTools_MapOfShape& theAllInvs,
                          const Standard_Boolean theForceUse,
                          TopTools_IndexedMapOfShape& theMECV,
                          TopTools_MapOfShape& theMECheckExt,
                          TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                          TopTools_ListOfShape& theLFEi,
                          TopTools_ListOfShape& theLFEj,
                          TopTools_IndexedMapOfShape& theMEToInt);

static
  void UpdateIntersectedFaces(const TopoDS_Shape& theFInv,
                              const TopoDS_Shape& theFi,
                              const TopoDS_Shape& theFj,
                              const TopTools_ListOfShape& theLFInv,
                              const TopTools_ListOfShape& theLFImi,
                              const TopTools_ListOfShape& theLFImj,
                              const TopTools_ListOfShape& theLFEi,
                              const TopTools_ListOfShape& theLFEj,
                              TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                              TopTools_IndexedMapOfShape& theMEToInt);

static
  void IntersectFaces(const TopoDS_Shape& theFInv,
                      const TopoDS_Shape& theFi,
                      const TopoDS_Shape& theFj,
                      const TopTools_ListOfShape& theLFInv,
                      const TopTools_ListOfShape& theLFImi,
                      const TopTools_ListOfShape& theLFImj,
                      TopTools_ListOfShape& theLFEi,
                      TopTools_ListOfShape& theLFEj,
                      TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      TopTools_IndexedMapOfShape& theMECV,
                      TopTools_IndexedMapOfShape& theMEToInt);

static 
  void FindOrigins(const TopTools_ListOfShape& theLFIm1,
                   const TopTools_ListOfShape& theLFIm2,
                   const TopTools_IndexedMapOfShape& theME,
                   const TopTools_DataMapOfShapeListOfShape& theOrigins,
                   TopTools_ListOfShape& theLEOr);

static
  void IntersectAndTrimEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                             const TopTools_IndexedMapOfShape& theMFInt,
                             const TopTools_IndexedMapOfShape& theMEInt,
                             const TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                             const TopTools_IndexedMapOfShape& theMSInv,
                             const TopTools_IndexedMapOfShape& theMVE,
                             const TopTools_MapOfShape& theVertsToAvoid,
                             const TopTools_MapOfShape& theNewVertsToAvoid,
                             const TopTools_MapOfShape& theMECheckExt,
                             TopTools_MapOfShape& theMVBounds,
                             TopTools_DataMapOfShapeListOfShape& theEImages);

static
  void GetInvalidEdges(const TopTools_MapOfShape& theVertsToAvoid,
                       const TopTools_MapOfShape& theMVBounds,
                       BOPAlgo_Builder& theGF,
                       TopTools_MapOfShape& theMEInv);

static
  void UpdateValidEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                        const TopTools_MapOfShape& theMVBounds,
                        const TopoDS_Shape& theSolids,
                        const TopTools_IndexedMapOfShape& theInvEdges,
                        const TopTools_MapOfShape& theInvertedEdges,
                        const TopTools_MapOfShape& theMEInvOnArt,
                        TopTools_MapOfShape& theMECheckExt,
                        TopTools_IndexedMapOfShape& theEdgesToAvoid,
                        TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        TopTools_DataMapOfShapeListOfShape& theOEImages,
                        TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        TopTools_MapOfShape& theVertsToAvoid,
                        TopTools_DataMapOfShapeShape& theETrimEInf,
                        TopTools_DataMapOfShapeListOfShape& theEImages,
                        TopTools_DataMapOfShapeListOfShape& theEETrim,
                        TopTools_MapOfShape& theModifiedEdges,
                        Handle(BRepAlgo_AsDes)& theAsDes);

static
  void TrimNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                                const TopTools_DataMapOfShapeListOfShape& theEETrim,
                                const TopTools_MapOfShape& theMVBounds,
                                TopTools_MapOfShape& theMECheckExt,
                                TopTools_DataMapOfShapeListOfShape& theEImages,
                                TopTools_MapOfShape& theMEB,
                                TopTools_MapOfShape& theMVOld,
                                TopTools_ListOfShape& theLENew,
                                BOPCol_ListOfShape& theLA,
                                TopTools_DataMapOfShapeListOfShape& theDMEOr,
                                TopTools_DataMapOfShapeListOfShape& theMELF);

static
  void IntersectEdges(const BOPCol_ListOfShape& theLA,
                      const TopTools_ListOfShape& theLE,
                      const TopTools_ListOfShape& theLENew,
                      const TopTools_MapOfShape& theMVBounds,
                      const TopTools_MapOfShape& theVertsToAvoid,
                      TopTools_MapOfShape& theMECheckExt,
                      TopTools_DataMapOfShapeListOfShape& theEImages,
                      TopTools_MapOfShape& theModifiedEdges,
                      TopTools_DataMapOfShapeListOfShape& theDMEOr,
                      TopTools_DataMapOfShapeListOfShape& theMELF,
                      TopTools_MapOfShape& theMENew,
                      TopoDS_Shape& theSplits);

static
  void GetBounds(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                 const TopTools_MapOfShape& theMEB,
                 TopoDS_Shape& theBounds);

static
  void GetBoundsToUpdate(const TopTools_ListOfShape& theLF,
                         const TopTools_DataMapOfShapeListOfShape& theOEImages,
                         const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                         const TopTools_MapOfShape& theMEB,
                         TopTools_ListOfShape& theLABounds,
                         TopTools_ListOfShape& theLAValid,
                         TopoDS_Shape& theBounds,
                         Handle(BRepAlgo_AsDes)& theAsDes);

static
  void GetInvalidEdgesByBounds(const TopoDS_Shape& theSplits,
                               const TopoDS_Shape& theBounds,
                               const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                               const TopoDS_Shape& theSolids,
                               const TopTools_IndexedMapOfShape& theInvEdges,
                               const TopTools_MapOfShape& theMVOld,
                               const TopTools_MapOfShape& theMENew,
                               const TopTools_DataMapOfShapeListOfShape& theDMEOr,
                               const TopTools_DataMapOfShapeListOfShape& theMELF,
                               const TopTools_DataMapOfShapeListOfShape& theEImages,
                               const TopTools_MapOfShape& theMECheckExt,
                               const TopTools_MapOfShape& theMEInvOnArt,
                               TopTools_MapOfShape& theVertsToAvoid,
                               TopTools_MapOfShape& theMEInv);

static
  void UpdateNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                                  const TopTools_DataMapOfShapeListOfShape& theMELF,
                                  const TopTools_DataMapOfShapeListOfShape& theEImages,
                                  const TopTools_IndexedMapOfShape& theInvEdges,
                                  const TopTools_MapOfShape& theInvertedEdges,
                                  TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                  TopTools_DataMapOfShapeListOfShape& theOEImages,
                                  TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                  TopTools_DataMapOfShapeShape& theETrimEInf,
                                  TopTools_DataMapOfShapeListOfShape& theEETrim,
                                  TopTools_MapOfShape& theModifiedEdges,
                                  Handle(BRepAlgo_AsDes)& theAsDes);

static
  void FillHistory(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                   const TopTools_DataMapOfShapeListOfShape& theEImages,
                   BRepAlgo_Image& theImage);

static
  void UpdateOrigins(const TopTools_ListOfShape& theLA,
                     TopTools_DataMapOfShapeListOfShape& theOrigins,
                     BOPAlgo_Builder& theGF);

static
  void UpdateImages(const TopTools_ListOfShape& theLA,
                    TopTools_DataMapOfShapeListOfShape& theImages,
                    BOPAlgo_Builder& theGF,
                    TopTools_MapOfShape& theModified);

static
  void UpdateIntersectedEdges(const TopTools_ListOfShape& theLA,
                              TopTools_DataMapOfShapeShape& theETrimEInf,
                              BOPAlgo_Builder& theGF);

static
  Standard_Boolean ProcessMicroEdge(const TopoDS_Edge& theEdge,
                                    const Handle(IntTools_Context)& theCtx);

static
  void FindCommonParts(const TopTools_ListOfShape& theLS1,
                       const TopTools_ListOfShape& theLS2,
                       TopTools_ListOfShape& theLSC,
                       const TopAbs_ShapeEnum theType = TopAbs_EDGE);

static
  Standard_Integer NbPoints(const TopoDS_Edge& theE);

static
  Standard_Boolean FindShape(const TopoDS_Shape& theSWhat,
                             const TopoDS_Shape& theSWhere,
                             TopoDS_Shape& theRes);

static
  void AppendToList(TopTools_ListOfShape& theL,
                    const TopoDS_Shape& theS);

//=======================================================================
//function : BuildSplitsOfTrimmedFaces
//purpose  : Building splits of already trimmed faces
//=======================================================================
void BRepOffset_MakeOffset::BuildSplitsOfTrimmedFaces(const TopTools_ListOfShape& theLF,
                                                      Handle(BRepAlgo_AsDes)& theAsDes,
                                                      BRepAlgo_Image& theImage)
{
  TopTools_DataMapOfShapeListOfShape anEImages, anEOrigins;
  TopTools_IndexedDataMapOfShapeListOfShape aDMFFIm;
  TopTools_IndexedMapOfShape anEmptyIM;
  TopTools_DataMapOfShapeListOfShape anEmptyDMSLS;
  TopTools_DataMapOfShapeShape anEmptyDMSS;
  TopTools_MapOfShape aNewEdges, anEmptyM;
  //
  // firstly it is necessary to fuse all the edges
  Handle(IntTools_Context) aCtx = new IntTools_Context();
  //
  IntersectTrimmedEdges(theLF, theAsDes, anEImages, anEOrigins, anEmptyDMSLS, aCtx, aNewEdges, anEmptyDMSS);
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    TopoDS_Shape aCE;
    TopTools_ListOfShape aLFImages;
    //
    Standard_Boolean bFound = GetEdges(aF, theAsDes, anEImages, anEmptyM,
                                       anEmptyIM, aCtx, aNewEdges, aCE, anEmptyIM);
    // split the face by the edges
    if (!bFound) {
      if (!theImage.HasImage(aF)) {
        aLFImages.Append(aF);
        aDMFFIm.Add(aF, aLFImages);
      }
      continue;
    }
    //
    BuildSplitsOfTrimmedFace(aF, aCE, aLFImages);
    aDMFFIm.Add(aF, aLFImages);
  }
  // Fill history for faces and edges
  FillHistory(aDMFFIm, anEImages, theImage);
}

//=======================================================================
//function : BuildSplitsOfExtendedFaces
//purpose  : Building splits of not-trimmed offset faces.
//           For the cases in which invalidity will be found,
//           these invalidities will be rebuilt.
//=======================================================================
void BRepOffset_MakeOffset::BuildSplitsOfExtendedFaces(const TopTools_ListOfShape& theLF,
                                                       Handle(BRepAlgo_AsDes)& theAsDes,
                                                       TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                                       TopTools_DataMapOfShapeShape& theFacesOrigins,
                                                       TopTools_DataMapOfShapeShape& theETrimEInf,
                                                       BRepAlgo_Image& theImage)
{
  Handle(IntTools_Context) aCtx = new IntTools_Context();
  // images and origins for offset edges
  TopTools_DataMapOfShapeListOfShape anOEImages, anOEOrigins;
  TopTools_MapOfShape aNewEdges;
  // fusing all trimmed offset edges to avoid self-intersections in the splits
  IntersectTrimmedEdges(theLF, theAsDes, anOEImages, anOEOrigins,
                        theEdgesOrigins, aCtx, aNewEdges, theETrimEInf);
  //
  // valid/invalid edges
  TopTools_IndexedMapOfShape anInvEdges, aValidEdges, anEdgesToAvoid;
  // inverted edges
  TopTools_MapOfShape anInvertedEdges;
  // splits of faces
  TopTools_IndexedDataMapOfShapeListOfShape aFImages;
  // found invalid faces
  TopTools_IndexedDataMapOfShapeListOfShape anInvFaces;
  // artificially invalid faces - it will be empty here,
  // but may be filled on the following rebuilding steps
  TopTools_DataMapOfShapeShape anArtInvFaces;
  // shapes connections for using in rebuilding
  TopTools_DataMapOfShapeListOfShape aSSInterfs;
  // edges to avoid on second steps
  TopTools_MapOfShape aLastInvEdges;
  // keep information of already invalid faces to avoid
  // infinite rebuilding of the same invalid face
  TopTools_DataMapOfShapeInteger anAlreadyInvFaces;
  // images of the hole faces of the original faces
  TopTools_DataMapOfShapeListOfShape aDMFNewHoles;
  // solid build from the new splits
  TopoDS_Shape aSolids;
  // now we can split the faces
  BuildSplitsOfFaces(theLF, aNewEdges, theEdgesOrigins, theAsDes, theFacesOrigins,
                     anOEImages, anOEOrigins, aLastInvEdges, anEdgesToAvoid, anInvEdges, aValidEdges,
                     anInvertedEdges, anAlreadyInvFaces, anInvFaces, anArtInvFaces, aFImages,
                     aDMFNewHoles, aSolids, aSSInterfs);
  //
  // Find faces to rebuild
  if (anInvFaces.Extent()) {
    TopTools_IndexedDataMapOfShapeListOfShape aFToRebuild;
    TopTools_MapOfShape aFSelfRebAvoid;
    FindFacesToRebuild(aFImages, anInvEdges, anInvFaces, aSSInterfs, aFToRebuild, aFSelfRebAvoid);
    //
    if (aFToRebuild.Extent()) {
      // vertices to avoid
      TopTools_MapOfShape aVAEmpty;
      RebuildFaces(aFToRebuild, aFSelfRebAvoid, aSolids, aSSInterfs, aFImages, aDMFNewHoles,
                   theEdgesOrigins, theFacesOrigins, anOEImages, anOEOrigins, aLastInvEdges,
                   anEdgesToAvoid, anInvEdges, aValidEdges, anInvertedEdges, anAlreadyInvFaces,
                   anInvFaces, anArtInvFaces, aVAEmpty, theETrimEInf, theAsDes);
    }
  }
  // Fill history for faces and edges
  FillHistory(aFImages, anOEImages, theImage);
}

//=======================================================================
//function : BuildSplitsOfInvFaces
//purpose  : Rebuilding splits of faces with new intersection edges
//=======================================================================
void BuildSplitsOfInvFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild, 
                           const TopTools_MapOfShape& theModifiedEdges,
                           TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                           TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                           TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                           TopTools_DataMapOfShapeShape& theFacesOrigins,
                           TopTools_DataMapOfShapeListOfShape& theOEImages,
                           TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                           TopTools_MapOfShape& theLastInvEdges,
                           TopTools_IndexedMapOfShape& theEdgesToAvoid,
                           TopTools_MapOfShape& theVertsToAvoid,
                           TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                           TopTools_IndexedMapOfShape& theValidEdges,
                           TopTools_DataMapOfShapeShape& theETrimEInf, 
                           Handle(BRepAlgo_AsDes)& theAsDes)
{
  Standard_Integer aNb = theFToRebuild.Extent();
  if (!aNb) {
    return;
  }
  //
  TopTools_ListOfShape aLF;
  aNb = theFImages.Extent();
  for (Standard_Integer i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFImages.FindKey(i);
    aLF.Append(aF);
  }
  //
  // invalid faces
  TopTools_IndexedDataMapOfShapeListOfShape anInvFaces;
  // artificially invalid faces
  TopTools_DataMapOfShapeShape anArtInvFaces;
  // invalid edges
  TopTools_IndexedMapOfShape anInvEdges;
  // inverted edges
  TopTools_MapOfShape anInvertedEdges;
  // shapes connection for using in rebuilding process
  TopTools_DataMapOfShapeListOfShape aSSInterfs;
  //
  TopoDS_Shape aSolids;
  //
  BuildSplitsOfFaces(aLF, theModifiedEdges, theEdgesOrigins, theAsDes, theFacesOrigins, 
                     theOEImages, theOEOrigins, theLastInvEdges, theEdgesToAvoid, anInvEdges, theValidEdges, 
                     anInvertedEdges, theAlreadyInvFaces, anInvFaces, anArtInvFaces, theFImages,
                     theDMFNewHoles, aSolids, aSSInterfs);
  //
  if (anInvFaces.Extent()) {
    TopTools_IndexedDataMapOfShapeListOfShape aFToRebuild;
    TopTools_MapOfShape aFSelfRebAvoid;
    FindFacesToRebuild(theFImages, anInvEdges, anInvFaces, aSSInterfs, aFToRebuild, aFSelfRebAvoid);
    //
    if (aFToRebuild.Extent()) {
      RebuildFaces(aFToRebuild, aFSelfRebAvoid, aSolids, aSSInterfs, theFImages, theDMFNewHoles,
                   theEdgesOrigins, theFacesOrigins, theOEImages, theOEOrigins, theLastInvEdges,
                   theEdgesToAvoid, anInvEdges, theValidEdges, anInvertedEdges, theAlreadyInvFaces,
                   anInvFaces, anArtInvFaces, theVertsToAvoid, theETrimEInf, theAsDes);
    }
  }
}

//=======================================================================
//function : BuildSplitsOfFaces
//purpose  : Building the splits of offset faces and
//           looking for the invalid splits
//=======================================================================
void BuildSplitsOfFaces(const TopTools_ListOfShape& theLF,
                        const TopTools_MapOfShape& theModifiedEdges,
                        const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                        Handle(BRepAlgo_AsDes)& theAsDes,
                        TopTools_DataMapOfShapeShape& theFacesOrigins,
                        TopTools_DataMapOfShapeListOfShape& theOEImages,
                        TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        TopTools_MapOfShape& theLastInvEdges,
                        TopTools_IndexedMapOfShape& theEdgesToAvoid,
                        TopTools_IndexedMapOfShape& theInvEdges,
                        TopTools_IndexedMapOfShape& theValidEdges,
                        TopTools_MapOfShape& theInvertedEdges,
                        TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                        TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        TopTools_DataMapOfShapeShape& theArtInvFaces,
                        TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                        TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                        TopoDS_Shape& theSolids,
                        TopTools_DataMapOfShapeListOfShape& theSSInterfs)
{
  if (theLF.IsEmpty()) {
    return;
  }
  //
  BRep_Builder aBB;
  Standard_Integer i, aNb;
  //
  // processed faces
  TopTools_ListOfShape aLFDone;
  // extended face - list of neutral edges, i.e. in one splits - valid and in others - invalid
  TopTools_DataMapOfShapeListOfShape aDMFLNE;
  // list of valid edges for each face
  TopTools_DataMapOfShapeListOfShape aDMFLVE;
  // list of invalid edges for each face
  TopTools_DataMapOfShapeListOfShape aDMFLIE;
  // map of valid inverted edges for the face
  TopTools_DataMapOfShapeListOfShape aDMFLVIE;
  // map of splits to check for internals
  TopTools_IndexedMapOfShape aMFToCheckInt;
  // map of edges created from vertex and marked as invalid
  TopTools_MapOfShape aMEdgeInvalidByVertex;
  // connection map from old edges to new ones
  TopTools_DataMapOfShapeListOfShape aDMEOrLEIm;
  //
  Handle(IntTools_Context) aCtx = new IntTools_Context;
  // build splits of faces
  TopTools_ListIteratorOfListOfShape aItLF(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    TopTools_ListOfShape* pLFIm = theFImages.ChangeSeek(aF);
    if (pLFIm && pLFIm->IsEmpty()) {
      continue;
    }
    // get edges by which the face should be split
    TopoDS_Shape aCE;
    TopTools_IndexedMapOfShape aMapEInv;
    Standard_Boolean bFound =
      GetEdges(aF, theAsDes, theOEImages, theLastInvEdges,
               theEdgesToAvoid, aCtx, theModifiedEdges, aCE, aMapEInv);
    if (!bFound) {
      continue;
    }
    //
    // build splits
    TopTools_ListOfShape aLFImages;
    BuildSplitsOfFace(aF, aCE, theFacesOrigins, aLFImages);
    //
    if (aMapEInv.Extent()) {
      // check if all possible faces are built
      TopTools_MapOfShape aMENInv;
      Standard_Boolean bArtificialCase = aLFImages.IsEmpty() ||
        CheckIfArtificial(aF, aLFImages, aCE, aMapEInv, theOEImages, aMENInv, theAsDes);
      //
      // try to build splits using invalid edges
      TopoDS_Compound aCE1;
      aBB.MakeCompound(aCE1);
      aBB.Add(aCE1, aCE);
      for (i = 1; i <= aMapEInv.Extent(); ++i) {
        aBB.Add(aCE1, aMapEInv(i));
      }
      //
      TopTools_ListOfShape aLFImages1;
      BuildSplitsOfFace(aF, aCE1, theFacesOrigins, aLFImages1);
      //
      // check if the rebuilding has added some new faces to the splits
      for (TopTools_ListIteratorOfListOfShape aItLFIm(aLFImages1); aItLFIm.More();)
      {
        Standard_Boolean bAllInv = Standard_True;
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aE = aExpE.Current();
          if (!aMapEInv.Contains(aE)) {
            bAllInv = Standard_False;
            if (!aMENInv.Contains(aE)) {
              break;
            }
          }
        }
        //
        if (!aExpE.More()) {
          if (bAllInv) {
            aMFToCheckInt.Add(aFIm);
          }
          aLFImages1.Remove(aItLFIm);
        }
        else {
          aItLFIm.Next();
        }
      }
      //
      if (bArtificialCase) {
        if (aLFImages.Extent() == aLFImages1.Extent()) {
          bArtificialCase = Standard_False;
        }
        else {
          aLFImages = aLFImages1;
        }
      }
      //
      if (bArtificialCase) {
        TopTools_ListOfShape aLEInv;
        // make the face invalid
        theArtInvFaces.Bind(aF, aCE);
        //
        *pLFIm = aLFImages;
        TopTools_ListIteratorOfListOfShape aItLFIm(aLFImages);
        for (; aItLFIm.More(); aItLFIm.Next()) {
          const TopoDS_Shape& aFIm = aItLFIm.Value();
          TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
          for (; aExpE.More(); aExpE.Next()) {
            const TopoDS_Shape& aE = aExpE.Current();
            if (aMapEInv.Contains(aE)) {
              theInvEdges.Add(aE);
              AppendToList(aLEInv, aE);
            }
            else {
              theValidEdges.Add(aE);
            }
          }
        }
        //
        aDMFLIE.Bind(aF, aLEInv);
        aLFDone.Append(aF);
        //
        continue;
      }
    }
    //
    // find invalid edges
    FindInvalidEdges(aF, aLFImages, theEdgesOrigins, theFacesOrigins, theOEImages,
                     theOEOrigins, theInvEdges, theValidEdges, aDMFLVE, aDMFLNE, aDMFLIE,
                     aDMFLVIE, aDMEOrLEIm, theInvertedEdges, aMEdgeInvalidByVertex);
    //
    // save the new splits
    if (!pLFIm) {
      pLFIm = &theFImages(theFImages.Add(aF, TopTools_ListOfShape()));
    }
    else {
      pLFIm->Clear();
    }
    pLFIm->Append(aLFImages);
    //
    aLFDone.Append(aF);
  }
  //
  if (theInvEdges.IsEmpty() && theArtInvFaces.IsEmpty()) {
    return;
  }
  //
#ifdef OFFSET_DEBUG
  // show invalid edges
  TopoDS_Compound aCEInv1;
  BRep_Builder().MakeCompound(aCEInv1);
  Standard_Integer aNbEInv = theInvEdges.Extent();
  for (i = 1; i <= aNbEInv; ++i) {
    const TopoDS_Shape& aE = theInvEdges(i);
    BRep_Builder().Add(aCEInv1, aE);
  }
  // show valid edges
  TopoDS_Compound aCEVal1;
  BRep_Builder().MakeCompound(aCEVal1);
  aNbEInv = theValidEdges.Extent();
  for (i = 1; i <= aNbEInv; ++i) {
    const TopoDS_Shape& aE = theValidEdges(i);
    BRep_Builder().Add(aCEVal1, aE);
  }
  // show inverted edges
  TopoDS_Compound aCEInverted;
  BRep_Builder().MakeCompound(aCEInverted);
  TopTools_MapIteratorOfMapOfShape aItM(theInvertedEdges);
  for (; aItM.More(); aItM.Next()) {
    BRep_Builder().Add(aCEInverted, aItM.Value());
  }
#endif
  //
  TopTools_ListOfShape anEmptyList;
  // invalid faces inside the holes
  TopTools_IndexedMapOfShape aMFInvInHole;
  // all hole faces
  TopoDS_Compound aFHoles;
  aBB.MakeCompound(aFHoles);
  // find invalid faces
  // considering faces containing only invalid edges as invalid
  aItLF.Initialize(aLFDone);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = TopoDS::Face(aItLF.Value());
    TopTools_ListOfShape& aLFImages = theFImages.ChangeFromKey(aF);
    //
    TopTools_ListOfShape aLFInv;
    Standard_Boolean bArtificialCase = theArtInvFaces.IsBound(aF);
    if (bArtificialCase) {
      aLFInv = aLFImages;
    }
    else {
      // neutral edges
      TopTools_ListOfShape* pLNE = aDMFLNE.ChangeSeek(aF);
      if (!pLNE) {
        pLNE = &anEmptyList;
      }
      // valid inverted edges
      TopTools_ListOfShape* pLIVE = aDMFLVIE.ChangeSeek(aF);
      if (!pLIVE) {
        pLIVE = &anEmptyList;
      }
      //
      // find faces inside holes wires
      TopTools_MapOfShape aMFHoles;
      const TopoDS_Face& aFOr = TopoDS::Face(theFacesOrigins.Find(aF));
      FindFacesInsideHoleWires(aFOr, aF, aLFImages, theInvertedEdges,
                               aDMEOrLEIm, aMFHoles, theDMFNewHoles, aCtx);
      //
      TopTools_MapIteratorOfMapOfShape aItMH(aMFHoles);
      for (; aItMH.More(); aItMH.Next()) {
        aBB.Add(aFHoles, aItMH.Value());
      }
      //
      // find invalid faces
      FindInvalidFaces(aLFImages, theInvEdges, theValidEdges, aDMFLVE, aDMFLIE,
                       *pLNE, *pLIVE, theInvertedEdges, aMEdgeInvalidByVertex,
                       aMFHoles, aMFInvInHole, aLFInv);
    }
    //
    if (aLFInv.Extent()) {
      if (theAlreadyInvFaces.IsBound(aF)) {
        if (theAlreadyInvFaces.Find(aF) > 2) {
          if (aLFInv.Extent() == aLFImages.Extent() && !bArtificialCase) {
            aLFImages.Clear();
          }
          //
          aLFInv.Clear();
        }
      }
      theInvFaces.Add(aF, aLFInv);
    }
  }
  //
  if (theInvFaces.IsEmpty()) {
    theInvEdges.Clear();
    return;
  }
  //
#ifdef OFFSET_DEBUG
  // show invalid faces
  TopoDS_Compound aCFInv1;
  BRep_Builder().MakeCompound(aCFInv1);
  Standard_Integer aNbFInv = theInvFaces.Extent();
  for (i = 1; i <= aNbFInv; ++i) {
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    TopTools_ListIteratorOfListOfShape aItLFInv(aLFInv);
    for (; aItLFInv.More(); aItLFInv.Next()) {
      const TopoDS_Shape& aFIm = aItLFInv.Value();
      BRep_Builder().Add(aCFInv1, aFIm);
    }
  }
#endif
  //
  TopTools_IndexedMapOfShape aMERemoved;
  // remove invalid splits of faces using inverted edges
  RemoveInvalidSplitsByInvertedEdges(theInvertedEdges, theOEOrigins,
                                     theInvFaces, theFImages, aMERemoved);
  if (theInvFaces.IsEmpty()) {
    theInvEdges.Clear();
    return;
  }
  //
  // remove invalid splits from valid splits
  RemoveInvalidSplitsFromValid(theInvFaces, theArtInvFaces, theInvertedEdges, theFImages);
  //
  // remove inside faces
  TopTools_IndexedMapOfShape aMEInside;
  RemoveInsideFaces(theFImages, theInvFaces, theArtInvFaces, theInvEdges,
                    aMFToCheckInt, aMFInvInHole, aFHoles, theSSInterfs,
                    aMERemoved, aMEInside, theSolids);
  //
  // make compound of valid splits
  TopoDS_Compound aCFIm;
  aBB.MakeCompound(aCFIm);
  //
  aNb = theFImages.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFIm = theFImages(i);
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aBB.Add(aCFIm, aFIm);
    }
  }
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMEF;
  TopExp::MapShapesAndAncestors(aCFIm, TopAbs_EDGE, TopAbs_FACE, aDMEF);
  //
  // filter maps of images and origins
  FilterEdgesImages(aCFIm, theOEImages, theOEOrigins);
  //
  // filter invalid faces
  FilterInvalidFaces(theFImages, aDMEF, aMEInside, theInvFaces, theArtInvFaces);
  aNb = theInvFaces.Extent();
  if (!aNb) {
    theInvEdges.Clear();
    return;
  }
  //
#ifdef OFFSET_DEBUG
  // show invalid faces
  TopoDS_Compound aCFInv;
  BRep_Builder().MakeCompound(aCFInv);
  aNbFInv = theInvFaces.Extent();
  for (i = 1; i <= aNbFInv; ++i) {
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    TopTools_ListIteratorOfListOfShape aItLFInv(aLFInv);
    for (; aItLFInv.More(); aItLFInv.Next()) {
      const TopoDS_Shape& aFIm = aItLFInv.Value();
      BRep_Builder().Add(aCFInv, aFIm);
    }
  }
#endif
  //
  // filter invalid edges
  FilterInvalidEdges(theInvFaces, theArtInvFaces, aDMFLIE, aMERemoved, theInvEdges);
  //
#ifdef OFFSET_DEBUG
  // show invalid edges
  TopoDS_Compound aCEInv;
  BRep_Builder().MakeCompound(aCEInv);
  aNbEInv = theInvEdges.Extent();
  for (i = 1; i <= aNbEInv; ++i) {
    const TopoDS_Shape& aE = theInvEdges(i);
    BRep_Builder().Add(aCEInv, aE);
  }
#endif
  //
  theLastInvEdges.Clear();
  aNb = theInvEdges.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aE = theInvEdges(i);
    theEdgesToAvoid.Add(aE);
    theLastInvEdges.Add(aE);
  }
  //
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    if (theAlreadyInvFaces.IsBound(aF)) {
      theAlreadyInvFaces.ChangeFind(aF)++;
    }
    else {
      theAlreadyInvFaces.Bind(aF, 1);
    }
  }
}

//=======================================================================
//function : IntersectTrimmedEdges
//purpose  : Intersection of the trimmed edges among themselves
//=======================================================================
void IntersectTrimmedEdges(const TopTools_ListOfShape& theLF,
                           const Handle(BRepAlgo_AsDes)& theAsDes,
                           TopTools_DataMapOfShapeListOfShape& theOEImages,
                           TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                           TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                           Handle(IntTools_Context)& theCtx,
                           TopTools_MapOfShape& theNewEdges,
                           TopTools_DataMapOfShapeShape& theETrimEInf)
{
  if (theLF.IsEmpty()) {
    return;
  }
  //
  // get edges to intersect from descendants of the offset faces
  BOPCol_ListOfShape aLS;
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aF = *(TopoDS_Face*)&aItLF.Value();
    //
    const TopTools_ListOfShape& aLE = theAsDes->Descendant(aF);
    TopTools_ListIteratorOfListOfShape aItLE(aLE);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Edge& aE = *(TopoDS_Edge*)&aItLE.Value();
      //
      if (ProcessMicroEdge(aE, theCtx)) {
        continue;
      }
      //
      if (theNewEdges.Add(aE)) {
        aLS.Append(aE);
      }
    }
  }
  //
  if (aLS.Extent() < 2) {
    // nothing to intersect
    return;
  }
  //
  // perform intersection of the edges
  BOPAlgo_Builder aGFE;
  aGFE.SetArguments(aLS);
  aGFE.Perform();
  if (aGFE.HasErrors()) {
    return;
  }
  //
  TopTools_ListOfShape aLA;
  // fill map with edges images
  BOPCol_ListIteratorOfListOfShape aIt(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    const TopTools_ListOfShape& aLEIm = aGFE.Modified(aE);
    if (aLEIm.IsEmpty()) {
      continue;
    }
    //
    aLA.Append(aE);
    // save images
    theOEImages.Bind(aE, aLEIm);
    // save origins
    TopTools_ListIteratorOfListOfShape aItLE(aLEIm);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aEIm = aItLE.Value();
      TopTools_ListOfShape* pLEOr = theOEOrigins.ChangeSeek(aEIm);
      if (!pLEOr) {
        pLEOr = theOEOrigins.Bound(aEIm, TopTools_ListOfShape());
      }
      AppendToList(*pLEOr, aE);
    }
  }
  //
  UpdateOrigins(aLA, theEdgesOrigins, aGFE);
  UpdateIntersectedEdges(aLA, theETrimEInf, aGFE);
}

//=======================================================================
//function : GetEdges
//purpose  : Getting edges from AsDes map to build the splits of faces
//=======================================================================
Standard_Boolean GetEdges(const TopoDS_Face& theFace,
                          const Handle(BRepAlgo_AsDes)& theAsDes,
                          const TopTools_DataMapOfShapeListOfShape& theEImages,
                          const TopTools_MapOfShape& theLastInvEdges,
                          const TopTools_IndexedMapOfShape& theInvEdges,
                          Handle(IntTools_Context)& theCtx,
                          const TopTools_MapOfShape& theModifiedEdges,
                          TopoDS_Shape& theEdges,
                          TopTools_IndexedMapOfShape& theInv)
{
  // get boundary edges
  TopTools_MapOfShape aMFBounds;
  TopExp_Explorer aExp(theFace, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aE = aExp.Current();
    const TopTools_ListOfShape* pLEIm = theEImages.Seek(aE);
    if (!pLEIm) {
      aMFBounds.Add(aE);
    }
    else {
      TopTools_ListIteratorOfListOfShape aItLE(*pLEIm);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aEIm = aItLE.Value();
        aMFBounds.Add(aEIm);
      }
    }
  }
  //
  BRep_Builder aBB;
  Standard_Boolean bFound(Standard_False), bUpdate(Standard_False);
  // the resulting edges
  TopoDS_Compound anEdges;
  aBB.MakeCompound(anEdges);
  //
  // the edges by which the offset face should be split
  const TopTools_ListOfShape& aLE = theAsDes->Descendant(theFace);
  TopTools_ListIteratorOfListOfShape aItLE(aLE);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
    //
    if (!bUpdate) {
      bUpdate = theModifiedEdges.Contains(aE);
    }
    //
    const TopTools_ListOfShape* pLEIm = theEImages.Seek(aE);
    if (pLEIm) {
      TopTools_ListIteratorOfListOfShape aItLEIm(*pLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Edge& aEIm = TopoDS::Edge(aItLEIm.Value());
        //
        if (theInvEdges.Contains(aEIm)) {
          theInv.Add(aEIm);
          if (!bUpdate) {
            bUpdate = theLastInvEdges.Contains(aEIm);
          }
          continue;
        }
        // check for micro edge
        if (ProcessMicroEdge(aEIm, theCtx)) {
          continue;
        }
        //
        aBB.Add(anEdges, aEIm);
        if (!bFound) {
          bFound = !aMFBounds.Contains(aEIm);
        }
        //
        if (!bUpdate) {
          bUpdate = theModifiedEdges.Contains(aEIm);
        }
      }
    }
    else {
      if (theInvEdges.Contains(aE)) {
        theInv.Add(aE);
        if (!bUpdate) {
          bUpdate = theLastInvEdges.Contains(aE);
        }
        continue;
      }
      //
      if (ProcessMicroEdge(aE, theCtx)) {
        continue;
      }
      aBB.Add(anEdges, aE);
      if (!bFound) {
        bFound = !aMFBounds.Contains(aE);
      }
    }
  }
  //
  theEdges = anEdges;
  return bFound && bUpdate;
}

//=======================================================================
//function : BuildSplitsOfFace
//purpose  : Building the splits of offset face
//=======================================================================
void BuildSplitsOfFace(const TopoDS_Face& theFace,
                       const TopoDS_Shape& theEdges,
                       TopTools_DataMapOfShapeShape& theFacesOrigins,
                       TopTools_ListOfShape& theLFImages)
{
  theLFImages.Clear();
  //
  // take edges to split the face
  BOPCol_ListOfShape aLE;
  TopExp_Explorer aExp(theEdges, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    TopoDS_Edge aE = TopoDS::Edge(aExp.Current());
    aE.Orientation(TopAbs_FORWARD);
    aLE.Append(aE);
    aE.Orientation(TopAbs_REVERSED);
    aLE.Append(aE);
  }
  //
  TopoDS_Face aFF = theFace;
  TopAbs_Orientation anOr = theFace.Orientation();
  aFF.Orientation(TopAbs_FORWARD);
  //
  // build pcurves for edges on the face
  BOPTools_AlgoTools2D::BuildPCurveForEdgesOnPlane(aLE, aFF);
  //
  // build splits of faces
  BOPAlgo_BuilderFace aBF;
  aBF.SetFace(aFF);
  aBF.SetShapes(aLE);
  aBF.Perform();
  //
  const BOPCol_ListOfShape& aLFSp = aBF.Areas();
  BOPCol_ListIteratorOfListOfShape aItLF(aLFSp);
  for (; aItLF.More(); aItLF.Next()) {
    TopoDS_Shape& aFSp = aItLF.ChangeValue();
    aFSp.Orientation(anOr);
    theLFImages.Append(aFSp);
    //
    theFacesOrigins.Bind(aFSp, theFace);
  }
}

//=======================================================================
//function : BuildSplitsOfFace
//purpose  : Building the splits of offset face
//=======================================================================
void BuildSplitsOfTrimmedFace(const TopoDS_Face& theFace,
                              const TopoDS_Shape& theEdges,
                              TopTools_ListOfShape& theLFImages)
{
  BOPAlgo_Builder aGF;
  //
  aGF.AddArgument(theFace);
  aGF.AddArgument(theEdges);
  aGF.Perform();
  if (aGF.HasErrors()) {
    return;
  }
  //
  // splits of the offset shape
  theLFImages = aGF.Modified(theFace);
}

//=======================================================================
//function : CheckIfArtificial
//purpose  : Checks if the face is artificially invalid
//=======================================================================
Standard_Boolean CheckIfArtificial(const TopoDS_Shape& theF,
                                   const TopTools_ListOfShape& theLFImages,
                                   const TopoDS_Shape& theCE,
                                   const TopTools_IndexedMapOfShape& theMapEInv,
                                   const TopTools_DataMapOfShapeListOfShape& theOEImages,
                                   TopTools_MapOfShape& theMENInv,
                                   Handle(BRepAlgo_AsDes)& theAsDes)
{
  // all boundary edges should be used
  TopTools_IndexedMapOfShape aMEUsed;
  TopTools_ListIteratorOfListOfShape aItLFIm(theLFImages);
  for (; aItLFIm.More(); aItLFIm.Next()) {
    const TopoDS_Shape& aFIm = aItLFIm.Value();
    TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEUsed);
    TopExp::MapShapes(aFIm, TopAbs_VERTEX, aMEUsed);
  }
  //
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  TopExp::MapShapesAndAncestors(theCE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
  //
  Standard_Integer i, aNb = theMapEInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aEInv = theMapEInv(i);
    TopExp_Explorer aExpV(aEInv, TopAbs_VERTEX);
    for (; aExpV.More(); aExpV.Next()) {
      const TopoDS_Shape& aVEInv = aExpV.Current();
      const TopTools_ListOfShape* pLENInv = aMVE.Seek(aVEInv);
      if (pLENInv) {
        TopTools_ListIteratorOfListOfShape aItLEInv(*pLENInv);
        for (; aItLEInv.More(); aItLEInv.Next()) {
          const TopoDS_Shape& aENInv = aItLEInv.Value();
          if (!aMEUsed.Contains(aENInv)) {
            theMENInv.Add(aENInv);
          }
        }
      }
    }
  }
  //
  if (theMENInv.IsEmpty()) {
    return Standard_False;
  }
  //
  TopTools_IndexedMapOfShape aMEFound;
  TopExp::MapShapes(theCE, TopAbs_EDGE, aMEFound);
  //
  const TopTools_ListOfShape& aLE = theAsDes->Descendant(theF);
  TopTools_ListIteratorOfListOfShape aItLE(aLE);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Edge& aE = TopoDS::Edge(aItLE.Value());
    //
    if (theOEImages.IsBound(aE)) {
      Standard_Boolean bChecked = Standard_False;
      const TopTools_ListOfShape& aLEIm = theOEImages.Find(aE);
      TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Edge& aEIm = TopoDS::Edge(aItLEIm.Value());
        if (!aMEFound.Contains(aEIm) || theMENInv.Contains(aEIm)) {
          continue;
        }
        //
        bChecked = Standard_True;
        if (aMEUsed.Contains(aEIm)) {
          break;
        }
      }
      //
      if (bChecked && !aItLEIm.More()) {
        break;
      }
    }
    else {
      if (aMEFound.Contains(aE) && !theMENInv.Contains(aE) && !aMEUsed.Contains(aE)) {
        break;
      }
    }
  }
  //
  return aItLE.More();
}

//=======================================================================
//function : FindInvalidEdges
//purpose  : Looking for the invalid edges
//=======================================================================
void FindInvalidEdges(const TopoDS_Face& theF,
                      const TopTools_ListOfShape& theLFImages,
                      const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      const TopTools_DataMapOfShapeShape& theFacesOrigins,
                      const TopTools_DataMapOfShapeListOfShape& theOEImages,
                      const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                      TopTools_IndexedMapOfShape& theInvEdges,
                      TopTools_IndexedMapOfShape& theValidEdges,
                      TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                      TopTools_DataMapOfShapeListOfShape& theDMFLNE,
                      TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                      TopTools_DataMapOfShapeListOfShape& theDMFLVIE,
                      TopTools_DataMapOfShapeListOfShape& theDMEOrLEIm,
                      TopTools_MapOfShape& theMEInverted,
                      TopTools_MapOfShape& theEdgesInvalidByVertex)
{
  // Edge is considered as invalid in the following cases:
  // 1. Its orientation on the face has changed comparing to the originals edge and face;
  // 2. The vertices of the edge have changed places comparing to the originals edge and face.
  //
  // The edges created from vertices, i.e. as intersection between two faces connected only
  // by VERTEX, will also be checked on validity. For these edges the correct orientation will
  // be defined by the edges on the original face adjacent to the connection vertex

  // original face
  const TopoDS_Face& aFOr = *(TopoDS_Face*)&theFacesOrigins.Find(theF);
  // invalid edges
  TopTools_IndexedMapOfShape aMEInv;
  // valid edges
  TopTools_MapOfShape aMEVal;
  // internal edges
  TopTools_MapOfShape aMEInt;
  //
  // maps for checking the inverted edges
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE, aDMEF;
  TopTools_IndexedMapOfShape aMEdges;
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLFImages);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aE = aExp.Current();
      // keep all edges
      aMEdges.Add(aE);
      //
      // keep connection from edges to faces
      TopTools_ListOfShape* pLF = aDMEF.ChangeSeek(aE);
      if (!pLF) {
        pLF = &aDMEF(aDMEF.Add(aE, TopTools_ListOfShape()));
      }
      AppendToList(*pLF, aFIm);
      //
      // keep connection from vertices to edges
      TopoDS_Iterator aItV(aE);
      for (; aItV.More(); aItV.Next()) {
        const TopoDS_Shape& aV = aItV.Value();
        //
        TopTools_ListOfShape* pLE = aDMVE.ChangeSeek(aV);
        if (!pLE) {
          pLE = &aDMVE(aDMVE.Add(aV, TopTools_ListOfShape()));
        }
        AppendToList(*pLE, aE);
      }
    }
  }
  //
  // the map will be used to find the edges on the original face
  // adjacent to the same vertex. It will be filled at first necessity;
  TopTools_IndexedDataMapOfShapeListOfShape aDMVEFOr;
  //
  aItLF.Initialize(theLFImages);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    // valid edges for this split
    TopTools_ListOfShape aLVE;
    // invalid edges for this split
    TopTools_ListOfShape aLIE;
    //
    TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aEIm = *(TopoDS_Edge*)&aExp.Current();
      //
      if (aEIm.Orientation() == TopAbs_INTERNAL) {
        aMEInt.Add(aEIm);
        continue;
      }
      //
      if (!theEdgesOrigins.IsBound(aEIm)) {
        continue;
      }
      //
      const TopTools_ListOfShape& aLEOr = theEdgesOrigins.Find(aEIm);
      if (aLEOr.IsEmpty()) {
        continue;
      }
      //
      Standard_Integer aNbVOr = 0;
      TopTools_ListIteratorOfListOfShape aItLEO(aLEOr);
      for (; aItLEO.More(); aItLEO.Next()) {
        if (aItLEO.Value().ShapeType() == TopAbs_VERTEX) {
          ++aNbVOr;
        }
      }
      //
      TopTools_MapOfShape aME, aMV;
      Standard_Boolean bInvalid = Standard_False, bChecked = Standard_False;
      Standard_Integer aNbP = NbPoints(aEIm);
      Standard_Boolean bUseVertex = !aNbVOr ? Standard_False :
        (aNbVOr == 1 &&
         aDMEF.FindFromKey(aEIm).Extent() == 1 &&
         !theOEOrigins.IsBound(aEIm));
      //
      aItLEO.Initialize(aLEOr);
      for (; aItLEO.More(); aItLEO.Next()) {
        const TopoDS_Shape& aSOr = aItLEO.Value();
        Standard_Boolean bVertex = (aSOr.ShapeType() == TopAbs_VERTEX);
        //
        TopoDS_Shape aEOrF;
        if (bVertex) {
          // for some cases it is impossible to check the validity of the edge
          if (!bUseVertex) {
            continue;
          }
          // find edges on the original face adjacent to this vertex
          if (aDMVEFOr.IsEmpty()) {
            // fill the map
            TopExp::MapShapesAndAncestors(aFOr, TopAbs_VERTEX, TopAbs_EDGE, aDMVEFOr);
          }
          //
          TopTools_ListOfShape *pLEFOr = aDMVEFOr.ChangeSeek(aSOr);
          if (pLEFOr) {
            TopoDS_Compound aCEOr;
            BRep_Builder().MakeCompound(aCEOr);
            TopTools_ListIteratorOfListOfShape aItLEFOr(*pLEFOr);
            for (; aItLEFOr.More(); aItLEFOr.Next()) {
              const TopoDS_Shape& aEOr = aItLEFOr.Value();
              BRep_Builder().Add(aCEOr, aEOr);
            }
            aEOrF = aCEOr;
          }
        }
        else {
          FindShape(aSOr, aFOr, aEOrF);
          //
          TopTools_ListOfShape *pLEIm = theDMEOrLEIm.ChangeSeek(aSOr);
          if (!pLEIm) {
            pLEIm = theDMEOrLEIm.Bound(aSOr, TopTools_ListOfShape());
          }
          AppendToList(*pLEIm, aEIm);
        }
        //
        if (aEOrF.IsNull()) {
          // the edge has not been found
          continue;
        }
        //
        // Check orientations of the image edge and original edge.
        // In case the 3d curves are having the same direction the orientations 
        // must be the same. Otherwise the orientations should also be different.
        //
        // get average tangent vector for each curve taking into account
        // the orientations of the edges, i.e. the edge is reversed
        // the vector is reversed as well
        gp_Vec aVSum1 = GetAverageTangent(aEIm, aNbP);
        gp_Vec aVSum2 = GetAverageTangent(aEOrF, aNbP);
        //
        aVSum1.Normalize();
        aVSum2.Normalize();
        //
        Standard_Real aCos = aVSum1.Dot(aVSum2);
        if (!bVertex) {
          if (Abs(aCos) < 0.9999) {
            continue;
          }
          //
          aME.Add(aEOrF);
          TopExp_Explorer aExpE(aEOrF, TopAbs_VERTEX);
          for (; aExpE.More(); aExpE.Next()) {
            const TopoDS_Shape& aV = aExpE.Current();
            aMV.Add(aV);
          }
        }
        //
        if (aCos < Precision::Confusion()) {
          bInvalid = Standard_True;
          if (bVertex) {
            theEdgesInvalidByVertex.Add(aEIm);
          }
        }
        bChecked = Standard_True;
      }
      //
      if (!bChecked) {
        continue;
      }
      //
      Standard_Integer aNbE = aME.Extent(), aNbV = aMV.Extent();
      if ((aNbE > 1) && (aNbV == 2*aNbE)) {
        continue;
      }
      //
      if (bInvalid) {
        theInvEdges.Add(aEIm);
        aLIE.Append(aEIm);
        aMEInv.Add(aEIm);
        continue;
      }
      //
      // check if the edge has been inverted
      Standard_Boolean bInverted = !aNbE ? Standard_False :
        CheckInverted(aEIm, aFOr, theOEImages, theOEOrigins,
          theEdgesOrigins, aDMVE, aMEdges, theMEInverted);
      //
      if (!bInverted || !aNbVOr) {
        theValidEdges.Add(aEIm);
        aLVE.Append(aEIm);
        aMEVal.Add(aEIm);
      }
    }
    //
    // valid edges
    if (aLVE.Extent()) {
      theDMFLVE.Bind(aFIm, aLVE);
    }
    //
    // invalid edges
    if (aLIE.Extent()) {
      theDMFLIE.Bind(aFIm, aLIE);
    }
  }
  //
  // process invalid edges:
  // check for the inverted edges
  TopTools_ListOfShape aLVIE;
  // fill neutral edges
  TopTools_ListOfShape aLNE;
  //
  Standard_Integer i, aNbEInv = aMEInv.Extent();
  for (i = 1; i <= aNbEInv; ++i) {
    const TopoDS_Shape& aEIm = aMEInv(i);
    //
    // neutral edges - on the splits of the same offset face
    // it is valid for one split and invalid for other
    if (aMEVal.Contains(aEIm)) {
      aLNE.Append(aEIm);
      continue;
    }
    //
    // the inverted images of the origins of invalid edges should also be invalid
    if (!theMEInverted.Contains(aEIm)) {
      continue;
    }
    //
    const TopTools_ListOfShape* pLOEOr = theOEOrigins.Seek(aEIm);
    if (!pLOEOr) {
      continue;
    }
    //
    TopTools_ListIteratorOfListOfShape aItLOEOr(*pLOEOr);
    for (; aItLOEOr.More(); aItLOEOr.Next()) {
      const TopoDS_Shape& aOEOr = aItLOEOr.Value();
      const TopTools_ListOfShape& aLEIm1 = theOEImages.Find(aOEOr);
      //
      TopTools_ListIteratorOfListOfShape aItLEIm1(aLEIm1);
      for (; aItLEIm1.More(); aItLEIm1.Next()) {
        const TopoDS_Shape& aEIm1 = aItLEIm1.Value();
        if (aMEdges.Contains(aEIm1) &&
            !aMEInv.Contains(aEIm1) && !aMEInt.Contains(aEIm1) &&
            theMEInverted.Contains(aEIm1)) {
          theInvEdges.Add(aEIm1);
          aLVIE.Append(aEIm1);
        }
      }
    }
  }
  //
  if (aLNE.Extent()) {
    theDMFLNE.Bind(theF, aLNE);
  }
  //
  if (aLVIE.Extent()) {
    theDMFLVIE.Bind(theF, aLVIE);
  }
}

//=======================================================================
//function : FindInvalidFaces
//purpose  : Looking for the invalid faces by analyzing their invalid edges
//=======================================================================
void FindInvalidFaces(TopTools_ListOfShape& theLFImages,
                      const TopTools_IndexedMapOfShape& theInvEdges,
                      const TopTools_IndexedMapOfShape& theValidEdges,
                      const TopTools_DataMapOfShapeListOfShape& theDMFLVE,
                      const TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                      const TopTools_ListOfShape& theLENeutral,
                      const TopTools_ListOfShape& theLEValInverted,
                      const TopTools_MapOfShape& theMEInverted,
                      const TopTools_MapOfShape& theEdgesInvalidByVertex,
                      const TopTools_MapOfShape& theMFHoles,
                      TopTools_IndexedMapOfShape& theMFInvInHole,
                      TopTools_ListOfShape& theInvFaces)
{
  // The face should be considered as invalid in the following cases:
  // 1. It has been reverted, i.e. at least two not connected edges 
  //    have changed orientation (i.e. invalid). In this case all edges,
  //    should be invalid for that face, because edges have also been reverted;
  // 2. All checked edges of the face are invalid for this face;
  // The face should be removed from the splits in the following cases:
  // 1. All checked edges of the face are invalid for this one, but valid for
  //    some other face in this list of splits.
  // The face will be kept in the following cases:
  // 1. Some of the edges are valid for this face.
  Standard_Boolean bHasValid, bAllValid, bAllInvalid, bHasReallyInvalid, bAllInvNeutral;
  Standard_Boolean bValid, bValidLoc, bInvalid, bInvalidLoc, bNeutral;
  Standard_Integer i, aNbChecked;
  //
  // neutral edges
  TopTools_MapOfShape aMEN;
  for (TopTools_ListIteratorOfListOfShape aItLE(theLENeutral); aItLE.More(); aItLE.Next())
  {
    aMEN.Add(aItLE.Value());
  }
  //
  // valid inverted edges
  TopTools_MapOfShape aMEValInverted;
  for (TopTools_ListIteratorOfListOfShape aItLE(theLEValInverted); aItLE.More(); aItLE.Next())
  {
    aMEValInverted.Add(aItLE.Value());
  }
  //
  Standard_Boolean bCheckInverted = (theLFImages.Extent() == 1);
  //
  // neutral edges to remove
  TopTools_IndexedMapOfShape aMENRem;
  //
  // faces for post treat
  TopTools_ListOfShape aLFPT;
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLFImages);
  for (; aItLF.More(); ) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    // valid edges for this split
    TopTools_MapOfShape aMVE;
    // invalid edges for this split
    TopTools_MapOfShape aMIE;
    //
    for (i = 0; i < 2; ++i) {
      TopTools_MapOfShape& aME = !i ? aMVE : aMIE;
      const TopTools_ListOfShape* pLE = !i ? theDMFLVE.Seek(aFIm) : theDMFLIE.Seek(aFIm);
      if (pLE) {
        TopTools_ListIteratorOfListOfShape aItLE(*pLE);
        for (; aItLE.More(); aItLE.Next()) {
          const TopoDS_Shape& aE = aItLE.Value();
          aME.Add(aE);
        }
      }
    }
    //
    bHasValid = Standard_False;
    bAllValid = Standard_True;
    bAllInvalid = Standard_True;
    bHasReallyInvalid = Standard_False;
    bAllInvNeutral = Standard_True;
    aNbChecked = 0;
    //
    const TopoDS_Wire& aWIm = BRepTools::OuterWire(aFIm);
    TopExp_Explorer aExp(aWIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      bValid = theValidEdges.Contains(aEIm);
      bInvalid = theInvEdges.Contains(aEIm);
      //
      if (!bValid && !bInvalid) {
        // edge has not been checked for some reason
        continue;
      }
      //
      ++aNbChecked;
      //
      bInvalidLoc = aMIE.Contains(aEIm);
      bHasReallyInvalid = bInvalidLoc && !bValid && !theEdgesInvalidByVertex.Contains(aEIm);
      if (bHasReallyInvalid) {
        break;
      }
      //
      bNeutral = aMEN.Contains(aEIm);
      bValidLoc = aMVE.Contains(aEIm);
      //
      if (!bInvalid && bCheckInverted) {
        bInvalid = theMEInverted.Contains(aEIm);
      }
      //
      if (bValidLoc && (bNeutral || aMEValInverted.Contains(aEIm))) {
        bHasValid = Standard_True;
      }
      //
      bAllValid = bAllValid && bValidLoc;
      bAllInvalid = bAllInvalid && bInvalid;
      bAllInvNeutral = bAllInvNeutral && bAllInvalid && bNeutral;
    }
    //
    if (!aNbChecked) {
      aItLF.Next();
      continue;
    }
    //
    if (!bHasReallyInvalid && (bAllInvNeutral && !bHasValid) && (aNbChecked > 1)) {
      // remove edges from neutral
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMENRem);
      // remove face
      theLFImages.Remove(aItLF);
      continue;
    }
    //
    if (bHasReallyInvalid || (bAllInvalid && 
                             !(bHasValid || bAllValid) &&
                             !(bAllInvNeutral && (aNbChecked == 1)))) {
      theInvFaces.Append(aFIm);
      if (theMFHoles.Contains(aFIm)) {
        theMFInvInHole.Add(aFIm);
      }
      aItLF.Next();
      continue;
    }
    //
    if (theMFHoles.Contains(aFIm)) {
      // remove edges from neutral
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMENRem);
      // remove face
      theLFImages.Remove(aItLF);
      continue;
    }
    //
    if (!bAllInvNeutral) {
      aLFPT.Append(aFIm);
    }
    else {
      // remove edges from neutral
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMENRem);
    }
    aItLF.Next();
  }
  //
  if (aLFPT.IsEmpty() || aMENRem.IsEmpty()) {
    return;
  }
  //
  Standard_Integer aNb = aMENRem.Extent();
  for (i = 1; i <= aNb; ++i) {
    aMEN.Remove(aMENRem(i));
  }
  //
  // check the splits once more
  aItLF.Initialize(aLFPT);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aFIm = *(TopoDS_Face*)&aItLF.Value();
    //
    // valid edges for this split
    TopTools_MapOfShape aMVE;
    const TopTools_ListOfShape* pLVE = theDMFLVE.Seek(aFIm);
    if (pLVE) {
      TopTools_ListIteratorOfListOfShape aItLE(*pLVE);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aE = aItLE.Value();
        aMVE.Add(aE);
      }
    }
    //
    bHasValid = Standard_False;
    bAllValid = Standard_True;
    bAllInvalid = Standard_True;
    //
    const TopoDS_Wire& aWIm = BRepTools::OuterWire(aFIm);
    TopExp_Explorer aExp(aWIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      bValid = theValidEdges.Contains(aEIm);
      bInvalid = theInvEdges.Contains(aEIm);
      bNeutral = aMEN.Contains(aEIm);
      bValidLoc = aMVE.Contains(aEIm);
      //
      if (!bInvalid && bCheckInverted) {
        bInvalid = theMEInverted.Contains(aEIm);
      }
      //
      if (bValidLoc && (bNeutral || aMEValInverted.Contains(aEIm))) {
        bHasValid = Standard_True;
      }
      //
      bAllValid = bAllValid && bValidLoc;
      bAllInvalid = bAllInvalid && bInvalid;
    }
    //
    if (bAllInvalid && !bHasValid && !bAllValid) {
      theInvFaces.Append(aFIm);
    }
  }
}

//=======================================================================
//function : FindFacesInsideHoleWires
//purpose  : Find faces inside holes wires from the original face
//=======================================================================
void FindFacesInsideHoleWires(const TopoDS_Face& theFOrigin,
                              const TopoDS_Face& theFOffset,
                              const TopTools_ListOfShape& theLFImages,
                              const TopTools_MapOfShape& theInvertedEdges,
                              const TopTools_DataMapOfShapeListOfShape& theDMEOrLEIm,
                              TopTools_MapOfShape& theMFHoles,
                              TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                              Handle(IntTools_Context)& theContext)
{
  if (theLFImages.IsEmpty()) {
    return;
  }
  //
  // find all hole wires in the original face
  TopTools_ListOfShape aLHoleWires;
  const TopoDS_Wire& anOuterWire = BRepTools::OuterWire(theFOrigin);
  TopExp_Explorer aExpW(theFOrigin, TopAbs_WIRE);
  for (; aExpW.More(); aExpW.Next()) {
    const TopoDS_Wire& aHoleWire = TopoDS::Wire(aExpW.Current());
    if (!aHoleWire.IsSame(anOuterWire) && aHoleWire.Orientation() != TopAbs_INTERNAL) {
      aLHoleWires.Append(aHoleWire);
    }
  }
  //
  if (aLHoleWires.IsEmpty()) {
    // no holes in the face
    return;
  }
  //
  TopTools_ListOfShape *pLFNewHoles = theDMFNewHoles.ChangeSeek(theFOrigin);
  //
  if (!pLFNewHoles) {
    pLFNewHoles = theDMFNewHoles.Bound(theFOrigin, TopTools_ListOfShape());
  }
  if (pLFNewHoles->IsEmpty()) {
    //
    // find the faces representing holes in the images of the faces:
    // 1. for each original hole wire try to build its image
    // 2. build the new planar face from the images
    //
    // map vertices and edges of the splits
    TopTools_IndexedMapOfShape aMESplits;
    TopTools_ListIteratorOfListOfShape aItLF(theLFImages);
    for (; aItLF.More(); aItLF.Next()) {
      TopExp::MapShapes(aItLF.Value(), TopAbs_EDGE, aMESplits);
    }
    //
    TopTools_ListIteratorOfListOfShape aItLW(aLHoleWires);
    for (; aItLW.More(); aItLW.Next()) {
      const TopoDS_Wire& aHoleWire = TopoDS::Wire(aItLW.Value());
      // find images of all edges of the original wire
      TopTools_IndexedMapOfShape aMEImWire;
      TopoDS_Iterator aItE(aHoleWire);
      for (; aItE.More(); aItE.Next()) {
        const TopoDS_Shape& aEOr = aItE.Value();
        const TopTools_ListOfShape *pLEIm = theDMEOrLEIm.Seek(aEOr);
        if (!pLEIm || pLEIm->IsEmpty()) {
          continue;
        }
        TopTools_ListIteratorOfListOfShape aItLEIm(*pLEIm);
        for (; aItLEIm.More(); aItLEIm.Next()) {
          const TopoDS_Shape& aEIm = aItLEIm.Value();
          if (aMESplits.Contains(aEIm)) {
            aMEImWire.Add(aEIm);
          }
        }
      }
      //
      if (aMEImWire.IsEmpty()) {
        continue;
      }
      //
      // build new planar face using these edges
      BOPCol_ListOfShape aLE;
      Standard_Integer i, aNbE = aMEImWire.Extent();
      for (i = 1; i <= aNbE; ++i) {
        aLE.Append(aMEImWire(i).Oriented(TopAbs_FORWARD));
        aLE.Append(aMEImWire(i).Oriented(TopAbs_REVERSED));
      }
      //
      BOPAlgo_BuilderFace aBF;
      aBF.SetFace(TopoDS::Face(theFOffset.Oriented(TopAbs_FORWARD)));
      aBF.SetShapes(aLE);
      aBF.Perform();
      //
      const BOPCol_ListOfShape& aLFNew = aBF.Areas();
      if (aLFNew.IsEmpty()) {
        continue;
      }
      //
      // check if outer edges in the new faces are not inverted
      // because the inverted edges mean that the hole has been
      // filled during offset and there will be no faces to remove
      TopTools_IndexedDataMapOfShapeListOfShape aDMEFNew;
      TopTools_ListIteratorOfListOfShape aItLFNew(aLFNew);
      for (; aItLFNew.More(); aItLFNew.Next()) {
        TopExp::MapShapesAndAncestors(aItLFNew.Value(), TopAbs_EDGE, TopAbs_FACE, aDMEFNew);
      }
      //
      aNbE = aDMEFNew.Extent();
      for (i = 1; i <= aNbE; ++i) {
        if (aDMEFNew(i).Extent() == 1) {
          const TopoDS_Shape& aE = aDMEFNew.FindKey(i);
          if (theInvertedEdges.Contains(aE)) {
            break;
          }
        }
      }
      //
      if (i <= aNbE) {
        continue;
      }
      //
      aItLFNew.Initialize(aLFNew);
      for (; aItLFNew.More(); aItLFNew.Next()) {
        pLFNewHoles->Append(aItLFNew.Value());
      }
    }
  }
  //
  // among the splits of the offset face find those that are
  // located inside the hole faces
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLFImages);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
    //
    // get the point inside the face and classify it relatively hole faces
    gp_Pnt aP3D;
    gp_Pnt2d aP2D;
    Standard_Integer iErr = BOPTools_AlgoTools3D::PointInFace(aFIm, aP3D, aP2D, theContext);
    if (iErr) {
      continue;
    }
    //
    Standard_Real aTol = BRep_Tool::Tolerance(aFIm);
    //
    TopTools_ListIteratorOfListOfShape aItLFNew(*pLFNewHoles);
    for (; aItLFNew.More(); aItLFNew.Next()) {
      const TopoDS_Face& aFNew = TopoDS::Face(aItLFNew.Value());
      if (theContext->IsValidPointForFace(aP3D, aFNew, aTol)) {
        // the face is classified as IN
        theMFHoles.Add(aFIm);
        break;
      }
    }
  }
}

//=======================================================================
//function : GetAverageTangent
//purpose  : Computes average tangent vector along the curve
//=======================================================================
gp_Vec GetAverageTangent(const TopoDS_Shape& theS,
                         const Standard_Integer theNbP)
{
  gp_Vec aVA;
  TopExp_Explorer aExp(theS, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE = *(TopoDS_Edge*)&aExp.Current();
    //
    Standard_Real aT1, aT2;
    const Handle(Geom_Curve)& aC = BRep_Tool::Curve(aE, aT1, aT2);
    //
    gp_Pnt aP;
    gp_Vec aV, aVSum;
    Standard_Real aT = aT1;
    Standard_Real aDt = (aT2 - aT1) / theNbP;
    while (aT <= aT2) {
      aC->D1(aT, aP, aV);
      aVSum += aV.Normalized();
      aT += aDt;
    }
    //
    if (aE.Orientation() == TopAbs_REVERSED) {
      aVSum.Reverse();
    }
    //
    aVA += aVSum;
  }
  return aVA;
}

//=======================================================================
//function : CheckInverted
//purpose  : Checks if the edge has been inverted
//=======================================================================
Standard_Boolean CheckInverted(const TopoDS_Edge& theEIm,
                               const TopoDS_Face& theFOr,
                               const TopTools_DataMapOfShapeListOfShape& theOEImages,
                               const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                               const TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                               const TopTools_IndexedDataMapOfShapeListOfShape& theDMVE,
                               const TopTools_IndexedMapOfShape& theMEdges,
                               TopTools_MapOfShape& theMEInverted)
{
  // It is necessary to compare the direction from first vertex
  // to the last vertex on the original edge with the
  // same direction on the new edge. If the directions
  // will be different - the edge has been inverted.
  //
  TopoDS_Vertex aVI1, aVI2; // vertices on the offset edge
  TopoDS_Vertex aVO1, aVO2; // vertices on the original edge
  //
  Standard_Integer i;
  // find vertices of the offset shape
  TopExp::Vertices(theEIm, aVI1, aVI2);
  //
  // find images
  TopTools_ListOfShape aLEImages;
  if (theOEOrigins.IsBound(theEIm)) {
    TopoDS_Wire anImages;
    BRep_Builder().MakeWire(anImages);
    //
    TopTools_MapOfShape aMImFence;
    const TopTools_ListOfShape& aLOffsetOr = theOEOrigins.Find(theEIm);
    TopTools_ListIteratorOfListOfShape aItOffset(aLOffsetOr);
    for (; aItOffset.More(); aItOffset.Next()) {
      const TopoDS_Shape& aEOffsetOr = aItOffset.Value();
      const TopTools_ListOfShape& aLImages = theOEImages.Find(aEOffsetOr);
      //
      TopTools_ListIteratorOfListOfShape aItImages(aLImages);
      for (; aItImages.More(); aItImages.Next()) {
        const TopoDS_Edge& anIm = *(TopoDS_Edge*)&aItImages.Value();
        if (theMEdges.Contains(anIm) && aMImFence.Add(anIm)) {
          BRep_Builder().Add(anImages, anIm);
          aLEImages.Append(anIm);
        }
      }
    }
    //
    // find alone vertices
    TopoDS_Vertex aVW1, aVW2;
    TopTools_IndexedDataMapOfShapeListOfShape aDMImVE;
    TopExp::MapShapesAndAncestors(anImages, TopAbs_VERTEX, TopAbs_EDGE, aDMImVE);
    //
    TopTools_ListOfShape aLVAlone;
    Standard_Integer aNb = aDMImVE.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopTools_ListOfShape& aLImE = aDMImVE(i);
      if (aLImE.Extent() == 1) {
        aLVAlone.Append(aDMImVE.FindKey(i));
      }
    }
    //
    if (aLVAlone.Extent() > 1) {
      aVW1 = *(TopoDS_Vertex*)&aLVAlone.First();
      aVW2 = *(TopoDS_Vertex*)&aLVAlone.Last();
      //
      // check distances
      const gp_Pnt& aPI1 = BRep_Tool::Pnt(aVI1);
      const gp_Pnt& aPW1 = BRep_Tool::Pnt(aVW1);
      const gp_Pnt& aPW2 = BRep_Tool::Pnt(aVW2);
      //
      Standard_Real aDist1 = aPI1.SquareDistance(aPW1);
      Standard_Real aDist2 = aPI1.SquareDistance(aPW2);
      //
      if (aDist1 < aDist2) {
        aVI1 = aVW1;
        aVI2 = aVW2;
      }
      else {
        aVI1 = aVW2;
        aVI2 = aVW1;
      }
    }
  }
  else {
    aLEImages.Append(theEIm);
  }
  //
  // Find edges connected to these vertices
  const TopTools_ListOfShape& aLIE1 = theDMVE.FindFromKey(aVI1);
  const TopTools_ListOfShape& aLIE2 = theDMVE.FindFromKey(aVI2);
  //
  // Find vertices on the original face corresponding to vertices on the offset edge
  //
  // find original edges for both lists
  TopTools_ListOfShape aLOE1, aLOE2;
  for (i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLIE = !i ? aLIE1 : aLIE2;
    TopTools_ListOfShape& aLOE = !i ? aLOE1 : aLOE2;
    //
    TopTools_MapOfShape aMFence;
    //
    TopTools_ListIteratorOfListOfShape aItLIE(aLIE);
    for (; aItLIE.More(); aItLIE.Next()) {
      const TopoDS_Shape& aEI = aItLIE.Value();
      if (theEdgesOrigins.IsBound(aEI)) {
        const TopTools_ListOfShape& aLEOrigins = theEdgesOrigins.Find(aEI);
        //
        TopTools_ListIteratorOfListOfShape aItLOE(aLEOrigins);
        for (; aItLOE.More(); aItLOE.Next()) {
          const TopoDS_Shape& aEO = aItLOE.Value();
          if (aEO.ShapeType() == TopAbs_EDGE && aMFence.Add(aEO)) {
            TopoDS_Shape aEOin;
            if (FindShape(aEO, theFOr, aEOin)) {
              AppendToList(aLOE, aEO);
            }
          }
        }
      }
    }
  }
  //
  if (aLOE1.Extent() < 2 || aLOE2.Extent() < 2) {
    return Standard_False;
  }
  //
  // find vertices common for all edges in the lists
  for (i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLOE = !i ? aLOE1 : aLOE2;
    TopoDS_Vertex& aVO = !i ? aVO1 : aVO2;
    //
    const TopoDS_Shape& aEO = aLOE.First();
    TopExp_Explorer aExpV(aEO, TopAbs_VERTEX);
    for (; aExpV.More(); aExpV.Next()) {
      const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aExpV.Current();
      //
      Standard_Boolean bVertValid = Standard_True;
      TopTools_ListIteratorOfListOfShape aItLOE(aLOE);
      for (aItLOE.Next(); aItLOE.More(); aItLOE.Next()) {
        const TopoDS_Shape& aEOx = aItLOE.Value();
        TopExp_Explorer aExpVx(aEOx, TopAbs_VERTEX);
        for (; aExpVx.More(); aExpVx.Next()) {
          const TopoDS_Shape& aVx = aExpVx.Current();
          if (aVx.IsSame(aV)) {
            break;
          }
        }
        //
        if (!aExpVx.More()) {
          bVertValid = Standard_False;
          break;
        }
      }
      //
      if (bVertValid) {
        aVO = aV;
        break;
      }
    }
  }
  //
  if (aVO1.IsNull() || aVO2.IsNull() || aVO1.IsSame(aVO2)) {
    return Standard_False;
  }
  //
  // check positions of the offset and original vertices
  const gp_Pnt& aPI1 = BRep_Tool::Pnt(aVI1);
  const gp_Pnt& aPI2 = BRep_Tool::Pnt(aVI2);
  const gp_Pnt& aPO1 = BRep_Tool::Pnt(aVO1);
  const gp_Pnt& aPO2 = BRep_Tool::Pnt(aVO2);
  //
  gp_Vec aVI(aPI1, aPI2);
  gp_Vec aVO(aPO1, aPO2);
  //
  Standard_Real anAngle = aVI.Angle(aVO);
  Standard_Boolean bInverted = Abs(anAngle - M_PI) < 1.e-4;
  if (bInverted) {
    TopTools_ListIteratorOfListOfShape aItLEIm(aLEImages);
    for (; aItLEIm.More(); aItLEIm.Next()) {
      const TopoDS_Shape& aEInvr = aItLEIm.Value();
      theMEInverted.Add(aEInvr);
    }
  }
  return bInverted;
}

//=======================================================================
//function : CheckInvertedBlock
//purpose  : Checks if it is possible to remove the block containing
//           inverted edges
//=======================================================================
Standard_Boolean CheckInvertedBlock(const TopoDS_Shape& theCB,
                                    const TopTools_ListOfShape& theLCBF,
                                    const TopTools_MapOfShape& theMEInverted,
                                    const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                    BRepOffset_DataMapOfShapeMapOfShape& theDMCBVInverted,
                                    BRepOffset_DataMapOfShapeMapOfShape& theDMCBVAll)
{
  // For possible removal of the block:
  // 1. There should be more than just one face in the block
  TopoDS_Iterator aItF(theCB);
  aItF.Next();
  if (!aItF.More()) {
    return Standard_False;
  }
  //
  // 2. The block should at least contain two connected inverted edges with
  //    different origins (not just two images/splits of the same edge)
  TopTools_MapOfShape aMECBInv;
  TopoDS_Compound aCECBInv;
  BRep_Builder().MakeCompound(aCECBInv);
  //
  TopExp_Explorer aExp(theCB, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aE = aExp.Current();
    if (theMEInverted.Contains(aE)) {
      if (aMECBInv.Add(aE)) {
        BRep_Builder().Add(aCECBInv, aE);
      }
    }
  }
  //
  if (aMECBInv.Extent() < 2) {
    return Standard_False;
  }
  //
  // check that the edges are connected and different
  TopTools_ListOfShape aLCBE;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCECBInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
  //
  TopTools_ListIteratorOfListOfShape aItLCBE(aLCBE);
  for (; aItLCBE.More(); aItLCBE.Next()) {
    const TopoDS_Shape& aCBE = aItLCBE.Value();
    // count the unique edges in the block
    Standard_Integer aNbUnique = 0;
    TopTools_MapOfShape aMEOrigins;
    TopoDS_Iterator aItE(aCBE);
    for (; aItE.More(); aItE.Next()) {
      const TopoDS_Shape& aE = aItE.Value();
      const TopTools_ListOfShape *pLEOr = theOEOrigins.Seek(aE);
      if (!pLEOr) {
        aMEOrigins.Add(aE);
        ++aNbUnique;
        continue;
      }
      TopTools_ListIteratorOfListOfShape aItLEOr(*pLEOr);
      for (; aItLEOr.More(); aItLEOr.Next()) {
        const TopoDS_Shape& aEOr = aItLEOr.Value();
        if (aMEOrigins.Add(aEOr)) {
          ++aNbUnique;
        }
      }
    }
    //
    if (aNbUnique >= 2) {
      break;
    }
  }
  //
  if (!aItLCBE.More()) {
    return Standard_False;
  }
  //
  // 3. the block should not contain inverted edges which vertices
  //    are contained in other blocks
  //
  // collect vertices from inverted edges and compare them with
  // vertices from other blocks
  TopTools_MapOfShape* pMVInverted = theDMCBVInverted.ChangeSeek(theCB);
  TopTools_MapOfShape* pMVAll = theDMCBVAll.ChangeSeek(theCB);
  if (!pMVInverted) {
    pMVInverted = theDMCBVInverted.Bound(theCB, TopTools_MapOfShape());
    pMVAll = theDMCBVAll.Bound(theCB, TopTools_MapOfShape());
    //
    GetVerticesOnEdges(theCB, theMEInverted, *pMVInverted, *pMVAll);
  }
  //
  TopTools_ListIteratorOfListOfShape aItLCB1(theLCBF);
  for (; aItLCB1.More(); aItLCB1.Next()) {
    const TopoDS_Shape& aCB1 = aItLCB1.Value();
    if (aCB1.IsSame(theCB)) {
      continue;
    }
    //
    // collect vertices from inverted edges
    TopTools_MapOfShape* pMVInverted1 = theDMCBVInverted.ChangeSeek(aCB1);
    TopTools_MapOfShape* pMVAll1 = theDMCBVAll.ChangeSeek(aCB1);
    if (!pMVInverted1) {
      pMVInverted1 = theDMCBVInverted.Bound(aCB1, TopTools_MapOfShape());
      pMVAll1 = theDMCBVAll.Bound(aCB1, TopTools_MapOfShape());
      //
      GetVerticesOnEdges(aCB1, theMEInverted, *pMVInverted1, *pMVAll1);
    }
    //
    if (pMVInverted->HasIntersection(*pMVAll1)) {
      return Standard_False;
    }
  }
  //
  return Standard_True;
}

//=======================================================================
//function : GetVerticesOnEdges
//purpose  : Get vertices from the given shape belonging to the given edges
//=======================================================================
void GetVerticesOnEdges(const TopoDS_Shape& theCB,
                        const TopTools_MapOfShape& theEdges,
                        TopTools_MapOfShape& theVerticesOnEdges,
                        TopTools_MapOfShape& theAllVertices)
{
  TopExp_Explorer aExp(theCB, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aE = aExp.Current();
    Standard_Boolean bOnGivenEdge = theEdges.Contains(aE);
    for (TopoDS_Iterator aItV(aE); aItV.More(); aItV.Next()) {
      theAllVertices.Add(aItV.Value());
      if (bOnGivenEdge) {
        theVerticesOnEdges.Add(aItV.Value());
      }
    }
  }
}

//=======================================================================
//function : RemoveInvalidSplitsByInvertedEdges
//purpose  : Looking for the invalid faces containing inverted edges
//           that can be safely removed
//=======================================================================
void RemoveInvalidSplitsByInvertedEdges(const TopTools_MapOfShape& theMEInverted,
                                        const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                        TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                        TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                        TopTools_IndexedMapOfShape& theMERemoved)
{
  if (theMEInverted.IsEmpty()) {
    return;
  }
  //
  // check the faces on regularity, i.e. the splits of the same face
  // should not be connected only by vertex. Such irregular splits
  // will have to be rebuilt and cannot be removed.
  //
  BRep_Builder aBB;
  TopTools_IndexedMapOfShape aMEAvoid;
  TopTools_DataMapOfShapeListOfShape aDMVF;
  Standard_Integer aNb = theFImages.Extent(), i;
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFIm = theFImages(i);
    //
    TopoDS_Compound aCFIm;
    aBB.MakeCompound(aCFIm);
    //
    TopTools_DataMapOfShapeListOfShape aDMEF;
    TopTools_ListIteratorOfListOfShape aIt(aLFIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF = aIt.Value();
      aBB.Add(aCFIm, aF);
      //
      // make a map to use only outer edges
      TopExp_Explorer aExp(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        //
        TopTools_ListOfShape *pLF = aDMEF.ChangeSeek(aE);
        if (!pLF) {
          pLF = aDMEF.Bound(aE, TopTools_ListOfShape());
        }
        else {
          // internal edges should not be used
          aMEAvoid.Add(aE);
        }
        AppendToList(*pLF, aF);
      }
      //
      // fill connection map of the vertices of inverted edges to faces
      aExp.Init(aF, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV = aExp.Current();
        //
        TopTools_ListOfShape *pLF = aDMVF.ChangeSeek(aV);
        if (!pLF) {
          pLF = aDMVF.Bound(aV, TopTools_ListOfShape());
        }
        AppendToList(*pLF, aF);
      }
    }
    //
    // for the splits to be regular they should form only one block
    TopTools_ListOfShape aLCBF;
    BOPTools_AlgoTools::MakeConnexityBlocks(aCFIm, TopAbs_EDGE, TopAbs_FACE, aLCBF);
    if (aLCBF.Extent() == 1) {
      continue;
    }
    //
    // check if the inverted edges create the irregularity
    BRepOffset_DataMapOfShapeMapOfShape aDMCBVInverted, aDMCBVAll;
    //
    TopTools_ListIteratorOfListOfShape aItLCB(aLCBF);
    for (; aItLCB.More(); aItLCB.Next()) {
      const TopoDS_Shape& aCB = aItLCB.Value();
      //
      // check if it is possible to remove the block
      if (!CheckInvertedBlock(aCB, aLCBF, theMEInverted, theOEOrigins, aDMCBVInverted, aDMCBVAll)) {
        // non of the edges in this block should be removed
        TopExp::MapShapes(aCB, TopAbs_EDGE, aMEAvoid);
        continue;
      }
    }
  }
  //
  // all edges not included in aMEAvoid can be removed
  TopTools_MapOfShape aMERem;
  TopTools_MapIteratorOfMapOfShape aItM(theMEInverted);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aE = aItM.Value();
    if (!aMEAvoid.Contains(aE)) {
      TopoDS_Iterator aIt(aE);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aV = aIt.Value();
        const TopTools_ListOfShape *pLF = aDMVF.Seek(aV);
        if (pLF && (pLF->Extent() > 3)) {
          aMERem.Add(aE);
          break;
        }
      }
    }
  }
  //
  if (aMERem.IsEmpty()) {
    return;
  }
  //
  // all invalid faces containing these edges can be removed
  TopTools_IndexedDataMapOfShapeListOfShape aInvFaces;
  TopTools_MapOfShape aMFRem;
  TopTools_IndexedMapOfShape aMFToUpdate;
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    TopTools_ListOfShape& aLFIm = theInvFaces(i);
    //
    TopTools_ListIteratorOfListOfShape aIt(aLFIm);
    for (; aIt.More(); ) {
      const TopoDS_Shape& aFIm = aIt.Value();
      //
      // to be removed the face should have at least two not connected
      // inverted edges
      TopoDS_Compound aCEInv;
      aBB.MakeCompound(aCEInv);
      TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        if (aMERem.Contains(aE)) {
          aBB.Add(aCEInv, aE);
        }
      }
      //
      // check connectivity
      TopTools_ListOfShape aLCBE;
      BOPTools_AlgoTools::MakeConnexityBlocks(aCEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
      //
      if (aLCBE.Extent() >= 2) {
        aMFToUpdate.Add(aF);
        aMFRem.Add(aFIm);
        aLFIm.Remove(aIt);
      }
      else {
        aIt.Next();
      }
    }
    //
    if (aLFIm.Extent()) {
      aInvFaces.Add(aF, aLFIm);
    }
  }
  //
  if (aMFRem.IsEmpty()) {
    return;
  }
  //
  theInvFaces = aInvFaces;
  // remove from splits
  aNb = aMFToUpdate.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = aMFToUpdate(i);
    TopTools_ListOfShape& aLFIm = theFImages.ChangeFromKey(aF);
    //
    TopTools_ListIteratorOfListOfShape aIt(aLFIm);
    for (; aIt.More(); ) {
      const TopoDS_Shape& aFIm = aIt.Value();
      if (aMFRem.Contains(aFIm)) {
        TopExp::MapShapes(aFIm, TopAbs_EDGE, theMERemoved);
        aLFIm.Remove(aIt);
      }
      else {
        aIt.Next();
      }
    }
  }
}

//=======================================================================
//function : RemoveInvalidSplitsFromValid
//purpose  : Removing invalid splits of faces from valid
//=======================================================================
void RemoveInvalidSplitsFromValid(const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                  const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                  const TopTools_MapOfShape& theMEInverted,
                                  TopTools_IndexedDataMapOfShapeListOfShape& theFImages)
{
  // Decide whether to remove the found invalid faces or not.
  // The procedure is the following:
  // 1. Make connexity blocks from invalid faces;
  // 2. Find free edges in this blocks;
  // 3. If all free edges are valid for the faces - remove block.
  //
  TopTools_MapOfShape aMFence, aMFToRem;
  TopoDS_Compound aCFInv;
  BRep_Builder aBB;
  aBB.MakeCompound(aCFInv);
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  // make compound of invalid faces
  TopTools_DataMapOfShapeShape aDMIFOF;
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    // artificially invalid faces should not be removed
    if (theArtInvFaces.IsBound(aF)) {
      continue;
    }
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      if (aMFence.Add(aFIm)) {
        aBB.Add(aCFInv, aFIm);
        aDMIFOF.Bind(aFIm, aF);
      }
    }
  }
  //
  // make connexity blocks
  TopTools_ListOfShape aLCBInv;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFInv, TopAbs_EDGE, TopAbs_FACE, aLCBInv);
  //
  // analyze each block
  aItLF.Initialize(aLCBInv);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Shape& aCB = aItLF.Value();
    //
    // if connexity block contains only one face - it should be removed;
    TopExp_Explorer aExp(aCB, TopAbs_FACE);
    aExp.Next();
    if (aExp.More()) {
      // check if there are valid images left
      aExp.Init(aCB, TopAbs_FACE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aFIm = aExp.Current();
        const TopoDS_Shape& aF = aDMIFOF.Find(aFIm);
        //
        const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
        const TopTools_ListOfShape& aLFInv = theInvFaces.FindFromKey(aF);
        //
        if (aLFIm.Extent() == aLFInv.Extent()) {
          break;
        }
      }
    }
    //
    if (!aExp.More()) {
      aExp.Init(aCB, TopAbs_FACE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aF = aExp.Current();
        aMFToRem.Add(aF);
      }
      continue;
    }
    //
    // remove faces connected by inverted edges
    TopTools_IndexedDataMapOfShapeListOfShape aDMEF;
    TopExp::MapShapesAndAncestors(aCB, TopAbs_EDGE, TopAbs_FACE, aDMEF);
    //
    aExp.Init(aCB, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aFCB = aExp.Current();
      //
      TopExp_Explorer aExpE(aFCB, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aECB = aExpE.Current();
        if (aDMEF.FindFromKey(aECB).Extent() > 1) {
          if (!theMEInverted.Contains(aECB)) {
            break;
          }
        }
      }
      //
      if (!aExpE.More()) {
        aMFToRem.Add(aFCB);
      }
    }
  }
  //
  if (aMFToRem.Extent()) {
    // remove invalid faces from images
    aNb = theInvFaces.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aF = theInvFaces.FindKey(i);
      TopTools_ListOfShape& aLFImages = theFImages.ChangeFromKey(aF);
      aItLF.Initialize(aLFImages);
      for (; aItLF.More();) {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (aMFToRem.Contains(aFIm)) {
          aLFImages.Remove(aItLF);
        }
        else {
          aItLF.Next();
        }
      }
    }
  }
}

//=======================================================================
//function : RemoveInsideFaces
//purpose  : Looking for the inside faces that can be safely removed
//=======================================================================
void RemoveInsideFaces(TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                       TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                       const TopTools_DataMapOfShapeShape& theArtInvFaces,
                       const TopTools_IndexedMapOfShape& theInvEdges,
                       const TopTools_IndexedMapOfShape& theMFToCheckInt,
                       const TopTools_IndexedMapOfShape& theMFInvInHole,
                       const TopoDS_Shape& theFHoles,
                       TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                       TopTools_IndexedMapOfShape& theMERemoved,
                       TopTools_IndexedMapOfShape& theMEInside,
                       TopoDS_Shape& theSolids)
{
  BOPCol_ListOfShape aLS;
  TopTools_MapOfShape aMFence;
  TopTools_IndexedMapOfShape aMFInv;
  TopTools_ListIteratorOfListOfShape aItLF;
  TopTools_DataMapOfShapeShape aDMFImF;
  //
  Standard_Integer i, aNb = theFImages.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFImages.FindKey(i);
    // to avoid intersection of the splits of the same
    // offset faces among themselves make compound of the
    // splits and use it as one argument
    TopoDS_Compound aCFImi;
    BRep_Builder().MakeCompound(aCFImi);
    //
    for (Standard_Integer j = 0; j < 2; ++j) {
      const TopTools_ListOfShape* pLFSp = !j ? theInvFaces.Seek(aF) : &theFImages(i);
      if (!pLFSp) {
        continue;
      }
      //
      aItLF.Initialize(*pLFSp);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (aMFence.Add(aFIm)) {
          BRep_Builder().Add(aCFImi, aFIm);
          aDMFImF.Bind(aFIm, aF);
          if (!j) {
            aMFInv.Add(aFIm);
          }
        }
      }
    }
    //
    aLS.Append(aCFImi);
  }
  //
  // to make the solids more complete add for intersection also the faces
  // consisting only of invalid edges and not included into splits
  aNb = theMFToCheckInt.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aFSp = theMFToCheckInt(i);
    if (aMFence.Add(aFSp)) {
      aLS.Append(aFSp);
    }
  }
  //
  BOPAlgo_MakerVolume aMV;
  aMV.SetArguments(aLS);
  aMV.SetIntersect(Standard_True);
  aMV.Perform();
  //
  // get shapes connection for using in the rebuilding process
  // for the cases in which some of the intersection left undetected
  ShapesConnections(theInvFaces, theInvEdges, aDMFImF, aMV, theSSInterfs);
  //
  // find faces to remove
  const TopoDS_Shape& aSols = aMV.Shape();
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMFS;
  TopExp::MapShapesAndAncestors(aSols, TopAbs_FACE, TopAbs_SOLID, aDMFS);
  //
  aNb = aDMFS.Extent();
  if (!aNb) {
    return;
  }
  //
  // To use the created solids for classifications, firstly, it is necessary
  // to check them on validity - the created solids should be complete,
  // i.e. all faces should be included.
  //
  TopTools_MapOfShape aMFToRem;
  // Check completeness
  if (aMV.HasDeleted()) {
    TopTools_IndexedMapOfShape aMEHoles;
    TopExp::MapShapes(theFHoles, TopAbs_EDGE, aMEHoles);
    //
    // perform additional check on faces
    aNb = theFImages.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopTools_ListOfShape& aLFIm = theFImages(i);
      if (aLFIm.IsEmpty()) {
        continue;
      }
      //
      Standard_Boolean bFaceKept = Standard_False;
      aItLF.Initialize(aLFIm);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFIm = aItLF.Value();
        if (!aMV.IsDeleted(aFIm)) {
          bFaceKept = Standard_True;
          continue;
        }
        //
        TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          if (aMEHoles.Contains(aExpE.Current())) {
            bFaceKept = Standard_True;
            aMFToRem.Add(aFIm);
            break;
          }
        }
      }
      //
      if (!bFaceKept) {
        return;
      }
    }
  }
  //
  TopTools_IndexedMapOfShape aMEBoundary;
  aNb = aDMFS.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aFIm = aDMFS.FindKey(i);
    const TopTools_ListOfShape& aLSol = aDMFS(i);
    if (aLSol.Extent() > 1) {
      aMFToRem.Add(aFIm);
    }
    else if (aFIm.Orientation() != TopAbs_INTERNAL) {
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEBoundary);
    }
  }
  //
  // update invalid faces with images
  aNb = aMFInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aFInv = aMFInv(i);
    const TopTools_ListOfShape& aLFInvIm = aMV.Modified(aFInv);
    TopTools_ListIteratorOfListOfShape aItLFInvIm(aLFInvIm);
    for (; aItLFInvIm.More(); aItLFInvIm.Next()) {
      const TopoDS_Shape& aFInvIm = aItLFInvIm.Value();
      aMFInv.Add(aFInvIm);
    }
  }
  //
  // check if the invalid faces inside the holes are really invalid:
  // check its normal direction - if it has changed relatively the
  // original face the offset face is invalid and should be kept for rebuilding
  Standard_Integer aNbFH = theMFInvInHole.Extent();
  for (i = 1; i <= aNbFH; ++i) {
    const TopoDS_Shape& aFInv = theMFInvInHole(i);
    TopTools_ListOfShape aLFInvIm = aMV.Modified(aFInv);
    if (aLFInvIm.IsEmpty()) {
      aLFInvIm.Append(aFInv);
    }
    //
    const TopoDS_Shape *pFOffset = aDMFImF.Seek(aFInv);
    if (!pFOffset) {
      continue;
    }
    TopTools_ListIteratorOfListOfShape aItLFInv(aLFInvIm);
    for (; aItLFInv.More(); aItLFInv.Next()) {
      const TopoDS_Shape& aFInvIm = aItLFInv.Value();
      const TopTools_ListOfShape* pLSols = aDMFS.Seek(aFInvIm);
      if (!pLSols || pLSols->Extent() != 1) {
        continue;
      }
      //
      const TopoDS_Shape& aFSol = pLSols->First();
      //
      TopoDS_Shape aFx;
      if (!FindShape(aFInvIm, aFSol, aFx)) {
        continue;
      }
      //
      if (BRepOffset_Tool::CheckPlanesNormals(TopoDS::Face(aFx), TopoDS::Face(*pFOffset))) {
        // the normal direction has not changed, thus the face can be removed
        aMFToRem.Add(aFInvIm);
      }
    }
  }
  //
  TopoDS_Compound aSolids;
  BRep_Builder().MakeCompound(aSolids);
  TopTools_MapOfShape aMFKeep;
  //
  TopExp_Explorer aExpS(aSols, TopAbs_SOLID);
  for (; aExpS.More(); aExpS.Next()) {
    const TopoDS_Shape& aSol = aExpS.Current();
    //
    Standard_Boolean bAllInv(Standard_True), bAllRemoved(Standard_True);

    for (TopExp_Explorer aExpF(aSol, TopAbs_FACE); aExpF.More(); aExpF.Next())
    {
      const TopoDS_Shape& aFS = aExpF.Current();
      //
      if (aFS.Orientation() == TopAbs_INTERNAL) {
        aMFToRem.Add(aFS);
      }
      //
      bAllRemoved = bAllRemoved && aMFToRem.Contains(aFS);
      bAllInv = bAllInv && (aMFToRem.Contains(aFS) || aMFInv.Contains(aFS));
    }
    //
    if (bAllInv && !bAllRemoved) {
      // remove invalid faces but keep those that have already been marked for removal
      TopExp_Explorer aExpF(aSol, TopAbs_FACE);
      for (; aExpF.More(); aExpF.Next()) {
        const TopoDS_Shape& aFS = aExpF.Current();
        //
        if (aMFToRem.Contains(aFS)) {
          if (!aMFKeep.Add(aFS)) {
            aMFKeep.Remove(aFS);
          }
        }
        else {
          aMFToRem.Add(aFS);
        }
      }
    }
    else {
      BRep_Builder().Add(aSolids, aSol);
      theSolids = aSolids;
    }
  }
  //
  TopTools_MapIteratorOfMapOfShape aItM(aMFKeep);
  for (; aItM.More(); aItM.Next()) {
    aMFToRem.Remove(aItM.Value());
  }
  //
  // remove newly found internal faces
  RemoveValidSplits(aMFToRem, theFImages, aMV, theMERemoved);
  RemoveInvalidSplits(aMFToRem, theArtInvFaces, theInvEdges, theInvFaces, aMV, theMERemoved);
  //
  // Get inside faces from the removed ones comparing them with boundary edges
  aNb = theMERemoved.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aE = theMERemoved(i);
    if (!aMEBoundary.Contains(aE)) {
      theMEInside.Add(aE);
    }
  }
}

//=======================================================================
//function : ShapesConnections
//purpose  : Looking for the connections between faces not to miss
//           some necessary intersection
//=======================================================================
void ShapesConnections(const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                       const TopTools_IndexedMapOfShape& theInvEdges,
                       const TopTools_DataMapOfShapeShape& theDMFOr,
                       BOPAlgo_Builder& theBuilder,
                       TopTools_DataMapOfShapeListOfShape& theSSInterfs)
{
  // update invalid edges with images and keep connection to original edge
  TopTools_DataMapOfShapeListOfShape aDMEOr;
  Standard_Integer aNb = theInvEdges.Extent();
  for (Standard_Integer i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aEInv = theInvEdges(i);
    const TopTools_ListOfShape& aLEIm = theBuilder.Modified(aEInv);
    if (aLEIm.IsEmpty()) {
      aDMEOr.Bound(aEInv, TopTools_ListOfShape())->Append(aEInv);
      continue;
    }
    //
    TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
    for (; aItLEIm.More(); aItLEIm.Next()) {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      //
      TopTools_ListOfShape* pLEOr = aDMEOr.ChangeSeek(aEIm);
      if (!pLEOr) {
        pLEOr = aDMEOr.Bound(aEIm, TopTools_ListOfShape());
      }
      AppendToList(*pLEOr, aEInv);
    }
  }
  //
  // get shapes connections for using in the rebuilding process
  const BOPDS_PDS& pDS = theBuilder.PDS();
  // analyze all Face/Face intersections
  const BOPDS_VectorOfInterfFF& aFFs = pDS->InterfFF();
  Standard_Integer iInt, aNbFF = aFFs.Extent();
  for (iInt = 0; iInt < aNbFF; ++iInt) {
    const BOPDS_InterfFF& aFF = aFFs(iInt);
    const BOPDS_VectorOfCurve& aVNC = aFF.Curves();
    Standard_Integer aNbC = aVNC.Extent();
    if (!aNbC) {
      continue;
    }
    //
    const TopoDS_Shape& aFIm1 = pDS->Shape(aFF.Index1());
    const TopoDS_Shape& aFIm2 = pDS->Shape(aFF.Index2());
    //
    const TopoDS_Shape* pF1 = theDMFOr.Seek(aFIm1);
    const TopoDS_Shape* pF2 = theDMFOr.Seek(aFIm2);
    //
    if (!pF1 || !pF2) {
      continue;
    }
    //
    if (pF1->IsSame(*pF2)) {
      continue;
    }
    //
    Standard_Boolean bInv1 = theInvFaces.Contains(*pF1);
    Standard_Boolean bInv2 = theInvFaces.Contains(*pF2);
    //
    if (!bInv1 && !bInv2) {
      continue;
    }
    //
    // check if it is real Face/Face intersection
    TopTools_MapOfShape aMEInt;
    for (Standard_Integer iC = 0; iC < aNbC; ++iC) {
      const BOPDS_Curve& aNC = aVNC(iC);
      const BOPDS_ListOfPaveBlock& aLPB = aNC.PaveBlocks();
      BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
        Standard_Integer nEInt;
        if (aPB->HasEdge(nEInt)) {
          const TopoDS_Shape& aEInt = pDS->Shape(nEInt);
          aMEInt.Add(aEInt);
        }
      }
    }
    //
    if (aMEInt.IsEmpty()) {
      continue;
    }
    //
    // check if invalid edges of the face are in the same splits with intersection edges
    for (Standard_Integer i = 0; i < 2; ++i) {
      if ((!i && !bInv1) || (i && !bInv2)) {
        continue;
      }
      //
      const TopoDS_Shape& aF = !i ? *pF1 : *pF2;
      const TopoDS_Shape& aFOp = !i ? *pF2 : *pF1;
      const TopoDS_Shape& aFIm = !i ? aFIm1 : aFIm2;
      //
      Standard_Boolean bFound = Standard_False;
      //
      TopTools_ListOfShape aLFIm = theBuilder.Modified(aFIm);
      if (aLFIm.IsEmpty()) {
        aLFIm.Append(aFIm);
      }
      //
      TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next()) {
        const TopoDS_Shape& aFImIm = aItLFIm.Value();
        //
        Standard_Boolean bInv(Standard_False), bInt(Standard_False);
        TopExp_Explorer aExpE(aFImIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aE = aExpE.Current();
          if (!bInv) {
            bInv = aDMEOr.IsBound(aE);
          }
          if (!bInt) {
            bInt = aMEInt.Contains(aE);
          }
          if (bInv && bInt) {
            break;
          }
        }
        //
        if (!bInt || !bInv) {
          continue;
        }
        //
        bFound = Standard_True;
        //
        // append opposite face to all invalid edges in the split
        aExpE.Init(aFImIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aE = aExpE.Current();
          const TopTools_ListOfShape* pLEOr = aDMEOr.Seek(aE);
          if (!pLEOr) {
            continue;
          }
          //
          TopTools_ListIteratorOfListOfShape aItLE(*pLEOr);
          for (; aItLE.More(); aItLE.Next()) {
            const TopoDS_Shape& aEOr = aItLE.Value();
            TopTools_ListOfShape *pLFE = theSSInterfs.ChangeSeek(aEOr);
            if (!pLFE) {
              pLFE = theSSInterfs.Bound(aEOr, TopTools_ListOfShape());
            }
            AppendToList(*pLFE, aFOp);
          }
        }
      }
      if (bFound) {
        // save connection between offset faces
        TopTools_ListOfShape *pLF = theSSInterfs.ChangeSeek(aF);
        if (!pLF) {
          pLF = theSSInterfs.Bound(aF, TopTools_ListOfShape());
        }
        AppendToList(*pLF, aFOp);
      }
    }
  }
}

//=======================================================================
//function : RemoveValidSplits
//purpose  : Removing valid splits according to results of intersection
//=======================================================================
void RemoveValidSplits(const TopTools_MapOfShape& theSpRem,
                       TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                       BOPAlgo_Builder& theGF,
                       TopTools_IndexedMapOfShape& theMERemoved)
{
  Standard_Integer i, aNb = theImages.Extent();
  if (!aNb) {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i) {
    TopTools_ListOfShape& aLSIm = theImages(i);
    TopTools_ListIteratorOfListOfShape aIt(aLSIm);
    for (; aIt.More(); ) {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (theSpRem.Contains(aSIm)) {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
        continue;
      }
      //
      // check if all its images are have to be removed
      const TopTools_ListOfShape& aLSImIm = theGF.Modified(aSIm);
      if (aLSImIm.Extent()) {
        Standard_Boolean bAllRem = Standard_True;
        TopTools_ListIteratorOfListOfShape aIt1(aLSImIm);
        for (; aIt1.More(); aIt1.Next()) {
          const TopoDS_Shape& aSImIm = aIt1.Value();
          if (theSpRem.Contains(aSImIm)) {
            TopExp::MapShapes(aSImIm, TopAbs_EDGE, theMERemoved);
          }
          else {
            bAllRem = Standard_False;
          }
        }
        //
        if (bAllRem) {
          TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
          aLSIm.Remove(aIt);
          continue;
        }
      }
      aIt.Next();
    }
  }
}

//=======================================================================
//function : RemoveInvalidSplits
//purpose  : Removing invalid splits according to the results of intersection
//=======================================================================
void RemoveInvalidSplits(const TopTools_MapOfShape& theSpRem,
                         const TopTools_DataMapOfShapeShape& theArtInvFaces,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         TopTools_IndexedDataMapOfShapeListOfShape& theImages,
                         BOPAlgo_Builder& theGF,
                         TopTools_IndexedMapOfShape& theMERemoved)
{
  Standard_Integer i, aNb = theImages.Extent();
  if (!aNb) {
    return;
  }
  //
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aS = theImages.FindKey(i);
    Standard_Boolean bArt = theArtInvFaces.IsBound(aS);
    //
    TopTools_ListOfShape& aLSIm = theImages(i);
    TopTools_ListIteratorOfListOfShape aIt(aLSIm);
    for (; aIt.More();) {
      const TopoDS_Shape& aSIm = aIt.Value();
      if (theSpRem.Contains(aSIm)) {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
        continue;
      }
      //
      // check if all its images are have to be removed
      const TopTools_ListOfShape& aLSImIm = theGF.Modified(aSIm);
      if (aLSImIm.IsEmpty()) {
        aIt.Next();
        continue;
      }
      //
      Standard_Boolean bAllRem = Standard_True;
      TopTools_IndexedMapOfShape aMERemoved;
      TopTools_ListIteratorOfListOfShape aIt1(aLSImIm);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aSImIm = aIt1.Value();
        if (theSpRem.Contains(aSImIm)) {
          TopExp::MapShapes(aSImIm, TopAbs_EDGE, aMERemoved);
        }
        else {
          bAllRem = Standard_False;
        }
      }
      //
      if (bAllRem) {
        aLSIm.Remove(aIt);
        continue;
      }
      //
      if (bArt) {
        aIt.Next();
        continue;
      }
      //
      // remove the face from invalid if all invalid edges of this face
      // have been marked for removal
      TopExp_Explorer aExpE(aSIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aEInv = aExpE.Current();
        if (theInvEdges.Contains(aEInv) && !aMERemoved.Contains(aEInv)) {
          break;
        }
      }
      if (!aExpE.More()) {
        TopExp::MapShapes(aSIm, TopAbs_EDGE, theMERemoved);
        aLSIm.Remove(aIt);
      }
      else {
        aIt.Next();
      }
    }
  }
}

//=======================================================================
//function : FilterEdgesImages
//purpose  : Updating the maps of images and origins of the offset edges
//=======================================================================
void FilterEdgesImages(const TopoDS_Shape& theS,
                       TopTools_DataMapOfShapeListOfShape& theOEImages,
                       TopTools_DataMapOfShapeListOfShape& theOEOrigins)
{
  // map edges
  TopTools_IndexedMapOfShape aME;
  TopExp::MapShapes(theS, TopAbs_EDGE, aME);
  //
  theOEOrigins.Clear();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItDM(theOEImages);
  for (; aItDM.More(); aItDM.Next()) {
    const TopoDS_Shape& aE = aItDM.Key();
    TopTools_ListOfShape& aLEIm = aItDM.ChangeValue();
    //
    TopTools_ListIteratorOfListOfShape aIt(aLEIm);
    for (; aIt.More(); ) {
      const TopoDS_Shape& aEIm = aIt.Value();
      // filter images
      if (!aME.Contains(aEIm)) {
        // remove the image
        // edges with no images left should be kept in the map
        // to avoid their usage when building the splits of faces
        aLEIm.Remove(aIt);
        continue;
      }
      //
      // save origins
      if (theOEOrigins.IsBound(aEIm)) {
        AppendToList(theOEOrigins.ChangeFind(aEIm), aE);
      }
      else {
        TopTools_ListOfShape aLOr;
        aLOr.Append(aE);
        theOEOrigins.Bind(aEIm, aLOr);
      }
      //
      aIt.Next();
    }
  }
}

//=======================================================================
//function : FilterInvalidFaces
//purpose  : Filtering of the invalid faces
//=======================================================================
void FilterInvalidFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theDMEF,
                        const TopTools_IndexedMapOfShape& theMERemoved,
                        TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        TopTools_DataMapOfShapeShape& theArtInvFaces)
{
  //
  // filter invalid faces, considering faces having only valid 
  // images left with non-free edges as valid
  // do not remove invalid faces if it creates free edges
  //
  TopTools_IndexedDataMapOfShapeListOfShape aReallyInvFaces;
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    //
    if (theArtInvFaces.IsBound(aF)) {
      if (aLFInv.IsEmpty()) {
        theArtInvFaces.UnBind(aF);
      }
      else {
        aReallyInvFaces.Add(aF, aLFInv);
      }
      continue;
    }
    //
    if (aLFInv.IsEmpty()) {
      continue;
    }
    //
    const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
    Standard_Boolean bInvalid = aLFIm.IsEmpty();
    //
    if (!bInvalid) {
      // check two lists on common splits
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFInv = aItLF.Value();
        //
        TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
        for (; aItLFIm.More(); aItLFIm.Next()) {
          const TopoDS_Shape& aFIm = aItLFIm.Value();
          //
          if (aFInv.IsSame(aFIm)) {
            break;
          }
        }
        //
        if (aItLFIm.More()) {
          break;
        }
      }
      //
      bInvalid = aItLF.More();
    }
    //
    if (!bInvalid) {
      // check for free edges
      for (Standard_Integer j = 0; !bInvalid && j < 2; ++j) {
        const TopTools_ListOfShape& aLI = !j ? aLFIm : aLFInv;
        aItLF.Initialize(aLI);
        for (; aItLF.More(); aItLF.Next()) {
          const TopoDS_Shape& aFIm = aItLF.Value();
          //
          TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
          for (; aExp.More(); aExp.Next()) {
            const TopoDS_Shape& aE = aExp.Current();
            if (!theMERemoved.Contains(aE)) {
              const TopTools_ListOfShape* pLEF = theDMEF.Seek(aE);
              if (pLEF && pLEF->Extent() == 1) {
                break;
              }
            }
          }
          //
          if (aExp.More()) {
            break;
          }
        }
        bInvalid = aItLF.More();
      }
    }
    //
    if (bInvalid) {
      aReallyInvFaces.Add(aF, aLFInv);
    }
  }
  //
  theInvFaces = aReallyInvFaces;
}

//=======================================================================
//function : FilterInvalidEdges
//purpose  : Filtering the invalid edges according to currently invalid faces
//=======================================================================
void FilterInvalidEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        const TopTools_DataMapOfShapeShape& theArtInvFaces,
                        const TopTools_DataMapOfShapeListOfShape& theDMFLIE,
                        const TopTools_IndexedMapOfShape& theMERemoved,
                        TopTools_IndexedMapOfShape& theInvEdges)
{
  TopoDS_Compound aCEInv;
  TopTools_IndexedMapOfShape aMEInv;
  BRep_Builder aBB;
  aBB.MakeCompound(aCEInv);
  TopTools_ListIteratorOfListOfShape aItLF;
  //
  Standard_Integer i, aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFInv = theInvFaces(i);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      TopExp::MapShapes(aFIm, TopAbs_EDGE, aMEInv);
      //
      TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aE = aExpE.Current();
        if (theInvEdges.Contains(aE)) {
          aBB.Add(aCEInv, aE);
        }
      }
    }
  }
  //
  // remove edges which have been marked for removal
  TopTools_IndexedMapOfShape aMEInvToAvoid;
  TopTools_ListOfShape aLCBE;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCEInv, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
  //
  TopTools_ListIteratorOfListOfShape aItLCBE(aLCBE);
  for (; aItLCBE.More(); aItLCBE.Next()) {
    const TopoDS_Shape& aCBE = aItLCBE.Value();
    TopExp_Explorer aExpCB(aCBE, TopAbs_EDGE);
    for (; aExpCB.More(); aExpCB.Next()) {
      const TopoDS_Shape& aE = aExpCB.Current();
      if (!theMERemoved.Contains(aE)) {
        break;
      }
    }
    //
    if (!aExpCB.More()) {
      TopExp::MapShapes(aCBE, TopAbs_EDGE, aMEInvToAvoid);
    }
  }
  //
  TopTools_IndexedMapOfShape aReallyInvEdges;
  //
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    if (theArtInvFaces.IsBound(aF)) {
      const TopTools_ListOfShape& aLEInv = theDMFLIE.Find(aF);
      aItLF.Initialize(aLEInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aE = aItLF.Value();
        if (aMEInv.Contains(aE) && !aMEInvToAvoid.Contains(aE)) {
          aReallyInvEdges.Add(aE);
        }
      }
    }
    else {
      const TopTools_ListOfShape& aLFInv = theInvFaces(i);
      aItLF.Initialize(aLFInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFIm = aItLF.Value();
        TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
        for (; aExpE.More(); aExpE.Next()) {
          const TopoDS_Shape& aE = aExpE.Current();
          if (theInvEdges.Contains(aE) && !aMEInvToAvoid.Contains(aE)) {
            aReallyInvEdges.Add(aE);
          }
        }
      }
    }
  }
  //
  theInvEdges = aReallyInvEdges;
}

//=======================================================================
//function : FindFacesToRebuild
//purpose  : Looking for the faces that have to be rebuilt:
//           1. Faces close to invalidity
//           2. Faces containing some invalid parts
//=======================================================================
void FindFacesToRebuild(const TopTools_IndexedDataMapOfShapeListOfShape&  theLFImages,
                        const TopTools_IndexedMapOfShape&  theInvEdges,
                        const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                        const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                        TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                        TopTools_MapOfShape& theFSelfRebAvoid)
{
  Standard_Integer i, aNb = theLFImages.Extent();
  if (!aNb) {
    return;
  }
  //
  Standard_Boolean bRebuild;
  TopTools_ListIteratorOfListOfShape aItLF;
  TopTools_ListOfShape aLEValid;
  TopTools_MapOfShape aMFence, aMEReb, aMFReb;
  TopExp_Explorer aExp;
  //
  TopTools_DataMapOfShapeListOfShape aDMFLV;
  // get edges from invalid faces
  aNb = theInvFaces.Extent();
  for (i = 1; i <= aNb; i++) {
    const TopoDS_Shape& aF = theInvFaces.FindKey(i);
    aMFence.Clear();
    TopTools_ListOfShape aLVAvoid;
    const TopTools_ListOfShape& aLFIm = theInvFaces(i);
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aExp.Init(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        aMEReb.Add(aE);
        if (theInvEdges.Contains(aE)) {
          TopExp_Explorer aExpV(aE, TopAbs_VERTEX);
          for (; aExpV.More(); aExpV.Next()) {
            const TopoDS_Shape& aV = aExpV.Current();
            if (aMFence.Add(aV)) {
              aLVAvoid.Append(aV);
              aMEReb.Add(aV);
            }
          }
        }
      }
    }
    //
    if (aLVAvoid.Extent()) {
      aDMFLV.Bind(aF, aLVAvoid);
    }
    //
    const TopTools_ListOfShape* pLF = theSSInterfs.Seek(aF);
    if (pLF) {
      TopTools_ListIteratorOfListOfShape aItLFE(*pLF);
      for (; aItLFE.More(); aItLFE.Next()) {
        const TopoDS_Shape& aFE = aItLFE.Value();
        aMFReb.Add(aFE);
      }
    }
  }
  //
  // get face to rebuild
  aNb = theLFImages.Extent();
  for (i = 1; i <= aNb; i++) {
    const TopoDS_Shape& aF = theLFImages.FindKey(i);
    const TopTools_ListOfShape& aLFIm = theLFImages(i);
    TopTools_MapOfShape aMVAvoid;
    if (aDMFLV.IsBound(aF)) {
      const TopTools_ListOfShape& aLVAvoid = aDMFLV.Find(aF);
      TopTools_ListIteratorOfListOfShape aItLV(aLVAvoid);
      for (; aItLV.More(); aItLV.Next()) {
        const TopoDS_Shape& aV = aItLV.Value();
        aMVAvoid.Add(aV);
      }
    }
    //
    bRebuild = aMFReb.Contains(aF);
    aLEValid.Clear();
    aMFence.Clear();
    //
    aItLF.Initialize(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aExp.Init(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Edge& anEIm = TopoDS::Edge(aExp.Current());
        if (!theInvEdges.Contains(anEIm)) {
          if (aMFence.Add(anEIm)) {
            aLEValid.Append(anEIm);
          }
        }
        //
        if (!bRebuild) {
          bRebuild = aMEReb.Contains(anEIm);
        }
        //
        if (!bRebuild) {
          // check vertices
          TopExp_Explorer aExpV(anEIm, TopAbs_VERTEX);
          for (; aExpV.More() && !bRebuild; aExpV.Next()) {
            const TopoDS_Shape& aV = aExpV.Current();
            if (!aMVAvoid.Contains(aV)) {
              bRebuild = aMEReb.Contains(aV);
            }
          }
        }
      }
    }
    //
    if (!bRebuild) {
      bRebuild = aLFIm.Extent() && theInvFaces.Contains(aF);
      if (bRebuild) {
        theFSelfRebAvoid.Add(aF);
      }
    }
    //
    if (bRebuild) {
      theFToRebuild.Add(aF, aLEValid);
    }
  }
}

//=======================================================================
//function : RebuildFaces
//purpose  : Rebuilding of the faces
//=======================================================================
void RebuildFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                  const TopTools_MapOfShape& theFSelfRebAvoid,
                  const TopoDS_Shape& theSolids,
                  const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                  TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                  TopTools_DataMapOfShapeListOfShape& theDMFNewHoles,
                  TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                  TopTools_DataMapOfShapeShape& theFacesOrigins,
                  TopTools_DataMapOfShapeListOfShape& theOEImages,
                  TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                  TopTools_MapOfShape& theLastInvEdges,
                  TopTools_IndexedMapOfShape& theEdgesToAvoid,
                  TopTools_IndexedMapOfShape& theInvEdges,
                  TopTools_IndexedMapOfShape& theValidEdges,
                  const TopTools_MapOfShape& theInvertedEdges,
                  TopTools_DataMapOfShapeInteger& theAlreadyInvFaces,
                  TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                  const TopTools_DataMapOfShapeShape& theArtInvFaces,
                  TopTools_MapOfShape& theVertsToAvoid,
                  TopTools_DataMapOfShapeShape& theETrimEInf,
                  Handle(BRepAlgo_AsDes)& theAsDes)
{
  TopTools_MapOfShape aModifiedEdges;
  //
  // 1. Intersect faces
  IntersectFaces(theFToRebuild, theFSelfRebAvoid, theSolids, theSSInterfs, theFImages, theEdgesOrigins, theOEImages, 
                 theOEOrigins, theInvEdges, theValidEdges, theInvertedEdges, theEdgesToAvoid,
                 theInvFaces, theArtInvFaces, theVertsToAvoid, theETrimEInf, aModifiedEdges, theAsDes);
  //
  // 2. Repeat steps to build the correct faces
  BuildSplitsOfInvFaces(theFToRebuild, aModifiedEdges, theFImages, theDMFNewHoles, theEdgesOrigins,
                        theFacesOrigins, theOEImages, theOEOrigins, theLastInvEdges,
                        theEdgesToAvoid, theVertsToAvoid, theAlreadyInvFaces, theValidEdges, 
                        theETrimEInf, theAsDes);
}

//=======================================================================
//function : IntersectFaces
//purpose  : Intersection of the faces that should be rebuild
//           to resolve all invalidities
//=======================================================================
void IntersectFaces(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                    const TopTools_MapOfShape& theFSelfRebAvoid,
                    const TopoDS_Shape& theSolids,
                    const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                    TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                    TopTools_DataMapOfShapeListOfShape& theOEImages,
                    TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                    TopTools_IndexedMapOfShape& theInvEdges,
                    TopTools_IndexedMapOfShape& theValidEdges,
                    const TopTools_MapOfShape& theInvertedEdges,
                    TopTools_IndexedMapOfShape& theEdgesToAvoid,
                    TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                    const TopTools_DataMapOfShapeShape& theArtInvFaces,
                    TopTools_MapOfShape& theVertsToAvoid,
                    TopTools_DataMapOfShapeShape& theETrimEInf,
                    TopTools_MapOfShape& theModifiedEdges,
                    Handle(BRepAlgo_AsDes)& theAsDes)
{
  Standard_Integer aNbFR = theFToRebuild.Extent();
  if (!aNbFR) {
    return;
  }
  //
  Standard_Integer i, j, k, aNbInv;
  TopTools_ListIteratorOfListOfShape aItLF, aItLE;
  //
  // get vertices from invalid edges
  TopTools_MapOfShape aMVInv, aMVInvAll;
  aNbInv = theInvEdges.Extent();
  for (i = 1; i <= aNbInv; ++i) {
    const TopoDS_Shape& aEInv = theInvEdges(i);
    Standard_Boolean bValid = theValidEdges.Contains(aEInv);
    for (TopExp_Explorer aExp (aEInv, TopAbs_VERTEX); aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV = aExp.Current();
      aMVInvAll.Add(aV);
      if (!bValid) {
        aMVInv.Add(aV);
      }
    }
  }
  //
  Standard_Boolean bLookVertToAvoid = (aMVInv.Extent() > 0);
  //
  TopTools_DataMapOfShapeListOfShape aDMSF, aMDone, aMEInfETrim, aDMVEFull;
  TopTools_IndexedDataMapOfShapeListOfShape aFLE, aDMEFInv;
  //
  // Add all faces to rebuild to outgoing map <aFLE>,
  // plus link edges and vertices to the faces to
  // define intersection faces
  PrepareFacesForIntersection(theFToRebuild, theFImages, theInvFaces, theArtInvFaces, 
                              bLookVertToAvoid, aFLE, aMDone, aDMSF, aMEInfETrim, 
                              aDMVEFull, theETrimEInf, aDMEFInv);

  // Find vertices to avoid while trimming the edges.
  // These vertices are taken from the invalid edges common between
  // splits of different invalid, but not artificially, faces.
  // Additional condition for these vertices is that all 
  // edges adjacent to this vertex must be either invalid
  // or contained in invalid faces
  TopTools_MapOfShape aMVRInv = theVertsToAvoid;
  FindVerticesToAvoid(aDMEFInv, theInvEdges, theValidEdges, aDMVEFull, aMVRInv);
  //
  // The faces should be intersected selectively -
  // intersect only faces neighboring to the same invalid face
  // and connected to its invalid edges;
  // when dealing with artificially invalid faces for intersection to be
  // complete we need to use not only invalid edges, but also the 
  // edges connected to invalid ones

  // find blocks of artificially invalid faces
  TopTools_DataMapOfShapeShape aDMFImF;
  TopoDS_Compound aCFArt;
  BRep_Builder().MakeCompound(aCFArt);
  TopTools_DataMapIteratorOfDataMapOfShapeShape aItM(theArtInvFaces);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aF = aItM.Key();
    const TopTools_ListOfShape& aLFInv = theInvFaces.FindFromKey(aF);
    aItLF.Initialize(aLFInv);
    for (; aItLF.More(); aItLF.Next()) {
      BRep_Builder().Add(aCFArt, aItLF.Value());
      aDMFImF.Bind(aItLF.Value(), aF);
    }
  }
  //
  // make connexity blocks
  TopTools_ListOfShape aLCBArt;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCFArt, TopAbs_VERTEX, TopAbs_FACE, aLCBArt);
  //
  // alone edges
  TopTools_MapOfShape aMEAlone, aMEInvOnArt;
  //
  TopTools_ListIteratorOfListOfShape aItLCBArt(aLCBArt);
  for (; aItLCBArt.More(); aItLCBArt.Next()) {
    const TopoDS_Shape& aCB = aItLCBArt.Value();
    //
    // check if aCB contains splits of only one offset face
    TopTools_MapOfShape aMFArt;
    TopExp_Explorer aExpF(aCB, TopAbs_FACE);
    for (; aExpF.More(); aExpF.Next()) {
      aMFArt.Add(aDMFImF.Find(aExpF.Current()));
    }
    //
    Standard_Boolean bAlone = (aMFArt.Extent() == 1);
    //
    // vertices on invalid edges
    TopTools_MapOfShape aMVEInv;
    TopTools_MapOfShape aMFence;
    // edges that should not be marked as alone - edges having same origins as invalid ones
    TopTools_MapOfShape aMEAvoid;
    // map to find alone edges by looking for free vertices
    TopTools_IndexedDataMapOfShapeListOfShape aDMVEVal;
    //
    TopExp_Explorer aExpE(aCB, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next()) {
      const TopoDS_Shape& aE = aExpE.Current();
      if (theInvEdges.Contains(aE)) {
        aMEInvOnArt.Add(aE);
        for (TopoDS_Iterator aItV(aE); aItV.More(); aItV.Next()) {
          aMVEInv.Add(aItV.Value());
        }
        //
        if (bAlone) {
          const TopTools_ListOfShape *pLEOr = theOEOrigins.Seek(aE);
          if (pLEOr) {
            TopTools_ListIteratorOfListOfShape aItLEOr(*pLEOr);
            for (; aItLEOr.More(); aItLEOr.Next()) {
              TopTools_ListIteratorOfListOfShape aItLEIm(theOEImages.Find(aItLEOr.Value()));
              for (; aItLEIm.More(); aItLEIm.Next()) {
                aMEAvoid.Add(aItLEIm.Value());
              }
            }
          }
        }
        continue;
      }
      //
      if (aMFence.Add(aE)) {
        TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aDMVEVal);
      }
    }
    //
    // find edges with free vertices
    Standard_Integer aNbV = aDMVEVal.Extent();
    for (i = 1; i <= aNbV; ++i) {
      const TopoDS_Shape& aV = aDMVEVal.FindKey(i);
      if (!aMVEInv.Contains(aV)) {
        continue;
      }
      //
      const TopTools_ListOfShape& aLEV = aDMVEVal(i);
      if (aLEV.Extent() > 1) {
        continue;
      }
      //
      const TopoDS_Shape& aE = aLEV.First();
      if (aMEAvoid.Contains(aE)) {
        continue;
      }
      //
      aMEAlone.Add(aE);
      //
      // if this alone edge adds nothing to the intersection list
      // it means that the origin of this edge has been split and we need to
      // add the neighboring images of the same origins
      if (aDMSF.Find(aE).Extent() > 1) {
        continue;
      }
      //
      // check also its vertices
      TopoDS_Iterator aItE(aE);
      for (; aItE.More(); aItE.Next()) {
        const TopoDS_Shape& aVE = aItE.Value();
        if (aDMSF.Find(aVE).Extent() > 2) {
          break;
        }
      }
      //
      if (aItE.More()) {
        continue;
      }
      //
      // the edge is useless - look for other images
      const TopTools_ListOfShape *pLEOr = theOEOrigins.Seek(aE);
      if (!pLEOr) {
        continue;
      }
      //
      TopTools_ListIteratorOfListOfShape aItLEOr(*pLEOr);
      for (; aItLEOr.More(); aItLEOr.Next()) {
        const TopoDS_Shape& aEOr = aItLEOr.Value();
        //
        const TopTools_ListOfShape& aLEIm = theOEImages.Find(aEOr);
        TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
        for (; aItLEIm.More(); aItLEIm.Next()) {
          const TopoDS_Shape& aEIm = aItLEIm.Value();
          //
          if (aMFence.Contains(aEIm)) {
            aMEAlone.Add(aEIm);
          }
        }
      }
    }
  }
  //
  // Get all invalidities from all faces to be used for avoiding
  // repeated usage of the common edges
  TopTools_MapOfShape aMAllInvs;
  aNbInv = theInvFaces.Extent();
  for (k = 1; k <= aNbInv; ++k) {
    TopTools_ListIteratorOfListOfShape aIt(theInvFaces(k));
    for (; aIt.More(); aIt.Next()) {
      TopExp_Explorer aExp(aIt.Value(), TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        if (theInvEdges.Contains(aE) || aMEAlone.Contains(aE)) {
          aMAllInvs.Add(aE);
          TopoDS_Iterator aItV(aE);
          for (; aItV.More(); aItV.Next()) {
            aMAllInvs.Add(aItV.Value());
          }
        }
      }
    }
  }
  //
  // Bounding vertices of not trimmed edges
  TopTools_MapOfShape aMVBounds;
  //
  TopTools_MapOfShape aMECheckExt;
  // Save connections between not trimmed edge and its trimmed parts
  TopTools_DataMapOfShapeListOfShape aDMEETrim;
  // Splits of the new edges
  TopTools_DataMapOfShapeListOfShape aEImages;
  BRep_Builder aBB;
  //
  aNbInv = theInvFaces.Extent();
  for (k = 1; k <= aNbInv; ++k) {
    const TopoDS_Shape& aFInv = theInvFaces.FindKey(k);
    Standard_Boolean bSelfRebAvoid = theFSelfRebAvoid.Contains(aFInv);
    const TopTools_ListOfShape& aLFInv = theInvFaces(k);
    //
    TopTools_ListOfShape aLCB;
    if (aLFInv.Extent() > 1) {
      // make compound of invalid faces
      TopoDS_Compound aCFInv;
      aBB.MakeCompound(aCFInv);
      //
      TopTools_ListIteratorOfListOfShape aIt(aLFInv);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aFIm = aIt.Value();
        aBB.Add(aCFInv, aFIm);
      }
      //
      // make connexity blocks
      BOPTools_AlgoTools::MakeConnexityBlocks(aCFInv, TopAbs_EDGE, TopAbs_FACE, aLCB);
    }
    else {
      aLCB = aLFInv;
    }
    //
    Standard_Boolean bArtificial = theArtInvFaces.IsBound(aFInv);
    TopTools_ListIteratorOfListOfShape aItLCB(aLCB);
    for (; aItLCB.More(); aItLCB.Next()) {
      const TopoDS_Shape& aCBInv = aItLCB.Value();
      //
      TopTools_MapOfShape aMEFence;
      //
      TopoDS_Compound aCBE;
      aBB.MakeCompound(aCBE);
      //
      TopExp_Explorer aExp(aCBInv, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        if (theInvEdges.Contains(aE) || (bArtificial && aMEAlone.Contains(aE))) {
          if (aMEFence.Add(aE)) {
            aBB.Add(aCBE, aE);
          }
        }
      }
      //
      // make connexity blocks of edges
      TopTools_ListOfShape aLCBE;
      BOPTools_AlgoTools::MakeConnexityBlocks(aCBE, TopAbs_VERTEX, TopAbs_EDGE, aLCBE);
      //
      TopTools_ListIteratorOfListOfShape aItLCBE(aLCBE);
      for (; aItLCBE.More(); aItLCBE.Next()) {
        const TopoDS_Shape& aCBELoc = aItLCBE.Value();
        //
        // map of edges and vertices of processing invalidity
        TopTools_IndexedMapOfShape aME;
        // map of vertices to trim the new edges
        TopTools_IndexedMapOfShape  aMECV;
        TopExp::MapShapes(aCBELoc, TopAbs_EDGE, aME);
        aMECV = aME;
        TopExp::MapShapes(aCBELoc, TopAbs_VERTEX, aME);
        //
        // Using the map <aME> find chain of faces to be intersected;
        //
        // faces for intersection
        TopTools_IndexedMapOfShape aMFInt;
        // additional faces for intersection
        TopTools_IndexedMapOfShape aMFIntExt;
        // splits of faces for intersection
        TopTools_ListOfShape aLFInt;
        // faces to avoid intersection
        TopTools_IndexedMapOfShape aMFAvoid;
        //
        FindFacesForIntersection(aFInv, aME, theFImages, aDMSF, aMVInvAll,
          theArtInvFaces, bArtificial, theSSInterfs, aMFAvoid, aMFInt, aMFIntExt, aLFInt);
        if (aMFInt.Extent() < 3) {
          // nothing to intersect
          continue;
        }
        //
        // intersect the faces, but do not intersect the invalid ones
        // among each other (except for the artificially invalid faces)
        TopTools_IndexedMapOfShape aMEToInt;
        Standard_Integer aNb = aMFInt.Extent();
        for (i = 1; i <= aNb; ++i) {
          const TopoDS_Face& aFi = TopoDS::Face(aMFInt(i));
          if (bSelfRebAvoid && aFi.IsSame(aFInv)) {
            continue;
          }
          //
          const TopTools_ListOfShape& aLFImi = theFImages.FindFromKey(aFi);
          //
          TopTools_ListOfShape& aLFEi = aFLE.ChangeFromKey(aFi);
          //
          TopTools_ListOfShape& aLFDone = aMDone.ChangeFind(aFi);
          //
          for (j = i + 1; j <= aNb; ++j) {
            const TopoDS_Face& aFj = TopoDS::Face(aMFInt(j));
            if (bSelfRebAvoid && aFj.IsSame(aFInv)) {
              continue;
            }
            //
            const TopTools_ListOfShape& aLFImj = theFImages.FindFromKey(aFj);
            //
            TopTools_ListOfShape& aLFEj = aFLE.ChangeFromKey(aFj);
            //
            // if there are some common edges between faces
            // we should use these edges and do not intersect again.
            TopTools_ListOfShape aLEC;
            FindCommonParts(aLFImi, aLFImj, aLEC);
            //
            if (aLEC.Extent()) {
              // no need to intersect if we have common edges between faces
              Standard_Boolean bForceUse = aMFIntExt.Contains(aFi) || aMFIntExt.Contains(aFj);
              ProcessCommonEdges(aLEC, theInvEdges, theValidEdges, aME, theETrimEInf, aMEInfETrim,
                                 theOEOrigins, aMAllInvs, bForceUse, aMECV, aMECheckExt, aDMEETrim, aLFEi, aLFEj, aMEToInt);
              continue;
            }
            //
            // check if both these faces are invalid and sharing edges
            if (theInvFaces.Contains(aFi) && theInvFaces.Contains(aFj) &&
              !theArtInvFaces.IsBound(aFi) && !theArtInvFaces.IsBound(aFj)) {
              continue;
            }
            //
            // check if these two faces have already been treated
            aItLE.Initialize(aLFDone);
            for (; aItLE.More(); aItLE.Next()) {
              const TopoDS_Shape& aF = aItLE.Value();
              if (aF.IsSame(aFj)) {
                break;
              }
            }
            //
            if (aItLE.More()) {
              // use intersection line obtained on the previous steps
              // plus, find new origins for these lines
              UpdateIntersectedFaces(aFInv, aFi, aFj, aLFInv, aLFImi, aLFImj,
                                     aLFEi, aLFEj, theEdgesOrigins, aMEToInt);
              continue;
            }
            //
            if (aMFAvoid.Contains(aFi) || aMFAvoid.Contains(aFj)) {
              continue;
            }
            //
            aLFDone.Append(aFj);
            aMDone.ChangeFind(aFj).Append(aFi);
            //
            IntersectFaces(aFInv, aFi, aFj, aLFInv, aLFImi, aLFImj, 
                           aLFEi, aLFEj, theEdgesOrigins, aMECV, aMEToInt);
          }
        }
        //
        // intersect and trim edges for this chain
        IntersectAndTrimEdges(theFToRebuild, aMFInt, aMEToInt, aDMEETrim, aME, aMECV,
                              aMVInv, aMVRInv, aMECheckExt, aMVBounds, aEImages);
      }
    }
  }
  //
  // filter the obtained edges
  UpdateValidEdges(theFImages, aFLE, aMVBounds, theSolids, theInvEdges, theInvertedEdges, aMEInvOnArt,
                   aMECheckExt, theEdgesToAvoid, theEdgesOrigins, theOEImages, theOEOrigins,
                   theVertsToAvoid, theETrimEInf, aEImages, aDMEETrim, theModifiedEdges, theAsDes);
}

//=======================================================================
//function : PrepareFacesForIntersection
//purpose  : Preparation of the maps for analyzing intersections of the faces
//=======================================================================
void PrepareFacesForIntersection(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                                 const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                                 const TopTools_IndexedDataMapOfShapeListOfShape& theInvFaces,
                                 const TopTools_DataMapOfShapeShape& theArtInvFaces,
                                 const Standard_Boolean bLookVertToAvoid,
                                 TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                                 TopTools_DataMapOfShapeListOfShape& theMDone,
                                 TopTools_DataMapOfShapeListOfShape& theDMSF,
                                 TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                                 TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                                 TopTools_DataMapOfShapeShape& theETrimEInf,
                                 TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv)
{
  Standard_Integer i, aNb = theFToRebuild.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFToRebuild.FindKey(i);
    //
    TopTools_ListOfShape aLE;
    theFLE.Add(aF, aLE);
    theMDone.Bind(aF, aLE);
    //
    const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
    TopTools_ListIteratorOfListOfShape aItLF(aLFIm);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        // save connection to untrimmed face
        TopTools_ListOfShape *pLF = theDMSF.ChangeSeek(aE);
        if (!pLF) {
          pLF = theDMSF.Bound(aE, TopTools_ListOfShape());
        }
        AppendToList(*pLF, aF);
        //
        // save connection to untrimmed edge
        const TopoDS_Shape& aEInf = theETrimEInf.Find(aE);
        TopTools_ListOfShape *pLETrim = theMEInfETrim.ChangeSeek(aEInf);
        if (!pLETrim) {
          pLETrim = theMEInfETrim.Bound(aEInf, TopTools_ListOfShape());
        }
        AppendToList(*pLETrim, aE);
        //
        TopExp_Explorer aExpV(aE, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next()) {
          const TopoDS_Shape& aV = aExpV.Current();
          // save connection to face
          TopTools_ListOfShape *pLFV = theDMSF.ChangeSeek(aV);
          if (!pLFV) {
            pLFV = theDMSF.Bound(aV, TopTools_ListOfShape());
          }
          AppendToList(*pLFV, aF);
          //
          if (bLookVertToAvoid) {
            // save connection to edges
            TopTools_ListOfShape *pLEV = theDMVEFull.ChangeSeek(aV);
            if (!pLEV) {
              pLEV = theDMVEFull.Bound(aV, TopTools_ListOfShape());
            }
            AppendToList(*pLEV, aE);
          }
        }
      }
    }
    //
    if (bLookVertToAvoid) {
      // get edges of invalid faces (from invalid splits only)
      const TopTools_ListOfShape *pLFInv = theInvFaces.Seek(aF);
      if (!pLFInv || theArtInvFaces.IsBound(aF)) {
        continue;
      }
      //
      aItLF.Initialize(*pLFInv);
      for (; aItLF.More(); aItLF.Next()) {
        const TopoDS_Shape& aFInv = aItLF.Value();
        TopExp_Explorer aExp(aFInv, TopAbs_EDGE);
        for (; aExp.More(); aExp.Next()) {
          const TopoDS_Shape& aE = aExp.Current();
          TopTools_ListOfShape *pLF = theDMEFInv.ChangeSeek(aE);
          if (!pLF) {
            pLF = &theDMEFInv(theDMEFInv.Add(aE, TopTools_ListOfShape()));
          }
          AppendToList(*pLF, aF);
        }
      }
    }
  }
}

//=======================================================================
//function : FindVerticesToAvoid
//purpose  : Looking for the invalid vertices
//=======================================================================
void FindVerticesToAvoid(const TopTools_IndexedDataMapOfShapeListOfShape& theDMEFInv,
                         const TopTools_IndexedMapOfShape& theInvEdges,
                         const TopTools_IndexedMapOfShape& theValidEdges,
                         TopTools_DataMapOfShapeListOfShape& theDMVEFull,
                         TopTools_MapOfShape& theMVRInv)
{
  TopTools_MapOfShape aMFence;
  Standard_Integer i, aNb = theDMEFInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFInv = theDMEFInv(i);
    if (aLFInv.Extent() == 1) {
      continue;
    }
    //
    const TopoDS_Shape& aE = theDMEFInv.FindKey(i);
    if (!theInvEdges.Contains(aE) || theValidEdges.Contains(aE)) {
      continue;
    }
    //
    TopExp_Explorer aExp(aE, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV = aExp.Current();
      TopTools_ListOfShape *pLE = theDMVEFull.ChangeSeek(aV);
      if (!pLE) {
        theMVRInv.Add(aV);
        continue;
      }
      //
      TopTools_ListIteratorOfListOfShape aItLE(*pLE);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aEV = aItLE.Value();
        if (!theInvEdges.Contains(aEV) && !theDMEFInv.Contains(aEV)) {
          break;
        }
      }
      if (!aItLE.More()) {
        theMVRInv.Add(aV);
      }
    }
  }
}

//=======================================================================
//function : FindFacesForIntersection
//purpose  : Looking for the faces around each invalidity for intersection
//=======================================================================
void FindFacesForIntersection(const TopoDS_Shape& theFInv,
                              const TopTools_IndexedMapOfShape& theME,
                              const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                              const TopTools_DataMapOfShapeListOfShape& theDMSF,
                              const TopTools_MapOfShape& theMVInvAll,
                              const TopTools_DataMapOfShapeShape& theArtInvFaces,
                              const Standard_Boolean theArtCase,
                              const TopTools_DataMapOfShapeListOfShape& theSSInterfs,
                              TopTools_IndexedMapOfShape& theMFAvoid,
                              TopTools_IndexedMapOfShape& theMFInt,
                              TopTools_IndexedMapOfShape& theMFIntExt,
                              TopTools_ListOfShape& theLFImInt)
{
  Standard_Integer i, aNbE = theME.Extent();
  //
  TopTools_IndexedMapOfShape aMShapes;
  //
  for (i = 1; i <= aNbE; ++i) {
    const TopoDS_Shape& aS = theME(i);
    if (!theDMSF.IsBound(aS)) {
      continue;
    }
    //
    // in artificial case we intersect the faces which are close to invalidity
    Standard_Boolean bAvoid = theArtCase ? 
      ((aS.ShapeType() == TopAbs_VERTEX) && !theMVInvAll.Contains(aS)) : Standard_False;
    //
    const TopTools_ListOfShape& aLF = theDMSF.Find(aS);
    TopTools_ListIteratorOfListOfShape aItLF(aLF);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aF = aItLF.Value();
      if (theMFInt.Contains(aF)) {
        continue;
      }
      //
      if (bAvoid && theArtInvFaces.IsBound(aF)) {
        theMFAvoid.Add(aF);
      }
      //
      theMFInt.Add(aF);
      //
      Standard_Boolean bUse = !aF.IsSame(theFInv);
      //
      const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
      TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next()) {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        theLFImInt.Append(aFIm);
        if (bUse) {
          TopExp::MapShapes(aFIm, TopAbs_EDGE, aMShapes);
        }
      }
    }
  }
  //
  if (theArtCase) {
    return;
  }
  //
  const TopTools_ListOfShape* pLFInv = theSSInterfs.Seek(theFInv);
  if (!pLFInv) {
    return;
  }
  //
  TopTools_MapOfShape aMF;
  TopTools_ListIteratorOfListOfShape aItLF(*pLFInv);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Shape& aF = aItLF.Value();
    aMF.Add(aF);
  }
  //
  // the faces should be unique in each place
  TopoDS_Compound aCF;
  BRep_Builder().MakeCompound(aCF);
  //
  TopTools_IndexedMapOfShape aMFToAdd;
  TopTools_DataMapOfShapeShape aDMFOr;
  //
  for (i = 1; i <= aNbE; ++i) {
    const TopoDS_Shape& aS = theME(i);
    const TopTools_ListOfShape* pLF = theSSInterfs.Seek(aS);
    if (!pLF) {
      continue;
    }
    //
    aItLF.Initialize(*pLF);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aF = aItLF.Value();
      if (theMFInt.Contains(aF) || aMFToAdd.Contains(aF) || !aMF.Contains(aF)) {
        continue;
      }
      //
      // check if the face has some connection to already added for intersection faces
      const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
      TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next()) {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        TopExp_Explorer aExp(aFIm, TopAbs_EDGE);
        for (; aExp.More(); aExp.Next()) {
          if (aMShapes.Contains(aExp.Current())) {
            break;
          }
        }
        if (aExp.More()) {
          break;
        }
      }
      if (!aItLFIm.More()) {
        continue;
      }
      //
      aMFToAdd.Add(aF);
      aItLFIm.Initialize(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next()) {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        aDMFOr.Bind(aFIm, aF);
        BRep_Builder().Add(aCF, aFIm);
      }
    }
  }
  //
  if (aMFToAdd.IsEmpty()) {
    return;
  }
  //
  TopTools_ListOfShape aLCB;
  BOPTools_AlgoTools::MakeConnexityBlocks(aCF, TopAbs_EDGE, TopAbs_FACE, aLCB);
  //
  if ((aLCB.Extent() == 1) && (aMFToAdd.Extent() > 1)) {
    return;
  }
  //
  TopTools_ListIteratorOfListOfShape aItLCB(aLCB);
  for (; aItLCB.More(); aItLCB.Next()) {
    const TopoDS_Shape& aCB = aItLCB.Value();
    aMFToAdd.Clear();
    TopExp_Explorer aExpF(aCB, TopAbs_FACE);
    for (; aExpF.More(); aExpF.Next()) {
      const TopoDS_Shape& aFIm = aExpF.Current();
      aMFToAdd.Add(aDMFOr.Find(aFIm));
    }
    //
    if (aMFToAdd.Extent() == 1) {
      const TopoDS_Shape& aF = aMFToAdd(1);
      //
      theMFInt.Add(aF);
      theMFIntExt.Add(aF);
      //
      const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
      TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
      for (; aItLFIm.More(); aItLFIm.Next()) {
        const TopoDS_Shape& aFIm = aItLFIm.Value();
        theLFImInt.Append(aFIm);
      }
    }
  }
}

//=======================================================================
//function : ProcessCommonEdges
//purpose  : Analyzing the common edges between splits of offset faces
//=======================================================================
void ProcessCommonEdges(const TopTools_ListOfShape& theLEC,
                        const TopTools_IndexedMapOfShape& theInvEdges,
                        const TopTools_IndexedMapOfShape& theValidEdges,
                        const TopTools_IndexedMapOfShape& theME,
                        const TopTools_DataMapOfShapeShape& theETrimEInf,
                        const TopTools_DataMapOfShapeListOfShape& theMEInfETrim,
                        const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                        const TopTools_MapOfShape& theAllInvs,
                        const Standard_Boolean theForceUse,
                        TopTools_IndexedMapOfShape& theMECV,
                        TopTools_MapOfShape& theMECheckExt,
                        TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                        TopTools_ListOfShape& theLFEi,
                        TopTools_ListOfShape& theLFEj,
                        TopTools_IndexedMapOfShape& theMEToInt)
{
  TopTools_ListOfShape aLEC;
  // process common edges
  TopTools_ListIteratorOfListOfShape aItLE(theLEC);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    //
    // check first if common edges are valid
    if (theInvEdges.Contains(aEC) && !theValidEdges.Contains(aEC)) {
      continue;
    }
    //
    // common edge should have connection to current invalidity
    if (theME.Contains(aEC)) {
      aLEC.Append(aEC);
      continue;
    }
    //
    TopoDS_Iterator aItV(aEC);
    for (; aItV.More(); aItV.Next()) {
      const TopoDS_Shape& aVE = aItV.Value();
      if (theME.Contains(aVE)) {
        aLEC.Append(aEC);
        break;
      }
    }
  }
  //
  Standard_Boolean bUseOnlyInf = aLEC.IsEmpty();
  if (bUseOnlyInf) {
    if (theForceUse) {
      aLEC = theLEC;
    }
    else {
      aItLE.Initialize(theLEC);
      for (; aItLE.More(); aItLE.Next()) {
        const TopoDS_Shape& aEC = aItLE.Value();
        // check if all images of the origin of this edge
        // are not connected to any invalidity
        const TopoDS_Shape& aEInt = theETrimEInf.Find(aEC);
        const TopTools_ListOfShape& aLVE = theMEInfETrim.Find(aEInt);
        TopTools_ListIteratorOfListOfShape aItLVE(aLVE);
        for (; aItLVE.More(); aItLVE.Next()) {
          const TopoDS_Shape& aECx = aItLVE.Value();
          if (theAllInvs.Contains(aECx) || theInvEdges.Contains(aECx)) {
            return;
          }
          //
          TopoDS_Iterator aItV(aECx);
          for (; aItV.More(); aItV.Next()) {
            if (theAllInvs.Contains(aItV.Value())) {
              return;
            }
          }
        }
        // use only one element
        if (aLEC.IsEmpty()) {
          aLEC.Append(aEC);
        }
      }
    }
  }
  //
  aItLE.Initialize(aLEC);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    //
    const TopoDS_Shape& aEInt = theETrimEInf.Find(aEC);
    if (!bUseOnlyInf) {
      // find the edges of the same original edge
      // and take their vertices as well
      const TopTools_ListOfShape& aLVE = theMEInfETrim.Find(aEInt);
      TopTools_ListIteratorOfListOfShape aItLVE(aLVE);
      for (; aItLVE.More(); aItLVE.Next()) {
        const TopoDS_Shape& aECx = aItLVE.Value();
        //
        const TopTools_ListOfShape* pLEOr = theOEOrigins.Seek(aECx);
        if (!pLEOr || (pLEOr->Extent() == 1)) {
          TopExp::MapShapes(aECx, TopAbs_VERTEX, theMECV);
        }
      }
      //
      // bind unlimited edge to its trimmed part in face to update maps of 
      // images and origins in the future
      TopTools_ListOfShape* pLTAdded = theDMEETrim.ChangeSeek(aEInt);
      if (!pLTAdded) {
        pLTAdded = theDMEETrim.Bound(aEInt, TopTools_ListOfShape());
      }
      AppendToList(*pLTAdded, aEC);
    }
    else if (!theForceUse) {
      theMECheckExt.Add(aEInt);
    }
    //
    AppendToList(theLFEi, aEInt);
    AppendToList(theLFEj, aEInt);
    theMEToInt.Add(aEInt);
  }
}

//=======================================================================
//function : UpdateIntersectedFaces
//purpose  : Updating the already interfered faces
//=======================================================================
void UpdateIntersectedFaces(const TopoDS_Shape& theFInv,
                            const TopoDS_Shape& theFi,
                            const TopoDS_Shape& theFj,
                            const TopTools_ListOfShape& theLFInv,
                            const TopTools_ListOfShape& theLFImi,
                            const TopTools_ListOfShape& theLFImj,
                            const TopTools_ListOfShape& theLFEi,
                            const TopTools_ListOfShape& theLFEj,
                            TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                            TopTools_IndexedMapOfShape& theMEToInt)
{
  // Find common edges in these two lists
  TopTools_MapOfShape aMEi;
  TopTools_ListIteratorOfListOfShape aItLE(theLFEi);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aE = aItLE.Value();
    aMEi.Add(aE);
  }
  //
  // find origins
  TopTools_IndexedMapOfShape aMEToFindOrigins;
  TopTools_ListOfShape aLEToFindOrigins;
  if (!theFi.IsSame(theFInv)) {
    FindCommonParts(theLFImi, theLFInv, aLEToFindOrigins);
  }
  if (!theFj.IsSame(theFInv)) {
    FindCommonParts(theLFImj, theLFInv, aLEToFindOrigins);
  }
  //
  TopTools_ListOfShape aLEOrInit;
  aItLE.Initialize(aLEToFindOrigins);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    aMEToFindOrigins.Add(aEC);
  }
  //
  FindOrigins(theLFImi, theLFImj, aMEToFindOrigins, theEdgesOrigins, aLEOrInit);
  //
  aItLE.Initialize(theLFEj);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aE = aItLE.Value();
    if (aMEi.Contains(aE)) {
      theMEToInt.Add(aE);
      if (aLEOrInit.Extent()) {
        if (theEdgesOrigins.IsBound(aE)) {
          TopTools_ListOfShape& aLEOr = theEdgesOrigins.ChangeFind(aE);
          TopTools_ListIteratorOfListOfShape aItLEOr(aLEOrInit);
          for (; aItLEOr.More(); aItLEOr.Next()) {
            const TopoDS_Shape& aEOr = aItLEOr.Value();
            AppendToList(aLEOr, aEOr);
          }
        }
        else {
          theEdgesOrigins.Bind(aE, aLEOrInit);
        }
      }
    }
  }
}

//=======================================================================
//function : IntersectFaces
//purpose  : Intersection of the pair of faces
//=======================================================================
void IntersectFaces(const TopoDS_Shape& theFInv,
                    const TopoDS_Shape& theFi,
                    const TopoDS_Shape& theFj,
                    const TopTools_ListOfShape& theLFInv,
                    const TopTools_ListOfShape& theLFImi,
                    const TopTools_ListOfShape& theLFImj,
                    TopTools_ListOfShape& theLFEi,
                    TopTools_ListOfShape& theLFEj,
                    TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                    TopTools_IndexedMapOfShape& theMECV,
                    TopTools_IndexedMapOfShape& theMEToInt)
{
  // intersect faces
  TopAbs_State aSide = TopAbs_OUT;
  TopTools_ListOfShape aLInt1, aLInt2;
  TopoDS_Edge aNullEdge;
  BRepOffset_Tool::Inter3D(TopoDS::Face(theFi), TopoDS::Face(theFj), aLInt1, aLInt2, aSide, aNullEdge);
  //
  if (aLInt1.IsEmpty()) {
    return;
  }
  //
  // find common vertices for trimming edges
  TopTools_ListOfShape aLCV;
  TopTools_ListIteratorOfListOfShape aItLE;
  FindCommonParts(theLFImi, theLFImj, aLCV, TopAbs_VERTEX);
  if (aLCV.Extent() > 1) {
    aItLE.Initialize(aLCV);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aCV = aItLE.Value();
      theMECV.Add(aCV);
    }
  }
  //
  // find origins
  TopTools_IndexedMapOfShape aMEToFindOrigins;
  TopTools_ListOfShape aLEToFindOrigins;
  if (!theFi.IsSame(theFInv)) {
    FindCommonParts(theLFImi, theLFInv, aLEToFindOrigins);
  }
  if (!theFj.IsSame(theFInv)) {
    FindCommonParts(theLFImj, theLFInv, aLEToFindOrigins);
  }
  TopTools_ListOfShape aLEOrInit;
  aItLE.Initialize(aLEToFindOrigins);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEC = aItLE.Value();
    aMEToFindOrigins.Add(aEC);
  }
  //
  FindOrigins(theLFImi, theLFImj, aMEToFindOrigins, theEdgesOrigins, aLEOrInit);
  //
  aItLE.Initialize(aLInt1);
  for (; aItLE.More(); aItLE.Next()) {
    const TopoDS_Shape& aEInt = aItLE.Value();
    theLFEi.Append(aEInt);
    theLFEj.Append(aEInt);
    //
    if (aLEOrInit.Extent()) {
      theEdgesOrigins.Bind(aEInt, aLEOrInit);
    }
    //
    theMEToInt.Add(aEInt);
  }
}

//=======================================================================
//function : FindOrigins
//purpose  : Looking for the origin edges
//=======================================================================
void FindOrigins(const TopTools_ListOfShape& theLFIm1,
                 const TopTools_ListOfShape& theLFIm2,
                 const TopTools_IndexedMapOfShape& theME,
                 const TopTools_DataMapOfShapeListOfShape& theOrigins,
                 TopTools_ListOfShape& theLEOr)
{
  Standard_Integer i;
  TopTools_MapOfShape aMFence;
  TopExp_Explorer aExp;
  TopTools_ListIteratorOfListOfShape aIt, aItLE;
  //
  for (i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLF = !i ? theLFIm1 : theLFIm2;
    aIt.Initialize(aLF);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aF = aIt.Value();
      //
      aExp.Init(aF, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aE = aExp.Current();
        //
        if (theME.Contains(aE) && theOrigins.IsBound(aE)) {
          const TopTools_ListOfShape& aLEOr = theOrigins.Find(aE);
          aItLE.Initialize(aLEOr);
          for (; aItLE.More(); aItLE.Next()) {
            const TopoDS_Shape& aEOr = aItLE.Value();
            //
            if (aMFence.Add(aEOr) && (aEOr.ShapeType() == TopAbs_EDGE)) {
              theLEOr.Append(aEOr);
            }
          } // for (; aItLE.More(); aItLE.Next()) {
        } // if (theME.Contains(aE) && theOrigins.IsBound(aE)) {
      } // aExp.Init(aF, TopAbs_EDGE);
    } // for (; aIt.More(); aIt.Next()) {
  } // for (i = 0; i < 2; ++i) {
}

//=======================================================================
//function : IntersectAndTrimEdges
//purpose  : Intersection of the new intersection edges among themselves
//=======================================================================
void IntersectAndTrimEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFToRebuild,
                           const TopTools_IndexedMapOfShape& theMFInt,
                           const TopTools_IndexedMapOfShape& theMEInt,
                           const TopTools_DataMapOfShapeListOfShape& theDMEETrim,
                           const TopTools_IndexedMapOfShape& theMSInv,
                           const TopTools_IndexedMapOfShape& theMVE,
                           const TopTools_MapOfShape& theVertsToAvoid,
                           const TopTools_MapOfShape& theNewVertsToAvoid,
                           const TopTools_MapOfShape& theMECheckExt,
                           TopTools_MapOfShape& theMVBounds,
                           TopTools_DataMapOfShapeListOfShape& theEImages)
{
  Standard_Integer i, aNb = theMEInt.Extent();
  if (!aNb) {
    return;
  }
  //
  BOPCol_ListOfShape aLArgs;
  TopTools_MapOfShape aMFence;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  TopExp_Explorer aExp;
  //
  // get vertices from the splits of intersected faces.
  // vertices are taken from the edges close to invalidity
  //
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  aNb = theMFInt.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theMFInt(i);
    const TopTools_ListOfShape& aLE = theFToRebuild.FindFromKey(aF);
    //
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
      //
      aExp.Init(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV1 = aExp.Current();
        if (!theVertsToAvoid.Contains(aV1) && theMVE.Contains(aV1) && aMFence.Add(aV1)) {
          aLArgs.Append(aV1);
        }
      }
    }
  }
  //
  aNb = theMSInv.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aV = theMSInv(i);
    if (aV.ShapeType() != TopAbs_VERTEX) {
      continue;
    }
    //
    TopTools_ListOfShape *pLVE = aDMVE.ChangeSeek(aV);
    if (!pLVE) {
      continue;
    }
    //
    aIt.Initialize(*pLVE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      //
      aExp.Init(aE, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aV1 = aExp.Current();
        if (!theVertsToAvoid.Contains(aV1) && aMFence.Add(aV1)) {
          aLArgs.Append(aV1);
        }
      }
    }
  }
  //
  // bounding vertices of untrimmed edges
  TopTools_ListOfShape aLVBounds;
  // new intersection edges
  TopTools_ListOfShape aLENew;
  // get edges to intersect
  TopTools_ListOfShape aLEInt;
  // Common intersection edges. Should be intersected separetely
  TopTools_ListOfShape aLCE;
  //
  aNb = theMEInt.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aE = theMEInt(i);
    if (theMECheckExt.Contains(aE)) {
      // avoid trimming of the intersection edges by additional common edges
      aLCE.Append(aE);
      continue;
    }
    //
    if (!theDMEETrim.IsBound(aE)) {
      aLENew.Append(aE);
    }
    //
    aLEInt.Append(aE);
    aLArgs.Append(aE);
    //
    aExp.Init(aE, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV = aExp.Current();
      aLVBounds.Append(aV);
    }
  }
  //
  // Intersect Edges
  BOPAlgo_Builder aGF;
  aGF.SetArguments(aLArgs);
  aGF.Perform();
  if (aGF.HasErrors()) {
    return;
  }
  //
  // update vertices to avoid with SD vertices
  aIt.Initialize(aLVBounds);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aV = aIt.Value();
    const TopTools_ListOfShape& aLVIm = aGF.Modified(aV);
    if (aLVIm.IsEmpty()) {
      theMVBounds.Add(aV);
    }
    else {
      const TopoDS_Shape& aVIm = aLVIm.First();
      theMVBounds.Add(aVIm);
    }
  }
  //
  // find invalid splits of edges
  TopTools_MapOfShape aMEInv;
  GetInvalidEdges(theNewVertsToAvoid, theMVBounds, aGF, aMEInv);
  //
  BRep_Builder aBB;
  // get valid splits to intersect with the commons
  TopoDS_Compound aCEIm;
  aBB.MakeCompound(aCEIm);
  //
  // remove the splits containing vertices from invalid edges
  aIt.Initialize(aLEInt);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    //
    TopTools_ListOfShape aLEIm = aGF.Modified(aE);
    if (aLEIm.IsEmpty()) {
      continue;
    }
    //
    aIt1.Initialize(aLEIm);
    for (; aIt1.More(); ) {
      const TopoDS_Shape& aEIm = aIt1.Value();
      //
      if (aMEInv.Contains(aEIm)) {
        aLEIm.Remove(aIt1);
      }
      else {
        aBB.Add(aCEIm, aEIm);
        aIt1.Next();
      }
    }
    //
    if (aLEIm.Extent()) {
      TopTools_ListOfShape* pLEIm = theEImages.ChangeSeek(aE);
      if (!pLEIm) {
        pLEIm = theEImages.Bound(aE, TopTools_ListOfShape());
      }
      pLEIm->Append(aLEIm);
    }
  }
  //
  if (!aLCE.Extent()) {
    return;
  }
  //
  // trim common edges by other intersection edges
  BOPAlgo_Builder aGFCE;
  aGFCE.SetArguments(aLCE);
  aGFCE.AddArgument(aCEIm);
  aGFCE.Perform();
  //
  if (aGFCE.HasErrors()) {
    return;
  }
  //
  const BOPDS_PDS& pDS = aGFCE.PDS();
  TopTools_ListIteratorOfListOfShape aItLCE(aLCE);
  for (; aItLCE.More(); aItLCE.Next()) {
    const TopoDS_Shape& aE = aItLCE.Value();
    TopTools_ListOfShape aLEIm = aGFCE.Modified(aE);
    if (aLEIm.IsEmpty()) {
      continue;
    }
    //
    // check if it's not coincide with some intersection edge
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(pDS->PaveBlocks(pDS->Index(aE)));
    for (; aItLPB.More(); aItLPB.Next()) {
      if (pDS->IsCommonBlock(aItLPB.Value())) {
        // find with what it is a common
        const BOPDS_ListOfPaveBlock& aLPBC = pDS->CommonBlock(aItLPB.Value())->PaveBlocks();
        BOPDS_ListIteratorOfListOfPaveBlock aItLPBC(aLPBC);
        for (; aItLPBC.More(); aItLPBC.Next()) {
          const TopoDS_Shape& aEC = pDS->Shape(aItLPBC.Value()->OriginalEdge());
          if (!theMECheckExt.Contains(aEC)) {
            break;
          }
        }
        if (aItLPBC.More()) {
          break;
        }
      }
    }
    if (aItLPB.More()) {
      // avoid creation of unnecessary splits from commons which
      // coincide with intersection edges
      continue;
    }
    //
    // save the images
    TopTools_ListOfShape* pLEIm = theEImages.ChangeSeek(aE);
    if (!pLEIm) {
      pLEIm = theEImages.Bound(aE, TopTools_ListOfShape());
    }
    pLEIm->Append(aLEIm);
    //
    // save bounding vertices
    for (TopoDS_Iterator aItV(aE); aItV.More(); aItV.Next()) {
      const TopoDS_Shape& aV = aItV.Value();
      const TopTools_ListOfShape& aLVIm = aGFCE.Modified(aV);
      theMVBounds.Add(aLVIm.IsEmpty() ? aV : aLVIm.First());
    }
  }
}

//=======================================================================
//function : GetInvalidEdges
//purpose  : Looking for the invalid edges by intersecting with invalid vertices
//=======================================================================
void GetInvalidEdges(const TopTools_MapOfShape& theVertsToAvoid,
                     const TopTools_MapOfShape& theMVBounds,
                     BOPAlgo_Builder& theGF,
                     TopTools_MapOfShape& theMEInv)
{
  if (theVertsToAvoid.IsEmpty()) {
    return;
  }
  //
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  // get vertices created with intersection edges
  const TopoDS_Shape& aRes = theGF.Shape();
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  TopExp::MapShapesAndAncestors(aRes, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  const BOPDS_PDS& pDS = theGF.PDS();
  //
  // find invalid splits of edges
  // check if the vertex is invalid:
  // a. it may be the vertex SD with the vertices to avoid
  // b. or it may be the vertex which is created by the intersection 
  //    of only existing edges, i.e. no new intersection edges goes
  //    through this vertex
  //
  TopTools_MapOfShape aMVInv;
  Standard_Integer i, aNb = aDMVE.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aDMVE.FindKey(i));
    if (theMVBounds.Contains(aV)) {
      continue;
    }
    //
    Standard_Integer nV = pDS->Index(aV);
    if ((nV >= 0) && !pDS->IsNewShape(nV)) {
      continue;
    }
    //
    TopTools_MapIteratorOfMapOfShape aItM(theVertsToAvoid);
    for (; aItM.More(); aItM.Next()) {
      const TopoDS_Vertex& aVInv = *(TopoDS_Vertex*)&aItM.Value();
      Standard_Integer iFlag = BOPTools_AlgoTools::ComputeVV(aV, aVInv);
      if (!iFlag) {
        aMVInv.Add(aV);
        break;
      }
    }
    //
    if (aItM.More()) {
      const TopTools_ListOfShape& aLVE = aDMVE.FindFromKey(aV);
      aIt.Initialize(aLVE);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aE = aIt.Value();
        theMEInv.Add(aE);
      }
    }
  }
}

//=======================================================================
//function : UpdateValidEdges
//purpose  : Making the new splits and updating the maps
//=======================================================================
void UpdateValidEdges(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                      const TopTools_IndexedDataMapOfShapeListOfShape& theFLE,
                      const TopTools_MapOfShape& theMVBounds,
                      const TopoDS_Shape& theSolids,
                      const TopTools_IndexedMapOfShape& theInvEdges,
                      const TopTools_MapOfShape& theInvertedEdges,
                      const TopTools_MapOfShape& theMEInvOnArt,
                      TopTools_MapOfShape& theMECheckExt,
                      TopTools_IndexedMapOfShape& theEdgesToAvoid,
                      TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                      TopTools_DataMapOfShapeListOfShape& theOEImages,
                      TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                      TopTools_MapOfShape& theVertsToAvoid,
                      TopTools_DataMapOfShapeShape& theETrimEInf,
                      TopTools_DataMapOfShapeListOfShape& theEImages,
                      TopTools_DataMapOfShapeListOfShape& theEETrim,
                      TopTools_MapOfShape& theModifiedEdges,
                      Handle(BRepAlgo_AsDes)& theAsDes)
{
  // update images and origins of edges, plus update AsDes
  //
  // new edges
  TopTools_ListOfShape aLE;
  // back connection from edges to faces
  TopTools_DataMapOfShapeListOfShape aMELF;
  //
  TopTools_MapOfShape aMETmp;
  Standard_Integer i, aNb = theFLE.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Face& aF = TopoDS::Face(theFLE.FindKey(i));
    //
    const TopTools_ListOfShape& aLEInt = theFLE(i);
    TopTools_ListIteratorOfListOfShape aItLE(aLEInt);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aE = aItLE.Value();
      if ((theMECheckExt.Contains(aE) || aMETmp.Contains(aE)) && !theEImages.IsBound(aE)) {
        theMECheckExt.Remove(aE);
        aMETmp.Add(aE);
        continue;
      }
      TopTools_ListOfShape* pLF = aMELF.ChangeSeek(aE);
      if (!pLF) {
        pLF = aMELF.Bound(aE, TopTools_ListOfShape());
        aLE.Append(aE);
      }
      pLF->Append(aF);
    }
  }
  //
  if (aLE.IsEmpty()) {
    return;
  }
  //
  // bounding edges, that are going to be replaced
  TopTools_MapOfShape aMEB;
  //
  // new intersection edges
  TopTools_ListOfShape aLENew;
  TopTools_MapOfShape aMENew;
  // map of old vertices
  TopTools_MapOfShape aMVOld;
  // back connection to untrimmed edges
  TopTools_DataMapOfShapeListOfShape aDMEOr;
  //
  // trim the new intersection edges
  BOPCol_ListOfShape aLA;
  TrimNewIntersectionEdges(aLE, theEETrim, theMVBounds, theMECheckExt, theEImages,
                           aMEB, aMVOld, aLENew, aLA, aDMEOr, aMELF);
  //
  if (aLA.IsEmpty()) {
    // update intersection edges
    UpdateNewIntersectionEdges(aLE, aMELF, theEImages, theInvEdges, theInvertedEdges, theEdgesOrigins,
      theOEImages, theOEOrigins, theETrimEInf, theEETrim, theModifiedEdges, theAsDes);
    return;
  }
  //
  TopoDS_Shape aSplits1;
  if (aLA.Extent() > 1) {
    // intersect the new splits among themselves to avoid self-intersections
    IntersectEdges(aLA, aLE, aLENew, theMVBounds, theVertsToAvoid, theMECheckExt,
                   theEImages, theModifiedEdges, aDMEOr, aMELF, aMENew, aSplits1);
  }
  else {
    aSplits1 = aLA.First();
  }
  //
  // filter the new splits with bounds
  TopoDS_Shape aFilterBounds;
  GetBounds(theFImages, aMEB, aFilterBounds);
  //
  // intersect splits and bounds and remove those splits which have pure E/E intersection
  TopTools_MapOfShape aMEInv;
  GetInvalidEdgesByBounds(aSplits1, aFilterBounds, theFImages, theSolids,
                          theInvEdges, aMVOld, aMENew, aDMEOr, aMELF, theEImages,
                          theMECheckExt, theMEInvOnArt, theVertsToAvoid, aMEInv);
  //
  // get valid edges only
  TopoDS_Shape aSplits;
  if (aMEInv.Extent()) {
    // clear images from found invalid edges
    TopoDS_Compound aSp;
    BRep_Builder().MakeCompound(aSp);
    TopTools_MapOfShape aMFence;
    //
    TopTools_ListIteratorOfListOfShape aItLE(aLE);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aE = aItLE.Value();
      //
      TopTools_ListOfShape* pLEIm = theEImages.ChangeSeek(aE);
      if (!pLEIm) {
        continue;
      }
      //
      TopTools_ListIteratorOfListOfShape aItLEIm(*pLEIm);
      for (; aItLEIm.More();) {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        if (aMEInv.Contains(aEIm)) {
          pLEIm->Remove(aItLEIm);
        }
        else {
          if (aMFence.Add(aEIm)) {
            BRep_Builder().Add(aSp, aEIm);
          }
          aItLEIm.Next();
        }
      }
      //
      if (pLEIm->IsEmpty()) {
        theEImages.UnBind(aE);
      }
    }
    aSplits = aSp;
  }
  else {
    aSplits = aSplits1;
  }
  //
  // get bounds to update
  // we need to update the edges of all the affected faces
  TopTools_ListOfShape aLF;
  // prepare the vertices from new splits of edges
  TopTools_IndexedMapOfShape aMVSp;
  TopExp::MapShapes(aSplits, TopAbs_VERTEX, aMVSp);
  //
  Standard_Integer aNbF = theFImages.Extent();
  for (i = 1; i <= aNbF; ++i) {
    const TopoDS_Shape& aF = theFImages.FindKey(i);
    if (theFLE.Contains(aF)) {
      aLF.Append(aF);
      continue;
    }
    //
    // check the splits of faces to have vertices from splits
    const TopTools_ListOfShape& aLFIm = theFImages(i);
    TopTools_ListIteratorOfListOfShape aItLFIm(aLFIm);
    for (; aItLFIm.More(); aItLFIm.Next()) {
      const TopoDS_Shape& aFIm = aItLFIm.Value();
      //
      TopExp_Explorer aExpV(aFIm, TopAbs_VERTEX);
      for (; aExpV.More(); aExpV.Next()) {
        const TopoDS_Shape& aV = aExpV.Current();
        if (aMVSp.Contains(aV)) {
          break;
        }
      }
      //
      if (aExpV.More()) {
        break;
      }
    }
    //
    if (aItLFIm.More()) {
      aLF.Append(aF);
    }
  }
  //
  // get bounds from splits of faces of aLF
  TopoDS_Shape aBounds;
  TopTools_ListOfShape aLAValid, aLABounds;
  GetBoundsToUpdate(aLF, theOEImages, theOEOrigins, aMEB,
                    aLABounds, aLAValid, aBounds, theAsDes);
  //
  // intersect valid splits with bounds and update both
  BOPAlgo_Builder aGF;
  // The order is important here, because we need to keep the
  // unmodified edges from the Bounds in the resulting maps.
  // In case of total coincidence of the edges with the same vertices
  // the edges in the common block will not be split and no new
  // edges will be created and the first pave block
  // will be used as a real pave block.
  aGF.AddArgument(aBounds);
  aGF.AddArgument(aSplits);
  aGF.Perform();
  //
  // update splits
  UpdateImages(aLE, theEImages, aGF, theModifiedEdges);
  //
  // update new intersection edges
  UpdateNewIntersectionEdges(aLE, aMELF, theEImages, theInvEdges, theInvertedEdges, theEdgesOrigins,
    theOEImages, theOEOrigins, theETrimEInf, theEETrim, theModifiedEdges, theAsDes);
  //
  // update bounds
  UpdateImages(aLAValid, theOEImages, aGF, theModifiedEdges);
  UpdateOrigins(aLABounds, theOEOrigins, aGF);
  UpdateOrigins(aLABounds, theEdgesOrigins, aGF);
  UpdateIntersectedEdges(aLABounds, theETrimEInf, aGF);
  //
  // update the EdgesToAvoid with the splits
  TopTools_IndexedMapOfShape aNewEdges;
  const TopTools_ListOfShape* pSplitsIm = aGF.Images().Seek(aSplits);
  if (pSplitsIm) {
    TopTools_ListIteratorOfListOfShape aItSpIm(*pSplitsIm);
    for (; aItSpIm.More(); aItSpIm.Next()) {
      TopExp::MapShapes(aItSpIm.Value(), TopAbs_EDGE, aNewEdges);
    }
  }
  //
  Standard_Integer aNbE = theEdgesToAvoid.Extent();
  for (i = 1; i <= aNbE; ++i) {
    const TopoDS_Shape& aE = theEdgesToAvoid(i);
    const TopTools_ListOfShape& aLEIm = aGF.Modified(aE);
    TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
    for (; aItLEIm.More(); aItLEIm.Next()) {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      if (!aNewEdges.Contains(aEIm)) {
        theEdgesToAvoid.Add(aEIm);
      }
    }
  }
}

//=======================================================================
//function : TrimNewIntersectionEdges
//purpose  : 
//=======================================================================
void TrimNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                              const TopTools_DataMapOfShapeListOfShape& theEETrim,
                              const TopTools_MapOfShape& theMVBounds,
                              TopTools_MapOfShape& theMECheckExt,
                              TopTools_DataMapOfShapeListOfShape& theEImages,
                              TopTools_MapOfShape& theMEB,
                              TopTools_MapOfShape& theMVOld,
                              TopTools_ListOfShape& theLENew,
                              BOPCol_ListOfShape& theLA,
                              TopTools_DataMapOfShapeListOfShape& theDMEOr,
                              TopTools_DataMapOfShapeListOfShape& theMELF)
{
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  aIt.Initialize(theLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    //
    Standard_Boolean bCheckExt = theMECheckExt.Remove(aE);
    //
    Standard_Boolean bOld = theEETrim.IsBound(aE);
    if (bOld) {
      const TopTools_ListOfShape& aLET = theEETrim.Find(aE);
      aIt1.Initialize(aLET);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aET = aIt1.Value();
        theMEB.Add(aET);
        TopExp_Explorer aExpV(aET, TopAbs_VERTEX);
        for (; aExpV.More(); aExpV.Next()) {
          const TopoDS_Shape& aV = aExpV.Current();
          theMVOld.Add(aV);
        }
      }
    }
    //
    if (!theEImages.IsBound(aE)) {
      continue;
    }
    //
    TopTools_ListOfShape& aLEIm = theEImages.ChangeFind(aE);
    if (aLEIm.IsEmpty()) {
      theEImages.UnBind(aE);
      continue;
    }
    //
    TopoDS_Shape aCEIm;
    TopTools_MapOfShape aMEVBounds;
    //
    if (aLEIm.Extent() > 1) {
      TopTools_IndexedMapOfShape aMV;
      // fuse these parts
      BOPAlgo_Builder aGFE;
      TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        aGFE.AddArgument(aEIm);
        TopExp::MapShapes(aEIm, TopAbs_VERTEX, aMV);
      }
      //
      // add two bounding vertices of this edge to the operation
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(TopoDS::Edge(aE), aV1, aV2);
      //
      aGFE.AddArgument(aV1);
      aGFE.AddArgument(aV2);
      aMV.Add(aV1);
      aMV.Add(aV2);
      //
      aGFE.Perform();
      if (!aGFE.HasErrors()) {
        // get images of bounding vertices to remove splits containing them
        // in case some of the bounding edges has been interfered
        // during operation it is necessary to update their images as well
        Standard_Integer iV, aNbV = aMV.Extent();
        for (iV = 1; iV <= aNbV; ++iV) {
          const TopoDS_Shape& aV = aMV(iV);
          if (theMVBounds.Contains(aV) || aV.IsSame(aV1) || aV.IsSame(aV2)) {
            const TopTools_ListOfShape& aLVIm = aGFE.Modified(aV);
            aMEVBounds.Add(aLVIm.IsEmpty() ? aV : aLVIm.First());
          }
        }
        //
        aCEIm = aGFE.Shape();
      }
    }
    else {
      aCEIm = aLEIm.First();
    }
    //
    aLEIm.Clear();
    //
    TopExp_Explorer aExp(aCEIm, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aEIm = aExp.Current();
      //
      // check the split not to contain bounding vertices
      TopoDS_Iterator aItV(aEIm);
      for (; aItV.More(); aItV.Next()) {
        const TopoDS_Shape& aV = aItV.Value();
        if (aMEVBounds.Contains(aV) || theMVBounds.Contains(aV)) {
          break;
        }
      }
      //
      if (!aItV.More()) {
        theLA.Append(aEIm);
        aLEIm.Append(aEIm);
        //
        theDMEOr.Bound(aEIm, TopTools_ListOfShape())->Append(aE);
      }
    }
    //
    if (aLEIm.IsEmpty()) {
      theEImages.UnBind(aE);
    }
    else {
      const TopTools_ListOfShape& aLFE = theMELF.Find(aE);
      TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        TopTools_ListOfShape* pLFEIm = theMELF.ChangeSeek(aEIm);
        if (!pLFEIm) {
          pLFEIm = theMELF.Bound(aEIm, TopTools_ListOfShape());
        }
        TopTools_ListIteratorOfListOfShape aItLF(aLFE);
        for (; aItLF.More(); aItLF.Next()) {
          AppendToList(*pLFEIm, aItLF.Value());
        }
        //
        if (bCheckExt) {
          theMECheckExt.Add(aEIm);
        }
        else if (!bOld) {
          theLENew.Append(aEIm);
        }
      }
    }
  }
}

//=======================================================================
//function : IntersectEdges
//purpose  : Intersecting the trimmed edges to avoid self-intersections
//=======================================================================
void IntersectEdges(const BOPCol_ListOfShape& theLA,
                    const TopTools_ListOfShape& theLE,
                    const TopTools_ListOfShape& theLENew,
                    const TopTools_MapOfShape& theMVBounds,
                    const TopTools_MapOfShape& theVertsToAvoid,
                    TopTools_MapOfShape& theMECheckExt,
                    TopTools_DataMapOfShapeListOfShape& theEImages,
                    TopTools_MapOfShape& theModifiedEdges,
                    TopTools_DataMapOfShapeListOfShape& theDMEOr,
                    TopTools_DataMapOfShapeListOfShape& theMELF,
                    TopTools_MapOfShape& theMENew,
                    TopoDS_Shape& theSplits)
{
  BOPAlgo_Builder aGFA;
  aGFA.SetArguments(theLA);
  aGFA.Perform();
  if (aGFA.HasErrors()) {
    // just copy input to the result
    TopoDS_Compound aSp;
    BRep_Builder aBB;
    aBB.MakeCompound(aSp);
    BOPCol_ListIteratorOfListOfShape anIt(theLA);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aE = anIt.Value();
      aBB.Add(aSp, aE);
    }
    theSplits = aSp;
    return;
  }
  //
  UpdateImages(theLE, theEImages, aGFA, theModifiedEdges);
  //
  // compound of valid splits
  theSplits = aGFA.Shape();
  //
  // update new edges
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  aIt.Initialize(theLENew);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    const TopTools_ListOfShape& aLEIm = aGFA.Modified(aE);
    if (aLEIm.Extent()) {
      aIt1.Initialize(aLEIm);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aEIm = aIt1.Value();
        theMENew.Add(aEIm);
      }
    }
    else {
      theMENew.Add(aE);
    }
  }
  //
  // update edges after intersection for extended checking
  aIt.Initialize(theLA);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    const TopTools_ListOfShape& aLEIm = aGFA.Modified(aE);
    if (aLEIm.IsEmpty()) {
      continue;
    }
    //
    if (theMECheckExt.Contains(aE)) {
      aIt1.Initialize(aLEIm);
      for (; aIt1.More(); aIt1.Next()) {
        theMECheckExt.Add(aIt1.Value());
      }
      theMECheckExt.Remove(aE);
    }
    //
    const TopTools_ListOfShape& aLFE = theMELF.Find(aE);
    aIt1.Initialize(aLEIm);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aEIm = aIt1.Value();
      TopTools_ListOfShape* pLFEIm = theMELF.ChangeSeek(aEIm);
      if (!pLFEIm) {
        pLFEIm = theMELF.Bound(aEIm, TopTools_ListOfShape());
      }
      TopTools_ListIteratorOfListOfShape aItLF(aLFE);
      for (; aItLF.More(); aItLF.Next()) {
        AppendToList(*pLFEIm, aItLF.Value());
      }
    }
  }
  //
  TopTools_MapOfShape aMEInv;
  GetInvalidEdges(theVertsToAvoid, theMVBounds, aGFA, aMEInv);
  if (aMEInv.Extent()) {
    // update shape
    TopoDS_Compound aSp;
    BRep_Builder aBB;
    aBB.MakeCompound(aSp);
    TopExp_Explorer aExp(theSplits, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aE = aExp.Current();
      if (!aMEInv.Contains(aE)) {
        aBB.Add(aSp, aE);
      }
    }
    theSplits = aSp;
  }
  //
  UpdateOrigins(theLA, theDMEOr, aGFA);
}

//=======================================================================
//function : GetBounds
//purpose  : Getting edges from the splits of offset faces
//=======================================================================
void GetBounds(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
               const TopTools_MapOfShape& theMEB,
               TopoDS_Shape& theBounds)
{
  // make compound of edges contained in the splits of faces
  TopoDS_Compound aBounds;
  BRep_Builder aBB;
  aBB.MakeCompound(aBounds);
  //
  TopTools_MapOfShape aMFence;
  //
  Standard_Integer i, aNb = theFImages.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLFIm = theFImages(i);
    TopTools_ListIteratorOfListOfShape aIt(aLFIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aFIm = aIt.Value();
      //
      TopExp_Explorer aExpE(aFIm, TopAbs_EDGE);
      for (; aExpE.More(); aExpE.Next()) {
        const TopoDS_Shape& aEIm = aExpE.Current();
        if (!theMEB.Contains(aEIm) && aMFence.Add(aEIm)) {
          aBB.Add(aBounds, aEIm);
        }
      }
    }
  }
  theBounds = aBounds;
}

//=======================================================================
//function : GetBoundsToUpdate
//purpose  : Get bounding edges that should be updated
//=======================================================================
void GetBoundsToUpdate(const TopTools_ListOfShape& theLF,
                       const TopTools_DataMapOfShapeListOfShape& theOEImages,
                       const TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                       const TopTools_MapOfShape& theMEB,
                       TopTools_ListOfShape& theLABounds,
                       TopTools_ListOfShape& theLAValid,
                       TopoDS_Shape& theBounds,
                       Handle(BRepAlgo_AsDes)& theAsDes)
{
  // get all edges
  TopoDS_Compound aBounds;
  BRep_Builder aBB;
  aBB.MakeCompound(aBounds);
  //
  TopTools_MapOfShape aMAValid, aMFence;
  //
  TopTools_ListIteratorOfListOfShape aItLF(theLF);
  for (; aItLF.More(); aItLF.Next()) {
    const TopoDS_Shape& aF = aItLF.Value();
    //
    TopTools_IndexedMapOfShape aMDE;
    const TopTools_ListOfShape& aLFDes = theAsDes->Descendant(aF);
    TopTools_ListIteratorOfListOfShape aItLFDes(aLFDes);
    for (; aItLFDes.More(); aItLFDes.Next()) {
      const TopoDS_Shape& aED = aItLFDes.Value();
      const TopTools_ListOfShape *pLEDIm = theOEImages.Seek(aED);
      if (!pLEDIm) {
        aMDE.Add(aED);
        continue;
      }
      //
      TopTools_ListIteratorOfListOfShape aItLEDIm(*pLEDIm);
      for (; aItLEDIm.More(); aItLEDIm.Next()) {
        const TopoDS_Shape& aEDIm = aItLEDIm.Value();
        aMDE.Add(aEDIm);
      }
    }
    //
    Standard_Integer j, aNbE = aMDE.Extent();
    for (j = 1; j <= aNbE; ++j) {
      const TopoDS_Edge& aEIm = TopoDS::Edge(aMDE(j));
      //
      if (!theMEB.Contains(aEIm) && aMFence.Add(aEIm)) {
        aBB.Add(aBounds, aEIm);
        theLABounds.Append(aEIm);
      }
      //
      const TopTools_ListOfShape *pLO = theOEOrigins.Seek(aEIm);
      if (pLO) {
        TopTools_ListIteratorOfListOfShape aItLO(*pLO);
        for (; aItLO.More(); aItLO.Next()) {
          const TopoDS_Shape& aEO = aItLO.Value();
          //
          if (aMAValid.Add(aEO)) {
            theLAValid.Append(aEO);
          }
        }
      }
      else {
        if (aMAValid.Add(aEIm)) {
          theLAValid.Append(aEIm);
        }
      }
    }
  }
  theBounds = aBounds;
}

//=======================================================================
//function : GetInvalidEdgesByBounds
//purpose  : Filter new splits by intersection with bounds
//=======================================================================
void GetInvalidEdgesByBounds(const TopoDS_Shape& theSplits,
                             const TopoDS_Shape& theBounds,
                             const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                             const TopoDS_Shape& theSolids,
                             const TopTools_IndexedMapOfShape& theInvEdges,
                             const TopTools_MapOfShape& theMVOld,
                             const TopTools_MapOfShape& theMENew,
                             const TopTools_DataMapOfShapeListOfShape& theDMEOr,
                             const TopTools_DataMapOfShapeListOfShape& theMELF,
                             const TopTools_DataMapOfShapeListOfShape& theEImages,
                             const TopTools_MapOfShape& theMECheckExt,
                             const TopTools_MapOfShape& theMEInvOnArt,
                             TopTools_MapOfShape& theVertsToAvoid,
                             TopTools_MapOfShape& theMEInv)
{
  // map splits to check the vertices of edges
  TopTools_IndexedDataMapOfShapeListOfShape aDMVE;
  TopExp::MapShapesAndAncestors(theSplits, TopAbs_VERTEX, TopAbs_EDGE, aDMVE);
  //
  BOPAlgo_Section aSec;
  aSec.AddArgument(theSplits);
  aSec.AddArgument(theBounds);
  //
  aSec.Perform();
  //
  // invalid vertices
  TopTools_IndexedMapOfShape aMVInv;
  // vertices to check additionally by classification relatively to solid
  TopTools_MapOfShape aMVCheckAdd;
  // collect parts for removal
  const BOPDS_PDS& pDS = aSec.PDS();
  //
  // check edge/edge intersections
  const BOPDS_VectorOfInterfEE& aEEs = pDS->InterfEE();
  Standard_Integer i, aNb = aEEs.Extent();
  for (i = 0; i < aNb; ++i) {
    const BOPDS_InterfEE& aEE = aEEs(i);
    //
    const TopoDS_Shape& aE1 = pDS->Shape(aEE.Index1());
    const TopoDS_Shape& aE2 = pDS->Shape(aEE.Index2());
    //
    if (!aEE.HasIndexNew()) {
      if (theMECheckExt.Contains(aE1) && (aEE.CommonPart().Type() == TopAbs_EDGE)) {
        theMEInv.Add(aE1);
      }
      continue;
    }
    //
    if (theInvEdges.Contains(aE2)) {
      theMEInv.Add(aE1);
    }
    //
    if (theMEInvOnArt.Contains(aE2)) {
      // avoid checking of the vertices of the split edge intersected by
      // the invalid edge from artificial face
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(TopoDS::Edge(aE2), aV1, aV2);
      if (aDMVE.Contains(aV1) && aDMVE.Contains(aV2)) {
        continue;
      }
    }
    //
    // add vertices of all images of the edge from splits for checking
    const TopTools_ListOfShape& aLEOr = theDMEOr.Find(aE1);
    TopTools_ListIteratorOfListOfShape aItLEOr(aLEOr);
    for (; aItLEOr.More(); aItLEOr.Next()) {
      const TopoDS_Shape& aEOr = aItLEOr.Value();
      //
      const TopTools_ListOfShape& aLEIm = theEImages.Find(aEOr);
      TopTools_ListIteratorOfListOfShape aItLEIm(aLEIm);
      for (; aItLEIm.More(); aItLEIm.Next()) {
        const TopoDS_Shape& aEIm = aItLEIm.Value();
        //
        TopoDS_Iterator aItV(aEIm);
        for (; aItV.More(); aItV.Next()) {
          const TopoDS_Shape& aV = aItV.Value();
          if (!theMVOld.Contains(aV)) {
            aMVInv.Add(aV);
            aMVCheckAdd.Add(aV);
          }
        }
      }
    }
  }
  //
  // to avoid unnecessary filling of parts due to extra trim of the edges
  // process Edge/Edge interferences of type EDGE, i.e. common blocks and check
  // not the bounding vertices of the edges, but check the edge itself
  // to be lying on some face
  //
  // all common blocks are contained in the result of SECTION operation
  // between sets of edges
  const TopoDS_Shape& aSecR = aSec.Shape();
  //
  TopTools_IndexedMapOfShape aMSSec;
  TopExp::MapShapes(aSecR, aMSSec);
  //
  const BOPCol_DataMapOfShapeListOfShape& anIm = aSec.Images();
  for (TopExp_Explorer aExp(theSplits, TopAbs_EDGE); aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aE = aExp.Current();
    if (aSec.IsDeleted(aE)) {
      // no common blocks for this edge
      continue;
    }
    //
    const BOPCol_ListOfShape* pLEIm = anIm.Seek(aE);
    if (!pLEIm) {
      // no splits, i.e. completely coincides with some edge from boundary
      continue;
    }
    //
    BOPCol_ListIteratorOfListOfShape aItLEIm(*pLEIm);
    for (; aItLEIm.More(); aItLEIm.Next()) {
      const TopoDS_Shape& aEIm = aItLEIm.Value();
      if (!aMSSec.Contains(aEIm)) {
        // the edge included in section only partially.
        // the part not included in section may be excessive
        //
        // check vertices of this edge - if one of them is new
        // the edge might be removed
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(TopoDS::Edge(aEIm), aV1, aV2);
        if (!theMVOld.Contains(aV1) || !theMVOld.Contains(aV2)) {
          // add this edge for checking by making new vertex in the middle of the edge
          TopoDS_Vertex aV;
          Standard_Real f, l;
          const Handle(Geom_Curve)& aC = BRep_Tool::Curve(TopoDS::Edge(aEIm), f, l);
          BRep_Builder().MakeVertex(aV, aC->Value((f+l)*0.5), Precision::Confusion());
          // and adding this vertex for checking
          aDMVE.ChangeFromIndex(aDMVE.Add(aV, TopTools_ListOfShape())).Append(aE);
          aMVInv.Add(aV);
          break;
        }
      }
    }
  }
  //
  // Add for check also the edges created from common between splits
  // of offset faces edges not connected to any invalidity.
  // These edges may also accidentally fill some part.
  TopTools_MapIteratorOfMapOfShape aItM(theMECheckExt);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aE = aItM.Value();
    //
    // make new vertex in the middle of the edge
    TopoDS_Vertex aV;
    Standard_Real f, l;
    const Handle(Geom_Curve)& aC = BRep_Tool::Curve(TopoDS::Edge(aE), f, l);
    BRep_Builder().MakeVertex(aV, aC->Value((f + l)*0.5), Precision::Confusion());
    // add this vertex for checking
    aDMVE.ChangeFromIndex(aDMVE.Add(aV, TopTools_ListOfShape())).Append(aE);
    aMVInv.Add(aV);
  }
  //
  // add for check also the vertices connected only to new or old edges
  aNb = aDMVE.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aV = aDMVE.FindKey(i);
    if (theMVOld.Contains(aV)) {
      continue;
    }
    //
    Standard_Boolean bNew = Standard_False, bOld = Standard_False;
    const TopTools_ListOfShape& aLEx = aDMVE(i);
    TopTools_ListIteratorOfListOfShape aIt(aLEx);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE = aIt.Value();
      if (theMECheckExt.Contains(aE)) {
        continue;
      }
      //
      if (theMENew.Contains(aE)) {
        bNew = Standard_True;
      }
      else {
        bOld = Standard_True;
      }
      //
      if (bNew && bOld) {
        break;
      }
    }
    //
    if (!bNew || !bOld) {
      aMVInv.Add(aV);
      aMVCheckAdd.Remove(aV);
    }
  }
  //
  // perform the checking
  Handle(IntTools_Context) aCtx = new IntTools_Context;
  // filter vertices
  Standard_Integer iv, aNbIV = aMVInv.Extent();
  for (iv = 1; iv <= aNbIV; ++iv) {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aMVInv(iv));
    if (theMVOld.Contains(aV)) {
      continue;
    }
    //
    const TopTools_ListOfShape* pLEInv = aDMVE.Seek(aV);
    if (!pLEInv) {
      continue;
    }
    // find faces by the edges to check the vertex
    TopTools_IndexedMapOfShape aMF;
    TopTools_ListIteratorOfListOfShape aItLE(*pLEInv);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aE = aItLE.Value();
      const TopTools_ListOfShape& aLF = theMELF.Find(aE);
      TopTools_ListIteratorOfListOfShape aItLF(aLF);
      for (; aItLF.More(); aItLF.Next()) {
        aMF.Add(aItLF.Value());
      }
    }
    //
    // check the vertex to belong to some split of the faces
    Standard_Boolean bInvalid = Standard_True;
    //
    Standard_Integer aNbF = aMF.Extent();
    for (i = 1; i <= aNbF && bInvalid; ++i) {
      const TopoDS_Face& aF = TopoDS::Face(aMF(i));
      const TopTools_ListOfShape& aLFIm = theFImages.FindFromKey(aF);
      //
      TopTools_ListIteratorOfListOfShape aItLF(aLFIm);
      for (; aItLF.More() && bInvalid; aItLF.Next()) {
        const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
        TopExp_Explorer aExp(aFIm, TopAbs_VERTEX);
        for (; aExp.More() && bInvalid; aExp.Next()) {
          const TopoDS_Shape& aVF = aExp.Current();
          bInvalid = !aVF.IsSame(aV);
        }
      }
      //
      if (bInvalid) {
        Standard_Real U, V, aTol;
        Standard_Integer iStatus = aCtx->ComputeVF(aV, aF, U, V, aTol);
        if (!iStatus) {
          // classify the point relatively faces
          gp_Pnt2d aP2d(U, V);
          aItLF.Initialize(aLFIm);
          for (; aItLF.More() && bInvalid; aItLF.Next()) {
            const TopoDS_Face& aFIm = TopoDS::Face(aItLF.Value());
            bInvalid = !aCtx->IsPointInOnFace(aFIm, aP2d);
          }
        }
      }
    }
    //
    if (bInvalid && aMVCheckAdd.Contains(aV)) {
      // the vertex is invalid for all faces
      // check the same vertex for the solids
      const gp_Pnt& aP = BRep_Tool::Pnt(aV);
      Standard_Real aTolV = BRep_Tool::Tolerance(aV);
      //
      TopExp_Explorer aExpS(theSolids, TopAbs_SOLID);
      for (; aExpS.More() && bInvalid; aExpS.Next()) {
        const TopoDS_Solid& aSol = TopoDS::Solid(aExpS.Current());
        BRepClass3d_SolidClassifier& aSC = aCtx->SolidClassifier(aSol);
        aSC.Perform(aP, aTolV);
        bInvalid = (aSC.State() == TopAbs_OUT);
      }
    }
    //
    if (bInvalid) {
      theVertsToAvoid.Add(aV);
      aItLE.Initialize(*pLEInv);
      for (; aItLE.More(); aItLE.Next()) {
        theMEInv.Add(aItLE.Value());
      }
    }
  }
}

//=======================================================================
//function : UpdateNewIntersectionEdges
//purpose  : Updating the maps of images and origins of the offset edges
//=======================================================================
void UpdateNewIntersectionEdges(const TopTools_ListOfShape& theLE,
                                const TopTools_DataMapOfShapeListOfShape& theMELF,
                                const TopTools_DataMapOfShapeListOfShape& theEImages,
                                const TopTools_IndexedMapOfShape& theInvEdges,
                                const TopTools_MapOfShape& theInvertedEdges,
                                TopTools_DataMapOfShapeListOfShape& theEdgesOrigins,
                                TopTools_DataMapOfShapeListOfShape& theOEImages,
                                TopTools_DataMapOfShapeListOfShape& theOEOrigins,
                                TopTools_DataMapOfShapeShape& theETrimEInf,
                                TopTools_DataMapOfShapeListOfShape& theEETrim,
                                TopTools_MapOfShape& theModifiedEdges,
                                Handle(BRepAlgo_AsDes)& theAsDes)
{
  TopTools_ListOfShape aLEImEmpty;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  // update global maps of images and origins with new splits
  aIt.Initialize(theLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE = aIt.Value();
    //
    if (!theEImages.IsBound(aE)) {
      TopTools_ListOfShape* pLET = theEETrim.ChangeSeek(aE);
      if (!pLET) {
        continue;
      }
      //
      TopTools_ListIteratorOfListOfShape aItLET(*pLET);
      for (; aItLET.More();) {
        const TopoDS_Shape& aET = aItLET.Value();
        if (!theInvEdges.Contains(aET) && !theInvertedEdges.Contains(aET)) {
          pLET->Remove(aItLET);
        }
        else {
          aItLET.Next();
        }
      }
      //
      if (pLET->IsEmpty()) {
        continue;
      }
    }
    // new images
    const TopTools_ListOfShape& aLENew = 
      theEImages.IsBound(aE) ? theEImages.Find(aE) : aLEImEmpty;
    //
    // save connection to untrimmed edge for the next steps
    aIt1.Initialize(aLENew);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aET = aIt1.Value();
      theETrimEInf.Bind(aET, aE);
      theModifiedEdges.Add(aET);
    }
    //
    // check if it is existing edge
    if (!theEETrim.IsBound(aE)) {
      const TopTools_ListOfShape& aLF = theMELF.Find(aE);
      // the edge is new
      // add this edge to AsDes
      aIt1.Initialize(aLF);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aF = aIt1.Value();
        theAsDes->Add(aF, aE);
      }
      //
      // add aE to the images
      theOEImages.Bind(aE, aLENew);
      theModifiedEdges.Add(aE);
      //
      // add to origins
      TopTools_ListIteratorOfListOfShape aItNew(aLENew);
      for (; aItNew.More(); aItNew.Next()) {
        const TopoDS_Shape& aENew = aItNew.Value();
        if (theOEOrigins.IsBound(aENew)) {
          TopTools_ListOfShape& aEOrigins = theOEOrigins.ChangeFind(aENew);
          AppendToList(aEOrigins, aE);
        }
        else {
          TopTools_ListOfShape aEOrigins;
          aEOrigins.Append(aE);
          theOEOrigins.Bind(aENew, aEOrigins);
        }
      }
      //
      // update connection to initial origins
      if (theEdgesOrigins.IsBound(aE)) {
        const TopTools_ListOfShape& aLEOrInit = theEdgesOrigins.Find(aE);
        aIt1.Initialize(aLENew);
        for (; aIt1.More(); aIt1.Next()) {
          const TopoDS_Shape& aENew = aIt1.Value();
          if (theEdgesOrigins.IsBound(aENew)) {
            TopTools_ListOfShape& aLENewOr = theEdgesOrigins.ChangeFind(aENew);
            TopTools_ListIteratorOfListOfShape aItOrInit(aLEOrInit);
            for (; aItOrInit.More(); aItOrInit.Next()) {
              const TopoDS_Shape& aEOr = aItOrInit.Value();
              AppendToList(aLENewOr, aEOr);
            }
          }
          else {
            theEdgesOrigins.Bind(aENew, aLEOrInit);
          }
        }
      }
      //
      continue;
    }
    //
    // old images
    const TopTools_ListOfShape& aLEOld = theEETrim.Find(aE);
    //
    // list of initial origins
    TopTools_ListOfShape anInitOrigins;
    //
    // it is necessary to replace the old edges with new ones
    aIt1.Initialize(aLEOld);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aEOld = aIt1.Value();
      //
      if (theOEOrigins.IsBound(aEOld)) {
        // get its origins
        const TopTools_ListOfShape& aEOrigins = theOEOrigins.Find(aEOld);
        //
        TopTools_ListIteratorOfListOfShape aItOr(aEOrigins);
        for (; aItOr.More(); aItOr.Next()) {
          const TopoDS_Shape& aEOr = aItOr.Value();
          //
          theModifiedEdges.Add(aEOr);
          //
          TopTools_ListOfShape& aEImages = theOEImages.ChangeFind(aEOr);
          //
          // remove old edge from images
          TopTools_ListIteratorOfListOfShape aItIm(aEImages);
          for (; aItIm.More(); ) {
            const TopoDS_Shape& aEIm = aItIm.Value();
            if (aEIm.IsSame(aEOld)) {
              aEImages.Remove(aItIm);
            }
            else {
              aItIm.Next();
            }
          }
          //
          // add new images
          TopTools_ListIteratorOfListOfShape aItNew(aLENew);
          for (; aItNew.More(); aItNew.Next()) {
            const TopoDS_Shape& aENew = aItNew.Value();
            AppendToList(aEImages, aENew);
            if (theOEOrigins.IsBound(aENew)) {
              TopTools_ListOfShape& aENewOrigins = theOEOrigins.ChangeFind(aENew);
              AppendToList(aENewOrigins, aEOr);
            }
            else {
              TopTools_ListOfShape aENewOrigins;
              aENewOrigins.Append(aEOr);
              theOEOrigins.Bind(aENew, aENewOrigins);
            }
          }
        }
      }
      else {
        // add to images
        theOEImages.Bind(aEOld, aLENew);
        //
        theModifiedEdges.Add(aEOld);
        //
        // add to origins
        TopTools_ListIteratorOfListOfShape aItNew(aLENew);
        for (; aItNew.More(); aItNew.Next()) {
          const TopoDS_Shape& aENew = aItNew.Value();
          if (theOEOrigins.IsBound(aENew)) {
            TopTools_ListOfShape& aEOrigins = theOEOrigins.ChangeFind(aENew);
            AppendToList(aEOrigins, aEOld);
          }
          else {
            TopTools_ListOfShape aEOrigins;
            aEOrigins.Append(aEOld);
            theOEOrigins.Bind(aENew, aEOrigins);
          }
        }
      }
      //
      // update connection to initial shape
      if (theEdgesOrigins.IsBound(aEOld)) {
        const TopTools_ListOfShape& aLEOrInit = theEdgesOrigins.Find(aEOld);
        TopTools_ListIteratorOfListOfShape aItEOrInit(aLEOrInit);
        for (; aItEOrInit.More(); aItEOrInit.Next()) {
          const TopoDS_Shape& aEOrInit = aItEOrInit.Value();
          AppendToList(anInitOrigins, aEOrInit);
        }
      }
    }
    //
    if (anInitOrigins.Extent()) {
      TopTools_ListIteratorOfListOfShape aItNew(aLENew);
      for (; aItNew.More(); aItNew.Next()) {
        const TopoDS_Shape& aENew = aItNew.Value();
        if (theEdgesOrigins.IsBound(aENew)) {
          TopTools_ListOfShape& aLENewOr = theEdgesOrigins.ChangeFind(aENew);
          TopTools_ListIteratorOfListOfShape aItOrInit(anInitOrigins);
          for (; aItOrInit.More(); aItOrInit.Next()) {
            const TopoDS_Shape& aEOr = aItOrInit.Value();
            AppendToList(aLENewOr, aEOr);
          }
        }
        else {
          theEdgesOrigins.Bind(aENew, anInitOrigins);
        }
      }
    }
  }
}

//=======================================================================
//function : FillHistory
//purpose  : Saving obtained results in history tools
//=======================================================================
void FillHistory(const TopTools_IndexedDataMapOfShapeListOfShape& theFImages,
                 const TopTools_DataMapOfShapeListOfShape& theEImages,
                 BRepAlgo_Image& theImage)
{
  Standard_Integer i, aNb = theFImages.Extent();
  if (!aNb) {
    return;
  }
  //
  BRep_Builder aBB;
  TopoDS_Compound aFaces;
  aBB.MakeCompound(aFaces);
  //
  // Fill history for faces
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aF = theFImages.FindKey(i);
    const TopTools_ListOfShape& aLFImages = theFImages(i);
    //
    if (aLFImages.Extent()) {
      if (theImage.HasImage(aF)) {
        theImage.Add(aF, aLFImages);
      }
      else {
        theImage.Bind(aF, aLFImages);
      }
    }
    //
    TopTools_ListIteratorOfListOfShape aItLF(aLFImages);
    for (; aItLF.More(); aItLF.Next()) {
      const TopoDS_Shape& aFIm = aItLF.Value();
      aBB.Add(aFaces, aFIm);
    }
  }
  //
  // fill history for edges
  TopTools_IndexedMapOfShape aMFE;
  TopExp::MapShapes(aFaces, TopAbs_EDGE, aMFE);
  //
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItEIm(theEImages);
  for (; aItEIm.More(); aItEIm.Next()) {
    const TopoDS_Shape& aE = aItEIm.Key();
    const TopTools_ListOfShape& aLEIm = aItEIm.Value();
    //
    Standard_Boolean bHasImage = theImage.HasImage(aE);
    TopTools_ListIteratorOfListOfShape aItLE(aLEIm);
    for (; aItLE.More(); aItLE.Next()) {
      const TopoDS_Shape& aEIm = aItLE.Value();
      if (aMFE.Contains(aEIm)) {
        if (bHasImage) {
          theImage.Add(aE, aEIm);
        }
        else {
          theImage.Bind(aE, aEIm);
          bHasImage = Standard_True;
        }
      }
    }
  }
}

//=======================================================================
//function : ProcessMicroEdge
//purpose  : Checking if the edge is micro edge
//=======================================================================
Standard_Boolean ProcessMicroEdge(const TopoDS_Edge& theEdge,
                                  const Handle(IntTools_Context)& theCtx)
{
  TopoDS_Vertex aV1, aV2;
  TopExp::Vertices(theEdge, aV1, aV2);
  if (aV1.IsNull() || aV2.IsNull()) {
    return Standard_False;
  }
  //
  Standard_Boolean bMicro = BOPTools_AlgoTools::IsMicroEdge(theEdge, theCtx);
  if (bMicro && BRepAdaptor_Curve(theEdge).GetType() == GeomAbs_Line) {
    Standard_Real aLen = BRep_Tool::Pnt(aV1).Distance(BRep_Tool::Pnt(aV2));
    BRep_Builder().UpdateVertex(aV1, aLen / 2.);
    BRep_Builder().UpdateVertex(aV2, aLen / 2.);
  }
  //
  return bMicro;
}

//=======================================================================
//function : UpdateOrigins
//purpose  : Updating origins
//=======================================================================
void UpdateOrigins(const TopTools_ListOfShape& theLA,
                   TopTools_DataMapOfShapeListOfShape& theOrigins,
                   BOPAlgo_Builder& theGF)
{
  TopTools_ListIteratorOfListOfShape aItA(theLA);
  for (; aItA.More(); aItA.Next()) {
    const TopoDS_Shape& aS = aItA.Value();
    //
    const TopTools_ListOfShape& aLSIm = theGF.Modified(aS);
    if (aLSIm.IsEmpty()) {
      continue;
    }
    //
    TopTools_ListOfShape aLSEmpt;
    TopTools_ListOfShape *pLS = theOrigins.ChangeSeek(aS);
    if (!pLS) {
      pLS = &aLSEmpt;
      pLS->Append(aS);
    }
    //
    TopTools_ListIteratorOfListOfShape aIt(aLSIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSIm = aIt.Value();
      //
      TopTools_ListOfShape *pLSOr = theOrigins.ChangeSeek(aSIm);
      if (!pLSOr) {
        // just bind the origins
        theOrigins.Bind(aSIm, *pLS);
      }
      else {
        // merge two lists
        TopTools_ListIteratorOfListOfShape aIt1(*pLS);
        for (; aIt1.More(); aIt1.Next()) {
          const TopoDS_Shape& aS1 = aIt1.Value();
          AppendToList(*pLSOr, aS1);
        }
      }
    }
  }
}

//=======================================================================
//function : UpdateImages
//purpose  : Updating images of the shapes
//=======================================================================
void UpdateImages(const TopTools_ListOfShape& theLA,
                  TopTools_DataMapOfShapeListOfShape& theImages,
                  BOPAlgo_Builder& theGF,
                  TopTools_MapOfShape& theModified)
{
  TopTools_ListIteratorOfListOfShape aIt(theLA);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    //
    TopTools_ListOfShape* pLSIm = theImages.ChangeSeek(aS);
    if (!pLSIm) {
      const TopTools_ListOfShape& aLSIm = theGF.Modified(aS);
      if (aLSIm.Extent()) {
        theImages.Bind(aS, aLSIm);
        theModified.Add(aS);
      }
      continue;
    }
    //
    TopTools_MapOfShape aMFence;
    TopTools_ListOfShape aLSImNew;
    //
    Standard_Boolean bModified = Standard_False;
    //
    // check modifications of the images
    TopTools_ListIteratorOfListOfShape aIt1(*pLSIm);
    for (; aIt1.More(); aIt1.Next()) {
      const TopoDS_Shape& aSIm = aIt1.Value();
      const TopTools_ListOfShape& aLSIm1 = theGF.Modified(aSIm);
      if (aLSIm1.IsEmpty()) {
        if (aMFence.Add(aSIm)) {
          aLSImNew.Append(aSIm);
        }
      }
      else {
        TopTools_ListIteratorOfListOfShape aIt2(aLSIm1);
        for (; aIt2.More(); aIt2.Next()) {
          const TopoDS_Shape& aSImIm = aIt2.Value();
          if (aMFence.Add(aSImIm)) {
            aLSImNew.Append(aSImIm);
          }
        }
        bModified = Standard_True;
      }
    }
    //
    if (bModified) {
      *pLSIm = aLSImNew;
      theModified.Add(aS);
    }
  }
}

//=======================================================================
//function : UpdateIntersectedEdges
//purpose  : Saving connection from trimmed edges to not trimmed ones
//=======================================================================
void UpdateIntersectedEdges(const TopTools_ListOfShape& theLA,
                            TopTools_DataMapOfShapeShape& theETrimEInf,
                            BOPAlgo_Builder& theGF)
{
  TopTools_ListIteratorOfListOfShape aItA(theLA);
  for (; aItA.More(); aItA.Next()) {
    const TopoDS_Shape& aS = aItA.Value();
    //
    const TopoDS_Shape* pEInf = theETrimEInf.Seek(aS);
    if (!pEInf) {
      continue;
    }
    //
    const TopTools_ListOfShape& aLSIm = theGF.Modified(aS);
    if (aLSIm.IsEmpty()) {
      continue;
    }
    //
    TopTools_ListIteratorOfListOfShape aIt(aLSIm);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aEIm = aIt.Value();
      if (!theETrimEInf.IsBound(aEIm)) {
        theETrimEInf.Bind(aEIm, *pEInf);
      }
    }
  }
}

//=======================================================================
//function : FindCommonParts
//purpose  : Looking for the parts of type <theType> contained in both lists
//=======================================================================
void FindCommonParts(const TopTools_ListOfShape& theLS1,
                     const TopTools_ListOfShape& theLS2,
                     TopTools_ListOfShape& theLSC,
                     const TopAbs_ShapeEnum theType)
{
  // map shapes in the first list
  TopTools_IndexedMapOfShape aMS1;
  TopTools_ListIteratorOfListOfShape aIt(theLS1);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    TopExp::MapShapes(aS, theType, aMS1);
  }
  //
  if (aMS1.IsEmpty()) {
    return;
  }
  //
  TopTools_MapOfShape aMFence;
  // check for such shapes in the other list
  aIt.Initialize(theLS2);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    //
    TopExp_Explorer aExp(aS, theType);
    for(; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aST = aExp.Current();
      //
      if (aMS1.Contains(aST) && aMFence.Add(aST)) {
        theLSC.Append(aST);
      }
    }
  }
}

//=======================================================================
//function : NbPoints
//purpose  : Defines number of sample points to get average direction of the edge
//=======================================================================
Standard_Integer NbPoints(const TopoDS_Edge& theEdge)
{
  Standard_Integer aNbP;
  BRepAdaptor_Curve aBAC(theEdge);
  switch (aBAC.GetType()) {
  case GeomAbs_Line:
    aNbP = 1;
    break;
  default:
    aNbP = 11;
  }
  //
  return aNbP;
}

//=======================================================================
//function : FindShape
//purpose  : Looking for the same sub-shape in the shape
//=======================================================================
Standard_Boolean FindShape(const TopoDS_Shape& theSWhat,
                           const TopoDS_Shape& theSWhere,
                           TopoDS_Shape& theRes)
{
  Standard_Boolean bFound = Standard_False;
  TopAbs_ShapeEnum aType = theSWhat.ShapeType();
  TopExp_Explorer aExp(theSWhere, aType);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    if (aS.IsSame(theSWhat)) {
      theRes = aS;
      bFound = Standard_True;
      break;
    }
  }
  return bFound;
}


//=======================================================================
//function : AppendToList
//purpose  : Add to a list only unique elements
//=======================================================================
void AppendToList(TopTools_ListOfShape& theList,
                  const TopoDS_Shape& theShape)
{
  TopTools_ListIteratorOfListOfShape aIt(theList);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    if (aS.IsSame(theShape)) {
      return;
    }
  }
  theList.Append(theShape);
}
