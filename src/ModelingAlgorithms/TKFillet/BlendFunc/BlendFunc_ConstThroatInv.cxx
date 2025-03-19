// Created by: Julia GERASIMOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <Adaptor2d_Curve2d.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ConstThroatInv.hxx>
#include <math_Matrix.hxx>

//=================================================================================================

BlendFunc_ConstThroatInv::BlendFunc_ConstThroatInv(const Handle(Adaptor3d_Surface)& S1,
                                                   const Handle(Adaptor3d_Surface)& S2,
                                                   const Handle(Adaptor3d_Curve)&   C)
    : BlendFunc_GenChamfInv(S1, S2, C),
      Throat(0.0),
      param(0.0),
      sign1(0.0),
      sign2(0.0),
      normtg(0.0),
      theD(0.0)
{
}

//=================================================================================================

void BlendFunc_ConstThroatInv::Set(const Standard_Real theThroat,
                                   const Standard_Real,
                                   const Standard_Integer Choix)
{
  // Standard_Real dis1,dis2;

  Throat = theThroat;

  choix = Choix;
  switch (choix)
  {
    case 1:
    case 2: {
      sign1 = -1;
      sign2 = -1;
    }
    break;
    case 3:
    case 4: {
      sign1 = 1;
      sign2 = -1;
    }
    break;
    case 5:
    case 6: {
      sign1 = 1;
      sign2 = 1;
    }
    break;
    case 7:
    case 8: {
      sign1 = -1;
      sign2 = 1;
    }
    break;
    default:
      sign1 = -1;
      sign2 = -1;
  }
}

//=================================================================================================

Standard_Boolean BlendFunc_ConstThroatInv::IsSolution(const math_Vector&  Sol,
                                                      const Standard_Real Tol)
{
  math_Vector valsol(1, 4);
  Value(Sol, valsol);

  if (Abs(valsol(1)) <= Tol && Abs(valsol(2)) <= Tol && Abs(valsol(3)) <= Tol * Tol
      && Abs(valsol(4)) <= Tol * Tol)
    return Standard_True;

  return Standard_False;
}

//=================================================================================================

