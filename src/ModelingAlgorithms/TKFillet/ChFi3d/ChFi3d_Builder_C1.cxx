// Created on: 1994-03-09
// Created by: Isabelle GRIGNON
// Copyright (c) 1994-1999 Matra Datavision
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

//  Modified by skv - Mon Jun  7 18:38:57 2004 OCC5898
//  Modified by skv - Thu Aug 21 11:55:58 2008 OCC20222

#include <Adaptor2d_Curve2d.hxx>
#include <Blend_FuncInv.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRepBlend_Line.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ChFi3d.hxx>
#include <ChFi3d_Builder.hxx>
#include <ChFi3d_Builder_0.hxx>
#include <ChFiDS_CommonPoint.hxx>
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_SurfData.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <ChFiDS_Stripe.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_Map.hxx>
#include <ChFiDS_Spine.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_LocateExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_IntSS.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_SolidSurfaceInterference.hxx>
#include <TopOpeBRepDS_Surface.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

#ifdef OCCT_DEBUG
  #ifdef DRAW
    #include <OSD_Chronometer.hxx>
    #include <DrawTrSurf.hxx>
  #endif // DRAW
//  Modified by Sergey KHROMOV - Thu Apr 11 12:23:40 2002 Begin
// The method
// ChFi3d_Builder::PerformMoreSurfdata(const int Index)
// is totally rewroted.
//  Modified by Sergey KHROMOV - Thu Apr 11 12:23:40 2002 End

extern double t_same, t_inter, t_sameinter;
extern void   ChFi3d_InitChron(OSD_Chronometer& ch);
extern void   ChFi3d_ResultChron(OSD_Chronometer& ch, double& time);
#endif
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <math_FunctionSample.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BoundedSurface.hxx>

static double recadre(const double p,
                      const double ref,
                      const bool   isfirst,
                      const double first,
                      const double last)
{
  double pp = p;
  if (isfirst)
    pp -= (last - first);
  else
    pp += (last - first);
  if (std::abs(pp - ref) < std::abs(p - ref))
    return pp;
  return p;
}

//=======================================================================
// function : Update
// purpose  : Calculate the intersection of the face at the end of
//           the tangency line to update CommonPoint and its
//           parameter in FaceInterference.
//=======================================================================

