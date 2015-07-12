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


#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepTools.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Array1.hxx>
#include <QANewModTopOpe_Tools.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_IndexedMapOfReal.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <algorithm>
static Standard_Boolean AddShapeToHistoryMap(const TopoDS_Shape& theOldShape,
                                             const TopoDS_Shape& theNewShape,
                                             TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap);

static void FillEdgeHistoryMap(BRepAlgoAPI_BooleanOperation&              theBOP,
                               TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap);

static void SortVertexOnEdge(const TopoDS_Edge&          theEdge,
                             const TopTools_ListOfShape& theListOfVertex,
                             TopTools_ListOfShape&       theListOfVertexSorted);

static TopAbs_State GetEdgeState(const BOPDS_PDS& pDS,
                                 const Handle(BOPDS_PaveBlock)& aPB);

// ========================================================================================
// function: NbPoints
// purpose:
// ========================================================================================
Standard_Integer QANewModTopOpe_Tools::NbPoints(const BOPAlgo_PPaveFiller& theDSFiller) 
{
  Standard_Integer i, anbpoints, aNb;
  //
  const BOPDS_PDS& pDS = theDSFiller->PDS();
  anbpoints = 0;

  //FF
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();
  aNb=aFFs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    const BOPDS_VectorOfPoint& aVP=aFF.Points();
    anbpoints += aVP.Extent();
  }

  //EF
  BOPDS_VectorOfInterfEF& aEFs=pDS->InterfEF();
  aNb = aEFs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfEF& aEF=aEFs(i);
    IntTools_CommonPrt aCP = aEF.CommonPart();
    if(aCP.Type() == TopAbs_VERTEX) {
      anbpoints++;
    }
  }
       
  //EE
  BOPDS_VectorOfInterfEE& aEEs=pDS->InterfEE();
  aNb = aEEs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfEE& aEE=aEEs(i);
    IntTools_CommonPrt aCP = aEE.CommonPart();
    if(aCP.Type() == TopAbs_VERTEX) {
      anbpoints++;
    }
  }

  return anbpoints;
}

// ========================================================================================
// function: NewVertex
// purpose:
// ========================================================================================
TopoDS_Shape QANewModTopOpe_Tools::NewVertex(const BOPAlgo_PPaveFiller& theDSFiller, 
                                             const Standard_Integer     theIndex) 
{
  TopoDS_Shape aVertex;
  Standard_Integer i, j, anbpoints, aNb, aNbP;
  //
  const BOPDS_PDS& pDS = theDSFiller->PDS();
  anbpoints = 0;

  //FF
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();
  aNb=aFFs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    const BOPDS_VectorOfPoint& aVP=aFF.Points();
    aNbP = aVP.Extent();
    for(j = 0; j < aNbP; ++j) {
      anbpoints++;
      //
      if (theIndex == anbpoints) {
        const BOPDS_Point& aNP = aVP(j);
        return pDS->Shape(aNP.Index());
      }
    }
  }

  //EF
  BOPDS_VectorOfInterfEF& aEFs=pDS->InterfEF();
  aNb = aEFs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfEF& aEF=aEFs(i);
    IntTools_CommonPrt aCP = aEF.CommonPart();
    if(aCP.Type() == TopAbs_VERTEX) {
      anbpoints++;
      //
      if (theIndex == anbpoints) {
        return pDS->Shape(aEF.IndexNew());
      }
    }
  }
       
  //EE
  BOPDS_VectorOfInterfEE& aEEs=pDS->InterfEE();
  aNb = aEEs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfEE& aEE=aEEs(i);
    IntTools_CommonPrt aCP = aEE.CommonPart();
    if(aCP.Type() == TopAbs_VERTEX) {
      anbpoints++;
      //
      if (theIndex == anbpoints) {
        return pDS->Shape(aEE.IndexNew());
      }
    }
  }

  return aVertex;
}


