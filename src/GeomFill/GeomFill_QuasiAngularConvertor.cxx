// Created on: 1997-08-06
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Convert_CompPolynomialToPoles.hxx>
#include <GeomFill_QuasiAngularConvertor.hxx>
#include <gp_Ax1.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <PLib.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#define NullAngle 1.e-6

// QuasiAngular is rational definition of Cos(theta(t) and sin(theta)
// on [-alpha, +alpha] with
//                     2      2
//                    U   -  V 
//  cos (theta(t)) = ----------
//                     2      2
//                    U   +  V 

//                      2 * U*V
//   sin (theta(t)) = ----------
//                      2      2
//                     U   +  V 
//                     3
//       V(t) = t + c t
//                     2
//       U(t) = 1 + b t
//            1
//       c = ---  + b   
//            3 
//            -1                     gamma
//       b =---------  +   -----------------------  
//                 2 
//            gamma         3*(tang gamma - gamma) 
//     with gamma = alpha / 2


GeomFill_QuasiAngularConvertor::GeomFill_QuasiAngularConvertor(): 
                    myinit(Standard_False),
		    B(1, 7, 1, 7), 
		    Px(1, 7), Py(1, 7), 
		    W(1,7), Vx(1, 7), Vy(1, 7), Vw(1,7)
{
}

Standard_Boolean GeomFill_QuasiAngularConvertor::Initialized() const
{
 return myinit;
}

void GeomFill_QuasiAngularConvertor::Init() 
{
  if (myinit) return; //On n'initialise qu'une fois
  Standard_Integer ii, jj, Ordre=7;
  Standard_Real terme;
  TColStd_Array1OfReal Coeffs(1, Ordre*Ordre), TrueInter(1,2), Inter(1,2);
  Handle(TColStd_HArray2OfReal) 
    Poles1d = new (TColStd_HArray2OfReal) (1, Ordre, 1, Ordre);

  //Calcul de B
  Inter.SetValue(1, -1);
  Inter.SetValue(2,  1);
  TrueInter.SetValue(1, -1);
  TrueInter.SetValue(2,  1);

  Coeffs.Init(0);
  for (ii=1; ii<=Ordre; ii++) {  Coeffs.SetValue(ii+(ii-1)*Ordre, 1); }

   //Convertion
  Convert_CompPolynomialToPoles 
	 AConverter(Ordre, Ordre-1,  Ordre-1,
		    Coeffs, 
		    Inter,
		    TrueInter);
  AConverter.Poles(Poles1d);

  for (jj=1; jj<=Ordre; jj++) {
      for (ii=1; ii<=Ordre; ii++) {
        terme = Poles1d->Value(ii,jj);
        if (Abs(terme-1) < 1.e-9) terme = 1 ; //petite retouche
        if (Abs(terme+1) < 1.e-9) terme = -1; 
	B(ii, jj) =  terme;
      }
  }
 
  // Init des polynomes
  Vx.Init(0);
  Vx(1) = 1;
  Vy.Init(0);
  Vy(2) = 2;
  Vw.Init(0);
  Vw(1) = 1;
  myinit = Standard_True;
}

