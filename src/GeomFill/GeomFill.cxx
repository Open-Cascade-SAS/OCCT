// Created on: 1994-02-25
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill.hxx>
#include <GeomFill_Generator.hxx>
#include <GeomFill_PolynomialConvertor.hxx>
#include <GeomFill_QuasiAngularConvertor.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>

//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
Handle(Geom_Surface) GeomFill::Surface
  (const Handle(Geom_Curve)& Curve1, 
   const Handle(Geom_Curve)& Curve2)

{
  Handle(Geom_Curve) TheCurve1, TheCurve2;
  Handle(Geom_Surface) Surf;
  
  // recherche du type de la surface resultat:
  // les surfaces reglees particulieres sont :
  //     - les plans
  //     - les cylindres
  //     - les cones
  // dans ces trois cas les courbes doivent etre de meme type :
  //     - ou 2 droites
  //     - ou 2 cercles
  
  
  Standard_Real a1=0, a2=0, b1=0, b2=0;
  Standard_Boolean Trim1= Standard_False, Trim2 = Standard_False;
  if ( Curve1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) Ctrim 
      = Handle(Geom_TrimmedCurve)::DownCast(Curve1);
    TheCurve1 = Ctrim->BasisCurve();
    a1 = Ctrim->FirstParameter();
    b1 = Ctrim->LastParameter();
    Trim1 = Standard_True;
  }
  else {
    TheCurve1 = Handle(Geom_Curve)::DownCast(Curve1->Copy());
  }
  if ( Curve2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) Ctrim 
      = Handle(Geom_TrimmedCurve)::DownCast(Curve2);
    TheCurve2 = Ctrim->BasisCurve();
    a2 = Ctrim->FirstParameter();
    b2 = Ctrim->LastParameter();
    Trim2 = Standard_True;
  }
  else {
    TheCurve2 = Handle(Geom_Curve)::DownCast(Curve2->Copy());
  }

  Standard_Boolean IsDone = Standard_False;
  // Les deux courbes sont des droites.
  if ( TheCurve1->IsKind(STANDARD_TYPE(Geom_Line)) &&
       TheCurve2->IsKind(STANDARD_TYPE(Geom_Line)) &&
       Trim1 && Trim2                     ) {

    gp_Lin L1 = (Handle(Geom_Line)::DownCast(TheCurve1))->Lin();
    gp_Lin L2 = (Handle(Geom_Line)::DownCast(TheCurve2))->Lin();
    gp_Dir D1 = L1.Direction();
    gp_Dir D2 = L2.Direction();
    
    if ( D1.IsParallel(D2, Precision::Angular())) {
      gp_Vec P1P2(L1.Location(),L2.Location()); 
      Standard_Real proj = P1P2.Dot(D1);

      if ( D1.IsEqual(D2, Precision::Angular())) {
	if ( Abs( a1 - proj - a2 ) <= Precision::Confusion() &&
	     Abs( b1 - proj - b2 ) <= Precision::Confusion()    ) {
	  gp_Ax3 Ax(L1.Location(), gp_Dir(D1.Crossed(P1P2)),D1);
	  Handle(Geom_Plane) P = new Geom_Plane(Ax);
	  Standard_Real V = P1P2.Dot( Ax.YDirection());
	  Surf = new Geom_RectangularTrimmedSurface( P , a1, b1,
						    Min(0.,V),Max(0.,V));
	  IsDone = Standard_True;
	}
      }
      if ( D1.IsOpposite(D2, Precision::Angular())) {
	if ( Abs( a1 - proj + b2 ) <= Precision::Confusion() &&
	     Abs( b1 - proj + a2 ) <= Precision::Confusion()    ) {
	  gp_Ax3 Ax(L1.Location(), gp_Dir(D1.Crossed(P1P2)),D1);
	  Handle(Geom_Plane) P = new Geom_Plane(Ax);
	  Standard_Real V = P1P2.Dot( Ax.YDirection());
	  Surf = new Geom_RectangularTrimmedSurface( P , a1, b1,
						    Min(0.,V),Max(0.,V));
	  IsDone = Standard_True;
	}
      }
    }
  }
  
  
  // Les deux courbes sont des cercles.
  else if ( TheCurve1->IsKind(STANDARD_TYPE(Geom_Circle)) &&
	    TheCurve2->IsKind(STANDARD_TYPE(Geom_Circle))   ) {

    gp_Circ C1 = (Handle(Geom_Circle)::DownCast(TheCurve1))->Circ();
    gp_Circ C2 = (Handle(Geom_Circle)::DownCast(TheCurve2))->Circ();

    gp_Ax3 A1 = C1.Position();
    gp_Ax3 A2 = C2.Position();

    // first, A1 & A2 must be coaxials
    if ( A1.Axis().IsCoaxial(A2.Axis(),Precision::Angular(),
			               Precision::Confusion()) ) {
      Standard_Real V = 
	gp_Vec( A1.Location(),A2.Location()).Dot(gp_Vec(A1.Direction()));
      if ( !Trim1 && !Trim2) {
	if ( Abs( C1.Radius() - C2.Radius()) < Precision::Confusion()) {
	  Handle(Geom_CylindricalSurface) C = 
	    new Geom_CylindricalSurface( A1, C1.Radius());
	  Surf = new Geom_RectangularTrimmedSurface
	    ( C, Min(0.,V), Max(0.,V), Standard_False);
	}
	else {
	  Standard_Real Rad = C2.Radius() - C1.Radius();
	  Standard_Real Ang = ATan( Rad / V);
	  if ( Ang < 0.) {
	    A1.ZReverse();
	    V = -V;
	    Ang = -Ang;
	  }
	  Handle(Geom_ConicalSurface) C = 
	    new Geom_ConicalSurface( A1, Ang, C1.Radius());
	  V /= Cos(Ang);
	  Surf = new Geom_RectangularTrimmedSurface
	    ( C, Min(0.,V), Max(0.,V), Standard_False);
	}
	IsDone = Standard_True;
      }
      else if ( Trim1 && Trim2) {


      }

    }

  }

  if ( !IsDone) {
    GeomFill_Generator Generator;
    Generator.AddCurve(Curve1);
    Generator.AddCurve(Curve2);
    Generator.Perform(Precision::PConfusion());
    Surf = Generator.Surface();
  }
  
  return Surf;
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void GeomFill::GetShape (const Standard_Real MaxAng,
			 Standard_Integer& NbPoles,
			 Standard_Integer& NbKnots,
			 Standard_Integer& Degree,
			 Convert_ParameterisationType& TConv)
{
  switch (TConv) {
  case Convert_QuasiAngular:
    {
      NbPoles = 7 ;
      NbKnots = 2 ;
      Degree  = 6 ;
    }
    break;
  case Convert_Polynomial:
    {
      NbPoles = 8;
      NbKnots = 2;
      Degree = 7;
    }
    break;
  default:
    {
      Standard_Integer NbSpan =
	(Standard_Integer)(Ceiling(3.*Abs(MaxAng)/2./M_PI));
      NbPoles = 2*NbSpan+1;
      NbKnots = NbSpan+1;
      Degree = 2;
      if (NbSpan == 1) {
	TConv = Convert_TgtThetaOver2_1;
      }
      else if (NbSpan == 2) {
	TConv = Convert_TgtThetaOver2_2;
      }
      else if (NbSpan == 3) {
	TConv = Convert_TgtThetaOver2_3;
      }
    }
  }
}

