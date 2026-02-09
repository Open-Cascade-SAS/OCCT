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

#include <DrawTrSurf_BSplineCurve.hxx>

#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <Draw_MarkerShape.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Params.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <GeomTools_CurveSet.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DrawTrSurf_BSplineCurve, DrawTrSurf_Curve)

DrawTrSurf_BSplineCurve::DrawTrSurf_BSplineCurve(const occ::handle<Geom_BSplineCurve>& C)
    : DrawTrSurf_Curve(C, Draw_vert, 16, 0.05, 1)
{

  drawKnots = true;
  knotsForm = Draw_Losange;
  knotsLook = Draw_violet;
  knotsDim  = 5;
  drawPoles = true;
  polesLook = Draw_rouge;
}

DrawTrSurf_BSplineCurve::DrawTrSurf_BSplineCurve(const occ::handle<Geom_BSplineCurve>& C,
                                                 const Draw_Color&                     CurvColor,
                                                 const Draw_Color&                     PolesColor,
                                                 const Draw_Color&                     KnotsColor,
                                                 const Draw_MarkerShape                KnotsShape,
                                                 const int                             KnotsSize,
                                                 const bool                            ShowPoles,
                                                 const bool                            ShowKnots,
                                                 const int                             Discret,
                                                 const double                          Deflection,
                                                 const int                             DrawMode)
    : DrawTrSurf_Curve(C, CurvColor, Discret, Deflection, DrawMode)
{
  drawKnots = ShowKnots;
  knotsForm = KnotsShape;
  knotsLook = KnotsColor;
  knotsDim  = KnotsSize;
  drawPoles = ShowPoles;
  polesLook = PolesColor;
}

void DrawTrSurf_BSplineCurve::DrawOn(Draw_Display& dis) const
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(curv);
  if (drawPoles)
  {
    dis.SetColor(polesLook);
    const NCollection_Array1<gp_Pnt>& CPoles = C->Poles();
    int NbPoles = CPoles.Length();
    dis.MoveTo(CPoles(1));
    for (int i = 2; i <= NbPoles; i++)
    {
      dis.DrawTo(CPoles(i));
    }
    if (C->IsPeriodic())
    {
      dis.DrawTo(CPoles(1));
    }
  }

  DrawTrSurf_Curve::DrawOn(dis);
  if (drawKnots)
  {
    const NCollection_Array1<double>& CKnots = C->Knots();
    dis.SetColor(knotsLook);
    int first = C->FirstUKnotIndex();
    int last  = C->LastUKnotIndex();
    for (int i = first; i <= last; i++)
    {
      dis.DrawMarker(C->Value(CKnots(i)), knotsForm, knotsDim);
    }
  }
}

void DrawTrSurf_BSplineCurve::DrawOn(Draw_Display& dis,
                                     const bool    ShowPoles,
                                     const bool    ShowKnots) const
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(curv);
  if (drawPoles && ShowPoles)
  {
    dis.SetColor(polesLook);
    const NCollection_Array1<gp_Pnt>& CPoles = C->Poles();
    int NbPoles = CPoles.Length();
    dis.MoveTo(CPoles(1));
    for (int i = 2; i <= NbPoles; i++)
    {
      dis.DrawTo(CPoles(i));
    }
  }

  DrawTrSurf_Curve::DrawOn(dis);

  if (drawKnots && ShowKnots)
  {
    const NCollection_Array1<double>& CKnots = C->Knots();
    dis.SetColor(knotsLook);
    for (int i = CKnots.Lower(); i <= CKnots.Upper(); i++)
    {
      dis.DrawMarker(C->Value(CKnots(i)), knotsForm, knotsDim);
    }
  }
}

