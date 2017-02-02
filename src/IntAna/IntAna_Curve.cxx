// Created on: 1992-06-30
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif

//----------------------------------------------------------------------
//-- Differents constructeurs sont proposes qui correspondent aux
//-- polynomes en Z :  
//--    A(Sin(Theta),Cos(Theta)) Z**2 
//--  + B(Sin(Theta),Cos(Theta)) Z
//--  + C(Sin(Theta),Cos(Theta))
//--
//-- Une Courbe est definie sur un domaine 
//--
//-- Value retourne le point de parametre U(Theta),V(Theta)
//--       ou V est la solution du polynome A V**2 + B V + C
//--       (Selon les cas, on prend V+ ou V-)
//--
//-- D1u   calcule le vecteur tangent a la courbe 
//--       et retourne le booleen Standard_False si ce calcul ne peut
//--       pas etre mene a bien.  
//----------------------------------------------------------------------

#include <ElSLib.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IntAna_Curve.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : IntAna_Curve
//purpose  : 
//=======================================================================
  IntAna_Curve::IntAna_Curve()
{
  typequadric=GeomAbs_OtherSurface;
  firstbounded=Standard_False;
  lastbounded=Standard_False;
}
//=======================================================================
//function : SetConeQuadValues
//purpose  : Description de l intersection Cone Quadrique
//=======================================================================
  void IntAna_Curve::SetConeQuadValues(const gp_Cone& Cone,
				       const Standard_Real Qxx,
				       const Standard_Real Qyy,
				       const Standard_Real Qzz,
				       const Standard_Real Qxy,
				       const Standard_Real Qxz,
				       const Standard_Real Qyz,
				       const Standard_Real Qx,
				       const Standard_Real Qy,
				       const Standard_Real Qz,
				       const Standard_Real Q1,
				       const Standard_Real TOL,
				       const Standard_Real DomInf,
				       const Standard_Real DomSup,
				       const Standard_Boolean twocurves,
				       const Standard_Boolean takezpositive) 
{
  
  Ax3        = Cone.Position();
  RCyl       = Cone.RefRadius();

  Angle      = Cone.SemiAngle();
  Standard_Real UnSurTgAngle = 1.0/(Tan(Cone.SemiAngle()));

  typequadric= GeomAbs_Cone;
  
  TwoCurves     = twocurves;         //-- deux  Z pour un meme parametre 
  TakeZPositive = takezpositive;     //-- Prendre sur la courbe le Z Positif
                                     //--   ( -B + Sqrt()) et non (-B - Sqrt())
  
  
  Z0Cte      = Q1;                   //-- Attention On a    Z?Cos Cos(t)
  Z0Sin      = 0.0;                  //-- et Non          2 Z?Cos Cos(t) !!!
  Z0Cos      = 0.0;                  //-- Ce pour tous les Parametres
  Z0CosCos   = 0.0;                  //--  ie pas de Coefficient 2  
  Z0SinSin   = 0.0;                  //--     devant les termes CS C S 
  Z0CosSin   = 0.0;
  
  Z1Cte      = 2.0*(UnSurTgAngle)*Qz;
  Z1Sin      = Qy+Qy;
  Z1Cos      = Qx+Qx;
  Z1CosCos   = 0.0;
  Z1SinSin   = 0.0;
  Z1CosSin   = 0.0;
  
  Z2Cte      = Qzz * UnSurTgAngle*UnSurTgAngle;
  Z2Sin      = (UnSurTgAngle+UnSurTgAngle)*Qyz;
  Z2Cos      = (UnSurTgAngle+UnSurTgAngle)*Qxz;
  Z2CosCos   = Qxx;
  Z2SinSin   = Qyy;
  Z2CosSin   = Qxy+Qxy;

  Tolerance  = TOL;
  DomainInf  = DomInf;
  DomainSup  = DomSup;
  
  RestrictedInf = RestrictedSup = Standard_True;   //-- Le Domaine est Borne
  firstbounded = lastbounded = Standard_False;
}

