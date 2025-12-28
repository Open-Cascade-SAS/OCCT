// Created on: 1994-09-15
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <ProjLib_ProjectOnSurface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Approx_FitAndDivide.hxx>
#include <BSplCLib.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_POnSurf.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Precision.hxx>
#include <Standard_NoSuchObject.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

//=======================================================================
// function : OnSurface_Value
// purpose  : Evaluate current point of the projected curve
//=======================================================================
static gp_Pnt OnSurface_Value(const double                        U,
                              const occ::handle<Adaptor3d_Curve>& myCurve,
                              Extrema_ExtPS*                      myExtPS)
{
  // Try to find the closest solution point.
  myExtPS->Perform(myCurve->Value(U));

  double Dist2Min = RealLast();
  int    Index    = 0;

  for (int i = 1; i <= myExtPS->NbExt(); i++)
  {
    if (myExtPS->SquareDistance(i) < Dist2Min)
    {
      Index    = i;
      Dist2Min = myExtPS->SquareDistance(Index);
    }
  }
  if (Index == 0)
  {
    std::cout << " Extrema non trouve pour U = " << U << std::endl;
    return gp_Pnt(0., 0., 0.);
  }
  else
  {
    return (myExtPS->Point(Index)).Value();
  }
}

//=================================================================================================

static bool OnSurface_D1(const double,                        // U,
                         gp_Pnt&,                             // P,
                         gp_Vec&,                             // V,
                         const occ::handle<Adaptor3d_Curve>&, //  myCurve,
                         Extrema_ExtPS*)                      // myExtPS)
{
  return false;
}

//=======================================================================
//  class  : ProjLib_OnSurface
// purpose  : Use to approximate the projection on a plane
//=======================================================================

class ProjLib_OnSurface : public AppCont_Function

{
public:
  ProjLib_OnSurface(const occ::handle<Adaptor3d_Curve>& C, const occ::handle<Adaptor3d_Surface>& S)
      : myCurve(C)
  {
    myNbPnt              = 1;
    myNbPnt2d            = 0;
    double           U   = myCurve->FirstParameter();
    gp_Pnt           P   = myCurve->Value(U);
    constexpr double Tol = Precision::PConfusion();
    myExtPS              = new Extrema_ExtPS(P, *S, Tol, Tol);
  }

  ~ProjLib_OnSurface() override { delete myExtPS; }

  double FirstParameter() const override { return myCurve->FirstParameter(); }

  double LastParameter() const override { return myCurve->LastParameter(); }

  bool Value(const double theT,
             NCollection_Array1<gp_Pnt2d>& /*thePnt2d*/,
             NCollection_Array1<gp_Pnt>& thePnt) const override
  {
    thePnt(1) = OnSurface_Value(theT, myCurve, myExtPS);
    return true;
  }

  bool D1(const double theT,
          NCollection_Array1<gp_Vec2d>& /*theVec2d*/,
          NCollection_Array1<gp_Vec>& theVec) const override
  {
    gp_Pnt aPnt;
    return OnSurface_D1(theT, aPnt, theVec(1), myCurve, myExtPS);
  }

private:
  ProjLib_OnSurface(const ProjLib_OnSurface&)            = delete;
  ProjLib_OnSurface& operator=(const ProjLib_OnSurface&) = delete;

private:
  occ::handle<Adaptor3d_Curve> myCurve;
  Extrema_ExtPS*               myExtPS;
};

//=====================================================================//
//                                                                     //
//  D E S C R I P T I O N   O F   T H E   C L A S S  :                 //
//                                                                     //
//         P r o j L i b _ A p p r o x P r o j e c t O n P l a n e     //
//                                                                     //
//=====================================================================//

//=================================================================================================

ProjLib_ProjectOnSurface::ProjLib_ProjectOnSurface()
    : myTolerance(0.0),
      myIsDone(false)
{
}

//=================================================================================================