static bool Update(const occ::handle<Adaptor3d_Surface>& fb,
                   const occ::handle<Adaptor2d_Curve2d>& pcfb,
                   const occ::handle<Adaptor3d_Surface>& surf,
                   ChFiDS_FaceInterference&              fi,
                   ChFiDS_CommonPoint&                   cp,
                   gp_Pnt2d&                             p2dbout,
                   const bool                            isfirst,
                   double&                               pared,
                   double&                               wop,
                   const double                          tol)
{
  Adaptor3d_CurveOnSurface         c1(pcfb, fb);
  occ::handle<Geom2d_Curve>        pc  = fi.PCurveOnSurf();
  occ::handle<Geom2dAdaptor_Curve> hpc = new Geom2dAdaptor_Curve(pc);
  Adaptor3d_CurveOnSurface         c2(hpc, surf);
  Extrema_LocateExtCC              ext(c1, c2, pared, wop);
  if (ext.IsDone())
  {
    double dist2 = ext.SquareDistance();
    if (dist2 < tol * tol)
    {
      Extrema_POnCurv ponc1, ponc2;
      ext.Point(ponc1, ponc2);
      double parfb = ponc1.Parameter();
      p2dbout      = pcfb->Value(parfb);
      pared        = ponc1.Parameter();
      wop          = ponc2.Parameter();
      fi.SetParameter(wop, isfirst);
      cp.Reset();
      cp.SetPoint(ponc1.Value());
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : Update
// purpose  : Intersect surface <fb> and 3d curve <ct>
//           Update <isfirst> parameter of FaceInterference <fi> and point of
//           CommonPoint <cp>. Return new intersection parameters in <wop>
//           and <p2dbout>
//=======================================================================

static bool Update(const occ::handle<Adaptor3d_Surface>& fb,
                   const occ::handle<Adaptor3d_Curve>&   ct,
                   ChFiDS_FaceInterference&              fi,
                   ChFiDS_CommonPoint&                   cp,
                   gp_Pnt2d&                             p2dbout,
                   const bool                            isfirst,
                   double&                               wop)
{
  IntCurveSurface_HInter Intersection;
  // check if in KPart the limits of the tangency line
  // are already in place at this stage.
  // Modif lvt : the periodic cases are reframed, espercially if nothing was found.
  double w, uf = ct->FirstParameter(), ul = ct->LastParameter();

  double wbis = 0.;

  bool isperiodic = ct->IsPeriodic(), recadrebis = false;
  Intersection.Perform(ct, fb);
  if (Intersection.IsDone())
  {
    int    nbp = Intersection.NbPoints(), i, isol = 0, isolbis = 0;
    double dist    = Precision::Infinite();
    double distbis = Precision::Infinite();
    for (i = 1; i <= nbp; i++)
    {
      w = Intersection.Point(i).W();
      if (isperiodic)
        w = recadre(w, wop, isfirst, uf, ul);
      if (uf <= w && ul >= w && std::abs(w - wop) < dist)
      {
        isol = i;
        dist = std::abs(w - wop);
      }
    }
    if (isperiodic)
    {
      for (i = 1; i <= nbp; i++)
      {
        w = Intersection.Point(i).W();
        if (uf <= w && ul >= w && std::abs(w - wop) < distbis
            && (std::abs(w - ul) <= 0.01 || std::abs(w - uf) <= 0.01))
        {
          isolbis    = i;
          wbis       = recadre(w, wop, isfirst, uf, ul);
          distbis    = std::abs(wbis - wop);
          recadrebis = true;
        }
      }
    }
    if (isol == 0 && isolbis == 0)
    {
      return false;
    }
    if (!recadrebis)
    {
      IntCurveSurface_IntersectionPoint pint = Intersection.Point(isol);
      p2dbout.SetCoord(pint.U(), pint.V());
      w = pint.W();
      if (isperiodic)
        w = ElCLib::InPeriod(w, uf, ul);
    }
    else
    {
      if (dist > distbis)
      {
        IntCurveSurface_IntersectionPoint pint = Intersection.Point(isolbis);
        p2dbout.SetCoord(pint.U(), pint.V());
        w = wbis;
      }
      else
      {
        IntCurveSurface_IntersectionPoint pint = Intersection.Point(isol);
        p2dbout.SetCoord(pint.U(), pint.V());
        w = pint.W();
        w = ElCLib::InPeriod(w, uf, ul);
      }
    }
    fi.SetParameter(w, isfirst);
    cp.Reset();
    cp.SetPoint(ct->Value(w));
    wop = w;
    return true;
  }
  return false;
}

//=======================================================================
// function : IntersUpdateOnSame
// purpose  : Intersect  curve <c3dFI> of ChFi-<Fop> interference with extended
//           surface <HBs> of <Fprol> . Return intersection parameters in
//           <FprolUV>, <c3dU> and updating <FIop> and <CPop>
//           <HGs> is a surface of ChFi
//           <Fop> is a face having 2 edges at corner with OnSame state
//           <Fprol> is a face non-adjacent to spine edge
//           <Vtx> is a corner vertex
//=======================================================================

static bool IntersUpdateOnSame(occ::handle<GeomAdaptor_Surface>& HGs,
                               occ::handle<BRepAdaptor_Surface>& HBs,
                               const occ::handle<Geom_Curve>&    c3dFI,
                               const TopoDS_Face&                Fop,
                               const TopoDS_Face&                Fprol,
                               const TopoDS_Edge&                Eprol,
                               const TopoDS_Vertex&              Vtx,
                               const bool                        isFirst,
                               const double                      Tol,
                               ChFiDS_FaceInterference&          FIop,
                               ChFiDS_CommonPoint&               CPop,
                               gp_Pnt2d&                         FprolUV,
                               double&                           c3dU)
{
  // add more or less restrictive criterions to
  // decide if the intersection is done with the face at
  // extended end or if the end is sharp.
  double                         uf = FIop.FirstParameter();
  double                         ul = FIop.LastParameter();
  occ::handle<GeomAdaptor_Curve> Hc3df;
  if (c3dFI->IsPeriodic())
    Hc3df = new GeomAdaptor_Curve(c3dFI);
  else
    Hc3df = new GeomAdaptor_Curve(c3dFI, uf, ul);

  if (Update(HBs, Hc3df, FIop, CPop, FprolUV, isFirst, c3dU))
    return true;

  if (!ChFi3d::IsTangentFaces(Eprol, Fprol, Fop))
    return false;

  occ::handle<Geom2d_Curve> gpcprol = BRep_Tool::CurveOnSurface(Eprol, Fprol, uf, ul);
  if (gpcprol.IsNull())
    throw Standard_ConstructionError("Failed to get p-curve of edge");
  occ::handle<Geom2dAdaptor_Curve> pcprol  = new Geom2dAdaptor_Curve(gpcprol);
  double                           partemp = BRep_Tool::Parameter(Vtx, Eprol);

  return Update(HBs, pcprol, HGs, FIop, CPop, FprolUV, isFirst, partemp, c3dU, Tol);
}

//=======================================================================
// function : Update
// purpose  : Calculate the extrema curveonsurf/curveonsurf to prefer
//           the values concerning the trace on surf and the pcurve on the
//           face at end.
//=======================================================================

static bool Update(const occ::handle<Adaptor3d_Surface>& face,
                   const occ::handle<Adaptor2d_Curve2d>& edonface,
                   const occ::handle<Adaptor3d_Surface>& surf,
                   ChFiDS_FaceInterference&              fi,
                   ChFiDS_CommonPoint&                   cp,
                   const bool                            isfirst)
{
  if (!cp.IsOnArc())
    return false;
  Adaptor3d_CurveOnSurface  c1(edonface, face);
  double                    pared      = cp.ParameterOnArc();
  double                    parltg     = fi.Parameter(isfirst);
  occ::handle<Geom2d_Curve> pc         = fi.PCurveOnSurf();
  double                    f          = fi.FirstParameter();
  double                    l          = fi.LastParameter();
  double                    delta      = 0.1 * (l - f);
  f                                    = std::max(f - delta, pc->FirstParameter());
  l                                    = std::min(l + delta, pc->LastParameter());
  occ::handle<Geom2dAdaptor_Curve> hpc = new Geom2dAdaptor_Curve(pc, f, l);
  Adaptor3d_CurveOnSurface         c2(hpc, surf);

  Extrema_LocateExtCC ext(c1, c2, pared, parltg);
  if (ext.IsDone())
  {
    Extrema_POnCurv ponc1, ponc2;
    ext.Point(ponc1, ponc2);
    pared  = ponc1.Parameter();
    parltg = ponc2.Parameter();
    if ((parltg > f) && (parltg < l))
    {
      ////modified by jgv, 10.05.2012 for the bug 23139, 25657////
      occ::handle<Geom2d_Curve> PConF = fi.PCurveOnFace();
      if (!PConF.IsNull())
      {
        occ::handle<Geom2d_TrimmedCurve> aTrCurve = occ::down_cast<Geom2d_TrimmedCurve>(PConF);
        if (!aTrCurve.IsNull())
          PConF = aTrCurve->BasisCurve();
        if (!PConF->IsPeriodic())
        {
          if (isfirst)
          {
            double fpar = PConF->FirstParameter();
            if (parltg < fpar)
              parltg = fpar;
          }
          else
          {
            double lpar = PConF->LastParameter();
            if (parltg > lpar)
              parltg = lpar;
          }
        }
      }
      /////////////////////////////////////////////////////
      fi.SetParameter(parltg, isfirst);
      cp.SetArc(cp.Tolerance(), cp.Arc(), pared, cp.TransitionOnArc());
      return true;
    }
  }
  return false;
}

//=================================================================================================

static void ChFi3d_ExtendSurface(occ::handle<Geom_Surface>& S, int& prol)
{
  if (prol)
    return;

  prol = (S->IsKind(STANDARD_TYPE(Geom_BSplineSurface))  ? 1
          : S->IsKind(STANDARD_TYPE(Geom_BezierSurface)) ? 2
                                                         : 0);
  if (!prol)
    return;

  double length, umin, umax, vmin, vmax;
  gp_Pnt P1, P2;
  S->Bounds(umin, umax, vmin, vmax);
  S->D0(umin, vmin, P1);
  S->D0(umax, vmax, P2);
  length = P1.Distance(P2);

  occ::handle<Geom_BoundedSurface> aBS = occ::down_cast<Geom_BoundedSurface>(S);
  GeomLib::ExtendSurfByLength(aBS, length, 1, false, true);
  GeomLib::ExtendSurfByLength(aBS, length, 1, true, true);
  GeomLib::ExtendSurfByLength(aBS, length, 1, false, false);
  GeomLib::ExtendSurfByLength(aBS, length, 1, true, false);
  S = aBS;
}

//=======================================================================
// function : ComputeCurve2d
// purpose  : calculate the 2d of the curve Ct on face Face
//=======================================================================

static void ComputeCurve2d(const occ::handle<Geom_Curve>& Ct,
                           TopoDS_Face&                   Face,
                           occ::handle<Geom2d_Curve>&     C2d)
{
  TopoDS_Edge                                                   E1;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapE1;
  BRepLib_MakeEdge                                              Bedge(Ct);
  TopoDS_Edge                                                   edg = Bedge.Edge();
  BRepAlgo_NormalProjection                                     OrtProj;
  OrtProj.Init(Face);
  OrtProj.Add(edg);
  OrtProj.SetParams(1.e-6, 1.e-6, GeomAbs_C1, 14, 16);
  OrtProj.SetLimit(false);
  OrtProj.Compute3d(false);
  OrtProj.Build();
  double up1, up2;
  if (OrtProj.IsDone())
  {
    TopExp::MapShapes(OrtProj.Projection(), TopAbs_EDGE, MapE1);
    if (MapE1.Extent() != 0)
    {
      TopoDS_Shape aLocalShape = TopoDS_Shape(MapE1(1));
      E1                       = TopoDS::Edge(aLocalShape);
      //      E1=TopoDS::Edge( TopoDS_Shape (MapE1(1)));
      C2d = BRep_Tool::CurveOnSurface(E1, Face, up1, up2);
    }
  }
}

//=================================================================================================

static void ChFi3d_Recale(BRepAdaptor_Surface& Bs, gp_Pnt2d& p1, gp_Pnt2d& p2, const bool refon1)
{
  occ::handle<Geom_Surface>                   surf = Bs.ChangeSurface().Surface();
  occ::handle<Geom_RectangularTrimmedSurface> ts =
    occ::down_cast<Geom_RectangularTrimmedSurface>(surf);
  if (!ts.IsNull())
    surf = ts->BasisSurface();
  if (surf->IsUPeriodic())
  {
    double u1 = p1.X(), u2 = p2.X();
    double uper = surf->UPeriod();
    if (fabs(u2 - u1) > 0.5 * uper)
    {
      if (u2 < u1 && refon1)
        u2 += uper;
      else if (u2 < u1 && !refon1)
        u1 -= uper;
      else if (u1 < u2 && refon1)
        u2 -= uper;
      else if (u1 < u2 && !refon1)
        u1 += uper;
    }
    p1.SetX(u1);
    p2.SetX(u2);
  }
  if (surf->IsVPeriodic())
  {
    double v1 = p1.Y(), v2 = p2.Y();
    double vper = surf->VPeriod();
    if (fabs(v2 - v1) > 0.5 * vper)
    {
      if (v2 < v1 && refon1)
        v2 += vper;
      else if (v2 < v1 && !refon1)
        v1 -= vper;
      else if (v1 < v2 && refon1)
        v2 -= vper;
      else if (v1 < v2 && !refon1)
        v1 += vper;
    }
    p1.SetY(v1);
    p2.SetY(v2);
  }
}

//=======================================================================
// function : ChFi3d_SelectStripe
// purpose  : find stripe with ChFiDS_OnSame state if <thePrepareOnSame> is True
//=======================================================================

bool ChFi3d_SelectStripe(NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator& It,
                         const TopoDS_Vertex&                                    Vtx,
                         const bool                                              thePrepareOnSame)
{
  if (!thePrepareOnSame)
    return true;

  for (; It.More(); It.Next())
  {
    int                        sens   = 0;
    occ::handle<ChFiDS_Stripe> stripe = It.Value();
    ChFi3d_IndexOfSurfData(Vtx, stripe, sens);
    ChFiDS_State stat;
    if (sens == 1)
      stat = stripe->Spine()->FirstStatus();
    else
      stat = stripe->Spine()->LastStatus();
    if (stat == ChFiDS_OnSame)
      return true;
  }

  return false;
}

//=======================================================================
// function : PerformOneCorner
// purpose  : Calculate a corner with three edges and a fillet.
//           3 separate case: (22/07/94 only 1st is implemented)
//
//           - same concavity on three edges, intersection with the
//             face at end,
//           - concavity of 2 outgoing edges is opposite to the one of the fillet,
//             if the face at end is ready for that, the same in  case 1 on extended face,
//             otherwise a small cap is done with GeomFill,
//           - only one outgoing edge has concavity opposed to the edge of the
//             fillet and the third edge, the top of the corner is reread
//             in the empty of the fillet and closed, either by extending the face
//             at end if it is plane and orthogonal to the
//             guiding edge, or by a cap of type GeomFill.
//
//           <thePrepareOnSame> means that only needed thing is redefinition
//           of intersection pameter of OnSame-Stripe with <Arcprol>
//           (eap, Arp 9 2002, occ266)
//=======================================================================

void ChFi3d_Builder::PerformOneCorner(const int Index, const bool thePrepareOnSame)
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();

#ifdef OCCT_DEBUG
  OSD_Chronometer ch; // init perf for PerformSetOfKPart
#endif
  // the top,
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  // The fillet is returned,
  NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator StrIt;
  StrIt.Initialize(myVDataMap(Index));
  if (!ChFi3d_SelectStripe(StrIt, Vtx, thePrepareOnSame))
    return;
  occ::handle<ChFiDS_Stripe>                          stripe = StrIt.Value();
  const occ::handle<ChFiDS_Spine>                     spine  = stripe->Spine();
  NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqFil =
    stripe->ChangeSetOfSurfData()->ChangeSequence();
  // SurfData and its CommonPoints,
  int sens = 0;

  // Choose proper SurfData
  int  num     = ChFi3d_IndexOfSurfData(Vtx, stripe, sens);
  bool isfirst = (sens == 1);
  if (isfirst)
  {
    for (; num < SeqFil.Length()
           && ((SeqFil.Value(num)->IndexOfS1() == 0) || (SeqFil.Value(num)->IndexOfS2() == 0));)
    {
      SeqFil.Remove(num); // The surplus is removed
    }
  }
  else
  {
    for (; num > 1
           && ((SeqFil.Value(num)->IndexOfS1() == 0) || (SeqFil.Value(num)->IndexOfS2() == 0));)
    {
      SeqFil.Remove(num); // The surplus is removed
      num--;
    }
  }

  occ::handle<ChFiDS_SurfData>& Fd  = SeqFil.ChangeValue(num);
  ChFiDS_CommonPoint&           CV1 = Fd->ChangeVertex(isfirst, 1);
  ChFiDS_CommonPoint&           CV2 = Fd->ChangeVertex(isfirst, 2);
  // To evaluate the new points.
  Bnd_Box box1, box2;

  // The cases of cap and intersection are processed separately.
  // ----------------------------------------------------------
  ChFiDS_State stat;
  if (isfirst)
    stat = spine->FirstStatus();
  else
    stat = spine->LastStatus();
  bool        onsame = (stat == ChFiDS_OnSame);
  TopoDS_Face Fv, Fad, Fop;
  TopoDS_Edge Arcpiv, Arcprol, Arcspine;
  if (isfirst)
    Arcspine = spine->Edges(1);
  else
    Arcspine = spine->Edges(spine->NbEdges());
  TopAbs_Orientation               OArcprolv = TopAbs_FORWARD, OArcprolop = TopAbs_FORWARD;
  int                              ICurve;
  occ::handle<BRepAdaptor_Surface> HBs  = new BRepAdaptor_Surface();
  occ::handle<BRepAdaptor_Surface> HBad = new BRepAdaptor_Surface();
  occ::handle<BRepAdaptor_Surface> HBop = new BRepAdaptor_Surface();
  BRepAdaptor_Surface&             Bs   = *HBs;
  BRepAdaptor_Surface&             Bad  = *HBad;
  BRepAdaptor_Surface&             Bop  = *HBop;
  occ::handle<Geom_Curve>          Cc;
  occ::handle<Geom2d_Curve>        Pc, Ps;
  double                           Ubid, Vbid; //,mu,Mu,mv,Mv;
  double                           Udeb = 0., Ufin = 0.;
  //  gp_Pnt2d UVf1,UVl1,UVf2,UVl2;
  //  double Du,Dv,Step;
  bool inters  = true;
  int  IFadArc = 1, IFopArc = 2;
  Fop = TopoDS::Face(DStr.Shape(Fd->Index(IFopArc)));
  TopExp_Explorer ex;

#ifdef OCCT_DEBUG
  ChFi3d_InitChron(ch); // init perf condition  if (onsame)
#endif

  if (onsame)
  {
    if (!CV1.IsOnArc() && !CV2.IsOnArc())
      throw Standard_ConstructionError("Corner OnSame : no point on arc");
    else if (CV1.IsOnArc() && CV2.IsOnArc())
    {
      bool sur1 = false, sur2 = false;
      for (ex.Init(CV1.Arc(), TopAbs_VERTEX); ex.More(); ex.Next())
      {
        if (Vtx.IsSame(ex.Current()))
        {
          sur1 = true;
          break;
        }
      }
      for (ex.Init(CV2.Arc(), TopAbs_VERTEX); ex.More(); ex.Next())
      {
        if (Vtx.IsSame(ex.Current()))
        {
          sur2 = true;
          break;
        }
      }
      if (sur1 && sur2)
      {
        TopoDS_Edge E[3];
        E[0] = CV1.Arc();
        E[1] = CV2.Arc();
        E[2] = Arcspine;
        if (ChFi3d_EdgeState(E, myEFMap) != ChFiDS_OnDiff)
          IFadArc = 2;
      }
      else if (sur2)
        IFadArc = 2;
    }
    else if (CV2.IsOnArc())
      IFadArc = 2;
    IFopArc = 3 - IFadArc;

    Arcpiv = Fd->Vertex(isfirst, IFadArc).Arc();
    Fad    = TopoDS::Face(DStr.Shape(Fd->Index(IFadArc)));
    Fop    = TopoDS::Face(DStr.Shape(Fd->Index(IFopArc)));
    NCollection_List<TopoDS_Shape>::Iterator It;
    // The face at end is returned without check of its unicity.
    for (It.Initialize(myEFMap(Arcpiv)); It.More(); It.Next())
    {
      if (!Fad.IsSame(It.Value()))
      {
        Fv = TopoDS::Face(It.Value());
        break;
      }
    }

    // Does the face at bout contain the Vertex ?
    bool isinface = false;
    for (ex.Init(Fv, TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (ex.Current().IsSame(Vtx))
      {
        isinface = true;
        break;
      }
    }
    if (!isinface && Fd->Vertex(isfirst, 3 - IFadArc).IsOnArc())
    {
      IFadArc = 3 - IFadArc;
      IFopArc = 3 - IFopArc;
      Arcpiv  = Fd->Vertex(isfirst, IFadArc).Arc();
      Fad     = TopoDS::Face(DStr.Shape(Fd->Index(IFadArc)));
      Fop     = TopoDS::Face(DStr.Shape(Fd->Index(IFopArc)));
      // NCollection_List<TopoDS_Shape>::Iterator It;
      // The face at end is returned without check of its unicity.
      for (It.Initialize(myEFMap(Arcpiv)); It.More(); It.Next())
      {
        if (!Fad.IsSame(It.Value()))
        {
          Fv = TopoDS::Face(It.Value());
          break;
        }
      }
    }

    if (Fv.IsNull())
      throw StdFail_NotDone("OneCorner : face at end not found");

    Fv.Orientation(TopAbs_FORWARD);
    Fad.Orientation(TopAbs_FORWARD);
    Fop.Orientation(TopAbs_FORWARD);

    // The edge that will be extended is returned.
    for (It.Initialize(myVEMap(Vtx)); It.More() && Arcprol.IsNull(); It.Next())
    {
      if (!Arcpiv.IsSame(It.Value()))
      {
        for (ex.Init(Fv, TopAbs_EDGE); ex.More(); ex.Next())
        {
          if (It.Value().IsSame(ex.Current()))
          {
            Arcprol   = TopoDS::Edge(It.Value());
            OArcprolv = ex.Current().Orientation();
            break;
          }
        }
      }
    }
    if (Arcprol.IsNull()) /*throw StdFail_NotDone("OneCorner : edge a prolonger non trouve");*/
    {
      PerformIntersectionAtEnd(Index);
      return;
    }
    for (ex.Init(Fop, TopAbs_EDGE); ex.More(); ex.Next())
    {
      if (Arcprol.IsSame(ex.Current()))
      {
        OArcprolop = ex.Current().Orientation();
        break;
      }
    }
    TopoDS_Face               FFv;
    double                    tol;
    int                       prol = 0;
    BRep_Builder              BRE;
    occ::handle<Geom_Surface> Sface;
    Sface = BRep_Tool::Surface(Fv);
    ChFi3d_ExtendSurface(Sface, prol);
    tol = BRep_Tool::Tolerance(Fv);
    BRE.MakeFace(FFv, Sface, tol);
    if (prol)
    {
      Bs.Initialize(FFv, false);
      DStr.SetNewSurface(Fv, Sface);
    }
    else
      Bs.Initialize(Fv, false);
    Bad.Initialize(Fad);
    Bop.Initialize(Fop);
  }
  // in case of OnSame it is necessary to modify the CommonPoint
  // in the empty and its parameter in the FaceInterference.
  // They are both returned in non const references. Attention the modifications are done behind
  // de CV1,CV2,Fi1,Fi2.
  ChFiDS_CommonPoint&      CPopArc = Fd->ChangeVertex(isfirst, IFopArc);
  ChFiDS_FaceInterference& FiopArc = Fd->ChangeInterference(IFopArc);
  ChFiDS_CommonPoint&      CPadArc = Fd->ChangeVertex(isfirst, IFadArc);
  ChFiDS_FaceInterference& FiadArc = Fd->ChangeInterference(IFadArc);
  // the parameter of the vertex in the air is initialiced with the value of
  // its opposite (point on arc).
  double                           wop = Fd->ChangeInterference(IFadArc).Parameter(isfirst);
  occ::handle<Geom_Curve>          c3df;
  occ::handle<GeomAdaptor_Surface> HGs =
    new GeomAdaptor_Surface(DStr.Surface(Fd->Surf()).Surface());
  gp_Pnt2d p2dbout;

  if (onsame)
  {

    ChFiDS_CommonPoint saveCPopArc = CPopArc;
    c3df                           = DStr.Curve(FiopArc.LineIndex()).Curve();

    inters = IntersUpdateOnSame(HGs,
                                HBs,
                                c3df,
                                Fop,
                                Fv,
                                Arcprol,
                                Vtx,
                                isfirst,
                                10 * tolapp3d, // in
                                FiopArc,
                                CPopArc,
                                p2dbout,
                                wop); // out

    occ::handle<BRepAdaptor_Curve2d> pced = new BRepAdaptor_Curve2d();
    pced->Initialize(CPadArc.Arc(), Fv);
    // in the case of degenerated Fi, parameter difference can be even negative (eap, occ293)
    if ((FiadArc.LastParameter() - FiadArc.FirstParameter()) > 10 * tolesp)
      Update(HBs, pced, HGs, FiadArc, CPadArc, isfirst);

    if (thePrepareOnSame)
    {
      // saveCPopArc.SetParameter(wop);
      saveCPopArc.SetPoint(CPopArc.Point());
      CPopArc = saveCPopArc;
      return;
    }
  }
  else
  {
    inters = FindFace(Vtx, CV1, CV2, Fv, Fop);
    if (!inters)
    {
      PerformIntersectionAtEnd(Index);
      return;
    }
    Bs.Initialize(Fv);
    occ::handle<BRepAdaptor_Curve2d> pced = new BRepAdaptor_Curve2d();
    pced->Initialize(CV1.Arc(), Fv);
    Update(HBs, pced, HGs, Fd->ChangeInterferenceOnS1(), CV1, isfirst);
    pced->Initialize(CV2.Arc(), Fv);
    Update(HBs, pced, HGs, Fd->ChangeInterferenceOnS2(), CV2, isfirst);
  }

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_same); // result perf condition if (same)
  ChFi3d_InitChron(ch);           // init perf condition if (inters)
#endif

  TopoDS_Edge                    edgecouture;
  bool                           couture, intcouture = false;
  double                         tolreached = tolapp3d;
  double                         par1 = 0., par2 = 0.;
  int                            indpt = 0, Icurv1 = 0, Icurv2 = 0;
  occ::handle<Geom_TrimmedCurve> curv1, curv2;
  occ::handle<Geom2d_Curve>      c2d1, c2d2;

  int Isurf = Fd->Surf();

  if (inters)
  {
    HGs                                = ChFi3d_BoundSurf(DStr, Fd, 1, 2);
    const ChFiDS_FaceInterference& Fi1 = Fd->InterferenceOnS1();
    const ChFiDS_FaceInterference& Fi2 = Fd->InterferenceOnS2();
    NCollection_Array1<double>     Pardeb(1, 4), Parfin(1, 4);
    gp_Pnt2d                       pfil1, pfac1, pfil2, pfac2;
    occ::handle<Geom2d_Curve>      Hc1, Hc2;
    if (onsame && IFopArc == 1)
      pfac1 = p2dbout;
    else
    {
      Hc1 = BRep_Tool::CurveOnSurface(CV1.Arc(), Fv, Ubid, Ubid);
      if (Hc1.IsNull())
        throw Standard_ConstructionError("Failed to get p-curve of edge");
      pfac1 = Hc1->Value(CV1.ParameterOnArc());
    }
    if (onsame && IFopArc == 2)
      pfac2 = p2dbout;
    else
    {
      Hc2 = BRep_Tool::CurveOnSurface(CV2.Arc(), Fv, Ubid, Ubid);
      if (Hc2.IsNull())
        throw Standard_ConstructionError("Failed to get p-curve of edge");
      pfac2 = Hc2->Value(CV2.ParameterOnArc());
    }
    if (Fi1.LineIndex() != 0)
    {
      pfil1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(isfirst));
    }
    else
    {
      pfil1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(!isfirst));
    }
    if (Fi2.LineIndex() != 0)
    {
      pfil2 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(isfirst));
    }
    else
    {
      pfil2 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(!isfirst));
    }
    if (onsame)
      ChFi3d_Recale(Bs, pfac1, pfac2, (IFadArc == 1));

    Pardeb(1) = pfil1.X();
    Pardeb(2) = pfil1.Y();
    Pardeb(3) = pfac1.X();
    Pardeb(4) = pfac1.Y();
    Parfin(1) = pfil2.X();
    Parfin(2) = pfil2.Y();
    Parfin(3) = pfac2.X();
    Parfin(4) = pfac2.Y();

    double uu1, uu2, vv1, vv2;
    ChFi3d_Boite(pfac1, pfac2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bs, uu1, uu2, vv1, vv2);

    if (!ChFi3d_ComputeCurves(HGs, HBs, Pardeb, Parfin, Cc, Ps, Pc, tolapp3d, tol2d, tolreached))
      throw Standard_Failure("OneCorner : echec calcul intersection");

    Udeb = Cc->FirstParameter();
    Ufin = Cc->LastParameter();

    //  determine if the curve has an intersection with edge of sewing

    ChFi3d_Couture(Fv, couture, edgecouture);

    if (couture && !BRep_Tool::Degenerated(edgecouture))
    {

      // double Ubid,Vbid;
      occ::handle<Geom_Curve>        C     = BRep_Tool::Curve(edgecouture, Ubid, Vbid);
      occ::handle<Geom_TrimmedCurve> Ctrim = new Geom_TrimmedCurve(C, Ubid, Vbid);
      GeomAdaptor_Curve              cur1(Ctrim->BasisCurve());
      GeomAdaptor_Curve              cur2(Cc);
      Extrema_ExtCC                  extCC(cur1, cur2);
      if (extCC.IsDone() && extCC.NbExt() != 0)
      {
        int    imin     = 0;
        double distmin2 = RealLast();
        for (int i = 1; i <= extCC.NbExt(); i++)
          if (extCC.SquareDistance(i) < distmin2)
          {
            distmin2 = extCC.SquareDistance(i);
            imin     = i;
          }
        if (distmin2 <= Precision::SquareConfusion())
        {
          Extrema_POnCurv ponc1, ponc2;
          extCC.Points(imin, ponc1, ponc2);
          par1       = ponc1.Parameter();
          par2       = ponc2.Parameter();
          double Tol = 1.e-4;
          if (std::abs(par2 - Udeb) > Tol && std::abs(Ufin - par2) > Tol)
          {
            gp_Pnt             P1 = ponc1.Value();
            TopOpeBRepDS_Point tpoint(P1, Tol);
            indpt      = DStr.AddPoint(tpoint);
            intcouture = true;
            curv1      = new Geom_TrimmedCurve(Cc, Udeb, par2);
            curv2      = new Geom_TrimmedCurve(Cc, par2, Ufin);
            TopOpeBRepDS_Curve tcurv1(curv1, tolreached);
            TopOpeBRepDS_Curve tcurv2(curv2, tolreached);
            Icurv1 = DStr.AddCurve(tcurv1);
            Icurv2 = DStr.AddCurve(tcurv2);
          }
        }
      }
    }
  }
  else
  { // (!inters)
    throw Standard_NotImplemented("OneCorner : bouchon non ecrit");
  }
  int                IShape = DStr.AddShape(Fv);
  TopAbs_Orientation Et     = TopAbs_FORWARD;
  if (IFadArc == 1)
  {
    TopExp_Explorer Exp;
    for (Exp.Init(Fv.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp.More(); Exp.Next())
    {
      if (Exp.Current().IsSame(CV1.Arc()))
      {
        Et = TopAbs::Reverse(TopAbs::Compose(Exp.Current().Orientation(), CV1.TransitionOnArc()));
        break;
      }
    }
  }
  else
  {
    TopExp_Explorer Exp;
    for (Exp.Init(Fv.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp.More(); Exp.Next())
    {
      if (Exp.Current().IsSame(CV2.Arc()))
      {
        Et = TopAbs::Compose(Exp.Current().Orientation(), CV2.TransitionOnArc());
        break;
      }
    }

    //
  }

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_inter); // result perf condition if (inter)
  ChFi3d_InitChron(ch);            // init perf condition  if (onsame && inters)
#endif

  stripe->SetIndexPoint(ChFi3d_IndexPointInDS(CV1, DStr), isfirst, 1);
  stripe->SetIndexPoint(ChFi3d_IndexPointInDS(CV2, DStr), isfirst, 2);

  if (!intcouture)
  {
    // there is no intersection with the sewing edge
    // the curve Cc is stored in the stripe
    // the storage in the DS is not done by FILDS.

    TopOpeBRepDS_Curve Tc(Cc, tolreached);
    ICurve = DStr.AddCurve(Tc);
    occ::handle<TopOpeBRepDS_SurfaceCurveInterference> Interfc =
      ChFi3d_FilCurveInDS(ICurve, IShape, Pc, Et);

    // 31/01/02 akm vvv : (OCC119) Prevent the builder from creating
    //                    intersecting fillets - they are bad.
    Geom2dInt_GInter    anIntersector;
    Geom2dAdaptor_Curve aCorkPCurve(Pc, Udeb, Ufin);

    // Take all the interferences with faces from all the stripes
    // and look if their pcurves intersect our cork pcurve.
    // Unfortunately, by this moment they do not exist in DStr.
    NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator aStrIt(myListStripe);
    for (; aStrIt.More(); aStrIt.Next())
    {
      occ::handle<ChFiDS_Stripe> aCheckStripe = aStrIt.Value();
      occ::handle<NCollection_HSequence<occ::handle<ChFiDS_SurfData>>> aSeqData =
        aCheckStripe->SetOfSurfData();
      // Loop on parts of the stripe
      int iPart;
      for (iPart = 1; iPart <= aSeqData->Length(); iPart++)
      {
        occ::handle<ChFiDS_SurfData> aData = aSeqData->Value(iPart);
        Geom2dAdaptor_Curve          anOtherPCurve;
        if (IShape == aData->IndexOfS1())
        {
          const occ::handle<Geom2d_Curve>& aPCurve = aData->InterferenceOnS1().PCurveOnFace();
          if (aPCurve.IsNull())
            continue;

          anOtherPCurve.Load(aPCurve,
                             aData->InterferenceOnS1().FirstParameter(),
                             aData->InterferenceOnS1().LastParameter());
        }
        else if (IShape == aData->IndexOfS2())
        {
          const occ::handle<Geom2d_Curve>& aPCurve = aData->InterferenceOnS2().PCurveOnFace();
          if (aPCurve.IsNull())
            continue;

          anOtherPCurve.Load(aPCurve,
                             aData->InterferenceOnS2().FirstParameter(),
                             aData->InterferenceOnS2().LastParameter());
        }
        else
        {
          // Normal case - no common surface
          continue;
        }
        if (IsEqual(anOtherPCurve.LastParameter(), anOtherPCurve.FirstParameter()))
          // Degenerates
          continue;
        anIntersector.Perform(aCorkPCurve, anOtherPCurve, tol2d, Precision::PConfusion());
        if (anIntersector.NbSegments() > 0 || anIntersector.NbPoints() > 0)
          throw StdFail_NotDone("OneCorner : fillets have too big radiuses");
      }
    }
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator anIter(
      DStr.ChangeShapeInterferences(IShape));
    for (; anIter.More(); anIter.Next())
    {
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> anOtherIntrf =
        occ::down_cast<TopOpeBRepDS_SurfaceCurveInterference>(anIter.Value());
      // We need only interferences between cork face and curves
      // of intersection with another fillet surfaces
      if (anOtherIntrf.IsNull())
        continue;
      // Look if there is an intersection between pcurves
      occ::handle<Geom_TrimmedCurve> anOtherCur =
        occ::down_cast<Geom_TrimmedCurve>(DStr.Curve(anOtherIntrf->Geometry()).Curve());
      if (anOtherCur.IsNull())
        continue;
      Geom2dAdaptor_Curve anOtherPCurve(anOtherIntrf->PCurve(),
                                        anOtherCur->FirstParameter(),
                                        anOtherCur->LastParameter());
      anIntersector.Perform(aCorkPCurve, anOtherPCurve, tol2d, Precision::PConfusion());
      if (anIntersector.NbSegments() > 0 || anIntersector.NbPoints() > 0)
        throw StdFail_NotDone("OneCorner : fillets have too big radiuses");
    }
    // 31/01/02 akm ^^^
    DStr.ChangeShapeInterferences(IShape).Append(Interfc);
    //// modified by jgv, 26.03.02 for OCC32 ////
    ChFiDS_CommonPoint CV[2];
    CV[0] = CV1;
    CV[1] = CV2;
    for (int i = 0; i < 2; i++)
    {
      if (CV[i].IsOnArc() && ChFi3d_IsPseudoSeam(CV[i].Arc(), Fv))
      {
        gp_Pnt2d                  pfac1, PcF, PcL;
        gp_Vec2d                  DerPc, DerHc;
        double                    first, last, prm1, prm2;
        bool                      onfirst, FirstToPar;
        occ::handle<Geom2d_Curve> Hc = BRep_Tool::CurveOnSurface(CV[i].Arc(), Fv, first, last);
        if (Hc.IsNull())
          throw Standard_ConstructionError("Failed to get p-curve of edge");
        pfac1   = Hc->Value(CV[i].ParameterOnArc());
        PcF     = Pc->Value(Udeb);
        PcL     = Pc->Value(Ufin);
        onfirst = (pfac1.Distance(PcF) < pfac1.Distance(PcL)) ? true : false;
        if (onfirst)
          Pc->D1(Udeb, PcF, DerPc);
        else
        {
          Pc->D1(Ufin, PcL, DerPc);
          DerPc.Reverse();
        }
        Hc->D1(CV[i].ParameterOnArc(), pfac1, DerHc);
        if (DerHc.Dot(DerPc) > 0.)
        {
          prm1       = CV[i].ParameterOnArc();
          prm2       = last;
          FirstToPar = false;
        }
        else
        {
          prm1       = first;
          prm2       = CV[i].ParameterOnArc();
          FirstToPar = true;
        }
        occ::handle<Geom_Curve> Ct = BRep_Tool::Curve(CV[i].Arc(), first, last);
        Ct                         = new Geom_TrimmedCurve(Ct, prm1, prm2);
        double                                           toled = BRep_Tool::Tolerance(CV[i].Arc());
        TopOpeBRepDS_Curve                               tcurv(Ct, toled);
        occ::handle<TopOpeBRepDS_CurvePointInterference> Interfp1, Interfp2;
        int                                              indcurv;
        indcurv       = DStr.AddCurve(tcurv);
        int indpoint  = (isfirst) ? stripe->IndexFirstPointOnS1() : stripe->IndexLastPointOnS1();
        int indvertex = DStr.AddShape(Vtx);
        if (FirstToPar)
        {
          Interfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indcurv, indvertex, prm1, true);
          Interfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indcurv, indpoint, prm2, false);
        }
        else
        {
          Interfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indcurv, indpoint, prm1, false);
          Interfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indcurv, indvertex, prm2, true);
        }
        DStr.ChangeCurveInterferences(indcurv).Append(Interfp1);
        DStr.ChangeCurveInterferences(indcurv).Append(Interfp2);
        int indface = DStr.AddShape(Fv);
        Interfc     = ChFi3d_FilCurveInDS(indcurv, indface, Hc, CV[i].Arc().Orientation());
        DStr.ChangeShapeInterferences(indface).Append(Interfc);
        TopoDS_Edge aLocalEdge = CV[i].Arc();
        aLocalEdge.Reverse();
        occ::handle<Geom2d_Curve> HcR = BRep_Tool::CurveOnSurface(aLocalEdge, Fv, first, last);
        if (HcR.IsNull())
          throw Standard_ConstructionError("Failed to get p-curve of edge");
        Interfc = ChFi3d_FilCurveInDS(indcurv, indface, HcR, aLocalEdge.Orientation());
        DStr.ChangeShapeInterferences(indface).Append(Interfc);
        // modify degenerated edge
        bool            DegenExist = false;
        TopoDS_Edge     Edeg;
        TopExp_Explorer Explo(Fv, TopAbs_EDGE);
        for (; Explo.More(); Explo.Next())
        {
          const TopoDS_Edge& Ecur = TopoDS::Edge(Explo.Current());
          if (BRep_Tool::Degenerated(Ecur))
          {
            TopoDS_Vertex Vf, Vl;
            TopExp::Vertices(Ecur, Vf, Vl);
            if (Vf.IsSame(Vtx) || Vl.IsSame(Vtx))
            {
              DegenExist = true;
              Edeg       = Ecur;
              break;
            }
          }
        }
        if (DegenExist)
        {
          double                    fd, ld;
          occ::handle<Geom2d_Curve> Cd = BRep_Tool::CurveOnSurface(Edeg, Fv, fd, ld);
          if (Cd.IsNull())
            throw Standard_ConstructionError("Failed to get p-curve of edge");
          occ::handle<Geom2d_TrimmedCurve> tCd = occ::down_cast<Geom2d_TrimmedCurve>(Cd);
          if (!tCd.IsNull())
            Cd = tCd->BasisCurve();
          gp_Pnt2d                      P2d = (FirstToPar) ? Hc->Value(first) : Hc->Value(last);
          Geom2dAPI_ProjectPointOnCurve Projector(P2d, Cd);
          double                        par  = Projector.LowerDistanceParameter();
          int                           Ideg = DStr.AddShape(Edeg);
          // clang-format off
          TopAbs_Orientation ori = (par < fd)? TopAbs_FORWARD : TopAbs_REVERSED; //if par<fd => par>ld
          // clang-format on
          Interfp1 = ChFi3d_FilPointInDS(ori, Ideg, indvertex, par, true);
          DStr.ChangeShapeInterferences(Ideg).Append(Interfp1);
        }
      }
    }
    /////////////////////////////////////////////
    stripe->ChangePCurve(isfirst) = Ps;
    stripe->SetCurve(ICurve, isfirst);
    stripe->SetParameters(isfirst, Udeb, Ufin);
  }
  else
  {
    // curves curv1 are curv2 stored in the DS
    // these curves will not be reconstructed by FILDS as
    // one places stripe->InDS(isfirst);

    // interferences of curv1 and curv2 on Fv
    ComputeCurve2d(curv1, Fv, c2d1);
    occ::handle<TopOpeBRepDS_SurfaceCurveInterference> InterFv;
    InterFv = ChFi3d_FilCurveInDS(Icurv1, IShape, c2d1, Et);
    DStr.ChangeShapeInterferences(IShape).Append(InterFv);
    ComputeCurve2d(curv2, Fv, c2d2);
    InterFv = ChFi3d_FilCurveInDS(Icurv2, IShape, c2d2, Et);
    DStr.ChangeShapeInterferences(IShape).Append(InterFv);
    // interferences of curv1 and curv2 on Isurf
    if (Fd->Orientation() == Fv.Orientation())
      Et = TopAbs::Reverse(Et);
    c2d1    = new Geom2d_TrimmedCurve(Ps, Udeb, par2);
    InterFv = ChFi3d_FilCurveInDS(Icurv1, Isurf, c2d1, Et);
    DStr.ChangeSurfaceInterferences(Isurf).Append(InterFv);
    c2d2    = new Geom2d_TrimmedCurve(Ps, par2, Ufin);
    InterFv = ChFi3d_FilCurveInDS(Icurv2, Isurf, c2d2, Et);
    DStr.ChangeSurfaceInterferences(Isurf).Append(InterFv);

    // limitation of the sewing edge
    int                                              Iarc = DStr.AddShape(edgecouture);
    occ::handle<TopOpeBRepDS_CurvePointInterference> Interfedge;
    TopAbs_Orientation                               ori;
    TopoDS_Vertex                                    Vdeb, Vfin;
    Vdeb = TopExp::FirstVertex(edgecouture);
    Vfin = TopExp::LastVertex(edgecouture);
    double pard, parf;
    pard = BRep_Tool::Parameter(Vdeb, edgecouture);
    parf = BRep_Tool::Parameter(Vfin, edgecouture);
    if (std::abs(par1 - pard) < std::abs(parf - par1))
      ori = TopAbs_FORWARD;
    else
      ori = TopAbs_REVERSED;
    Interfedge = ChFi3d_FilPointInDS(ori, Iarc, indpt, par1);
    DStr.ChangeShapeInterferences(Iarc).Append(Interfedge);

    // creation of CurveInterferences from Icurv1 and Icurv2
    stripe->InDS(isfirst);
    int                                              ind1 = stripe->IndexPoint(isfirst, 1);
    int                                              ind2 = stripe->IndexPoint(isfirst, 2);
    occ::handle<TopOpeBRepDS_CurvePointInterference> interfprol =
      ChFi3d_FilPointInDS(TopAbs_FORWARD, Icurv1, ind1, Udeb);
    DStr.ChangeCurveInterferences(Icurv1).Append(interfprol);
    interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv1, indpt, par2);
    DStr.ChangeCurveInterferences(Icurv1).Append(interfprol);
    interfprol = ChFi3d_FilPointInDS(TopAbs_FORWARD, Icurv2, indpt, par2);
    DStr.ChangeCurveInterferences(Icurv2).Append(interfprol);
    interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv2, ind2, Ufin);
    DStr.ChangeCurveInterferences(Icurv2).Append(interfprol);
  }

  ChFi3d_EnlargeBox(HBs, Pc, Udeb, Ufin, box1, box2);

  if (onsame && inters)
  {
// VARIANT 1:
// A small missing end of curve is added for the extension
// of the face at end and the limitation of the opposing face.

//   VARIANT 2 : extend Arcprol, not create new small edge
//   To do: modify for intcouture
#define VARIANT1

    // First of all the points are cut with the edge of the spine.
    int                IArcspine = DStr.AddShape(Arcspine);
    int                IVtx      = DStr.AddShape(Vtx);
    TopAbs_Orientation OVtx      = TopAbs_FORWARD;
    for (ex.Init(Arcspine.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (Vtx.IsSame(ex.Current()))
      {
        OVtx = ex.Current().Orientation();
        break;
      }
    }
    OVtx                                                    = TopAbs::Reverse(OVtx);
    double                                           parVtx = BRep_Tool::Parameter(Vtx, Arcspine);
    occ::handle<TopOpeBRepDS_CurvePointInterference> interfv =
      ChFi3d_FilVertexInDS(OVtx, IArcspine, IVtx, parVtx);
    DStr.ChangeShapeInterferences(IArcspine).Append(interfv);

    // Now the missing curves are constructed.
    TopoDS_Vertex V2;
    for (ex.Init(Arcprol.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (Vtx.IsSame(ex.Current()))
        OVtx = ex.Current().Orientation();
      else
        V2 = TopoDS::Vertex(ex.Current());
    }

    occ::handle<Geom2d_Curve> Hc;
#ifdef VARIANT1
    parVtx = BRep_Tool::Parameter(Vtx, Arcprol);
#else
    parVtx = BRep_Tool::Parameter(V2, Arcprol);
#endif
    const ChFiDS_FaceInterference& Fiop = Fd->Interference(IFopArc);
    gp_Pnt2d                       pop1, pop2, pv1, pv2;
    Hc = BRep_Tool::CurveOnSurface(Arcprol, Fop, Ubid, Ubid);
    if (Hc.IsNull())
      throw Standard_ConstructionError("Failed to get p-curve of edge");
    pop1 = Hc->Value(parVtx);
    pop2 = Fiop.PCurveOnFace()->Value(Fiop.Parameter(isfirst));
    Hc   = BRep_Tool::CurveOnSurface(Arcprol, Fv, Ubid, Ubid);
    if (Hc.IsNull())
      throw Standard_ConstructionError("Failed to get p-curve of edge");
    pv1 = Hc->Value(parVtx);
    pv2 = p2dbout;
    ChFi3d_Recale(Bs, pv1, pv2, true);
    NCollection_Array1<double> Pardeb(1, 4), Parfin(1, 4);
    Pardeb(1) = pop1.X();
    Pardeb(2) = pop1.Y();
    Pardeb(3) = pv1.X();
    Pardeb(4) = pv1.Y();
    Parfin(1) = pop2.X();
    Parfin(2) = pop2.Y();
    Parfin(3) = pv2.X();
    Parfin(4) = pv2.Y();
    double uu1, uu2, vv1, vv2;
    ChFi3d_Boite(pv1, pv2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bs, uu1, uu2, vv1, vv2);
    ChFi3d_Boite(pop1, pop2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bop, uu1, uu2, vv1, vv2);

    occ::handle<Geom_Curve>   zob3d;
    occ::handle<Geom2d_Curve> zob2dop, zob2dv;
    // double tolreached;
    if (!ChFi3d_ComputeCurves(HBop,
                              HBs,
                              Pardeb,
                              Parfin,
                              zob3d,
                              zob2dop,
                              zob2dv,
                              tolapp3d,
                              tol2d,
                              tolreached))
      throw Standard_Failure("OneCorner : echec calcul intersection");

    Udeb = zob3d->FirstParameter();
    Ufin = zob3d->LastParameter();
    TopOpeBRepDS_Curve Zob(zob3d, tolreached);
    int                IZob = DStr.AddCurve(Zob);

    // it is determined if Fop has an edge of sewing
    // it is determined if the curve has an intersection with the edge of sewing

    // TopoDS_Edge edgecouture;
    // bool couture;
    ChFi3d_Couture(Fop, couture, edgecouture);

    if (couture && !BRep_Tool::Degenerated(edgecouture))
    {
      BRepLib_MakeEdge  Bedge(zob3d);
      TopoDS_Edge       edg = Bedge.Edge();
      BRepExtrema_ExtCC extCC(edgecouture, edg);
      if (extCC.IsDone() && extCC.NbExt() != 0)
      {
        for (int i = 1; i <= extCC.NbExt() && !intcouture; i++)
        {
          if (extCC.SquareDistance(i) <= 1.e-8)
          {
            par1                  = extCC.ParameterOnE1(i);
            par2                  = extCC.ParameterOnE2(i);
            gp_Pnt             P1 = extCC.PointOnE1(i);
            TopOpeBRepDS_Point tpoint(P1, 1.e-4);
            indpt      = DStr.AddPoint(tpoint);
            intcouture = true;
            curv1      = new Geom_TrimmedCurve(zob3d, Udeb, par2);
            curv2      = new Geom_TrimmedCurve(zob3d, par2, Ufin);
            TopOpeBRepDS_Curve tcurv1(curv1, tolreached);
            TopOpeBRepDS_Curve tcurv2(curv2, tolreached);
            Icurv1 = DStr.AddCurve(tcurv1);
            Icurv2 = DStr.AddCurve(tcurv2);
          }
        }
      }
    }
    if (intcouture)
    {

      // interference of curv1 and curv2 on Ishape
      Et = TopAbs::Reverse(TopAbs::Compose(OVtx, OArcprolv));
      ComputeCurve2d(curv1, Fop, c2d1);
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> InterFv =
        ChFi3d_FilCurveInDS(Icurv1, IShape, /*zob2dv*/ c2d1, Et);
      DStr.ChangeShapeInterferences(IShape).Append(InterFv);
      ComputeCurve2d(curv2, Fop, c2d2);
      InterFv = ChFi3d_FilCurveInDS(Icurv2, IShape, /*zob2dv*/ c2d2, Et);
      DStr.ChangeShapeInterferences(IShape).Append(InterFv);

      // limitation of the sewing edge
      int                                              Iarc = DStr.AddShape(edgecouture);
      occ::handle<TopOpeBRepDS_CurvePointInterference> Interfedge;
      TopAbs_Orientation                               ori;
      TopoDS_Vertex                                    Vdeb, Vfin;
      Vdeb = TopExp::FirstVertex(edgecouture);
      Vfin = TopExp::LastVertex(edgecouture);
      double pard, parf;
      pard = BRep_Tool::Parameter(Vdeb, edgecouture);
      parf = BRep_Tool::Parameter(Vfin, edgecouture);
      if (std::abs(par1 - pard) < std::abs(parf - par1))
        ori = TopAbs_REVERSED;
      else
        ori = TopAbs_FORWARD;
      Interfedge = ChFi3d_FilPointInDS(ori, Iarc, indpt, par1);
      DStr.ChangeShapeInterferences(Iarc).Append(Interfedge);

      //  interference of curv1 and curv2 on Iop
      int Iop = DStr.AddShape(Fop);
      Et      = TopAbs::Reverse(TopAbs::Compose(OVtx, OArcprolop));
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> Interfop;
      ComputeCurve2d(curv1, Fop, c2d1);
      Interfop = ChFi3d_FilCurveInDS(Icurv1, Iop, c2d1, Et);
      DStr.ChangeShapeInterferences(Iop).Append(Interfop);
      ComputeCurve2d(curv2, Fop, c2d2);
      Interfop = ChFi3d_FilCurveInDS(Icurv2, Iop, c2d2, Et);
      DStr.ChangeShapeInterferences(Iop).Append(Interfop);
      occ::handle<TopOpeBRepDS_CurvePointInterference> interfprol =
        ChFi3d_FilVertexInDS(TopAbs_FORWARD, Icurv1, IVtx, Udeb);
      DStr.ChangeCurveInterferences(Icurv1).Append(interfprol);
      interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv1, indpt, par2);
      DStr.ChangeCurveInterferences(Icurv1).Append(interfprol);
      int icc    = stripe->IndexPoint(isfirst, IFopArc);
      interfprol = ChFi3d_FilPointInDS(TopAbs_FORWARD, Icurv2, indpt, par2);
      DStr.ChangeCurveInterferences(Icurv2).Append(interfprol);
      interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv2, icc, Ufin);
      DStr.ChangeCurveInterferences(Icurv2).Append(interfprol);
    }
    else
    {
      Et = TopAbs::Reverse(TopAbs::Compose(OVtx, OArcprolv));
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> InterFv =
        ChFi3d_FilCurveInDS(IZob, IShape, zob2dv, Et);
      DStr.ChangeShapeInterferences(IShape).Append(InterFv);
      Et = TopAbs::Reverse(TopAbs::Compose(OVtx, OArcprolop));
      int                                                Iop = DStr.AddShape(Fop);
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> Interfop =
        ChFi3d_FilCurveInDS(IZob, Iop, zob2dop, Et);
      DStr.ChangeShapeInterferences(Iop).Append(Interfop);
      occ::handle<TopOpeBRepDS_CurvePointInterference> interfprol;
#ifdef VARIANT1
      interfprol = ChFi3d_FilVertexInDS(TopAbs_FORWARD, IZob, IVtx, Udeb);
#else
      {
        int IV2    = DStr.AddShape(V2); // VARIANT 2
        interfprol = ChFi3d_FilVertexInDS(TopAbs_FORWARD, IZob, IV2, Udeb);
      }
#endif
      DStr.ChangeCurveInterferences(IZob).Append(interfprol);
      int icc    = stripe->IndexPoint(isfirst, IFopArc);
      interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, IZob, icc, Ufin);
      DStr.ChangeCurveInterferences(IZob).Append(interfprol);
#ifdef VARIANT1
      {
        if (IFopArc == 1)
          box1.Add(zob3d->Value(Ufin));
        else
          box2.Add(zob3d->Value(Ufin));
      }
#else
      {
        // cut off existing Arcprol
        int iArcprol = DStr.AddShape(Arcprol);
        interfprol   = ChFi3d_FilPointInDS(OVtx, iArcprol, icc, Udeb);
        DStr.ChangeShapeInterferences(Arcprol).Append(interfprol);
      }
#endif
    }
  }
  ChFi3d_EnlargeBox(DStr, stripe, Fd, box1, box2, isfirst);
  if (CV1.IsOnArc())
  {
    ChFi3d_EnlargeBox(CV1.Arc(), myEFMap(CV1.Arc()), CV1.ParameterOnArc(), box1);
  }
  if (CV2.IsOnArc())
  {
    ChFi3d_EnlargeBox(CV2.Arc(), myEFMap(CV2.Arc()), CV2.ParameterOnArc(), box2);
  }
  if (!CV1.IsVertex())
    ChFi3d_SetPointTolerance(DStr, box1, stripe->IndexPoint(isfirst, 1));
  if (!CV2.IsVertex())
    ChFi3d_SetPointTolerance(DStr, box2, stripe->IndexPoint(isfirst, 2));

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_sameinter); // result perf condition if (same &&inter)
#endif
}