// ========================================================================================
// function: HasDomain
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::HasSameDomain
  (const BOPAlgo_PBuilder& theBuilder, 
   const TopoDS_Shape& theFace) 
{
  Standard_Integer bRet;
  bRet = Standard_False;
  //
  if(theFace.IsNull() || (theFace.ShapeType() != TopAbs_FACE))
    return bRet;

  BOPCol_ListIteratorOfListOfShape aIt;
  const BOPCol_DataMapOfShapeListOfShape& aImages = theBuilder->Images();
  if (!aImages.IsBound(theFace)) {
    return bRet;
  }
  const BOPCol_ListOfShape& aLF=aImages.Find(theFace);
  
  if (aLF.Extent() == 0) {
    return bRet;
  }
  const BOPCol_DataMapOfShapeShape& aShapesSD = theBuilder->ShapesSD();

  aIt.Initialize(aLF);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aFsp = aIt.Value();
    if (aShapesSD.IsBound(aFsp)) {
      bRet = Standard_True;
      break;
    }
  }

  return bRet;
}

// ========================================================================================
// function: SameDomain
// purpose:
// ========================================================================================
void QANewModTopOpe_Tools::SameDomain
  (const BOPAlgo_PBuilder&   theBuilder, 
   const TopoDS_Shape&   theFace,
   TopTools_ListOfShape& theResultList) 
{
  theResultList.Clear();

  if(theFace.IsNull() || (theFace.ShapeType() != TopAbs_FACE))
    return;

  BOPCol_ListIteratorOfListOfShape aIt;
  const BOPCol_ListOfShape& aLF=theBuilder->Splits().Find(theFace);
  
  if (aLF.Extent() == 0) {
    return;
  }
  const BOPCol_DataMapOfShapeShape& aShapesSD = theBuilder->ShapesSD();
  const BOPCol_DataMapOfShapeShape& aOrigins = theBuilder->Origins();
  
  aIt.Initialize(aLF);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aFSp = aIt.Value();
    if (aShapesSD.IsBound(aFSp)) {
      const TopoDS_Shape& aFSD = aShapesSD.Find(aFSp);
      const TopoDS_Shape& aFOr = aOrigins.Find(aFSD);
      if (theFace.IsEqual(aFOr)) {
        BOPCol_DataMapIteratorOfDataMapOfShapeShape aItSD;
        aItSD.Initialize(aShapesSD);
        for (; aItSD.More(); aItSD.Next()) {
          const TopoDS_Shape& aS = aItSD.Value();
          if (aFSD.IsEqual(aS)) {
            const TopoDS_Shape& aSK = aItSD.Key();
            const TopoDS_Shape& aSKOr = aOrigins.Find(aSK);
            if (!aSKOr.IsEqual(theFace)) {
              theResultList.Append(aSKOr);
            }
          }
        }
      } else {
        theResultList.Append(aFOr);
      }
    }
  }
}

// ========================================================================================
// function: IsSplit
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::IsSplit(const BOPAlgo_PPaveFiller& theDSFiller,
                                          const TopoDS_Shape&       theEdge,
                                          const TopAbs_State        theState) 
{
  if(theEdge.IsNull() || (theEdge.ShapeType() != TopAbs_EDGE))
    return Standard_False;

  Standard_Integer index;
  //
  const BOPDS_PDS& pDS = theDSFiller->PDS();
  index = pDS->Index(theEdge);
  if (index == -1) {
    return Standard_False;
  }

  const BOPDS_ListOfPaveBlock& aLPB = pDS->PaveBlocks(index);
  BOPDS_ListIteratorOfListOfPaveBlock aPBIt;
  aPBIt.Initialize(aLPB);
  for (; aPBIt.More(); aPBIt.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aPBIt.Value();

    TopAbs_State aSplitState = GetEdgeState(pDS, aPB);

    if(aSplitState == theState) {
      return Standard_True;
    }
  }

  return Standard_False;
}

