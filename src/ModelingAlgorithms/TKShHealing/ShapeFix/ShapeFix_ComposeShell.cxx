// Created on: 1999-04-27
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

//    pdn  01.06.99 S4205: handling not-SameRange edges
//    abv  22.07.99 implementing patch indices
//    svv  10.01.00 porting on DEC

#include <Bnd_Box2d.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ElementarySurface.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeBuild_Vertex.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_WireSegment.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_ComposeShell, ShapeFix_Root)

//=================================================================================================

ShapeFix_ComposeShell::ShapeFix_ComposeShell()
    : myOrient(TopAbs_FORWARD),
      myStatus(0),
      myUResolution(RealLast()),
      myVResolution(RealLast()),
      myInvertEdgeStatus(true),
      myClosedMode(false),
      myUClosed(false),
      myVClosed(false),
      myUPeriod(0.),
      myVPeriod(0.)
{
  myTransferParamTool = new ShapeAnalysis_TransferParametersProj;
}

//=================================================================================================

void ShapeFix_ComposeShell::Init(const occ::handle<ShapeExtend_CompositeSurface>& Grid,
                                 const TopLoc_Location&                           L,
                                 const TopoDS_Face&                               Face,
                                 const double                                     Prec)
{
  myGrid    = Grid;
  myUClosed = myGrid->IsUClosed();
  myVClosed = myGrid->IsVClosed();
  myUPeriod = myGrid->UJointValue(myGrid->NbUPatches() + 1) - myGrid->UJointValue(1);
  myVPeriod = myGrid->VJointValue(myGrid->NbVPatches() + 1) - myGrid->VJointValue(1);

  //  DTK-CKY 100531 : protection against very thin face
  //  Test "isclosed" should be filtered on the overall (non trimmed) surface, must be closed
  occ::handle<Geom_Surface> theSurface = BRep_Tool::Surface(Face, myLoc);
  // avoid false detection of 'Closed' on very thin faces
  if (theSurface->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
  {
    myUClosed = myUClosed && theSurface->IsUClosed();
    myVClosed = myVClosed && theSurface->IsVClosed();
  }
  else
  {
    double U0, U1, V0, V1, GU0 = 0., GU1 = 0., GV0 = 0., GV1 = 0.;
    theSurface->Bounds(U0, U1, V0, V1);
    if (::Precision::IsInfinite(U0) || ::Precision::IsInfinite(U1) || ::Precision::IsInfinite(V0)
        || ::Precision::IsInfinite(V1))
      BRepTools::UVBounds(Face, GU0, GU1, GV0, GV1);
    if (myUClosed)
    {
      if (::Precision::IsInfinite(V0))
        V0 = GV0;
      if (::Precision::IsInfinite(V1))
        V1 = GV1;
      gp_Pnt P0 = theSurface->Value(U0, (V0 + V1) / 2.);
      gp_Pnt P1 = theSurface->Value(U1, (V0 + V1) / 2.);
      if (P0.Distance(P1) > Precision::Confusion() * 10)
        myUClosed = false;
    }
    if (myVClosed)
    {
      if (::Precision::IsInfinite(U0))
        U0 = GU0;
      if (::Precision::IsInfinite(U1))
        U1 = GU1;
      gp_Pnt P0 = theSurface->Value((U0 + U1) / 2., V0);
      gp_Pnt P1 = theSurface->Value((U0 + U1) / 2., V1);
      if (P0.Distance(P1) > Precision::Confusion() * 10)
        myVClosed = false;
    }
  }
  //  DTK-CKY 100531 end

  myLoc = L;
  // smh#8
  TopoDS_Shape tmpF = Face.Oriented(TopAbs_FORWARD);
  myFace            = TopoDS::Face(tmpF); // for correct dealing with seams
  myOrient          = Face.Orientation();
  SetPrecision(Prec);
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  // Compute resolution (checking in 2d is necessary for splitting
  // degenerated edges and avoiding NotClosed)
  myUResolution = myVResolution = RealLast();
  for (int i = 1; i <= myGrid->NbUPatches(); i++)
  {
    double uRange = myGrid->UJointValue(i + 1) - myGrid->UJointValue(i);
    for (int j = 1; j <= myGrid->NbVPatches(); j++)
    {
      double vRange = myGrid->VJointValue(j + 1) - myGrid->VJointValue(j);
      double u1, u2, v1, v2;
      myGrid->Patch(i, j)->Bounds(u1, u2, v1, v2);
      GeomAdaptor_Surface GAS(myGrid->Patch(i, j));
      double              ures = GAS.UResolution(1.) * uRange / (u2 - u1);
      double              vres = GAS.VResolution(1.) * vRange / (v2 - v1);
      if (ures > 0. && myUResolution > ures)
        myUResolution = ures;
      if (vres > 0. && myVResolution > vres)
        myVResolution = vres;
    }
  }
  if (myUResolution == RealLast())
    myUResolution = ::Precision::Parametric(1.);
  if (myVResolution == RealLast())
    myVResolution = ::Precision::Parametric(1.);
}

//=================================================================================================

bool ShapeFix_ComposeShell::Perform()
{
  myStatus           = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myInvertEdgeStatus = false;

  NCollection_Sequence<ShapeFix_WireSegment> seqw; // working data: wire segments

  // Init seqw by initial set of wires (with corresponding orientation)
  LoadWires(seqw);
  if (seqw.Length() == 0)
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL6);
    return false;
  }

  // Split edges in the wires by grid and add internal segments of grid (parts of cutting lines)
  SplitByGrid(seqw);

  // Split all the wires into segments by common vertices (intersections)
  BreakWires(seqw);

  // Then, collect resulting wires
  NCollection_Sequence<ShapeFix_WireSegment> wires; // resulting wires
  CollectWires(wires, seqw);

  // And construct resulting faces
  NCollection_Sequence<TopoDS_Shape> faces;
  DispatchWires(faces, wires);

  // Finally, construct resulting shell
  if (faces.Length() != 1)
  {
    TopoDS_Shell S;
    BRep_Builder B;
    B.MakeShell(S);
    for (int i = 1; i <= faces.Length(); i++)
      B.Add(S, faces(i));
    myResult = S;
  }
  else
    myResult = faces(1);
  myResult.Orientation(myOrient);

  myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return true;
}

//=================================================================================================

void ShapeFix_ComposeShell::SplitEdges()
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  NCollection_Sequence<ShapeFix_WireSegment> seqw; // working data: wire segments

  // Init seqw by initial set of wires (with corresponding orientation)
  LoadWires(seqw);

  // Split edges in the wires by grid and add internal segments of grid (parts of cutting lines)
  SplitByGrid(seqw);
}

//=================================================================================================

const TopoDS_Shape& ShapeFix_ComposeShell::Result() const
{
  return myResult;
}

//=================================================================================================

bool ShapeFix_ComposeShell::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus(myStatus, status);
}

//=======================================================================
// PRIVATE (working) METHODS
//=======================================================================

#define TOLINT 1.e-10 // precision for intersection

// Local definitions: characteristics of intersection point

#define IOR_UNDEF 0 // undefined side
#define IOR_LEFT 1  // to left side of cutting line
#define IOR_RIGHT 2 // to right side of cutting line
#define IOR_BOTH 3  // crossing
#define IOR_POS 4   // in case of cycle on full period, whether first point is right

#define ITP_INTER 8   // crossing
#define ITP_BEGSEG 16 // start of tangential segment
#define ITP_ENDSEG 32 // stop of tangential segment
#define ITP_TANG 64   // tangential point

//=================================================================================================

// Return (signed) deviation of point from line
static double PointLineDeviation(const gp_Pnt2d& p, const gp_Lin2d& line)
{
  gp_Dir2d dir = line.Direction();
  gp_Dir2d n(-dir.Y(), dir.X());
  return n.XY() * (p.XY() - line.Location().XY());
}

//=================================================================================================

// Define position of point relative to line
static int PointLinePosition(const gp_Pnt2d& p, const gp_Lin2d& line, double& dev)
{
  dev = PointLineDeviation(p, line);
  return (dev > TOLINT ? IOR_LEFT : (dev < -TOLINT ? IOR_RIGHT : IOR_UNDEF));
}

//=================================================================================================

// Define position of point relative to line
static int PointLinePosition(const gp_Pnt2d& p, const gp_Lin2d& line)
{
  double dev;
  return PointLinePosition(p, line, dev);
}

//=================================================================================================

// Compute parameter of point on line
static inline double ParamPointOnLine(const gp_Pnt2d& p, const gp_Lin2d& line)
{
  return line.Direction().XY() * (p.XY() - line.Location().XY());
}

//=================================================================================================

// Compute parameter of two points on line (as intersection of segment)
static double ParamPointsOnLine(const gp_Pnt2d& p1, const gp_Pnt2d& p2, const gp_Lin2d& line)
{
  double dist1 = PointLineDeviation(p1, line);
  double dist2 = PointLineDeviation(p2, line);
  // in most cases, one of points is on line
  if (std::abs(dist1) < ::Precision::PConfusion())
  {
    if (std::abs(dist2) < ::Precision::PConfusion())
      return 0.5 * (ParamPointOnLine(p1, line) + ParamPointOnLine(p2, line));
    return ParamPointOnLine(p1, line);
  }
  if (std::abs(dist2) < ::Precision::PConfusion())
    return ParamPointOnLine(p2, line);
  // just protection
  if (dist2 * dist1 > 0)
    return 0.5 * (ParamPointOnLine(p1, line) + ParamPointOnLine(p2, line));
  // else compute intersection
  return (ParamPointOnLine(p1, line) * dist2 - ParamPointOnLine(p2, line) * dist1)
         / (dist2 - dist1);
}

//=================================================================================================

// Compute projection of point on line
static inline gp_Pnt2d ProjectPointOnLine(const gp_Pnt2d& p, const gp_Lin2d& line)
{
  return line.Location().XY() + line.Direction().XY() * ParamPointOnLine(p, line);
}

//=================================================================================================