//=======================================================================
// function : cherche_face
// purpose  : find face F belonging to the map, different from faces
//           F1  F2 F3 and containing edge E
//=======================================================================

static void cherche_face(const NCollection_List<TopoDS_Shape>& map,
                         const TopoDS_Edge&                    E,
                         const TopoDS_Face&                    F1,
                         const TopoDS_Face&                    F2,
                         const TopoDS_Face&                    F3,
                         TopoDS_Face&                          F)
{
  TopoDS_Face                              Fcur;
  bool                                     trouve = false;
  NCollection_List<TopoDS_Shape>::Iterator It;
  int                                      ie;
  for (It.Initialize(map); It.More() && !trouve; It.Next())
  {
    Fcur = TopoDS::Face(It.Value());
    if (!Fcur.IsSame(F1) && !Fcur.IsSame(F2) && !Fcur.IsSame(F3))
    {
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapE;
      TopExp::MapShapes(Fcur, TopAbs_EDGE, MapE);
      for (ie = 1; ie <= MapE.Extent() && !trouve; ie++)
      {
        TopoDS_Shape aLocalShape = TopoDS_Shape(MapE(ie));
        if (E.IsSame(TopoDS::Edge(aLocalShape)))
        //            if (E.IsSame(TopoDS::Edge(TopoDS_Shape (MapE(ie)))))
        {
          F      = Fcur;
          trouve = true;
        }
      }
    }
  }
  if (F.IsNull())
  {
    throw Standard_ConstructionError("Failed to find face.");
  }
}

//=======================================================================
// function : cherche_edge1
// purpose  : find common edge between faces F1 and F2
//=======================================================================

static void cherche_edge1(const TopoDS_Face& F1, const TopoDS_Face& F2, TopoDS_Edge& Edge)
{
  int                                                           i, j;
  TopoDS_Edge                                                   Ecur1, Ecur2;
  bool                                                          trouve = false;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapE1, MapE2;
  TopExp::MapShapes(F1, TopAbs_EDGE, MapE1);
  TopExp::MapShapes(F2, TopAbs_EDGE, MapE2);
  for (i = 1; i <= MapE1.Extent() && !trouve; i++)
  {
    TopoDS_Shape aLocalShape = TopoDS_Shape(MapE1(i));
    Ecur1                    = TopoDS::Edge(aLocalShape);
    //	Ecur1=TopoDS::Edge(TopoDS_Shape (MapE1(i)));
    for (j = 1; j <= MapE2.Extent() && !trouve; j++)
    {
      aLocalShape = TopoDS_Shape(MapE2(j));
      Ecur2       = TopoDS::Edge(aLocalShape);
      //	      Ecur2=TopoDS::Edge(TopoDS_Shape (MapE2(j)));
      if (Ecur2.IsSame(Ecur1))
      {
        Edge   = Ecur1;
        trouve = true;
      }
    }
  }
  if (Edge.IsNull())
  {
    throw Standard_ConstructionError("Failed to find edge.");
  }
}

//=======================================================================
// function : containV
// purpose  : return true if vertex V belongs to F1
//=======================================================================

static bool containV(const TopoDS_Face& F1, const TopoDS_Vertex& V)
{
  int                                                           i;
  TopoDS_Vertex                                                 Vcur;
  bool                                                          trouve  = false;
  bool                                                          contain = false;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapV;
  TopExp::MapShapes(F1, TopAbs_VERTEX, MapV);
  for (i = 1; i <= MapV.Extent() && !trouve; i++)
  {
    TopoDS_Shape aLocalShape = TopoDS_Shape(MapV(i));
    Vcur                     = TopoDS::Vertex(aLocalShape);
    //	Vcur=TopoDS::Vertex(TopoDS_Shape (MapV(i)));
    if (Vcur.IsSame(V))
    {
      contain = true;
      trouve  = true;
    }
  }
  return contain;
}

//=======================================================================
// function : containE
// purpose  : return true if edge E belongs to F1
//=======================================================================

static bool containE(const TopoDS_Face& F1, const TopoDS_Edge& E)
{
  int                                                           i;
  TopoDS_Edge                                                   Ecur;
  bool                                                          trouve  = false;
  bool                                                          contain = false;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapE;
  TopExp::MapShapes(F1, TopAbs_EDGE, MapE);
  for (i = 1; i <= MapE.Extent() && !trouve; i++)
  {
    TopoDS_Shape aLocalShape = TopoDS_Shape(MapE(i));
    Ecur                     = TopoDS::Edge(aLocalShape);
    //	Ecur=TopoDS::Edge(TopoDS_Shape (MapE(i)));
    if (Ecur.IsSame(E))
    {
      contain = true;
      trouve  = true;
    }
  }
  return contain;
}

//=======================================================================
// function : IsShrink
// purpose  : check if U (if <isU>==True) or V of points of <PC> is within
//           <tol> from <Param>, check points between <Pf> and <Pl>
//=======================================================================

static bool IsShrink(const Geom2dAdaptor_Curve& PC,
                     const double               Pf,
                     const double               Pl,
                     const double               Param,
                     const bool                 isU,
                     const double               tol)
{
  switch (PC.GetType())
  {
    case GeomAbs_Line: {
      gp_Pnt2d P1 = PC.Value(Pf);
      gp_Pnt2d P2 = PC.Value(Pl);
      if (std::abs(P1.Coord(isU ? 1 : 2) - Param) <= tol
          && std::abs(P2.Coord(isU ? 1 : 2) - Param) <= tol)
        return true;
      else
        return false;
    }
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve: {
      math_FunctionSample aSample(Pf, Pl, 10);
      int                 i;
      for (i = 1; i <= aSample.NbPoints(); i++)
      {
        gp_Pnt2d P = PC.Value(aSample.GetParameter(i));
        if (std::abs(P.Coord(isU ? 1 : 2) - Param) > tol)
          return false;
      }
      return true;
    }
    default:;
  }
  return false;
}

//=================================================================================================

