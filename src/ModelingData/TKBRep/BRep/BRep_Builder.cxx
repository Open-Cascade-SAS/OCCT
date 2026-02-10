// Created on: 1991-07-02
// Created by: Remi LEQUETTE
// Copyright (c) 1991-1999 Matra Datavision
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

#include <BRep_Builder.hxx>
#include <BRep_Curve3D.hxx>
#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_LockedShape.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

//=======================================================================
// function : UpdateCurves
// purpose  : Insert a 3d curve <C> with location <L>
//           in a list of curve representations <lcr>
//=======================================================================
static void UpdateCurves(NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr,
                         const occ::handle<Geom_Curve>&                           C,
                         const TopLoc_Location&                                   L,
                         occ::handle<BRep_Curve3D>& theCached3D)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  bool                                                              aHasRange = false;
  double                                                            f = 0., l = 0.;

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D || aKind == BRep_CurveRepKind::CurveOnSurface
        || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
    {
      static_cast<const BRep_GCurve*>(itcr.Value().get())->Range(f, l);
      aHasRange = true;
      if (aKind == BRep_CurveRepKind::Curve3D)
        break;
    }
    itcr.Next();
  }

  if (itcr.More())
  {
    itcr.Value()->Curve3D(C);
    itcr.Value()->Location(L);
    theCached3D = occ::down_cast<BRep_Curve3D>(itcr.Value());
  }
  else
  {
    occ::handle<BRep_Curve3D> C3d = new BRep_Curve3D(C, L);
    // test if there is already a range
    if (aHasRange)
    {
      C3d->SetRange(f, l);
    }
    lcr.Append(C3d);
    theCached3D = C3d;
  }
}

//=======================================================================
// function : UpdateCurves
// purpose  : Insert a pcurve <C> on surface <S> with location <L>
//           in a list of curve representations <lcr>
//           Remove the pcurve on <S> from <lcr> if <C> is null
//=======================================================================

static void UpdateCurves(NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr,
                         const occ::handle<Geom2d_Curve>&                         C,
                         const occ::handle<Geom_Surface>&                         S,
                         const TopLoc_Location&                                   L)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  occ::handle<BRep_CurveRepresentation>                             cr;
  double f = -Precision::Infinite(), l = Precision::Infinite();
  // search the range of the 3d curve
  // and remove any existing representation

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D)
    {
      static_cast<const BRep_GCurve*>(itcr.Value().get())->Range(f, l);
      itcr.Next();
    }
    else if (aKind == BRep_CurveRepKind::CurveOnSurface
             || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
    {
      if (itcr.Value()->IsCurveOnSurface(S, L))
      {
        // remove existing curve on surface
        // cr is used to keep a reference on the curve representation
        // this avoid deleting it as its content may be referenced by C or S
        cr = itcr.Value();
        lcr.Remove(itcr);
      }
      else
      {
        itcr.Next();
      }
    }
    else
    {
      itcr.Next();
    }
  }

  if (!C.IsNull())
  {
    occ::handle<BRep_CurveOnSurface> COS   = new BRep_CurveOnSurface(C, S, L);
    double                           aFCur = 0.0, aLCur = 0.0;
    COS->Range(aFCur, aLCur);
    if (!Precision::IsInfinite(f))
    {
      aFCur = f;
    }

    if (!Precision::IsInfinite(l))
    {
      aLCur = l;
    }

    COS->SetRange(aFCur, aLCur);
    lcr.Append(COS);
  }
}

//=======================================================================
// function : UpdateCurves
// purpose  : Insert a pcurve <C> on surface <S> with location <L>
//           in a list of curve representations <lcr>
//           Remove the pcurve on <S> from <lcr> if <C> is null
//=======================================================================
static void UpdateCurves(NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr,
                         const occ::handle<Geom2d_Curve>&                         C,
                         const occ::handle<Geom_Surface>&                         S,
                         const TopLoc_Location&                                   L,
                         const gp_Pnt2d&                                          Pf,
                         const gp_Pnt2d&                                          Pl)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  occ::handle<BRep_CurveRepresentation>                             cr;
  double f = -Precision::Infinite(), l = Precision::Infinite();

  // search the range of the 3d curve
  // and remove any existing representation

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D)
    {
      static_cast<const BRep_GCurve*>(itcr.Value().get())->Range(f, l);
      itcr.Next();
    }
    else if (aKind == BRep_CurveRepKind::CurveOnSurface
             || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
    {
      if (itcr.Value()->IsCurveOnSurface(S, L))
      {
        // remove existing curve on surface
        // cr is used to keep a reference on the curve representation
        // this avoid deleting it as its content may be referenced by C or S
        cr = itcr.Value();
        lcr.Remove(itcr);
      }
      else
      {
        itcr.Next();
      }
    }
    else
    {
      itcr.Next();
    }
  }

  if (!C.IsNull())
  {
    occ::handle<BRep_CurveOnSurface> COS   = new BRep_CurveOnSurface(C, S, L);
    double                           aFCur = 0.0, aLCur = 0.0;
    COS->Range(aFCur, aLCur);
    if (!Precision::IsInfinite(f))
    {
      aFCur = f;
    }

    if (!Precision::IsInfinite(l))
    {
      aLCur = l;
    }

    COS->SetRange(aFCur, aLCur);
    COS->SetUVPoints(Pf, Pl);
    lcr.Append(COS);
  }
}

