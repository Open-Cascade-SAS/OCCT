// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Standard_Integer.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>

#include <algorithm>
#include <cmath>

#ifdef OCCT_DEBUG
extern bool TopOpeBRepBuild_GettraceCU();
#endif

namespace
{
struct TopOpeBRepBuild_SurfaceCurve
{
  int                       Index;
  occ::handle<Geom2d_Curve> PCurve;
  double                    FirstParameter;
  double                    LastParameter;
};

bool TopOpeBRepBuild_CurveRange(const TopOpeBRepDS_Curve&        theCurve,
                                const occ::handle<Geom2d_Curve>& thePCurve,
                                double&                          theFirst,
                                double&                          theLast)
{
  const auto isValidRange = [](const double theRangeFirst, const double theRangeLast) {
    return theRangeFirst < theRangeLast && !Precision::IsInfinite(theRangeFirst)
           && !Precision::IsInfinite(theRangeLast);
  };

  if (theCurve.Range(theFirst, theLast) && isValidRange(theFirst, theLast))
  {
    return true;
  }
  if (!thePCurve.IsNull())
  {
    theFirst = thePCurve->FirstParameter();
    theLast  = thePCurve->LastParameter();
    if (isValidRange(theFirst, theLast))
    {
      return true;
    }
  }

  const occ::handle<Geom_Curve>& aCurve3d = theCurve.Curve();
  if (!aCurve3d.IsNull())
  {
    theFirst = aCurve3d->FirstParameter();
    theLast  = aCurve3d->LastParameter();
    return isValidRange(theFirst, theLast);
  }
  return false;
}

bool TopOpeBRepBuild_HasCompleteCoincidence(const Geom2dInt_GInter&    theIntersector,
                                            const Geom2dAdaptor_Curve& theFirstCurve,
                                            const Geom2dAdaptor_Curve& theSecondCurve,
                                            bool&                      theIsReversed)
{
  if (theIntersector.NbSegments() != 1)
  {
    return false;
  }

  const IntRes2d_IntersectionSegment& aSegment = theIntersector.Segment(1);
  if (!aSegment.HasFirstPoint() || !aSegment.HasLastPoint())
  {
    return false;
  }

  const double aFirstOnFirst  = aSegment.FirstPoint().ParamOnFirst();
  const double aLastOnFirst   = aSegment.LastPoint().ParamOnFirst();
  const double aFirstOnSecond = aSegment.FirstPoint().ParamOnSecond();
  const double aLastOnSecond  = aSegment.LastPoint().ParamOnSecond();
  const double aTolerance     = Precision::PConfusion();
  const bool   isComplete =
    std::abs(std::min(aFirstOnFirst, aLastOnFirst) - theFirstCurve.FirstParameter()) <= aTolerance
    && std::abs(std::max(aFirstOnFirst, aLastOnFirst) - theFirstCurve.LastParameter()) <= aTolerance
    && std::abs(std::min(aFirstOnSecond, aLastOnSecond) - theSecondCurve.FirstParameter())
         <= aTolerance
    && std::abs(std::max(aFirstOnSecond, aLastOnSecond) - theSecondCurve.LastParameter())
         <= aTolerance;
  if (isComplete)
  {
    theIsReversed = (aLastOnFirst - aFirstOnFirst) * (aLastOnSecond - aFirstOnSecond) < 0.0;
  }
  return isComplete;
}

bool TopOpeBRepBuild_FindCurveEnd(const occ::handle<TopOpeBRepDS_HDataStructure>& theHDS,
                                  const int                                       theCurveIndex,
                                  const bool                                      theCurveStart,
                                  int&                                            theGeometryIndex,
                                  bool&                                           theIsPoint)
{
  double aFirst, aLast;
  if (!TopOpeBRepBuild_CurveRange(theHDS->Curve(theCurveIndex), nullptr, aFirst, aLast))
  {
    return false;
  }

  const double aParameter    = theCurveStart ? aFirst : aLast;
  double       aBestDistance = RealLast();
  for (TopOpeBRepDS_PointIterator aPointIt(theHDS->CurvePoints(theCurveIndex)); aPointIt.More();
       aPointIt.Next())
  {
    const double aDistance = std::abs(aPointIt.Parameter() - aParameter);
    if (aDistance < aBestDistance)
    {
      aBestDistance    = aDistance;
      theGeometryIndex = aPointIt.Current();
      theIsPoint       = aPointIt.IsPoint();
    }
  }
  return aBestDistance <= Precision::PConfusion();
}
} // namespace