void ChFi3d_Builder::PerformIntersectionAtEnd(const int Index)
{

  // intersection at end of fillet with at least two faces
  // process the following cases:
  // - top has n (n>3) adjacent edges
  // - top has 3 edges and fillet on one of edges touches
  //   more than one face

#ifdef OCCT_DEBUG
  OSD_Chronometer ch; // init perf
#endif

  TopOpeBRepDS_DataStructure&                            DStr = myDS->ChangeDS();
  const int                                              nn   = 15;
  NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator It;
  It.Initialize(myVDataMap(Index));
  occ::handle<ChFiDS_Stripe>                          stripe = It.Value();
  const occ::handle<ChFiDS_Spine>                     spine  = stripe->Spine();
  NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqFil =
    stripe->ChangeSetOfSurfData()->ChangeSequence();
  const TopoDS_Vertex& Vtx     = myVDataMap.FindKey(Index);
  int                  sens    = 0, num, num1;
  bool                 couture = false, isfirst;
  // int sense;
  TopoDS_Edge edgelibre1, edgelibre2, EdgeSpine;
  bool        bordlibre;
  // determine the number of faces and edges
  NCollection_Array1<TopoDS_Shape>         tabedg(0, nn);
  TopoDS_Face                              F1, F2;
  int                                      nface = ChFi3d_nbface(myVFMap(Vtx));
  NCollection_List<TopoDS_Shape>::Iterator ItF;
  int                                      nbarete;
  nbarete = ChFi3d_NbNotDegeneratedEdges(Vtx, myVEMap);
  ChFi3d_ChercheBordsLibres(myVEMap, Vtx, bordlibre, edgelibre1, edgelibre2);
  if (bordlibre)
    nbarete = (nbarete - 2) / 2 + 2;
  else
    nbarete = nbarete / 2;
  // it is determined if there is an edge of sewing and it face

  TopoDS_Face facecouture;
  TopoDS_Edge edgecouture;

  bool trouve = false;
  for (ItF.Initialize(myVFMap(Vtx)); ItF.More() && !couture; ItF.Next())
  {
    TopoDS_Face fcur = TopoDS::Face(ItF.Value());
    ChFi3d_CoutureOnVertex(fcur, Vtx, couture, edgecouture);
    if (couture)
      facecouture = fcur;
  }
  // it is determined if one of edges adjacent to the fillet is regular
  bool          reg1, reg2;
  TopoDS_Edge   Ecur, Eadj1, Eadj2;
  TopoDS_Face   Fga, Fdr;
  TopoDS_Vertex Vbid1;
  int           nbsurf, nbedge;
  reg1    = false;
  reg2    = false;
  nbsurf  = SeqFil.Length();
  nbedge  = spine->NbEdges();
  num     = ChFi3d_IndexOfSurfData(Vtx, stripe, sens);
  isfirst = (sens == 1);
  ChFiDS_State state;
  if (isfirst)
  {
    EdgeSpine = spine->Edges(1);
    num1      = num + 1;
    state     = spine->FirstStatus();
  }
  else
  {
    EdgeSpine = spine->Edges(nbedge);
    num1      = num - 1;
    state     = spine->LastStatus();
  }
  if (nbsurf != nbedge && nbsurf != 1)
  {
    ChFi3d_edge_common_faces(myEFMap(EdgeSpine), F1, F2);
    if (F1.IsSame(facecouture))
      Eadj1 = edgecouture;
    else
      ChFi3d_cherche_element(Vtx, EdgeSpine, F1, Eadj1, Vbid1);
    ChFi3d_edge_common_faces(myEFMap(Eadj1), Fga, Fdr);
    //  Modified by Sergey KHROMOV - Fri Dec 21 17:57:32 2001 Begin
    //  reg1=BRep_Tool::Continuity(Eadj1,Fga,Fdr)!=GeomAbs_C0;
    reg1 = ChFi3d::IsTangentFaces(Eadj1, Fga, Fdr);
    //  Modified by Sergey KHROMOV - Fri Dec 21 17:57:33 2001 End
    if (F2.IsSame(facecouture))
      Eadj2 = edgecouture;
    else
      ChFi3d_cherche_element(Vtx, EdgeSpine, F2, Eadj2, Vbid1);
    ChFi3d_edge_common_faces(myEFMap(Eadj2), Fga, Fdr);
    //  Modified by Sergey KHROMOV - Fri Dec 21 17:58:22 2001 Begin
    //  reg2=BRep_Tool::Continuity(Eadj2,Fga,Fdr)!=GeomAbs_C0;
    reg2 = ChFi3d::IsTangentFaces(Eadj2, Fga, Fdr);
    //  Modified by Sergey KHROMOV - Fri Dec 21 17:58:24 2001 End

    // two faces common to the edge are found
    if (reg1 || reg2)
    {
      bool               compoint1 = false;
      bool               compoint2 = false;
      ChFiDS_CommonPoint cp1, cp2;
      cp1 = SeqFil(num1)->ChangeVertex(isfirst, 1);
      cp2 = SeqFil(num1)->ChangeVertex(isfirst, 2);
      if (cp1.IsOnArc())
      {
        if (cp1.Arc().IsSame(Eadj1) || cp1.Arc().IsSame(Eadj2))
          compoint1 = true;
      }
      if (cp2.IsOnArc())
      {
        if (cp2.Arc().IsSame(Eadj1) || cp2.Arc().IsSame(Eadj2))
          compoint2 = true;
      }
      if (compoint1 && compoint2)
      {
        SeqFil.Remove(num);
        num = ChFi3d_IndexOfSurfData(Vtx, stripe, sens);
        if (isfirst)
        {
          num1 = num + 1;
        }
        else
        {
          num1 = num - 1;
        }
        reg1 = false;
        reg2 = false;
      }
    }
  }
  // there is only one face at end if FindFace is true and if the face
  // is not the face with sewing edge
  TopoDS_Face                  face;
  occ::handle<ChFiDS_SurfData> Fd        = SeqFil.ChangeValue(num);
  ChFiDS_CommonPoint&          CV1       = Fd->ChangeVertex(isfirst, 1);
  ChFiDS_CommonPoint&          CV2       = Fd->ChangeVertex(isfirst, 2);
  bool                         onecorner = false;
  if (FindFace(Vtx, CV1, CV2, face))
  {
    if (!couture)
      onecorner = true;
    else if (!face.IsSame(facecouture))
      onecorner = true;
  }
  if (onecorner)
  {
    if (ChFi3d_Builder::MoreSurfdata(Index))
    {
      ChFi3d_Builder::PerformMoreSurfdata(Index);
      return;
    }
  }
  if (!onecorner && (reg1 || reg2) && !couture && state != ChFiDS_OnSame)
  {
    PerformMoreThreeCorner(Index, 1);
    return;
  }
  occ::handle<GeomAdaptor_Surface> HGs = ChFi3d_BoundSurf(DStr, Fd, 1, 2);
  ChFiDS_FaceInterference          Fi1 = Fd->InterferenceOnS1();
  ChFiDS_FaceInterference          Fi2 = Fd->InterferenceOnS2();
  GeomAdaptor_Surface&             Gs  = *HGs;
  occ::handle<BRepAdaptor_Surface> HBs = new BRepAdaptor_Surface();
  BRepAdaptor_Surface&             Bs  = *HBs;
  occ::handle<Geom_Curve>          Cc;
  occ::handle<Geom2d_Curve>        Pc, Ps;
  double                           Ubid, Vbid;
  TopAbs_Orientation               orsurfdata;
  orsurfdata                             = Fd->Orientation();
  int                          IsurfPrev = 0, Isurf = Fd->Surf();
  occ::handle<ChFiDS_SurfData> SDprev;
  if (num1 > 0 && num1 <= SeqFil.Length())
  {
    SDprev    = SeqFil(num1);
    IsurfPrev = SDprev->Surf();
  }
  // calculate the orientation of curves at end

  double             tolpt = 1.e-4;
  double             tolreached;
  TopAbs_Orientation orcourbe, orface, orien;

  stripe->SetIndexPoint(ChFi3d_IndexPointInDS(CV1, DStr), isfirst, 1);
  stripe->SetIndexPoint(ChFi3d_IndexPointInDS(CV2, DStr), isfirst, 2);

  //  gp_Pnt p3d;
  //  gp_Pnt2d p2d;
  double             dist;
  int                Ishape1 = Fd->IndexOfS1();
  TopAbs_Orientation trafil1 = TopAbs_FORWARD;
  if (Ishape1 != 0)
  {
    if (Ishape1 > 0)
    {
      trafil1 = DStr.Shape(Ishape1).Orientation();
    }
#ifdef OCCT_DEBUG
    else
    {
      std::cout << "erreur" << std::endl;
    }
#endif
    trafil1 = TopAbs::Compose(trafil1, Fd->Orientation());

    trafil1 = TopAbs::Compose(TopAbs::Reverse(Fi1.Transition()), trafil1);
  }
#ifdef OCCT_DEBUG
  else
    std::cout << "erreur" << std::endl;
#endif
  // eap, Apr 22 2002, occ 293
  //   Fi1.PCurveOnFace()->D0(Fi1.LastParameter(),p2d);
  //   const occ::handle<Geom_Surface> Stemp =
  //     BRep_Tool::Surface(TopoDS::Face(DStr.Shape(Ishape1)));
  //   Stemp ->D0(p2d.X(),p2d.Y(),p3d);
  //   dist=p3d.Distance(CV1.Point());
  //   if (dist<tolpt) orcourbe=trafil1;
  //   else            orcourbe=TopAbs::Reverse(trafil1);
  if (!isfirst)
    orcourbe = trafil1;
  else
    orcourbe = TopAbs::Reverse(trafil1);

  // eap, Apr 22 2002, occ 293
  // variables to show OnSame situation
  bool isOnSame1, isOnSame2;
  // In OnSame situation, the case of degenerated FaceInterference curve
  // is probable when a corner cuts the ChFi3d earlier built on OnSame edge.
  // In such a case, chamfer face can partially shrink to a line and we need
  // to cut off that shrinked part
  // If <isOnSame1>, FaceInterference with F2 can be degenerated
  bool checkShrink, isShrink, isUShrink;
  isShrink = isUShrink = isOnSame1 = isOnSame2 = false;
  double   checkShrParam = 0., prevSDParam = 0.;
  gp_Pnt2d midP2d;
  int      midIpoint = 0;

  // find Fi1,Fi2 lengths used to extend ChFi surface
  // and by the way define necessity to check shrink
  gp_Pnt2d P2d1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(isfirst));
  gp_Pnt2d P2d2 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(!isfirst));
  gp_Pnt   aP1, aP2;
  HGs->D0(P2d1.X(), P2d1.Y(), aP1);
  HGs->D0(P2d2.X(), P2d2.Y(), aP2);
  double Fi1Length = aP1.Distance(aP2);
  //  double eps = Precision::Confusion();
  checkShrink = (Fi1Length <= Precision::Confusion());

  gp_Pnt2d P2d3 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(isfirst));
  gp_Pnt2d P2d4 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(!isfirst));
  HGs->D0(P2d3.X(), P2d3.Y(), aP1);
  HGs->D0(P2d4.X(), P2d4.Y(), aP2);
  double Fi2Length = aP1.Distance(aP2);
  checkShrink      = checkShrink || (Fi2Length <= Precision::Confusion());

  if (checkShrink)
  {
    if (std::abs(P2d2.Y() - P2d4.Y()) <= Precision::PConfusion())
    {
      isUShrink     = false;
      checkShrParam = P2d2.Y();
    }
    else if (std::abs(P2d2.X() - P2d4.X()) <= Precision::PConfusion())
    {
      isUShrink     = true;
      checkShrParam = P2d2.X();
    }
    else
      checkShrink = false;
  }

  /***********************************************************************/
  //  find faces intersecting with the fillet and edges limiting intersections
  //  nbface is the nb of faces intersected, Face[i] contains the faces
  //  to intersect (i=0.. nbface-1). Edge[i] contains edges limiting
  //  the intersections (i=0 ..nbface)
  /**********************************************************************/

  int                                      nb = 1, nbface;
  TopoDS_Edge                              E1, E2, Edge[nn], E, Ei, edgesau;
  TopoDS_Face                              facesau;
  bool                                     oneintersection1 = false;
  bool                                     oneintersection2 = false;
  TopoDS_Face                              Face[nn], F, F3;
  TopoDS_Vertex                            V1, V2, V, Vfin;
  bool                                     findonf1 = false, findonf2 = false;
  NCollection_List<TopoDS_Shape>::Iterator It3;
  F1 = TopoDS::Face(DStr.Shape(Fd->IndexOfS1()));
  F2 = TopoDS::Face(DStr.Shape(Fd->IndexOfS2()));
  F3 = F1;
  if (couture || bordlibre)
    nface = nface + 1;
  if (nface == 3)
    nbface = 2;
  else
    nbface = nface - 2;
  if (!CV1.IsOnArc() || !CV2.IsOnArc())
  {
    PerformMoreThreeCorner(Index, 1);
    return;
  }

  Edge[0]      = CV1.Arc();
  Edge[nbface] = CV2.Arc();
  tabedg.SetValue(0, Edge[0]);
  tabedg.SetValue(nbface, Edge[nbface]);
  // processing of a fillet arriving on a vertex
  // edge contained in CV.Arc is not inevitably good
  // the edge concerned by the intersection is found

  double dist1, dist2;
  if (CV1.IsVertex())
  {
    trouve               = false;
    /*TopoDS_Vertex */ V = CV1.Vertex();
    for (It3.Initialize(myVEMap(V)); It3.More() && !trouve; It3.Next())
    {
      E = TopoDS::Edge(It3.Value());
      if (!E.IsSame(Edge[0]) && (containE(F1, E)))
        trouve = true;
    }
    TopoDS_Vertex Vt, V3, V4;
    V1 = TopExp::FirstVertex(Edge[0]);
    V2 = TopExp::LastVertex(Edge[0]);
    if (V.IsSame(V1))
      Vt = V2;
    else
      Vt = V1;
    dist1 = (BRep_Tool::Pnt(Vt)).Distance(BRep_Tool::Pnt(Vtx));
    V3    = TopExp::FirstVertex(E);
    V4    = TopExp::LastVertex(E);
    if (V.IsSame(V3))
      Vt = V4;
    else
      Vt = V3;
    dist2 = (BRep_Tool::Pnt(Vt)).Distance(BRep_Tool::Pnt(Vtx));
    if (dist2 < dist1)
    {
      Edge[0] = E;
      TopAbs_Orientation ori;
      if (V2.IsSame(V3) || V1.IsSame(V4))
        ori = CV1.TransitionOnArc();
      else
        ori = TopAbs::Reverse(CV1.TransitionOnArc());
      double par = BRep_Tool::Parameter(V, Edge[0]);
      double tol = CV1.Tolerance();
      CV1.SetArc(tol, Edge[0], par, ori);
    }
  }

  if (CV2.IsVertex())
  {
    trouve              = false;
    /*TopoDS_Vertex*/ V = CV2.Vertex();
    for (It3.Initialize(myVEMap(V)); It3.More() && !trouve; It3.Next())
    {
      E = TopoDS::Edge(It3.Value());
      if (!E.IsSame(Edge[2]) && (containE(F2, E)))
        trouve = true;
    }
    TopoDS_Vertex Vt, V3, V4;
    V1 = TopExp::FirstVertex(Edge[2]);
    V2 = TopExp::LastVertex(Edge[2]);
    if (V.IsSame(V1))
      Vt = V2;
    else
      Vt = V1;
    dist1 = (BRep_Tool::Pnt(Vt)).Distance(BRep_Tool::Pnt(Vtx));
    V3    = TopExp::FirstVertex(E);
    V4    = TopExp::LastVertex(E);
    if (V.IsSame(V3))
      Vt = V4;
    else
      Vt = V3;
    dist2 = (BRep_Tool::Pnt(Vt)).Distance(BRep_Tool::Pnt(Vtx));
    if (dist2 < dist1)
    {
      Edge[2] = E;
      TopAbs_Orientation ori;
      if (V2.IsSame(V3) || V1.IsSame(V4))
        ori = CV2.TransitionOnArc();
      else
        ori = TopAbs::Reverse(CV2.TransitionOnArc());
      double par = BRep_Tool::Parameter(V, Edge[2]);
      double tol = CV2.Tolerance();
      CV2.SetArc(tol, Edge[2], par, ori);
    }
  }
  if (!onecorner)
  {
    // If there is a regular edge, the faces adjacent to it
    // are not in Fd->IndexOfS1 or Fd->IndexOfS2

    //     TopoDS_Face Find1 ,Find2;
    //     if (isfirst)
    //       edge=stripe->Spine()->Edges(1);
    //     else  edge=stripe->Spine()->Edges(stripe->Spine()->NbEdges());
    //     It3.Initialize(myEFMap(edge));
    //     Find1=TopoDS::Face(It3.Value());
    //     trouve=false;
    //     for (It3.Initialize(myEFMap(edge));It3.More()&&!trouve;It3.Next()) {
    //       F=TopoDS::Face (It3.Value());
    //       if (!F.IsSame(Find1)) {
    // 	Find2=F;trouve=true;
    //       }
    //     }

    // if nface =3 there is a top with 3 edges and a fillet
    // and their common points are on different faces
    // otherwise there is a case when a top has more than 3 edges

    if (nface == 3)
    {
      if (CV1.IsVertex())
        findonf1 = true;
      if (CV2.IsVertex())
        findonf2 = true;
      if (!findonf1)
      {
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapV;
        TopExp::MapShapes(Edge[0], TopAbs_VERTEX, MapV);
        if (MapV.Extent() == 2)
          if (!MapV(1).IsSame(Vtx) && !MapV(2).IsSame(Vtx))
            findonf1 = true;
      }
      if (!findonf2)
      {
        NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapV;
        TopExp::MapShapes(Edge[2], TopAbs_VERTEX, MapV);
        if (MapV.Extent() == 2)
          if (!MapV(1).IsSame(Vtx) && !MapV(2).IsSame(Vtx))
            findonf2 = true;
      }

      // detect and process OnSame situatuation
      if (state == ChFiDS_OnSame)
      {
        TopoDS_Edge threeE[3];
        ChFi3d_cherche_element(Vtx, EdgeSpine, F1, threeE[0], V2);
        ChFi3d_cherche_element(Vtx, EdgeSpine, F2, threeE[1], V2);
        threeE[2] = EdgeSpine;
        if (ChFi3d_EdgeState(threeE, myEFMap) == ChFiDS_OnSame)
        {
          isOnSame1 = true;
          nb        = 1;
          Edge[0]   = threeE[0];
          ChFi3d_cherche_face1(myEFMap(Edge[0]), F1, Face[0]);
          if (findonf2)
            findonf1 = true; // not to look for Face[0] again
          else
            Edge[1] = CV2.Arc();
        }
        else
        {
          isOnSame2 = true;
        }
      }

      // findonf1 findonf2 show if F1 and/or F2 are adjacent
      // to many faces at end
      // the faces at end and intersected edges are found

      if (findonf1 && !isOnSame1)
      {
        if (CV1.TransitionOnArc() == TopAbs_FORWARD)
          V1 = TopExp::FirstVertex(CV1.Arc());
        else
          V1 = TopExp::LastVertex(CV1.Arc());
        ChFi3d_cherche_face1(myEFMap(CV1.Arc()), F1, Face[0]);
        nb = 1;
        Ei = Edge[0];
        while (!V1.IsSame(Vtx))
        {
          ChFi3d_cherche_element(V1, Ei, F1, E, V2);
          V1 = V2;
          Ei = E;
          ChFi3d_cherche_face1(myEFMap(E), F1, Face[nb]);
          cherche_edge1(Face[nb - 1], Face[nb], Edge[nb]);
          nb++;
          if (nb >= nn)
            throw Standard_Failure("IntersectionAtEnd : the max number of faces reached");
        }
        if (!findonf2)
          Edge[nb] = CV2.Arc();
      }
      if (findonf2 && !isOnSame2)
      {
        if (!findonf1)
          nb = 1;
        V1 = Vtx;
        if (CV2.TransitionOnArc() == TopAbs_FORWARD)
          Vfin = TopExp::LastVertex(CV2.Arc());
        else
          Vfin = TopExp::FirstVertex(CV2.Arc());
        if (!findonf1)
          ChFi3d_cherche_face1(myEFMap(CV1.Arc()), F1, Face[nb - 1]);
        ChFi3d_cherche_element(V1, EdgeSpine, F2, E, V2);
        Ei = E;
        V1 = V2;
        while (!V1.IsSame(Vfin))
        {
          ChFi3d_cherche_element(V1, Ei, F2, E, V2);
          Ei = E;
          V1 = V2;
          ChFi3d_cherche_face1(myEFMap(E), F2, Face[nb]);
          cherche_edge1(Face[nb - 1], Face[nb], Edge[nb]);
          nb++;
          if (nb >= nn)
            throw Standard_Failure("IntersectionAtEnd : the max number of faces reached");
        }
        Edge[nb] = CV2.Arc();
      }
      if (isOnSame2)
      {
        cherche_edge1(Face[nb - 1], F2, Edge[nb]);
        Face[nb] = F2;
      }

      nbface = nb;
    }

    else
    {

      //  this is the case when a top has more than three edges
      //  the faces and edges concerned are found
      bool /*trouve,*/ possible1, possible2;
      trouve = possible1 = possible2 = false;
      TopExp_Explorer ex;
      nb = 0;
      for (ex.Init(CV1.Arc(), TopAbs_VERTEX); ex.More(); ex.Next())
      {
        if (Vtx.IsSame(ex.Current()))
          possible1 = true;
      }
      for (ex.Init(CV2.Arc(), TopAbs_VERTEX); ex.More(); ex.Next())
      {
        if (Vtx.IsSame(ex.Current()))
          possible2 = true;
      }
      if ((possible1 && possible2) || (!possible1 && !possible2) || (nbarete > 4))
      {
        while (!trouve)
        {
          nb++;
          if (nb >= nn)
            throw Standard_Failure("IntersectionAtEnd : the max number of faces reached");
          if (nb != 1)
            F3 = Face[nb - 2];
          Face[nb - 1] = F3;
          if (CV1.Arc().IsSame(edgelibre1))
            cherche_face(myVFMap(Vtx), edgelibre2, F1, F2, F3, Face[nb - 1]);
          else if (CV1.Arc().IsSame(edgelibre2))
            cherche_face(myVFMap(Vtx), edgelibre1, F1, F2, F3, Face[nb - 1]);
          else
            cherche_face(myVFMap(Vtx), Edge[nb - 1], F1, F2, F3, Face[nb - 1]);
          ChFi3d_cherche_edge(Vtx, tabedg, Face[nb - 1], Edge[nb], V);
          tabedg.SetValue(nb, Edge[nb]);
          if (Edge[nb].IsSame(CV2.Arc()))
            trouve = true;
        }
        nbface = nb;
      }
      else
      {
        IntersectMoreCorner(Index);
        return;
      }
      if (nbarete == 4)
      {
        // if two consecutive edges are G1 there is only one face of intersection
        double        ang1 = 0.0;
        TopoDS_Vertex Vcom;
        trouve = false;
        ChFi3d_cherche_vertex(Edge[0], Edge[1], Vcom, trouve);
        if (Vcom.IsSame(Vtx))
          ang1 = ChFi3d_AngleEdge(Vtx, Edge[0], Edge[1]);
        if (std::abs(ang1 - M_PI) < 0.01)
        {
          oneintersection1 = true;
          facesau          = Face[0];
          edgesau          = Edge[1];
          Face[0]          = Face[1];
          Edge[1]          = Edge[2];
          nbface           = 1;
        }

        if (!oneintersection1)
        {
          trouve = false;
          ChFi3d_cherche_vertex(Edge[1], Edge[2], Vcom, trouve);
          if (Vcom.IsSame(Vtx))
            ang1 = ChFi3d_AngleEdge(Vtx, Edge[1], Edge[2]);
          if (std::abs(ang1 - M_PI) < 0.01)
          {
            oneintersection2 = true;
            facesau          = Face[1];
            edgesau          = Edge[1];
            Edge[1]          = Edge[2];
            nbface           = 1;
          }
        }
      }
      else if (nbarete == 5)
      {
        // pro15368
        //  Modified by Sergey KHROMOV - Fri Dec 21 18:07:43 2001 End
        bool isTangent0 = ChFi3d::IsTangentFaces(Edge[0], F1, Face[0]);
        bool isTangent1 = ChFi3d::IsTangentFaces(Edge[1], Face[0], Face[1]);
        bool isTangent2 = ChFi3d::IsTangentFaces(Edge[2], Face[1], Face[2]);
        if ((isTangent0 || isTangent2) && isTangent1)
        {
          //         GeomAbs_Shape cont0,cont1,cont2;
          //         cont0=BRep_Tool::Continuity(Edge[0],F1,Face[0]);
          //         cont1=BRep_Tool::Continuity(Edge[1],Face[0],Face[1]);
          //         cont2=BRep_Tool::Continuity(Edge[2],Face[1],Face[2]);
          //         if ((cont0!=GeomAbs_C0 || cont2!=GeomAbs_C0) && cont1!=GeomAbs_C0) {
          //  Modified by Sergey KHROMOV - Fri Dec 21 18:07:49 2001 Begin
          facesau          = Face[0];
          edgesau          = Edge[0];
          nbface           = 1;
          Edge[1]          = Edge[3];
          Face[0]          = Face[2];
          oneintersection1 = true;
        }
      }
    }
  }
  else
  {
    nbface  = 1;
    Face[0] = face;
    Edge[1] = Edge[2];
  }

  NCollection_Array1<double> Pardeb(1, 4), Parfin(1, 4);
  gp_Pnt2d                   pfil1, pfac1, pfil2, pfac2, pint, pfildeb;
  occ::handle<Geom2d_Curve>  Hc1, Hc2;
  IntCurveSurface_HInter     inters;
  int                        proledge[nn], prolface[nn + 1]; // last prolface[nn] is for Fd
  int                        shrink[nn];
  TopoDS_Face                faceprol[nn];
  int                        indcurve[nn], indpoint2 = 0, indpoint1 = 0;
  occ::handle<TopOpeBRepDS_CurvePointInterference>   Interfp1, Interfp2, Interfedge[nn];
  occ::handle<TopOpeBRepDS_SurfaceCurveInterference> Interfc, InterfPC[nn], InterfPS[nn];
  double                                             u2, v2, p1, p2, paredge1;
  double                                             paredge2 = 0., tolex = 1.e-4;
  bool                                               extend = false;
  occ::handle<Geom_Surface>                          Sfacemoins1, Sface;
  /***************************************************************************/
  // calculate intersection of the fillet and each face
  // and storage in the DS
  /***************************************************************************/
  for (nb = 1; nb <= nbface; nb++)
  {
    prolface[nb - 1] = 0;
    proledge[nb - 1] = 0;
    shrink[nb - 1]   = 0;
  }
  proledge[nbface] = 0;
  prolface[nn]     = 0;
  if (oneintersection1 || oneintersection2)
    faceprol[1] = facesau;
  if (!isOnSame1 && !isOnSame2)
    checkShrink = false;
  // in OnSame situation we need intersect Fd with Edge[0] or Edge[nbface] as well
  if (isOnSame1)
    nb = 0;
  else
    nb = 1;
  bool intersOnSameFailed = false;

  for (; nb <= nbface; nb++)
  {
    extend = false;
    E2     = Edge[nb];
    if (!nb)
      F = F1;
    else
    {
      F = Face[nb - 1];
      if (!prolface[nb - 1])
        faceprol[nb - 1] = F;
    }

    if (F.IsNull())
      throw Standard_NullObject("IntersectionAtEnd : Trying to intersect with NULL face");

    Sfacemoins1 = BRep_Tool::Surface(F);
    occ::handle<Geom_Curve>   cint;
    occ::handle<Geom2d_Curve> C2dint1, C2dint2, cface, cfacemoins1;

    ///////////////////////////////////////////////////////
    // determine intersections of edges and the fillet
    // to find limitations of intersections face - fillet
    ///////////////////////////////////////////////////////

    if (nb == 1)
    {
      Hc1 = BRep_Tool::CurveOnSurface(Edge[0], Face[0], Ubid, Ubid);
      if (isOnSame1)
      {
        // update interference param on Fi1 and point of CV1
        if (prolface[0])
          Bs.Initialize(faceprol[0], false);
        else
          Bs.Initialize(Face[0], false);
        const occ::handle<Geom_Curve>& c3df = DStr.Curve(Fi1.LineIndex()).Curve();
        double                         Ufi  = Fi2.Parameter(isfirst);
        ChFiDS_FaceInterference&       Fi   = Fd->ChangeInterferenceOnS1();
        if (!IntersUpdateOnSame(HGs,
                                HBs,
                                c3df,
                                F1,
                                Face[0],
                                Edge[0],
                                Vtx,
                                isfirst,
                                10 * tolapp3d, // in
                                Fi,
                                CV1,
                                pfac1,
                                Ufi)) // out
          throw Standard_Failure("IntersectionAtEnd: pb intersection Face - Fi");
        Fi1 = Fi;
        if (intersOnSameFailed)
        { // probable at fillet building
          // look for paredge2
          Geom2dAPI_ProjectPointOnCurve proj;
          if (C2dint2.IsNull())
            proj.Init(pfac1, Hc1);
          else
            proj.Init(pfac1, C2dint2);
          paredge2 = proj.LowerDistanceParameter();
        }
        // update stripe point
        TopOpeBRepDS_Point tpoint(CV1.Point(), tolapp3d);
        indpoint1 = DStr.AddPoint(tpoint);
        stripe->SetIndexPoint(indpoint1, isfirst, 1);
        // reset arc of CV1
        TopoDS_Vertex vert1, vert2;
        TopExp::Vertices(Edge[0], vert1, vert2);
        TopAbs_Orientation arcOri = Vtx.IsSame(vert1) ? TopAbs_FORWARD : TopAbs_REVERSED;
        CV1.SetArc(tolapp3d, Edge[0], paredge2, arcOri);
      }
      else
      {
        if (Hc1.IsNull())
        {
          // curve 2d not found. Sfacemoins1 is extended and projection is done there
          // CV1.Point ()
          ChFi3d_ExtendSurface(Sfacemoins1, prolface[0]);
          if (prolface[0])
          {
            extend = true;
            BRep_Builder BRE;
            double       tol = BRep_Tool::Tolerance(F);
            BRE.MakeFace(faceprol[0], Sfacemoins1, F.Location(), tol);
            if (!isOnSame1)
            {
              GeomAdaptor_Surface Asurf;
              Asurf.Load(Sfacemoins1);
              Extrema_ExtPS ext(CV1.Point(), Asurf, tol, tol);
              double        uc1, vc1;
              if (ext.IsDone())
              {
                ext.Point(1).Parameter(uc1, vc1);
                pfac1.SetX(uc1);
                pfac1.SetY(vc1);
              }
            }
          }
        }
        else
          pfac1 = Hc1->Value(CV1.ParameterOnArc());
      }
      paredge1 = CV1.ParameterOnArc();
      if (Fi1.LineIndex() != 0)
      {
        pfil1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(isfirst));
      }
      else
      {
        pfil1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(!isfirst));
      }
      pfildeb = pfil1;
    }
    else
    {
      pfil1    = pfil2;
      paredge1 = paredge2;
      pfac1    = pint;
    }

    if (nb != nbface || isOnSame2)
    {
      int nbp;

      occ::handle<Geom_Curve> C;
      C                                    = BRep_Tool::Curve(E2, Ubid, Vbid);
      occ::handle<Geom_TrimmedCurve> Ctrim = new Geom_TrimmedCurve(C, Ubid, Vbid);
      double                         Utrim, Vtrim;
      Utrim = Ctrim->BasisCurve()->FirstParameter();
      Vtrim = Ctrim->BasisCurve()->LastParameter();
      if (Ctrim->IsPeriodic())
      {
        if (Ubid > Ctrim->Period())
        {
          Ubid = (Utrim + Vtrim) / 2;
          Vbid = Vtrim;
        }
        else
        {
          Ubid = Utrim;
          Vbid = (Utrim + Vtrim) / 2;
        }
      }
      else
      {
        Ubid = Utrim;
        Vbid = Vtrim;
      }
      occ::handle<GeomAdaptor_Curve> HC  = new GeomAdaptor_Curve(C, Ubid, Vbid);
      GeomAdaptor_Curve&             Cad = *HC;
      inters.Perform(HC, HGs);
      if (!prolface[nn] && (!inters.IsDone() || (inters.NbPoints() == 0)))
      {
        // extend surface of conge
        occ::handle<Geom_BoundedSurface> S1 =
          occ::down_cast<Geom_BoundedSurface>(DStr.Surface(Fd->Surf()).Surface());
        if (!S1.IsNull())
        {
          double length = 0.5 * std::max(Fi1Length, Fi2Length);
          GeomLib::ExtendSurfByLength(S1, length, 1, false, !isfirst);
          prolface[nn] = 1;
          if (!stripe->IsInDS(!isfirst))
          {
            Gs.Load(S1);
            inters.Perform(HC, HGs);
            if (inters.IsDone() && inters.NbPoints() != 0)
            {
              Fd->ChangeSurf(
                DStr.AddSurface(TopOpeBRepDS_Surface(S1, DStr.ChangeSurface(Isurf).Tolerance())));
              // update history
              if (myEVIMap.IsBound(EdgeSpine))
              {
                NCollection_List<int>::Iterator itl(myEVIMap.ChangeFind(EdgeSpine));
                for (; itl.More(); itl.Next())
                  if (itl.Value() == Isurf)
                  {
                    myEVIMap.ChangeFind(EdgeSpine).Remove(itl);
                    break;
                  }
                myEVIMap.ChangeFind(EdgeSpine).Append(Fd->Surf());
              }
              else
              {
                NCollection_List<int> IndexList;
                IndexList.Append(Fd->Surf());
                myEVIMap.Bind(EdgeSpine, IndexList);
              }
              ////////////////
              Isurf = Fd->Surf();
            }
          }
        }
      }
      if (!inters.IsDone() || (inters.NbPoints() == 0))
      {
        occ::handle<Geom_BSplineCurve> cd  = occ::down_cast<Geom_BSplineCurve>(C);
        occ::handle<Geom_BezierCurve>  cd1 = occ::down_cast<Geom_BezierCurve>(C);
        if (!cd.IsNull() || !cd1.IsNull())
        {
          BRep_Builder BRE;
          Sface = BRep_Tool::Surface(Face[nb]);
          ChFi3d_ExtendSurface(Sface, prolface[nb]);
          double tol = BRep_Tool::Tolerance(F);
          BRE.MakeFace(faceprol[nb], Sface, Face[nb].Location(), tol);
          if (nb && !prolface[nb - 1])
          {
            ChFi3d_ExtendSurface(Sfacemoins1, prolface[nb - 1]);
            if (prolface[nb - 1])
            {
              tol = BRep_Tool::Tolerance(F);
              BRE.MakeFace(faceprol[nb - 1], Sfacemoins1, F.Location(), tol);
            }
          }
          else
          {
            int prol = 0;
            ChFi3d_ExtendSurface(Sfacemoins1, prol);
          }
          GeomInt_IntSS InterSS(Sfacemoins1, Sface, 1.e-7, true, true, true);
          if (InterSS.IsDone())
          {
            trouve = false;
            for (int i = 1; i <= InterSS.NbLines() && !trouve; i++)
            {
              extend  = true;
              cint    = InterSS.Line(i);
              C2dint1 = InterSS.LineOnS1(i);
              C2dint2 = InterSS.LineOnS2(i);
              Cad.Load(cint);
              inters.Perform(HC, HGs);
              trouve = inters.IsDone() && inters.NbPoints() != 0;
              // eap occ293, eval tolex on finally trimmed curves
              //               occ::handle<GeomAdaptor_Surface> H1=new
              //               GeomAdaptor_Surface(Sfacemoins1); occ::handle<GeomAdaptor_Surface>
              //               H2=new GeomAdaptor_Surface(Sface);
              //              tolex=ChFi3d_EvalTolReached(H1,C2dint1,H2,C2dint2,cint);
              tolex = InterSS.TolReached3d();
            }
          }
        }
      }
      if (inters.IsDone())
      {
        nbp = inters.NbPoints();
        if (nbp == 0)
        {
          if (nb == 0 || nb == nbface)
            intersOnSameFailed = true;
          else
          {
            PerformMoreThreeCorner(Index, 1);
            return;
          }
        }
        else
        {
          gp_Pnt P       = BRep_Tool::Pnt(Vtx);
          double distmin = P.Distance(inters.Point(1).Pnt());
          nbp            = 1;
          for (int i = 2; i <= inters.NbPoints(); i++)
          {
            dist = P.Distance(inters.Point(i).Pnt());
            if (dist < distmin)
            {
              distmin = dist;
              nbp     = i;
            }
          }
          gp_Pnt2d pt2d(inters.Point(nbp).U(), inters.Point(nbp).V());
          pfil2    = pt2d;
          paredge2 = inters.Point(nbp).W();
          if (!extend)
          {
            cfacemoins1 = BRep_Tool::CurveOnSurface(E2, F, u2, v2);
            if (cfacemoins1.IsNull())
              throw Standard_ConstructionError("Failed to get p-curve of edge");
            cface = BRep_Tool::CurveOnSurface(E2, Face[nb], u2, v2);
            if (cface.IsNull())
              throw Standard_ConstructionError("Failed to get p-curve of edge");
            cfacemoins1->D0(paredge2, pfac2);
            cface->D0(paredge2, pint);
          }
          else if (C2dint1.IsNull() || C2dint2.IsNull())
          {
            throw Standard_ConstructionError("Failed to get p-curve of edge");
          }
          else
          {
            C2dint1->D0(paredge2, pfac2);
            C2dint2->D0(paredge2, pint);
          }
        }
      }
      else
        throw Standard_Failure("IntersectionAtEnd: pb intersection Face cb");
    }
    else
    {
      Hc2 = BRep_Tool::CurveOnSurface(E2, Face[nbface - 1], Ubid, Ubid);
      if (Hc2.IsNull())
      {
        // curve 2d is not found,  Sfacemoins1 is extended CV2.Point() is projected there

        ChFi3d_ExtendSurface(Sfacemoins1, prolface[0]);
        if (prolface[0])
        {
          BRep_Builder BRE;
          extend     = true;
          double tol = BRep_Tool::Tolerance(F);
          BRE.MakeFace(faceprol[nb - 1], Sfacemoins1, F.Location(), tol);
          GeomAdaptor_Surface Asurf;
          Asurf.Load(Sfacemoins1);
          Extrema_ExtPS ext(CV2.Point(), Asurf, tol, tol);
          double        uc2, vc2;
          if (ext.IsDone())
          {
            ext.Point(1).Parameter(uc2, vc2);
            pfac2.SetX(uc2);
            pfac2.SetY(vc2);
          }
        }
      }
      else
        pfac2 = Hc2->Value(CV2.ParameterOnArc());
      paredge2 = CV2.ParameterOnArc();
      if (Fi2.LineIndex() != 0)
      {
        pfil2 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(isfirst));
      }
      else
      {
        pfil2 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(!isfirst));
      }
    }
    if (!nb)
      continue; // found paredge1 on Edge[0] in OnSame situation on F1

    if (nb == nbface && isOnSame2)
    {
      // update interference param on Fi2 and point of CV2
      if (prolface[nb - 1])
        Bs.Initialize(faceprol[nb - 1]);
      else
        Bs.Initialize(Face[nb - 1]);
      const occ::handle<Geom_Curve>& c3df = DStr.Curve(Fi2.LineIndex()).Curve();
      double                         Ufi  = Fi1.Parameter(isfirst);
      ChFiDS_FaceInterference&       Fi   = Fd->ChangeInterferenceOnS2();
      if (!IntersUpdateOnSame(HGs,
                              HBs,
                              c3df,
                              F2,
                              F,
                              Edge[nb],
                              Vtx,
                              isfirst,
                              10 * tolapp3d, // in
                              Fi,
                              CV2,
                              pfac2,
                              Ufi)) // out
        throw Standard_Failure("IntersectionAtEnd: pb intersection Face - Fi");
      Fi2 = Fi;
      if (intersOnSameFailed)
      { // probable at fillet building
        // look for paredge2
        Geom2dAPI_ProjectPointOnCurve proj;
        if (extend)
          proj.Init(pfac2, C2dint2);
        else
          proj.Init(pfac2, BRep_Tool::CurveOnSurface(E2, Face[nbface - 1], Ubid, Ubid));
        paredge2 = proj.LowerDistanceParameter();
      }
      // update stripe point
      TopOpeBRepDS_Point tpoint(CV2.Point(), tolapp3d);
      indpoint2 = DStr.AddPoint(tpoint);
      stripe->SetIndexPoint(indpoint2, isfirst, 2);
      // reset arc of CV2
      TopoDS_Vertex vert1, vert2;
      TopExp::Vertices(Edge[nbface], vert1, vert2);
      TopAbs_Orientation arcOri = Vtx.IsSame(vert1) ? TopAbs_FORWARD : TopAbs_REVERSED;
      CV2.SetArc(tolapp3d, Edge[nbface], paredge2, arcOri);
    }

    if (prolface[nb - 1])
      Bs.Initialize(faceprol[nb - 1]);
    else
      Bs.Initialize(Face[nb - 1]);

    // offset of parameters if they are not in the same period

    // commented by eap 30 May 2002 occ354
    // the following code may cause trimming a wrong part of periodic surface

    //     double  deb,xx1,xx2;
    //     bool  moins2pi,moins2pi1,moins2pi2;
    //     if (DStr.Surface(Fd->Surf()).Surface()->IsUPeriodic()) {
    //       deb=pfildeb.X();
    //       xx1=pfil1.X();
    //       xx2=pfil2.X();
    //       moins2pi=std::abs(deb)< std::abs(std::abs(deb)-2*M_PI);
    //       moins2pi1=std::abs(xx1)< std::abs(std::abs(xx1)-2*M_PI);
    //       moins2pi2=std::abs(xx2)< std::abs(std::abs(xx2)-2*M_PI);
    //       if (moins2pi1!=moins2pi2) {
    //         if  (moins2pi) {
    //           if (!moins2pi1) xx1=xx1-2*M_PI;
    //           if (!moins2pi2) xx2=xx2-2*M_PI;
    //         }
    //         else {
    //           if (moins2pi1) xx1=xx1+2*M_PI;
    //           if (moins2pi2) xx2=xx2+2*M_PI;
    //         }
    //       }
    //       pfil1.SetX(xx1);
    //       pfil2.SetX(xx2);
    //     }
    //     if (couture || Sfacemoins1->IsUPeriodic()) {

    //       double ufmin,ufmax,vfmin,vfmax;
    //       BRepTools::UVBounds(Face[nb-1],ufmin,ufmax,vfmin,vfmax);
    //       deb=ufmin;
    //       xx1=pfac1.X();
    //       xx2=pfac2.X();
    //       moins2pi=std::abs(deb)< std::abs(std::abs(deb)-2*M_PI);
    //       moins2pi1=std::abs(xx1)< std::abs(std::abs(xx1)-2*M_PI);
    //       moins2pi2=std::abs(xx2)< std::abs(std::abs(xx2)-2*M_PI);
    //       if (moins2pi1!=moins2pi2) {
    //         if  (moins2pi) {
    //           if (!moins2pi1) xx1=xx1-2*M_PI;
    //           if (!moins2pi2) xx2=xx2-2*M_PI;
    //         }
    //         else {
    //           if (moins2pi1) xx1=xx1+2*M_PI;
    //           if (moins2pi2) xx2=xx2+2*M_PI;
    //         }
    //       }
    //       pfac1.SetX(xx1);
    //       pfac2.SetX(xx2);
    //     }

    Pardeb(1) = pfil1.X();
    Pardeb(2) = pfil1.Y();
    Pardeb(3) = pfac1.X();
    Pardeb(4) = pfac1.Y();
    Parfin(1) = pfil2.X();
    Parfin(2) = pfil2.Y();
    Parfin(3) = pfac2.X();
    Parfin(4) = pfac2.Y();

    double uu1, uu2, vv1, vv2;
    ChFi3d_Boite(pfac1, pfac2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bs, uu1, uu2, vv1, vv2);

    //////////////////////////////////////////////////////////////////////
    // calculate intersections face - fillet
    //////////////////////////////////////////////////////////////////////

    if (!ChFi3d_ComputeCurves(HGs,
                              HBs,
                              Pardeb,
                              Parfin,
                              Cc,
                              Ps,
                              Pc,
                              tolapp3d,
                              tol2d,
                              tolreached,
                              nbface == 1))
    {
      PerformMoreThreeCorner(Index, 1);
      return;
    }
    // storage of information in the data structure

    // evaluate tolerances
    p1 = Cc->FirstParameter();
    p2 = Cc->LastParameter();
    double   to1, to2;
    gp_Pnt2d p2d1, p2d2;
    gp_Pnt   P1, P2, P3, P4, P5, P6, P7, P8;
    HGs->D0(Pardeb(1), Pardeb(2), P1);
    HGs->D0(Parfin(1), Parfin(2), P2);
    HBs->D0(Pardeb(3), Pardeb(4), P3);
    HBs->D0(Parfin(3), Parfin(4), P4);
    Pc->D0(p1, p2d1);
    Pc->D0(p2, p2d2);
    HBs->D0(p2d1.X(), p2d1.Y(), P7);
    HBs->D0(p2d2.X(), p2d2.Y(), P8);
    Ps->D0(p1, p2d1);
    Ps->D0(p2, p2d2);
    HGs->D0(p2d1.X(), p2d1.Y(), P5);
    HGs->D0(p2d2.X(), p2d2.Y(), P6);
    to1 = std::max(P1.Distance(P5) + P3.Distance(P7), tolreached);
    to2 = std::max(P2.Distance(P6) + P4.Distance(P8), tolreached);

    //////////////////////////////////////////////////////////////////////
    // storage in the DS of the intersection curve
    //////////////////////////////////////////////////////////////////////

    bool Isvtx1 = false;
    bool Isvtx2 = false;
    int  indice;

    if (nb == 1)
    {
      indpoint1 = stripe->IndexPoint(isfirst, 1);
      if (!CV1.IsVertex())
      {
        TopOpeBRepDS_Point& tpt = DStr.ChangePoint(indpoint1);
        tpt.Tolerance(std::max(tpt.Tolerance(), to1));
      }
      else
        Isvtx1 = true;
    }
    if (nb == nbface)
    {
      indpoint2 = stripe->IndexPoint(isfirst, 2);
      if (!CV2.IsVertex())
      {
        TopOpeBRepDS_Point& tpt = DStr.ChangePoint(indpoint2);
        tpt.Tolerance(std::max(tpt.Tolerance(), to2));
      }
      else
        Isvtx2 = true;
    }
    else
    {
      gp_Pnt             point = Cc->Value(Cc->LastParameter());
      TopOpeBRepDS_Point tpoint(point, to2);
      indpoint2 = DStr.AddPoint(tpoint);
    }

    if (nb != 1)
    {
      TopOpeBRepDS_Point& tpt = DStr.ChangePoint(indpoint1);
      tpt.Tolerance(std::max(tpt.Tolerance(), to1));
    }
    TopOpeBRepDS_Curve tcurv3d(Cc, tolreached);
    indcurve[nb - 1] = DStr.AddCurve(tcurv3d);
    Interfp1         = ChFi3d_FilPointInDS(TopAbs_FORWARD,
                                   indcurve[nb - 1],
                                   indpoint1,
                                   Cc->FirstParameter(),
                                   Isvtx1);
    Interfp2         = ChFi3d_FilPointInDS(TopAbs_REVERSED,
                                   indcurve[nb - 1],
                                   indpoint2,
                                   Cc->LastParameter(),
                                   Isvtx2);

    DStr.ChangeCurveInterferences(indcurve[nb - 1]).Append(Interfp1);
    DStr.ChangeCurveInterferences(indcurve[nb - 1]).Append(Interfp2);

    //////////////////////////////////////////////////////////////////////
    // storage for the face
    //////////////////////////////////////////////////////////////////////

    TopAbs_Orientation ori = TopAbs_FORWARD;
    orface                 = Face[nb - 1].Orientation();
    if (orface == orsurfdata)
      orien = TopAbs::Reverse(orcourbe);
    else
      orien = orcourbe;
    // limitation of edges of faces
    if (nb == 1)
    {
      int Iarc1 = DStr.AddShape(Edge[0]);
      Interfedge[0] =
        ChFi3d_FilPointInDS(CV1.TransitionOnArc(), Iarc1, indpoint1, paredge1, Isvtx1);
      // DStr.ChangeShapeInterferences(Edge[0]).Append(Interfp1);
    }
    if (nb == nbface)
    {
      int Iarc2 = DStr.AddShape(Edge[nb]);
      Interfedge[nb] =
        ChFi3d_FilPointInDS(CV2.TransitionOnArc(), Iarc2, indpoint2, paredge2, Isvtx2);
      // DStr.ChangeShapeInterferences(Edge[nb]).Append(Interfp2);
    }

    if (nb != nbface || oneintersection1 || oneintersection2)
    {
      if (nface == 3)
      {
        V1 = TopExp::FirstVertex(Edge[nb]);
        V2 = TopExp::LastVertex(Edge[nb]);
        if (containV(F1, V1) || containV(F2, V1))
          ori = TopAbs_FORWARD;
        else if (containV(F1, V2) || containV(F2, V2))
          ori = TopAbs_REVERSED;
        else
          throw Standard_Failure("IntersectionAtEnd : pb orientation");

        if (containV(F1, V1) && containV(F1, V2))
        {
          dist1 = (BRep_Tool::Pnt(V1)).Distance(BRep_Tool::Pnt(Vtx));
          dist2 = (BRep_Tool::Pnt(V2)).Distance(BRep_Tool::Pnt(Vtx));
          if (dist1 < dist2)
            ori = TopAbs_FORWARD;
          else
            ori = TopAbs_REVERSED;
        }
        if (containV(F2, V1) && containV(F2, V2))
        {
          dist1 = (BRep_Tool::Pnt(V1)).Distance(BRep_Tool::Pnt(Vtx));
          dist2 = (BRep_Tool::Pnt(V2)).Distance(BRep_Tool::Pnt(Vtx));
          if (dist1 < dist2)
            ori = TopAbs_FORWARD;
          else
            ori = TopAbs_REVERSED;
        }
      }
      else
      {
        if (TopExp::FirstVertex(Edge[nb]).IsSame(Vtx))
          ori = TopAbs_FORWARD;
        else
          ori = TopAbs_REVERSED;
      }
      if (!extend && !(oneintersection1 || oneintersection2))
      {
        int Iarc2      = DStr.AddShape(Edge[nb]);
        Interfedge[nb] = ChFi3d_FilPointInDS(ori, Iarc2, indpoint2, paredge2);
        //  DStr.ChangeShapeInterferences(Edge[nb]).Append(Interfp2);
      }
      else
      {
        if (!(oneintersection1 || oneintersection2))
          proledge[nb] = true;
        int    indp1, indp2, ind;
        gp_Pnt pext;
        double ubid, vbid;
        pext = BRep_Tool::Pnt(Vtx);
        GeomAdaptor_Curve       cad;
        occ::handle<Geom_Curve> csau;
        if (!(oneintersection1 || oneintersection2))
        {
          cad.Load(cint);
          csau = cint;
        }
        else
        {
          csau                              = BRep_Tool::Curve(edgesau, ubid, vbid);
          occ::handle<Geom_BoundedCurve> C1 = occ::down_cast<Geom_BoundedCurve>(csau);
          if (oneintersection1 && extend)
          {
            if (!C1.IsNull())
            {
              gp_Pnt Pl;
              Pl = C1->Value(C1->LastParameter());
              // bool sens;
              sens = Pl.Distance(pext) < tolpt;
              GeomLib::ExtendCurveToPoint(C1, CV1.Point(), 1, sens != 0);
              csau = C1;
            }
          }
          else if (oneintersection2 && extend)
          {
            if (!C1.IsNull())
            {
              gp_Pnt Pl;
              Pl = C1->Value(C1->LastParameter());
              // bool sens;
              sens = Pl.Distance(pext) < tolpt;
              GeomLib::ExtendCurveToPoint(C1, CV2.Point(), 1, sens != 0);
              csau = C1;
            }
          }
          cad.Load(csau);
        }
        Extrema_ExtPC ext(pext, cad, tolpt);
        double        par1, par2, par, ParVtx;
        bool          vtx1 = false;
        bool          vtx2 = false;
        par1               = ext.Point(1).Parameter();
        ParVtx             = par1;
        if (oneintersection1 || oneintersection2)
        {
          if (oneintersection2)
          {
            pext = CV2.Point();
            ind  = indpoint2;
          }
          else
          {
            pext = CV1.Point();
            ind  = indpoint1;
          }
          Extrema_ExtPC ext2(pext, cad, tolpt);
          par2 = ext2.Point(1).Parameter();
        }
        else
        {
          par2 = paredge2;
          ind  = indpoint2;
        }
        if (par1 > par2)
        {
          indp1 = ind;
          indp2 = DStr.AddShape(Vtx);
          vtx2  = true;
          par   = par1;
          par1  = par2;
          par2  = par;
        }
        else
        {
          indp1 = DStr.AddShape(Vtx);
          indp2 = ind;
          vtx1  = true;
        }
        occ::handle<Geom_Curve> Ct = new Geom_TrimmedCurve(csau, par1, par2);
        TopAbs_Orientation      orient;
        Cc->D0(Cc->FirstParameter(), P1);
        Cc->D0(Cc->LastParameter(), P2);
        Ct->D0(Ct->FirstParameter(), P3);
        Ct->D0(Ct->LastParameter(), P4);
        if (P2.Distance(P3) < tolpt || P1.Distance(P4) < tolpt)
          orient = orien;
        else
          orient = TopAbs::Reverse(orien);
        if (oneintersection1 || oneintersection2)
        {
          indice = DStr.AddShape(Face[0]);
          if (extend)
          {
            DStr.SetNewSurface(Face[0], Sfacemoins1);
            ComputeCurve2d(Ct, faceprol[0], C2dint1);
          }
          else
          {
            TopoDS_Edge aLocalEdge = edgesau;
            if (edgesau.Orientation() != orient)
              aLocalEdge.Reverse();
            C2dint1 = BRep_Tool::CurveOnSurface(aLocalEdge, Face[0], ubid, vbid);
          }
        }
        else
        {
          indice = DStr.AddShape(Face[nb - 1]);
          DStr.SetNewSurface(Face[nb - 1], Sfacemoins1);
        }
        //// for periodic 3d curves ////
        if (cad.IsPeriodic() && !C2dint1.IsNull())
        {
          gp_Pnt2d                      P2d = BRep_Tool::Parameters(Vtx, Face[0]);
          Geom2dAPI_ProjectPointOnCurve Projector(P2d, C2dint1);
          par          = Projector.LowerDistanceParameter();
          double shift = par - ParVtx;
          if (std::abs(shift) > Precision::Confusion())
          {
            par1 += shift;
            par2 += shift;
          }
        }
        ////////////////////////////////

        Ct = new Geom_TrimmedCurve(csau, par1, par2);
        if (oneintersection1 || oneintersection2)
          tolex = 10 * BRep_Tool::Tolerance(edgesau);
        if (extend)
        {
          occ::handle<GeomAdaptor_Surface> H1, H2;
          H1 = new GeomAdaptor_Surface(Sfacemoins1);
          if (Sface.IsNull())
            tolex = std::max(tolex, ChFi3d_EvalTolReached(H1, C2dint1, H1, C2dint1, Ct));
          else
          {
            H2    = new GeomAdaptor_Surface(Sface);
            tolex = std::max(tolex, ChFi3d_EvalTolReached(H1, C2dint1, H2, C2dint2, Ct));
          }
        }
        TopOpeBRepDS_Curve tcurv(Ct, tolex);
        int                indcurv;
        indcurv  = DStr.AddCurve(tcurv);
        Interfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indcurv, indp1, par1, vtx1);
        Interfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indcurv, indp2, par2, vtx2);
        DStr.ChangeCurveInterferences(indcurv).Append(Interfp1);
        DStr.ChangeCurveInterferences(indcurv).Append(Interfp2);

        Interfc = ChFi3d_FilCurveInDS(indcurv, indice, C2dint1, orient);
        DStr.ChangeShapeInterferences(indice).Append(Interfc);
        if (oneintersection1 || oneintersection2)
        {
          indice = DStr.AddShape(facesau);
          if (facesau.Orientation() == Face[0].Orientation())
            orient = TopAbs::Reverse(orient);
          if (extend)
          {
            ComputeCurve2d(Ct, faceprol[1], C2dint2);
          }
          else
          {
            TopoDS_Edge aLocalEdge = edgesau;
            if (edgesau.Orientation() != orient)
              aLocalEdge.Reverse();
            C2dint2 = BRep_Tool::CurveOnSurface(aLocalEdge, facesau, ubid, vbid);
            // Reverse for case of edgesau on closed surface (Face[0] is equal to facesau)
          }
        }
        else
        {
          indice = DStr.AddShape(Face[nb]);
          DStr.SetNewSurface(Face[nb], Sface);
          if (Face[nb].Orientation() == Face[nb - 1].Orientation())
            orient = TopAbs::Reverse(orient);
        }
        if (!bordlibre)
        {
          Interfc = ChFi3d_FilCurveInDS(indcurv, indice, C2dint2, orient);
          DStr.ChangeShapeInterferences(indice).Append(Interfc);
        }
      }
    }

    if (checkShrink
        && IsShrink(Ps, p1, p2, checkShrParam, isUShrink, Precision::Parametric(tolreached)))
    {
      shrink[nb - 1] = 1;
      // store section face-chamf curve for previous SurfData
      // Suppose Fd and SDprev are parametrized similarly
      if (!isShrink)
      { // first time
        const ChFiDS_FaceInterference& Fi = SDprev->InterferenceOnS1();
        gp_Pnt2d                       UV = Fi.PCurveOnSurf()->Value(Fi.Parameter(isfirst));
        prevSDParam                       = isUShrink ? UV.X() : UV.Y();
      }
      gp_Pnt2d UV1 = p2d1, UV2 = p2d2;
      UV1.SetCoord(isUShrink ? 1 : 2, prevSDParam);
      UV2.SetCoord(isUShrink ? 1 : 2, prevSDParam);
      double aTolreached;
      ChFi3d_ComputePCurv(Cc,
                          UV1,
                          UV2,
                          Ps,
                          DStr.Surface(SDprev->Surf()).Surface(),
                          p1,
                          p2,
                          tolapp3d,
                          aTolreached);
      TopOpeBRepDS_Curve& TCurv = DStr.ChangeCurve(indcurve[nb - 1]);
      TCurv.Tolerance(std::max(TCurv.Tolerance(), aTolreached));

      InterfPS[nb - 1] = ChFi3d_FilCurveInDS(indcurve[nb - 1], IsurfPrev, Ps, orcourbe);
      DStr.ChangeSurfaceInterferences(IsurfPrev).Append(InterfPS[nb - 1]);

      if (isOnSame2)
      {
        midP2d    = p2d2;
        midIpoint = indpoint2;
      }
      else if (!isShrink)
      {
        midP2d    = p2d1;
        midIpoint = indpoint1;
      }
      isShrink = true;
    } // end if shrink

    indice           = DStr.AddShape(Face[nb - 1]);
    InterfPC[nb - 1] = ChFi3d_FilCurveInDS(indcurve[nb - 1], indice, Pc, orien);
    if (!shrink[nb - 1])
      InterfPS[nb - 1] = ChFi3d_FilCurveInDS(indcurve[nb - 1], Isurf, Ps, orcourbe);
    indpoint1 = indpoint2;

  } // end loop on faces being intersected with ChFi

  if (isOnSame1)
    CV1.Reset();
  if (isOnSame2)
    CV2.Reset();

  for (nb = 1; nb <= nbface; nb++)
  {
    int indice = DStr.AddShape(Face[nb - 1]);
    DStr.ChangeShapeInterferences(indice).Append(InterfPC[nb - 1]);
    if (!shrink[nb - 1])
      DStr.ChangeSurfaceInterferences(Isurf).Append(InterfPS[nb - 1]);
    if (!proledge[nb - 1])
      DStr.ChangeShapeInterferences(Edge[nb - 1]).Append(Interfedge[nb - 1]);
  }
  DStr.ChangeShapeInterferences(Edge[nbface]).Append(Interfedge[nbface]);

  if (!isShrink)
    stripe->InDS(isfirst);
  else
  {
    // compute curves for !<isfirst> end of <Fd> and <isfirst> end of previous <SurfData>

    // for Fd
    // Bnd_Box box;
    gp_Pnt2d UV, UV1 = midP2d, UV2 = midP2d;
    if (isOnSame1)
      UV = UV2 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(!isfirst));
    else
      UV = UV1 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(!isfirst));
    double                    aTolreached;
    occ::handle<Geom_Curve>   C3d;
    occ::handle<Geom_Surface> aSurf = DStr.Surface(Fd->Surf()).Surface();
    // box.Add(aSurf->Value(UV.X(), UV.Y()));

    ChFi3d_ComputeArete(CV1,
                        UV1,
                        CV2,
                        UV2,
                        aSurf, // in
                        C3d,
                        Ps,
                        p1,
                        p2,
                        tolapp3d,
                        tol2d,
                        aTolreached,
                        0); // out except tolers

    indpoint1 = indpoint2 = midIpoint;
    gp_Pnt point;
    if (isOnSame1)
    {
      point = C3d->Value(p2);
      TopOpeBRepDS_Point tpoint(point, aTolreached);
      indpoint2 = DStr.AddPoint(tpoint);
      UV        = Ps->Value(p2);
    }
    else
    {
      point = C3d->Value(p1);
      TopOpeBRepDS_Point tpoint(point, aTolreached);
      indpoint1 = DStr.AddPoint(tpoint);
      UV        = Ps->Value(p1);
    }
    // box.Add(point);
    // box.Add(aSurf->Value(UV.X(), UV.Y()));

    TopOpeBRepDS_Curve Crv   = TopOpeBRepDS_Curve(C3d, aTolreached);
    int                Icurv = DStr.AddCurve(Crv);
    Interfp1                 = ChFi3d_FilPointInDS(TopAbs_FORWARD, Icurv, indpoint1, p1, false);
    Interfp2                 = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv, indpoint2, p2, false);
    Interfc                  = ChFi3d_FilCurveInDS(Icurv, Isurf, Ps, orcourbe);
    DStr.ChangeCurveInterferences(Icurv).Append(Interfp1);
    DStr.ChangeCurveInterferences(Icurv).Append(Interfp2);
    DStr.ChangeSurfaceInterferences(Isurf).Append(Interfc);

    // for SDprev
    aSurf = DStr.Surface(SDprev->Surf()).Surface();
    UV1.SetCoord(isUShrink ? 1 : 2, prevSDParam);
    UV2.SetCoord(isUShrink ? 1 : 2, prevSDParam);

    ChFi3d_ComputePCurv(C3d, UV1, UV2, Pc, aSurf, p1, p2, tolapp3d, aTolreached);

    Crv.Tolerance(std::max(Crv.Tolerance(), aTolreached));
    Interfc = ChFi3d_FilCurveInDS(Icurv, IsurfPrev, Pc, TopAbs::Reverse(orcourbe));
    DStr.ChangeSurfaceInterferences(IsurfPrev).Append(Interfc);

    // UV = isOnSame1 ? UV2 : UV1;
    // box.Add(aSurf->Value(UV.X(), UV.Y()));
    // UV = Ps->Value(isOnSame1 ? p2 : p1);
    // box.Add(aSurf->Value(UV.X(), UV.Y()));
    // ChFi3d_SetPointTolerance(DStr,box, isOnSame1 ? indpoint2 : indpoint1);

    // to process properly this case in ChFi3d_FilDS()
    stripe->InDS(isfirst, 2);
    Fd->ChangeInterference(isOnSame1 ? 2 : 1).SetLineIndex(0);
    ChFiDS_CommonPoint& CPprev1 = SDprev->ChangeVertex(isfirst, isOnSame1 ? 2 : 1);
    ChFiDS_CommonPoint& CPlast1 = Fd->ChangeVertex(isfirst, isOnSame1 ? 2 : 1);
    ChFiDS_CommonPoint& CPlast2 = Fd->ChangeVertex(!isfirst, isOnSame1 ? 2 : 1);
    if (CPprev1.IsOnArc())
    {
      CPlast1 = CPprev1;
      CPprev1.Reset();
      CPprev1.SetPoint(CPlast1.Point());
      CPlast2.Reset();
      CPlast2.SetPoint(CPlast1.Point());
    }

    // in shrink case, self intersection is possible at <midIpoint>,
    // eval its tolerance intersecting Ps and Pcurve at end.
    // Find end curves closest to shrinked part
    for (nb = 0; nb < nbface; nb++)
      if (isOnSame1 ? shrink[nb + 1] : !shrink[nb])
        break;
    occ::handle<Geom_Curve>   Cend  = DStr.Curve(indcurve[nb]).Curve();
    occ::handle<Geom2d_Curve> PCend = InterfPS[nb]->PCurve();
    // point near which self intersection may occur
    TopOpeBRepDS_Point& Pds   = DStr.ChangePoint(midIpoint);
    const gp_Pnt&       Pvert = Pds.Point();
    double              tol   = Pds.Tolerance();

    Geom2dAdaptor_Curve PC1(Ps), PC2(PCend);
    Geom2dInt_GInter    Intersector(PC1, PC2, Precision::PConfusion(), Precision::PConfusion());
    if (!Intersector.IsDone())
      return;
    for (nb = 1; nb <= Intersector.NbPoints(); nb++)
    {
      const IntRes2d_IntersectionPoint& ip   = Intersector.Point(nb);
      gp_Pnt                            Pint = C3d->Value(ip.ParamOnFirst());
      tol                                    = std::max(tol, Pvert.Distance(Pint));
      Pint                                   = Cend->Value(ip.ParamOnSecond());
      tol                                    = std::max(tol, Pvert.Distance(Pint));
    }
    for (nb = 1; nb <= Intersector.NbSegments(); nb++)
    {
      const IntRes2d_IntersectionSegment& is = Intersector.Segment(nb);
      if (is.HasFirstPoint())
      {
        const IntRes2d_IntersectionPoint& ip   = is.FirstPoint();
        gp_Pnt                            Pint = C3d->Value(ip.ParamOnFirst());
        tol                                    = std::max(tol, Pvert.Distance(Pint));
        Pint                                   = Cend->Value(ip.ParamOnSecond());
        tol                                    = std::max(tol, Pvert.Distance(Pint));
      }
      if (is.HasLastPoint())
      {
        const IntRes2d_IntersectionPoint& ip   = is.LastPoint();
        gp_Pnt                            Pint = C3d->Value(ip.ParamOnFirst());
        tol                                    = std::max(tol, Pvert.Distance(Pint));
        Pint                                   = Cend->Value(ip.ParamOnSecond());
        tol                                    = std::max(tol, Pvert.Distance(Pint));
      }
    }
    Pds.Tolerance(tol);
  }
}