// Apply context to one edge in the wire and put result into this wire
static int ApplyContext(ShapeFix_WireSegment&                  wire,
                        const int                              iedge,
                        const occ::handle<ShapeBuild_ReShape>& context)
{
  TopoDS_Edge  edge = wire.Edge(iedge);
  TopoDS_Shape res  = context->Apply(edge);

  if (res.IsSame(edge))
    return 1;

  if (res.ShapeType() == TopAbs_EDGE)
  {
    wire.SetEdge(iedge, TopoDS::Edge(res));
    return 1;
  }

  int index = iedge;

  occ::handle<ShapeExtend_WireData> segw = new ShapeExtend_WireData;
  segw->ManifoldMode()                   = false;
  for (TopoDS_Iterator it(res); it.More(); it.Next())
  {
    TopoDS_Edge E = TopoDS::Edge(it.Value());
    if (!E.IsNull())
      segw->Add(E);
#ifdef OCCT_DEBUG
    else
      std::cout << "Error: ShapeFix_ComposeShell, ApplyContext: wrong mapping of edge" << std::endl;
#endif
  }

  // add edges into the wire in correct order
  if (segw->NbEdges() > 0)
  {
    int ind, iumin, iumax, ivmin, ivmax;
    wire.GetPatchIndex(iedge, iumin, iumax, ivmin, ivmax);
    int nbEdges = segw->NbEdges();
    for (int i = 1; i <= nbEdges; i++, index++)
    {
      ind = (edge.Orientation() == TopAbs_FORWARD || edge.Orientation() == TopAbs_INTERNAL
               ? i
               : segw->NbEdges() - i + 1);
      TopoDS_Edge aE = segw->Edge(ind);
      if (i == 1)
        wire.SetEdge(index, aE);
      else
        wire.AddEdge(index, aE, iumin, iumax, ivmin, ivmax);
    }
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "Warning: ShapeFix_ComposeShell, ApplyContext: edge is to remove - not implemented"
              << std::endl;
#endif

  return index - iedge;
}

//=================================================================================================

// check points coincidence
static inline bool IsCoincided(const gp_Pnt2d& p1,
                               const gp_Pnt2d& p2,
                               const double    UResolution,
                               const double    VResolution,
                               const double    tol)
{
  // pdn Maximal accuracy is working precision of intersector.
  double UTolerance = UResolution * tol;
  double VTolerance = VResolution * tol;
  return std::abs(p1.X() - p2.X()) <= std::max(TOLINT, UTolerance)
         && std::abs(p1.Y() - p2.Y()) <= std::max(TOLINT, VTolerance);
}

//=================================================================================================

// computes index for the patch by given parameter Param
static int GetPatchIndex(const double                                    Param,
                         const occ::handle<NCollection_HArray1<double>>& Params,
                         const bool                                      isClosed)
{
  int    NP     = Params->Upper();
  double period = Params->Value(NP) - Params->Value(1);
  double shift  = 0;
  if (isClosed)
    shift = ShapeAnalysis::AdjustToPeriod(Param, Params->Value(1), Params->Value(NP));
  double p = Param + shift;

  // locate patch: the same algo as in SE_CS::LocateParameter()
  int i; // svv #1
  for (i = 2; i < NP; i++)
  {
    //    double par = Params->Value(i);
    if (p < Params->Value(i))
      break;
  }
  i--;

  double ish    = shift / period;
  int    ishift = (int)(ish < 0 ? ish - 0.5 : ish + 0.5);
  return i - ishift * (NP - 1);
}

//=================================================================================================

void ShapeFix_ComposeShell::LoadWires(NCollection_Sequence<ShapeFix_WireSegment>& seqw) const
{
  seqw.Clear();

  // Init seqw by initial set of wires (with corresponding orientation)
  for (TopoDS_Iterator iw(myFace, false); iw.More(); iw.Next())
  {
    TopoDS_Shape tmpW = Context()->Apply(iw.Value());
    if (tmpW.ShapeType() != TopAbs_WIRE)
    {
      if (tmpW.ShapeType() == TopAbs_VERTEX)
      {
        ShapeFix_WireSegment seg; //(( isOuter ? TopAbs_REVERSED : TopAbs_FORWARD ) );
        seg.SetVertex(TopoDS::Vertex(tmpW));
        seg.Orientation(tmpW.Orientation());
        seqw.Append(seg);
      }

      continue;
    }

    TopoDS_Wire wire = TopoDS::Wire(tmpW);

    bool isNonManifold =
      (wire.Orientation() != TopAbs_REVERSED && wire.Orientation() != TopAbs_FORWARD);

    // protect against INTERNAL/EXTERNAL wires
    //    if ( wire.Orientation() != TopAbs_REVERSED &&
    //	 wire.Orientation() != TopAbs_FORWARD ) continue;

    // determine orientation of the wire
    //    TopoDS_Face face = TopoDS::Face ( myFace.EmptyCopied() );
    //    B.Add ( face, wire );
    //    bool isOuter = ShapeAnalysis::IsOuterBound ( face );

    if (isNonManifold)
    {
      occ::handle<ShapeExtend_WireData> sbwd = new ShapeExtend_WireData(wire, true, false);
      // pdn protection against wires w/o edges
      int nbEdges = sbwd->NbEdges();
      if (nbEdges)
      {
        // wire segments for non-manifold topology should have INTERNAL orientation
        ShapeFix_WireSegment seg(sbwd, TopAbs_INTERNAL);
        seqw.Append(seg);
      }
    }
    else
    {
      // splitting wires containing manifold and non-manifold parts on a separate
      // wire segment
      occ::handle<ShapeExtend_WireData> sbwdM  = new ShapeExtend_WireData();
      occ::handle<ShapeExtend_WireData> sbwdNM = new ShapeExtend_WireData();
      sbwdNM->ManifoldMode()                   = false;
      TopoDS_Iterator aIt(wire);
      for (; aIt.More(); aIt.Next())
      {
        TopoDS_Edge E = TopoDS::Edge(aIt.Value());
        if (E.Orientation() == TopAbs_FORWARD || E.Orientation() == TopAbs_REVERSED)
          sbwdM->Add(E);
        else
          sbwdNM->Add(E);
      }

      int nbMEdges  = sbwdM->NbEdges();
      int nbNMEdges = sbwdNM->NbEdges();

      if (nbNMEdges)
      {
        // clang-format off
        ShapeFix_WireSegment seg ( sbwdNM, TopAbs_INTERNAL); //(( isOuter ? TopAbs_REVERSED : TopAbs_FORWARD ) );
        // clang-format on
        seqw.Append(seg);
      }

      if (nbMEdges)
      {
        // Orientation is set so as to allow the segment to be traversed in only one direction
        // skl 01.04.2002
        occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire;
        sfw->Load(sbwdM);
        int                       stat = 0;
        occ::handle<Geom_Surface> gs   = BRep_Tool::Surface(myFace);
        if (gs->IsUPeriodic() && gs->IsVPeriodic())
        {
          // For torus-like shapes, first reorder in 2d since reorder is indifferent in 3d
          ShapeAnalysis_WireOrder sawo(false, 0);
          ShapeAnalysis_Edge      sae;
          for (int i = 1; i <= nbMEdges; i++)
          {
            double                    f, l;
            occ::handle<Geom2d_Curve> c2d;
            // smh#8
            TopoDS_Shape tmpF = myFace.Oriented(TopAbs_FORWARD);
            if (!sae.PCurve(sbwdM->Edge(i), TopoDS::Face(tmpF), c2d, f, l))
              continue;
            sawo.Add(c2d->Value(f).XY(), c2d->Value(l).XY());
          }

          sawo.Perform();
          stat = (sawo.Status() < 0 ? -1 : 1);
          sfw->FixReorder(sawo);
        }

        sfw->FixReorder();
        if (sfw->StatusReorder(ShapeExtend_DONE3))
          stat = -1;

        if (stat < 0)
        {
          BRep_Builder B;
          TopoDS_Shape dummy = myFace.EmptyCopied();
          TopoDS_Face  face  = TopoDS::Face(dummy);
          B.Add(face, wire);
          bool        isOuter = ShapeAnalysis::IsOuterBound(face);
          TopoDS_Wire w       = sbwdM->Wire();
          dummy               = myFace.EmptyCopied();
          face                = TopoDS::Face(dummy);
          B.Add(face, w);
          bool isOuterAfter = ShapeAnalysis::IsOuterBound(face);
          if (isOuter != isOuterAfter)
            sbwdM->Reverse(face);
        }

        // clang-format off
        ShapeFix_WireSegment seg ( sbwdM, TopAbs_REVERSED ); //(( isOuter ? TopAbs_REVERSED : TopAbs_FORWARD ) );
        // clang-format on
        seqw.Append(seg);
      }
    }
  }
}

//=======================================================================
// function : ComputeCode
// purpose  : compute code for wire segment between two intersections (by deviation)
//=======================================================================

int ShapeFix_ComposeShell::ComputeCode(const occ::handle<ShapeExtend_WireData>& wire,
                                       const gp_Lin2d&                          line,
                                       const int                                begInd,
                                       const int                                endInd,
                                       const double                             begPar,
                                       const double                             endPar,
                                       const bool                               isInternal)
{
  int code = IOR_UNDEF;

  ShapeAnalysis_Edge sae;
  const int          NPOINTS = 5; // number of points for measuring deviation

  // track special closed case: segment starts at end of edge and ends at its beginning
  int special = (begInd == endInd
                     && (wire->Edge(begInd).Orientation() == TopAbs_FORWARD
                         || wire->Edge(begInd).Orientation() == TopAbs_INTERNAL)
                          == (begPar > endPar)
                   ? 1
                   : 0);
  if (!special && begInd == endInd && begPar == endPar && (myClosedMode || isInternal))
    special = 1;

  // for tracking cases in closed mode
  bool     begin = true;
  double   shift = 0;
  gp_Pnt2d p2d0;

  // check if segment is tangency
  // Segment is considered as tangency if deviation of pcurve from line
  // (in 2d) measured by NPOINTS points is less than tolerance of edge
  // (recomputed to 2d using Resolution).

  int nb = wire->NbEdges();

  int i; // svv #1
  for (i = begInd;; i++)
  {
    if (i > nb)
      i = 1;
    TopoDS_Edge edge = wire->Edge(i);

    occ::handle<Geom2d_Curve> c2d;
    double                    f, l;
    if (!sae.PCurve(edge, myFace, c2d, f, l, false))
    {
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
      continue;
    }
    double tol        = LimitTolerance(BRep_Tool::Tolerance(edge));
    bool   isreversed = (edge.Orientation() == TopAbs_REVERSED);

    double par1 = (i == begInd && special >= 0 ? begPar : (isreversed ? l : f));
    double par2 = (i == endInd && special <= 0 ? endPar : (isreversed ? f : l));
    double dpar = (par2 - par1) / (NPOINTS - 1);
    int    np   = (std::abs(dpar) < ::Precision::PConfusion() ? 1 : NPOINTS);
    int    j; // svv #1
    for (j = 0; j < np; j++)
    {
      double   par = par1 + dpar * j;
      gp_Pnt2d p2d = c2d->Value(par);
      if (myClosedMode)
      {
        if (myUClosed && std::abs(line.Direction().X()) < ::Precision::PConfusion())
        {
          if (begin)
            shift = ShapeAnalysis::AdjustByPeriod(p2d.X(), line.Location().X(), myUPeriod);
          else if (!j)
            shift = ShapeAnalysis::AdjustByPeriod(p2d.X() - p2d0.X(), 0., myUPeriod);
          p2d.SetX(p2d.X() + shift);
        }
        if (myVClosed && std::abs(line.Direction().Y()) < ::Precision::PConfusion())
        {
          if (begin)
            shift = ShapeAnalysis::AdjustByPeriod(p2d.Y(), line.Location().Y(), myVPeriod);
          else if (!j)
            shift = ShapeAnalysis::AdjustByPeriod(p2d.Y() - p2d0.Y(), 0., myVPeriod);
          p2d.SetY(p2d.Y() + shift);
        }
        begin = false;
      }
      p2d0    = p2d;
      int pos = PointLinePosition(p2d, line);
      if (pos == IOR_UNDEF)
        continue;

      // analyse the deviation
      gp_Pnt2d p2dl = ProjectPointOnLine(p2d, line);
      if (!IsCoincided(p2d, p2dl, myUResolution, myVResolution, tol))
      {
        if (!myClosedMode)
        {
          code = pos;
          break;
        }
        else
        {
          code |= pos;
        }
      }
    }
    if (j < np)
    {
      i = 0;
      break;
    } // not tangency
    if (i == endInd)
    {
      if (special <= 0)
        break;
      else
        special = -1;
    }
  }
  if (myClosedMode)
  {
    if (code != IOR_UNDEF && !begin)
    {
      // in closed mode, if segment is of 2*pi length, it is BOTH
      double dev = PointLineDeviation(p2d0, line);
      if (myUClosed && std::abs(line.Direction().X()) < ::Precision::PConfusion())
      {
        if (std::abs(std::abs(dev) - myUPeriod) < 0.1 * myUPeriod)
        {
          code = IOR_BOTH;
          if (dev > 0)
            code |= IOR_POS;
        }
        else if (code == IOR_BOTH)
          code = IOR_UNDEF;
      }
      if (myVClosed && std::abs(line.Direction().Y()) < ::Precision::PConfusion())
      {
        if (std::abs(std::abs(dev) - myVPeriod) < 0.1 * myVPeriod)
        {
          code = IOR_BOTH;
          if (dev > 0)
            code |= IOR_POS;
        }
        else if (code == IOR_BOTH)
          code = IOR_UNDEF;
      }
    }
    return code;
  }
  if (i)
    code = IOR_UNDEF; // tangency
  else if (code == IOR_BOTH)
  { // parity error in intersector
    code = IOR_LEFT;
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeFix_ComposeShell::ComputeCode: lost intersection point"
              << std::endl;
#endif
  }
  return code;
}

//=================================================================================================

// After applying context to (seam) edge, distribute its indices on new edges,
// according to their parameters on that edge
static void DistributeSplitPoints(const occ::handle<ShapeExtend_WireData>& sbwd,
                                  const TopoDS_Face&                       myFace,
                                  const int                                index,
                                  const int                                nsplit,
                                  NCollection_Sequence<int>&               indexes,
                                  const NCollection_Sequence<double>&      values)
{
  bool isreversed = (nsplit > 0 && sbwd->Edge(index).Orientation() == TopAbs_REVERSED);

  NCollection_Array1<double> params(0, nsplit);
  int                        i; // svv #1
  for (i = 0; i < nsplit; i++)
  {
    double f, l;
    BRep_Tool::Range(sbwd->Edge(index + i), myFace, f, l);
    params.SetValue(i, (isreversed ? l : f));
  }

  for (i = 1; i <= indexes.Length() && indexes(i) < index; i++)
    ;
  for (int shift = 1; i <= indexes.Length() && indexes(i) == index; i++)
  {
    while (shift < nsplit && isreversed != (values(i) > params(shift)))
      shift++;
    indexes.SetValue(i, index + shift - 1);
  }
  for (; i <= indexes.Length(); i++)
    indexes.SetValue(i, indexes(i) + nsplit - 1);
}

//=================================================================================================

static int CheckByCurve3d(const gp_Pnt&                  pos,
                          const occ::handle<Geom_Curve>& c3d,
                          const double                   param,
                          const gp_Trsf&                 T,
                          const double                   tol)
{
  if (c3d.IsNull())
    return true;
  gp_Pnt p = c3d->Value(param);
  if (T.Form() != gp_Identity)
    p.Transform(T);
  return pos.SquareDistance(p) <= tol * tol;
}

//=================================================================================================

static void DefinePatch(ShapeFix_WireSegment& wire,
                        const int             code,
                        const bool            isCutByU,
                        const int             cutIndex,
                        const int             number = -1)
{
  int nb = (number > 0 ? number : wire.NbEdges());
  if (isCutByU)
  {
    if (!(code & IOR_LEFT))
      wire.DefineIUMin(nb, cutIndex);
    if (!(code & IOR_RIGHT))
      wire.DefineIUMax(nb, cutIndex);
  }
  else
  {
    if (!(code & IOR_RIGHT))
      wire.DefineIVMin(nb, cutIndex);
    if (!(code & IOR_LEFT))
      wire.DefineIVMax(nb, cutIndex);
  }
}

//=================================================================================================

static double GetGridResolution(const occ::handle<NCollection_HArray1<double>>& SplitValues,
                                const int                                       cutIndex)
{
  int    nb       = SplitValues->Length();
  double leftLen  = (cutIndex > 1 ? SplitValues->Value(cutIndex) - SplitValues->Value(cutIndex - 1)
                                  : SplitValues->Value(nb) - SplitValues->Value(nb - 1));
  double rigthLen = (cutIndex < nb ? SplitValues->Value(cutIndex + 1) - SplitValues->Value(cutIndex)
                                   : SplitValues->Value(2) - SplitValues->Value(1));
  return std::min(leftLen, rigthLen) / 3.;
}

//=================================================================================================

ShapeFix_WireSegment ShapeFix_ComposeShell::SplitWire(ShapeFix_WireSegment&               wire,
                                                      NCollection_Sequence<int>&          indexes,
                                                      const NCollection_Sequence<double>& values,
                                                      NCollection_Sequence<TopoDS_Shape>& vertices,
                                                      const NCollection_Sequence<int>& SegmentCodes,
                                                      const bool                       isCutByU,
                                                      const int                        cutIndex)
{
  BRep_Builder                       B;
  ShapeFix_WireSegment               result;
  occ::handle<ShapeAnalysis_Surface> aSurfTool =
    new ShapeAnalysis_Surface(BRep_Tool::Surface(myFace));
  int                nbSplits = indexes.Length();
  ShapeAnalysis_Edge sae;
  int                start        = 1;
  TopAbs_Orientation anWireOrient = wire.Orientation();
  gp_Trsf            T;
  if (!myLoc.IsIdentity())
    T = myLoc.Inverted().Transformation();

  // Processing edge by edge (assuming that split points are sorted along the wire)
  for (int i = 1; i <= wire.NbEdges(); i++)
  {

    // for already split seam edge, redistribute its splitting points
    int nsplit = ApplyContext(wire, i, Context());
    if (nsplit != 1)
    {
      DistributeSplitPoints(wire.WireData(), myFace, i, nsplit, indexes, values);
      if (nsplit <= 0)
      {
#ifdef OCCT_DEBUG
        std::cout << "Error: ShapeFix_ComposeShell::SplitWire: edge dismissed" << std::endl;
#endif
        i--;
        continue;
      }
    }
    TopoDS_Edge edge = wire.Edge(i);

    int iumin, iumax, ivmin, ivmax;
    wire.GetPatchIndex(i, iumin, iumax, ivmin, ivmax);

    // Position code for first segment of edge
    int code = SegmentCodes(start > 1 ? start - 1 : SegmentCodes.Length());

    // Defining split parameters on edge
    int stop = start;
    while (stop <= nbSplits && indexes(stop) == i)
      stop++;
    if (stop == start)
    {
      result.AddEdge(0, edge, iumin, iumax, ivmin, ivmax);
      if (code != 0
          || wire.Orientation() != TopAbs_EXTERNAL) // pdn 0 code handling for extertnal wires
        DefinePatch(result, code, isCutByU, cutIndex);
      continue;
    }
    // find non-manifold vertices on edge
    NCollection_Sequence<TopoDS_Shape> aNMVertices;
    TopoDS_Iterator                    aIt(edge, false);
    for (; aIt.More(); aIt.Next())
    {
      if (aIt.Value().Orientation() != TopAbs_FORWARD
          && aIt.Value().Orientation() != TopAbs_REVERSED)
        aNMVertices.Append(aIt.Value());
    }

    // Collect data on edge
    double        tolEdge  = BRep_Tool::Tolerance(edge);
    TopoDS_Vertex prevV    = sae.FirstVertex(edge);
    TopoDS_Vertex lastV    = sae.LastVertex(edge);
    double        prevVTol = LimitTolerance(BRep_Tool::Tolerance(prevV));
    double        lastVTol = LimitTolerance(BRep_Tool::Tolerance(lastV));
    gp_Pnt        prevVPnt = BRep_Tool::Pnt(prevV);
    gp_Pnt        lastVPnt = BRep_Tool::Pnt(lastV);
    if (T.Form() != gp_Identity)
    {
      prevVPnt.Transform(T);
      lastVPnt.Transform(T);
    }

    occ::handle<Geom_Curve> c3d;
    double                  f3d, l3d;
    if (!sae.Curve3d(edge, c3d, f3d, l3d))
    { // not a crime
      c3d.Nullify();
      f3d = l3d = 0;
    }

    double                    firstPar, lastPar;
    occ::handle<Geom2d_Curve> C2d;
    if (!sae.PCurve(edge, myFace, C2d, firstPar, lastPar))
    {
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    }
    // finding sequence of non-manifold parameters
    int                          nbNMVert = aNMVertices.Length();
    NCollection_Sequence<double> aNMVertParams;
    if (nbNMVert)
    {
      Geom2dAdaptor_Curve adc(C2d);
      int                 n = 1;
      for (; n <= nbNMVert; n++)
      {
        gp_Pnt apV    = BRep_Tool::Pnt(TopoDS::Vertex(aNMVertices.Value(n)));
        double apar   = firstPar;
        double adist2 = RealLast();
        gp_Pnt aPproj;
        if (!c3d.IsNull())
        {
          ShapeAnalysis_Curve asae;
          adist2 = asae.Project(c3d, apV, Precision::Confusion(), aPproj, apar);
          adist2 *= adist2;
        }
        else
        {
          gp_Pnt2d        aP2d = aSurfTool->ValueOfUV(apV, Precision::Confusion());
          Extrema_ExtPC2d aExtr(aP2d, adc);
          if (aExtr.IsDone() && aExtr.NbExt())
          {
            adist2    = aExtr.SquareDistance(1);
            int index = 1;
            int k     = 2;
            for (; k <= aExtr.NbExt(); k++)
            {
              double ad2 = aExtr.SquareDistance(k);
              if (ad2 < adist2)
              {
                adist2 = ad2;
                index  = k;
              }
            }
            apar = aExtr.Point(index).Parameter();
          }
        }
        aNMVertParams.Append(apar);
      }
    }

    // pdn Calculating parametric shift
    bool   sp     = (f3d == firstPar && l3d == lastPar);
    double span2d = lastPar - firstPar;
    //    double ln2d  = lastPar-prevPar;
    //    double ln3d  = l3d - f3d;
    //    double fact = ln2d/ln3d;
    //    double shift =  prevPar - f3d*fact;
    double   prevPar    = firstPar;
    gp_Pnt2d prevPnt2d  = C2d->Value(prevPar);
    gp_Pnt2d lastPnt2d  = C2d->Value(lastPar);
    gp_Pnt   prevPnt    = myGrid->Value(prevPnt2d);
    gp_Pnt   lastPnt    = myGrid->Value(lastPnt2d);
    bool     isPeriodic = C2d->IsPeriodic();
    double   aPeriod    = (isPeriodic ? C2d->Period() : 0.);

    // Splitting edge
    int    NbEdgesStart = result.NbEdges();
    bool   splitted     = false;
    double currPar      = lastPar; // SK
    for (int j = start; j <= stop; prevPar = currPar, j++)
    {
      if (!splitted && j >= stop)
      { // no splitting at all
        // code = SegmentCodes ( j >1 ? j-1 : SegmentCodes.Length() ); // classification code
        break;
      }
      currPar = (j < stop ? values.Value(j) : lastPar);
      // fix for case when pcurve is periodic and first parameter of edge is more than 2P
      // method ShapeBuild_Edge::CopyRanges shift pcurve to range 0-2P and parameters of cutting
      // should be shifted too. gka SAMTECH 28.07.06
      if (isPeriodic)
      {
        if (currPar > (std::max(lastPar, firstPar) + Precision::PConfusion())
            || currPar < (std::min(firstPar, lastPar) - Precision::PConfusion()))
        {
          double aShift =
            ShapeAnalysis::AdjustByPeriod(currPar, (firstPar + lastPar) * 0.5, aPeriod);
          currPar += aShift;
        }
      }

      gp_Pnt2d currPnt2d;
      gp_Pnt   currPnt;

      // Try to adjust current splitting point to previous or end of edge
      bool          doCut = true;
      TopoDS_Vertex V;
      if (std::abs(currPar - lastPar) < ::Precision::PConfusion())
      {
        V     = lastV;
        doCut = false;
      }
      else if (std::abs(currPar - prevPar) < ::Precision::PConfusion())
      {
        vertices.Append(prevV);
        code = SegmentCodes(j); // classification code - update for next segment
        continue;               // no splitting at this point, go to next one
      }
      else
      {
        currPnt2d = C2d->Value(currPar);
        currPnt   = myGrid->Value(currPnt2d);
        if (currPnt.Distance(lastVPnt) <= lastVTol &&
            // Tolerance is increased to prevent degenerated cuts in cases where all vertex
            // tolerance is covered by distance of the edge curve from vertex point.
            // Doubled to prevent edge being fully covered by its vertices tolerance (invalid edge).
            CheckByCurve3d(lastVPnt,
                           c3d,
                           f3d + (currPar - firstPar) * (l3d - f3d) / span2d,
                           T,
                           lastVTol + 2 * Precision::Confusion())
            && lastPnt.Distance(myGrid->Value(C2d->Value(0.5 * (currPar + lastPar)))) <= lastVTol)
        {
          V           = lastV;
          double uRes = myUResolution;
          double vRes = myVResolution;
          if (isCutByU)
          {
            double gridRes = GetGridResolution(myGrid->UJointValues(), cutIndex) / lastVTol;
            uRes           = std::min(myUResolution, gridRes);
          }
          else
          {
            double gridRes = GetGridResolution(myGrid->VJointValues(), cutIndex) / lastVTol;
            vRes           = std::min(myVResolution, gridRes);
          }
          if (IsCoincided(lastPnt2d, currPnt2d, uRes, vRes, lastVTol)
              && IsCoincided(lastPnt2d,
                             C2d->Value(0.5 * (currPar + lastPar)),
                             uRes,
                             vRes,
                             lastVTol))
            doCut = false;
        }
        else if (currPnt.Distance(prevVPnt) <= prevVTol &&
                 // Tolerance is increased to prevent degenerated cuts in cases where all vertex
                 // tolerance is covered by distance of the edge curve from vertex point.
                 // Doubled to prevent edge being fully covered by its vertices tolerance (invalid
                 // edge).
                 CheckByCurve3d(prevVPnt,
                                c3d,
                                f3d + (currPar - firstPar) * (l3d - f3d) / span2d,
                                T,
                                prevVTol + 2 * Precision::Confusion())
                 && prevPnt.Distance(myGrid->Value(C2d->Value(0.5 * (currPar + prevPar))))
                      <= prevVTol)
        {
          V           = prevV;
          double uRes = myUResolution;
          double vRes = myVResolution;
          if (isCutByU)
          {
            double gridRes = GetGridResolution(myGrid->UJointValues(), cutIndex) / prevVTol;
            uRes           = std::min(myUResolution, gridRes);
          }
          else
          {
            double gridRes = GetGridResolution(myGrid->VJointValues(), cutIndex) / prevVTol;
            vRes           = std::min(myVResolution, gridRes);
          }
          if (IsCoincided(prevPnt2d, currPnt2d, uRes, vRes, prevVTol)
              && IsCoincided(prevPnt2d,
                             C2d->Value(0.5 * (currPar + prevPar)),
                             uRes,
                             vRes,
                             prevVTol))
          {

            vertices.Append(prevV);
            code = SegmentCodes(j); // classification code - update for next segment
            continue;               // no splitting at this point, go to next one
          }
        }
        //: abv 28.05.02: OCC320 Sample_2: if maxtol = 1e-7, the vertex tolerance
        // is actually ignored - protect against new vertex on degenerated edge
        else if (BRep_Tool::Degenerated(edge) && prevV.IsSame(lastV))
        {
          V = prevV;
        }
      }
      // classification code for current segment
      if (j > start)
        code = SegmentCodes(j > 1 ? j - 1 : SegmentCodes.Length());

      // if not adjusted, make new vertex
      if (V.IsNull())
      {
        B.MakeVertex(V, currPnt.Transformed(myLoc.Transformation()), tolEdge);
        vertices.Append(V);
      }
      // else adjusted to end, fill all resting vertices
      else if (!doCut)
      {
        for (; j < stop; j++)
          vertices.Append(lastV);
        if (!splitted)
          break; // no splitting at all
        currPar = lastPar;
      }
      else
        vertices.Append(V);

      // When edge is about to be split, copy end vertices to protect
      // original shape from increasing tolerance after fixing SameParameter
      if (!splitted)
      {
        // smh#8
        TopoDS_Shape  emptyCopiedfV = prevV.EmptyCopied();
        TopoDS_Vertex fV            = TopoDS::Vertex(emptyCopiedfV);
        Context()->Replace(prevV, fV);
        TopoDS_Vertex lV;
        if (prevV.IsSame(lastV))
        {
          // smh#8
          TopoDS_Shape tmpV = fV.Oriented(lastV.Orientation());
          lV                = TopoDS::Vertex(tmpV);
        }
        else
        {
          // smh#8
          TopoDS_Shape emptyCopied = lastV.EmptyCopied();
          lV                       = TopoDS::Vertex(emptyCopied);
          Context()->Replace(lastV, lV);
        }
        if (V.IsSame(lastV))
          V = lV;
        else if (V.IsSame(prevV))
          V = fV;
        lastV = lV;
        prevV = fV;
      }

      // Splitting of the edge
      splitted = true;
      prevV.Orientation(TopAbs_FORWARD);
      V.Orientation(TopAbs_REVERSED);
      ShapeBuild_Edge sbe;
      TopoDS_Edge     anInitEdge = edge;
      bool            ismanifold =
        (edge.Orientation() == TopAbs_FORWARD || edge.Orientation() == TopAbs_REVERSED);
      if (!ismanifold)
        anInitEdge.Orientation(TopAbs_FORWARD);
      TopoDS_Edge newEdge = sbe.CopyReplaceVertices(anInitEdge, prevV, V);

      // addition internal vertices if they exists on edge
      int n = 1;
      for (; n <= aNMVertParams.Length(); n++)
      {
        double        apar    = aNMVertParams.Value(n);
        TopoDS_Vertex aNMVert = TopoDS::Vertex(aNMVertices.Value(n));
        TopoDS_Vertex atmpV   = TopoDS::Vertex(Context()->Apply(aNMVert));
        if (fabs(apar - prevPar) <= Precision::PConfusion())
        {
          Context()->Replace(atmpV, prevV);
          aNMVertParams.Remove(n);
          aNMVertices.Remove(n);
          n--;
        }
        else if (fabs(apar - currPar) <= Precision::PConfusion())
        {
          Context()->Replace(atmpV, V);
          aNMVertParams.Remove(n);
          aNMVertices.Remove(n);
          n--;
        }
        if (apar > prevPar && apar < currPar)
        {
          B.Add(newEdge, atmpV);
          aNMVertParams.Remove(n);
          aNMVertices.Remove(n);
          n--;
        }
      }

      sbe.CopyPCurves(newEdge, anInitEdge);

      occ::handle<ShapeAnalysis_TransferParameters> theTransferParamtool = GetTransferParamTool();
      theTransferParamtool->SetMaxTolerance(MaxTolerance());
      theTransferParamtool->Init(anInitEdge, myFace);
      theTransferParamtool->TransferRange(newEdge, prevPar, currPar, true);

      if (!ismanifold)
      {
        if (code == IOR_UNDEF) // tangential segment
          newEdge.Orientation(TopAbs_EXTERNAL);
        else
          newEdge.Orientation(edge.Orientation());
      }

      if (!sp && !BRep_Tool::Degenerated(newEdge))
        B.SameRange(newEdge, false);
      // pdn take into account 0 codes (if ext)
      if (code == 0 && wire.Orientation() == TopAbs_EXTERNAL)
      {
        code = ((isCutByU == (j == 1)) ? 1 : 2);
      }

      result.AddEdge(0, newEdge, iumin, iumax, ivmin, ivmax);
      DefinePatch(result, code, isCutByU, cutIndex);

      // Changing prev parameters
      prevV     = V;
      prevVTol  = LimitTolerance(BRep_Tool::Tolerance(V));
      prevVPnt  = BRep_Tool::Pnt(V);
      prevPnt   = currPnt;
      prevPnt2d = currPnt2d;
    }
    start = stop;

    if (splitted)
    {
      // record replacement in context
      // NOTE: order of edges in the replacing wire corresponds to FORWARD orientation of the edge
      TopoDS_Wire resWire;
      B.MakeWire(resWire);
      for (int k = NbEdgesStart; k < result.NbEdges(); k++)
      {
        if (edge.Orientation() == TopAbs_FORWARD || edge.Orientation() == TopAbs_INTERNAL)
          B.Add(resWire, result.Edge(k + 1));
        else
          B.Add(resWire, result.Edge(result.NbEdges() - k + NbEdgesStart));
      }
      Context()->Replace(edge, resWire);
    }
    else
    {
      if (anWireOrient == TopAbs_INTERNAL && code == 0)
      {
        ShapeBuild_Edge sbe;
        if (edge.Orientation() == TopAbs_INTERNAL)
          edge.Orientation(TopAbs_FORWARD);
        TopoDS_Edge               e1   = sbe.Copy(edge, false);
        occ::handle<Geom2d_Curve> C2d2 = occ::down_cast<Geom2d_Curve>(C2d->Copy());
        B.UpdateEdge(e1, C2d, C2d2, myFace, 0.);
        e1.Orientation(TopAbs_EXTERNAL);
        Context()->Replace(edge, e1);
        result.AddEdge(0, e1, iumin, iumax, ivmin, ivmax);
      }
      else
        result.AddEdge(0, edge, iumin, iumax, ivmin, ivmax);
      if (code == 0 && wire.Orientation() == TopAbs_EXTERNAL)
      {
        // pdn defining code for intersection of two isos
        code = ((isCutByU == (std::abs(firstPar - currPar) < std::abs(lastPar - currPar))) ? 2 : 1);
      }
      DefinePatch(result, code, isCutByU, cutIndex);
    }
  }
  result.Orientation(anWireOrient);
  return result;
}

//=================================================================================================

bool ShapeFix_ComposeShell::SplitByLine(ShapeFix_WireSegment&               wire,
                                        const gp_Lin2d&                     line,
                                        const bool                          isCutByU,
                                        const int                           cutIndex,
                                        NCollection_Sequence<double>&       SplitLinePar,
                                        NCollection_Sequence<int>&          SplitLineCode,
                                        NCollection_Sequence<TopoDS_Shape>& SplitLineVertex)
{
  ShapeAnalysis_Edge sae;
  // prepare data on cutting line
  occ::handle<Geom2d_Line> jC2d = new Geom2d_Line(line);
  Geom2dAdaptor_Curve      jGAC(jC2d);

  NCollection_Sequence<int>    IntEdgeInd; // index of intersecting edge
  NCollection_Sequence<double> IntEdgePar; // parameter of intersection point on edge
  NCollection_Sequence<double> IntLinePar; // parameter of intersection point on line

  bool isnonmanifold = (wire.Orientation() == TopAbs_INTERNAL);
  // gka correction for non-manifold vertices SAMTECH
  if (wire.IsVertex())
  {
    occ::handle<ShapeAnalysis_Surface> aSurfTool =
      new ShapeAnalysis_Surface(BRep_Tool::Surface(myFace));
    TopoDS_Vertex aVert = wire.GetVertex();
    gp_Pnt        aP3d  = BRep_Tool::Pnt(aVert);
    gp_Pnt2d      aP2d  = aSurfTool->ValueOfUV(aP3d, Precision::Confusion());
    double        dev   = 0.;
    int           code  = PointLinePosition(aP2d, line, dev);
    if (code != IOR_UNDEF)
      return false;
    double par = ParamPointOnLine(aP2d, line);
    SplitLinePar.Append(par);
    // splitting codes for non-manifold topology should be tangential
    SplitLineCode.Append(ITP_TANG); // ITP_INTER);
    TopoDS_Vertex aVertNew;
    BRep_Builder  aB;
    aB.MakeVertex(aVertNew, aP3d, BRep_Tool::Tolerance(aVert));
    aVertNew.Orientation(TopAbs_FORWARD);
    Context()->Replace(aVert, aVertNew);
    SplitLineVertex.Append(aVertNew);
    wire.SetVertex(aVertNew);
    return true;
  }
  const occ::handle<ShapeExtend_WireData> sewd = wire.WireData();

  int nbe = sewd->NbEdges();

  //: abv 31.10.01: for closed mode
  int closedDir = 0;
  if (myClosedMode)
  {
    if (myUClosed && std::abs(line.Direction().X()) < ::Precision::PConfusion())
      closedDir = -1;
    else if (myVClosed && std::abs(line.Direction().Y()) < ::Precision::PConfusion())
      closedDir = 1;
  }
  double halfPeriod = 0.5 * (closedDir ? closedDir < 0 ? myUPeriod : myVPeriod : 0.);

  //============================================
  // make intersections and collect all data on intersection points
  int      firstCode = 0, prevCode = 0;
  gp_Pnt2d firstPos, prevPos;
  double   firstDev = 0., prevDev = 0.;
  for (int iedge = 1; iedge <= nbe; iedge++)
  {
    TopoDS_Edge E          = sewd->Edge(iedge);
    bool        isreversed = (E.Orientation() == TopAbs_REVERSED);

    double                    f, l;
    occ::handle<Geom2d_Curve> c2d;
    if (!sae.PCurve(E, myFace, c2d, f, l, false))
      continue;
    occ::handle<Geom2d_Curve> c2d_sav = c2d;

    // get end points
    gp_Pnt2d posf = c2d->Value(f), posl = c2d->Value(l);
    gp_XY    pppf = posf.XY(), pppl = posl.XY();

    // In case of ClosedMode, adjust curve and end points to period on closed surface
    //: abv 16.10.01: Ziegler_CADDY01.sat -18: if pcurve is longer than period,
    // ensure processing of all intersections
    int      nbIter = 1;
    gp_Vec2d shiftNext(0., 0.);
    if (myClosedMode)
    {
      // get bounding box of pcurve
      ShapeAnalysis_Curve sac;
      Bnd_Box2d           box;
      const int           aNbPoints = 41;
      sac.FillBndBox(c2d, f, l, aNbPoints, true, box);
      double umin, vmin, umax, vmax;
      box.Get(umin, vmin, umax, vmax);

      // compute shifts and adjust points adjust
      if (closedDir < 0)
      {
        double x     = line.Location().X();
        double shift = ShapeAnalysis::AdjustToPeriod(umin, x - myUPeriod, x);
        if (shift != 0.)
        {
          c2d = occ::down_cast<Geom2d_Curve>(c2d->Copy());
          gp_Vec2d V(shift, 0.);
          c2d->Translate(V);
          pppf.SetX(pppf.X() + shift);
          pppl.SetX(pppl.X() + shift);
        }
        double dUmax = umax + shift - x;
        shiftNext.SetX(dUmax > 0 ? -myUPeriod : myUPeriod);
        nbIter = (int)(1 + std::abs(dUmax) / myUPeriod);
        shift  = ShapeAnalysis::AdjustByPeriod(posf.X(), x, myUPeriod);
        posf.SetX(posf.X() + shift);
        shift = ShapeAnalysis::AdjustByPeriod(posl.X(), x, myUPeriod);
        posl.SetX(posl.X() + shift);
      }
      else if (closedDir > 0)
      {
        double y     = line.Location().Y();
        double shift = ShapeAnalysis::AdjustToPeriod(vmin, y - myVPeriod, y);
        if (shift != 0.)
        {
          c2d = occ::down_cast<Geom2d_Curve>(c2d->Copy());
          gp_Vec2d V(0., shift);
          c2d->Translate(V);
          pppf.SetY(pppf.Y() + shift);
          pppl.SetY(pppl.Y() + shift);
        }
        double dVmax = vmax + shift - y;
        shiftNext.SetY(dVmax > 0 ? -myVPeriod : myVPeriod);
        nbIter = (int)(1 + std::abs(dVmax) / myVPeriod);
        shift  = ShapeAnalysis::AdjustByPeriod(posf.Y(), y, myVPeriod);
        posf.SetY(posf.Y() + shift);
        shift = ShapeAnalysis::AdjustByPeriod(posl.Y(), y, myVPeriod);
        posl.SetY(posl.Y() + shift);
      }
    }

    // detect intersections at junction of two edges
    gp_Pnt2d pos = (isreversed ? posl : posf);
    double   dev;
    int      code = PointLinePosition(pos, line, dev);
    if (iedge == 1)
    {
      firstCode = code;
      firstPos  = pos;
      firstDev  = dev;
    }
    else if (code == IOR_UNDEF || code != prevCode)
    {
      if (!closedDir || std::abs(dev - prevDev) < halfPeriod)
      {
        // clang-format off
        IntLinePar.Append ( ParamPointsOnLine ( pos, prevPos, line ) ); // !! - maybe compute exactly ?
        // clang-format on
        IntEdgePar.Append(isreversed ? l : f);
        IntEdgeInd.Append(iedge);
      }
    }

    // fill data on end point (for next edge)
    pos      = (isreversed ? posf : posl);
    prevCode = PointLinePosition(pos, line, prevDev);
    prevPos  = pos;

    // cycle with shift in order to track all possible intersections
    for (int iter = 1; iter <= nbIter; iter++)
    {
      // data for intersection
      IntRes2d_Domain     iDom(pppf, f, TOLINT, pppl, l, TOLINT);
      Geom2dAdaptor_Curve iGAC(c2d);

      // intersection
      Geom2dInt_GInter Inter;
      Inter.Perform(jGAC, /*jDom,*/ iGAC, iDom, TOLINT, TOLINT);

      // Fill arrays with new intersection points
      if (Inter.IsDone())
      {
        int i;
        for (i = 1; i <= Inter.NbPoints(); i++)
        {
          IntRes2d_IntersectionPoint IP = Inter.Point(i);
          if (IP.TransitionOfSecond().PositionOnCurve() == IntRes2d_Middle
              || (code != IOR_UNDEF && prevCode != IOR_UNDEF))
          {
            IntLinePar.Append(IP.ParamOnFirst());
            IntEdgePar.Append(IP.ParamOnSecond());
          }
        }
        for (i = 1; i <= Inter.NbSegments(); i++)
        {
          IntRes2d_IntersectionSegment IS = Inter.Segment(i);
          if (IS.HasFirstPoint())
          {
            IntRes2d_IntersectionPoint IP = IS.FirstPoint();
            IntLinePar.Append(IP.ParamOnFirst());
            IntEdgePar.Append(IP.ParamOnSecond());
          }
          if (IS.HasLastPoint())
          {
            IntRes2d_IntersectionPoint IP = IS.LastPoint();
            IntLinePar.Append(IP.ParamOnFirst());
            IntEdgePar.Append(IP.ParamOnSecond());
          }
        }
      }
      if (iter < nbIter)
      {
        if (iter == 1)
          c2d = occ::down_cast<Geom2d_Curve>(c2d->Copy());
        pppf += shiftNext.XY();
        pppl += shiftNext.XY();
        c2d->Translate(shiftNext);
      }
    }

    int start = IntEdgeInd.Length() + 1; // first of the new points

    // Move all points into range [f,l] (intersector sometimes gives params out of range)
    int i;
    for (i = start; i <= IntEdgePar.Length(); i++)
    {
      if (IntEdgePar(i) < f)
        IntEdgePar.SetValue(i, f);
      else if (IntEdgePar(i) > l)
        IntEdgePar.SetValue(i, l);
    }

    // Sort by parameter on edge
    for (i = IntEdgePar.Length(); i > start; i--)
      for (int j = start; j < i; j++)
      {
        if (isreversed == (IntEdgePar(j + 1) < IntEdgePar(j)))
          continue;
        IntLinePar.Exchange(j, j + 1);
        IntEdgePar.Exchange(j, j + 1);
      }

    // and fill indices
    for (i = start; i <= IntEdgePar.Length(); i++)
      IntEdgeInd.Append(iedge);

    // Detect intersection at closing point
    // Only wires which are not EXTERNAL are considered (as closed)
    if (iedge == nbe && wire.Orientation() != TopAbs_EXTERNAL
        && wire.Orientation() != TopAbs_INTERNAL
        && (prevCode == IOR_UNDEF || prevCode != firstCode))
    {
      if (!closedDir || std::abs(firstDev - prevDev) < halfPeriod)
      {
        IntLinePar.Append(ParamPointsOnLine(pos, firstPos, line));
        IntEdgePar.Append(isreversed ? f : l);
        IntEdgeInd.Append(iedge);
      }
    }
  }

  if (IntEdgePar.Length() < 1)
  {
    // pdn Defining position of wire. There is no intersection, so by any point.
    // DefinePatchForWire ( wire, firstCode, isCutByU, cutIndex );
    return false; // pdn ??
  }

  //======================================
  // Fill sequence of transition codes for intersection points
  NCollection_Sequence<int> IntCode;      // parameter of intersection point on line
  NCollection_Sequence<int> SegmentCodes; // classification codes for segments of wire

  // remove duplicated points to ensure correct results of ComputeCode
  int i, j = IntEdgePar.Length();
  if (myClosedMode && j > 1)
  {
    for (i = 1; i <= IntEdgePar.Length();)
    {
      if (i == j)
        break;
      if (IntEdgeInd(i) == IntEdgeInd(j)
          && std::abs(IntEdgePar(i) - IntEdgePar(j)) < ::Precision::PConfusion())
      {
        IntLinePar.Remove(i);
        IntEdgePar.Remove(i);
        IntEdgeInd.Remove(i);
        if (j > i)
          j--;
        continue;
      }
      else if (nbe == 1 || IntEdgeInd(i) == (IntEdgeInd(j) % nbe) + 1)
      {
        TopoDS_Edge E1 = sewd->Edge(IntEdgeInd(j));
        TopoDS_Edge E2 = sewd->Edge(IntEdgeInd(i));
        double      a1, b1, a2, b2;
        BRep_Tool::Range(E1, myFace, a1, b1);
        BRep_Tool::Range(E2, myFace, a2, b2);
        if (std::abs(IntEdgePar(j) - (E1.Orientation() == TopAbs_FORWARD ? b1 : a1))
              < ::Precision::PConfusion()
            && std::abs(IntEdgePar(i) - (E2.Orientation() == TopAbs_FORWARD ? a2 : b2))
                 < ::Precision::PConfusion())
        {
          IntLinePar.Remove(i);
          IntEdgePar.Remove(i);
          IntEdgeInd.Remove(i);
          if (j > i)
            j--;
          continue;
        }
      }
      j = i++;
    }
  }
  // sequence of real codes for each segment
  NCollection_Sequence<int> aNewSegCodes;
  // Compute segment codes (left side of line, right or tangential)
  for (i = 1; i <= IntEdgePar.Length(); i++)
  {
    j        = (i < IntEdgePar.Length() ? i + 1 : 1);
    int code = ComputeCode(sewd,
                           line,
                           IntEdgeInd(i),
                           IntEdgeInd(j),
                           IntEdgePar(i),
                           IntEdgePar(j),
                           isnonmanifold);
    SegmentCodes.Append(code);
  }

  // for EXTERNAL wire, i.e. another joint line, every point is double intersection
  if (wire.Orientation() == TopAbs_EXTERNAL)
  {
    for (i = 1; i <= IntEdgePar.Length(); i++)
    {
      IntCode.Append(ITP_TANG | IOR_BOTH);
      aNewSegCodes.Append(SegmentCodes(i));
    }
  }
  // For real (closed) wire, analyze tangencies
  else
  {
    if (wire.Orientation() != TopAbs_INTERNAL)
    {
      // Two consecutive tangential segments are considered as one, merge them.
      for (i = 1; i <= IntEdgePar.Length(); i++)
      {
        j = (i > 1 ? i - 1 : IntEdgePar.Length());

        int k = (i < IntEdgePar.Length() ? i + 1 : 1); // [ACIS22539]

        if (SegmentCodes(j) == IOR_UNDEF && SegmentCodes(i) == IOR_UNDEF)
        {

          // Very specific case when the constructed seam edge
          // overlaps with spur edge [ACIS22539]
          if (myClosedMode
              && (IntLinePar(i) - IntLinePar(j)) * (IntLinePar(k) - IntLinePar(i)) <= 0.)
            continue;

          IntEdgeInd.Remove(i);
          IntEdgePar.Remove(i);
          IntLinePar.Remove(i);
          SegmentCodes.Remove(i);
          i--;
        }
      }
    }
    // pdn exit if all split points removed
    if (IntEdgePar.Length() < 1)
    {
      return false; // pdn ??
    }

    // Analyze type of intersection point and encode it
    // Three kinds of points (ITP): clear intersection, tangency in-point,
    // beginning and end of tangential segment.
    // Orientation (IOR) tells on which side of line edge crosses it
    j = IntEdgePar.Length();

    for (i = 1; i <= IntEdgePar.Length(); j = i++)
    {
      int codej = SegmentCodes(j);
      int codei = SegmentCodes(i);
      if (myClosedMode)
      {
        if ((codej & IOR_BOTH) == IOR_BOTH) // IOR_LEFT : IOR_RIGHT
          codej = (codej & IOR_POS ? IOR_RIGHT : IOR_LEFT);
        if ((codei & IOR_BOTH) == IOR_BOTH) // IOR_RIGHT : IOR_LEFT
          codei = (codei & IOR_POS ? IOR_LEFT : IOR_RIGHT);
        aNewSegCodes.Append(codei);
        if (IntEdgeInd(i) == IntEdgeInd(j))
          aNewSegCodes.Append(codej);
      }
      else
        aNewSegCodes.Append(codei);
      int ipcode = (codej | codei);
      if (codej == IOR_UNDEF)
      { // previous segment was tangency
        if (IntLinePar(i) > IntLinePar(j))
          ipcode |= ITP_ENDSEG; // end of segment
        else
          ipcode |= ITP_BEGSEG; // beginning of segment
      }
      else if (codei == IOR_UNDEF)
      { // current segment is tangency
        if (IntLinePar(i < IntLinePar.Length() ? i + 1 : 1) > IntLinePar(i))
          ipcode |= ITP_BEGSEG; // beginning of segment
        else
          ipcode |= ITP_ENDSEG; // end of segment
      }
      // internal wire can be only tangent
      else if (i == j)
        ipcode |= ((ipcode & IOR_BOTH) == IOR_BOTH && !isnonmanifold ? ITP_INTER : ITP_TANG);
      else if (codei == codej || isnonmanifold)
        ipcode |= ITP_TANG; // tangency in-point
      else
        ipcode |= ITP_INTER; // standard crossing
      IntCode.Append(ipcode);
    }
  }

  //=======================================
  // Split edges in the wire by intersection points and fill vertices array
  NCollection_Sequence<TopoDS_Shape> IntVertices;
  wire = SplitWire(wire, IntEdgeInd, IntEdgePar, IntVertices, aNewSegCodes, isCutByU, cutIndex);

  // add all data to input arrays
  for (i = 1; i <= IntLinePar.Length(); i++)
  {
    SplitLinePar.Append(IntLinePar(i));
    SplitLineCode.Append(IntCode(i));
    SplitLineVertex.Append(IntVertices(i));
  }

  return true;
}

//=================================================================================================

void ShapeFix_ComposeShell::SplitByLine(NCollection_Sequence<ShapeFix_WireSegment>& wires,
                                        const gp_Lin2d&                             line,
                                        const bool                                  isCutByU,
                                        const int                                   cutIndex)
{
  NCollection_Sequence<double>       SplitLinePar;
  NCollection_Sequence<int>          SplitLineCode;
  NCollection_Sequence<TopoDS_Shape> SplitLineVertex;

  // split wires one by one, collecting data on intersection points
  int i; // svv #1
  for (i = 1; i <= wires.Length(); i++)
  {
    SplitByLine(wires(i), line, isCutByU, cutIndex, SplitLinePar, SplitLineCode, SplitLineVertex);
  }

  // sort intersection points along parameter on cutting line
  for (i = SplitLinePar.Length(); i > 1; i--)
    for (int j = 1; j < i; j++)
    {
      if (SplitLinePar(j) > SplitLinePar(j + 1))
      {
        SplitLinePar.Exchange(j, j + 1);
        SplitLineCode.Exchange(j, j + 1);
        SplitLineVertex.Exchange(j, j + 1);
      }
    }

  // merge null-length tangential segments into one-point tangencies or intersections
  for (i = 1; i < SplitLinePar.Length(); i++)
  {
    if (std::abs(SplitLinePar(i + 1) - SplitLinePar(i)) > ::Precision::PConfusion()
        && !SplitLineVertex(i).IsSame(SplitLineVertex(i + 1)))
      continue;
    if ((SplitLineCode(i) & ITP_ENDSEG && SplitLineCode(i + 1) & ITP_BEGSEG)
        || (SplitLineCode(i) & ITP_BEGSEG && SplitLineCode(i + 1) & ITP_ENDSEG))
    {
      int code = (SplitLineCode(i) | SplitLineCode(i + 1)) & IOR_BOTH;
      SplitLineCode.SetValue(i, code | (code == IOR_BOTH ? ITP_INTER : ITP_TANG));
      SplitLinePar.Remove(i + 1);
      SplitLineCode.Remove(i + 1);
      SplitLineVertex.Remove(i + 1);
    }
  }

  // go along line, split it by intersection points and create edges
  // (only for internal parts, in particular not for tangential segments)
  BRep_Builder B;
  int          parity     = 0; // 0 - out, 1 - in
  int          halfparity = 0; // left/right for tangential segments
  int          tanglevel  = 0; // tangency nesting level
  for (i = 1; i <= SplitLinePar.Length(); i++)
  {
    int  code     = SplitLineCode(i);
    bool interior = (!tanglevel && parity % 2); // create an edge
    if (code & ITP_INTER)
    { // crossing
      parity++;
    }
    else if (code & ITP_BEGSEG)
    { // beginning of tangential segment
      tanglevel++;
      if (!halfparity)
        halfparity = (code & IOR_BOTH);
      else if (halfparity != (code & IOR_BOTH))
        parity++;
    }
    else if (code & ITP_ENDSEG)
    { // end of tangential segment
      tanglevel--;
      if (!halfparity)
        halfparity = (code & IOR_BOTH);
      else if (halfparity != (code & IOR_BOTH))
        parity++;
    }
    if (tanglevel < 0)
    {
//      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_FAIL4 );
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeFix_ComposeShell::SplitByLine: tangency level <0 !" << std::endl;
#endif
    }
    if (!interior)
      continue;

    // apply context to vertices (to perform replacing/merging vertices)
    // smh#8
    TopoDS_Shape  tmpV1 = Context()->Apply(SplitLineVertex(i - 1));
    TopoDS_Shape  tmpV2 = Context()->Apply(SplitLineVertex(i));
    TopoDS_Vertex V1    = TopoDS::Vertex(tmpV1);
    TopoDS_Vertex V2    = TopoDS::Vertex(tmpV2);
    // protection against creating null-length edges or edges lying inside tolerance of vertices
    // first and last vertices for split line can not be merged to each other
    bool   canbeMerged = (/*myClosedMode &&*/ (i - 1 > 1 || i < SplitLinePar.Length()));
    double aMaxTol     = MaxTolerance();
    // case when max tolerance is not defined tolerance of vertices will be used as is
    if (aMaxTol <= 2. * Precision::Confusion())
      aMaxTol = Precision::Infinite();
    double aTol1 = std::min(BRep_Tool::Tolerance(V1), aMaxTol);
    double aTol2 = std::min(BRep_Tool::Tolerance(V2), aMaxTol);
    gp_Pnt aP1   = BRep_Tool::Pnt(V1);
    gp_Pnt aP2   = BRep_Tool::Pnt(V2);
    double aD    = aP1.SquareDistance(aP2);
    if (SplitLinePar(i) - SplitLinePar(i - 1) < ::Precision::PConfusion()
        || (canbeMerged && (aD <= (aTol1 * aTol1) || aD <= (aTol2 * aTol2))))
    { // BRepTools::Compare(V1, V2)) ) {

#ifdef OCCT_DEBUG
      std::cout << "Info: ShapeFix_ComposeShell::SplitByLine: Short segment ignored" << std::endl;
#endif
      if (!V1.IsSame(V2))
      { // merge coincident vertices
        ShapeBuild_Vertex sbv;
        TopoDS_Vertex     V = sbv.CombineVertex(V1, V2);
        Context()->Replace(V1, V.Oriented(V1.Orientation()));
        Context()->Replace(V2, V.Oriented(V2.Orientation()));
        V1 = V2 = V;
#ifdef OCCT_DEBUG
        std::cout << "Info: ShapeFix_ComposeShell::SplitByLine: Coincided vertices merged"
                  << std::endl;
#endif
      }
      continue;
    }

    // create an edge (without 3d curve), put it in wire segment and add to sequence
    // NOTE: i here is always >1
    TopoDS_Edge edge;
    B.MakeEdge(edge);
    V1.Orientation(TopAbs_FORWARD);
    V2.Orientation(TopAbs_REVERSED);
    B.Add(edge, V1);
    B.Add(edge, V2);
    occ::handle<Geom2d_Line> Lin1 = new Geom2d_Line(line);
    occ::handle<Geom2d_Line> Lin2 = new Geom2d_Line(line);
    B.UpdateEdge(edge, Lin1, Lin2, myFace, ::Precision::Confusion());
    B.Range(edge, myFace, SplitLinePar(i - 1), SplitLinePar(i));

    occ::handle<ShapeExtend_WireData> sbwd = new ShapeExtend_WireData;
    sbwd->Add(edge);
    ShapeFix_WireSegment seg(sbwd, TopAbs_EXTERNAL);

    // set patch indices
    DefinePatch(seg, IOR_UNDEF, isCutByU, cutIndex);
    if (!isCutByU)
    {
      double shiftU =
        (myClosedMode && myUClosed ? ShapeAnalysis::AdjustToPeriod(SplitLinePar(i - 1) - TOLINT,
                                                                   myGrid->UJointValue(1),
                                                                   myGrid->UJointValue(2))
                                   : 0.);
      double aPar = SplitLinePar(i - 1) + shiftU;

      seg.DefineIUMin(
        1,
        GetPatchIndex(aPar + ::Precision::PConfusion(), myGrid->UJointValues(), myUClosed));
      seg.DefineIUMax(
        1,
        GetPatchIndex(aPar - ::Precision::PConfusion(), myGrid->UJointValues(), myUClosed) + 1);
    }
    else
    {
      double shiftV =
        (myClosedMode && myVClosed ? ShapeAnalysis::AdjustToPeriod(SplitLinePar(i - 1) - TOLINT,
                                                                   myGrid->VJointValue(1),
                                                                   myGrid->VJointValue(2))
                                   : 0.);
      double aPar = SplitLinePar(i - 1) + shiftV;
      seg.DefineIVMin(
        1,
        GetPatchIndex(aPar + ::Precision::PConfusion(), myGrid->VJointValues(), myVClosed));
      seg.DefineIVMax(
        1,
        GetPatchIndex(aPar - ::Precision::PConfusion(), myGrid->VJointValues(), myVClosed) + 1);
    }
    wires.Append(seg);
  }
  if (parity % 2)
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL4);
#ifdef OCCT_DEBUG
    std::cout << "Error: ShapeFix_ComposeShell::SplitByLine: parity error" << std::endl;
#endif
  }

  // Apply context to all wires to perform all recorded replacements/merging
  for (i = 1; i <= wires.Length(); i++)
  {
    for (int j = 1; j <= wires(i).NbEdges();)
      j += ApplyContext(wires(i), j, Context());
  }
}

