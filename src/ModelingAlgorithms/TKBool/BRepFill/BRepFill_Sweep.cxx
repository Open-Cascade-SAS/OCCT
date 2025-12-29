// Created on: 1998-01-07
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Approx_CurveOnSurface.hxx>
#include <Approx_SameParameter.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <BRepFill_Sweep.hxx>
#include <BRepFill_TrimShellCorner.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools_Substitution.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_Sweep.hxx>
#include <GeomLib.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_HArray2.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>

#include <cstdio>

// #include <BRepFill_TrimCorner.hxx>
//  modified by NIZHNY-MKK  Wed Oct 22 12:25:45 2003
// #include <GeomPlate_BuildPlateSurface.hxx>
// #include <GeomPlate_Surface.hxx>
// #include <GeomPlate_PointConstraint.hxx>
// OCC500(apo)

//=================================================================================================

static int NumberOfPoles(const TopoDS_Wire& W)
{
  int NbPoints = 0;

  TopoDS_Iterator iter(W);
  for (; iter.More(); iter.Next())
  {
    BRepAdaptor_Curve c(TopoDS::Edge(iter.Value()));

    double dfUf = c.FirstParameter();
    double dfUl = c.LastParameter();
    if (IsEqual(dfUf, dfUl))
      // Degenerate
      continue;

    switch (c.GetType())
    {
      case GeomAbs_BezierCurve: {
        // Put all poles for bezier
        occ::handle<Geom_BezierCurve> GC     = c.Bezier();
        int                           iNbPol = GC->NbPoles();
        if (iNbPol >= 2)
          NbPoints += iNbPol;
        break;
      }
      case GeomAbs_BSplineCurve: {
        // Put all poles for bspline
        occ::handle<Geom_BSplineCurve> GC     = c.BSpline();
        int                            iNbPol = GC->NbPoles();
        if (iNbPol >= 2)
          NbPoints += iNbPol;
        break;
      }
      case GeomAbs_Line: {
        NbPoints += 2;
        break;
      }
      case GeomAbs_Circle:
      case GeomAbs_Ellipse:
      case GeomAbs_Hyperbola:
      case GeomAbs_Parabola: {
        NbPoints += 4;
        break;
      }
      default:
        NbPoints += 15 + c.NbIntervals(GeomAbs_C3);
    } // switch (c.GetType()) ...
  } // for (; iter.More(); iter.Next())

  return NbPoints;
}

//=================================================================================================

static bool HasPCurves(const TopoDS_Edge& E)
{
  bool haspcurves = false;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(
    (*((occ::handle<BRep_TEdge>*)&E.TShape()))->Curves());
  for (; itcr.More(); itcr.Next())
  {
    const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
    if (cr->IsCurveOnSurface())
    {
      haspcurves = true;
      break;
    }
  }
  return haspcurves;
}

//=======================================================================
// function : Translate
// purpose  : Copy a column from one table to another.
//=======================================================================
static void Translate(const occ::handle<NCollection_HArray2<TopoDS_Shape>>& ArrayIn,
                      const int                                             In,
                      occ::handle<NCollection_HArray2<TopoDS_Shape>>&       ArrayOut,
                      const int                                             Out)
{
  int ii, Nb;
  Nb = ArrayOut->ColLength();
  for (ii = 1; ii <= Nb; ii++)
  {
    ArrayOut->SetValue(ii, Out, ArrayIn->Value(ii, In));
  }
}

//=======================================================================
// function : Box
// purpose  : Bounding box of a section.
//=======================================================================
static void Box(occ::handle<GeomFill_SectionLaw>& Sec, const double U, Bnd_Box& Box)

{
  int NbPoles, bid;
  Box.SetVoid();
  Sec->SectionShape(NbPoles, bid, bid);
  NCollection_Array1<gp_Pnt> Poles(1, NbPoles);
  NCollection_Array1<double> W(1, NbPoles);
  Sec->D0(U, Poles, W);
  for (int ii = 1; ii <= NbPoles; ii++)
  {
    Box.Add(Poles(ii));
  }
}

//=======================================================================
// function : Couture
// purpose  : Check if E is an edge of sewing on S
//           and make the representation HadHoc
//=======================================================================
static occ::handle<Geom2d_Curve> Couture(const TopoDS_Edge&               E,
                                         const occ::handle<Geom_Surface>& S,
                                         const TopLoc_Location&           L)
{
  TopLoc_Location l           = L.Predivided(E.Location());
  bool            Eisreversed = (E.Orientation() == TopAbs_REVERSED);

  // find the representation
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(
    (*((occ::handle<BRep_TEdge>*)&E.TShape()))->ChangeCurves());

  while (itcr.More())
  {
    occ::handle<BRep_CurveRepresentation> cr = itcr.Value();
    if (cr->IsCurveOnSurface(S, l))
    {
      occ::handle<BRep_GCurve> GC(occ::down_cast<BRep_GCurve>(cr));
      if (GC->IsCurveOnClosedSurface() && Eisreversed)
        return GC->PCurve2();
      else
        return GC->PCurve();
    }
    itcr.Next();
  }
  occ::handle<Geom2d_Curve> pc;
  pc.Nullify();
  return pc;
}

//=======================================================================
// function : CheckSameParameter
// purpose  : Check a posteriori that sameparameter has worked correctly
//=======================================================================

static bool CheckSameParameter(const occ::handle<Adaptor3d_Curve>&   C3d,
                               const occ::handle<Geom2d_Curve>&      Pcurv,
                               const occ::handle<Adaptor3d_Surface>& S,
                               const double                          tol3d,
                               double&                               tolreached)
{
  tolreached  = 0.;
  double f    = C3d->FirstParameter();
  double l    = C3d->LastParameter();
  int    nbp  = 45;
  double step = 1. / (nbp - 1);
  for (int i = 0; i < nbp; i++)
  {
    double t, u, v;
    t = step * i;
    t = (1 - t) * f + t * l;
    Pcurv->Value(t).Coord(u, v);
    gp_Pnt pS  = S->Value(u, v);
    gp_Pnt pC  = C3d->Value(t);
    double d2  = pS.SquareDistance(pC);
    tolreached = std::max(tolreached, d2);
  }
  tolreached = sqrt(tolreached);
  if (tolreached > tol3d)
  {
    tolreached *= 2.;
    return false;
  }
  tolreached *= 2.;
  tolreached = std::max(tolreached, Precision::Confusion());
  return true;
}

//=======================================================================
// function : CheckSameParameterExact
// purpose  : Check a posteriori that sameparameter has worked correctly
// with exact calculation method of edge tolerance
//=======================================================================
static bool CheckSameParameterExact(const occ::handle<Adaptor3d_Curve>&          C3d,
                                    const occ::handle<Adaptor3d_CurveOnSurface>& curveOnSurface,
                                    const double                                 tol3d,
                                    double&                                      tolreached)
{
  GeomLib_CheckCurveOnSurface aCheckCurveOnSurface(C3d);
  aCheckCurveOnSurface.SetParallel(false);
  aCheckCurveOnSurface.Perform(curveOnSurface);

  tolreached = aCheckCurveOnSurface.MaxDistance();

  if (tolreached > tol3d)
  {
    return false;
  }
  else
  {
    tolreached = std::max(tolreached, Precision::Confusion());
    tolreached *= 1.05;
  }
  return true;
}

//=======================================================================
// function : SameParameter
// purpose  : Encapsulation of Sameparameter
// Boolean informs if the pcurve was computed or not...
// The tolerance is always OK.
//=======================================================================

static bool SameParameter(TopoDS_Edge&                     E,
                          occ::handle<Geom2d_Curve>&       Pcurv,
                          const occ::handle<Geom_Surface>& Surf,
                          const double                     tol3d,
                          double&                          tolreached)
{
  // occ::handle<BRepAdaptor_Curve> C3d = new (BRepAdaptor_Curve)(E);
  double                         f, l;
  occ::handle<Geom_Curve>        C3d = BRep_Tool::Curve(E, f, l);
  GeomAdaptor_Curve              GAC3d(C3d, f, l);
  occ::handle<GeomAdaptor_Curve> HC3d = new GeomAdaptor_Curve(GAC3d);

  occ::handle<GeomAdaptor_Surface> S = new (GeomAdaptor_Surface)(Surf);
  double                           ResTol;

  if (CheckSameParameter(HC3d, Pcurv, S, tol3d, tolreached))
    return true;

  if (!HasPCurves(E))
  {
    occ::handle<Geom2dAdaptor_Curve> HC2d = new Geom2dAdaptor_Curve(Pcurv);
    Approx_CurveOnSurface            AppCurve(HC2d,
                                   S,
                                   HC2d->FirstParameter(),
                                   HC2d->LastParameter(),
                                   Precision::Confusion());
    AppCurve.Perform(10, 10, GeomAbs_C1, true);
    if (AppCurve.IsDone() && AppCurve.HasResult())
    {
      C3d        = AppCurve.Curve3d();
      tolreached = AppCurve.MaxError3d();
      BRep_Builder B;
      B.UpdateEdge(E, C3d, tolreached);
      return true;
    }
  }

  const occ::handle<Adaptor3d_Curve>& aHCurve = HC3d; // to avoid ambiguity
  Approx_SameParameter                sp(aHCurve, Pcurv, S, tol3d);
  if (sp.IsDone() && !sp.IsSameParameter())
    Pcurv = sp.Curve2d();
  else if (!sp.IsDone() && !sp.IsSameParameter())
  {
#ifdef OCCT_DEBUG
    std::cout << "echec SameParameter" << std::endl;
#endif
    return false;
  }

  occ::handle<Adaptor3d_Curve>          curve3d        = sp.Curve3d();
  occ::handle<Adaptor3d_CurveOnSurface> curveOnSurface = sp.CurveOnSurface();

  if (!CheckSameParameterExact(curve3d, curveOnSurface, tol3d, ResTol) && ResTol > tolreached)
  {
#ifdef OCCT_DEBUG
    std::cout << "SameParameter : Tolerance not reached!" << std::endl;
    std::cout << "tol visee : " << tol3d << " tol obtained : " << ResTol << std::endl;
#endif
    return false;
  }
  else
  {
    tolreached = 1.1 * ResTol;
    if (sp.IsDone() && !sp.IsSameParameter())
      Pcurv = sp.Curve2d();
  }
  return true;
}

static void CorrectSameParameter(TopoDS_Edge&       theEdge,
                                 const TopoDS_Face& theFace1,
                                 const TopoDS_Face& theFace2)
{
  if (BRep_Tool::Degenerated(theEdge))
    return;

  double                  fpar, lpar;
  occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(theEdge, fpar, lpar);

  bool              PCurveExists[2] = {false, false};
  BRepAdaptor_Curve BAcurve[2];

  if (!theFace1.IsNull())
  {
    PCurveExists[0] = true;
    BAcurve[0].Initialize(theEdge, theFace1);
  }
  if (!theFace1.IsNull() && theFace1.IsSame(theFace2))
    theEdge.Reverse();
  if (!theFace2.IsNull())
  {
    PCurveExists[1] = true;
    BAcurve[1].Initialize(theEdge, theFace2);
  }

  double    MaxSqDist = 0.;
  const int NCONTROL  = 23;
  double    delta     = (lpar - fpar) / NCONTROL;

  for (int i = 0; i <= NCONTROL; i++)
  {
    double aParam = fpar + i * delta;
    gp_Pnt aPnt   = aCurve->Value(aParam);
    for (int j = 0; j < 2; j++)
      if (PCurveExists[j])
      {
        gp_Pnt aPntFromFace = BAcurve[j].Value(aParam);
        double aSqDist      = aPnt.SquareDistance(aPntFromFace);
        if (aSqDist > MaxSqDist)
          MaxSqDist = aSqDist;
      }
  }

  double       aTol = sqrt(MaxSqDist);
  BRep_Builder BB;
  BB.UpdateEdge(theEdge, aTol);
}

//=======================================================================
// Objet : Orientate an edge of natural restriction
//      : General
//=======================================================================
static void Oriente(const occ::handle<Geom_Surface>& S, TopoDS_Edge& E)
{
  gp_Pnt2d P;
  gp_Vec2d D, URef(1, 0), VRef(0, 1);
  bool     isuiso, isfirst, isopposite;
  double   UFirst, ULast, VFirst, VLast, f, l;
  S->Bounds(UFirst, ULast, VFirst, VLast);
  occ::handle<Geom2d_Curve> C;
  TopLoc_Location           bid;

  C = BRep_Tool::CurveOnSurface(E, S, bid, f, l);
  C->D1((f + l) / 2, P, D);

  isuiso = D.IsParallel(VRef, 0.1);

  if (isuiso)
  {
    isfirst    = (std::abs(P.X() - UFirst) < Precision::Confusion());
    isopposite = D.IsOpposite(VRef, 0.1);
    E.Orientation(TopAbs_REVERSED);
  }
  else
  {
    isfirst    = (std::abs(P.Y() - VFirst) < Precision::Confusion());
    isopposite = D.IsOpposite(URef, 0.1);
    E.Orientation(TopAbs_FORWARD);
  }

  if (!isfirst)
    E.Reverse();
  if (isopposite)
    E.Reverse();
}

// OCC500(apo)->
static void UpdateEdgeOnPlane(const TopoDS_Face& F, const TopoDS_Edge& E, const BRep_Builder& BB)
{
  double                    f, l;
  occ::handle<Geom2d_Curve> C2d = BRep_Tool::CurveOnSurface(E, F, f, l);
  occ::handle<Geom_Surface> S   = BRep_Tool::Surface(F);
  TopLoc_Location           Loc;
  double                    Tol = BRep_Tool::Tolerance(E);
  BB.UpdateEdge(E, C2d, S, Loc, Tol);
  BRepCheck_Edge Check(E);
  Tol = std::max(Tol, Check.Tolerance());
  BB.UpdateEdge(E, Tol);
  TopoDS_Vertex V;
  Tol *= 1.01;
  V = TopExp::FirstVertex(E);
  if (BRep_Tool::Tolerance(V) < Tol)
    BB.UpdateVertex(V, Tol);
  V = TopExp::LastVertex(E);
  if (BRep_Tool::Tolerance(V) < Tol)
    BB.UpdateVertex(V, Tol);
}