void DrawTrSurf_BSplineCurve::DrawOn(Draw_Display& dis,
                                     const double  U1,
                                     const double  U2,
                                     const int     Pindex,
                                     const bool    ShowPoles,
                                     const bool    ShowKnots) const
{
  occ::handle<Geom_BSplineCurve> C    = occ::down_cast<Geom_BSplineCurve>(curv);
  double                         Eps1 = std::abs(Epsilon(U1));
  double                         Eps2 = std::abs(Epsilon(U2));
  int                            I1, J1, I2, J2;
  C->LocateU(U1, Eps1, I1, J1);
  C->LocateU(U2, Eps2, I2, J2);
  int ka = C->FirstUKnotIndex();
  int kb = C->LastUKnotIndex();

  if (drawPoles && ShowPoles)
  {
    dis.SetColor(polesLook);
    const NCollection_Array1<gp_Pnt>& CPoles = C->Poles();
    int NbPoles = CPoles.Length();
    if (Pindex == 0)
    {
      dis.MoveTo(CPoles(1));
      for (int i = 2; i <= NbPoles; i++)
      {
        dis.DrawTo(CPoles(i));
      }
    }
    else if (Pindex == 1)
    {
      dis.MoveTo(CPoles(1));
      dis.DrawTo(CPoles(2));
    }
    else if (Pindex == NbPoles)
    {
      dis.MoveTo(CPoles(NbPoles - 1));
      dis.DrawTo(CPoles(NbPoles));
    }
    else
    {
      dis.MoveTo(CPoles(Pindex - 1));
      dis.DrawTo(CPoles(Pindex));
      dis.DrawTo(CPoles(Pindex + 1));
    }
  }

  dis.SetColor(look);
  int Degree = C->Degree();
  if (Degree == 1)
  {
    dis.MoveTo(C->Value(U1));
    dis.DrawTo(C->Value(U2));
  }
  else
  {
    int    NbPoints;
    int    Discret = GetDiscretisation();
    double Ustart  = C->Knot(ka);
    double Uend    = C->Knot(kb);
    double Du, U, Ua, Ub, Uk1, Uk2;

    if (I1 > ka)
    {
      ka  = I1;
      Uk1 = U1;
    }
    else
    {
      U        = U1;
      NbPoints = (int)std::abs(Discret * (U1 - Ustart) / (Ustart - Uend));
      NbPoints = std::max(NbPoints, 30);
      Du       = (Ustart - U1) / NbPoints;
      dis.MoveTo(C->Value(U));
      for (int i = 1; i <= NbPoints - 2; i++)
      {
        U += Du;
        dis.DrawTo(C->Value(U));
      }
      dis.DrawTo(C->Value(Ustart));
      Uk1 = Ustart;
    }

    if (J2 < kb)
    {
      kb  = J2;
      Uk2 = U2;
    }
    else
    {
      Uk2      = Uend;
      U        = Uend;
      NbPoints = (int)std::abs(Discret * (U2 - Uend) / (Ustart - Uend));
      NbPoints = std::max(NbPoints, 30);
      Du       = (U2 - Uend) / NbPoints;
      dis.MoveTo(C->Value(U));
      for (int i = 1; i <= NbPoints - 2; i++)
      {
        U += Du;
        dis.DrawTo(C->Value(U));
      }
      dis.DrawTo(C->Value(U2));
    }

    for (int k = ka; k < kb; k++)
    {
      if (k == ka)
      {
        Ua = Uk1;
        Ub = C->Knot(k + 1);
      }
      else if (k == kb - 1)
      {
        Ua = C->Knot(k);
        Ub = Uk2;
      }
      else
      {
        Ua = C->Knot(k);
        Ub = C->Knot(k + 1);
      }
      U        = Ua;
      NbPoints = (int)std::abs(Discret * (Ua - Ub) / (Ustart - Uend));
      NbPoints = std::max(NbPoints, 30);
      Du       = (Ub - Ua) / NbPoints;
      dis.MoveTo(C->Value(U));
      for (int i = 1; i <= NbPoints - 2; i++)
      {
        U += Du;
        dis.DrawTo(C->Value(U));
      }
      dis.DrawTo(C->Value(Ub));
    }
  }

  if (drawKnots && ShowKnots)
  {
    const NCollection_Array1<double>& CKnots = C->Knots();
    dis.SetColor(knotsLook);
    for (int i = J1; i <= I2; i++)
    {
      dis.DrawMarker(C->Value(CKnots(i)), knotsForm, knotsDim);
    }
  }
}

void DrawTrSurf_BSplineCurve::FindPole(const double        X,
                                       const double        Y,
                                       const Draw_Display& D,
                                       const double        XPrec,
                                       int&                Index) const
{
  occ::handle<Geom_BSplineCurve> bc   = occ::down_cast<Geom_BSplineCurve>(curv);
  double                         Prec = XPrec / D.Zoom();
  gp_Pnt2d                       p1(X / D.Zoom(), Y / D.Zoom());
  Index++;
  int NbPoles = bc->NbPoles();
  while (Index <= NbPoles)
  {
    if (D.Project(bc->Pole(Index)).Distance(p1) <= Prec)
    {
      return;
    }
    Index++;
  }
  Index = 0;
}

void DrawTrSurf_BSplineCurve::FindKnot(const double        X,
                                       const double        Y,
                                       const Draw_Display& D,
                                       const double        Prec,
                                       int&                Index) const
{
  occ::handle<Geom_BSplineCurve> bc = occ::down_cast<Geom_BSplineCurve>(curv);
  gp_Pnt2d                       p1(X, Y);
  Index++;
  int NbKnots = bc->NbKnots();
  while (Index <= NbKnots)
  {
    if (D.Project(bc->Value(bc->Knot(Index))).Distance(p1) <= Prec)
    {
      return;
    }
    Index++;
  }
  Index = 0;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BSplineCurve::Copy() const
{
  occ::handle<DrawTrSurf_BSplineCurve> DC =
    new DrawTrSurf_BSplineCurve(occ::down_cast<Geom_BSplineCurve>(curv->Copy()),
                                look,
                                polesLook,
                                knotsLook,
                                knotsForm,
                                knotsDim,
                                drawPoles,
                                drawKnots,
                                GetDiscretisation(),
                                GetDeflection(),
                                GetDrawMode());

  return DC;
}

//=================================================================================================

occ::handle<Draw_Drawable3D> DrawTrSurf_BSplineCurve::Restore(std::istream& theStream)
{
  const DrawTrSurf_Params&       aParams = DrawTrSurf::Parameters();
  occ::handle<Geom_BSplineCurve> aGeomCurve =
    occ::down_cast<Geom_BSplineCurve>(GeomTools_CurveSet::ReadCurve(theStream));
  occ::handle<DrawTrSurf_BSplineCurve> aDrawCurve = new DrawTrSurf_BSplineCurve(aGeomCurve,
                                                                                aParams.CurvColor,
                                                                                aParams.PolesColor,
                                                                                aParams.KnotsColor,
                                                                                aParams.KnotsMarker,
                                                                                aParams.KnotsSize,
                                                                                aParams.IsShowPoles,
                                                                                aParams.IsShowKnots,
                                                                                aParams.Discret,
                                                                                aParams.Deflection,
                                                                                aParams.DrawMode);
  return aDrawCurve;
}
