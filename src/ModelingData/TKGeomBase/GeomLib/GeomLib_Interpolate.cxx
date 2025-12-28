// Created on: 1996-08-30
// Created by: Xavier BENVENISTE
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomLib_Interpolate.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

GeomLib_Interpolate::GeomLib_Interpolate(const int      Degree,
                                         const int      NumPoints,
                                         const NCollection_Array1<gp_Pnt>&   PointsArray,
                                         const NCollection_Array1<double>& ParametersArray)
    : myIsDone(false)
{
  int ii, num_knots, inversion_problem, num_controls, jj;

  if (NumPoints < Degree || PointsArray.Lower() != 1 || PointsArray.Upper() < NumPoints
      || ParametersArray.Lower() != 1 || ParametersArray.Upper() < NumPoints)
  {
    myError = GeomLib_NotEnoughtPoints;
  }
  else if (Degree < 3)
  {
    myError = GeomLib_DegreeSmallerThan3;
  }
  else
  {
    gp_Pnt           null_point(0.0e0, 0.0e0, 0.0e0);
    int order = Degree + 1, half_order;
    if (order % 2)
    {
      order -= 1;
    }
    half_order   = order / 2;
    num_knots    = NumPoints + 2 * order - 2;
    num_controls = num_knots - order;
    NCollection_Array1<double>    flat_knots(1, num_knots);
    NCollection_Array1<int> contacts(1, num_controls);
    NCollection_Array1<int> multiplicities(1, NumPoints);
    NCollection_Array1<double>    parameters(1, num_controls);
    NCollection_Array1<gp_Pnt>      poles(1, num_controls);

    for (ii = 1; ii <= NumPoints; ii++)
    {
      multiplicities(ii) = 1;
    }
    multiplicities(1)         = order;
    multiplicities(NumPoints) = order;
    for (ii = 1, jj = num_controls + 1; ii <= order; ii++, jj++)
    {

      flat_knots(ii) = ParametersArray(1);
      flat_knots(jj) = ParametersArray(NumPoints);
    }
    jj = order + 1;
    for (ii = 2; ii < NumPoints; ii++)
    {
      flat_knots(jj) = ParametersArray(ii);
      jj += 1;
    }
    for (ii = 1; ii <= num_controls; ii++)
    {
      contacts(ii) = 0;
    }
    jj = num_controls;
    for (ii = 1; ii <= half_order; ii++)
    {
      contacts(ii)   = half_order + ii - 1;
      parameters(ii) = ParametersArray(1);
      poles(ii)      = null_point;
      contacts(jj)   = half_order + ii - 1;
      parameters(jj) = ParametersArray(NumPoints);
      poles(jj)      = null_point;
      jj -= 1;
    }
    jj = half_order + 1;
    for (ii = 2; ii < NumPoints; ii++)
    {
      parameters(jj) = ParametersArray(ii);
      poles(jj)      = PointsArray(ii);
      jj += 1;
    }
    contacts(1)            = 0;
    contacts(num_controls) = 0;
    poles(1)               = PointsArray(1);
    poles(num_controls)    = PointsArray(NumPoints);
    BSplCLib::Interpolate(order - 1, flat_knots, parameters, contacts, poles, inversion_problem);

    if (!inversion_problem)
    {
      myCurve  = new Geom_BSplineCurve(poles, ParametersArray, multiplicities, order - 1);
      myIsDone = true;
    }
    else
    {
      myError = GeomLib_InversionProblem;
    }
  }
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomLib_Interpolate::Curve() const
{
  return myCurve;
}
