// Created on: 1993-08-12
// Created by: Bruno DUMORTIER
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

#include <DrawTrSurf.hxx>

#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf_BezierCurve.hxx>
#include <DrawTrSurf_BezierCurve2d.hxx>
#include <DrawTrSurf_BezierSurface.hxx>
#include <DrawTrSurf_BSplineCurve.hxx>
#include <DrawTrSurf_BSplineCurve2d.hxx>
#include <DrawTrSurf_BSplineSurface.hxx>
#include <DrawTrSurf_Curve.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <DrawTrSurf_Drawable.hxx>
#include <DrawTrSurf_Params.hxx>
#include <DrawTrSurf_Point.hxx>
#include <DrawTrSurf_Polygon2D.hxx>
#include <DrawTrSurf_Polygon3D.hxx>
#include <DrawTrSurf_Surface.hxx>
#include <DrawTrSurf_Triangulation.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Stream.hxx>
#include <TCollection_AsciiString.hxx>

static TCollection_AsciiString ColorsHint("The possible colors are: \n\
  white, red, green, blue, cyan,\n\
  golden, magenta, brown, orange, pink,\n\
  salmon, violet, yellow, darkgreen, coral");

static TCollection_AsciiString MarkersHint("The possible markers are: \n\
  square, diamond, x, plus, circle, circle_zoom");

//=======================================================================
// function : DrawTrSurf_CurveColor
// purpose  : Sets new color for rendering of curves. Returns the
//           previous one to keep possibility to restore the initial
//           state
//=======================================================================
Standard_EXPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color theColor)
{
  DrawTrSurf_Params& aParams    = DrawTrSurf::Parameters();
  Draw_Color         aLastColor = aParams.CurvColor;
  aParams.CurvColor             = theColor;
  return aLastColor;
}

//=======================================================================
// function : DrawTrSurf_PointColor
// purpose  : Sets new color for rendering of points. Returns the
//           previous one to keep possibility to restore the initial
//           state
//=======================================================================
Standard_EXPORT Draw_Color DrawTrSurf_PointColor(const Draw_Color theColor)
{
  DrawTrSurf_Params& aParams    = DrawTrSurf::Parameters();
  Draw_Color         aLastColor = aParams.PntColor;
  aParams.PntColor              = theColor;
  return aLastColor;
}

//=======================================================================
// function : DrawTrSurf_PointMarker
// purpose  : Sets new marker for rendering of points. Returns the
//           previous one to keep possibility to restore the initial
//           state
//=======================================================================
Standard_EXPORT Draw_MarkerShape DrawTrSurf_PointMarker(const Draw_MarkerShape theMarker)
{
  DrawTrSurf_Params& aParams     = DrawTrSurf::Parameters();
  Draw_MarkerShape   aLastMarker = aParams.PntMarker;
  aParams.PntMarker              = theMarker;
  return aLastMarker;
}

//=================================================================================================

DrawTrSurf_Params& DrawTrSurf::Parameters()
{
  static DrawTrSurf_Params aParams;
  return aParams;
}

//=================================================================================================

static Handle(DrawTrSurf_Drawable) GetDrawable(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)     D = Draw::Get(Name);
  Handle(DrawTrSurf_Drawable) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_Drawable)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_Surface) GetSurface(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)    D = Draw::Get(Name);
  Handle(DrawTrSurf_Surface) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_Surface)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_BezierSurface) GetBezierSurface(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)          D = Draw::Get(Name);
  Handle(DrawTrSurf_BezierSurface) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_BezierSurface)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_BSplineSurface) GetBSplineSurface(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)           D = Draw::Get(Name);
  Handle(DrawTrSurf_BSplineSurface) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_BSplineSurface)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_BezierCurve) GetBezierCurve(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)        D = Draw::Get(Name);
  Handle(DrawTrSurf_BezierCurve) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_BezierCurve)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_BSplineCurve) GetBSplineCurve(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)         D = Draw::Get(Name);
  Handle(DrawTrSurf_BSplineCurve) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_BSplineCurve)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_BezierCurve2d) GetBezierCurve2d(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)          D = Draw::Get(Name);
  Handle(DrawTrSurf_BezierCurve2d) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_BezierCurve2d)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Handle(DrawTrSurf_BSplineCurve2d) GetBSplineCurve2d(Standard_CString& Name)
{
  Handle(Draw_Drawable3D)           D = Draw::Get(Name);
  Handle(DrawTrSurf_BSplineCurve2d) Dr;
  if (!D.IsNull())
    Dr = Handle(DrawTrSurf_BSplineCurve2d)::DownCast(D);
  return Dr;
}

//=================================================================================================

