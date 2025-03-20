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

GeomLib_CheckBSplineCurve::GeomLib_CheckBSplineCurve(const Handle(Geom_BSplineCurve)& Curve,
                                                     const Standard_Real              Tolerance,
                                                     const Standard_Real AngularTolerance)
    : myCurve(Curve),
      myDone(Standard_False),
      myFixFirstTangent(Standard_False),
      myFixLastTangent(Standard_False),
      myAngularTolerance(Abs(AngularTolerance)),
      myTolerance(Abs(Tolerance)),
      myIndSecondPole(-1),
      myIndPrelastPole(-1)
{
  Standard_Integer ii, num_poles;
  Standard_Real    tangent_magnitude, value, vector_magnitude;
  num_poles = myCurve->NbPoles();

  if ((!myCurve->IsPeriodic()) && num_poles >= 4)
  {

    gp_Vec tangent, tangent_normalized, a_vector, avector_normalized;

    const Standard_Real CrossProdSqTol = myAngularTolerance * myAngularTolerance;

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

        gp_Vec        CrossProd         = tangent_normalized ^ avector_normalized;
        Standard_Real CrossProdSqLength = CrossProd.SquareMagnitude();
        if (CrossProdSqLength > CrossProdSqTol)
          break;

        value = tangent.Dot(a_vector);
        if (value < 0.0e0)
        {
          myFixFirstTangent = Standard_True;
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

        gp_Vec        CrossProd         = tangent_normalized ^ avector_normalized;
        Standard_Real CrossProdSqLength = CrossProd.SquareMagnitude();
        if (CrossProdSqLength > CrossProdSqTol)
          break;

        value = tangent.Dot(a_vector);
        if (value < 0.0e0)
        {
          myFixLastTangent = Standard_True;
          myIndPrelastPole = ii;
          break;
        }
      }
    }
  } // if (( ! myCurve->IsPeriodic() )&& num_poles >= 4)
  else
  {
    myDone = Standard_True;
  }
}

//=================================================================================================

void GeomLib_CheckBSplineCurve::NeedTangentFix(Standard_Boolean& FirstFlag,
                                               Standard_Boolean& LastFlag) const
{
  FirstFlag = myFixFirstTangent;
  LastFlag  = myFixLastTangent;
}

//=================================================================================================

Handle(Geom_BSplineCurve) GeomLib_CheckBSplineCurve::FixedTangent(const Standard_Boolean FirstFlag,
                                                                  const Standard_Boolean LastFlag)
{
  Handle(Geom_BSplineCurve) new_curve;
  if ((myFixFirstTangent && FirstFlag) || (myFixLastTangent && LastFlag))
  {
    new_curve = Handle(Geom_BSplineCurve)::DownCast(myCurve->Copy());

    FixTangentOnCurve(new_curve, FirstFlag, LastFlag);
  }
  return new_curve;
}

//=================================================================================================

void GeomLib_CheckBSplineCurve::FixTangent(const Standard_Boolean FirstFlag,
                                           const Standard_Boolean LastFlag)
{
  FixTangentOnCurve(myCurve, FirstFlag, LastFlag);
}

//=================================================================================================

void GeomLib_CheckBSplineCurve::FixTangentOnCurve(Handle(Geom_BSplineCurve)& theCurve,
                                                  const Standard_Boolean     FirstFlag,
                                                  const Standard_Boolean     LastFlag)
{
  if (myFixFirstTangent && FirstFlag)
  {
    gp_XYZ        XYZ1      = theCurve->Pole(1).XYZ();
    gp_XYZ        XYZ2      = theCurve->Pole(myIndSecondPole).XYZ();
    Standard_Real NbSamples = myIndSecondPole - 1;
    for (Standard_Integer i = 2; i < myIndSecondPole; i++)
    {
      Standard_Real ii = i - 1;
      gp_Pnt        aNewPole((1. - ii / NbSamples) * XYZ1 + ii / NbSamples * XYZ2);
      theCurve->SetPole(i, aNewPole);
    }
  }

  if (myFixLastTangent && LastFlag)
  {
    Standard_Integer num_poles = theCurve->NbPoles();

    gp_XYZ        XYZ1      = theCurve->Pole(num_poles).XYZ();
    gp_XYZ        XYZ2      = theCurve->Pole(myIndPrelastPole).XYZ();
    Standard_Real NbSamples = num_poles - myIndPrelastPole;
    for (Standard_Integer i = num_poles - 1; i > myIndPrelastPole; i--)
    {
      Standard_Real ii = num_poles - i;
      gp_Pnt        aNewPole((1. - ii / NbSamples) * XYZ1 + ii / NbSamples * XYZ2);
      theCurve->SetPole(i, aNewPole);
    }
  }

  myDone = Standard_True;
}