//=======================================================================
// function : UpdateCurves
// purpose  : Insert two pcurves <C1,C2> on surface <S> with location <L>
//           in a list of curve representations <lcr>
//           Remove the pcurves on <S> from <lcr> if <C1> or <C2> is null
//=======================================================================

static void UpdateCurves(NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr,
                         const occ::handle<Geom2d_Curve>&                         C1,
                         const occ::handle<Geom2d_Curve>&                         C2,
                         const occ::handle<Geom_Surface>&                         S,
                         const TopLoc_Location&                                   L)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  occ::handle<BRep_CurveRepresentation>                             cr;
  double f = -Precision::Infinite(), l = Precision::Infinite();

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D)
    {
      static_cast<const BRep_GCurve*>(itcr.Value().get())->Range(f, l);
    }
    else if ((aKind == BRep_CurveRepKind::CurveOnSurface
              || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
             && itcr.Value()->IsCurveOnSurface(S, L))
    {
      break;
    }
    itcr.Next();
  }

  if (itcr.More())
  {
    // cr is used to keep a reference on the curve representation
    // this avoid deleting it as its content may be referenced by C or S
    cr = itcr.Value();
    lcr.Remove(itcr);
  }

  if (!C1.IsNull() && !C2.IsNull())
  {
    occ::handle<BRep_CurveOnClosedSurface> COS =
      new BRep_CurveOnClosedSurface(C1, C2, S, L, GeomAbs_C0);
    double aFCur = 0.0, aLCur = 0.0;
    COS->Range(aFCur, aLCur);
    if (!Precision::IsInfinite(f))
    {
      aFCur = f;
    }

    if (!Precision::IsInfinite(l))
    {
      aLCur = l;
    }

    COS->SetRange(aFCur, aLCur);
    lcr.Append(COS);
  }
}

//=======================================================================
// function : UpdateCurves
// purpose  : Insert two pcurves <C1,C2> on surface <S> with location <L>
//           in a list of curve representations <lcr>
//           Remove the pcurves on <S> from <lcr> if <C1> or <C2> is null
//=======================================================================
static void UpdateCurves(NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr,
                         const occ::handle<Geom2d_Curve>&                         C1,
                         const occ::handle<Geom2d_Curve>&                         C2,
                         const occ::handle<Geom_Surface>&                         S,
                         const TopLoc_Location&                                   L,
                         const gp_Pnt2d&                                          Pf,
                         const gp_Pnt2d&                                          Pl)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  occ::handle<BRep_CurveRepresentation>                             cr;
  double f = -Precision::Infinite(), l = Precision::Infinite();

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D)
    {
      static_cast<const BRep_GCurve*>(itcr.Value().get())->Range(f, l);
    }
    else if ((aKind == BRep_CurveRepKind::CurveOnSurface
              || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
             && itcr.Value()->IsCurveOnSurface(S, L))
    {
      break;
    }
    itcr.Next();
  }

  if (itcr.More())
  {
    // cr is used to keep a reference on the curve representation
    // this avoid deleting it as its content may be referenced by C or S
    cr = itcr.Value();
    lcr.Remove(itcr);
  }

  if (!C1.IsNull() && !C2.IsNull())
  {
    occ::handle<BRep_CurveOnClosedSurface> COS =
      new BRep_CurveOnClosedSurface(C1, C2, S, L, GeomAbs_C0);
    double aFCur = 0.0, aLCur = 0.0;
    COS->Range(aFCur, aLCur);
    if (!Precision::IsInfinite(f))
    {
      aFCur = f;
    }

    if (!Precision::IsInfinite(l))
    {
      aLCur = l;
    }

    COS->SetRange(aFCur, aLCur);
    COS->SetUVPoints2(Pf, Pl);
    lcr.Append(COS);
  }
}

