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

#include <DrawTrSurf_Surface.hxx>

#include <Adaptor3d_IsoCurve.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Params.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_Surface, DrawTrSurf_Drawable)

double DrawTrSurf_SurfaceLimit = 400;

//=================================================================================================

DrawTrSurf_Surface::DrawTrSurf_Surface(const occ::handle<Geom_Surface>& S)
    : DrawTrSurf_Drawable(16, 0.01, 1)
{
  surf       = S;
  boundsLook = Draw_jaune;
  isosLook   = Draw_bleu;
  nbUIsos    = 1;
  nbVIsos    = 1;
}

//=================================================================================================

DrawTrSurf_Surface::DrawTrSurf_Surface(const occ::handle<Geom_Surface>& S,
                                       const int      Nu,
                                       const int      Nv,
                                       const Draw_Color&           BoundsColor,
                                       const Draw_Color&           IsosColor,
                                       const int      Discret,
                                       const double         Deflection,
                                       const int      DrawMode)
    : DrawTrSurf_Drawable(Discret, Deflection, DrawMode)
{
  surf       = S;
  boundsLook = BoundsColor;
  isosLook   = IsosColor;
  nbUIsos    = std::abs(Nu);
  nbVIsos    = std::abs(Nv);
}

//=================================================================================================

void DrawTrSurf_Surface::DrawOn(Draw_Display& dis) const
{
  DrawOn(dis, true);
}

//=================================================================================================

void DrawTrSurf_Surface::DrawOn(Draw_Display& dis, const bool Iso) const
{
  double UFirst, ULast, VFirst, VLast;
  surf->Bounds(UFirst, ULast, VFirst, VLast);

  bool UfirstInf = Precision::IsNegativeInfinite(UFirst);
  bool UlastInf  = Precision::IsPositiveInfinite(ULast);
  bool VfirstInf = Precision::IsNegativeInfinite(VFirst);
  bool VlastInf  = Precision::IsPositiveInfinite(VLast);

  if (UfirstInf || UlastInf)
  {
    gp_Pnt        P1, P2;
    double v;
    if (VfirstInf && VlastInf)
      v = 0;
    else if (VfirstInf)
      v = VLast;
    else if (VlastInf)
      v = VFirst;
    else
      v = (VFirst + VLast) / 2;

    double delta = 1.;

    if (UfirstInf && UlastInf)
    {
      do
      {
        delta *= 2;
        UFirst = -delta;
        ULast  = delta;
        surf->D0(UFirst, v, P1);
        surf->D0(ULast, v, P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (UfirstInf)
    {
      surf->D0(ULast, v, P2);
      do
      {
        delta *= 2;
        UFirst = ULast - delta;
        surf->D0(UFirst, v, P1);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (UlastInf)
    {
      surf->D0(UFirst, v, P1);
      do
      {
        delta *= 2;
        ULast = UFirst + delta;
        surf->D0(ULast, v, P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
  }

  if (VfirstInf || VlastInf)
  {
    gp_Pnt        P1, P2;
    double u = (UFirst + ULast) / 2;

    double delta = 1.;

    if (VfirstInf && VlastInf)
    {
      do
      {
        delta *= 2;
        VFirst = -delta;
        VLast  = delta;
        surf->D0(u, VFirst, P1);
        surf->D0(u, VLast, P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (VfirstInf)
    {
      surf->D0(u, VLast, P2);
      do
      {
        delta *= 2;
        VFirst = VLast - delta;
        surf->D0(u, VFirst, P1);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
    else if (VlastInf)
    {
      surf->D0(u, VFirst, P1);
      do
      {
        delta *= 2;
        VLast = VFirst + delta;
        surf->D0(u, VLast, P2);
      } while (P1.Distance(P2) < DrawTrSurf_SurfaceLimit);
    }
  }

  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();
  HS->Load(surf, UFirst, ULast, VFirst, VLast);

  Adaptor3d_IsoCurve C(HS);

  if (Iso)
  {
    dis.SetColor(isosLook);
    int i, j;

    double Du = (ULast - UFirst) / (nbUIsos + 1);
    double U  = UFirst;
    for (i = 1; i <= nbUIsos; i++)
    {
      U += Du;
      DrawIsoCurveOn(C, GeomAbs_IsoU, U, VFirst, VLast, dis);
    }

    double Dv = (VLast - VFirst) / (nbVIsos + 1);
    double V  = VFirst;
    for (j = 1; j <= nbVIsos; j++)
    {
      V += Dv;
      DrawIsoCurveOn(C, GeomAbs_IsoV, V, UFirst, ULast, dis);
    }
  }

  // draw bounds
  dis.SetColor(boundsLook);
  if (!UfirstInf)
    DrawIsoCurveOn(C, GeomAbs_IsoU, UFirst, VFirst, VLast, dis);
  if (!UlastInf)
    DrawIsoCurveOn(C, GeomAbs_IsoU, ULast, VFirst, VLast, dis);
  if (!VfirstInf)
    DrawIsoCurveOn(C, GeomAbs_IsoV, VFirst, UFirst, ULast, dis);
  if (!VlastInf)
    DrawIsoCurveOn(C, GeomAbs_IsoV, VLast, UFirst, ULast, dis);

  // draw marker
  DrawIsoCurveOn(C,
                 GeomAbs_IsoU,
                 UFirst + (ULast - UFirst) / 10.,
                 VFirst,
                 VFirst + (VLast - VFirst) / 10.,
                 dis);
}

//=================================================================================================

void DrawTrSurf_Surface::ClearIsos()
{
  nbUIsos = 0;
  nbVIsos = 0;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_Surface::Copy() const
{
  occ::handle<DrawTrSurf_Surface> DS =
    new DrawTrSurf_Surface(occ::down_cast<Geom_Surface>(surf->Copy()),
                           nbUIsos,
                           nbVIsos,
                           boundsLook,
                           isosLook,
                           GetDiscretisation(),
                           GetDeflection(),
                           GetDrawMode());

  return DS;
}

//=================================================================================================

void DrawTrSurf_Surface::Dump(Standard_OStream& S) const
{
  GeomTools_SurfaceSet::PrintSurface(surf, S);
}

//=================================================================================================

void DrawTrSurf_Surface::Save(Standard_OStream& theStream) const
{
  GeomTools_SurfaceSet::PrintSurface(surf, theStream, true);
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_Surface::Restore(std::istream& theStream)
{
  const DrawTrSurf_Params&   aParams      = DrawTrSurf::Parameters();
  occ::handle<Geom_Surface>       aGeomSurface = GeomTools_SurfaceSet::ReadSurface(theStream);
  occ::handle<DrawTrSurf_Surface> aDrawSurface = new DrawTrSurf_Surface(aGeomSurface,
                                                                   aParams.NbUIsos,
                                                                   aParams.NbVIsos,
                                                                   aParams.BoundsColor,
                                                                   aParams.IsosColor,
                                                                   aParams.Discret,
                                                                   aParams.Deflection,
                                                                   aParams.DrawMode);
  return aDrawSurface;
}

//=================================================================================================

void DrawTrSurf_Surface::Whatis(Draw_Interpretor& S) const
{
  S << "a surface";
}
