// Created on: 1993-10-06
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomFill_BezierCurves.hxx>
#include <GeomFill_Coons.hxx>
#include <GeomFill_Curved.hxx>
#include <GeomFill_Filling.hxx>
#include <GeomFill_Stretch.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

//=======================================================================
//function : SetSameWeights
//purpose  : Internal Use Only
//           This function uses the following property of Rational 
//           BezierCurves
//              if Wi = Weight(i); Pi = Pole(i); n = NbPoles
//              with any a,b,c != 0,      
//                                             i    n-i
//              The transformation : Wi = a * b  * c    doesn't modify 
//              the geometry of the curve.
//              Only the length of the derivatives are changed.
//=======================================================================
void SetSameWeights(TColStd_Array1OfReal& W1, 
		    TColStd_Array1OfReal& W2, 
		    TColStd_Array1OfReal& W3, 
		    TColStd_Array1OfReal& W4 ) 
{
  Standard_Real Eps = Precision::Confusion();

  Standard_Integer NU = W1.Length();
  Standard_Integer NV = W2.Length();

  Standard_Real A = ( W1( 1) * W2( 1)) / ( W1( NU) * W2( NV));
  Standard_Real B = ( W3( 1) * W4( 1)) / ( W3( NU) * W4( NV));

  Standard_Integer i;
  Standard_Real Alfa = W1( NU) / W2( 1);
  for ( i=1; i<=NV; i++) {
    W2(i) *= Alfa;
  }
  Standard_Real Beta = W2( NV) / W3( NU);
  for ( i=1; i<=NU; i++) {
    W3(i) *= Beta;
  }
  Standard_Real Gamma = W3( 1) / W4( NV);
  for ( i=1; i<=NV; i++) {
    W4(i) *= Gamma;
  }
  
  if ( Abs(A-B) > Eps) {
    Standard_Real w = Pow( W1(1)/W4(1), 1./(Standard_Real)(NV-1));
    Standard_Real x = w;
    for ( i=NV-1; i>=1; i--) {
      W4(i) *= x;
      x *= w;
    }
  }
}


//=======================================================================
//function : Arrange
//purpose  : Internal Use Only
//           This function is used to prepare the Filling: The Curves
//           are arranged in this way:
//
//                      CC3
//                  ----->-----
//                 |           |
//                 |           |
//                 |           |
//             CC4 ^           ^ CC2
//                 |           |
//                 |           |
//                  ----->-----
//                   CC1 = C1
//=======================================================================

Standard_Boolean Arrange(const Handle(Geom_BezierCurve)& C1,
			 const Handle(Geom_BezierCurve)& C2,
			 const Handle(Geom_BezierCurve)& C3,
			 const Handle(Geom_BezierCurve)& C4,
			       Handle(Geom_BezierCurve)& CC1,
			       Handle(Geom_BezierCurve)& CC2,
			       Handle(Geom_BezierCurve)& CC3,
			       Handle(Geom_BezierCurve)& CC4,
			 const Standard_Real Tol             )
{
  Handle(Geom_BezierCurve) GC[4];
  Handle(Geom_BezierCurve) Dummy;
  GC[0] = Handle(Geom_BezierCurve)::DownCast(C1->Copy());
  GC[1] = Handle(Geom_BezierCurve)::DownCast(C2->Copy());
  GC[2] = Handle(Geom_BezierCurve)::DownCast(C3->Copy());
  GC[3] = Handle(Geom_BezierCurve)::DownCast(C4->Copy());
  
  Standard_Integer i,j;
  Standard_Boolean Trouve;

  for (i=1; i<=3; i++) {
    Trouve = Standard_False;
    for ( j=i; j<=3 && !Trouve; j++) {
      if (GC[j]->StartPoint().Distance( GC[i-1]->EndPoint()) < Tol) {
	Dummy  = GC[i];
	GC[i]  = GC[j];
	GC[j]  = Dummy;
	Trouve = Standard_True;
      }
      else if (GC[j]->EndPoint().Distance( GC[i-1]->EndPoint()) < Tol) {
	GC[j]  = Handle(Geom_BezierCurve)::DownCast(GC[j]->Reversed());
	Dummy  = GC[i];
	GC[i]  = GC[j];
	GC[j]  = Dummy;
	Trouve = Standard_True;
      }
    }
    if (!Trouve) return Standard_False;
  }

  CC1 = GC[0];
  CC2 = GC[1];
  CC3 = Handle(Geom_BezierCurve)::DownCast( GC[2]->Reversed());
  CC4 = Handle(Geom_BezierCurve)::DownCast( GC[3]->Reversed());

  return Standard_True;
}