// ========================================================================================
// function: Splits
// purpose:
// ========================================================================================
void QANewModTopOpe_Tools::Splits(const BOPAlgo_PPaveFiller& theDSFiller,
                             const TopoDS_Shape&       theEdge,
                             const TopAbs_State        theState,
                             TopTools_ListOfShape&     theResultList) 
{
  theResultList.Clear();

  if(theEdge.IsNull() || (theEdge.ShapeType() != TopAbs_EDGE))
    return;

  Standard_Integer index, nSp;
  //
  const BOPDS_PDS& pDS = theDSFiller->PDS();
  index = pDS->Index(theEdge);
  if (index == -1) {
    return;
  }

  const BOPDS_ListOfPaveBlock& aLPB = pDS->PaveBlocks(index);
  BOPDS_ListIteratorOfListOfPaveBlock aPBIt;
  aPBIt.Initialize(aLPB);
  for (; aPBIt.More(); aPBIt.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aPBIt.Value();
    nSp = aPB->Edge();

    TopAbs_State aSplitState = GetEdgeState(pDS, aPB);

    if(aSplitState == theState) {
      TopoDS_Shape aSplit = pDS->Shape(nSp);
      theResultList.Append(aSplit);
    }
  }
}

// ========================================================================================
// function: SplitE
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::SplitE(const TopoDS_Edge&    theEdge,
                                         TopTools_ListOfShape& theSplits) 
{
  // prequesitory : <Eanc> is a valid edge.
  TopAbs_Orientation oEanc = theEdge.Orientation();
  TopoDS_Shape aLocalShape = theEdge.Oriented(TopAbs_FORWARD);
  TopoDS_Edge EFOR = TopoDS::Edge(aLocalShape);
  TopTools_ListOfShape aListOfVertex;
  TopExp_Explorer exv(EFOR,TopAbs_VERTEX);  

  for (;exv.More(); exv.Next()) {
    const TopoDS_Shape& v = exv.Current();
    aListOfVertex.Append(v);
  }
  Standard_Integer nv = aListOfVertex.Extent();

  if (nv <= 2) return Standard_False;
  TopTools_ListOfShape aListOfVertexSorted;

  SortVertexOnEdge(EFOR, aListOfVertex, aListOfVertexSorted);

  TopoDS_Vertex v0;
  TopTools_ListIteratorOfListOfShape anIt(aListOfVertexSorted);

  if (anIt.More()) {
    v0 = TopoDS::Vertex(anIt.Value()); 
    anIt.Next();
  }
  else return Standard_False;

  for (; anIt.More(); anIt.Next()) {
    TopoDS_Vertex v = TopoDS::Vertex(anIt.Value());
    
    // prequesitory: par0 < par
    Standard_Real par0 = BRep_Tool::Parameter(v0, EFOR);
    Standard_Real par  = BRep_Tool::Parameter(v, EFOR);

    // here, ed has the same geometries than Ein, but with no subshapes.
    TopoDS_Edge ed = TopoDS::Edge(EFOR.EmptyCopied());
    BRep_Builder BB;
    v0.Orientation(TopAbs_FORWARD); 
    BB.Add(ed, v0);
    v.Orientation(TopAbs_REVERSED); 
    BB.Add(ed, v);
    BB.Range(ed, par0, par);

    theSplits.Append(ed.Oriented(oEanc));
    v0 = v;
  }
  return Standard_True;
}


// ========================================================================================
// function: EdgeCurveAncestors
// purpose:
// ========================================================================================
 Standard_Boolean QANewModTopOpe_Tools::EdgeCurveAncestors(const BOPAlgo_PPaveFiller& theDSFiller,
                                                      const TopoDS_Shape&       theEdge,
                                                      TopoDS_Shape&             theFace1,
                                                      TopoDS_Shape&             theFace2) 
{
  theFace1.Nullify();
  theFace2.Nullify();
  //
  Standard_Integer i, j, aNb, aNbC, nE, nF1, nF2;
  BOPDS_ListIteratorOfListOfPaveBlock aIt;

  const BOPDS_PDS& pDS = theDSFiller->PDS();
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();

  aNb=aFFs.Extent();
  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfFF& aFF=aFFs(i);
    
    const BOPDS_VectorOfCurve& aVC = aFF.Curves();
    aNbC = aVC.Extent();
    for (j = 0; j < aNbC; ++j) {
      const BOPDS_Curve& aNC = aVC(j);
      const BOPDS_ListOfPaveBlock& aLPB = aNC.PaveBlocks();
      aIt.Initialize(aLPB);
      for (; aIt.More(); aIt.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = aIt.Value();
        nE = aPB->Edge();
        const TopoDS_Shape& aE = pDS->Shape(nE);
        if (theEdge.IsSame(aE)) {
          aFF.Indices(nF1, nF2);
          theFace1 = pDS->Shape(nF1);
          theFace2 = pDS->Shape(nF2);
          return Standard_True;
        }
      }
    }
  }

  return Standard_False;
}

