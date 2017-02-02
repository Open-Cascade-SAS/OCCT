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

#include <ChFi2d_AnaFilletAlgo.hxx>

#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>

#include <Standard_TypeMismatch.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <Geom_Circle.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <ProjLib.hxx>
#include <TopExp.hxx>
#include <ElSLib.hxx>

// Compute the flag: CW || CCW
static Standard_Boolean isCW(const BRepAdaptor_Curve& AC)
{
  const Standard_Real f = AC.FirstParameter();
  const Standard_Real l = AC.LastParameter();
  Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(AC.Curve().Curve());
  gp_Pnt start = AC.Value(f);
  gp_Pnt end = AC.Value(l);
  gp_Pnt center = AC.Circle().Location();
  gp_Ax3 plane = AC.Circle().Position();

  // Get point on circle at half angle
  gp_Pnt m;
  circle->D0(0.5 * (f + l), m);

  // Compare angles between vectors to middle point and to the end point.
  gp_Vec startv(center, start), endv(center, end), middlev(center, m);
  double middlea = startv.AngleWithRef(middlev, plane.Direction());
  while(middlea < 0.0)
    middlea += 2.0 * M_PI;
  double enda = startv.AngleWithRef(endv, plane.Direction());
  while(enda < 0.0)
    enda += 2.0 * M_PI;

  Standard_Boolean is_cw = middlea > enda ? Standard_True : Standard_False;
  return is_cw;
}

// Equality of points computed through square distance between the points.
static Standard_Boolean IsEqual(const gp_Pnt& p1, const gp_Pnt& p2)
{
  return p1.SquareDistance(p2) < Precision::SquareConfusion();
}
static Standard_Boolean IsEqual(const gp_Pnt2d& p1, const gp_Pnt2d& p2)
{
  return p1.SquareDistance(p2) < Precision::SquareConfusion();
}

// An empty constructor.
// Use the method Init() to initialize the class.
ChFi2d_AnaFilletAlgo::ChFi2d_AnaFilletAlgo()
: segment1(Standard_False),
  x11(0.0),
  y11(0.0),
  x12(0.0),
  y12(0.0),
  xc1(0.0),
  yc1(0.0),
  radius1(0.0),
  cw1(Standard_False),
  segment2(Standard_False),
  x21(0.0),
  y21(0.0),
  x22(0.0),
  y22(0.0),
  xc2(0.0),
  yc2(0.0),
  radius2(0.0),
  cw2(Standard_False)
{
}

// An constructor.
// It expects two edges having a common point of type:
// - segment
// - arc of circle.
ChFi2d_AnaFilletAlgo::ChFi2d_AnaFilletAlgo(const TopoDS_Wire& theWire, 
                                           const gp_Pln&      thePlane)
: plane(thePlane),
  segment1(Standard_False),
  x11(0.0),
  y11(0.0),
  x12(0.0),
  y12(0.0),
  xc1(0.0),
  yc1(0.0),
  radius1(0.0),
  cw1(Standard_False),
  segment2(Standard_False),
  x21(0.0),
  y21(0.0),
  x22(0.0),
  y22(0.0),
  xc2(0.0),
  yc2(0.0),
  radius2(0.0),
  cw2(Standard_False)
{
  Init(theWire, thePlane);
}

// A constructor.
// It expects two edges having a common point of type:
// - segment
// - arc of circle.
ChFi2d_AnaFilletAlgo::ChFi2d_AnaFilletAlgo(const TopoDS_Edge& theEdge1, 
                                           const TopoDS_Edge& theEdge2,
                                           const gp_Pln& thePlane)
: plane(thePlane),
  segment1(Standard_False),
  x11(0.0),
  y11(0.0),
  x12(0.0),
  y12(0.0),
  xc1(0.0),
  yc1(0.0),
  radius1(0.0),
  cw1(Standard_False),
  segment2(Standard_False),
  x21(0.0),
  y21(0.0),
  x22(0.0),
  y22(0.0),
  xc2(0.0),
  yc2(0.0),
  radius2(0.0),
  cw2(Standard_False)
{
  // Make a wire consisting of two edges.
  Init(theEdge1, theEdge2, thePlane);
}

