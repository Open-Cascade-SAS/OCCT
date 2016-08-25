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

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfOrientedShapeInteger.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopOpeBRepDS_DataMapOfShapeState.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_SequenceOfShape.hxx>
class TopOpeBRepBuild_HBuilder;
class TopOpeBRepDS_BuildTool;
class TopOpeBRepDS_HDataStructure;
class TopoDS_Shape;
class TopOpeBRepBuild_GTopo;
class TopOpeBRepBuild_ShellFaceSet;
class TopOpeBRepBuild_WireEdgeSet;
class TopOpeBRepBuild_PaveSet;
class TopoDS_Edge;
class TopoDS_Face;


//! extension  of  the  class  TopOpeBRepBuild_Builder  dedicated
//! to  avoid  bugs  in  "Rebuilding Result" algorithm  for  the  case  of  SOLID/SOLID  Boolean  Operations
class TopOpeBRepBuild_Builder1  : public TopOpeBRepBuild_Builder
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_Builder1(const TopOpeBRepDS_BuildTool& BT);
  
  Standard_EXPORT virtual ~TopOpeBRepBuild_Builder1();
  
  //! Removes all splits and merges already performed.
  //! Does NOT clear the handled DS  (except  ShapeWithStatesMaps).
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Perform (const Handle(TopOpeBRepDS_HDataStructure)& HDS) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Perform (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopoDS_Shape& S1, const TopoDS_Shape& S2) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void MergeKPart() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void MergeKPart (const TopAbs_State TB1, const TopAbs_State TB2) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void GFillSolidSFS (const TopoDS_Shape& SO1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void GFillShellSFS (const TopoDS_Shape& SH1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void GWESMakeFaces (const TopoDS_Shape& FF, TopOpeBRepBuild_WireEdgeSet& WES, TopTools_ListOfShape& LOF) Standard_OVERRIDE;
  
  Standard_EXPORT void GFillSplitsPVS (const TopoDS_Shape& anEdge, const TopOpeBRepBuild_GTopo& G1, TopOpeBRepBuild_PaveSet& PVS);
  
  Standard_EXPORT void GFillFaceNotSameDomSFS (const TopoDS_Shape& F1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GFillFaceNotSameDomWES (const TopoDS_Shape& F1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillWireNotSameDomWES (const TopoDS_Shape& W1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillEdgeNotSameDomWES (const TopoDS_Shape& E1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillFaceSameDomSFS (const TopoDS_Shape& F1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_ShellFaceSet& SFS);
  
  Standard_EXPORT void GFillFaceSameDomWES (const TopoDS_Shape& F1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillWireSameDomWES (const TopoDS_Shape& W1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void GFillEdgeSameDomWES (const TopoDS_Shape& E1, const TopTools_ListOfShape& LSO2, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void PerformONParts (const TopoDS_Shape& F, const TopTools_IndexedMapOfShape& SDfaces, const TopOpeBRepBuild_GTopo& G, TopOpeBRepBuild_WireEdgeSet& WES);
  
  Standard_EXPORT void PerformPieceIn2D (const TopoDS_Edge& aPieceToPerform, const TopoDS_Edge& aOriginalEdge, const TopoDS_Face& edgeFace, const TopoDS_Face& toFace, const TopOpeBRepBuild_GTopo& G, Standard_Boolean& keep);
  
  Standard_EXPORT Standard_Integer PerformPieceOn2D (const TopoDS_Shape& aPieceObj, const TopoDS_Shape& aFaceObj, const TopoDS_Shape& aEdgeObj, TopTools_ListOfShape& aListOfPieces, TopTools_ListOfShape& aListOfFaces, TopTools_ListOfShape& aListOfPiecesOut2d);
  
  Standard_EXPORT Standard_Integer TwoPiecesON (const TopTools_SequenceOfShape& aSeq, TopTools_ListOfShape& aListOfPieces, TopTools_ListOfShape& aListOfFaces, TopTools_ListOfShape& aListOfPiecesOut2d);
  
  Standard_EXPORT Standard_Integer CorrectResult2d (TopoDS_Shape& aResult);


friend class TopOpeBRepBuild_HBuilder;


protected:

  
  Standard_EXPORT void PerformShapeWithStates();
  
  Standard_EXPORT void PerformShapeWithStates (const TopoDS_Shape& anObj, const TopoDS_Shape& aTool);
  
  Standard_EXPORT void StatusEdgesToSplit (const TopoDS_Shape& anObj, const TopTools_IndexedMapOfShape& anEdgesToSplitMap, const TopTools_IndexedMapOfShape& anEdgesToRestMap);
  
  Standard_EXPORT void SplitEdge (const TopoDS_Shape& anEdge, TopTools_ListOfShape& aLNew, TopOpeBRepDS_DataMapOfShapeState& aDataMapOfShapeState);
  
  Standard_EXPORT void PerformFacesWithStates (const TopoDS_Shape& anObj, const TopTools_IndexedMapOfShape& aFaces, TopOpeBRepDS_DataMapOfShapeState& aSplF);
  
  Standard_EXPORT Standard_Integer IsSame2d (const TopTools_SequenceOfShape& aSeq, TopTools_ListOfShape& aListOfPiecesOut2d);
  
  Standard_EXPORT void OrientateEdgeOnFace (TopoDS_Edge& EdgeToPerform, const TopoDS_Face& baseFace, const TopoDS_Face& edgeFace, const TopOpeBRepBuild_GTopo& G1, Standard_Boolean& stateOfFaceOri) const;


  TopTools_DataMapOfShapeListOfShape myFSplits;
  TopTools_DataMapOfShapeListOfShape myESplits;


private:

  TopTools_IndexedMapOfShape mySameDomMap;
  TopoDS_Shape mySDFaceToFill;
  TopoDS_Shape myBaseFaceToFill;
  TopTools_IndexedDataMapOfShapeListOfShape myMapOfEdgeFaces;
  NCollection_DataMap<TopoDS_Shape, Standard_Boolean, TopTools_OrientedShapeMapHasher> myMapOfEdgeWithFaceState;
  TopTools_IndexedMapOfShape myProcessedPartsOut2d;
  TopTools_IndexedMapOfShape myProcessedPartsON2d;
  TopTools_IndexedMapOfShape mySplitsONtoKeep;
  TopTools_IndexedMapOfOrientedShape mySourceShapes;
  TopTools_IndexedDataMapOfShapeShape myMapOfCorrect2dEdges;

};

#endif // _TopOpeBRepBuild_Builder1_HeaderFile