// ========================================================================================
// function: EdgeSectionAncestors
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::EdgeSectionAncestors(const BOPAlgo_PPaveFiller& theDSFiller,
                                                            const TopoDS_Shape&       theEdge,
                                                            TopTools_ListOfShape&     LF1,
                                                            TopTools_ListOfShape&     LF2,
                                                            TopTools_ListOfShape&     LE1,
                                                            TopTools_ListOfShape&     LE2) 
{
  if(theEdge.ShapeType() != TopAbs_EDGE)
    return Standard_False;
  
  const BOPDS_PDS& pDS = theDSFiller->PDS();
  Standard_Integer i = 0, nb = 0, nF, nE, nEOr;
  BOPCol_MapOfInteger aMIF;
  nb = pDS->NbSourceShapes();

  nE = pDS->Index(theEdge);
  //
  if (nE<0) {
    return Standard_False;
  }
  //
  const BOPDS_ListOfPaveBlock& aLPB1 = pDS->PaveBlocks(nE);
  if (!aLPB1.Extent()) {
    return Standard_False;
  }

  const Handle(BOPDS_PaveBlock)& aPB1 = aLPB1.First();
  const Handle(BOPDS_CommonBlock)& aCB=pDS->CommonBlock(aPB1);
  if (aCB.IsNull()) {
    return Standard_False;
  }
  
  const BOPCol_ListOfInteger& aLIF = aCB->Faces();
  BOPCol_ListIteratorOfListOfInteger aItLI;
  aItLI.Initialize(aLIF);
  for ( ; aItLI.More(); aItLI.Next()) {
    nF = aItLI.Value();
    if(pDS->Rank(nF) == 0)
      LF1.Append(pDS->Shape(nF));
    else
      LF2.Append(pDS->Shape(nF));
    
    aMIF.Add(nF);
  }

  const BOPDS_ListOfPaveBlock& aLPB = aCB->PaveBlocks();
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  aItPB.Initialize(aLPB);
  for (; aItPB.More(); aItPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
    nEOr = aPB->OriginalEdge();

    if(pDS->Rank(nEOr) == 0)
      LE1.Append(pDS->Shape(nEOr));
    else
      LE2.Append(pDS->Shape(nEOr));

    //find edge ancestors
    for(i = 0; i < nb; ++i) {
      const BOPDS_ShapeInfo& aSI = pDS->ShapeInfo(i);
      if(aSI.ShapeType() != TopAbs_FACE) {
        continue;
      }
      const BOPCol_ListOfInteger& aSubShapes = aSI.SubShapes();
      aItLI.Initialize(aSubShapes);
      for (; aItLI.More(); aItLI.Next()) {
        if (nEOr == aItLI.Value()) {
          if (aMIF.Add(i)) {
            if(pDS->Rank(i) == 0) LF1.Append(pDS->Shape(i));
            else LF2.Append(pDS->Shape(i));
          }//if (aMIF.Add(i)) {
        }//if (nEOr == aItLI.Value()) {
      }//for (; aItLI.More(); aItLI.Next()) {
    }//for(i = 0; i < nb; ++i) {
  }

  Standard_Boolean r = (!LF1.IsEmpty() && !LF2.IsEmpty());
  r = r && (!LE1.IsEmpty() || !LE2.IsEmpty());
  return r;
}