// Initializes the class by a wire consisting of two edges.
void ChFi2d_AnaFilletAlgo::Init(const TopoDS_Wire& theWire, const gp_Pln& thePlane)
{
  plane = thePlane;
  TopoDS_Iterator itr(theWire);
  for (; itr.More(); itr.Next())
  {
    if (e1.IsNull())
      e1 = TopoDS::Edge(itr.Value());
    else if (e2.IsNull())
      e2 = TopoDS::Edge(itr.Value());
  }
  if (e1.IsNull() || e2.IsNull())
    throw Standard_TypeMismatch("The algorithm expects a wire consisting of two linear or circular edges.");

  // Left neighbour.
  BRepAdaptor_Curve AC1(e1);
  if (AC1.GetType() != GeomAbs_Line && AC1.GetType() != GeomAbs_Circle)
    throw Standard_TypeMismatch("A segment or an arc of circle is expected.");

  TopoDS_Vertex v1, v2;
  TopExp::Vertices(e1, v1, v2, Standard_True);
  if (v1.IsNull() || v2.IsNull())
    throw Standard_Failure("An infinite edge.");

  gp_Pnt P1 = BRep_Tool::Pnt(v1);
  gp_Pnt P2 = BRep_Tool::Pnt(v2);
  gp_Pnt2d p1 = ProjLib::Project(thePlane, P1);
  gp_Pnt2d p2 = ProjLib::Project(thePlane, P2);
  p1.Coord(x11, y11);
  p2.Coord(x12, y12);

  segment1 = true;
  if (AC1.GetType() == GeomAbs_Circle)
  {
    segment1 = false;
    gp_Circ c = AC1.Circle();

    gp_Pnt2d loc = ProjLib::Project(thePlane, c.Location());
    loc.Coord(xc1, yc1);

    radius1 = c.Radius();
    cw1 = isCW(AC1);
  }

  // Right neighbour.
  BRepAdaptor_Curve AC2(e2);
  if (AC2.GetType() != GeomAbs_Line && AC2.GetType() != GeomAbs_Circle)
    throw Standard_TypeMismatch("A segment or an arc of circle is expected.");

  TopExp::Vertices(e2, v1, v2, Standard_True);
  if (v1.IsNull() || v2.IsNull())
    throw Standard_Failure("An infinite edge.");

  P1 = BRep_Tool::Pnt(v1);
  P2 = BRep_Tool::Pnt(v2);
  p1 = ProjLib::Project(thePlane, P1);
  p2 = ProjLib::Project(thePlane, P2);
  p1.Coord(x21, y21);
  p2.Coord(x22, y22);

  segment2 = true;
  if (AC2.GetType() == GeomAbs_Circle)
  {
    segment2 = false;
    gp_Circ c = AC2.Circle();

    gp_Pnt2d loc = ProjLib::Project(thePlane, c.Location());
    loc.Coord(xc2, yc2);

    radius2 = c.Radius();
    cw2 = isCW(AC2);
  }
}

// Initializes the class by two edges.
void ChFi2d_AnaFilletAlgo::Init(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2, 
                                const gp_Pln& thePlane)
{
  // Make a wire consisting of two edges.

  // Get common point.
  TopoDS_Vertex v11, v12, v21, v22;
  TopExp::Vertices(theEdge1, v11, v12, Standard_True);
  TopExp::Vertices(theEdge2, v21, v22, Standard_True);
  if (v11.IsNull() || v12.IsNull() || v21.IsNull() || v22.IsNull())
    throw Standard_Failure("An infinite edge.");

  gp_Pnt p11 = BRep_Tool::Pnt(v11);
  gp_Pnt p12 = BRep_Tool::Pnt(v12);
  gp_Pnt p21 = BRep_Tool::Pnt(v21);
  gp_Pnt p22 = BRep_Tool::Pnt(v22);

  gp_Pnt pcommon;
  if (IsEqual(p11, p21) || IsEqual(p11, p22))
  {
    pcommon = p11;
  }
  else if (IsEqual(p12, p21) || IsEqual(p12, p22))
  {
    pcommon = p12;
  }
  else
    throw Standard_Failure("The edges have no common point.");

  // Reverse the edges in case of need (to construct a wire).
  Standard_Boolean is1stReversed(Standard_False), is2ndReversed(Standard_False);
  if (IsEqual(pcommon, p11))
    is1stReversed = Standard_True;
  else if (IsEqual(pcommon, p22))
    is2ndReversed = Standard_True;

  // Make a wire.
  BRepBuilderAPI_MakeWire mkWire;
  if (is1stReversed)
    mkWire.Add(TopoDS::Edge(theEdge1.Reversed()));
  else
    mkWire.Add(theEdge1);
  if (is2ndReversed)
    mkWire.Add(TopoDS::Edge(theEdge2.Reversed()));
  else
    mkWire.Add(theEdge2);
  if (!mkWire.IsDone())
    throw Standard_Failure("Can't make a wire.");

  const TopoDS_Wire& W = mkWire.Wire();
  Init(W, thePlane);
}

