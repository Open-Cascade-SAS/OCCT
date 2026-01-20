// Copyright (c) 2014-2014 OPEN CASCADE SAS
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

#include <GCPnts_DistFunction.hxx>
#include <gp_Pnt.hxx>

//=================================================================================================

GCPnts_DistFunction::GCPnts_DistFunction(const Adaptor3d_Curve& theCurve,
                                         const double    U1,
                                         const double    U2)
    : myCurve(theCurve),
      myU1(U1),
      myU2(U2)
{
  gp_Pnt P1 = theCurve.Value(U1), P2 = theCurve.Value(U2);
  if (P1.SquareDistance(P2) > gp::Resolution())
  {
    myLin = gp_Lin(P1, P2.XYZ() - P1.XYZ());
  }
  else
  {
    // For #28812
    theCurve.D0(U1 + .01 * (U2 - U1), P2);
    myLin = gp_Lin(P1, P2.XYZ() - P1.XYZ());
  }
}

//
//=================================================================================================

bool GCPnts_DistFunction::Value(const double X, double& F)
{
  if (X < myU1 || X > myU2)
    return false;
  //
  F = -myLin.SquareDistance(myCurve.Value(X));
  return true;
}

//=================================================================================================

GCPnts_DistFunctionMV::GCPnts_DistFunctionMV(GCPnts_DistFunction& theCurvLinDist)
    : myMaxCurvLinDist(theCurvLinDist)
{
}

//=================================================================================================

bool GCPnts_DistFunctionMV::Value(const math_Vector& X, double& F)
{
  bool Ok = myMaxCurvLinDist.Value(X(1), F);
  return Ok;
}

//=================================================================================================

int GCPnts_DistFunctionMV::NbVariables() const
{
  return 1;
}