//=================================================================================================

void ShapeFix_ComposeShell::SplitByGrid(NCollection_Sequence<ShapeFix_WireSegment>& seqw)
{
  // process splitting by U- anv V-seams (i.e. U=const and V=const curves)
  // closed composite surface is processed as periodic
  double Uf, Ul, Vf, Vl;
  BRepTools::UVBounds(myFace, Uf, Ul, Vf, Vl);
  double Umin, Umax, Vmin, Vmax;
  myGrid->Bounds(Umin, Umax, Vmin, Vmax);

  // value of precision to define number of patch should be the same as used in the definitin
  // position of point relatively to seam edge (TOLINT)
  double pprec = TOLINT; //::Precision::PConfusion();
  int    i     = 1;
  if (myClosedMode)
  {
    // for closed mode when only one patch exist and location of the splitting line is coincident
    // with first joint value Therefore in this case it is necessary to move all wire segments in
    // the range of the patch between first and last joint values. Then all wire segments are lie
    // between -period and period in order to have valid split ranges after splitting. Because for
    // closed mode cut index always equal to 1 and parts of segments after splitting always should
    // have index either (0,1) or (1,2).
    for (i = 1; i <= seqw.Length(); i++)
    {
      ShapeFix_WireSegment& wire = seqw(i);

      TopoDS_Shape atmpF = myFace.EmptyCopied();
      BRep_Builder aB;
      atmpF.Orientation(TopAbs_FORWARD);
      aB.Add(atmpF, wire.WireData()->Wire());
      double Uf1, Ul1, Vf1, Vl1;
      ShapeAnalysis::GetFaceUVBounds(TopoDS::Face(atmpF), Uf1, Ul1, Vf1, Vl1);

      // for closed mode it is necessary to move wire segment in the interval defined by first and
      // last grid UV values
      double shiftU =
        (myClosedMode && myUClosed ? ShapeAnalysis::AdjustToPeriod(Ul1 - pprec,
                                                                   myGrid->UJointValue(1),
                                                                   myGrid->UJointValue(2))
                                   : 0.);
      double shiftV =
        (myClosedMode && myVClosed ? ShapeAnalysis::AdjustToPeriod(Vl1 - pprec,
                                                                   myGrid->VJointValue(1),
                                                                   myGrid->VJointValue(2))
                                   : 0.);
      Uf1 += shiftU;
      Ul1 += shiftU;
      Vf1 += shiftV;
      Vl1 += shiftV;
      // limit patch indices to be in range of grid (extended for periodic) (0, 2)
      // in same cases for example trj4_pm2-ug-203.stp (entity #8024) wire in 2D space has length
      // greater then period
      int iumin = std::max(0, GetPatchIndex(Uf1 + pprec, myGrid->UJointValues(), myUClosed));
      int iumax = GetPatchIndex(Ul1 - pprec, myGrid->UJointValues(), myUClosed) + 1;

      for (int j = 1; j <= wire.NbEdges(); j++)
      {
        wire.DefineIUMin(j, iumin);
        wire.DefineIUMax(j, iumax);
      }

      int ivmin = std::max(0, GetPatchIndex(Vf1 + pprec, myGrid->VJointValues(), myVClosed));
      int ivmax = GetPatchIndex(Vl1 - pprec, myGrid->VJointValues(), myVClosed) + 1;

      for (int j = 1; j <= wire.NbEdges(); j++)
      {
        wire.DefineIVMin(j, ivmin);
        wire.DefineIVMax(j, ivmax);
      }
    }
  }
  else
  {
    // limit patch indices to be in range of grid (extended for periodic)
    int iumin = GetPatchIndex(Uf + pprec, myGrid->UJointValues(), myUClosed);
    int iumax = GetPatchIndex(Ul - pprec, myGrid->UJointValues(), myUClosed) + 1;
    for (i = 1; i <= seqw.Length(); i++)
    {
      ShapeFix_WireSegment& wire = seqw(i);
      for (int j = 1; j <= wire.NbEdges(); j++)
      {
        wire.DefineIUMin(j, iumin);
        wire.DefineIUMax(j, iumax);
      }
    }
    int ivmin = GetPatchIndex(Vf + pprec, myGrid->VJointValues(), myVClosed);
    int ivmax = GetPatchIndex(Vl - pprec, myGrid->VJointValues(), myVClosed) + 1;
    for (i = 1; i <= seqw.Length(); i++)
    {
      ShapeFix_WireSegment& wire = seqw(i);
      for (int j = 1; j <= wire.NbEdges(); j++)
      {
        wire.DefineIVMin(j, ivmin);
        wire.DefineIVMax(j, ivmax);
      }
    }
  }
  // split by u lines

  for (i = (myUClosed ? 1 : 2); i <= myGrid->NbUPatches(); i++)
  {
    // clang-format off
    gp_Pnt2d pos ( myGrid->UJointValue(i), 0. ); // 0. - for infinite ranges: myGrid->VJointValue(1) ;
    // clang-format on
    gp_Lin2d line(pos, gp_Dir2d(gp_Dir2d::D::Y));
    if (!myClosedMode && myUClosed)
    {
      double period = Umax - Umin;
      double X      = pos.X();
      double sh     = ShapeAnalysis::AdjustToPeriod(X, Uf, Uf + period);
      for (; X + sh <= Ul + pprec; sh += period)
      {
        gp_Lin2d ln       = line.Translated(gp_Vec2d(sh, 0));
        int      cutIndex = GetPatchIndex(X + sh + pprec, myGrid->UJointValues(), myUClosed);
        SplitByLine(seqw, ln, true, cutIndex);
      }
    }
    else
      SplitByLine(seqw, line, true, i);
  }

  // split by v lines
  for (i = (myVClosed ? 1 : 2); i <= myGrid->NbVPatches(); i++)
  {
    gp_Pnt2d pos(0., myGrid->VJointValue(i));
    gp_Lin2d line(pos, gp_Dir2d(gp_Dir2d::D::X));
    if (!myClosedMode && myVClosed)
    {
      double period = Vmax - Vmin;
      double Y      = pos.Y();
      double sh     = ShapeAnalysis::AdjustToPeriod(Y, Vf, Vf + period);
      for (; Y + sh <= Vl + pprec; sh += period)
      {
        gp_Lin2d ln       = line.Translated(gp_Vec2d(0, sh));
        int      cutIndex = GetPatchIndex(Y + sh + pprec, myGrid->VJointValues(), myVClosed);
        SplitByLine(seqw, ln, false, cutIndex);
      }
    }
    else
      SplitByLine(seqw, line, false, i);
  }
}