//  Modified by Sergey KHROMOV - Thu Apr 11 12:23:40 2002 Begin

//=======================================================================
// function : PerformMoreSurfdata
// purpose  :  determine intersections at end on several surfdata
//=======================================================================
void ChFi3d_Builder::PerformMoreSurfdata(const int Index)
{
  TopOpeBRepDS_DataStructure&                         DStr       = myDS->ChangeDS();
  const NCollection_List<occ::handle<ChFiDS_Stripe>>& aLOfStripe = myVDataMap(Index);
  occ::handle<ChFiDS_Stripe>                          aStripe;
  occ::handle<ChFiDS_Spine>                           aSpine;
  double                                              aTol3d = 1.e-4;

  if (aLOfStripe.IsEmpty())
    return;

  aStripe = aLOfStripe.First();
  aSpine  = aStripe->Spine();

  NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& aSeqSurfData =
    aStripe->ChangeSetOfSurfData()->ChangeSequence();
  const TopoDS_Vertex&         aVtx    = myVDataMap.FindKey(Index);
  int                          aSens   = 0;
  int                          anInd   = ChFi3d_IndexOfSurfData(aVtx, aStripe, aSens);
  bool                         isFirst = (aSens == 1);
  int                          anIndPrev;
  occ::handle<ChFiDS_SurfData> aSurfData;
  ChFiDS_CommonPoint           aCP1;
  ChFiDS_CommonPoint           aCP2;

  aSurfData = aSeqSurfData.Value(anInd);

  aCP1 = aSurfData->Vertex(isFirst, 1);
  aCP2 = aSurfData->Vertex(isFirst, 2);

  occ::handle<Geom_Surface> aSurfPrev;
  occ::handle<Geom_Surface> aSurf;
  TopoDS_Face               aFace;
  TopoDS_Face               aNeighborFace;

  FindFace(aVtx, aCP1, aCP2, aFace);
  aSurfPrev = BRep_Tool::Surface(aFace);

  if (aSens == 1)
    anIndPrev = anInd + 1;
  else
    anIndPrev = anInd - 1;

  TopoDS_Edge                              anArc1;
  TopoDS_Edge                              anArc2;
  NCollection_List<TopoDS_Shape>::Iterator anIter(myVEMap(aVtx));
  bool                                     isFound = false;

  for (; anIter.More() && !isFound; anIter.Next())
  {
    anArc1 = TopoDS::Edge(anIter.Value());

    if (containE(aFace, anArc1))
      isFound = true;
  }

  isFound = false;
  anIter.Initialize(myVEMap(aVtx));

  for (; anIter.More() && !isFound; anIter.Next())
  {
    anArc2 = TopoDS::Edge(anIter.Value());

    if (containE(aFace, anArc2) && !anArc2.IsSame(anArc1))
      isFound = true;
  }

  // determination of common points aCP1onArc, aCP2onArc and aCP2NotonArc
  // aCP1onArc    is the point on arc of index anInd
  // aCP2onArc    is the point on arc of index anIndPrev
  // aCP2NotonArc is the point of index anIndPrev which is not on arc.

  bool               is1stCP1OnArc;
  bool               is2ndCP1OnArc;
  ChFiDS_CommonPoint aCP1onArc;
  ChFiDS_CommonPoint aCP2onArc;
  ChFiDS_CommonPoint aCP2NotonArc;

  aSurfData = aSeqSurfData.Value(anIndPrev);
  aCP1      = aSurfData->Vertex(isFirst, 1);
  aCP2      = aSurfData->Vertex(isFirst, 2);

  if (aCP1.IsOnArc() && (aCP1.Arc().IsSame(anArc1) || aCP1.Arc().IsSame(anArc2)))
  {
    aCP2onArc     = aCP1;
    aCP2NotonArc  = aCP2;
    is2ndCP1OnArc = true;
  }
  else if (aCP2.IsOnArc() && (aCP2.Arc().IsSame(anArc1) || aCP2.Arc().IsSame(anArc2)))
  {
    aCP2onArc     = aCP2;
    aCP2NotonArc  = aCP1;
    is2ndCP1OnArc = false;
  }
  else
    return;

  aSurfData = aSeqSurfData.Value(anInd);
  aCP1      = aSurfData->Vertex(isFirst, 1);
  aCP2      = aSurfData->Vertex(isFirst, 2);

  if (aCP1.Point().Distance(aCP2onArc.Point()) <= aTol3d)
  {
    aCP1onArc     = aCP2;
    is1stCP1OnArc = false;
  }
  else
  {
    aCP1onArc     = aCP1;
    is1stCP1OnArc = true;
  }

  if (!aCP1onArc.IsOnArc())
    return;

  // determination of neighbor surface
  int indSurface;
  if (is1stCP1OnArc)
    indSurface = myListStripe.First()->SetOfSurfData()->Value(anInd)->IndexOfS1();
  else
    indSurface = myListStripe.First()->SetOfSurfData()->Value(anInd)->IndexOfS2();

  aNeighborFace = TopoDS::Face(myDS->Shape(indSurface));

  // calculation of intersections
  occ::handle<Geom_Curve>   aCracc;
  occ::handle<Geom2d_Curve> aPCurv1;
  double                    aParf;
  double                    aParl;
  double                    aTolReached;

  aSurfData = aSeqSurfData.Value(anInd);

  if (isFirst)
    ChFi3d_ComputeArete(aSurfData->VertexLastOnS1(),
                        aSurfData->InterferenceOnS1().PCurveOnSurf()->Value(
                          aSurfData->InterferenceOnS1().LastParameter()),
                        aSurfData->VertexLastOnS2(),
                        aSurfData->InterferenceOnS2().PCurveOnSurf()->Value(
                          aSurfData->InterferenceOnS2().LastParameter()),
                        DStr.Surface(aSurfData->Surf()).Surface(),
                        aCracc,
                        aPCurv1,
                        aParf,
                        aParl,
                        aTol3d,
                        tol2d,
                        aTolReached,
                        0);
  else
    ChFi3d_ComputeArete(aSurfData->VertexFirstOnS1(),
                        aSurfData->InterferenceOnS1().PCurveOnSurf()->Value(
                          aSurfData->InterferenceOnS1().FirstParameter()),
                        aSurfData->VertexFirstOnS2(),
                        aSurfData->InterferenceOnS2().PCurveOnSurf()->Value(
                          aSurfData->InterferenceOnS2().FirstParameter()),
                        DStr.Surface(aSurfData->Surf()).Surface(),
                        aCracc,
                        aPCurv1,
                        aParf,
                        aParl,
                        aTol3d,
                        tol2d,
                        aTolReached,
                        0);

  // calculation of the index of the line on anInd.
  // aPClineOnSurf is the pcurve on anInd.
  // aPClineOnFace is the pcurve on face.
  ChFiDS_FaceInterference aFI;

  if (is1stCP1OnArc)
    aFI = aSurfData->InterferenceOnS1();
  else
    aFI = aSurfData->InterferenceOnS2();

  occ::handle<Geom_Curve>   aCline;
  occ::handle<Geom2d_Curve> aPClineOnSurf;
  occ::handle<Geom2d_Curve> aPClineOnFace;
  int                       indLine;

  indLine       = aFI.LineIndex();
  aCline        = DStr.Curve(aFI.LineIndex()).Curve();
  aPClineOnSurf = aFI.PCurveOnSurf();
  aPClineOnFace = aFI.PCurveOnFace();

  // intersection between the SurfData number anInd and the Face aFace.
  // Obtaining of curves aCint1, aPCint11 and aPCint12.
  aSurf = DStr.Surface(aSurfData->Surf()).Surface();

  GeomInt_IntSS                    anInterSS(aSurfPrev, aSurf, 1.e-7, true, true, true);
  occ::handle<Geom_Curve>          aCint1;
  occ::handle<Geom2d_Curve>        aPCint11;
  occ::handle<Geom2d_Curve>        aPCint12;
  occ::handle<GeomAdaptor_Surface> H1      = new GeomAdaptor_Surface(aSurfPrev);
  occ::handle<GeomAdaptor_Surface> H2      = new GeomAdaptor_Surface(aSurf);
  double                           aTolex1 = 0.;
  int                              i;
  gp_Pnt                           aPext1;
  gp_Pnt                           aPext2;
  gp_Pnt                           aPext;
  bool                             isPextFound;

  if (!anInterSS.IsDone())
    return;

  isFound = false;

  for (i = 1; i <= anInterSS.NbLines() && !isFound; i++)
  {
    aCint1   = anInterSS.Line(i);
    aPCint11 = anInterSS.LineOnS1(i);
    aPCint12 = anInterSS.LineOnS2(i);
    aTolex1  = ChFi3d_EvalTolReached(H1, aPCint11, H2, aPCint12, aCint1);

    aCint1->D0(aCint1->FirstParameter(), aPext1);
    aCint1->D0(aCint1->LastParameter(), aPext2);

    //  Modified by skv - Mon Jun  7 18:38:57 2004 OCC5898 Begin
    //     if (aPext1.Distance(aCP1onArc.Point()) <= aTol3d ||
    // 	aPext2.Distance(aCP1onArc.Point()))
    if (aPext1.Distance(aCP1onArc.Point()) <= aTol3d
        || aPext2.Distance(aCP1onArc.Point()) <= aTol3d)
      //  Modified by skv - Mon Jun  7 18:38:58 2004 OCC5898 End
      isFound = true;
  }

  if (!isFound)
    return;

  if (aPext1.Distance(aCP2onArc.Point()) > aTol3d && aPext1.Distance(aCP1onArc.Point()) > aTol3d)
  {
    aPext       = aPext1;
    isPextFound = true;
  }
  else if (aPext2.Distance(aCP2onArc.Point()) > aTol3d
           && aPext2.Distance(aCP1onArc.Point()) > aTol3d)
  {
    aPext       = aPext2;
    isPextFound = true;
  }
  else
  {
    isPextFound = false;
  }

  bool   isDoSecondSection = false;
  double aPar              = 0.;

  if (isPextFound)
  {
    GeomAdaptor_Curve aCad(aCracc);
    Extrema_ExtPC     anExt(aPext, aCad, aTol3d);

    if (!anExt.IsDone())
      return;

    isFound = false;
    for (i = 1; i <= anExt.NbExt() && !isFound; i++)
    {
      if (anExt.IsMin(i))
      {
        gp_Pnt aProjPnt = anExt.Point(i).Value();

        if (aPext.Distance(aProjPnt) <= aTol3d)
        {
          aPar              = anExt.Point(i).Parameter();
          isDoSecondSection = true;
        }
      }
    }
  }

  occ::handle<Geom_Curve> aTrCracc;
  TopAbs_Orientation      anOrSD1;
  TopAbs_Orientation      anOrSD2;
  int                     indShape;

  anOrSD1   = aSurfData->Orientation();
  aSurfData = aSeqSurfData.Value(anIndPrev);
  anOrSD2   = aSurfData->Orientation();
  aSurf     = DStr.Surface(aSurfData->Surf()).Surface();

  // The following variables will be used if isDoSecondSection is true
  occ::handle<Geom_Curve>   aCint2;
  occ::handle<Geom2d_Curve> aPCint21;
  occ::handle<Geom2d_Curve> aPCint22;
  double                    aTolex2 = 0.;

  if (isDoSecondSection)
  {
    double aPar1;

    aCracc->D0(aCracc->FirstParameter(), aPext1);

    if (aPext1.Distance(aCP2NotonArc.Point()) <= aTol3d)
      aPar1 = aCracc->FirstParameter();
    else
      aPar1 = aCracc->LastParameter();

    if (aPar1 < aPar)
      aTrCracc = new Geom_TrimmedCurve(aCracc, aPar1, aPar);
    else
      aTrCracc = new Geom_TrimmedCurve(aCracc, aPar, aPar1);

    // Second section
    GeomInt_IntSS anInterSS2(aSurfPrev, aSurf, 1.e-7, true, true, true);

    if (!anInterSS2.IsDone())
      return;

    H1 = new GeomAdaptor_Surface(aSurfPrev);
    H2 = new GeomAdaptor_Surface(aSurf);

    isFound = false;

    for (i = 1; i <= anInterSS2.NbLines() && !isFound; i++)
    {
      aCint2   = anInterSS2.Line(i);
      aPCint21 = anInterSS2.LineOnS1(i);
      aPCint22 = anInterSS2.LineOnS2(i);
      aTolex2  = ChFi3d_EvalTolReached(H1, aPCint21, H2, aPCint22, aCint2);

      aCint2->D0(aCint2->FirstParameter(), aPext1);
      aCint2->D0(aCint2->LastParameter(), aPext2);

      if (aPext1.Distance(aCP2onArc.Point()) <= aTol3d
          || aPext2.Distance(aCP2onArc.Point()) <= aTol3d)
        isFound = true;
    }

    if (!isFound)
      return;
  }
  else
  {
    aTrCracc = new Geom_TrimmedCurve(aCracc, aCracc->FirstParameter(), aCracc->LastParameter());
  }

  // Storage of the data structure

  // calculation of the orientation of line of surfdata number
  // anIndPrev which contains aCP2onArc

  occ::handle<Geom2d_Curve> aPCraccS = GeomProjLib::Curve2d(aTrCracc, aSurf);

  if (is2ndCP1OnArc)
  {
    aFI      = aSurfData->InterferenceOnS1();
    indShape = aSurfData->IndexOfS1();
  }
  else
  {
    aFI      = aSurfData->InterferenceOnS2();
    indShape = aSurfData->IndexOfS2();
  }

  if (indShape <= 0)
    return;

  TopAbs_Orientation aCurOrient;

  aCurOrient = DStr.Shape(indShape).Orientation();
  aCurOrient = TopAbs::Compose(aCurOrient, aSurfData->Orientation());
  aCurOrient = TopAbs::Compose(TopAbs::Reverse(aFI.Transition()), aCurOrient);

  // Filling the data structure
  aSurfData = aSeqSurfData.Value(anInd);

  TopOpeBRepDS_Point aPtCP1(aCP1onArc.Point(), aCP1onArc.Tolerance());
  int                indCP1onArc = DStr.AddPoint(aPtCP1);
  int                indSurf1    = aSurfData->Surf();
  int                indArc1     = DStr.AddShape(aCP1onArc.Arc());
  int                indSol      = aStripe->SolidIndex();

  occ::handle<TopOpeBRepDS_CurvePointInterference> anInterfp1;
  occ::handle<TopOpeBRepDS_CurvePointInterference> anInterfp2;

  anInterfp1 = ChFi3d_FilPointInDS(aCP1onArc.TransitionOnArc(),
                                   indArc1,
                                   indCP1onArc,
                                   aCP1onArc.ParameterOnArc());
  DStr.ChangeShapeInterferences(aCP1onArc.Arc()).Append(anInterfp1);

  NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& SolidInterfs =
    DStr.ChangeShapeInterferences(indSol);
  occ::handle<TopOpeBRepDS_SolidSurfaceInterference> SSI =
    new TopOpeBRepDS_SolidSurfaceInterference(TopOpeBRepDS_Transition(anOrSD1),
                                              TopOpeBRepDS_SOLID,
                                              indSol,
                                              TopOpeBRepDS_SURFACE,
                                              indSurf1);
  SolidInterfs.Append(SSI);

  // deletion of Surface Data.
  aSeqSurfData.Remove(anInd);

  if (!isFirst)
    anInd--;

  aSurfData = aSeqSurfData.Value(anInd);

  // definition of indices of common points in Data Structure

  int indCP2onArc;
  int indCP2NotonArc;

  if (is2ndCP1OnArc)
  {
    aStripe->SetIndexPoint(ChFi3d_IndexPointInDS(aCP2onArc, DStr), isFirst, 1);
    aStripe->SetIndexPoint(ChFi3d_IndexPointInDS(aCP2NotonArc, DStr), isFirst, 2);

    if (isFirst)
    {
      indCP2onArc    = aStripe->IndexFirstPointOnS1();
      indCP2NotonArc = aStripe->IndexFirstPointOnS2();
    }
    else
    {
      indCP2onArc    = aStripe->IndexLastPointOnS1();
      indCP2NotonArc = aStripe->IndexLastPointOnS2();
    }
  }
  else
  {
    aStripe->SetIndexPoint(ChFi3d_IndexPointInDS(aCP2onArc, DStr), isFirst, 2);
    aStripe->SetIndexPoint(ChFi3d_IndexPointInDS(aCP2NotonArc, DStr), isFirst, 1);

    if (isFirst)
    {
      indCP2onArc    = aStripe->IndexFirstPointOnS2();
      indCP2NotonArc = aStripe->IndexFirstPointOnS1();
    }
    else
    {
      indCP2onArc    = aStripe->IndexLastPointOnS2();
      indCP2NotonArc = aStripe->IndexLastPointOnS1();
    }
  }

  int    indPoint1;
  int    indPoint2;
  gp_Pnt aPoint1;
  gp_Pnt aPoint2;

  if (is2ndCP1OnArc)
  {
    aFI      = aSurfData->InterferenceOnS1();
    indShape = aSurfData->IndexOfS1();
  }
  else
  {
    aFI      = aSurfData->InterferenceOnS2();
    indShape = aSurfData->IndexOfS2();
  }

  gp_Pnt2d                                           aP2d;
  occ::handle<TopOpeBRepDS_SurfaceCurveInterference> anInterfc;
  TopAbs_Orientation                                 anOrSurf = aCurOrient;
  TopAbs_Orientation                                 anOrFace = aFace.Orientation();
  int                                                indaFace = DStr.AddShape(aFace);
  int                                                indPoint = indCP2onArc;
  int                                                indCurve;

  aFI.PCurveOnFace()->D0(aFI.LastParameter(), aP2d);
  occ::handle<Geom_Surface> Stemp2 = BRep_Tool::Surface(TopoDS::Face(DStr.Shape(indShape)));
  Stemp2->D0(aP2d.X(), aP2d.Y(), aPoint2);
  aFI.PCurveOnFace()->D0(aFI.FirstParameter(), aP2d);
  Stemp2->D0(aP2d.X(), aP2d.Y(), aPoint1);

  if (isDoSecondSection)
  {
    TopOpeBRepDS_Point tpoint(aPext, aTolex2);
    TopOpeBRepDS_Curve tcint2(aCint2, aTolex2);

    indPoint = DStr.AddPoint(tpoint);
    indCurve = DStr.AddCurve(tcint2);

    aCint2->D0(aCint2->FirstParameter(), aPext1);
    aCint2->D0(aCint2->LastParameter(), aPext2);

    if (aPext1.Distance(aPext) <= aTol3d)
    {
      indPoint1 = indPoint;
      indPoint2 = indCP2onArc;
    }
    else
    {
      indPoint1 = indCP2onArc;
      indPoint2 = indPoint;
    }

    // define the orientation of aCint2
    if (aPext1.Distance(aPoint2) > aTol3d && aPext2.Distance(aPoint1) > aTol3d)
      anOrSurf = TopAbs::Reverse(anOrSurf);

    // ---------------------------------------------------------------
    // storage of aCint2
    anInterfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indCurve, indPoint1, aCint2->FirstParameter());
    anInterfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indCurve, indPoint2, aCint2->LastParameter());
    DStr.ChangeCurveInterferences(indCurve).Append(anInterfp1);
    DStr.ChangeCurveInterferences(indCurve).Append(anInterfp2);

    // interference of aCint2 on the SurfData number anIndPrev
    anInterfc = ChFi3d_FilCurveInDS(indCurve, aSurfData->Surf(), aPCint22, anOrSurf);

    DStr.ChangeSurfaceInterferences(aSurfData->Surf()).Append(anInterfc);
    // interference of aCint2 on aFace

    if (anOrFace == anOrSD2)
      anOrFace = TopAbs::Reverse(anOrSurf);
    else
      anOrFace = anOrSurf;

    anInterfc = ChFi3d_FilCurveInDS(indCurve, indaFace, aPCint21, anOrFace);
    DStr.ChangeShapeInterferences(indaFace).Append(anInterfc);
  }

  aTrCracc->D0(aTrCracc->FirstParameter(), aPext1);
  aTrCracc->D0(aTrCracc->LastParameter(), aPext2);
  if (aPext1.Distance(aCP2NotonArc.Point()) <= aTol3d)
  {
    indPoint1 = indCP2NotonArc;
    indPoint2 = indPoint;
  }
  else
  {
    indPoint1 = indPoint;
    indPoint2 = indCP2NotonArc;
  }

  // Define the orientation of aTrCracc
  bool   isToReverse;
  gp_Pnt aP1;
  gp_Pnt aP2;
  gp_Pnt aP3;
  gp_Pnt aP4;

  if (isDoSecondSection)
  {
    aTrCracc->D0(aTrCracc->FirstParameter(), aP1);
    aTrCracc->D0(aTrCracc->LastParameter(), aP2);
    aCint2->D0(aCint2->FirstParameter(), aP3);
    aCint2->D0(aCint2->LastParameter(), aP4);
    isToReverse = (aP1.Distance(aP4) > aTol3d && aP2.Distance(aP3) > aTol3d);
  }
  else
  {
    isToReverse = (aPext1.Distance(aPoint2) > aTol3d && aPext2.Distance(aPoint1) > aTol3d);
  }

  if (isToReverse)
    anOrSurf = TopAbs::Reverse(anOrSurf);

  // ---------------------------------------------------------------
  // storage of aTrCracc
  TopOpeBRepDS_Curve tct2(aTrCracc, aTolReached);

  indCurve   = DStr.AddCurve(tct2);
  anInterfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indCurve, indPoint1, aTrCracc->FirstParameter());
  anInterfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indCurve, indPoint2, aTrCracc->LastParameter());
  DStr.ChangeCurveInterferences(indCurve).Append(anInterfp1);
  DStr.ChangeCurveInterferences(indCurve).Append(anInterfp2);

  // interference of aTrCracc on the SurfData number anIndPrev

  anInterfc = ChFi3d_FilCurveInDS(indCurve, aSurfData->Surf(), aPCraccS, anOrSurf);
  DStr.ChangeSurfaceInterferences(aSurfData->Surf()).Append(anInterfc);
  aStripe->InDS(isFirst);

  // interference of aTrCracc on the SurfData number anInd
  if (anOrSD1 == anOrSD2)
    anOrSurf = TopAbs::Reverse(anOrSurf);

  anInterfc = ChFi3d_FilCurveInDS(indCurve, indSurf1, aPCurv1, anOrSurf);
  DStr.ChangeSurfaceInterferences(indSurf1).Append(anInterfc);

  // ---------------------------------------------------------------
  // storage of aCint1

  aCint1->D0(aCint1->FirstParameter(), aPext1);
  if (aPext1.Distance(aCP1onArc.Point()) <= aTol3d)
  {
    indPoint1 = indCP1onArc;
    indPoint2 = indPoint;
  }
  else
  {
    indPoint1 = indPoint;
    indPoint2 = indCP1onArc;
  }

  //  definition of the orientation of aCint1

  aCint1->D0(aCint1->FirstParameter(), aP1);
  aCint1->D0(aCint1->LastParameter(), aP2);
  aTrCracc->D0(aTrCracc->FirstParameter(), aP3);
  aTrCracc->D0(aTrCracc->LastParameter(), aP4);

  if (aP1.Distance(aP4) > aTol3d && aP2.Distance(aP3) > aTol3d)
    anOrSurf = TopAbs::Reverse(anOrSurf);

  TopOpeBRepDS_Curve aTCint1(aCint1, aTolex1);
  indCurve   = DStr.AddCurve(aTCint1);
  anInterfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indCurve, indPoint1, aCint1->FirstParameter());
  anInterfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indCurve, indPoint2, aCint1->LastParameter());
  DStr.ChangeCurveInterferences(indCurve).Append(anInterfp1);
  DStr.ChangeCurveInterferences(indCurve).Append(anInterfp2);

  // interference of aCint1 on the SurfData number anInd

  anInterfc = ChFi3d_FilCurveInDS(indCurve, indSurf1, aPCint12, anOrSurf);
  DStr.ChangeSurfaceInterferences(indSurf1).Append(anInterfc);

  // interference of aCint1 on aFace

  anOrFace = aFace.Orientation();

  if (anOrFace == anOrSD1)
    anOrFace = TopAbs::Reverse(anOrSurf);
  else
    anOrFace = anOrSurf;

  anInterfc = ChFi3d_FilCurveInDS(indCurve, indaFace, aPCint11, anOrFace);
  DStr.ChangeShapeInterferences(indaFace).Append(anInterfc);
  // ---------------------------------------------------------------
  // storage of aCline passing through aCP1onArc and aCP2NotonArc

  occ::handle<Geom_Curve> aTrCline =
    new Geom_TrimmedCurve(aCline, aCline->FirstParameter(), aCline->LastParameter());
  double             aTolerance = DStr.Curve(indLine).Tolerance();
  TopOpeBRepDS_Curve aTct3(aTrCline, aTolerance);

  indCurve = DStr.AddCurve(aTct3);

  aTrCline->D0(aTrCline->FirstParameter(), aPext1);

  if (aPext1.Distance(aCP1onArc.Point()) < aTol3d)
  {
    indPoint1 = indCP1onArc;
    indPoint2 = indCP2NotonArc;
  }
  else
  {
    indPoint1 = indCP2NotonArc;
    indPoint2 = indCP1onArc;
  }
  //  definition of the orientation of aTrCline

  aTrCline->D0(aTrCline->FirstParameter(), aP1);
  aTrCline->D0(aTrCline->LastParameter(), aP2);
  aCint1->D0(aCint1->FirstParameter(), aP3);
  aCint1->D0(aCint1->LastParameter(), aP4);

  if (aP1.Distance(aP4) > aTol3d && aP2.Distance(aP3) > aTol3d)
    anOrSurf = TopAbs::Reverse(anOrSurf);

  anInterfp1 = ChFi3d_FilPointInDS(TopAbs_FORWARD, indCurve, indPoint1, aTrCline->FirstParameter());
  anInterfp2 = ChFi3d_FilPointInDS(TopAbs_REVERSED, indCurve, indPoint2, aTrCline->LastParameter());
  DStr.ChangeCurveInterferences(indCurve).Append(anInterfp1);
  DStr.ChangeCurveInterferences(indCurve).Append(anInterfp2);

  // interference of aTrCline on the SurfData number anInd

  anInterfc = ChFi3d_FilCurveInDS(indCurve, indSurf1, aPClineOnSurf, anOrSurf);
  DStr.ChangeSurfaceInterferences(indSurf1).Append(anInterfc);

  // interference de ctlin par rapport a Fvoisin
  indShape = DStr.AddShape(aNeighborFace);
  anOrFace = aNeighborFace.Orientation();

  if (anOrFace == anOrSD1)
    anOrFace = TopAbs::Reverse(anOrSurf);
  else
    anOrFace = anOrSurf;

  anInterfc = ChFi3d_FilCurveInDS(indCurve, indShape, aPClineOnFace, anOrFace);
  DStr.ChangeShapeInterferences(indShape).Append(anInterfc);
}

