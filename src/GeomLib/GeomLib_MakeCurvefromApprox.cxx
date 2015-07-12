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
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : GeomLib_MakeCurvefromApprox
//purpose  : 
//=======================================================================
GeomLib_MakeCurvefromApprox::GeomLib_MakeCurvefromApprox
(const AdvApprox_ApproxAFunction& Approx)
:myApprox(Approx)
{
}


//=======================================================================
//function : Nb1DSpaces
//purpose  : 
//=======================================================================

Standard_Integer GeomLib_MakeCurvefromApprox::Nb1DSpaces() const 
{
  return myApprox.NumSubSpaces(1);
}


//=======================================================================
//function : Nb2DSpaces
//purpose  : 
//=======================================================================

Standard_Integer GeomLib_MakeCurvefromApprox::Nb2DSpaces() const 
{
  return myApprox.NumSubSpaces(2);
}


//=======================================================================
//function : Nb3DSpaces
//purpose  : 
//=======================================================================

Standard_Integer GeomLib_MakeCurvefromApprox::Nb3DSpaces() const 
{
  return myApprox.NumSubSpaces(3);
}


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) GeomLib_MakeCurvefromApprox::Curve2d
(const Standard_Integer Index2d) const 
{
  Standard_OutOfRange_Raise_if
    ( Index2d < 0 || Index2d > Nb2DSpaces(),
     " GeomLib_MakeCurvefromApprox : Curve2d");
  StdFail_NotDone_Raise_if
    ( !IsDone(),
     " GeomLib_MakeCurvefromApprox : Curve2d");

  TColgp_Array1OfPnt2d    Poles( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Knots( 1, myApprox.NbKnots());
  TColStd_Array1OfInteger Mults( 1, myApprox.NbKnots());
  
  myApprox.Poles2d(Index2d, Poles);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();
  
  Handle(Geom2d_BSplineCurve) C = 
    new Geom2d_BSplineCurve( Poles, Knots, Mults, myApprox.Degree());

  return C;
}


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) GeomLib_MakeCurvefromApprox::Curve2d
(const Standard_Integer Index1d,
 const Standard_Integer Index2d) const 
{
  Standard_OutOfRange_Raise_if
    ( Index1d < 0 || Index1d > Nb1DSpaces() ||
      Index2d < 0 || Index2d > Nb2DSpaces(),
     " GeomLib_MakeCurvefromApprox : Curve2d");
  StdFail_NotDone_Raise_if
    ( !IsDone() && ! myApprox.HasResult(),
     " GeomLib_MakeCurvefromApprox : Curve2d");

  TColgp_Array1OfPnt2d    Poles  ( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Weigths( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Knots  ( 1, myApprox.NbKnots());
  TColStd_Array1OfInteger Mults  ( 1, myApprox.NbKnots());
  
  myApprox.Poles2d(Index2d, Poles);
  myApprox.Poles1d(Index1d, Weigths);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();
  
  Standard_Real X,Y,W;
  for ( Standard_Integer i = 1; i <= myApprox.NbPoles(); i++) {
    W = Weigths(i);
    Poles(i).Coord(X,Y);
    Poles(i).SetCoord(X/W, Y/W);
  }

  Handle(Geom2d_BSplineCurve) C = 
    new Geom2d_BSplineCurve( Poles, Knots, Mults, myApprox.Degree());

  return C;
}
//=======================================================================
//function : Curve2dFromTwo1d
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) GeomLib_MakeCurvefromApprox::Curve2dFromTwo1d
(const Standard_Integer Index1d,
 const Standard_Integer Index2d) const 
{
  Standard_OutOfRange_Raise_if
    ( Index1d < 0 || Index1d > Nb1DSpaces() ||
      Index2d < 0 || Index2d > Nb1DSpaces(),
     " GeomLib_MakeCurvefromApprox : Curve2d");
  StdFail_NotDone_Raise_if
    ( !IsDone() && ! myApprox.HasResult(),
     " GeomLib_MakeCurvefromApprox : Curve2d");

  TColgp_Array1OfPnt2d    Poles  ( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Poles1d1( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Poles1d2( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Knots  ( 1, myApprox.NbKnots());
  TColStd_Array1OfInteger Mults  ( 1, myApprox.NbKnots());
  
  myApprox.Poles1d(Index2d, Poles1d2);
  myApprox.Poles1d(Index1d, Poles1d1);
 
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();
  
//  Standard_Real X,Y,W;
  for ( Standard_Integer i = 1; i <= myApprox.NbPoles(); i++) {
    Poles(i).SetCoord(Poles1d1.Value(i),Poles1d2.Value(i));
  }

  Handle(Geom2d_BSplineCurve) C = 
    new Geom2d_BSplineCurve( Poles, Knots, Mults, myApprox.Degree());

  return C;
}



//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) GeomLib_MakeCurvefromApprox::Curve
(const Standard_Integer Index3d) const 
{
  Standard_OutOfRange_Raise_if
    ( Index3d < 0 || Index3d > Nb3DSpaces(),
     " GeomLib_MakeCurvefromApprox : Curve");
  StdFail_NotDone_Raise_if
    ( !IsDone() && ! myApprox.HasResult(),
     " GeomLib_MakeCurvefromApprox : Curve");

  TColgp_Array1OfPnt      Poles( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Knots( 1, myApprox.NbKnots());
  TColStd_Array1OfInteger Mults( 1, myApprox.NbKnots());
  
  myApprox.Poles(Index3d, Poles);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();
  
  Handle(Geom_BSplineCurve) C = 
    new Geom_BSplineCurve( Poles, Knots, Mults, myApprox.Degree());

  return C;
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) GeomLib_MakeCurvefromApprox::Curve
(const Standard_Integer Index1d,
 const Standard_Integer Index3d) const 
{
  Standard_OutOfRange_Raise_if
    ( Index1d < 0 || Index1d > Nb1DSpaces() ||
      Index3d < 0 || Index3d > Nb3DSpaces(),
     " GeomLib_MakeCurvefromApprox : Curve3d");
  StdFail_NotDone_Raise_if
    ( !IsDone(),
     " GeomLib_MakeCurvefromApprox : Curve3d");

  TColgp_Array1OfPnt      Poles  ( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Weigths( 1, myApprox.NbPoles());
  TColStd_Array1OfReal    Knots  ( 1, myApprox.NbKnots());
  TColStd_Array1OfInteger Mults  ( 1, myApprox.NbKnots());
  
  myApprox.Poles  (Index3d, Poles);
  myApprox.Poles1d(Index1d, Weigths);
  Knots = myApprox.Knots()->Array1();
  Mults = myApprox.Multiplicities()->Array1();
  
  Standard_Real X,Y,Z,W;
  for ( Standard_Integer i = 1; i <= myApprox.NbPoles(); i++) {
    W = Weigths(i);
    Poles(i).Coord(X,Y,Z);
    Poles(i).SetCoord(X/W, Y/W, Z/W);
  }

  Handle(Geom_BSplineCurve) C = 
    new Geom_BSplineCurve( Poles, Knots, Mults, myApprox.Degree());

  return C;
}