//=================================================================================================

void ShapeFix_ComposeShell::BreakWires(NCollection_Sequence<ShapeFix_WireSegment>& seqw)
{
  // split all the wires by vertices
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> splitVertices;
  ShapeAnalysis_Edge                                     sae;

  // first collect splitting vertices
  int i; // svv #1
  for (i = 1; i <= seqw.Length(); i++)
  {
    TopAbs_Orientation ori_wire = seqw(i).Orientation();
    if (ori_wire != TopAbs_EXTERNAL && ori_wire != TopAbs_INTERNAL)
      continue;

    occ::handle<ShapeExtend_WireData> sbwd = seqw(i).WireData();
    for (int j = 1; j <= sbwd->NbEdges(); j++)
    {
      TopoDS_Edge        edge     = sbwd->Edge(j);
      TopAbs_Orientation ori_edge = (ori_wire == TopAbs_EXTERNAL ? ori_wire : edge.Orientation());
      if (ori_edge == TopAbs_EXTERNAL)
      {
        splitVertices.Add(sae.FirstVertex(edge));
        splitVertices.Add(sae.LastVertex(edge));
      }
    }
  }

  // and then split each vire
  // Here each wire is supposed to be connected (while probably not closed)
  for (i = 1; i <= seqw.Length(); i++)
  {
    TopAbs_Orientation   ori  = seqw(i).Orientation();
    ShapeFix_WireSegment wire = seqw(i);
    if (wire.IsVertex())
      continue;
    const occ::handle<ShapeExtend_WireData>& sbwd = wire.WireData();

    // find first vertex for split
    int j; // svv #1
    for (j = 1; j <= sbwd->NbEdges(); j++)
    {
      TopoDS_Vertex V = sae.FirstVertex(sbwd->Edge(j));
      if (splitVertices.Contains(V))
        break;
    }
    if (j > sbwd->NbEdges())
      continue; // splitting not needed

    // if first split of closed edge is not its start, make permutation
    int shift = 0;
    if (j > 1 && !myClosedMode && wire.IsClosed())
    {
      TopoDS_Vertex V = sae.FirstVertex(sbwd->Edge(1));
      if (!splitVertices.Contains(V))
        shift = j - 1;
      // wire.SetLast ( j-1 );
    }

    // perform splitting
    int                  nbnew = 0;
    ShapeFix_WireSegment newwire;
    TopAbs_Orientation   curOri = ori;
    for (int ind = 1; ind <= sbwd->NbEdges(); ind++)
    {
      j                  = 1 + (ind - 1 + shift) % sbwd->NbEdges();
      TopoDS_Edge   edge = sbwd->Edge(j);
      TopoDS_Vertex V    = sae.FirstVertex(edge);
      if (ind == 1 || splitVertices.Contains(V))
      {
        if (newwire.NbEdges())
        {
          newwire.Orientation(curOri);
          // ShapeFix_WireSegment seg ( newwire, ori );
          seqw.InsertBefore(i++, newwire);
          nbnew++;
        }
        newwire.Clear();
        curOri = ori;
      }
      int iumin, iumax, ivmin, ivmax;
      wire.GetPatchIndex(j, iumin, iumax, ivmin, ivmax);
      if (ori == TopAbs_INTERNAL && edge.Orientation() == TopAbs_EXTERNAL)
      {
        curOri = TopAbs_EXTERNAL;
        edge.Orientation(TopAbs_FORWARD);
        nbnew++;
      }
      newwire.AddEdge(0, edge, iumin, iumax, ivmin, ivmax);
    }
    if (nbnew)
    {
      newwire.Orientation(curOri);
      // ShapeFix_WireSegment seg ( newwire, ori );
      seqw.SetValue(i, newwire);
    }
  }
}

