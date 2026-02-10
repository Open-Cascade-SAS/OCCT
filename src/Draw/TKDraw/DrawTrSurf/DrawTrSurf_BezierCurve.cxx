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

#include <DrawTrSurf_BezierCurve.hxx>

#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Params.hxx>
#include <Geom_BezierCurve.hxx>
#include <GeomTools_CurveSet.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BezierCurve, DrawTrSurf_Curve)

DrawTrSurf_BezierCurve::DrawTrSurf_BezierCurve(const occ::handle<Geom_BezierCurve>& C)
    : DrawTrSurf_Curve(C, Draw_vert, 16, 0.05, 1)
{
  drawPoles = true;
  polesLook = Draw_rouge;
}

DrawTrSurf_BezierCurve::DrawTrSurf_BezierCurve(const occ::handle<Geom_BezierCurve>& C,
                                               const Draw_Color&                    CurvColor,
                                               const Draw_Color&                    PolesColor,
                                               const bool                           ShowPoles,
                                               const int                            Discret,
                                               const double                         Deflection,
                                               const int                            DrawMode)
    : DrawTrSurf_Curve(C, CurvColor, Discret, Deflection, DrawMode)
{
  drawPoles = ShowPoles;
  polesLook = PolesColor;
}

void DrawTrSurf_BezierCurve::DrawOn(Draw_Display& dis) const
{
  occ::handle<Geom_BezierCurve> C = occ::down_cast<Geom_BezierCurve>(curv);
  if (drawPoles)
  {
    dis.SetColor(polesLook);
    const NCollection_Array1<gp_Pnt>& CPoles  = C->Poles();
    int                               NbPoles = CPoles.Length();
    dis.MoveTo(CPoles(1));
    for (int i = 2; i <= NbPoles; i++)
    {
      dis.DrawTo(CPoles(i));
    }
  }

  DrawTrSurf_Curve::DrawOn(dis);
}

void DrawTrSurf_BezierCurve::FindPole(const double        X,
                                      const double        Y,
                                      const Draw_Display& D,
                                      const double        XPrec,
                                      int&                Index) const
{
  occ::handle<Geom_BezierCurve> bz = occ::down_cast<Geom_BezierCurve>(curv);
  gp_Pnt2d                      p1(X / D.Zoom(), Y / D.Zoom());
  double                        Prec = XPrec / D.Zoom();
  Index++;
  int NbPoles = bz->NbPoles();
  while (Index <= NbPoles)
  {
    if (D.Project(bz->Pole(Index)).Distance(p1) <= Prec)
    {
      return;
    }
    Index++;
  }
  Index = 0;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BezierCurve::Copy() const
{
  occ::handle<DrawTrSurf_BezierCurve> DC =
    new DrawTrSurf_BezierCurve(occ::down_cast<Geom_BezierCurve>(curv->Copy()),
                               look,
                               polesLook,
                               drawPoles,
                               GetDiscretisation(),
                               GetDeflection(),
                               GetDrawMode());
  return DC;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BezierCurve::Restore(Standard_IStream& theStream)
{
  const DrawTrSurf_Params&      aParams = DrawTrSurf::Parameters();
  occ::handle<Geom_BezierCurve> aGeomCurve =
    occ::down_cast<Geom_BezierCurve>(GeomTools_CurveSet::ReadCurve(theStream));
  occ::handle<DrawTrSurf_BezierCurve> aDrawCurve = new DrawTrSurf_BezierCurve(aGeomCurve,
                                                                              aParams.CurvColor,
                                                                              aParams.PolesColor,
                                                                              aParams.IsShowPoles,
                                                                              aParams.Discret,
                                                                              aParams.Deflection,
                                                                              aParams.DrawMode);
  return aDrawCurve;
}