//<-OCC500(apo)
//=======================================================================
// Function : BuildFace
// Objet : Construct a Face via a surface and 4 Edges (natural borders)
//      : Only one Hypothesis : isos u and v are switched :
//        Edge1/3 are iso u (recp v)
//        Edge2/4 are iso v (recp u)
//=======================================================================
static void BuildFace(
  const occ::handle<Geom_Surface>&                                          S,
  const TopoDS_Edge&                                                        E1,
  const TopoDS_Edge&                                                        E2,
  const TopoDS_Edge&                                                        E3,
  const TopoDS_Edge&                                                        E4,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& EEmap,
  const bool                                                                ExchUV,
  const bool                                                                UReverse,
  TopoDS_Face&                                                              F)
{
  double Tol;
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  TopoDS_Edge             e1, e2, E;
  TopoDS_Wire             WW;
  BRep_Builder            BB;
  BRepBuilderAPI_MakeWire B;
  TopoDS_Iterator         Iter;
  // gp_Pnt2d P;

  // Is the surface planar ?
  double Tol1, Tol2, Tol3, Tol4;
  Tol1 = BRep_Tool::Tolerance(E1);
  Tol2 = BRep_Tool::Tolerance(E2);
  Tol3 = BRep_Tool::Tolerance(E3);
  Tol4 = BRep_Tool::Tolerance(E4);
  //  Tol = std::min( BT.Tolerance(E1), BT.Tolerance(E2));
  Tol = std::min(Tol1, Tol2);
  //  Tol = std::min(Tol, std::min(BT.Tolerance(E3),BT.Tolerance(E4)));
  Tol                            = std::min(Tol, std::min(Tol3, Tol4));
  bool                    IsPlan = false;
  occ::handle<Geom_Plane> thePlane;

  if (!E1.IsSame(E3) && !E2.IsSame(E4)) // exclude cases with seam edges: they are not planar
  {
    GeomLib_IsPlanarSurface IsP(S, Tol);
    if (IsP.IsPlanar())
    {
      IsPlan   = true;
      thePlane = new Geom_Plane(IsP.Plan());
    }
    else
    {
      occ::handle<BRep_TEdge>& TE1 = *((occ::handle<BRep_TEdge>*)&E1.TShape());
      occ::handle<BRep_TEdge>& TE2 = *((occ::handle<BRep_TEdge>*)&E2.TShape());
      occ::handle<BRep_TEdge>& TE3 = *((occ::handle<BRep_TEdge>*)&E3.TShape());
      occ::handle<BRep_TEdge>& TE4 = *((occ::handle<BRep_TEdge>*)&E4.TShape());
      TE1->Tolerance(Precision::Confusion());
      TE2->Tolerance(Precision::Confusion());
      TE3->Tolerance(Precision::Confusion());
      TE4->Tolerance(Precision::Confusion());

      TopoDS_Wire theWire  = BRepLib_MakeWire(E1, E2, E3, E4);
      int         NbPoints = NumberOfPoles(theWire);
      if (NbPoints <= 100) // limitation for CPU
      {
        BRepLib_FindSurface FS(theWire, -1, true);
        if (FS.Found())
        {
          IsPlan   = true;
          thePlane = occ::down_cast<Geom_Plane>(FS.Surface());
        }
      }
      BB.UpdateEdge(E1, Tol1);
      BB.UpdateEdge(E2, Tol2);
      BB.UpdateEdge(E3, Tol3);
      BB.UpdateEdge(E4, Tol4);
    }
  }

  // Construction of the wire
  //  B.MakeWire(WW);
  e1 = E1;
  Oriente(S, e1);
  //  if (!IsPlan || !BT.Degenerated(e1))
  if (!IsPlan || !BRep_Tool::Degenerated(e1))
    B.Add(e1);

  e2 = E2;
  Oriente(S, e2);
  //  if (!IsPlan || !BT.Degenerated(e2))
  if (!IsPlan || !BRep_Tool::Degenerated(e2))
  {
    B.Add(e2);
    if (!BRep_Tool::Degenerated(e2))
    {
      WW = B.Wire();
      TopoDS_Shape NewEdge;
      // take the last edge added to WW
      for (Iter.Initialize(WW); Iter.More(); Iter.Next())
        NewEdge = Iter.Value();
      if (!e2.IsSame(NewEdge))
        EEmap.Bind(e2, NewEdge);
    }
  }

  if (E3.IsSame(E1))
  {
    E = e1;
    E.Reverse();
  }
  else
  {
    E = E3;
    Oriente(S, E);
  }
  //  if (!IsPlan || !BT.Degenerated(E))
  if (!IsPlan || !BRep_Tool::Degenerated(E))
  {
    B.Add(E);
    if (!BRep_Tool::Degenerated(E))
    {
      WW = B.Wire();
      TopoDS_Shape NewEdge;
      // take the last edge added to WW
      for (Iter.Initialize(WW); Iter.More(); Iter.Next())
        NewEdge = Iter.Value();
      if (!E.IsSame(NewEdge))
        EEmap.Bind(E, NewEdge);
    }
  }

  if (E4.IsSame(E2))
  {
    E = e2;
    E.Reverse();
  }
  else
  {
    E = E4;
    Oriente(S, E);
  }
  //  if (!IsPlan || !BT.Degenerated(E))
  if (!IsPlan || !BRep_Tool::Degenerated(E))
  {
    B.Add(E);
    if (!BRep_Tool::Degenerated(E))
    {
      WW = B.Wire();
      TopoDS_Shape NewEdge;
      // take the last edge added to WW
      for (Iter.Initialize(WW); Iter.More(); Iter.Next())
        NewEdge = Iter.Value();
      if (!E.IsSame(NewEdge))
        EEmap.Bind(E, NewEdge);
    }
  }

  WW = B.Wire();

  // Construction of the face.
  if (IsPlan)
  { // Suspend representation 2d
    // and construct face Plane

    // BRepLib_MakeFace MkF(IsP.Plan(), WW);
    gp_Pnt aPnt;
    gp_Vec DU, DV, NS, NP;
    double Ufirst, Ulast, Vfirst, Vlast;
    S->Bounds(Ufirst, Ulast, Vfirst, Vlast);
    S->D1((Ufirst + Ulast) / 2., (Vfirst + Vlast) / 2., aPnt, DU, DV);
    NS = DU ^ DV;
    NP = thePlane->Pln().Axis().Direction();
    if (NS.Dot(NP) < 0.)
      thePlane->UReverse();
    BRepLib_MakeFace MkF(thePlane, WW);
    if (MkF.Error() != BRepLib_FaceDone)
    {
#ifdef OCCT_DEBUG
      BRepLib_FaceError Err = MkF.Error();
      std::cout << "Planar Face Error :" << Err << std::endl;
#endif
    }
    else
    {
      occ::handle<Geom2d_Curve> NullC2d;
      TopLoc_Location           Loc;
      BB.UpdateEdge(E1, NullC2d, S, Loc, Tol1);
      BB.UpdateEdge(E2, NullC2d, S, Loc, Tol2);
      BB.UpdateEdge(E3, NullC2d, S, Loc, Tol3);
      BB.UpdateEdge(E4, NullC2d, S, Loc, Tol4);

      F = MkF.Face();
      UpdateEdgeOnPlane(F, E1, BB);
      UpdateEdgeOnPlane(F, E2, BB);
      UpdateEdgeOnPlane(F, E3, BB);
      UpdateEdgeOnPlane(F, E4, BB);
      /*OCC500(apo)->
            TopLoc_Location Loc;
            occ::handle<Geom2d_Curve> NC;
            NC.Nullify();
      //      B.UpdateEdge(E1, NC, S, Loc, BT.Tolerance(E1));
            BB.UpdateEdge(E1, NC, S, Loc, BRep_Tool::Tolerance(E1));
      //      B.UpdateEdge(E2, NC, S, Loc, BT.Tolerance(E2));
            BB.UpdateEdge(E2, NC, S, Loc, BRep_Tool::Tolerance(E2));
      //      B.UpdateEdge(E3, NC, S, Loc, BT.Tolerance(E3));
            BB.UpdateEdge(E3, NC, S, Loc, BRep_Tool::Tolerance(E3));
      //      B.UpdateEdge(E4, NC, S, Loc, BT.Tolerance(E4));
            BB.UpdateEdge(E4, NC, S, Loc, BRep_Tool::Tolerance(E4));
      <-OCC500(apo)*/
    }
  }

  if (!IsPlan)
  { // Cas Standard : Ajout
    BB.MakeFace(F, S, Precision::Confusion());
    BB.Add(F, WW);
  }

  // Reorientation
  if (ExchUV)
    F.Reverse();
  if (UReverse)
    F.Reverse();
}

//=======================================================================
// Fonction : BuildEdge
// Objet : Construct non-closed Edge
//=======================================================================
static TopoDS_Edge BuildEdge(occ::handle<Geom_Curve>&   C3d,
                             occ::handle<Geom2d_Curve>& C2d,
                             occ::handle<Geom_Surface>& S,
                             const TopoDS_Vertex&       VF,
                             const TopoDS_Vertex&       VL,
                             const double               f,
                             const double               l,
                             const double               Tol3d)
{
  gp_Pnt P;
  double Tol1, Tol2, Tol, d;
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  BRep_Builder B;
  TopoDS_Edge  E;

  //  P1  = BT.Pnt(VF);
  const gp_Pnt P1 = BRep_Tool::Pnt(VF);
  //  Tol1 = BT.Tolerance(VF);
  Tol1 = BRep_Tool::Tolerance(VF);
  //  P2  = BT.Pnt(VL);
  const gp_Pnt P2 = BRep_Tool::Pnt(VL);
  //  Tol2 = BT.Tolerance(VF);
  Tol2 = BRep_Tool::Tolerance(VL);
  Tol  = std::max(Tol1, Tol2);

  if (VF.IsSame(VL) || (P1.Distance(P2) < Tol))
  {
    // Degenerated case
    gp_Pnt2d P2d;
    C2d->D0(f, P2d);
    S->D0(P2d.X(), P2d.Y(), P);
    d = P1.Distance(P);
    if (d > Tol)
      Tol = d;
    C2d->D0(l, P2d);
    S->D0(P2d.X(), P2d.Y(), P);
    d = P2.Distance(P);
    if (d > Tol)
      Tol = d;

    B.UpdateVertex(VF, Tol);
    B.UpdateVertex(VL, Tol);

    B.MakeEdge(E);
    B.UpdateEdge(E, C2d, S, TopLoc_Location(), Tol);
    B.Add(E, VF);
    B.Add(E, VL);
    B.Range(E, f, l);
    B.Degenerated(E, true);

    return E;
  }

  C3d->D0(f, P);
  d = P1.Distance(P);
  if (d > Tol1)
    B.UpdateVertex(VF, d);

  C3d->D0(l, P);
  d = P2.Distance(P);
  if (d > Tol2)
    B.UpdateVertex(VL, d);

  BRepLib_MakeEdge MkE(C3d, VF, VL, f, l);
  if (!MkE.IsDone())
  { // Error of construction !!
  }

  E = MkE.Edge();
  TopLoc_Location Loc;
  B.UpdateEdge(E, C2d, S, Loc, Tol3d);

  const occ::handle<IntTools_Context> aNullCtx;
  if (BOPTools_AlgoTools::IsMicroEdge(E, aNullCtx))
  {
    const TopoDS_Vertex& aV = VF;
    B.UpdateVertex(aV, P1.Distance(P2));
    B.MakeEdge(E);
    B.UpdateEdge(E, C2d, S, TopLoc_Location(), Tol);
    B.Add(E, TopoDS::Vertex(aV.Oriented(TopAbs_FORWARD)));
    B.Add(E, TopoDS::Vertex(aV.Oriented(TopAbs_REVERSED)));
    B.Range(E, f, l);
    B.Degenerated(E, true);
  }

  return E;
}