//=================================================================================================

// BUC60035 2053: check if wire segment is very short (in order not to skip it)
// 0  - long
// 1  - short even in 2d (to be taken always)
// -1 - short in 3d but not in 2d (to be checked after algo and atteching to
//      another wire if alone)
static int IsShortSegment(const ShapeFix_WireSegment&      seg,
                          const TopoDS_Face&               myFace,
                          const occ::handle<Geom_Surface>& myGrid,
                          const TopLoc_Location&           myLoc,
                          const double                     UResolution,
                          const double                     VResolution)
{
  TopoDS_Vertex Vf = seg.FirstVertex();
  if (!Vf.IsSame(seg.LastVertex()))
    return 0;

  gp_Pnt pnt  = BRep_Tool::Pnt(Vf);
  double tol  = BRep_Tool::Tolerance(Vf);
  double tol2 = tol * tol;

  int                                      code = 1;
  ShapeAnalysis_Edge                       sae;
  const occ::handle<ShapeExtend_WireData>& sbwd = seg.WireData();
  for (int i = 1; i <= sbwd->NbEdges(); i++)
  {
    TopoDS_Edge edge = sbwd->Edge(i);
    if (!Vf.IsSame(sae.LastVertex(edge)))
      return 0;
    occ::handle<Geom2d_Curve> c2d;
    double                    f, l;
    if (!sae.PCurve(edge, myFace, c2d, f, l))
      continue;

    // check 2d
    gp_Pnt2d endPnt = c2d->Value(l);
    gp_Pnt2d midPnt = c2d->Value((f + l) / 2);
    if (!IsCoincided(endPnt, midPnt, UResolution, VResolution, tol))
      code = -1;

    // check 3d
    gp_Pnt midPnt3d = myGrid->Value(midPnt.X(), midPnt.Y());
    if (!myLoc.IsIdentity())
      midPnt3d.Transform(myLoc.Transformation());
    if (midPnt3d.SquareDistance(pnt) > tol2)
      return 0;
  }
  return code;
}