static Standard_Integer nbiso(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  DrawTrSurf_Params& aParams = DrawTrSurf::Parameters();
  if (n < 4)
  {
    if (n == 3)
    {
      aParams.NbUIsos = Draw::Atoi(a[1]);
      aParams.NbVIsos = Draw::Atoi(a[2]);
    }
    di << aParams.NbUIsos << " " << aParams.NbVIsos;
  }
  else
  {
    for (Standard_Integer i = 1; i < n - 2; i++)
    {
      Handle(DrawTrSurf_Surface) DS = GetSurface(a[i]);
      if (!DS.IsNull())
      {
        DS->ShowIsos(Draw::Atoi(a[n - 2]), Draw::Atoi(a[n - 1]));
        Draw::Repaint();
      }
    }
  }
  return 0;
}

//=================================================================================================

static Standard_Integer drawpoles(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  DrawTrSurf_Params& aParams = DrawTrSurf::Parameters();
  if (n == 1)
  {
    if (!strcmp(a[0], "shpoles"))
    {
      aParams.IsShowPoles = Standard_True;
    }
    else if (!strcmp(a[0], "clpoles"))
    {
      aParams.IsShowPoles = Standard_False;
    }
    else if (!strcmp(a[0], "shknots"))
    {
      aParams.IsShowKnots = Standard_True;
    }
    else if (!strcmp(a[0], "clknots"))
    {
      aParams.IsShowKnots = Standard_False;
    }
  }

  if (n < 2)
  {
    return 0;
  }

  Handle(DrawTrSurf_BezierSurface) BZS;
  BZS = GetBezierSurface(a[1]);
  if (!BZS.IsNull())
  {
    if (!strcmp(a[0], "shpoles"))
    {
      BZS->ShowPoles();
    }
    else if (!strcmp(a[0], "clpoles"))
    {
      BZS->ClearPoles();
    }
    else
    {
      return 1;
    }
  }
  else
  {
    Handle(DrawTrSurf_BSplineSurface) BSS = GetBSplineSurface(a[1]);
    if (!BSS.IsNull())
    {
      if (!strcmp(a[0], "shpoles"))
      {
        BSS->ShowPoles();
      }
      else if (!strcmp(a[0], "clpoles"))
      {
        BSS->ClearPoles();
      }
      else if (!strcmp(a[0], "shknots"))
      {
        BSS->ShowKnots();
      }
      else if (!strcmp(a[0], "clknots"))
      {
        BSS->ClearKnots();
      }
    }
    else
    {
      Handle(DrawTrSurf_BezierCurve) BZC = GetBezierCurve(a[1]);
      if (!BZC.IsNull())
      {
        if (!strcmp(a[0], "shpoles"))
        {
          BZC->ShowPoles();
        }
        else if (!strcmp(a[0], "clpoles"))
        {
          BZC->ClearPoles();
        }
        else
        {
          return 1;
        }
      }
      else
      {
        Handle(DrawTrSurf_BSplineCurve) BSC = GetBSplineCurve(a[1]);
        if (!BSC.IsNull())
        {
          if (!strcmp(a[0], "shpoles"))
          {
            BSC->ShowPoles();
          }
          else if (!strcmp(a[0], "clpoles"))
          {
            BSC->ClearPoles();
          }
          else if (!strcmp(a[0], "shknots"))
          {
            BSC->ShowKnots();
          }
          else if (!strcmp(a[0], "clknots"))
          {
            BSC->ClearKnots();
          }
        }
        else
        {
          Handle(DrawTrSurf_BezierCurve2d) BZ2 = GetBezierCurve2d(a[1]);
          if (!BZ2.IsNull())
          {
            if (!strcmp(a[0], "shpoles"))
            {
              BZ2->ShowPoles();
            }
            else if (!strcmp(a[0], "clpoles"))
            {
              BZ2->ClearPoles();
            }
            else
            {
              return 1;
            }
          }
          else
          {
            Handle(DrawTrSurf_BSplineCurve2d) BS2 = GetBSplineCurve2d(a[1]);
            if (!BS2.IsNull())
            {
              if (!strcmp(a[0], "shpoles"))
              {
                BS2->ShowPoles();
              }
              else if (!strcmp(a[0], "clpoles"))
              {
                BS2->ClearPoles();
              }
              else if (!strcmp(a[0], "shknots"))
              {
                BS2->ShowKnots();
              }
              else if (!strcmp(a[0], "clknots"))
              {
                BS2->ClearKnots();
              }
            }
            else
            {
              return 1;
            }
          }
        }
      }
    }
  }
  Draw::Repaint();
  return 0;
}

//=================================================================================================

