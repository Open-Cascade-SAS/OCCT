// Created by: Peter KURNEV
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRepLib_ValidateEdge.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomProjLib.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>

static void CheckEdge(
  const TopoDS_Edge&                                                   E,
  const double                                                         aMaxTol,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid);
static void CorrectEdgeTolerance(
  const TopoDS_Edge&                                                   myShape,
  const TopoDS_Face&                                                   S,
  const double                                                         aMaxTol,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid);

static void CorrectVertexTolerance(
  const TopoDS_Edge&                                                   aE,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid);

static void CorrectWires(
  const TopoDS_Face&                                                   aF,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid);

static void UpdateEdges(
  const TopoDS_Face&                                                   aF,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid);

static void UpdateShape(
  const TopoDS_Shape&                                                  aS,
  const double                                                         aTol,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid);

//=================================================================================================

class BOPTools_CPC
{
public:
  BOPTools_CPC()
      : myMaxTol(1.e-7),
        mypMapToAvoid(0L)
  {
  }

  //
  ~BOPTools_CPC() {}

  //
  void SetEdge(const TopoDS_Edge& aE) { myEdge = aE; }

  //
  const TopoDS_Edge& Edge() const { return myEdge; }

  //
  void SetMaxTol(const double aMaxTol) { myMaxTol = aMaxTol; }

  //
  double MaxTol() const { return myMaxTol; }

  //
  void SetMapToAvoid(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
  {
    mypMapToAvoid = &aMapToAvoid;
  }

  //
  void Perform()
  {
    Standard_ProgramError_Raise_if(!mypMapToAvoid, "mypMapToAvoid is null");
    CheckEdge(myEdge, myMaxTol, *mypMapToAvoid);
  }

protected:
  double                                                               myMaxTol;
  TopoDS_Edge                                                          myEdge;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* mypMapToAvoid;
};

//
//=======================================================================
typedef NCollection_Vector<BOPTools_CPC> BOPTools_VectorOfCPC;

//=================================================================================================

class BOPTools_CWT
{
public:
  BOPTools_CWT()
      : mypMapToAvoid(0L)
  {
  }

  //
  ~BOPTools_CWT() {}

  //
  void SetFace(const TopoDS_Face& aF) { myFace = aF; }

  //
  void SetMapToAvoid(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
  {
    mypMapToAvoid = &aMapToAvoid;
  }

  //
  void Perform()
  {
    Standard_ProgramError_Raise_if(!mypMapToAvoid, "mypMapToAvoid is null");
    CorrectWires(myFace, *mypMapToAvoid);
  }

  //
protected:
  TopoDS_Face                                                          myFace;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* mypMapToAvoid;
};

//=======================================================================
typedef NCollection_Vector<BOPTools_CWT> BOPTools_VectorOfCWT;

//=================================================================================================

class BOPTools_CDT
{
public:
  BOPTools_CDT()
      : myMaxTol(1.e-7),
        mypMapToAvoid(0L)
  {
  }

  //
  ~BOPTools_CDT() {}

  //
  void SetEdge(const TopoDS_Edge& aE) { myEdge = aE; }

  //
  void SetFace(const TopoDS_Face& aF) { myFace = aF; }

  //
  void SetMaxTol(const double aMaxTol) { myMaxTol = aMaxTol; }

  //
  void SetMapToAvoid(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
  {
    mypMapToAvoid = &aMapToAvoid;
  }

  //
  void Perform()
  {
    Standard_ProgramError_Raise_if(!mypMapToAvoid, "mypMapToAvoid is null");
    CorrectEdgeTolerance(myEdge, myFace, myMaxTol, *mypMapToAvoid);
  }

  //
protected:
  double                                                               myMaxTol;
  TopoDS_Edge                                                          myEdge;
  TopoDS_Face                                                          myFace;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* mypMapToAvoid;
};

//=======================================================================
typedef NCollection_Vector<BOPTools_CDT> BOPTools_VectorOfCDT;

//=================================================================================================

class BOPTools_CVT
{
public:
  BOPTools_CVT()
      : mypMapToAvoid(0L)
  {
  }

  //
  ~BOPTools_CVT() {}

  //
  void SetEdge(const TopoDS_Edge& aE) { myEdge = aE; }