//=================================================================================================

void TopOpeBRepBuild_Builder::BuildEdges(const int                                       iC,
                                         const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceCU())
    std::cout << "\nBuildEdges on C " << iC << std::endl;
#endif
  const TopOpeBRepDS_Curve&                     C   = HDS->Curve(iC);
  const occ::handle<Geom_Curve>&                C3D = C.Curve();
  const occ::handle<TopOpeBRepDS_Interference>& I1  = C.GetSCI1();
  const occ::handle<TopOpeBRepDS_Interference>& I2  = C.GetSCI2();
  bool                                          nnn = C3D.IsNull() && I1.IsNull() && I2.IsNull();
  if (nnn)
  {
    return;
  }

  if (C.EquivalentCurve() > 0 && C.EquivalentCurve() != iC)
  {
    const int aReferenceCurve = C.EquivalentCurve();
    if (NewEdges(aReferenceCurve).IsEmpty())
    {
      BuildEdges(aReferenceCurve, HDS);
    }
    ChangeNewEdges(iC) = NewEdges(aReferenceCurve);
    return;
  }

  if (!C.ExistingEdge().IsNull())
  {
    if (!myCoincidentEdges.IsBound(C.ExistingEdge()))
    {
      TopoDS_Shape aCopiedEdge;
      myBuildTool.CopyEdge(C.ExistingEdge(), aCopiedEdge);
      for (TopExp_Explorer aVertexIt(C.ExistingEdge(), TopAbs_VERTEX); aVertexIt.More();
           aVertexIt.Next())
      {
        myBuildTool.AddEdgeVertex(C.ExistingEdge(), aCopiedEdge, aVertexIt.Current());
      }
      myCoincidentEdges.Bind(C.ExistingEdge(), aCopiedEdge);
    }
    TopoDS_Shape anEdge = myCoincidentEdges(C.ExistingEdge());
    anEdge.Orientation(TopAbs_FORWARD);
    ChangeNewEdges(iC).Append(anEdge);
    return;
  }

  TopoDS_Shape              anEdge;
  const TopOpeBRepDS_Curve& curC = HDS->Curve(iC);
  myBuildTool.MakeEdge(anEdge, curC, HDS->DS());
  TopOpeBRepBuild_PaveSet    PVS(anEdge);
  TopOpeBRepDS_PointIterator CPIT(HDS->CurvePoints(iC));
  FillVertexSet(CPIT, TopAbs_IN, PVS);
  TopOpeBRepBuild_PaveClassifier VCL(anEdge);
  bool                           equalpar = PVS.HasEqualParameters();
  if (equalpar)
  {
    VCL.SetFirstParameter(PVS.EqualParameters());
  }
  bool closvert = PVS.ClosedVertices();
  VCL.ClosedVertices(closvert);
  PVS.InitLoop();
  if (!PVS.MoreLoop())
  {
    return;
  }
  TopOpeBRepBuild_EdgeBuilder     EDBU(PVS, VCL);
  NCollection_List<TopoDS_Shape>& EL = ChangeNewEdges(iC);
  MakeEdges(anEdge, EDBU, EL);
  NCollection_List<TopoDS_Shape>::Iterator It(EL);
  int                                      inewC = -1;
  for (; It.More(); It.Next())
  {
    TopoDS_Edge& newEdge = TopoDS::Edge(It.ChangeValue());
    myBuildTool.RecomputeCurves(curC, TopoDS::Edge(anEdge), newEdge, inewC, HDS);
    if (inewC != -1)
    {
      ChangeNewEdges(inewC).Append(newEdge);
    }
  }
  if (inewC != -1)
  {
    HDS->RemoveCurve(iC);
  }
  else
  {
    for (It.Initialize(EL); It.More(); It.Next())
    {
      TopoDS_Edge& newEdge = TopoDS::Edge(It.ChangeValue());
      myBuildTool.UpdateEdge(anEdge, newEdge);
    }
  }
}

//=================================================================================================