//  Modified by Sergey KHROMOV - Thu Apr 11 12:23:40 2002 End

//==============================================================
// function : FindFace
// purpose  : attention it works only if there is only one common face
//           between P1,P2,V
//===========================================================

bool ChFi3d_Builder::FindFace(const TopoDS_Vertex&      V,
                              const ChFiDS_CommonPoint& P1,
                              const ChFiDS_CommonPoint& P2,
                              TopoDS_Face&              Fv) const
{
  TopoDS_Face Favoid;
  return FindFace(V, P1, P2, Fv, Favoid);
}

bool ChFi3d_Builder::FindFace(const TopoDS_Vertex&      V,
                              const ChFiDS_CommonPoint& P1,
                              const ChFiDS_CommonPoint& P2,
                              TopoDS_Face&              Fv,
                              const TopoDS_Face&        Favoid) const
{
  if (P1.IsVertex() || P2.IsVertex())
  {
#ifdef OCCT_DEBUG
    std::cout << "change of face on vertex" << std::endl;
#endif
  }
  if (!(P1.IsOnArc() && P2.IsOnArc()))
  {
    return false;
  }
  NCollection_List<TopoDS_Shape>::Iterator It, Jt;
  bool                                     Found = false;
  for (It.Initialize(myEFMap(P1.Arc())); It.More() && !Found; It.Next())
  {
    Fv = TopoDS::Face(It.Value());
    if (!Fv.IsSame(Favoid))
    {
      for (Jt.Initialize(myEFMap(P2.Arc())); Jt.More() && !Found; Jt.Next())
      {
        if (TopoDS::Face(Jt.Value()).IsSame(Fv))
          Found = true;
      }
    }
  }
#ifdef OCCT_DEBUG
  bool ContainsV = false;
  if (Found)
  {
    for (It.Initialize(myVFMap(V)); It.More(); It.Next())
    {
      if (TopoDS::Face(It.Value()).IsSame(Fv))
      {
        ContainsV = true;
        break;
      }
    }
  }
  if (!ContainsV)
  {
    std::cout << "FindFace : the extremity of the spine is not in the end face" << std::endl;
  }
#else
  (void)V; // avoid compiler warning on unused variable
#endif
  return Found;
}

