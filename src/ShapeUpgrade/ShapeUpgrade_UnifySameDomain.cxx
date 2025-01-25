// Created on: 2012-06-09
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#include <ShapeUpgrade_UnifySameDomain.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom2d_Circle.hxx>
#include <GCE2d_MakeLine.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_Array1OfBSplineCurve.hxx>
#include <TColGeom2d_HArray1OfBSplineCurve.hxx>
#include <TColGeom2d_SequenceOfBoundedCurve.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColGeom_SequenceOfSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <gp_Circ.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Vec2d.hxx>
#include <Extrema_ExtPS.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <ElSLib.hxx>
#include <GeomProjLib.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_UnifySameDomain, Standard_Transient)

static Standard_Boolean IsOnSingularity(const TopTools_ListOfShape& theEdgeList)
{
  TopTools_ListIteratorOfListOfShape anItl(theEdgeList);
  for (; anItl.More(); anItl.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anItl.Value());
    if (BRep_Tool::Degenerated(anEdge))
      return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
// function : IsUiso
// purpose  : only for seam edges assumed to be U- or V- isolines
//=======================================================================
static Standard_Boolean IsUiso(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace)
{
  BRepAdaptor_Curve2d aBAcurve2d(theEdge, theFace);
  gp_Pnt2d            aP2d;
  gp_Vec2d            aVec;
  aBAcurve2d.D1(aBAcurve2d.FirstParameter(), aP2d, aVec);
  return (Abs(aVec.Y()) > Abs(aVec.X()));
}

static Standard_Boolean IsLinear(const BRepAdaptor_Curve& theBAcurve, gp_Dir& theDir)
{
  GeomAbs_CurveType aType = theBAcurve.GetType();

  if (aType == GeomAbs_Line)
  {
    theDir = theBAcurve.Line().Position().Direction();
    return Standard_True;
  }

  if ((aType == GeomAbs_BezierCurve || aType == GeomAbs_BSplineCurve) && theBAcurve.NbPoles() == 2)
  {
    gp_Pnt aFirstPnt = theBAcurve.Value(theBAcurve.FirstParameter());
    gp_Pnt aLastPnt  = theBAcurve.Value(theBAcurve.LastParameter());
    theDir           = gp_Vec(aFirstPnt, aLastPnt);
    return Standard_True;
  }

  return Standard_False;
}

static void SplitWire(const TopoDS_Wire&                theWire,
                      const TopoDS_Face&                theFace,
                      const TopTools_IndexedMapOfShape& theVmap,
                      TopTools_SequenceOfShape&         theWireSeq);

static Standard_Real TrueValueOfOffset(const Standard_Real theValue, const Standard_Real thePeriod)
{
  if (theValue > 0.)
    return thePeriod;

  return (-thePeriod);
}

//=======================================================================
// function : GetFaceFromSeq
// purpose  : gets a face from the sequence that has a common boundary
//           with already got <theUsedFaces>. If it is possible,
//           gets a face that has a common boundary in 2D space too
//           (excludes seam edges).
//           <theIndFace> returns the Index of the face in the sequence.
//=======================================================================
static TopoDS_Face GetFaceFromSeq(const TopTools_SequenceOfShape&                  theFaces,
                                  const TopoDS_Face&                               theRefFace,
                                  const TopTools_IndexedDataMapOfShapeListOfShape& theMapEF,
                                  TopTools_IndexedMapOfShape&                      theUsedFaces,
                                  Standard_Integer&                                theIndFace)
{
  if (theUsedFaces.IsEmpty())
  {
    theIndFace = 1;
    theUsedFaces.Add(theFaces(1));
    return TopoDS::Face(theFaces(1));
  }

  TopoDS_Edge aSeamEdge;
  TopoDS_Face aTargetFace;
  for (Standard_Integer ii = 1; ii <= theUsedFaces.Extent(); ii++)
  {
    const TopoDS_Face& aUsedFace = TopoDS::Face(theUsedFaces(ii));
    TopExp_Explorer    anExplo(aUsedFace, TopAbs_EDGE);
    for (; anExplo.More(); anExplo.Next())
    {
      const TopoDS_Edge&          anEdge    = TopoDS::Edge(anExplo.Current());
      const TopTools_ListOfShape& aFaceList = theMapEF.FindFromKey(anEdge);
      if (aFaceList.Extent() < 2)
        continue;

      const TopoDS_Face& aFace = (aFaceList.First().IsSame(aUsedFace))
                                   ? TopoDS::Face(aFaceList.Last())
                                   : TopoDS::Face(aFaceList.First());

      if (theUsedFaces.Contains(aFace))
        continue;

      if (BRep_Tool::IsClosed(anEdge, theRefFace))
      {
        aSeamEdge = anEdge;
        continue;
      }

      aTargetFace = aFace;
      break;
    }
    if (aTargetFace.IsNull() && !aSeamEdge.IsNull())
    {
      const TopTools_ListOfShape& aFaceList = theMapEF.FindFromKey(aSeamEdge);
      aTargetFace = (aFaceList.First().IsSame(aUsedFace)) ? TopoDS::Face(aFaceList.Last())
                                                          : TopoDS::Face(aFaceList.First());
    }
    if (!aTargetFace.IsNull())
      break;
  }

  theUsedFaces.Add(aTargetFace);
  for (Standard_Integer ii = 2; ii <= theFaces.Length(); ii++)
    if (theFaces(ii).IsSame(aTargetFace))
    {
      theIndFace = ii;
      break;
    }

  return aTargetFace;
}

//=================================================================================================

static void UpdateBoundaries(const Handle(Geom2d_Curve)& thePCurve,
                             const Standard_Real         theFirst,
                             const Standard_Real         theLast,
                             const Standard_Integer      theIndCoord,
                             Standard_Real&              theMinCoord,
                             Standard_Real&              theMaxCoord)
{
  const Standard_Integer NbSamples = 4;
  Standard_Real          Delta     = (theLast - theFirst) / NbSamples;

  for (Standard_Integer i = 0; i <= NbSamples; i++)
  {
    Standard_Real aParam = theFirst + i * Delta;
    gp_Pnt2d      aPoint = thePCurve->Value(aParam);

    if (aPoint.Coord(theIndCoord) < theMinCoord)
      theMinCoord = aPoint.Coord(theIndCoord);

    if (aPoint.Coord(theIndCoord) > theMaxCoord)
      theMaxCoord = aPoint.Coord(theIndCoord);
  }
}

static Standard_Boolean TryMakeLine(const Handle(Geom2d_Curve)& thePCurve,
                                    const Standard_Real         theFirst,
                                    const Standard_Real         theLast,
                                    Handle(Geom2d_Line)&        theLine)
{
  gp_Pnt2d      aFirstPnt = thePCurve->Value(theFirst);
  gp_Pnt2d      aLastPnt  = thePCurve->Value(theLast);
  gp_Vec2d      aVec(aFirstPnt, aLastPnt);
  Standard_Real aSqLen      = aVec.SquareMagnitude();
  Standard_Real aSqParamLen = (theLast - theFirst) * (theLast - theFirst);
  if (Abs(aSqLen - aSqParamLen) > Precision::Confusion())
    return Standard_False;

  gp_Dir2d aDir     = aVec;
  gp_Vec2d anOffset = -aDir;
  anOffset *= theFirst;
  gp_Pnt2d anOrigin = aFirstPnt.Translated(anOffset);
  gp_Lin2d aLin(anOrigin, aDir);

  const Standard_Integer NbSamples = 10;
  Standard_Real          aDelta    = (theLast - theFirst) / NbSamples;
  for (Standard_Integer i = 1; i < NbSamples; i++)
  {
    Standard_Real aParam = theFirst + i * aDelta;
    gp_Pnt2d      aPnt   = thePCurve->Value(aParam);
    Standard_Real aDist  = aLin.Distance(aPnt);
    if (aDist > Precision::Confusion())
      return Standard_False;
  }

  theLine = new Geom2d_Line(aLin);
  return Standard_True;
}

static void RemoveEdgeFromMap(const TopoDS_Edge&                         theEdge,
                              TopTools_IndexedDataMapOfShapeListOfShape& theVEmap)
{
  TopoDS_Vertex VV[2];
  TopExp::Vertices(theEdge, VV[0], VV[1]);
  for (Standard_Integer i = 0; i < 2; i++)
  {
    if (!theVEmap.Contains(VV[i]))
      continue;
    TopTools_ListOfShape&              Elist = theVEmap.ChangeFromKey(VV[i]);
    TopTools_ListIteratorOfListOfShape itl(Elist);
    while (itl.More())
    {
      const TopoDS_Shape& anEdge = itl.Value();
      if (anEdge.IsSame(theEdge))
        Elist.Remove(itl);
      else
        itl.Next();
    }
  }
}

static Standard_Real ComputeMinEdgeSize(const TopTools_SequenceOfShape& theEdges,
                                        const TopoDS_Face&              theRefFace,
                                        TopTools_MapOfShape&            theEdgesMap)
{
  Standard_Real MinSize = RealLast();

  for (Standard_Integer ind = 1; ind <= theEdges.Length(); ind++)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(theEdges(ind));
    theEdgesMap.Add(anEdge);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anEdge, V1, V2);
    BRepAdaptor_Curve2d BAcurve2d(anEdge, theRefFace);
    gp_Pnt2d            FirstP2d = BAcurve2d.Value(BAcurve2d.FirstParameter());
    gp_Pnt2d            LastP2d  = BAcurve2d.Value(BAcurve2d.LastParameter());
    Standard_Real       aSqDist;
    if (V1.IsSame(V2) && !BRep_Tool::Degenerated(anEdge))
    {
      gp_Pnt2d MidP2d =
        BAcurve2d.Value((BAcurve2d.FirstParameter() + BAcurve2d.LastParameter()) / 2);
      aSqDist = FirstP2d.SquareDistance(MidP2d);
    }
    else
      aSqDist = FirstP2d.SquareDistance(LastP2d);
    if (aSqDist < MinSize)
      MinSize = aSqDist;
  }
  MinSize = Sqrt(MinSize);
  return MinSize;
}

//=======================================================================
// function : FindCoordBounds
// purpose  : Searching for origin of U in 2d space
//           Returns Standard_False if could not find curve on surface
//           Returns Standard_True if succeed
//=======================================================================
static Standard_Boolean FindCoordBounds(const TopTools_SequenceOfShape&                  theFaces,
                                        const TopoDS_Face&                               theRefFace,
                                        const TopTools_IndexedDataMapOfShapeListOfShape& theMapEF,
                                        const TopTools_MapOfShape& theEdgesMap,
                                        const Standard_Integer     theIndCoord,
                                        const Standard_Real        thePeriod,
                                        Standard_Real&             theMinCoord,
                                        Standard_Real&             theMaxCoord,
                                        Standard_Integer&          theNumberOfIntervals,
                                        Standard_Integer&          theIndFaceMax)
{
  NCollection_Sequence<std::pair<Standard_Real, Standard_Real>> aPairSeq;

  Standard_Real aSimpleMax = RealFirst();
  theIndFaceMax            = 0;

  TopTools_IndexedMapOfShape aUsedFaces;
  TopoDS_Face                aLastFace;
  for (Standard_Integer ii = 1; ii <= theFaces.Length(); ii++)
  {
    Standard_Integer anIndFace = 0;
    // Get a face bordering with previous ones
    TopoDS_Face     aFace = GetFaceFromSeq(theFaces, theRefFace, theMapEF, aUsedFaces, anIndFace);
    Standard_Real   aMinCoord = RealLast(), aMaxCoord = RealFirst();
    TopExp_Explorer Explo(aFace, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
      if (!theEdgesMap.Contains(anEdge))
        continue;
      Standard_Real        fpar, lpar;
      Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, theRefFace, fpar, lpar);
      if (aPCurve.IsNull())
      {
        return Standard_False;
      }
      UpdateBoundaries(aPCurve, fpar, lpar, theIndCoord, aMinCoord, aMaxCoord);
    }

    if (Precision::IsInfinite(aMinCoord) || Precision::IsInfinite(aMaxCoord))
      continue;

    if (aMaxCoord > aSimpleMax)
    {
      aSimpleMax    = aMaxCoord;
      theIndFaceMax = anIndFace;
    }

    // Insert new interval (aMinCoord, aMaxCoord) into sequence of intervals
    Standard_Boolean anIsInInterval = Standard_False;
    for (Standard_Integer jj = 1; jj <= aPairSeq.Length(); jj++)
    {
      Standard_Real aLocalMin = aPairSeq(jj).first;
      Standard_Real aLocalMax = aPairSeq(jj).second;
      if (aMinCoord >= aLocalMin && aMinCoord <= aLocalMax && aMaxCoord >= aLocalMin
          && aMaxCoord <= aLocalMax)
      {
        anIsInInterval = Standard_True;
        break;
      }

      if (aMinCoord < aLocalMin && (aMaxCoord >= aLocalMin && aMaxCoord <= aLocalMax))
      {
        aPairSeq(jj).first = aMinCoord;
        anIsInInterval     = Standard_True;
        break;
      }
      else if (aMinCoord < aLocalMin && aMaxCoord > aLocalMax)
      {
        aPairSeq(jj).first  = aMinCoord;
        aPairSeq(jj).second = aMaxCoord;
        anIsInInterval      = Standard_True;
        break;
      }
      else if ((aMinCoord >= aLocalMin && aMinCoord <= aLocalMax) && aMaxCoord > aLocalMax)
      {
        aPairSeq(jj).second = aMaxCoord;
        anIsInInterval      = Standard_True;
        break;
      }
    }
    if (!anIsInInterval)
    {
      std::pair<Standard_Real, Standard_Real> anInterval(aMinCoord, aMaxCoord);
      if (!aPairSeq.IsEmpty() && aMaxCoord < aPairSeq(1).first)
        aPairSeq.Prepend(anInterval);
      else
        aPairSeq.Append(anInterval);
    }
  }

  theNumberOfIntervals = aPairSeq.Length();

  if (aPairSeq.Length() == 2)
    theMinCoord = aPairSeq(2).first - thePeriod;
  else
    theMinCoord = aPairSeq(1).first;

  theMaxCoord = aPairSeq(1).second;
  return Standard_True;
}

static void RelocatePCurvesToNewUorigin(
  const TopTools_SequenceOfShape&                          theEdges,
  const TopoDS_Shape&                                      theFirstFace,
  const TopoDS_Face&                                       theRefFace,
  const Standard_Real                                      theCoordTol,
  const Standard_Integer                                   theIndCoord,
  const Standard_Real                                      thePeriod,
  TopTools_IndexedDataMapOfShapeListOfShape&               theVEmap,
  NCollection_DataMap<TopoDS_Shape, Handle(Geom2d_Curve)>& theEdgeNewPCurve,
  TopTools_MapOfShape&                                     theUsedEdges)
{
  TopTools_MapOfShape EdgesOfFirstFace;
  TopExp::MapShapes(theFirstFace, EdgesOfFirstFace);

  for (;;) // walk by contours
  {
    // try to find the start edge that:
    // 1. belongs to outer edges of first face;
    // 2. has minimum U-coord of its start point
    TopoDS_Edge        StartEdge;
    TopAbs_Orientation anOr      = TopAbs_FORWARD;
    Standard_Real      aCoordMin = RealLast();
    for (Standard_Integer ii = 1; ii <= theEdges.Length(); ii++)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(theEdges(ii));
      if (theUsedEdges.Contains(anEdge))
        continue;
      if (EdgesOfFirstFace.Contains(anEdge))
      {
        if (StartEdge.IsNull())
        {
          StartEdge = anEdge;
          BRepAdaptor_Curve2d StartBAcurve(StartEdge, theRefFace);
          Standard_Real       aFirstParam, aLastParam;
          if (StartEdge.Orientation() == TopAbs_FORWARD)
          {
            aFirstParam = StartBAcurve.FirstParameter();
            aLastParam  = StartBAcurve.LastParameter();
          }
          else
          {
            aFirstParam = StartBAcurve.LastParameter();
            aLastParam  = StartBAcurve.FirstParameter();
          }
          gp_Pnt2d aFirstPoint = StartBAcurve.Value(aFirstParam);
          gp_Pnt2d aLastPoint  = StartBAcurve.Value(aLastParam);
          if (aFirstPoint.Coord(theIndCoord) < aLastPoint.Coord(theIndCoord))
          {
            aCoordMin = aFirstPoint.Coord(theIndCoord);
            anOr      = TopAbs_FORWARD;
          }
          else
          {
            aCoordMin = aLastPoint.Coord(theIndCoord);
            anOr      = TopAbs_REVERSED;
          }
        }
        else
        {
          BRepAdaptor_Curve2d aBAcurve(anEdge, theRefFace);
          Standard_Real       aFirstParam, aLastParam;
          if (anEdge.Orientation() == TopAbs_FORWARD)
          {
            aFirstParam = aBAcurve.FirstParameter();
            aLastParam  = aBAcurve.LastParameter();
          }
          else
          {
            aFirstParam = aBAcurve.LastParameter();
            aLastParam  = aBAcurve.FirstParameter();
          }
          gp_Pnt2d aFirstPoint = aBAcurve.Value(aFirstParam);
          gp_Pnt2d aLastPoint  = aBAcurve.Value(aLastParam);
          if (aFirstPoint.Coord(theIndCoord) < aCoordMin)
          {
            StartEdge = anEdge;
            aCoordMin = aFirstPoint.Coord(theIndCoord);
            anOr      = TopAbs_FORWARD;
          }
          if (aLastPoint.Coord(theIndCoord) < aCoordMin)
          {
            StartEdge = anEdge;
            aCoordMin = aLastPoint.Coord(theIndCoord);
            anOr      = TopAbs_REVERSED;
          }
        }
      } // if (EdgesOfFirstFace.Contains(anEdge))
    } // for (Standard_Integer ii = 1; ii <= edges.Length(); ii++)

    if (StartEdge.IsNull()) // all contours are passed
      break;

    TopoDS_Vertex        StartVertex = (anOr == TopAbs_FORWARD)
                                         ? TopExp::FirstVertex(StartEdge, Standard_True)
                                         : TopExp::LastVertex(StartEdge, Standard_True);
    TopoDS_Edge          CurEdge     = StartEdge;
    Standard_Real        fpar, lpar;
    Handle(Geom2d_Curve) CurPCurve = BRep_Tool::CurveOnSurface(CurEdge, theRefFace, fpar, lpar);
    CurPCurve                      = new Geom2d_TrimmedCurve(CurPCurve, fpar, lpar);
    theEdgeNewPCurve.Bind(CurEdge, CurPCurve);
    if (CurEdge.Orientation() == TopAbs_REVERSED)
    {
      Standard_Real tmp = fpar;
      fpar              = lpar;
      lpar              = tmp;
    }
    Standard_Real CurParam = (anOr == TopAbs_FORWARD) ? lpar : fpar;
    gp_Pnt2d      CurPoint = CurPCurve->Value(CurParam);

    for (;;) // collect pcurves of a contour
    {
      RemoveEdgeFromMap(CurEdge, theVEmap);
      theUsedEdges.Add(CurEdge);
      TopoDS_Vertex CurVertex = (anOr == TopAbs_FORWARD)
                                  ? TopExp::LastVertex(CurEdge, Standard_True)
                                  : TopExp::FirstVertex(CurEdge, Standard_True);

      const TopTools_ListOfShape& Elist = theVEmap.FindFromKey(CurVertex);
      if (Elist.IsEmpty())
        break; // end of contour in 3d

      TopTools_ListIteratorOfListOfShape itl(Elist);
      for (; itl.More(); itl.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
        if (anEdge.IsSame(CurEdge))
          continue;
        TopoDS_Vertex aFirstVertex = (anOr == TopAbs_FORWARD)
                                       ? TopExp::FirstVertex(anEdge, Standard_True)
                                       : TopExp::LastVertex(anEdge, Standard_True);
        if (!aFirstVertex.IsSame(CurVertex)) // may be if CurVertex is deg.vertex
          continue;

        Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(anEdge, theRefFace, fpar, lpar);
        aPCurve                      = new Geom2d_TrimmedCurve(aPCurve, fpar, lpar);
        if (anEdge.Orientation() == TopAbs_REVERSED)
        {
          Standard_Real tmp = fpar;
          fpar              = lpar;
          lpar              = tmp;
        }
        Standard_Real aParam   = (anOr == TopAbs_FORWARD) ? fpar : lpar;
        gp_Pnt2d      aPoint   = aPCurve->Value(aParam);
        Standard_Real anOffset = CurPoint.Coord(theIndCoord) - aPoint.Coord(theIndCoord);
        if (!(Abs(anOffset) < theCoordTol || Abs(Abs(anOffset) - thePeriod) < theCoordTol))
          continue; // may be if CurVertex is deg.vertex

        if (Abs(anOffset) > thePeriod / 2)
        {
          anOffset = TrueValueOfOffset(anOffset, thePeriod);
          gp_Vec2d aVec;
          if (theIndCoord == 1)
            aVec.SetCoord(anOffset, 0.);
          else
            aVec.SetCoord(0., anOffset);
          Handle(Geom2d_Curve) aNewPCurve = Handle(Geom2d_Curve)::DownCast(aPCurve->Copy());
          aNewPCurve->Translate(aVec);
          aPCurve = aNewPCurve;
        }
        theEdgeNewPCurve.Bind(anEdge, aPCurve);
        CurEdge                  = anEdge;
        TopAbs_Orientation CurOr = TopAbs::Compose(anOr, CurEdge.Orientation());
        CurParam = (CurOr == TopAbs_FORWARD) ? aPCurve->LastParameter() : aPCurve->FirstParameter();
        CurPoint = aPCurve->Value(CurParam);
        break;
      }
    } // for (;;) (collect pcurves of a contour)
  } // for (;;) (walk by contours)
}