void TopOpeBRepBuild_Builder::BuildEdges(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();

  myNewEdges.Clear();
  myCoincidentEdges.Clear();
  TopOpeBRepDS_CurveExplorer cex;

  NCollection_LinearVector<TopOpeBRepBuild_SurfaceCurve> aSurfaceCurves;
  for (int aSurfaceIndex = 1; aSurfaceIndex <= HDS->NbSurfaces(); ++aSurfaceIndex)
  {
    aSurfaceCurves.Clear();
    for (TopOpeBRepDS_CurveIterator aCurveIt(HDS->SurfaceCurves(aSurfaceIndex)); aCurveIt.More();
         aCurveIt.Next())
    {
      const occ::handle<Geom2d_Curve>& aPCurve = aCurveIt.PCurve();
      const TopOpeBRepDS_Curve&        aCurve  = HDS->Curve(aCurveIt.Current());
      if (aPCurve.IsNull() || aCurve.Curve().IsNull())
      {
        continue;
      }

      double aFirst, aLast;
      if (TopOpeBRepBuild_CurveRange(aCurve, aPCurve, aFirst, aLast))
      {
        aSurfaceCurves.Append({aCurveIt.Current(), aPCurve, aFirst, aLast});
      }
    }

    for (size_t aFirstIndex = 0; aFirstIndex < aSurfaceCurves.Size(); ++aFirstIndex)
    {
      for (size_t aSecondIndex = aFirstIndex + 1; aSecondIndex < aSurfaceCurves.Size();
           ++aSecondIndex)
      {
        const TopOpeBRepBuild_SurfaceCurve& aFirst  = aSurfaceCurves[aFirstIndex];
        const TopOpeBRepBuild_SurfaceCurve& aSecond = aSurfaceCurves[aSecondIndex];
        Geom2dAdaptor_Curve aFirstCurve(aFirst.PCurve, aFirst.FirstParameter, aFirst.LastParameter);
        Geom2dAdaptor_Curve aSecondCurve(aSecond.PCurve,
                                         aSecond.FirstParameter,
                                         aSecond.LastParameter);
        Geom2dInt_GInter    anIntersector(aFirstCurve,
                                       aSecondCurve,
                                       Precision::PConfusion(),
                                       Precision::PConfusion());
        bool                isReversed = false;
        if (TopOpeBRepBuild_HasCompleteCoincidence(anIntersector,
                                                   aFirstCurve,
                                                   aSecondCurve,
                                                   isReversed))
        {
          BDS.MergeEquivalentCurves(aFirst.Index, aSecond.Index, isReversed);
        }
      }
    }
  }

  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    const int aCurveIndex = cex.Index();
    if (cex.Curve().EquivalentCurve() <= 0)
    {
      continue;
    }
    bool      isReversed = false;
    const int aRoot      = BDS.FindEquivalentCurve(aCurveIndex, isReversed);
    BDS.ChangeCurve(aCurveIndex).SetEquivalentCurve(aRoot, isReversed);
  }

  NCollection_DataMap<int, TopoDS_Shape> anEquivalentPointVertices;
  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    const int aCurveIndex = cex.Index();
    for (int anEnd = 0; anEnd < 2; ++anEnd)
    {
      const bool isCurveStart     = (anEnd == 0);
      bool       isReferenceStart = false;
      const int  aReferenceCurve =
        BDS.FindEquivalentCurvePoint(aCurveIndex, isCurveStart, isReferenceStart);
      if (aReferenceCurve == aCurveIndex && isReferenceStart == isCurveStart)
      {
        continue;
      }

      const int aReference = 2 * aReferenceCurve + (isReferenceStart ? 0 : 1);
      if (anEquivalentPointVertices.IsBound(aReference))
      {
        continue;
      }

      int  aGeometryIndex = 0;
      bool isPoint        = false;
      if (TopOpeBRepBuild_FindCurveEnd(HDS,
                                       aReferenceCurve,
                                       isReferenceStart,
                                       aGeometryIndex,
                                       isPoint))
      {
        anEquivalentPointVertices.Bind(aReference,
                                       isPoint ? NewVertex(aGeometryIndex)
                                               : HDS->Shape(aGeometryIndex));
      }
    }
  }

  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    const int aCurveIndex = cex.Index();
    for (int anEnd = 0; anEnd < 2; ++anEnd)
    {
      const bool isCurveStart     = (anEnd == 0);
      bool       isReferenceStart = false;
      const int  aReferenceCurve =
        BDS.FindEquivalentCurvePoint(aCurveIndex, isCurveStart, isReferenceStart);
      const int aReference = 2 * aReferenceCurve + (isReferenceStart ? 0 : 1);
      if (!anEquivalentPointVertices.IsBound(aReference))
      {
        continue;
      }

      int  aGeometryIndex = 0;
      bool isPoint        = false;
      if (TopOpeBRepBuild_FindCurveEnd(HDS, aCurveIndex, isCurveStart, aGeometryIndex, isPoint)
          && isPoint)
      {
        ChangeNewVertex(aGeometryIndex) = anEquivalentPointVertices(aReference);
      }
    }
  }

  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    const int aCurveIndex     = cex.Index();
    const int aReferenceCurve = cex.Curve().EquivalentCurve();
    if (aReferenceCurve <= 0 || aReferenceCurve == aCurveIndex)
    {
      continue;
    }

    for (TopOpeBRepDS_PointIterator aPointIt(HDS->CurvePoints(aCurveIndex)); aPointIt.More();
         aPointIt.Next())
    {
      if (!aPointIt.IsPoint())
      {
        continue;
      }
      const int                 aPointIndex = aPointIt.Current();
      const TopOpeBRepDS_Point& aPoint      = HDS->Point(aPointIndex);
      for (TopOpeBRepDS_PointIterator aReferencePointIt(HDS->CurvePoints(aReferenceCurve));
           aReferencePointIt.More();
           aReferencePointIt.Next())
      {
        if (!aReferencePointIt.IsPoint())
        {
          continue;
        }
        const int                 aReferencePointIndex = aReferencePointIt.Current();
        const TopOpeBRepDS_Point& aReferencePoint      = HDS->Point(aReferencePointIndex);
        const double aTolerance = std::max(aPoint.Tolerance(), aReferencePoint.Tolerance());
        if (aPoint.Point().Distance(aReferencePoint.Point()) <= aTolerance)
        {
          ChangeNewVertex(aPointIndex) = NewVertex(aReferencePointIndex);
          break;
        }
      }
    }
  }

  int ick = 0;
  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    int  ic = cex.Index();
    bool ck = cex.IsCurveKeep(ic);
    int  im = cex.Curve(ic).Mother();
    if (ck == 1 && im != 0 && ick == 0)
    {
      ick = ic;
      break;
    }
  }
  if (ick)
  {
    for (cex.Init(BDS, true); cex.More(); cex.Next())
    {
      int ic = cex.Index();
      BDS.RemoveCurve(ic);
    }
    BDS.ChangeNbCurves(ick - 1);
  }

  for (cex.Init(BDS, false); cex.More(); cex.Next())
  {
    int ic = cex.Index();
    int im = cex.Curve(ic).Mother();
    if (im != 0)
    {
      continue;
    }
    BuildEdges(ic, HDS);
  }

  int                      ip, np = HDS->NbPoints();
  NCollection_HArray1<int> tp(0, np, 0);
  for (cex.Init(BDS); cex.More(); cex.Next())
  {
#ifdef OCCT_DEBUG
//    const TopOpeBRepDS_Curve& C = cex.Curve();
#endif
    int                                                                ic = cex.Index();
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(
      BDS.CurveInterferences(ic));
    for (; it.More(); it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      {
        int               ig = I->Geometry();
        TopOpeBRepDS_Kind kg = I->GeometryType();
        if (kg == TopOpeBRepDS_POINT && ig <= np)
        {
          tp.ChangeValue(ig) = tp.Value(ig) + 1;
        }
      }
      {
        int               is = I->Support();
        TopOpeBRepDS_Kind ks = I->SupportType();
        if (ks == TopOpeBRepDS_POINT)
        {
          tp.ChangeValue(is) = tp.Value(is) + 1;
        }
      }
    }
  }
  int is, ns = BDS.NbShapes();
  for (is = 1; is <= ns; is++)
  {
    const TopoDS_Shape& S = BDS.Shape(is);
    if (S.IsNull())
    {
      continue;
    }
    bool test = (S.ShapeType() == TopAbs_EDGE);
    if (!test)
    {
      continue;
    }
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(
      BDS.ShapeInterferences(is));
    for (; it.More(); it.Next())
    {
      const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
      {
        int               ig = I->Geometry();
        TopOpeBRepDS_Kind kg = I->GeometryType();
        if (kg == TopOpeBRepDS_POINT)
        {
          tp.ChangeValue(ig) = tp.Value(ig) + 1;
        }
      }
      {
        int               is1 = I->Support();
        TopOpeBRepDS_Kind ks  = I->SupportType();
        if (ks == TopOpeBRepDS_POINT)
        {
          tp.ChangeValue(is1) = tp.Value(is1) + 1;
        }
      }
    }
  }
  for (ip = 1; ip <= np; ip++)
  {
    if (tp.Value(ip) == 0)
    {
      BDS.RemovePoint(ip);
    }
  }
}
