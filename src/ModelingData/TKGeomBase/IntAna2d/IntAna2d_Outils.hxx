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
  MyDirectPolynomialRoots(const Standard_Real A4,
                          const Standard_Real A3,
                          const Standard_Real A2,
                          const Standard_Real A1,
                          const Standard_Real A0);

  MyDirectPolynomialRoots(const Standard_Real A2, const Standard_Real A1, const Standard_Real A0);

  Standard_Integer NbSolutions() const { return (nbsol); }

  Standard_Real Value(const Standard_Integer i) const { return (sol[i - 1]); }

  Standard_Real IsDone() const { return (nbsol > -1); }

  Standard_Boolean InfiniteRoots() const { return (same); }

private:
  Standard_Real    sol[16];
  Standard_Real    val[16];
  Standard_Integer nbsol;
  Standard_Boolean same;
};

Standard_Boolean Points_Confondus(const Standard_Real xa,
                                  const Standard_Real ya,
                                  const Standard_Real xb,
                                  const Standard_Real yb);

void Traitement_Points_Confondus(Standard_Integer& nb_pts, IntAna2d_IntPoint* pts);

void Coord_Ancien_Repere(Standard_Real& Ancien_X,
                         Standard_Real& Ancien_Y,
                         const gp_Ax2d& Axe_Nouveau_Repere);

#endif
