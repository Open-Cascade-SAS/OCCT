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


#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomFill_Coons.hxx>
#include <GeomFill_Curved.hxx>
#include <GeomFill_Filling.hxx>
#include <GeomFill_Stretch.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

//=======================================================================
//function : Arrange
//purpose  : Internal Use Only
//           This function is used to prepare the Filling: The Curves
//           are arranged in this way:
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
Standard_Boolean Arrange(const Handle(Geom_BSplineCurve)& C1,
			 const Handle(Geom_BSplineCurve)& C2,
			 const Handle(Geom_BSplineCurve)& C3,
			 const Handle(Geom_BSplineCurve)& C4,
			       Handle(Geom_BSplineCurve)& CC1,
			       Handle(Geom_BSplineCurve)& CC2,
			       Handle(Geom_BSplineCurve)& CC3,
			       Handle(Geom_BSplineCurve)& CC4,
			 const Standard_Real Tol             )
{
  Handle(Geom_BSplineCurve) GC[4];
  Handle(Geom_BSplineCurve) Dummy;
  GC[0] = Handle(Geom_BSplineCurve)::DownCast(C1->Copy());
  GC[1] = Handle(Geom_BSplineCurve)::DownCast(C2->Copy());
  GC[2] = Handle(Geom_BSplineCurve)::DownCast(C3->Copy());
  GC[3] = Handle(Geom_BSplineCurve)::DownCast(C4->Copy());
  
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
	GC[j]  = Handle(Geom_BSplineCurve)::DownCast(GC[j]->Reversed());
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
  CC3 = Handle(Geom_BSplineCurve)::DownCast( GC[2]->Reversed());
  CC4 = Handle(Geom_BSplineCurve)::DownCast( GC[3]->Reversed());

  return Standard_True;
}



//=======================================================================
//function : SetSameDistribution
//purpose  : Internal Use Only
//=======================================================================

Standard_Integer SetSameDistribution(Handle(Geom_BSplineCurve)& C1,
				     Handle(Geom_BSplineCurve)& C2 )
{
  Standard_Integer nbp1 = C1->NbPoles();
  Standard_Integer nbk1 = C1->NbKnots();
  TColgp_Array1OfPnt      P1(1,nbp1);
  TColStd_Array1OfReal    W1(1,nbp1);
  W1.Init(1.);
  TColStd_Array1OfReal    K1(1,nbk1);
  TColStd_Array1OfInteger M1(1,nbk1);
  
  C1->Poles(P1);
  if( C1->IsRational()) 
    C1->Weights(W1);
  C1->Knots(K1);
  C1->Multiplicities(M1);
  
  Standard_Integer nbp2 = C2->NbPoles();
  Standard_Integer nbk2 = C2->NbKnots();
  TColgp_Array1OfPnt      P2(1,nbp2);
  TColStd_Array1OfReal    W2(1,nbp2);
  W2.Init(1.);
  TColStd_Array1OfReal    K2(1,nbk2);
  TColStd_Array1OfInteger M2(1,nbk2);
  
  C2->Poles(P2);
  if( C2->IsRational()) 
    C2->Weights(W2);
  C2->Knots(K2);
  C2->Multiplicities(M2);
  
  Standard_Real K11 = K1( 1  );
  Standard_Real K12 = K1(nbk1);
  Standard_Real K21 = K2( 1  );
  Standard_Real K22 = K2(nbk2);
  
  if ( (K12-K11) > (K22-K21)) {
    BSplCLib::Reparametrize( K11, K12, K2);
    C2->SetKnots(K2);
  }
  else if ( (K12-K11) < (K22-K21)) {
    BSplCLib::Reparametrize( K21, K22, K1);
    C1->SetKnots(K1);
  }
  else if(Abs(K12-K11) > Precision::PConfusion()) {
    BSplCLib::Reparametrize( K11, K12, K2);
    C2->SetKnots(K2);
  }    
  
  Standard_Integer NP,NK;
  if ( BSplCLib::PrepareInsertKnots(C1->Degree(),Standard_False,
				    K1,M1,K2,&M2,NP,NK,Precision::PConfusion(),
				    Standard_False)) {
    TColgp_Array1OfPnt      NewP(1, NP);
    TColStd_Array1OfReal    NewW(1, NP);
    TColStd_Array1OfReal    NewK(1, NK);
    TColStd_Array1OfInteger NewM(1, NK);
    BSplCLib::InsertKnots(C1->Degree(),Standard_False,
			  P1,&W1,K1,M1,K2,&M2,
			  NewP,&NewW,NewK,NewM,Precision::PConfusion(),
			  Standard_False);
    if ( C1->IsRational()) {
      C1 = new Geom_BSplineCurve(NewP,NewW,NewK,NewM,C1->Degree());
    }
    else {
      C1 = new Geom_BSplineCurve(NewP,NewK,NewM,C1->Degree());
    }
    BSplCLib::InsertKnots(C2->Degree(),Standard_False,
			  P2,&W2,K2,M2,K1,&M1,
			  NewP,&NewW,NewK,NewM,Precision::PConfusion(),
			  Standard_False);
    if ( C2->IsRational()) {
      C2 = new Geom_BSplineCurve(NewP,NewW,NewK,NewM,C2->Degree());
    }
    else {
      C2 = new Geom_BSplineCurve(NewP,NewK,NewM,C2->Degree());
    }
  }
  else {
    throw Standard_ConstructionError(" ");
  }
  
  return C1->NbPoles();
}