//=======================================================================
//function : SetCylinderQuadValues
//purpose  : Description de l intersection Cylindre Quadrique
//=======================================================================
  void IntAna_Curve::SetCylinderQuadValues(const gp_Cylinder& Cyl,
					   const Standard_Real Qxx,
					   const Standard_Real Qyy,
					   const Standard_Real Qzz,
					   const Standard_Real Qxy,
					   const Standard_Real Qxz,
					   const Standard_Real Qyz,
					   const Standard_Real Qx,
					   const Standard_Real Qy,
					   const Standard_Real Qz,
					   const Standard_Real Q1,
					   const Standard_Real TOL,
					   const Standard_Real DomInf,
					   const Standard_Real DomSup,
					   const Standard_Boolean twocurves,
					   const Standard_Boolean takezpositive) 
{
  
  Ax3        = Cyl.Position();
  RCyl       = Cyl.Radius();
  typequadric= GeomAbs_Cylinder;
  
  TwoCurves     = twocurves;         //-- deux  Z pour un meme parametre 
  TakeZPositive = takezpositive;     //-- Prendre sur la courbe le Z Positif
  Standard_Real RCylmul2 = RCyl+RCyl;         //--   ( -B + Sqrt()) 

  Z0Cte      = Q1;
  Z0Sin      = RCylmul2*Qy;
  Z0Cos      = RCylmul2*Qx;
  Z0CosCos   = Qxx*RCyl*RCyl;
  Z0SinSin   = Qyy*RCyl*RCyl;
  Z0CosSin   = RCylmul2*RCyl*Qxy;

  Z1Cte      = Qz+Qz;
  Z1Sin      = RCylmul2*Qyz;
  Z1Cos      = RCylmul2*Qxz;
  Z1CosCos   = 0.0;
  Z1SinSin   = 0.0;
  Z1CosSin   = 0.0;

  Z2Cte      = Qzz;
  Z2Sin      = 0.0;
  Z2Cos      = 0.0;
  Z2CosCos   = 0.0;
  Z2SinSin   = 0.0;
  Z2CosSin   = 0.0;

  Tolerance  = TOL;
  DomainInf  = DomInf;
  DomainSup  = DomSup;
  
  RestrictedInf = RestrictedSup = Standard_True;
  firstbounded = lastbounded = Standard_False;
}

//=======================================================================
//function : IsOpen
//purpose  : 
//=======================================================================
  Standard_Boolean IntAna_Curve::IsOpen() const 
{
  return(RestrictedInf && RestrictedSup);
}

//=======================================================================
//function : Domain
//purpose  : 
//=======================================================================
  void IntAna_Curve::Domain(Standard_Real& DInf,
			    Standard_Real& DSup) const
{
  if(RestrictedInf && RestrictedSup) {
    DInf=DomainInf;
    DSup=DomainSup;
    if(TwoCurves) {
      DSup+=DSup-DInf;
    }
  }
  else {
    throw Standard_DomainError("IntAna_Curve::Domain");
  }
}
//=======================================================================
//function : IsConstant
//purpose  : 
//=======================================================================
  Standard_Boolean IntAna_Curve::IsConstant() const 
{
  //-- ???  Pas facile de decider a la seule vue des Param.
  return(Standard_False);
}

//=======================================================================
//function : IsFirstOpen
//purpose  : 
//=======================================================================
  Standard_Boolean IntAna_Curve::IsFirstOpen() const 
{
  return(firstbounded);
}

//=======================================================================
//function : IsLastOpen
//purpose  : 
//=======================================================================
  Standard_Boolean IntAna_Curve::IsLastOpen() const 
{
  return(lastbounded);
}
//=======================================================================
//function : SetIsFirstOpen
//purpose  : 
//=======================================================================
  void IntAna_Curve::SetIsFirstOpen(const Standard_Boolean Flag) 
{
  firstbounded = Flag;
}

//=======================================================================
//function : SetIsLastOpen
//purpose  : 
//=======================================================================
  void IntAna_Curve::SetIsLastOpen(const Standard_Boolean Flag) 
{
  lastbounded = Flag;
}