  //
  void SetMapToAvoid(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
  {
    mypMapToAvoid = &aMapToAvoid;
  }

  //
  void Perform()
  {
    Standard_ProgramError_Raise_if(!mypMapToAvoid, "mypMapToAvoid is null");
    CorrectVertexTolerance(myEdge, *mypMapToAvoid);
  }

  //
protected:
  TopoDS_Edge                                                          myEdge;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* mypMapToAvoid;
};

//
//=======================================================================
typedef NCollection_Vector<BOPTools_CVT> BOPTools_VectorOfCVT;

//=================================================================================================

class BOPTools_CET
{
public:
  BOPTools_CET()
      : mypMapToAvoid(0L)
  {
  }

  //
  ~BOPTools_CET() {}

  //
  void SetFace(const TopoDS_Face& aF) { myFace = aF; }

  //
  void SetMapToAvoid(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
  {
    mypMapToAvoid = &aMapToAvoid;
  }

  //
  void Perform()
  {
    Standard_ProgramError_Raise_if(!mypMapToAvoid, "mypMapToAvoid is null");
    UpdateEdges(myFace, *mypMapToAvoid);
  }

  //
protected:
  TopoDS_Face                                                          myFace;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* mypMapToAvoid;
};

//=======================================================================
typedef NCollection_Vector<BOPTools_CET> BOPTools_VectorOfCET;

//=================================================================================================

void BOPTools_AlgoTools::CorrectTolerances(
  const TopoDS_Shape&                                                  aShape,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid,
  const double                                                         aMaxTol,
  const bool                                                           bRunParallel)
{
  BOPTools_AlgoTools::CorrectPointOnCurve(aShape, aMapToAvoid, aMaxTol, bRunParallel);
  BOPTools_AlgoTools::CorrectCurveOnSurface(aShape, aMapToAvoid, aMaxTol, bRunParallel);
}

//
//=================================================================================================

void BOPTools_AlgoTools::CorrectPointOnCurve(
  const TopoDS_Shape&                                                  aS,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid,
  const double                                                         aMaxTol,
  const bool                                                           bRunParallel)
{
  TopExp_Explorer      aExp;
  BOPTools_VectorOfCPC aVCPC;
  //
  aExp.Init(aS, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Edge& aE   = *((TopoDS_Edge*)&aExp.Current());
    BOPTools_CPC&      aCPC = aVCPC.Appended();
    aCPC.SetEdge(aE);
    aCPC.SetMaxTol(aMaxTol);
    aCPC.SetMapToAvoid(aMapToAvoid);
  }
  //
  //======================================================
  BOPTools_Parallel::Perform(bRunParallel, aVCPC);
  //======================================================
}

//=================================================================================================

void BOPTools_AlgoTools::CorrectCurveOnSurface(
  const TopoDS_Shape&                                                  aS,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid,
  const double                                                         aMaxTol,
  const bool                                                           bRunParallel)
{
  TopExp_Explorer      aExpF, aExpE;
  BOPTools_VectorOfCWT aVCWT;
  BOPTools_VectorOfCDT aVCDT;
  //
  aExpF.Init(aS, TopAbs_FACE);
  for (; aExpF.More(); aExpF.Next())
  {
    const TopoDS_Face& aF = *((TopoDS_Face*)&aExpF.Current());
    //
    BOPTools_CWT& aCWT = aVCWT.Appended();
    aCWT.SetFace(aF);
    aCWT.SetMapToAvoid(aMapToAvoid);
    //
    aExpE.Init(aF, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next())
    {
      const TopoDS_Edge& aE = *((TopoDS_Edge*)&aExpE.Current());
      //
      BOPTools_CDT& aCDT = aVCDT.Appended();
      aCDT.SetEdge(aE);
      aCDT.SetFace(aF);
      aCDT.SetMaxTol(aMaxTol);
      aCDT.SetMapToAvoid(aMapToAvoid);
    }
  }
  //
  //======================================================
  BOPTools_Parallel::Perform(bRunParallel, aVCWT);
  //======================================================
  BOPTools_Parallel::Perform(bRunParallel, aVCDT);
  //======================================================
}

//=================================================================================================

void BOPTools_AlgoTools::CorrectShapeTolerances(
  const TopoDS_Shape&                                                  aShape,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid,
  const bool                                                           bRunParallel)
{
  TopExp_Explorer      aExp;
  BOPTools_VectorOfCVT aVCVT;
  BOPTools_VectorOfCET aVCET;
  //
  aExp.Init(aShape, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Edge& aE   = *(TopoDS_Edge*)&aExp.Current();
    BOPTools_CVT&      aCVT = aVCVT.Appended();
    aCVT.SetEdge(aE);
    aCVT.SetMapToAvoid(aMapToAvoid);
  }
  //
  //======================================================
  BOPTools_Parallel::Perform(bRunParallel, aVCVT);
  //======================================================
  //
  aExp.Init(aShape, TopAbs_FACE);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Face& aF   = *(TopoDS_Face*)&aExp.Current();
    BOPTools_CET&      aCET = aVCET.Appended();
    aCET.SetFace(aF);
    aCET.SetMapToAvoid(aMapToAvoid);
  }
  //
  //======================================================
  BOPTools_Parallel::Perform(bRunParallel, aVCET);
  //======================================================
}

//
//=======================================================================
// Function : CheckEdge
// purpose :  Correct tolerances for Vertices on Edge
//=======================================================================
void CheckEdge(const TopoDS_Edge&                                                   Ed,
               const double                                                         aMaxTol,
               const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
{
  TopoDS_Edge aE = Ed;
  aE.Orientation(TopAbs_FORWARD);
  double aTolE = BRep_Tool::Tolerance(aE);
  //
  occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&aE.TShape());
  //
  const TopLoc_Location& Eloc = aE.Location();

  TopoDS_Iterator aItS(aE);
  for (; aItS.More(); aItS.Next())
  {
    const TopoDS_Vertex& aV = TopoDS::Vertex(aItS.Value());
    //
    occ::handle<BRep_TVertex>& TV  = *((occ::handle<BRep_TVertex>*)&aV.TShape());
    const gp_Pnt&              aPV = TV->Pnt();
    //
    double aTol = BRep_Tool::Tolerance(aV);
    aTol        = std::max(aTol, aTolE);
    double dd   = 0.1 * aTol;
    aTol *= aTol;
    //
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator aItCR(TE->Curves());
    while (aItCR.More())
    {
      const occ::handle<BRep_CurveRepresentation>& aCR = aItCR.Value();
      //
      if (aCR->IsCurve3D())
      {
        const occ::handle<Geom_Curve>& aC = aCR->Curve3D();
        if (!aC.IsNull())
        {
          TopLoc_Location L = (Eloc * aCR->Location()).Predivided(aV.Location());
          NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator aItPR(TV->Points());
          while (aItPR.More())
          {
            const occ::handle<BRep_PointRepresentation>& aPR = aItPR.Value();
            if (aPR->IsPointOnCurve(aC, L))
            {
              gp_Pnt aPC = aC->Value(aPR->Parameter());
              aPC.Transform(L.Transformation());
              double aD2 = aPV.SquareDistance(aPC);
              if (aD2 > aTol)
              {
                double aNewTolerance = sqrt(aD2) + dd;
                if (aNewTolerance < aMaxTol)
                  UpdateShape(aV, aNewTolerance, aMapToAvoid);
              }
            }
            aItPR.Next();
          }
          //
          TopAbs_Orientation aOrV = aV.Orientation();
          if (aOrV == TopAbs_FORWARD || aOrV == TopAbs_REVERSED)
          {
            occ::handle<BRep_GCurve> aGC(occ::down_cast<BRep_GCurve>(aCR));
            gp_Pnt                   aPC;
            if (aOrV == TopAbs_FORWARD)
            {
              aPC = aC->Value(aGC->First());
            }
            else
            {
              aPC = aC->Value(aGC->Last());
            }
            aPC.Transform(L.Transformation());
            //
            double aD2 = aPV.SquareDistance(aPC);
            if (aD2 > aTol)
            {
              double aNewTolerance = sqrt(aD2) + dd;
              if (aNewTolerance < aMaxTol)
                UpdateShape(aV, aNewTolerance, aMapToAvoid);
            }
          }
        }
      }
      aItCR.Next();
    } //  while (itcr.More()) {
  } // for (; aVExp.More(); aVExp.Next()) {
}

//=======================================================================
// Function : MapEdgeLength
// purpose  : Compute edge length and cache it in the map
//=======================================================================
static double MapEdgeLength(const TopoDS_Edge&                         theEdge,
                            NCollection_DataMap<TopoDS_Shape, double>& theMapEdgeLen)
{
  const double* pLen = theMapEdgeLen.Seek(theEdge);
  if (!pLen)
  {
    double aLen = 0.;
    if (!BRep_Tool::Degenerated(theEdge))
    {
      BRepAdaptor_Curve aCurve(theEdge);
      aLen = GCPnts_AbscissaPoint::Length(aCurve);
    }
    pLen = theMapEdgeLen.Bound(theEdge, aLen);
  }
  return *pLen;
}

//=======================================================================
// Function : EdgeData
// purpose : Structure to store edge data
//=======================================================================
namespace
{
struct EdgeData
{
  const TopoDS_Edge*  Edge;       // Edge
  double              VParameter; // Parameter of the vertex on the edge
  bool                IsClosed;   // Closed flag of the edge
  Geom2dAdaptor_Curve GAdaptor;   // 2D adaptor for PCurve of the edge on the face
  double              First;      // First parameter in the range
  double              Last;       // Last parameter in the rage
};
} // namespace

//=======================================================================
// Function : IntersectCurves2d
// purpose  : Intersect 2d curves of edges
//=======================================================================
static double IntersectCurves2d(const TopoDS_Vertex&                       theV,
                                const occ::handle<Geom_Surface>&           theS,
                                const EdgeData&                            theEData1,
                                const EdgeData&                            theEData2,
                                NCollection_DataMap<TopoDS_Shape, double>& theMapEdgeLen)
{
  Geom2dInt_GInter anInter;
  // Range of the first edge
  double aT11 = theEData1.First;
  double aT12 = theEData1.Last;
  // Range of the second edge
  double aT21 = theEData2.First;
  double aT22 = theEData2.Last;

  double aMaxDist = 0.;
  double aTol2d   = 1.e-10;
  //
  IntRes2d_Domain aDom1(theEData1.GAdaptor.Value(aT11),
                        aT11,
                        aTol2d,
                        theEData1.GAdaptor.Value(aT12),
                        aT12,
                        aTol2d);
  IntRes2d_Domain aDom2(theEData2.GAdaptor.Value(aT21),
                        aT21,
                        aTol2d,
                        theEData2.GAdaptor.Value(aT22),
                        aT22,
                        aTol2d);
  //
  anInter.Perform(theEData1.GAdaptor, aDom1, theEData2.GAdaptor, aDom2, aTol2d, aTol2d);
  if (!anInter.IsDone() || (!anInter.NbSegments() && !anInter.NbPoints()))
  {
    return aMaxDist;
  }
  //
  double                                       aT1, aT2, aTint1, aTint2, aHalfR1, aHalfR2, aDist;
  int                                          i, aNb;
  gp_Pnt                                       aP, aPV;
  gp_Pnt2d                                     aP2d;
  NCollection_List<IntRes2d_IntersectionPoint> aLP;
  NCollection_List<IntRes2d_IntersectionPoint>::Iterator aItLP;
  //
  aPV = BRep_Tool::Pnt(theV);
  aT1 = theEData1.VParameter;
  aT2 = theEData2.VParameter;
  //
  aHalfR1 = (aT12 - aT11) / 2.;
  aHalfR2 = (aT22 - aT21) / 2.;
  //
  aDist = 0.;
  //
  aNb = anInter.NbSegments();
  for (i = 1; i <= aNb; ++i)
  {
    const IntRes2d_IntersectionSegment& aSeg = anInter.Segment(i);
    aLP.Append(aSeg.FirstPoint());
    aLP.Append(aSeg.LastPoint());
  }
  //
  aNb = anInter.NbPoints();
  for (i = 1; i <= aNb; ++i)
  {
    const IntRes2d_IntersectionPoint& aPnt = anInter.Point(i);
    aLP.Append(aPnt);
  }
  //
  // evaluate the length of the smallest edge, so that not to return too large distance
  double       aLen1                      = MapEdgeLength(*theEData1.Edge, theMapEdgeLen);
  double       aLen2                      = MapEdgeLength(*theEData1.Edge, theMapEdgeLen);
  const double MaxEdgePartCoveredByVertex = 0.3;
  double       aMaxThresDist              = std::min(aLen1, aLen2) * MaxEdgePartCoveredByVertex;
  aMaxThresDist *= aMaxThresDist;
  aItLP.Initialize(aLP);
  for (; aItLP.More(); aItLP.Next())
  {
    const IntRes2d_IntersectionPoint& aPnt = aItLP.Value();
    //
    aTint1 = aPnt.ParamOnFirst();
    aTint2 = aPnt.ParamOnSecond();
    //
    if ((aTint1 < aT11 || aTint1 > aT12) || (aTint2 < aT21 || aTint2 > aT22))
    {
      // out of range;
      continue;
    }
    //
    if ((!theEData1.IsClosed && std::abs(aTint1 - aT1) > aHalfR1)
        || (!theEData2.IsClosed && std::abs(aTint2 - aT2) > aHalfR2))
    {
      // intersection is on the other end of the edge
      continue;
    }
    //
    aP2d = aPnt.Value();
    theS->D0(aP2d.X(), aP2d.Y(), aP);
    aDist = aPV.SquareDistance(aP);
    if (aDist > aMaxDist && aDist < aMaxThresDist)
    {
      aMaxDist = aDist;
    }
  }
  //
  return aMaxDist;
}

//=================================================================================================

void CorrectWires(const TopoDS_Face&                                                   aFx,
                  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
{
  int         i, aNbV;
  double      aTol, aTol2, aD2, aD2max, aT1, aT2;
  gp_Pnt      aP, aPV;
  gp_Pnt2d    aP2D;
  TopoDS_Face aF;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                           aMVE;
  NCollection_List<TopoDS_Shape>::Iterator aIt;
  //
  aF = aFx;
  aF.Orientation(TopAbs_FORWARD);
  const occ::handle<Geom_Surface>& aS = BRep_Tool::Surface(aFx);

  TopExp::MapShapesAndUniqueAncestors(aF, TopAbs_VERTEX, TopAbs_EDGE, aMVE, true);

  NCollection_DataMap<TopoDS_Shape, double> aMapEdgeLen;
  aNbV = aMVE.Extent();
  for (i = 1; i <= aNbV; ++i)
  {
    const TopoDS_Vertex& aV = *((TopoDS_Vertex*)&aMVE.FindKey(i));
    aPV                     = BRep_Tool::Pnt(aV);
    aTol                    = BRep_Tool::Tolerance(aV);
    aTol2                   = aTol * aTol;
    //
    aD2max = -1.;
    // Save edge's data to avoid its recalculation during intersection of 2d curves
    NCollection_List<EdgeData>            aLEPars;
    const NCollection_List<TopoDS_Shape>& aLE = aMVE.FindFromIndex(i);
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next())
    {
      const TopoDS_Edge&               aE   = *(TopoDS_Edge*)(&aIt.Value());
      const occ::handle<Geom2d_Curve>& aC2D = BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
      if (aC2D.IsNull())
      {
        continue;
      }

      double aT       = BRep_Tool::Parameter(aV, aE);
      bool   isClosed = false;
      {
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(aE, aV1, aV2);
        isClosed = aV1.IsSame(aV2);
      }
      //
      aC2D->D0(aT, aP2D);
      aS->D0(aP2D.X(), aP2D.Y(), aP);
      aD2 = aPV.SquareDistance(aP);
      if (aD2 > aD2max)
      {
        aD2max = aD2;
      }
      EdgeData anEData = {&aE, aT, isClosed, Geom2dAdaptor_Curve(aC2D), aT1, aT2};
      aLEPars.Append(anEData);
    }
    //
    // check wires on self interference by intersecting 2d curves of the edges
    NCollection_List<EdgeData>::Iterator aItE1(aLEPars);
    for (; aItE1.More(); aItE1.Next())
    {
      const EdgeData&     aEData1 = aItE1.Value();
      const TopoDS_Shape& aE1     = *aEData1.Edge;

      NCollection_List<EdgeData>::Iterator aItE2 = aItE1;
      for (aItE2.Next(); aItE2.More(); aItE2.Next())
      {
        const EdgeData&     aEData2 = aItE2.Value();
        const TopoDS_Shape& aE2     = *aEData2.Edge;

        if (aE1.IsSame(aE2))
          continue;

        aD2 = IntersectCurves2d(aV, aS, aEData1, aEData2, aMapEdgeLen);
        if (aD2 > aD2max)
        {
          aD2max = aD2;
        }
      }
    }
    //
    if (aD2max > aTol2)
    {
      aTol = 1.01 * sqrt(aD2max);
      UpdateShape(aV, aTol, aMapToAvoid);
    }
  } // for (i=1; i<=aNbV; ++i) {
}

//=======================================================================
// Function : CorrectEdgeTolerance
// purpose :  Correct tolerances for Edge
//=======================================================================
void CorrectEdgeTolerance(
  const TopoDS_Edge&                                                   myShape,
  const TopoDS_Face&                                                   S,
  const double                                                         aMaxTol,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
{
  //
  // 1. Minimum of conditions to Perform
  occ::handle<BRep_CurveRepresentation> myCref;
  occ::handle<Adaptor3d_Curve>          myHCurve;

  myCref.Nullify();

  occ::handle<BRep_TEdge>& TEx = *((occ::handle<BRep_TEdge>*)&myShape.TShape());
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcrx(TEx->Curves());
  bool Degenerated, SameParameterx, SameRangex;

  int unique = 0;

  Degenerated    = TEx->Degenerated();
  SameParameterx = TEx->SameParameter();
  SameRangex     = TEx->SameRange();

  if (!SameRangex && SameParameterx)
  {
    return;
  }

  occ::handle<Geom_Curve> C3d;
  while (itcrx.More())
  {
    const occ::handle<BRep_CurveRepresentation>& cr = itcrx.Value();
    if (cr->IsCurve3D())
    {
      unique++;
      if (myCref.IsNull() && !cr->Curve3D().IsNull())
      {
        myCref = cr;
      }
    }
    itcrx.Next();
  }

  if (unique == 0)
  {
    return; //...No3DCurve
  }
  if (unique > 1)
  {
    return; //...Multiple3DCurve;
  }

  if (myCref.IsNull() && !Degenerated)
  {
    itcrx.Initialize(TEx->Curves());
    while (itcrx.More())
    {
      const occ::handle<BRep_CurveRepresentation>& cr = itcrx.Value();
      if (cr->IsCurveOnSurface())
      {
        myCref = cr;
        break;
      }
      itcrx.Next();
    }
  }

  else if (!myCref.IsNull() && Degenerated)
  {
    return; //...InvalidDegeneratedFlag;
  }

  if (!myCref.IsNull())
  {
    occ::handle<BRep_GCurve> GCref(occ::down_cast<BRep_GCurve>(myCref));
    double                   First, Last;
    GCref->Range(First, Last);
    if (Last <= First)
    {
      myCref.Nullify();
      return; // InvalidRange;
    }

    else
    {
      if (myCref->IsCurve3D())
      {
        occ::handle<Geom_Curve> C3dx = occ::down_cast<Geom_Curve>(
          myCref->Curve3D()->Transformed(myCref->Location().Transformation()));
        GeomAdaptor_Curve GAC3d(C3dx, First, Last);
        myHCurve = new GeomAdaptor_Curve(GAC3d);
      }
      else
      { // curve on surface
        occ::handle<Geom_Surface> Sref = myCref->Surface();
        Sref = occ::down_cast<Geom_Surface>(Sref->Transformed(myCref->Location().Transformation()));
        const occ::handle<Geom2d_Curve>& PCref   = myCref->PCurve();
        occ::handle<GeomAdaptor_Surface> GAHSref = new GeomAdaptor_Surface(Sref);
        occ::handle<Geom2dAdaptor_Curve> GHPCref = new Geom2dAdaptor_Curve(PCref, First, Last);
        Adaptor3d_CurveOnSurface         ACSref(GHPCref, GAHSref);
        myHCurve = new Adaptor3d_CurveOnSurface(ACSref);
      }
    }
  }

  //===============================================
  // 2. Tolerances in InContext
  {
    if (myCref.IsNull())
      return;

    occ::handle<BRep_TEdge>& TE      = *((occ::handle<BRep_TEdge>*)&myShape.TShape());
    double                   Tol     = BRep_Tool::Tolerance(TopoDS::Edge(myShape));
    double                   aNewTol = Tol;

    bool   SameParameter = TE->SameParameter();
    bool   SameRange     = TE->SameRange();
    double First         = myHCurve->FirstParameter();
    double Last          = myHCurve->LastParameter();

    occ::handle<BRep_TFace>&         TF          = *((occ::handle<BRep_TFace>*)&S.TShape());
    const TopLoc_Location&           Floc        = S.Location();
    const TopLoc_Location&           TFloc       = TF->Location();
    const occ::handle<Geom_Surface>& Su          = TF->Surface();
    TopLoc_Location                  L           = (Floc * TFloc).Predivided(myShape.Location());
    bool                             pcurvefound = false;

    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
    while (itcr.More())
    {
      const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
      if (cr != myCref && cr->IsCurveOnSurface(Su, L))
      {
        pcurvefound = true;
        occ::handle<BRep_GCurve> GC(occ::down_cast<BRep_GCurve>(cr));
        double                   f, l;
        GC->Range(f, l);
        if (SameRange && (f != First || l != Last))
        {
          return; // BRepCheck_InvalidSameRangeFlag;
        }

        occ::handle<Geom_Surface> Sb = cr->Surface();
        Sb = occ::down_cast<Geom_Surface>(Su->Transformed(L.Transformation()));
        occ::handle<Geom2d_Curve>             PC   = cr->PCurve();
        occ::handle<GeomAdaptor_Surface>      GAHS = new GeomAdaptor_Surface(Sb);
        occ::handle<Geom2dAdaptor_Curve>      GHPC = new Geom2dAdaptor_Curve(PC, f, l);
        occ::handle<Adaptor3d_CurveOnSurface> ACS  = new Adaptor3d_CurveOnSurface(GHPC, GAHS);

        BRepLib_ValidateEdge aValidateEdge(myHCurve, ACS, SameParameter);
        aValidateEdge.Process();
        aValidateEdge.UpdateTolerance(aNewTol);
        if (aValidateEdge.IsDone() && !aValidateEdge.CheckTolerance(Tol))
        {
          if (aNewTol < aMaxTol)
          {
            UpdateShape(myShape, aNewTol, aMapToAvoid);
            CorrectVertexTolerance(myShape, aMapToAvoid);
          }
        }

        if (cr->IsCurveOnClosedSurface())
        {
          GHPC->Load(cr->PCurve2(), f, l); // same bounds
          ACS->Load(GHPC, GAHS);           // sans doute inutile

          BRepLib_ValidateEdge aValidateEdgeOnClosedSurf(myHCurve, ACS, SameParameter);
          aValidateEdgeOnClosedSurf.Process();
          aValidateEdgeOnClosedSurf.UpdateTolerance(aNewTol);
          if (aValidateEdgeOnClosedSurf.IsDone() && !aValidateEdgeOnClosedSurf.CheckTolerance(Tol))
          {
            if (aNewTol < aMaxTol)
            {
              UpdateShape(myShape, aNewTol, aMapToAvoid);
              CorrectVertexTolerance(myShape, aMapToAvoid);
            }
          }
        }
      }
      itcr.Next();
    }

    if (!pcurvefound)
    {
      occ::handle<Geom_Plane>    P;
      occ::handle<Standard_Type> styp = Su->DynamicType();
      if (styp == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        P = occ::down_cast<Geom_Plane>(
          occ::down_cast<Geom_RectangularTrimmedSurface>(Su)->BasisSurface());
      }
      else
      {
        P = occ::down_cast<Geom_Plane>(Su);
      }
      if (P.IsNull())
      { // not a plane
        return;
      }

      else
      { // on fait la projection a la volee, comme BRep_Tool
        P = occ::down_cast<Geom_Plane>(P->Transformed(L.Transformation()));
        // on projette Cref sur ce plan
        occ::handle<GeomAdaptor_Surface> GAHS = new GeomAdaptor_Surface(P);

        // Dub - Normalement myHCurve est une GeomAdaptor_Curve
        occ::handle<GeomAdaptor_Curve> Gac  = occ::down_cast<GeomAdaptor_Curve>(myHCurve);
        occ::handle<Geom_Curve>        C3dx = Gac->Curve();
        occ::handle<Geom_Curve>        ProjOnPlane =
          GeomProjLib::ProjectOnPlane(new Geom_TrimmedCurve(C3dx, First, Last),
                                      P,
                                      P->Position().Direction(),
                                      true);

        occ::handle<GeomAdaptor_Curve> aHCurve = new GeomAdaptor_Curve(ProjOnPlane);

        ProjLib_ProjectedCurve           proj(GAHS, aHCurve);
        occ::handle<Geom2d_Curve>        PC = Geom2dAdaptor::MakeCurve(proj);
        occ::handle<Geom2dAdaptor_Curve> GHPC =
          new Geom2dAdaptor_Curve(PC, myHCurve->FirstParameter(), myHCurve->LastParameter());

        occ::handle<Adaptor3d_CurveOnSurface> ACS = new Adaptor3d_CurveOnSurface(GHPC, GAHS);

        BRepLib_ValidateEdge aValidateProjEdge(myHCurve, ACS, true);
        aValidateProjEdge.Process();
        aValidateProjEdge.UpdateTolerance(aNewTol);
        if (aValidateProjEdge.IsDone() && !aValidateProjEdge.CheckTolerance(Tol))
        {
          if (aNewTol < aMaxTol)
          {
            UpdateShape(myShape, aNewTol, aMapToAvoid);
            CorrectVertexTolerance(myShape, aMapToAvoid);
          }
        }
      }
    } // end of if (!pcurvefound) {
  } // end of  2. Tolerances in InContext
}

//=================================================================================================

void CorrectVertexTolerance(
  const TopoDS_Edge&                                                   aE,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
{
  double          aTolE, aTolV;
  TopoDS_Iterator aIt;
  //
  aTolE = BRep_Tool::Tolerance(aE);
  aIt.Initialize(aE);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Vertex& aV = *((TopoDS_Vertex*)&aIt.Value());
    aTolV                   = BRep_Tool::Tolerance(aV);
    if (aTolV < aTolE)
    {
      UpdateShape(aV, aTolE, aMapToAvoid);
    }
  }
}

//=================================================================================================

void UpdateEdges(const TopoDS_Face&                                                   aF,
                 const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
{
  double          aTolF, aTolE, aTolV;
  TopoDS_Iterator aItF, aItW, aItE;
  //
  aTolE = aTolF = BRep_Tool::Tolerance(aF);
  aItF.Initialize(aF);
  for (; aItF.More(); aItF.Next())
  {
    const TopoDS_Shape& aS = aItF.Value();
    if (aS.ShapeType() == TopAbs_WIRE)
    {
      aItW.Initialize(aS);
      for (; aItW.More(); aItW.Next())
      {
        const TopoDS_Edge& aE = *((TopoDS_Edge*)&aItW.Value());
        aTolE                 = BRep_Tool::Tolerance(aE);
        if (aTolE < aTolF)
        {
          UpdateShape(aE, aTolF, aMapToAvoid);
          aTolE = aTolF;
        }
      }
    }
    else
    {
      const TopoDS_Vertex& aV = *(TopoDS_Vertex*)&aItF.Value();
      aTolV                   = BRep_Tool::Tolerance(aV);
      if (aTolV < aTolE)
      {
        UpdateShape(aV, aTolF, aMapToAvoid);
      }
    }
  }
}

//=================================================================================================

void UpdateShape(const TopoDS_Shape&                                                  aS,
                 const double                                                         aTol,
                 const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapToAvoid)
{
  if (aMapToAvoid.Contains(aS))
  {
    return;
  }
  //
  TopAbs_ShapeEnum aType;
  BRep_Builder     aBB;
  //
  aType = aS.ShapeType();
  if (aType == TopAbs_EDGE)
  {
    const TopoDS_Edge& aE = *((TopoDS_Edge*)&aS);
    aBB.UpdateEdge(aE, aTol);
  }
  else if (aType == TopAbs_VERTEX)
  {
    const TopoDS_Vertex& aV = *((TopoDS_Vertex*)&aS);
    aBB.UpdateVertex(aV, aTol);
  }
}

//=================================================================================================

bool BOPTools_AlgoTools::ComputeTolerance(const TopoDS_Face& theFace,
                                          const TopoDS_Edge& theEdge,
                                          double&            theMaxDist,
                                          double&            theMaxPar)
{
  BRepLib_CheckCurveOnSurface aCS;
  //
  aCS.Init(theEdge, theFace);
  aCS.Perform();
  if (!aCS.IsDone())
  {
    return false;
  }
  //
  theMaxDist = aCS.MaxDistance();
  theMaxPar  = aCS.MaxParameter();
  //
  return true;
}