//=================================================================================================

static bool IsSamePatch(const ShapeFix_WireSegment& wire,
                        const int                   NU,
                        const int                   NV,
                        int&                        iumin,
                        int&                        iumax,
                        int&                        ivmin,
                        int&                        ivmax,
                        const bool                  extend = false)
{
  // get patch indices for current segment
  int jumin, jumax, jvmin, jvmax;
  wire.GetPatchIndex(1, jumin, jumax, jvmin, jvmax);

  // shift to the same period
  int du = 0, dv = 0;
  if (jumin - iumin > NU)
    du = -(jumin - iumin) / NU;
  else if (iumin - jumin > NU)
    du = (iumin - jumin) / NU;
  if (jvmin - ivmin > NV)
    dv = -(jvmin - ivmin) / NV;
  else if (ivmin - jvmin > NV)
    dv = (ivmin - jvmin) / NV;
  if (du)
  {
    jumin += du * NU;
    jumax += du * NU;
  }
  if (dv)
  {
    jvmin += dv * NV;
    jvmax += dv * NV;
  }

  // compute common (extended) indices
  int  iun = std::min(iumin, jumin);
  int  iux = std::max(iumax, jumax);
  int  ivn = std::min(ivmin, jvmin);
  int  ivx = std::max(ivmax, jvmax);
  bool ok  = (iun == iux || iun + 1 == iux) && (ivn == ivx || ivn + 1 == ivx);
  if (ok && extend)
  {
    iumin = iun;
    iumax = iux;
    ivmin = ivn;
    ivmax = ivx;
  }
  return ok;
}

//=================================================================================================