// ========================================================================================
// function: BoolOpe
// purpose:
// ========================================================================================
Standard_Boolean QANewModTopOpe_Tools::BoolOpe(const TopoDS_Shape& theFace1,
                                               const TopoDS_Shape& theFace2,
                                               Standard_Boolean&   IsCommonFound,
                                               TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap) 
{
  IsCommonFound = Standard_False;
  theHistoryMap.Clear();
  gp_Dir aDNF1, aDNF2;
  Standard_Integer iSenseFlag;

  BOPAlgo_PaveFiller aDSFiller;
  BOPCol_ListOfShape aLS;
  aLS.Append(theFace1);
  aLS.Append(theFace2);
  aDSFiller.SetArguments(aLS);
  
  aDSFiller.Perform();
  if (aDSFiller.ErrorStatus()) {
    return Standard_False;
  }
  
  const BOPDS_PDS& pDS = aDSFiller.PDS();

  Standard_Integer aNb = 0, aNbSps;
  Standard_Integer i = 0;
  TopTools_IndexedMapOfShape aMapV;

  {
    BRepAlgoAPI_Common aCommon(theFace1, theFace2, aDSFiller);

    if(!aCommon.IsDone()) {
      return Standard_False;
    }

    TopExp_Explorer anExp(aCommon.Shape(), TopAbs_FACE);
    if(!anExp.More()) {
      IsCommonFound = Standard_False;
      return Standard_True;
    }

    IsCommonFound = Standard_True;
    TopExp::MapShapes(aCommon.Shape(), TopAbs_VERTEX, aMapV);
    // fill edge history.begin
    FillEdgeHistoryMap(aCommon, theHistoryMap);
    // fill edge history.end

    // fill face history.begin
    BOPDS_VectorOfInterfFF& aFFs = pDS->InterfFF();
    aNb = aFFs.Extent();
    Standard_Boolean bReverseFlag = Standard_True;
    Standard_Boolean fillhistory = Standard_True;

    for (i=0; i<aNb; ++i) {
      BOPDS_InterfFF& aFF = aFFs(i);
      Standard_Integer nF1, nF2;
      aFF.Indices(nF1, nF2);
    
      const TopoDS_Face& aF1 = *(TopoDS_Face*)(&pDS->Shape(nF1));
      const TopoDS_Face& aF2 = *(TopoDS_Face*)(&pDS->Shape(nF2));

      BOPCol_ListOfInteger aLSE;
      pDS->SharedEdges(nF1, nF2, aLSE, aDSFiller.Allocator());
      aNbSps = aLSE.Extent();
      
      if (!aNbSps) {
        fillhistory = Standard_False;
        continue;
      }
      
      Standard_Integer nE = aLSE.First();
      const TopoDS_Edge& aSpE = *(TopoDS_Edge*)(&pDS->Shape(nE));
    
      BOPTools_AlgoTools3D::GetNormalToFaceOnEdge (aSpE, aF1, aDNF1); 
      BOPTools_AlgoTools3D::GetNormalToFaceOnEdge (aSpE, aF2, aDNF2);
      iSenseFlag=BOPTools_AlgoTools3D::SenseFlag (aDNF1, aDNF2);

      if(iSenseFlag == 1) {
        fillhistory = Standard_True;
        bReverseFlag = Standard_False;
      }
      else if(iSenseFlag == -1) {
        fillhistory = Standard_True;
        bReverseFlag = Standard_True;
      }
      else
        fillhistory = Standard_False;
    }

    if(fillhistory) {

      for(; anExp.More(); anExp.Next()) {
        TopoDS_Shape aResShape = anExp.Current();

        if(theFace1.Orientation() == aResShape.Orientation()) {
          AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);

          if(bReverseFlag)
            aResShape.Reverse();
          AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);
        }
        else if(theFace2.Orientation() == aResShape.Orientation()) {
          AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);

          if(bReverseFlag)
            aResShape.Reverse();
          AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);
        }
        else {
          aResShape.Orientation(theFace1.Orientation());
          AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);
          aResShape.Orientation(theFace2.Orientation());

          if(bReverseFlag)
            aResShape.Reverse();
          AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);
        }
      }
    }
    // fill face history.end
  }
  {
    BRepAlgoAPI_Cut aCut1(theFace1, theFace2, aDSFiller);

    if(!aCut1.IsDone())
      return Standard_False;
    TopExp::MapShapes(aCut1.Shape(), TopAbs_VERTEX, aMapV);
    // fill edge history.begin
    FillEdgeHistoryMap(aCut1, theHistoryMap);
    // fill edge history.end

    // fill face history.begin
    TopExp_Explorer anExp(aCut1.Shape(), TopAbs_FACE);

    for(; anExp.More(); anExp.Next()) {
      TopoDS_Shape aResShape = anExp.Current();
      aResShape.Orientation(theFace1.Orientation());
      AddShapeToHistoryMap(theFace1, aResShape, theHistoryMap);
    }
    // fill face history.end
  }

  {
    BRepAlgoAPI_Cut aCut2(theFace1, theFace2, aDSFiller, Standard_False);

    if(!aCut2.IsDone())
      return Standard_False;
    TopExp::MapShapes(aCut2.Shape(), TopAbs_VERTEX, aMapV);
    // fill edge history.begin
    FillEdgeHistoryMap(aCut2, theHistoryMap);
    // fill edge history.end

    // fill face history.begin
    TopExp_Explorer anExp(aCut2.Shape(), TopAbs_FACE);

    for(; anExp.More(); anExp.Next()) {
      TopoDS_Shape aResShape = anExp.Current();
      aResShape.Orientation(theFace2.Orientation());
      AddShapeToHistoryMap(theFace2, aResShape, theHistoryMap);
    }
    // fill face history.end
  }
  
  // fill vertex history.begin
  BOPDS_VectorOfInterfVV& aVVs = pDS->InterfVV();
  aNb = aVVs.Extent();

  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfVV& aVVi = aVVs(i);
    if (!aVVi.HasIndexNew()) {
      continue;
    }
    Standard_Integer aNewShapeIndex = aVVi.IndexNew();

    const TopoDS_Shape& aNewVertex = pDS->Shape(aNewShapeIndex);

    if(!aMapV.Contains(aNewVertex)) {
      continue;
    }
    
    const TopoDS_Shape& aV1 = pDS->Shape(aVVi.Index1());
    const TopoDS_Shape& aV2 = pDS->Shape(aVVi.Index2());
    AddShapeToHistoryMap(aV1, aNewVertex, theHistoryMap);
    AddShapeToHistoryMap(aV2, aNewVertex, theHistoryMap);
  }

  BOPDS_VectorOfInterfVE& aVEs = pDS->InterfVE();
  aNb = aVEs.Extent();

  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfVE& aVEi = aVEs(i);
    
    Standard_Integer anIndex = aVEi.Index1();
    const TopoDS_Shape& aNewVertex = pDS->Shape(anIndex);

    if(!aMapV.Contains(aNewVertex))
      continue;

    AddShapeToHistoryMap(aNewVertex, aNewVertex, theHistoryMap);
  }

  BOPDS_VectorOfInterfVF& aVSs = pDS->InterfVF();
  aNb = aVSs.Extent();

  for (i = 0; i < aNb; ++i) {
    BOPDS_InterfVF& aVSi = aVSs(i);

    Standard_Integer anIndex = aVSi.Index1();
    const TopoDS_Shape& aNewVertex = pDS->Shape(anIndex);

    if(!aMapV.Contains(aNewVertex))
      continue;

    AddShapeToHistoryMap(aNewVertex, aNewVertex, theHistoryMap);
  }
  // fill vertex history.end
  return Standard_True;
}

