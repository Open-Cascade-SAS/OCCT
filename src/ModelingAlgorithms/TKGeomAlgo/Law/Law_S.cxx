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

// pmn -> modified 17/01/1996 : utilisation de Curve() et SetCurve()

#include <Law_BSpline.hxx>
#include <Law_S.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_S, Law_BSpFunc)

Law_S::Law_S() {}

void Law_S::Set(const double Pdeb, const double Valdeb, const double Pfin, const double Valfin)
{
  Set(Pdeb, Valdeb, 0., Pfin, Valfin, 0.);
}

void Law_S::Set(const double Pdeb,
                const double Valdeb,
                const double Ddeb,
                const double Pfin,
                const double Valfin,
                const double Dfin)
{
  NCollection_Array1<double> poles(1, 4);
  NCollection_Array1<double> knots(1, 2);
  NCollection_Array1<int>    mults(1, 2);
  poles(1)   = Valdeb;
  poles(4)   = Valfin;
  double coe = (Pfin - Pdeb) / 3.;
  poles(2)   = Valdeb + coe * Ddeb;
  poles(3)   = Valfin - coe * Dfin;
  knots(1)   = Pdeb;
  knots(2)   = Pfin;
  mults(1) = mults(2) = 4;

  SetCurve(new Law_BSpline(poles, knots, mults, 3));
}