void ShapeFix_ComposeShell::CollectWires(NCollection_Sequence<ShapeFix_WireSegment>& wires,
                                         NCollection_Sequence<ShapeFix_WireSegment>& seqw)
{
  ShapeAnalysis_Edge sae;
  int                i; // svv #1
  // Collect information on short closed segments
  NCollection_Array1<int> shorts(1, seqw.Length());
  for (i = 1; i <= seqw.Length(); i++)
  {
    if (seqw(i).IsVertex() || seqw(i).Orientation() == TopAbs_INTERNAL)
    {
      wires.Append(seqw(i));
      seqw.Remove(i);
      i--;
      continue;
    }
#ifdef OCCT_DEBUG
    for (int k = 1; !myClosedMode && k <= seqw(i).NbEdges(); k++)
      if (!seqw(i).CheckPatchIndex(k))
      {
        std::cout << "Warning: ShapeFix_ComposeShell::CollectWires: Wrong patch indices"
                  << std::endl;
        break;
      }
#endif
    int isshort = IsShortSegment(seqw(i), myFace, myGrid, myLoc, myUResolution, myVResolution);
    shorts.SetValue(i, isshort);
    if (isshort > 0
        && (seqw(i).Orientation() == TopAbs_EXTERNAL
            || (seqw(i).NbEdges() == 1 && //: abv 13.05.02: OCC320 - remove if degenerated
                BRep_Tool::Degenerated(seqw(i).Edge(1)))))
    {
#ifdef OCCT_DEBUG
      std::cout << "Info: ShapeFix_ComposeShell::CollectWires: Short segment ignored" << std::endl;
#endif
      seqw(i).Orientation(TopAbs_INTERNAL);
    }
  }

  occ::handle<ShapeExtend_WireData> sbwd;
  gp_Pnt2d                          endPnt, firstPnt;
  gp_Vec2d                          endTan, firstTan;
  TopoDS_Vertex                     firstV, endV;
  TopoDS_Edge                       firstEdge, lastEdge;
  double                            tol   = 0;
  int                               iumin = 0, iumax = 0, ivmin = 0, ivmax = 0;
  double                            dsu = 0., dsv = 0.;
  bool                              canBeClosed = false;
  for (;;)
  {
    int    index       = 0;
    bool   misoriented = true, samepatch = false;
    bool   reverse = false, connected = false;
    double angle = -M_PI, mindist = RealLast();
    int    weigth = 0;
    double shiftu = 0., shiftv = 0.;

    // find next segment to connect (or first if sbwd is NULL)
    for (i = 1; i <= seqw.Length(); i++)
    {
      const ShapeFix_WireSegment& seg = seqw.Value(i);
      if (seg.IsVertex())
        continue;
      TopAbs_Orientation anOr = seg.Orientation();
      if (anOr == TopAbs_INTERNAL)
        continue;

      // for first segment, take any
      if (sbwd.IsNull())
      {
        if (shorts(i) > 0)
          continue;
        if (anOr == TopAbs_EXTERNAL)
          continue;
        if (anOr == TopAbs_FORWARD)
          reverse = true;
        index = i;
        seg.GetPatchIndex(1, iumin, iumax, ivmin, ivmax);

        misoriented = false;
        dsu = dsv = 0.;
        break;
      }

      // check whether current segment is on the same patch with previous
      bool sp =
        IsSamePatch(seg, myGrid->NbUPatches(), myGrid->NbVPatches(), iumin, iumax, ivmin, ivmax);

      // not same patch has lowest priority
      if (!sp && (canBeClosed || (index && samepatch)))
        continue;

      // try to connect, with the following priorities:
      // The name of property      Weigth:
      // sharing vertex            auto
      // samepatch = 1             16
      // ! sameedge                auto
      // misorientation = 0        8
      // connected in 2d           4
      // distance                  2
      // short                     auto
      // angle ->> PI              1
      const occ::handle<ShapeExtend_WireData>& wire = seg.WireData();
      for (int j = 0; j < 2; j++)
      {
        if (!endV.IsSame(j ? seg.LastVertex() : seg.FirstVertex()))
          continue;

        // check for misorientation only if nothing better is found
        bool misor = (anOr == (j ? TopAbs_REVERSED : TopAbs_FORWARD));
        // if ( misor ) continue; // temporarily, to be improved

        // returning back by the same edge is lowest priority
        if (lastEdge.IsSame(wire->Edge(j ? wire->NbEdges() : 1)))
        {
          if (!index && !canBeClosed)
          { // || ( sp && ! samepatch ) ) {
            index       = i;
            reverse     = j != 0;
            connected   = true;
            misoriented = misor;
            samepatch   = sp;
            weigth      = (sp ? 16 : 0) + (connected ? 8 : 0) + (misor == 0 ? 4 : 0);
            dsu = dsv = 0.;
          }
          continue;
        }

        // compute starting tangent
        gp_Pnt2d lPnt;
        gp_Vec2d lVec;
        int      k;
        double   edgeTol = 0;
        for (k = 1; k <= wire->NbEdges(); k++)
        {
          TopoDS_Shape tmpE = wire->Edge(wire->NbEdges() - k + 1).Reversed();
          TopoDS_Edge  edge = (j ? TopoDS::Edge(tmpE) : wire->Edge(k));
          edgeTol           = BRep_Tool::Tolerance(edge);
          // if ( sae.GetEndTangent2d ( edge, myFace, false, lPnt, lVec ) ) break;
          if (sae.GetEndTangent2d(edge, myFace, false, lPnt, lVec, 1.e-3))
            break;
        }
        if (k > wire->NbEdges())
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);

        if (myClosedMode)
        {
          if (myUClosed)
          {
            shiftu = ShapeAnalysis::AdjustByPeriod(lPnt.X(), endPnt.X(), myUPeriod);
            lPnt.SetX(lPnt.X() + shiftu);
          }
          if (myVClosed)
          {
            shiftv = ShapeAnalysis::AdjustByPeriod(lPnt.Y(), endPnt.Y(), myVPeriod);
            lPnt.SetY(lPnt.Y() + shiftv);
          }
        }

        // short segment is to be taken with highest priority by angle
        double ang = (shorts(i) > 0 ? M_PI : endTan.Angle(lVec));
        if (myClosedMode && shorts(i) <= 0 && M_PI - ang < ::Precision::Angular())
          ang = 0.; // abv 21 Mar 00: trj3_s1-md-214.stp #2471: avoid going back

        // abv 05 Feb 02: face from Parasolid: use tolerance of edges for check
        // for coincidence (instead of vertex tolerance) in order
        // this check to be in agreement with check for position of wire segments
        // thus avoiding bad effects on overlapping edges
        double ctol = std::max(edgeTol, BRep_Tool::Tolerance(endV /*lastEdge*/));
        bool   conn = IsCoincided(endPnt, lPnt, myUResolution, myVResolution, ctol);
        double dist = endPnt.SquareDistance(lPnt);

        // check if case is better than last found
        int w1    = (sp ? 16 : 0) + (conn ? 4 : 0) + (misor == 0 ? 8 : 0);
        int tail1 = (!conn && (dist < mindist) ? 2 : 0) + (ang > angle ? 1 : 0);
        int tail2 = (!connected && (dist > mindist) ? 2 : 0) + (ang < angle ? 1 : 0);
        if (w1 + tail1 <= weigth + tail2)
          continue;

        index       = i;
        reverse     = j != 0;
        angle       = ang;
        mindist     = dist;
        connected   = conn;
        misoriented = misor;
        samepatch   = sp;
        weigth      = w1;
        dsu         = shiftu;
        dsv         = shiftv;
      }
    }

    // if next segment found, connect it
    if (index)
    {
      if (misoriented)
        myInvertEdgeStatus = true;
      ShapeFix_WireSegment seg = seqw.Value(index);
      if (sbwd.IsNull())
        sbwd = new ShapeExtend_WireData;
      else if (samepatch)
      { // extend patch indices
        IsSamePatch(seg,
                    myGrid->NbUPatches(),
                    myGrid->NbVPatches(),
                    iumin,
                    iumax,
                    ivmin,
                    ivmax,
                    true);
      }

      // for closed mode in case if current segment is seam segment it is necessary to detect
      // crossing seam edge in order to have possibility to take candidate from other patch
      if (myClosedMode)
        seg.GetPatchIndex(1, iumin, iumax, ivmin, ivmax);

      // TopAbs_Orientation or = seg.Orientation();
      if (!reverse)
        sbwd->Add(seg.WireData());
      else
      {
        occ::handle<ShapeExtend_WireData> wire = new ShapeExtend_WireData;
        wire->ManifoldMode()                   = false;
        wire->Add(seg.WireData());
        wire->Reverse(myFace);
        sbwd->Add(wire);
      }
      if (seg.Orientation() == TopAbs_EXTERNAL)
        seg.Orientation(reverse ? TopAbs_REVERSED : TopAbs_FORWARD);
      else
        seg.Orientation(TopAbs_INTERNAL);
      seqw.SetValue(index, seg);
    }
    else if (sbwd.IsNull())
      break; // stop when no free segments available
    // for first segment, remember start point
    if (endV.IsNull())
    {
      firstEdge = sbwd->Edge(1);
      firstV    = sae.FirstVertex(firstEdge);
      // sae.GetEndTangent2d ( firstEdge, myFace, false, firstPnt, firstTan );
      sae.GetEndTangent2d(firstEdge, myFace, false, firstPnt, firstTan, 1.e-3);
    }

    // update last edge and vertex (only for not short segments)
    bool doupdate = (index && (shorts(index) <= 0 || endV.IsNull()));
    if (doupdate)
    {
      lastEdge = sbwd->Edge(sbwd->NbEdges());
      endV     = sae.LastVertex(lastEdge);
      tol      = BRep_Tool::Tolerance(endV);
      // BUC60035 2053: iteration on edges is required
      int k; // svv #1
      for (k = sbwd->NbEdges(); k >= 1; k--)
        // if ( sae.GetEndTangent2d ( sbwd->Edge ( k ), myFace, true, endPnt, endTan ) )
        if (sae.GetEndTangent2d(sbwd->Edge(k), myFace, true, endPnt, endTan, 1.e-3))
          break;
      if (k < 1)
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      if (myUClosed)
        endPnt.SetX(endPnt.X() + dsu);
      if (myVClosed)
        endPnt.SetY(endPnt.Y() + dsv);
    }

    // if closed or no next segment found, add to wires
    canBeClosed = endV.IsSame(firstV);
    if (!index
        || (canBeClosed && !lastEdge.IsSame(firstEdge) && // cylinder (seam)
            IsCoincided(endPnt, firstPnt, myUResolution, myVResolution, 2. * tol)))
    {
      if (!endV.IsSame(sae.FirstVertex(firstEdge)))
      {
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL5);
#ifdef OCCT_DEBUG
        std::cout << "Warning: ShapeFix_ComposeShell::CollectWires: can't close wire" << std::endl;
#endif
      }
      ShapeFix_WireSegment s(sbwd, TopAbs_FORWARD);
      s.DefineIUMin(1, iumin);
      s.DefineIUMax(1, iumax);
      s.DefineIVMin(1, ivmin);
      s.DefineIVMax(1, ivmax);
      wires.Append(s);
      sbwd.Nullify();
      endV.Nullify();
      canBeClosed = false;
    }
  }

  // Check if some wires are short in 3d (lie entirely inside one vertex),
  // and if yes try to merge them with others
  // pdn The short seqments are still placed in "in" sequence.

  for (i = 1; i <= seqw.Length(); i++)
  {
    if (shorts(i) != 1 || seqw(i).IsVertex() || seqw(i).Orientation() == TopAbs_INTERNAL
        || seqw(i).Orientation() == TopAbs_EXTERNAL)
      continue;

    // find any other wire containing the same vertex
    occ::handle<ShapeExtend_WireData> wd   = seqw(i).WireData();
    TopoDS_Vertex                     V    = seqw(i).FirstVertex();
    int                               minj = 0, mink = 0;
    gp_Pnt2d                          p2d;
    gp_Vec2d                          vec;
    double                            mindist   = 0;
    bool                              samepatch = false;
    // int iumin, iumax, ivmin, ivmax;
    seqw(i).GetPatchIndex(1, iumin, iumax, ivmin, ivmax);
    sae.GetEndTangent2d(wd->Edge(1), myFace, false, p2d, vec);
    for (int j = 1; j <= wires.Length(); j++)
    {
      // if ( j == i ) continue;
      // occ::handle<ShapeExtend_WireData>
      sbwd = wires(j).WireData();
      for (int k = 1; k <= sbwd->NbEdges(); k++)
      {
        // clang-format off
        if ( !V.IsSame ( sae.FirstVertex ( sbwd->Edge(k) ) ) ) continue; //pdn I suppose that short segment should be inserted into the SAME vertex.
        // clang-format on

        bool sp = IsSamePatch(wires(j),
                              myGrid->NbUPatches(),
                              myGrid->NbVPatches(),
                              iumin,
                              iumax,
                              ivmin,
                              ivmax);
        if (samepatch && !sp)
          continue;
        gp_Pnt2d pp;
        sae.GetEndTangent2d(sbwd->Edge(k), myFace, false, pp, vec);
        double dist = pp.SquareDistance(p2d);
        if (sp && !samepatch)
        {
          minj      = j;
          mink      = k;
          mindist   = dist;
          samepatch = sp;
        }
        else if (!minj || mindist > dist)
        {
          minj      = j;
          mink      = k;
          mindist   = dist;
          samepatch = sp;
        }
      }
    }
    if (!minj)
    {
      // pdn add into resulting sequence!
      ShapeFix_WireSegment s(wd, TopAbs_FORWARD);
      wires.Append(s);
#ifdef OCCT_DEBUG
      std::cout << "Warning: Short segment processed as separate wire" << std::endl;
#endif
      continue;
    }

    // and if found, merge
    // occ::handle<ShapeExtend_WireData>
    sbwd = wires(minj).WireData();
    for (int n = 1; n <= wd->NbEdges(); n++)
      sbwd->Add(wd->Edge(n), mink++);

    // wires.Remove ( i );
    // i--;
  }
}

//=================================================================================================

static gp_Pnt2d GetMiddlePoint(const ShapeFix_WireSegment& wire, const TopoDS_Face& face)
{
  if (wire.IsVertex())
  {
    TopoDS_Vertex                      aV        = wire.GetVertex();
    gp_Pnt                             aP3D      = BRep_Tool::Pnt(aV);
    occ::handle<Geom_Surface>          surf      = BRep_Tool::Surface(face);
    occ::handle<ShapeAnalysis_Surface> aSurfTool = new ShapeAnalysis_Surface(surf);
    return aSurfTool->ValueOfUV(aP3D, Precision::Confusion());
  }
  Bnd_Box2d                                box;
  ShapeAnalysis_Edge                       sae;
  ShapeAnalysis_Curve                      sac;
  const occ::handle<ShapeExtend_WireData>& wd = wire.WireData();
  for (int i = 1; i <= wd->NbEdges(); i++)
  {
    TopoDS_Edge               E = wd->Edge(i);
    double                    cf, cl;
    occ::handle<Geom2d_Curve> c2d;
    if (sae.PCurve(E, face, c2d, cf, cl, false))
    {
      sac.FillBndBox(c2d, cf, cl, 3, false, box);
      // box.Add(c2d->Value(cf));
      // box.Add(c2d->Value(cl));
      // box.Add(c2d->Value((cl+cf)/2.));
    }
  }
  if (box.IsVoid())
    return gp_Pnt2d(0., 0.);
  double aXmin, aYmin, aXmax, aYmax;
  box.Get(aXmin, aYmin, aXmax, aYmax);
  return gp_Pnt2d(0.5 * (aXmax + aXmin), 0.5 * (aYmax + aYmin));
}

//=================================================================================================

void ShapeFix_ComposeShell::MakeFacesOnPatch(NCollection_Sequence<TopoDS_Shape>& faces,
                                             const occ::handle<Geom_Surface>&    surf,
                                             NCollection_Sequence<TopoDS_Shape>& loops) const
{
  BRep_Builder B;

  // Case of single loop: just add it to face
  if (loops.Length() == 1)
  {
    TopoDS_Face newFace;
    B.MakeFace(newFace, surf, myLoc, ::Precision::Confusion());
    const TopoDS_Shape& aSH = loops.Value(1);
    if (aSH.ShapeType() != TopAbs_WIRE)
      return;
    TopoDS_Wire wire = TopoDS::Wire(loops.Value(1));

    B.Add(newFace, wire);
    if (myInvertEdgeStatus)
    {
      occ::handle<ShapeFix_Face> sff = new ShapeFix_Face(newFace);
      sff->FixAddNaturalBoundMode()  = false;
      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
        MapWires;
      MapWires.Clear();
      sff->FixOrientation(MapWires);
      newFace = sff->Face();
    }

    faces.Append(newFace);
    return;
  }

  // For several loops, first find roots
  // make pseudo-face,
  TopoDS_Face pf;
  B.MakeFace(pf, surf, myLoc, ::Precision::Confusion());
  occ::handle<Geom_Surface> atSurf = BRep_Tool::Surface(pf);

  occ::handle<ShapeAnalysis_Surface> aSurfTool = new ShapeAnalysis_Surface(atSurf);
  NCollection_Sequence<TopoDS_Shape> roots;
  int                                i; // svv #1
  for (i = 1; i <= loops.Length(); i++)
  {
    gp_Pnt2d     unp;
    TopoDS_Wire  wr;
    TopoDS_Shape aShape = loops(i);
    if (aShape.ShapeType() != TopAbs_WIRE
        || (aShape.Orientation() != TopAbs_FORWARD && aShape.Orientation() != TopAbs_REVERSED))
      continue;

    wr = TopoDS::Wire(loops(i));
    TopoDS_Iterator ew(wr);
    if (!ew.More())
      continue;

    TopoDS_Edge ed = TopoDS::Edge(ew.Value());
    while (ed.Orientation() != TopAbs_FORWARD && ed.Orientation() != TopAbs_REVERSED)
    {
      ew.Next();
      if (ew.More())
        ed = TopoDS::Edge(ew.Value());
      else
        break;
    }
    if (!ew.More())
      continue;
    double                    cf, cl;
    occ::handle<Geom2d_Curve> cw = BRep_Tool::CurveOnSurface(ed, pf, cf, cl);
    if (cw.IsNull())
      continue;
    unp = cw->Value(0.5 * (cf + cl));

    int j; // svv #1
    for (j = 1; j <= loops.Length(); j++)
    {
      if (i == j)
        continue;
      TopoDS_Shape aShape2 = loops(j);
      if (aShape2.ShapeType() != TopAbs_WIRE
          || (aShape2.Orientation() != TopAbs_FORWARD && aShape2.Orientation() != TopAbs_REVERSED))
        continue;
      TopoDS_Wire w1 = TopoDS::Wire(aShape2);
      TopoDS_Wire awtmp;
      B.MakeWire(awtmp);
      awtmp.Orientation(TopAbs_FORWARD);
      TopoDS_Iterator aIt(w1);
      int             nbe = 0;
      for (; aIt.More(); aIt.Next())
      {
        if (aIt.Value().Orientation() == TopAbs_FORWARD
            || aIt.Value().Orientation() == TopAbs_REVERSED)
        {
          B.Add(awtmp, aIt.Value());
          nbe++;
        }
      }
      if (!nbe)
        continue;
      TopoDS_Face fc;
      B.MakeFace(fc, surf, myLoc, ::Precision::Confusion());
      B.Add(fc, awtmp);
      BRepTopAdaptor_FClass2d clas(fc, ::Precision::PConfusion());
      TopAbs_State            stPoint = clas.Perform(unp, false);
      if (stPoint == TopAbs_ON || stPoint == TopAbs_UNKNOWN)
      {

        TopoDS_Edge               anEdge = TopoDS::Edge(ew.Value());
        double                    aCF, aCL;
        occ::handle<Geom2d_Curve> aCW = BRep_Tool::CurveOnSurface(anEdge, pf, aCF, aCL);
        // handle tangential case (ON)
        while (stPoint == TopAbs_ON || stPoint == TopAbs_UNKNOWN)
        {
          stPoint = clas.Perform(aCW->Value(aCL), false);
          if (!ew.More())
            break;
          ew.Next();
          if (!ew.More())
            break;
          TopoDS_Edge edge = TopoDS::Edge(ew.Value());
          if (edge.Orientation() != TopAbs_FORWARD && edge.Orientation() != TopAbs_REVERSED)
            continue;
          occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(edge, pf, aCF, aCL);
          if (!c2d.IsNull())
            aCW = c2d;
        }
      }
      TopAbs_State stInfin = clas.PerformInfinitePoint();
      if (stPoint != stInfin)
        break;
    }
    if (j > loops.Length())
    {
      roots.Append(wr);
      // loops.Remove ( i-- );
    }
  }

  // And remove them from the list of loops
  for (i = 1; i <= loops.Length(); i++)
    for (int j = 1; j <= roots.Length(); j++)
      if (loops(i).IsSame(roots(j)))
      {
        loops.Remove(i--);
        break;
      }

  // check for lost wires, and if they are, make them roots
  if (roots.Length() <= 0 && loops.Length() > 0)
  {
#ifdef OCCT_DEBUG
    std::cout << "Error: ShapeFix_ComposeShell::MakeFacesOnPatch: can't dispatch wires"
              << std::endl;
#endif
    for (int j = 1; j <= loops.Length(); j++)
    {
      roots.Append(loops(j));
    }
    loops.Clear();
  }

  // Then iterate on loops
  for (i = 1; i <= roots.Length(); i++)
  {
    bool        reverse = false;
    TopoDS_Wire wire    = TopoDS::Wire(roots(i));
    TopoDS_Face fc;
    B.MakeFace(fc, surf, myLoc, ::Precision::Confusion());
    B.Add(fc, wire);
    BRepTopAdaptor_FClass2d clas(fc, ::Precision::PConfusion());
    if (clas.PerformInfinitePoint() == TopAbs_IN)
    {
      reverse = true;
#ifdef OCCT_DEBUG
      std::cout << "Warning: ShapeFix_ComposeShell::MakeFacesOnPatch: badly oriented wire"
                << std::endl;
#endif
    }

    // find all holes for that loop
    NCollection_Sequence<TopoDS_Shape> holes; // holes in holes not supported
    int                                j;     // svv #1
    for (j = 1; j <= loops.Length(); j++)
    {
      gp_Pnt2d unp;
      if (loops(j).ShapeType() == TopAbs_WIRE)
      {
        TopoDS_Wire     bw = TopoDS::Wire(loops(j));
        TopoDS_Iterator ew(bw);
        if (!ew.More())
          continue;
        TopoDS_Edge               ed = TopoDS::Edge(ew.Value());
        double                    cf, cl;
        occ::handle<Geom2d_Curve> cw = BRep_Tool::CurveOnSurface(ed, pf, cf, cl);
        if (cw.IsNull())
          continue;
        unp = cw->Value(0.5 * (cf + cl));
      }
      else if (loops(j).ShapeType() == TopAbs_VERTEX)
      {
        TopoDS_Vertex aV = TopoDS::Vertex(loops(j));
        gp_Pnt        aP = BRep_Tool::Pnt(aV);
        unp              = aSurfTool->ValueOfUV(aP, Precision::Confusion());
      }
      else
        continue;
      TopAbs_State state = clas.Perform(unp, false);
      if ((state == TopAbs_OUT) == reverse)
      {
        holes.Append(loops(j));
        loops.Remove(j--);
      }
    }

    // and add them to new face (no orienting is done)
    TopoDS_Face newFace;
    B.MakeFace(newFace, surf, myLoc, ::Precision::Confusion());
    B.Add(newFace, wire);
    for (j = 1; j <= holes.Length(); j++)
    {
      TopoDS_Shape aSh = holes(j);
      if (aSh.ShapeType() == TopAbs_VERTEX)
      {
        TopoDS_Vertex aNewV =
          ShapeAnalysis_TransferParametersProj::CopyNMVertex(TopoDS::Vertex(aSh), newFace, myFace);
        Context()->Replace(aSh, aNewV);
        B.Add(newFace, aNewV);
      }
      else
        B.Add(newFace, holes(j));
    }
    faces.Append(newFace);

    // check for lost wires, and if they are, make them roots
    if (i == roots.Length() && loops.Length() > 0)
    {
#ifdef OCCT_DEBUG
      std::cout << "Error: ShapeFix_ComposeShell::MakeFacesOnPatch: can't dispatch wires"
                << std::endl;
#endif
      for (j = 1; j <= loops.Length(); j++)
      {
        TopoDS_Shape aSh = loops(j);
        if (aSh.ShapeType() == TopAbs_WIRE
            && (aSh.Orientation() == TopAbs_FORWARD || aSh.Orientation() == TopAbs_REVERSED))
          roots.Append(loops(j));
      }
      loops.Clear();
    }
  }
}