//=======================================================================
//function : GeomFill_BezierCurves
//purpose  : 
//=======================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves()
{
}


//=======================================================================
//function : GeomFill_BezierCurves
//purpose  : 
//=======================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves(const Handle(Geom_BezierCurve)& C1, 
				       const Handle(Geom_BezierCurve)& C2, 
				       const Handle(Geom_BezierCurve)& C3, 
				       const Handle(Geom_BezierCurve)& C4, 
				       const GeomFill_FillingStyle Type      )
{
  Init( C1, C2, C3, C4, Type);
}


//=======================================================================
//function : GeomFill_BezierCurves
//purpose  : 
//=======================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves(const Handle(Geom_BezierCurve)& C1, 
				       const Handle(Geom_BezierCurve)& C2, 
				       const Handle(Geom_BezierCurve)& C3, 
				       const GeomFill_FillingStyle Type      )
{
  Init( C1, C2, C3, Type);
}


//=======================================================================
//function : GeomFill_BezierCurves
//purpose  : 
//=======================================================================

GeomFill_BezierCurves::GeomFill_BezierCurves(const Handle(Geom_BezierCurve)& C1, 
				       const Handle(Geom_BezierCurve)& C2,
				       const GeomFill_FillingStyle Type    )
{
  Init( C1, C2, Type);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  GeomFill_BezierCurves::Init(const Handle(Geom_BezierCurve)& C1, 
			       const Handle(Geom_BezierCurve)& C2, 
			       const Handle(Geom_BezierCurve)& C3, 
			       const Handle(Geom_BezierCurve)& C4, 
			       const GeomFill_FillingStyle Type      )
{
  // On ordonne les courbes
  Handle(Geom_BezierCurve) CC1, CC2, CC3, CC4;
  
  Standard_Real Tol = Precision::Confusion();
#ifndef No_Exception
  Standard_Boolean IsOK =
#endif
    Arrange( C1, C2, C3, C4, CC1, CC2, CC3, CC4, Tol);
  
  Standard_ConstructionError_Raise_if 
    (!IsOK, " GeomFill_BezierCurves: Courbes non jointives");

  // Mise en conformite des degres
  Standard_Integer DegU = Max( CC1->Degree(), CC3->Degree());
  Standard_Integer DegV = Max( CC2->Degree(), CC4->Degree());

  if (Type == GeomFill_CoonsStyle) {
    DegU = Max( DegU, 3);
    DegV = Max( DegV, 3);
  }

  if ( CC1->Degree() < DegU )  CC1->Increase(DegU);
  if ( CC2->Degree() < DegV )  CC2->Increase(DegV);
  if ( CC3->Degree() < DegU )  CC3->Increase(DegU);
  if ( CC4->Degree() < DegV )  CC4->Increase(DegV);

  TColgp_Array1OfPnt P1(1,DegU+1);
  TColgp_Array1OfPnt P3(1,DegU+1);
  TColgp_Array1OfPnt P2(1,DegV+1);
  TColgp_Array1OfPnt P4(1,DegV+1);
  CC1->Poles(P1);
  CC2->Poles(P2);
  CC3->Poles(P3);
  CC4->Poles(P4);
 
  // Traitement des courbes rationelles
  Standard_Boolean isRat = ( CC1->IsRational() || CC2->IsRational() ||
			     CC3->IsRational() || CC4->IsRational()   );

  TColStd_Array1OfReal W1(1,DegU+1);
  TColStd_Array1OfReal W3(1,DegU+1);
  TColStd_Array1OfReal W2(1,DegV+1);
  TColStd_Array1OfReal W4(1,DegV+1);
  W1.Init(1.);
  W2.Init(1.);
  W3.Init(1.);
  W4.Init(1.);

  if ( isRat) {
    if (CC1->IsRational()) {
      CC1->Weights(W1);
    }
    if (CC2->IsRational()) {
      CC2->Weights(W2);
    }
    if (CC3->IsRational()) {
      CC3->Weights(W3);
    }
    if (CC4->IsRational()) {
      CC4->Weights(W4);
    }
  }
  
  GeomFill_Filling Caro;
  if (isRat) {
    // Mise en conformite des poids aux coins.
    SetSameWeights( W1, W2, W3, W4);
    switch (Type)
      {
      case GeomFill_StretchStyle :
	Caro = GeomFill_Stretch( P1, P2, P3, P4, W1, W2, W3, W4); 
	break;
      case GeomFill_CoonsStyle   :
	Caro = GeomFill_Coons  ( P1, P4, P3, P2, W1, W4, W3, W2); 
	break;
      case GeomFill_CurvedStyle  :
	Caro = GeomFill_Curved ( P1, P2, P3, P4, W1, W2, W3, W4); 
	break;
      }
  }
  else {
    switch (Type) 
      {
      case GeomFill_StretchStyle :
	Caro = GeomFill_Stretch( P1, P2, P3, P4); 
	break;
      case GeomFill_CoonsStyle   :
	Caro = GeomFill_Coons  ( P1, P4, P3, P2); 
	break;
      case GeomFill_CurvedStyle  :
	Caro = GeomFill_Curved ( P1, P2, P3, P4); 
	break;
      }
  }
  
  Standard_Integer NbUPoles = Caro.NbUPoles();
  Standard_Integer NbVPoles = Caro.NbVPoles();
  TColgp_Array2OfPnt Poles(1,NbUPoles,1,NbVPoles);
  
  Caro.Poles(Poles);
  
  if (Caro.isRational()) {
    TColStd_Array2OfReal Weights(1,NbUPoles, 1,NbVPoles);
    Caro.Weights(Weights);
    mySurface = new Geom_BezierSurface(Poles,Weights);
  }
  else {
    mySurface = new Geom_BezierSurface(Poles);
  }
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  GeomFill_BezierCurves::Init(const Handle(Geom_BezierCurve)& C1, 
			       const Handle(Geom_BezierCurve)& C2, 
			       const Handle(Geom_BezierCurve)& C3, 
			       const GeomFill_FillingStyle Type      )
{
  Handle(Geom_BezierCurve) C4;
  TColgp_Array1OfPnt Poles(1,2);
  Standard_Real Tol = Precision::Confusion();
  Tol = Tol * Tol; 
  if(C1->StartPoint().SquareDistance(C2->StartPoint()) > Tol &&
     C1->StartPoint().SquareDistance(C2->EndPoint()) > Tol )
    Poles( 1) = C1->StartPoint();
  else 
    Poles( 1) = C1->EndPoint();

  if(C3->StartPoint().SquareDistance(C2->StartPoint()) > Tol &&
     C3->StartPoint().SquareDistance(C2->EndPoint()) > Tol )
    Poles( 2) = C3->StartPoint();
  else 
    Poles( 2) = C3->EndPoint();
//  Poles(1) = C1->StartPoint();
//  Poles(2) = C1->StartPoint();
  C4 = new Geom_BezierCurve(Poles);
  Init( C1, C2, C3, C4, Type);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  GeomFill_BezierCurves::Init(const Handle(Geom_BezierCurve)& C1, 
			       const Handle(Geom_BezierCurve)& C2,
			       const GeomFill_FillingStyle Type     )
{
  Handle(Geom_BezierCurve) 
    CC1 = Handle(Geom_BezierCurve)::DownCast(C1->Copy());
  Handle(Geom_BezierCurve) 
    CC2 = Handle(Geom_BezierCurve)::DownCast(C2->Copy());

  Standard_Integer Deg1 = CC1->Degree();
  Standard_Integer Deg2 = CC2->Degree();

  Standard_Boolean isRat = ( CC1->IsRational() || CC2->IsRational());
    
  if ( Type != GeomFill_CurvedStyle) {
    Standard_Integer DegU = Max( Deg1, Deg2);
    
    if ( CC1->Degree() < DegU )  CC1->Increase(DegU);
    if ( CC2->Degree() < DegU )  CC2->Increase(DegU);
    
    TColgp_Array2OfPnt Poles( 1, DegU+1, 1, 2);
    TColgp_Array1OfPnt P1(1,DegU+1);
    TColgp_Array1OfPnt P2(1,DegU+1);
    CC1->Poles(P1);
    CC2->Poles(P2);
    
    Standard_Integer i;
    for (i=1; i<=DegU+1; i++) {
      Poles(i, 1) = P1(i);
      Poles(i, 2) = P2(i);
    }
    if (isRat) {
      TColStd_Array1OfReal W1(1,DegU+1);
      TColStd_Array1OfReal W2(1,DegU+1);
      W1.Init(1.);
      W2.Init(1.);

      if (CC1->IsRational()) {
	CC1->Weights(W1);
      }
      if (CC2->IsRational()) {
	CC2->Weights(W2);
      }
      TColStd_Array2OfReal Weights(1,DegU+1, 1,2);
      for ( i=1; i<=DegU+1; i++) {
	Weights(i, 1) = W1(i);
	Weights(i, 2) = W2(i);
      }
      mySurface = new Geom_BezierSurface(Poles,Weights);
    }
    else {
      mySurface = new Geom_BezierSurface(Poles);
    }
  }
  else {
    TColgp_Array1OfPnt P1(1,Deg1+1);
    TColgp_Array1OfPnt P2(1,Deg2+1);
    
    Standard_Real Eps = Precision::Confusion();
    Standard_Boolean IsOK = Standard_False;
    if ( CC1->StartPoint().IsEqual(CC2->StartPoint(),Eps)) {
      IsOK = Standard_True;
    }
    else if ( CC1->StartPoint().IsEqual(CC2->EndPoint(),Eps)) {
      CC2->Reverse();
      IsOK = Standard_True;
    }
    else if ( CC1->EndPoint().IsEqual(CC2->StartPoint(),Eps)) {
      C1->Reverse();
      IsOK = Standard_True;
    }
    else if ( CC1->EndPoint().IsEqual(CC2->EndPoint(),Eps)) {
      CC1->Reverse();
      CC2->Reverse();
      IsOK = Standard_True;
    }

    if(!IsOK)
      throw Standard_OutOfRange("GeomFill_BezierCurves: Courbes non jointives");

    CC1->Poles(P1);
    CC2->Poles(P2);

    TColStd_Array1OfReal W1(1,Deg1+1);
    TColStd_Array1OfReal W2(1,Deg2+1);
    W1.Init(1.);
    W2.Init(1.);

    GeomFill_Filling Caro;
    if ( isRat) {
      if (CC1->IsRational()) {
	CC1->Weights(W1);
      }
      if (CC2->IsRational()) {
	CC2->Weights(W2);
      }
      Caro = GeomFill_Curved( P1, P2, W1, W2);
    }
    else {
      Caro = GeomFill_Curved( P1, P2);
    }
    
    Standard_Integer NbUPoles = Caro.NbUPoles();
    Standard_Integer NbVPoles = Caro.NbVPoles();
    TColgp_Array2OfPnt Poles(1,NbUPoles,1,NbVPoles);
    
    Caro.Poles(Poles);
    
    if (Caro.isRational()) {
      TColStd_Array2OfReal Weights(1,NbUPoles, 1,NbVPoles);
      Caro.Weights(Weights);
      mySurface = new Geom_BezierSurface(Poles,Weights);
    }
    else {
      mySurface = new Geom_BezierSurface(Poles);
    }
  }
}