static void UpdateCurves(NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr,
                         const occ::handle<Geom_Surface>&                         S1,
                         const occ::handle<Geom_Surface>&                         S2,
                         const TopLoc_Location&                                   L1,
                         const TopLoc_Location&                                   L2,
                         const GeomAbs_Shape                                      C)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  while (itcr.More())
  {
    const occ::handle<BRep_CurveRepresentation>& cr     = itcr.Value();
    bool                                         isregu = cr->IsRegularity() && cr->IsRegularity(S1, S2, L1, L2);
    if (isregu)
      break;
    itcr.Next();
  }

  if (itcr.More())
  {
    occ::handle<BRep_CurveRepresentation> cr = itcr.Value();
    cr->Continuity(C);
  }
  else
  {
    occ::handle<BRep_CurveOn2Surfaces> COS = new BRep_CurveOn2Surfaces(S1, S2, L1, L2, C);
    lcr.Append(COS);
  }
}

static void UpdatePoints(NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr,
                         double                                                   p,
                         const occ::handle<Geom_Curve>&                           C,
                         const TopLoc_Location&                                   L)
{
  NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(lpr);
  while (itpr.More())
  {
    const occ::handle<BRep_PointRepresentation>& pr     = itpr.Value();
    bool                                         isponc = pr->IsPointOnCurve() && pr->IsPointOnCurve(C, L);
    if (isponc)
      break;
    itpr.Next();
  }

  if (itpr.More())
  {
    occ::handle<BRep_PointRepresentation> pr = itpr.Value();
    pr->Parameter(p);
  }
  else
  {
    occ::handle<BRep_PointOnCurve> POC = new BRep_PointOnCurve(p, C, L);
    lpr.Append(POC);
  }
}

static void UpdatePoints(NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr,
                         double                                                   p,
                         const occ::handle<Geom2d_Curve>&                         PC,
                         const occ::handle<Geom_Surface>&                         S,
                         const TopLoc_Location&                                   L)
{
  NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(lpr);
  while (itpr.More())
  {
    const occ::handle<BRep_PointRepresentation>& pr        = itpr.Value();
    bool                                         isponcons = pr->IsPointOnCurveOnSurface() && pr->IsPointOnCurveOnSurface(PC, S, L);
    if (isponcons)
      break;
    itpr.Next();
  }

  if (itpr.More())
  {
    occ::handle<BRep_PointRepresentation> pr = itpr.Value();
    pr->Parameter(p);
  }
  else
  {
    occ::handle<BRep_PointOnCurveOnSurface> POCS = new BRep_PointOnCurveOnSurface(p, PC, S, L);
    lpr.Append(POCS);
  }
}

static void UpdatePoints(NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr,
                         double                                                   p1,
                         double                                                   p2,
                         const occ::handle<Geom_Surface>&                         S,
                         const TopLoc_Location&                                   L)
{
  NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(lpr);
  while (itpr.More())
  {
    const occ::handle<BRep_PointRepresentation>& pr     = itpr.Value();
    bool                                         ispons = pr->IsPointOnSurface() && pr->IsPointOnSurface(S, L);
    if (ispons)
      break;
    itpr.Next();
  }

  if (itpr.More())
  {
    occ::handle<BRep_PointRepresentation> pr = itpr.Value();
    pr->Parameter(p1);
    //    pr->Parameter(p2); // skv
    pr->Parameter2(p2); // skv
  }
  else
  {
    occ::handle<BRep_PointOnSurface> POS = new BRep_PointOnSurface(p1, p2, S, L);
    lpr.Append(POS);
  }
}

//=================================================================================================

void BRep_Builder::MakeFace(TopoDS_Face&                     F,
                            const occ::handle<Geom_Surface>& S,
                            const double                     Tol) const
{
  occ::handle<BRep_TFace> TF = new BRep_TFace();
  if (!F.IsNull() && F.Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::MakeFace");
  }
  TF->Surface(S);
  TF->Tolerance(Tol);
  MakeShape(F, TF);
}

//=================================================================================================

void BRep_Builder::MakeFace(TopoDS_Face&                           theFace,
                            const occ::handle<Poly_Triangulation>& theTriangulation) const
{
  occ::handle<BRep_TFace> aTFace = new BRep_TFace();
  if (!theFace.IsNull() && theFace.Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::MakeFace");
  }
  aTFace->Triangulation(theTriangulation);
  MakeShape(theFace, aTFace);
}

