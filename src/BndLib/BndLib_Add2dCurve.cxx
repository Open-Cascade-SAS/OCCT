// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BndLib_Add2dCurve.ixx>
#include <BndLib.hxx>
#include <ElCLib.hxx>
#include <GeomAbs_CurveType.hxx>

#include <TColgp_Array1OfPnt2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_Add2dCurve::Add(const Adaptor2d_Curve2d& C,
			    const Standard_Real   Tol,
			          Bnd_Box2d&      B ) 
{
  BndLib_Add2dCurve::Add(C,
			 C.FirstParameter(),
			 C.LastParameter (),
			 Tol,B);
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_Add2dCurve::Add( const Adaptor2d_Curve2d& C,
			     const Standard_Real U1,
			     const Standard_Real U2,
			     const Standard_Real Tol,
			     Bnd_Box2d& B ) 
{
  GeomAbs_CurveType Type = C.GetType();

  switch (Type) {
    
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
      Handle(Geom2d_BezierCurve) Bz = C.Bezier();
      if(Abs(Bz->FirstParameter() - U1) > Precision::Parametric(Tol)||
	 Abs(Bz->LastParameter()  - U2) > Precision::Parametric(Tol)) {

	Handle(Geom2d_Geometry) G = Bz->Copy();
	
	const  Handle(Geom2d_BezierCurve)& Bzaux = (*((Handle(Geom2d_BezierCurve)*)&G));
	  
	Bzaux->Segment(U1, U2);
	for (Standard_Integer i = 1; i <= Bzaux->NbPoles(); i++) {
	  B.Add(Bzaux->Pole(i));
	}

      }

      else {

	for (Standard_Integer i = 1;i<=Bz->NbPoles();i++) {
	  B.Add(Bz->Pole(i));
	}
      }
      B.Enlarge(Tol);
      break;
    }
  case GeomAbs_BSplineCurve:  
    {
      Handle(Geom2d_BSplineCurve) Bs = C.BSpline();
      if(Abs(Bs->FirstParameter() - U1) > Precision::Parametric(Tol)||
	 Abs(Bs->LastParameter()  - U2) > Precision::Parametric(Tol)) {


	Handle(Geom2d_Geometry) G = Bs->Copy();
	const Handle(Geom2d_BSplineCurve)& Bsaux = (*((Handle(Geom2d_BSplineCurve)*)&G));
	Standard_Real u1 = U1, u2 = U2;
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
	for (Standard_Integer i = 1; i <= Bsaux->NbPoles(); i++) {
	  B.Add(Bsaux->Pole(i));
	}

      }

      else {

	for (Standard_Integer i = 1;i<=Bs->NbPoles();i++) {
	  B.Add(Bs->Pole(i));
	}

      }
      B.Enlarge(Tol);
      break;
    }
  default:
    {
      Standard_Real N = 33;
      gp_Pnt2d P;
      Standard_Real U  = U1;
      Standard_Real DU = (U2-U1)/(N-1);
      for (Standard_Integer j=1;j<N;j++) {
	C.D0(U,P);
	U+=DU;
	B.Add(P);
      }
      C.D0(U2,P);
      B.Add(P);
      B.Enlarge(Tol);
    }
  }
}