// Calculates a fillet.
Standard_Boolean ChFi2d_AnaFilletAlgo::Perform(const Standard_Real radius)
{
  Standard_Boolean bRet(false);
  if (e1.IsNull() || e2.IsNull() ||
      radius < Precision::Confusion())
  {
    return bRet;
  }

  // Fillet definition.
  Standard_Real xc = 0.0, yc = 0.0;
  Standard_Real start = 0.0, end = 0.0;             // parameters on neighbours
  Standard_Real xstart = DBL_MAX, ystart = DBL_MAX; // point on left neighbour
  Standard_Real xend = DBL_MAX, yend = DBL_MAX;     // point on right neighbour
  Standard_Boolean cw = Standard_False;

  // Analytical algorithm works for non-intersecting arcs only.
  // Check arcs on self-intersection.
  Standard_Boolean isCut(Standard_False);
  if (!segment1 || !segment2)
  {
    BRepBuilderAPI_MakeWire mkWire(e1, e2);
    if (mkWire.IsDone())
    {
      const TopoDS_Wire& W = mkWire.Wire();
      BRepBuilderAPI_MakeFace mkFace(plane);
      if (mkFace.IsDone())
      {
        const TopoDS_Face& F = mkFace.Face();
        ShapeAnalysis_Wire analyzer(W, F, Precision::Confusion());
        if (analyzer.CheckSelfIntersection() == Standard_True)
        {
          // Cut the edges at the point of intersection.
          isCut = Standard_True;
          if (!Cut(plane, e1, e2))
          {
            return Standard_False;
          }
        }
      }
    }
  }// a case of segment - segment

  // Choose the case.
  BRepAdaptor_Curve AC1(e1), AC2(e2);
  if (segment1 && segment2)
  {
    bRet = SegmentFilletSegment(radius, xc, yc, cw, start, end);
  }
  else if (segment1 && !segment2)
  {
    bRet = SegmentFilletArc(radius, xc, yc, cw, start, end, xend, yend);
  }
  else if (!segment1 && segment2)
  {
    bRet = ArcFilletSegment(radius, xc, yc, cw, start, end, xstart, ystart);
  }
  else if (!segment1 && !segment2)
  {
    bRet = ArcFilletArc(radius, xc, yc, cw, start, end);
  }

  if (!bRet)
    return Standard_False;

  // Invert the fillet for left-handed plane.
  if (plane.Position().Direct() == Standard_False)
    cw = !cw;

  // Construct a fillet.
  // Make circle.
  gp_Pnt center = ElSLib::Value(xc, yc, plane);
  const gp_Dir& normal = plane.Position().Direction();
  gp_Circ circ(gp_Ax2(center, cw ? -normal : normal), radius);

  // Fillet may only shrink a neighbour edge, it can't prolongate it.
  const Standard_Real delta1 = AC1.LastParameter() - AC1.FirstParameter();
  const Standard_Real delta2 = AC2.LastParameter() - AC2.FirstParameter();
  if (!isCut && (start > delta1 || end > delta2))
  {
    // Check a case when a neighbour edge almost disappears: 
    // try to reduce the fillet radius for a little (1.e-5 mm).
    const Standard_Real little = 100.0 * Precision::Confusion();
    const Standard_Real d1 = fabs(start - delta1);
    const Standard_Real d2 = fabs(end   - delta2);
    if (d1 < little || d2 < little)
    {
      if (segment1 && segment2)
      {
        bRet = SegmentFilletSegment(radius - little, xc, yc, cw, start, end);
      }
      else if (segment1 && !segment2)
      {
        bRet = SegmentFilletArc(radius - little, xc, yc, cw, start, end, xend, yend);
      }
      else if (!segment1 && segment2)
      {
        bRet = ArcFilletSegment(radius - little, xc, yc, cw, start, end, xstart, ystart);
      }
      else if (!segment1 && !segment2)
      {
        bRet = ArcFilletArc(radius - little, xc, yc, cw, start, end);
      }
      if (bRet)
      {
        // Invert the fillet for left-handed planes.
        if (plane.Position().Direct() == Standard_False)
          cw = !cw;

        // Make the circle again.
        center = ElSLib::Value(xc, yc, plane);
        circ.SetLocation(center);
        circ.SetRadius(radius - little);
      }
      else
      {
        return Standard_False;
      }
    }
    else
    {
      return Standard_False;
    }
  }
  if (bRet)
  {
    // start: (xstart, ystart) - pstart.
    gp_Pnt pstart;
    if (xstart != DBL_MAX)
    {
      pstart = ElSLib::Value(xstart, ystart, plane);
    }
    else
    {
      if (e1.Orientation() == TopAbs_FORWARD)
        pstart = AC1.Value(AC1.LastParameter() - start);
      else
        pstart = AC1.Value(AC1.FirstParameter() + start);
    }
    // end: (xend, yend) -> pend.
    gp_Pnt pend;
    if (xend != DBL_MAX)
    {
      pend = ElSLib::Value(xend, yend, plane);
    }
    else
    {
      if (e2.Orientation() == TopAbs_FORWARD)
        pend = AC2.Value(AC2.FirstParameter() + end);
      else
        pend = AC2.Value(AC2.LastParameter() - end);
    }

    // Make arc.
    BRepBuilderAPI_MakeEdge mkEdge(circ, pstart, pend);
    bRet = mkEdge.IsDone();
    if (bRet)
    {
      fillet = mkEdge.Edge();

      // Limit the neighbours.
      // Left neighbour.
      gp_Pnt p1, p2;
      shrinke1.Nullify();
      if (e1.Orientation() == TopAbs_FORWARD)
      {
        p1 = AC1.Value(AC1.FirstParameter());
        p2 = pstart;
      }
      else
      {
        p1 = pstart;
        p2 = AC1.Value(AC1.LastParameter());
      }
      if (segment1)
      {
        BRepBuilderAPI_MakeEdge mkSegment1;
        mkSegment1.Init(AC1.Curve().Curve(), p1, p2);
        if (mkSegment1.IsDone())
          shrinke1 = mkSegment1.Edge();
      }
      else
      {
        BRepBuilderAPI_MakeEdge mkCirc1;
        mkCirc1.Init(AC1.Curve().Curve(), p1, p2);
        if (mkCirc1.IsDone())
          shrinke1 = mkCirc1.Edge();
      }
      
      // Right neighbour.
      shrinke2.Nullify();
      if (e1.Orientation() == TopAbs_FORWARD)
      {
        p1 = pend;
        p2 = AC2.Value(AC2.LastParameter());
      }
      else
      {
        p1 = AC2.Value(AC2.FirstParameter());
        p2 = pend;
      }
      if (segment2)
      {
        BRepBuilderAPI_MakeEdge mkSegment2;
        mkSegment2.Init(AC2.Curve().Curve(), p1, p2);
        if (mkSegment2.IsDone())
          shrinke2 = mkSegment2.Edge();
      }
      else
      {
        BRepBuilderAPI_MakeEdge mkCirc2;
        mkCirc2.Init(AC2.Curve().Curve(), p1, p2);
        if (mkCirc2.IsDone())
          shrinke2 = mkCirc2.Edge();
      }

      bRet = !shrinke1.IsNull() && !shrinke2.IsNull();
    }// fillet edge is done
  }// shrinking is good

  return bRet;
}

