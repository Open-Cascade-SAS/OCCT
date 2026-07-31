// Copyright (c) 1995-1999 Matra Datavision
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

#include <DrawTrSurf_BSplineSurface.hxx>

#include <Adaptor3d_IsoCurve.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_MarkerShape.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_Params.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BSplineSurface, DrawTrSurf_Surface)

DrawTrSurf_BSplineSurface::DrawTrSurf_BSplineSurface(const occ::handle<Geom_BSplineSurface>& S)
    : DrawTrSurf_Surface(S,
                         S->NbUKnots() - 2,
                         S->NbVKnots() - 2,
                         Draw_jaune,
                         Draw_bleu,
                         30,
                         0.05,
                         0)
{
  drawPoles = true;
  drawKnots = true;
  knotsIsos = true;
  knotsForm = Draw_Losange;
  knotsLook = Draw_violet;
  knotsDim  = 5;
  polesLook = Draw_rouge;
}

DrawTrSurf_BSplineSurface::DrawTrSurf_BSplineSurface(const occ::handle<Geom_BSplineSurface>& S,
                                                     const Draw_Color&      BoundsColor,
                                                     const Draw_Color&      IsosColor,
                                                     const Draw_Color&      PolesColor,
                                                     const Draw_Color&      KnotsColor,
                                                     const Draw_MarkerShape KnotsShape,
                                                     const int              KnotsSize,
                                                     const bool             ShowPoles,
                                                     const bool             ShowKnots,
                                                     const int              Discret,
                                                     const double           Deflection,
                                                     const int              DrawMode)
    : DrawTrSurf_Surface(S,
                         S->NbUKnots() - 2,
                         S->NbVKnots() - 2,
                         BoundsColor,
                         IsosColor,
                         Discret,
                         Deflection,
                         DrawMode)
{
  knotsIsos = true;
  drawPoles = ShowPoles;
  drawKnots = ShowKnots;
  knotsForm = KnotsShape;
  knotsLook = KnotsColor;
  knotsDim  = KnotsSize;
  polesLook = PolesColor;
}

DrawTrSurf_BSplineSurface::DrawTrSurf_BSplineSurface(const occ::handle<Geom_BSplineSurface>& S,
                                                     const int              NbUIsos,
                                                     const int              NbVIsos,
                                                     const Draw_Color&      BoundsColor,
                                                     const Draw_Color&      IsosColor,
                                                     const Draw_Color&      PolesColor,
                                                     const Draw_Color&      KnotsColor,
                                                     const Draw_MarkerShape KnotsShape,
                                                     const int              KnotsSize,
                                                     const bool             ShowPoles,
                                                     const bool             ShowKnots,
                                                     const int              Discret,
                                                     const double           Deflection,
                                                     const int              DrawMode)
    : DrawTrSurf_Surface(S,
                         std::abs(NbUIsos),
                         std::abs(NbVIsos),
                         BoundsColor,
                         IsosColor,
                         Discret,
                         Deflection,
                         DrawMode)
{
  knotsIsos = false;
  drawPoles = ShowPoles;
  drawKnots = ShowKnots;
  knotsForm = KnotsShape;
  knotsLook = KnotsColor;
  knotsDim  = KnotsSize;
  polesLook = PolesColor;
}