void GeomFill_QuasiAngularConvertor::Section(const gp_Pnt& FirstPnt,
				       const gp_Pnt& Center,
				       const gp_Vec& Dir,
				       const Standard_Real Angle,
				       TColgp_Array1OfPnt& Poles,
				       TColStd_Array1OfReal& Weights) 
{ 
  Standard_Real b, b2, c, c2,tan_b;
  Standard_Integer ii;
  Standard_Real beta, beta2, beta3, beta4, beta5, beta6, wi;
  gp_XYZ aux;
  gp_Mat Rot;
  // Calcul de la transformation
  gp_Vec V1(Center, FirstPnt), V2;
  Rot.SetRotation(Dir.XYZ(), Angle/2);
  aux = V1.XYZ();
  aux *= Rot;
  V1.SetXYZ(aux);
  V2 =  Dir^V1;
 
  gp_Mat M(V1.X(), V2.X(), 0, 
	   V1.Y(), V2.Y(), 0,
	   V1.Z(), V2.Z(), 0);

  // Calcul des coeffs  -----------
  beta = Angle/4;
  beta2 = beta * beta;
  beta3 = beta * beta2;
  beta4 = beta2*beta2;
  beta5 = beta3*beta2;
  beta6 = beta3*beta3; 
 
  if  ((M_PI/2 - beta)> NullAngle) {
    if (Abs(beta) < NullAngle) {
     Standard_Real cf = 2.0/(3*5*7);
     b = - (0.2+cf*beta2) / (1+ 0.2*beta2);
//     b = beta5 / cf; 
    }
    else {
      tan_b = Tan(beta);
      b = - 1.0e0 / beta2;
      b += beta / (3*(tan_b - beta));
    }
  }
  else b = ((Standard_Real) -1)/beta2;
  c =  ((Standard_Real) 1)/ 3 + b;
  b2 = b*b;
  c2 = c*c;

  // X = U*U - V*V
  Vx(3) = beta2*(2*b - 1);
  Vx(5) = beta4*(b2 - 2*c); 
  Vx(7) = -beta6*c2;

  //Y = 2*U*V
  Vy(2) = 2*beta;
  Vy(4) = beta3*2*(c+b);
  Vy(6) = 2*beta5*b*c;

 // W = U*U + V*V
  Vw(3) = beta2*(1 + 2*b);
  Vw(5) = beta4*(2*c + b2); 
  Vw(7) = beta6*c2;   
    
// Calculs des poles
  Px.Multiply(B, Vx);
  Py.Multiply(B, Vy);
  W.Multiply(B, Vw);

// Transfo
  gp_XYZ pnt;
  for (ii=1; ii<=7; ii++) {
     wi = W(ii);
     pnt.SetCoord(Px(ii)/wi, Py(ii)/wi, 0);
     pnt *= M;
     pnt += Center.XYZ();
     Poles(ii).ChangeCoord() = pnt;
     Weights(ii) = wi;
  }
}

