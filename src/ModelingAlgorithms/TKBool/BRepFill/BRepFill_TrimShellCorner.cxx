// Created on: 2003-10-21
// Created by: Mikhail KLOKOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepFill_TrimShellCorner.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeWire.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepTools_ReShape.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomLib.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <IntTools_BeanFaceIntersector.hxx>
#include <IntTools_Context.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_Tools.hxx>
#include <gp_Dir.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <ShapeFix_Edge.hxx>

static TopoDS_Edge FindEdgeCloseToBisectorPlane(const TopoDS_Vertex& theVertex,
                                                TopoDS_Compound&     theComp,
                                                const gp_Ax1&        theAxis);

static bool FindMiddleEdges(const TopoDS_Vertex&  theVertex1,
                                        const TopoDS_Vertex&  theVertex2,
                                        const gp_Ax1&         theAxis,
                                        TopoDS_Compound&      theComp,
                                        NCollection_List<TopoDS_Shape>& theElist);

static bool FindCommonVertex(const TopoDS_Edge&   theFirstEdge,
                                         const TopoDS_Edge&   theLastEdge,
                                         const TopoDS_Vertex& theFirstVertex,
                                         const TopoDS_Vertex& theLastVertex,
                                         TopoDS_Vertex&       theCommonVertex);

static bool FindCommonVertex(const BOPDS_PDS&       theDS,
                                         const int theEIndex1,
                                         const int theEIndex2,
                                         const gp_Vec&          theCrossDirection,
                                         TopoDS_Vertex&         theCommonVertex,
                                         double&         theParamOnE1,
                                         double&         theParamOnE2);

static bool SplitUEdges(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theUEdges,
                                    const BOPDS_PDS&                       theDS,
                                    const gp_Vec&                          theCrossDirection,
                                    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&    theHistMap);

static void StoreVedgeInHistMap(const occ::handle<NCollection_HArray1<TopoDS_Shape>>& theVEdges,
                                const int                 theIndex,
                                const TopoDS_Shape&                    theNewVedge,
                                NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&    theHistMap);

static void FindFreeVertices(const TopoDS_Shape&        theShape,
                             const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVerticesToAvoid,
                             NCollection_List<TopoDS_Shape>&      theListOfVertex);

static bool CheckAndOrientEdges(const NCollection_List<TopoDS_Shape>& theOrderedList,
                                            const gp_Pnt2d&             theFirstPoint,
                                            const gp_Pnt2d&             theLastPoint,
                                            const TopoDS_Face&          theFace,
                                            NCollection_List<TopoDS_Shape>&       theOrientedList);

static bool FillGap(const TopoDS_Vertex&   theFirstVertex,
                                const TopoDS_Vertex&   theLastVertex,
                                const gp_Pnt2d&        theFirstPoint,
                                const gp_Pnt2d&        theLastPoint,
                                const TopoDS_Face&     theFace,
                                const TopoDS_Compound& theSectionEdges,
                                NCollection_List<TopoDS_Shape>&  theOrderedList);

static bool FindNextEdge(const TopoDS_Vertex& theFirstVertex,
                                     const TopoDS_Vertex& theLastVertex,
                                     const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMapVE,
                                     const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theMapToAvoid,
                                     NCollection_List<TopoDS_Shape>& theOrderedList);

static bool FindVertex(const TopoDS_Edge&                        theEdge,
                                   const int                    theRank,
                                   const BOPDS_PDS&                          theDS,
                                   const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theHistMap,
                                   TopoDS_Vertex&                            theVertex,
                                   BOPDS_Pave&                               thePave);

static bool FindNextVertex(const int theEdgeIndex,
                                       const BOPDS_Pave&      thePrevPave,
                                       const BOPDS_PDS&       theDS,
                                       TopoDS_Vertex&         theNextVertex,
                                       BOPDS_Pave&            thePave);

static bool GetPave(const int theEdgeIndex,
                                const bool isFirst,
                                const BOPDS_PDS&       theDS,
                                BOPDS_Pave&            thePave);

static bool FindFromUEdge(const TopoDS_Edge&                        theUE1Old,
                                      const TopoDS_Edge&                        theUE2Old,
                                      const TopoDS_Edge&                        theUE1New,
                                      const TopoDS_Edge&                        theUE2New,
                                      const TopoDS_Face&                        theFace,
                                      const TopoDS_Compound&                    theSecEdges,
                                      const int                    theRank,
                                      const TopoDS_Edge&                        theBoundEdge,
                                      const int                    theBoundEdgeIndex,
                                      const BOPDS_PDS&                          theDS,
                                      const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theHistMap,
                                      TopoDS_Compound&                          theSecEdgesNew,
                                      NCollection_List<TopoDS_Shape>&                     theListOfWireEdges,
                                      BOPDS_Pave&                               theFoundPave,
                                      bool&                         isOnUEdge);

static bool FindFromVEdge(const BOPDS_Pave&                         thePrevPave,
                                      const bool&                   isOnUEdge,
                                      const TopoDS_Edge&                        theUE1Old,
                                      const TopoDS_Edge&                        theUE2Old,
                                      const TopoDS_Face&                        theFace,
                                      const TopoDS_Compound&                    theSecEdges,
                                      const int                    theRank,
                                      const TopoDS_Edge&                        theBoundEdge,
                                      const int                    theBoundEdgeIndex,
                                      const BOPDS_PDS&                          theDS,
                                      const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theHistMap,
                                      NCollection_List<TopoDS_Shape>&                     theListOfWireEdges,
                                      bool&                         isSectionFound);

static void RemoveEdges(const TopoDS_Compound&      theSourceComp,
                        const NCollection_List<TopoDS_Shape>& theListToRemove,
                        TopoDS_Compound&            theResultComp);

static bool FilterSectionEdges(const NCollection_Vector<BOPDS_Curve>& theBCurves,
                                           const TopoDS_Face&         theSecPlane,
                                           const BOPDS_PDS&           theDS,
                                           TopoDS_Compound&           theResult);

static bool GetUEdges(const int                 theIndex,
                                  const int                 theRank,
                                  const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theUEdges,
                                  const TopoDS_Edge&                     theBoundEdge,
                                  const TopoDS_Face&                     theFace,
                                  TopoDS_Edge&                           theFirstUEdge,
                                  TopoDS_Edge&                           theSecondUEdge);

static double ComputeAveragePlaneAndMaxDeviation(const TopoDS_Shape& aWire,
                                                        gp_Pln&             thePlane,
                                                        bool&   IsSingular);

static void UpdateSectionEdge(TopoDS_Edge&         theEdge,
                              const TopoDS_Vertex& theConstVertex,
                              TopoDS_Vertex&       theVertex,
                              const double  theParam);

//=================================================================================================

BRepFill_TrimShellCorner::BRepFill_TrimShellCorner(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theFaces,
                                                   const BRepFill_TransitionStyle theTransition,
                                                   const gp_Ax2&                  theAxeOfBisPlane,
                                                   const gp_Vec& theIntPointCrossDir)
    : myTransition(theTransition),
      myAxeOfBisPlane(theAxeOfBisPlane),
      myIntPointCrossDir(theIntPointCrossDir),
      myDone(false),
      myHasSection(false)
{
  myFaces                 = new NCollection_HArray2<TopoDS_Shape>(theFaces->LowerRow(),
                                        theFaces->UpperRow(),
                                        theFaces->LowerCol(),
                                        theFaces->UpperCol());
  myFaces->ChangeArray2() = theFaces->Array2();
}

//=================================================================================================

void BRepFill_TrimShellCorner::AddBounds(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theBounds)
{
  myBounds                 = new NCollection_HArray2<TopoDS_Shape>(theBounds->LowerRow(),
                                         theBounds->UpperRow(),
                                         theBounds->LowerCol(),
                                         theBounds->UpperCol());
  myBounds->ChangeArray2() = theBounds->Array2();
}

//=================================================================================================

void BRepFill_TrimShellCorner::AddUEdges(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theUEdges)
{
  myUEdges                 = new NCollection_HArray2<TopoDS_Shape>(theUEdges->LowerRow(),
                                         theUEdges->UpperRow(),
                                         theUEdges->LowerCol(),
                                         theUEdges->UpperCol());
  myUEdges->ChangeArray2() = theUEdges->Array2();
}

//=================================================================================================

void BRepFill_TrimShellCorner::AddVEdges(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theVEdges,
                                         const int                 theIndex)
{
  myVEdges = new NCollection_HArray1<TopoDS_Shape>(theVEdges->LowerRow(), theVEdges->UpperRow());

  for (int i = theVEdges->LowerRow(); i <= theVEdges->UpperRow(); i++)
    myVEdges->SetValue(i, theVEdges->Value(i, theIndex));
}

//=================================================================================================

void BRepFill_TrimShellCorner::Perform()
{
  int anIndex1, anIndex2, nF1, nF2, i, j, aNbP, aNbC;
  bool bhassec;

  myDone = false;
  myHistMap.Clear();

  if (myFaces->RowLength() != 2)
    return;
  int ii = 0, jj = 0;
  BRep_Builder     aBB;

  for (jj = myFaces->LowerCol(); jj <= myFaces->UpperCol(); jj++)
  {
    TopoDS_Shell aShell;
    aBB.MakeShell(aShell);

    for (ii = myFaces->LowerRow(); ii <= myFaces->UpperRow(); ii++)
    {
      aBB.Add(aShell, myFaces->Value(ii, jj));
    }
    aShell.Closed(BRep_Tool::IsClosed(aShell));

    if (jj == myFaces->LowerCol())
    {
      myShape1 = aShell;
    }
    else
    {
      myShape2 = aShell;
    }
  }

  double   aMaxTol = 0.;
  TopExp_Explorer anExp(myShape1, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next())
  {
    aMaxTol = std::max(aMaxTol, BRep_Tool::Tolerance(TopoDS::Vertex(anExp.Current())));
  }

  anExp.Init(myShape2, TopAbs_VERTEX);
  for (; anExp.More(); anExp.Next())
  {
    aMaxTol = std::max(aMaxTol, BRep_Tool::Tolerance(TopoDS::Vertex(anExp.Current())));
  }

  double        aFuzzy = 4. * Precision::Confusion();
  BOPAlgo_PaveFiller   aPF;
  NCollection_List<TopoDS_Shape> aLS;
  aLS.Append(myShape1);
  aLS.Append(myShape2);
  aPF.SetArguments(aLS);
  if (aMaxTol < 1.005 * Precision::Confusion())
  {
    aFuzzy = std::max(aPF.FuzzyValue(), aFuzzy);
    aPF.SetFuzzyValue(aFuzzy);
  }
  //
  aPF.Perform();
  if (aPF.HasErrors())
  {
    return;
  }
  //
  const BOPDS_PDS& theDS = aPF.PDS();
  //
  NCollection_Vector<BOPDS_InterfFF>& aFFs   = theDS->InterfFF();
  int        aNbFFs = aFFs.Length();

  if (!SplitUEdges(myUEdges, theDS, myIntPointCrossDir, myHistMap))
  {
    return;
  }

  for (ii = myFaces->LowerRow(); ii <= myFaces->UpperRow(); ii++)
  {
    TopoDS_Shape aF1 = myFaces->Value(ii, myFaces->LowerCol());
    TopoDS_Shape aF2 = myFaces->Value(ii, myFaces->UpperCol());
    //
    anIndex1 = theDS->Index(aF1);
    anIndex2 = theDS->Index(aF2);

    if ((anIndex1 == -1) || (anIndex2 == -1))
      continue;

    for (i = 0; i < aNbFFs; ++i)
    {
      BOPDS_InterfFF& aFFi = aFFs(i);
      aFFi.Indices(nF1, nF2);
      //
      NCollection_Vector<BOPDS_Point>& aVP       = aFFi.ChangePoints();
      aNbP                           = aVP.Length();
      const NCollection_Vector<BOPDS_Curve>& aVC = aFFi.Curves();
      aNbC                           = aVC.Length();
      if (!aNbP && !aNbC)
      {
        if (!theDS->HasInterfSubShapes(nF1, nF2))
        {
          continue;
        }
      }
      //
      if ((nF1 == anIndex1) && (nF2 == anIndex2))
      {
        bhassec = false;
        //
        for (j = 0; j < aNbC; ++j)
        {
          const BOPDS_Curve&           aBCurve    = aVC(j);
          const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aSectEdges = aBCurve.PaveBlocks();
          //
          if (aSectEdges.Extent())
          {
            bhassec = true;
            break;
          }
        }

        if (!bhassec)
        {
          if (!MakeFacesNonSec(ii, theDS, anIndex1, anIndex2))
          {
            myHistMap.Clear();
            return;
          }
        }
        else
        {
          if (!MakeFacesSec(ii, theDS, anIndex1, anIndex2, i))
          {
            myHistMap.Clear();
            return;
          }
        }
        break;
      }
    }
  }
  myDone = true;
}