void DrawTrSurf_BSplineSurface::DrawOn(Draw_Display& dis) const
{
  occ::handle<Geom_BSplineSurface> S = occ::down_cast<Geom_BSplineSurface>(surf);
  int                              i, j;

  double Ua, Ub, Va, Vb;
  S->Bounds(Ua, Ub, Va, Vb);

  if (drawPoles)
  {
    dis.SetColor(polesLook);
    const NCollection_Array2<gp_Pnt>& SPoles   = S->Poles();
    int                               NbUPoles = SPoles.NbRows();
    int                               NbVPoles = SPoles.NbColumns();
    for (j = 1; j <= NbVPoles; j++)
    {
      dis.MoveTo(SPoles(1, j));
      for (i = 2; i <= NbUPoles; i++)
      {
        dis.DrawTo(SPoles(i, j));
      }
      if (S->IsUPeriodic())
      {
        dis.DrawTo(SPoles(1, j));
      }
    }
    for (i = 1; i <= NbUPoles; i++)
    {
      dis.MoveTo(SPoles(i, 1));
      for (j = 2; j <= NbVPoles; j++)
      {
        dis.DrawTo(SPoles(i, j));
      }
      if (S->IsVPeriodic())
      {
        dis.DrawTo(SPoles(i, 1));
      }
    }
  }

  if (knotsIsos)
  {
    int                              first, last;
    occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
    HS->Load(surf);

    Adaptor3d_IsoCurve C(HS);

    dis.SetColor(isosLook);
    first = S->FirstUKnotIndex() + 1;
    last  = S->LastUKnotIndex() - 1;
    for (i = first; i <= last; i++)
    {
      DrawIsoCurveOn(C, GeomAbs_IsoU, S->UKnot(i), Va, Vb, dis);
    }
    first = S->FirstVKnotIndex() + 1;
    last  = S->LastVKnotIndex() - 1;
    for (j = first; j <= last; j++)
    {
      DrawIsoCurveOn(C, GeomAbs_IsoV, S->VKnot(j), Ua, Ub, dis);
    }
  }

  DrawTrSurf_Surface::DrawOn(dis, !knotsIsos);

  if (drawKnots)
  {
    int                               first, last;
    const NCollection_Array1<double>& SUKnots = S->UKnots();
    dis.SetColor(knotsLook);
    first = S->FirstUKnotIndex();
    last  = S->LastUKnotIndex();
    for (i = first; i <= last; i++)
    {
      dis.DrawMarker(S->Value(SUKnots(i), Va), knotsForm, knotsDim);
    }
    const NCollection_Array1<double>& SVKnots = S->VKnots();
    dis.SetColor(knotsLook);
    first = S->FirstVKnotIndex();
    last  = S->LastVKnotIndex();
    for (j = first; j <= last; j++)
    {
      dis.DrawMarker(S->Value(Ua, SVKnots(j)), knotsForm, knotsDim);
    }
  }
}

void DrawTrSurf_BSplineSurface::ShowKnotsIsos()
{
  knotsIsos                          = true;
  occ::handle<Geom_BSplineSurface> S = occ::down_cast<Geom_BSplineSurface>(surf);
  nbUIsos                            = S->NbUKnots() - 2;
  nbVIsos                            = S->NbVKnots() - 2;
}

void DrawTrSurf_BSplineSurface::ClearIsos()
{
  knotsIsos = false;
  nbUIsos   = 0;
  nbVIsos   = 0;
}

void DrawTrSurf_BSplineSurface::ShowIsos(const int Nu, const int Nv)
{
  knotsIsos = false;
  nbUIsos   = std::abs(Nu);
  nbVIsos   = std::abs(Nv);
}

void DrawTrSurf_BSplineSurface::FindPole(const double        X,
                                         const double        Y,
                                         const Draw_Display& D,
                                         const double        XPrec,
                                         int&                UIndex,
                                         int&                VIndex) const
{
  occ::handle<Geom_BSplineSurface> bs = occ::down_cast<Geom_BSplineSurface>(surf);
  gp_Pnt2d                         p1(X / D.Zoom(), Y / D.Zoom());
  double                           Prec = XPrec / D.Zoom();
  UIndex++;
  VIndex++;
  int NbUPoles = bs->NbUPoles();
  int NbVPoles = bs->NbVPoles();
  while (VIndex <= NbVPoles)
  {
    while (UIndex <= NbUPoles)
    {
      if (D.Project(bs->Pole(UIndex, VIndex)).Distance(p1) <= Prec)
      {
        return;
      }
      UIndex++;
    }
    UIndex = 1;
    VIndex++;
  }
  UIndex = VIndex = 0;
}