// --------------------------------------------------------------------------------------------
// static function: AddShapeToHistoryMap
// purpose: 
// --------------------------------------------------------------------------------------------
Standard_Boolean AddShapeToHistoryMap(const TopoDS_Shape& theOldShape,
                                      const TopoDS_Shape& theNewShape,
                                      TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap) {

  if(!theHistoryMap.Contains(theOldShape)) {
    TopTools_ListOfShape aList;
    aList.Append(theNewShape);
    theHistoryMap.Add(theOldShape, aList);
    return Standard_True;
  }
  
  Standard_Boolean found = Standard_False;
  TopTools_ListOfShape& aList = theHistoryMap.ChangeFromKey(theOldShape);
  TopTools_ListIteratorOfListOfShape aVIt(aList);

  for(; aVIt.More(); aVIt.Next()) {
    if(theNewShape.IsSame(aVIt.Value())) {
      found = Standard_True;
      break;
    }
  }

  if(!found) {
    aList.Append(theNewShape);
  }
  return !found;
}

// --------------------------------------------------------------------------------------------
// static function: FillEdgeHistoryMap
// purpose: 
// --------------------------------------------------------------------------------------------
void FillEdgeHistoryMap(BRepAlgoAPI_BooleanOperation&              theBOP,
                        TopTools_IndexedDataMapOfShapeListOfShape& theHistoryMap) {

  TopExp_Explorer anExp;
  anExp.Init(theBOP.Shape1(), TopAbs_EDGE);

  for(; anExp.More(); anExp.Next()) {
    const TopTools_ListOfShape& aList = theBOP.Modified(anExp.Current());
    TopTools_ListIteratorOfListOfShape anIt(aList);

    for(; anIt.More(); anIt.Next()) {
      AddShapeToHistoryMap(anExp.Current(), anIt.Value(), theHistoryMap);
    }
  }

  anExp.Init(theBOP.Shape2(), TopAbs_EDGE);

  for(; anExp.More(); anExp.Next()) {
    const TopTools_ListOfShape& aList = theBOP.Modified(anExp.Current());
    TopTools_ListIteratorOfListOfShape anIt(aList);

    for(; anIt.More(); anIt.Next()) {
      AddShapeToHistoryMap(anExp.Current(), anIt.Value(), theHistoryMap);
    }
  }
}