static void InsertWiresIntoFaces(const TopTools_SequenceOfShape& theWires,
                                 const TopTools_SequenceOfShape& theFaces,
                                 const TopoDS_Face&              theRefFace)
{
  BRep_Builder BB;
  for (Standard_Integer ii = 1; ii <= theWires.Length(); ii++)
  {
    const TopoDS_Wire&  aWire = TopoDS::Wire(theWires(ii));
    TopoDS_Iterator     iter(aWire);
    const TopoDS_Edge&  anEdge = TopoDS::Edge(iter.Value());
    BRepAdaptor_Curve2d BAcurve2d(anEdge, theRefFace);
    gp_Pnt2d            aPnt2d =
      BAcurve2d.Value((BAcurve2d.FirstParameter() + BAcurve2d.LastParameter()) / 2.);
    TopoDS_Shape RequiredFace;
    for (Standard_Integer jj = 1; jj <= theFaces.Length(); jj++)
    {
      const TopoDS_Face&      aFace = TopoDS::Face(theFaces(jj));
      BRepTopAdaptor_FClass2d Classifier(aFace, Precision::Confusion());
      TopAbs_State            aStatus = Classifier.Perform(aPnt2d);
      if (aStatus == TopAbs_IN)
      {
        RequiredFace = aFace.Oriented(TopAbs_FORWARD);
        break;
      }
    }
    if (!RequiredFace.IsNull())
    {
      BB.Add(RequiredFace, aWire);
    }
    else
    {
      Standard_ASSERT_INVOKE("ShapeUpgrade_UnifySameDomain: wire remains unclassified");
    }
  }
}

static TopoDS_Face FindCommonFace(const TopoDS_Edge&                               theEdge1,
                                  const TopoDS_Edge&                               theEdge2,
                                  const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap,
                                  TopAbs_Orientation&                              theOrOfE1OnFace,
                                  TopAbs_Orientation&                              theOrOfE2OnFace)
{
  TopoDS_Vertex aVertex;
  TopExp::CommonVertex(theEdge1, theEdge2, aVertex);
  const TopTools_ListOfShape&        Flist = theVFmap.FindFromKey(aVertex);
  TopTools_ListIteratorOfListOfShape itl(Flist);
  for (; itl.More(); itl.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(itl.Value());
    aFace.Orientation(TopAbs_FORWARD);
    Standard_Boolean e1found = Standard_False, e2found = Standard_False;
    TopExp_Explorer  Explo(aFace, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Shape& anEdge = Explo.Current();
      if (anEdge.IsSame(theEdge1))
      {
        e1found         = Standard_True;
        theOrOfE1OnFace = anEdge.Orientation();
      }
      if (anEdge.IsSame(theEdge2))
      {
        e2found         = Standard_True;
        theOrOfE2OnFace = anEdge.Orientation();
      }
      if (e1found && e2found)
        return aFace;
    }
  }

  TopoDS_Face NullFace;
  return NullFace;
}

static Standard_Boolean FindClosestPoints(const TopoDS_Edge&                               theEdge1,
                                          const TopoDS_Edge&                               theEdge2,
                                          const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap,
                                          TopoDS_Face&        theCommonFace,
                                          Standard_Real&      theMinSqDist,
                                          TopAbs_Orientation& OrOfE1OnFace,
                                          TopAbs_Orientation& OrOfE2OnFace,
                                          Standard_Integer&   theIndOnE1,
                                          Standard_Integer&   theIndOnE2,
                                          gp_Pnt2d*           PointsOnEdge1,
                                          gp_Pnt2d*           PointsOnEdge2)

{
  theCommonFace = FindCommonFace(theEdge1, theEdge2, theVFmap, OrOfE1OnFace, OrOfE2OnFace);
  if (theCommonFace.IsNull())
    return Standard_False;

  Standard_Real        fpar1, lpar1, fpar2, lpar2;
  Handle(Geom2d_Curve) PCurve1 = BRep_Tool::CurveOnSurface(theEdge1, theCommonFace, fpar1, lpar1);
  Handle(Geom2d_Curve) PCurve2 = BRep_Tool::CurveOnSurface(theEdge2, theCommonFace, fpar2, lpar2);
  PointsOnEdge1[0]             = PCurve1->Value(fpar1);
  PointsOnEdge1[1]             = PCurve1->Value(lpar1);
  PointsOnEdge2[0]             = PCurve2->Value(fpar2);
  PointsOnEdge2[1]             = PCurve2->Value(lpar2);
  theMinSqDist                 = RealLast();
  theIndOnE1                   = -1;
  theIndOnE2                   = -1;
  for (Standard_Integer ind1 = 0; ind1 < 2; ind1++)
    for (Standard_Integer ind2 = 0; ind2 < 2; ind2++)
    {
      Standard_Real aSqDist = PointsOnEdge1[ind1].SquareDistance(PointsOnEdge2[ind2]);
      if (aSqDist < theMinSqDist)
      {
        theMinSqDist = aSqDist;
        theIndOnE1   = ind1;
        theIndOnE2   = ind2;
      }
    }
  return Standard_True;
}

//=================================================================================================

static void ReconstructMissedSeam(const TopTools_SequenceOfShape& theRemovedEdges,
                                  const TopoDS_Face&              theFrefFace,
                                  const TopoDS_Edge&              theCurEdge,
                                  const TopoDS_Vertex&            theCurVertex,
                                  const gp_Pnt2d&                 theCurPoint,
                                  const Standard_Real             theUperiod,
                                  const Standard_Real             theVperiod,
                                  TopoDS_Edge&                    theSeamEdge,
                                  gp_Pnt2d&                       theNextPoint)
{
  Handle(Geom_Surface) RefSurf = BRep_Tool::Surface(theFrefFace);

  // Find seam edge between removed edges
  theSeamEdge.Nullify();
  for (Standard_Integer i = 1; i <= theRemovedEdges.Length(); i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(theRemovedEdges(i));
    if (anEdge.IsSame(theCurEdge))
      continue;
    Standard_Real        Param1, Param2;
    Handle(Geom2d_Curve) aPC = BRep_Tool::CurveOnSurface(anEdge, theFrefFace, Param1, Param2);
    if (aPC.IsNull())
      continue;

    TopoDS_Vertex aFirstVertex, aLastVertex;
    TopExp::Vertices(anEdge, aFirstVertex, aLastVertex, Standard_True);

    if ((aFirstVertex.IsSame(theCurVertex) || aLastVertex.IsSame(theCurVertex))
        && BRep_Tool::IsClosed(anEdge, theFrefFace))
    {
      Standard_Real aParam = (anEdge.Orientation() == TopAbs_FORWARD) ? Param1 : Param2;
      gp_Pnt2d      aPoint = aPC->Value(aParam);
      Standard_Real aUdiff = Abs(aPoint.X() - theCurPoint.X());
      Standard_Real aVdiff = Abs(aPoint.Y() - theCurPoint.Y());
      if ((theUperiod != 0. && aUdiff > theUperiod / 2)
          || (theVperiod != 0. && aVdiff > theVperiod / 2))
      {
        if (aLastVertex.IsSame(theCurVertex) || (theUperiod != 0. && theVperiod != 0.))
        {
          anEdge.Reverse();
        }
        else
        {
          TopAbs_Orientation anOri = anEdge.Orientation();
          anEdge.Orientation(TopAbs_FORWARD);
          Handle(Geom2d_Curve) aPC1 =
            BRep_Tool::CurveOnSurface(anEdge, theFrefFace, Param1, Param2);
          anEdge.Reverse();
          Handle(Geom2d_Curve) aPC2 =
            BRep_Tool::CurveOnSurface(anEdge, theFrefFace, Param1, Param2);
          anEdge.Reverse(); // again FORWARD
          TopLoc_Location             aLoc;
          BRep_Builder                aBB;
          Standard_Real               aTol  = BRep_Tool::Tolerance(anEdge);
          const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(theFrefFace, aLoc);
          aBB.UpdateEdge(anEdge, aPC2, aPC1, aSurf, aLoc, aTol);
          anEdge.Orientation(anOri);
        }
        aPC    = BRep_Tool::CurveOnSurface(anEdge, theFrefFace, Param1, Param2);
        aParam = (anEdge.Orientation() == TopAbs_FORWARD) ? Param1 : Param2;
        aPoint = aPC->Value(aParam);
        aUdiff = Abs(aPoint.X() - theCurPoint.X());
        aVdiff = Abs(aPoint.Y() - theCurPoint.Y());
      }
      if ((theUperiod == 0. || aUdiff < theUperiod / 2)
          && (theVperiod == 0. || aVdiff < theVperiod / 2))
      {
        aFirstVertex = TopExp::FirstVertex(anEdge, Standard_True);
        if (aFirstVertex.IsSame(theCurVertex))
        {
          theSeamEdge = anEdge;
          break;
        }
      }
    }
  }

  if (!theSeamEdge.IsNull())
  {
    Standard_Real        Param1, Param2;
    Handle(Geom2d_Curve) aPC = BRep_Tool::CurveOnSurface(theSeamEdge, theFrefFace, Param1, Param2);
    Standard_Real        aParam = (theSeamEdge.Orientation() == TopAbs_FORWARD) ? Param2 : Param1;
    theNextPoint                = aPC->Value(aParam);
  }
}

//=================================================================================================

static Standard_Boolean SameSurf(const Handle(Geom_Surface)& theS1,
                                 const Handle(Geom_Surface)& theS2)
{
  static Standard_Real aCoefs[2] = {0.3399811, 0.7745966};

  Standard_Real uf1, ul1, vf1, vl1, uf2, ul2, vf2, vl2;
  theS1->Bounds(uf1, ul1, vf1, vl1);
  theS2->Bounds(uf2, ul2, vf2, vl2);
  constexpr Standard_Real aPTol = Precision::PConfusion();
  if (Precision::IsNegativeInfinite(uf1))
  {
    if (!Precision::IsNegativeInfinite(uf2))
    {
      return Standard_False;
    }
    else
    {
      uf1 = Min(-1., (ul1 - 1.));
    }
  }
  else
  {
    if (Precision::IsNegativeInfinite(uf2))
    {
      return Standard_False;
    }
    else
    {
      if (Abs(uf1 - uf2) > aPTol)
      {
        return Standard_False;
      }
    }
  }
  //
  if (Precision::IsNegativeInfinite(vf1))
  {
    if (!Precision::IsNegativeInfinite(vf2))
    {
      return Standard_False;
    }
    else
    {
      vf1 = Min(-1., (vl1 - 1.));
    }
  }
  else
  {
    if (Precision::IsNegativeInfinite(vf2))
    {
      return Standard_False;
    }
    else
    {
      if (Abs(vf1 - vf2) > aPTol)
      {
        return Standard_False;
      }
    }
  }
  //
  if (Precision::IsPositiveInfinite(ul1))
  {
    if (!Precision::IsPositiveInfinite(ul2))
    {
      return Standard_False;
    }
    else
    {
      ul1 = Max(1., (uf1 + 1.));
    }
  }
  else
  {
    if (Precision::IsPositiveInfinite(ul2))
    {
      return Standard_False;
    }
    else
    {
      if (Abs(ul1 - ul2) > aPTol)
      {
        return Standard_False;
      }
    }
  }
  //
  if (Precision::IsPositiveInfinite(vl1))
  {
    if (!Precision::IsPositiveInfinite(vl2))
    {
      return Standard_False;
    }
    else
    {
      vl1 = Max(1., (vf1 + 1.));
    }
  }
  else
  {
    if (Precision::IsPositiveInfinite(vl2))
    {
      return Standard_False;
    }
    else
    {
      if (Abs(vl1 - vl2) > aPTol)
      {
        return Standard_False;
      }
    }
  }
  //

  Standard_Real    u, v, du = (ul1 - uf1), dv = (vl1 - vf1);
  Standard_Integer i, j;
  for (i = 0; i < 2; ++i)
  {
    u = uf1 + aCoefs[i] * du;
    for (j = 0; j < 2; ++j)
    {
      v          = vf1 + aCoefs[j] * dv;
      gp_Pnt aP1 = theS1->Value(u, v);
      gp_Pnt aP2 = theS2->Value(u, v);
      if (!aP1.IsEqual(aP2, aPTol))
      {
        return Standard_False;
      }
    }
  }

  return Standard_True;
}

//=================================================================================================