//=======================================================================
// Fonction : Filling
// Objet : Construct the faces of filling
//=======================================================================
static bool Filling(const TopoDS_Shape&                                                       EF,
                    const TopoDS_Shape&                                                       F1,
                    const TopoDS_Shape&                                                       EL,
                    const TopoDS_Shape&                                                       F2,
                    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& EEmap,
                    const double                                                              Tol,
                    const gp_Ax2&                                                             Axe,
                    const gp_Vec& TangentOnPart1,
                    TopoDS_Edge&  Aux1,
                    TopoDS_Edge&  Aux2,
                    TopoDS_Face&  Result)
{
  BRep_Builder B;
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  //  int NbInt =0;
  //  double Tol3d = Tol;
  bool WithE3, WithE4;

  // Return constraints
  TopoDS_Vertex V1, V2, Vf, Vl;
  TopoDS_Edge   E1, E2, E3, E4;
  E1 = TopoDS::Edge(EF);
  E2 = TopoDS::Edge(EL);

  TopExp::Vertices(E1, Vf, Vl);
  Vf.Orientation(TopAbs_FORWARD);
  Vl.Orientation(TopAbs_FORWARD);

  TopExp::Vertices(E2, V1, V2);
  V1.Orientation(TopAbs_REVERSED);
  V2.Orientation(TopAbs_REVERSED);

  B.MakeEdge(E3);
  B.MakeEdge(E4);

  WithE3 = WithE4 = false;

  if ((!Aux1.IsNull()) && (!Vf.IsSame(V1)))
  {
    E3 = Aux1;
    //    E3 = TopoDS::Edge(Aux1);
    WithE3 = true;
  }

  if (Vf.IsSame(Vl))
  {
    E4 = E3;
    E4.Reverse();
    WithE4 = WithE3;
  }
  else if (!Aux2.IsNull() && (!Vl.IsSame(V2)))
  {
    E4 = Aux2;
    //    E4 = TopoDS::Edge(Aux2);
    WithE4 = true;
  }

  // Construction of a surface of revolution
  occ::handle<Geom_Curve> Prof1, Prof2;
  // int ii, jj;//, Nb;
  double f1, f2, l1, l2, /*d1, d2,*/ Angle; //, Eps = 1.e-9;
                                            //  Prof1 = BT.Curve(E1, f1, l1);
  Prof1 = BRep_Tool::Curve(E1, f1, l1);
  //  Prof2 = BT.Curve(E2, f2, l2);
  Prof2 = BRep_Tool::Curve(E2, f2, l2);

  // Indeed, both Prof1 and Prof2 are the same curves but in different positions
  // Prof1's param domain may equals to Prof2's param domain *(-1), which means EF.Orientation() ==
  // EL.Orientation()
  bool   bSameCurveDomain = EF.Orientation() != EL.Orientation();
  gp_Pnt P1, P2, P;

  // Choose the angle of opening
  gp_Trsf aTf;
  aTf.SetTransformation(Axe);

  // Choose the furthest point from the "center of revolution"
  // to provide correct angle measurement.
  const double aPrm[] = {f1, 0.5 * (f1 + l1), l1};
  const gp_Pnt aP1[]  = {Prof1->Value(aPrm[0]).Transformed(aTf),
                         Prof1->Value(aPrm[1]).Transformed(aTf),
                         Prof1->Value(aPrm[2]).Transformed(aTf)};

  int    aMaxIdx  = -1;
  double aMaxDist = RealFirst();
  for (int i = 0; i < 3; i++)
  {
    const double aDist = aP1[i].X() * aP1[i].X() + aP1[i].Z() * aP1[i].Z();
    if (aDist > aMaxDist)
    {
      aMaxDist = aDist;
      aMaxIdx  = i;
    }
  }

  const double aPrm2[] = {f2, 0.5 * (f2 + l2), l2};
  const gp_Pnt aP2 = Prof2->Value(aPrm2[bSameCurveDomain ? aMaxIdx : 2 - aMaxIdx]).Transformed(aTf);
  const gp_Vec2d aV1(aP1[aMaxIdx].Z(), aP1[aMaxIdx].X());
  const gp_Vec2d aV2(aP2.Z(), aP2.X());
  if (aV1.SquareMagnitude() <= gp::Resolution() || aV2.SquareMagnitude() <= gp::Resolution())
  {
    return false;
  }

  Angle = aV1.Angle(aV2);

  gp_Ax1 axe(Axe.Location(), Axe.YDirection());

  if (Angle < 0)
  {
    Angle = -Angle;
    axe.Reverse();
  }

  occ::handle<Geom_SurfaceOfRevolution> Rev = new (Geom_SurfaceOfRevolution)(Prof1, axe);

  occ::handle<Geom_Surface> Surf = new (Geom_RectangularTrimmedSurface)(Rev, 0, Angle, f1, l1);

  // Control the direction of the rotation
  bool   ToReverseResult = false;
  gp_Vec d1u;
  d1u = Surf->DN(0, aPrm[aMaxIdx], 1, 0);
  if (d1u.Angle(TangentOnPart1) > M_PI / 2)
  { // Invert everything
    ToReverseResult = true;
    /*
    axe.Reverse();
    Angle = 2*M_PI - Angle;
    Rev = new (Geom_SurfaceOfRevolution) (Prof1, axe);
    Surf = new (Geom_RectangularTrimmedSurface)
           (Rev, 0, Angle, f1, l1);
    */
  }

  occ::handle<Geom2d_Curve> C1, C2, C3, C4;
  /*
  // Deform the surface of revolution.
    GeomPlate_BuildPlateSurface BPS;

    occ::handle<BRepAdaptor_Surface> AS;
    occ::handle<BRepAdaptor_Curve2d> AC2d;
    occ::handle<Adaptor3d_CurveOnSurface> HConS;
  */
  occ::handle<Geom2d_Line> L;
  gp_Pnt2d                 P2d(0., 0.);

  L  = new (Geom2d_Line)(P2d, gp::DY2d());
  C1 = new (Geom2d_TrimmedCurve)(L, f1, l1);

  P2d.SetCoord(Angle, 0.);
  L  = new (Geom2d_Line)(P2d, gp::DY2d());
  C2 = new (Geom2d_TrimmedCurve)(L, f1, l1);

  // It is required to control the direction and the range.
  C2->D0(f1, P2d);
  Surf->D0(P2d.X(), P2d.Y(), P1);
  C2->D0(l1, P2d);
  Surf->D0(P2d.X(), P2d.Y(), P2);
  //  P = BT.Pnt(V1);
  P = BRep_Tool::Pnt(V1);
  if (P.Distance(P2) + Tol < P.Distance(P1))
  {
    // E2 is parsed in the direction opposite to E1
    C2->Reverse();
    TopoDS_Vertex aux;
    aux = V2;
    V2  = V1;
    V1  = aux;
  }
  GeomLib::SameRange(Precision::PConfusion(),
                     C2,
                     C2->FirstParameter(),
                     C2->LastParameter(),
                     f2,
                     l2,
                     C3);
  C2 = C3;

  //  P1 = BT.Pnt(Vf);
  P1 = BRep_Tool::Pnt(Vf);
  //  P2 = BT.Pnt(V1);
  P2 = BRep_Tool::Pnt(V1);
  //  pointu_f = Vf.IsSame(V1) || (P1.Distance(P2) < BT.Tolerance(Vf));
  //  P1 = BT.Pnt(Vl);
  P1 = BRep_Tool::Pnt(Vl);
  //  P2 = BT.Pnt(V2);
  P2 = BRep_Tool::Pnt(V2);
  //  pointu_l = Vl.IsSame(V2) || (P1.Distance(P2) < BT.Tolerance(Vl));

  P2d.SetCoord(0., f1);
  L  = new (Geom2d_Line)(P2d, gp::DX2d());
  C3 = new (Geom2d_TrimmedCurve)(L, 0, Angle);

  P2d.SetCoord(0., l1);
  L  = new (Geom2d_Line)(P2d, gp::DX2d());
  C4 = new (Geom2d_TrimmedCurve)(L, 0, Angle);
  /*
    // Determine the constraints and
    // their parametric localisation.
    if (!E1.IsNull()) {
       AS = new BRepAdaptor_Surface(TopoDS::Face(F1));
       AC2d = new BRepAdaptor_Curve2d();
       AC2d->ChangeCurve2d().Initialize(E1,TopoDS::Face(F1));
       HConS = new (Adaptor3d_CurveOnSurface)();
       HConS->ChangeCurve().Load(AC2d);
       HConS->ChangeCurve().Load(AS);

       occ::handle<BRepFill_CurveConstraint> Cont
        = new BRepFill_CurveConstraint(HConS, 1, 15);
       Cont->SetCurve2dOnSurf(C1);

       BPS.Add(Cont);
       NbInt = HConS->NbIntervals(GeomAbs_CN);
     }

    if (!E2.IsNull()) {
      AS = new BRepAdaptor_Surface(TopoDS::Face(F2));
      AC2d = new BRepAdaptor_Curve2d();
      AC2d->ChangeCurve2d().Initialize(E2,TopoDS::Face(F2));
      HConS = new (Adaptor3d_CurveOnSurface);

      HConS->ChangeCurve().Load(AC2d);
      HConS->ChangeCurve().Load(AS);

      occ::handle<BRepFill_CurveConstraint> Cont
        = new BRepFill_CurveConstraint(HConS, 1, 15);
      Cont->SetCurve2dOnSurf(C2);
      BPS.Add(Cont);
    }

    if (WithE3) {
      occ::handle<BRepAdaptor_Curve> AC = new (BRepAdaptor_Curve) (E3);
      occ::handle<BRepFill_CurveConstraint> Cont
        = new BRepFill_CurveConstraint(AC, 0);
      Cont->SetCurve2dOnSurf(C3);
      BPS.Add(Cont);
    }
    else if (pointu_f) {
      double delta = Angle / 11;
  //    P = BT.Pnt(Vf);
      P = BRep_Tool::Pnt(Vf);
      occ::handle<GeomPlate_PointConstraint> PC;
      for (ii=1; ii<=10; ii++) {
        C3->D0(ii*delta, P2d);
        PC = new (GeomPlate_PointConstraint) (P, 0);
        PC->SetPnt2dOnSurf(P2d);
        BPS.Add(PC);
      }
    }

    if (WithE4) {
      occ::handle<BRepAdaptor_Curve> AC = new (BRepAdaptor_Curve) (E4);
      occ::handle<BRepFill_CurveConstraint> Cont
        = new BRepFill_CurveConstraint(AC, 0);
      Cont->SetCurve2dOnSurf(C4);
      BPS.Add(Cont);
    }
    else if (pointu_l) {
      double delta = Angle / 11;
  //    P = BT.Pnt(Vl);
      P = BRep_Tool::Pnt(Vl);
      occ::handle<GeomPlate_PointConstraint> PC;
      for (ii=1; ii<=10; ii++) {
        C4->D0(ii*delta, P2d);
        PC = new (GeomPlate_PointConstraint) (P, 0);
        PC->SetPnt2dOnSurf(P2d);
        BPS.Add(PC);
      }
    }

    BPS.LoadInitSurface(Surf);
    BPS.Perform();

    // Controle s'il y a une deformation effective
    occ::handle<GeomPlate_Surface> plate;
    plate = BPS.Surface();
    plate->SetBounds(0, Angle, f1, l1);
    bool Ok=true;
    double u, v;
    d1 = (l1-f1)/5;
    d2 = Angle/5;
    for (ii=0; ii<=5 && Ok; ii++) {
      u = f1 + ii*d1;
      for (jj=0; jj<=5 && Ok; jj++) {
        v = jj*d2;
        plate->D0(u, v, P1);
        Surf->D0(u, v, P2);
        Ok = (P2.IsEqual(P1, Tol));
      }
    }

    if (!Ok) {
      // Approx de la plate surface
      // Bords naturelles => pas besoin de criteres.
      GeomConvert_ApproxSurface App(BPS.Surface(),
                    Tol3d,
                    GeomAbs_C1, GeomAbs_C1,
                    8, 8, 2*NbInt, 0);
      if (!App.HasResult()) {
  #ifdef OCCT_DEBUG
        std::cout << "Filling_Approx : Pas de resultat" << std::endl;
  #endif
        return false;
      }
  #ifdef OCCT_DEBUG
      std::cout <<  "Filling_Approx Error 3d = " <<
        App.MaxError() << std::endl;
  #endif
      Surf = App.Surface();
      Tol3d = App.MaxError();
    }
  */

  // Update des Edges
  TopLoc_Location         Loc;
  occ::handle<Geom_Curve> C3d;
  B.UpdateEdge(E1, C1, Surf, Loc, /*Tol3d*/ Precision::Confusion());
  B.UpdateEdge(E2, C2, Surf, Loc, /*Tol3d*/ Precision::Confusion());

  if (E3.IsSame(E4))
  {
    if (!WithE3)
    {
      C3d = Surf->VIso(f1);
      E3  = BuildEdge(C3d, C3, Surf, Vf, V1, 0, Angle, /*Tol3d*/ Precision::Confusion());
    }
    else
    {
      BRepAdaptor_Curve aCurve(E3);
      double            AngleOld = aCurve.LastParameter();
      if (Angle > AngleOld)
      {
        B.Range(E3, 0, Angle);
        TopoDS_Vertex              V(TopExp::LastVertex(E3));
        occ::handle<BRep_TVertex>& TVlast = *((occ::handle<BRep_TVertex>*)&V.TShape());
        TVlast->Tolerance(Precision::Confusion());
      }
    }

    B.UpdateEdge(E3, C3, C4, Surf, Loc, /*Tol3d*/ Precision::Confusion());
    E4 = E3;
    E4.Reverse();
  }
  else
  {
    if (!WithE3)
    {
      C3d = Surf->VIso(f1);
      E3  = BuildEdge(C3d, C3, Surf, Vf, V1, 0, Angle, /*Tol3d*/ Precision::Confusion());
    }
    else
    {
      BRepAdaptor_Curve aCurve(E3);
      double            AngleOld = aCurve.LastParameter();
      if (Angle > AngleOld)
      {
        B.Range(E3, 0, Angle);
        TopoDS_Vertex              V(TopExp::LastVertex(E3));
        occ::handle<BRep_TVertex>& TVlast = *((occ::handle<BRep_TVertex>*)&V.TShape());
        TVlast->Tolerance(Precision::Confusion());
      }

      B.UpdateEdge(E3, C3, Surf, Loc, /*Tol3d*/ Precision::Confusion());
    }

    if (!WithE4)
    {
      C3d = Surf->VIso(l1);
      E4  = BuildEdge(C3d, C4, Surf, Vl, V2, 0, Angle, /*Tol3d*/ Precision::Confusion());
    }
    else
    {
      BRepAdaptor_Curve aCurve(E4);
      double            AngleOld = aCurve.LastParameter();
      if (Angle > AngleOld)
      {
        B.Range(E4, 0, Angle);
        TopoDS_Vertex              V(TopExp::LastVertex(E4));
        occ::handle<BRep_TVertex>& TVlast = *((occ::handle<BRep_TVertex>*)&V.TShape());
        TVlast->Tolerance(Precision::Confusion());
      }

      B.UpdateEdge(E4, C4, Surf, Loc, /*Tol3d*/ Precision::Confusion());
    }
  }

  // Construct face
  BuildFace(Surf, E1, E3, E2, E4, EEmap, false, false, Result);

  // Set the continuities.
  B.Continuity(E1, TopoDS::Face(F1), Result, GeomAbs_G1);
  B.Continuity(E2, TopoDS::Face(F2), Result, GeomAbs_G1);

  // Render the calculated borders.
  //  if (!BT.Degenerated(E3))
  if (!BRep_Tool::Degenerated(E3))
    Aux1 = E3;
  else
    B.MakeEdge(Aux1); // Nullify

  //  if (!BT.Degenerated(E4))
  if (!BRep_Tool::Degenerated(E4))
    Aux2 = E4;
  else
    B.MakeEdge(Aux2);

  // Set the orientation

  // Provide correct normals computation
  // (the normal will be computed not in
  // singularity point definitely).
  Angle = RealFirst();
  for (int i = 0; i < 3; i++)
  {
    gp_Vec D1U, D1V, N1, N2;
    C1->D0(aPrm[i], P2d);
    Surf->D1(P2d.X(), P2d.Y(), P, D1U, D1V);
    N1 = D1U ^ D1V;

    if (N1.SquareMagnitude() < Precision::SquareConfusion())
      continue;

    //  C1 = BT.CurveOnSurface(E1, TopoDS::Face(F1), f2, l2);
    C1 = BRep_Tool::CurveOnSurface(E1, TopoDS::Face(F1), f2, l2);
    C1->D0(aPrm[i], P2d);
    occ::handle<BRepAdaptor_Surface> AS = new BRepAdaptor_Surface(TopoDS::Face(F1));
    AS->D1(P2d.X(), P2d.Y(), P, D1U, D1V);
    N2 = D1U ^ D1V;

    if (N2.SquareMagnitude() < Precision::SquareConfusion())
      continue;

    Angle = N1.Angle(N2);

    break;
  }

  if (Angle == RealFirst())
    return false;

  if ((F1.Orientation() == TopAbs_REVERSED) ^ (Angle > M_PI / 2))
    Result.Orientation(TopAbs_REVERSED);
  else
    Result.Orientation(TopAbs_FORWARD);

  if (ToReverseResult)
    Result.Reverse();

  return true;
}

//=================================================================================================

static void Substitute(BRepTools_Substitution& aSubstitute,
                       const TopoDS_Edge&      Old,
                       const TopoDS_Edge&      New)
{
  NCollection_List<TopoDS_Shape> listShape;

  TopoDS_Vertex OldV1, OldV2, NewV1, NewV2;
  TopExp::Vertices(Old, OldV1, OldV2);
  TopExp::Vertices(New, NewV1, NewV2);

  if (!aSubstitute.IsCopied(OldV1))
  {
    listShape.Append(NewV1.Oriented(TopAbs_FORWARD));
    aSubstitute.Substitute(OldV1, listShape);
    listShape.Clear();
  }
  if (!aSubstitute.IsCopied(OldV2))
  {
    listShape.Append(NewV2.Oriented(TopAbs_FORWARD));
    aSubstitute.Substitute(OldV2, listShape);
    listShape.Clear();
  }
  if (!aSubstitute.IsCopied(Old))
  {
    listShape.Append(New.Oriented(TopAbs_FORWARD));
    aSubstitute.Substitute(Old, listShape);
  }
}

//=======================================================================
// Function : SetCommonEdgeInFace
// Purpose  : Replace an edge of the face by the corresponding edge from
//           myUEdges
//=======================================================================
/*
static void SetCommonEdgeInFace(BRepTools_Substitution& aSubstitute,
                const TopoDS_Shape& Face,
                const TopoDS_Shape& Edge)
{
  if (Edge.IsNull())
    return;

  bool done = false;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool BT;
  double f, l;
  TopExp_Explorer Exp(Face, TopAbs_EDGE);
  occ::handle<Geom_Curve> Cref, C;
  TopLoc_Location Lref, L;
//  Cref = BT.Curve(TopoDS::Edge(Edge), Lref, f, l);
  const TopoDS_Edge& NewEdge = TopoDS::Edge(Edge);
  Cref = BRep_Tool::Curve( NewEdge, Lref, f, l );

  for ( ; Exp.More() && !done; Exp.Next()) {
//    C = BT.Curve(TopoDS::Edge(Exp.Current()), L, f, l);
    const TopoDS_Edge& OldEdge = TopoDS::Edge(Exp.Current());
    C = BRep_Tool::Curve(OldEdge, L, f, l);
    if ((Cref==C) && (Lref == L)) {
      done = true;
      Substitute( aSubstitute, OldEdge, NewEdge );
    }
  }
#ifdef OCCT_DEBUG
  if (!done) std::cout << "Substitution of Edge failed" << std::endl;
#endif
}
*/

//=======================================================================
// Fonction : KeepEdge
// Objet : Find edges of the face supported by the same Curve.
//=======================================================================
static void KeepEdge(const TopoDS_Shape&             Face,
                     const TopoDS_Shape&             Edge,
                     NCollection_List<TopoDS_Shape>& List)
{
  List.Clear();
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  double                  f, l;
  TopExp_Explorer         Exp(Face, TopAbs_EDGE);
  occ::handle<Geom_Curve> Cref, C;
  TopLoc_Location         Lref, L;
  //  Cref = BT.Curve(TopoDS::Edge(Edge), Lref, f, l);
  Cref = BRep_Tool::Curve(TopoDS::Edge(Edge), Lref, f, l);

  for (; Exp.More(); Exp.Next())
  {
    //    C = BT.Curve(TopoDS::Edge(Exp.Current()), L, f, l);
    C = BRep_Tool::Curve(TopoDS::Edge(Exp.Current()), L, f, l);
    if ((Cref == C) && (Lref == L))
    {
      List.Append(Exp.Current());
    }
  }
}

//=======================================================================
// Function :
// Objet : Construct a vertex via an iso
//=======================================================================
static void BuildVertex(const occ::handle<Geom_Curve>& Iso,
                        const bool                     isfirst,
                        const double                   First,
                        const double                   Last,
                        TopoDS_Shape&                  Vertex)
{
  BRep_Builder B;
  double       val;

  if (isfirst)
    val = First;
  else
    val = Last;
  B.MakeVertex(TopoDS::Vertex(Vertex), Iso->Value(val), Precision::Confusion());
}

//=======================================================================
// Function :
// Objet : Construct an empty edge
//=======================================================================
static TopoDS_Edge NullEdge(TopoDS_Shape& Vertex)
{
  TopoDS_Edge  E;
  BRep_Builder B;
  B.MakeEdge(E);
  Vertex.Orientation(TopAbs_FORWARD);
  B.Add(E, Vertex);
  B.Add(E, Vertex.Reversed());
  B.Degenerated(E, true);
  return E;
}