//=================================================================================================

void ShapeFix_ComposeShell::DispatchWires(NCollection_Sequence<TopoDS_Shape>&         faces,
                                          NCollection_Sequence<ShapeFix_WireSegment>& wires) const
{
  BRep_Builder B;

  // in closed mode, apply FixShifted to all wires before dispatching them
  if (myClosedMode)
  {
    ShapeFix_Wire sfw;
    sfw.SetFace(myFace);
    sfw.SetPrecision(Precision());

    // pdn: shift pcurves in the seam to make OK shape w/o fixshifted
    int i;
    for (i = 1; i <= wires.Length(); i++)
    {
      if (wires(i).IsVertex())
        continue;
      occ::handle<ShapeExtend_WireData> sbwd = wires(i).WireData();

      for (int jL = 1; jL <= sbwd->NbEdges(); jL++)
      {
        TopoDS_Edge E = sbwd->Edge(jL);
        if (E.Orientation() == TopAbs_REVERSED && BRep_Tool::IsClosed(E, myFace))
        {
          double                    f1, l1, f2, l2;
          occ::handle<Geom2d_Curve> c21   = BRep_Tool::CurveOnSurface(E, myFace, f1, l1);
          TopoDS_Shape              dummy = E.Reversed();
          occ::handle<Geom2d_Curve> c22 =
            BRep_Tool::CurveOnSurface(TopoDS::Edge(dummy), myFace, f2, l2);
          constexpr double dPreci = ::Precision::PConfusion() * Precision::PConfusion();
          gp_Pnt2d         pf1    = c21->Value(f1);
          gp_Pnt2d         pl1    = c21->Value(l1);
          gp_Pnt2d         pf2    = c22->Value(f2);
          gp_Pnt2d         pl2    = c22->Value(l2);
          if (c21 == c22 || pf1.SquareDistance(pf2) < dPreci || pl1.SquareDistance(pl2) < dPreci)
          {
            gp_Vec2d shift(0., 0.);
            if (myUClosed && std::abs(pf2.X() - pl2.X()) < ::Precision::PConfusion())
              shift.SetX(myUPeriod);
            if (myVClosed && std::abs(pf2.Y() - pl2.Y()) < ::Precision::PConfusion())
              shift.SetY(myVPeriod);
            c22->Translate(shift);
          }
        }
      }
    }

    for (i = 1; i <= wires.Length(); i++)
    {
      if (wires(i).IsVertex())
        continue;
      occ::handle<ShapeExtend_WireData> sbwd = wires(i).WireData();

      //: abv 30.08.01: torHalf2.sat: if wire contains single degenerated
      // edge, skip that wire
      if (sbwd->NbEdges() <= 0 || (sbwd->NbEdges() == 1 && BRep_Tool::Degenerated(sbwd->Edge(1))))
      {
        wires.Remove(i--);
        continue;
      }

      sfw.Load(sbwd);
      sfw.FixShifted();

      // force recomputation of degenerated edges (clear pcurves)
      ShapeBuild_Edge sbe;
      for (int jL = 1; jL <= sbwd->NbEdges(); jL++)
      {
        if (BRep_Tool::Degenerated(sbwd->Edge(jL)))
          sbe.RemovePCurve(sbwd->Edge(jL), myFace);
        // sfw.FixDegenerated(jL);
      }
      sfw.FixDegenerated();
    }
  }

  // Compute center points for wires
  NCollection_Sequence<gp_Pnt2d> mPnts;
  int                            nb = wires.Length();

  // pdn protection on empty sequence
  if (nb == 0)
    return;

  int i; // svv #1
  for (i = 1; i <= nb; i++)
    mPnts.Append(GetMiddlePoint(wires(i), myFace));

  // Put each wire on its own surface patch (by reassigning pcurves)
  // and build 3d curve if necessary
  ShapeBuild_ReShape         rs;
  ShapeBuild_Edge            sbe;
  ShapeAnalysis_Edge         sae;
  occ::handle<ShapeFix_Edge> sfe = new ShapeFix_Edge;

  double U1, U2, V1, V2;
  myGrid->Bounds(U1, U2, V1, V2);
  for (i = 1; i <= nb; i++)
  {
    gp_Pnt2d pnt = mPnts(i);
    double   ush = 0., vsh = 0.;
    if (myUClosed)
    {
      ush = ShapeAnalysis::AdjustToPeriod(pnt.X(), U1, U2);
      pnt.SetX(pnt.X() + ush);
    }
    if (myVClosed)
    {
      vsh = ShapeAnalysis::AdjustToPeriod(pnt.Y(), V1, V2);
      pnt.SetY(pnt.Y() + vsh);
    }
    mPnts(i) = pnt;
    int indU = myGrid->LocateUParameter(pnt.X());
    int indV = myGrid->LocateVParameter(pnt.Y());

    // compute parametric transformation
    gp_Trsf2d T;
    double    uFact = 1.;
    bool      needT = myGrid->GlobalToLocalTransformation(indU, indV, uFact, T);
    if (ush != 0. || vsh != 0.)
    {
      gp_Trsf2d Sh;
      Sh.SetTranslation(gp_Vec2d(ush, vsh));
      T.Multiply(Sh);
      needT = true;
    }
    if (wires(i).IsVertex())
      continue;
    occ::handle<Geom_Surface> surf = myGrid->Patch(indU, indV);
    TopoDS_Face               face;
    B.MakeFace(face, surf, myLoc, ::Precision::Confusion());
    occ::handle<ShapeExtend_WireData> sewd = wires(i).WireData();
    for (int j = 1; j <= sewd->NbEdges(); j++)
    {
      // int nsplit = ApplyContext ( sewd, j, context );
      // if ( nsplit <1 ) { j--; continue; }

      TopoDS_Edge edge = sewd->Edge(j);

      // !! Accurately copy pcurves for SEAMS and SEAM-like edges !!

      // if edge is already copied, don`t copy any more
      TopoDS_Edge newEdge;
      TopoDS_Edge anInitEdge = edge;
      bool        ismanifold =
        (edge.Orientation() == TopAbs_FORWARD || edge.Orientation() == TopAbs_REVERSED);
      if (rs.IsRecorded(edge))
      {
        // smh#8
        TopoDS_Shape tmpNE = rs.Value(edge);
        newEdge            = TopoDS::Edge(tmpNE);
      }
      else
      {
        if (!ismanifold)
          anInitEdge.Orientation(TopAbs_FORWARD);

        newEdge = sbe.Copy(anInitEdge, false);
        if (!ismanifold)
          newEdge.Orientation(edge.Orientation());
        rs.Replace(edge, newEdge);
        Context()->Replace(edge, newEdge);
      }

      sbe.ReassignPCurve(newEdge, myFace, face);

      // transform pcurve to parametric space of patch
      if (needT)
      {
        double                    f, l;
        occ::handle<Geom2d_Curve> c2d;
        if (sae.PCurve(newEdge, face, c2d, f, l, false))
        {
          double                    newf = f, newl = l;
          occ::handle<Geom2d_Curve> c2dnew = sbe.TransformPCurve(c2d, T, uFact, newf, newl);
          if (BRep_Tool::IsClosed(newEdge, face))
          {
            double                    cf, cl;
            occ::handle<Geom2d_Curve> c2d2;
            // smh#8
            TopoDS_Shape tmpE = newEdge.Reversed();
            TopoDS_Edge  e2   = TopoDS::Edge(tmpE);
            if (sae.PCurve(e2, face, c2d2, cf, cl, false))
            {
              if (newEdge.Orientation() == TopAbs_FORWARD)
                B.UpdateEdge(newEdge, c2dnew, c2d2, face, 0.);
              else
                B.UpdateEdge(newEdge, c2d2, c2dnew, face, 0.);
            }
            else
              B.UpdateEdge(newEdge, c2dnew, face, 0.);
          }
          else
            B.UpdateEdge(newEdge, c2dnew, face, 0.);
          B.Range(newEdge, face, newf, newl);
          if ((newf != f || newl != l) && !BRep_Tool::Degenerated(newEdge))
            B.SameRange(newEdge, false);
        }
      }

      if (!BRep_Tool::SameRange(newEdge))
      {
        TopoDS_Edge etmp;
        if (!ismanifold)
        {
          TopoDS_Edge afe = TopoDS::Edge(newEdge.Oriented(TopAbs_FORWARD));
          etmp            = sbe.Copy(afe, false);
        }
        else
          etmp = sbe.Copy(newEdge, false);
        sfe->FixAddCurve3d(etmp);
        double                  cf, cl;
        occ::handle<Geom_Curve> c3d;
        if (sae.Curve3d(etmp, c3d, cf, cl, false))
        {
          B.UpdateEdge(newEdge, c3d, 0.);
          sbe.SetRange3d(newEdge, cf, cl);
        }
      }
      else
        sfe->FixAddCurve3d(newEdge);
      sewd->Set(newEdge, j);
    }
  }

  // Collect wires in packets lying on same surface and dispatch them
  NCollection_Array1<bool> used(1, nb);
  used.Init(false);
  for (;;)
  {
    NCollection_Sequence<TopoDS_Shape> loops;

    occ::handle<Geom_Surface> Surf;
    for (i = 1; i <= nb; i++)
    {
      if (used(i))
        continue;
      occ::handle<Geom_Surface> S = myGrid->Patch(mPnts(i));
      if (Surf.IsNull())
        Surf = S;
      else if (S != Surf)
        continue;
      used(i)                   = true;
      ShapeFix_WireSegment aSeg = wires(i);
      if (aSeg.IsVertex())
      {
        TopoDS_Vertex aVert = aSeg.GetVertex();
        if (aVert.Orientation() == TopAbs_INTERNAL)
          loops.Append(wires(i).GetVertex());
      }
      else
      {
        const occ::handle<ShapeExtend_WireData>& aWD = aSeg.WireData();
        if (!aWD.IsNull())
          loops.Append(aWD->Wire());
      }
    }
    if (Surf.IsNull())
      break;

    MakeFacesOnPatch(faces, Surf, loops);
  }
}

//=================================================================================================

void ShapeFix_ComposeShell::SetTransferParamTool(
  const occ::handle<ShapeAnalysis_TransferParameters>& TransferParam)
{
  myTransferParamTool = TransferParam;
}

//=================================================================================================

occ::handle<ShapeAnalysis_TransferParameters> ShapeFix_ComposeShell::GetTransferParamTool() const
{
  return myTransferParamTool;
}

//=================================================================================================

bool& ShapeFix_ComposeShell::ClosedMode()
{
  return myClosedMode;
}
