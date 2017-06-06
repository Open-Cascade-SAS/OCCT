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

//=======================================================================
//function : MaxCurvLinDist
//purpose  : 
//=======================================================================
GCPnts_DistFunction::GCPnts_DistFunction(const Adaptor3d_Curve& theCurve,
                          const Standard_Real U1, const Standard_Real U2)
: myCurve(theCurve),
  myU1(U1), myU2(U2)
{
  gp_Pnt P1 = theCurve.Value(U1), P2 = theCurve.Value(U2);
  if (P1.SquareDistance(P2) > gp::Resolution())
  {
    myLin = gp_Lin(P1, P2.XYZ() - P1.XYZ());
  }
  else
  {
    //For #28812
    theCurve.D0(U1 + .01*(U2-U1), P2);
    myLin = gp_Lin(P1, P2.XYZ() - P1.XYZ());
  }
}
//
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Boolean GCPnts_DistFunction::Value (const Standard_Real X,
                                                   Standard_Real& F)
{
  if (X < myU1 || X > myU2)
    return Standard_False;
  //
  F = -myLin.SquareDistance(myCurve.Value(X));
  return Standard_True;
}

//=======================================================================
//function : MaxCurvLinDistMV
//purpose  : 
//=======================================================================

GCPnts_DistFunctionMV::GCPnts_DistFunctionMV(GCPnts_DistFunction& theCurvLinDist)
: myMaxCurvLinDist(theCurvLinDist)
{
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
Standard_Boolean GCPnts_DistFunctionMV::Value (const math_Vector& X,
                                                     Standard_Real& F)
{
  Standard_Boolean Ok = myMaxCurvLinDist.Value(X(1), F);
  return Ok;
}

//=======================================================================
//function : NbVariables
//purpose  : 
//=======================================================================
Standard_Integer GCPnts_DistFunctionMV::NbVariables() const
{
  return 1;
}

