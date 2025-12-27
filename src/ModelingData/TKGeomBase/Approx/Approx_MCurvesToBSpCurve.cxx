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

#include <AppParCurves_MultiPoint.hxx>
#include <Approx_MCurvesToBSpCurve.hxx>
#include <BSplCLib.hxx>
#include <Convert_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <Convert_CompBezierCurvesToBSplineCurve.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

#ifdef OCCT_DEBUG
static void DEBUG(const AppParCurves_MultiCurve& MC)
{
  int     i, j;
  int     nbcu    = MC.NbCurves();
  int     nbpoles = MC.NbPoles();
  NCollection_Array1<gp_Pnt>   Poles(1, nbpoles);
  NCollection_Array1<gp_Pnt2d> Poles2d(1, nbpoles);

  for (i = 1; i <= nbcu; i++)
  {
    std::cout << " Curve No. " << i << std::endl;
    if (MC.Dimension(i) == 3)
    {
      MC.Curve(i, Poles);
      for (j = 1; j <= nbpoles; j++)
      {
        std::cout << " Pole = " << Poles(j).X() << " " << Poles(j).Y() << " " << Poles(j).Z()
                  << std::endl;
      }
    }
    else
    {
      MC.Curve(i, Poles2d);
      for (j = 1; j <= nbpoles; j++)
      {
        std::cout << " Pole = " << Poles2d(j).X() << " " << Poles2d(j).Y() << std::endl;
      }
    }
  }
}
#endif

Approx_MCurvesToBSpCurve::Approx_MCurvesToBSpCurve()
{
  myDone = false;
}

void Approx_MCurvesToBSpCurve::Reset()
{
  myDone = false;
  myCurves.Clear();
}

void Approx_MCurvesToBSpCurve::Append(const AppParCurves_MultiCurve& MC)
{
  myCurves.Append(MC);
}

void Approx_MCurvesToBSpCurve::Perform()
{
  Perform(myCurves);
}