//=================================================================================================

void BRep_Builder::MakeFace(
  TopoDS_Face&                                             theFace,
  const NCollection_List<occ::handle<Poly_Triangulation>>& theTriangulations,
  const occ::handle<Poly_Triangulation>&                   theActiveTriangulation) const
{
  occ::handle<BRep_TFace> aTFace = new BRep_TFace();
  if (!theFace.IsNull() && theFace.Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::MakeFace");
  }
  aTFace->Triangulations(theTriangulations, theActiveTriangulation);
  MakeShape(theFace, aTFace);
}

//=================================================================================================

void BRep_Builder::MakeFace(TopoDS_Face&                     F,
                            const occ::handle<Geom_Surface>& S,
                            const TopLoc_Location&           L,
                            const double                     Tol) const
{
  occ::handle<BRep_TFace> TF = new BRep_TFace();
  if (!F.IsNull() && F.Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::MakeFace");
  }
  TF->Surface(S);
  TF->Tolerance(Tol);
  TF->Location(L);
  MakeShape(F, TF);
}

//=================================================================================================

void BRep_Builder::UpdateFace(const TopoDS_Face&               F,
                              const occ::handle<Geom_Surface>& S,
                              const TopLoc_Location&           L,
                              const double                     Tol) const
{
  const occ::handle<BRep_TFace>& TF = *((occ::handle<BRep_TFace>*)&F.TShape());
  if (TF->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateFace");
  }
  TF->Surface(S);
  TF->Tolerance(Tol);
  TF->Location(L.Predivided(F.Location()));
  F.TShape()->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateFace(const TopoDS_Face&                     theFace,
                              const occ::handle<Poly_Triangulation>& theTriangulation,
                              const bool                             theToReset) const
{
  const occ::handle<BRep_TFace>& aTFace = *((occ::handle<BRep_TFace>*)&theFace.TShape());
  if (aTFace->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateFace");
  }
  aTFace->Triangulation(theTriangulation, theToReset);
  theFace.TShape()->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateFace(const TopoDS_Face& F, const double Tol) const
{
  const occ::handle<BRep_TFace>& TF = *((occ::handle<BRep_TFace>*)&F.TShape());
  if (TF->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateFace");
  }
  TF->Tolerance(Tol);
  F.TShape()->Modified(true);
}

//=================================================================================================

void BRep_Builder::NaturalRestriction(const TopoDS_Face& F, const bool N) const
{
  const occ::handle<BRep_TFace>& TF = (*((occ::handle<BRep_TFace>*)&F.TShape()));
  if (TF->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::NaturalRestriction");
  }
  TF->NaturalRestriction(N);
  F.TShape()->Modified(true);
}

//=================================================================================================

void BRep_Builder::MakeEdge(TopoDS_Edge& E) const
{
  occ::handle<BRep_TEdge> TE = new BRep_TEdge();
  if (!E.IsNull() && E.Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::MakeEdge");
  }
  MakeShape(E, TE);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&             E,
                              const occ::handle<Geom_Curve>& C,
                              const TopLoc_Location&         L,
                              const double                   Tol) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  occ::handle<BRep_Curve3D> aCached3D;
  UpdateCurves(TE->ChangeCurves(), C, l, aCached3D);
  TE->Curve3D(aCached3D);

  TE->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&               E,
                              const occ::handle<Geom2d_Curve>& C,
                              const occ::handle<Geom_Surface>& S,
                              const TopLoc_Location&           L,
                              const double                     Tol) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  UpdateCurves(TE->ChangeCurves(), C, S, l);

  TE->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=======================================================================
// function : UpdateEdge
// purpose  : for the second format (for XML Persistence)
//=======================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&               E,
                              const occ::handle<Geom2d_Curve>& C,
                              const occ::handle<Geom_Surface>& S,
                              const TopLoc_Location&           L,
                              const double                     Tol,
                              const gp_Pnt2d&                  Pf,
                              const gp_Pnt2d&                  Pl) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  UpdateCurves(TE->ChangeCurves(), C, S, l, Pf, Pl);

  TE->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&               E,
                              const occ::handle<Geom2d_Curve>& C1,
                              const occ::handle<Geom2d_Curve>& C2,
                              const occ::handle<Geom_Surface>& S,
                              const TopLoc_Location&           L,
                              const double                     Tol) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  UpdateCurves(TE->ChangeCurves(), C1, C2, S, l);

  TE->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=======================================================================
