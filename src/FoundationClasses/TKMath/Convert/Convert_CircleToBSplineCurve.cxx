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

// JCV 16/10/91

#include <Convert_CircleToBSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

// Attention :
// To avoid use of persistent tables in the fields
// the tables are dimensioned to the maximum (TheNbKnots and TheNbPoles)
// that correspond to the full circle. For an arc of circle there is a
// need of less poles and nodes, that is why the fields
// nbKnots and nbPoles are present and updated in the
// constructor of an arc of B-spline circle to take into account
// the real number of poles and nodes.
//  parametrization :
//  Reference : Rational B-spline for Curve and Surface Representation
//              Wayne Tiller  CADG September 1983
//  x(t) = (1 - t^2) / (1 + t^2)
//  y(t) =  2 t / (1 + t^2)
//  then t = std::sqrt(2) u /  ((std::sqrt(2) - 2) u + 2)
//  => u = 2 t / (std::sqrt(2) + (2 - std::sqrt(2)) t)
//=======================================================================
// function : Convert_CircleToBSplineCurve
// purpose  : this constructs a periodic circle
//=======================================================================
Convert_CircleToBSplineCurve::Convert_CircleToBSplineCurve(
  const gp_Circ2d&                   C,
  const Convert_ParameterisationType Parameterisation)
    : Convert_ConicToBSplineCurve(0, 0, 0)
{

  int ii;

  double                    R, value;
  NCollection_Array1<double> CosNumerator, SinNumerator;

  R = C.Radius();
  if (Parameterisation != Convert_TgtThetaOver2 && Parameterisation != Convert_RationalC1)
  {
    // In case if BuildCosAndSin does not know how to manage the periodicity
    // => trim on 0,2*PI
    myIsPeriodic = false;
    Convert_ConicToBSplineCurve::BuildCosAndSin(Parameterisation,
                                                0,
                                                2 * M_PI,
                                                CosNumerator,
                                                SinNumerator,
                                                myWeights,
                                                myDegree,
                                                myKnots,
                                                myMults);
  }
  else
  {
    myIsPeriodic = true;
    Convert_ConicToBSplineCurve::BuildCosAndSin(Parameterisation,
                                                CosNumerator,
                                                SinNumerator,
                                                myWeights,
                                                myDegree,
                                                myKnots,
                                                myMults);
  }

  myNbPoles = CosNumerator.Length();
  myNbKnots = myKnots.Length();

  myPoles = NCollection_Array1<gp_Pnt2d>(1, myNbPoles);

  gp_Dir2d  Ox = C.XAxis().Direction();
  gp_Dir2d  Oy = C.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation(C.XAxis(), gp::OX2d());
  if (Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0)
  {
    value = R;
  }
  else
  {
    value = -R;
  }

  // Replace the bspline in the reference of the circle.
  // and calculate the weight of the bspline.

  for (ii = 1; ii <= myNbPoles; ii++)
  {
    myPoles(ii).SetCoord(1, R * CosNumerator(ii));
    myPoles(ii).SetCoord(2, value * SinNumerator(ii));
    myPoles(ii).Transform(Trsf);
  }
}

//=======================================================================
// function : Convert_CircleToBSplineCurve
// purpose  : this constructs a non periodic circle
//=======================================================================

Convert_CircleToBSplineCurve::Convert_CircleToBSplineCurve(
  const gp_Circ2d&                   C,
  const double                       UFirst,
  const double                       ULast,
  const Convert_ParameterisationType Parameterisation)
    : Convert_ConicToBSplineCurve(0, 0, 0)
{
  double delta = ULast - UFirst;
  double Eps   = Precision::PConfusion();

  if ((delta > (2 * M_PI + Eps)) || (delta <= 0.0e0))
  {
    throw Standard_DomainError("Convert_CircleToBSplineCurve");
  }

  int                       ii;
  double                    R, value;
  NCollection_Array1<double> CosNumerator, SinNumerator;

  R            = C.Radius();
  myIsPeriodic = false;
  Convert_ConicToBSplineCurve::BuildCosAndSin(Parameterisation,
                                              UFirst,
                                              ULast,
                                              CosNumerator,
                                              SinNumerator,
                                              myWeights,
                                              myDegree,
                                              myKnots,
                                              myMults);

  myNbPoles = CosNumerator.Length();
  myNbKnots = myKnots.Length();

  myPoles = NCollection_Array1<gp_Pnt2d>(1, myNbPoles);

  gp_Dir2d  Ox = C.XAxis().Direction();
  gp_Dir2d  Oy = C.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation(C.XAxis(), gp::OX2d());
  if (Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0)
  {
    value = R;
  }
  else
  {
    value = -R;
  }

  // Replace the bspline in the reference of the circle.
  // and calculate the weight of the bspline.

  for (ii = 1; ii <= myNbPoles; ii++)
  {
    myPoles(ii).SetCoord(1, R * CosNumerator(ii));
    myPoles(ii).SetCoord(2, value * SinNumerator(ii));
    myPoles(ii).Transform(Trsf);
  }
}