static void TransformPCurves(const TopoDS_Face&   theRefFace,
                             const TopoDS_Face&   theFace,
                             TopTools_MapOfShape& theMapEdgesWithTemporaryPCurves)
{
  Handle(Geom_Surface) RefSurf = BRep_Tool::Surface(theRefFace);
  if (RefSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    RefSurf = (Handle(Geom_RectangularTrimmedSurface)::DownCast(RefSurf))->BasisSurface();

  Handle(Geom_Surface) SurfFace = BRep_Tool::Surface(theFace);
  if (SurfFace->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    SurfFace = (Handle(Geom_RectangularTrimmedSurface)::DownCast(SurfFace))->BasisSurface();

  Standard_Boolean ToModify = Standard_False, ToTranslate = Standard_False,
                   ToRotate = Standard_False, X_Reverse = Standard_False,
                   Y_Reverse = Standard_False, ToProject = Standard_False;

  Standard_Real aTranslation = 0., anAngle = 0.;

  // Get axes of surface of face and of surface of RefFace
  Handle(Geom_ElementarySurface) ElemSurfFace = Handle(Geom_ElementarySurface)::DownCast(SurfFace);
  Handle(Geom_ElementarySurface) ElemRefSurf  = Handle(Geom_ElementarySurface)::DownCast(RefSurf);

  if (!ElemSurfFace.IsNull() && !ElemRefSurf.IsNull())
  {
    gp_Ax3 AxisOfSurfFace = ElemSurfFace->Position();
    gp_Ax3 AxisOfRefSurf  = ElemRefSurf->Position();

    gp_Pnt OriginRefSurf = AxisOfRefSurf.Location();

    Standard_Real aParam = ElCLib::LineParameter(AxisOfSurfFace.Axis(), OriginRefSurf);

    if (Abs(aParam) > Precision::PConfusion())
      aTranslation = -aParam;

    gp_Dir VdirSurfFace = AxisOfSurfFace.Direction();
    gp_Dir VdirRefSurf  = AxisOfRefSurf.Direction();
    gp_Dir XdirSurfFace = AxisOfSurfFace.XDirection();
    gp_Dir XdirRefSurf  = AxisOfRefSurf.XDirection();

    gp_Dir CrossProd1 = AxisOfRefSurf.XDirection() ^ AxisOfRefSurf.YDirection();
    gp_Dir CrossProd2 = AxisOfSurfFace.XDirection() ^ AxisOfSurfFace.YDirection();
    if (CrossProd1 * CrossProd2 < 0.)
      X_Reverse = Standard_True;

    Standard_Real ScalProd = VdirSurfFace * VdirRefSurf;
    if (ScalProd < 0.)
      Y_Reverse = Standard_True;

    if (!X_Reverse && !Y_Reverse)
    {
      gp_Dir DirRef = VdirRefSurf;
      if (!AxisOfRefSurf.Direct())
        DirRef.Reverse();
      anAngle = XdirRefSurf.AngleWithRef(XdirSurfFace, DirRef);
    }
    else
      anAngle = XdirRefSurf.Angle(XdirSurfFace);

    ToRotate = (Abs(anAngle) > Precision::PConfusion());

    ToTranslate = (Abs(aTranslation) > Precision::PConfusion());

    ToModify = ToTranslate || ToRotate || X_Reverse || Y_Reverse;
  }
  else
  {
    if (!SameSurf(RefSurf, SurfFace))
    {
      ToProject = Standard_True;
    }
  }

  BRep_Builder        BB;
  TopExp_Explorer     Explo(theFace, TopAbs_EDGE);
  TopTools_MapOfShape aEmap;
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(Explo.Current());
    if (BRep_Tool::Degenerated(anEdge) && ToModify)
      continue;

    if (ToProject && BRep_Tool::IsClosed(anEdge, theFace))
      continue;

    if (!aEmap.Add(anEdge))
      continue;

    TopAbs_Orientation anOr = anEdge.Orientation();

    Standard_Real        fpar, lpar;
    Handle(Geom2d_Curve) PCurveOnRef = BRep_Tool::CurveOnSurface(anEdge, theRefFace, fpar, lpar);
    Handle(Geom2d_Curve) PCurve2;
    if (!PCurveOnRef.IsNull() /* && !(ToModify || ToProject)*/)
    {
      anEdge.Reverse();
      PCurve2 = BRep_Tool::CurveOnSurface(anEdge, theRefFace, fpar, lpar);
      if (PCurve2 != PCurveOnRef) // two pcurves
        continue;
    }

    Handle(Geom2d_Curve) PCurves[2], NewPCurves[2];
    anEdge.Orientation(TopAbs_FORWARD);
    PCurves[0] = BRep_Tool::CurveOnSurface(anEdge, theFace, fpar, lpar);
    anEdge.Reverse();
    PCurves[1] = BRep_Tool::CurveOnSurface(anEdge, theFace, fpar, lpar);

    Standard_Integer NbPcurves = (PCurves[0] == PCurves[1]) ? 1 : 2;

    for (Standard_Integer ii = 0; ii < NbPcurves; ii++)
    {
      if (ToProject)
      {
        Handle(Geom_Curve) aC3d = BRep_Tool::Curve(anEdge, fpar, lpar);
        aC3d                    = new Geom_TrimmedCurve(aC3d, fpar, lpar);
        Standard_Real tol       = BRep_Tool::Tolerance(anEdge);
        tol                     = Min(tol, Precision::Approximation());
        NewPCurves[ii]          = GeomProjLib::Curve2d(aC3d, RefSurf);
      }
      else
      {
        NewPCurves[ii] = Handle(Geom2d_Curve)::DownCast(PCurves[ii]->Copy());
      }
      if (ToTranslate)
        NewPCurves[ii]->Translate(gp_Vec2d(0., aTranslation));
      if (Y_Reverse)
        NewPCurves[ii]->Mirror(gp::OX2d());
      if (X_Reverse)
      {
        NewPCurves[ii]->Mirror(gp::OY2d());
        NewPCurves[ii]->Translate(gp_Vec2d(2 * M_PI, 0.));
      }
      if (ToRotate)
        NewPCurves[ii]->Translate(gp_Vec2d(anAngle, 0.));
    }

    anEdge.Orientation(TopAbs_FORWARD);

    if (NbPcurves == 1)
    {
      if (PCurve2.IsNull() || (!RefSurf->IsUClosed() && !RefSurf->IsVClosed()))
      {
        BB.UpdateEdge(anEdge, NewPCurves[0], theRefFace, 0.);
        theMapEdgesWithTemporaryPCurves.Add(anEdge);
      }
      else
      {
        // check: may be it is the same pcurve
        Standard_Real aUmin, aUmax, aVmin, aVmax;
        RefSurf->Bounds(aUmin, aUmax, aVmin, aVmax);
        Standard_Real aUperiod      = (RefSurf->IsUClosed()) ? (aUmax - aUmin) : 0.;
        Standard_Real aVperiod      = (RefSurf->IsVClosed()) ? (aVmax - aVmin) : 0.;
        gp_Pnt2d      aP2dOnPCurve1 = PCurveOnRef->Value(fpar);
        gp_Pnt2d      aP2dOnPCurve2 = NewPCurves[0]->Value(fpar);
        if ((aUperiod != 0. && Abs(aP2dOnPCurve1.X() - aP2dOnPCurve2.X()) > aUperiod / 2)
            || (aVperiod != 0. && Abs(aP2dOnPCurve1.Y() - aP2dOnPCurve2.Y()) > aVperiod / 2))
        {
          Handle(Geom2d_Curve) NullPCurve;
          BB.UpdateEdge(anEdge, NullPCurve, theRefFace, 0.);
          if (anOr == TopAbs_FORWARD)
            BB.UpdateEdge(anEdge, NewPCurves[0], PCurveOnRef, theRefFace, 0.);
          else
            BB.UpdateEdge(anEdge, PCurveOnRef, NewPCurves[0], theRefFace, 0.);

          theMapEdgesWithTemporaryPCurves.Add(anEdge);
        }
      }
    }
    else
    {
      BB.UpdateEdge(anEdge, NewPCurves[0], NewPCurves[1], theRefFace, 0.);
      theMapEdgesWithTemporaryPCurves.Add(anEdge);
    }

    BB.Range(anEdge, fpar, lpar);
  }
}

//=================================================================================================

static void AddPCurves(const TopTools_SequenceOfShape& theFaces,
                       const TopoDS_Face&              theRefFace,
                       TopTools_MapOfShape&            theMapEdgesWithTemporaryPCurves)
{
  BRepAdaptor_Surface RefBAsurf(theRefFace, Standard_False);

  GeomAbs_SurfaceType aType = RefBAsurf.GetType();
  if (aType == GeomAbs_Plane)
    return;

  for (Standard_Integer i = 1; i <= theFaces.Length(); i++)
  {
    TopoDS_Face aFace = TopoDS::Face(theFaces(i));
    aFace.Orientation(TopAbs_FORWARD);
    if (aFace.IsSame(theRefFace))
      continue;

    TransformPCurves(theRefFace, aFace, theMapEdgesWithTemporaryPCurves);
  }
}

//=================================================================================================

// adds edges from the shape to the sequence
// seams and equal edges are dropped
// Returns true if one of original edges dropped
static Standard_Boolean AddOrdinaryEdges(TopTools_SequenceOfShape& edges,
                                         const TopoDS_Shape&       aShape,
                                         Standard_Integer&         anIndex,
                                         TopTools_SequenceOfShape& theRemovedEdges)
{
  // map of edges
  TopTools_IndexedMapOfShape aNewEdges;
  // add edges without seams
  for (TopExp_Explorer exp(aShape, TopAbs_EDGE); exp.More(); exp.Next())
  {
    const TopoDS_Shape& edge = exp.Current();
    if (aNewEdges.Contains(edge))
    {
      aNewEdges.RemoveKey(edge);
      theRemovedEdges.Append(edge);
    }
    else
      aNewEdges.Add(edge);
  }

  Standard_Boolean isDropped = Standard_False;
  // merge edges and drop seams
  Standard_Integer i;
  for (i = 1; i <= edges.Length(); i++)
  {
    TopoDS_Shape current = edges(i);
    if (aNewEdges.Contains(current))
    {

      aNewEdges.RemoveKey(current);
      edges.Remove(i);
      theRemovedEdges.Append(current);
      i--;

      if (!isDropped)
      {
        isDropped = Standard_True;
        anIndex   = i;
      }
    }
  }

  // add edges to the sequence
  for (i = 1; i <= aNewEdges.Extent(); i++)
    edges.Append(aNewEdges(i));

  return isDropped;
}

//=================================================================================================

static Standard_Boolean getCylinder(Handle(Geom_Surface)& theInSurface, gp_Cylinder& theOutCylinder)
{
  Standard_Boolean isCylinder = Standard_False;

  if (theInSurface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
  {
    Handle(Geom_CylindricalSurface) aGC = Handle(Geom_CylindricalSurface)::DownCast(theInSurface);

    theOutCylinder = aGC->Cylinder();
    isCylinder     = Standard_True;
  }
  else if (theInSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    Handle(Geom_SurfaceOfRevolution) aRS = Handle(Geom_SurfaceOfRevolution)::DownCast(theInSurface);
    Handle(Geom_Curve)               aBasis = aRS->BasisCurve();
    if (aBasis->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      Handle(Geom_Line) aBasisLine = Handle(Geom_Line)::DownCast(aBasis);
      gp_Dir            aDir       = aRS->Direction();
      gp_Dir            aBasisDir  = aBasisLine->Position().Direction();
      if (aBasisDir.IsParallel(aDir, Precision::Angular()))
      {
        // basis line is parallel to the revolution axis: it is a cylinder
        gp_Pnt        aLoc = aRS->Location();
        Standard_Real aR   = aBasisLine->Lin().Distance(aLoc);
        gp_Ax3        aCylAx(aLoc, aDir);

        theOutCylinder = gp_Cylinder(aCylAx, aR);
        isCylinder     = Standard_True;
      }
    }
  }
  else if (theInSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
  {
    Handle(Geom_SurfaceOfLinearExtrusion) aLES =
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theInSurface);
    Handle(Geom_Curve) aBasis = aLES->BasisCurve();
    if (aBasis->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      Handle(Geom_Circle) aBasisCircle = Handle(Geom_Circle)::DownCast(aBasis);
      gp_Dir              aDir         = aLES->Direction();
      gp_Dir              aBasisDir    = aBasisCircle->Position().Direction();
      if (aBasisDir.IsParallel(aDir, Precision::Angular()))
      {
        // basis circle is normal to the extrusion axis: it is a cylinder
        gp_Pnt        aLoc = aBasisCircle->Location();
        Standard_Real aR   = aBasisCircle->Radius();
        gp_Ax3        aCylAx(aLoc, aDir);

        theOutCylinder = gp_Cylinder(aCylAx, aR);
        isCylinder     = Standard_True;
      }
    }
  }
  else
  {
  }

  return isCylinder;
}

//=================================================================================================

static Handle(Geom_Surface) ClearRts(const Handle(Geom_Surface)& aSurface)
{
  if (aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    Handle(Geom_RectangularTrimmedSurface) rts =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    return rts->BasisSurface();
  }
  return aSurface;
}

//=======================================================================
// function : GetNormalToSurface
// purpose  : Gets the normal to surface by the given parameter on edge.
//           Returns True if normal was computed.
//=======================================================================
static Standard_Boolean GetNormalToSurface(const TopoDS_Face&  theFace,
                                           const TopoDS_Edge&  theEdge,
                                           const Standard_Real theP,
                                           gp_Dir&             theNormal)
{
  Standard_Real f, l;
  // get 2d curve to get point in 2d
  Handle(Geom2d_Curve) aC2d;
  if (BRep_Tool::IsClosed(theEdge, theFace))
  {
    // Find the edge in the face: it will have correct orientation
    TopoDS_Edge anEdgeInFace;
    TopoDS_Face aFace = theFace;
    aFace.Orientation(TopAbs_FORWARD);
    TopExp_Explorer anExplo(aFace, TopAbs_EDGE);
    for (; anExplo.More(); anExplo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anExplo.Current());
      if (anEdge.IsSame(theEdge))
      {
        anEdgeInFace = anEdge;
        break;
      }
    }
    if (anEdgeInFace.IsNull())
      return Standard_False;

    aC2d = BRep_Tool::CurveOnSurface(anEdgeInFace, aFace, f, l);
  }
  else
    aC2d = BRep_Tool::CurveOnSurface(theEdge, theFace, f, l);

  if (aC2d.IsNull())
  {
    return Standard_False;
  }
  //
  // 2d point
  gp_Pnt2d aP2d;
  aC2d->D0(theP, aP2d);
  //
  // get D1
  gp_Vec                      aDU, aDV;
  gp_Pnt                      aP3d;
  TopLoc_Location             aLoc;
  const Handle(Geom_Surface)& aS = BRep_Tool::Surface(theFace, aLoc);
  aS->D1(aP2d.X(), aP2d.Y(), aP3d, aDU, aDV);
  //
  // compute normal
  gp_Vec aVNormal = aDU.Crossed(aDV);
  if (aVNormal.Magnitude() < Precision::Confusion())
  {
    return Standard_False;
  }
  //
  if (theFace.Orientation() == TopAbs_REVERSED)
  {
    aVNormal.Reverse();
  }
  //
  aVNormal.Transform(aLoc.Transformation());
  theNormal = gp_Dir(aVNormal);
  return Standard_True;
}

//=================================================================================================

static Standard_Boolean IsSameDomain(
  const TopoDS_Face&                                aFace,
  const TopoDS_Face&                                aCheckedFace,
  const Standard_Real                               theLinTol,
  const Standard_Real                               theAngTol,
  ShapeUpgrade_UnifySameDomain::DataMapOfFacePlane& theFacePlaneMap)
{
  // checking the same handles
  TopLoc_Location      L1, L2;
  Handle(Geom_Surface) S1, S2;

  S1 = BRep_Tool::Surface(aFace, L1);
  S2 = BRep_Tool::Surface(aCheckedFace, L2);

  if (S1 == S2 && L1 == L2)
    return Standard_True;

  S1 = BRep_Tool::Surface(aFace);
  S2 = BRep_Tool::Surface(aCheckedFace);

  S1 = ClearRts(S1);
  S2 = ClearRts(S2);

  // Handle(Geom_OffsetSurface) aGOFS1, aGOFS2;
  // aGOFS1 = Handle(Geom_OffsetSurface)::DownCast(S1);
  // aGOFS2 = Handle(Geom_OffsetSurface)::DownCast(S2);
  // if (!aGOFS1.IsNull()) S1 = aGOFS1->BasisSurface();
  // if (!aGOFS2.IsNull()) S2 = aGOFS2->BasisSurface();

  // case of two planar surfaces:
  // all kinds of surfaces checked, including b-spline and bezier
  GeomLib_IsPlanarSurface aPlanarityChecker1(S1, theLinTol);
  if (aPlanarityChecker1.IsPlanar())
  {
    GeomLib_IsPlanarSurface aPlanarityChecker2(S2, theLinTol);
    if (aPlanarityChecker2.IsPlanar())
    {
      gp_Pln aPln1 = aPlanarityChecker1.Plan();
      gp_Pln aPln2 = aPlanarityChecker2.Plan();

      if (aPln1.Position().Direction().IsParallel(aPln2.Position().Direction(), theAngTol)
          && aPln1.Distance(aPln2) < theLinTol)
      {
        Handle(Geom_Plane) aPlaneOfFaces;
        if (theFacePlaneMap.IsBound(aFace))
          aPlaneOfFaces = theFacePlaneMap(aFace);
        else if (theFacePlaneMap.IsBound(aCheckedFace))
          aPlaneOfFaces = theFacePlaneMap(aCheckedFace);
        else
          aPlaneOfFaces = new Geom_Plane(aPln1);

        theFacePlaneMap.Bind(aFace, aPlaneOfFaces);
        theFacePlaneMap.Bind(aCheckedFace, aPlaneOfFaces);

        return Standard_True;
      }
    }
  }

  // case of two elementary surfaces: use OCCT tool
  // elementary surfaces: ConicalSurface, CylindricalSurface,
  //                      Plane, SphericalSurface and ToroidalSurface
  if (S1->IsKind(STANDARD_TYPE(Geom_ElementarySurface))
      && S2->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
  {
    Handle(GeomAdaptor_Surface) aGA1 = new GeomAdaptor_Surface(S1);
    Handle(GeomAdaptor_Surface) aGA2 = new GeomAdaptor_Surface(S2);

    Handle(BRepTopAdaptor_TopolTool) aTT1 = new BRepTopAdaptor_TopolTool();
    Handle(BRepTopAdaptor_TopolTool) aTT2 = new BRepTopAdaptor_TopolTool();

    try
    {
      IntPatch_ImpImpIntersection anIIInt(aGA1, aTT1, aGA2, aTT2, theLinTol, theLinTol);
      if (!anIIInt.IsDone() || anIIInt.IsEmpty())
        return Standard_False;

      return anIIInt.TangentFaces();
    }
    catch (Standard_Failure const&)
    {
      return Standard_False;
    }
  }

  // case of two cylindrical surfaces, at least one of which is a swept surface
  // swept surfaces: SurfaceOfLinearExtrusion, SurfaceOfRevolution
  if ((S1->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
       || S1->IsKind(STANDARD_TYPE(Geom_SweptSurface)))
      && (S2->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
          || S2->IsKind(STANDARD_TYPE(Geom_SweptSurface))))
  {
    gp_Cylinder aCyl1, aCyl2;
    if (getCylinder(S1, aCyl1) && getCylinder(S2, aCyl2))
    {
      if (fabs(aCyl1.Radius() - aCyl2.Radius()) < theLinTol)
      {
        gp_Dir aDir1 = aCyl1.Position().Direction();
        gp_Dir aDir2 = aCyl2.Position().Direction();
        if (aDir1.IsParallel(aDir2, Precision::Angular()))
        {
          gp_Pnt aLoc1 = aCyl1.Location();
          gp_Pnt aLoc2 = aCyl2.Location();
          gp_Vec aVec12(aLoc1, aLoc2);
          if (aVec12.SquareMagnitude() < theLinTol * theLinTol
              || aVec12.IsParallel(aDir1, Precision::Angular()))
          {
            return Standard_True;
          }
        }
      }
    }
  }

  return Standard_False;
}

//=================================================================================================

static void UpdateMapOfShapes(TopTools_MapOfShape&        theMapOfShapes,
                              Handle(ShapeBuild_ReShape)& theContext)
{
  for (TopTools_MapIteratorOfMapOfShape it(theMapOfShapes); it.More(); it.Next())
  {
    const TopoDS_Shape& aShape        = it.Value();
    TopoDS_Shape        aContextShape = theContext->Apply(aShape);
    if (!aContextShape.IsSame(aShape))
      theMapOfShapes.Add(aContextShape);
  }
}

//=======================================================================
// function : GlueEdgesWithPCurves
// purpose  : Glues the pcurves of the sequence of edges
//           and glues their 3d curves
//=======================================================================
static TopoDS_Edge GlueEdgesWithPCurves(const TopTools_SequenceOfShape& aChain,
                                        const TopoDS_Vertex&            FirstVertex,
                                        const TopoDS_Vertex&            LastVertex)
{
  Standard_Integer i, j;

  TopoDS_Edge                           FirstEdge = TopoDS::Edge(aChain(1));
  TColGeom_SequenceOfSurface            SurfSeq;
  NCollection_Sequence<TopLoc_Location> LocSeq;

  for (int aCurveIndex = 0;; aCurveIndex++)
  {
    Handle(Geom2d_Curve) aCurve;
    Handle(Geom_Surface) aSurface;
    TopLoc_Location      aLocation;
    Standard_Real        aFirst, aLast;
    BRep_Tool::CurveOnSurface(FirstEdge, aCurve, aSurface, aLocation, aFirst, aLast, aCurveIndex);
    if (aCurve.IsNull())
      break;

    SurfSeq.Append(aSurface);
    LocSeq.Append(aLocation);
  }

  Standard_Real fpar, lpar;
  BRep_Tool::Range(FirstEdge, fpar, lpar);
  TopoDS_Edge   PrevEdge = FirstEdge;
  TopoDS_Vertex CV;
  Standard_Real MaxTol = 0.;

  TopoDS_Edge  ResEdge;
  BRep_Builder BB;

  Standard_Integer              nb_curve = aChain.Length(); // number of curves
  TColGeom_Array1OfBSplineCurve tab_c3d(0, nb_curve - 1);   // array of the curves
  TColStd_Array1OfReal tabtolvertex(0, nb_curve - 1); //(0,nb_curve-2);  //array of the tolerances

  TopoDS_Vertex PrevVertex = FirstVertex;
  for (i = 1; i <= nb_curve; i++)
  {
    TopoDS_Edge   anEdge = TopoDS::Edge(aChain(i));
    TopoDS_Vertex VF, VL;
    TopExp::Vertices(anEdge, VF, VL);
    Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));

    Standard_Real Tol1 = BRep_Tool::Tolerance(VF);
    Standard_Real Tol2 = BRep_Tool::Tolerance(VL);
    if (Tol1 > MaxTol)
      MaxTol = Tol1;
    if (Tol2 > MaxTol)
      MaxTol = Tol2;

    if (i > 1)
    {
      TopExp::CommonVertex(PrevEdge, anEdge, CV);
      Standard_Real Tol   = BRep_Tool::Tolerance(CV);
      tabtolvertex(i - 2) = Tol;
    }

    Handle(Geom_Curve)        aCurve   = BRep_Tool::Curve(anEdge, fpar, lpar);
    Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve, fpar, lpar);
    tab_c3d(i - 1)                     = GeomConvert::CurveToBSplineCurve(aTrCurve);
    GeomConvert::C0BSplineToC1BSplineCurve(tab_c3d(i - 1), Precision::Confusion());
    if (ToReverse)
      tab_c3d(i - 1)->Reverse();
    PrevVertex = (ToReverse) ? VF : VL;
    PrevEdge   = anEdge;
  }
  Handle(TColGeom_HArray1OfBSplineCurve) concatcurve;    // array of the concatenated curves
  Handle(TColStd_HArray1OfInteger)       ArrayOfIndices; // array of the remaining Vertex
  Standard_Boolean                       closed_flag = Standard_False;
  GeomConvert::ConcatC1(tab_c3d,
                        tabtolvertex,
                        ArrayOfIndices,
                        concatcurve,
                        closed_flag,
                        Precision::Confusion()); // C1 concatenation

  if (concatcurve->Length() > 1)
  {
    GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));

    for (i = concatcurve->Lower() + 1; i <= concatcurve->Upper(); i++)
      Concat.Add(concatcurve->Value(i), MaxTol, Standard_True);

    concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
  }
  Handle(Geom_BSplineCurve) ResCurve = concatcurve->Value(concatcurve->Lower());

  TColGeom2d_SequenceOfBoundedCurve ResPCurves;
  for (j = 1; j <= SurfSeq.Length(); j++)
  {
    TColGeom2d_Array1OfBSplineCurve tab_c2d(0, nb_curve - 1); // array of the pcurves

    PrevVertex = FirstVertex;
    PrevEdge   = FirstEdge;
    for (i = 1; i <= nb_curve; i++)
    {
      TopoDS_Edge   anEdge = TopoDS::Edge(aChain(i));
      TopoDS_Vertex VF, VL;
      TopExp::Vertices(anEdge, VF, VL);
      Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));

      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, SurfSeq(j), LocSeq(j), fpar, lpar);
      if (aPCurve.IsNull())
        continue;
      Handle(Geom2d_TrimmedCurve) aTrPCurve = new Geom2d_TrimmedCurve(aPCurve, fpar, lpar);
      tab_c2d(i - 1)                        = Geom2dConvert::CurveToBSplineCurve(aTrPCurve);
      Geom2dConvert::C0BSplineToC1BSplineCurve(tab_c2d(i - 1), Precision::Confusion());
      if (ToReverse)
        tab_c2d(i - 1)->Reverse();
      PrevVertex = (ToReverse) ? VF : VL;
      PrevEdge   = anEdge;
    }
    Handle(TColGeom2d_HArray1OfBSplineCurve) concatc2d;    // array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)         ArrayOfInd2d; // array of the remaining Vertex
    closed_flag = Standard_False;
    Geom2dConvert::ConcatC1(tab_c2d,
                            tabtolvertex,
                            ArrayOfInd2d,
                            concatc2d,
                            closed_flag,
                            Precision::Confusion()); // C1 concatenation

    if (concatc2d->Length() > 1)
    {
      Geom2dConvert_CompCurveToBSplineCurve Concat2d(concatc2d->Value(concatc2d->Lower()));

      for (i = concatc2d->Lower() + 1; i <= concatc2d->Upper(); i++)
        Concat2d.Add(concatc2d->Value(i), MaxTol, Standard_True);

      concatc2d->SetValue(concatc2d->Lower(), Concat2d.BSplineCurve());
    }
    Handle(Geom2d_BSplineCurve) aResPCurve = concatc2d->Value(concatc2d->Lower());
    ResPCurves.Append(aResPCurve);
  }

  ResEdge = BRepLib_MakeEdge(ResCurve,
                             FirstVertex,
                             LastVertex,
                             ResCurve->FirstParameter(),
                             ResCurve->LastParameter());
  BB.SameRange(ResEdge, Standard_False);
  BB.SameParameter(ResEdge, Standard_False);
  for (j = 1; j <= ResPCurves.Length(); j++)
  {
    BB.UpdateEdge(ResEdge, ResPCurves(j), SurfSeq(j), LocSeq(j), MaxTol);
    BB.Range(ResEdge,
             SurfSeq(j),
             LocSeq(j),
             ResPCurves(j)->FirstParameter(),
             ResPCurves(j)->LastParameter());
  }

  BRepLib::SameParameter(ResEdge, MaxTol, Standard_True);

  return ResEdge;
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::UnionPCurves(const TopTools_SequenceOfShape& theChain,
                                                TopoDS_Edge&                    theEdge)
{
  Standard_Real      aFirst3d, aLast3d;
  Handle(Geom_Curve) aCurve   = BRep_Tool::Curve(theEdge, aFirst3d, aLast3d);
  Standard_Real      aTolEdge = BRep_Tool::Tolerance(theEdge);
  Standard_Real      aMaxTol  = aTolEdge;

  TopTools_SequenceOfShape aFaceSeq;

  const TopoDS_Edge&                 aFirstEdge = TopoDS::Edge(theChain.Value(1));
  const TopTools_ListOfShape&        aFaceList  = myEFmap.FindFromKey(aFirstEdge);
  TopTools_ListIteratorOfListOfShape anItl(aFaceList);
  for (; anItl.More(); anItl.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(anItl.Value());
    if (myFacePlaneMap.IsBound(aFace))
      continue;

    if (myFaceNewFace.IsBound(aFace))
      aFace = TopoDS::Face(myFaceNewFace(aFace));

    aFace.Orientation(TopAbs_FORWARD); // to get proper pcurves of seam edges

    BRepAdaptor_Surface aBAsurf(aFace, Standard_False);
    if (aBAsurf.GetType() == GeomAbs_Plane)
      continue;

    TopLoc_Location      aLoc;
    Handle(Geom_Surface) aSurf   = BRep_Tool::Surface(aFace, aLoc);
    Standard_Boolean     isFound = Standard_False;
    for (Standard_Integer ii = 1; ii <= aFaceSeq.Length(); ii++)
    {
      TopLoc_Location      aPrevLoc;
      Handle(Geom_Surface) aPrevSurf = BRep_Tool::Surface(TopoDS::Face(aFaceSeq(ii)), aPrevLoc);
      if (aPrevSurf == aSurf && aPrevLoc == aLoc)
      {
        isFound = Standard_True;
        break;
      }
    }
    if (isFound)
      continue;

    aFaceSeq.Append(aFace);
  }

  TColGeom2d_SequenceOfCurve ResPCurves;
  TColStd_SequenceOfReal     ResFirsts;
  TColStd_SequenceOfReal     ResLasts;
  TColStd_SequenceOfReal     aTolVerSeq;
  TopoDS_Edge                aPrevEdge;
  Standard_Boolean           anIsSeam = Standard_False;

  for (Standard_Integer j = 1; j <= aFaceSeq.Length(); j++)
  {
    TColGeom2d_SequenceOfCurve aPCurveSeq;
    TColStd_SequenceOfReal     aFirstsSeq;
    TColStd_SequenceOfReal     aLastsSeq;
    TColStd_SequenceOfBoolean  aForwardsSeq;
    GeomAbs_CurveType          aCurrentType = GeomAbs_OtherCurve;

    Standard_Real aFirst, aLast;
    for (Standard_Integer i = 1; i <= theChain.Length(); i++)
    {
      TopoDS_Edge      anEdge    = TopoDS::Edge(theChain.Value(i));
      Standard_Boolean isForward = (anEdge.Orientation() != TopAbs_REVERSED);

      if (anIsSeam && j == 2) // second pass
        anEdge.Reverse();

      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, TopoDS::Face(aFaceSeq(j)), aFirst, aLast);
      if (aPCurve.IsNull())
        continue;

      if (aFaceSeq.Length() == 1)
      {
        TopoDS_Edge aReversedEdge = anEdge;
        aReversedEdge.Reverse();
        Handle(Geom2d_Curve) aPCurve2 =
          BRep_Tool::CurveOnSurface(aReversedEdge, TopoDS::Face(aFaceSeq(j)), aFirst, aLast);
        if (aPCurve != aPCurve2)
        {
          anIsSeam = Standard_True;
          aFaceSeq.Append(aFaceSeq(j));
        }
      }

      Geom2dAdaptor_Curve anAdaptor(aPCurve);
      GeomAbs_CurveType   aType = anAdaptor.GetType();

      Handle(Geom2d_Line) aLine;
      if (aType == GeomAbs_BSplineCurve || aType == GeomAbs_BezierCurve)
        TryMakeLine(aPCurve, aFirst, aLast, aLine);
      if (!aLine.IsNull())
      {
        aPCurve = aLine;
        anAdaptor.Load(aPCurve);
        aType = GeomAbs_Line;
      }

      if (aPCurveSeq.IsEmpty())
      {
        Handle(Geom2d_Curve) aCopyPCurve = Handle(Geom2d_Curve)::DownCast(aPCurve->Copy());
        if (aCopyPCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
          aCopyPCurve = (Handle(Geom2d_TrimmedCurve)::DownCast(aCopyPCurve))->BasisCurve();

        aPCurveSeq.Append(aCopyPCurve);
        aFirstsSeq.Append(aFirst);
        aLastsSeq.Append(aLast);
        aForwardsSeq.Append(isForward);
        aCurrentType = aType;
        aPrevEdge    = anEdge;
        continue;
      }

      Standard_Boolean isSameCurve = Standard_False;
      Standard_Real    aNewF       = aFirst;
      Standard_Real    aNewL       = aLast;
      if (aPCurve == aPCurveSeq.Last())
      {
        isSameCurve = Standard_True;
      }
      else if (aType == aCurrentType)
      {
        Geom2dAdaptor_Curve aPrevAdaptor(aPCurveSeq.Last());
        switch (aType)
        {
          case GeomAbs_Line: {
            gp_Lin2d aPrevLin  = aPrevAdaptor.Line();
            gp_Pnt2d aFirstP2d = aPCurve->Value(aFirst);
            gp_Pnt2d aLastP2d  = aPCurve->Value(aLast);
            if (aPrevLin.Contains(aFirstP2d, Precision::Confusion())
                && aPrevLin.Contains(aLastP2d, Precision::Confusion()))
            {
              isSameCurve = Standard_True;
              gp_Pnt2d p1 = anAdaptor.Value(aFirst);
              gp_Pnt2d p2 = anAdaptor.Value(aLast);
              aNewF       = ElCLib::Parameter(aPrevLin, p1);
              aNewL       = ElCLib::Parameter(aPrevLin, p2);
              if (aNewF > aNewL)
              {
                Standard_Real aTmp = aNewF;
                aNewF              = aNewL;
                aNewL              = aTmp;
              }
            }
            break;
          }
          case GeomAbs_Circle: {
            gp_Circ2d aCirc     = anAdaptor.Circle();
            gp_Circ2d aPrevCirc = aPrevAdaptor.Circle();
            if (aCirc.Location().Distance(aPrevCirc.Location()) <= Precision::Confusion()
                && Abs(aCirc.Radius() - aPrevCirc.Radius()) <= Precision::Confusion())
            {
              isSameCurve = Standard_True;
              gp_Pnt2d p1 = anAdaptor.Value(aFirst);
              gp_Pnt2d p2 = anAdaptor.Value(aLast);
              aNewF       = ElCLib::Parameter(aPrevCirc, p1);
              aNewL       = ElCLib::Parameter(aPrevCirc, p2);
              if (aNewF > aNewL)
              {
                Standard_Real aTmp = aNewF;
                aNewF              = aNewL;
                aNewL              = aTmp;
              }
            }
            break;
          }
          default:
            break;
        }
      }
      if (isSameCurve)
      {
        if (aForwardsSeq.Last() == Standard_True)
          aLastsSeq.ChangeLast() = aNewL;
        else
          aFirstsSeq.ChangeLast() = aNewF;
      }
      else
      {
        Handle(Geom2d_Curve) aCopyPCurve = Handle(Geom2d_Curve)::DownCast(aPCurve->Copy());
        if (aCopyPCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
          aCopyPCurve = (Handle(Geom2d_TrimmedCurve)::DownCast(aCopyPCurve))->BasisCurve();

        aPCurveSeq.Append(aCopyPCurve);
        aFirstsSeq.Append(aFirst);
        aLastsSeq.Append(aLast);
        aForwardsSeq.Append(isForward);
        aCurrentType = aType;
        TopoDS_Vertex aV;
        TopExp::CommonVertex(aPrevEdge, anEdge, aV);
        Standard_Real aTol = BRep_Tool::Tolerance(aV);
        aTolVerSeq.Append(aTol);
      }
      aPrevEdge = anEdge;
    }

    Handle(Geom2d_Curve) aResPCurve;
    Standard_Real        aResFirst, aResLast;
    if (aPCurveSeq.Length() == 1)
    {
      aResPCurve = aPCurveSeq.Last();
      aResFirst  = aFirstsSeq.Last();
      aResLast   = aLastsSeq.Last();
      if (aForwardsSeq.Last() == Standard_False)
      {
        Standard_Real aNewLast  = aResPCurve->ReversedParameter(aResFirst);
        Standard_Real aNewFirst = aResPCurve->ReversedParameter(aResLast);
        aResPCurve->Reverse();
        aResFirst = aNewFirst;
        aResLast  = aNewLast;
      }
    }
    else
    {
      // C1 concatenation for PCurveSeq
      TColGeom2d_Array1OfBSplineCurve tab_c2d(0, aPCurveSeq.Length() - 1);
      for (Standard_Integer i = 1; i <= aPCurveSeq.Length(); i++)
      {
        Handle(Geom2d_TrimmedCurve) aTrPCurve =
          new Geom2d_TrimmedCurve(aPCurveSeq(i), aFirstsSeq(i), aLastsSeq(i));
        if (aForwardsSeq(i) == Standard_False)
        {
          aTrPCurve->Reverse();
        }
        tab_c2d(i - 1) = Geom2dConvert::CurveToBSplineCurve(aTrPCurve);
        Geom2dConvert::C0BSplineToC1BSplineCurve(tab_c2d(i - 1), Precision::Confusion());
      }

      TColStd_Array1OfReal tabtolvertex(0, aTolVerSeq.Length() - 1);
      for (Standard_Integer i = 1; i <= aTolVerSeq.Length(); i++)
      {
        Standard_Real aTol  = aTolVerSeq(i);
        tabtolvertex(i - 1) = aTol;
        if (aTol > aMaxTol)
          aMaxTol = aTol;
      }

      Handle(TColGeom2d_HArray1OfBSplineCurve) concatc2d;    // array of the concatenated curves
      Handle(TColStd_HArray1OfInteger)         ArrayOfInd2d; // array of the remaining Vertex
      Standard_Boolean                         aClosedFlag = Standard_False;
      Geom2dConvert::ConcatC1(tab_c2d,
                              tabtolvertex,
                              ArrayOfInd2d,
                              concatc2d,
                              aClosedFlag,
                              Precision::Confusion()); // C1 concatenation

      if (concatc2d->Length() > 1)
      {
        Geom2dConvert_CompCurveToBSplineCurve Concat2d(concatc2d->Value(concatc2d->Lower()));

        for (Standard_Integer i = concatc2d->Lower() + 1; i <= concatc2d->Upper(); i++)
          Concat2d.Add(concatc2d->Value(i), aMaxTol, Standard_True);

        concatc2d->SetValue(concatc2d->Lower(), Concat2d.BSplineCurve());
      }
      Handle(Geom2d_BSplineCurve) aBSplineCurve = concatc2d->Value(concatc2d->Lower());
      aResPCurve                                = aBSplineCurve;
      aResFirst                                 = aBSplineCurve->FirstParameter();
      aResLast                                  = aBSplineCurve->LastParameter();
    }
    ResPCurves.Append(aResPCurve);
    ResFirsts.Append(aResFirst);
    ResLasts.Append(aResLast);
  }

  BRep_Builder aBuilder;

  // Check the results for consistency
  Standard_Boolean IsBadRange = Standard_False;
  Standard_Real    aRange3d   = aLast3d - aFirst3d;
  for (Standard_Integer ii = 1; ii <= ResPCurves.Length(); ii++)
  {
    Standard_Real aRange = ResLasts(ii) - ResFirsts(ii);
    if (Abs(aRange3d - aRange) > aMaxTol)
      IsBadRange = Standard_True;
  }

  if (IsBadRange)
  {
    for (Standard_Integer ii = 1; ii <= ResPCurves.Length(); ii++)
    {
      const TopoDS_Face&                   aFace = TopoDS::Face(aFaceSeq(ii));
      Handle(Geom_Surface)                 aSurf = BRep_Tool::Surface(aFace);
      Handle(ShapeAnalysis_Surface)        aSAS  = new ShapeAnalysis_Surface(aSurf);
      ShapeConstruct_ProjectCurveOnSurface aToolProj;
      aToolProj.Init(aSAS, Precision::Confusion());
      Handle(Geom2d_Curve) aNewPCurve;
      if (aToolProj.Perform(aCurve, aFirst3d, aLast3d, aNewPCurve))
        ResPCurves(ii) = aNewPCurve;
      else
      {
        // Reparametrize pcurve
        Handle(Geom2d_TrimmedCurve) aTrPCurve =
          new Geom2d_TrimmedCurve(ResPCurves(ii), ResFirsts(ii), ResLasts(ii));
        Handle(Geom2d_BSplineCurve) aBSplinePCurve = Geom2dConvert::CurveToBSplineCurve(aTrPCurve);
        TColStd_Array1OfReal        aKnots(1, aBSplinePCurve->NbKnots());
        aBSplinePCurve->Knots(aKnots);
        BSplCLib::Reparametrize(aFirst3d, aLast3d, aKnots);
        aBSplinePCurve->SetKnots(aKnots);
        ResPCurves(ii) = aBSplinePCurve;
      }
      ResFirsts(ii) = aFirst3d;
      ResLasts(ii)  = aLast3d;
    }
  }

  // Reparametrize pcurves if needed
  if (!ResPCurves.IsEmpty())
  {
    for (Standard_Integer ii = 1; ii <= ResPCurves.Length(); ii++)
    {
      if (Abs(aFirst3d - ResFirsts(ii)) > aMaxTol || Abs(aLast3d - ResLasts(ii)) > aMaxTol)
      {
        Geom2dAdaptor_Curve aGAcurve(ResPCurves(ii));
        GeomAbs_CurveType   aType = aGAcurve.GetType();
        if (aType == GeomAbs_Line)
        {
          gp_Lin2d aLin2d   = aGAcurve.Line();
          gp_Dir2d aDir2d   = aLin2d.Direction();
          gp_Pnt2d aPnt2d   = aGAcurve.Value(ResFirsts(ii));
          gp_Vec2d anOffset = -aDir2d;
          anOffset *= aFirst3d;
          aPnt2d.Translate(anOffset);
          Handle(Geom2d_Line) aNewLine2d = new Geom2d_Line(aPnt2d, aDir2d);
          ResPCurves(ii)                 = aNewLine2d;
        }
        else if (aType == GeomAbs_Circle)
        {
          gp_Circ2d     aCirc2d   = aGAcurve.Circle();
          Standard_Real aRadius   = aCirc2d.Radius();
          gp_Ax22d      aPosition = aCirc2d.Position();
          gp_Pnt2d      aLocation = aCirc2d.Location();
          Standard_Real anOffset  = ResFirsts(ii) - aFirst3d;
          aPosition.Rotate(aLocation, anOffset);
          Handle(Geom2d_Circle) aNewCircle2d = new Geom2d_Circle(aPosition, aRadius);
          ResPCurves(ii)                     = aNewCircle2d;
        }
        else // general case
        {
          Handle(Geom2d_TrimmedCurve) aTrPCurve =
            new Geom2d_TrimmedCurve(ResPCurves(ii), ResFirsts(ii), ResLasts(ii));
          Handle(Geom2d_BSplineCurve) aBSplinePCurve =
            Geom2dConvert::CurveToBSplineCurve(aTrPCurve);
          TColStd_Array1OfReal aKnots(1, aBSplinePCurve->NbKnots());
          aBSplinePCurve->Knots(aKnots);
          BSplCLib::Reparametrize(aFirst3d, aLast3d, aKnots);
          aBSplinePCurve->SetKnots(aKnots);
          ResPCurves(ii) = aBSplinePCurve;
        }
        ResFirsts(ii) = aFirst3d;
        ResLasts(ii)  = aLast3d;
      } // if ranges > aMaxTol
    } // for (Standard_Integer ii = 1; ii <= ResPCurves.Length(); ii++)
  }

  if (anIsSeam)
  {
    aBuilder.UpdateEdge(theEdge, ResPCurves(1), ResPCurves(2), TopoDS::Face(aFaceSeq(1)), aTolEdge);
  }
  else
    for (Standard_Integer j = 1; j <= ResPCurves.Length(); j++)
    {
      aBuilder.UpdateEdge(theEdge, ResPCurves(j), TopoDS::Face(aFaceSeq(j)), aTolEdge);
    }
}

//=======================================================================
// function : MergeSubSeq
// purpose  : Merges a sequence of edges into one edge if possible
//=======================================================================

Standard_Boolean ShapeUpgrade_UnifySameDomain::MergeSubSeq(
  const TopTools_SequenceOfShape&                  theChain,
  const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap,
  TopoDS_Edge&                                     OutEdge)
{
  ShapeAnalysis_Edge sae;
  BRep_Builder       B;
  // union edges in chain
  int                j;
  Standard_Real      fp1, lp1, fp2, lp2;
  Standard_Boolean   IsUnionOfLinesPossible   = Standard_True;
  Standard_Boolean   IsUnionOfCirclesPossible = Standard_True;
  Handle(Geom_Curve) c3d1, c3d2;
  for (j = 1; j < theChain.Length(); j++)
  {
    TopoDS_Edge edge1 = TopoDS::Edge(theChain.Value(j));
    TopoDS_Edge edge2 = TopoDS::Edge(theChain.Value(j + 1));

    if (BRep_Tool::Degenerated(edge1) && BRep_Tool::Degenerated(edge2))
    {
      // Find the closest points in 2d
      TopoDS_Edge        edgeFirst = TopoDS::Edge(theChain.First());
      TopoDS_Edge        edgeLast  = TopoDS::Edge(theChain.Last());
      TopoDS_Face        CommonFace;
      Standard_Real      MinSqDist;
      TopAbs_Orientation OrOfE1OnFace, OrOfE2OnFace;
      Standard_Integer   IndOnE1, IndOnE2;
      gp_Pnt2d           PointsOnEdge1[2], PointsOnEdge2[2];
      if (!FindClosestPoints(edgeFirst,
                             edgeLast,
                             theVFmap,
                             CommonFace,
                             MinSqDist,
                             OrOfE1OnFace,
                             OrOfE2OnFace,
                             IndOnE1,
                             IndOnE2,
                             PointsOnEdge1,
                             PointsOnEdge2))
        return Standard_False;

      // Define indices corresponding to extremities of future edge
      IndOnE1 = 1 - IndOnE1;
      IndOnE2 = 1 - IndOnE2;

      // Construct new degenerated edge
      gp_Pnt2d StartPoint = PointsOnEdge1[IndOnE1];
      gp_Pnt2d EndPoint   = PointsOnEdge2[IndOnE2];
      if ((OrOfE1OnFace == TopAbs_FORWARD && IndOnE1 == 1)
          || (OrOfE1OnFace == TopAbs_REVERSED && IndOnE1 == 0))
      {
        gp_Pnt2d Tmp = StartPoint;
        StartPoint   = EndPoint;
        EndPoint     = Tmp;
      }

      Handle(Geom2d_Line) aLine = GCE2d_MakeLine(StartPoint, EndPoint);

      TopoDS_Vertex aVertex     = TopExp::FirstVertex(edgeFirst);
      TopoDS_Vertex StartVertex = aVertex, EndVertex = aVertex;
      StartVertex.Orientation(TopAbs_FORWARD);
      EndVertex.Orientation(TopAbs_REVERSED);

      TopoDS_Edge NewEdge;
      B.MakeEdge(NewEdge);
      B.UpdateEdge(NewEdge, aLine, CommonFace, Precision::Confusion());
      B.Range(NewEdge, 0., StartPoint.Distance(EndPoint));
      B.Add(NewEdge, StartVertex);
      B.Add(NewEdge, EndVertex);
      B.Degenerated(NewEdge, Standard_True);
      OutEdge = NewEdge;
      return Standard_True;
    }

    c3d1 = BRep_Tool::Curve(edge1, fp1, lp1);
    c3d2 = BRep_Tool::Curve(edge2, fp2, lp2);

    BRepAdaptor_Curve aBAcurve1(edge1);
    BRepAdaptor_Curve aBAcurve2(edge2);
    gp_Dir            aDir1, aDir2;

    if (c3d1.IsNull() || c3d2.IsNull())
      return Standard_False;

    if (c3d1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(c3d1);
      c3d1                         = tc->BasisCurve();
    }
    if (c3d2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(c3d2);
      c3d2                         = tc->BasisCurve();
    }
    if (IsLinear(aBAcurve1, aDir1) && IsLinear(aBAcurve2, aDir2))
    {
      Handle(Geom_Line) L1 = Handle(Geom_Line)::DownCast(c3d1);
      Handle(Geom_Line) L2 = Handle(Geom_Line)::DownCast(c3d2);
      if (!aDir1.IsParallel(aDir2, myAngTol))
        IsUnionOfLinesPossible = Standard_False;
    }
    else
      IsUnionOfLinesPossible = Standard_False;
    if (c3d1->IsKind(STANDARD_TYPE(Geom_Circle)) && c3d2->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      Handle(Geom_Circle) C1  = Handle(Geom_Circle)::DownCast(c3d1);
      Handle(Geom_Circle) C2  = Handle(Geom_Circle)::DownCast(c3d2);
      gp_Pnt              P01 = C1->Location();
      gp_Pnt              P02 = C2->Location();
      if (P01.Distance(P02) > Precision::Confusion())
        IsUnionOfCirclesPossible = Standard_False;
    }
    else
      IsUnionOfCirclesPossible = Standard_False;
  }
  if (IsUnionOfLinesPossible && IsUnionOfCirclesPossible)
    return Standard_False;

  // union of lines is possible
  if (IsUnionOfLinesPossible)
  {
    TopoDS_Vertex V[2];
    V[0]       = sae.FirstVertex(TopoDS::Edge(theChain.First()));
    gp_Pnt PV1 = BRep_Tool::Pnt(V[0]);
    V[1]       = sae.LastVertex(TopoDS::Edge(theChain.Last()));
    gp_Pnt PV2 = BRep_Tool::Pnt(V[1]);
    gp_Vec Vec(PV1, PV2);
    if (mySafeInputMode)
    {
      for (int k = 0; k < 2; k++)
      {
        if (!myContext->IsRecorded(V[k]))
        {
          TopoDS_Vertex Vcopy = TopoDS::Vertex(V[k].EmptyCopied());
          myContext->Replace(V[k], Vcopy);
          V[k] = Vcopy;
        }
        else
          V[k] = TopoDS::Vertex(myContext->Apply(V[k]));
      }
    }
    Handle(Geom_Line)         L    = new Geom_Line(gp_Ax1(PV1, Vec));
    Standard_Real             dist = PV1.Distance(PV2);
    Handle(Geom_TrimmedCurve) tc   = new Geom_TrimmedCurve(L, 0.0, dist);
    TopoDS_Edge               E;
    B.MakeEdge(E, tc, Precision::Confusion());
    B.Add(E, V[0]);
    B.Add(E, V[1]);
    B.UpdateVertex(V[0], 0., E, 0.);
    B.UpdateVertex(V[1], dist, E, 0.);
    UnionPCurves(theChain, E);
    OutEdge = E;
    return Standard_True;
  }

  if (IsUnionOfCirclesPossible)
  {
    double             f, l;
    TopoDS_Edge        FE  = TopoDS::Edge(theChain.First());
    Handle(Geom_Curve) c3d = BRep_Tool::Curve(FE, f, l);

    if (c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(c3d);
      c3d                          = tc->BasisCurve();
    }
    Handle(Geom_Circle) Cir = Handle(Geom_Circle)::DownCast(c3d);

    TopoDS_Vertex V[2];
    V[0]                      = sae.FirstVertex(FE);
    V[1]                      = sae.LastVertex(TopoDS::Edge(theChain.Last()));
    Standard_Boolean isClosed = V[0].IsSame(V[1]);
    if (!isClosed)
    {
      // additionally check the points for equality to make a final decision about closedness of the
      // result curve
      gp_Pnt        aP0  = BRep_Tool::Pnt(V[0]);
      gp_Pnt        aP1  = BRep_Tool::Pnt(V[1]);
      Standard_Real aTol = Max(BRep_Tool::Tolerance(V[0]), BRep_Tool::Tolerance(V[1]));
      if (aP0.SquareDistance(aP1) < aTol * aTol)
      {
        isClosed = Standard_True;
        V[1]     = V[0];
        V[1].Reverse();
      }
    }
    TopoDS_Edge E;
    if (isClosed)
    {
      // closed chain
      BRepAdaptor_Curve   adef(FE);
      Handle(Geom_Circle) Cir1;
      double              FP, LP;
      if (FE.Orientation() == TopAbs_FORWARD)
      {
        FP = adef.FirstParameter();
        LP = adef.LastParameter();
      }
      else
      {
        FP = adef.LastParameter();
        LP = adef.FirstParameter();
      }
      if (Abs(FP) < Precision::PConfusion())
      {
        B.MakeEdge(E, Cir, Precision::Confusion());
        B.Add(E, V[0]);
        B.Add(E, V[1]);
        E.Orientation(FE.Orientation());
      }
      else
      {
        GC_MakeCircle MC1(adef.Value(FP), adef.Value((FP + LP) * 0.5), adef.Value(LP));
        if (MC1.IsDone())
          Cir1 = MC1.Value();
        else
          return Standard_False;
        B.MakeEdge(E, Cir1, Precision::Confusion());
        B.Add(E, V[0]);
        B.Add(E, V[1]);
      }
    }
    else // open chain
    {
      Standard_Real ParamFirst     = BRep_Tool::Parameter(V[0], FE);
      TopoDS_Vertex VertexLastOnFE = sae.LastVertex(FE);
      Standard_Real ParamLast      = BRep_Tool::Parameter(VertexLastOnFE, FE);

      if (mySafeInputMode)
      {
        for (int k = 0; k < 2; k++)
        {
          if (!myContext->IsRecorded(V[k]))
          {
            TopoDS_Vertex Vcopy = TopoDS::Vertex(V[k].EmptyCopied());
            myContext->Replace(V[k], Vcopy);
            V[k] = Vcopy;
          }
          else
            V[k] = TopoDS::Vertex(myContext->Apply(V[k]));
        }
      }

      gp_Pnt PointFirst = BRep_Tool::Pnt(V[0]);
      while (Abs(ParamLast - ParamFirst) > 7 * M_PI / 8)
        ParamLast = (ParamFirst + ParamLast) / 2;
      BRepAdaptor_Curve   BAcurveFE(FE);
      gp_Pnt              PointLast = BAcurveFE.Value(ParamLast);
      gp_Pnt              Origin    = Cir->Circ().Location();
      gp_Dir              Dir1      = gp_Vec(Origin, PointFirst);
      gp_Dir              Dir2      = gp_Vec(Origin, PointLast);
      gp_Dir              Vdir      = Dir1 ^ Dir2;
      gp_Ax2              anAx2(Origin, Vdir, Dir1);
      Handle(Geom_Circle) aNewCircle       = new Geom_Circle(anAx2, Cir->Radius());
      gp_Pnt              PointLastInChain = BRep_Tool::Pnt(V[1]);
      gp_Dir              DirLastInChain   = gp_Vec(Origin, PointLastInChain);
      Standard_Real       lpar             = Dir1.AngleWithRef(DirLastInChain, Vdir);
      if (lpar < 0.)
        lpar += 2 * M_PI;

      Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(aNewCircle, 0., lpar);
      B.MakeEdge(E, tc, Precision::Confusion());
      B.Add(E, V[0]);
      B.Add(E, V[1]);
      B.UpdateVertex(V[0], 0., E, 0.);
      B.UpdateVertex(V[1], lpar, E, 0.);
    }
    UnionPCurves(theChain, E);
    OutEdge = E;
    return Standard_True;
  }
  if (theChain.Length() > 1 && myConcatBSplines)
  {
    // second step: union edges with various curves
    // skl for bug 0020052 from Mantis: perform such unions
    // only if curves are bspline or bezier

    TopoDS_Vertex    VF        = sae.FirstVertex(TopoDS::Edge(theChain.First()));
    TopoDS_Vertex    VL        = sae.LastVertex(TopoDS::Edge(theChain.Last()));
    Standard_Boolean NeedUnion = Standard_True;
    for (j = 1; j <= theChain.Length(); j++)
    {
      TopoDS_Edge        edge = TopoDS::Edge(theChain.Value(j));
      TopLoc_Location    Loc;
      Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge, Loc, fp1, lp1);
      if (c3d.IsNull())
        continue;
      if (c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
      {
        Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(c3d);
        c3d                          = tc->BasisCurve();
      }
      if ((c3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve))
           || c3d->IsKind(STANDARD_TYPE(Geom_BezierCurve))))
        continue;
      NeedUnion = Standard_False;
      break;
    }
    if (NeedUnion)
    {
#ifdef OCCT_DEBUG
      std::cout << "can not make analytical union => make approximation" << std::endl;
#endif
      TopoDS_Edge E = GlueEdgesWithPCurves(theChain, VF, VL);
      OutEdge       = E;
      return Standard_True;
    }
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "can not make approximation for such types of curves" << std::endl;
#endif
      return Standard_False;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : IsMergingPossible
// purpose  : Checks if merging of two edges is possible
//=======================================================================

static Standard_Boolean IsMergingPossible(const TopoDS_Edge&         edge1,
                                          const TopoDS_Edge&         edge2,
                                          double                     theAngTol,
                                          double                     theLinTol,
                                          const TopTools_MapOfShape& AvoidEdgeVrt,
                                          const bool                 theLineDirectionOk,
                                          const gp_Pnt&              theFirstPoint,
                                          const gp_Vec&              theDirectionVec,
                                          const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap)
{
  Standard_Boolean IsDegE1 = BRep_Tool::Degenerated(edge1);
  Standard_Boolean IsDegE2 = BRep_Tool::Degenerated(edge2);

  if (IsDegE1 && IsDegE2)
  {
    // Find connstion point in 2d
    TopoDS_Face        CommonFace;
    Standard_Real      MinSqDist;
    TopAbs_Orientation OrOfE1OnFace, OrOfE2OnFace;
    Standard_Integer   IndOnE1, IndOnE2;
    gp_Pnt2d           PointsOnEdge1[2], PointsOnEdge2[2];
    if (!FindClosestPoints(edge1,
                           edge2,
                           theVFmap,
                           CommonFace,
                           MinSqDist,
                           OrOfE1OnFace,
                           OrOfE2OnFace,
                           IndOnE1,
                           IndOnE2,
                           PointsOnEdge1,
                           PointsOnEdge2))
      return Standard_False;

    if (MinSqDist <= Precision::SquareConfusion())
      return Standard_True;

    return Standard_False;
  }
  else if (IsDegE1 || IsDegE2)
    return Standard_False;

  TopoDS_Vertex CV = TopExp::LastVertex(edge1, Standard_True);
  if (CV.IsNull() || AvoidEdgeVrt.Contains(CV))
    return Standard_False;

  BRepAdaptor_Curve ade1(edge1);
  BRepAdaptor_Curve ade2(edge2);

  GeomAbs_CurveType t1 = ade1.GetType();
  GeomAbs_CurveType t2 = ade2.GetType();

  if (t1 == GeomAbs_Circle && t2 == GeomAbs_Circle)
  {
    if (ade1.Circle().Location().Distance(ade2.Circle().Location()) > Precision::Confusion())
      return Standard_False;
  }

  gp_Dir aDir1, aDir2;
  if (!(IsLinear(ade1, aDir1) && IsLinear(ade2, aDir2))
      && ((t1 != GeomAbs_BezierCurve && t1 != GeomAbs_BSplineCurve)
          || (t2 != GeomAbs_BezierCurve && t2 != GeomAbs_BSplineCurve))
      && t1 != t2)
    return Standard_False;

  gp_Vec Diff1, Diff2;
  gp_Pnt P1, P2;
  if (edge1.Orientation() == TopAbs_FORWARD)
    ade1.D1(ade1.LastParameter(), P1, Diff1);
  else
  {
    ade1.D1(ade1.FirstParameter(), P1, Diff1);
    Diff1 = -Diff1;
  }

  if (edge2.Orientation() == TopAbs_FORWARD)
    ade2.D1(ade2.FirstParameter(), P2, Diff2);
  else
  {
    ade2.D1(ade2.LastParameter(), P2, Diff2);
    Diff2 = -Diff2;
  }

  if (Diff1.Angle(Diff2) > theAngTol)
    return Standard_False;

  if (theLineDirectionOk && t2 == GeomAbs_Line)
  {
    // Check that the accumulated deflection does not exceed the linear tolerance
    Standard_Real aLast =
      (edge2.Orientation() == TopAbs_FORWARD) ? ade2.LastParameter() : ade2.FirstParameter();
    gp_Vec        aCurV(theFirstPoint, ade2.Value(aLast));
    Standard_Real aDD = theDirectionVec.CrossSquareMagnitude(aCurV);
    if (aDD > theLinTol * theLinTol)
      return Standard_False;

    // Check that the accumulated angle does not exceed the angular tolerance.
    // For symmetry, check the angle between vectors of:
    // - first edge and resulting curve, and
    // - the last edge and resulting curve.
    if (theDirectionVec.Angle(aCurV) > theAngTol || Diff2.Angle(aCurV) > theAngTol)
      return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

static Standard_Boolean GetLineEdgePoints(const TopoDS_Edge& theInpEdge,
                                          gp_Pnt&            theFirstPoint,
                                          gp_Vec&            theDirectionVec)
{
  double             f, l;
  Handle(Geom_Curve) aCur = BRep_Tool::Curve(theInpEdge, f, l);
  if (aCur.IsNull())
    return Standard_False;

  Handle(Geom_TrimmedCurve) aTC = Handle(Geom_TrimmedCurve)::DownCast(aCur);
  if (!aTC.IsNull())
    aCur = aTC->BasisCurve();

  if (aCur->DynamicType() != STANDARD_TYPE(Geom_Line))
    return Standard_False;

  if (theInpEdge.Orientation() == TopAbs_REVERSED)
  {
    Standard_Real tmp = f;
    f                 = l;
    l                 = tmp;
  }
  theFirstPoint   = aCur->Value(f);
  gp_Pnt aLP      = aCur->Value(l);
  theDirectionVec = aLP.XYZ().Subtracted(theFirstPoint.XYZ());
  theDirectionVec.Normalize();
  return Standard_True;
}

struct ShapeUpgrade_UnifySameDomain::SubSequenceOfEdges
{
  TopTools_SequenceOfShape SeqsEdges;
  TopoDS_Edge              UnionEdges;
};

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::generateSubSeq(
  const TopTools_SequenceOfShape&                  anInpEdgeSeq,
  NCollection_Sequence<SubSequenceOfEdges>&        SeqOfSubSeqOfEdges,
  Standard_Boolean                                 IsClosed,
  double                                           theAngTol,
  double                                           theLinTol,
  const TopTools_MapOfShape&                       AvoidEdgeVrt,
  const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap)
{
  Standard_Boolean isOk = Standard_False;
  TopoDS_Edge      edge1, edge2;

  SubSequenceOfEdges SubSeq;
  TopoDS_Edge        RefEdge = TopoDS::Edge(anInpEdgeSeq(1));
  SubSeq.SeqsEdges.Append(RefEdge);
  SeqOfSubSeqOfEdges.Append(SubSeq);

  gp_Pnt           aFirstPoint;
  gp_Vec           aDirectionVec;
  Standard_Boolean isLineDirectionOk = GetLineEdgePoints(RefEdge, aFirstPoint, aDirectionVec);

  for (int i = 1; i < anInpEdgeSeq.Length(); i++)
  {
    edge1 = TopoDS::Edge(anInpEdgeSeq(i));
    edge2 = TopoDS::Edge(anInpEdgeSeq(i + 1));
    isOk  = IsMergingPossible(edge1,
                             edge2,
                             theAngTol,
                             theLinTol,
                             AvoidEdgeVrt,
                             isLineDirectionOk,
                             aFirstPoint,
                             aDirectionVec,
                             theVFmap);
    if (!isOk)
    {
      SubSequenceOfEdges aSubSeq;
      aSubSeq.SeqsEdges.Append(edge2);
      SeqOfSubSeqOfEdges.Append(aSubSeq);
      isLineDirectionOk = GetLineEdgePoints(edge2, aFirstPoint, aDirectionVec);
    }
    else
      SeqOfSubSeqOfEdges.ChangeLast().SeqsEdges.Append(edge2);
  }
  /// check first and last chain segments
  if (IsClosed && SeqOfSubSeqOfEdges.Length() > 1)
  {
    edge1 = TopoDS::Edge(anInpEdgeSeq.Last());
    edge2 = TopoDS::Edge(anInpEdgeSeq.First());
    if (IsMergingPossible(edge1,
                          edge2,
                          theAngTol,
                          theLinTol,
                          AvoidEdgeVrt,
                          Standard_False,
                          aFirstPoint,
                          aDirectionVec,
                          theVFmap))
    {
      SeqOfSubSeqOfEdges.ChangeLast().SeqsEdges.Append(SeqOfSubSeqOfEdges.ChangeFirst().SeqsEdges);
      SeqOfSubSeqOfEdges.Remove(1);
    }
  }
}

//=================================================================================================

Standard_Boolean ShapeUpgrade_UnifySameDomain::MergeEdges(
  TopTools_SequenceOfShape&                        SeqEdges,
  const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap,
  NCollection_Sequence<SubSequenceOfEdges>&        SeqOfSubSeqOfEdges,
  const TopTools_MapOfShape&                       NonMergVrt)
{
  TopTools_IndexedDataMapOfShapeListOfShape aMapVE;
  Standard_Integer                          j;
  TopTools_MapOfShape                       VerticesToAvoid;
  const Standard_Integer                    aNbE = SeqEdges.Length();
  for (j = 1; j <= aNbE; j++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(SeqEdges(j));
    // fill in the map V-E
    for (TopoDS_Iterator it(anEdge.Oriented(TopAbs_FORWARD)); it.More(); it.Next())
    {
      const TopoDS_Shape& aV = it.Value();
      if (aV.Orientation() == TopAbs_FORWARD || aV.Orientation() == TopAbs_REVERSED)
      {
        if (!aMapVE.Contains(aV))
          aMapVE.Add(aV, TopTools_ListOfShape());
        aMapVE.ChangeFromKey(aV).Append(anEdge);
      }
    }
  }
  NCollection_MapAlgo::Unite(VerticesToAvoid, NonMergVrt);

  // do loop while there are unused edges
  TopTools_MapOfShape aUsedEdges;

  for (Standard_Integer iE = 1; iE <= aNbE; ++iE)
  {
    TopoDS_Edge edge = TopoDS::Edge(SeqEdges(iE));
    if (!aUsedEdges.Add(edge))
      continue;

    // make chain for unite
    TopTools_SequenceOfShape aChain;
    aChain.Append(edge);
    TopoDS_Vertex V[2];
    TopExp::Vertices(edge, V[0], V[1], Standard_True);

    // connect more edges to the chain in both directions
    for (j = 0; j < 2; j++)
    {
      Standard_Boolean isAdded = Standard_True;
      while (isAdded)
      {
        isAdded = Standard_False;
        if (V[j].IsNull())
          break;
        const TopTools_ListOfShape& aLE = aMapVE.FindFromKey(V[j]);
        for (TopTools_ListIteratorOfListOfShape itL(aLE); itL.More(); itL.Next())
        {
          edge = TopoDS::Edge(itL.Value());
          if (!aUsedEdges.Contains(edge))
          {
            TopoDS_Vertex V2[2];
            TopExp::Vertices(edge, V2[0], V2[1], Standard_True);
            // the neighboring edge must have V[j] reversed and located on the opposite end
            if (V2[1 - j].IsEqual(V[j].Reversed()))
            {
              if (j == 0)
                aChain.Prepend(edge);
              else
                aChain.Append(edge);
              aUsedEdges.Add(edge);
              V[j]    = V2[j];
              isAdded = Standard_True;
              break;
            }
          }
        }
      }
    }

    if (aChain.Length() < 2)
      continue;

    Standard_Boolean IsClosed = Standard_False;
    if (V[0].IsSame(V[1]))
      IsClosed = Standard_True;

    // split chain by vertices at which merging is not possible
    NCollection_Sequence<SubSequenceOfEdges> aOneSeq;
    generateSubSeq(aChain, aOneSeq, IsClosed, myAngTol, myLinTol, VerticesToAvoid, theVFmap);

    // put sub-chains in the result
    SeqOfSubSeqOfEdges.Append(aOneSeq);
  }

  for (int i = 1; i <= SeqOfSubSeqOfEdges.Length(); i++)
  {
    TopoDS_Edge UE;
    if (SeqOfSubSeqOfEdges(i).SeqsEdges.Length() < 2)
      continue;
    if (MergeSubSeq(SeqOfSubSeqOfEdges(i).SeqsEdges, theVFmap, UE))
      SeqOfSubSeqOfEdges(i).UnionEdges = UE;
  }
  return Standard_True;
}

//=======================================================================
// function : MergeSeq
// purpose  : Tries to unify the sequence of edges with the set of
//           another edges which lies on the same geometry
//=======================================================================
Standard_Boolean ShapeUpgrade_UnifySameDomain::MergeSeq(
  TopTools_SequenceOfShape&                        SeqEdges,
  const TopTools_IndexedDataMapOfShapeListOfShape& theVFmap,
  const TopTools_MapOfShape&                       nonMergVert)
{
  NCollection_Sequence<SubSequenceOfEdges> SeqOfSubsSeqOfEdges;
  if (MergeEdges(SeqEdges, theVFmap, SeqOfSubsSeqOfEdges, nonMergVert))
  {
    for (Standard_Integer i = 1; i <= SeqOfSubsSeqOfEdges.Length(); i++)
    {
      if (SeqOfSubsSeqOfEdges(i).UnionEdges.IsNull())
        continue;

      myContext->Merge(SeqOfSubsSeqOfEdges(i).SeqsEdges, SeqOfSubsSeqOfEdges(i).UnionEdges);
    }
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
// function : CheckSharedVertices
// purpose  : Checks the sequence of edges on the presence of shared vertex
//=======================================================================

static void CheckSharedVertices(const TopTools_SequenceOfShape&                  theSeqEdges,
                                const TopTools_IndexedDataMapOfShapeListOfShape& theMapEdgesVertex,
                                const TopTools_MapOfShape&                       theMapKeepShape,
                                TopTools_MapOfShape&                             theShareVertMap)
{
  ShapeAnalysis_Edge       sae;
  TopTools_SequenceOfShape SeqVertexes;
  TopTools_MapOfShape      MapVertexes;
  for (Standard_Integer k = 1; k <= theSeqEdges.Length(); k++)
  {
    TopoDS_Vertex aV1 = sae.FirstVertex(TopoDS::Edge(theSeqEdges(k)));
    TopoDS_Vertex aV2 = sae.LastVertex(TopoDS::Edge(theSeqEdges(k)));
    if (!MapVertexes.Add(aV1))
      SeqVertexes.Append(aV1);
    if (!MapVertexes.Add(aV2))
      SeqVertexes.Append(aV2);
  }

  for (Standard_Integer k = 1; k <= SeqVertexes.Length() /* && !IsSharedVertexPresent*/; k++)
  {
    const TopTools_ListOfShape& ListEdgesV1 = theMapEdgesVertex.FindFromKey(SeqVertexes(k));
    if (ListEdgesV1.Extent() > 2 || theMapKeepShape.Contains(SeqVertexes(k)))
      theShareVertMap.Add(SeqVertexes(k));
  }
  // return theShareVertMap.IsEmpty() ? false : true;
}

//=================================================================================================

ShapeUpgrade_UnifySameDomain::ShapeUpgrade_UnifySameDomain()
    : myLinTol(Precision::Confusion()),
      myAngTol(Precision::Angular()),
      myUnifyFaces(Standard_True),
      myUnifyEdges(Standard_True),
      myConcatBSplines(Standard_False),
      myAllowInternal(Standard_False),
      mySafeInputMode(Standard_True),
      myHistory(new BRepTools_History)
{
  myContext = new ShapeBuild_ReShape;
}

//=================================================================================================

ShapeUpgrade_UnifySameDomain::ShapeUpgrade_UnifySameDomain(const TopoDS_Shape&    aShape,
                                                           const Standard_Boolean UnifyEdges,
                                                           const Standard_Boolean UnifyFaces,
                                                           const Standard_Boolean ConcatBSplines)
    : myInitShape(aShape),
      myLinTol(Precision::Confusion()),
      myAngTol(Precision::Angular()),
      myUnifyFaces(UnifyFaces),
      myUnifyEdges(UnifyEdges),
      myConcatBSplines(ConcatBSplines),
      myAllowInternal(Standard_False),
      mySafeInputMode(Standard_True),
      myShape(aShape),
      myHistory(new BRepTools_History)
{
  myContext = new ShapeBuild_ReShape;
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::Initialize(const TopoDS_Shape&    aShape,
                                              const Standard_Boolean UnifyEdges,
                                              const Standard_Boolean UnifyFaces,
                                              const Standard_Boolean ConcatBSplines)
{
  myInitShape      = aShape;
  myShape          = aShape;
  myUnifyEdges     = UnifyEdges;
  myUnifyFaces     = UnifyFaces;
  myConcatBSplines = ConcatBSplines;

  myContext->Clear();
  myKeepShapes.Clear();
  myFacePlaneMap.Clear();
  myEFmap.Clear();
  myFaceNewFace.Clear();
  myHistory->Clear();
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::AllowInternalEdges(const Standard_Boolean theValue)
{
  myAllowInternal = theValue;
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::SetSafeInputMode(Standard_Boolean theValue)
{
  mySafeInputMode = theValue;
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::KeepShape(const TopoDS_Shape& theShape)
{
  if (theShape.ShapeType() == TopAbs_EDGE || theShape.ShapeType() == TopAbs_VERTEX)
    myKeepShapes.Add(theShape);
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::KeepShapes(const TopTools_MapOfShape& theShapes)
{
  for (TopTools_MapIteratorOfMapOfShape it(theShapes); it.More(); it.Next())
  {
    if (it.Value().ShapeType() == TopAbs_EDGE || it.Value().ShapeType() == TopAbs_VERTEX)
      myKeepShapes.Add(it.Value());
  }
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::UnifyFaces()
{
  // creating map of edge faces for the whole shape
  TopTools_IndexedDataMapOfShapeListOfShape aGMapEdgeFaces;

  TopTools_IndexedMapOfShape aFaceMap;
  TopExp::MapShapes(myShape, TopAbs_FACE, aFaceMap);
  for (Standard_Integer i = 1; i <= aFaceMap.Extent(); i++)
    TopExp::MapShapesAndAncestors(aFaceMap(i), TopAbs_EDGE, TopAbs_FACE, aGMapEdgeFaces);

  // creating map of face shells for the whole shape to avoid
  // unification of faces belonging to the different shells
  DataMapOfShapeMapOfShape aGMapFaceShells;
  for (TopExp_Explorer anExp(myShape, TopAbs_SHELL); anExp.More(); anExp.Next())
  {
    const TopoDS_Shape& aShell = anExp.Current();
    for (TopoDS_Iterator anItF(aShell); anItF.More(); anItF.Next())
    {
      const TopoDS_Shape& aF = anItF.Value();
      if (TopTools_MapOfShape* pShells = aGMapFaceShells.ChangeSeek(aF))
      {
        pShells->Add(aShell);
      }
      else
      {
        (aGMapFaceShells.Bound(aF, TopTools_MapOfShape()))->Add(aShell);
      }
    }
  }

  // creating map of free boundaries
  TopTools_MapOfShape aFreeBoundMap;
  // look at only shells not belonging to solids
  TopExp_Explorer anExplo(myShape, TopAbs_SHELL, TopAbs_SOLID);
  for (; anExplo.More(); anExplo.Next())
  {
    const TopoDS_Shape&                       aShell = anExplo.Current();
    TopTools_IndexedDataMapOfShapeListOfShape aEFmap;
    TopExp::MapShapesAndAncestors(aShell, TopAbs_EDGE, TopAbs_FACE, aEFmap);
    for (Standard_Integer ii = 1; ii <= aEFmap.Extent(); ii++)
    {
      const TopoDS_Edge&          anEdge    = TopoDS::Edge(aEFmap.FindKey(ii));
      const TopTools_ListOfShape& aFaceList = aEFmap(ii);
      if (!BRep_Tool::Degenerated(anEdge) && aFaceList.Extent() == 1)
        aFreeBoundMap.Add(anEdge);
    }
  }

  // unify faces in each shell separately
  TopExp_Explorer exps;
  for (exps.Init(myShape, TopAbs_SHELL); exps.More(); exps.Next())
    IntUnifyFaces(exps.Current(), aGMapEdgeFaces, aGMapFaceShells, aFreeBoundMap);

  // gather all faces out of shells in one compound and unify them at once
  BRep_Builder    aBB;
  TopoDS_Compound aCmp;
  aBB.MakeCompound(aCmp);
  Standard_Integer nbf = 0;
  for (exps.Init(myShape, TopAbs_FACE, TopAbs_SHELL); exps.More(); exps.Next(), nbf++)
    aBB.Add(aCmp, exps.Current());

  if (nbf > 0)
  {
    // No connection to shells, thus no need to pass the face-shell map
    IntUnifyFaces(aCmp, aGMapEdgeFaces, DataMapOfShapeMapOfShape(), aFreeBoundMap);
  }

  myShape = myContext->Apply(myShape);
}

//=================================================================================================

static void SetFixWireModes(ShapeFix_Face& theSff)
{
  Handle(ShapeFix_Wire) aFixWire                  = theSff.FixWireTool();
  aFixWire->FixSelfIntersectionMode()             = 0;
  aFixWire->FixNonAdjacentIntersectingEdgesMode() = 0;
  aFixWire->FixLackingMode()                      = 0;
  aFixWire->FixNotchedEdgesMode()                 = 0;
  aFixWire->ModifyTopologyMode()                  = Standard_False;
  aFixWire->ModifyRemoveLoopMode()                = 0;
  aFixWire->FixGapsByRangesMode()                 = Standard_False;
  aFixWire->FixSmallMode()                        = 0;
}

//=======================================================================
// function : isSameSets
// purpose  : Compares two sets of shapes. Returns true if they are the same,
//           false otherwise.
//=======================================================================

template <class Container>
static Standard_Boolean isSameSets(const Container* theFShells1, const Container* theFShells2)
{
  // If both are null - no problem
  if (theFShells1 == nullptr && theFShells2 == nullptr)
  {
    return Standard_True;
  }
  // If only one is null - not the same
  if (theFShells1 == nullptr || theFShells2 == nullptr)
  {
    return Standard_False;
  }
  // Both not null
  if (theFShells1->Extent() != theFShells2->Extent())
  {
    return Standard_False;
  }
  // number of shells in each set should be very small in normal cases - max 2.
  // thus just check if all objects of one are contained in the other and vice versa.
  for (typename Container::Iterator it1(*theFShells1), it2(*theFShells2); it1.More() && it2.More();
       it1.Next(), it2.Next())
  {
    if (!theFShells1->Contains(it2.Value()) || !theFShells2->Contains(it1.Value()))
    {
      return Standard_False;
    }
  }
  return Standard_True;
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::IntUnifyFaces(
  const TopoDS_Shape&                              theInpShape,
  const TopTools_IndexedDataMapOfShapeListOfShape& theGMapEdgeFaces,
  const DataMapOfShapeMapOfShape&                  theGMapFaceShells,
  const TopTools_MapOfShape&                       theFreeBoundMap)
{
  // creating map of edge faces for the shape
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(theInpShape, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

  // map of processed shapes
  TopTools_MapOfShape aProcessed;

  // processing each face
  TopExp_Explorer exp;
  for (exp.Init(theInpShape, TopAbs_FACE); exp.More(); exp.Next())
  {

    TopoDS_Face aFace = TopoDS::Face(exp.Current());

    if (aProcessed.Contains(aFace))
      continue;

    // Boundary edges for the new face
    TopTools_SequenceOfShape edges;
    TopTools_SequenceOfShape RemovedEdges;

    Standard_Integer dummy;
    AddOrdinaryEdges(edges, aFace, dummy, RemovedEdges);

    // Faces to get unified with the current faces
    TopTools_SequenceOfShape faces;

    // Add the current face for unification
    faces.Append(aFace);

    // surface and location to construct result
    TopLoc_Location      aBaseLocation;
    Handle(Geom_Surface) aBaseSurface     = BRep_Tool::Surface(aFace);
    aBaseSurface                          = ClearRts(aBaseSurface);
    TopAbs_Orientation RefFaceOrientation = aFace.Orientation();

    // Take original surface
    TopoDS_Face  RefFace;
    BRep_Builder BB;
    BB.MakeFace(RefFace, aBaseSurface, aBaseLocation, 0.);
    RefFace.Orientation(RefFaceOrientation);
    TopTools_MapOfShape MapEdgesWithTemporaryPCurves; // map of edges not lying on RefFace
    // these edges may be updated by temporary pcurves

    Standard_Real Uperiod = (aBaseSurface->IsUPeriodic()) ? aBaseSurface->UPeriod() : 0.;
    Standard_Real Vperiod = (aBaseSurface->IsVPeriodic()) ? aBaseSurface->VPeriod() : 0.;

    // Get shells connected to the face (in normal cases should not be more than 2)
    const TopTools_MapOfShape* pFShells1 = theGMapFaceShells.Seek(aFace);

    // find adjacent faces to union
    Standard_Integer i;
    for (i = 1; i <= edges.Length(); i++)
    {
      TopoDS_Edge edge = TopoDS::Edge(edges(i));
      if (BRep_Tool::Degenerated(edge))
        continue;

      // get connectivity of the edge in the global shape
      const TopTools_ListOfShape& aGList = theGMapEdgeFaces.FindFromKey(edge);
      if (!myAllowInternal
          && (aGList.Extent() != 2 || myKeepShapes.Contains(edge)
              || theFreeBoundMap.Contains(edge)))
      {
        // non manifold case is not processed unless myAllowInternal
        continue;
      }
      //
      // Get the faces connected through the edge in the current shape
      const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
      if (aList.Extent() < 2)
      {
        continue;
      }

      // for a planar face create and store pcurve of edge on face
      // to speed up all operations
      if (!mySafeInputMode && aBaseSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
        BRepLib::BuildPCurveForEdgeOnPlane(edge, aFace);

      // get normal of the face to compare it with normals of other faces
      gp_Dir aDN1;
      //
      // take intermediate point on edge to compute the normal
      Standard_Real f, l;
      BRep_Tool::Range(edge, f, l);
      Standard_Real aTMid = (f + l) * .5;
      //
      Standard_Boolean bCheckNormals = GetNormalToSurface(aFace, edge, aTMid, aDN1);
      //
      // Process the faces
      TopTools_ListIteratorOfListOfShape anIter(aList);
      for (; anIter.More(); anIter.Next())
      {

        TopoDS_Face aCheckedFace = TopoDS::Face(anIter.Value());
        if (aCheckedFace.IsSame(aFace))
          continue;

        if (aProcessed.Contains(aCheckedFace))
          continue;

        // Get shells connected to the checked face
        const TopTools_MapOfShape* pFShells2 = theGMapFaceShells.Seek(aCheckedFace);
        // Faces can be unified only if the shells of faces connected to
        // these faces are the same. Otherwise, topology would be broken.
        if (!isSameSets(pFShells1, pFShells2))
        {
          continue;
        }

        if (bCheckNormals)
        {
          // get normal of checked face using the same parameter on edge
          gp_Dir aDN2;
          if (GetNormalToSurface(aCheckedFace, edge, aTMid, aDN2))
          {
            // and check if the adjacent faces are having approximately same normals
            Standard_Real anAngle = aDN1.Angle(aDN2);
            if (anAngle > myAngTol)
            {
              continue;
            }
          }
        }
        //
        if (IsSameDomain(aFace, aCheckedFace, myLinTol, myAngTol, myFacePlaneMap))
        {

          if (AddOrdinaryEdges(edges, aCheckedFace, dummy, RemovedEdges))
          {
            // sequence edges is modified
            i = dummy;
          }

          faces.Append(aCheckedFace);
          aProcessed.Add(aCheckedFace);
          break;
        }
      }
    }

    if (faces.Length() > 1)
    {
      if (myFacePlaneMap.IsBound(faces(1)))
      {
        const Handle(Geom_Plane)& aPlane = myFacePlaneMap(faces(1));
        TopLoc_Location           aLoc;
        BB.UpdateFace(RefFace, aPlane, aLoc, Precision::Confusion());
      }
      // Add correct pcurves for the reference surface to the edges of other faces
      TopoDS_Face F_RefFace = RefFace;
      F_RefFace.Orientation(TopAbs_FORWARD);
      AddPCurves(faces, F_RefFace, MapEdgesWithTemporaryPCurves);

      // fill in the connectivity map for selected faces
      TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
      for (i = 1; i <= faces.Length(); i++)
      {
        TopExp::MapShapesAndAncestors(faces(i), TopAbs_EDGE, TopAbs_FACE, aMapEF);
      }
      // Collect keep edges and multi-connected edges, i.e. edges that are internal to
      // the set of selected faces and have connections to other faces.
      TopTools_ListOfShape aKeepEdges;
      for (i = 1; i <= aMapEF.Extent(); i++)
      {
        const TopTools_ListOfShape& aLF = aMapEF(i);
        if (aLF.Extent() == 2)
        {
          const TopoDS_Shape&         aE   = aMapEF.FindKey(i);
          const TopTools_ListOfShape& aGLF = theGMapEdgeFaces.FindFromKey(aE);
          if (aGLF.Extent() > 2 || myKeepShapes.Contains(aE) || theFreeBoundMap.Contains(aE))
          {
            aKeepEdges.Append(aE);
          }
        }
      }
      if (!aKeepEdges.IsEmpty())
      {
        if (!myAllowInternal)
        {
          // Remove from the selection the faces which have no other connect edges
          // and contain multi-connected edges and/or keep edges.
          TopTools_MapOfShape                anAvoidFaces;
          TopTools_ListIteratorOfListOfShape it(aKeepEdges);
          for (; it.More(); it.Next())
          {
            const TopoDS_Shape&         aE  = it.Value();
            const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(aE);
            anAvoidFaces.Add(aLF.First());
            anAvoidFaces.Add(aLF.Last());
          }
          for (i = 1; i <= faces.Length(); i++)
          {
            if (anAvoidFaces.Contains(faces(i)))
            {
              // update the boundaries of merged area, for that
              // remove from 'edges' the edges of this face and add to 'edges'
              // the edges of this face that were not present in 'edges' before
              Standard_Boolean hasConnectAnotherFaces = Standard_False;
              TopExp_Explorer  ex(faces(i), TopAbs_EDGE);
              for (; ex.More() && !hasConnectAnotherFaces; ex.Next())
              {
                const TopoDS_Shape&         aE  = ex.Current();
                const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(aE);
                if (aLF.Extent() > 1)
                {
                  for (it.Init(aLF); it.More() && !hasConnectAnotherFaces; it.Next())
                  {
                    if (!anAvoidFaces.Contains(it.Value()))
                      hasConnectAnotherFaces = Standard_True;
                  }
                }
              }
              if (!hasConnectAnotherFaces)
              {
                AddOrdinaryEdges(edges, faces(i), dummy, RemovedEdges);
                faces.Remove(i);
                i--;
              }
            }
          }
          // check if the faces with keep edges contained in
          // already updated the boundaries of merged area
          if (!faces.IsEmpty())
          {
            TopTools_MapOfShape aMapFaces;
            for (i = 1; i <= faces.Length(); i++)
            {
              aMapFaces.Add(faces(i));
            }
            for (it.Init(aKeepEdges); it.More(); it.Next())
            {
              const TopoDS_Shape&         aE  = it.Value();
              const TopTools_ListOfShape& aLF = aMapEF.FindFromKey(aE);
              if (aLF.Extent() < 2)
                continue;
              if (aMapFaces.Contains(aLF.First()) && aMapFaces.Contains(aLF.Last()))
              {
                for (i = 1; i <= faces.Length(); i++)
                {
                  if (faces(i).IsEqual(aLF.First()) || faces(i).IsEqual(aLF.Last()))
                  {
                    AddOrdinaryEdges(edges, faces(i), dummy, RemovedEdges);
                    faces.Remove(i);
                    i--;
                  }
                }
              }
            }
          }
        } // if (!myAllowInternal)
        else
        { // internal edges are allowed
          // add multi-connected and keep edges as internal in new face
          TopTools_ListIteratorOfListOfShape it(aKeepEdges);
          for (; it.More(); it.Next())
          {
            const TopoDS_Shape& aE = it.Value();
            edges.Append(aE.Oriented(TopAbs_INTERNAL));
          }
        }
      } // if (!aKeepEdges.IsEmpty())
    } // if (faces.Length() > 1)

    TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
    for (i = 1; i <= faces.Length(); i++)
      TopExp::MapShapesAndUniqueAncestors(faces(i), TopAbs_EDGE, TopAbs_FACE, aMapEF);

    // Correct orientation of edges
    for (Standard_Integer ii = 1; ii <= edges.Length(); ii++)
    {
      const TopoDS_Shape&         anEdge = edges(ii);
      Standard_Integer            indE   = aMapEF.FindIndex(anEdge);
      const TopTools_ListOfShape& aLF    = aMapEF(indE);
      if (myAllowInternal && myKeepShapes.Contains(anEdge) && aLF.Extent() == 2)
        edges(ii).Orientation(TopAbs_INTERNAL);

      if (anEdge.Orientation() != TopAbs_INTERNAL)
        edges(ii) = aMapEF.FindKey(indE);
    }

    // Exclude internal edges
    TopTools_IndexedMapOfShape InternalEdges;
    Standard_Integer           ind_e = 1;
    while (ind_e <= edges.Length())
    {
      const TopoDS_Shape& anEdge = edges(ind_e);
      if (anEdge.Orientation() == TopAbs_INTERNAL)
      {
        InternalEdges.Add(anEdge);
        edges.Remove(ind_e);
      }
      else
        ind_e++;
    }

    if (RefFaceOrientation == TopAbs_REVERSED)
      for (Standard_Integer ii = 1; ii <= edges.Length(); ii++)
        edges(ii).Reverse();
    TopoDS_Face F_RefFace = RefFace;
    F_RefFace.Orientation(TopAbs_FORWARD);

    // all faces collected in the sequence. Perform union of faces
    if (faces.Length() > 1)
    {
      Standard_Real       CoordTol = Precision::Confusion();
      TopTools_MapOfShape edgesMap;
      CoordTol = ComputeMinEdgeSize(edges, F_RefFace, edgesMap);
      CoordTol /= 10.;
      CoordTol = Max(CoordTol, Precision::Confusion());

      TopTools_IndexedDataMapOfShapeListOfShape VEmap;
      for (Standard_Integer ind = 1; ind <= edges.Length(); ind++)
        TopExp::MapShapesAndUniqueAncestors(edges(ind), TopAbs_VERTEX, TopAbs_EDGE, VEmap);

      // Try to find seam edge and an edge that is not seam but has 2 pcurves on the surface
      Standard_Boolean SeamFound = Standard_False, UseamFound = Standard_False,
                       VseamFound = Standard_False;
      TopoDS_Edge EdgeWith2pcurves;
      for (Standard_Integer ii = 1; ii <= faces.Length(); ii++)
      {
        const TopoDS_Face& face_ii     = TopoDS::Face(faces(ii));
        TopoDS_Wire        anOuterWire = BRepTools::OuterWire(face_ii);
        TopoDS_Iterator    itw(anOuterWire);
        for (; itw.More(); itw.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
          if (BRep_Tool::IsClosed(anEdge, face_ii))
          {
            if (BRepTools::IsReallyClosed(anEdge, face_ii))
            {
              if (IsUiso(anEdge, face_ii))
                UseamFound = Standard_True;
              else
                VseamFound = Standard_True;
            }
            else
              EdgeWith2pcurves = anEdge;
          }
        }
      }
      SeamFound = UseamFound || VseamFound;

      Standard_Boolean aIsEdgeWith2pcurvesSmooth = Standard_False;
      if (myConcatBSplines && !EdgeWith2pcurves.IsNull() && !SeamFound)
      {
        const TopTools_ListOfShape& aFaceList = theGMapEdgeFaces.FindFromKey(EdgeWith2pcurves);
        const TopoDS_Face&          aFace1    = TopoDS::Face(aFaceList.First());
        const TopoDS_Face&          aFace2    = TopoDS::Face(aFaceList.Last());
        GeomAbs_Shape               anOrderOfCont =
          BRepLib::ContinuityOfFaces(EdgeWith2pcurves, aFace1, aFace2, myAngTol);
        aIsEdgeWith2pcurvesSmooth = (anOrderOfCont >= GeomAbs_G1);
      }

      if (aIsEdgeWith2pcurvesSmooth)
      {
        Handle(Geom2d_Curve) aPC1, aPC2;
        Standard_Real        aFirst, aLast;
        aPC1 = BRep_Tool::CurveOnSurface(EdgeWith2pcurves, F_RefFace, aFirst, aLast);
        EdgeWith2pcurves.Reverse();
        aPC2 = BRep_Tool::CurveOnSurface(EdgeWith2pcurves, F_RefFace, aFirst, aLast);
        gp_Pnt2d         aPnt1       = aPC1->Value(aFirst);
        gp_Pnt2d         aPnt2       = aPC2->Value(aFirst);
        Standard_Boolean anIsUclosed = (Abs(aPnt1.X() - aPnt2.X()) > Abs(aPnt1.Y() - aPnt2.Y()));
        Standard_Boolean aToMakeUPeriodic = Standard_False, aToMakeVPeriodic = Standard_False;
        if (anIsUclosed && Uperiod == 0.)
          aToMakeUPeriodic = Standard_True;
        if (!anIsUclosed && Vperiod == 0.)
          aToMakeVPeriodic = Standard_True;

        if (aToMakeUPeriodic || aToMakeVPeriodic)
        {
          Handle(Geom_BSplineSurface) aBSplineSurface =
            Handle(Geom_BSplineSurface)::DownCast(aBaseSurface);
          if (aBSplineSurface.IsNull())
          {
            Standard_Real    aTol   = 1.e-4;
            GeomAbs_Shape    aUCont = GeomAbs_C1, aVCont = GeomAbs_C1;
            Standard_Integer degU = 14, degV = 14;
            Standard_Integer nmax  = 16;
            Standard_Integer aPrec = 1;
            GeomConvert_ApproxSurface
              Approximator(aBaseSurface, aTol, aUCont, aVCont, degU, degV, nmax, aPrec);
            aBSplineSurface = Approximator.Surface();
          }

          if (aToMakeUPeriodic)
          {
            aBSplineSurface->SetUPeriodic();
            Uperiod = aBSplineSurface->UPeriod();
          }
          if (aToMakeVPeriodic)
          {
            aBSplineSurface->SetVPeriodic();
            Vperiod = aBSplineSurface->VPeriod();
          }

          // Update ref face and pcurves if the surface changed
          if (aBSplineSurface != aBaseSurface)
          {
            TopoDS_Face          OldRefFace = RefFace;
            Handle(Geom2d_Curve) NullPCurve;
            RefFace.Nullify();
            BB.MakeFace(RefFace, aBSplineSurface, aBaseLocation, 0.);
            for (Standard_Integer ii = 1; ii <= edges.Length(); ii++)
            {
              TopoDS_Edge          anEdge = TopoDS::Edge(edges(ii));
              Handle(Geom2d_Curve) aPCurve =
                BRep_Tool::CurveOnSurface(anEdge, OldRefFace, aFirst, aLast);
              if (MapEdgesWithTemporaryPCurves.Contains(anEdge))
                BB.UpdateEdge(anEdge, NullPCurve, OldRefFace, 0.);
              BB.UpdateEdge(anEdge, aPCurve, RefFace, 0.);
            }
            F_RefFace = RefFace;
            F_RefFace.Orientation(TopAbs_FORWARD);
          }
        }
      } // if (myConcatBSplines && !EdgeWith2pcurves.IsNull() && !SeamFound)

      // Perform relocating to new U-origin
      // Define boundaries in 2d space of RefFace
      Standard_Real    aPeriods[2]      = {Uperiod, Vperiod};
      Standard_Boolean anIsSeamFound[2] = {UseamFound, VseamFound};
      Standard_Real    aSurfMin[2], aSurfMax[2];
      aBaseSurface->Bounds(aSurfMin[0], aSurfMax[0], aSurfMin[1], aSurfMax[1]);

      for (Standard_Integer ii = 0; ii < 2; ii++)
        if (aPeriods[ii] != 0.)
        {
          // if seam edge exists, do nothing
          if (!anIsSeamFound[ii])
          {
            // try to find the origin of U in 2d space
            // so that all the faces are in [origin, origin + Uperiod]
            Standard_Real    aMinCoord, aMaxCoord; // Umin, Umax;
            Standard_Integer aNumberOfIntervals, i_face_max;
            if (!FindCoordBounds(faces,
                                 F_RefFace,
                                 aMapEF,
                                 edgesMap,
                                 ii + 1,
                                 aPeriods[ii],
                                 aMinCoord,
                                 aMaxCoord,
                                 aNumberOfIntervals,
                                 i_face_max))
            {
              break;
            }

            if (aMaxCoord - aMinCoord > aPeriods[ii] - 1.e-5)
              anIsSeamFound[ii] = Standard_True;
            else if (aNumberOfIntervals == 2)
            {
              TopTools_MapOfShape                                     UsedEdges;
              NCollection_DataMap<TopoDS_Shape, Handle(Geom2d_Curve)> EdgeNewPCurve;

              // Relocate pcurves to new U-origin
              RelocatePCurvesToNewUorigin(edges,
                                          faces(i_face_max),
                                          F_RefFace,
                                          CoordTol,
                                          ii + 1,
                                          aPeriods[ii],
                                          VEmap,
                                          EdgeNewPCurve,
                                          UsedEdges);

              // PCurves from unused edges (may be degenerated edges)
              for (Standard_Integer ind = 1; ind <= edges.Length(); ind++)
              {
                const TopoDS_Edge& anEdge = TopoDS::Edge(edges(ind));
                if (!UsedEdges.Contains(anEdge))
                {
                  Standard_Real        fpar, lpar;
                  Handle(Geom2d_Curve) aPCurve =
                    BRep_Tool::CurveOnSurface(anEdge, F_RefFace, fpar, lpar);
                  aPCurve = new Geom2d_TrimmedCurve(aPCurve, fpar, lpar);
                  EdgeNewPCurve.Bind(anEdge, aPCurve);
                }
              }

              // Restore VEmap
              VEmap.Clear();
              for (Standard_Integer ind = 1; ind <= edges.Length(); ind++)
                TopExp::MapShapesAndUniqueAncestors(edges(ind), TopAbs_VERTEX, TopAbs_EDGE, VEmap);

              // Find NewUmin and NewUmax
              Standard_Real NewCoordMin = RealLast(), NewCoordMax = RealFirst();
              for (Standard_Integer jj = 1; jj <= edges.Length(); jj++)
              {
                const Handle(Geom2d_Curve)& aPCurve = EdgeNewPCurve(edges(jj));
                UpdateBoundaries(aPCurve,
                                 aPCurve->FirstParameter(),
                                 aPCurve->LastParameter(),
                                 ii + 1,
                                 NewCoordMin,
                                 NewCoordMax);
              }

              if (NewCoordMax - NewCoordMin < aPeriods[ii] - CoordTol
                  && !(-Precision::Confusion() < NewCoordMin
                       && NewCoordMin < aPeriods[ii] + Precision::Confusion()
                       && -Precision::Confusion() < NewCoordMax
                       && NewCoordMax < aPeriods[ii] + Precision::Confusion()))
              {
                // we can build a face without seam edge:
                // update the edges with earlier computed relocated pcurves
                // fitting into (NewUorigin, NewUorigin + Uperiod)
                Standard_Real RestSpaceInCoord = aPeriods[ii] - (NewCoordMax - NewCoordMin);
                Standard_Real NewCoordOrigin   = NewCoordMin - RestSpaceInCoord / 2;
                if (NewCoordOrigin < aSurfMin[ii])
                  NewCoordOrigin = aSurfMin[ii];
                Handle(Geom_Surface) NewSurf;
                Standard_Boolean     anIsInU = (ii == 0);
                if (NewCoordOrigin == aSurfMin[ii])
                  NewSurf = aBaseSurface;
                else
                  NewSurf = new Geom_RectangularTrimmedSurface(aBaseSurface,
                                                               NewCoordOrigin,
                                                               NewCoordOrigin + aPeriods[ii],
                                                               anIsInU); // trim in U or V
                TopoDS_Face          OldRefFace = RefFace;
                Handle(Geom2d_Curve) NullPCurve;
                RefFace.Nullify();
                BB.MakeFace(RefFace, NewSurf, aBaseLocation, 0.);
                for (Standard_Integer jj = 1; jj <= edges.Length(); jj++)
                {
                  TopoDS_Edge anEdge = TopoDS::Edge(edges(jj));
                  if (MapEdgesWithTemporaryPCurves.Contains(anEdge))
                    BB.UpdateEdge(anEdge, NullPCurve, OldRefFace, 0.);
                  const Handle(Geom2d_Curve)& aPCurve = EdgeNewPCurve(anEdge);
                  BB.UpdateEdge(anEdge, aPCurve, RefFace, 0.);
                }
              }
            } // else (Umax - Umin < Uperiod - 1.e-5, no Useam)
          } // if (!UseamFound)
        } // if (Uperiod != 0.)
      UseamFound = anIsSeamFound[0];
      VseamFound = anIsSeamFound[1];
      ////////////////////////////////////
      F_RefFace = RefFace;
      F_RefFace.Orientation(TopAbs_FORWARD);

      TopTools_SequenceOfShape NewFaces, NewWires;

      if (Uperiod == 0 || Vperiod == 0)
      {
        // Set the "periods" for closed non-periodic surface
        TopLoc_Location      aLoc;
        Handle(Geom_Surface) aSurf = BRep_Tool::Surface(RefFace, aLoc);
        if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
          aSurf = (Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf))->BasisSurface();
        Standard_Real Ufirst, Ulast, Vfirst, Vlast;
        aSurf->Bounds(Ufirst, Ulast, Vfirst, Vlast);
        if (Uperiod == 0 && aSurf->IsUClosed())
          Uperiod = Ulast - Ufirst;
        if (Vperiod == 0 && aSurf->IsVClosed())
          Vperiod = Vlast - Vfirst;
      }

      TopTools_MapOfShape UsedEdges;

      Standard_Real FaceUmin = RealLast();
      Standard_Real FaceVmin = RealLast();
      for (Standard_Integer ii = 1; ii <= edges.Length(); ii++)
      {
        const TopoDS_Edge&  anEdge = TopoDS::Edge(edges(ii));
        BRepAdaptor_Curve2d aBAcurve(anEdge, F_RefFace);
        gp_Pnt2d            aFirstPoint = aBAcurve.Value(aBAcurve.FirstParameter());
        gp_Pnt2d            aLastPoint  = aBAcurve.Value(aBAcurve.LastParameter());

        if (aFirstPoint.X() < FaceUmin)
          FaceUmin = aFirstPoint.X();
        if (aLastPoint.X() < FaceUmin)
          FaceUmin = aLastPoint.X();

        if (aFirstPoint.Y() < FaceVmin)
          FaceVmin = aFirstPoint.Y();
        if (aLastPoint.Y() < FaceVmin)
          FaceVmin = aLastPoint.Y();
      }

      // Building new wires from <edges>
      // and build faces
      while (!edges.IsEmpty())
      {
        // try to find non-degenerated edge
        TopoDS_Edge      StartEdge = TopoDS::Edge(edges(1));
        Standard_Integer istart    = 1;
        while (BRep_Tool::Degenerated(StartEdge) && istart < edges.Length())
        {
          istart++;
          StartEdge = TopoDS::Edge(edges(istart));
        }

        TopoDS_Wire aNewWire;
        BB.MakeWire(aNewWire);
        BB.Add(aNewWire, StartEdge);
        RemoveEdgeFromMap(StartEdge, VEmap);
        TopTools_IndexedMapOfShape SplittingVertices;

        Standard_Real        fpar, lpar;
        Handle(Geom2d_Curve) StartPCurve =
          BRep_Tool::CurveOnSurface(StartEdge, F_RefFace, fpar, lpar);
        TopoDS_Vertex StartVertex, CurVertex;
        TopExp::Vertices(StartEdge, StartVertex, CurVertex, Standard_True); // with orientation
        Standard_Real StartParam, CurParam;
        if (StartEdge.Orientation() == TopAbs_FORWARD)
        {
          StartParam = fpar;
          CurParam   = lpar;
        }
        else
        {
          StartParam = lpar;
          CurParam   = fpar;
        }
        gp_Pnt2d StartPoint = StartPCurve->Value(StartParam);
        gp_Pnt2d CurPoint   = StartPCurve->Value(CurParam);

        TopoDS_Edge CurEdge = StartEdge;
        for (;;) // loop till the end of current new wire
        {
          TopoDS_Edge NextEdge;
          gp_Pnt2d    NextPoint;

          TopTools_ListOfShape Elist;
          // const TopTools_ListOfShape& Elist = VEmap.FindFromKey(CurVertex);
          if (VEmap.Contains(CurVertex))
            Elist = VEmap.FindFromKey(CurVertex);
          TopTools_ListIteratorOfListOfShape itl(Elist);
          if (Elist.IsEmpty())
          {
            if (CurVertex.IsSame(StartVertex))
            {
              // Points of two vertices coincide in 3d but may be not in 2d
              if ((Uperiod != 0. && Abs(StartPoint.X() - CurPoint.X()) > Uperiod / 2)
                  || (Vperiod != 0.
                      && Abs(StartPoint.Y() - CurPoint.Y())
                           > Vperiod / 2)) // end of parametric space
              {
                //<edges> do not contain seams => we must reconstruct the seam up to <NextEdge>
                gp_Pnt2d      StartOfNextEdge;
                TopoDS_Vertex LastVertexOfSeam;
                ReconstructMissedSeam(RemovedEdges,
                                      F_RefFace,
                                      CurEdge,
                                      CurVertex,
                                      CurPoint,
                                      Uperiod,
                                      Vperiod,
                                      NextEdge,
                                      NextPoint);
              }
              else
              {
                break; // end of wire
              }
            }
          }

          if (NextEdge.IsNull())
          {
            Standard_Boolean EndOfWire = Standard_False;

            Standard_Boolean anIsOnSingularity = IsOnSingularity(Elist);
            if (!anIsOnSingularity && Elist.Extent() > 1)
              SplittingVertices.Add(CurVertex);

            TopTools_ListOfShape TmpElist, TrueElist;
            //<TrueElist> will be the list of candidates to become <NextEdge>
            for (itl.Initialize(Elist); itl.More(); itl.Next())
            {
              const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());
              if (UsedEdges.Contains(anEdge))
                continue;
              TopoDS_Vertex aFirstVertex = TopExp::FirstVertex(anEdge, Standard_True);
              if (!aFirstVertex.IsSame(CurVertex))
                continue;
              TmpElist.Append(anEdge);
            }
            if (TmpElist.Extent() <= 1 || (Uperiod != 0. || Vperiod != 0))
              TrueElist.Assign(TmpElist);
            else
            {
              // we must choose the closest direction - the biggest angle
              Standard_Real        MaxAngle = RealFirst();
              TopoDS_Edge          TrueEdge;
              Handle(Geom2d_Curve) CurPCurve =
                BRep_Tool::CurveOnSurface(CurEdge, F_RefFace, fpar, lpar);
              CurParam = (CurEdge.Orientation() == TopAbs_FORWARD) ? lpar : fpar;
              gp_Vec2d CurDir;
              CurPCurve->D1(CurParam, CurPoint, CurDir);
              CurDir.Normalize();
              if (CurEdge.Orientation() == TopAbs_REVERSED)
                CurDir.Reverse();
              for (itl.Initialize(TmpElist); itl.More(); itl.Next())
              {
                const TopoDS_Edge&   anEdge = TopoDS::Edge(itl.Value());
                Handle(Geom2d_Curve) aPCurve =
                  BRep_Tool::CurveOnSurface(anEdge, F_RefFace, fpar, lpar);
                Standard_Real aParam = (anEdge.Orientation() == TopAbs_FORWARD) ? fpar : lpar;
                gp_Pnt2d      aPoint;
                gp_Vec2d      aDir;
                aPCurve->D1(aParam, aPoint, aDir);
                aDir.Normalize();
                if (anEdge.Orientation() == TopAbs_REVERSED)
                  aDir.Reverse();
                Standard_Real anAngle = CurDir.Angle(aDir);
                if (anAngle > MaxAngle)
                {
                  MaxAngle = anAngle;
                  TrueEdge = anEdge;
                }
              }
              TrueElist.Append(TrueEdge);
            }

            // Find next edge in TrueElist
            for (itl.Initialize(TrueElist); itl.More(); itl.Next())
            {
              const TopoDS_Edge& anEdge = TopoDS::Edge(itl.Value());

              Handle(Geom2d_Curve) aPCurve =
                BRep_Tool::CurveOnSurface(anEdge, F_RefFace, fpar, lpar);
              Standard_Real aParam = (anEdge.Orientation() == TopAbs_FORWARD) ? fpar : lpar;
              gp_Pnt2d      aPoint = aPCurve->Value(aParam);
              Standard_Real DiffU  = Abs(aPoint.X() - CurPoint.X());
              Standard_Real DiffV  = Abs(aPoint.Y() - CurPoint.Y());
              if (Uperiod != 0. && DiffU > CoordTol
                  && Abs(DiffU - Uperiod) > CoordTol) // may be it is a deg.vertex
                continue;
              if (Vperiod != 0. && DiffV > CoordTol
                  && Abs(DiffV - Vperiod) > CoordTol) // may be it is a deg.vertex
                continue;

              // Check: may be <CurPoint> and <aPoint> are on Period from each other
              if ((Uperiod != 0. && DiffU > Uperiod / 2)
                  || (Vperiod != 0. && DiffV > Vperiod / 2)) // end of parametric space
              {
                //<edges> do not contain seams => we must reconstruct the seam up to <NextEdge>
                gp_Pnt2d      StartOfNextEdge;
                TopoDS_Vertex LastVertexOfSeam;
                ReconstructMissedSeam(RemovedEdges,
                                      F_RefFace,
                                      CurEdge,
                                      CurVertex,
                                      CurPoint,
                                      Uperiod,
                                      Vperiod,
                                      NextEdge,
                                      NextPoint);

                // Check: may be it is the end
                if (LastVertexOfSeam.IsSame(StartVertex)
                    && Abs(StartPoint.X() - StartOfNextEdge.X()) < Uperiod / 2)
                  EndOfWire = Standard_True;

                break;
              }
              else
              {
                NextEdge                = anEdge;
                Standard_Real LastParam = (NextEdge.Orientation() == TopAbs_FORWARD) ? lpar : fpar;
                NextPoint               = aPCurve->Value(LastParam);
                break;
              }
            } // for (itl.Initialize(TrueElist); itl.More(); itl.Next())

            if (EndOfWire)
              break;
          }

          if (NextEdge.IsNull())
          {
            if (Uperiod != 0. || Vperiod != 0.)
            {
              if (CurVertex.IsSame(StartVertex)
                  && (Uperiod == 0. || Abs(StartPoint.X() - CurPoint.X()) < Uperiod / 2)
                  && (Vperiod == 0. || Abs(StartPoint.Y() - CurPoint.Y()) < Vperiod / 2))
                break; // end of wire

              ReconstructMissedSeam(RemovedEdges,
                                    F_RefFace,
                                    CurEdge,
                                    CurVertex,
                                    CurPoint,
                                    Uperiod,
                                    Vperiod,
                                    NextEdge,
                                    NextPoint);
              if (NextEdge.IsNull())
              {
                return;
              }
            }
            else
              return;
          }
          CurPoint  = NextPoint;
          CurEdge   = NextEdge;
          CurVertex = TopExp::LastVertex(CurEdge, Standard_True); // with orientation
          BB.Add(aNewWire, CurEdge);
          UsedEdges.Add(CurEdge);
          RemoveEdgeFromMap(CurEdge, VEmap);
        } // for (;;)

        aNewWire.Closed(Standard_True);
        UsedEdges.Add(StartEdge);

        // Remove used edges from sequence
        Standard_Integer ind = 1;
        while (ind <= edges.Length())
        {
          if (UsedEdges.Contains(edges(ind)))
            edges.Remove(ind);
          else
            ind++;
        }

        // add just built wire to current face or save it in the sequence of wires
        Standard_Boolean EdgeOnBoundOfSurfFound = Standard_False;
        TopoDS_Iterator  itw(aNewWire);
        for (; itw.More(); itw.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
          if (BRep_Tool::IsClosed(anEdge, RefFace))
          {
            EdgeOnBoundOfSurfFound = Standard_True;
            break;
          }
        }
        if (EdgeOnBoundOfSurfFound) // this wire can not be a hole
        {
          TopLoc_Location      aLoc;
          Handle(Geom_Surface) aSurf = BRep_Tool::Surface(RefFace, aLoc);
          TopoDS_Face          aResult;
          BB.MakeFace(aResult, aSurf, aLoc, 0);
          BB.Add(aResult, aNewWire);
          aResult.Orientation(RefFaceOrientation);
          NewFaces.Append(aResult);
        }
        else // may be this wire is a hole
        {
          // split this wire if needed
          if (!SplittingVertices.IsEmpty())
            SplitWire(aNewWire, F_RefFace, SplittingVertices, NewWires);
          else
            NewWires.Append(aNewWire);
        }
      } // while (!edges.IsEmpty())

      // Build wires from internal edges
      TopTools_IndexedDataMapOfShapeListOfShape IntVEmap;
      for (Standard_Integer ii = 1; ii <= InternalEdges.Extent(); ii++)
        TopExp::MapShapesAndAncestors(InternalEdges(ii), TopAbs_VERTEX, TopAbs_EDGE, IntVEmap);
      TopTools_SequenceOfShape InternalWires;
      while (!InternalEdges.IsEmpty())
      {
        TopoDS_Edge aFirstEdge = TopoDS::Edge(InternalEdges(1));
        InternalEdges.RemoveFromIndex(1);
        TopoDS_Wire anInternalWire;
        BB.MakeWire(anInternalWire);
        BB.Add(anInternalWire, aFirstEdge);
        TopoDS_Edge EndEdges[2];
        EndEdges[0] = EndEdges[1] = aFirstEdge;
        TopoDS_Vertex VV[2];
        TopExp::Vertices(aFirstEdge, VV[0], VV[1]);
        for (;;)
        {
          if (VV[0].IsSame(VV[1])) // closed wire
            break;
          Standard_Boolean found = Standard_False;
          for (Standard_Integer ii = 0; ii < 2; ii++)
          {
            const TopTools_ListOfShape&        Elist = IntVEmap.FindFromKey(VV[ii]);
            TopTools_ListIteratorOfListOfShape itl(Elist);
            for (; itl.More(); itl.Next())
            {
              TopoDS_Edge anEdge = TopoDS::Edge(itl.Value());
              if (anEdge.IsSame(EndEdges[ii]))
                continue;
              found = Standard_True;
              InternalEdges.RemoveKey(anEdge);
              BB.Add(anInternalWire, anEdge);
              TopoDS_Vertex V1, V2;
              TopExp::Vertices(anEdge, V1, V2);
              VV[ii]       = (V1.IsSame(VV[ii])) ? V2 : V1;
              EndEdges[ii] = anEdge;
              break;
            }
          }
          if (!found) // end of open wire
            break;
        }
        InternalWires.Append(anInternalWire);
      }

      // Insert new faces instead of old ones
      if (NewFaces.IsEmpty())
      {
        // one face without seam
        TopLoc_Location      aLoc;
        Handle(Geom_Surface) aSurf = BRep_Tool::Surface(RefFace, aLoc);
        TopoDS_Face          aResult;
        BB.MakeFace(aResult, aSurf, aLoc, 0.);
        for (Standard_Integer ii = 1; ii <= NewWires.Length(); ii++)
          BB.Add(aResult, NewWires(ii));
        for (Standard_Integer ii = 1; ii <= InternalWires.Length(); ii++)
          BB.Add(aResult, InternalWires(ii));
        aResult.Orientation(RefFaceOrientation);
        myContext->Merge(faces, aResult);
        // Update the map Face-NewFace
        for (Standard_Integer jj = 1; jj <= faces.Length(); jj++)
          myFaceNewFace.Bind(faces(jj), aResult);
      }
      else if (NewFaces.Length() == 1)
      {
        TopoDS_Shape aNewFace = NewFaces(1).Oriented(TopAbs_FORWARD);
        for (Standard_Integer ii = 1; ii <= NewWires.Length(); ii++)
          BB.Add(aNewFace, NewWires(ii));
        for (Standard_Integer ii = 1; ii <= InternalWires.Length(); ii++)
          BB.Add(aNewFace, InternalWires(ii));
        myContext->Merge(faces, NewFaces(1));
        // Update the map Face-NewFace
        for (Standard_Integer jj = 1; jj <= faces.Length(); jj++)
          myFaceNewFace.Bind(faces(jj), NewFaces(1));
      }
      else
      {
        // Insert new wires and internal wires into correspondent faces
        InsertWiresIntoFaces(NewWires, NewFaces, RefFace);
        InsertWiresIntoFaces(InternalWires, NewFaces, RefFace);

        NCollection_Sequence<TopTools_MapOfShape> Emaps;
        for (Standard_Integer ii = 1; ii <= faces.Length(); ii++)
        {
          TopTools_MapOfShape aEmap;
          TopExp::MapShapes(faces(ii), aEmap);
          Emaps.Append(aEmap);
        }
        for (Standard_Integer ii = 1; ii <= NewFaces.Length(); ii++)
        {
          TopTools_SequenceOfShape facesForThisFace;
          TopTools_MapOfShape      UsedFaces;
          TopExp_Explorer          Explo(NewFaces(ii), TopAbs_EDGE);
          for (; Explo.More(); Explo.Next())
          {
            const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
            if (BRep_Tool::Degenerated(anEdge) || BRep_Tool::IsClosed(anEdge, RefFace))
              continue;
            Standard_Integer jj;
            for (jj = 1; jj <= Emaps.Length(); jj++)
              if (Emaps(jj).Contains(anEdge))
                break;
            if (UsedFaces.Add(faces(jj)))
              facesForThisFace.Append(faces(jj));
          }
          myContext->Merge(facesForThisFace, NewFaces(ii));
          // Update the map Face-NewFace
          for (Standard_Integer jj = 1; jj <= facesForThisFace.Length(); jj++)
            myFaceNewFace.Bind(facesForThisFace(jj), NewFaces(ii));
        }
      }
    } // if (faces.Length() > 1)
  } // end processing each face
}

//=================================================================================================

void ShapeUpgrade_UnifySameDomain::UnifyEdges()
{
  TopoDS_Shape aRes = myContext->Apply(myShape);
  // creating map of edge faces
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(aRes, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);
  // creating map of vertex edges
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgesVertex;
  TopExp::MapShapesAndUniqueAncestors(aRes, TopAbs_VERTEX, TopAbs_EDGE, aMapEdgesVertex);
  // creating map of vertex faces
  TopTools_IndexedDataMapOfShapeListOfShape aVFmap;
  TopExp::MapShapesAndUniqueAncestors(aRes, TopAbs_VERTEX, TopAbs_FACE, aVFmap);

  if (mySafeInputMode)
    UpdateMapOfShapes(myKeepShapes, myContext);

  // Sequence of the edges of the shape
  TopTools_SequenceOfShape aSeqEdges;
  const Standard_Integer   aNbE = aMapEdgeFaces.Extent();
  for (Standard_Integer i = 1; i <= aNbE; ++i)
    aSeqEdges.Append(aMapEdgeFaces.FindKey(i));

  // Prepare map of shared vertices (with the number of connected edges greater then 2)
  TopTools_MapOfShape aSharedVert;
  CheckSharedVertices(aSeqEdges, aMapEdgesVertex, myKeepShapes, aSharedVert);
  // Merge the edges avoiding removal of the shared vertices
  Standard_Boolean isMerged = MergeSeq(aSeqEdges, aVFmap, aSharedVert);
  // Collect faces to rebuild
  TopTools_IndexedMapOfShape aChangedFaces;
  if (isMerged)
  {
    for (Standard_Integer i = 1; i <= aNbE; ++i)
    {
      const TopoDS_Shape& aE = aMapEdgeFaces.FindKey(i);
      if (myContext->IsRecorded(aE))
      {
        TopTools_ListIteratorOfListOfShape it(aMapEdgeFaces(i));
        for (; it.More(); it.Next())
          aChangedFaces.Add(it.Value());
      }
    }
  }

  // fix changed faces and replace them in the local context
  constexpr Standard_Real aPrec = Precision::Confusion();
  for (Standard_Integer i = 1; i <= aChangedFaces.Extent(); i++)
  {
    TopoDS_Face aFace = TopoDS::Face(myContext->Apply(aChangedFaces.FindKey(i)));
    if (aFace.IsNull())
      continue;

    // for a planar face create and store pcurve of edge on face
    // to speed up all operations; but this is allowed only when non-safe mode in force
    if (!mySafeInputMode)
    {
      TopLoc_Location      aLoc;
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace, aLoc);
      aSurface                      = ClearRts(aSurface);
      if (aSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
      {
        TopTools_ListOfShape aLE;
        for (TopExp_Explorer anEx(aFace, TopAbs_EDGE); anEx.More(); anEx.Next())
          aLE.Append(anEx.Current());
        BRepLib::BuildPCurveForEdgesOnPlane(aLE, aFace);
      }
    }

    ShapeFix_Face sff(aFace);
    if (mySafeInputMode)
      sff.SetContext(myContext);
    sff.SetPrecision(aPrec);
    sff.SetMinTolerance(aPrec);
    sff.SetMaxTolerance(Max(1., aPrec * 1000.));
    sff.FixOrientationMode()         = 0;
    sff.FixAddNaturalBoundMode()     = 0;
    sff.FixIntersectingWiresMode()   = 0;
    sff.FixLoopWiresMode()           = 0;
    sff.FixSplitFaceMode()           = 0;
    sff.FixPeriodicDegeneratedMode() = 0;
    SetFixWireModes(sff);
    sff.Perform();
    TopoDS_Shape aNewFace = sff.Face();
    myContext->Replace(aFace, aNewFace);
  }

  if (aChangedFaces.Extent() > 0)
  {
    // fix changed shell and replace it in the local context
    TopoDS_Shape     aRes1     = myContext->Apply(aRes);
    Standard_Boolean isChanged = Standard_False;
    TopExp_Explorer  expsh;
    for (expsh.Init(aRes1, TopAbs_SHELL); expsh.More(); expsh.Next())
    {
      TopoDS_Shell           aShell = TopoDS::Shell(expsh.Current());
      Handle(ShapeFix_Shell) sfsh   = new ShapeFix_Shell;
      sfsh->FixFaceOrientation(aShell);
      TopoDS_Shape aNewShell = sfsh->Shell();
      if (!aNewShell.IsSame(aShell))
      {
        myContext->Replace(aShell, aNewShell);
        isChanged = Standard_True;
      }
    }
    if (isChanged)
      aRes1 = myContext->Apply(aRes1);
    myContext->Replace(myShape, aRes1);
  }

  myShape = myContext->Apply(myShape);
}

//=======================================================================
// function : Build
// purpose  : builds the resulting shape
//=======================================================================
void ShapeUpgrade_UnifySameDomain::Build()
{
  TopExp::MapShapesAndAncestors(myInitShape, TopAbs_EDGE, TopAbs_FACE, myEFmap);

  if (myUnifyFaces)
    UnifyFaces();
  if (myUnifyEdges)
    UnifyEdges();

  // Fill the history of modifications during the operation
  FillHistory();
}

//=======================================================================
// function : FillHistory
// purpose  : Fill the history of modifications during the operation
//=======================================================================
void ShapeUpgrade_UnifySameDomain::FillHistory()
{
  if (myHistory.IsNull())
    // History is not requested
    return;

  // Only Vertices, Edges and Faces can be modified during unification.
  // Thus, only these kind of shapes should be checked.

  // Get history from the context.
  // It contains all modifications of the operation. Some of these
  // modifications become not relevant and should be filtered.
  Handle(BRepTools_History) aCtxHistory = myContext->History();

  // Explore the history of the context and fill
  // the history of UnifySameDomain algorithm
  Handle(BRepTools_History) aUSDHistory = new BRepTools_History();

  // Map all Vertices, Edges, Faces and Solids in the input shape
  TopTools_IndexedMapOfShape aMapInputShape;
  TopExp::MapShapes(myInitShape, TopAbs_VERTEX, aMapInputShape);
  TopExp::MapShapes(myInitShape, TopAbs_EDGE, aMapInputShape);
  TopExp::MapShapes(myInitShape, TopAbs_FACE, aMapInputShape);
  TopExp::MapShapes(myInitShape, TopAbs_SOLID, aMapInputShape);

  // Map all Vertices, Edges, Faces and Solids in the result shape
  TopTools_IndexedMapOfShape aMapResultShapes;
  TopExp::MapShapes(myShape, TopAbs_VERTEX, aMapResultShapes);
  TopExp::MapShapes(myShape, TopAbs_EDGE, aMapResultShapes);
  TopExp::MapShapes(myShape, TopAbs_FACE, aMapResultShapes);
  TopExp::MapShapes(myShape, TopAbs_SOLID, aMapResultShapes);

  // Iterate on all input shapes and get their modifications
  Standard_Integer i, aNb = aMapInputShape.Extent();
  for (i = 1; i <= aNb; ++i)
  {
    const TopoDS_Shape& aS = aMapInputShape(i);

    // Check the shape itself to be present in the result
    if (aMapResultShapes.Contains(aS))
    {
      // The shape is present in the result as is, thus has not been modified
      continue;
    }

    // Check if the shape has been modified during the operation
    const TopTools_ListOfShape& aLSImages = aCtxHistory->Modified(aS);
    if (aLSImages.IsEmpty())
    {
      // The shape has not been modified and not present in the result,
      // thus it has been removed
      aUSDHistory->Remove(aS);
      continue;
    }

    // Check the images of the shape to be present in the result
    Standard_Boolean                   bRemoved = Standard_True;
    TopTools_ListIteratorOfListOfShape aItLSIm(aLSImages);
    for (; aItLSIm.More(); aItLSIm.Next())
    {
      const TopoDS_Shape& aSIm = aItLSIm.Value();
      if (aMapResultShapes.Contains(aSIm))
      {
        if (!aSIm.IsSame(aS))
          // Image is found in the result, thus the shape has been modified
          aUSDHistory->AddModified(aS, aSIm);
        bRemoved = Standard_False;
      }
    }

    if (bRemoved)
    {
      // No images are found in the result, thus the shape has been removed
      aUSDHistory->Remove(aS);
    }
  }

  // Merge the history of the operation into global history
  myHistory->Merge(aUSDHistory);
}

void SplitWire(const TopoDS_Wire&                theWire,
               const TopoDS_Face&                theFace,
               const TopTools_IndexedMapOfShape& theVmap,
               TopTools_SequenceOfShape&         theWireSeq)
{
  TopTools_DataMapOfShapeListOfShape aVEmap;

  TopTools_MapOfShape aEmap;
  TopoDS_Iterator     itw(theWire);
  for (; itw.More(); itw.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(itw.Value());
    if (!aEmap.Add(anEdge))
      continue;
    if (anEdge.Orientation() != TopAbs_FORWARD && anEdge.Orientation() != TopAbs_REVERSED)
      continue;

    const TopoDS_Vertex& aVertex = TopExp::FirstVertex(anEdge, Standard_True); // with orientation
    if (aVEmap.IsBound(aVertex))
      aVEmap(aVertex).Append(anEdge);
    else
    {
      TopTools_ListOfShape aElist;
      aElist.Append(anEdge);
      aVEmap.Bind(aVertex, aElist);
    }
  }

  BRep_Builder aBB;
  for (Standard_Integer ii = 1; ii <= theVmap.Extent(); ii++)
  {
    const TopoDS_Vertex&               anOrigin  = TopoDS::Vertex(theVmap(ii));
    TopTools_ListOfShape&              aBranches = aVEmap(anOrigin);
    TopTools_ListIteratorOfListOfShape anItl(aBranches);
    while (anItl.More())
    {
      TopoDS_Edge CurEdge = TopoDS::Edge(anItl.Value());
      aBranches.Remove(anItl);

      TopoDS_Wire aNewWire;
      aBB.MakeWire(aNewWire);
      for (;;)
      {
        aBB.Add(aNewWire, CurEdge);

        // clang-format off
        const TopoDS_Vertex& aVertex = TopExp::LastVertex (CurEdge, Standard_True); //with orientation
        // clang-format on
        if (aVertex.IsSame(anOrigin))
          break;

        if (!aVEmap.IsBound(aVertex))
          break;

        TopTools_ListOfShape& aElist = aVEmap(aVertex);
        if (aElist.Extent() == 0)
          break;

        if (aElist.Extent() == 1)
        {
          CurEdge = TopoDS::Edge(aElist.First());
          aElist.Clear();
        }
        else
        {
          Standard_Real        fpar, lpar;
          Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(CurEdge, theFace, fpar, lpar);
          Standard_Real        aParam  = (CurEdge.Orientation() == TopAbs_FORWARD) ? lpar : fpar;
          gp_Pnt2d             aPoint;
          gp_Vec2d             CurDir;
          aPCurve->D1(aParam, aPoint, CurDir);
          CurDir.Normalize();
          if (CurEdge.Orientation() == TopAbs_REVERSED)
            CurDir.Reverse();
          // choose the rightest direction - the smallest angle
          Standard_Real                      MinAngle = RealLast();
          TopoDS_Edge                        NextEdge;
          TopTools_ListIteratorOfListOfShape aLocalIter(aElist);
          for (; aLocalIter.More(); aLocalIter.Next())
          {
            const TopoDS_Edge& anEdge = TopoDS::Edge(aLocalIter.Value());
            aPCurve                   = BRep_Tool::CurveOnSurface(anEdge, theFace, fpar, lpar);
            aParam                    = (anEdge.Orientation() == TopAbs_FORWARD) ? fpar : lpar;
            gp_Vec2d aDir;
            aPCurve->D1(aParam, aPoint, aDir);
            aDir.Normalize();
            if (anEdge.Orientation() == TopAbs_REVERSED)
              aDir.Reverse();
            Standard_Real anAngle = CurDir.Angle(aDir);
            if (anAngle < MinAngle)
            {
              MinAngle = anAngle;
              NextEdge = anEdge;
            }
          }
          CurEdge = NextEdge;
          // Remove <CurEdge> from list
          for (aLocalIter.Initialize(aElist); aLocalIter.More(); aLocalIter.Next())
            if (CurEdge.IsSame(aLocalIter.Value()))
            {
              aElist.Remove(aLocalIter);
              break;
            }
        } // else (more than one edge)
      } // for (;;)
      theWireSeq.Append(aNewWire);
    } // while (anItl.More())
  }
}