// function : UpdateEdge
// purpose  : for the second format (for XML Persistence)
//=======================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&               E,
                              const occ::handle<Geom2d_Curve>& C1,
                              const occ::handle<Geom2d_Curve>& C2,
                              const occ::handle<Geom_Surface>& S,
                              const TopLoc_Location&           L,
                              const double                     Tol,
                              const gp_Pnt2d&                  Pf,
                              const gp_Pnt2d&                  Pl) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  UpdateCurves(TE->ChangeCurves(), C1, C2, S, l, Pf, Pl);

  TE->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                 E,
                              const occ::handle<Poly_Polygon3D>& P,
                              const TopLoc_Location&             L) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

  while (itcr.More())
  {
    if (itcr.Value()->IsPolygon3D())
    {
      if (P.IsNull())
      {
        lcr.Remove(itcr);
        TE->Polygon3D(nullptr);
      }
      else
      {
        itcr.Value()->Polygon3D(P);
        TE->Polygon3D(static_cast<BRep_Polygon3D*>(itcr.Value().get()));
      }
      TE->Modified(true);
      return;
    }
    itcr.Next();
  }

  const TopLoc_Location       l   = L.Predivided(E.Location());
  occ::handle<BRep_Polygon3D> P3d = new BRep_Polygon3D(P, l);
  lcr.Append(P3d);
  TE->Polygon3D(P3d);

  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                              E,
                              const occ::handle<Poly_PolygonOnTriangulation>& P,
                              const occ::handle<Poly_Triangulation>&          T,
                              const TopLoc_Location&                          L) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  bool isModified = false;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  occ::handle<BRep_CurveRepresentation>                             cr;

  while (itcr.More())
  {
    if (itcr.Value()->IsPolygonOnTriangulation(T, l))
    {
      // cr is used to keep a reference on the curve representation
      // this avoid deleting it as its content may be referenced by T
      cr = itcr.Value();
      lcr.Remove(itcr);
      isModified = true;
      break;
    }
    itcr.Next();
  }

  if (!P.IsNull())
  {
    occ::handle<BRep_PolygonOnTriangulation> PT = new BRep_PolygonOnTriangulation(P, T, l);
    lcr.Append(PT);
    isModified = true;
  }

  if (isModified)
    TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                              E,
                              const occ::handle<Poly_PolygonOnTriangulation>& P1,
                              const occ::handle<Poly_PolygonOnTriangulation>& P2,
                              const occ::handle<Poly_Triangulation>&          T,
                              const TopLoc_Location&                          L) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  bool isModified = false;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  occ::handle<BRep_CurveRepresentation>                             cr;

  while (itcr.More())
  {
    if (itcr.Value()->IsPolygonOnTriangulation(T, l)) // szv:was L
    {
      // cr is used to keep a reference on the curve representation
      // this avoid deleting it as its content may be referenced by T
      cr = itcr.Value();
      lcr.Remove(itcr);
      isModified = true;
      break;
    }
    itcr.Next();
  }

  if (!P1.IsNull() && !P2.IsNull())
  {
    occ::handle<BRep_PolygonOnClosedTriangulation> PT =
      new BRep_PolygonOnClosedTriangulation(P1, P2, T, l);
    lcr.Append(PT);
    isModified = true;
  }

  if (isModified)
    TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                 E,
                              const occ::handle<Poly_Polygon2D>& P,
                              const TopoDS_Face&                 F) const
{
  TopLoc_Location                  l;
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(F, l);
  UpdateEdge(E, P, S, l);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                 E,
                              const occ::handle<Poly_Polygon2D>& P,
                              const occ::handle<Geom_Surface>&   S,
                              const TopLoc_Location&             L) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  TopLoc_Location l = L.Predivided(E.Location());

  NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr = TE->ChangeCurves();
  occ::handle<BRep_CurveRepresentation>                    cr;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  while (itcr.More())
  {
    if (itcr.Value()->IsPolygonOnSurface(S, l))
      break;
    itcr.Next();
  }

  if (itcr.More())
  {
    // cr is used to keep a reference on the curve representation
    // this avoid deleting it as its content may be referenced by T
    cr = itcr.Value();
    lcr.Remove(itcr);
  }

  if (!P.IsNull())
  {
    occ::handle<BRep_PolygonOnSurface> PS = new BRep_PolygonOnSurface(P, S, l);
    lcr.Append(PS);
  }

  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                 E,
                              const occ::handle<Poly_Polygon2D>& P1,
                              const occ::handle<Poly_Polygon2D>& P2,
                              const TopoDS_Face&                 F) const
{
  TopLoc_Location                  l;
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(F, l);
  UpdateEdge(E, P1, P2, S, l);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge&                 E,
                              const occ::handle<Poly_Polygon2D>& P1,
                              const occ::handle<Poly_Polygon2D>& P2,
                              const occ::handle<Geom_Surface>&   S,
                              const TopLoc_Location&             L) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  TopLoc_Location l = L.Predivided(E.Location());

  NCollection_List<occ::handle<BRep_CurveRepresentation>>& lcr = TE->ChangeCurves();
  occ::handle<BRep_CurveRepresentation>                    cr;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);
  while (itcr.More())
  {
    if (itcr.Value()->IsPolygonOnSurface(S, l))
      break;
    itcr.Next();
  }

  if (itcr.More())
  {
    // cr is used to keep a reference on the curve representation
    // this avoid deleting it as its content may be referenced by T
    cr = itcr.Value();
    lcr.Remove(itcr);
  }

  if (!P1.IsNull() && !P2.IsNull())
  {
    occ::handle<BRep_PolygonOnClosedSurface> PS =
      new BRep_PolygonOnClosedSurface(P1, P2, S, TopLoc_Location());
    lcr.Append(PS);
  }

  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::UpdateEdge(const TopoDS_Edge& E, const double Tol) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateEdge");
  }
  TE->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::Continuity(const TopoDS_Edge&  E,
                              const TopoDS_Face&  F1,
                              const TopoDS_Face&  F2,
                              const GeomAbs_Shape C) const
{
  TopLoc_Location                  l1, l2;
  const occ::handle<Geom_Surface>& S1 = BRep_Tool::Surface(F1, l1);
  const occ::handle<Geom_Surface>& S2 = BRep_Tool::Surface(F2, l2);
  Continuity(E, S1, S2, l1, l2, C);
}