// Retrieves a result (fillet and shrinked neighbours).
const TopoDS_Edge& ChFi2d_AnaFilletAlgo::Result(TopoDS_Edge& theE1, TopoDS_Edge& theE2)
{
  theE1 = shrinke1;
  theE2 = shrinke2;
  return fillet;
}

// WW5 method to compute fillet.
// It returns a constructed fillet definition:
//     center point (xc, yc)
//     point on the 1st segment (xstart, ystart)
//     point on the 2nd segment (xend, yend)
//     is the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
Standard_Boolean ChFi2d_AnaFilletAlgo::SegmentFilletSegment(const Standard_Real radius, 
                                                            Standard_Real& xc, Standard_Real& yc, 
                                                            Standard_Boolean& cw,
                                                            Standard_Real& start, Standard_Real& end)
{
  // Make normalized vectors at p12.
  gp_Pnt2d p11(x11, y11);
  gp_Pnt2d p12(x12, y12);
  gp_Pnt2d p22(x22, y22);

  // Check length of segments.
  if (IsEqual(p12, p11) || IsEqual(p12, p22))
  {
    return Standard_False;
  }

  // Make vectors.
  gp_Vec2d v1(p12, p11);
  gp_Vec2d v2(p12, p22);
  v1.Normalize();
  v2.Normalize();

  // Make bisectrissa.
  gp_Vec2d bisec = 0.5 * (v1 + v2);

  // Check bisectrissa.
  if (bisec.SquareMagnitude() < Precision::SquareConfusion())
    return Standard_False;

  // Normalize the bisectrissa.
  bisec.Normalize();

  // Angle at bisectrissa.
  Standard_Real beta = v1.Angle(bisec);

  // Length along the bisectrissa till the center of fillet.
  Standard_Real L = radius / sin(fabs(beta));

  // Center point of fillet.
  gp_Pnt2d pc = p12.Translated(L * bisec);
  pc.Coord(xc, yc);

  // Shrinking length along segments.
  start = sqrt(L * L - radius * radius);
  end = start;

  // Orientation of fillet.
  cw = beta > 0.0;
  return Standard_True;
}