ProjLib_ProjectOnSurface::ProjLib_ProjectOnSurface(const occ::handle<Adaptor3d_Surface>& S)
    : myTolerance(0.0),
      myIsDone(false)
{
  mySurface = S;
}

//=================================================================================================

void ProjLib_ProjectOnSurface::Load(const occ::handle<Adaptor3d_Surface>& S)
{
  mySurface = S;
  myIsDone  = false;
}

//=================================================================================================

void ProjLib_ProjectOnSurface::Load(const occ::handle<Adaptor3d_Curve>& C, const double Tolerance)
{
  myTolerance = Tolerance;
  myCurve     = C;
  myIsDone    = false;
  if (!mySurface.IsNull())
  {

    ProjLib_OnSurface F(myCurve, mySurface);

    int Deg1, Deg2;
    Deg1 = 8;
    Deg2 = 8;

    Approx_FitAndDivide Fit(F,
                            Deg1,
                            Deg2,
                            Precision::Approximation(),
                            Precision::PApproximation(),
                            true);
    int                 i;
    int                 NbCurves = Fit.NbMultiCurves();
    int                 MaxDeg   = 0;

    // To convert the MultiCurve to BSpline, all constituent Bezier curves
    // must have the same degree -> Calculate MaxDeg
    int NbPoles = 1;
    for (i = 1; i <= NbCurves; i++)
    {
      int Deg = Fit.Value(i).Degree();
      MaxDeg  = std::max(MaxDeg, Deg);
    }
    NbPoles = MaxDeg * NbCurves + 1; // Poles on the BSpline
    NCollection_Array1<gp_Pnt> Poles(1, NbPoles);

    NCollection_Array1<gp_Pnt> TempPoles(1, MaxDeg + 1); // for degree elevation

    NCollection_Array1<double> Knots(1, NbCurves + 1); // Knots of the BSpline

    int Compt = 1;
    for (i = 1; i <= Fit.NbMultiCurves(); i++)
    {
      Fit.Parameters(i, Knots(i), Knots(i + 1));

      AppParCurves_MultiCurve    MC = Fit.Value(i);              // Load the i-th Curve
      NCollection_Array1<gp_Pnt> LocalPoles(1, MC.Degree() + 1); // Get the poles
      MC.Curve(1, Poles);

      // Possible degree elevation
      int Inc = MaxDeg - MC.Degree();
      if (Inc > 0)
      {
        BSplCLib::IncreaseDegree(Inc,
                                 LocalPoles,
                                 BSplCLib::NoWeights(),
                                 TempPoles,
                                 BSplCLib::NoWeights());
        // update the poles of the PCurve
        for (int j = 1; j <= MaxDeg + 1; j++)
        {
          Poles.SetValue(Compt, TempPoles(j));
          Compt++;
        }
      }
      else
      {
        // update the poles of the PCurve
        for (int j = 1; j <= MaxDeg + 1; j++)
        {
          Poles.SetValue(Compt, LocalPoles(j));
          Compt++;
        }
      }

      Compt--;
    }

    // update the fields of ProjLib_Approx

    int NbKnots = NbCurves + 1;

    NCollection_Array1<int> Mults(1, NbKnots);
    Mults.SetValue(1, MaxDeg + 1);
    for (i = 2; i <= NbCurves; i++)
    {
      Mults.SetValue(i, MaxDeg);
    }
    Mults.SetValue(NbKnots, MaxDeg + 1);
    myResult = new Geom_BSplineCurve(Poles, Knots, Mults, MaxDeg, false);
    myIsDone = true;
  }
}

//=================================================================================================

ProjLib_ProjectOnSurface::~ProjLib_ProjectOnSurface() = default;

//=================================================================================================

occ::handle<Geom_BSplineCurve> ProjLib_ProjectOnSurface::BSpline() const
{
  Standard_NoSuchObject_Raise_if(!myIsDone, "ProjLib_ProjectOnSurface:BSpline");
  return myResult;
}