//=================================================================================================

void BRep_Builder::Continuity(const TopoDS_Edge&               E,
                              const occ::handle<Geom_Surface>& S1,
                              const occ::handle<Geom_Surface>& S2,
                              const TopLoc_Location&           L1,
                              const TopLoc_Location&           L2,
                              const GeomAbs_Shape              C) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::Continuity");
  }
  const TopLoc_Location l1 = L1.Predivided(E.Location());
  const TopLoc_Location l2 = L2.Predivided(E.Location());

  UpdateCurves(TE->ChangeCurves(), S1, S2, l1, l2, C);

  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::SameParameter(const TopoDS_Edge& E, const bool S) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::SameParameter");
  }
  TE->SameParameter(S);
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::SameRange(const TopoDS_Edge& E, const bool S) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::SameRange");
  }
  TE->SameRange(S);
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::Degenerated(const TopoDS_Edge& E, const bool D) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::Degenerated");
  }
  TE->Degenerated(D);
  if (D)
  {
    // set a null 3d curve
    occ::handle<BRep_Curve3D> aCached3D;
    UpdateCurves(TE->ChangeCurves(), occ::handle<Geom_Curve>(), E.Location(), aCached3D);
    TE->Curve3D(aCached3D);
  }
  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::Range(const TopoDS_Edge& E,
                         const double       First,
                         const double       Last,
                         const bool         Only3d) const
{
  //  set the range to all the representations if Only3d=FALSE
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::Range");
  }
  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D || aKind == BRep_CurveRepKind::CurveOnSurface
        || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
    {
      if (!Only3d || aKind == BRep_CurveRepKind::Curve3D)
        static_cast<BRep_GCurve*>(itcr.Value().get())->SetRange(First, Last);
    }
    itcr.Next();
  }

  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::Range(const TopoDS_Edge&               E,
                         const occ::handle<Geom_Surface>& S,
                         const TopLoc_Location&           L,
                         const double                     First,
                         const double                     Last) const
{
  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
  if (TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::Range");
  }
  const TopLoc_Location l = L.Predivided(E.Location());

  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if ((aKind == BRep_CurveRepKind::CurveOnSurface
         || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
        && itcr.Value()->IsCurveOnSurface(S, l))
    {
      static_cast<BRep_GCurve*>(itcr.Value().get())->SetRange(First, Last);
      break;
    }
    itcr.Next();
  }

  if (!itcr.More())
    throw Standard_DomainError("BRep_Builder::Range, no pcurve");

  TE->Modified(true);
}

//=================================================================================================