//=======================================================================
//function : GeomFill_BSplineCurves
//purpose  : 
//=======================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves()
{
}


//=======================================================================
//function : GeomFill_BSplineCurves
//purpose  : 
//=======================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves
  (const Handle(Geom_BSplineCurve)& C1, 
   const Handle(Geom_BSplineCurve)& C2, 
   const Handle(Geom_BSplineCurve)& C3, 
   const Handle(Geom_BSplineCurve)& C4, 
   const GeomFill_FillingStyle Type      )
{
  Init( C1, C2, C3, C4, Type);
}


//=======================================================================
//function : GeomFill_BSplineCurves
//purpose  : 
//=======================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves
  (const Handle(Geom_BSplineCurve)& C1, 
   const Handle(Geom_BSplineCurve)& C2, 
   const Handle(Geom_BSplineCurve)& C3, 
   const GeomFill_FillingStyle Type      )
{
  Init( C1, C2, C3, Type);
}


//=======================================================================
//function : GeomFill_BSplineCurves
//purpose  : 
//=======================================================================

GeomFill_BSplineCurves::GeomFill_BSplineCurves
  (const Handle(Geom_BSplineCurve)& C1, 
   const Handle(Geom_BSplineCurve)& C2,
   const GeomFill_FillingStyle Type      )
{
  Init( C1, C2, Type);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  GeomFill_BSplineCurves::Init
  (const Handle(Geom_BSplineCurve)& C1, 
   const Handle(Geom_BSplineCurve)& C2, 
   const Handle(Geom_BSplineCurve)& C3, 
   const Handle(Geom_BSplineCurve)& C4, 
   const GeomFill_FillingStyle Type      )
{
  // On ordonne les courbes
  Handle(Geom_BSplineCurve) CC1, CC2, CC3, CC4;
  
  Standard_Real Tol = Precision::Confusion();
#ifndef No_Exception
  Standard_Boolean IsOK =
#endif
    Arrange( C1, C2, C3, C4, CC1, CC2, CC3, CC4, Tol);
  
  Standard_ConstructionError_Raise_if 
    (!IsOK, " GeomFill_BSplineCurves: Courbes non jointives");

  // Mise en conformite des degres
  Standard_Integer Deg1 = CC1->Degree();
  Standard_Integer Deg2 = CC2->Degree();
  Standard_Integer Deg3 = CC3->Degree();
  Standard_Integer Deg4 = CC4->Degree();
  Standard_Integer DegU = Max( Deg1, Deg3);
  Standard_Integer DegV = Max( Deg2, Deg4);
  if ( Deg1 < DegU) CC1->IncreaseDegree(DegU);
  if ( Deg2 < DegV) CC2->IncreaseDegree(DegV);
  if ( Deg3 < DegU) CC3->IncreaseDegree(DegU);
  if ( Deg4 < DegV) CC4->IncreaseDegree(DegV);

  // Mise en conformite des distributions de noeuds
  Standard_Integer NbUPoles = SetSameDistribution(CC1,CC3);
  Standard_Integer NbVPoles = SetSameDistribution(CC2,CC4);

  if(Type == GeomFill_CoonsStyle) {
    if(NbUPoles < 4 || NbVPoles < 4)
      throw Standard_ConstructionError("GeomFill_BSplineCurves: invalid filling style");
  }

  TColgp_Array1OfPnt P1(1,NbUPoles);
  TColgp_Array1OfPnt P2(1,NbVPoles);
  TColgp_Array1OfPnt P3(1,NbUPoles);
  TColgp_Array1OfPnt P4(1,NbVPoles);
  CC1->Poles(P1);
  CC2->Poles(P2);
  CC3->Poles(P3);
  CC4->Poles(P4);

  // Traitement des courbes rationelles
  Standard_Boolean isRat = ( CC1->IsRational() || CC2->IsRational() ||
			     CC3->IsRational() || CC4->IsRational()   );

  TColStd_Array1OfReal W1(1,NbUPoles);
  TColStd_Array1OfReal W3(1,NbUPoles);
  TColStd_Array1OfReal W2(1,NbVPoles);
  TColStd_Array1OfReal W4(1,NbVPoles);
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
  
  NbUPoles = Caro.NbUPoles();
  NbVPoles = Caro.NbVPoles();
  TColgp_Array2OfPnt Poles(1,NbUPoles,1,NbVPoles);
  
  
  // Creation de la surface
  Standard_Integer NbUKnot = CC1->NbKnots();
  TColStd_Array1OfReal    UKnots(1,NbUKnot);
  TColStd_Array1OfInteger UMults(1,NbUKnot);
  CC1->Knots(UKnots);
  CC1->Multiplicities(UMults);

  Standard_Integer NbVKnot = CC2->NbKnots();
  TColStd_Array1OfReal    VKnots(1,NbVKnot);
  TColStd_Array1OfInteger VMults(1,NbVKnot);
  CC2->Knots(VKnots);
  CC2->Multiplicities(VMults);

  Caro.Poles(Poles);
  
  if (Caro.isRational()) {
    TColStd_Array2OfReal Weights(1,NbUPoles, 1,NbVPoles);
    Caro.Weights(Weights);
    mySurface = new Geom_BSplineSurface(Poles        , Weights,
					UKnots       , VKnots,
					UMults       , VMults,
					CC1->Degree(), CC2->Degree());
  }
  else {
    mySurface = new Geom_BSplineSurface(Poles        ,
					UKnots       , VKnots,
					UMults       , VMults,
					CC1->Degree(), CC2->Degree());
  }
  
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  GeomFill_BSplineCurves::Init
  (const Handle(Geom_BSplineCurve)& C1, 
   const Handle(Geom_BSplineCurve)& C2, 
   const Handle(Geom_BSplineCurve)& C3, 
   const GeomFill_FillingStyle Type      )
{
  Handle(Geom_BSplineCurve) C4;
  TColgp_Array1OfPnt      Poles(1,2);
  TColStd_Array1OfReal    Knots(1,2);
  TColStd_Array1OfInteger Mults(1,2);
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

  Knots( 1) = C2->Knot(C2->FirstUKnotIndex());
  Knots( 2) = C2->Knot(C2->LastUKnotIndex());
  Mults( 1) = Mults( 2) = 2;
  C4 = new Geom_BSplineCurve( Poles, Knots, Mults, 1);
  Init( C1, C2, C3, C4, Type);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  GeomFill_BSplineCurves::Init
  (const Handle(Geom_BSplineCurve)& C1, 
   const Handle(Geom_BSplineCurve)& C2,
   const GeomFill_FillingStyle Type      ) 
{
  Handle(Geom_BSplineCurve) 
    CC1 = Handle(Geom_BSplineCurve)::DownCast(C1->Copy());
  Handle(Geom_BSplineCurve) 
    CC2 = Handle(Geom_BSplineCurve)::DownCast(C2->Copy());
  
  Standard_Integer Deg1 = CC1->Degree();
  Standard_Integer Deg2 = CC2->Degree();

  Standard_Boolean isRat = ( CC1->IsRational() || CC2->IsRational());

  if ( Type != GeomFill_CurvedStyle) {
    Standard_Integer DegU = Max( Deg1, Deg2);
    
    if ( CC1->Degree() < DegU )  CC1->IncreaseDegree(DegU);
    if ( CC2->Degree() < DegU )  CC2->IncreaseDegree(DegU);
    
    // Mise en conformite des distributions de noeuds
    Standard_Integer NbPoles = SetSameDistribution(CC1,CC2);
    TColgp_Array2OfPnt Poles(1,NbPoles, 1,2);
    TColgp_Array1OfPnt P1( 1, NbPoles);
    TColgp_Array1OfPnt P2( 1, NbPoles);
    CC1->Poles(P1);
    CC2->Poles(P2);
    Standard_Integer i;
    for (i=1; i<=NbPoles; i++) {
      Poles(i, 1) = P1(i);
      Poles(i, 2) = P2(i);
    }
    Standard_Integer NbUKnots = CC1->NbKnots();
    TColStd_Array1OfReal UKnots( 1, NbUKnots);
    TColStd_Array1OfInteger UMults( 1, NbUKnots);
    CC1->Knots(UKnots);
    CC1->Multiplicities(UMults);
//    Standard_Integer NbVKnots = 2;
    TColStd_Array1OfReal VKnots( 1, 2);
    TColStd_Array1OfInteger VMults( 1, 2);
    VKnots( 1) = 0;
    VKnots( 2) = 1;
    VMults( 1) = 2;
    VMults( 2) = 2;
    
    
    // Traitement des courbes rationelles
    if (isRat) {
      TColStd_Array2OfReal Weights(1,NbPoles, 1,2);
      TColStd_Array1OfReal W1(1,NbPoles);
      TColStd_Array1OfReal W2(1,NbPoles);
      W1.Init(1.);
      W2.Init(1.);

      if ( isRat) {
	if (CC1->IsRational()) {
	  CC1->Weights(W1);
	}
	if (CC2->IsRational()) {
	  CC2->Weights(W2);
	}
	for (i=1; i<=NbPoles; i++) {
	  Weights(i, 1) = W1( i);
	  Weights(i, 2) = W2( i);
	}
      }
      mySurface = new Geom_BSplineSurface(Poles        , Weights,
					  UKnots       , VKnots, 
					  UMults       , VMults,
					  CC1->Degree(), 1,
					  CC1->IsPeriodic(), 
					  Standard_False);
    }
    else {
      mySurface = new Geom_BSplineSurface(Poles        ,
					  UKnots       , VKnots, 
					  UMults       , VMults,
					  CC1->Degree(), 1);
    }
  }
  else {
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
      throw Standard_OutOfRange("GeomFill_BSplineCurves: Courbes non jointives");

    Standard_Integer NbUPoles = CC1->NbPoles();
    Standard_Integer NbVPoles = CC2->NbPoles();
    TColgp_Array1OfPnt P1(1,NbUPoles);
    TColgp_Array1OfPnt P2(1,NbVPoles);
    CC1->Poles(P1);
    CC2->Poles(P2);

    Standard_Integer NbUKnots = CC1->NbKnots();
    Standard_Integer NbVKnots = CC2->NbKnots();
    TColStd_Array1OfReal UKnots(1,NbUKnots);
    TColStd_Array1OfReal VKnots(1,NbVKnots);
    TColStd_Array1OfInteger UMults(1,NbUKnots);
    TColStd_Array1OfInteger VMults(1,NbVKnots);
    CC1->Knots(UKnots);
    CC1->Multiplicities(UMults);
    CC2->Knots(VKnots);
    CC2->Multiplicities(VMults);

    TColStd_Array1OfReal W1(1,NbUPoles);
    TColStd_Array1OfReal W2(1,NbVPoles);
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

    NbUPoles = Caro.NbUPoles();
    NbVPoles = Caro.NbVPoles();
    TColgp_Array2OfPnt Poles(1,NbUPoles,1,NbVPoles);
    
    Caro.Poles(Poles);
    
    if (Caro.isRational()) {
      TColStd_Array2OfReal Weights(1,NbUPoles, 1,NbVPoles);
      Caro.Weights(Weights);
      mySurface = new Geom_BSplineSurface(Poles         , Weights,
					  UKnots        , VKnots,
					  UMults        , VMults,
					  Deg1          , Deg2,
					  Standard_False, Standard_False);
    }
    else {
      mySurface = new Geom_BSplineSurface(Poles         ,
					  UKnots        , VKnots,
					  UMults        , VMults,
					  Deg1          , Deg2,
					  Standard_False, Standard_False);
    }
  }
}


