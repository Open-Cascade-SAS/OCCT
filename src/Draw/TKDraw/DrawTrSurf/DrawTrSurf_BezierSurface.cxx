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

#include <DrawTrSurf_BezierSurface.hxx>

#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_BezierCurve.hxx>
#include <DrawTrSurf_Params.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BezierSurface, DrawTrSurf_Surface)

DrawTrSurf_BezierSurface::DrawTrSurf_BezierSurface(const occ::handle<Geom_BezierSurface>& S)
    : DrawTrSurf_Surface(S, 1, 1, Draw_jaune, Draw_bleu, 30, 0.05, 0)
{
  drawPoles = true;
  polesLook = Draw_rouge;
}

DrawTrSurf_BezierSurface::DrawTrSurf_BezierSurface(const occ::handle<Geom_BezierSurface>& S,
                                                   const int                              NbUIsos,
                                                   const int                              NbVIsos,
                                                   const Draw_Color& BoundsColor,
                                                   const Draw_Color& IsosColor,
                                                   const Draw_Color& PolesColor,
                                                   const bool        ShowPoles,
                                                   const int         Discret,
                                                   const double      Deflection,
                                                   const int         DrawMode)
    : DrawTrSurf_Surface(S, NbUIsos, NbVIsos, BoundsColor, IsosColor, Discret, Deflection, DrawMode)
{
  drawPoles = ShowPoles;
  polesLook = PolesColor;
}

void DrawTrSurf_BezierSurface::DrawOn(Draw_Display& dis) const
{
  int                             i, j;
  occ::handle<Geom_BezierSurface> S = occ::down_cast<Geom_BezierSurface>(surf);
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
    }
    for (i = 1; i <= NbUPoles; i++)
    {
      dis.MoveTo(SPoles(i, 1));
      for (j = 2; j <= NbVPoles; j++)
      {
        dis.DrawTo(SPoles(i, j));
      }
    }
  }
  DrawTrSurf_Surface::DrawOn(dis);
}

void DrawTrSurf_BezierSurface::FindPole(const double        X,
                                        const double        Y,
                                        const Draw_Display& D,
                                        const double        XPrec,
                                        int&                UIndex,
                                        int&                VIndex) const
{
  occ::handle<Geom_BezierSurface> bs = occ::down_cast<Geom_BezierSurface>(surf);
  gp_Pnt2d                        p1(X / D.Zoom(), Y / D.Zoom());
  double                          Prec = XPrec / D.Zoom();
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

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BezierSurface::Copy() const
{
  occ::handle<DrawTrSurf_BezierSurface> DS =
    new DrawTrSurf_BezierSurface(occ::down_cast<Geom_BezierSurface>(surf->Copy()),
                                 nbUIsos,
                                 nbVIsos,
                                 boundsLook,
                                 isosLook,
                                 polesLook,
                                 drawPoles,
                                 GetDiscretisation(),
                                 GetDeflection(),
                                 GetDrawMode());

  return DS;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BezierSurface::Restore(Standard_IStream& theStream)
{
  const DrawTrSurf_Params&        aParams = DrawTrSurf::Parameters();
  occ::handle<Geom_BezierSurface> aGeomSurface =
    occ::down_cast<Geom_BezierSurface>(GeomTools_SurfaceSet::ReadSurface(theStream));
  occ::handle<DrawTrSurf_BezierSurface> aDrawSurface =
    new DrawTrSurf_BezierSurface(aGeomSurface,
                                 aParams.NbUIsos,
                                 aParams.NbVIsos,
                                 aParams.BoundsColor,
                                 aParams.IsosColor,
                                 aParams.PolesColor,
                                 aParams.IsShowPoles,
                                 aParams.Discret,
                                 aParams.Deflection,
                                 aParams.DrawMode);
  return aDrawSurface;
}
