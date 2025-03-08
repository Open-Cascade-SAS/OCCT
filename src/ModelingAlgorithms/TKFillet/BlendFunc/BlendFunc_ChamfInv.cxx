// Created on: 1996-06-06
// Created by: Stagiaire Xuan Trang PHAMPHU
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

#include <Adaptor2d_Curve2d.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ChamfInv.hxx>
#include <math_Matrix.hxx>

//=================================================================================================

BlendFunc_ChamfInv::BlendFunc_ChamfInv(const Handle(Adaptor3d_Surface)& S1,
                                       const Handle(Adaptor3d_Surface)& S2,
                                       const Handle(Adaptor3d_Curve)&   C)
    : BlendFunc_GenChamfInv(S1, S2, C),
      corde1(surf1, curv),
      corde2(surf2, curv)
{
}

//=================================================================================================

void BlendFunc_ChamfInv::Set(const Standard_Real    Dist1,
                             const Standard_Real    Dist2,
                             const Standard_Integer Choix)
{
  Standard_Real dis1, dis2;

  choix = Choix;
  switch (choix)
  {
    case 1:
    case 2: {
      dis1 = -Dist1;
      dis2 = -Dist2;
    }
    break;
    case 3:
    case 4: {
      dis1 = Dist1;
      dis2 = -Dist2;
    }
    break;
    case 5:
    case 6: {
      dis1 = Dist1;
      dis2 = Dist2;
    }
    break;
    case 7:
    case 8: {
      dis1 = -Dist1;
      dis2 = Dist2;
    }
    break;
    default:
      dis1 = -Dist1;
      dis2 = -Dist2;
  }
  corde1.SetDist(dis1);
  corde2.SetDist(dis2);
}

//=================================================================================================

Standard_Boolean BlendFunc_ChamfInv::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  gp_Pnt2d p2d;
  gp_Vec2d v2d;
  csurf->D1(Sol(1), p2d, v2d);

  math_Vector      Sol1(1, 2), Sol2(1, 2);
  Standard_Boolean issol;

  Sol1(1) = p2d.X();
  Sol1(2) = p2d.Y();

  Sol2(1) = Sol(3);
  Sol2(2) = Sol(4);

  if (first)
  {
    issol = corde1.IsSolution(Sol1, Tol);
    issol = issol && corde2.IsSolution(Sol2, Tol);
  }
  else
  {
    issol = corde1.IsSolution(Sol2, Tol);
    issol = issol && corde2.IsSolution(Sol1, Tol);
  }

  return issol;
}

//=================================================================================================

Standard_Boolean BlendFunc_ChamfInv::Value(const math_Vector& X, math_Vector& F)
{
  gp_Pnt2d p2d;
  gp_Vec2d v2d;
  csurf->D1(X(1), p2d, v2d);
  corde1.SetParam(X(2));
  corde2.SetParam(X(2));

  math_Vector x1(1, 2), f1(1, 2), x2(1, 2), f2(1, 2);
  x1(1) = p2d.X();
  x1(2) = p2d.Y();
  x2(1) = X(3);
  x2(2) = X(4);

  if (first)
  {
    corde1.Value(x1, f1);
    corde2.Value(x2, f2);
  }

  else
  {
    corde1.Value(x2, f1);
    corde2.Value(x1, f2);
  }
  F(1) = f1(1);
  F(2) = f1(2);
  F(3) = f2(1);
  F(4) = f2(2);

  return Standard_True;
}

//=================================================================================================

Standard_Boolean BlendFunc_ChamfInv::Derivatives(const math_Vector& X, math_Matrix& D)
{
  Standard_Integer i, j;
  gp_Pnt2d         p2d;
  gp_Vec2d         v2d, df1, df2;
  gp_Pnt           pts, ptgui;
  gp_Vec           temp, d1u, d1v, nplan;
  math_Vector      x1(1, 2), x2(1, 2);
  math_Matrix      d1(1, 2, 1, 2), d2(1, 2, 1, 2);

  csurf->D1(X(1), p2d, v2d);
  corde1.SetParam(X(2));
  corde2.SetParam(X(2));

  x1(1) = p2d.X();
  x1(2) = p2d.Y();
  x2(1) = X(3);
  x2(2) = X(4);

  if (first)
  {
    // p2d = pts est sur surf1
    ptgui = corde1.PointOnGuide();
    nplan = corde1.NPlan();
    corde2.Derivatives(x2, d2);
    corde1.DerFguide(x1, df1);
    corde2.DerFguide(x2, df2);
    surf1->D1(x1(1), x1(2), pts, d1u, d1v);
  }
  else
  {
    //  p2d = pts est sur surf2
    ptgui = corde2.PointOnGuide();
    nplan = corde2.NPlan();
    corde1.Derivatives(x2, d1);
    corde1.DerFguide(x2, df1);
    corde2.DerFguide(x1, df2);
    surf2->D1(x1(1), x1(2), pts, d1u, d1v);
  }

  // derivees par rapport a T
  temp.SetLinearForm(v2d.X(), d1u, v2d.Y(), d1v);
  if (first)
  {
    D(1, 1) = nplan.Dot(temp);
    D(2, 1) = 2 * (gp_Vec(ptgui, pts).Dot(temp));
    D(3, 1) = 0.;
    D(4, 1) = 0.;
  }
  else
  {
    D(1, 1) = 0.;
    D(2, 1) = 0.;
    D(3, 1) = nplan.Dot(temp);
    D(4, 1) = 2 * (gp_Vec(ptgui, pts).Dot(temp));
  }

  // derivees par rapport a W
  D(1, 2) = df1.X();
  D(2, 2) = df1.Y();
  D(3, 2) = df2.X();
  D(4, 2) = df2.Y();

  // derivees par rapport a U et V
  if (first)
  {
    for (i = 1; i < 3; i++)
    {
      for (j = 3; j < 5; j++)
      {
        D(i, j)     = 0.;
        D(i + 2, j) = d2(i, j - 2);
      }
    }
  }
  else
  {
    for (i = 1; i < 3; i++)
    {
      for (j = 3; j < 5; j++)
      {
        D(i, j)     = d1(i, j - 2);
        D(i + 2, j) = 0.;
      }
    }
  }

  return Standard_True;
}