void Approx_MCurvesToBSpCurve::Perform(const NCollection_Sequence<AppParCurves_MultiCurve>& TheSeq)
{

  int        i, j, deg = 0;
  int        nbcu = TheSeq.Length();
  AppParCurves_MultiCurve CU;
  int        nbpolesspl = 0, nbknots = 0;
#ifdef OCCT_DEBUG
  bool debug = false;
#endif

  if (nbcu == 1)
  {
    CU  = TheSeq.Value(1);
    deg = CU.Degree();
    NCollection_Array1<double>    Knots(1, 2);
    NCollection_Array1<int> Mults(1, 2);
    Knots(1) = 0.0;
    Knots(2) = 1.0;
    Mults(1) = Mults(2) = deg + 1;
    mySpline            = AppParCurves_MultiBSpCurve(CU, Knots, Mults);
  }
  else
  {

    AppParCurves_MultiPoint P    = TheSeq.Value(nbcu).Value(1);
    int        nb3d = P.NbPoints();
    int        nb2d = P.NbPoints2d();

    Convert_CompBezierCurvesToBSplineCurve     conv;
    Convert_CompBezierCurves2dToBSplineCurve2d conv2d;

    if (nb3d != 0)
    {
      for (i = 1; i <= nbcu; i++)
      {
        CU = TheSeq.Value(i);
        NCollection_Array1<gp_Pnt> ThePoles3d(1, CU.NbPoles());
        CU.Curve(1, ThePoles3d);
        conv.AddCurve(ThePoles3d);
      }
      conv.Perform();
    }

    else if (nb2d != 0)
    {
      for (i = 1; i <= nbcu; i++)
      {
        CU = TheSeq.Value(i);
        NCollection_Array1<gp_Pnt2d> ThePoles2d(1, CU.NbPoles());
        CU.Curve(1 + nb3d, ThePoles2d);
        conv2d.AddCurve(ThePoles2d);
      }
      conv2d.Perform();
    }

    // Recuperation:
    if (nb3d != 0)
    {
      nbpolesspl = conv.NbPoles();
      nbknots    = conv.NbKnots();
    }
    else if (nb2d != 0)
    {
      nbpolesspl = conv2d.NbPoles();
      nbknots    = conv2d.NbKnots();
    }

    NCollection_Array1<AppParCurves_MultiPoint> tabMU(1, nbpolesspl);
    NCollection_Array1<gp_Pnt>              PolesSpl(1, nbpolesspl);
    NCollection_Array1<gp_Pnt2d>            PolesSpl2d(1, nbpolesspl);
    NCollection_Array1<int>         TheMults(1, nbknots);
    NCollection_Array1<double>            TheKnots(1, nbknots);

    if (nb3d != 0)
    {
      conv.KnotsAndMults(TheKnots, TheMults);
      conv.Poles(PolesSpl);
      deg = conv.Degree();
    }
    else if (nb2d != 0)
    {
      conv2d.KnotsAndMults(TheKnots, TheMults);
      conv2d.Poles(PolesSpl2d);
      deg = conv2d.Degree();
    }

    for (j = 1; j <= nbpolesspl; j++)
    {
      AppParCurves_MultiPoint MP(nb3d, nb2d);
      if (nb3d != 0)
      {
        MP.SetPoint(1, PolesSpl(j));
      }
      else if (nb2d != 0)
      {
        MP.SetPoint2d(1 + nb3d, PolesSpl2d(j));
      }
      tabMU.SetValue(j, MP);
    }

    int kpol = 1, kpoles3d = 1, kpoles2d = 1;
    int mydegre, k;
    int first, last, Inc, thefirst;
    if (nb3d != 0)
      thefirst = 1;
    else
      thefirst = 2;

    for (i = 1; i <= nbcu; i++)
    {
      CU      = TheSeq.Value(i);
      mydegre = CU.Degree();
      if (TheMults(i + 1) == deg)
        last = deg + 1; // Continuite C0
      else
        last = deg; // Continuite C1
      if (i == nbcu)
      {
        last = deg + 1;
      }
      first = 1;
      if (i == 1)
        first = 1;
      else if ((TheMults(i) == deg - 1) || (TheMults(i) == deg))
        first = 2;

      for (j = 2; j <= nb3d; j++)
      {
        kpol = kpoles3d;
        NCollection_Array1<gp_Pnt> ThePoles(1, CU.NbPoles());
        CU.Curve(j, ThePoles);

        Inc = deg - mydegre;
        NCollection_Array1<gp_Pnt> Points(1, deg + 1);
        if (Inc > 0)
        {
          BSplCLib::IncreaseDegree(deg,
                                   ThePoles,
                                   BSplCLib::NoWeights(),
                                   Points,
                                   BSplCLib::NoWeights());
        }
        else
        {
          Points = ThePoles;
        }

        for (k = first; k <= last; k++)
        {
          tabMU.ChangeValue(kpol++).SetPoint(j, Points(k));
        }
      }
      kpoles3d = kpol;

      for (j = thefirst; j <= nb2d; j++)
      {
        kpol = kpoles2d;
        NCollection_Array1<gp_Pnt2d> ThePoles2d(1, CU.NbPoles());
        CU.Curve(j + nb3d, ThePoles2d);

        Inc = deg - mydegre;
        NCollection_Array1<gp_Pnt2d> Points2d(1, deg + 1);
        if (Inc > 0)
        {
          BSplCLib::IncreaseDegree(deg,
                                   ThePoles2d,
                                   BSplCLib::NoWeights(),
                                   Points2d,
                                   BSplCLib::NoWeights());
        }
        else
        {
          Points2d = ThePoles2d;
        }
        for (k = first; k <= last; k++)
        {
          tabMU.ChangeValue(kpol++).SetPoint2d(j + nb3d, Points2d(k));
        }
      }
      kpoles2d = kpol;
    }

    mySpline = AppParCurves_MultiBSpCurve(tabMU, TheKnots, TheMults);
  }
#ifdef OCCT_DEBUG
  if (debug)
    DEBUG(mySpline);
#endif

  myDone = true;
}

const AppParCurves_MultiBSpCurve& Approx_MCurvesToBSpCurve::Value() const
{
  return mySpline;
}

const AppParCurves_MultiBSpCurve& Approx_MCurvesToBSpCurve::ChangeValue()
{
  return mySpline;
}