static Standard_Integer draw(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  DrawTrSurf_Params& aParams = DrawTrSurf::Parameters();
  if (n <= 2)
  {
    if (!strcmp(a[0], "dmode"))
    {
      if (n == 2)
      {
        aParams.DrawMode = 1;
      }
      di << (aParams.DrawMode ? "u" : "d");
    }
    else if (!strcmp(a[0], "discr"))
    {
      if (n == 2)
      {
        aParams.Discret = Draw::Atoi(a[n - 1]);
      }
      di << aParams.Discret;
    }
    else if (!strcmp(a[0], "defle"))
    {
      if (n == 2)
      {
        aParams.Deflection = Draw::Atof(a[n - 1]);
      }
      di << aParams.Deflection;
    }
  }
  else
  {
    for (Standard_Integer i = 1; i < n - 1; i++)
    {
      Handle(DrawTrSurf_Drawable) aDrawable = GetDrawable(a[1]);
      if (!aDrawable.IsNull())
      {
        if (!strcmp(a[0], "dmode"))
        {
          Standard_Integer mod = 0;
          if ((*a[n - 1] == 'U') || (*a[n - 1] == 'u'))
            mod = 1;
          aDrawable->SetDrawMode(mod);
        }
        else if (!strcmp(a[0], "discr"))
        {
          aDrawable->SetDiscretisation(Draw::Atoi(a[n - 1]));
        }
        else if (!strcmp(a[0], "defle"))
        {
          aDrawable->SetDeflection(Draw::Atof(a[n - 1]));
        }
        Draw::Repaint();
      }
    }
  }
  return 0;
}

//=================================================================================================

