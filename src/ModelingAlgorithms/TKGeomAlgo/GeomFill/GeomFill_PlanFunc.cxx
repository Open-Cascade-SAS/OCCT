// Created on: 1998-10-29
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#include <Adaptor3d_Curve.hxx>
#include <GeomFill_PlanFunc.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

GeomFill_PlanFunc::GeomFill_PlanFunc(const gp_Pnt&                       theP,
                                     const gp_Vec&                       theV,
                                     const occ::handle<Adaptor3d_Curve>& theC)
    : myCurve(theC)
{
  myPnt = theP.XYZ();
  myVec = theV.XYZ();
}

bool GeomFill_PlanFunc::Value(const double X, double& F)
{
  myCurve->D0(X, G);
  V.SetLinearForm(-1, myPnt, G.XYZ());
  F = myVec.Dot(V);
  return true;
}

bool GeomFill_PlanFunc::Derivative(const double X, double& D)
{
  gp_Vec dg;
  myCurve->D1(X, G, dg);
  D = myVec.Dot(dg.XYZ());

  return true;
}

bool GeomFill_PlanFunc::Values(const double X, double& F, double& D)
{
  gp_Vec dg;
  myCurve->D1(X, G, dg);
  V.SetLinearForm(-1, myPnt, G.XYZ());
  F = myVec.Dot(V);
  D = myVec.Dot(dg.XYZ());

  return true;
}

// void  GeomFill_PlanFunc::D2(const double  X,
//			    double& F,
//			    double& D1,
//			    double& D2)
void GeomFill_PlanFunc::D2(const double, double&, double&, double&) {}

void GeomFill_PlanFunc::DEDT(const double X, const gp_Vec& DPnt, const gp_Vec& DVec, double& DFDT)
{
  myCurve->D0(X, G);
  V.SetLinearForm(-1, myPnt, G.XYZ());
  DFDT = DVec.Dot(V) - myVec.Dot(DPnt.XYZ());
}

void GeomFill_PlanFunc::D2E(const double  X,
                            const gp_Vec& DP,
                            //			     const gp_Vec& D2P,
                            const gp_Vec&,
                            const gp_Vec& DV,
                            //			     const gp_Vec& D2V,
                            const gp_Vec&,
                            double& DFDT,
                            //			     double& ,
                            double& /*D2FDT2*/,
                            //			     double& D2FDTDX)
                            double&)
{
  gp_Vec dg;
  myCurve->D1(X, G, dg);
  gp_XYZ DVDT;
  V.SetLinearForm(-1, myPnt, G.XYZ());
  DVDT.SetLinearForm(-1, DP.XYZ(), G.XYZ());
  DFDT = DV.Dot(V) + myVec.Dot(DVDT);
}
