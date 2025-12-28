// Created on: 1993-05-07
// Created by: Jacques GOUSSARD
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

// ----------------------------------------------------------------------
//-- lbr: Modifs importantes du 16-17 Nov 95
//--      - Chercher APointOnRstStillExist et OnDifferentRst
//--      On veut pouvoir creer un Vtx (A1 sur S1, A2    sur S2)
//--                         et le Vtx (A1 sur S1, A2bis sur S2)
//--      ce qui revient a distinguer un point deja pose sur une
//--      restriction de S avec un point pose sur une nouvelle
//--       restriction de S.
//--      - Pour rester coherent avec cette facon de faire,
//--      Chercher(Nbvtx++).

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_CurvIntSurf.hxx>
#include <IntPatch_HInterTool.hxx>
#include <IntPatch_PolyArc.hxx>
#include <IntPatch_PolyLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_RstInt.hxx>
#include <IntPatch_SearchPnt.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>

#include <ElCLib.hxx>

#define myInfinite 1.e15 // the same as was in Adaptor3d_TopolTool

static void Recadre(GeomAbs_SurfaceType                typeS1,
                    GeomAbs_SurfaceType                typeS2,
                    const occ::handle<IntPatch_WLine>& wlin,
                    int                                Param,
                    double&                            U1,
                    double&                            V1,
                    double&                            U2,
                    double&                            V2)
{
  int nbpnts = wlin->NbPnts();
  if (Param < 1)
    Param = 1;
  else if (Param > nbpnts)
    Param = nbpnts;
  double U1p, V1p, U2p, V2p;

  wlin->Point(Param).Parameters(U1p, V1p, U2p, V2p);
  switch (typeS1)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      while (U1 < (U1p - 1.5 * M_PI))
        U1 += M_PI + M_PI;
      while (U1 > (U1p + 1.5 * M_PI))
        U1 -= M_PI + M_PI;
      break;
    default:
      break;
  }
  if (typeS1 == GeomAbs_Torus)
  {
    while (V1 < (V1p - 1.5 * M_PI))
      V1 += M_PI + M_PI;
    while (V1 > (V1p + 1.5 * M_PI))
      V1 -= M_PI + M_PI;
  }

  switch (typeS2)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
      while (U2 < (U2p - 1.5 * M_PI))
        U2 += M_PI + M_PI;
      while (U2 > (U2p + 1.5 * M_PI))
        U2 -= M_PI + M_PI;
      break;
    default:
      break;
  }
  if (typeS2 == GeomAbs_Torus)
  {
    while (V2 < (V1p - 1.5 * M_PI))
      V2 += M_PI + M_PI;
    while (V2 > (V2p + 1.5 * M_PI))
      V2 -= M_PI + M_PI;
  }
}

const double Confusion = Precision::Confusion();

inline double Tol3d(const occ::handle<Adaptor3d_HVertex>&   vtx,
                    const occ::handle<Adaptor3d_TopolTool>& Domain,
                    const double                            tolDef = 0.)
{
  return (Domain->Has3d() ? Domain->Tol3d(vtx) : tolDef < Confusion ? Confusion : tolDef);
}

inline double Tol3d(const occ::handle<Adaptor2d_Curve2d>&   arc,
                    const occ::handle<Adaptor3d_TopolTool>& Domain,
                    const double                            tolDef = 0.)
{
  return (Domain->Has3d() ? Domain->Tol3d(arc) : tolDef < Confusion ? Confusion : tolDef);
}

static bool CoincideOnArc(const gp_Pnt&                           Ptsommet,
                          const occ::handle<Adaptor2d_Curve2d>&   A,
                          const occ::handle<Adaptor3d_Surface>&   Surf,
                          const double                            Toler,
                          const occ::handle<Adaptor3d_TopolTool>& Domain,
                          occ::handle<Adaptor3d_HVertex>&         Vtx)
{
  double distmin = RealLast();
  double tolarc  = std::max(Toler, Tol3d(A, Domain));

  Domain->Initialize(A);
  Domain->InitVertexIterator();
  while (Domain->MoreVertex())
  {
    occ::handle<Adaptor3d_HVertex> vtx1  = Domain->Vertex();
    double                         prm   = IntPatch_HInterTool::Parameter(vtx1, A);
    gp_Pnt2d                       p2d   = A->Value(prm);
    gp_Pnt                         point = Surf->Value(p2d.X(), p2d.Y());
    const double                   dist  = point.Distance(Ptsommet);
    double                         tol   = std::max(tolarc, Tol3d(vtx1, Domain));

    if (dist <= tol && dist <= distmin)
    { // the best coincidence
      distmin = dist;
      Vtx     = vtx1;
    }
    Domain->NextVertex();
  }
  return distmin < RealLast();
}

static void VerifyTgline(const occ::handle<IntPatch_WLine>& wlin,
                         const int                          param,
                         const double                       Tol,
                         gp_Vec&                            Tgl)
{

  if (std::abs(Tgl.X()) < Tol && std::abs(Tgl.Y()) < Tol && std::abs(Tgl.Z()) < Tol)
  {
    //-- On construit une tangente plus grande
    //-- (Eviter des points tres proches ds Walking)
    int  i, n, nbpt = wlin->NbPnts();
    bool forward = (nbpt - param) >= (param - 1);
    for (n = 2; n > 0; n--, forward = !forward)
    {
      if (forward)
      {
        for (i = param + 1; i <= nbpt; i++)
        {
          gp_Vec T(wlin->Point(param).Value(), wlin->Point(i).Value());
          if (std::abs(T.X()) >= Tol || std::abs(T.Y()) >= Tol || std::abs(T.Z()) >= Tol)
          {
            Tgl = T;
            return;
          }
        }
      }
      else
      {
        for (i = param - 1; i >= 1; i--)
        {
          gp_Vec T(wlin->Point(i).Value(), wlin->Point(param).Value());
          if (std::abs(T.X()) >= Tol || std::abs(T.Y()) >= Tol || std::abs(T.Z()) >= Tol)
          {
            Tgl = T;
            return;
          }
        }
      }
    }
  }
}