//=================================================================================================

bool BRepFill_TrimShellCorner::IsDone() const
{
  return myDone;
}

//=================================================================================================

bool BRepFill_TrimShellCorner::HasSection() const
{
  return myHasSection;
}

//=================================================================================================

void BRepFill_TrimShellCorner::Modified(const TopoDS_Shape&   theShape,
                                        NCollection_List<TopoDS_Shape>& theModified)
{
  theModified.Clear();

  if (myHistMap.IsBound(theShape))
  {
    theModified = myHistMap.Find(theShape);
  }
}

// ----------------------------------------------------------------------------------------------------
// function: MakeFacesNonSec
// purpose:         Updates <myHistMap> by new faces in the case when old faces do not intersect
// ----------------------------------------------------------------------------------------------------
bool BRepFill_TrimShellCorner::MakeFacesNonSec(const int theIndex,
                                                           const BOPDS_PDS&       theDS,
                                                           const int theFaceIndex1,
                                                           const int theFaceIndex2)
{
  bool bHasNewEdge = false;
  TopoDS_Edge      aNewEdge;

  BRep_Builder        aBB;
  const TopoDS_Shape& aE1 = myBounds->Value(theIndex, 1);
  const TopoDS_Shape& aE2 = myBounds->Value(theIndex, 2);

  // search common vertex between bounds. begin
  TopoDS_Vertex    aCommonVertex;
  int anIndex1 = theDS->Index(aE1);
  int anIndex2 = theDS->Index(aE2);
  double    apar1 = 0., apar2 = 0.;

  bool bvertexfound =
    FindCommonVertex(theDS, anIndex1, anIndex2, myIntPointCrossDir, aCommonVertex, apar1, apar2);
  // search common vertex between bounds. end

  occ::handle<BRepTools_ReShape> aSubstitutor = new BRepTools_ReShape();

  // search common vertices between uedges. begin
  NCollection_List<TopoDS_Shape> aCommonVertices;
  int     acommonflag = 0; // 0 - no, 1 - first pair, 2 - second pair, 3 - both
  int     ueit = 0, eindex = 0;

  for (ueit = 1, eindex = theIndex; ueit <= 2; ueit++, eindex++)
  {
    const TopoDS_Shape& aShape1 = myUEdges->Value(eindex, myUEdges->LowerCol());
    const TopoDS_Shape& aShape2 = myUEdges->Value(eindex, myUEdges->UpperCol());
    TopoDS_Edge         aUE1    = TopoDS::Edge(aShape1);
    TopoDS_Edge         aUE2    = TopoDS::Edge(aShape2);

    if (myHistMap.IsBound(aShape1))
    {
      const NCollection_List<TopoDS_Shape>& lst = myHistMap.Find(aShape1);

      if (!lst.IsEmpty())
        aUE1 = TopoDS::Edge(lst.First());
    }

    if (myHistMap.IsBound(aShape2))
    {
      const NCollection_List<TopoDS_Shape>& lst = myHistMap.Find(aShape2);

      if (!lst.IsEmpty())
        aUE2 = TopoDS::Edge(lst.First());
    }

    if (!aShape1.IsSame(aUE1))
      aSubstitutor->Replace(aShape1.Oriented(TopAbs_FORWARD), aUE1.Oriented(TopAbs_FORWARD));

    if (!aShape2.IsSame(aUE2))
      aSubstitutor->Replace(aShape2.Oriented(TopAbs_FORWARD), aUE2.Oriented(TopAbs_FORWARD));

    TopoDS_Vertex V1 = TopExp::LastVertex(aUE1);
    TopoDS_Vertex V2 = TopExp::FirstVertex(aUE2);

    if (V1.IsSame(V2))
    {
      acommonflag = (acommonflag == 0) ? ueit : 3;
      aCommonVertices.Append(V1);
    }
  }
  // search common vertices between uedges. end

  if (bvertexfound)
  {
    if (aCommonVertices.Extent() != 1)
      return false;

    if (acommonflag == 1)
      aNewEdge = BRepLib_MakeEdge(TopoDS::Vertex(aCommonVertices.First()), aCommonVertex);
    else
      aNewEdge = BRepLib_MakeEdge(aCommonVertex, TopoDS::Vertex(aCommonVertices.First()));

    bHasNewEdge = true;
  }

  if (aCommonVertices.Extent() == 2)
  {
    aNewEdge    = BRepLib_MakeEdge(TopoDS::Vertex(aCommonVertices.First()),
                                TopoDS::Vertex(aCommonVertices.Last()));
    bHasNewEdge = true;
  }
  int fit = 0;

  for (fit = 1; fit <= 2; fit++)
  {
    TopoDS_Compound     aComp;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapV;
    aBB.MakeCompound(aComp);

    for (ueit = 1, eindex = theIndex; ueit <= 2; ueit++, eindex++)
    {
      const TopoDS_Shape& aShape = myUEdges->Value(eindex, myUEdges->LowerCol() + fit - 1);
      TopoDS_Shape        aUE    = aShape;

      if (myHistMap.IsBound(aShape))
      {
        const NCollection_List<TopoDS_Shape>& lst = myHistMap.Find(aShape);

        if (!lst.IsEmpty())
          aUE = TopoDS::Edge(lst.First());
      }
      const TopoDS_Shape& aV =
        (fit == 1) ? TopExp::FirstVertex(TopoDS::Edge(aUE)) : TopExp::LastVertex(TopoDS::Edge(aUE));
      aMapV.Add(aV);
      aBB.Add(aComp, aUE);
    }

    if (bHasNewEdge)
    {
      aBB.Add(aComp, aNewEdge);
      StoreVedgeInHistMap(myVEdges, theIndex, aNewEdge, myHistMap);
    }

    NCollection_List<TopoDS_Shape> alonevertices;
    FindFreeVertices(aComp, aMapV, alonevertices);

    if (!alonevertices.IsEmpty() && (alonevertices.Extent() != 2))
      return false;

    int   aFaceIndex = (fit == 1) ? theFaceIndex1 : theFaceIndex2;
    TopoDS_Shape       aFace      = theDS->Shape(aFaceIndex);
    TopAbs_Orientation aFaceOri   = aFace.Orientation();
    aFace.Orientation(TopAbs_FORWARD);

    TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

    if (bHasNewEdge)
    {
      aNewEdge.Orientation(TopAbs_FORWARD);

      // Refer to BrepFill_Sweep.cxx BuildEdge Construct an edge via an iso
      gp_Pnt        P1, P2;
      double p11, p12, p21, p22;
      P1 = BRep_Tool::Pnt(TopExp::FirstVertex(TopoDS::Edge(aNewEdge)));
      P2 = BRep_Tool::Pnt(TopExp::LastVertex(TopoDS::Edge(aNewEdge)));

      TopoDS_Edge aERef = TopoDS::Edge(fit == 1 ? aE1 : aE2);
      p11               = P1.Distance(BRep_Tool::Pnt(TopExp::FirstVertex(aERef)));
      p22               = P2.Distance(BRep_Tool::Pnt(TopExp::LastVertex(aERef)));
      p12               = P1.Distance(BRep_Tool::Pnt(TopExp::LastVertex(aERef)));
      p21               = P2.Distance(BRep_Tool::Pnt(TopExp::FirstVertex(aERef)));

      if (p11 > p12 && p22 > p21)
      {
        aNewEdge.Reverse();
      }

      // for nonPlane surface, we should add pCurve
      occ::handle<ShapeFix_Edge> sfe = new ShapeFix_Edge();
      sfe->FixAddPCurve(aNewEdge, TopoDS::Face(aFace), false);
    }

    NCollection_List<TopoDS_Shape> aOrderedList;

    if (!alonevertices.IsEmpty())
    {
      int anEIndex = (fit == 1) ? anIndex1 : anIndex2;
      bool bfound1  = false;
      bool bfound2  = false;
      double    aparam1 = 0., aparam2 = 0.;

      NCollection_List<BOPDS_Pave> aLP;
      theDS->Paves(anEIndex, aLP);
      NCollection_List<BOPDS_Pave>::Iterator aIt;
      aIt.Initialize(aLP);
      for (; aIt.More(); aIt.Next())
      {
        const BOPDS_Pave&   aPave = aIt.Value();
        const TopoDS_Shape& aV    = theDS->Shape(aPave.Index());

        if (aV.IsSame(alonevertices.First()))
        {
          if (!bfound1)
          {
            aparam1 = aPave.Parameter();
            bfound1 = true;
          }
        }

        if (aV.IsSame(alonevertices.Last()))
        {
          if (!bfound2)
          {
            aparam2 = aPave.Parameter();
            bfound2 = true;
          }
        }
      }

      if (bfound1 && bfound2)
      {
        TopoDS_Edge aNewBoundE;

        if (fit == 1)
        {
          aNewBoundE = TopoDS::Edge(aE1.EmptyCopied());
        }
        else
        {
          aNewBoundE = TopoDS::Edge(aE2.EmptyCopied());
        }
        TopoDS_Vertex aV1, aV2;

        if (aparam1 < aparam2)
        {
          aV1 = TopoDS::Vertex(alonevertices.First());
          aV2 = TopoDS::Vertex(alonevertices.Last());
        }
        else
        {
          aV1               = TopoDS::Vertex(alonevertices.Last());
          aV2               = TopoDS::Vertex(alonevertices.First());
          double tmp = aparam1;
          aparam1           = aparam2;
          aparam2           = tmp;
        }
        aV1.Orientation(TopAbs_FORWARD);
        aV2.Orientation(TopAbs_REVERSED);
        aBB.Add(aNewBoundE, aV1);
        aBB.Add(aNewBoundE, aV2);
        aBB.Range(aNewBoundE, aparam1, aparam2);
        aNewBoundE.Orientation(TopAbs_FORWARD);

        aOrderedList.Append(aNewBoundE);

        if (bHasNewEdge)
        {
          TopExp_Explorer  anExpV(aNewEdge, TopAbs_VERTEX);
          bool bfoundv = false;

          for (; !bfoundv && anExpV.More(); anExpV.Next())
          {
            if (aV2.IsSame(anExpV.Current()))
              bfoundv = true;
          }

          if (bfoundv)
            aOrderedList.Append(aNewEdge);
          else
            aOrderedList.Prepend(aNewEdge);
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      if (bHasNewEdge)
      {
        aOrderedList.Append(aNewEdge);
      }
    }

    if (!aOrderedList.IsEmpty())
    {
      TopoDS_Wire aW;
      aBB.MakeWire(aW);
      NCollection_List<TopoDS_Shape>::Iterator anItE(aOrderedList);

      for (; anItE.More(); anItE.Next())
      {
        aBB.Add(aW, anItE.Value());
      }
      if (fit == 1)
        aSubstitutor->Replace(aE1.Oriented(TopAbs_FORWARD), aW);
      else
        aSubstitutor->Replace(aE2.Oriented(TopAbs_FORWARD), aW);
    }

    aSubstitutor->Apply(aFace);
    TopoDS_Shape aNewFace = aSubstitutor->Value(aFace);
    aNewFace.Orientation(aFaceOri);
    NCollection_List<TopoDS_Shape> atmpList;
    atmpList.Append(aNewFace);
    myHistMap.Bind(aFace, atmpList);

    anExpE.Init(aFace, TopAbs_EDGE);

    for (; anExpE.More(); anExpE.Next())
    {
      TopoDS_Shape aNewValue = aSubstitutor->Value(anExpE.Current());

      if (aNewValue.IsNull() || aNewValue.IsSame(anExpE.Current()))
        continue;

      if (myHistMap.IsBound(anExpE.Current()))
        continue;
      NCollection_List<TopoDS_Shape> aListOfNewEdge;
      TopExp_Explorer      anExpE2(aNewValue, TopAbs_EDGE);

      for (; anExpE2.More(); anExpE2.Next())
      {
        aListOfNewEdge.Append(anExpE2.Current());
      }
      myHistMap.Bind(anExpE.Current(), aListOfNewEdge);
    }
  }

  return true;
}

// ----------------------------------------------------------------------------------------------------
// function: MakeFacesSec
// purpose:  Updates <myHistMap> by new faces in the case when old faces intersect each other
// ----------------------------------------------------------------------------------------------------
bool BRepFill_TrimShellCorner::MakeFacesSec(const int theIndex,
                                                        const BOPDS_PDS&       theDS,
                                                        const int theFaceIndex1,
                                                        const int theFaceIndex2,
                                                        const int theSSInterfIndex)
{
  const NCollection_Vector<BOPDS_InterfFF>& aFFs     = theDS->InterfFF();
  const BOPDS_InterfFF&         aFFi     = aFFs(theSSInterfIndex);
  const NCollection_Vector<BOPDS_Curve>&    aBCurves = aFFi.Curves();

  TopoDS_Compound aSecEdges;
  TopoDS_Face     aSecPlane;

  if (!FilterSectionEdges(aBCurves, aSecPlane, theDS, aSecEdges))
    return false;

  // Extract vertices on the intersection of correspondent U-edges
  const TopoDS_Shape& LeftE1  = myUEdges->Value(theIndex, 1);
  const TopoDS_Shape& LeftE2  = myUEdges->Value(theIndex, 2);
  const TopoDS_Shape& RightE1 = myUEdges->Value(theIndex + 1, 1);
  const TopoDS_Shape& RightE2 = myUEdges->Value(theIndex + 1, 2);

  int IndexOfLeftE1  = theDS->Index(LeftE1);
  int IndexOfLeftE2  = theDS->Index(LeftE2);
  int IndexOfRightE1 = theDS->Index(RightE1);
  int IndexOfRightE2 = theDS->Index(RightE2);

  TopoDS_Vertex FirstVertex, LastVertex;
  double ParamOnLeftE1, ParamOnLeftE2, ParamOnRightE1, ParamOnRightE2;
  FindCommonVertex(theDS,
                   IndexOfLeftE1,
                   IndexOfLeftE2,
                   myIntPointCrossDir,
                   FirstVertex,
                   ParamOnLeftE1,
                   ParamOnLeftE2);
  FindCommonVertex(theDS,
                   IndexOfRightE1,
                   IndexOfRightE2,
                   myIntPointCrossDir,
                   LastVertex,
                   ParamOnRightE1,
                   ParamOnRightE2);

  TopoDS_Shape     SecWire;
  gp_Pln           SecPlane;
  bool IsSingular;
  bool WireFound =
    ChooseSection(aSecEdges, FirstVertex, LastVertex, SecWire, SecPlane, IsSingular);

  if (WireFound)
  {
    // aSecEdges = SecWire;
    TopoDS_Compound aComp;
    BRep_Builder    BB;
    BB.MakeCompound(aComp);
    TopExp_Explorer explo(SecWire, TopAbs_EDGE);

    for (; explo.More(); explo.Next())
      BB.Add(aComp, explo.Current());
    aSecEdges = aComp;

    StoreVedgeInHistMap(myVEdges, theIndex, SecWire, myHistMap);
  }

  NCollection_List<TopoDS_Shape> aCommonVertices;
  //  int acommonflag = 0; // 0 - no, 1 - first pair, 2 - second pair, 3 - both
  int          fit          = 0; //, ueit = 0, eindex = 0, i = 0;
  occ::handle<BRepTools_ReShape> aSubstitutor = new BRepTools_ReShape();

  for (fit = 0; fit < 2; fit++)
  {
    int   aFaceIndex = (fit == 0) ? theFaceIndex1 : theFaceIndex2;
    TopoDS_Face        aFace      = TopoDS::Face(theDS->Shape(aFaceIndex));
    TopAbs_Orientation aFaceOri   = aFace.Orientation();
    TopoDS_Face        aFaceF     = aFace;
    aFaceF.Orientation(TopAbs_FORWARD);
    TopoDS_Edge aBoundEdge = TopoDS::Edge(myBounds->Value(theIndex, myBounds->LowerCol() + fit));
    int aBoundEdgeIndex = theDS->Index(aBoundEdge);
    TopoDS_Edge      aUE1;
    TopoDS_Edge      aUE2;

    if (!GetUEdges(theIndex, fit, myUEdges, aBoundEdge, aFaceF, aUE1, aUE2))
      return false;

    TopoDS_Edge aUE1old = aUE1;
    TopoDS_Edge aUE2old = aUE2;

    if (myHistMap.IsBound(aUE1))
    {
      const NCollection_List<TopoDS_Shape>& lst = myHistMap.Find(aUE1);

      if (!lst.IsEmpty())
      {
        const TopoDS_Shape& anEdge = lst.First().Oriented(aUE1.Orientation());

        if (!aUE1.IsSame(anEdge))
          aSubstitutor->Replace(aUE1.Oriented(TopAbs_FORWARD), anEdge.Oriented(TopAbs_FORWARD));
        aUE1 = TopoDS::Edge(anEdge);
      }
    }

    if (myHistMap.IsBound(aUE2))
    {
      const NCollection_List<TopoDS_Shape>& lst = myHistMap.Find(aUE2);

      if (!lst.IsEmpty())
      {
        const TopoDS_Shape& anEdge = lst.First().Oriented(aUE2.Orientation());

        if (!aUE2.IsSame(anEdge))
          aSubstitutor->Replace(aUE2.Oriented(TopAbs_FORWARD), anEdge.Oriented(TopAbs_FORWARD));
        aUE2 = TopoDS::Edge(anEdge);
      }
    }
    TopoDS_Vertex        aPrevVertex, aNextVertex;
    TopoDS_Compound      aCompOfSecEdges = aSecEdges;
    NCollection_List<TopoDS_Shape> aListOfWireEdges;
    BRep_Builder         aBB;
    BOPDS_Pave           aPave1;
    bool     isPave1OnUEdge = true;

    if (FindFromUEdge(aUE1old,
                      aUE2old,
                      aUE1,
                      aUE2,
                      aFace,
                      aSecEdges,
                      fit,
                      aBoundEdge,
                      aBoundEdgeIndex,
                      theDS,
                      myHistMap,
                      aCompOfSecEdges,
                      aListOfWireEdges,
                      aPave1,
                      isPave1OnUEdge))
    {
      NCollection_List<TopoDS_Shape> aSecondListOfEdges;
      bool     bisSectionFound = false;

      if (!FindFromVEdge(aPave1,
                         isPave1OnUEdge,
                         aUE1old,
                         aUE2old,
                         aFace,
                         aCompOfSecEdges,
                         fit,
                         aBoundEdge,
                         aBoundEdgeIndex,
                         theDS,
                         myHistMap,
                         aSecondListOfEdges,
                         bisSectionFound))
      {
        return false;
      }

      if (!bisSectionFound && aListOfWireEdges.IsEmpty())
      {
        return false;
      }
      aListOfWireEdges.Append(aSecondListOfEdges);
    }
    else
    {
      return false;
    }

    if (!aListOfWireEdges.IsEmpty())
    {
      TopoDS_Wire aW;
      aBB.MakeWire(aW);
      NCollection_List<TopoDS_Shape>::Iterator aEIt(aListOfWireEdges);

      for (; aEIt.More(); aEIt.Next())
      {
        if (!aBoundEdge.IsSame(aEIt.Value()))
          aBB.Add(aW, aEIt.Value());
      }
      aSubstitutor->Replace(aBoundEdge.Oriented(TopAbs_FORWARD), aW);
    }

    aSubstitutor->Apply(aFace);
    TopoDS_Shape aNewFace = aSubstitutor->Value(aFace);
    aNewFace.Orientation(aFaceOri);
    NCollection_List<TopoDS_Shape> atmpList;
    atmpList.Append(aNewFace);
    myHistMap.Bind(aFace, atmpList);

    TopExp_Explorer anExpE(aFace, TopAbs_EDGE);

    for (; anExpE.More(); anExpE.Next())
    {
      TopoDS_Shape aNewValue = aSubstitutor->Value(anExpE.Current());

      if (aNewValue.IsNull() || aNewValue.IsSame(anExpE.Current()))
        continue;

      if (myHistMap.IsBound(anExpE.Current()))
        continue;
      NCollection_List<TopoDS_Shape> aListOfNewEdge;
      TopExp_Explorer      anExpE2(aNewValue, TopAbs_EDGE);

      for (; anExpE2.More(); anExpE2.Next())
      {
        aListOfNewEdge.Append(anExpE2.Current());
      }
      myHistMap.Bind(anExpE.Current(), aListOfNewEdge);
    }
  }
  return true;
}

//=================================================================================================

bool BRepFill_TrimShellCorner::ChooseSection(const TopoDS_Shape&  Comp,
                                                         const TopoDS_Vertex& theFirstVertex,
                                                         const TopoDS_Vertex& theLastVertex,
                                                         TopoDS_Shape&        resWire,
                                                         gp_Pln&              resPlane,
                                                         bool&    IsSingular)
{
  IsSingular = false;

  int ind, i, j;
  BRep_Builder     BB;

  if (myTransition == BRepFill_Right && !theFirstVertex.IsNull()
      && !theLastVertex.IsNull()) // the case where section wire goes from
  // its known first vertex to its known last vertex
  {
    TopoDS_Wire NewWire;
    BB.MakeWire(NewWire);

    TopoDS_Compound OldComp;
    BB.MakeCompound(OldComp);
    TopoDS_Iterator iter(Comp);
    for (; iter.More(); iter.Next())
      BB.Add(OldComp, iter.Value());

    TopoDS_Edge FirstEdge =
      FindEdgeCloseToBisectorPlane(theFirstVertex, OldComp, myAxeOfBisPlane.Axis());
    if (FirstEdge.IsNull())
      return false;

    iter.Initialize(OldComp);
    if (!iter.More())
    {
      iter.Initialize(Comp);
      BB.Add(OldComp, iter.Value());
    }
    TopoDS_Edge LastEdge =
      FindEdgeCloseToBisectorPlane(theLastVertex, OldComp, myAxeOfBisPlane.Axis());
    if (LastEdge.IsNull())
      return false;

    if (FirstEdge.IsNull() || LastEdge.IsNull())
    {
      return false;
    }

    BB.Add(NewWire, FirstEdge);

    if (!FirstEdge.IsSame(LastEdge))
    {
      TopoDS_Vertex    aCommonVertex;
      bool CommonVertexExists =
        FindCommonVertex(FirstEdge, LastEdge, theFirstVertex, theLastVertex, aCommonVertex);
      if (CommonVertexExists)
        BB.Add(NewWire, LastEdge);
      else
      {
        TopoDS_Vertex Vertex1, Vertex2, V1, V2;
        TopExp::Vertices(FirstEdge, V1, V2);
        Vertex1 = (theFirstVertex.IsSame(V1)) ? V2 : V1;
        TopExp::Vertices(LastEdge, V1, V2);
        Vertex2 = (theLastVertex.IsSame(V1)) ? V2 : V1;

        NCollection_List<TopoDS_Shape> MiddleEdges;
        if (FindMiddleEdges(Vertex1, Vertex2, myAxeOfBisPlane.Axis(), OldComp, MiddleEdges))
        {
          NCollection_List<TopoDS_Shape>::Iterator itl(MiddleEdges);
          for (; itl.More(); itl.Next())
            BB.Add(NewWire, itl.Value());
          BB.Add(NewWire, LastEdge);
        }
        else
        {
          // trim <FirstEdge> and <LastEdge> in the points of extrema
          // these points become new vertex with centre between them
          BRepExtrema_ExtCC Extrema(FirstEdge, LastEdge);
          if (Extrema.IsDone() && Extrema.NbExt() > 0)
          {
            int imin = 1;
            for (i = 2; i <= Extrema.NbExt(); i++)
              if (Extrema.SquareDistance(i) < Extrema.SquareDistance(imin))
                imin = i;

            double aMinDist         = sqrt(Extrema.SquareDistance(imin));
            double ParamOnFirstEdge = Extrema.ParameterOnE1(imin);
            double ParamOnLastEdge  = Extrema.ParameterOnE2(imin);
            gp_Pnt        PointOnFirstEdge = Extrema.PointOnE1(imin);
            gp_Pnt        PointOnLastEdge  = Extrema.PointOnE2(imin);
            gp_Pnt        MidPnt((PointOnFirstEdge.XYZ() + PointOnLastEdge.XYZ()) / 2);
            aCommonVertex = BRepLib_MakeVertex(MidPnt);
            BB.UpdateVertex(aCommonVertex, 1.001 * aMinDist / 2);

            UpdateSectionEdge(FirstEdge, theFirstVertex, aCommonVertex, ParamOnFirstEdge);
            UpdateSectionEdge(LastEdge, theLastVertex, aCommonVertex, ParamOnLastEdge);

            BB.Add(NewWire, LastEdge);
          }
        }
      }
    }

    resWire  = NewWire;
    resPlane = gp_Pln(myAxeOfBisPlane);
    return true;
  }

  // General case: try to find continuous section closest to bisector plane
  TopoDS_Compound OldComp;
  BRep_Builder    B;
  B.MakeCompound(OldComp);
  TopoDS_Iterator iter(Comp);
  for (; iter.More(); iter.Next())
    B.Add(OldComp, iter.Value());

  bool anError = false;
  // TopoDS_Wire NewWire [2];
  NCollection_Sequence<TopoDS_Shape> Wseq;
  for (;;)
  {
    TopExp_Explorer explo(OldComp, TopAbs_EDGE);
    if (!explo.More())
      break;
    TopoDS_Edge FirstEdge = TopoDS::Edge(explo.Current());
    TopoDS_Wire NewWire   = BRepLib_MakeWire(FirstEdge);
    B.Remove(OldComp, FirstEdge);
    if (NewWire.Closed())
    {
      Wseq.Append(NewWire);
      continue;
    }

    for (;;)
    {
      TopoDS_Vertex Extremity[2];
      TopExp::Vertices(NewWire, Extremity[0], Extremity[1]);
      if (Extremity[0].IsNull() || Extremity[1].IsNull())
      {
        anError = true;
        break;
      }
      NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> VEmap;
      TopExp::MapShapesAndAncestors(OldComp, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
      NCollection_List<TopoDS_Shape> Vedges[2];
      for (j = 0; j < 2; j++)
        if (VEmap.Contains(Extremity[j]))
          Vedges[j] = VEmap.FindFromKey(Extremity[j]);
      if (Vedges[0].IsEmpty() && Vedges[1].IsEmpty())
        // no more edges in OldComp to continue NewWire
        break;
      bool Modified = false;
      for (j = 0; j < 2; j++)
      {
        if (Vedges[j].Extent() == 1)
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(Vedges[j].First());
          NewWire                   = BRepLib_MakeWire(NewWire, anEdge);
          B.Remove(OldComp, anEdge);
          Modified = true;
        }
      }
      if (!Modified) // only multiple connections
      {
        ind = (Vedges[0].IsEmpty()) ? 1 : 0;
        NCollection_Sequence<TopoDS_Shape>           Edges;
        NCollection_List<TopoDS_Shape>::Iterator itl(Vedges[ind]);
        for (; itl.More(); itl.Next())
          Edges.Append(itl.Value());
        int theind       = 0;
        double    MinDeviation = RealLast();
        for (j = 1; j <= Edges.Length(); j++)
        {
          TopoDS_Wire      aWire = BRepLib_MakeWire(NewWire, TopoDS::Edge(Edges(j)));
          gp_Pln           aPlane;
          bool issing;
          double    Deviation = ComputeAveragePlaneAndMaxDeviation(aWire, aPlane, issing);
          if (Deviation < MinDeviation)
          {
            MinDeviation = Deviation;
            theind       = j;
          }
        }
        NewWire = BRepLib_MakeWire(NewWire, TopoDS::Edge(Edges(theind)));
        B.Remove(OldComp, Edges(theind));
      }
      if (NewWire.Closed())
        break;
    }
    Wseq.Append(NewWire);
    if (anError)
      break;
  }

  double   MinAngle = RealLast();
  TopExp_Explorer Explo(OldComp, TopAbs_EDGE);
  if (!anError && !Explo.More()) // wires are built successfully and compound <OldComp> is empty
  {
    if (Wseq.Length() == 1) // only one wire => it becomes result
    {
      resWire = Wseq.First();
      ComputeAveragePlaneAndMaxDeviation(resWire, resPlane, IsSingular);
      return true;
    }
    else // we must choose the wire which average plane is closest to bisector plane
    {    //(check angle between axes)
      for (i = 1; i <= Wseq.Length(); i++)
      {
        TopoDS_Wire      aWire = TopoDS::Wire(Wseq(i));
        gp_Pln           aPln;
        bool issing;
        ComputeAveragePlaneAndMaxDeviation(aWire, aPln, issing);
        if (issing)
          continue;

        double Angle = aPln.Axis().Angle(myAxeOfBisPlane.Axis());
        if (Angle > M_PI / 2)
          Angle = M_PI - Angle;

        if (Angle < MinAngle)
        {
          MinAngle = Angle;
          resWire  = aWire;
          resPlane = aPln;
        }
      }
      return true;
    }
  }
  return false;
}

// ------------------------------------------------------------------------------------------
// static function: SplitUEdges
// purpose:
// ------------------------------------------------------------------------------------------
bool SplitUEdges(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theUEdges,
                             const BOPDS_PDS&                       theDS,
                             const gp_Vec&                          theCrossDirection,
                             NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&    theHistMap)
{

  const NCollection_Vector<BOPDS_InterfVV>& aVVs = theDS->InterfVV();

  BRep_Builder           aBB;
  int       ueit = 0, upRow, lowCol, upCol;
  NCollection_Array2<TopoDS_Shape> aNewVertices(1, 2, 1, 2);
  //
  upRow  = theUEdges->UpperRow();
  lowCol = theUEdges->LowerCol();
  upCol  = theUEdges->UpperCol();
  //
  for (ueit = theUEdges->LowerRow(); ueit <= upRow; ueit++)
  {
    const TopoDS_Shape& aE1 = theUEdges->Value(ueit, lowCol);
    const TopoDS_Shape& aE2 = theUEdges->Value(ueit, upCol);

    if (theHistMap.IsBound(aE1) || theHistMap.IsBound(aE2))
      continue;

    int anEIndex1 = theDS->Index(aE1);
    int anEIndex2 = theDS->Index(aE2);

    TopoDS_Vertex    aCommonVertex;
    double    apar1 = 0., apar2 = 0.;
    bool bvertexfound =
      FindCommonVertex(theDS, anEIndex1, anEIndex2, theCrossDirection, aCommonVertex, apar1, apar2);
    //
    if (!bvertexfound)
    {
      TopoDS_Vertex    V1      = TopExp::LastVertex(TopoDS::Edge(aE1));
      TopoDS_Vertex    V2      = TopExp::FirstVertex(TopoDS::Edge(aE2));
      int vindex1 = theDS->Index(V1);
      int vindex2 = theDS->Index(V2);
      int vvit    = 0;
      int aNbVVs  = aVVs.Length();

      for (vvit = 0; !bvertexfound && (vvit < aNbVVs); vvit++)
      {
        // const BOPTools_VVInterference& aVV = aVVs(vvit);
        const BOPDS_InterfVV& aVV = aVVs(vvit);

        if (((vindex1 == aVV.Index1()) && (vindex2 == aVV.Index2()))
            || ((vindex1 == aVV.Index2()) && (vindex2 == aVV.Index1())))
        {

          if (!aVV.HasIndexNew())
          {
            continue;
          }
          aCommonVertex = TopoDS::Vertex(theDS->Shape(aVV.IndexNew()));
          bvertexfound  = true;
          apar1         = BRep_Tool::Parameter(V1, TopoDS::Edge(aE1));
          apar2         = BRep_Tool::Parameter(V2, TopoDS::Edge(aE2));
        }
      }
    }

    if (bvertexfound)
    {
      TopoDS_Vertex aV1, aV2;
      double f = 0., l = 0.;
      //
      TopoDS_Edge aNewE1 = TopoDS::Edge(aE1.EmptyCopied());
      TopExp::Vertices(TopoDS::Edge(aE1), aV1, aV2);
      aNewE1.Orientation(TopAbs_FORWARD);
      aV1.Orientation(TopAbs_FORWARD);
      aBB.Add(aNewE1, aV1);
      aCommonVertex.Orientation(TopAbs_REVERSED);
      aBB.Add(aNewE1, aCommonVertex);
      BRep_Tool::Range(TopoDS::Edge(aE1), f, l);
      aBB.Range(aNewE1, f, apar1);

      //
      TopoDS_Edge aNewE2 = TopoDS::Edge(aE2.EmptyCopied());
      TopExp::Vertices(TopoDS::Edge(aE2), aV1, aV2);
      aNewE2.Orientation(TopAbs_FORWARD);
      aCommonVertex.Orientation(TopAbs_FORWARD);
      aBB.Add(aNewE2, aCommonVertex);
      aBB.Add(aNewE2, aV2);
      BRep_Tool::Range(TopoDS::Edge(aE2), f, l);
      aBB.Range(aNewE2, apar2, l);

      NCollection_List<TopoDS_Shape> lst;
      lst.Append(aNewE1);
      theHistMap.Bind(aE1, lst);
      lst.Clear();
      lst.Append(aNewE2);
      theHistMap.Bind(aE2, lst);
    }
  }
  return true;
}

// ------------------------------------------------------------------------------------------
// static function: StoreVedgeInHistMap
// purpose:
// ------------------------------------------------------------------------------------------
void StoreVedgeInHistMap(const occ::handle<NCollection_HArray1<TopoDS_Shape>>& theVEdges,
                         const int                 theIndex,
                         const TopoDS_Shape&                    theNewVshape,
                         NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&    theHistMap)
{
  // Replace default value in the map (v-iso edge of face)
  // by intersection of two consecutive faces
  const TopoDS_Shape& aVEdge = theVEdges->Value(theIndex);

  theHistMap.Bound(aVEdge, NCollection_List<TopoDS_Shape>())->Append(theNewVshape);
}

// ------------------------------------------------------------------------------------------
// static function: FindFreeVertices
// purpose:
// ------------------------------------------------------------------------------------------
void FindFreeVertices(const TopoDS_Shape&        theShape,
                      const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theVerticesToAvoid,
                      NCollection_List<TopoDS_Shape>&      theListOfVertex)
{

  theListOfVertex.Clear();
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_VERTEX, TopAbs_EDGE, aMap);
  int i = 0;

  for (i = 1; i <= aMap.Extent(); i++)
  {
    const TopoDS_Shape& aKey = aMap.FindKey(i);

    if (theVerticesToAvoid.Contains(aKey))
      continue;
    const NCollection_List<TopoDS_Shape>& aList = aMap.FindFromIndex(i);

    if (aList.Extent() < 2)
    {
      theListOfVertex.Append(aKey);
    }
  }
}

// ------------------------------------------------------------------------------------------
// static function: FindCommonVertex
// purpose:
// ------------------------------------------------------------------------------------------
bool FindCommonVertex(const BOPDS_PDS&       theDS,
                                  const int theEIndex1,
                                  const int theEIndex2,
                                  const gp_Vec&          theCrossDirection,
                                  TopoDS_Vertex&         theCommonVertex,
                                  double&         theParamOnE1,
                                  double&         theParamOnE2)
{

  const NCollection_Vector<BOPDS_InterfEE>& aEEs = theDS->InterfEE();

  bool bvertexfound = false;
  TopoDS_Vertex    aCommonVertex;
  int eeit = 0;

  TopoDS_Edge       theE1 = TopoDS::Edge(theDS->Shape(theEIndex1));
  TopoDS_Edge       theE2 = TopoDS::Edge(theDS->Shape(theEIndex2));
  BRepAdaptor_Curve aBC1(theE1), aBC2(theE2);

  int aNbEEs;
  aNbEEs = aEEs.Length();
  for (eeit = 0; eeit < aNbEEs; ++eeit)
  {
    const BOPDS_InterfEE& aEE = aEEs(eeit);

    if ((theEIndex1 == aEE.Index1() && theEIndex2 == aEE.Index2())
        || (theEIndex1 == aEE.Index2() && theEIndex2 == aEE.Index1()))
    {

      if (!aEE.HasIndexNew())
        continue;

      IntTools_CommonPrt aCP = aEE.CommonPart();
      if (aCP.Type() == TopAbs_VERTEX)
      {
        theCommonVertex = *(TopoDS_Vertex*)&theDS->Shape(aEE.IndexNew());

        if (theEIndex1 == aEE.Index1())
          IntTools_Tools::VertexParameters(aCP, theParamOnE1, theParamOnE2);
        else
          IntTools_Tools::VertexParameters(aCP, theParamOnE2, theParamOnE1);

        gp_Pnt aPt;
        gp_Vec aDirOnE1, aDirOnE2;
        gp_Dir aIntersectPointCrossDir;

        // intersect point aDirOnE1.cross(aDirOnE2) should same direction with path
        // theCrossDirection
        aBC1.D1(theParamOnE1, aPt, aDirOnE1);
        aBC2.D1(theParamOnE2, aPt, aDirOnE2);
        aIntersectPointCrossDir = aDirOnE1.Crossed(aDirOnE2);

        if (aIntersectPointCrossDir.Dot(theCrossDirection) > Precision::SquareConfusion())
        {
          bvertexfound = true;
          break;
        }
      }
    }
  }
  return bvertexfound;
}

// ----------------------------------------------------------------------------------------------------
// static function: GetUEdges
// purpose:
// ----------------------------------------------------------------------------------------------------
bool GetUEdges(const int                 theIndex,
                           const int                 theRank,
                           const occ::handle<NCollection_HArray2<TopoDS_Shape>>& theUEdges,
                           const TopoDS_Edge&                     theBoundEdge,
                           const TopoDS_Face&                     theFace,
                           TopoDS_Edge&                           theFirstUEdge,
                           TopoDS_Edge&                           theSecondUEdge)
{
  const TopoDS_Shape& aUE1 = theUEdges->Value(theIndex, theUEdges->LowerCol() + theRank);
  const TopoDS_Shape& aUE2 = theUEdges->Value(theIndex + 1, theUEdges->LowerCol() + theRank);

  TopoDS_Face aFace = theFace;
  aFace.Orientation(TopAbs_FORWARD);
  TopoDS_Edge     E1, E2;
  TopExp_Explorer anExp(aFace, TopAbs_EDGE);

  for (; anExp.More(); anExp.Next())
  {
    if (E1.IsNull() && aUE1.IsSame(anExp.Current()))
    {
      E1 = TopoDS::Edge(anExp.Current());
    }
    else if (E2.IsNull() && aUE2.IsSame(anExp.Current()))
    {
      E2 = TopoDS::Edge(anExp.Current());
    }
  }

  if (E1.IsNull() || E2.IsNull())
    return false;

  double        f, l;
  occ::handle<Geom2d_Curve> C1 = BRep_Tool::CurveOnSurface(E1, aFace, f, l);

  if (C1.IsNull())
    return false;
  gp_Pnt2d             PU1 = (theRank == 0) ? C1->Value(l) : C1->Value(f);
  occ::handle<Geom2d_Curve> C2  = BRep_Tool::CurveOnSurface(theBoundEdge, aFace, f, l);

  if (C2.IsNull())
    return false;
  BRep_Tool::Range(theBoundEdge, f, l);
  gp_Pnt2d            pf = C2->Value(f);
  TopoDS_Vertex       aV = (theRank == 0) ? TopExp::LastVertex(E1) : TopExp::FirstVertex(E1);
  double       aTolerance = BRep_Tool::Tolerance(aV);
  BRepAdaptor_Surface aBAS(aFace, false);

  if (pf.Distance(PU1) > aBAS.UResolution(aTolerance))
  {
    TopoDS_Edge atmpE = E1;
    E1                = E2;
    E2                = atmpE;
  }
  theFirstUEdge  = E1;
  theSecondUEdge = E2;
  return true;
}

// ----------------------------------------------------------------------------------------------------
// static function: FillGap
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FillGap(const TopoDS_Vertex&   theFirstVertex,
                         const TopoDS_Vertex&   theLastVertex,
                         const gp_Pnt2d&        theFirstPoint,
                         const gp_Pnt2d&        theLastPoint,
                         const TopoDS_Face&     theFace,
                         const TopoDS_Compound& theSectionEdges,
                         NCollection_List<TopoDS_Shape>&  theOrderedList)
{

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> aMap;
  TopExp::MapShapesAndAncestors(theSectionEdges, TopAbs_VERTEX, TopAbs_EDGE, aMap);

  if (aMap.IsEmpty())
  {
    return false;
  }

  if (!aMap.Contains(theFirstVertex) || !aMap.Contains(theLastVertex))
  {
    return false;
  }
  NCollection_List<TopoDS_Shape> aListOfEdge;
  //  int i = 0;
  //  TopoDS_Vertex aCurVertex = theFirstVertex;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapToAvoid;

  if (FindNextEdge(theFirstVertex, theLastVertex, aMap, aMapToAvoid, aListOfEdge))
  {
    if (!aListOfEdge.IsEmpty())
    {
      return CheckAndOrientEdges(aListOfEdge, theFirstPoint, theLastPoint, theFace, theOrderedList);
    }
  }
  return false;
}

// ----------------------------------------------------------------------------------------------------
// static function: FindNextEdge
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FindNextEdge(const TopoDS_Vertex&                             theFirstVertex,
                              const TopoDS_Vertex&                             theLastVertex,
                              const NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theMapVE,
                              const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&                       theMapToAvoid,
                              NCollection_List<TopoDS_Shape>&                            theOrderedList)
{
  TopoDS_Vertex       aCurVertex = theFirstVertex;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapToAvoid;
  aMapToAvoid = theMapToAvoid;
  NCollection_List<TopoDS_Shape> aListOfEdge;
  int     i = 0;

  for (i = 1; i <= theMapVE.Extent(); i++)
  {
    if (!theMapVE.Contains(aCurVertex))
      break;
    const NCollection_List<TopoDS_Shape>& lste    = theMapVE.FindFromKey(aCurVertex);
    bool            befound = false;

    NCollection_List<TopoDS_Shape>::Iterator anIt(lste);

    for (; anIt.More(); anIt.Next())
    {
      TopoDS_Shape  anEdge         = anIt.Value();
      TopoDS_Vertex aSaveCurVertex = aCurVertex;

      if (!aMapToAvoid.Contains(anEdge))
      {
        TopoDS_Vertex V1, V2;
        TopExp::Vertices(TopoDS::Edge(anEdge), V1, V2);

        if (!aCurVertex.IsSame(V1))
        {
          aCurVertex = V1;
        }
        else if (!aCurVertex.IsSame(V2))
        {
          aCurVertex = V2;
        }
        aMapToAvoid.Add(anEdge);
        befound = true;
        aListOfEdge.Append(anEdge);

        if (!aCurVertex.IsSame(theLastVertex))
        {
          NCollection_List<TopoDS_Shape> aListtmp;

          if (!FindNextEdge(aCurVertex, theLastVertex, theMapVE, aMapToAvoid, aListtmp))
          {
            aListOfEdge.Clear();
            aCurVertex = aSaveCurVertex;
            continue;
          }
          else
          {
            aListOfEdge.Append(aListtmp);
            theOrderedList.Append(aListOfEdge);
            return true;
          }
        }
        break;
      }
    }

    if (aCurVertex.IsSame(theLastVertex))
      break;

    if (!befound)
    {
      return false;
    }
  }

  if (aCurVertex.IsSame(theLastVertex))
  {
    theOrderedList.Append(aListOfEdge);
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------------------------------
// static function: CheckAndOrientEdges
// purpose:
// ----------------------------------------------------------------------------------------------------
bool CheckAndOrientEdges(const NCollection_List<TopoDS_Shape>& theOrderedList,
                                     const gp_Pnt2d&             theFirstPoint,
                                     const gp_Pnt2d&             theLastPoint,
                                     const TopoDS_Face&          theFace,
                                     NCollection_List<TopoDS_Shape>&       theOrientedList)
{
  NCollection_List<TopoDS_Shape>::Iterator anIt(theOrderedList);

  if (!anIt.More())
    return true;

  double f, l;
  TopoDS_Edge   aEPrev = TopoDS::Edge(anIt.Value());
  anIt.Next();

  occ::handle<Geom2d_Curve> aCurve = BRep_Tool::CurveOnSurface(aEPrev, theFace, f, l);
  TopoDS_Vertex        Vf, Vl;
  TopExp::Vertices(aEPrev, Vf, Vl);
  BRepAdaptor_Surface aBAS(theFace, false);

  double aTolerance1 = (Vf.IsNull()) ? Precision::Confusion() : BRep_Tool::Tolerance(Vf);
  double aTolerance2 = (Vl.IsNull()) ? Precision::Confusion() : BRep_Tool::Tolerance(Vl);
  double utol        = aBAS.UResolution(aTolerance1);
  double vtol        = aBAS.VResolution(aTolerance1);
  aTolerance1               = (utol > vtol) ? utol : vtol;
  utol                      = aBAS.UResolution(aTolerance2);
  vtol                      = aBAS.VResolution(aTolerance2);
  aTolerance2               = (utol > vtol) ? utol : vtol;

  gp_Pnt2d         ap          = aCurve->Value(f);
  bool bFirstFound = false;
  bool bLastFound  = false;

  if (ap.Distance(theFirstPoint) < aTolerance1)
  {
    if (theOrientedList.IsEmpty())
      theOrientedList.Append(aEPrev.Oriented(TopAbs_FORWARD));
    bFirstFound = true;
  }
  else if (ap.Distance(theLastPoint) < aTolerance1)
  {
    if (theOrientedList.IsEmpty())
      theOrientedList.Append(aEPrev.Oriented(TopAbs_REVERSED));
    bLastFound = true;
  }
  ap = aCurve->Value(l);

  if (ap.Distance(theLastPoint) < aTolerance2)
  {
    if (theOrientedList.IsEmpty())
      theOrientedList.Append(aEPrev.Oriented(TopAbs_FORWARD));
    bLastFound = true;
  }
  else if (ap.Distance(theFirstPoint) < aTolerance2)
  {
    if (theOrientedList.IsEmpty())
      theOrientedList.Append(aEPrev.Oriented(TopAbs_REVERSED));
    bFirstFound = true;
  }

  if (!theOrientedList.IsEmpty())
    aEPrev = TopoDS::Edge(theOrientedList.Last());

  for (; anIt.More(); anIt.Next())
  {
    const TopoDS_Edge& aE = TopoDS::Edge(anIt.Value());
    TopoDS_Vertex      aV11, aV12;
    TopExp::Vertices(aEPrev, aV11, aV12, true);
    TopoDS_Vertex aV21, aV22;
    TopExp::Vertices(aE, aV21, aV22, false);

    TopAbs_Orientation anOri =
      (aV12.IsSame(aV21) || aV11.IsSame(aV22)) ? TopAbs_FORWARD : TopAbs_REVERSED;
    theOrientedList.Append(aE.Oriented(anOri));
    aEPrev = TopoDS::Edge(theOrientedList.Last());

    aTolerance1 = (aV21.IsNull()) ? Precision::Confusion() : BRep_Tool::Tolerance(aV21);
    aTolerance2 = (aV22.IsNull()) ? Precision::Confusion() : BRep_Tool::Tolerance(aV22);
    utol        = aBAS.UResolution(aTolerance1);
    vtol        = aBAS.VResolution(aTolerance1);
    aTolerance1 = (utol > vtol) ? utol : vtol;
    utol        = aBAS.UResolution(aTolerance2);
    vtol        = aBAS.VResolution(aTolerance2);
    aTolerance2 = (utol > vtol) ? utol : vtol;
    aCurve      = BRep_Tool::CurveOnSurface(aE, theFace, f, l);
    ap          = aCurve->Value(f);

    if (ap.Distance(theFirstPoint) < aTolerance1)
    {
      bFirstFound = true;
    }
    else if (ap.Distance(theLastPoint) < aTolerance1)
    {
      bLastFound = true;
    }
    ap = aCurve->Value(l);

    if (ap.Distance(theFirstPoint) < aTolerance2)
    {
      bFirstFound = true;
    }
    else if (ap.Distance(theLastPoint) < aTolerance2)
    {
      bLastFound = true;
    }
  }

  return bFirstFound && bLastFound;
}

// ----------------------------------------------------------------------------------------------------
// static function: FindVertex
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FindVertex(const TopoDS_Edge&                        theEdge,
                            const int                    theRank,
                            const BOPDS_PDS&                          theDS,
                            const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theHistMap,
                            TopoDS_Vertex&                            theVertex,
                            BOPDS_Pave&                               thePave)
{

  if (!theHistMap.IsBound(theEdge))
    return false;

  const NCollection_List<TopoDS_Shape>& lst = theHistMap.Find(theEdge);

  if (lst.IsEmpty())
    return false;

  TopoDS_Edge   aNewEdge = TopoDS::Edge(lst.First());
  double f, l;
  BRep_Tool::Range(aNewEdge, f, l);

  if (theRank == 0)
  {
    thePave.SetParameter(l);
    theVertex = TopExp::LastVertex(aNewEdge);
  }
  else
  {
    thePave.SetParameter(f);
    theVertex = TopExp::FirstVertex(aNewEdge);
  }
  int anIndex = theDS->Index(theVertex);
  if (anIndex == -1)
  {
    int i, i1, i2;
    i1 = theDS->NbSourceShapes();
    i2 = theDS->NbShapes();
    for (i = i1; i < i2; ++i)
    {
      const TopoDS_Shape& aSx = theDS->Shape(i);
      if (aSx.IsSame(theVertex))
      {
        anIndex = i;
        break;
      }
    }
  }

  thePave.SetIndex(anIndex);

  return true;
}

// ----------------------------------------------------------------------------------------------------
// static function: FindNextVertex
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FindNextVertex(const int theEdgeIndex,
                                const BOPDS_Pave&      thePrevPave,
                                const BOPDS_PDS&       theDS,
                                TopoDS_Vertex&         theNextVertex,
                                BOPDS_Pave&            thePave)
{

  bool               bTakePave, bFound;
  BOPDS_Pave                     aTmpPave;
  NCollection_List<BOPDS_Pave>::Iterator aItP;
  //
  BOPDS_Pave anullpave;
  bFound    = false;
  bTakePave = thePrevPave.IsEqual(anullpave);

  NCollection_List<BOPDS_Pave> aLP;
  theDS->Paves(theEdgeIndex, aLP);
  aItP.Initialize(aLP);
  for (; aItP.More(); aItP.Next())
  {
    aTmpPave = aItP.Value();
    //
    if (bTakePave)
    {
      if (theDS->IsNewShape(aTmpPave.Index()))
      {
        theNextVertex = *(TopoDS_Vertex*)&theDS->Shape(aTmpPave.Index());
        thePave       = aTmpPave;
        bFound        = true;
        break;
      }
    }
    //
    else if (aTmpPave.IsEqual(thePrevPave))
    {
      bTakePave = true;
    }
  }

  return bFound;
}

// ----------------------------------------------------------------------------------------------------
// static function: GetPave
// purpose:
// ----------------------------------------------------------------------------------------------------
bool GetPave(const int theEdgeIndex,
                         const bool isFirst,
                         const BOPDS_PDS&       theDS,
                         BOPDS_Pave&            thePave)
{

  occ::handle<BOPDS_PaveBlock> aPB;
  NCollection_List<BOPDS_Pave>        aLP;

  theDS->Paves(theEdgeIndex, aLP);
  if (!aLP.Extent())
  {
    return false;
  }
  //
  if (isFirst)
  {
    thePave = aLP.First();
  }
  else
  {
    thePave = aLP.Last();
  }

  return true;
}

// ----------------------------------------------------------------------------------------------------
// static function: FindFromUEdge
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FindFromUEdge(const TopoDS_Edge&                        theUE1Old,
                               const TopoDS_Edge&                        theUE2Old,
                               const TopoDS_Edge&                        theUE1New,
                               const TopoDS_Edge&                        theUE2New,
                               const TopoDS_Face&                        theFace,
                               const TopoDS_Compound&                    theSecEdges,
                               const int                    theRank,
                               const TopoDS_Edge&                        theBoundEdge,
                               const int                    theBoundEdgeIndex,
                               const BOPDS_PDS&                          theDS,
                               const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theHistMap,
                               TopoDS_Compound&                          theSecEdgesNew,
                               NCollection_List<TopoDS_Shape>&                     theListOfWireEdges,
                               BOPDS_Pave&                               theFoundPave,
                               bool&                         isOnUEdge)
{
  theFoundPave.SetIndex(0);
  theFoundPave.SetParameter(0.);
  isOnUEdge = true;

  TopoDS_Face aFaceF = theFace;
  aFaceF.Orientation(TopAbs_FORWARD);
  TopoDS_Vertex        aPrevVertex, aNextVertex;
  TopoDS_Compound      aCompOfSecEdges = theSecEdges;
  NCollection_List<TopoDS_Shape> aListOfWireEdges;
  //  BRep_Builder aBB;

  BOPDS_Pave    aPave1, aPave2;
  double f = 0., l = 0.;
  gp_Pnt2d      p1, p2;
  TopoDS_Vertex aFirstV, aLastV;
  BOPDS_Pave    atmpPave;

  if (!FindVertex(theUE1Old, theRank, theDS, theHistMap, aPrevVertex, atmpPave))
  {
    return true;
  }

  if (aPrevVertex.IsNull())
  {
    return false;
  }

  aFirstV                        = aPrevVertex;
  bool     bSecFound = false;
  occ::handle<Geom2d_Curve> aC1       = BRep_Tool::CurveOnSurface(theUE1New, aFaceF, f, l);
  p1                             = (theRank == 0) ? aC1->Value(l) : aC1->Value(f);
  BOPDS_Pave       afoundpave;
  NCollection_List<BOPDS_Pave> aLP;
  theDS->Paves(theBoundEdgeIndex, aLP);
  int nbpave = aLP.Extent();
  int pit    = 0;

  while (FindNextVertex(theBoundEdgeIndex, aPave1, theDS, aNextVertex, aPave2) && (pit < nbpave))
  {
    aLastV                   = aNextVertex;
    occ::handle<Geom2d_Curve> aC2 = BRep_Tool::CurveOnSurface(theBoundEdge, aFaceF, f, l);
    p2                       = aC2->Value(aPave2.Parameter());
    NCollection_List<TopoDS_Shape> aOrderedList;

    if (FillGap(aFirstV, aLastV, p1, p2, aFaceF, aCompOfSecEdges, aOrderedList))
    {
      // remove found edges...
      TopoDS_Compound aComp;
      RemoveEdges(aCompOfSecEdges, aOrderedList, aComp);
      aCompOfSecEdges = aComp;
      aListOfWireEdges.Append(aOrderedList);
      afoundpave = aPave2;
      isOnUEdge  = false;
      bSecFound  = true;
      break;
    }
    aPrevVertex = aNextVertex;
    aPave1      = aPave2;
    pit++;
  }

  if (!bSecFound && FindVertex(theUE2Old, theRank, theDS, theHistMap, aNextVertex, aPave2))
  {
    aLastV                   = aNextVertex;
    occ::handle<Geom2d_Curve> aC2 = BRep_Tool::CurveOnSurface(theUE2New, aFaceF, f, l);
    p2                       = aC2->Value(aPave2.Parameter());
    NCollection_List<TopoDS_Shape> aOrderedList;

    if (FillGap(aFirstV, aLastV, p1, p2, aFaceF, aCompOfSecEdges, aOrderedList))
    {
      // remove found edges...
      TopoDS_Compound aComp;

      RemoveEdges(aCompOfSecEdges, aOrderedList, aComp);
      aCompOfSecEdges = aComp;
      aListOfWireEdges.Append(aOrderedList);
      afoundpave = aPave2;
      bSecFound  = true;
      isOnUEdge  = true;
    }
  }

  if (bSecFound)
  {
    theFoundPave       = afoundpave;
    theListOfWireEdges = aListOfWireEdges;
    theSecEdgesNew     = aCompOfSecEdges;
  }
  return true;
}

// ----------------------------------------------------------------------------------------------------
// static function: FindFromVEdge
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FindFromVEdge(const BOPDS_Pave&                         thePrevPave,
                               const bool&                   isOnUEdge,
                               const TopoDS_Edge&                        theUE1Old,
                               const TopoDS_Edge&                        theUE2Old,
                               const TopoDS_Face&                        theFace,
                               const TopoDS_Compound&                    theSecEdges,
                               const int                    theRank,
                               const TopoDS_Edge&                        theBoundEdge,
                               const int                    theBoundEdgeIndex,
                               const BOPDS_PDS&                          theDS,
                               const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& theHistMap,
                               NCollection_List<TopoDS_Shape>&                     theListOfWireEdges,
                               bool&                         isSectionFound)
{

  theListOfWireEdges.Clear();
  isSectionFound = false;
  //
  TopoDS_Face aFaceF = theFace;
  aFaceF.Orientation(TopAbs_FORWARD);
  TopoDS_Vertex        aPrevVertex, aNextVertex;
  TopoDS_Compound      aCompOfSecEdges = theSecEdges;
  NCollection_List<TopoDS_Shape> aListOfWireEdges;
  //  BRep_Builder aBB;

  BOPDS_Pave aPave1, aPave2;

  if (isOnUEdge)
  {
    TopoDS_Vertex atmpVertex;
    BOPDS_Pave    aPaveOfE2;

    if (FindVertex(theUE2Old, theRank, theDS, theHistMap, atmpVertex, aPaveOfE2))
    {
      if (thePrevPave.IsEqual(aPaveOfE2))
        return true;
    }
  }

  double        f = 0., l = 0.;
  gp_Pnt2d             p1(0., 0.), p2(0., 0.);
  TopoDS_Vertex        aFirstV, aLastV;
  occ::handle<Geom2d_Curve> aC1       = BRep_Tool::CurveOnSurface(theUE1Old, aFaceF, f, l);
  occ::handle<Geom2d_Curve> aC2       = BRep_Tool::CurveOnSurface(theBoundEdge, aFaceF, f, l);
  bool     bSecFound = false;

  aPave1 = thePrevPave;

  if (isOnUEdge)
  {
    BOPDS_Pave atmpPave;

    if (!GetPave(theBoundEdgeIndex, true, theDS, atmpPave))
    {
      return false;
    }
    aPave1 = atmpPave;
  }
  p1          = aC2->Value(aPave1.Parameter());
  aPrevVertex = TopoDS::Vertex(theDS->Shape(aPave1.Index()));

  NCollection_List<BOPDS_Pave> aLP;
  theDS->Paves(theBoundEdgeIndex, aLP);
  int             nbpave = aLP.Extent();
  int             pit    = 0;
  NCollection_Array1<NCollection_List<TopoDS_Shape>> anArrayOfListOfSec(1, nbpave);

  // by pairs non continuously. begin
  int      k            = 0;
  BOPDS_Pave            aFirstPave   = aPave1;
  TopoDS_Vertex         aFirstVertex = aPrevVertex;
  gp_Pnt2d              apfirst      = p1;
  NCollection_List<BOPDS_Pave>      aFirstPaves, aLastPaves;
  NCollection_List<int> aListOfFlags;
  int      apaircounter = 1;

  for (k = 0; k < nbpave; k++)
  {
    aPave1                  = aFirstPave;
    p1                      = apfirst;
    aPrevVertex             = aFirstVertex;
    bool bfound = false;
    pit                     = 0;

    while (FindNextVertex(theBoundEdgeIndex, aPave1, theDS, aNextVertex, aPave2) && (pit < nbpave))
    {
      aFirstV = aPrevVertex;
      aLastV  = aNextVertex;
      p2      = aC2->Value(aPave2.Parameter());

      NCollection_List<TopoDS_Shape> aOrderedList;

      if (FillGap(aFirstV, aLastV, p1, p2, aFaceF, aCompOfSecEdges, aOrderedList))
      {
        TopoDS_Compound aComp;
        RemoveEdges(aCompOfSecEdges, aOrderedList, aComp);
        aCompOfSecEdges = aComp;

        anArrayOfListOfSec(apaircounter++).Append(aOrderedList);
        aFirstPaves.Append(aFirstPave);
        aLastPaves.Append(aPave2);
        aListOfFlags.Append(1);
        aFirstPave   = aPave2;
        aFirstVertex = aNextVertex;
        apfirst      = p2;
        aPrevVertex  = aNextVertex;
        bSecFound    = true;
        bfound       = true;
      }
      aPave1 = aPave2;
      pit++;
    }

    if (FindVertex(theUE2Old, theRank, theDS, theHistMap, aNextVertex, aPave2))
    {
      aFirstV                  = aPrevVertex;
      aLastV                   = aNextVertex;
      occ::handle<Geom2d_Curve> aC3 = BRep_Tool::CurveOnSurface(theUE2Old, aFaceF, f, l);
      p2                       = aC3->Value(aPave2.Parameter());

      NCollection_List<TopoDS_Shape> aOrderedList;

      if (FillGap(aFirstV, aLastV, p1, p2, aFaceF, aCompOfSecEdges, aOrderedList))
      {
        TopoDS_Compound aComp;
        RemoveEdges(aCompOfSecEdges, aOrderedList, aComp);
        aCompOfSecEdges = aComp;
        anArrayOfListOfSec(apaircounter++).Append(aOrderedList);
        aFirstPaves.Append(aFirstPave);
        aLastPaves.Append(aPave2);
        aListOfFlags.Append(0);
        bSecFound = true;
        break;
      }
    }

    if (!bfound)
    {
      if (!FindNextVertex(theBoundEdgeIndex, aFirstPave, theDS, aNextVertex, aPave2))
      {
        break;
      }
      aFirstPave   = aPave2;
      apfirst      = aC2->Value(aPave2.Parameter());
      aFirstVertex = aNextVertex;
    }
  }
  // by pairs non continuously. end

  // by pairs continuously. begin
  aPave1 = thePrevPave;

  if (isOnUEdge)
  {
    BOPDS_Pave atmpPave;

    if (!GetPave(theBoundEdgeIndex, true, theDS, atmpPave))
    {
      return false;
    }
    aPave1 = atmpPave;
  }
  p1          = aC2->Value(aPave1.Parameter());
  aPrevVertex = TopoDS::Vertex(theDS->Shape(aPave1.Index()));

  pit = 0;

  while (FindNextVertex(theBoundEdgeIndex, aPave1, theDS, aNextVertex, aPave2) && (pit < nbpave))
  {
    aFirstV = aPrevVertex;
    aLastV  = aNextVertex;
    p2      = aC2->Value(aPave2.Parameter());

    bool                    bisinside  = false;
    int                    apbindex   = 0;
    int                    apbcounter = 1;
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aPBIt;
    NCollection_List<BOPDS_Pave>::Iterator      aPIt1, aPIt2;
    NCollection_List<int>::Iterator aFlagIt;

    for (aPIt1.Initialize(aFirstPaves),
         aPIt2.Initialize(aLastPaves),
         aFlagIt.Initialize(aListOfFlags);
         aPIt1.More() && aPIt2.More() && aFlagIt.More();
         aPIt1.Next(), aPIt2.Next(), aFlagIt.Next(), apbcounter++)
    {

      bool bfin = false;
      bool blin = false;

      if (aPave1.IsEqual(aPIt1.Value()))
      {
        bfin = true;
      }
      else
      {
        bfin = (aPave1.Parameter() > aPIt1.Value().Parameter());
      }

      if (aFlagIt.Value())
      {
        if (aPave2.IsEqual(aPIt2.Value()))
        {
          blin = true;
        }
        else
        {
          blin = (aPave2.Parameter() < aPIt2.Value().Parameter());
        }
      }
      else
      {
        if ((aPave2.Index() == aPIt2.Value().Index()) && (aPave2.Index() > 0))
        {
          occ::handle<Geom2d_Curve> pc = BRep_Tool::CurveOnSurface(theUE2Old, aFaceF, f, l);
          gp_Pnt2d             p3 = pc->Value(aPIt2.Value().Parameter());
          TopoDS_Vertex        aV = TopoDS::Vertex(theDS->Shape(aPave2.Index()));
          BRepAdaptor_Surface  aBAS(aFaceF, false);
          double        aTolerance = BRep_Tool::Tolerance(aV);
          double        utol       = aBAS.UResolution(aTolerance);
          double        vtol       = aBAS.VResolution(aTolerance);
          aTolerance                      = (utol > vtol) ? utol : vtol;

          if (p2.Distance(p3) < aTolerance)
            blin = true;
        }
      }

      if (bfin && blin)
      {
        apbindex  = apbcounter;
        bisinside = true;
        break;
      }
    }

    if (!bisinside)
    {

      NCollection_List<TopoDS_Shape> aOrderedList;

      if (FillGap(aFirstV, aLastV, p1, p2, aFaceF, aCompOfSecEdges, aOrderedList))
      {
        TopoDS_Compound aComp;
        RemoveEdges(aCompOfSecEdges, aOrderedList, aComp);
        aCompOfSecEdges = aComp;
        aListOfWireEdges.Append(aOrderedList);

        bSecFound = true;
      }
      else
      {
        TopoDS_Edge aESplit;
        // get split
        aPBIt.Initialize(theDS->PaveBlocks(theBoundEdgeIndex));

        for (; aPBIt.More(); aPBIt.Next())
        {
          const occ::handle<BOPDS_PaveBlock>& aPB1 = aPBIt.Value();
          if (aPB1->OriginalEdge() == theBoundEdgeIndex && aPB1->Pave1().IsEqual(aPave1)
              && aPB1->Pave2().IsEqual(aPave2))
          {
            if (aPB1->Edge() > 0)
            {
              aESplit = *(TopoDS_Edge*)&theDS->Shape(aPB1->Edge());
              break;
            }
          }
        }

        if (!aESplit.IsNull())
        {
          aListOfWireEdges.Append(aESplit);
        }
      }
    }
    else
    {
      if (apbindex > 0)
      {
        NCollection_List<TopoDS_Shape>& aListOfSec = anArrayOfListOfSec(apbindex);
        aListOfWireEdges.Append(aListOfSec);
      }
    }
    aPave1      = aPave2;
    aPrevVertex = aNextVertex;
    p1          = p2;
    pit++;
  }

  if (FindVertex(theUE2Old, theRank, theDS, theHistMap, aNextVertex, aPave2))
  {
    aFirstV                  = aPrevVertex;
    aLastV                   = aNextVertex;
    occ::handle<Geom2d_Curve> aC3 = BRep_Tool::CurveOnSurface(theUE2Old, aFaceF, f, l);
    p2                       = aC3->Value(aPave2.Parameter());

    bool                    bisinside  = false;
    int                    apbindex   = 0;
    int                    apbcounter = 1;
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aPBIt;
    NCollection_List<BOPDS_Pave>::Iterator      aPIt1, aPIt2;
    NCollection_List<int>::Iterator aFlagIt;

    for (aPIt1.Initialize(aFirstPaves),
         aPIt2.Initialize(aLastPaves),
         aFlagIt.Initialize(aListOfFlags);
         aPIt1.More() && aPIt2.More() && aFlagIt.More();
         aPIt1.Next(), aPIt2.Next(), aFlagIt.Next(), apbcounter++)
    {

      bool bfin = false;
      bool blin = false;

      if (aPave1.IsEqual(aPIt1.Value()))
      {
        bfin = true;
      }
      else
      {
        bfin = (aPave1.Parameter() > aPIt1.Value().Parameter());
      }

      if (aFlagIt.Value())
      {
        if (aPave2.IsEqual(aPIt2.Value()))
        {
          blin = true;
        }
        else
        {
          blin = (aPave2.Parameter() < aPIt2.Value().Parameter());
        }
      }
      else
      {
        blin = true;
      }

      if (bfin && blin)
      {
        apbindex  = apbcounter;
        bisinside = true;
        break;
      }
    }

    if (!bisinside)
    {

      NCollection_List<TopoDS_Shape> aOrderedList;

      if (FillGap(aFirstV, aLastV, p1, p2, aFaceF, aCompOfSecEdges, aOrderedList))
      {
        TopoDS_Compound aComp;
        RemoveEdges(aCompOfSecEdges, aOrderedList, aComp);
        aCompOfSecEdges = aComp;
        aListOfWireEdges.Append(aOrderedList);

        bSecFound = true;
      }
      else
      {
        // add split
        TopoDS_Edge aESplit;
        // get split
        if (!GetPave(theBoundEdgeIndex, false, theDS, aPave2))
          return false;
        //
        aPBIt.Initialize(theDS->PaveBlocks(theBoundEdgeIndex));
        for (; aPBIt.More(); aPBIt.Next())
        {
          const occ::handle<BOPDS_PaveBlock>& aPB1 = aPBIt.Value();
          if (aPB1->OriginalEdge() == theBoundEdgeIndex && aPB1->Pave1().IsEqual(aPave1)
              && aPB1->Pave2().IsEqual(aPave2))
          {
            if (aPB1->Edge() > 0)
            {
              aESplit = *(TopoDS_Edge*)&theDS->Shape(aPB1->Edge());
              break;
            }
          }
        }

        if (!aESplit.IsNull())
        {
          aListOfWireEdges.Append(aESplit);
        }
      }
    }
    else
    {
      if (apbindex > 0)
      {
        NCollection_List<TopoDS_Shape>& aListOfSec = anArrayOfListOfSec(apbindex);
        aListOfWireEdges.Append(aListOfSec);
      }
    }
  }
  else
  {
    // add split
    TopoDS_Edge aESplit;
    // get split
    if (!GetPave(theBoundEdgeIndex, false, theDS, aPave2))
      return false;

    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aPBIt;
    aPBIt.Initialize(theDS->PaveBlocks(theBoundEdgeIndex));
    for (; aPBIt.More(); aPBIt.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB1 = aPBIt.Value();
      if (aPB1->OriginalEdge() == theBoundEdgeIndex && aPB1->Pave1().IsEqual(aPave1)
          && aPB1->Pave2().IsEqual(aPave2))
      {
        if (aPB1->Edge() > 0)
        {
          aESplit = *(TopoDS_Edge*)&theDS->Shape(aPB1->Edge());
          break;
        }
      }
    }

    if (!aESplit.IsNull())
    {
      aListOfWireEdges.Append(aESplit);
    }
  }

  // by pairs continuously. end
  theListOfWireEdges = aListOfWireEdges;
  isSectionFound     = bSecFound;
  return true;
}

// ----------------------------------------------------------------------------------------------------
// static function: RemoveEdges
// purpose:
// ----------------------------------------------------------------------------------------------------
void RemoveEdges(const TopoDS_Compound&      theSourceComp,
                 const NCollection_List<TopoDS_Shape>& theListToRemove,
                 TopoDS_Compound&            theResultComp)
{
  BRep_Builder    aBB;
  TopoDS_Compound aComp;
  aBB.MakeCompound(aComp);
  TopExp_Explorer anExp(theSourceComp, TopAbs_EDGE);

  for (; anExp.More(); anExp.Next())
  {
    bool                   bfound = false;
    NCollection_List<TopoDS_Shape>::Iterator anIt(theListToRemove);

    for (; !bfound && anIt.More(); anIt.Next())
    {
      bfound = anExp.Current().IsSame(anIt.Value());
    }

    if (!bfound)
    {
      aBB.Add(aComp, anExp.Current());
    }
  }
  theResultComp = aComp;
}

// ----------------------------------------------------------------------------------------------------
// static function: FilterSectionEdges
// purpose:
// ----------------------------------------------------------------------------------------------------
bool FilterSectionEdges(const NCollection_Vector<BOPDS_Curve>& theBCurves,
                                    const TopoDS_Face&         theSecPlane,
                                    const BOPDS_PDS&           theDS,
                                    TopoDS_Compound&           theResult)
{

  theResult.Nullify();

  BRep_Builder aBB;
  aBB.MakeCompound(theResult);
  int                    aNbCurves = theBCurves.Length();
  int                    cit       = 0;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aPBIt;

  for (cit = 0; cit < aNbCurves; ++cit)
  {
    const BOPDS_Curve&           aBCurve    = theBCurves(cit);
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aSectEdges = aBCurve.PaveBlocks();

    aPBIt.Initialize(aSectEdges);
    for (; aPBIt.More(); aPBIt.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB      = aPBIt.Value();
      int               nSect    = aPB->Edge();
      const TopoDS_Shape&            aS       = theDS->Shape(nSect);
      TopoDS_Edge                    anEdge   = TopoDS::Edge(aS);
      bool               bAddEdge = true;

      if (!theSecPlane.IsNull())
      {
        IntTools_BeanFaceIntersector anIntersector(anEdge, theSecPlane);
        double                f = 0., l = 0.;
        BRep_Tool::Range(anEdge, f, l);
        anIntersector.SetBeanParameters(f, l);
        //
        occ::handle<IntTools_Context> aContext = new IntTools_Context;
        anIntersector.SetContext(aContext);
        //
        anIntersector.Perform();

        if (anIntersector.IsDone())
        {
          bAddEdge           = false;
          int r = 0;

          for (r = 1; r <= anIntersector.Result().Length(); r++)
          {
            const IntTools_Range& aRange = anIntersector.Result().Value(r);

            if (((aRange.First() - f) < Precision::PConfusion())
                && ((l - aRange.Last()) < Precision::PConfusion()))
            {
              bAddEdge = true;
              break;
            } // if(((aRange.First() - f) < Precision::PConfusion()) &&
          } // for(r = 1; r <= anIntersector.Result().Length(); r++) {
        } // if(anIntersector.IsDone()) {
      } // if(!theSecPlane.IsNull()) {

      if (bAddEdge)
      {
        aBB.Add(theResult, aS);
      }
    } // for (; aPBIt.More(); aPBIt.Next()) {
  } // for(cit = 0; cit < aNbCurves; ++cit) {

  return true;
}

//=================================================================================================

static double ComputeAveragePlaneAndMaxDeviation(const TopoDS_Shape& aWire,
                                                        gp_Pln&             thePlane,
                                                        bool&   IsSingular)
{
  int N      = 40;
  int nedges = aWire.NbChildren();

  NCollection_Array1<gp_Pnt> Pnts(1, nedges * N);
  int   ind = 1, i;
  for (TopoDS_Iterator iter(aWire); iter.More(); iter.Next())
  {
    const TopoDS_Edge&     anEdge = TopoDS::Edge(iter.Value());
    BRepAdaptor_Curve      aCurve(anEdge);
    GCPnts_UniformAbscissa Distribution(aCurve, N + 1);
    for (i = 1; i <= N; i++)
    {
      double par = Distribution.Parameter(i);
      Pnts(ind++)       = aCurve.Value(par);
    }
  }

  gp_Ax2 Axe;
  GeomLib::AxeOfInertia(Pnts, Axe, IsSingular);
  if (IsSingular)
    return -1;

  thePlane                   = gp_Pln(Axe);
  double MaxDeviation = 0;
  for (i = 1; i <= Pnts.Length(); i++)
  {
    double dist = thePlane.Distance(Pnts(i));
    if (dist > MaxDeviation)
      MaxDeviation = dist;
  }
  return MaxDeviation;
}

static void UpdateSectionEdge(TopoDS_Edge&         theEdge,
                              const TopoDS_Vertex& theConstVertex,
                              TopoDS_Vertex&       theVertex,
                              const double  theParam)
{
  TopoDS_Edge F_Edge = theEdge;
  F_Edge.Orientation(TopAbs_FORWARD);

  TopAbs_Orientation OrOfVertex;
  TopoDS_Vertex      V1, V2, AnotherVertex;
  TopExp::Vertices(F_Edge, V1, V2);
  if (theConstVertex.IsSame(V1))
  {
    // OrOfConst = TopAbs_FORWARD;
    OrOfVertex    = TopAbs_REVERSED;
    AnotherVertex = V2;
  }
  else
  {
    // OrOfConst = TopAbs_REVERSED;
    OrOfVertex    = TopAbs_FORWARD;
    AnotherVertex = V1;
  }

  BRep_Builder  BB;
  double fpar, lpar;
  BRep_Tool::Range(F_Edge, fpar, lpar);
  if (OrOfVertex == TopAbs_FORWARD)
    fpar = theParam;
  else
    lpar = theParam;
  BB.Range(F_Edge, fpar, lpar);

  F_Edge.Free(true);
  BB.Remove(F_Edge, AnotherVertex);
  theVertex.Orientation(OrOfVertex);
  BB.Add(F_Edge, theVertex);
}

// Finds the edge connected to <theVertex> in the compound <theComp>
// that is closest to bisector plane angularly.
// Removes found edge from <theComp>
//<theAxis> is the axis of bisector plane
static TopoDS_Edge FindEdgeCloseToBisectorPlane(const TopoDS_Vertex& theVertex,
                                                TopoDS_Compound&     theComp,
                                                const gp_Ax1&        theAxis)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> VEmap;
  TopExp::MapShapesAndAncestors(theComp, TopAbs_VERTEX, TopAbs_EDGE, VEmap);

  TopoDS_Edge MinEdge;
  if (!VEmap.Contains(theVertex))
    return MinEdge;

  BRep_Builder BB;

  const NCollection_List<TopoDS_Shape>& Edges = VEmap.FindFromKey(theVertex);
  if (Edges.Extent() == 1)
    MinEdge = TopoDS::Edge(Edges.First());
  else
  {
    NCollection_List<TopoDS_Shape>::Iterator itl(Edges);
    double                      MinAngle = RealLast();
    for (; itl.More(); itl.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
      TopoDS_Wire        aWire;
      BB.MakeWire(aWire);
      BB.Add(aWire, anEdge);
      gp_Pln           aPln;
      bool issing;
      ComputeAveragePlaneAndMaxDeviation(aWire, aPln, issing);
      double anAngle;
      if (issing) // edge is a segment of line
      {
        //<anAngle> is angle between <anEdge> and its projection on bisector plane
        BRepAdaptor_Curve BAcurve(anEdge);
        gp_Pnt            FirstPnt = BAcurve.Value(BAcurve.FirstParameter());
        gp_Pnt            LastPnt  = BAcurve.Value(BAcurve.LastParameter());
        gp_Vec            EdgeVec(FirstPnt, LastPnt);
        gp_Ax1            EdgeAxis(FirstPnt, EdgeVec);
        anAngle = EdgeAxis.Direction().Angle(theAxis.Direction());
        if (anAngle > M_PI / 2)
          anAngle = M_PI - anAngle;
        anAngle = M_PI / 2 - anAngle;
      }
      else
      {
        anAngle = aPln.Axis().Angle(theAxis);
        if (anAngle > M_PI / 2)
          anAngle = M_PI - anAngle;
      }

      if (anAngle < MinAngle)
      {
        MinAngle = anAngle;
        MinEdge  = anEdge;
      }
    }
  } // else (more than one edge)

  BB.Remove(theComp, MinEdge);
  return MinEdge;
}

static bool FindMiddleEdges(const TopoDS_Vertex&  theVertex1,
                                        const TopoDS_Vertex&  theVertex2,
                                        const gp_Ax1&         theAxis,
                                        TopoDS_Compound&      theComp,
                                        NCollection_List<TopoDS_Shape>& theElist)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> VEmap;
  TopExp::MapShapesAndAncestors(theComp, TopAbs_VERTEX, TopAbs_EDGE, VEmap);
  if (VEmap.IsEmpty())
    return false;

  if (!VEmap.Contains(theVertex1) || !VEmap.Contains(theVertex2))
    return false;

  TopoDS_Vertex CurVertex = theVertex1;
  for (;;)
  {
    TopoDS_Edge CurEdge;

    CurEdge = FindEdgeCloseToBisectorPlane(CurVertex, theComp, theAxis);
    if (CurEdge.IsNull())
      return false;

    TopoDS_Vertex V1, V2;
    TopExp::Vertices(CurEdge, V1, V2);
    CurVertex = (V1.IsSame(CurVertex)) ? V2 : V1;

    theElist.Append(CurEdge);
    if (CurVertex.IsSame(theVertex2))
      return true;
  }
}

static bool FindCommonVertex(const TopoDS_Edge&   theFirstEdge,
                                         const TopoDS_Edge&   theLastEdge,
                                         const TopoDS_Vertex& theFirstVertex,
                                         const TopoDS_Vertex& theLastVertex,
                                         TopoDS_Vertex&       theCommonVertex)
{
  if (!theFirstVertex.IsSame(theLastVertex))
  {
    bool CommonVertexExists =
      TopExp::CommonVertex(theFirstEdge, theLastEdge, theCommonVertex);
    return CommonVertexExists;
  }

  TopoDS_Vertex V1, V2, V3, V4;
  TopExp::Vertices(theFirstEdge, V1, V2);
  TopExp::Vertices(theLastEdge, V3, V4);

  if (V1.IsSame(theFirstVertex))
  {
    if (V2.IsSame(V3) || V2.IsSame(V4))
    {
      theCommonVertex = V2;
      return true;
    }
  }
  else
  {
    if (V1.IsSame(V3) || V1.IsSame(V4))
    {
      theCommonVertex = V1;
      return true;
    }
  }

  return false;
}