//=======================================================================
// function : MoreSurfdata
// purpose  : detects if the intersection at end concerns several Surfdata
//=======================================================================
bool ChFi3d_Builder::MoreSurfdata(const int Index) const
{
  // intersection at end is created on several surfdata if :
  // - the number of surfdata concerning the vertex is more than 1.
  // - and if the last but one surfdata has one of commonpoints on one of
  // two arcs, which constitute the intersections of the face at end and of the fillet

  NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator It;
  It.Initialize(myVDataMap(Index));
  occ::handle<ChFiDS_Stripe>&                         stripe = It.ChangeValue();
  NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqFil =
    stripe->ChangeSetOfSurfData()->ChangeSequence();
  const TopoDS_Vertex&          Vtx     = myVDataMap.FindKey(Index);
  int                           sens    = 0;
  int                           num     = ChFi3d_IndexOfSurfData(Vtx, stripe, sens);
  bool                          isfirst = (sens == 1);
  occ::handle<ChFiDS_SurfData>& Fd      = SeqFil.ChangeValue(num);
  ChFiDS_CommonPoint&           CV1     = Fd->ChangeVertex(isfirst, 1);
  ChFiDS_CommonPoint&           CV2     = Fd->ChangeVertex(isfirst, 2);

  int         num1, num2, nbsurf;
  TopoDS_Face Fv;
  bool        inters, oksurf;
  nbsurf = stripe->SetOfSurfData()->Length();
  // Fv is the face at end
  inters = FindFace(Vtx, CV1, CV2, Fv);
  if (sens == 1)
  {
    num1 = 1;
    num2 = num1 + 1;
  }
  else
  {
    num1 = nbsurf;
    num2 = num1 - 1;
  }

  oksurf = false;

  if (nbsurf != 1 && inters)
  {

    // determination of arc1 and arc2 intersection of the fillet and the face at end

    TopoDS_Edge                              arc1, arc2;
    NCollection_List<TopoDS_Shape>::Iterator ItE;
    bool                                     trouve = false;
    for (ItE.Initialize(myVEMap(Vtx)); ItE.More() && !trouve; ItE.Next())
    {
      arc1 = TopoDS::Edge(ItE.Value());
      if (containE(Fv, arc1))
        trouve = true;
    }
    trouve = false;
    for (ItE.Initialize(myVEMap(Vtx)); ItE.More() && !trouve; ItE.Next())
    {
      arc2 = TopoDS::Edge(ItE.Value());
      if (containE(Fv, arc2) && !arc2.IsSame(arc1))
        trouve = true;
    }

    occ::handle<ChFiDS_SurfData> Fd1 = SeqFil.ChangeValue(num2);
    ChFiDS_CommonPoint&          CV3 = Fd1->ChangeVertex(isfirst, 1);
    ChFiDS_CommonPoint&          CV4 = Fd1->ChangeVertex(isfirst, 2);

    if (CV3.IsOnArc())
    {
      if (CV3.Arc().IsSame(arc1))
      {
        if (CV1.Point().Distance(CV3.Point()) < 1.e-4)
          oksurf = true;
      }
      else if (CV3.Arc().IsSame(arc2))
      {
        if (CV2.Point().Distance(CV3.Point()) < 1.e-4)
          oksurf = true;
      }
    }

    if (CV4.IsOnArc())
    {
      if (CV1.Point().Distance(CV4.Point()) < 1.e-4)
        oksurf = true;
      else if (CV4.Arc().IsSame(arc2))
      {
        if (CV2.Point().Distance(CV4.Point()) < 1.e-4)
          oksurf = true;
      }
    }
  }
  return oksurf;
}

// Case of fillets on top with 4 edges, one of them is on the same geometry as the edgeof the fillet