// A function constructs a fillet between a segment and an arc.
Standard_Boolean ChFi2d_AnaFilletAlgo::SegmentFilletArc(const Standard_Real radius, 
                                                        Standard_Real& xc, Standard_Real& yc, 
                                                        Standard_Boolean& cw,
                                                        Standard_Real& start, Standard_Real& end, 
                                                        Standard_Real& xend, Standard_Real& yend)
{
  // Make a line parallel to the segment at the side of center point of fillet.
  // This side may be defined through making a bisectrissa for vectors at p12 (or p21).

  // Make 2D points.
  gp_Pnt2d p12(x12, y12);
  gp_Pnt2d p11(x11, y11);
  gp_Pnt2d pc2(xc2, yc2);

  // Check length of segment.
  if (p11.SquareDistance(p12) < gp::Resolution())
    return Standard_False;

  // Make 2D vectors.
  gp_Vec2d v1(p12, p11);
  gp_Vec2d v2(p12, pc2);

  // Rotate the arc vector to become tangential at p21.
  if (cw2)
    v2.Rotate(+M_PI_2);
  else
    v2.Rotate(-M_PI_2);

  // If vectors coincide (segment and arc are tangent),
  // the algorithm doesn't work...
  Standard_Real angle = v1.Angle(v2);
  if (fabs(angle) < Precision::Angular())
    return Standard_False;

  // Make a bissectrisa of vectors at p12.
  v2.Normalize();
  v1.Normalize();
  gp_Vec2d bisec = 0.5 * (v1 + v2);

  // If segment and arc look in opposite direction, 
  // no fillet is possible.
  if (bisec.SquareMagnitude() < gp::Resolution())
    return Standard_False;

  // Define an appropriate point to choose center of fillet.
  bisec.Normalize();
  gp_Pnt2d nearp = p12.Translated(radius * bisec);
  gp_Lin2d nearl(p12, bisec);

  // Make a line parallel to segment and
  // passing near the "near" point.
  gp_Vec2d d1(v1);
  gp_Lin2d line(p11, -d1);
  d1.Rotate(M_PI_2);
  line.Translate(radius * d1);
  if (line.Distance(nearp) > radius)
    line.Translate(-2.0 * radius * d1);

  // Make a circle of radius of the arc +/- fillet radius.
  gp_Ax2d axes(pc2, gp::DX2d());
  gp_Circ2d circ(axes, radius2 + radius);
  if (radius2 > radius && circ.Distance(nearp) > radius)
    circ.SetRadius(radius2 - radius);

  // Calculate intersection of the line and the circle.
  IntAna2d_AnaIntersection intersector(line, circ);
  if (!intersector.IsDone() || !intersector.NbPoints())
    return Standard_False;

  // Find center point of fillet.
  Standard_Integer i;
  Standard_Real minDist = DBL_MAX;
  for (i = 1; i <= intersector.NbPoints(); ++i)
  {
    const IntAna2d_IntPoint& intp = intersector.Point(i);
    const gp_Pnt2d& p = intp.Value();

    Standard_Real d = nearl.Distance(p);
    if (d < minDist)
    {
      minDist = d;
      p.Coord(xc, yc);
    }
  }

  // Shrink of segment.
  gp_Pnt2d pc(xc, yc);
  Standard_Real L2 = pc.SquareDistance(p12);
  const Standard_Real Rf2 = radius * radius;
  start = sqrt(L2 - Rf2);

  // Shrink of arc.
  gp_Vec2d pcc(pc2, pc);
  end = fabs(gp_Vec2d(pc2, p12).Angle(pcc));

  // Duplicate the information on shrink the arc:
  // calculate a point on the arc coinciding with the end of fillet.
  line.SetLocation(pc2);
  line.SetDirection(pcc);
  circ.SetLocation(pc2);
  circ.SetRadius(radius2);
  intersector.Perform(line, circ);
  if (!intersector.IsDone() || !intersector.NbPoints())
    return Standard_False;

  xend = DBL_MAX;
  yend = DBL_MAX;
  for (i = 1; i <= intersector.NbPoints(); ++i)
  {
    const IntAna2d_IntPoint& intp = intersector.Point(i);
    const gp_Pnt2d& p = intp.Value();

    const Standard_Real d2 = p.SquareDistance(pc);
    if (fabs(d2 - Rf2) < Precision::Confusion())
    {
      p.Coord(xend, yend);
      break;
    }
  }

  // Orientation of the fillet.
  angle = v1.Angle(v2);
  cw = angle > 0.0;
  return Standard_True;
}