// --------------------------------------------------------------------------------------------
// static function: SortVertexOnEdge
// purpose: 
// --------------------------------------------------------------------------------------------
void SortVertexOnEdge(const TopoDS_Edge&          theEdge,
                      const TopTools_ListOfShape& theListOfVertex,
                      TopTools_ListOfShape&       theListOfVertexSorted) {

  TopTools_DataMapOfIntegerShape mapiv;// mapiv.Find(iV) = V
  TColStd_IndexedMapOfReal mappar;     // mappar.FindIndex(parV) = iV
  TopTools_ListIteratorOfListOfShape itlove(theListOfVertex);
  
  for (; itlove.More(); itlove.Next()){
    const TopoDS_Vertex& v = TopoDS::Vertex(itlove.Value());
    Standard_Real par = BRep_Tool::Parameter(v, theEdge);
    Standard_Integer iv = mappar.Add(par);
    mapiv.Bind(iv,v);
  }
  Standard_Integer nv = mapiv.Extent();
  NCollection_Array1<Standard_Real> tabpar(1,nv);
  Standard_Integer i = 0;

  for ( i = 1; i <= nv; i++) {
    Standard_Real p = mappar.FindKey(i);
    tabpar.SetValue(i,p);
  }
  theListOfVertexSorted.Clear();
  std::sort (tabpar.begin(), tabpar.end());

  for (i = 1; i <= nv; i++) {
    Standard_Real par = tabpar.Value(i);
    Standard_Integer iv = mappar.FindIndex(par);
    const TopoDS_Shape& v = mapiv.Find(iv);
    theListOfVertexSorted.Append(v);
  }
}

// --------------------------------------------------------------------------------------------
// static function: GetEdgeState
// purpose: 
// --------------------------------------------------------------------------------------------
  static TopAbs_State GetEdgeState(const BOPDS_PDS& pDS,
                                   const Handle(BOPDS_PaveBlock)& aPB) 
{
  Standard_Integer j, aNbFI;
  Standard_Boolean bIn;
  TopAbs_State aState = TopAbs_ON;
  //
  const BOPDS_VectorOfFaceInfo& aVFI = pDS->FaceInfoPool();
  aNbFI = aVFI.Extent();
  //
  for (j = 0; j < aNbFI; ++j) {
    const BOPDS_FaceInfo& aFI = aVFI(j);
    bIn = aFI.PaveBlocksIn().Contains(aPB);
    if (bIn) {
      aState = TopAbs_IN;
      break;
    }
  }
  return aState;
}
