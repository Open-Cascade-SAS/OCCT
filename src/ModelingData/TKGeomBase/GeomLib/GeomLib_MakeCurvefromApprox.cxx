// Created on: 1995-06-13
// Created by: Bruno DUMORTIER
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

#include <AdvApprox_ApproxAFunction.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomLib_MakeCurvefromApprox.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

GeomLib_MakeCurvefromApprox::GeomLib_MakeCurvefromApprox(const AdvApprox_ApproxAFunction& Approx)
    : myApprox(Approx)
{
}

//=================================================================================================

int GeomLib_MakeCurvefromApprox::Nb1DSpaces() const
{
  return myApprox.NumSubSpaces(1);
}

//=================================================================================================

int GeomLib_MakeCurvefromApprox::Nb2DSpaces() const
{
  return myApprox.NumSubSpaces(2);
}

//=================================================================================================

int GeomLib_MakeCurvefromApprox::Nb3DSpaces() const
{
  return myApprox.NumSubSpaces(3);
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> GeomLib_MakeCurvefromApprox::Curve2d(const int Index2d) const
{
  Standard_OutOfRange_Raise_if(Index2d < 0 || Index2d > Nb2DSpaces(),
                               " GeomLib_MakeCurvefromApprox : Curve2d");
  StdFail_NotDone_Raise_if(!IsDone(), " GeomLib_MakeCurvefromApprox : Curve2d");

  NCollection_Array1<gp_Pnt2d> Poles(1, myApprox.NbPoles());
  NCollection_Array1<double>   Knots(1, myApprox.NbKnots());
  NCollection_Array1<int>      Mults(1, myApprox.NbKnots());

  myApprox.Poles2d(Index2d, Poles);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();

  occ::handle<Geom2d_BSplineCurve> C =
    new Geom2d_BSplineCurve(Poles, Knots, Mults, myApprox.Degree());

  return C;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> GeomLib_MakeCurvefromApprox::Curve2d(const int Index1d,
                                                                      const int Index2d) const
{
  Standard_OutOfRange_Raise_if(Index1d < 0 || Index1d > Nb1DSpaces() || Index2d < 0
                                 || Index2d > Nb2DSpaces(),
                               " GeomLib_MakeCurvefromApprox : Curve2d");
  StdFail_NotDone_Raise_if(!IsDone() && !myApprox.HasResult(),
                           " GeomLib_MakeCurvefromApprox : Curve2d");

  NCollection_Array1<gp_Pnt2d> Poles(1, myApprox.NbPoles());
  NCollection_Array1<double>   Weigths(1, myApprox.NbPoles());
  NCollection_Array1<double>   Knots(1, myApprox.NbKnots());
  NCollection_Array1<int>      Mults(1, myApprox.NbKnots());

  myApprox.Poles2d(Index2d, Poles);
  myApprox.Poles1d(Index1d, Weigths);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();

  double X, Y, W;
  for (int i = 1; i <= myApprox.NbPoles(); i++)
  {
    W = Weigths(i);
    Poles(i).Coord(X, Y);
    Poles(i).SetCoord(X / W, Y / W);
  }

  occ::handle<Geom2d_BSplineCurve> C =
    new Geom2d_BSplineCurve(Poles, Knots, Mults, myApprox.Degree());

  return C;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> GeomLib_MakeCurvefromApprox::Curve2dFromTwo1d(
  const int Index1d,
  const int Index2d) const
{
  Standard_OutOfRange_Raise_if(Index1d < 0 || Index1d > Nb1DSpaces() || Index2d < 0
                                 || Index2d > Nb1DSpaces(),
                               " GeomLib_MakeCurvefromApprox : Curve2d");
  StdFail_NotDone_Raise_if(!IsDone() && !myApprox.HasResult(),
                           " GeomLib_MakeCurvefromApprox : Curve2d");

  NCollection_Array1<gp_Pnt2d> Poles(1, myApprox.NbPoles());
  NCollection_Array1<double>   Poles1d1(1, myApprox.NbPoles());
  NCollection_Array1<double>   Poles1d2(1, myApprox.NbPoles());
  NCollection_Array1<double>   Knots(1, myApprox.NbKnots());
  NCollection_Array1<int>      Mults(1, myApprox.NbKnots());

  myApprox.Poles1d(Index2d, Poles1d2);
  myApprox.Poles1d(Index1d, Poles1d1);

  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();

  //  double X,Y,W;
  for (int i = 1; i <= myApprox.NbPoles(); i++)
  {
    Poles(i).SetCoord(Poles1d1.Value(i), Poles1d2.Value(i));
  }

  occ::handle<Geom2d_BSplineCurve> C =
    new Geom2d_BSplineCurve(Poles, Knots, Mults, myApprox.Degree());

  return C;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomLib_MakeCurvefromApprox::Curve(const int Index3d) const
{
  Standard_OutOfRange_Raise_if(Index3d < 0 || Index3d > Nb3DSpaces(),
                               " GeomLib_MakeCurvefromApprox : Curve");
  StdFail_NotDone_Raise_if(!IsDone() && !myApprox.HasResult(),
                           " GeomLib_MakeCurvefromApprox : Curve");

  NCollection_Array1<gp_Pnt> Poles(1, myApprox.NbPoles());
  NCollection_Array1<double> Knots(1, myApprox.NbKnots());
  NCollection_Array1<int>    Mults(1, myApprox.NbKnots());

  myApprox.Poles(Index3d, Poles);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();

  occ::handle<Geom_BSplineCurve> C = new Geom_BSplineCurve(Poles, Knots, Mults, myApprox.Degree());

  return C;
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomLib_MakeCurvefromApprox::Curve(const int Index1d,
                                                                  const int Index3d) const
{
  Standard_OutOfRange_Raise_if(Index1d < 0 || Index1d > Nb1DSpaces() || Index3d < 0
                                 || Index3d > Nb3DSpaces(),
                               " GeomLib_MakeCurvefromApprox : Curve3d");
  StdFail_NotDone_Raise_if(!IsDone(), " GeomLib_MakeCurvefromApprox : Curve3d");

  NCollection_Array1<gp_Pnt> Poles(1, myApprox.NbPoles());
  NCollection_Array1<double> Weigths(1, myApprox.NbPoles());
  NCollection_Array1<double> Knots(1, myApprox.NbKnots());
  NCollection_Array1<int>    Mults(1, myApprox.NbKnots());

  myApprox.Poles(Index3d, Poles);
  myApprox.Poles1d(Index1d, Weigths);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();

  double X, Y, Z, W;
  for (int i = 1; i <= myApprox.NbPoles(); i++)
  {
    W = Weigths(i);
    Poles(i).Coord(X, Y, Z);
    Poles(i).SetCoord(X / W, Y / W, Z / W);
  }

  occ::handle<Geom_BSplineCurve> C = new Geom_BSplineCurve(Poles, Knots, Mults, myApprox.Degree());

  return C;
}