//=======================================================================
// Function :
// Objet : Construct an edge via an iso
//=======================================================================
static TopoDS_Edge BuildEdge(const occ::handle<Geom_Surface>& S,
                             const bool                       isUiso,
                             const double                     ValIso,
                             const TopoDS_Shape&              VFirst,
                             const TopoDS_Shape&              VLast,
                             const double                     Tol)
{
  TopoDS_Edge             E;
  BRep_Builder            B;
  occ::handle<Geom_Curve> Iso;
  bool                    sing = false;
  if (isUiso)
  {
    Iso = S->UIso(ValIso);
  }
  else
  {
    Iso = S->VIso(ValIso);
  }

  if (VFirst.IsSame(VLast))
  { // Singular case ?
    gp_Pnt P;
    // Class BRep_Tool without fields and without Constructor :
    //    BRep_Tool BT;
    const TopoDS_Vertex& V = TopoDS::Vertex(VFirst);
    //    double tol = BT.Tolerance(V);
    double tol = BRep_Tool::Tolerance(V);
    if (Tol > tol)
      tol = Tol;
    Iso->D0((Iso->FirstParameter() + Iso->LastParameter()) / 2, P);
    //    if (P.Distance(BT.Pnt(V)) < tol) {
    if (P.Distance(BRep_Tool::Pnt(V)) < tol)
    {
      GeomAdaptor_Curve AC(Iso);
      sing = GCPnts_AbscissaPoint::Length(AC, tol / 4) < tol;
    }
  }

  if (sing)
  { // Singular case
    TopoDS_Shape V;
    V = VFirst;
    E = NullEdge(V);
    //    Iso.Nullify();
    //    B.UpdateEdge(E, Iso, Precision::Confusion());
    B.Degenerated(E, true);
  }

  else
  {
    // Construction Via 3d
    //    if (isUiso) {
    //      Iso = S->UIso(ValIso);
    gp_Pnt P1, P2;
    double p1, p2, p11, p12, p21, p22;
    bool   fwd = false;
    p1         = Iso->FirstParameter();
    p2         = Iso->LastParameter();
    P1         = Iso->Value(p1);
    P2         = Iso->Value(p2);

    double t1 = BRep_Tool::Tolerance(TopoDS::Vertex(VFirst));
    double t2 = BRep_Tool::Tolerance(TopoDS::Vertex(VLast));

    BRep_Builder BB;

    p11 = P1.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VFirst)));
    p22 = P2.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VLast)));
    p12 = P1.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VLast)));
    p21 = P2.Distance(BRep_Tool::Pnt(TopoDS::Vertex(VFirst)));

    if (p11 < p12 && p22 < p21)
      fwd = true;

    if (fwd)
    { // OCC500(apo)
      if (p11 >= t1)
        BB.UpdateVertex(TopoDS::Vertex(VFirst), 1.01 * p11);
      if (p22 >= t2)
        BB.UpdateVertex(TopoDS::Vertex(VLast), 1.01 * p22);
    }
    else
    {
      //      Iso = S->VIso(ValIso);
      if (p12 >= t2)
        BB.UpdateVertex(TopoDS::Vertex(VLast), 1.01 * p12);
      if (p21 >= t1)
        BB.UpdateVertex(TopoDS::Vertex(VFirst), 1.01 * p21);
    }

    BRepLib_MakeEdge MkE;

    //    MkE.Init(Iso,
    //	     TopoDS::Vertex(VFirst),
    //	     TopoDS::Vertex(VLast),
    //	     Iso->FirstParameter(),
    //	     Iso->LastParameter());
    if (fwd)
      MkE.Init(Iso,
               TopoDS::Vertex(VFirst),
               TopoDS::Vertex(VLast),
               Iso->FirstParameter(),
               Iso->LastParameter());
    else
      MkE.Init(Iso,
               TopoDS::Vertex(VLast),
               TopoDS::Vertex(VFirst),
               Iso->FirstParameter(),
               Iso->LastParameter());

    //    if (!MkE.IsDone()) { // Il faut peut etre permuter les Vertex
    //      MkE.Init(Iso,
    //	       TopoDS::Vertex(VLast),
    //	       TopoDS::Vertex(VFirst),
    //	       Iso->FirstParameter(),
    //	       Iso->LastParameter());
    //    }

    if (!MkE.IsDone())
    { // Erreur de construction !!
      throw Standard_ConstructionError("BRepFill_Sweep::BuildEdge");
    }

    E = MkE.Edge();
  }

  // Associate 2d
  occ::handle<Geom2d_Line> L;
  TopLoc_Location          Loc;
  double                   Umin, Umax, Vmin, Vmax;
  S->Bounds(Umin, Umax, Vmin, Vmax);
  if (isUiso)
  {
    // gp_Pnt2d P(ValIso, 0);
    gp_Pnt2d P(ValIso, Vmin - Iso->FirstParameter());
    gp_Vec2d V(0., 1.);
    L = new (Geom2d_Line)(P, V);
  }
  else
  {
    // gp_Pnt2d P(0., ValIso);
    gp_Pnt2d P(Umin - Iso->FirstParameter(), ValIso);
    gp_Vec2d V(1., 0.);
    L = new (Geom2d_Line)(P, V);
  }

  B.UpdateEdge(E, L, S, Loc, Precision::Confusion());
  if (sing)
    B.Range(E, S, Loc, Iso->FirstParameter(), Iso->LastParameter());

  double                           MaxTol = 1.e-4;
  double                           theTol;
  GeomAdaptor_Curve                GAiso(Iso);
  occ::handle<GeomAdaptor_Curve>   GAHiso = new GeomAdaptor_Curve(GAiso);
  GeomAdaptor_Surface              GAsurf(S);
  occ::handle<GeomAdaptor_Surface> GAHsurf = new GeomAdaptor_Surface(GAsurf);
  CheckSameParameter(GAHiso, L, GAHsurf, MaxTol, theTol);
  B.UpdateEdge(E, theTol);

  return E;
}

//=======================================================================
// Function :
// Objet : Complete an edge via an iso
//=======================================================================
static void UpdateEdge(TopoDS_Edge&                     E,
                       const occ::handle<Geom_Surface>& S,
                       const bool                       isUiso,
                       const double                     ValIso)
{
  BRep_Builder              B;
  occ::handle<Geom2d_Line>  L;
  occ::handle<Geom2d_Curve> PCurve, CL;
  TopLoc_Location           Loc;
  double                    UFirst, ULast, VFirst, VLast, F2d, L2d;
  S->Bounds(UFirst, ULast, VFirst, VLast);

  bool                    sing = false;
  occ::handle<Geom_Curve> Iso;
  if (isUiso)
  {
    Iso = S->UIso(ValIso);
  }
  else
  {
    Iso = S->VIso(ValIso);
  }

  TopoDS_Vertex Vf, Vl;
  TopExp::Vertices(E, Vf, Vl);
  if (Vf.IsSame(Vl))
  { // Singular case ?
    gp_Pnt Pmid;
    double tol = BRep_Tool::Tolerance(Vf);
    Iso->D0((Iso->FirstParameter() + Iso->LastParameter()) / 2, Pmid);
    if (Pmid.Distance(BRep_Tool::Pnt(Vf)) < tol)
    {
      GeomAdaptor_Curve AC(Iso);
      sing = GCPnts_AbscissaPoint::Length(AC, tol / 4) < tol;
    }
  }

  if (isUiso)
  {
    gp_Pnt2d P(ValIso, 0);
    gp_Vec2d V(0., 1.);
    L   = new (Geom2d_Line)(P, V);
    F2d = VFirst;
    L2d = VLast;
  }
  else
  {
    gp_Pnt2d P(0., ValIso);
    gp_Vec2d V(1., 0.);
    L   = new (Geom2d_Line)(P, V);
    F2d = UFirst;
    L2d = ULast;
  }
  CL = new (Geom2d_TrimmedCurve)(L, F2d, L2d);

  // Control direction & Range
  double R, First, Last, Tol = 1.e-4;
  bool   reverse = false;

  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  gp_Pnt   POnS;
  gp_Pnt2d P2d;
  //  BT.Range(E, First, Last);
  BRep_Tool::Range(E, First, Last);

  if (!Vf.IsSame(Vl))
  {
    // Test distances between "FirstPoint" and "Vertex"
    P2d  = CL->Value(F2d);
    POnS = S->Value(P2d.X(), P2d.Y());
    //    reverse = POnS.Distance(BT.Pnt(Vl)) < POnS.Distance(BT.Pnt(Vf));
    reverse = POnS.Distance(BRep_Tool::Pnt(Vl)) < POnS.Distance(BRep_Tool::Pnt(Vf));
  }
  else if (!sing)
  {
    // Test angle between "First Tangente"
    gp_Vec2d          V2d;
    gp_Vec            V3d, du, dv, dC3d;
    BRepAdaptor_Curve C3d(E);

    C3d.D1(First, POnS, dC3d);
    CL->D1(F2d, P2d, V2d);
    S->D1(P2d.X(), P2d.Y(), POnS, du, dv);
    V3d.SetLinearForm(V2d.X(), du, V2d.Y(), dv);
    reverse = (dC3d.Angle(V3d) > Tol);
  }
  if (reverse)
  { // Return curve 2d
    CL = new (Geom2d_TrimmedCurve)(L, F2d, L2d);
    CL->Reverse();
    F2d = CL->FirstParameter();
    L2d = CL->LastParameter();
  }

  if (sing)
  {
    occ::handle<Geom_Curve> NullCurve;
    B.UpdateEdge(E, NullCurve, 0.);
    B.Degenerated(E, true);
    B.Range(E, F2d, L2d);
    First = F2d;
    Last  = L2d;
  }

  if (First != F2d || Last != L2d)
  {
    occ::handle<Geom2d_Curve> C2d;
    GeomLib::SameRange(Precision::PConfusion(), CL, F2d, L2d, First, Last, C2d);
    CL = new (Geom2d_TrimmedCurve)(C2d, First, Last);
  }

  // Update des Vertex

  TopoDS_Vertex V;

  P2d  = CL->Value(First);
  POnS = S->Value(P2d.X(), P2d.Y());
  V    = TopExp::FirstVertex(E);
  //  R = POnS.Distance(BT.Pnt(V));
  R = POnS.Distance(BRep_Tool::Pnt(V));
  B.UpdateVertex(V, R);

  P2d  = CL->Value(Last);
  POnS = S->Value(P2d.X(), P2d.Y());
  V    = TopExp::LastVertex(E);
  //  R = POnS.Distance(BT.Pnt(V));
  R = POnS.Distance(BRep_Tool::Pnt(V));
  B.UpdateVertex(V, R);

  // Update Edge
  if (!sing && SameParameter(E, CL, S, Tol, R))
  {
    B.UpdateEdge(E, R);
  }

  PCurve = Couture(E, S, Loc);
  if (PCurve.IsNull())
    B.UpdateEdge(E, CL, S, Loc, Precision::Confusion());
  else
  { // Sewing edge
    TopoDS_Edge e = E;
    Oriente(S, e);
    if (e.Orientation() == TopAbs_REVERSED)
      B.UpdateEdge(E, CL, PCurve, S, Loc, Precision::Confusion());
    else
      B.UpdateEdge(E, PCurve, CL, S, Loc, Precision::Confusion());
  }

  // Attention to case not SameRange on its shapes (PRO13551)
  //  if (!BT.SameRange(E)) B.Range(E, S, Loc, First, Last);
  if (!BRep_Tool::SameRange(E))
    B.Range(E, S, Loc, First, Last);
}

//=======================================================================
// Object : Check if a surface is degenerated
//=======================================================================
static bool IsDegen(const occ::handle<Geom_Surface>& S, const double Tol)
{
  int                     Nb = 5;
  bool                    B  = true;
  double                  Umax, Umin, Vmax, Vmin, t, dt, l;
  int                     ii;
  occ::handle<Geom_Curve> Iso;
  gp_Pnt                  P1, P2, P3;
  GCPnts_AbscissaPoint    GC;

  S->Bounds(Umin, Umax, Vmin, Vmax);

  // Check the length of Iso-U
  t = (Umin + Umax) / 2;
  S->D0(t, Vmin, P1);
  S->D0(t, (Vmin + Vmax) / 2, P2);
  S->D0(t, Vmax, P3);
  B = ((P1.Distance(P2) + P2.Distance(P3)) < Tol);

  for (ii = 1, dt = (Umax - Umin) / (Nb + 1); B && (ii <= Nb); ii++)
  {
    t   = Umin + ii * dt;
    Iso = S->UIso(t);
    GeomAdaptor_Curve AC(Iso);
    l = GC.Length(AC, Tol / 4);
    B = (l <= Tol);
  }

  if (B)
    return true;

  // Check the length of Iso-V
  t = (Vmin + Vmax) / 2;
  S->D0(Umin, t, P1);
  S->D0((Umin + Umax) / 2, t, P2);
  S->D0(Umax, t, P3);
  B = ((P1.Distance(P2) + P2.Distance(P3)) < Tol);

  for (ii = 1, dt = (Vmax - Vmin) / (Nb + 1); B && (ii <= Nb); ii++)
  {
    t   = Vmin + ii * dt;
    Iso = S->VIso(t);
    GeomAdaptor_Curve AC(Iso);
    l = GC.Length(AC, Tol / 4);
    B = (l <= Tol);
  }

  return B;
}

static void ReverseEdgeInFirstOrLastWire(TopoDS_Shape& theWire, const TopoDS_Shape& theEdge)
{
  TopoDS_Shape    EdgeToReverse;
  TopoDS_Iterator itw(theWire);

  for (; itw.More(); itw.Next())
  {
    const TopoDS_Shape& anEdge = itw.Value();
    if (anEdge.IsSame(theEdge))
    {
      EdgeToReverse = anEdge;
      break;
    }
  }

  if (!EdgeToReverse.IsNull())
  {
    BRep_Builder BB;
    theWire.Free(true);
    BB.Remove(theWire, EdgeToReverse);
    EdgeToReverse.Reverse();
    BB.Add(theWire, EdgeToReverse);
  }
}

static void ReverseModifiedEdges(
  TopoDS_Wire&                                                  theWire,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theEmap)
{
  if (theEmap.IsEmpty())
    return;

  TopoDS_Iterator itw(theWire);
  BRep_Builder    BB;

  NCollection_List<TopoDS_Shape> Ledges;
  for (; itw.More(); itw.Next())
    Ledges.Append(itw.Value());

  theWire.Free(true);
  NCollection_List<TopoDS_Shape>::Iterator itl(Ledges);
  for (; itl.More(); itl.Next())
    BB.Remove(theWire, itl.Value());

  for (itl.Initialize(Ledges); itl.More(); itl.Next())
  {
    TopoDS_Shape anEdge = itl.Value();
    if (theEmap.Contains(anEdge))
      anEdge.Reverse();
    BB.Add(theWire, anEdge);
  }
}

//=================================================================================================

BRepFill_Sweep::BRepFill_Sweep(const occ::handle<BRepFill_SectionLaw>&  Section,
                               const occ::handle<BRepFill_LocationLaw>& Location,
                               const bool                               WithKPart)
    : isDone(false),
      KPart(WithKPart)
{
  mySec = Section;
  myLoc = Location;

  SetTolerance(1.e-4);
  SetAngularControl();
  myAuxShape.Clear();

  myApproxStyle   = GeomFill_Location;
  myContinuity    = GeomAbs_C2;
  myDegmax        = 11;
  mySegmax        = 30;
  myForceApproxC1 = false;
}

//=======================================================================
// function : SetBounds
// purpose  : Define start and end shapes
//======================================================================
void BRepFill_Sweep::SetBounds(const TopoDS_Wire& First, const TopoDS_Wire& Last)
{
  FirstShape = First;
  LastShape  = Last;

  // It is necessary to check the SameRange on its (PRO13551)
#ifdef OCCT_DEBUG
  bool issame = true;
#endif
  BRep_Builder           B;
  BRepTools_WireExplorer wexp;
  if (!FirstShape.IsNull())
  {
    for (wexp.Init(FirstShape); wexp.More(); wexp.Next())
    {
      if (!BRepLib::CheckSameRange(wexp.Current()))
      {
        B.SameRange(wexp.Current(), false);
        B.SameParameter(wexp.Current(), false);
#ifdef OCCT_DEBUG
        issame = false;
#endif
      }
    }
  }

  if (!LastShape.IsNull())
  {
    for (wexp.Init(LastShape); wexp.More(); wexp.Next())
    {
      if (!BRepLib::CheckSameRange(wexp.Current()))
      {
        B.SameRange(wexp.Current(), false);
        B.SameParameter(wexp.Current(), false);
#ifdef OCCT_DEBUG
        issame = false;
#endif
      }
    }
  }

#ifdef OCCT_DEBUG
  if (!issame)
    std::cout << "Sweep Warning : Edge not SameRange in the limits" << std::endl;
#endif
}

//=================================================================================================

void BRepFill_Sweep::SetTolerance(const double Tol3d,
                                  const double BoundTol,
                                  const double Tol2d,
                                  const double TolAngular)
{
  myTol3d      = Tol3d;
  myBoundTol   = BoundTol;
  myTol2d      = Tol2d;
  myTolAngular = TolAngular;
}

//=================================================================================================

void BRepFill_Sweep::SetAngularControl(const double MinAngle, const double MaxAngle)
{
  myAngMin = std::max(MinAngle, Precision::Angular());
  myAngMax = std::min(MaxAngle, 6.28);
}

//=======================================================================
// function : SetForceApproxC1
// purpose  : Set the flag that indicates attempt to approximate
//           a C1-continuous surface if a swept surface proved
//           to be C0.
//=======================================================================
void BRepFill_Sweep::SetForceApproxC1(const bool ForceApproxC1)
{
  myForceApproxC1 = ForceApproxC1;
}

///=================================================================================================

bool BRepFill_Sweep::CorrectApproxParameters()
{
  TopoDS_Wire   thePath   = myLoc->Wire();
  GeomAbs_Shape NewCont   = myContinuity;
  int           NewSegmax = mySegmax;

  TopoDS_Iterator iter(thePath);
  for (; iter.More(); iter.Next())
  {
    TopoDS_Edge       anEdge = TopoDS::Edge(iter.Value());
    BRepAdaptor_Curve aBAcurve(anEdge);
    GeomAbs_Shape     aContinuity = aBAcurve.Continuity();
    int               aNbInterv   = aBAcurve.NbIntervals(GeomAbs_CN);
    if (aContinuity < NewCont)
      NewCont = aContinuity;
    if (aNbInterv > NewSegmax)
      NewSegmax = aNbInterv;
  }

  bool Corrected = false;
  if (NewCont != myContinuity || NewSegmax != mySegmax)
    Corrected = true;
  myContinuity = NewCont;
  mySegmax     = NewSegmax;
  return Corrected;
}

