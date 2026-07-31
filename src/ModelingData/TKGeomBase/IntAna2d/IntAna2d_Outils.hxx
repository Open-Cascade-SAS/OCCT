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

//============================================================================
//======================================================= IntAna2d_Outils.hxx
//============================================================================
#ifndef IntAna2d_Outils_HeaderFile
#define IntAna2d_Outils_HeaderFile

#include <math_TrigonometricFunctionRoots.hxx>
#include <IntAna2d_IntPoint.hxx>

class MyDirectPolynomialRoots
{
public:
  MyDirectPolynomialRoots(const double A4,
                          const double A3,
                          const double A2,
                          const double A1,
                          const double A0);

  MyDirectPolynomialRoots(const double A2, const double A1, const double A0);

  int NbSolutions() const { return (nbsol); }

  double Value(const int i) const { return (sol[i - 1]); }

  double IsDone() const { return (nbsol > -1); }

  bool InfiniteRoots() const { return (same); }

private:
  double sol[16];
  double val[16];
  int    nbsol;
  bool   same;
};

bool Points_Confondus(const double xa, const double ya, const double xb, const double yb);

void Traitement_Points_Confondus(int& nb_pts, IntAna2d_IntPoint* pts);

void Coord_Ancien_Repere(double& Ancien_X, double& Ancien_Y, const gp_Ax2d& Axe_Nouveau_Repere);

#endif