void ChFi3d_Builder::IntersectMoreCorner(const int Index)
{
  TopOpeBRepDS_DataStructure& DStr = myDS->ChangeDS();

#ifdef OCCT_DEBUG
  OSD_Chronometer ch; // init perf pour PerformSetOfKPart
#endif
  // The fillet is returned,
  NCollection_List<occ::handle<ChFiDS_Stripe>>::Iterator StrIt;
  StrIt.Initialize(myVDataMap(Index));
  occ::handle<ChFiDS_Stripe>                          stripe = StrIt.Value();
  const occ::handle<ChFiDS_Spine>                     spine  = stripe->Spine();
  NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqFil =
    stripe->ChangeSetOfSurfData()->ChangeSequence();
  // the top,
  const TopoDS_Vertex& Vtx = myVDataMap.FindKey(Index);
  // the SurfData concerned and its CommonPoints,
  int sens = 0;

  // Choose the proper SurfData
  int  num     = ChFi3d_IndexOfSurfData(Vtx, stripe, sens);
  bool isfirst = (sens == 1);
  if (isfirst)
  {
    for (; num < SeqFil.Length()
           && ((SeqFil.Value(num)->IndexOfS1() == 0) || (SeqFil.Value(num)->IndexOfS2() == 0));)
    {
      SeqFil.Remove(num); // The surplus is removed
    }
  }
  else
  {
    for (; num > 1
           && ((SeqFil.Value(num)->IndexOfS1() == 0) || (SeqFil.Value(num)->IndexOfS2() == 0));)
    {
      SeqFil.Remove(num); // The surplus is removed
      num--;
    }
  }

  occ::handle<ChFiDS_SurfData>& Fd  = SeqFil.ChangeValue(num);
  ChFiDS_CommonPoint&           CV1 = Fd->ChangeVertex(isfirst, 1);
  ChFiDS_CommonPoint&           CV2 = Fd->ChangeVertex(isfirst, 2);
  // To evaluate the cloud of new points.
  Bnd_Box box1, box2;

  // The cases of cap are processed separately from intersection.
  // ----------------------------------------------------------

  TopoDS_Face Fv, Fad, Fop, Fopbis;
  TopoDS_Edge Arcpiv, Arcprol, Arcspine, Arcprolbis;
  if (isfirst)
    Arcspine = spine->Edges(1);
  else
    Arcspine = spine->Edges(spine->NbEdges());
  TopAbs_Orientation               OArcprolbis = TopAbs_FORWARD;
  TopAbs_Orientation               OArcprolv = TopAbs_FORWARD, OArcprolop = TopAbs_FORWARD;
  int                              ICurve;
  occ::handle<BRepAdaptor_Surface> HBs  = new BRepAdaptor_Surface();
  occ::handle<BRepAdaptor_Surface> HBad = new BRepAdaptor_Surface();
  occ::handle<BRepAdaptor_Surface> HBop = new BRepAdaptor_Surface();
  BRepAdaptor_Surface&             Bs   = *HBs;
  BRepAdaptor_Surface&             Bad  = *HBad;
  BRepAdaptor_Surface&             Bop  = *HBop;
  occ::handle<Geom_Curve>          Cc;
  occ::handle<Geom2d_Curve>        Pc, Ps;
  double                           Ubid, Vbid; //,mu,Mu,mv,Mv;
  double                           Udeb = 0., Ufin = 0.;
  // gp_Pnt2d UVf1,UVl1,UVf2,UVl2;
  // double Du,Dv,Step;
  bool inters  = true;
  int  IFadArc = 1, IFopArc = 2;
  Fop = TopoDS::Face(DStr.Shape(Fd->Index(IFopArc)));
  TopExp_Explorer ex;

#ifdef OCCT_DEBUG
  ChFi3d_InitChron(ch); // init perf condition
#endif
  {
    if (!CV1.IsOnArc() && !CV2.IsOnArc())
      throw Standard_Failure("Corner intersmore : no point on arc");
    else if (CV1.IsOnArc() && CV2.IsOnArc())
    {
      bool sur2 = false;
      for (ex.Init(CV1.Arc(), TopAbs_VERTEX); ex.More(); ex.Next())
      {
        if (Vtx.IsSame(ex.Current()))
        {
          break;
        }
      }
      for (ex.Init(CV2.Arc(), TopAbs_VERTEX); ex.More(); ex.Next())
      {
        if (Vtx.IsSame(ex.Current()))
        {
          sur2 = true;
          break;
        }
      }
      if (sur2)
        IFadArc = 2;
    }
    else if (CV2.IsOnArc())
      IFadArc = 2;
    IFopArc = 3 - IFadArc;

    Arcpiv = Fd->Vertex(isfirst, IFadArc).Arc();
    Fad    = TopoDS::Face(DStr.Shape(Fd->Index(IFadArc)));
    Fop    = TopoDS::Face(DStr.Shape(Fd->Index(IFopArc)));
    NCollection_List<TopoDS_Shape>::Iterator It;
    // The face at end is returned without control of its unicity.
    for (It.Initialize(myEFMap(Arcpiv)); It.More(); It.Next())
    {
      if (!Fad.IsSame(It.Value()))
      {
        Fv = TopoDS::Face(It.Value());
        break;
      }
    }

    // does the face at end contain the Vertex ?
    bool isinface = false;
    for (ex.Init(Fv, TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (ex.Current().IsSame(Vtx))
      {
        isinface = true;
        break;
      }
    }
    if (!isinface)
    {
      IFadArc = 3 - IFadArc;
      IFopArc = 3 - IFopArc;
      Arcpiv  = Fd->Vertex(isfirst, IFadArc).Arc();
      Fad     = TopoDS::Face(DStr.Shape(Fd->Index(IFadArc)));
      Fop     = TopoDS::Face(DStr.Shape(Fd->Index(IFopArc)));
      // NCollection_List<TopoDS_Shape>::Iterator It;
      // The face at end is returned without control of its unicity.
      for (It.Initialize(myEFMap(Arcpiv)); It.More(); It.Next())
      {
        if (!Fad.IsSame(It.Value()))
        {
          Fv = TopoDS::Face(It.Value());
          break;
        }
      }
    }

    if (Fv.IsNull())
      throw StdFail_NotDone("OneCorner : face at end is not found");

    Fv.Orientation(TopAbs_FORWARD);
    Fad.Orientation(TopAbs_FORWARD);

    // In the same way the edge to be extended is returned.
    for (It.Initialize(myVEMap(Vtx)); It.More() && Arcprol.IsNull(); It.Next())
    {
      if (!Arcpiv.IsSame(It.Value()))
      {
        for (ex.Init(Fv, TopAbs_EDGE); ex.More(); ex.Next())
        {
          if (It.Value().IsSame(ex.Current()))
          {
            Arcprol   = TopoDS::Edge(It.Value());
            OArcprolv = ex.Current().Orientation();
            break;
          }
        }
      }
    }

    // Fopbis is the face containing the trace of fillet CP.Arc() which of does not contain Vtx.
    // Normally Fobis is either the same as Fop (cylinder), or Fobis is G1 with Fop.
    Fopbis.Orientation(TopAbs_FORWARD);

    // Fop calls the 4th face non-used for the vertex
    cherche_face(myVFMap(Vtx), Arcprol, Fad, Fv, Fv, Fopbis);
    Fop.Orientation(TopAbs_FORWARD);

    if (Arcprol.IsNull())
      throw StdFail_NotDone("OneCorner : edge to be extended is not found");
    for (ex.Init(Fopbis, TopAbs_EDGE); ex.More(); ex.Next())
    {
      if (Arcprol.IsSame(ex.Current()))
      {
        OArcprolop = ex.Current().Orientation();
        break;
      }
    }
    TopoDS_Face               FFv;
    double                    tol;
    int                       prol;
    BRep_Builder              BRE;
    occ::handle<Geom_Surface> Sface;
    Sface = BRep_Tool::Surface(Fv);
    ChFi3d_ExtendSurface(Sface, prol);
    tol = BRep_Tool::Tolerance(Fv);
    BRE.MakeFace(FFv, Sface, tol);
    if (prol)
    {
      Bs.Initialize(FFv, false);
      DStr.SetNewSurface(Fv, Sface);
    }
    else
      Bs.Initialize(Fv, false);
    Bad.Initialize(Fad);
    Bop.Initialize(Fop);
  }
  // it is necessary to modify the CommonPoint
  // in the space and its parameter in FaceInterference.
  // So both of them are returned in references
  // non const. Attention the modifications are done behind
  // CV1,CV2,Fi1,Fi2.
  ChFiDS_CommonPoint&      CPopArc = Fd->ChangeVertex(isfirst, IFopArc);
  ChFiDS_FaceInterference& FiopArc = Fd->ChangeInterference(IFopArc);
  ChFiDS_CommonPoint&      CPadArc = Fd->ChangeVertex(isfirst, IFadArc);
  ChFiDS_FaceInterference& FiadArc = Fd->ChangeInterference(IFadArc);
  // the parameter of the vertex is initialized with the value
  // of its opposing vertex (point on arc).
  double                           wop = Fd->ChangeInterference(IFadArc).Parameter(isfirst);
  occ::handle<Geom_Curve>          c3df;
  occ::handle<GeomAdaptor_Surface> HGs =
    new GeomAdaptor_Surface(DStr.Surface(Fd->Surf()).Surface());
  gp_Pnt2d p2dbout;
  {

    // add here more or less restrictive criteria to
    // decide if the intersection with face is done at the
    // extended end or if there will be a cap on sharp end.
    c3df                              = DStr.Curve(FiopArc.LineIndex()).Curve();
    double                         uf = FiopArc.FirstParameter();
    double                         ul = FiopArc.LastParameter();
    occ::handle<GeomAdaptor_Curve> Hc3df;
    if (c3df->IsPeriodic())
    {
      Hc3df = new GeomAdaptor_Curve(c3df);
    }
    else
    {
      Hc3df = new GeomAdaptor_Curve(c3df, uf, ul);
    }
    inters = Update(HBs, Hc3df, FiopArc, CPopArc, p2dbout, isfirst, wop);
    //  Modified by Sergey KHROMOV - Fri Dec 21 18:08:27 2001 Begin
    //  if(!inters && BRep_Tool::Continuity(Arcprol,Fv,Fop) != GeomAbs_C0){
    if (!inters && ChFi3d::IsTangentFaces(Arcprol, Fv, Fop))
    {
      //  Modified by Sergey KHROMOV - Fri Dec 21 18:08:29 2001 End
      // Arcprol is an edge of tangency, ultimate adjustment by an extrema curve/curve is attempted.
      double                    ff, ll;
      occ::handle<Geom2d_Curve> gpcprol = BRep_Tool::CurveOnSurface(Arcprol, Fv, ff, ll);
      if (gpcprol.IsNull())
        throw Standard_ConstructionError("Failed to get p-curve of edge");
      occ::handle<Geom2dAdaptor_Curve> pcprol  = new Geom2dAdaptor_Curve(gpcprol);
      double                           partemp = BRep_Tool::Parameter(Vtx, Arcprol);
      inters =
        Update(HBs, pcprol, HGs, FiopArc, CPopArc, p2dbout, isfirst, partemp, wop, 10 * tolapp3d);
    }
    occ::handle<BRepAdaptor_Curve2d> pced = new BRepAdaptor_Curve2d();
    pced->Initialize(CPadArc.Arc(), Fv);
    Update(HBs, pced, HGs, FiadArc, CPadArc, isfirst);
  }
#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_same); // result perf condition if (same)
  ChFi3d_InitChron(ch);           // init perf condition if (inters)
#endif

  TopoDS_Edge                    edgecouture;
  bool                           couture, intcouture = false;
  double                         tolreached = tolapp3d;
  double                         par1 = 0., par2 = 0.;
  int                            indpt = 0, Icurv1 = 0, Icurv2 = 0;
  occ::handle<Geom_TrimmedCurve> curv1, curv2;
  occ::handle<Geom2d_Curve>      c2d1, c2d2;

  int Isurf = Fd->Surf();

  if (inters)
  {
    HGs                                = ChFi3d_BoundSurf(DStr, Fd, 1, 2);
    const ChFiDS_FaceInterference& Fi1 = Fd->InterferenceOnS1();
    const ChFiDS_FaceInterference& Fi2 = Fd->InterferenceOnS2();
    NCollection_Array1<double>     Pardeb(1, 4), Parfin(1, 4);
    gp_Pnt2d                       pfil1, pfac1, pfil2, pfac2;
    occ::handle<Geom2d_Curve>      Hc1, Hc2;
    if (IFopArc == 1)
      pfac1 = p2dbout;
    else
    {
      Hc1 = BRep_Tool::CurveOnSurface(CV1.Arc(), Fv, Ubid, Ubid);
      if (Hc1.IsNull())
        throw Standard_ConstructionError("Failed to get p-curve of edge");
      pfac1 = Hc1->Value(CV1.ParameterOnArc());
    }
    if (IFopArc == 2)
      pfac2 = p2dbout;
    else
    {
      Hc2 = BRep_Tool::CurveOnSurface(CV2.Arc(), Fv, Ubid, Ubid);
      if (Hc2.IsNull())
        throw Standard_ConstructionError("Failed to get p-curve of edge");
      pfac2 = Hc2->Value(CV2.ParameterOnArc());
    }
    if (Fi1.LineIndex() != 0)
    {
      pfil1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(isfirst));
    }
    else
    {
      pfil1 = Fi1.PCurveOnSurf()->Value(Fi1.Parameter(!isfirst));
    }
    if (Fi2.LineIndex() != 0)
    {
      pfil2 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(isfirst));
    }
    else
    {
      pfil2 = Fi2.PCurveOnSurf()->Value(Fi2.Parameter(!isfirst));
    }
    ChFi3d_Recale(Bs, pfac1, pfac2, (IFadArc == 1));
    Pardeb(1) = pfil1.X();
    Pardeb(2) = pfil1.Y();
    Pardeb(3) = pfac1.X();
    Pardeb(4) = pfac1.Y();
    Parfin(1) = pfil2.X();
    Parfin(2) = pfil2.Y();
    Parfin(3) = pfac2.X();
    Parfin(4) = pfac2.Y();
    double uu1, uu2, vv1, vv2;
    ChFi3d_Boite(pfac1, pfac2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bs, uu1, uu2, vv1, vv2);

    if (!ChFi3d_ComputeCurves(HGs, HBs, Pardeb, Parfin, Cc, Ps, Pc, tolapp3d, tol2d, tolreached))
      throw Standard_Failure("OneCorner : failed calculation intersection");

    Udeb = Cc->FirstParameter();
    Ufin = Cc->LastParameter();

    // check if the curve has an intersection with sewing edge

    ChFi3d_Couture(Fv, couture, edgecouture);

    if (couture && !BRep_Tool::Degenerated(edgecouture))
    {

      // double Ubid,Vbid;
      occ::handle<Geom_Curve>        C     = BRep_Tool::Curve(edgecouture, Ubid, Vbid);
      occ::handle<Geom_TrimmedCurve> Ctrim = new Geom_TrimmedCurve(C, Ubid, Vbid);
      GeomAdaptor_Curve              cur1(Ctrim->BasisCurve());
      GeomAdaptor_Curve              cur2(Cc);
      Extrema_ExtCC                  extCC(cur1, cur2);
      if (extCC.IsDone() && extCC.NbExt() != 0)
      {
        int    imin     = 0;
        double dist2min = RealLast();
        for (int i = 1; i <= extCC.NbExt(); i++)
          if (extCC.SquareDistance(i) < dist2min)
          {
            dist2min = extCC.SquareDistance(i);
            imin     = i;
          }
        if (dist2min <= Precision::SquareConfusion())
        {
          Extrema_POnCurv ponc1, ponc2;
          extCC.Points(imin, ponc1, ponc2);
          par1       = ponc1.Parameter();
          par2       = ponc2.Parameter();
          double Tol = 1.e-4;
          if (std::abs(par2 - Udeb) > Tol && std::abs(Ufin - par2) > Tol)
          {
            gp_Pnt             P1 = ponc1.Value();
            TopOpeBRepDS_Point tpoint(P1, Tol);
            indpt      = DStr.AddPoint(tpoint);
            intcouture = true;
            curv1      = new Geom_TrimmedCurve(Cc, Udeb, par2);
            curv2      = new Geom_TrimmedCurve(Cc, par2, Ufin);
            TopOpeBRepDS_Curve tcurv1(curv1, tolreached);
            TopOpeBRepDS_Curve tcurv2(curv2, tolreached);
            Icurv1 = DStr.AddCurve(tcurv1);
            Icurv2 = DStr.AddCurve(tcurv2);
          }
        }
      }
    }
  }

  else
  {
    throw Standard_NotImplemented("OneCorner : cap not written");
  }
  int                IShape = DStr.AddShape(Fv);
  TopAbs_Orientation Et     = TopAbs_FORWARD;
  if (IFadArc == 1)
  {
    TopExp_Explorer Exp;
    for (Exp.Init(Fv.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp.More(); Exp.Next())
    {
      if (Exp.Current().IsSame(CV1.Arc()))
      {
        Et = TopAbs::Reverse(TopAbs::Compose(Exp.Current().Orientation(), CV1.TransitionOnArc()));
        break;
      }
    }
  }
  else
  {
    TopExp_Explorer Exp;
    for (Exp.Init(Fv.Oriented(TopAbs_FORWARD), TopAbs_EDGE); Exp.More(); Exp.Next())
    {
      if (Exp.Current().IsSame(CV2.Arc()))
      {
        Et = TopAbs::Compose(Exp.Current().Orientation(), CV2.TransitionOnArc());
        break;
      }
    }

    //
  }

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_inter); // result perf condition if (inter)
  ChFi3d_InitChron(ch);            // init perf condition  if ( inters)
#endif

  stripe->SetIndexPoint(ChFi3d_IndexPointInDS(CV1, DStr), isfirst, 1);
  stripe->SetIndexPoint(ChFi3d_IndexPointInDS(CV2, DStr), isfirst, 2);

  if (!intcouture)
  {
    // there is no intersection with edge of sewing
    // curve Cc is stored in the stripe
    // the storage in the DS is done by FILDS.

    TopOpeBRepDS_Curve Tc(Cc, tolreached);
    ICurve = DStr.AddCurve(Tc);
    occ::handle<TopOpeBRepDS_SurfaceCurveInterference> Interfc =
      ChFi3d_FilCurveInDS(ICurve, IShape, Pc, Et);
    DStr.ChangeShapeInterferences(IShape).Append(Interfc);
    stripe->ChangePCurve(isfirst) = Ps;
    stripe->SetCurve(ICurve, isfirst);
    stripe->SetParameters(isfirst, Udeb, Ufin);
  }
  else
  {
    // curves curv1 and curv2 are stored in the DS
    // these curves are not reconstructed by FILDS as
    // stripe->InDS(isfirst) is placed;

    // interferences of curv1 and curv2 on Fv
    ComputeCurve2d(curv1, Fv, c2d1);
    occ::handle<TopOpeBRepDS_SurfaceCurveInterference> InterFv;
    InterFv = ChFi3d_FilCurveInDS(Icurv1, IShape, c2d1, Et);
    DStr.ChangeShapeInterferences(IShape).Append(InterFv);
    ComputeCurve2d(curv2, Fv, c2d2);
    InterFv = ChFi3d_FilCurveInDS(Icurv2, IShape, c2d2, Et);
    DStr.ChangeShapeInterferences(IShape).Append(InterFv);
    // interferences of curv1 and curv2 on Isurf
    if (Fd->Orientation() == Fv.Orientation())
      Et = TopAbs::Reverse(Et);
    c2d1    = new Geom2d_TrimmedCurve(Ps, Udeb, par2);
    InterFv = ChFi3d_FilCurveInDS(Icurv1, Isurf, c2d1, Et);
    DStr.ChangeSurfaceInterferences(Isurf).Append(InterFv);
    c2d2    = new Geom2d_TrimmedCurve(Ps, par2, Ufin);
    InterFv = ChFi3d_FilCurveInDS(Icurv2, Isurf, c2d2, Et);
    DStr.ChangeSurfaceInterferences(Isurf).Append(InterFv);

    // limitation of the sewing edge
    int                                              Iarc = DStr.AddShape(edgecouture);
    occ::handle<TopOpeBRepDS_CurvePointInterference> Interfedge;
    TopAbs_Orientation                               ori;
    TopoDS_Vertex                                    Vdeb, Vfin;
    Vdeb = TopExp::FirstVertex(edgecouture);
    Vfin = TopExp::LastVertex(edgecouture);
    double pard, parf;
    pard = BRep_Tool::Parameter(Vdeb, edgecouture);
    parf = BRep_Tool::Parameter(Vfin, edgecouture);
    if (std::abs(par1 - pard) < std::abs(parf - par1))
      ori = TopAbs_FORWARD;
    else
      ori = TopAbs_REVERSED;
    Interfedge = ChFi3d_FilPointInDS(ori, Iarc, indpt, par1);
    DStr.ChangeShapeInterferences(Iarc).Append(Interfedge);

    // creation of CurveInterferences from Icurv1 and Icurv2
    stripe->InDS(isfirst);
    int                                              ind1 = stripe->IndexPoint(isfirst, 1);
    int                                              ind2 = stripe->IndexPoint(isfirst, 2);
    occ::handle<TopOpeBRepDS_CurvePointInterference> interfprol =
      ChFi3d_FilPointInDS(TopAbs_FORWARD, Icurv1, ind1, Udeb);
    DStr.ChangeCurveInterferences(Icurv1).Append(interfprol);
    interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv1, indpt, par2);
    DStr.ChangeCurveInterferences(Icurv1).Append(interfprol);
    interfprol = ChFi3d_FilPointInDS(TopAbs_FORWARD, Icurv2, indpt, par2);
    DStr.ChangeCurveInterferences(Icurv2).Append(interfprol);
    interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, Icurv2, ind2, Ufin);
    DStr.ChangeCurveInterferences(Icurv2).Append(interfprol);
  }

  ChFi3d_EnlargeBox(HBs, Pc, Udeb, Ufin, box1, box2);

  if (inters)
  {
    //

    // The small end of curve missing for the extension
    // of the face at end and the limitation of the opposing face is added.

    // Above all the points cut the points with the edge of the spine.
    int                IArcspine = DStr.AddShape(Arcspine);
    int                IVtx      = DStr.AddShape(Vtx);
    TopAbs_Orientation OVtx2     = TopAbs_FORWARD;
    TopAbs_Orientation OVtx      = TopAbs_FORWARD;
    for (ex.Init(Arcspine.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (Vtx.IsSame(ex.Current()))
      {
        OVtx = ex.Current().Orientation();
        break;
      }
    }
    OVtx                                                    = TopAbs::Reverse(OVtx);
    double                                           parVtx = BRep_Tool::Parameter(Vtx, Arcspine);
    occ::handle<TopOpeBRepDS_CurvePointInterference> interfv =
      ChFi3d_FilVertexInDS(OVtx, IArcspine, IVtx, parVtx);
    DStr.ChangeShapeInterferences(IArcspine).Append(interfv);

    // Modif of lvt to find the suite of Arcprol in the other face
    {
      NCollection_List<TopoDS_Shape>::Iterator It;
      for (It.Initialize(myVEMap(Vtx)); It.More(); It.Next())
      {
        if (!(Arcprol.IsSame(It.Value()) || Arcspine.IsSame(It.Value())
              || Arcpiv.IsSame(It.Value())))
        {
          Arcprolbis = TopoDS::Edge(It.Value());
          break;
        }
      }
    }
    // end of modif

    // Now the missing curves are constructed.
    for (ex.Init(Arcprolbis.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (Vtx.IsSame(ex.Current()))
      {
        OVtx2 = ex.Current().Orientation();
        break;
      }
    }
    for (ex.Init(Arcprol.Oriented(TopAbs_FORWARD), TopAbs_VERTEX); ex.More(); ex.Next())
    {
      if (Vtx.IsSame(ex.Current()))
      {
        OVtx = ex.Current().Orientation();
        break;
      }
    }
    // it is checked if Fop has a sewing edge

    //     TopoDS_Edge edgecouture;
    //     bool couture;
    ChFi3d_Couture(Fop, couture, edgecouture);
    occ::handle<Geom2d_Curve> Hc;
    //    parVtx = BRep_Tool::Parameter(Vtx,Arcprol);
    const ChFiDS_FaceInterference& Fiop = Fd->Interference(IFopArc);
    gp_Pnt2d                       pop1, pop2, pv1, pv2;
    // deb modif
    parVtx = BRep_Tool::Parameter(Vtx, Arcprolbis);
    //  Modified by skv - Thu Aug 21 11:55:58 2008 OCC20222 Begin
    //    if(Fop.IsSame(Fopbis)) OArcprolbis = OArcprolop;
    //    else OArcprolbis = Arcprolbis.Orientation();
    if (Fop.IsSame(Fopbis))
    {
      OArcprolbis = OArcprolop;
    }
    else
    {
      for (ex.Init(Fop, TopAbs_EDGE); ex.More(); ex.Next())
      {
        if (Arcprolbis.IsSame(ex.Current()))
        {
          OArcprolbis = ex.Current().Orientation();
          break;
        }
      }
    }
    //  Modified by skv - Thu Aug 21 11:55:58 2008 OCC20222 End
    // fin modif
    Hc = BRep_Tool::CurveOnSurface(Arcprolbis, Fop, Ubid, Ubid);
    if (Hc.IsNull())
      throw Standard_ConstructionError("Failed to get p-curve of edge");
    pop1 = Hc->Value(parVtx);
    pop2 = Fiop.PCurveOnFace()->Value(Fiop.Parameter(isfirst));
    Hc   = BRep_Tool::CurveOnSurface(Arcprol, Fv, Ubid, Ubid);
    if (Hc.IsNull())
      throw Standard_ConstructionError("Failed to get p-curve of edge");
    // modif
    parVtx = BRep_Tool::Parameter(Vtx, Arcprol);
    // fin modif
    pv1 = Hc->Value(parVtx);
    pv2 = p2dbout;
    ChFi3d_Recale(Bs, pv1, pv2, true);
    NCollection_Array1<double> Pardeb(1, 4), Parfin(1, 4);
    Pardeb(1) = pop1.X();
    Pardeb(2) = pop1.Y();
    Pardeb(3) = pv1.X();
    Pardeb(4) = pv1.Y();
    Parfin(1) = pop2.X();
    Parfin(2) = pop2.Y();
    Parfin(3) = pv2.X();
    Parfin(4) = pv2.Y();
    double uu1, uu2, vv1, vv2;
    ChFi3d_Boite(pv1, pv2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bs, uu1, uu2, vv1, vv2);
    ChFi3d_Boite(pop1, pop2, uu1, uu2, vv1, vv2);
    ChFi3d_BoundFac(Bop, uu1, uu2, vv1, vv2);

    occ::handle<Geom_Curve>   zob3d;
    occ::handle<Geom2d_Curve> zob2dop, zob2dv;
    //    double tolreached;
    if (!ChFi3d_ComputeCurves(HBop,
                              HBs,
                              Pardeb,
                              Parfin,
                              zob3d,
                              zob2dop,
                              zob2dv,
                              tolapp3d,
                              tol2d,
                              tolreached))
      throw Standard_Failure("OneCorner : echec calcul intersection");

    Udeb = zob3d->FirstParameter();
    Ufin = zob3d->LastParameter();
    TopOpeBRepDS_Curve Zob(zob3d, tolreached);
    int                IZob = DStr.AddCurve(Zob);

    // it is not determined if the curve has an intersection with the sewing edge

    {
      Et = TopAbs::Reverse(TopAbs::Compose(OVtx, OArcprolv));
      int                                                Iop = DStr.AddShape(Fop);
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> InterFv =
        ChFi3d_FilCurveInDS(IZob, IShape, zob2dv, Et);
      DStr.ChangeShapeInterferences(IShape).Append(InterFv);
      // OVtx = TopAbs::Reverse(OVtx);
      //  Modified by skv - Thu Aug 21 11:55:58 2008 OCC20222 Begin
      //      Et = TopAbs::Reverse(TopAbs::Compose(OVtx,OArcprolbis));
      Et = TopAbs::Reverse(TopAbs::Compose(OVtx2, OArcprolbis));
      //  Modified by skv - Thu Aug 21 11:55:58 2008 OCC20222 End
      // OVtx = TopAbs::Reverse(OVtx);
      //      Et = TopAbs::Reverse(Et);
      occ::handle<TopOpeBRepDS_SurfaceCurveInterference> Interfop =
        ChFi3d_FilCurveInDS(IZob, Iop, zob2dop, Et);
      DStr.ChangeShapeInterferences(Iop).Append(Interfop);
      occ::handle<TopOpeBRepDS_CurvePointInterference> interfprol =
        ChFi3d_FilVertexInDS(TopAbs_FORWARD, IZob, IVtx, Udeb);
      DStr.ChangeCurveInterferences(IZob).Append(interfprol);
      int icc    = stripe->IndexPoint(isfirst, IFopArc);
      interfprol = ChFi3d_FilPointInDS(TopAbs_REVERSED, IZob, icc, Ufin);
      DStr.ChangeCurveInterferences(IZob).Append(interfprol);
    }
  }
  ChFi3d_EnlargeBox(DStr, stripe, Fd, box1, box2, isfirst);
  if (CV1.IsOnArc())
  {
    ChFi3d_EnlargeBox(CV1.Arc(), myEFMap(CV1.Arc()), CV1.ParameterOnArc(), box1);
  }
  if (CV2.IsOnArc())
  {
    ChFi3d_EnlargeBox(CV2.Arc(), myEFMap(CV2.Arc()), CV2.ParameterOnArc(), box2);
  }
  if (!CV1.IsVertex())
    ChFi3d_SetPointTolerance(DStr, box1, stripe->IndexPoint(isfirst, 1));
  if (!CV2.IsVertex())
    ChFi3d_SetPointTolerance(DStr, box2, stripe->IndexPoint(isfirst, 2));

#ifdef OCCT_DEBUG
  ChFi3d_ResultChron(ch, t_sameinter); // result perf condition if (same &&inter)
#endif
}