// A function constructs a fillet between an arc and a segment.
Standard_Boolean ChFi2d_AnaFilletAlgo::ArcFilletSegment(const Standard_Real radius, 
                                                        Standard_Real& xc, Standard_Real& yc, 
                                                        Standard_Boolean& cw,
                                                        Standard_Real& start, Standard_Real& end, 
                                                        Standard_Real& xstart, Standard_Real& ystart)
{
  // Make a line parallel to the segment at the side of center point of fillet.
  // This side may be defined through making a bisectrissa for vectors at p12 (or p21).

  // Make 2D points.
  gp_Pnt2d p12(x12, y12);
  gp_Pnt2d p22(x22, y22);
  gp_Pnt2d pc1(xc1, yc1);

  // Check length of segment.
  if (p12.SquareDistance(p22) < gp::Resolution())
    return Standard_False;

  // Make 2D vectors.
  gp_Vec2d v1(p12, pc1);
  gp_Vec2d v2(p12, p22);

  // Rotate the arc vector to become tangential at p21.
  if (cw1)
    v1.Rotate(-M_PI_2);
  else
    v1.Rotate(+M_PI_2);

  // If vectors coincide (segment and arc are tangent),
  // the algorithm doesn't work...
  Standard_Real angle = v1.Angle(v2);
  if (fabs(angle) < Precision::Angular())
    return Standard_False;

  // Make a bisectrissa of vectors at p12.
  v1.Normalize();
  v2.Normalize();
  gp_Vec2d bisec = 0.5 * (v1 + v2);

  // If segment and arc look in opposite direction, 
  // no fillet is possible.
  if (bisec.SquareMagnitude() < gp::Resolution())
    return Standard_False;

  // Define an appropriate point to choose center of fillet.
  bisec.Normalize();
  gp_Pnt2d nearPoint = p12.Translated(radius * bisec);
  gp_Lin2d nearLine(p12, bisec);

  // Make a line parallel to segment and
  // passing near the "near" point.
  gp_Vec2d aD2Vec(v2);
  gp_Lin2d line(p22, -aD2Vec);
  aD2Vec.Rotate(M_PI_2);
  line.Translate(radius * aD2Vec);
  if (line.Distance(nearPoint) > radius)
    line.Translate(-2.0 * radius * aD2Vec);

  // Make a circle of radius of the arc +/- fillet radius.
  gp_Ax2d axes(pc1, gp::DX2d());
  gp_Circ2d circ(axes, radius1 + radius);
  if (radius1 > radius && circ.Distance(nearPoint) > radius)
    circ.SetRadius(radius1 - radius);

  // Calculate intersection of the line and the big circle.
  IntAna2d_AnaIntersection intersector(line, circ);
  if (!intersector.IsDone() || !intersector.NbPoints())
    return Standard_False;

  // Find center point of fillet.
  Standard_Integer i;
  Standard_Real minDist = DBL_MAX;
  for (i = 1; i <= intersector.NbPoints(); ++i)
  {
    const IntAna2d_IntPoint& intp = intersector.Point(i);
    const gp_Pnt2d& p = intp.Value();

    Standard_Real d = nearLine.Distance(p);
    if (d < minDist)
    {
      minDist = d;
      p.Coord(xc, yc);
    }
  }

  // Shrink of segment.
  gp_Pnt2d pc(xc, yc);
  Standard_Real L2 = pc.SquareDistance(p12);
  const Standard_Real Rf2 = radius * radius;
  end = sqrt(L2 - Rf2);

  // Shrink of arc.
  gp_Vec2d pcc(pc1, pc);
  start = fabs(gp_Vec2d(pc1, p12).Angle(pcc));

  // Duplicate the information on shrink the arc:
  // calculate a point on the arc coinciding with the start of fillet.
  line.SetLocation(pc1);
  line.SetDirection(pcc);
  circ.SetLocation(pc1);
  circ.SetRadius(radius1);
  intersector.Perform(line, circ);
  if (!intersector.IsDone() || !intersector.NbPoints())
    return Standard_False;

  xstart = DBL_MAX;
  ystart = DBL_MAX;
  for (i = 1; i <= intersector.NbPoints(); ++i)
  {
    const IntAna2d_IntPoint& intp = intersector.Point(i);
    const gp_Pnt2d& p = intp.Value();

    const Standard_Real d2 = p.SquareDistance(pc);
    if (fabs(d2 - Rf2) < Precision::SquareConfusion())
    {
      p.Coord(xstart, ystart);
      break;
    }
  }

  // Orientation of the fillet.
  angle = v2.Angle(v1);
  cw = angle < 0.0;
  return Standard_True;
}