void GeomFill_QuasiAngularConvertor::Section(const gp_Pnt& FirstPnt,
				       const gp_Vec& DFirstPnt,
				       const gp_Pnt& Center,
				       const gp_Vec& DCenter,
				       const gp_Vec& Dir,
				       const gp_Vec& DDir,
				       const Standard_Real Angle,
				       const Standard_Real DAngle,
				       TColgp_Array1OfPnt& Poles,
				       TColgp_Array1OfVec& DPoles,
				       TColStd_Array1OfReal& Weights,
				       TColStd_Array1OfReal& DWeights) 
{
  Standard_Integer Ordre = 7;
  math_Vector DVx(1, Ordre), DVy(1, Ordre), DVw(1, Ordre),
              DPx(1, Ordre), DPy(1, Ordre), DW(1, Ordre);
  Standard_Real b, b2, c, c2,tan_b;
  Standard_Real bpr, dtan_b;
  Standard_Integer ii;
  Standard_Real beta, beta2, beta3, beta4, beta5, beta6, betaprim;
  gp_Vec V1(Center, FirstPnt), V1Prim, V2;

  // Calcul des  transformations
  gp_XYZ aux;
  Standard_Real Sina, Cosa;
  gp_Mat Rot, RotPrim, D, DPrim;
  // La rotation s'ecrit I +  sin(Ang) * D + (1. - cos(Ang)) * D*D
  // ou D est l'application x -> Dir ^ x
  Rot.SetRotation(Dir.XYZ(), Angle/2);
  // La derive s'ecrit donc :
  // AngPrim * (sin(Ang)*D*D + cos(Ang)*D) 
  // + sin(Ang)*DPrim  + (1. - cos(Ang)) *(DPrim*D + D*DPrim)
  Sina = Sin(Angle/2); 
  Cosa = Cos(Angle/2);
  D.SetCross(Dir.XYZ());
  DPrim.SetCross(DDir.XYZ());

  RotPrim =  (D.Powered(2)).Multiplied(Sina);
  RotPrim += D.Multiplied(Cosa);
  RotPrim *= DAngle/2;
  RotPrim += DPrim.Multiplied(Sina);
  RotPrim += ((DPrim.Multiplied(D)).Added(D.Multiplied(DPrim))).Multiplied(1-Cosa);
  
  aux = (DFirstPnt - DCenter).XYZ().Multiplied(Rot);
  aux += V1.XYZ().Multiplied(RotPrim);
  V1Prim.SetXYZ(aux);
  aux = V1.XYZ();
  aux.Multiply(Rot);
  V1.SetXYZ(aux);
  V2 =  Dir^V1;
  gp_Mat M (V1.X(), V2.X(), 0,
	    V1.Y(), V2.Y(), 0,
	    V1.Z(), V2.Z(), 0);
  V2 = (DDir.Crossed(V1)).Added(Dir.Crossed(V1Prim));
  gp_Mat MPrim (V1Prim.X(), V2.X(), 0,
		V1Prim.Y(), V2.Y(), 0,
		V1Prim.Z(), V2.Z(), 0);

  // Calcul des constante  -----------
  beta = Angle/4;
  betaprim = DAngle/4;
  beta2 = beta * beta;
  beta3 = beta * beta2;
  beta4 = beta2*beta2;
  beta5 = beta3*beta2;
  beta6 = beta3*beta3;

  if (Abs(beta) < NullAngle) {
    // On calcul b par D.L
     Standard_Real cf = 2.0/(3*5*7);
     Standard_Real Num, Denom;
     Num = 0.2 + cf*beta2;
     Denom = 1+0.2*beta2;
     b = - Num/Denom;
     bpr = -2*beta*betaprim*(cf*Denom - 0.2*Num)/(Denom*Denom);
  }
  else {
    b = ((Standard_Real) -1)/beta2;
    bpr = (2*betaprim) / beta3;  
    if  ((M_PI/2 - beta)> NullAngle) {
      tan_b = Tan(beta);
      dtan_b = betaprim * (1 + tan_b*tan_b);
      b2 = tan_b - beta;
      b += beta / (3*b2);
      bpr += (betaprim*tan_b - beta*dtan_b) / (3*b2*b2);
    }
  }

  c =  ((Standard_Real) 1)/ 3 + b;
  b2 = b*b;
  c2 = c*c;


  // X = U*U - V*V
  Vx(3) = beta2*(2*b - 1);
  Vx(5) = beta4*(b2 - 2*c); 
  Vx(7) = -beta6*c2;
  DVx.Init(0);
  DVx(3) = 2*(beta*betaprim*(2*b - 1) + bpr*beta2);
  DVx(5) = 4*beta3*betaprim*(b2 - 2*c) + 2*beta4*bpr*(b-1); 
  DVx(7) = - 6*beta5*betaprim*c2 - 2*beta6*bpr*c;  

  //Y = 2*U*V
  Vy(2) = 2*beta;
  Vy(4) = beta3*2*(c+b);
  Vy(6) = 2*beta5*b*c;
  DVy.Init(0);
  DVy(2) = 2*betaprim;
  DVy(4) = 6*beta2*betaprim*(b+c) + 4*beta3*bpr;
  DVy(6) = 10*beta4*betaprim*b*c + 2*beta5*bpr*(b+c);

 // W = U*U + V*V
  Vw(3) = beta2*(1 + 2*b);
  Vw(5) = beta4*(2*c + b2); 
  Vw(7) = beta6*c2;   
  DVw.Init(0);
//  DVw(3) = 2*(beta*betaprim*(1 + 2*b) + beta2*bpr);
  DVw(3) = 2*beta*(betaprim*(1 + 2*b) + beta*bpr);
//  DVw(5) = 4*beta3*betaprim*(2*c + b2) + 2*beta4*bpr*(b+1);
  DVw(5) = 2*beta3*(2*betaprim*(2*c + b2) + beta*bpr*(b+1)); 
//  DVw(7) = 6*beta5*betaprim*c2 + 2*beta6*bpr*c;
  DVw(7) = 2*beta5*c*(3*betaprim*c + beta*bpr);

 

  // Calcul des poles
  Px.Multiply(B, Vx);
  Py.Multiply(B, Vy);
  W.Multiply(B, Vw);
  DPx.Multiply(B, DVx);
  DPy.Multiply(B, DVy);
  DW.Multiply(B, DVw);

  gp_XYZ P, DP;
  Standard_Real wi;

  for (ii=1; ii<=Ordre; ii++) {
    wi = W(ii);
    P.SetCoord(Px(ii)/wi, Py(ii)/wi, 0);
    DP.SetCoord(DPx(ii)/wi, DPy(ii)/wi, 0);
    DP -= (DW(ii)/wi)*P;

    Poles(ii).ChangeCoord() = M*P + Center.XYZ();
    P *= MPrim;
    DP *= M;
    aux.SetLinearForm(1, P, 1, DP, DCenter.XYZ());
    DPoles(ii).SetXYZ(aux);
    Weights(ii) = wi;
    DWeights(ii) = DW(ii);
  }
}

