// Created on: 1999-09-29
// Created by: Maxim ZVEREV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TopOpeBRepBuild_Builder1_HeaderFile
#define _TopOpeBRepBuild_Builder1_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
class TopOpeBRepDS_BuildTool;
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepBuild_GTopo;
class TopOpeBRepBuild_ShellFaceSet;
class TopOpeBRepBuild_WireEdgeSet;
class TopOpeBRepBuild_PaveSet;
class TopoDS_Edge;
class TopoDS_Face;

//! extension of the class TopOpeBRepBuild_Builder dedicated
//! to avoid bugs in "Rebuilding Result" algorithm for the
//! case of SOLID/SOLID Boolean Operations
class TopOpeBRepBuild_Builder1 : public TopOpeBRepBuild_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepBuild_Builder1(const TopOpeBRepDS_BuildTool& BT);

  Standard_EXPORT virtual ~TopOpeBRepBuild_Builder1();

  //! Removes all splits and merges already performed.
  //! Does NOT clear the handled DS (except ShapeWithStatesMaps).
  Standard_EXPORT virtual void Clear() override;

  Standard_EXPORT virtual void Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
    override;

  Standard_EXPORT virtual void Perform(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS,
                                       const TopoDS_Shape&                        S1,
                                       const TopoDS_Shape& S2) override;

  Standard_EXPORT virtual void MergeKPart() override;

  Standard_EXPORT virtual void MergeKPart(const TopAbs_State TB1,
                                          const TopAbs_State TB2) override;

  Standard_EXPORT virtual void GFillSolidSFS(const TopoDS_Shape&           SO1,
                                             const NCollection_List<TopoDS_Shape>&   LSO2,
                                             const TopOpeBRepBuild_GTopo&  G,
                                             TopOpeBRepBuild_ShellFaceSet& SFS) override;

  Standard_EXPORT virtual void GFillShellSFS(const TopoDS_Shape&           SH1,
                                             const NCollection_List<TopoDS_Shape>&   LSO2,
                                             const TopOpeBRepBuild_GTopo&  G,
                                             TopOpeBRepBuild_ShellFaceSet& SFS) override;

  Standard_EXPORT virtual void GWESMakeFaces(const TopoDS_Shape&          FF,
                                             TopOpeBRepBuild_WireEdgeSet& WES,
                                             NCollection_List<TopoDS_Shape>&        LOF) override;

  Standard_EXPORT void GFillSplitsPVS(const TopoDS_Shape&          anEdge,
                                      const TopOpeBRepBuild_GTopo& G1,
                                      TopOpeBRepBuild_PaveSet&     PVS);

  Standard_EXPORT void GFillFaceNotSameDomSFS(const TopoDS_Shape&           F1,
                                              const NCollection_List<TopoDS_Shape>&   LSO2,
                                              const TopOpeBRepBuild_GTopo&  G,
                                              TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GFillFaceNotSameDomWES(const TopoDS_Shape&          F1,
                                              const NCollection_List<TopoDS_Shape>&  LSO2,
                                              const TopOpeBRepBuild_GTopo& G,
                                              TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillWireNotSameDomWES(const TopoDS_Shape&          W1,
                                              const NCollection_List<TopoDS_Shape>&  LSO2,
                                              const TopOpeBRepBuild_GTopo& G,
                                              TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillEdgeNotSameDomWES(const TopoDS_Shape&          E1,
                                              const NCollection_List<TopoDS_Shape>&  LSO2,
                                              const TopOpeBRepBuild_GTopo& G,
                                              TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillFaceSameDomSFS(const TopoDS_Shape&           F1,
                                           const NCollection_List<TopoDS_Shape>&   LSO2,
                                           const TopOpeBRepBuild_GTopo&  G,
                                           TopOpeBRepBuild_ShellFaceSet& SFS);

  Standard_EXPORT void GFillFaceSameDomWES(const TopoDS_Shape&          F1,
                                           const NCollection_List<TopoDS_Shape>&  LSO2,
                                           const TopOpeBRepBuild_GTopo& G,
                                           TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillWireSameDomWES(const TopoDS_Shape&          W1,
                                           const NCollection_List<TopoDS_Shape>&  LSO2,
                                           const TopOpeBRepBuild_GTopo& G,
                                           TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void GFillEdgeSameDomWES(const TopoDS_Shape&          E1,
                                           const NCollection_List<TopoDS_Shape>&  LSO2,
                                           const TopOpeBRepBuild_GTopo& G,
                                           TopOpeBRepBuild_WireEdgeSet& WES);

  Standard_EXPORT void PerformONParts(const TopoDS_Shape&               F,
                                      const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& SDfaces,
                                      const TopOpeBRepBuild_GTopo&      G,
                                      TopOpeBRepBuild_WireEdgeSet&      WES);

  Standard_EXPORT void PerformPieceIn2D(const TopoDS_Edge&           aPieceToPerform,
                                        const TopoDS_Edge&           aOriginalEdge,
                                        const TopoDS_Face&           edgeFace,
                                        const TopoDS_Face&           toFace,
                                        const TopOpeBRepBuild_GTopo& G,
                                        bool&            keep);

  Standard_EXPORT int PerformPieceOn2D(const TopoDS_Shape&   aPieceObj,
                                                    const TopoDS_Shape&   aFaceObj,
                                                    const TopoDS_Shape&   aEdgeObj,
                                                    NCollection_List<TopoDS_Shape>& aListOfPieces,
                                                    NCollection_List<TopoDS_Shape>& aListOfFaces,
                                                    NCollection_List<TopoDS_Shape>& aListOfPiecesOut2d);

  Standard_EXPORT int TwoPiecesON(const NCollection_Sequence<TopoDS_Shape>& aSeq,
                                               NCollection_List<TopoDS_Shape>&           aListOfPieces,
                                               NCollection_List<TopoDS_Shape>&           aListOfFaces,
                                               NCollection_List<TopoDS_Shape>&           aListOfPiecesOut2d);

  Standard_EXPORT int CorrectResult2d(TopoDS_Shape& aResult);

  friend class TopOpeBRepBuild_HBuilder;

protected:
  Standard_EXPORT void PerformShapeWithStates();

  Standard_EXPORT void PerformShapeWithStates(const TopoDS_Shape& anObj, const TopoDS_Shape& aTool);

  Standard_EXPORT void StatusEdgesToSplit(const TopoDS_Shape&               anObj,
                                          const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& anEdgesToSplitMap,
                                          const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& anEdgesToRestMap);

  Standard_EXPORT void SplitEdge(const TopoDS_Shape&               anEdge,
                                 NCollection_List<TopoDS_Shape>&             aLNew,
                                 NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>& aDataMapOfShapeState);

  Standard_EXPORT void PerformFacesWithStates(const TopoDS_Shape&               anObj,
                                              const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aFaces,
                                              NCollection_DataMap<TopoDS_Shape, TopAbs_State, TopTools_ShapeMapHasher>& aSplF);

  Standard_EXPORT int IsSame2d(const NCollection_Sequence<TopoDS_Shape>& aSeq,
                                            NCollection_List<TopoDS_Shape>&           aListOfPiecesOut2d);

  Standard_EXPORT void OrientateEdgeOnFace(TopoDS_Edge&                 EdgeToPerform,
                                           const TopoDS_Face&           baseFace,
                                           const TopoDS_Face&           edgeFace,
                                           const TopOpeBRepBuild_GTopo& G1,
                                           bool&            stateOfFaceOri) const;

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myFSplits;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myESplits;

private:
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                          mySameDomMap;
  TopoDS_Shape                                        mySDFaceToFill;
  TopoDS_Shape                                        myBaseFaceToFill;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>           myMapOfEdgeFaces;
  NCollection_DataMap<TopoDS_Shape, bool> myMapOfEdgeWithFaceState;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                          myProcessedPartsOut2d;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                          myProcessedPartsON2d;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>                          mySplitsONtoKeep;
  NCollection_IndexedMap<TopoDS_Shape>                  mySourceShapes;
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>                 myMapOfCorrect2dEdges;
};

#endif // _TopOpeBRepBuild_Builder1_HeaderFile
