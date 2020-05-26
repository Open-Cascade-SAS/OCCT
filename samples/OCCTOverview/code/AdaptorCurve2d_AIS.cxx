// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "AdaptorCurve2d_AIS.h"

#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <StdPrs_PoleCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dLProp_CLProps2d.hxx>

AdaptorCurve2d_AIS::AdaptorCurve2d_AIS (const Handle(Geom2d_Curve)& theGeom2dCurve,
                                        const Aspect_TypeOfLine theTypeOfLine,
                                        const Aspect_WidthOfLine theWidthOfLine)
: myGeom2dCurve (theGeom2dCurve),
  myTypeOfLine (theTypeOfLine),
  myWidthOfLine (theWidthOfLine),
  myDisplayPole (Standard_True),
  myDisplayCurbure (Standard_False),
  myDiscretisation (20),
  myradiusmax (10),
  myradiusratio (1)
{
  //
}

void AdaptorCurve2d_AIS::Compute (const Handle(PrsMgr_PresentationManager3d)&,
                                  const Handle(Prs3d_Presentation)& thePrs,
                                  const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  Geom2dAdaptor_Curve anAdaptor(myGeom2dCurve);
  GCPnts_QuasiUniformDeflection anEdgeDistrib(anAdaptor, 1.e-2);
  if (anEdgeDistrib.IsDone())
  {
    Handle(Graphic3d_ArrayOfPolylines) aCurve = new Graphic3d_ArrayOfPolylines(anEdgeDistrib.NbPoints());
    for (Standard_Integer i = 1; i <= anEdgeDistrib.NbPoints(); ++i)
    {
      aCurve->AddVertex(anEdgeDistrib.Value(i));
    }

    Handle(Graphic3d_Group) aPrsGroup = thePrs->NewGroup();
    aPrsGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
    aPrsGroup->AddPrimitiveArray(aCurve);
  }

  if (myDisplayPole)
  {
    if (anAdaptor.GetType() == GeomAbs_BezierCurve)
    {
      Handle(Geom2d_BezierCurve) aBezier = anAdaptor.Bezier();
      Handle(Graphic3d_ArrayOfPolylines) anArrayOfVertex = new Graphic3d_ArrayOfPolylines(aBezier->NbPoles());
      for (int i = 1; i <= aBezier->NbPoles(); i++)
      {
        gp_Pnt2d CurrentPoint = aBezier->Pole(i);
        anArrayOfVertex->AddVertex(CurrentPoint.X(), CurrentPoint.Y(), 0.);
      }

      Handle(Graphic3d_Group) aPrsGroup = thePrs->NewGroup();
      aPrsGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
      aPrsGroup->AddPrimitiveArray(anArrayOfVertex);
    }

    if (anAdaptor.GetType() == GeomAbs_BSplineCurve)
    {
      Handle(Geom2d_BSplineCurve) aBSpline = anAdaptor.BSpline();
      Handle(Graphic3d_ArrayOfPolylines) anArrayOfVertex = new Graphic3d_ArrayOfPolylines(aBSpline->NbPoles());
      for (int i = 1; i <= aBSpline->NbPoles(); i++)
      {
        gp_Pnt2d CurrentPoint = aBSpline->Pole(i);
        anArrayOfVertex->AddVertex(CurrentPoint.X(), CurrentPoint.Y(), 0.);
      }

      Handle(Graphic3d_Group) aPrsGroup = thePrs->NewGroup();
      aPrsGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
      aPrsGroup->AddPrimitiveArray(anArrayOfVertex);
    }
  }

  if (myDisplayCurbure && (anAdaptor.GetType() != GeomAbs_Line))
  {
    const Standard_Integer nbintv = anAdaptor.NbIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1, nbintv + 1);
    anAdaptor.Intervals(TI, GeomAbs_CN);
    Standard_Real Resolution = 1.0e-9, Curvature;
    Geom2dLProp_CLProps2d LProp(myGeom2dCurve, 2, Resolution);
    gp_Pnt2d P1, P2;

    Handle(Graphic3d_Group) aPrsGroup = thePrs->NewGroup();
    aPrsGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
    for (Standard_Integer intrv = 1; intrv <= nbintv; intrv++)
    {
      Standard_Real t = TI(intrv);
      Standard_Real step = (TI(intrv + 1) - t) / GetDiscretisation();
      Standard_Real LRad, ratio;
      for (Standard_Integer ii = 1; ii <= myDiscretisation; ii++)
      {
        LProp.SetParameter(t);
        if (LProp.IsTangentDefined())
        {
          Curvature = Abs(LProp.Curvature());
          if (Curvature > Resolution)
          {
            myGeom2dCurve->D0(t, P1);
            LRad = 1. / Curvature;
            ratio = ((LRad > myradiusmax) ? myradiusmax / LRad : 1);
            ratio *= myradiusratio;
            LProp.CentreOfCurvature(P2);
            gp_Vec2d V(P1, P2);
            gp_Pnt2d P3 = P1.Translated(ratio*V);
            Handle(Graphic3d_ArrayOfPolylines) aSegment = new Graphic3d_ArrayOfPolylines(2);
            aSegment->AddVertex(P1.X(), P1.Y(), 0.);
            aSegment->AddVertex(P3.X(), P3.Y(), 0.);
            aPrsGroup->AddPrimitiveArray(aSegment);
          }
        }
        t += step;
      }
    }
  }
}
