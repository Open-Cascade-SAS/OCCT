// Created on: 1997-05-28
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomLib_Check2dBSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//=================================================================================================

GeomLib_Check2dBSplineCurve::GeomLib_Check2dBSplineCurve(const occ::handle<Geom2d_BSplineCurve>& Curve,
                                                         const double Tolerance,
                                                         const double AngularTolerance)
    : myCurve(Curve),
      myDone(false),
      myFixFirstTangent(false),
      myFixLastTangent(false),
      myAngularTolerance(std::abs(AngularTolerance)),
      myTolerance(std::abs(Tolerance)),
      myIndSecondPole(-1),
      myIndPrelastPole(-1)
{
  int ii, num_poles;
  double    tangent_magnitude, value, vector_magnitude;
  num_poles = myCurve->NbPoles();

  if ((!myCurve->IsPeriodic()) && num_poles >= 4)
  {

    gp_Vec2d tangent, tangent_normalized, a_vector, avector_normalized;

    const double CrossProdTol = myAngularTolerance;

    // Near first
    tangent           = gp_Vec2d(myCurve->Pole(1), myCurve->Pole(2));
    tangent_magnitude = tangent.Magnitude();
    if (tangent_magnitude > myTolerance)
      tangent_normalized = tangent / tangent_magnitude;

    for (ii = 3; ii <= num_poles; ii++)
    {
      a_vector         = gp_Vec2d(myCurve->Pole(1), myCurve->Pole(ii));
      vector_magnitude = a_vector.Magnitude();

      if (tangent_magnitude > myTolerance && vector_magnitude > myTolerance)
      {
        avector_normalized = a_vector / vector_magnitude;

        double CrossProd = tangent_normalized ^ avector_normalized;
        if (std::abs(CrossProd) > CrossProdTol)
          break;

        value = tangent.Dot(a_vector);
        if (value < 0.0e0)
        {
          myFixFirstTangent = true;
          myIndSecondPole   = ii;
          break;
        }
      }
    }

    // Near last
    tangent           = gp_Vec2d(myCurve->Pole(num_poles), myCurve->Pole(num_poles - 1));
    tangent_magnitude = tangent.Magnitude();
    if (tangent_magnitude > myTolerance)
      tangent_normalized = tangent / tangent_magnitude;

    for (ii = num_poles - 2; ii >= 1; ii--)
    {
      a_vector         = gp_Vec2d(myCurve->Pole(num_poles), myCurve->Pole(ii));
      vector_magnitude = a_vector.Magnitude();

      if (tangent_magnitude > myTolerance && vector_magnitude > myTolerance)
      {
        avector_normalized = a_vector / vector_magnitude;

        double CrossProd = tangent_normalized ^ avector_normalized;
        if (std::abs(CrossProd) > CrossProdTol)
          break;

        value = tangent.Dot(a_vector);
        if (value < 0.0e0)
        {
          myFixLastTangent = true;
          myIndPrelastPole = ii;
          break;
        }
      }
    }
  } // if (( ! myCurve->IsPeriodic() )&& num_poles >= 4)
  else
  {
    myDone = true;
  }
}

//=================================================================================================

void GeomLib_Check2dBSplineCurve::NeedTangentFix(bool& FirstFlag,
                                                 bool& LastFlag) const
{
  FirstFlag = myFixFirstTangent;
  LastFlag  = myFixLastTangent;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> GeomLib_Check2dBSplineCurve::FixedTangent(
  const bool FirstFlag,
  const bool LastFlag)
{
  occ::handle<Geom2d_BSplineCurve> new_curve;
  if ((myFixFirstTangent && FirstFlag) || (myFixLastTangent && LastFlag))
  {
    new_curve = occ::down_cast<Geom2d_BSplineCurve>(myCurve->Copy());

    FixTangentOnCurve(new_curve, FirstFlag, LastFlag);
  }
  return new_curve;
}

//=================================================================================================

void GeomLib_Check2dBSplineCurve::FixTangent(const bool FirstFlag,
                                             const bool LastFlag)
{
  FixTangentOnCurve(myCurve, FirstFlag, LastFlag);
}

//=================================================================================================

void GeomLib_Check2dBSplineCurve::FixTangentOnCurve(occ::handle<Geom2d_BSplineCurve>& theCurve,
                                                    const bool       FirstFlag,
                                                    const bool       LastFlag)
{
  if (myFixFirstTangent && FirstFlag)
  {
    gp_XY         XY1       = theCurve->Pole(1).XY();
    gp_XY         XY2       = theCurve->Pole(myIndSecondPole).XY();
    double NbSamples = myIndSecondPole - 1;
    for (int i = 2; i < myIndSecondPole; i++)
    {
      double ii = i - 1;
      gp_Pnt2d      aNewPole((1. - ii / NbSamples) * XY1 + ii / NbSamples * XY2);
      theCurve->SetPole(i, aNewPole);
    }
  }

  if (myFixLastTangent && LastFlag)
  {
    int num_poles = theCurve->NbPoles();

    gp_XY         XY1       = theCurve->Pole(num_poles).XY();
    gp_XY         XY2       = theCurve->Pole(myIndPrelastPole).XY();
    double NbSamples = num_poles - myIndPrelastPole;
    for (int i = num_poles - 1; i > myIndPrelastPole; i--)
    {
      double ii = num_poles - i;
      gp_Pnt2d      aNewPole((1. - ii / NbSamples) * XY1 + ii / NbSamples * XY2);
      theCurve->SetPole(i, aNewPole);
    }
  }

  myDone = true;
}