void BRep_Builder::Transfert(const TopoDS_Edge& Ein, const TopoDS_Edge& Eout) const
{
  const occ::handle<BRep_TEdge>& TE  = *((occ::handle<BRep_TEdge>*)&Ein.TShape());
  const double                   tol = TE->Tolerance();

  const NCollection_List<occ::handle<BRep_CurveRepresentation>>&    lcr = TE->Curves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

  while (itcr.More())
  {

    const occ::handle<BRep_CurveRepresentation>& CR = itcr.Value();

    // Check closed surface first: IsCurveOnClosedSurface() implies IsCurveOnSurface().
    if (CR->IsCurveOnClosedSurface())
    {
      UpdateEdge(Eout,
                 CR->PCurve(),
                 CR->PCurve2(),
                 CR->Surface(),
                 Ein.Location() * CR->Location(),
                 tol);
    }
    else if (CR->IsCurveOnSurface())
    {
      UpdateEdge(Eout, CR->PCurve(), CR->Surface(), Ein.Location() * CR->Location(), tol);
    }

    if (CR->IsRegularity())
    {
      Continuity(Eout,
                 CR->Surface(),
                 CR->Surface2(),
                 Ein.Location() * CR->Location(),
                 Ein.Location() * CR->Location2(),
                 CR->Continuity());
    }

    itcr.Next();
  }
}

//=======================================================================
// function : UpdateVertex
// purpose  : update vertex with 3d point
//=======================================================================

void BRep_Builder::UpdateVertex(const TopoDS_Vertex& V, const gp_Pnt& P, const double Tol) const
{
  const occ::handle<BRep_TVertex>& TV = *((occ::handle<BRep_TVertex>*)&V.TShape());
  if (TV->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateVertex");
  }
  TV->Pnt(P.Transformed(V.Location().Inverted().Transformation()));
  TV->UpdateTolerance(Tol);
  TV->Modified(true);
}

//=======================================================================
// function : UpdateVertex
// purpose  : update vertex with parameter on edge
//=======================================================================