//=======================================================================
// function : BuildWire
// purpose  : Construit a wire by sweeping
//======================================================================
bool BRepFill_Sweep::BuildWire(const BRepFill_TransitionStyle /*Transition*/)
{
  int    ipath, isec = 1;
  gp_Pnt P1; //, P2;

  BRep_Builder B;
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  int  NbPath = myLoc->NbLaw();
  bool vclose;
  vclose = (myLoc->IsClosed() && (myLoc->IsG1(0, myTol3d) >= 0));
  Error  = 0.;
  occ::handle<Geom_Surface> S;
  occ::handle<Geom_Curve>   Iso;
  double                    val, bid, First, Last, Tol;

  TopoDS_Wire wire;
  TopoDS_Edge E;
  B.MakeWire(wire);

  // (1) Construction of all curves

  // (1.1) Construction of Tables
  myFaces  = new (NCollection_HArray2<TopoDS_Shape>)(1, 1, 1, NbPath);
  myUEdges = new (NCollection_HArray2<TopoDS_Shape>)(1, 2, 1, NbPath);
  myVEdges = new (NCollection_HArray2<TopoDS_Shape>)(1, 1, 1, NbPath + 1);

  // (1.2) Calculate curves / vertex / edge
  for (ipath = 1; ipath <= NbPath; ipath++)
  {
    // Curve by iso value
    GeomFill_Sweep Sweep(myLoc->Law(ipath), KPart);
    Sweep.SetTolerance(myTol3d, myBoundTol, myTol2d, myTolAngular);
    Sweep.SetForceApproxC1(myForceApproxC1);
    Sweep.Build(mySec->Law(isec), myApproxStyle, myContinuity, myDegmax, mySegmax);
    if (!Sweep.IsDone())
      return false;
    S = Sweep.Surface();
    if (Sweep.ExchangeUV())
    {
      if (Sweep.UReversed())
        S->Bounds(First, Last, bid, val);
      else
        S->Bounds(First, Last, val, bid);
      Iso = S->VIso(val);
    }
    else
    {
      if (Sweep.UReversed())
        S->Bounds(bid, val, First, Last);
      else
        S->Bounds(val, bid, First, Last);
      Iso = S->UIso(val);
    }
    // Vertex by position
    if (ipath < NbPath)
      BuildVertex(Iso, false, First, Last, myVEdges->ChangeValue(1, ipath + 1));
    else
    {
      if (vclose)
      {
        TopoDS_Vertex& V = TopoDS::Vertex(myVEdges->ChangeValue(1, 1));
        myVEdges->SetValue(1, ipath + 1, V);
        Iso->D0(Last, P1);
        //	Tol = P1.Distance(BT.Pnt(V));
        Tol = P1.Distance(BRep_Tool::Pnt(V));
        B.UpdateVertex(V, Tol);
      }
      else
      {
        if (!LastShape.IsNull())
          myVEdges->SetValue(1, NbPath, FirstShape);
        else
          BuildVertex(Iso, false, First, Last, myVEdges->ChangeValue(1, NbPath + 1));
      }
    }

    if (ipath > 1)
    {
      Iso->D0(First, P1);
      TopoDS_Vertex& V = TopoDS::Vertex(myVEdges->ChangeValue(1, ipath));
      //      Tol = P1.Distance(BT.Pnt(V));
      Tol = P1.Distance(BRep_Tool::Pnt(V));
      B.UpdateVertex(V, Tol);
    }
    if (ipath == 1)
    {
      if (!FirstShape.IsNull())
        myVEdges->SetValue(1, 1, FirstShape);
      else
        BuildVertex(Iso, true, First, Last, myVEdges->ChangeValue(1, 1));
    }

    // Construction of the edge
    BRepLib_MakeEdge MkE;
    MkE.Init(Iso,
             TopoDS::Vertex(myVEdges->Value(1, ipath)),
             TopoDS::Vertex(myVEdges->Value(1, ipath + 1)),
             Iso->FirstParameter(),
             Iso->LastParameter());
    if (!MkE.IsDone())
    { // Error of construction !!
      return false;
    }
    E = MkE.Edge();
    B.UpdateEdge(E, Sweep.ErrorOnSurface());
    B.Add(wire, E);
    myFaces->SetValue(1, ipath, E);
  }
  myShape = wire;
  return true;
}