// WW5 method to compute fillet: arc - arc.
// It returns a constructed fillet definition:
//     center point (xc, yc)
//     shrinking parameter of the 1st circle (start)
//     shrinking parameter of the 2nd circle (end)
//     if the arc of fillet clockwise (cw = true) or counterclockwise (cw = false).
Standard_Boolean ChFi2d_AnaFilletAlgo::ArcFilletArc(const Standard_Real radius, 
                                                    Standard_Real& xc, Standard_Real& yc, 
                                                    Standard_Boolean& cw,
                                                    Standard_Real& start, Standard_Real& end)
{
  // Make points.
  const gp_Pnt2d pc1(xc1, yc1);
  const gp_Pnt2d pc2(xc2, yc2);
  const gp_Pnt2d p12(x12, y12);

  // Make vectors at p12.
  gp_Vec2d v1(pc1, p12);
  gp_Vec2d v2(pc2, p12);

  // Rotate the vectors so that they are tangent to circles at p12.
  if (cw1)
    v1.Rotate(+M_PI_2);
  else
    v1.Rotate(-M_PI_2);
  if (cw2)
    v2.Rotate(-M_PI_2);
  else
    v2.Rotate(+M_PI_2);

  // Make a "check" point for choosing an offset circle.
  v1.Normalize();
  v2.Normalize();
  gp_Vec2d bisec = 0.5 * (v1 + v2);
  if (bisec.SquareMagnitude() < gp::Resolution())
    return Standard_False;

  const gp_Pnt2d checkp = p12.Translated(radius * bisec);
  const gp_Lin2d checkl(p12, bisec);

  // Make two circles of radius r1 +/- r and r2 +/- r
  // with center point equal to pc1 and pc2.
  // Arc 1.
  gp_Ax2d axes(pc1, gp::DX2d());
  gp_Circ2d c1(axes, radius1 + radius);
  if (radius1 > radius && c1.Distance(checkp) > radius)
    c1.SetRadius(radius1 - radius);
  // Arc 2.
  axes.SetLocation(pc2);
  gp_Circ2d c2(axes, radius2 + radius);
  if (radius2 > radius && c2.Distance(checkp) > radius)
    c2.SetRadius(radius2 - radius);

  // Calculate an intersection point of these two circles
  // and choose the one closer to the "check" point.
  IntAna2d_AnaIntersection intersector(c1, c2);
  if (!intersector.IsDone() || !intersector.NbPoints())
    return Standard_False;

  // Find center point of fillet.
  gp_Pnt2d pc;
  Standard_Real minDist = DBL_MAX;
  for (int i = 1; i <= intersector.NbPoints(); ++i)
  {
    const IntAna2d_IntPoint& intp = intersector.Point(i);
    const gp_Pnt2d& p = intp.Value();

    Standard_Real d = checkp.SquareDistance(p);
    if (d < minDist)
    {
      minDist = d;
      pc = p;
    }
  }
  pc.Coord(xc, yc);

  // Orientation of fillet.
  Standard_Real angle = v1.Angle(v2);
  if (fabs(angle) < Precision::Angular())
  {
    angle = gp_Vec2d(pc, pc1).Angle(gp_Vec2d(pc, pc2));
    cw = angle < 0.0;
  }
  else
  {
    cw = angle > 0.0;
  }

  // Shrinking of circles.
  start = fabs(gp_Vec2d(pc1, p12).Angle(gp_Vec2d(pc1, pc)));
  end = fabs(gp_Vec2d(pc2, p12).Angle(gp_Vec2d(pc2, pc)));
  return Standard_True;
}

