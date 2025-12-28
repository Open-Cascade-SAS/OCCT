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

#include <Geom_BSplineCurve.hxx>
#include <GeomLib_CheckBSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

GeomLib_CheckBSplineCurve::GeomLib_CheckBSplineCurve(const occ::handle<Geom_BSplineCurve>& Curve,
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
  int    ii, num_poles;
  double tangent_magnitude, value, vector_magnitude;
  num_poles = myCurve->NbPoles();

  if ((!myCurve->IsPeriodic()) && num_poles >= 4)
  {

    gp_Vec tangent, tangent_normalized, a_vector, avector_normalized;

    const double CrossProdSqTol = myAngularTolerance * myAngularTolerance;

    // Near first
    tangent           = gp_Vec(myCurve->Pole(1), myCurve->Pole(2));
    tangent_magnitude = tangent.Magnitude();
    if (tangent_magnitude > myTolerance)
      tangent_normalized = tangent / tangent_magnitude;

    for (ii = 3; ii <= num_poles; ii++)
    {
      a_vector         = gp_Vec(myCurve->Pole(1), myCurve->Pole(ii));
      vector_magnitude = a_vector.Magnitude();

      if (tangent_magnitude > myTolerance && vector_magnitude > myTolerance)
      {
        avector_normalized = a_vector / vector_magnitude;

        gp_Vec CrossProd         = tangent_normalized ^ avector_normalized;
        double CrossProdSqLength = CrossProd.SquareMagnitude();
        if (CrossProdSqLength > CrossProdSqTol)
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
    tangent           = gp_Vec(myCurve->Pole(num_poles), myCurve->Pole(num_poles - 1));
    tangent_magnitude = tangent.Magnitude();
    if (tangent_magnitude > myTolerance)
      tangent_normalized = tangent / tangent_magnitude;

    for (ii = num_poles - 2; ii >= 1; ii--)
    {
      a_vector         = gp_Vec(myCurve->Pole(num_poles), myCurve->Pole(ii));
      vector_magnitude = a_vector.Magnitude();

      if (tangent_magnitude > myTolerance && vector_magnitude > myTolerance)
      {
        avector_normalized = a_vector / vector_magnitude;

        gp_Vec CrossProd         = tangent_normalized ^ avector_normalized;
        double CrossProdSqLength = CrossProd.SquareMagnitude();
        if (CrossProdSqLength > CrossProdSqTol)
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

void GeomLib_CheckBSplineCurve::NeedTangentFix(bool& FirstFlag, bool& LastFlag) const
{
  FirstFlag = myFixFirstTangent;
  LastFlag  = myFixLastTangent;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomLib_CheckBSplineCurve::FixedTangent(const bool FirstFlag,
                                                                       const bool LastFlag)
{
  occ::handle<Geom_BSplineCurve> new_curve;
  if ((myFixFirstTangent && FirstFlag) || (myFixLastTangent && LastFlag))
  {
    new_curve = occ::down_cast<Geom_BSplineCurve>(myCurve->Copy());

    FixTangentOnCurve(new_curve, FirstFlag, LastFlag);
  }
  return new_curve;
}

//=================================================================================================

void GeomLib_CheckBSplineCurve::FixTangent(const bool FirstFlag, const bool LastFlag)
{
  FixTangentOnCurve(myCurve, FirstFlag, LastFlag);
}

//=================================================================================================

void GeomLib_CheckBSplineCurve::FixTangentOnCurve(occ::handle<Geom_BSplineCurve>& theCurve,
                                                  const bool                      FirstFlag,
                                                  const bool                      LastFlag)
{
  if (myFixFirstTangent && FirstFlag)
  {
    gp_XYZ XYZ1      = theCurve->Pole(1).XYZ();
    gp_XYZ XYZ2      = theCurve->Pole(myIndSecondPole).XYZ();
    double NbSamples = myIndSecondPole - 1;
    for (int i = 2; i < myIndSecondPole; i++)
    {
      double ii = i - 1;
      gp_Pnt aNewPole((1. - ii / NbSamples) * XYZ1 + ii / NbSamples * XYZ2);
      theCurve->SetPole(i, aNewPole);
    }
  }

  if (myFixLastTangent && LastFlag)
  {
    int num_poles = theCurve->NbPoles();

    gp_XYZ XYZ1      = theCurve->Pole(num_poles).XYZ();
    gp_XYZ XYZ2      = theCurve->Pole(myIndPrelastPole).XYZ();
    double NbSamples = num_poles - myIndPrelastPole;
    for (int i = num_poles - 1; i > myIndPrelastPole; i--)
    {
      double ii = num_poles - i;
      gp_Pnt aNewPole((1. - ii / NbSamples) * XYZ1 + ii / NbSamples * XYZ2);
      theCurve->SetPole(i, aNewPole);
    }
  }

  myDone = true;
}