//=======================================================================
// function : BuildShell
// purpose  : Construct a Shell by sweeping
//======================================================================
bool BRepFill_Sweep::BuildShell(
  const BRepFill_TransitionStyle /*Transition*/,
  const int                                               IFirst,
  const int                                               ILast,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& ReversedEdges,
  NCollection_DataMap<TopoDS_Shape,
                      occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                      TopTools_ShapeMapHasher>&           Tapes,
  NCollection_DataMap<TopoDS_Shape,
                      occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                      TopTools_ShapeMapHasher>&           Rails,
  const double                                            ExtendFirst,
  const double                                            ExtendLast)
{
  int          ipath, isec, IPath;
  BRep_Builder B;
  int          NbPath = ILast - IFirst;
  int          NbLaw  = mySec->NbLaw();
  bool         uclose, vclose, global_vclose, constSection, hasdegen = false;
  constSection  = mySec->IsConstant();
  uclose        = mySec->IsUClosed();
  global_vclose = (myLoc->IsClosed()) && (myLoc->IsG1(0, myTol3d) >= 0);
  vclose        = global_vclose && (mySec->IsVClosed()) && (NbPath == myLoc->NbLaw());
  Error         = 0.;

  // (1) Construction of all surfaces

  // (1.1) Construction of Tables

  NCollection_Array2<bool> ExchUV(1, NbLaw, 1, NbPath);
  NCollection_Array2<bool> UReverse(1, NbLaw, 1, NbPath);
  NCollection_Array2<bool> Degenerated(1, NbLaw, 1, NbPath);
  Degenerated.Init(false);
  // No VReverse for the moment...
  NCollection_Array2<double>                    TabErr(1, NbLaw, 1, NbPath);
  NCollection_Array2<occ::handle<Geom_Surface>> TabS(1, NbLaw, 1, NbPath);

  NCollection_Array2<TopoDS_Shape> UEdge(1, NbLaw + 1, 1, NbPath);
  NCollection_Array2<TopoDS_Shape> VEdge(1, NbLaw, 1, NbPath + 1);
  NCollection_Array2<TopoDS_Shape> Vertex(1, NbLaw + 1, 1, NbPath + 1);

  TopoDS_Vertex VNULL;
  VNULL.Nullify();
  Vertex.Init(VNULL);

  NCollection_Array1<TopoDS_Shape> SecVertex(1, NbLaw + 1);
  NCollection_Array1<double>       VError(1, NbLaw + 1);
  NCollection_Array1<double>       Vi(1, NbPath + 1);

  // Initialization of management of parametric intervals
  //(Case of evolutionary sections)
  double Length, SecDom, SecDeb;
  myLoc->CurvilinearBounds(myLoc->NbLaw(), SecDom, Length);
  mySec->Law(1)->GetDomain(SecDeb, SecDom);
  SecDom -= SecDeb;
  if (IFirst > 1)
  {
    double Lf, Ll;
    myLoc->CurvilinearBounds(IFirst - 1, Lf, Ll);
    Vi(1) = SecDeb + (Ll / Length) * SecDom;
  }
  else
    Vi(1) = SecDeb;

  // Error a priori on vertices
  if (constSection)
  {
    for (isec = 1; isec <= NbLaw + 1; isec++)
    {
      VError(isec)    = mySec->VertexTol(isec - 1, 0.);
      SecVertex(isec) = mySec->Vertex(isec, 0.);
    }
  }

  // (1.2) Calculate surfaces
  for (ipath = 1, IPath = IFirst; ipath <= NbPath; ipath++, IPath++)
  {

    GeomFill_Sweep Sweep(myLoc->Law(IPath), KPart);
    Sweep.SetTolerance(myTol3d, myBoundTol, myTol2d, myTolAngular);
    Sweep.SetForceApproxC1(myForceApproxC1);

    // Case of evolutionary section, definition of parametric correspondence
    if (!constSection)
    {
      double lf, ll, Lf, Ll;
      myLoc->Law(IPath)->GetDomain(lf, ll);
      myLoc->CurvilinearBounds(IPath, Lf, Ll);
      Vi(ipath + 1) = SecDeb + (Ll / Length) * SecDom;
      Sweep.SetDomain(lf, ll, Vi(ipath), Vi(ipath + 1));
    }
    else // section is constant
    {
      double lf, ll, Lf, Ll;
      myLoc->Law(IPath)->GetDomain(lf, ll);
      myLoc->CurvilinearBounds(IPath, Lf, Ll);
      Vi(ipath + 1) = SecDeb + (Ll / Length) * SecDom;
    }

    for (isec = 1; isec <= NbLaw; isec++)
    {
      Sweep.Build(mySec->Law(isec), myApproxStyle, myContinuity, myDegmax, mySegmax);
      if (!Sweep.IsDone())
        return false;
      TabS(isec, ipath)     = Sweep.Surface();
      TabErr(isec, ipath)   = Sweep.ErrorOnSurface();
      ExchUV(isec, ipath)   = Sweep.ExchangeUV();
      UReverse(isec, ipath) = Sweep.UReversed();
      if (Sweep.ErrorOnSurface() > Error)
        Error = Sweep.ErrorOnSurface();

      if ((ipath == 1) && (ExtendFirst > 0))
      {
        occ::handle<Geom_BoundedSurface> BndS;
        BndS = occ::down_cast<Geom_BoundedSurface>(TabS(isec, ipath));
        GeomLib::ExtendSurfByLength(BndS, ExtendFirst, 1, Sweep.ExchangeUV(), false);
        TabS(isec, ipath) = BndS;
      }
      if ((ipath == NbPath) && (ExtendLast > 0))
      {
        occ::handle<Geom_BoundedSurface> BndS;
        BndS = occ::down_cast<Geom_BoundedSurface>(TabS(isec, ipath));
        GeomLib::ExtendSurfByLength(BndS, ExtendLast, 1, Sweep.ExchangeUV(), true);
        TabS(isec, ipath) = BndS;
      }
    }
  }

  // (2) Construction of Edges
  double                    UFirst, ULast, VFirst, VLast;
  bool                      exuv, singu, singv;
  occ::handle<Geom_Surface> S;

  // Correct <FirstShape> and <LastShape>: reverse modified edges
  ReverseModifiedEdges(FirstShape, ReversedEdges);
  ReverseModifiedEdges(LastShape, ReversedEdges);

  // (2.0) return preexisting Edges and vertices
  TopoDS_Edge              E;
  NCollection_Array1<bool> IsBuilt(1, NbLaw);
  IsBuilt.Init(false);
  NCollection_Array1<TopoDS_Shape> StartEdges(1, NbLaw);
  if (!FirstShape.IsNull() && (IFirst == 1))
  {
    mySec->Init(FirstShape);
    for (isec = 1; isec <= NbLaw; isec++)
    {
      E = mySec->CurrentEdge();
      TopoDS_Vertex Vfirst, Vlast;
      TopExp::Vertices(E, Vfirst, Vlast);
      VEdge(isec, 1) = E;
      if (E.Orientation() == TopAbs_REVERSED)
        Vertex(isec + 1, 1) = Vfirst; // TopExp::FirstVertex(E);
      else
        Vertex(isec + 1, 1) = Vlast; // TopExp::LastVertex(E);
      UpdateVertex(IFirst - 1, isec + 1, TabErr(isec, 1), Vi(1), Vertex(isec + 1, 1));

      StartEdges(isec) = E;
      if (Tapes.IsBound(E))
      {
        IsBuilt(isec) = true;

        // Initialize VEdge, UEdge, Vertex and myFaces
        int j;
        for (j = 1; j <= NbPath + 1; j++)
        {
          VEdge(isec, j) = Tapes(E)->Value(1, j);
          VEdge(isec, j).Reverse(); // direction of round is reversed
        }
        int ifirst = isec + 1, ilast = isec; // direction of round is reversed
        for (j = 1; j <= NbPath; j++)
          UEdge(ifirst, j) = Tapes(E)->Value(2, j);
        for (j = 1; j <= NbPath; j++)
          UEdge(ilast, j) = Tapes(E)->Value(3, j);
        for (j = 1; j <= NbPath + 1; j++)
          Vertex(ifirst, j) = Tapes(E)->Value(4, j);
        for (j = 1; j <= NbPath + 1; j++)
          Vertex(ilast, j) = Tapes(E)->Value(5, j);
        for (j = 1; j <= NbPath; j++)
          myFaces->SetValue(isec, j, Tapes(E)->Value(6, j));

        if (uclose && isec == 1)
        {
          for (j = 1; j <= NbPath; j++)
            UEdge(NbLaw + 1, j) = UEdge(1, j);
          for (j = 1; j <= NbPath + 1; j++)
            Vertex(NbLaw + 1, j) = Vertex(1, j);
        }
        if (uclose && isec == NbLaw)
        {
          for (j = 1; j <= NbPath; j++)
            UEdge(1, j) = UEdge(NbLaw + 1, j);
          for (j = 1; j <= NbPath + 1; j++)
            Vertex(1, j) = Vertex(NbLaw + 1, j);
        }
      }
      else
      {
        occ::handle<NCollection_HArray2<TopoDS_Shape>> EmptyArray =
          new NCollection_HArray2<TopoDS_Shape>(1, 6, 1, NbPath + 1);
        Tapes.Bind(E, EmptyArray);
        int j;
        if (Rails.IsBound(Vfirst))
        {
          int ind = (E.Orientation() == TopAbs_REVERSED) ? isec + 1 : isec;
          for (j = 1; j <= NbPath; j++)
            UEdge(ind, j) = Rails(Vfirst)->Value(1, j);
          for (j = 1; j <= NbPath + 1; j++)
            Vertex(ind, j) = Rails(Vfirst)->Value(2, j);
        }
        if (Rails.IsBound(Vlast))
        {
          int ind = (E.Orientation() == TopAbs_FORWARD) ? isec + 1 : isec;
          for (j = 1; j <= NbPath; j++)
            UEdge(ind, j) = Rails(Vlast)->Value(1, j);
          for (j = 1; j <= NbPath + 1; j++)
            Vertex(ind, j) = Rails(Vlast)->Value(2, j);
        }
      }
    }

    if (VEdge(1, 1).Orientation() == TopAbs_REVERSED)
      Vertex(1, 1) = TopExp::LastVertex(TopoDS::Edge(VEdge(1, 1)));
    else
      Vertex(1, 1) = TopExp::FirstVertex(TopoDS::Edge(VEdge(1, 1)));
    UpdateVertex(IFirst - 1, 1, TabErr(1, 1), Vi(1), Vertex(1, 1));
  }

  double u, v, aux;
  bool   ureverse;
  for (isec = 1; isec <= NbLaw + 1; isec++)
  {
    // Return data
    if (isec > NbLaw)
    {
      S        = TabS(NbLaw, 1);
      ureverse = UReverse(NbLaw, 1);
      exuv     = ExchUV(NbLaw, 1);
    }
    else
    {
      S        = TabS(isec, 1);
      ureverse = UReverse(isec, 1);
      exuv     = ExchUV(isec, 1);
    }
    S->Bounds(UFirst, ULast, VFirst, VLast);

    // Choice of parameters
    if (ureverse)
    {
      if (exuv)
      {
        aux    = VFirst;
        VFirst = VLast;
        VLast  = aux;
      }
      else
      {
        aux    = UFirst;
        UFirst = ULast;
        ULast  = aux;
      }
    }
    if (isec != NbLaw + 1)
    {
      u = UFirst;
      v = VFirst;
    }
    else
    {
      if (exuv)
      {
        u = UFirst;
        v = VLast;
      }
      else
      {
        u = ULast;
        v = VFirst;
      }
    }

    // construction of vertices
    if (Vertex(isec, 1).IsNull())
      B.MakeVertex(TopoDS::Vertex(Vertex(isec, 1)),
                   S->Value(u, v),
                   mySec->VertexTol(isec - 1, Vi(1)));
    else
    {
      TopLoc_Location Identity;
      Vertex(isec, 1).Location(Identity);
      B.UpdateVertex(TopoDS::Vertex(Vertex(isec, 1)),
                     S->Value(u, v),
                     mySec->VertexTol(isec - 1, Vi(1)));
    }
  } // end of for (isec=1; isec<=NbLaw+1; isec++)

  if (!LastShape.IsNull() && (ILast == myLoc->NbLaw() + 1))
  {
    mySec->Init(LastShape);
    for (isec = 1; isec <= NbLaw; isec++)
    {
      E = mySec->CurrentEdge();
      if (VEdge(isec, NbPath + 1).IsNull())
        VEdge(isec, NbPath + 1) = E;

      if (Vertex(isec + 1, NbPath + 1).IsNull())
      {
        if (VEdge(isec, NbPath + 1).Orientation() == TopAbs_REVERSED)
          Vertex(isec + 1, NbPath + 1) = TopExp::FirstVertex(TopoDS::Edge(VEdge(isec, NbPath + 1)));
        else
          Vertex(isec + 1, NbPath + 1) = TopExp::LastVertex(TopoDS::Edge(VEdge(isec, NbPath + 1)));
      }
      UpdateVertex(ILast - 1,
                   isec + 1,
                   TabErr(isec, NbPath),
                   Vi(NbPath + 1),
                   Vertex(isec + 1, NbPath + 1));
    }

    if (Vertex(1, NbPath + 1).IsNull())
    {
      if (VEdge(1, NbPath + 1).Orientation() == TopAbs_REVERSED)
        Vertex(1, NbPath + 1) = TopExp::LastVertex(TopoDS::Edge(VEdge(1, NbPath + 1)));
      else
        Vertex(1, NbPath + 1) = TopExp::FirstVertex(TopoDS::Edge(VEdge(1, NbPath + 1)));
    }
    UpdateVertex(ILast - 1, 1, TabErr(1, NbPath), Vi(NbPath + 1), Vertex(1, NbPath + 1));
  }

  for (isec = 1; isec <= NbLaw + 1; isec++)
  {
    // Return data
    if (isec > NbLaw)
    {
      S        = TabS(NbLaw, NbPath);
      ureverse = UReverse(NbLaw, NbPath);
      exuv     = ExchUV(NbLaw, NbPath);
    }
    else
    {
      S        = TabS(isec, NbPath);
      ureverse = UReverse(isec, NbPath);
      exuv     = ExchUV(isec, NbPath);
    }
    S->Bounds(UFirst, ULast, VFirst, VLast);

    // Choice of parametres
    if (ureverse)
    {
      if (exuv)
      {
        aux    = VFirst;
        VFirst = VLast;
        VLast  = aux;
      }
      else
      {
        aux    = UFirst;
        UFirst = ULast;
        ULast  = aux;
      }
    }
    if (isec == NbLaw + 1)
    {
      u = ULast;
      v = VLast;
    }
    else
    {
      if (exuv)
      {
        u = ULast;
        v = VFirst;
      }
      else
      {
        u = UFirst;
        v = VLast;
      }
    }

    // construction of vertex
    if (Vertex(isec, NbPath + 1).IsNull())
      B.MakeVertex(TopoDS::Vertex(Vertex(isec, NbPath + 1)),
                   S->Value(u, v),
                   mySec->VertexTol(isec - 1, Vi(NbPath + 1)));
    else
    {
      TopLoc_Location Identity;
      Vertex(isec, NbPath + 1).Location(Identity);
      B.UpdateVertex(TopoDS::Vertex(Vertex(isec, NbPath + 1)),
                     S->Value(u, v),
                     mySec->VertexTol(isec - 1, Vi(NbPath + 1)));
    }
  } // end of for (isec=1; isec<=NbLaw+1; isec++)

  // ---------- Creation of Vertex and edge ------------
  for (ipath = 1, IPath = IFirst; ipath <= NbPath; ipath++, IPath++)
  {
    for (isec = 1; isec <= NbLaw; isec++)
    {
      if (IsBuilt(isec))
        continue;

      S    = TabS(isec, ipath);
      exuv = ExchUV(isec, ipath);
      S->Bounds(UFirst, ULast, VFirst, VLast);
      if (UReverse(isec, ipath))
      {
        double aux2;
        if (exuv)
        {
          aux2   = VFirst;
          VFirst = VLast;
          VLast  = aux2;
        }
        else
        {
          aux2   = UFirst;
          UFirst = ULast;
          ULast  = aux2;
        }
      }

      // (2.1) Construction of new vertices
      if (isec == 1)
      {
        if (ipath == 1 && Vertex(1, 1).IsNull())
        {
          // All first
          if (constSection)
            myLoc->PerformVertex(IPath - 1,
                                 TopoDS::Vertex(SecVertex(1)),
                                 VError(1),
                                 TopoDS::Vertex(Vertex(1, 1)));
          else
            myLoc->PerformVertex(IPath - 1,
                                 mySec->Vertex(1, Vi(1)),
                                 mySec->VertexTol(0, Vi(1)),
                                 TopoDS::Vertex(Vertex(1, 1)));
        }
        // the first and the next column
        if (vclose && (ipath == NbPath))
        {
          Vertex(1, ipath + 1) = Vertex(1, 1);
        }
        else if (Vertex(1, ipath + 1).IsNull())
        {
          if (constSection)
            myLoc->PerformVertex(IPath,
                                 TopoDS::Vertex(SecVertex(1)),
                                 TabErr(1, ipath) + VError(1),
                                 TopoDS::Vertex(Vertex(1, ipath + 1)));
          else
            myLoc->PerformVertex(IPath,
                                 mySec->Vertex(1, Vi(ipath + 1)),
                                 TabErr(1, ipath) + mySec->VertexTol(0, Vi(ipath + 1)),
                                 TopoDS::Vertex(Vertex(1, ipath + 1)));

          if (MergeVertex(Vertex(1, ipath), Vertex(1, ipath + 1)))
          {
            UEdge(1, ipath) = NullEdge(Vertex(1, ipath));
          }
        }
      }

      if (ipath == 1)
      {
        if (uclose && (isec == NbLaw))
        {
          Vertex(isec + 1, 1) = Vertex(1, 1);
        }
        else if (Vertex(isec + 1, 1).IsNull())
        {
          if (constSection)
            myLoc->PerformVertex(IPath - 1,
                                 TopoDS::Vertex(SecVertex(isec + 1)),
                                 TabErr(isec, 1) + VError(isec + 1),
                                 TopoDS::Vertex(Vertex(isec + 1, 1)));
          else
            myLoc->PerformVertex(IPath - 1,
                                 mySec->Vertex(isec + 1, Vi(1)),
                                 TabErr(isec, 1) + mySec->VertexTol(isec, Vi(1)),
                                 TopoDS::Vertex(Vertex(isec + 1, 1)));

          if (MergeVertex(Vertex(isec, 1), Vertex(isec + 1, 1)))
          {
            VEdge(isec, 1) = NullEdge(Vertex(isec, 1));
          }
        }
      }

      if (uclose && (isec == NbLaw))
      {
        Vertex(isec + 1, ipath + 1) = Vertex(1, ipath + 1);
      }
      else if (vclose && (ipath == NbPath))
      {
        Vertex(isec + 1, ipath + 1) = Vertex(isec + 1, 1);
      }
      else if (Vertex(isec + 1, ipath + 1).IsNull())
      {
        if (constSection)
          myLoc->PerformVertex(IPath,
                               TopoDS::Vertex(SecVertex(isec + 1)),
                               TabErr(isec, ipath) + VError(isec + 1),
                               TopoDS::Vertex(Vertex(isec + 1, ipath + 1)));
        else
          myLoc->PerformVertex(IPath,
                               mySec->Vertex(isec + 1, Vi(ipath + 1)),
                               TabErr(isec, ipath) + mySec->VertexTol(isec, Vi(ipath + 1)),
                               TopoDS::Vertex(Vertex(isec + 1, ipath + 1)));
      }

      // Singular cases
      singv = MergeVertex(Vertex(isec, ipath + 1), Vertex(isec + 1, ipath + 1));
      singu = MergeVertex(Vertex(isec + 1, ipath), Vertex(isec + 1, ipath + 1));

      if (singu || singv)
      {
        Degenerated(isec, ipath) =
          IsDegen(TabS(isec, ipath), std::max(myTol3d, TabErr(isec, ipath)));
      }
      if (Degenerated(isec, ipath))
      {
#ifdef OCCT_DEBUG
        std::cout << "Sweep : Degenerated case" << std::endl;
#endif
        hasdegen = true;
        // Particular construction of edges
        if (UEdge(isec + 1, ipath).IsNull())
        {
          if (singu)
          {
            // Degenerated edge
            UEdge(isec + 1, ipath) = NullEdge(Vertex(isec + 1, ipath));
          }
          else
          { // Copy the previous edge
            UEdge(isec + 1, ipath) = UEdge(isec, ipath);
          }
        }
        if (VEdge(isec, ipath + 1).IsNull())
        {
          if (singv)
          {
            // Degenerated Edge
            VEdge(isec, ipath + 1) = NullEdge(Vertex(isec, ipath + 1));
          }
          else
          { // Copy the previous edge
            VEdge(isec, ipath + 1) = VEdge(isec, ipath);
          }
        }
      }
      else
      { // Construction of edges by isos
        if (exuv)
        {
          double UV;
          UV     = UFirst;
          UFirst = VFirst;
          VFirst = UV;
          UV     = ULast;
          ULast  = VLast;
          VLast  = UV;
        }

        // (2.2) Iso-u
        if (isec == 1 && UEdge(1, ipath).IsNull())
        {
          if (!Vertex(1, ipath).IsSame(Vertex(1, ipath + 1)))
          {
            gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(Vertex(1, ipath)));
            gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(Vertex(1, ipath + 1)));
            if (P1.Distance(P2) <= myTol3d)
              Vertex(1, ipath + 1) = Vertex(1, ipath);
          }
          UEdge(1, ipath) =
            BuildEdge(S, !exuv, UFirst, Vertex(1, ipath), Vertex(1, ipath + 1), myTol3d);
        }
        else
        {
          if (UEdge(isec, ipath).IsNull()) // sweep failed
            return false;
          UpdateEdge(TopoDS::Edge(UEdge(isec, ipath)), S, !exuv, UFirst);
        }

        if (uclose && (isec == NbLaw))
        {
          if (UEdge(1, ipath).IsNull()) // degenerated case
          {
            UEdge(isec + 1, ipath) = BuildEdge(S,
                                               !exuv,
                                               ULast,
                                               Vertex(isec + 1, ipath),
                                               Vertex(isec + 1, ipath + 1),
                                               myTol3d);
          }
          else
          {
            UpdateEdge(TopoDS::Edge(UEdge(1, ipath)), S, !exuv, ULast);
            UEdge(isec + 1, ipath) = UEdge(1, ipath);
          }
        }
        else
        {
          if (UEdge(isec + 1, ipath).IsNull())
            UEdge(isec + 1, ipath) = BuildEdge(S,
                                               !exuv,
                                               ULast,
                                               Vertex(isec + 1, ipath),
                                               Vertex(isec + 1, ipath + 1),
                                               myTol3d);
          else
            UpdateEdge(TopoDS::Edge(UEdge(isec + 1, ipath)), S, !exuv, ULast);
        }

        // (2.3) Iso-v
        if (ipath == 1)
        {
          TopoDS_Edge aNewFirstEdge =
            BuildEdge(S, exuv, VFirst, Vertex(isec, 1), Vertex(isec + 1, 1), myTol3d);
          if (VEdge(isec, ipath).IsNull())
            VEdge(isec, ipath) = aNewFirstEdge;
          else // rebuild first edge
          {
            RebuildTopOrBottomEdge(aNewFirstEdge, TopoDS::Edge(VEdge(isec, ipath)), ReversedEdges);
            if (ReversedEdges.Contains(VEdge(isec, ipath)))
            {
              ReverseEdgeInFirstOrLastWire(FirstShape, VEdge(isec, ipath));
              StartEdges(isec).Reverse();
            }
          }
        }

        else
          UpdateEdge(TopoDS::Edge(VEdge(isec, ipath)), S, exuv, VFirst);

        if (vclose && (ipath == NbPath))
        {
          if (VEdge(isec, 1).IsNull()) // degenerated case
          {
            VEdge(isec, ipath + 1) = BuildEdge(S,
                                               exuv,
                                               VLast,
                                               Vertex(isec, ipath + 1),
                                               Vertex(isec + 1, ipath + 1),
                                               myTol3d);
          }
          else
          {
            UpdateEdge(TopoDS::Edge(VEdge(isec, 1)), S, exuv, VLast);
            VEdge(isec, ipath + 1) = VEdge(isec, 1);
          }
        }
        else if (VEdge(isec, ipath + 1).IsNull())
          VEdge(isec, ipath + 1) = BuildEdge(S,
                                             exuv,
                                             VLast,
                                             Vertex(isec, ipath + 1),
                                             Vertex(isec + 1, ipath + 1),
                                             myTol3d);
        else
        {
          if (ipath != NbPath || vclose || (global_vclose && ILast == myLoc->NbLaw() + 1))

            UpdateEdge(TopoDS::Edge(VEdge(isec, ipath + 1)), S, exuv, VLast);
          else // ipath == NbPath && !vclose => rebuild last edge
          {
            TopoDS_Edge aNewLastEdge = BuildEdge(S,
                                                 exuv,
                                                 VLast,
                                                 Vertex(isec, ipath + 1),
                                                 Vertex(isec + 1, ipath + 1),
                                                 myTol3d);
            RebuildTopOrBottomEdge(aNewLastEdge,
                                   TopoDS::Edge(VEdge(isec, ipath + 1)),
                                   ReversedEdges);
            if (ReversedEdges.Contains(VEdge(isec, ipath + 1)))
              ReverseEdgeInFirstOrLastWire(LastShape, VEdge(isec, ipath + 1));
          }
        }
      }
    } // End of construction of edges
  }

  // (3) Construction of Faces
  TopoDS_Face face;

  for (ipath = 1, IPath = IFirst; ipath <= NbPath; ipath++, IPath++)
  {
    for (isec = 1; isec <= NbLaw; isec++)
    {
      if (Degenerated(isec, ipath))
      {
        if (UEdge(isec, ipath).IsSame(UEdge(isec + 1, ipath)))
          myFaces->SetValue(isec, IPath, UEdge(isec, ipath));
        else
          myFaces->SetValue(isec, IPath, VEdge(isec, ipath));
      }
      else if (myFaces->Value(isec, IPath).IsNull())
      {
        BuildFace(TabS(isec, ipath),
                  TopoDS::Edge(UEdge(isec, ipath)),
                  TopoDS::Edge(VEdge(isec, ipath)),
                  TopoDS::Edge(UEdge(isec + 1, ipath)),
                  TopoDS::Edge(VEdge(isec, ipath + 1)),
                  myVEdgesModified,
                  ExchUV(isec, ipath),
                  UReverse(isec, ipath),
                  face);
        myFaces->SetValue(isec, IPath, face);
      }
    }
  }

  // (3.1) Reverse the faces that have been built earlier
  for (ipath = 1; ipath <= NbPath; ipath++)
    for (isec = 1; isec <= NbLaw; isec++)
      if (IsBuilt(isec))
        myFaces->ChangeValue(isec, ipath).Reverse();

  // (4) History and Continuity

  if (hasdegen)
  {
    //(4.1) // Degenerated case => Sledgehammer
    TopoDS_Compound Comp;
    B.MakeCompound(Comp);
    for (isec = 1; isec <= NbLaw + 1; isec++)
      for (ipath = 1, IPath = IFirst; ipath <= NbPath + 1; ipath++, IPath++)
      {
        if (ipath <= NbPath)
          myUEdges->SetValue(isec, IPath, UEdge(isec, ipath));
        if (isec <= NbLaw)
          myVEdges->SetValue(isec, IPath, VEdge(isec, ipath));
        if ((ipath <= NbPath) && (isec <= NbLaw) && !myFaces->Value(isec, IPath).IsNull()
            && myFaces->Value(isec, IPath).ShapeType() == TopAbs_FACE)
          B.Add(Comp, myFaces->Value(isec, IPath));
      }
    BRepLib::EncodeRegularity(Comp, myTolAngular);
  }
  else
  {
    //(4.2) // General case => Tweezers
    bool        isG1;
    TopoDS_Face FF;
    TopoDS_Edge anEdge;

    for (isec = 1; isec <= NbLaw + 1; isec++)
    {
      if (isec > 1)
        isG1 = (mySec->Continuity(isec - 1, myTolAngular) >= GeomAbs_G1);
      else
        isG1 = false;
      for (ipath = 1, IPath = IFirst; ipath <= NbPath; ipath++, IPath++)
      {
        myUEdges->SetValue(isec, IPath, UEdge(isec, ipath));
        if (isG1)
        {
          if (isec == NbLaw + 1)
            FF = TopoDS::Face(myFaces->Value(1, IPath));
          else
            FF = TopoDS::Face(myFaces->Value(isec, IPath));
          B.Continuity(TopoDS::Edge(myUEdges->Value(isec, IPath)),
                       TopoDS::Face(myFaces->Value(isec - 1, IPath)),
                       FF,
                       GeomAbs_G1);
        }
      }
    }

    int nbpath = NbPath;
    if (vclose)
      nbpath++; // Another test G1
    for (ipath = 1, IPath = IFirst; ipath <= NbPath + 1; ipath++, IPath++)
    {
      if ((ipath > 1) && (ipath <= nbpath))
        isG1 = (myLoc->IsG1(IPath - 1, myTol3d, myTolAngular) >= 0);
      else
        isG1 = false;
      for (isec = 1; isec <= NbLaw; isec++)
      {
        myVEdges->SetValue(isec, IPath, VEdge(isec, ipath));
        if (isG1)
        {
          if (ipath == NbPath + 1)
            FF = TopoDS::Face(myFaces->Value(isec, 1));
          else
            FF = TopoDS::Face(myFaces->Value(isec, IPath));
          anEdge = TopoDS::Edge(myVEdges->Value(isec, IPath));
          BRepLib::EncodeRegularity(anEdge,
                                    FF,
                                    TopoDS::Face(myFaces->Value(isec, IPath - 1)),
                                    myTolAngular);
        }
      }
    }
  }

  // (5) Update Tapes and Rails
  int j;
  if (IFirst == 1 && !Tapes.IsEmpty()) // works only in case of single shell
  {
    for (isec = 1; isec <= NbLaw; isec++)
    {
      for (j = 1; j <= NbPath + 1; j++)
        Tapes(StartEdges(isec))->SetValue(1, j, myVEdges->Value(isec, j));
      for (j = 1; j <= NbPath; j++)
        Tapes(StartEdges(isec))->SetValue(2, j, myUEdges->Value(isec, j));
      for (j = 1; j <= NbPath; j++)
        Tapes(StartEdges(isec))->SetValue(3, j, myUEdges->Value(isec + 1, j));
      for (j = 1; j <= NbPath + 1; j++)
        Tapes(StartEdges(isec))->SetValue(4, j, Vertex(isec, j));
      for (j = 1; j <= NbPath + 1; j++)
        Tapes(StartEdges(isec))->SetValue(5, j, Vertex(isec + 1, j));
      for (j = 1; j <= NbPath; j++)
        Tapes(StartEdges(isec))->SetValue(6, j, myFaces->Value(isec, j));
      TopoDS_Vertex Vfirst, Vlast;
      // clang-format off
      TopExp::Vertices(TopoDS::Edge(StartEdges(isec)), Vfirst, Vlast, true); //with orientation
      // clang-format on
      if (!Rails.IsBound(Vfirst))
      {
        occ::handle<NCollection_HArray2<TopoDS_Shape>> anArray =
          new NCollection_HArray2<TopoDS_Shape>(1, 2, 1, NbPath + 1);
        for (j = 1; j <= NbPath; j++)
          anArray->SetValue(1, j, myUEdges->Value(isec, j));
        for (j = 1; j <= NbPath + 1; j++)
          anArray->SetValue(2, j, Vertex(isec, j));
        Rails.Bind(Vfirst, anArray);
      }
      if (!Rails.IsBound(Vlast))
      {
        occ::handle<NCollection_HArray2<TopoDS_Shape>> anArray =
          new NCollection_HArray2<TopoDS_Shape>(1, 2, 1, NbPath + 1);
        for (j = 1; j <= NbPath; j++)
          anArray->SetValue(1, j, myUEdges->Value(isec + 1, j));
        for (j = 1; j <= NbPath + 1; j++)
          anArray->SetValue(2, j, Vertex(isec + 1, j));
        Rails.Bind(Vlast, anArray);
      }
    }
  }

  return true;
}

