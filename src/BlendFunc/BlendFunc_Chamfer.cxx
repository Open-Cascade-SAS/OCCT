// Created on: 1996-06-05
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

// Modified : 20/08/96 PMN Ajout des methodes (Nb)Intervals et IsRationnal
// Modified : 30/12/96 PMN Ajout GetMinimalWeight, GetSectionSize;

#include <BlendFunc_Chamfer.hxx>
#include <ElCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <Standard_NotImplemented.hxx>

//=================================================================================================

BlendFunc_Chamfer::BlendFunc_Chamfer(const Handle(Adaptor3d_Surface)& S1,
                                     const Handle(Adaptor3d_Surface)& S2,
                                     const Handle(Adaptor3d_Curve)&   CG)
    : BlendFunc_GenChamfer(S1, S2, CG),
      corde1(S1, CG),
      corde2(S2, CG)
{
}

//=================================================================================================

void BlendFunc_Chamfer::Set(const Standard_Real    Dist1,
                            const Standard_Real    Dist2,
                            const Standard_Integer Choix)
{
  corde1.SetDist(Dist1);
  corde2.SetDist(Dist2);
  choix = Choix;
}

//=================================================================================================

void BlendFunc_Chamfer::Set(const Standard_Real Param)
{
  corde1.SetParam(Param);
  corde2.SetParam(Param);
}

//=================================================================================================

Standard_Boolean BlendFunc_Chamfer::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  math_Vector Sol1(1, 2), Sol2(1, 2);

  Sol1(1) = Sol(1);
  Sol1(2) = Sol(2);
  Sol2(1) = Sol(3);
  Sol2(2) = Sol(4);

  Standard_Boolean issol = corde1.IsSolution(Sol1, Tol);
  issol                  = issol && corde2.IsSolution(Sol2, Tol);
  tol                    = Tol;
  if (issol)
    distmin = Min(distmin, corde1.PointOnS().Distance(corde2.PointOnS()));

  return issol;
}

//=================================================================================================

Standard_Boolean BlendFunc_Chamfer::Value(const math_Vector& X, math_Vector& F)
{
  math_Vector x(1, 2), f(1, 2);

  x(1) = X(1);
  x(2) = X(2);
  corde1.Value(x, f);
  F(1) = f(1);
  F(2) = f(2);

  x(1) = X(3);
  x(2) = X(4);
  corde2.Value(x, f);
  F(3) = f(1);
  F(4) = f(2);

  return Standard_True;
}

//=================================================================================================

Standard_Boolean BlendFunc_Chamfer::Derivatives(const math_Vector& X, math_Matrix& D)
{
  Standard_Integer i, j;
  math_Vector      x(1, 2);
  math_Matrix      d(1, 2, 1, 2);

  x(1) = X(1);
  x(2) = X(2);
  corde1.Derivatives(x, d);
  for (i = 1; i < 3; i++)
  {
    for (j = 1; j < 3; j++)
    {
      D(i, j)     = d(i, j);
      D(i, j + 2) = 0.;
    }
  }

  x(1) = X(3);
  x(2) = X(4);
  corde2.Derivatives(x, d);
  for (i = 1; i < 3; i++)
  {
    for (j = 1; j < 3; j++)
    {
      D(i + 2, j + 2) = d(i, j);
      D(i + 2, j)     = 0.;
    }
  }

  return Standard_True;
}

//=================================================================================================

const gp_Pnt& BlendFunc_Chamfer::PointOnS1() const
{
  return corde1.PointOnS();
}

//=================================================================================================

const gp_Pnt& BlendFunc_Chamfer::PointOnS2() const
{
  return corde2.PointOnS();
}

//=================================================================================================

Standard_Boolean BlendFunc_Chamfer::IsTangencyPoint() const
{
  return corde1.IsTangencyPoint() && corde2.IsTangencyPoint();
}

//=================================================================================================

const gp_Vec& BlendFunc_Chamfer::TangentOnS1() const
{
  return corde1.TangentOnS();
}

//=================================================================================================

const gp_Vec& BlendFunc_Chamfer::TangentOnS2() const
{
  return corde2.TangentOnS();
}

//=================================================================================================

const gp_Vec2d& BlendFunc_Chamfer::Tangent2dOnS1() const
{
  return corde1.Tangent2dOnS();
}

//=================================================================================================

const gp_Vec2d& BlendFunc_Chamfer::Tangent2dOnS2() const
{
  return corde2.Tangent2dOnS();
}

//=======================================================================
// function : Tangent
// purpose  : TgF,NmF et TgL,NmL les tangentes et normales respectives
//           aux surfaces S1 et S2
//=======================================================================

void BlendFunc_Chamfer::Tangent(const Standard_Real U1,
                                const Standard_Real V1,
                                const Standard_Real U2,
                                const Standard_Real V2,
                                gp_Vec&             TgF,
                                gp_Vec&             TgL,
                                gp_Vec&             NmF,
                                gp_Vec&             NmL) const
{
  gp_Pnt           pt1, pt2, ptgui;
  gp_Vec           d1u1, d1v1, d1u2, d1v2;
  gp_Vec           nplan;
  Standard_Boolean revF = Standard_False;
  Standard_Boolean revL = Standard_False;

  ptgui = corde1.PointOnGuide();
  nplan = corde1.NPlan();
  surf1->D1(U1, V1, pt1, d1u1, d1v1);
  NmF = d1u1.Crossed(d1v1);

  surf2->D1(U2, V2, pt2, d1u2, d1v2);
  NmL = d1u2.Crossed(d1v2);

  TgF = (nplan.Crossed(NmF)).Normalized();
  TgL = (nplan.Crossed(NmL)).Normalized();

  if ((choix == 2) || (choix == 5))
  {
    revF = Standard_True;
    revL = Standard_True;
  }
  if ((choix == 4) || (choix == 7))
    revL = Standard_True;
  if ((choix == 3) || (choix == 8))
    revF = Standard_True;

  if (revF)
    TgF.Reverse();
  if (revL)
    TgL.Reverse();
}

//=======================================================================
// function : GetSectionSize
// purpose  : Non implementee (non necessaire car non rationel)
//=======================================================================
Standard_Real BlendFunc_Chamfer::GetSectionSize() const
{
  throw Standard_NotImplemented("BlendFunc_Chamfer::GetSectionSize()");
}