static void GetLinePoint2d(const occ::handle<IntPatch_Line>& L,
                           const double                      param,
                           const bool                        OnFirst,
                           double&                           U,
                           double&                           V)
{
  occ::handle<IntPatch_WLine> wlin = occ::down_cast<IntPatch_WLine>(L);
  occ::handle<IntPatch_RLine> rlin = occ::down_cast<IntPatch_RLine>(L);
  IntPatch_IType              typL = L->ArcType();
  int Nbptlin                      = (typL == IntPatch_Walking ? wlin->NbPnts() : rlin->NbPnts());

  double par   = std::trunc(param);
  int    Irang = int(par);
  if (Irang == Nbptlin)
  {
    Irang--;
    par = 1.0;
  }
  else
    par = std::abs(param - par);

  double us1, vs1, us2, vs2;
  if (typL == IntPatch_Walking)
  {
    if (OnFirst)
    {
      wlin->Point(Irang).ParametersOnS1(us1, vs1);
      wlin->Point(Irang + 1).ParametersOnS1(us2, vs2);
    }
    else
    {
      wlin->Point(Irang).ParametersOnS2(us1, vs1);
      wlin->Point(Irang + 1).ParametersOnS2(us2, vs2);
    }
  }
  else
  {
    if (OnFirst)
    {
      rlin->Point(Irang).ParametersOnS1(us1, vs1);
      rlin->Point(Irang + 1).ParametersOnS1(us2, vs2);
    }
    else
    {
      rlin->Point(Irang).ParametersOnS2(us1, vs1);
      rlin->Point(Irang + 1).ParametersOnS2(us2, vs2);
    }
  }

  U = (1. - par) * us1 + par * us2;
  V = (1. - par) * vs1 + par * vs2;
}

static bool FindParameter(const occ::handle<IntPatch_Line>&     L,
                          const occ::handle<Adaptor3d_Surface>& OtherSurf,
                          const double                          Tol,
                          const gp_Pnt&                         Ptsom,
                          const gp_Pnt2d&                       Ptsom2d,
                          double&                               Param,
                          gp_Vec&                               Tgl,
                          const int                             ParamApproche,
                          const bool                            OnFirst)

{
  // MSV 28.03.2002: find parameter on WLine in 2d space

  // Si la ligne est de type restriction, c est qu on provient necessairement
  //  du cas implicite/parametree, et que la ligne est restriction de
  //  la surface bi-parametree. Cette surface bi-parametree est necessairement
  //  passee en argument a PutVertexOnline dans la variable OtherSurf.

  // Dans le cas d une ligne de cheminement, il faudrait voir la projection
  // et le calcul de la tangente.

  // clang-format off
  occ::handle<IntPatch_RLine> rlin (Handle(IntPatch_RLine)::DownCast (L)); //-- aucune verification n est
  // clang-format on
  occ::handle<IntPatch_WLine> wlin(occ::down_cast<IntPatch_WLine>(L)); //-- faite au cast.
  gp_Pnt                      ptbid;
  gp_Vec                      d1u, d1v;
  gp_Pnt2d                    p2d;
  gp_Vec2d                    d2d;
  double                      Tol2 = Tol * Tol;
  IntPatch_IType              typL = L->ArcType();
  Tgl.SetCoord(0.0, 0.0, 0.0);

  if (typL == IntPatch_Restriction)
  {
    if (!OnFirst && rlin->IsArcOnS1())
    {
      IntPatch_HInterTool::Project(rlin->ArcOnS1(), Ptsom2d, Param, p2d);
      rlin->ArcOnS1()->D1(Param, p2d, d2d);
    }
    else if (OnFirst && rlin->IsArcOnS2())
    {
      IntPatch_HInterTool::Project(rlin->ArcOnS2(), Ptsom2d, Param, p2d);
      rlin->ArcOnS2()->D1(Param, p2d, d2d);
    }
    else
    {
      return (false);
    }
    OtherSurf->D1(p2d.X(), p2d.Y(), ptbid, d1u, d1v);
    if (ptbid.SquareDistance(Ptsom) > Tol2)
    {
      return false;
    }
    Tgl.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);
    return (true);
  }

  else if (typL == IntPatch_Walking)
  {
    int    i, is, nbpt = wlin->NbPnts();
    double norm1, norm2;
    int    ParamSearchInf = 1;
    int    ParamSearchSup = nbpt;

    if ((ParamApproche - 2) > ParamSearchInf)
    {
      ParamSearchInf = ParamApproche - 2;
    }
    if ((ParamApproche + 2) < ParamSearchSup)
    {
      ParamSearchSup = ParamApproche + 2;
    }

    int inf[3], sup[3];
    // first search inside close bounding around ParamApproche;
    // then search to the nearest end of line;
    // and then search to the farthest end of line.
    inf[0] = ParamSearchInf;
    sup[0] = ParamSearchSup;
    if (ParamSearchInf - 1 < nbpt - ParamSearchSup)
    {
      inf[1] = 1;
      sup[1] = ParamSearchInf;
      inf[2] = ParamSearchSup;
      sup[2] = nbpt;
    }
    else
    {
      inf[1] = ParamSearchSup;
      sup[1] = nbpt;
      inf[2] = 1;
      sup[2] = ParamSearchInf;
    }

    bool found = false;
    for (is = 0; is < 3 && !found; is++)
    {
      gp_Vec v1, v2;
      gp_Pnt p1, p2;
      p1             = wlin->Point(inf[is]).Value();
      v1             = gp_Vec(Ptsom, p1);
      norm1          = v1.SquareMagnitude();
      double normmin = Tol2;
      int    ibest   = 0;
      if (norm1 <= normmin)
      {
        normmin = norm1;
        ibest   = inf[is];
      }
      for (i = inf[is] + 1; i <= sup[is] && !found; i++)
      {
        p2    = wlin->Point(i).Value();
        v2    = gp_Vec(Ptsom, p2);
        norm2 = v2.SquareMagnitude();
        if (v1.Dot(v2) < 0.)
        {
          Param = (double)(i - 1) + 1. / (1. + std::sqrt(norm2 / norm1));
          Tgl   = gp_Vec(p1, p2);
          found = true;
        }
        else if (norm2 < normmin)
        {
          normmin = norm2;
          ibest   = i;
        }
        v1    = v2;
        p1    = p2;
        norm1 = norm2;
      }
      if (!found && ibest)
      {
        Param = (double)ibest;
        found = true;
      }
    }
    if (found)
      return true;
  }
  else
  {
    throw Standard_DomainError();
  }
  return false;
}