Standard_Boolean BlendFunc_ConstThroatInv::Value(const math_Vector& X, math_Vector& F)
{
  gp_Pnt2d p2d;
  gp_Vec2d v2d;
  csurf->D1(X(1), p2d, v2d);
  param = X(2);
  curv->D2(param, ptgui, d1gui, d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD   = -(nplan.XYZ().Dot(ptgui.XYZ()));

  math_Vector XX(1, 4);

  if (first)
  {
    XX(1) = p2d.X();
    XX(2) = p2d.Y();
    XX(3) = X(3);
    XX(4) = X(4);
  }

  else
  {
    XX(1) = X(3);
    XX(2) = X(4);
    XX(3) = p2d.X();
    XX(4) = p2d.Y();
  }

  surf1->D0(XX(1), XX(2), pts1);
  surf2->D0(XX(3), XX(4), pts2);

  F(1) = nplan.XYZ().Dot(pts1.XYZ()) + theD;
  F(2) = nplan.XYZ().Dot(pts2.XYZ()) + theD;

  const gp_Pnt ptmid((pts1.XYZ() + pts2.XYZ()) / 2);
  const gp_Vec vmid(ptgui, ptmid);

  F(3) = vmid.SquareMagnitude() - Throat * Throat;

  const gp_Vec vref1(ptgui, pts1);
  const gp_Vec vref2(ptgui, pts2);

  F(4) = vref1.SquareMagnitude() - vref2.SquareMagnitude();

  return Standard_True;
}

//=================================================================================================

Standard_Boolean BlendFunc_ConstThroatInv::Derivatives(const math_Vector& X, math_Matrix& D)
{
  // Standard_Integer i, j;
  gp_Pnt2d p2d;
  gp_Vec2d v2d; //, df1, df2;
  // gp_Pnt pts, ptgui;
  gp_Vec      dnplan, temp, temp1, temp2, tempmid; //, d1u, d1v, nplan;
  math_Vector XX(1, 4);                            // x1(1,2), x2(1,2);
  // math_Matrix d1(1,2,1,2), d2(1,2,1,2);

  csurf->D1(X(1), p2d, v2d);
  // corde1.SetParam(X(2));
  // corde2.SetParam(X(2));
  param = X(2);
  curv->D2(param, ptgui, d1gui, d2gui);
  normtg = d1gui.Magnitude();
  nplan  = d1gui.Normalized();
  theD   = -(nplan.XYZ().Dot(ptgui.XYZ()));

  dnplan.SetLinearForm(1. / normtg, d2gui, -1. / normtg * (nplan.Dot(d2gui)), nplan);

  temp1.SetXYZ(pts1.XYZ() - ptgui.XYZ());
  temp2.SetXYZ(pts2.XYZ() - ptgui.XYZ());
  tempmid.SetXYZ((pts1.XYZ() + pts2.XYZ()) / 2 - ptgui.XYZ());

  // x1(1) = p2d.X(); x1(2) = p2d.Y();
  // x2(1) = X(3); x2(2) = X(4);
  if (first)
  {
    XX(1) = p2d.X();
    XX(2) = p2d.Y();
    XX(3) = X(3);
    XX(4) = X(4);
  }
  else
  {
    XX(1) = X(3);
    XX(2) = X(4);
    XX(3) = p2d.X();
    XX(4) = p2d.Y();
  }

  surf1->D1(XX(1), XX(2), pts1, d1u1, d1v1);
  surf2->D1(XX(3), XX(4), pts2, d1u2, d1v2);

  if (first)
  {
    // p2d = pts est sur surf1
    // ptgui = corde1.PointOnGuide();
    // nplan = corde1.NPlan();
    temp.SetLinearForm(v2d.X(), d1u1, v2d.Y(), d1v1);

    D(1, 1) = nplan.Dot(temp);
    D(2, 1) = 0.;
    D(3, 1) = gp_Vec(ptgui, pts1).Dot(temp);
    D(4, 1) = 2 * (gp_Vec(ptgui, pts1).Dot(temp));

    D(1, 3) = 0.;
    D(1, 4) = 0.;
    D(2, 3) = nplan.Dot(d1u2);
    D(2, 4) = nplan.Dot(d1v2);
    D(3, 3) = gp_Vec((pts1.XYZ() + pts2.XYZ()) / 2 - ptgui.XYZ()).Dot(d1u2);
    D(3, 4) = gp_Vec((pts1.XYZ() + pts2.XYZ()) / 2 - ptgui.XYZ()).Dot(d1v2);
    D(4, 3) = -2. * gp_Vec(ptgui, pts2).Dot(d1u2);
    D(4, 4) = -2. * gp_Vec(ptgui, pts2).Dot(d1v2);

    // surf1->D1(x1(1),x1(2),pts,d1u,d1v);
  }
  else
  {
    //  p2d = pts est sur surf2
    // ptgui = corde2.PointOnGuide();
    // nplan = corde2.NPlan();
    temp.SetLinearForm(v2d.X(), d1u2, v2d.Y(), d1v2);

    D(1, 1) = 0.;
    D(2, 1) = nplan.Dot(temp);
    D(3, 1) = gp_Vec(ptgui, pts2).Dot(temp);
    D(4, 1) = -2 * (gp_Vec(ptgui, pts2).Dot(temp));

    D(1, 3) = nplan.Dot(d1u1);
    D(1, 4) = nplan.Dot(d1v1);
    D(2, 3) = 0.;
    D(2, 4) = 0.;
    D(3, 3) = gp_Vec((pts1.XYZ() + pts2.XYZ()) / 2 - ptgui.XYZ()).Dot(d1u1);
    D(3, 4) = gp_Vec((pts1.XYZ() + pts2.XYZ()) / 2 - ptgui.XYZ()).Dot(d1v1);
    D(4, 3) = 2. * gp_Vec(ptgui, pts1).Dot(d1u1);
    D(4, 4) = 2. * gp_Vec(ptgui, pts1).Dot(d1v1);

    // surf2->D1(x1(1),x1(2),pts,d1u,d1v);
  }

  D(1, 2) = dnplan.Dot(temp1) - nplan.Dot(d1gui);
  D(2, 2) = dnplan.Dot(temp2) - nplan.Dot(d1gui);
  D(3, 2) = -2. * d1gui.Dot(tempmid);
  D(4, 2) = 2. * d1gui.Dot(temp1) - 2. * d1gui.Dot(temp2);

  return Standard_True;
}