//=======================================================================
// function : Build
// purpose  : Construct the result of sweeping
//======================================================================
void BRepFill_Sweep::Build(NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& ReversedEdges,
                           NCollection_DataMap<TopoDS_Shape,
                                               occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                                               TopTools_ShapeMapHasher>&           Tapes,
                           NCollection_DataMap<TopoDS_Shape,
                                               occ::handle<NCollection_HArray2<TopoDS_Shape>>,
                                               TopTools_ShapeMapHasher>&           Rails,
                           const BRepFill_TransitionStyle                          Transition,
                           const GeomAbs_Shape                                     Continuity,
                           const GeomFill_ApproxStyle                              Approx,
                           const int                                               Degmax,
                           const int                                               Segmax)
{
  myContinuity  = Continuity;
  myApproxStyle = Approx;
  myDegmax      = Degmax;
  mySegmax      = Segmax;

  CorrectApproxParameters();

  // Wire
  if (mySec->IsVertex())
    isDone = BuildWire(Transition);

  else
  { // Shell
    int NbTrous = myLoc->NbHoles(myTol3d), NbPath = myLoc->NbLaw(), NbLaw = mySec->NbLaw(), ii, jj,
        NbPart = 1;
    int          ipath, isec;
    BRep_Builder B;
    myUEdges = new (NCollection_HArray2<TopoDS_Shape>)(1, NbLaw + 1, 1, NbPath);
    myVEdges = new (NCollection_HArray2<TopoDS_Shape>)(1, NbLaw, 1, NbPath + 1);
    myFaces  = new (NCollection_HArray2<TopoDS_Shape>)(1, NbLaw, 1, NbPath);
    myTapes  = new (NCollection_HArray1<TopoDS_Shape>)(1, NbLaw);
    BRep_Builder BB;
    for (int i = 1; i <= NbLaw; i++)
    {
      TopoDS_Shell aShell;
      BB.MakeShell(aShell);
      myTapes->ChangeValue(i) = aShell;
    }
    occ::handle<NCollection_HArray2<TopoDS_Shape>> Bounds =
      new (NCollection_HArray2<TopoDS_Shape>)(1, NbLaw, 1, 2);

    occ::handle<NCollection_HArray1<int>> Trous;

    if (NbTrous > 0)
    { // How many sub-parts ?
      Trous = new (NCollection_HArray1<int>)(1, NbTrous);
      myLoc->Holes(Trous->ChangeArray1());
      NbPart += NbTrous;
      if (Trous->Value(NbTrous) == NbPath + 1)
        NbPart--;
    }
    if (NbPart == 1)
    { // This is done at once
      double Extend = 0.0;
      if (NbTrous == 1)
        Extend = EvalExtrapol(1, Transition);
      isDone = BuildShell(Transition, 1, NbPath + 1, ReversedEdges, Tapes, Rails, Extend, Extend);
    }
    else
    { //  This is done piece by piece
      int IFirst = 1, ILast;
      for (ii = 1, isDone = true; ii <= NbPart && isDone; ii++)
      {
        if (ii > NbTrous)
          ILast = NbPath + 1;
        else
          ILast = Trous->Value(ii);
        isDone = BuildShell(Transition,
                            IFirst,
                            ILast,
                            ReversedEdges,
                            Tapes,
                            Rails,
                            EvalExtrapol(IFirst, Transition),
                            EvalExtrapol(ILast, Transition));
        if (IFirst > 1)
        {
          Translate(myVEdges, IFirst, Bounds, 2);
          if (!PerformCorner(IFirst, Transition, Bounds))
          {
            isDone = false;
            return;
          }
        }
        IFirst = ILast;
        Translate(myVEdges, IFirst, Bounds, 1);
      }
    }
    // Management of looping ends
    if ((NbTrous > 0) && (myLoc->IsClosed()) && (Trous->Value(NbTrous) == NbPath + 1))
    {
      Translate(myVEdges, NbPath + 1, Bounds, 1);
      Translate(myVEdges, 1, Bounds, 2);
      if (!PerformCorner(1, Transition, Bounds))
      {
        isDone = false;
        return;
      }
      Translate(myVEdges, 1, myVEdges, NbPath + 1);
    }

    // Construction of the shell
    TopoDS_Shell shell;
    B.MakeShell(shell);
    int aNbFaces = 0;
    for (ipath = 1; ipath <= NbPath; ipath++)
      for (isec = 1; isec <= NbLaw; isec++)
      {
        const TopoDS_Shape& face = myFaces->Value(isec, ipath);
        if (!face.IsNull() && (face.ShapeType() == TopAbs_FACE))
        {
          B.Add(shell, face);
          aNbFaces++;
        }
      }

    if ((NbTrous > 0) ? (aNbFaces < NbLaw) : (aNbFaces == 0))
    {
      isDone = false;
      return;
    }

    NCollection_List<TopoDS_Shape>::Iterator It(myAuxShape);
    for (; It.More(); It.Next())
    {
      const TopoDS_Shape& face = It.Value();
      if (!face.IsNull() && (face.ShapeType() == TopAbs_FACE))
        B.Add(shell, face);
    }
    // Set common Uedges to faces
    BRepTools_Substitution aSubstitute;
    /*
    for (ii = 1; ii <= NbLaw; ii++)
      for (jj = 1; jj <= NbPath; jj++)
    {
      SetCommonEdgeInFace(aSubstitute,
                  myFaces->Value(ii, jj),
                  myUEdges->Value(ii, jj));
      SetCommonEdgeInFace(aSubstitute,
                  myFaces->Value(ii, jj),
                  myUEdges->Value(ii+1, jj));
    }
    if (mySec->IsUClosed())
      for (jj = 1; jj <= NbPath; jj++)
    SetCommonEdgeInFace(aSubstitute,
                myFaces->Value( 1, jj ),
                myUEdges->Value( NbLaw+1, jj));
    */
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator mapit(
      myVEdgesModified);
    for (; mapit.More(); mapit.Next())
    {
      const TopoDS_Edge& OldEdge = TopoDS::Edge(mapit.Key());
      const TopoDS_Edge& NewEdge = TopoDS::Edge(mapit.Value());
      Substitute(aSubstitute, OldEdge, NewEdge);
    }
    aSubstitute.Build(shell);
    if (aSubstitute.IsCopied(shell))
    {
      const NCollection_List<TopoDS_Shape>& listSh = aSubstitute.Copy(shell);
      shell                                        = TopoDS::Shell(listSh.First());
    }

    for (ii = myFaces->LowerRow(); ii <= myFaces->UpperRow(); ii++)
    {
      for (jj = myFaces->LowerCol(); jj <= myFaces->UpperCol(); jj++)
      {
        const TopoDS_Shape& aLocalShape = myFaces->Value(ii, jj);

        if (!aLocalShape.IsNull() && aSubstitute.IsCopied(aLocalShape))
        {
          const NCollection_List<TopoDS_Shape>& aList = aSubstitute.Copy(aLocalShape);

          if (!aList.IsEmpty())
            myFaces->ChangeValue(ii, jj) = aList.First();
        }
      }
    }

    for (ii = myVEdges->LowerRow(); ii <= myVEdges->UpperRow(); ii++)
    {
      for (jj = myVEdges->LowerCol(); jj <= myVEdges->UpperCol(); jj++)
      {
        const TopoDS_Shape& aLocalShape = myVEdges->Value(ii, jj);

        if (!aLocalShape.IsNull() && aSubstitute.IsCopied(aLocalShape))
        {
          const NCollection_List<TopoDS_Shape>& aList = aSubstitute.Copy(aLocalShape);

          if (!aList.IsEmpty())
            myVEdges->ChangeValue(ii, jj) = aList.First();
        }
      }
    }

    for (ii = myUEdges->LowerRow(); ii <= myUEdges->UpperRow(); ii++)
    {
      for (jj = myUEdges->LowerCol(); jj <= myUEdges->UpperCol(); jj++)
      {
        const TopoDS_Shape& aLocalShape = myUEdges->Value(ii, jj);

        if (!aLocalShape.IsNull() && aSubstitute.IsCopied(aLocalShape))
        {
          const NCollection_List<TopoDS_Shape>& aList = aSubstitute.Copy(aLocalShape);

          if (!aList.IsEmpty())
            myUEdges->ChangeValue(ii, jj) = aList.First();
        }
      }
    }

    // Ensure Same Parameter on U-edges
    for (ii = myUEdges->LowerRow(); ii <= myUEdges->UpperRow(); ii++)
    {
      if (mySec->IsUClosed() && ii == myUEdges->UpperRow())
        continue;
      for (jj = myUEdges->LowerCol(); jj <= myUEdges->UpperCol(); jj++)
      {
        TopoDS_Edge anEdge = TopoDS::Edge(myUEdges->Value(ii, jj));
        if (anEdge.IsNull() || BRep_Tool::Degenerated(anEdge))
          continue;
        TopoDS_Face Face1, Face2;
        int         i1 = ii - 1, i2 = ii;
        if (i1 == 0 && mySec->IsUClosed())
          i1 = myFaces->UpperRow();
        if (i2 > myFaces->UpperRow())
          i2 = 0;
        if (i1 != 0)
        {
          const TopoDS_Shape& aShape1 = myFaces->Value(i1, jj);
          if (aShape1.ShapeType() == TopAbs_FACE)
            Face1 = TopoDS::Face(aShape1);
        }
        if (i2 != 0)
        {
          const TopoDS_Shape& aShape2 = myFaces->Value(i2, jj);
          if (aShape2.ShapeType() == TopAbs_FACE)
            Face2 = TopoDS::Face(aShape2);
        }
        if (!Face1.IsNull() && !Face2.IsNull())
          CorrectSameParameter(anEdge, Face1, Face2);
      }
    }

    for (ii = 1; ii <= NbLaw; ii++)
      for (jj = 1; jj <= NbPath; jj++)
      {
        const TopoDS_Shape& aFace = myFaces->Value(ii, jj);
        if (!aFace.IsNull() && aFace.ShapeType() == TopAbs_FACE)
          BB.Add(myTapes->ChangeValue(ii), aFace);
      }

    // Is it Closed ?
    if (myLoc->IsClosed() && mySec->IsUClosed())
    {
      // Check
      bool closed = true;
      int  iedge;
      NCollection_IndexedDataMap<TopoDS_Shape,
                                 NCollection_List<TopoDS_Shape>,
                                 TopTools_ShapeMapHasher>
        EFmap;
      TopExp::MapShapesAndAncestors(shell, TopAbs_EDGE, TopAbs_FACE, EFmap);

      for (iedge = 1; iedge <= EFmap.Extent() && closed; iedge++)
      {
        const TopoDS_Edge& theEdge = TopoDS::Edge(EFmap.FindKey(iedge));
        if (BRep_Tool::Degenerated(theEdge))
          continue;
        closed = (EFmap(iedge).Extent() > 1);
      }
      shell.Closed(closed);
    }
    myShape = shell;
  }
}

//=================================================================================================

bool BRepFill_Sweep::IsDone() const
{
  return isDone;
}

//=================================================================================================

TopoDS_Shape BRepFill_Sweep::Shape() const
{
  return myShape;
}

//=================================================================================================

double BRepFill_Sweep::ErrorOnSurface() const
{
  return Error;
}

//=======================================================================
// function : SubShape
// purpose  : Faces obtained by sweeping
//=======================================================================
occ::handle<NCollection_HArray2<TopoDS_Shape>> BRepFill_Sweep::SubShape() const
{
  return myFaces;
}

//=======================================================================
// function : InterFaces
// purpose  : Edges obtained by sweeping
//=======================================================================
occ::handle<NCollection_HArray2<TopoDS_Shape>> BRepFill_Sweep::InterFaces() const
{
  return myUEdges;
}

//=======================================================================
// function : Sections
// purpose  : Edges or Face (or compound of 2) Transition between 2 sweepings
//=======================================================================
occ::handle<NCollection_HArray2<TopoDS_Shape>> BRepFill_Sweep::Sections() const
{
  return myVEdges;
}

//=======================================================================
// function : Tape
// purpose  : returns the Tape corresponding to Index-th edge of section
//=======================================================================
TopoDS_Shape BRepFill_Sweep::Tape(const int Index) const
{
  return myTapes->Value(Index);
}