inline bool ArePnt2dEqual(const gp_Pnt2d& p1,
                          const gp_Pnt2d& p2,
                          const double    tolU,
                          const double    tolV)
{
  return std::abs(p1.X() - p2.X()) < tolU && std::abs(p1.Y() - p2.Y()) < tolV;
}

//=================================================================================================

void IntPatch_RstInt::PutVertexOnLine(const occ::handle<IntPatch_Line>&       L,
                                      const occ::handle<Adaptor3d_Surface>&   Surf,
                                      const occ::handle<Adaptor3d_TopolTool>& Domain,
                                      const occ::handle<Adaptor3d_Surface>&   OtherSurf,
                                      const bool                              OnFirst,
                                      const double                            Tol)
{

  // Domain est le domaine de restriction de la surface Surf.
  // On intersectera un arc de Surf avec la surface OtherSurf.
  // Si OnFirst = True, c est que la surface Surf correspond a la 1ere
  // surface donnee aux algo d intersection.

  IntPatch_SearchPnt Commun;

  double                         U, V, W;
  double                         U1, V1, U2 = 0., V2 = 0.;
  double                         paramarc = 0., paramline = 0.;
  int                            i, j, k;
  NCollection_Sequence<gp_Pnt>   locpt;
  NCollection_Sequence<gp_Pnt2d> locpt2;
  // clang-format off
  occ::handle<IntPatch_RLine> rlin (Handle(IntPatch_RLine)::DownCast (L)); //-- aucune verification n est
  // clang-format on
  occ::handle<IntPatch_WLine> wlin(occ::down_cast<IntPatch_WLine>(L)); //-- faite au cast.
  int                         Nbvtx   = 0;
  double                      tolPLin = Surf->UResolution(Precision::Confusion());
  tolPLin = std::max(tolPLin, Surf->VResolution(Precision::Confusion()));
  tolPLin = std::min(tolPLin, Precision::Confusion());
  IntPatch_PolyLine PLin(tolPLin);

  double PFirst, PLast;
  int    NbEchant;
  gp_Pnt ptsommet, ptbid;
  gp_Vec tgline, tgrst, d1u, d1v, normsurf;

  gp_Pnt2d p2d;
  gp_Vec2d d2d;

  IntPatch_Point                 Sommet, ptline;
  occ::handle<Adaptor3d_HVertex> vtxarc, vtxline;
  occ::handle<Adaptor2d_Curve2d> arc;
  bool                           VtxOnArc, duplicate, found;
  IntSurf_Transition             transarc, transline;

  IntPatch_IType typL = L->ArcType();
  if (typL == IntPatch_Walking)
  {
    Nbvtx = wlin->NbVertex();
    PLin.SetWLine(OnFirst, wlin);
  }
  else if (typL == IntPatch_Restriction)
  {
    Nbvtx = rlin->NbVertex();
    PLin.SetRLine(OnFirst, rlin);
  }
  else
  {
    throw Standard_DomainError();
  }
  if (!Domain->Has3d())
    // don't use computed deflection in the mode of pure geometric intersection
    PLin.ResetError();

  const bool SurfaceIsUClosed  = Surf->IsUClosed();
  const bool SurfaceIsVClosed  = Surf->IsVClosed();
  const bool OSurfaceIsUClosed = OtherSurf->IsUClosed();
  const bool OSurfaceIsVClosed = OtherSurf->IsVClosed();
  const bool possiblyClosed =
    (SurfaceIsUClosed || SurfaceIsVClosed || OSurfaceIsUClosed || OSurfaceIsVClosed);
  double tolUClosed = 0., tolVClosed = 0., tolOUClosed = 0., tolOVClosed = 0.;
  if (possiblyClosed)
  {
    if (SurfaceIsUClosed)
      tolUClosed = (Surf->LastUParameter() - Surf->FirstUParameter()) * 0.01;
    if (SurfaceIsVClosed)
      tolVClosed = (Surf->LastVParameter() - Surf->FirstVParameter()) * 0.01;
    if (OSurfaceIsUClosed)
      tolOUClosed = (OtherSurf->LastUParameter() - OtherSurf->FirstUParameter()) * 0.01;
    if (OSurfaceIsVClosed)
      tolOVClosed = (OtherSurf->LastVParameter() - OtherSurf->FirstVParameter()) * 0.01;
  }

  //------------------------------------------------------------------------
  //-- On traite le cas ou la surface est periodique                      --
  //-- il faut dans ce cas considerer la restriction                      --
  //--                                la restriction decalee de +-2PI     --
  //------------------------------------------------------------------------
  const occ::handle<Adaptor3d_Surface>& Surf1               = (OnFirst ? Surf : OtherSurf);
  const occ::handle<Adaptor3d_Surface>& Surf2               = (OnFirst ? OtherSurf : Surf);
  GeomAbs_SurfaceType                   TypeS1              = Surf1->GetType();
  GeomAbs_SurfaceType                   TypeS2              = Surf2->GetType();
  bool                                  SurfaceIsPeriodic   = false;
  bool                                  SurfaceIsBiPeriodic = false;
  GeomAbs_SurfaceType                   surfacetype         = (OnFirst ? TypeS1 : TypeS2);
  if (surfacetype == GeomAbs_Cylinder || surfacetype == GeomAbs_Cone || surfacetype == GeomAbs_Torus
      || surfacetype == GeomAbs_Sphere)
  {
    SurfaceIsPeriodic = true;
    if (surfacetype == GeomAbs_Torus)
    {
      SurfaceIsBiPeriodic = true;
    }
  }

  int NumeroEdge = 0;
  Domain->Init();
  while (Domain->More())
  {
    NumeroEdge++;
    arc = Domain->Value();

    // MSV Oct 15, 2001: use tolerance of this edge if possible
    double edgeTol = Tol3d(arc, Domain, Tol);

    IntPatch_HInterTool::Bounds(arc, PFirst, PLast);
    if (Precision::IsNegativeInfinite(PFirst))
      PFirst = -myInfinite;
    if (Precision::IsPositiveInfinite(PLast))
      PLast = myInfinite;
    // if (Precision::IsNegativeInfinite(PFirst) ||
    //  Precision::IsPositiveInfinite(PLast)) {
    //  //-- std::cout<<" IntPatch_RstInt::PutVertexOnLine  ---> Restrictions Infinies
    //  :"<<std::endl; return;
    //}

    gp_Pnt2d p2dFirst, p2dLast;
    Domain->Initialize(arc);
    for (Domain->InitVertexIterator(); Domain->MoreVertex(); Domain->NextVertex())
    {
      occ::handle<Adaptor3d_HVertex> vtx = Domain->Vertex();
      double                         prm = IntPatch_HInterTool::Parameter(vtx, arc);
      if (std::abs(prm - PFirst) < Precision::PConfusion())
      {
        arc->D0(PFirst, p2dFirst);
      }
      else if (std::abs(prm - PLast) < Precision::PConfusion())
      {
        arc->D0(PLast, p2dLast);
      }
    }

    Bnd_Box2d BPLin   = PLin.Bounding();
    double    OffsetV = 0.0;
    double    OffsetU = 0.0;

    switch (arc->GetType())
    {
      case GeomAbs_Line: {
        NbEchant = 10;

        double aXmin, aYmin, aXmax, aYmax;
        BPLin.Get(aXmin, aYmin, aXmax, aYmax);
        gp_Lin2d        aLin = arc->Line();
        const gp_Pnt2d& aLoc = aLin.Location();
        const gp_Dir2d& aDir = aLin.Direction();

        // Here, we consider rectangular axis-aligned domain only.
        const bool isAlongU = (std::abs(aDir.X()) > std::abs(aDir.Y()));

        if (SurfaceIsPeriodic && !isAlongU)
        {
          // Shift along U-direction
          const double aNewLocation = ElCLib::InPeriod(aLoc.X(), aXmin, aXmin + M_PI + M_PI);
          OffsetU                   = aNewLocation - aLoc.X();
        }
        else if (SurfaceIsBiPeriodic && isAlongU)
        {
          // Shift along V-direction
          const double aNewLocation = ElCLib::InPeriod(aLoc.Y(), aYmin, aYmin + M_PI + M_PI);
          OffsetV                   = aNewLocation - aLoc.Y();
        }
      }
      break;
      case GeomAbs_BezierCurve: {
        NbEchant = (3 + arc->NbPoles());
        if (NbEchant < 10)
          NbEchant = 10;
        else if (NbEchant > 50)
          NbEchant = 50;
      }
      break;
      case GeomAbs_BSplineCurve: {
        // szv:const double nbs = (arc->NbKnots() * arc->Degree())*(arc->LastParameter() -
        // arc->FirstParameter())/(PLast-PFirst);
        const double nbs = (arc->NbKnots() * arc->Degree()) * (PLast - PFirst)
                           / (arc->LastParameter() - arc->FirstParameter());
        NbEchant = (nbs < 2.0 ? 2 : (int)nbs);
        if (NbEchant < 10)
          NbEchant = 10;
        else if (NbEchant > 50)
          NbEchant = 50;
      }
      break;
      default: {
        NbEchant = 25;
      }
    }

    if (SurfaceIsPeriodic)
    {
      double xmin, ymin, xmax, ymax, g;
      BPLin.Get(xmin, ymin, xmax, ymax);
      g = BPLin.GetGap();
      BPLin.SetVoid();
      BPLin.Update(xmin - M_PI - M_PI, ymin, xmax + M_PI + M_PI, ymax);
      BPLin.SetGap(g);
    }
    if (SurfaceIsBiPeriodic)
    {
      double xmin, ymin, xmax, ymax, g;
      BPLin.Get(xmin, ymin, xmax, ymax);
      g = BPLin.GetGap();
      BPLin.SetVoid();
      BPLin.Update(xmin, ymin - M_PI - M_PI, xmax, ymax + M_PI + M_PI);
      BPLin.SetGap(g);
    }

    IntPatch_PolyArc Brise(arc, NbEchant, PFirst, PLast, BPLin);

    int          IndiceOffsetBiPeriodic = 0;
    int          IndiceOffsetPeriodic   = 0;
    const double aRefOU = OffsetU, aRefOV = OffsetV;

    do
    {
      if (IndiceOffsetBiPeriodic == 1)
        OffsetV = aRefOV - M_PI - M_PI;
      else if (IndiceOffsetBiPeriodic == 2)
        OffsetV = aRefOV + M_PI + M_PI;

      do
      {
        if (IndiceOffsetPeriodic == 1)
          OffsetU = aRefOU - M_PI - M_PI;
        else if (IndiceOffsetPeriodic == 2)
          OffsetU = aRefOU + M_PI + M_PI;

        Brise.SetOffset(OffsetU, OffsetV);

        static int debug_polygon2d = 0;
        if (debug_polygon2d)
        {
          std::cout << " ***** Numero Restriction : " << NumeroEdge << " *****" << std::endl;
          PLin.Dump();
          Brise.Dump();
        }

        Commun.Perform(PLin, Brise);
        locpt.Clear();
        locpt2.Clear();

        // We do not need in putting vertex into tangent zone(s).
        // Therefore, only section points are interested by us.
        // Boundary of WLine (its first/last points) will be
        // marked by some vertex later. See bug #29494.
        const int aNbSectionPts = Commun.NbSectionPoints();
        for (i = 1; i <= aNbSectionPts; i++)
        {
          const double aW1 = Commun.PntValue(i).ParamOnFirst(),
                       aW2 = Commun.PntValue(i).ParamOnSecond();

          int nbTreated = 0;
          GetLinePoint2d(L, aW1 + 1, !OnFirst, U, V);

          double par   = std::trunc(aW2);
          int    Irang = int(par) + 1;
          if (Irang == Brise.NbPoints())
          {
            Irang--;
            par = 1.;
          }
          else
          {
            par = std::abs(aW2 - par);
          }

          W = (1. - par) * Brise.Parameter(Irang) + par * Brise.Parameter(Irang + 1);

          //------------------------------------------------------------------------
          //-- On a trouve un point 2d approche Ua,Va  intersection de la ligne
          //-- de cheminement et de la restriction.
          //--
          //-- On injecte ce point ds les intersections Courbe-Surface
          //--
          IntPatch_CSFunction thefunc(OtherSurf, arc, Surf);
          // MSV: extend UV bounds to not miss solution near the boundary
          const double         margCoef = 0.004;
          bool                 refined  = false;
          IntPatch_CurvIntSurf IntCS(U, V, W, thefunc, edgeTol, margCoef);
          if (IntCS.IsDone() && !IntCS.IsEmpty())
          {
            ptsommet = IntCS.Point();
            IntCS.ParameterOnSurface(U2, V2);
            gp_Pnt anOldPnt, aNewPnt;
            OtherSurf->D0(U, V, anOldPnt);
            OtherSurf->D0(U2, V2, aNewPnt);
            // if (anOldPnt.SquareDistance(aNewPnt) < Precision::SquareConfusion())
            double aTolConf = std::max(Precision::Confusion(), edgeTol);

            if (anOldPnt.SquareDistance(aNewPnt) < aTolConf * aTolConf)
            {
              U2 = U;
              V2 = V;
            }
            paramarc = IntCS.ParameterOnCurve();
            refined  = true;
          }

          if (refined)
          {
            duplicate = false;
            for (j = 1; j <= locpt.Length(); j++)
            {
              if (ptsommet.Distance(locpt(j)) <= edgeTol)
              {
                if (possiblyClosed)
                {
                  locpt2(j).Coord(U, V);
                  if ((OSurfaceIsUClosed && std::abs(U - U2) > tolOUClosed)
                      || (OSurfaceIsVClosed && std::abs(V - V2) > tolOVClosed))
                    continue;
                }
                duplicate = true;
                break;
              }
            }

            if (!duplicate)
            {
              int ParamApproxOnLine = int(aW1) + 1;

              arc->D1(paramarc, p2d, d2d);
              U1 = p2d.X();
              V1 = p2d.Y();
              if (typL == IntPatch_Walking && SurfaceIsPeriodic)
              {
                if (OnFirst)
                  Recadre(TypeS1, TypeS2, wlin, ParamApproxOnLine, U1, V1, U2, V2);
                else
                  Recadre(TypeS1, TypeS2, wlin, ParamApproxOnLine, U2, V2, U1, V1);
              }
              locpt.Append(ptsommet);
              locpt2.Append(gp_Pnt2d(U2, V2));

              found = FindParameter(L,
                                    OtherSurf,
                                    edgeTol,
                                    ptsommet,
                                    gp_Pnt2d(U2, V2),
                                    paramline,
                                    tgline,
                                    ParamApproxOnLine,
                                    OnFirst);

              if (typL == IntPatch_Walking && found && possiblyClosed)
              {
                // check in 2d
                if (SurfaceIsUClosed || SurfaceIsVClosed)
                {
                  GetLinePoint2d(L, paramline, OnFirst, U, V);
                  if ((SurfaceIsUClosed && std::abs(U - U1) > tolUClosed)
                      || (SurfaceIsVClosed && std::abs(V - V1) > tolVClosed))
                    found = false;
                }
                if (found && (OSurfaceIsUClosed || OSurfaceIsVClosed))
                {
                  GetLinePoint2d(L, paramline, !OnFirst, U, V);
                  if ((OSurfaceIsUClosed && std::abs(U - U2) > tolOUClosed)
                      || (OSurfaceIsVClosed && std::abs(V - V2) > tolOVClosed))
                    found = false;
                }
              }
              if (!found)
              {
                continue;
              }

              VtxOnArc = CoincideOnArc(ptsommet, arc, Surf, edgeTol, Domain, vtxarc);
              double vtxTol;
              if (VtxOnArc)
              {
                vtxTol = Tol3d(vtxarc, Domain);
                if (edgeTol > vtxTol)
                  vtxTol = edgeTol;
              }
              else
                vtxTol = edgeTol;

              //-- It is necessary to test that the point does not already exist
              //--   - It can be already a point on arc
              //--        BUT on a different arc
              // MSV 27.03.2002: find the nearest point; add check in 2d
              int    ivtx = 0;
              double dmin = RealLast();
              for (j = 1; j <= Nbvtx; j++)
              {
                const IntPatch_Point& Rptline =
                  (typL == IntPatch_Walking ? wlin->Vertex(j) : rlin->Vertex(j));
                bool APointOnRstStillExist =
                  ((OnFirst && Rptline.IsOnDomS1() && Rptline.ArcOnS1() == arc)
                   || (!OnFirst && Rptline.IsOnDomS2() && Rptline.ArcOnS2() == arc));
                if (!APointOnRstStillExist)
                {
                  if (possiblyClosed)
                  {
                    if (SurfaceIsUClosed || SurfaceIsVClosed)
                    {
                      if (OnFirst)
                        Rptline.ParametersOnS1(U, V);
                      else
                        Rptline.ParametersOnS2(U, V);
                      if ((SurfaceIsUClosed && std::abs(U - U1) > tolUClosed)
                          || (SurfaceIsVClosed && std::abs(V - V1) > tolVClosed))
                        continue;
                    }
                    if (OSurfaceIsUClosed || OSurfaceIsVClosed)
                    {
                      if (OnFirst)
                        Rptline.ParametersOnS2(U, V);
                      else
                        Rptline.ParametersOnS1(U, V);
                      if ((OSurfaceIsUClosed && std::abs(U - U2) > tolOUClosed)
                          || (OSurfaceIsVClosed && std::abs(V - V2) > tolOVClosed))
                        continue;
                    }
                  }
                  double dist = ptsommet.Distance(Rptline.Value());
                  double dt   = std::max(vtxTol, Rptline.Tolerance());
                  if (dist < dmin)
                  {
                    if (dist <= dt)
                    {
                      ptline = Rptline;
                      ivtx   = j;
                      if (surfacetype == GeomAbs_Cone)
                      {
                        ivtx = 0;
                      }
                    }
                    else
                    {
                      // cancel previous solution because this point is better
                      // but its tolerance is not large enough
                      ivtx = 0;
                    }
                    dmin = dist;
                  }
                }
              }
              if (ivtx)
              {
                if (ptline.Tolerance() > vtxTol)
                {
                  vtxTol = ptline.Tolerance();
                  if (!VtxOnArc)
                  {
                    // now we should repeat attempt to coincide on a bound of arc
                    VtxOnArc = CoincideOnArc(ptsommet, arc, Surf, vtxTol, Domain, vtxarc);
                    if (VtxOnArc)
                    {
                      double tol = Tol3d(vtxarc, Domain);
                      if (tol > vtxTol)
                        vtxTol = tol;
                    }
                  }
                }
              }

              if (typL == IntPatch_Walking)
                VerifyTgline(wlin, (int)paramline, edgeTol, tgline);

              Surf->D1(U1, V1, ptbid, d1u, d1v);
              tgrst.SetLinearForm(d2d.X(), d1u, d2d.Y(), d1v);

              normsurf = d1u.Crossed(d1v);
              if (normsurf.Magnitude() < gp::Resolution())
              {
                transline.SetValue(true, IntSurf_Undecided);
                transarc.SetValue(true, IntSurf_Undecided);
              }
              else
                IntSurf::MakeTransition(tgline, tgrst, normsurf, transline, transarc);

              nbTreated++;
              if (!ivtx)
              {
                Sommet.SetValue(ptsommet, vtxTol, false); // pour tangence
                if (OnFirst)
                  Sommet.SetParameters(U1, V1, U2, V2);
                else
                  Sommet.SetParameters(U2, V2, U1, V1);

                if (VtxOnArc)
                  Sommet.SetVertex(OnFirst, vtxarc);

                //---------------------------------------------------------
                //-- lbr : On remplace le point d indice paramline sur la -
                //-- ligne par le vertex .                                -
                //---------------------------------------------------------
                Sommet.SetParameter(paramline); // sur ligne d intersection
                Sommet.SetArc(OnFirst, arc, paramarc, transline, transarc);

                if (typL == IntPatch_Walking)
                {
                  wlin->AddVertex(Sommet);
                  Nbvtx++;
                }
                else
                {
                  rlin->AddVertex(Sommet);
                  Nbvtx++;
                }
              }
              else
              {
                // CAS DE FIGURE : en appelant s1 la surf sur laquelle on
                //   connait les pts sur restriction, et s2 celle sur laquelle
                //   on les cherche. Le point trouve verifie necessairement
                //   IsOnDomS1 = True.
                //  Pas vtxS1, pas vtxS2 :
                //   on recupere le point et on applique SetArcOnS2 et
                //   eventuellement SetVertexOnS2. Si on a deja IsOnDomS2,
                //   on considere que le point est deja traite, mais ne devrait
                //   pas se produire.
                //  vtxS1, pas vtxS2     :
                //   si pas IsOnDomS2 : pour chaque occurrence, faire SetArcOnS2,
                //   et eventuellement SetVertexOnS2.
                //   si IsOnDomS2 : impossible, on doit avoir IsVtxOnS2.
                //  vtxS1,vtxS2          :
                //   on doit avoir VtxOnArc = True. On duplique chaque occurrence
                //   "sur S1" du point en changeant ArcOnS2.
                //  pas vtxS1, vtxS2     :
                //   on doit avoir VtxOnArc = True. On duplique le point sur S1
                //   en changeant ArcOnS2.
                bool OnDifferentRst =
                  ((OnFirst && ptline.IsOnDomS1() && ptline.ArcOnS1() != arc)
                   || (!OnFirst && ptline.IsOnDomS2() && ptline.ArcOnS2() != arc));
                ptline.SetTolerance(vtxTol);
                if ((!ptline.IsVertexOnS1() && OnFirst) || (!ptline.IsVertexOnS2() && !OnFirst)
                    || (OnDifferentRst))
                {
                  if ((!ptline.IsOnDomS2() && !OnFirst) || (!ptline.IsOnDomS1() && OnFirst)
                      || (OnDifferentRst))
                  {
                    ptline.SetArc(OnFirst, arc, paramarc, transline, transarc);
                    // ptline.SetParameter(paramline); //-- rajout lbr le 20 nov 97
                    if (VtxOnArc)
                      ptline.SetVertex(OnFirst, vtxarc);
                    if (typL == IntPatch_Walking)
                    {
                      if (OnDifferentRst)
                      {
                        wlin->AddVertex(ptline);
                        Nbvtx++;
                      }
                      else
                      {
                        wlin->Replace(ivtx, ptline);
                      }
                    }
                    else
                    {
                      if (OnDifferentRst)
                      {
                        rlin->AddVertex(ptline);
                        Nbvtx++;
                      }
                      else
                      {
                        rlin->Replace(ivtx, ptline);
                      }
                    }
                  }
                  else if ((OnFirst && ptline.IsVertexOnS2())
                           || (!OnFirst && ptline.IsVertexOnS1()))
                  {
                    Sommet = ptline;
                    Sommet.SetArc(OnFirst, arc, paramarc, transline, transarc);
                    if (VtxOnArc)
                      Sommet.SetVertex(OnFirst, vtxarc);
                    if (typL == IntPatch_Walking)
                    {
                      wlin->AddVertex(Sommet);
                      Nbvtx++;
                    }
                    else
                    {
                      rlin->AddVertex(Sommet);
                      Nbvtx++;
                    }
                  }
                  else
                  {
                    //-- std::cout << "pb dans RstInt  Type 1 " << std::endl;
                  }
                }
                else
                {
                  occ::handle<Adaptor3d_HVertex> vtxref =
                    (OnFirst) ? (ptline.VertexOnS1()) : (ptline.VertexOnS2());
                  if ((OnFirst && !ptline.IsOnDomS2()) || (!OnFirst && !ptline.IsOnDomS1()))
                  {
                    ptline.SetArc(OnFirst, arc, paramarc, transline, transarc);
                    if (VtxOnArc)
                      ptline.SetVertex(OnFirst, vtxarc);
                    if (typL == IntPatch_Walking)
                    {
                      wlin->Replace(ivtx, ptline);
                    }
                    else
                    {
                      rlin->Replace(ivtx, ptline);
                    }

                    for (k = 1; k <= Nbvtx; k++)
                      if (k != ivtx)
                      {
                        if (typL == IntPatch_Walking)
                        {
                          ptline = wlin->Vertex(k);
                        }
                        else
                        {
                          ptline = rlin->Vertex(k);
                        }
                        if ((OnFirst && ptline.IsVertexOnS1())
                            || (!OnFirst && ptline.IsVertexOnS2()))
                        {
                          if (Domain->Identical(vtxref,
                                                (OnFirst) ? (ptline.VertexOnS1())
                                                          : (ptline.VertexOnS2())))
                          {
                            if (ptline.Tolerance() < vtxTol)
                              ptline.SetTolerance(vtxTol);
                            ptline.SetArc(OnFirst, arc, paramarc, transline, transarc);
                            if (VtxOnArc)
                              ptline.SetVertex(OnFirst, vtxarc);
                            if (typL == IntPatch_Walking)
                            {
                              wlin->Replace(k, ptline);
                            }
                            else
                            {
                              rlin->Replace(k, ptline);
                            }
                          }
                        }
                      }
                  }
                  else if ((OnFirst && ptline.IsVertexOnS2())
                           || (!OnFirst && ptline.IsVertexOnS1()))
                  {
                    //                on doit avoir vtxons2 = vtxarc... pas de verif...
                    Sommet = ptline;
                    Sommet.SetArc(OnFirst, arc, paramarc, transline, transarc);
                    if (typL == IntPatch_Walking)
                    {
                      wlin->AddVertex(Sommet);
                      Nbvtx++;
                    }
                    else
                    {
                      rlin->AddVertex(Sommet);
                      Nbvtx++;
                    }
                    for (k = 1; k <= Nbvtx; k++)
                      if (k != ivtx)
                      {
                        if (typL == IntPatch_Walking)
                        {
                          ptline = wlin->Vertex(k);
                        }
                        else
                        {
                          ptline = rlin->Vertex(k);
                        }
                        if ((OnFirst && ptline.IsVertexOnS1())
                            || (!OnFirst && ptline.IsVertexOnS2()))
                        {
                          if (Domain->Identical(vtxref,
                                                (OnFirst) ? (ptline.VertexOnS1())
                                                          : (ptline.VertexOnS2())))
                          {
                            if (ptline.Tolerance() < vtxTol)
                              ptline.SetTolerance(vtxTol);
                            Sommet = ptline;
                            Sommet.SetArc(OnFirst, arc, paramarc, transline, transarc);
                            if (typL == IntPatch_Walking)
                            {
                              wlin->Replace(k, ptline);
                              wlin->AddVertex(Sommet);
                              Nbvtx++;
                            }
                            else
                            {
                              rlin->Replace(k, ptline);
                              rlin->AddVertex(Sommet);
                              Nbvtx++;
                            }
                          }
                        }
                      }
                  }
                  else
                  {
                    //-- std::cout << "pb dans RstInt  Type 2 " << std::endl;
                  }
                }
              }
            }
          }
          if (nbTreated == 2 && typL == IntPatch_Walking)
          {
            // We processed a tangent zone, and both ends have been treated.
            // So mark WLine as having arc
            if (OnFirst)
              wlin->SetArcOnS1(arc);
            else
              wlin->SetArcOnS2(arc);
          }
        }

        IndiceOffsetPeriodic++;
      } while (SurfaceIsPeriodic && IndiceOffsetPeriodic <= 2);

      IndiceOffsetBiPeriodic++;
    } while (SurfaceIsBiPeriodic && IndiceOffsetBiPeriodic <= 2);
    Domain->Next();
  }

  //--------------------------------------------------------------------------------
  //-- On reprend la ligne et on recale les parametres des vertex.
  //--
  if (typL == IntPatch_Walking)
  {
    double pu1, pv1, pu2, pv2;
    pu1 = pv1 = pu2 = pv2 = 0.0;
    switch (TypeS1)
    {
      case GeomAbs_Cylinder:
      case GeomAbs_Cone:
      case GeomAbs_Sphere:
        pu1 = M_PI + M_PI;
        break;
      case GeomAbs_Torus:
        pu1 = pv1 = M_PI + M_PI;
        break;
      default: {
        if (Surf1->IsUPeriodic())
        {
          pu1 = Surf1->UPeriod();
        }
        else if (Surf1->IsUClosed())
        {
          pu1 = Surf1->LastUParameter() - Surf1->FirstUParameter();
          // std::cout<<" UClosed1 "<<pu1<<std::endl;
        }
        if (Surf1->IsVPeriodic())
        {
          pv1 = Surf1->VPeriod();
        }
        else if (Surf1->IsVClosed())
        {
          pv1 = Surf1->LastVParameter() - Surf1->FirstVParameter();
          // std::cout<<" VClosed1 "<<pv1<<std::endl;
        }

        break;
      }
    }

    switch (TypeS2)
    {
      case GeomAbs_Cylinder:
      case GeomAbs_Cone:
      case GeomAbs_Sphere:

        pu2 = M_PI + M_PI;
        break;
      case GeomAbs_Torus:
        pu2 = pv2 = M_PI + M_PI;
        break;
      default: {
        if (Surf2->IsUPeriodic())
        {
          pu2 = Surf2->UPeriod();
        }
        else if (Surf2->IsUClosed())
        {
          pu2 = Surf2->LastUParameter() - Surf2->FirstUParameter();
          // std::cout<<" UClosed2 "<<pu2<<std::endl;
        }

        if (Surf2->IsVPeriodic())
        {
          pv2 = Surf2->VPeriod();
        }
        else if (Surf2->IsVClosed())
        {
          pv2 = Surf2->LastVParameter() - Surf2->FirstVParameter();
          // std::cout<<" VClosed2 "<<pv2<<std::endl;
        }

        break;
      }
    }

    /*
        if(pu1==0) {
          pu1=Surf1->LastUParameter() - Surf1->FirstUParameter();
          pu1+=pu1;
        }
        if(pu2==0) {
          pu2=Surf2->LastUParameter() - Surf2->FirstUParameter();
          pu2+=pu2;
        }
        if(pv1==0) {
          pv1=Surf1->LastVParameter() - Surf1->FirstVParameter();
          pv1+=pv1;
        }
        if(pv2==0) {
          pv2=Surf2->LastVParameter() - Surf2->FirstVParameter();
          pv2+=pv2;
        }
    */

    wlin->SetPeriod(pu1, pv1, pu2, pv2);
    wlin->ComputeVertexParameters(Tol);
  }
  else
  {
    rlin->ComputeVertexParameters(Tol);
  }
}