// Cuts intersecting edges of a contour.
Standard_Boolean ChFi2d_AnaFilletAlgo::Cut(const gp_Pln& thePlane, TopoDS_Edge& theE1, TopoDS_Edge& theE2)
{
  gp_Pnt p;
  Standard_Boolean found(Standard_False);
  Standard_Real param1 = 0.0, param2 = 0.0;
  Standard_Real f1, l1, f2, l2;
  Handle(Geom_Curve) c1 = BRep_Tool::Curve(theE1, f1, l1);
  Handle(Geom_Curve) c2 = BRep_Tool::Curve(theE2, f2, l2);
  GeomAPI_ExtremaCurveCurve extrema(c1, c2, f1, l1, f2, l2);
  if (extrema.NbExtrema())
  {
    Standard_Integer i, nb = extrema.NbExtrema();
    for (i = 1; i <= nb; ++i)
    {
      const Standard_Real d = extrema.Distance(i);
      if (d < Precision::Confusion())
      {
        extrema.Parameters(i, param1, param2);
        if (fabs(l1 - param1) > Precision::Confusion() &&
            fabs(f2 - param2) > Precision::Confusion())
        {
          found = Standard_True;
          extrema.Points(i, p, p);
          break;
        }
      }
    }
  }

  if (found)
  {
    BRepBuilderAPI_MakeEdge mkEdge1(c1, f1, param1);
    if (mkEdge1.IsDone())
    {
      theE1 = mkEdge1.Edge();

      BRepBuilderAPI_MakeEdge mkEdge2(c2, param2, l2);
      if (mkEdge2.IsDone())
      {
        theE2 = mkEdge2.Edge();

        gp_Pnt2d p2d = ProjLib::Project(thePlane, p);
        p2d.Coord(x12, y12);
        x21 = x12;
        y21 = y12;
        return Standard_True;
      }
    }
  }
  return Standard_False;
}
