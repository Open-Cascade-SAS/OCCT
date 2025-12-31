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

//-- IntAna_IntLinTorus.cxx
//-- lbr : la methode avec les coefficients est catastrophique.
//--       Mise en place d'une vraie solution.

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <IntAna_IntLinTorus.hxx>
#include <math_DirectPolynomialRoots.hxx>

IntAna_IntLinTorus::IntAna_IntLinTorus()
    : done(false),
      nbpt(0)
{
  memset(theFi, 0, sizeof(theFi));
  memset(theParam, 0, sizeof(theParam));
  memset(theTheta, 0, sizeof(theTheta));
}

IntAna_IntLinTorus::IntAna_IntLinTorus(const gp_Lin& L, const gp_Torus& T)
{
  Perform(L, T);
}

void IntAna_IntLinTorus::Perform(const gp_Lin& L, const gp_Torus& T)
{
  gp_Pnt PL = L.Location();
  gp_Dir DL = L.Direction();

  // Reparametrize the line:
  // set its location as nearest to the location of torus
  gp_Pnt TorLoc       = T.Location();
  double ParamOfNewPL = gp_Vec(PL, TorLoc).Dot(gp_Vec(DL));
  gp_Pnt NewPL(PL.XYZ() + ParamOfNewPL * DL.XYZ());

  //--------------------------------------------------------------
  //-- Coefficients de la ligne dans le repere du cone
  //--
  gp_Trsf trsf;
  trsf.SetTransformation(T.Position());
  NewPL.Transform(trsf);
  DL.Transform(trsf);

  double a, b, c, x1, y1, z1, x0, y0, z0;
  double a0, a1, a2, a3, a4;
  double R, r, R2, r2;

  x1 = DL.X();
  y1 = DL.Y();
  z1 = DL.Z();
  x0 = NewPL.X();
  y0 = NewPL.Y();
  z0 = NewPL.Z();
  R  = T.MajorRadius();
  R2 = R * R;
  r  = T.MinorRadius();
  r2 = r * r;

  a = x1 * x1 + y1 * y1 + z1 * z1;
  b = 2.0 * (x1 * x0 + y1 * y0 + z1 * z0);
  c = x0 * x0 + y0 * y0 + z0 * z0 - (R2 + r2);

  a4 = a * a;
  a3 = 2.0 * a * b;
  a2 = 2.0 * a * c + 4.0 * R2 * z1 * z1 + b * b;
  a1 = 2.0 * b * c + 8.0 * R2 * z1 * z0;
  a0 = c * c + 4.0 * R2 * (z0 * z0 - r2);

  double                     u, v;
  math_DirectPolynomialRoots mdpr(a4, a3, a2, a1, a0);
  if (mdpr.IsDone())
  {
    int nbsolvalid = 0;
    int n          = mdpr.NbSolutions();
    int aNbBadSol  = 0;
    for (int i = 1; i <= n; i++)
    {
      double t = mdpr.Value(i);
      t += ParamOfNewPL;
      gp_Pnt PSolL(ElCLib::Value(t, L));
      ElSLib::Parameters(T, PSolL, u, v);
      gp_Pnt PSolT(ElSLib::Value(u, v, T));
      a0 = PSolT.SquareDistance(PSolL);

      if (a0 > 0.0000000001)
      {
        aNbBadSol++;
      }
      else
      {
        theParam[nbsolvalid] = t;
        theFi[nbsolvalid]    = u;
        theTheta[nbsolvalid] = v;
        thePoint[nbsolvalid] = PSolL;
        nbsolvalid++;
      }
    }
    if (n > 0 && nbsolvalid == 0 && aNbBadSol == n)
    {
      nbpt = 0;
      done = false;
    }
    else
    {
      nbpt = nbsolvalid;
      done = true;
    }
  }
  else
  {
    nbpt = 0;
    done = false;
  }
}
