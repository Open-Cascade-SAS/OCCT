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
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include "BndLib_LegacyCheck.hxx"
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomBndLib_Curve2d.hxx>
#include <gp_Pnt2d.hxx>

//=================================================================================================

void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& aC, const double aTol, Bnd_Box2d& aBox2D)
{
  BndLib_Add2dCurve::Add(aC, aC.FirstParameter(), aC.LastParameter(), aTol, aBox2D);
}

//=================================================================================================

void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& aC,
                            const double             aU1,
                            const double             aU2,
                            const double             aTol,
                            Bnd_Box2d&               aBox2D)
{
  const Geom2dAdaptor_Curve* pA = dynamic_cast<const Geom2dAdaptor_Curve*>(&aC);
  if (!pA)
  {
    double   U, DU;
    int      N, j;
    gp_Pnt2d P;
    N  = 33;
    U  = aU1;
    DU = (aU2 - aU1) / (N - 1);
    for (j = 1; j < N; j++)
    {
      aC.D0(U, P);
      U += DU;
      aBox2D.Add(P);
    }
    aC.D0(aU2, P);
    aBox2D.Add(P);
    aBox2D.Enlarge(aTol);
    return;
  }
  //
  const occ::handle<Geom2d_Curve>& aC2D = pA->Curve();
  //
  BndLib_Add2dCurve::Add(aC2D, aU1, aU2, aTol, aBox2D);
}

//=================================================================================================

void BndLib_Add2dCurve::Add(const occ::handle<Geom2d_Curve>& aC2D,
                            const double                     aTol,
                            Bnd_Box2d&                       aBox2D)
{
  const double aT1 = aC2D->FirstParameter();
  const double aT2 = aC2D->LastParameter();
  BndLib_Add2dCurve::Add(aC2D, aT1, aT2, aTol, aBox2D);
}

//=================================================================================================

void BndLib_Add2dCurve::Add(const occ::handle<Geom2d_Curve>& aC2D,
                            const double                     aT1,
                            const double                     aT2,
                            const double                     aTol,
                            Bnd_Box2d&                       aBox2D)
{
  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aC2D).Add(aT1, aT2, aTol, aNewBox);
  BndLib_LegacyCheck::Compare2dCurveAdd(aC2D, aT1, aT2, aTol, aNewBox);
  aBox2D.Add(aNewBox);
}

//=================================================================================================

void BndLib_Add2dCurve::AddOptimal(const occ::handle<Geom2d_Curve>& aC2D,
                                   const double                     aT1,
                                   const double                     aT2,
                                   const double                     aTol,
                                   Bnd_Box2d&                       aBox2D)
{
  Bnd_Box2d aNewBox;
  GeomBndLib_Curve2d(aC2D).AddOptimal(aT1, aT2, aTol, aNewBox);
  BndLib_LegacyCheck::Compare2dCurveAddOptimal(aC2D, aT1, aT2, aTol, aNewBox);
  aBox2D.Add(aNewBox);
}