static Standard_Integer transform(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 5)
    return 1;
  gp_Trsf          T;
  Standard_Integer i, last = n - 1;
  if (!strcmp(a[0], "pscale"))
  {
    Standard_Real s = Draw::Atof(a[last]);
    last--;
    if (last < 4)
      return 1;
    gp_Pnt P(Draw::Atof(a[last - 2]), Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    T.SetScale(P, s);
  }
  else if (!strcmp(a[0] + 1, "mirror"))
  {
    if (last < 4)
      return 1;
    gp_Pnt P(Draw::Atof(a[last - 2]), Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    if (*a[0] == 'p')
    {
      T.SetMirror(P);
    }
    else
    {
      last -= 3;
      if (last < 4)
        return 1;
      gp_Pnt O(Draw::Atof(a[last - 2]), Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
      last -= 3;
      gp_Dir D(P.X(), P.Y(), P.Z());
      if (*a[0] == 'l')
      {
        T.SetMirror(gp_Ax1(O, D));
      }
      else if (*a[0] == 's')
      {
        gp_Pln Pl(O, D);
        T.SetMirror(Pl.Position().Ax2());
      }
    }
  }

  else if (!strcmp(a[0], "translate"))
  {
    if (last < 4)
      return 1;
    gp_Vec V(Draw::Atof(a[last - 2]), Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    last -= 3;
    T.SetTranslation(V);
  }

  else if (!strcmp(a[0], "rotate"))
  {
    if (last < 8)
      return 1;
    Standard_Real ang = Draw::Atof(a[last]) * (M_PI / 180.0);
    last--;
    gp_Dir D(Draw::Atof(a[last - 2]), Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    last -= 3;
    gp_Pnt P(Draw::Atof(a[last - 2]), Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    last -= 3;
    T.SetRotation(gp_Ax1(P, D), ang);
  }

  for (i = 1; i <= last; i++)
  {
    Handle(Geom_Geometry) G = DrawTrSurf::Get(a[i]);
    if (!G.IsNull())
    {
      G->Transform(T);
      Draw::Repaint();
    }
    else
    {
      gp_Pnt P;
      if (DrawTrSurf::GetPoint(a[i], P))
      {
        P.Transform(T);
        DrawTrSurf::Set(a[i], P);
      }
    }
    di << a[i] << " ";
  }
  return 0;
}

//=================================================================================================

static Standard_Integer d2transform(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 4)
    return 1;
  gp_Trsf2d        T;
  Standard_Integer i, last = n - 1;
  if (!strcmp(a[0], "2dpscale"))
  {
    Standard_Real s = Draw::Atof(a[last]);
    last--;
    if (last < 3)
      return 1;
    gp_Pnt2d P(Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    T.SetScale(P, s);
  }
  else if ((!strcmp(a[0], "2dpmirror")) || (!strcmp(a[0], "2dlmirror")))
  {
    if (last < 3)
      return 1;
    gp_Pnt2d P(Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    if (!strcmp(a[0], "2dpmirror"))
    {
      T.SetMirror(P);
    }
    else
    {
      last -= 2;
      if (last < 3)
        return 1;
      gp_Pnt2d O(Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
      last -= 2;
      gp_Dir2d D(P.X(), P.Y());
      T.SetMirror(gp_Ax2d(O, D));
    }
  }

  else if (!strcmp(a[0], "2dtranslate"))
  {
    if (last < 3)
      return 1;
    gp_Vec2d V(Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    last -= 2;
    T.SetTranslation(V);
  }

  else if (!strcmp(a[0], "2drotate"))
  {
    if (last < 4)
      return 1;
    Standard_Real ang = Draw::Atof(a[last]) * (M_PI / 180.0);
    last--;
    gp_Pnt2d P(Draw::Atof(a[last - 1]), Draw::Atof(a[last]));
    last -= 2;
    T.SetRotation(P, ang);
  }

  for (i = 1; i <= last; i++)
  {
    Handle(Geom2d_Curve) G = DrawTrSurf::GetCurve2d(a[i]);
    if (!G.IsNull())
    {
      G->Transform(T);
      Draw::Repaint();
    }
    else
    {
      gp_Pnt2d P;
      if (DrawTrSurf::GetPoint2d(a[i], P))
      {
        P.Transform(T);
        DrawTrSurf::Set(a[i], P);
      }
    }
    di << a[i] << " ";
  }
  return 0;
}

//=================================================================================================

void DrawTrSurf::Set(const Standard_CString theName, const gp_Pnt& thePoint)
{
  DrawTrSurf_Params&       aParams = DrawTrSurf::Parameters();
  Handle(DrawTrSurf_Point) aDrawPoint =
    new DrawTrSurf_Point(thePoint, aParams.PntMarker, aParams.PntColor);
  Draw::Set(theName, aDrawPoint);
}

//=================================================================================================

void DrawTrSurf::Set(const Standard_CString theName, const gp_Pnt2d& thePoint)
{
  DrawTrSurf_Params&       aParams = DrawTrSurf::Parameters();
  Handle(DrawTrSurf_Point) aDrawPoint =
    new DrawTrSurf_Point(thePoint, aParams.PntMarker, aParams.PntColor);
  Draw::Set(theName, aDrawPoint);
}

//=======================================================================
// function : Set
// purpose  : Geometry from Geom
//=======================================================================
void DrawTrSurf::Set(const Standard_CString       theName,
                     const Handle(Geom_Geometry)& theGeometry,
                     const Standard_Boolean       isSenseMarker)
{
  DrawTrSurf_Params&          aParams = DrawTrSurf::Parameters();
  Handle(DrawTrSurf_Drawable) aDrawable;
  if (Handle(Geom_BezierCurve) aGeomBezierCurve = Handle(Geom_BezierCurve)::DownCast(theGeometry))
  {
    aDrawable = new DrawTrSurf_BezierCurve(aGeomBezierCurve,
                                           aParams.CurvColor,
                                           aParams.PolesColor,
                                           aParams.IsShowPoles,
                                           aParams.Discret,
                                           aParams.Deflection,
                                           aParams.DrawMode);
  }
  else if (Handle(Geom_BSplineCurve) aGeomBSplineCurve =
             Handle(Geom_BSplineCurve)::DownCast(theGeometry))
  {
    aDrawable = new DrawTrSurf_BSplineCurve(aGeomBSplineCurve,
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
  }
  else if (Handle(Geom_Curve) aCurve = Handle(Geom_Curve)::DownCast(theGeometry))
  {
    aDrawable = new DrawTrSurf_Curve(aCurve,
                                     aParams.CurvColor,
                                     aParams.Discret,
                                     aParams.Deflection,
                                     aParams.DrawMode,
                                     isSenseMarker);
  }
  else if (Handle(Geom_BezierSurface) aBezSurf = Handle(Geom_BezierSurface)::DownCast(theGeometry))
  {
    aDrawable = new DrawTrSurf_BezierSurface(aBezSurf,
                                             aParams.NbUIsos,
                                             aParams.NbVIsos,
                                             aParams.BoundsColor,
                                             aParams.IsosColor,
                                             aParams.PolesColor,
                                             aParams.IsShowPoles,
                                             aParams.Discret,
                                             aParams.Deflection,
                                             aParams.DrawMode);
  }
  else if (Handle(Geom_BSplineSurface) aBSplineSurf =
             Handle(Geom_BSplineSurface)::DownCast(theGeometry))
  {
    if (!aParams.NeedKnotsIsos)
    {
      aDrawable = new DrawTrSurf_BSplineSurface(aBSplineSurf,
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
      aDrawable = new DrawTrSurf_BSplineSurface(aBSplineSurf,
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
  }
  else if (Handle(Geom_Surface) aSurface = Handle(Geom_Surface)::DownCast(theGeometry))
  {
    aDrawable = new DrawTrSurf_Surface(aSurface,
                                       aParams.NbUIsos,
                                       aParams.NbVIsos,
                                       aParams.BoundsColor,
                                       aParams.IsosColor,
                                       aParams.Discret,
                                       aParams.Deflection,
                                       aParams.DrawMode);
  }

  Draw::Set(theName, aDrawable);
}

//=======================================================================
// function : Set
// purpose  : Curve from Geom2d
//=======================================================================
void DrawTrSurf::Set(const Standard_CString      theName,
                     const Handle(Geom2d_Curve)& theCurve,
                     const Standard_Boolean      isSenseMarker)
{
  DrawTrSurf_Params&          aParams = DrawTrSurf::Parameters();
  Handle(DrawTrSurf_Drawable) aDrawable;
  if (Handle(Geom2d_BezierCurve) aBezierCurve = Handle(Geom2d_BezierCurve)::DownCast(theCurve))
  {
    aDrawable = new DrawTrSurf_BezierCurve2d(aBezierCurve,
                                             aParams.CurvColor,
                                             aParams.PolesColor,
                                             aParams.IsShowPoles,
                                             aParams.Discret);
  }
  else if (Handle(Geom2d_BSplineCurve) aBSplineCurve =
             Handle(Geom2d_BSplineCurve)::DownCast(theCurve))
  {
    aDrawable = new DrawTrSurf_BSplineCurve2d(aBSplineCurve,
                                              aParams.CurvColor,
                                              aParams.PolesColor,
                                              aParams.KnotsColor,
                                              aParams.KnotsMarker,
                                              aParams.KnotsSize,
                                              aParams.IsShowPoles,
                                              aParams.IsShowKnots,
                                              aParams.Discret);
  }
  else if (!theCurve.IsNull())
  {
    aDrawable = new DrawTrSurf_Curve2d(theCurve, aParams.CurvColor, aParams.Discret, isSenseMarker);
  }

  Draw::Set(theName, aDrawable);
}

//=================================================================================================

void DrawTrSurf::Set(const Standard_CString Name, const Handle(Poly_Triangulation)& T)
{
  Handle(DrawTrSurf_Triangulation) D = new DrawTrSurf_Triangulation(T);
  Draw::Set(Name, D);
}

//=================================================================================================

void DrawTrSurf::Set(const Standard_CString Name, const Handle(Poly_Polygon3D)& P)
{
  Handle(DrawTrSurf_Polygon3D) D = new DrawTrSurf_Polygon3D(P);
  Draw::Set(Name, D);
}

//=================================================================================================

void DrawTrSurf::Set(const Standard_CString Name, const Handle(Poly_Polygon2D)& P)
{
  Handle(DrawTrSurf_Polygon2D) D = new DrawTrSurf_Polygon2D(P);
  Draw::Set(Name, D);
}

//=================================================================================================

Handle(Geom_Geometry) DrawTrSurf::Get(Standard_CString& Name)
{
  Handle(Draw_Drawable3D) D = Draw::Get(Name);

  Handle(DrawTrSurf_Curve) DC = Handle(DrawTrSurf_Curve)::DownCast(D);
  if (!DC.IsNull())
    return DC->GetCurve();

  Handle(DrawTrSurf_Surface) DS = Handle(DrawTrSurf_Surface)::DownCast(D);
  if (!DS.IsNull())
    return DS->GetSurface();

  return Handle(Geom_Geometry)();
}

//=================================================================================================

Standard_Boolean DrawTrSurf::GetPoint(Standard_CString& Name, gp_Pnt& P)
{
  Handle(DrawTrSurf_Point) D = Handle(DrawTrSurf_Point)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Standard_False;
  else if (!D->Is3D())
    return Standard_False;
  else
  {
    P = D->Point();
    return Standard_True;
  }
}

//=================================================================================================

Standard_Boolean DrawTrSurf::GetPoint2d(Standard_CString& Name, gp_Pnt2d& P)
{
  Handle(DrawTrSurf_Point) D = Handle(DrawTrSurf_Point)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Standard_False;
  else if (D->Is3D())
    return Standard_False;
  else
  {
    P = D->Point2d();
    return Standard_True;
  }
}

//=================================================================================================

Handle(Geom_Curve) DrawTrSurf::GetCurve(Standard_CString& Name)
{
  Handle(DrawTrSurf_Curve) D = Handle(DrawTrSurf_Curve)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom_Curve)();
  else
    return D->GetCurve();
}

//=================================================================================================

Handle(Geom_BezierCurve) DrawTrSurf::GetBezierCurve(Standard_CString& Name)
{
  Handle(DrawTrSurf_BezierCurve) D = Handle(DrawTrSurf_BezierCurve)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom_BezierCurve)();
  else
    return Handle(Geom_BezierCurve)::DownCast(D->GetCurve());
}

//=================================================================================================

Handle(Geom_BSplineCurve) DrawTrSurf::GetBSplineCurve(Standard_CString& Name)
{
  Handle(DrawTrSurf_BSplineCurve) D = Handle(DrawTrSurf_BSplineCurve)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom_BSplineCurve)();
  else
    return Handle(Geom_BSplineCurve)::DownCast(D->GetCurve());
}

//=================================================================================================

Handle(Geom2d_Curve) DrawTrSurf::GetCurve2d(Standard_CString& Name)
{
  Handle(DrawTrSurf_Curve2d) D = Handle(DrawTrSurf_Curve2d)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom2d_Curve)();
  else
    return D->GetCurve();
}

//=================================================================================================

Handle(Geom2d_BezierCurve) DrawTrSurf::GetBezierCurve2d(Standard_CString& Name)
{
  Handle(DrawTrSurf_Curve2d) D = Handle(DrawTrSurf_Curve2d)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom2d_BezierCurve)();
  else
    return Handle(Geom2d_BezierCurve)::DownCast(D->GetCurve());
}

//=================================================================================================

Handle(Geom2d_BSplineCurve) DrawTrSurf::GetBSplineCurve2d(Standard_CString& Name)
{
  Handle(DrawTrSurf_Curve2d) D = Handle(DrawTrSurf_Curve2d)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom2d_BSplineCurve)();
  else
    return Handle(Geom2d_BSplineCurve)::DownCast(D->GetCurve());
}

//=================================================================================================

Handle(Geom_Surface) DrawTrSurf::GetSurface(Standard_CString& Name)
{
  Handle(DrawTrSurf_Surface) D = Handle(DrawTrSurf_Surface)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom_Surface)();
  else
    return D->GetSurface();
}

//=================================================================================================

Handle(Geom_BezierSurface) DrawTrSurf::GetBezierSurface(Standard_CString& Name)
{
  Handle(DrawTrSurf_BezierSurface) D = Handle(DrawTrSurf_BezierSurface)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom_BezierSurface)();
  else
    return Handle(Geom_BezierSurface)::DownCast(D->GetSurface());
}

//=================================================================================================

Handle(Geom_BSplineSurface) DrawTrSurf::GetBSplineSurface(Standard_CString& Name)
{
  Handle(DrawTrSurf_BSplineSurface) D =
    Handle(DrawTrSurf_BSplineSurface)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Geom_BSplineSurface)();
  else
    return Handle(Geom_BSplineSurface)::DownCast(D->GetSurface());
}

//=================================================================================================

Handle(Poly_Triangulation) DrawTrSurf::GetTriangulation(Standard_CString& Name)
{
  Handle(DrawTrSurf_Triangulation) D = Handle(DrawTrSurf_Triangulation)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Poly_Triangulation)();
  else
    return D->Triangulation();
}

//=================================================================================================

Handle(Poly_Polygon3D) DrawTrSurf::GetPolygon3D(Standard_CString& Name)
{
  Handle(DrawTrSurf_Polygon3D) D = Handle(DrawTrSurf_Polygon3D)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Poly_Polygon3D)();
  else
    return D->Polygon3D();
}

//=================================================================================================

Handle(Poly_Polygon2D) DrawTrSurf::GetPolygon2D(Standard_CString& Name)
{
  Handle(DrawTrSurf_Polygon2D) D = Handle(DrawTrSurf_Polygon2D)::DownCast(Draw::Get(Name));
  if (D.IsNull())
    return Handle(Poly_Polygon2D)();
  else
    return D->Polygon2D();
}

//=================================================================================================

static void printColor(Draw_Interpretor& di, const Draw_Color& theColor)
{
  switch (theColor.ID())
  {
    case Draw_blanc:
      di << "white " << "\n";
      break;
    case Draw_rouge:
      di << "red " << "\n";
      break;
    case Draw_vert:
      di << "green " << "\n";
      break;
    case Draw_bleu:
      di << "blue " << "\n";
      break;
    case Draw_cyan:
      di << "cyan " << "\n";
      break;
    case Draw_or:
      di << "golden " << "\n";
      break;
    case Draw_magenta:
      di << "magenta " << "\n";
      break;
    case Draw_marron:
      di << "brown " << "\n";
      break;
    case Draw_orange:
      di << "orange " << "\n";
      break;
    case Draw_rose:
      di << "pink " << "\n";
      break;
    case Draw_saumon:
      di << "salmon " << "\n";
      break;
    case Draw_violet:
      di << "violet " << "\n";
      break;
    case Draw_jaune:
      di << "yellow " << "\n";
      break;
    case Draw_kaki:
      di << "dark-olive green \n";
      break;
    case Draw_corail:
      di << "coral " << "\n";
      break;
  }
}

//=================================================================================================

static Draw_Color recognizeColor(const char* theColorStr, const Draw_Color& theDefaultColor)
{
  Draw_Color aResult = theDefaultColor;

  if (!strcasecmp(theColorStr, "white"))
    aResult = Draw_blanc;
  if (!strcasecmp(theColorStr, "red"))
    aResult = Draw_rouge;
  if (!strcasecmp(theColorStr, "green"))
    aResult = Draw_vert;
  if (!strcasecmp(theColorStr, "blue"))
    aResult = Draw_bleu;
  if (!strcasecmp(theColorStr, "cyan"))
    aResult = Draw_cyan;
  if (!strcasecmp(theColorStr, "golden"))
    aResult = Draw_or;
  if (!strcasecmp(theColorStr, "magenta"))
    aResult = Draw_magenta;
  if (!strcasecmp(theColorStr, "brown"))
    aResult = Draw_marron;
  if (!strcasecmp(theColorStr, "orange"))
    aResult = Draw_orange;
  if (!strcasecmp(theColorStr, "pink"))
    aResult = Draw_rose;
  if (!strcasecmp(theColorStr, "salmon"))
    aResult = Draw_saumon;
  if (!strcasecmp(theColorStr, "violet"))
    aResult = Draw_violet;
  if (!strcasecmp(theColorStr, "yellow"))
    aResult = Draw_jaune;
  if (!strcasecmp(theColorStr, "darkgreen"))
    aResult = Draw_kaki;
  if (!strcasecmp(theColorStr, "coral"))
    aResult = Draw_corail;

  return aResult;
}

//=================================================================================================

static Standard_Integer setcurvcolor(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Draw_Color col, savecol;

  savecol = DrawTrSurf_CurveColor(Draw_Color(Draw_jaune));
  DrawTrSurf_CurveColor(savecol);

  if (n < 2)
  {
    printColor(di, savecol);
  }
  else
  {
    col = recognizeColor(a[1], savecol);
    DrawTrSurf_CurveColor(col);
  }
  return 0;
}

//=================================================================================================

static Standard_Integer changecurvcolor(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  Draw_Color col, savecol;

  savecol = DrawTrSurf_CurveColor(Draw_Color(Draw_jaune));
  DrawTrSurf_CurveColor(savecol);

  if (n < 3)
    return 1;

  col = recognizeColor(a[1], savecol);

  Handle(DrawTrSurf_Curve) D = Handle(DrawTrSurf_Curve)::DownCast(Draw::Get(a[2]));
  if (!D.IsNull())
  {
    D->SetColor(col);
    Draw::Repaint();
  }

  return 0;
}

//=================================================================================================

static Standard_Integer setpointcolor(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Draw_Color col, savecol;

  savecol = DrawTrSurf_PointColor(Draw_Color(Draw_jaune));
  DrawTrSurf_PointColor(savecol);

  if (n < 2)
  {
    printColor(di, savecol);
  }
  else
  {
    col = recognizeColor(a[1], savecol);
    DrawTrSurf_PointColor(col);
  }
  return 0;
}

//=================================================================================================

static Standard_Integer changepointcolor(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  Draw_Color col, savecol;

  savecol = DrawTrSurf_PointColor(Draw_Color(Draw_jaune));
  DrawTrSurf_PointColor(savecol);

  if (n < 3)
    return 1;

  col = recognizeColor(a[1], savecol);

  Handle(DrawTrSurf_Point) D = Handle(DrawTrSurf_Point)::DownCast(Draw::Get(a[2]));
  if (!D.IsNull())
  {
    D->Color(col);
    Draw::Repaint();
  }

  return 0;
}

//=================================================================================================

static void printMarker(Draw_Interpretor& di, const Draw_MarkerShape& theMarker)
{
  switch (theMarker)
  {
    case Draw_Square:
      di << "square " << "\n";
      break;
    case Draw_Losange:
      di << "diamond " << "\n";
      break;
    case Draw_X:
      di << "x " << "\n";
      break;
    case Draw_Plus:
      di << "plus " << "\n";
      break;
    case Draw_Circle:
      di << "circle " << "\n";
      break;
    case Draw_CircleZoom:
      di << "circle_zoom \n";
      break;
  }
}

//=================================================================================================

static Draw_MarkerShape recognizeMarker(const char*             theMarkerStr,
                                        const Draw_MarkerShape& theDefaultMarker)
{
  Draw_MarkerShape aResult = theDefaultMarker;

  if (!strcasecmp(theMarkerStr, "square"))
    aResult = Draw_Square;
  if (!strcasecmp(theMarkerStr, "diamond"))
    aResult = Draw_Losange;
  if (!strcasecmp(theMarkerStr, "x"))
    aResult = Draw_X;
  if (!strcasecmp(theMarkerStr, "plus"))
    aResult = Draw_Plus;
  if (!strcasecmp(theMarkerStr, "circle"))
    aResult = Draw_Circle;
  if (!strcasecmp(theMarkerStr, "circle_zoom"))
    aResult = Draw_CircleZoom;

  return aResult;
}

//=================================================================================================

static Standard_Integer setpointmarker(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  Draw_MarkerShape mark, savemark;

  savemark = DrawTrSurf_PointMarker(Draw_MarkerShape(Draw_Plus));
  DrawTrSurf_PointMarker(savemark);

  if (n < 2)
  {
    printMarker(di, savemark);
  }
  else
  {
    mark = recognizeMarker(a[1], savemark);
    DrawTrSurf_PointMarker(mark);
  }
  return 0;
}

//=================================================================================================

static Standard_Integer changepointmarker(Draw_Interpretor&, Standard_Integer n, const char** a)
{
  Draw_MarkerShape mark, savemark;

  savemark = DrawTrSurf_PointMarker(Draw_MarkerShape(Draw_Plus));
  DrawTrSurf_PointMarker(savemark);

  if (n < 3)
    return 1;

  mark = recognizeMarker(a[1], savemark);

  Handle(DrawTrSurf_Point) D = Handle(DrawTrSurf_Point)::DownCast(Draw::Get(a[2]));
  if (!D.IsNull())
  {
    D->Shape(mark);
    Draw::Repaint();
  }

  return 0;
}

//=================================================================================================

static Standard_Boolean done = Standard_False;

void DrawTrSurf::BasicCommands(Draw_Interpretor& theCommands)
{
  if (done)
    return;
  done = Standard_True;

  DrawTrSurf_BezierCurve::RegisterFactory();
  DrawTrSurf_BezierCurve2d::RegisterFactory();
  DrawTrSurf_BezierSurface::RegisterFactory();
  DrawTrSurf_BSplineCurve::RegisterFactory();
  DrawTrSurf_BSplineCurve2d::RegisterFactory();
  DrawTrSurf_BSplineSurface::RegisterFactory();
  DrawTrSurf_Curve::RegisterFactory();
  DrawTrSurf_Curve2d::RegisterFactory();
  DrawTrSurf_Point::RegisterFactory();
  DrawTrSurf_Polygon2D::RegisterFactory();
  DrawTrSurf_Polygon3D::RegisterFactory();
  DrawTrSurf_Surface::RegisterFactory();
  DrawTrSurf_Triangulation::RegisterFactory();

  const char* g;
  g = "geometric display commands";

  theCommands.Add("nbiso", "nbiso name [names...] nuiso nviso", __FILE__, nbiso, g);

  theCommands.Add("clpoles", "clpoles [name], no args : modal ", __FILE__, drawpoles, g);

  theCommands.Add("shpoles", "shpoles [name], no args : modal ", __FILE__, drawpoles, g);

  theCommands.Add("clknots", "clknots [name], no args : modal ", __FILE__, drawpoles, g);

  theCommands.Add("shknots", "shknots [name], no args : modal ", __FILE__, drawpoles, g);

  theCommands.Add("dmode", "dmode [names...] Uniform/Discret", __FILE__, draw, g);

  theCommands.Add("discr", "discr [names...] nbintervals", __FILE__, draw, g);

  theCommands.Add("defle", "defle [names...] defle", __FILE__, draw, g);

  theCommands.Add("setcurvcolor",
                  TCollection_AsciiString("setcurvcolor [color] : set curve color\
 by default, or print the current curve color if no argument (this does not modify\
 the color of the curve)\n\n")
                    .Cat(ColorsHint)
                    .ToCString(),
                  __FILE__,
                  setcurvcolor,
                  g);

  theCommands.Add("changecurvcolor",
                  TCollection_AsciiString("changecurvcolor color curve: change\
 color of the curve\n\n")
                    .Cat(ColorsHint)
                    .ToCString(),
                  __FILE__,
                  changecurvcolor,
                  g);

  theCommands.Add("setpointcolor",
                  TCollection_AsciiString("setpointcolor [color] : set point color\
 by default, or print the current point color if no argument (this does not modify\
 the color of the point)\n\n")
                    .Cat(ColorsHint)
                    .ToCString(),
                  __FILE__,
                  setpointcolor,
                  g);

  theCommands.Add("changepointcolor",
                  TCollection_AsciiString("changepointcolor color point: change\
 color of the point\n\n")
                    .Cat(ColorsHint)
                    .ToCString(),
                  __FILE__,
                  changepointcolor,
                  g);

  theCommands.Add("setpointmarker",
                  TCollection_AsciiString("setpointmarker [marker] : set point marker\
 by default, or print the current point marker if no argument (this does not modify\
 the marker of the point)\n\n")
                    .Cat(MarkersHint)
                    .ToCString(),
                  __FILE__,
                  setpointmarker,
                  g);

  theCommands.Add("changepointmarker",
                  TCollection_AsciiString("changepointmarker marker point: change\
 marker of the point\n\n")
                    .Cat(MarkersHint)
                    .ToCString(),
                  __FILE__,
                  changepointmarker,
                  g);

  g = "Geometric transformations";

  theCommands.Add("translate", "translate name [names...] dx dy dz", __FILE__, transform, g);

  theCommands.Add("rotate", "rotate name [names...] x y z dx dy dz angle", __FILE__, transform, g);

  theCommands.Add("pmirror", "pmirror name [names...] x y z", __FILE__, transform, g);

  theCommands.Add("lmirror", "lmirror name [names...] x y z dx dy dz", __FILE__, transform, g);

  theCommands.Add("smirror", "smirror name [names...] x y z dx dy dz", __FILE__, transform, g);

  theCommands.Add("pscale", "pscale name [names...] x y z s", __FILE__, transform, g);

  theCommands.Add("2dtranslate", "translate name [names...] dx dy", __FILE__, d2transform, g);

  theCommands.Add("2drotate", "rotate name [names...] x y dx dy  angle", __FILE__, d2transform, g);

  theCommands.Add("2dpmirror", "pmirror name [names...] x y", __FILE__, d2transform, g);

  theCommands.Add("2dlmirror", "lmirror name [names...] x y dx dy", __FILE__, d2transform, g);

  theCommands.Add("2dpscale", "pscale name [names...] x y s", __FILE__, d2transform, g);
}