void GeomFill_QuasiAngularConvertor::Section(const gp_Pnt& FirstPnt,
				       const gp_Vec& DFirstPnt,
				       const gp_Vec& D2FirstPnt,
				       const gp_Pnt& Center,
				       const gp_Vec& DCenter,
				       const gp_Vec& D2Center,
				       const gp_Vec& Dir,
				       const gp_Vec& DDir,
				       const gp_Vec& D2Dir,
				       const Standard_Real Angle,
				       const Standard_Real DAngle,
				       const Standard_Real D2Angle,
				       TColgp_Array1OfPnt& Poles,
				       TColgp_Array1OfVec& DPoles,
				       TColgp_Array1OfVec& D2Poles,
				       TColStd_Array1OfReal& Weights,
				       TColStd_Array1OfReal& DWeights,
				       TColStd_Array1OfReal& D2Weights) 
{
  Standard_Integer Ordre = 7;
  math_Vector DVx(1, Ordre), DVy(1, Ordre),  DVw(1, Ordre),
              D2Vx(1, Ordre), D2Vy(1, Ordre),  D2Vw(1, Ordre);
  math_Vector DPx(1, Ordre), DPy(1, Ordre), DW(1, Ordre),
              D2Px(1, Ordre), D2Py(1, Ordre), D2W(1, Ordre);
 
  Standard_Integer ii;
  Standard_Real daux, b, b2, c, c2, bpr, bsc;
  gp_Vec V1(Center, FirstPnt), V1Prim, V1Secn, V2;

  // Calcul des  transformations
  gp_XYZ auxyz;
  Standard_Real Sina, Cosa;
  gp_Mat Rot, RotPrim, RotSecn, D, DPrim, DSecn, DDP, Maux;
  // La rotation s'ecrit I +  sin(Ang) * D + (1. - cos(Ang)) * D*D
  // ou D est l'application x -> Dir ^ x
  Rot.SetRotation(Dir.XYZ(), Angle/2);
  // La derive s'ecrit donc :
  // AngPrim * (sin(Ang)*D*D + cos(Ang)*D) 
  // + sin(Ang)*DPrim  + (1. - cos(Ang)) *(DPrim*D + D*DPrim)
  Sina = Sin(Angle/2); 
  Cosa = Cos(Angle/2);
  D.SetCross(Dir.XYZ());
  DPrim.SetCross(DDir.XYZ());
  DSecn.SetCross(D2Dir.XYZ());
  
  DDP = (DPrim.Multiplied(D)).Added(D.Multiplied(DPrim));
  RotPrim =  (D.Powered(2)).Multiplied(Sina);
  RotPrim += D.Multiplied(Cosa);
  RotPrim *= DAngle/2;
  RotPrim += DPrim.Multiplied(Sina);
  RotPrim += DDP.Multiplied(1-Cosa);

  RotSecn =  (D.Powered(2)).Multiplied(Sina);
  RotSecn += D.Multiplied(Cosa);
  RotSecn *= D2Angle/2;
  Maux = (D.Powered(2)).Multiplied(Cosa);
  Maux -= D.Multiplied(Sina);
  Maux *= DAngle/2;
  Maux += DDP.Multiplied(2*Sina);
  Maux += DPrim.Multiplied(2*Cosa);
  Maux *= DAngle/2;
  RotSecn += Maux;
  Maux = (DSecn.Multiplied(D)).Added(D.Multiplied(DSecn));
  Maux +=  (DPrim.Powered(2)).Multiplied(2);
  Maux *= 1 - Cosa;
  Maux += DSecn.Multiplied(Sina);
  RotSecn += Maux;
  
  V1Prim = DFirstPnt - DCenter;
  auxyz = (D2FirstPnt - D2Center).XYZ().Multiplied(Rot);
  auxyz += 2*(V1Prim.XYZ().Multiplied(RotPrim));
  auxyz += V1.XYZ().Multiplied(RotSecn);
  V1Secn.SetXYZ(auxyz);
  auxyz = V1Prim.XYZ().Multiplied(Rot);
  auxyz += V1.XYZ().Multiplied(RotPrim);
  V1Prim.SetXYZ(auxyz);
  auxyz = V1.XYZ();
  auxyz.Multiply(Rot);
  V1.SetXYZ(auxyz);
  V2 =  Dir^V1;

  gp_Mat M (V1.X(), V2.X(), 0,
	    V1.Y(), V2.Y(), 0,
	    V1.Z(), V2.Z(), 0);
  V2 = (DDir.Crossed(V1)).Added(Dir.Crossed(V1Prim));
  gp_Mat MPrim (V1Prim.X(), V2.X(), 0,
		V1Prim.Y(), V2.Y(), 0,
		V1Prim.Z(), V2.Z(), 0);
 
  V2 = DDir^V1Prim;
  V2 *= 2;
  V2 += (D2Dir.Crossed(V1)).Added(Dir.Crossed(V1Secn));
  gp_Mat MSecn (V1Secn.X(), V2.X(), 0,
		V1Secn.Y(), V2.Y(), 0,
		V1Secn.Z(), V2.Z(), 0);

     
  // Calcul des coeff  -----------
  Standard_Real tan_b, dtan_b, d2tan_b;
  Standard_Real beta, beta2, beta3, beta4, beta5, beta6, betaprim, betasecn;
  Standard_Real betaprim2, bpr2;
  beta = Angle/4;
  betaprim = DAngle/4;
  betasecn = D2Angle/4;
  beta2 = beta * beta;
  beta3 = beta * beta2;
  beta4 = beta2*beta2;
  beta5 = beta3*beta2;
  beta6 = beta3*beta3;
  betaprim2 =  betaprim * betaprim;

 if (Abs(beta) < NullAngle) {
    // On calcul b par D.L
     Standard_Real cf =-2.0/21;
     Standard_Real Num, Denom, aux;
     Num = 0.2 + cf*beta2;
     Denom = 1+0.2*beta2;
     aux = (cf*Denom - 0.2*Num)/(Denom*Denom);
     b = - Num/Denom;
     bpr = -2*beta*betaprim*aux;
     bsc = 2*aux*(betaprim2 + beta*betasecn - 2*beta*betaprim2);
    
  }
  else { 
    b = ((Standard_Real) -1)/beta2;
    bpr = (2*betaprim) / beta3;
    bsc = (2*betasecn - 6*betaprim*(betaprim/beta)) / beta3; 
    if  ((M_PI/2 - beta)> NullAngle) {
      tan_b = Tan(beta);
      dtan_b = betaprim * (1 + tan_b*tan_b);
      d2tan_b = betasecn * (1 + tan_b*tan_b)
	+ 2*betaprim * tan_b * dtan_b;
      b2 = tan_b - beta;
      b += beta / (3*b2);
      Standard_Real aux =  betaprim*tan_b - beta*dtan_b;
      bpr += aux / (3*b2*b2);
      daux =  betasecn*tan_b - beta*d2tan_b;
      bsc += (daux - 2*aux*betaprim*tan_b*tan_b/b2)/(3*b2*b2);
    }
  }

  c =  ((Standard_Real) 1)/ 3 + b;
  b2 = b*b;
  c2 = c*c;
  bpr2 = bpr * bpr;


  // X = U*U - V*V
  Vx(3) = beta2*(2*b - 1);
  Vx(5) = beta4*(b2 - 2*c); 
  Vx(7) = -beta6*c2;
  DVx.Init(0);
  DVx(3) = 2*(beta*betaprim*(2*b - 1) + bpr*beta2);
  DVx(5) = 4*beta3*betaprim*(b2 - 2*c) + 2*beta4*bpr*(b-1); 
  DVx(7) = - 6*beta5*betaprim*c2 - 2*beta6*bpr*c;
  D2Vx.Init(0);  
  D2Vx(3) = 2*((betaprim2+beta*betasecn)*(2*b - 1) 
	       + 8*beta*betaprim*bpr 
	       + bsc*beta2);
  D2Vx(5) =  4*(b2 - 2*c)*(3*beta2*betaprim2 + beta3*betasecn)
             + 16*beta3*betaprim*bpr*(b-1)
             + 2*beta4*(bsc*(b-1)+bpr2);
  D2Vx(7) = - 6 * c2 * (5*beta4*betaprim2+beta5*betasecn)
            - 24*beta5*betaprim*bpr*c
	    - 2*beta6*(bsc*c + bpr2);

  //Y = 2*U*V
  Vy(2) = 2*beta;
  Vy(4) = beta3*2*(c+b);
  Vy(6) = 2*beta5*b*c;
  DVy.Init(0);
  DVy(2) = 2*betaprim;
  DVy(4) = 6*beta2*betaprim*(b+c) + 4*beta3*bpr;
  DVy(6) = 10*beta4*betaprim*b*c + 2*beta5*bpr*(b+c);
  D2Vy.Init(0);
  D2Vy(2) = 2*betasecn;
  D2Vy(4) = 6*(b+c)*(2*beta*betaprim2 + beta2*betasecn) 
          + 24*beta2*betaprim*bpr*(b+c)
	  + 4*beta3*bsc;
  D2Vy(6) = 10*b*c*(4*beta3*betaprim2 + beta4*betasecn)
          + 40 * beta4*betaprim*bpr*(b+c)
	  + 2*beta5*(bsc*(b+c)+ 2*bpr2);

 // W = U*U + V*V
  Vw(3) = beta2*(1 + 2*b);
  Vw(5) = beta4*(2*c + b2); 
  Vw(7) = beta6*c2;   
  DVw.Init(0);
  DVw(3) = 2*(beta*betaprim*(1 + 2*b) + beta2*bpr);
  DVw(5) = 4*beta3*betaprim*(2*c + b2) + 2*beta4*bpr*(b+1); 
  DVw(7) = 6*beta5*betaprim*c2 + 2*beta6*bpr*c;
  D2Vw.Init(0);
  D2Vw(3) = 2*((betaprim2+beta*betasecn)*(2*b + 1) 
	       + 8*beta*betaprim*bpr 
	       + bsc*beta2);
  D2Vw(5) = 4*(b2 + 2*c)*(3*beta2*betaprim2 + beta3*betasecn)
             + 16*beta3*betaprim*bpr*(b+11)
             + 2*beta4*(bsc*(b+1)+bpr2); 
  D2Vw(7) = 6 * c2 * (5*beta4*betaprim2+beta5*betasecn)
            + 24*beta5*betaprim*bpr*c
	    + 2*beta6*(bsc*c + bpr2);
 

  // Calcul des poles
  Px = B * Vx;
  Py = B * Vy;
  W.Multiply(B, Vw);
  DPx = B * DVx;
  DPy = B * DVy;
  DW.Multiply(B, DVw);
  D2Px = B * D2Vx;
  D2Py = B * D2Vy;
  D2W.Multiply(B, D2Vw);

  gp_XYZ P, DP, D2P;
  Standard_Real wi, dwi;
  for (ii=1; ii<=Ordre; ii++) {
     wi = W(ii);
     dwi = DW(ii);
     P.SetCoord(Px(ii)/wi, Py(ii)/wi, 0);
     DP.SetCoord(DPx(ii)/wi, DPy(ii)/wi, 0);
   
     D2P.SetCoord(D2Px(ii)/wi, D2Py(ii)/wi, 0);
     D2P -= 2*(dwi/wi)*DP;
     D2P += (2*Pow(dwi/wi, 2) - D2W(ii)/wi)*P;
     DP -= (DW(ii)/wi)*P;  

     Poles(ii).ChangeCoord() = M*P + Center.XYZ();
     auxyz.SetLinearForm(1, MPrim*P, 
			 1, M*DP, 
			 DCenter.XYZ());  
     DPoles(ii).SetXYZ(auxyz);
     P  *= MSecn;
     DP *= MPrim;
     D2P*= M;
     auxyz.SetLinearForm(1, P, 
			 2, DP, 
			 1, D2P,
			 D2Center.XYZ());
     D2Poles(ii).SetXYZ(auxyz);
     Weights(ii) = wi;
     DWeights(ii) = dwi; 
     D2Weights(ii) = D2W(ii);     
  }
}
