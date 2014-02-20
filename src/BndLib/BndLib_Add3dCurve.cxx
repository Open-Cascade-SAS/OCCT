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

#include <BndLib_Add3dCurve.ixx>
#include <GeomAbs_CurveType.hxx>
#include <BndLib.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <gp_Pnt.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <gp_Hypr.hxx>
#include <Precision.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <ElCLib.hxx>

#include <gp_Pnt.hxx>
#include <GeomAdaptor_Curve.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_Add3dCurve::Add( const Adaptor3d_Curve& C,
			   const Standard_Real Tol,
			         Bnd_Box&      B )
{
  BndLib_Add3dCurve::Add(C,
			 C.FirstParameter(),
			 C.LastParameter (),
			 Tol,B);
}

//OCC566(apo)->
static Standard_Real FillBox(Bnd_Box& B, const Adaptor3d_Curve& C, 
			     const Standard_Real first, const Standard_Real last, 
			     const Standard_Integer N)
{
  gp_Pnt P1, P2, P3;
  C.D0(first,P1);  B.Add(P1);
  Standard_Real p = first, dp = last-first, tol= 0.;
  if(Abs(dp) > Precision::PConfusion()){
    Standard_Integer i;
    dp /= 2*N; 
    for(i = 1; i <= N; i++){
      p += dp;  C.D0(p,P2);  B.Add(P2);
      p += dp;  C.D0(p,P3);  B.Add(P3);
      gp_Pnt Pc((P1.XYZ()+P3.XYZ())/2.0);
      tol = Max(tol,Pc.Distance(P2));
      P1 = P3;
    }
  }else{
    C.D0(first,P1);  B.Add(P1);
    C.D0(last,P3);  B.Add(P3);
    tol = 0.;
  }
  return tol;
}
//<-OCC566(apo)
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_Add3dCurve::Add( const Adaptor3d_Curve& C,
			   const Standard_Real U1,
			   const Standard_Real U2,
			   const Standard_Real Tol,
			         Bnd_Box&      B )
{
  static Standard_Real weakness = 1.5;  //OCC566(apo)
  Standard_Real tol = 0.0;
  switch (C.GetType()) {

  case GeomAbs_Line: 
    {
      BndLib::Add(C.Line(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Circle: 
    {
      BndLib::Add(C.Circle(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Ellipse: 
    {
      BndLib::Add(C.Ellipse(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Hyperbola: 
    {
      BndLib::Add(C.Hyperbola(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_Parabola: 
    {
      BndLib::Add(C.Parabola(),U1,U2,Tol,B);
      break;
    }
  case GeomAbs_BezierCurve: 
    {
      Handle(Geom_BezierCurve) Bz = C.Bezier();
      //OCC566(apo)->
      Standard_Integer N = Bz->Degree();
      GeomAdaptor_Curve GACurve(Bz);
      Bnd_Box B1;
      tol = FillBox(B1,GACurve,U1,U2,N);
      B1.Enlarge(weakness*tol);
      Standard_Real x, y, z, X, Y, Z;
      B1.Get(x, y, z, X, Y, Z);
      B.Update(x, y, z, X, Y, Z);
      B.Enlarge(Tol);
      //<-OCC566(apo)
      break;
    }
  case GeomAbs_BSplineCurve: 
    {
      Handle(Geom_BSplineCurve) Bs = C.BSpline();
      if(Abs(Bs->FirstParameter() - U1) > Precision::Parametric(Tol)||
	 Abs(Bs->LastParameter()  - U2) > Precision::Parametric(Tol)) {

	Handle(Geom_Geometry) G = Bs->Copy();
	Handle(Geom_BSplineCurve)& Bsaux = (*((Handle(Geom_BSplineCurve)*)&G));
	Standard_Real u1 = U1, u2 = U2;
	//// modified by jgv, 24.10.01 for BUC61031 ////
	if (Bsaux->IsPeriodic())
	  ElCLib::AdjustPeriodic( Bsaux->FirstParameter(), Bsaux->LastParameter(), Precision::PConfusion(), u1, u2 );
	else {
	  ////////////////////////////////////////////////
	  //  modified by NIZHNY-EAP Fri Dec  3 14:29:14 1999 ___BEGIN___
	  // To avoid exeption in Segment
	  if(Bsaux->FirstParameter() > U1) u1 = Bsaux->FirstParameter();
	  if(Bsaux->LastParameter()  < U2 ) u2  = Bsaux->LastParameter();
	  //  modified by NIZHNY-EAP Fri Dec  3 14:29:18 1999 ___END___
	}
	Bsaux->Segment(u1, u2);
	Bs = Bsaux;
      }
      //OCC566(apo)->
      Bnd_Box B1;
      Standard_Integer k, k1 = Bs->FirstUKnotIndex(), k2 = Bs->LastUKnotIndex(),
                       N = Bs->Degree(), NbKnots = Bs->NbKnots();
      TColStd_Array1OfReal Knots(1,NbKnots);
      Bs->Knots(Knots);
      GeomAdaptor_Curve GACurve(Bs);
      Standard_Real first = Knots(k1), last;
      for(k = k1 + 1; k <= k2; k++){
	last = Knots(k); 
	tol = Max(FillBox(B1,GACurve,first,last,N), tol);
	first = last;
      }
      if (!B1.IsVoid())
      {
        B1.Enlarge(weakness*tol);
        Standard_Real x, y, z, X, Y, Z;
        B1.Get(x, y, z, X, Y, Z);
        B.Update(x, y, z, X, Y, Z);
        B.Enlarge(Tol);
      }
      //<-OCC566(apo)
      break;
    }
  default:
    {
      Bnd_Box B1;
      static Standard_Integer N = 33;
      tol = FillBox(B1,C,U1,U2,N);
      B1.Enlarge(weakness*tol);
      Standard_Real x, y, z, X, Y, Z;
      B1.Get(x, y, z, X, Y, Z);
      B.Update(x, y, z, X, Y, Z);
      B.Enlarge(Tol);
    }
  }
}
