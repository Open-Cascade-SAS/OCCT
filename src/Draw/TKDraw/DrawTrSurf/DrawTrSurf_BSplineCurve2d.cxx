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

#include <DrawTrSurf_BSplineCurve2d.hxx>

#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_MarkerShape.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Params.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BSplineCurve2d, DrawTrSurf_Curve2d)

DrawTrSurf_BSplineCurve2d::DrawTrSurf_BSplineCurve2d(const occ::handle<Geom2d_BSplineCurve>& C)
    : DrawTrSurf_Curve2d(C, Draw_vert, 100)
{
  drawKnots = true;
  knotsForm = Draw_Losange;
  knotsLook = Draw_violet;
  knotsDim  = 5;
  drawPoles = true;
  polesLook = Draw_rouge;
}

DrawTrSurf_BSplineCurve2d::DrawTrSurf_BSplineCurve2d(const occ::handle<Geom2d_BSplineCurve>& C,
                                                     const Draw_Color&      CurvColor,
                                                     const Draw_Color&      PolesColor,
                                                     const Draw_Color&      KnotsColor,
                                                     const Draw_MarkerShape KnotsShape,
                                                     const int              KnotsSize,
                                                     const bool             ShowPoles,
                                                     const bool             ShowKnots,
                                                     const int              Discret)
    : DrawTrSurf_Curve2d(C, CurvColor, Discret)
{
  drawKnots = ShowKnots;
  knotsForm = KnotsShape;
  knotsLook = KnotsColor;
  knotsDim  = KnotsSize;
  drawPoles = ShowPoles;
  polesLook = PolesColor;
}

void DrawTrSurf_BSplineCurve2d::DrawOn(Draw_Display& dis) const
{
  occ::handle<Geom2d_BSplineCurve> C = occ::down_cast<Geom2d_BSplineCurve>(curv);

  if (drawPoles)
  {
    dis.SetColor(polesLook);
    const NCollection_Array1<gp_Pnt2d>& CPoles = C->Poles();
    dis.MoveTo(CPoles(1));
    for (int i = 2; i <= CPoles.Length(); i++)
    {
      dis.DrawTo(CPoles(i));
    }
    if (C->IsPeriodic())
      dis.DrawTo(CPoles(1));
  }

  DrawTrSurf_Curve2d::DrawOn(dis);

  if (drawKnots)
  {
    const NCollection_Array1<double>& CKnots = C->Knots();
    dis.SetColor(knotsLook);
    for (int i = CKnots.Lower(); i <= CKnots.Upper(); i++)
    {
      gp_Pnt2d P = C->Value(CKnots(i));
      dis.DrawMarker(P, knotsForm, knotsDim);
    }
  }
}

void DrawTrSurf_BSplineCurve2d::FindPole(const double        X,
                                         const double        Y,
                                         const Draw_Display& D,
                                         const double        XPrec,
                                         int&                Index) const
{
  occ::handle<Geom2d_BSplineCurve> bc   = occ::down_cast<Geom2d_BSplineCurve>(curv);
  double                           Prec = XPrec / D.Zoom();
  gp_Pnt2d                         p1(X / D.Zoom(), Y / D.Zoom());
  Index++;
  int      NbPoles = bc->NbPoles();
  gp_Pnt   P;
  gp_Pnt2d P2d;
  while (Index <= NbPoles)
  {
    P2d = bc->Pole(Index);
    P.SetCoord(P2d.X(), P2d.Y(), 0.0);
    if (D.Project(P).Distance(p1) <= Prec)
    {
      return;
    }
    Index++;
  }
  Index = 0;
}

void DrawTrSurf_BSplineCurve2d::FindKnot(const double        X,
                                         const double        Y,
                                         const Draw_Display& D,
                                         const double        Prec,
                                         int&                Index) const
{
  occ::handle<Geom2d_BSplineCurve> bc = occ::down_cast<Geom2d_BSplineCurve>(curv);
  gp_Pnt2d                         P2d;
  gp_Pnt                           P;
  gp_Pnt2d                         p1(X, Y);
  Index++;
  int NbKnots = bc->NbKnots();
  while (Index <= NbKnots)
  {
    P2d = bc->Value(bc->Knot(Index));
    P.SetCoord(P2d.X(), P2d.Y(), 0.0);
    if (D.Project(P).Distance(p1) <= Prec)
    {
      return;
    }
    Index++;
  }
  Index = 0;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BSplineCurve2d::Copy() const
{
  occ::handle<DrawTrSurf_BSplineCurve2d> DC =
    new DrawTrSurf_BSplineCurve2d(occ::down_cast<Geom2d_BSplineCurve>(curv->Copy()),
                                  look,
                                  polesLook,
                                  knotsLook,
                                  knotsForm,
                                  knotsDim,
                                  drawPoles,
                                  drawKnots,
                                  GetDiscretisation());

  return DC;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BSplineCurve2d::Restore(Standard_IStream& theStream)
{
  const DrawTrSurf_Params&         aParams = DrawTrSurf::Parameters();
  occ::handle<Geom2d_BSplineCurve> aGeomCurve =
    occ::down_cast<Geom2d_BSplineCurve>(GeomTools_Curve2dSet::ReadCurve2d(theStream));
  occ::handle<DrawTrSurf_BSplineCurve2d> aDrawCurve =
    new DrawTrSurf_BSplineCurve2d(aGeomCurve,
                                  aParams.CurvColor,
                                  aParams.PolesColor,
                                  aParams.KnotsColor,
                                  aParams.KnotsMarker,
                                  aParams.KnotsSize,
                                  aParams.IsShowPoles,
                                  aParams.IsShowKnots,
                                  aParams.Discret);
  return aDrawCurve;
}