//=======================================================================
//function : InternalUVValue
//purpose  : 
//=======================================================================
  void IntAna_Curve::InternalUVValue(const Standard_Real theta,
				     Standard_Real& Param1,
				     Standard_Real& Param2,
				     Standard_Real& A,
				     Standard_Real& B,
				     Standard_Real& C,
				     Standard_Real& cost,
				     Standard_Real& sint,
				     Standard_Real& SigneSqrtDis) const 
{
  const Standard_Real aRelTolp = 1.0+Epsilon(1.0), aRelTolm = 1.0-Epsilon(1.0);
  Standard_Real Theta=theta;
  Standard_Boolean SecondSolution=Standard_False; 

  if((Theta<DomainInf*aRelTolm)  ||  
     ((Theta>DomainSup*aRelTolp) && (!TwoCurves)) ||  
     (Theta>(DomainSup+DomainSup-DomainInf)*aRelTolp)) {
    SigneSqrtDis = 0.;
    throw Standard_DomainError("IntAna_Curve::Domain");
  }
  
  if(Theta>DomainSup) { 
    Theta=DomainSup+DomainSup-Theta;
    SecondSolution=Standard_True; 
  }

  Param1=Theta;

  if(!TwoCurves) { 
    SecondSolution=TakeZPositive; 
  }
  //
  cost = Cos(Theta);
  sint = Sin(Theta);
  Standard_Real costsint = cost*sint;
  
  A=Z2Cte+sint*(Z2Sin+sint*Z2SinSin)+cost*(Z2Cos+cost*Z2CosCos)
    +Z2CosSin*costsint;
  
  B=Z1Cte+sint*(Z1Sin+sint*Z1SinSin)+cost*(Z1Cos+cost*Z1CosCos)
    +Z1CosSin*costsint;
  
  C=Z0Cte+sint*(Z0Sin+sint*Z0SinSin)+cost*(Z0Cos+cost*Z0CosCos)
    +Z0CosSin*costsint;
  

  const Standard_Real aDiscriminant = Max(B*B-4.0*A*C, 0.0);
  
  if(Abs(A)<=Precision::PConfusion()) {   
    //-- cout<<" IntAna_Curve:: Internal UV Value : A="<<A<<"  -> Abs(A)="<<Abs(A)<<endl;
    if(Abs(B)<=Precision::PConfusion()) { 
      //-- cout<<" Probleme :  Pas de solutions "<<endl;
      Param2=0.0;
    }
    else {
      //modified by NIZNHY-PKV Fri Dec  2 16:02:46 2005f
      Param2 = -C/B;
      /*
      if(!SecondSolution) {   
	//-- Cas Param2 = (-B+Sqrt(Discriminant))/(A+A);
	//--            = (-B+Sqrt(B**2 - Eps)) / 2A
	//--            = -C / B
	Param2 = -C/B;
      } 
      else {
	//-- Cas Param2 = (-B-Sqrt(Discriminant))/(A+A);
	//--            = (-B-Sqrt(B**2 - Eps)) / 2A
	if(A) { 
	  Param2 = -B/A; 
	}
	else { 
	  Param2 = -B*10000000.0;
	}
      }
      */
      //modified by NIZNHY-PKV Fri Dec  2 16:02:54 2005t
    }
  }
  else {
    SigneSqrtDis = (SecondSolution)? Sqrt(aDiscriminant) : -Sqrt(aDiscriminant);
    Param2=(-B+SigneSqrtDis)/(A+A);
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
  gp_Pnt IntAna_Curve::Value(const Standard_Real theta)  
{
  Standard_Real A, B, C, U, V, sint, cost, SigneSqrtDis;
  //
  A=0.0;  B=0.0;   C=0.0;
  U=0.0;  V=0.0;  
  sint=0.0;  cost=0.0;
  SigneSqrtDis=0.0;
  InternalUVValue(theta,U,V,A,B,C,cost,sint,SigneSqrtDis); 
  //-- checked the parameter U and Raises Domain Error if Error
  return(InternalValue(U,V));
}
//=======================================================================
//function : D1u
//purpose  : 
//=======================================================================
  Standard_Boolean IntAna_Curve::D1u(const Standard_Real theta,
				     gp_Pnt& Pt,
				     gp_Vec& Vec) 
{
  //-- Pour detecter le cas ou le calcul est impossible 
  Standard_Real A, B, C, U, V, sint, cost, SigneSqrtDis;
  A=0.0;  B=0.0;   C=0.0;
  U=0.0;  V=0.0;  
  sint=0.0;  cost=0.0;
  //
  InternalUVValue(theta,U,V,A,B,C,cost,sint,SigneSqrtDis);
  //
  Pt = Value(theta);
  if(Abs(A)<0.0000001 || Abs(SigneSqrtDis)<0.0000000001) return(Standard_False);
  
  
  //-- Approximation de la derivee (mieux que le calcul mathematique!)
  Standard_Real dtheta = (DomainSup-DomainInf)*0.000001;
  Standard_Real theta2 = theta+dtheta;
  if((theta2<DomainInf)  ||  ((theta2>DomainSup) && (!TwoCurves))
     ||  (theta2>(DomainSup+DomainSup-DomainInf+0.00000000000001))) {
    dtheta = -dtheta;
    theta2 = theta+dtheta;
  }
  gp_Pnt P2 = Value(theta2);
  dtheta = 1.0/dtheta;
  Vec.SetCoord((P2.X()-Pt.X())*dtheta,
	       (P2.Y()-Pt.Y())*dtheta,
	       (P2.Z()-Pt.Z())*dtheta);
  
  return(Standard_True);
}  
//=======================================================================
//function : FindParameter
//purpose  : Para est en sortie le parametre sur la courbe 
//=======================================================================
  Standard_Boolean IntAna_Curve::FindParameter (const gp_Pnt& P,
						Standard_Real& Para) const
{
  Standard_Real theta,z, aTolPrecision=0.0001;
  Standard_Real PIpPI = M_PI + M_PI;
  //
  switch (typequadric) {

  case GeomAbs_Cylinder:
    {
      ElSLib::CylinderParameters(Ax3,RCyl,P,theta,z);
    }
    break;
    
  case GeomAbs_Cone :
    {
      ElSLib::ConeParameters(Ax3,RCyl,Angle,P,theta,z);
    }
    break;

  default:
    return Standard_False;
    break;
  }
  //
  Standard_Real epsAng = 1.e-8;
  Standard_Real tmin = DomainInf;
  Standard_Real tmax = DomainSup;
  Standard_Real U,V,A,B,C,sint,cost,SigneSqrtDis;
  Standard_Real z1,z2;

  A=0.0;  B=0.0;   C=0.0;
  U=0.0;  V=0.0;  
  sint=0.0;  cost=0.0;
  SigneSqrtDis=0.0;
  //U=V=A=B=C=sint=cost=SigneSqrtDis=0.0;
  //
  if (!firstbounded && tmin > theta && (tmin-theta) <= epsAng) {
    theta = tmin;
  }
  else if (!lastbounded && theta > tmax && (theta-tmax) <= epsAng) {
    theta = tmax;
  }
  //
  if (theta < tmin ) {
    theta = theta + PIpPI;
  }
  else if (theta > tmax) {
    theta = theta - PIpPI;
  }
  if (theta < tmin || theta > tmax) {
    if(theta>tmax) { 
      InternalUVValue(tmax,U,V,A,B,C,cost,sint,SigneSqrtDis); 
      gp_Pnt PMax(InternalValue(U,V)); 
      if(PMax.Distance(P) < aTolPrecision) { 
	Para = tmax; 
	return(Standard_True); 
      }
    }
    if(theta<tmin) { 
      InternalUVValue(tmin,U,V,A,B,C,cost,sint,SigneSqrtDis); 
      gp_Pnt PMin(InternalValue(U,V));  
      if(PMin.Distance(P) < aTolPrecision) { 
	Para = tmin; 
	return(Standard_True); 
      }
    }
    //-- lbr le 14 Fev 96 : On teste malgre tout si le point n est pas le 
    //-- point de debut ou de fin 
    //-- cout<<"False 1 "<<endl;
    //    theta = tmin;                               le 25 Nov 96
  }

  if (TwoCurves) {
    if(theta > tmax) 
      theta =  tmax;
    if(theta < tmin) 
      theta = tmin;
    InternalUVValue(theta,U,z1,A,B,C,cost,sint,SigneSqrtDis); 
    A = B = C = sint = cost = SigneSqrtDis = 0.0;
    InternalUVValue(tmax+tmax - theta,U,z2,A,B,C,cost,sint,SigneSqrtDis); 

    if (Abs(z-z1) <= Abs(z-z2)) {
      Para = theta;
    }
    else {
      Para = tmax+tmax - theta;
    }
  }
  else {
    Para = theta;
  }

  if((Para<DomainInf)  ||  ((Para>DomainSup) && (!TwoCurves))
     ||  (Para>(DomainSup+DomainSup-DomainInf+0.00000000000001))) {
    return(Standard_False);
  }
  
  InternalUVValue(Para,U,V,A,B,C,cost,sint,SigneSqrtDis);
  gp_Pnt PPara = InternalValue(U,V);
  Standard_Real Dist = PPara.Distance(P);
  if(Dist > aTolPrecision) { 
    //-- Il y a eu un probleme  
    //-- On teste si le point est un point double 
    InternalUVValue(tmin,U,V,A,B,C,cost,sint,SigneSqrtDis);
    PPara = InternalValue(U,V);
    Dist = PPara.Distance(P);
    if(Dist <= aTolPrecision) {
      Para = tmin; 
      return(Standard_True);
    }

    InternalUVValue(tmax,U,V,A,B,C,cost,sint,SigneSqrtDis);
    PPara = InternalValue(U,V);
    Dist = PPara.Distance(P);
    if(Dist <= aTolPrecision) {
      Para = tmax; 
      return(Standard_True);
    }
    if (TwoCurves) {
      Standard_Real Theta = DomainSup+DomainSup-DomainInf;
      InternalUVValue(Theta,U,V,A,B,C,cost,sint,SigneSqrtDis);
      PPara = InternalValue(U,V);
      Dist = PPara.Distance(P);
      if(Dist <= aTolPrecision) {
	Para = Theta; 
	return(Standard_True);
      } 
    }
    return(Standard_False);
  }
  return(Standard_True);
}
//=======================================================================
//function : InternalValue
//purpose  : 
//=======================================================================
  gp_Pnt IntAna_Curve::InternalValue(const Standard_Real U,
				     const Standard_Real _V) const 
{
  //-- cout<<" ["<<U<<","<<V<<"]";
  Standard_Real V = _V;
  if(V > 100000.0 )   {   V= 100000.0; }       
  if(V < -100000.0 )  {   V=-100000.0; }      

  switch(typequadric) {
  case GeomAbs_Cone:
    {
      //------------------------------------------------
      //-- Parametrage : X = V * Cos(U)              ---
      //--               Y = V * Sin(U)              ---
      //--               Z = (V-RCyl) / Tan(SemiAngle)--
      //------------------------------------------------ 
      //-- Angle Vaut Cone.SemiAngle()  
      return(ElSLib::ConeValue(U,(V-RCyl)/Sin(Angle),Ax3,RCyl,Angle));
    }
    break;
    
  case GeomAbs_Cylinder:
    return(ElSLib::CylinderValue(U,V,Ax3,RCyl)); 
  case GeomAbs_Sphere:
    return(ElSLib::SphereValue(U,V,Ax3,RCyl)); 
  default:
    return(gp_Pnt(0.0,0.0,0.0));
  }
}

//
//=======================================================================
//function : SetDomain
//purpose  : 
//=======================================================================
  void IntAna_Curve::SetDomain(const Standard_Real DInf,
			       const Standard_Real DSup) 
{
  if(DInf>=DSup) {
    throw Standard_DomainError("IntAna_Curve::Domain");
  }
  //
  DomainInf=DInf;
  DomainSup=DSup;
}