void BRep_Builder::UpdateVertex(const TopoDS_Vertex& V,
                                const double         Par,
                                const TopoDS_Edge&   E,
                                const double         Tol) const
{
  if (Precision::IsPositiveInfinite(Par) || Precision::IsNegativeInfinite(Par))
    throw Standard_DomainError("BRep_Builder::Infinite parameter");

  const occ::handle<BRep_TVertex>& TV = *((occ::handle<BRep_TVertex>*)&V.TShape());
  const occ::handle<BRep_TEdge>&   TE = *((occ::handle<BRep_TEdge>*)&E.TShape());

  if (TV->Locked() || TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateVertex");
  }

  TopLoc_Location L = E.Location().Predivided(V.Location());

  // Search the vertex in the edge
  TopAbs_Orientation ori = TopAbs_INTERNAL;

  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  // if the edge has no vertices
  // and is degenerated use the vertex orientation
  // RLE, june 94

  if (!itv.More() && TE->Degenerated())
    ori = V.Orientation();

  while (itv.More())
  {
    const TopoDS_Shape& Vcur = itv.Value();
    if (V.IsSame(Vcur))
    {
      ori = Vcur.Orientation();
      if (ori == V.Orientation())
        break;
    }
    itv.Next();
  }

  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if (aKind == BRep_CurveRepKind::Curve3D || aKind == BRep_CurveRepKind::CurveOnSurface
        || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
    {
      BRep_GCurve* GC = static_cast<BRep_GCurve*>(itcr.Value().get());
      if (ori == TopAbs_FORWARD)
        GC->First(Par);
      else if (ori == TopAbs_REVERSED)
        GC->Last(Par);
      else
      {
        NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr    = TV->ChangePoints();
        const TopLoc_Location&                                   GCloc  = GC->Location();
        TopLoc_Location                                          LGCloc = L * GCloc;
        if (aKind == BRep_CurveRepKind::Curve3D)
        {
          const occ::handle<Geom_Curve>& GC3d = GC->Curve3D();
          UpdatePoints(lpr, Par, GC3d, LGCloc);
        }
        else // CurveOnSurface or CurveOnClosedSurface
        {
          const occ::handle<Geom2d_Curve>& GCpc = GC->PCurve();
          const occ::handle<Geom_Surface>& GCsu = GC->Surface();
          UpdatePoints(lpr, Par, GCpc, GCsu, LGCloc);
        }
      }
    }
    itcr.Next();
  }

  if ((ori != TopAbs_FORWARD) && (ori != TopAbs_REVERSED))
    TV->Modified(true);
  TV->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=======================================================================
// function : UpdateVertex
// purpose  : update vertex with parameter on edge on face
//=======================================================================

void BRep_Builder::UpdateVertex(const TopoDS_Vertex&             V,
                                const double                     Par,
                                const TopoDS_Edge&               E,
                                const occ::handle<Geom_Surface>& S,
                                const TopLoc_Location&           L,
                                const double                     Tol) const
{
  if (Precision::IsPositiveInfinite(Par) || Precision::IsNegativeInfinite(Par))
    throw Standard_DomainError("BRep_Builder::Infinite parameter");

  // Find the curve representation
  TopLoc_Location l = L.Predivided(V.Location());

  const occ::handle<BRep_TVertex>& TV = *((occ::handle<BRep_TVertex>*)&V.TShape());
  const occ::handle<BRep_TEdge>&   TE = *((occ::handle<BRep_TEdge>*)&E.TShape());

  if (TV->Locked() || TE->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateVertex");
  }

  // Search the vertex in the edge
  TopAbs_Orientation ori = TopAbs_INTERNAL;

  TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));

  // if the edge has no vertices
  // and is degenerated use the vertex orientation
  // RLE, june 94

  if (!itv.More() && TE->Degenerated())
    ori = V.Orientation();

  while (itv.More())
  {
    const TopoDS_Shape& Vcur = itv.Value();
    if (V.IsSame(Vcur))
    {
      ori = Vcur.Orientation();
      if (ori == V.Orientation())
        break;
    }
    itv.Next();
  }

  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          lcr = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(lcr);

  while (itcr.More())
  {
    const BRep_CurveRepKind aKind = itcr.Value()->RepresentationKind();
    if ((aKind == BRep_CurveRepKind::CurveOnSurface
         || aKind == BRep_CurveRepKind::CurveOnClosedSurface)
        && itcr.Value()->IsCurveOnSurface(S, L))
    { // xpu020198 : BUC60407
      BRep_GCurve* GC = static_cast<BRep_GCurve*>(itcr.Value().get());
      if (ori == TopAbs_FORWARD)
        GC->First(Par);
      else if (ori == TopAbs_REVERSED)
        GC->Last(Par);
      else
      {
        NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr  = TV->ChangePoints();
        const occ::handle<Geom2d_Curve>&                         GCpc = GC->PCurve();
        UpdatePoints(lpr, Par, GCpc, S, l);
        TV->Modified(true);
      }
      break;
    }
    itcr.Next();
  }

  if (!itcr.More())
    throw Standard_DomainError("BRep_Builder:: no pcurve");

  TV->UpdateTolerance(Tol);
  TE->Modified(true);
}

//=======================================================================
// function : UpdateVertex
// purpose  : update vertex with parameters on face
//=======================================================================

void BRep_Builder::UpdateVertex(const TopoDS_Vertex& Ve,
                                const double         U,
                                const double         V,
                                const TopoDS_Face&   F,
                                const double         Tol) const
{
  const occ::handle<BRep_TVertex>& TV = *((occ::handle<BRep_TVertex>*)&Ve.TShape());

  if (TV->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateVertex");
  }

  TopLoc_Location                  L;
  const occ::handle<Geom_Surface>& S                           = BRep_Tool::Surface(F, L);
  L                                                            = L.Predivided(Ve.Location());
  NCollection_List<occ::handle<BRep_PointRepresentation>>& lpr = TV->ChangePoints();
  UpdatePoints(lpr, U, V, S, L);

  TV->UpdateTolerance(Tol);
  TV->Modified(true);
}

//=======================================================================
// function : UpdateVertex
// purpose  : update vertex with 3d point
//=======================================================================

void BRep_Builder::UpdateVertex(const TopoDS_Vertex& V, const double Tol) const
{
  const occ::handle<BRep_TVertex>& TV = *((occ::handle<BRep_TVertex>*)&V.TShape());

  if (TV->Locked())
  {
    throw TopoDS_LockedShape("BRep_Builder::UpdateVertex");
  }

  TV->UpdateTolerance(Tol);
  TV->Modified(true);
}

//=================================================================================================

void BRep_Builder::Transfert(const TopoDS_Edge&   Ein,
                             const TopoDS_Edge&   Eout,
                             const TopoDS_Vertex& Vin,
                             const TopoDS_Vertex& Vout) const
{
  const double tol   = BRep_Tool::Tolerance(Vin);
  const double parin = BRep_Tool::Parameter(Vin, Ein);
  UpdateVertex(Vout, parin, Eout, tol);
}