//=======================================================================
// function : PerformCorner
// purpose  : Trim and/or loop a corner
//======================================================================
bool BRepFill_Sweep::PerformCorner(const int                                             Index,
                                   const BRepFill_TransitionStyle                        Transition,
                                   const occ::handle<NCollection_HArray2<TopoDS_Shape>>& Bounds)
{

  if (Transition == BRepFill_Modified)
    return true; // Do nothing.

  const double anAngularTol = 0.025;

  BRepFill_TransitionStyle TheTransition = Transition;
  bool                     isTangent     = false;
  double                   F, L;
  int                      I1, I2, ii; //, jj;
  gp_Pnt                   P1, P2;
  gp_Vec                   T1, T2, Tang, Sortant;
  //  gp_Mat M;
  // occ::handle<NCollection_HArray1<TopoDS_Shape>> TheShape =
  // new NCollection_HArray1<TopoDS_Shape>( 1, mySec->NbLaw() );
  //  NCollection_List<TopoDS_Shape>::Iterator Iterator;

  if (Index > 1)
  {
    I1 = Index - 1;
    I2 = Index;
  }
  else
  {
    I1 = myLoc->NbLaw();
    I2 = 1;
  }

  // Construct an axis supported by the bissectrice
  myLoc->Law(I1)->GetDomain(F, L);
  myLoc->Law(I1)->GetCurve()->D1(L, P1, T1);
  T1.Normalize();

  myLoc->Law(I2)->GetDomain(F, L);
  myLoc->Law(I2)->GetCurve()->D1(F, P2, T2);
  T2.Normalize();

  if (T1.Angle(T2) < myAngMin)
  {
    isTangent = true;
    gp_Vec t1, t2, V;
    gp_Mat M;
    myLoc->Law(I1)->GetDomain(F, L);
    myLoc->Law(I1)->D0(L, M, V);
    t1 = M.Column(3);
    myLoc->Law(I2)->GetDomain(F, L);
    myLoc->Law(I2)->D0(L, M, V);
    t2 = M.Column(3);

    if (t1.Angle(t2) < myAngMin)
    {
#ifdef OCCT_DEBUG
      std::cout << "BRepFill_Sweep::PerformCorner : This is not a corner !" << std::endl;
#endif
      return true;
    }
    Sortant = t2 - t1;
  }

  if (T1.Angle(T2) >= M_PI - anAngularTol)
  {
    return false;
  }
  if ((TheTransition == BRepFill_Right) && (T1.Angle(T2) > myAngMax))
  {
    TheTransition = BRepFill_Round;
  }

  Tang                                  = T1 + T2; // Average direction
  gp_Dir NormalOfBisPlane               = Tang;
  gp_Vec anIntersectPointCrossDirection = T1.Crossed(T2);
  if (isTangent)
  {
    Sortant -= Tang.Dot(Tang) * Tang;
  }
  else
  {
    Sortant = T2 - T1; // Direction input
    Sortant *= -1;     //   "   "   output
    Tang -= (Tang.Dot(T2)) * T2;
  }

  P1.BaryCenter(0.5, P2, 0.5);
  gp_Dir N(Sortant);
  gp_Dir Dx(Tang);

  gp_Ax2 Axe(P1, N, Dx);
  gp_Ax2 AxeOfBisPlane(P1, NormalOfBisPlane);

  // Construct 2 intersecting Shells
  occ::handle<NCollection_HArray2<TopoDS_Shape>> UEdges =
    new NCollection_HArray2<TopoDS_Shape>(1, mySec->NbLaw() + 1, 1, myLoc->NbLaw());
  UEdges->ChangeArray2() = myUEdges->Array2();

  // modified by NIZHNY-MKK  Wed Oct 29 18:31:47 2003.BEGIN
  occ::handle<NCollection_HArray2<TopoDS_Shape>> aFaces =
    new NCollection_HArray2<TopoDS_Shape>(myFaces->LowerRow(), myFaces->UpperRow(), 1, 2);
  Translate(myFaces, I1, aFaces, 1);
  Translate(myFaces, I2, aFaces, 2);

  occ::handle<NCollection_HArray2<TopoDS_Shape>> aUEdges =
    new NCollection_HArray2<TopoDS_Shape>(myUEdges->LowerRow(), myUEdges->UpperRow(), 1, 2);
  Translate(myUEdges, I1, aUEdges, 1);
  Translate(myUEdges, I2, aUEdges, 2);

  gp_Vec      aNormal = T2 + T1;
  TopoDS_Face aPlaneF;

  if (aNormal.Magnitude() > gp::Resolution())
  {
    gp_Pln           pl(P1, gp_Dir(aNormal));
    BRepLib_MakeFace aFMaker(pl);

    if (aFMaker.Error() == BRepLib_FaceDone)
    {
      aPlaneF = aFMaker.Face();
      BRep_Builder aBB;
      aBB.UpdateFace(aPlaneF, Precision::Confusion() * 10.);
    }
  }

  BRepFill_TrimShellCorner aTrim(aFaces, Transition, AxeOfBisPlane, anIntersectPointCrossDirection);
  aTrim.AddBounds(Bounds);
  aTrim.AddUEdges(aUEdges);
  aTrim.AddVEdges(myVEdges, Index);
  aTrim.Perform();

  if (aTrim.IsDone())
  {

    NCollection_List<TopoDS_Shape> listmodif;
    for (ii = 1; ii <= mySec->NbLaw(); ii++)
    {
      listmodif.Clear();
      aTrim.Modified(myVEdges->Value(ii, Index), listmodif);

      if (listmodif.IsEmpty())
      {
        TopoDS_Edge NullEdge;
        myVEdges->SetValue(ii, Index, NullEdge);
      }
      else
        myVEdges->SetValue(ii, Index, listmodif.First());
    }

    listmodif.Clear();
    int iit = 0;

    for (iit = 0; iit < 2; iit++)
    {
      int II = (iit == 0) ? I1 : I2;

      for (ii = 1; ii <= mySec->NbLaw(); ii++)
      {
        aTrim.Modified(myFaces->Value(ii, II), listmodif);

        if (!listmodif.IsEmpty())
        {
          myFaces->SetValue(ii, II, listmodif.First());
        }
      }

      for (ii = myUEdges->LowerRow(); ii <= myUEdges->UpperRow(); ii++)
      {
        aTrim.Modified(myUEdges->Value(ii, II), listmodif);

        if (!listmodif.IsEmpty())
        {
          myUEdges->SetValue(ii, II, listmodif.First());
        }
      }
    }
  }
  else if ((TheTransition == BRepFill_Right) || aTrim.HasSection())
  {
#ifdef OCCT_DEBUG
    std::cout << "Fail of TrimCorner" << std::endl;
#endif
    return true; // Nothing is touched
  }

  if (mySec->IsUClosed())
  {
    myUEdges->SetValue(1, I1, myUEdges->Value(mySec->NbLaw() + 1, I1));
    myUEdges->SetValue(1, I2, myUEdges->Value(mySec->NbLaw() + 1, I2));
  }

  if (TheTransition == BRepFill_Round)
  {
    // Filling
    NCollection_List<TopoDS_Shape> list1, list2;
    TopoDS_Edge                    Bord1, Bord2, BordFirst;
    BordFirst.Nullify();
    Bord1.Nullify();
    Bord2.Nullify();
    bool        HasFilling = false;
    TopoDS_Face FF;
    for (ii = 1; ii <= mySec->NbLaw(); ii++)
    {
      KeepEdge(myFaces->Value(ii, I1), Bounds->Value(ii, 1), list1);
      KeepEdge(myFaces->Value(ii, I2), Bounds->Value(ii, 2), list2);
      if (list1.Extent() == list2.Extent())
      {
        NCollection_List<TopoDS_Shape>::Iterator It1(list1);
        NCollection_List<TopoDS_Shape>::Iterator It2(list2);
        bool                                     B;
        for (; It1.More(); It1.Next(), It2.Next())
        {
          if (HasFilling)
          { // Transversal choice of constraints
            TopoDS_Vertex VF, VL, VC;
            TopoDS_Edge   E = TopoDS::Edge(It1.Value());
            TopoDS_Edge   E1, E2;
            E1.Nullify();
            E2.Nullify();
            TopExp::Vertices(E, VF, VL);
            if (!Bord1.IsNull() && TopExp::CommonVertex(E, Bord1, VC))
            {
              if (VC.IsSame(VF))
                E1 = Bord1;
              else
                E2 = Bord1;
            }
            if (!Bord2.IsNull() && TopExp::CommonVertex(E, Bord2, VC))
            {
              if (VC.IsSame(VF))
                E1 = Bord2;
              else
                E2 = Bord2;
            }
            if (!BordFirst.IsNull() && TopExp::CommonVertex(E, BordFirst, VC))
            {
              if (VC.IsSame(VF))
                E1 = BordFirst;
              else
                E2 = BordFirst;
            }
            Bord1 = E1;
            Bord2 = E2;
          }

          // Filling
          B = Filling(It1.Value(),
                      myFaces->Value(ii, I1),
                      It2.Value(),
                      myFaces->Value(ii, I2),
                      myVEdgesModified,
                      myTol3d,
                      Axe,
                      T1,
                      Bord1,
                      Bord2,
                      FF);

          if (B)
          {
            myAuxShape.Append(FF);
            BRep_Builder    BB;
            TopoDS_Shape    aVshape = myVEdges->Value(ii, I2);
            TopoDS_Compound aCompound;
            BB.MakeCompound(aCompound);
            if (!aVshape.IsNull())
              BB.Add(aCompound, aVshape);
            BB.Add(aCompound, FF);
            myVEdges->ChangeValue(ii, I2) = aCompound;

            BB.Add(myTapes->ChangeValue(ii), FF);
            HasFilling = true;
          }
          if (ii == 1)
            BordFirst = Bord1;
        }
      }
#ifdef OCCT_DEBUG
      else
        std::cout << "PerformCorner : Unsymmetry of free border" << std::endl;
#endif
    }
  }
  return true;
  /*
  #if DRAW
    if (Affich) {
      int jj;
      char name[100];
      DBRep::Set("TrimmedShell", TheShape);
      for (jj=1; jj <=myFaces->ColLength(); jj++){
        Sprintf(name,"Tfaces_%d_%d", jj, I1);
        DBRep::Set(name, myFaces->Value(jj, I1));
        Sprintf(name,"Tfaces_%d_%d", jj, I2);
        DBRep::Set(name, myFaces->Value(jj, I2));
      }
    }
  #endif
  */
}

//=================================================================================================

double BRepFill_Sweep::EvalExtrapol(const int                      Index,
                                    const BRepFill_TransitionStyle Transition) const
{
  double Extrap = 0.0;
  if (Transition == BRepFill_Right)
  {
    int I1, I2;
    if ((Index == 1) || (Index == myLoc->NbLaw() + 1))
    {
      if (!myLoc->IsClosed() || !mySec->IsVClosed())
        return Extrap;
      I1 = myLoc->NbLaw();
      I2 = 1;
    }
    else
    {
      I1 = Index - 1;
      I2 = Index;
    }

    gp_Vec V1, V2, T1, T2;
    gp_Mat M1, M2;
    double Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, R, f, l;

    myLoc->Law(I1)->GetDomain(f, l);
    myLoc->Law(I1)->D0(l, M1, V1);
    T1.SetXYZ(M1.Column(3));
    myLoc->Law(I2)->GetDomain(f, l);
    myLoc->Law(I2)->D0(f, M2, V2);
    T2.SetXYZ(M2.Column(3));

    double alpha = T1.Angle(T2);
    if ((alpha > myAngMax) || (alpha < myAngMin))
    {
      // Angle too great => No "straight" connection
      // Angle too small => No connection
      return Extrap; // = 0.0
    }

    occ::handle<GeomFill_SectionLaw> Sec;
    Sec = mySec->ConcatenedLaw();

    // Calculating parameter U
    double U, Length, SecFirst, SecLen, Lf, Ll;
    myLoc->CurvilinearBounds(myLoc->NbLaw(), Lf, Length);
    mySec->Law(1)->GetDomain(SecFirst, SecLen);
    SecLen -= SecFirst;
    myLoc->CurvilinearBounds(I1, Lf, Ll);
    U = SecFirst + (Ll / Length) * SecLen;

    Bnd_Box box;
    // Box(Sec, 0., box);
    Box(Sec, U, box);
    box.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

    R =
      std::max(std::max(std::abs(Xmin), std::abs(Xmax)), std::max(std::abs(Ymin), std::abs(Ymax)));
    // R *= 1.1;
    //  modified by NIZHNY-MKK  Fri Oct 31 18:57:51 2003
    //      double coef = 1.2;
    double coef = 2.;
    R *= coef;
    Extrap = std::max(std::abs(Zmin), std::abs(Zmax)) + 100 * myTol3d;
    Extrap += R * std::tan(alpha / 2);
  }
  return Extrap;
}

//=======================================================================
// function : MergeVertex
// purpose  : Make V2 = V1 if V2 is too close to V1
//======================================================================
bool BRepFill_Sweep::MergeVertex(const TopoDS_Shape& V1, TopoDS_Shape& V2) const
{
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  const TopoDS_Vertex& v1 = TopoDS::Vertex(V1);
  const TopoDS_Vertex& v2 = TopoDS::Vertex(V2);
  double               tol;
  //  tol = std::max(BT.Tolerance(v1), BT.Tolerance(v2));
  tol = std::max(BRep_Tool::Tolerance(v1), BRep_Tool::Tolerance(v2));
  if (tol < myTol3d)
    tol = myTol3d;
  //  if (BT.Pnt(v1).Distance(BT.Pnt(v2)) <= tol ){
  if (BRep_Tool::Pnt(v1).Distance(BRep_Tool::Pnt(v2)) <= tol)
  {
    V2 = V1;
    return true;
  }
  return false;
}

//=======================================================================
// function : UpdateVertex
// purpose  : Update the Tolerance of Vertices depending on Laws.
//======================================================================
void BRepFill_Sweep::UpdateVertex(const int     ipath,
                                  const int     isec,
                                  const double  ErrApp,
                                  const double  Param,
                                  TopoDS_Shape& V) const
{
  TopoDS_Vertex vv, TheV;
  TheV = TopoDS::Vertex(V);
  myLoc->PerformVertex(ipath,
                       mySec->Vertex(isec, Param),
                       ErrApp + mySec->VertexTol(isec - 1, Param),
                       vv);
  // Class BRep_Tool without fields and without Constructor :
  //  BRep_Tool BT;
  gp_Pnt P1, P2;
  //  P1 = BT.Pnt(vv);
  P1 = BRep_Tool::Pnt(vv);
  //  P2 = BT.Pnt(TheV);
  P2 = BRep_Tool::Pnt(TheV);

  //  double Tol = BT.Tolerance(vv);
  double Tol = BRep_Tool::Tolerance(vv);
  Tol += P1.Distance(P2);

  //  if (Tol >  BT.Tolerance(TheV)) {
  if (Tol > BRep_Tool::Tolerance(TheV))
  {
    BRep_Builder B;
    B.UpdateVertex(TheV, Tol);
  }
}

//=======================================================================
// function : RebuildTopOrBottomEdge
// purpose  : Rebuild v-iso edge of top or bottom section
//           inserting new 3d and 2d curves taken from swept surfaces
//======================================================================
void BRepFill_Sweep::RebuildTopOrBottomEdge(
  const TopoDS_Edge&                                      aNewEdge,
  TopoDS_Edge&                                            anEdge,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& ReversedEdges) const
{
  double                  fpar, lpar;
  occ::handle<Geom_Curve> aNewCurve = BRep_Tool::Curve(aNewEdge, fpar, lpar);
  TopLoc_Location         Identity;

  bool ToReverse = false;
  bool IsDegen   = BRep_Tool::Degenerated(aNewEdge);
  if (IsDegen)
    BRep_Tool::Range(aNewEdge, fpar, lpar);
  else
  {
    TopoDS_Vertex V1, V2, NewV1, NewV2;
    TopExp::Vertices(anEdge, V1, V2);
    if (!V1.IsSame(V2))
    {
      TopExp::Vertices(aNewEdge, NewV1, NewV2);
      V1.Location(Identity);
      if (!V1.IsSame(NewV1))
      {
        if (V1.IsSame(NewV2))
          ToReverse = true;
        else
        {
          gp_Pnt Pnt1    = BRep_Tool::Pnt(V1);
          gp_Pnt NewPnt1 = BRep_Tool::Pnt(NewV1);
          double TolSum  = BRep_Tool::Tolerance(V1) + BRep_Tool::Tolerance(NewV1);
          if (!Pnt1.IsEqual(NewPnt1, TolSum))
            ToReverse = true;
        }
      }
    }
    else
    {
      double                  OldFirst, OldLast;
      occ::handle<Geom_Curve> OldCurve = BRep_Tool::Curve(anEdge, OldFirst, OldLast);
      gp_Vec                  OldD1, NewD1;
      gp_Pnt                  MidPnt;
      OldCurve->D1(0.5 * (OldFirst + OldLast), MidPnt, OldD1);
      aNewCurve->D1(0.5 * (fpar + lpar), MidPnt, NewD1);
      if (OldD1 * NewD1 < 0.)
        ToReverse = true;
    }
  }

  anEdge.Location(Identity);
  const occ::handle<BRep_TEdge>& TEdge = *((occ::handle<BRep_TEdge>*)&anEdge.TShape());
  TEdge->Tolerance(BRep_Tool::Tolerance(aNewEdge));
  BRep_Builder BB;
  BB.Range(anEdge, fpar, lpar);
  BB.UpdateEdge(anEdge, aNewCurve, Precision::Confusion());
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&aNewEdge.TShape());
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>&    lcr = TE->Curves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itrep(lcr);
  for (; itrep.More(); itrep.Next())
  {
    const occ::handle<BRep_CurveRepresentation>& CurveRep = itrep.Value();
    if (CurveRep->IsCurveOnSurface())
    {
      occ::handle<BRep_GCurve>  GC(occ::down_cast<BRep_GCurve>(CurveRep));
      occ::handle<Geom2d_Curve> aPCurve = GC->PCurve();
      occ::handle<Geom_Surface> aSurf   = GC->Surface();
      TopLoc_Location           aLoc    = aNewEdge.Location() * GC->Location();
      BB.UpdateEdge(anEdge, aPCurve, aSurf, aLoc, Precision::Confusion());
    }
  }

  anEdge.Free(true);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2);

  TopoDS_Shape anEdgeFORWARD = anEdge.Oriented(TopAbs_FORWARD);

  BB.Remove(anEdgeFORWARD, V1);
  BB.Remove(anEdgeFORWARD, V2);

  V1.Location(Identity);
  V2.Location(Identity);
  if (ToReverse)
  {
    V2.Orientation(TopAbs_FORWARD);
    V1.Orientation(TopAbs_REVERSED);
  }
  BB.Add(anEdgeFORWARD, V1);
  BB.Add(anEdgeFORWARD, V2);

  if (ToReverse)
  {
    anEdge.Reverse();
    ReversedEdges.Add(anEdge);
  }

  BB.Degenerated(anEdge, IsDegen);
}