void DrawTrSurf_BSplineSurface::FindUKnot(const double        X,
                                          const double        Y,
                                          const Draw_Display& D,
                                          const double        Prec,
                                          int&                UIndex) const
{
  occ::handle<Geom_BSplineSurface> bs = occ::down_cast<Geom_BSplineSurface>(surf);
  gp_Pnt2d                         p1(X, Y);
  UIndex++;
  int    NbUKnots = bs->NbUKnots();
  double U1, U2, V1, V2;
  surf->Bounds(U1, U2, V1, V2);
  while (UIndex <= NbUKnots)
  {
    if (D.Project(bs->Value(bs->UKnot(UIndex), V1)).Distance(p1) <= Prec)
    {
      return;
    }
    UIndex++;
  }
  UIndex = 0;
}

void DrawTrSurf_BSplineSurface::FindVKnot(const double        X,
                                          const double        Y,
                                          const Draw_Display& D,
                                          const double        Prec,
                                          int&                VIndex) const
{
  occ::handle<Geom_BSplineSurface> bs = occ::down_cast<Geom_BSplineSurface>(surf);
  gp_Pnt2d                         p1(X, Y);
  VIndex++;
  int    NbVKnots = bs->NbVKnots();
  double U1, U2, V1, V2;
  surf->Bounds(U1, U2, V1, V2);
  while (VIndex <= NbVKnots)
  {
    if (D.Project(bs->Value(U1, bs->VKnot(VIndex))).Distance(p1) <= Prec)
    {
      return;
    }
    VIndex++;
  }
  VIndex = 0;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BSplineSurface::Copy() const
{
  occ::handle<DrawTrSurf_BSplineSurface> DS;
  if (!knotsIsos)
  {
    DS = new DrawTrSurf_BSplineSurface(occ::down_cast<Geom_BSplineSurface>(surf->Copy()),
                                       nbUIsos,
                                       nbVIsos,
                                       boundsLook,
                                       isosLook,
                                       polesLook,
                                       knotsLook,
                                       knotsForm,
                                       knotsDim,
                                       drawPoles,
                                       drawKnots,
                                       GetDiscretisation(),
                                       GetDeflection(),
                                       GetDrawMode());
  }
  else
  {
    DS = new DrawTrSurf_BSplineSurface(occ::down_cast<Geom_BSplineSurface>(surf->Copy()),
                                       boundsLook,
                                       isosLook,
                                       polesLook,
                                       knotsLook,
                                       knotsForm,
                                       knotsDim,
                                       drawPoles,
                                       drawKnots,
                                       GetDiscretisation(),
                                       GetDeflection(),
                                       GetDrawMode());
  }
  return DS;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BSplineSurface::Restore(Standard_IStream& theStream)
{
  const DrawTrSurf_Params&         aParams = DrawTrSurf::Parameters();
  occ::handle<Geom_BSplineSurface> aGeomSurface =
    occ::down_cast<Geom_BSplineSurface>(GeomTools_SurfaceSet::ReadSurface(theStream));
  occ::handle<DrawTrSurf_BSplineSurface> aDrawSurface;
  if (!aParams.NeedKnotsIsos)
  {
    aDrawSurface = new DrawTrSurf_BSplineSurface(aGeomSurface,
                                                 aParams.NbUIsos,
                                                 aParams.NbVIsos,
                                                 aParams.BoundsColor,
                                                 aParams.IsosColor,
                                                 aParams.PolesColor,
                                                 aParams.KnotsColor,
                                                 aParams.KnotsMarker,
                                                 aParams.KnotsSize,
                                                 aParams.IsShowPoles,
                                                 aParams.IsShowKnots,
                                                 aParams.Discret,
                                                 aParams.Deflection,
                                                 aParams.DrawMode);
  }
  else
  {
    aDrawSurface = new DrawTrSurf_BSplineSurface(aGeomSurface,
                                                 aParams.BoundsColor,
                                                 aParams.IsosColor,
                                                 aParams.PolesColor,
                                                 aParams.KnotsColor,
                                                 aParams.KnotsMarker,
                                                 aParams.KnotsSize,
                                                 aParams.IsShowPoles,
                                                 aParams.IsShowKnots,
                                                 aParams.Discret,
                                                 aParams.Deflection,
                                                 aParams.DrawMode);
  }
  return aDrawSurface;
}