//=======================================================================
//function : GetMinimalWeights
//purpose  : On suppose les extremum de poids sont obtenus pour les
//           extremums d'angles (A verifier eventuelement pour Quasi-Angular)
//=======================================================================

void GeomFill::GetMinimalWeights(const Convert_ParameterisationType  TConv,
				 const Standard_Real MinAng,
				 const Standard_Real MaxAng,
				 TColStd_Array1OfReal& Weights)

{
  if (TConv == Convert_Polynomial) Weights.Init(1);
  else {
    gp_Ax2 popAx2(gp_Pnt(0, 0, 0), gp_Dir(0,0,1));
    gp_Circ C (popAx2, 1);
    Handle(Geom_TrimmedCurve) Sect1 = 
      new Geom_TrimmedCurve(new Geom_Circle(C), 0., MaxAng);
    Handle(Geom_BSplineCurve) CtoBspl = 
      GeomConvert::CurveToBSplineCurve(Sect1, TConv);
    CtoBspl->Weights(Weights);
      
    TColStd_Array1OfReal poids (Weights.Lower(),  Weights.Upper());
    Standard_Real angle_min = Max(Precision::PConfusion(), MinAng);
   
    Handle(Geom_TrimmedCurve) Sect2 = 
      new Geom_TrimmedCurve(new Geom_Circle(C), 0., angle_min);
    CtoBspl = GeomConvert::CurveToBSplineCurve(Sect2, TConv);
    CtoBspl->Weights(poids);

    for (Standard_Integer ii=Weights.Lower(); ii<=Weights.Upper(); ii++) {
      if (poids(ii) < Weights(ii)) {
	Weights(ii) = poids(ii);
      }
    }
  }
}


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void GeomFill::Knots(const Convert_ParameterisationType  TConv,
		     TColStd_Array1OfReal& TKnots)
{
  if ((TConv!=Convert_QuasiAngular) && 
      (TConv!=Convert_Polynomial) ) {
    Standard_Integer i;
    Standard_Real val = 0.;
    for (i=TKnots.Lower(); i<=TKnots.Upper(); i++) {
      TKnots(i) = val;
      val = val+1.;
    }
  }
  else {
    TKnots(1) = 0.;
    TKnots(2) = 1.;
  }
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void GeomFill::Mults(const Convert_ParameterisationType  TConv,
		     TColStd_Array1OfInteger& TMults)
{
  switch (TConv) {
  case Convert_QuasiAngular :  
    {
      TMults(1) = 7;
      TMults(2) = 7;
    }
    break;
  case Convert_Polynomial :
    {
      TMults(1) = 8;
      TMults(2) = 8;
    }
    break;

  default : 
    {
      // Cas rational classsique
      Standard_Integer i;
      TMults(TMults.Lower())=3;
      for (i=TMults.Lower()+1; i<=TMults.Upper()-1; i++) {
	TMults(i) = 2;
      }
      TMults(TMults.Upper())=3;
    }
  }
}
//=======================================================================
//function : GetTolerance
//purpose  : Determiner la Tolerance 3d permetant de respecter la Tolerance
//           de continuite G1.
//=======================================================================

Standard_Real GeomFill::GetTolerance(const Convert_ParameterisationType TConv,
				     const Standard_Real AngleMin, 
				     const Standard_Real Radius, 
				     const Standard_Real AngularTol, 
				     const Standard_Real SpatialTol)
{ 
  gp_Ax2 popAx2(gp_Pnt(0, 0, 0), gp_Dir(0,0,1));
  gp_Circ C (popAx2, Radius);
  Handle(Geom_Circle) popCircle = new Geom_Circle(C);
  Handle(Geom_TrimmedCurve) Sect = 
    new Geom_TrimmedCurve(popCircle ,
			  0.,Max(AngleMin, 0.02) );
  // 0.02 est proche d'1 degree, en desous on ne se preocupe pas de la tngence
  // afin d'eviter des tolerances d'approximation tendant vers 0 !
  Handle(Geom_BSplineCurve) CtoBspl = 
	GeomConvert::CurveToBSplineCurve(Sect, TConv);
  Standard_Real Dist;
  Dist = CtoBspl->Pole(1).Distance(CtoBspl->Pole(2)) + SpatialTol;
  return Dist*AngularTol/2;
}

//===========================================================================
//function : GetCircle
//purpose  : Calculs les poles et poids d'un cercle definie par ses extremites
//           et son rayon.
//   On evite (si possible) de passer par les convertions pour
//  1) Des problemes de performances.
//  2) Assurer la coherance entre cette methode est celle qui donne la derive
//============================================================================
void GeomFill::GetCircle( const Convert_ParameterisationType  TConv,
			  const gp_Vec& ns1, // Normal rentrente au premier point
			  const gp_Vec& ns2, // Normal rentrente au second point
			  const gp_Vec& nplan, // Normal au plan
			  const gp_Pnt& pts1, 
			  const gp_Pnt& pts2,
			  const Standard_Real Rayon, // Rayon (doit etre positif)
			  const gp_Pnt& Center, 
			  TColgp_Array1OfPnt& Poles, 
			  TColStd_Array1OfReal& Weights)
{
  // La classe de convertion

  Standard_Integer i, jj;
  Standard_Real Cosa,Sina,Angle,Alpha,Cosas2,lambda;
  gp_Vec temp, np2;
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  Cosa = ns1.Dot(ns2);
  Sina = nplan.Dot(ns1.Crossed(ns2));

  if (Cosa<-1.) {Cosa=-1; Sina = 0;}
  if (Cosa>1.) {Cosa=1; Sina = 0;}
  Angle = ACos(Cosa);
  // Recadrage sur ]-pi/2, 3pi/2]
  if (Sina <0.) {
    if (Cosa > 0.) Angle = -Angle;
    else           Angle =  2.*M_PI - Angle;
  }

  switch (TConv) {
  case Convert_QuasiAngular:
    {
      GeomFill_QuasiAngularConvertor QConvertor;
      QConvertor.Init();
      QConvertor.Section(pts1, Center, nplan, Angle, Poles, Weights);
      break;
    }
  case Convert_Polynomial:
    { 
      GeomFill_PolynomialConvertor PConvertor;
      PConvertor.Init();
      PConvertor.Section(pts1, Center, nplan, Angle, Poles);
      Weights.Init(1);
      break;
    }
  default:
    {
      // Cas Rational, on utilise une expression directe beaucoup plus
      // performente que GeomConvert
      Standard_Integer NbSpan=(Poles.Length()-1)/2;

      Poles(low) = pts1;
      Poles(upp) = pts2;
      Weights(low)    = 1;
      Weights(upp)    = 1; 
 
      np2 = nplan.Crossed(ns1);

      Alpha = Angle/((Standard_Real)(NbSpan));
      Cosas2 = Cos(Alpha/2);
  
      for (i=1, jj=low+2; i<= NbSpan-1; i++, jj+=2) {
	lambda = ((Standard_Real)(i))*Alpha;
	Cosa = Cos(lambda);
	Sina = Sin(lambda);
	temp.SetLinearForm(Cosa-1, ns1, Sina, np2);
	Poles(jj).SetXYZ(pts1.XYZ() + Rayon*temp.XYZ());
	Weights(jj)    = 1; 
      }
  
      lambda = 1./(2.*Cosas2*Cosas2);
      for (i=1, jj=low+1; i<=NbSpan; i++, jj+=2) {
	temp.SetXYZ(Poles(jj-1).XYZ() + Poles(jj+1).XYZ()
		    -2.*Center.XYZ());
	Poles(jj).SetXYZ(Center.XYZ() + lambda*temp.XYZ());
	Weights(jj)    = Cosas2;
      }
    }
  }
}

Standard_Boolean GeomFill::GetCircle(const Convert_ParameterisationType  TConv, 
				      const gp_Vec& ns1,   const gp_Vec& ns2, 
				      const gp_Vec& dn1w,  const gp_Vec& dn2w, 
				      const gp_Vec& nplan, const gp_Vec& dnplan, 
				      const gp_Pnt& pts1,  const gp_Pnt& pts2, 
				      const gp_Vec& tang1, const gp_Vec& tang2, 
				      const Standard_Real Rayon, 
				      const Standard_Real DRayon, 
				      const gp_Pnt& Center, 
				      const gp_Vec& DCenter, 
				      TColgp_Array1OfPnt& Poles, 
				      TColgp_Array1OfVec& DPoles,
				      TColStd_Array1OfReal& Weights, 
				      TColStd_Array1OfReal& DWeights)
{
  Standard_Real Cosa,Sina,Cosas2,Sinas2,Angle,DAngle,Alpha,lambda,Dlambda;
  gp_Vec temp, np2, dnp2;
  Standard_Integer i, jj;
  Standard_Integer NbSpan=(Poles.Length()-1)/2;
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  Cosa = ns1.Dot(ns2);
  Sina = nplan.Dot(ns1.Crossed(ns2));

  if (Cosa<-1.){Cosa=-1; Sina = 0;}
  if (Cosa>1.) {Cosa=1; Sina = 0;}
  Angle = ACos(Cosa);
  // Recadrage sur ]-pi/2, 3pi/2]
  if (Sina <0.) {
    if (Cosa > 0.) Angle = -Angle;
    else           Angle =  2.*M_PI - Angle;
  }

  if (Abs(Sina)>Abs(Cosa)) {
    DAngle = -(dn1w.Dot(ns2) + ns1.Dot(dn2w))/Sina;
  }
  else{
    DAngle = (dnplan.Dot(ns1.Crossed(ns2)) + nplan.Dot(dn1w.Crossed(ns2) 
					   + ns1.Crossed(dn2w)))/Cosa;
  }

// Aux Extremites.  
  Poles(low) = pts1;
  Poles(upp) = pts2;
  Weights(low)  = 1;
  Weights(upp) = 1;

  DPoles(low) = tang1;
  DPoles(upp) = tang2;
  DWeights(low) = 0;
  DWeights(upp) = 0;


  switch (TConv) {
  case Convert_QuasiAngular:
    {
      GeomFill_QuasiAngularConvertor QConvertor;
      QConvertor.Init();
      QConvertor.Section(pts1, tang1, 
			Center, DCenter, 
			nplan, dnplan,
			Angle, DAngle,
			Poles, DPoles,
			Weights, DWeights);
      return Standard_True;
  }
  case Convert_Polynomial:
    {
      GeomFill_PolynomialConvertor PConvertor;
      PConvertor.Init();
      PConvertor.Section(pts1, tang1, 
			Center, DCenter, 
			nplan, dnplan,
			Angle, DAngle,
			Poles, DPoles);
      Weights.Init(1);
      DWeights.Init(0);
      return Standard_True;
  }

  default:
     // Cas rationel classique
    { 
      np2 = nplan.Crossed(ns1);
      dnp2 = dnplan.Crossed(ns1).Added(nplan.Crossed(dn1w));
 
      Alpha = Angle/((Standard_Real)(NbSpan));
      Cosas2 = Cos(Alpha/2);
      Sinas2 = Sin(Alpha/2);

      for (i=1, jj=low+2; i<= NbSpan-1; i++, jj+=2) {
	lambda = ((Standard_Real)(i))*Alpha;
	Cosa = Cos(lambda);
	Sina = Sin(lambda);
	temp.SetLinearForm(Cosa-1,ns1,Sina,np2);
	Poles(jj).SetXYZ(pts1.XYZ() + Rayon*temp.XYZ());
    
	DPoles(jj).SetLinearForm(DRayon, temp, tang1);
	temp.SetLinearForm(-Sina,ns1,Cosa,np2);
	temp.Multiply(((Standard_Real)(i))/((Standard_Real)(NbSpan))*DAngle);
	temp.Add(((Cosa-1)*dn1w).Added(Sina*dnp2));
	DPoles(jj)+= Rayon*temp;
      }
  
      lambda = 1./(2.*Cosas2*Cosas2);
      Dlambda = (lambda*Sinas2*DAngle)/(Cosas2*NbSpan);

      for (i=1, jj=low; i<=NbSpan; i++, jj+=2) {
	temp.SetXYZ(Poles(jj).XYZ() + Poles(jj+2).XYZ()
		    -2.*Center.XYZ());
	Poles(jj+1).SetXYZ(Center.XYZ()+lambda*temp.XYZ());
	DPoles(jj+1).SetLinearForm(Dlambda, temp,
				   1.-2*lambda, DCenter,
				   lambda, (DPoles(jj)+ DPoles(jj+2)));       
      }
  
      // Les poids
      Dlambda = -Sinas2*DAngle/(2*NbSpan);
      for (i=low; i<upp; i+=2) {
	Weights(i)    = 1.;
	Weights(i+1)  = Cosas2;
	DWeights(i)   = 0.;
	DWeights(i+1) = Dlambda;
      }
    }
    return Standard_True;
  }
//  return Standard_False;
}

Standard_Boolean GeomFill::GetCircle(const Convert_ParameterisationType  TConv, 
				     const gp_Vec& ns1,   const gp_Vec& ns2, 
				     const gp_Vec& dn1w,  const gp_Vec& dn2w,
				     const gp_Vec& d2n1w, const gp_Vec& d2n2w,
				     const gp_Vec& nplan, const gp_Vec& dnplan, 
				     const gp_Vec& d2nplan, 
				     const gp_Pnt& pts1,  const gp_Pnt& pts2, 
				     const gp_Vec& tang1, const gp_Vec& tang2,
				     const gp_Vec& Dtang1, const gp_Vec& Dtang2, 
				     const Standard_Real Rayon, 
				     const Standard_Real DRayon,
				     const Standard_Real D2Rayon, 
				     const gp_Pnt& Center, 
				     const gp_Vec& DCenter,
				     const gp_Vec& D2Center,
				     TColgp_Array1OfPnt& Poles, 
				     TColgp_Array1OfVec& DPoles,
				     TColgp_Array1OfVec& D2Poles,
				     TColStd_Array1OfReal& Weights, 
				     TColStd_Array1OfReal& DWeights,
				     TColStd_Array1OfReal& D2Weights)
{
  Standard_Real Cosa,Sina,Cosas2,Sinas2;
  Standard_Real Angle, DAngle, D2Angle, Alpha;
  Standard_Real lambda, Dlambda, D2lambda, aux;
  gp_Vec temp, dtemp, np2, dnp2, d2np2;
  Standard_Integer i, jj;
  Standard_Integer NbSpan=(Poles.Length()-1)/2;
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  Cosa = ns1.Dot(ns2);
  Sina = nplan.Dot(ns1.Crossed(ns2));

  if (Cosa<-1.){Cosa=-1; Sina = 0;}
  if (Cosa>1.) {Cosa=1; Sina = 0;}
  Angle = ACos(Cosa);
  // Recadrage sur ]-pi/2, 3pi/2]
  if (Sina <0.) {
    if (Cosa > 0.) Angle = -Angle;
    else           Angle =  2.*M_PI - Angle;
  }

  if (Abs(Sina)>Abs(Cosa)) {
    aux = dn1w.Dot(ns2) + ns1.Dot(dn2w);
    DAngle  = -aux/Sina;
    D2Angle = -(d2n1w.Dot(ns2) + 2*dn1w.Dot(dn2w) + ns1.Dot(d2n2w))/Sina
              + aux*(dnplan.Dot(ns1.Crossed(ns2)) + nplan.Dot(dn1w.Crossed(ns2) 
	      + ns1.Crossed(dn2w)))/(Sina*Sina); 
  }
  else{
    temp = dn1w.Crossed(ns2) + ns1.Crossed(dn2w);
    DAngle = (dnplan.Dot(ns1.Crossed(ns2)) + nplan.Dot(temp))/Cosa;
    D2Angle = ( d2nplan.Dot(ns1.Crossed(ns2)) +2*dnplan.Dot(temp)
	      + nplan.Dot(d2n1w.Crossed(ns2) + 2*dn1w.Crossed(dn2w) 
	      + ns1.Crossed(d2n2w)) )/Cosa
	      - ( dn1w.Dot(ns2) + ns1.Dot(dn2w))
                * (dnplan.Dot(ns1.Crossed(ns2)) + nplan.Dot(temp)) /(Cosa*Cosa);
  }

// Aux Extremites.  
  Poles(low) = pts1;
  Poles(upp) = pts2;
  Weights(low)  = 1;
  Weights(upp) = 1;

  DPoles(low) = tang1;
  DPoles(upp) = tang2;
  DWeights(low) = 0;
  DWeights(upp) = 0;

  D2Poles(low) = Dtang1;
  D2Poles(upp) = Dtang2;
  D2Weights(low) = 0;
  D2Weights(upp) = 0;  


  switch (TConv) {
  case Convert_QuasiAngular:
    {
      GeomFill_QuasiAngularConvertor QConvertor;
      QConvertor.Init();
      QConvertor.Section(pts1, tang1, Dtang1,
			   Center, DCenter, D2Center,
			   nplan, dnplan, d2nplan,
			   Angle, DAngle, D2Angle,
			   Poles, DPoles, D2Poles,
			   Weights, DWeights, D2Weights);
      return Standard_True;  
  }
  case Convert_Polynomial:
    {
      GeomFill_PolynomialConvertor PConvertor;
      PConvertor.Init();
      PConvertor.Section(pts1, tang1, Dtang1,
			   Center, DCenter, D2Center,
			   nplan, dnplan,  d2nplan,
			   Angle, DAngle, D2Angle,
			   Poles, DPoles, D2Poles);
      Weights.Init(1);
      DWeights.Init(0);
      D2Weights.Init(0);
      return Standard_True;
  }

  default:
    { 
      np2 = nplan.Crossed(ns1);
      dnp2 = dnplan.Crossed(ns1).Added(nplan.Crossed(dn1w));
      d2np2 = d2nplan.Crossed(ns1).Added(nplan.Crossed(dn2w));
      d2np2 += 2*dnplan.Crossed(dn1w);
 
      Alpha = Angle/((Standard_Real)(NbSpan));
      Cosas2 = Cos(Alpha/2);
      Sinas2 = Sin(Alpha/2);

      for (i=1, jj=low+2; i<= NbSpan-1; i++, jj+=2) {
	lambda = ((Standard_Real)(i))*Alpha;
	Cosa = Cos(lambda);
	Sina = Sin(lambda);
	temp.SetLinearForm(Cosa-1,ns1,Sina,np2);
	Poles(jj).SetXYZ(pts1.XYZ() + Rayon*temp.XYZ());
    
	DPoles(jj).SetLinearForm(DRayon, temp, tang1);
	dtemp.SetLinearForm(-Sina,ns1,Cosa,np2);
        aux = ((Standard_Real)(i))/((Standard_Real)(NbSpan));
	dtemp.Multiply(aux*DAngle);
	dtemp.Add(((Cosa-1)*dn1w).Added(Sina*dnp2));
	DPoles(jj)+= Rayon*dtemp;

        D2Poles(jj).SetLinearForm(D2Rayon, temp, 
                                  2*DRayon, dtemp, Dtang1);
 	temp.SetLinearForm(Cosa-1, dn2w, Sina, d2np2);
        dtemp.SetLinearForm(-Sina,ns1,Cosa,np2);
        temp+= (aux*aux*D2Angle)*dtemp;
        dtemp.SetLinearForm(-Sina, dn1w+np2, Cosa, dnp2,
			    -Cosa, ns1);
        temp+=(aux*DAngle)*dtemp;
	D2Poles(jj)+= Rayon*temp;       
      }
  
      lambda = 1./(2.*Cosas2*Cosas2);
      Dlambda = (lambda*Sinas2*DAngle)/(Cosas2*NbSpan);
      aux = Sinas2/Cosas2;
      D2lambda = (  Dlambda * aux*DAngle 
		  + D2Angle * aux*lambda
		  + (1+aux*aux)*(DAngle/(2*NbSpan)) * DAngle*lambda ) 
	       / NbSpan;
      for (i=1, jj=low; i<=NbSpan; i++, jj+=2) {
	temp.SetXYZ(Poles(jj).XYZ() + Poles(jj+2).XYZ()
		    -2.*Center.XYZ());
	Poles(jj+1).SetXYZ(Center.XYZ()+lambda*temp.XYZ());


        dtemp.SetXYZ(DPoles(jj).XYZ() + DPoles(jj+2).XYZ()
		    -2.*DCenter.XYZ());
	DPoles(jj+1).SetLinearForm(Dlambda, temp,
				   lambda, dtemp,
				   DCenter);
	D2Poles(jj+1).SetLinearForm(D2lambda, temp,
				    2*Dlambda, dtemp,
				    lambda, (D2Poles(jj)+ D2Poles(jj+2)));
        D2Poles(jj+1)+= (1-2*lambda)*D2Center;
      }
  
      // Les poids
      Dlambda = -Sinas2*DAngle/(2*NbSpan);
      D2lambda = -Sinas2*D2Angle/(2*NbSpan)
                 -Cosas2*Pow(DAngle/(2*NbSpan),2);

      for (i=low; i<upp; i+=2) {
	Weights(i)    = 1.;
	Weights(i+1)  = Cosas2;
	DWeights(i)   = 0.;
	DWeights(i+1) = Dlambda;
        D2Weights(i)  = 0.;
        D2Weights(i+1)  = D2lambda;
      }
    }
    return Standard_True;
  }
}
