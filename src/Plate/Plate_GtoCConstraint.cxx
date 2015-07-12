// Created on: 1995-10-20
// Created by: Andre LIEUTIER
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


#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Plate_D1.hxx>
#include <Plate_D2.hxx>
#include <Plate_D3.hxx>
#include <Plate_GtoCConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>

//alr le 12/11/96
static const Standard_Real NORMIN = 1.e-10;
static const Standard_Real COSMIN = 1.e-2;

Plate_GtoCConstraint::Plate_GtoCConstraint(const Plate_GtoCConstraint& ref)
:myD1SurfInit(ref.myD1SurfInit)
{
  pnt2d = ref.pnt2d;  
  nb_PPConstraints = ref.nb_PPConstraints;
  for(Standard_Integer i = 0; i<nb_PPConstraints; i++)
    myPPC[i] = ref.myPPC[i];
}

Plate_GtoCConstraint::Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T)
:myD1SurfInit(D1S)
{
  pnt2d = point2d;
  nb_PPConstraints = 0;

  gp_XYZ normale = D1T.Du^D1T.Dv;
//alr le 12/11/96
  if(normale.Modulus() < NORMIN) return;
  normale.Normalize();

// G1 Constraints

  gp_XYZ normaleS = D1S.Du^D1S.Dv;
//alr le 12/11/96
  if(normaleS.Modulus() < NORMIN) return;
  normaleS.Normalize();
  Standard_Real cos_normales = normale*normaleS;
  if( fabs(cos_normales)<COSMIN) return;
  Standard_Real invcos = 1./cos_normales;

  gp_XYZ du = normaleS* -(normale*D1S.Du)*invcos;
  gp_XYZ dv = normaleS* -(normale*D1S.Dv)*invcos;


  myPPC[0] = Plate_PinpointConstraint(pnt2d, du,1,0);
  myPPC[1] = Plate_PinpointConstraint(pnt2d, dv,0,1);

  nb_PPConstraints = 2;
}

Plate_GtoCConstraint::Plate_GtoCConstraint(const gp_XY& point2d, 
  const Plate_D1& D1S, const Plate_D1& D1T, const gp_XYZ& nP)
:myD1SurfInit(D1S)
{
  pnt2d = point2d;
  nb_PPConstraints = 0;

  gp_XYZ normale = D1T.Du^D1T.Dv;
  if(normale.Modulus() < NORMIN) return;
  normale.Normalize();

// G1 Constraints

  gp_XYZ normaleS = D1S.Du^D1S.Dv;
  if(normaleS.Modulus() < NORMIN) return;
  normaleS.Normalize();
  
  gp_XYZ nSP = normaleS - nP*(nP*normaleS);
  if(nSP.Modulus() < NORMIN) return;
  nSP.Normalize();

  Standard_Real cos_normales = normale*nSP;
  if( fabs(cos_normales)<COSMIN) return;
  Standard_Real invcos = 1./cos_normales;

  gp_XYZ du = nSP* -(normale*D1S.Du)*invcos;
  gp_XYZ dv = nSP* -(normale*D1S.Dv)*invcos;


  myPPC[0] = Plate_PinpointConstraint(pnt2d, du,1,0);
  myPPC[1] = Plate_PinpointConstraint(pnt2d, dv,0,1);

  nb_PPConstraints = 2;
}

Plate_GtoCConstraint::Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, 
					   const Plate_D2& D2S, const Plate_D2& D2T)
:myD1SurfInit(D1S)
{

  pnt2d = point2d;
  nb_PPConstraints = 0;

  gp_XYZ normale = D1T.Du^D1T.Dv;
//alr le 12/11/96
  if(normale.Modulus() < NORMIN) return;
  normale.Normalize();

// G1 Constraints
  gp_XYZ normaleS = D1S.Du^D1S.Dv;
//alr le 12/11/96
  if(normaleS.Modulus() < NORMIN) return;
  normaleS.Normalize();

  Standard_Real cos_normales = normale*normaleS;
  if( fabs(cos_normales)<COSMIN) return;
  Standard_Real invcos = 1./cos_normales;

  gp_XYZ du = normaleS* -(normale*D1S.Du)*invcos;
  gp_XYZ dv = normaleS* -(normale*D1S.Dv)*invcos;

  myPPC[0] = Plate_PinpointConstraint(pnt2d, du,1,0);
  myPPC[1] = Plate_PinpointConstraint(pnt2d, dv,0,1);
  nb_PPConstraints = 2;

// G2 Constraints  
  gp_XYZ Su = D1S.Du+du;
  gp_XYZ Sv = D1S.Dv+dv;

  math_Matrix mat(0,1,0,1);
  mat(0,0) = Su*D1T.Du;
  mat(0,1) = Su*D1T.Dv;
  mat(1,0) = Sv*D1T.Du;
  mat(1,1) = Sv*D1T.Dv;
  math_Gauss gauss(mat);
  if(!gauss.IsDone()) return;

  math_Vector vec(0,1);
  vec(0) = Su*Su;
  vec(1) = Su*Sv;
  math_Vector sol(0,1);

  gauss.Solve(vec,sol);
  Standard_Real a = sol(0);
  Standard_Real b = sol(1);

  vec(0) = Sv*Su;
  vec(1) = Sv*Sv;

  gauss.Solve(vec,sol);
  Standard_Real c = sol(0);
  Standard_Real d = sol(1);

  gp_XYZ Suu = D2T.Duu*(a*a) + D2T.Duv*(2*a*b) + D2T.Dvv*(b*b);
  gp_XYZ Suv = D2T.Duu*(a*c) + D2T.Duv*(a*d+b*c) + D2T.Dvv*(b*d);
  gp_XYZ Svv = D2T.Duu*(c*c) + D2T.Duv*(2*c*d) + D2T.Dvv*(d*d);  

  gp_XYZ duu = normaleS * (normale*(Suu-D2S.Duu))*invcos;
  gp_XYZ duv = normaleS * (normale*(Suv-D2S.Duv))*invcos;
  gp_XYZ dvv = normaleS * (normale*(Svv-D2S.Dvv))*invcos;


  myPPC[2] = Plate_PinpointConstraint(pnt2d, duu,2,0);
  myPPC[3] = Plate_PinpointConstraint(pnt2d, duv,1,1);
  myPPC[4] = Plate_PinpointConstraint(pnt2d, dvv,0,2);
  nb_PPConstraints = 5;
}



Plate_GtoCConstraint::Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, 
					   const Plate_D2& D2S, const Plate_D2& D2T, const gp_XYZ& nP)
:myD1SurfInit(D1S)
{

  pnt2d = point2d;
  nb_PPConstraints = 0;

  gp_XYZ normale = D1T.Du^D1T.Dv;
//alr le 12/11/96
  if(normale.Modulus() < NORMIN) return;
  normale.Normalize();

// G1 Constraints
  gp_XYZ normaleS = D1S.Du^D1S.Dv;
//alr le 12/11/96
  if(normaleS.Modulus() < NORMIN) return;
  normaleS.Normalize();

  gp_XYZ nSP = normaleS - nP*(nP*normaleS);
  if(nSP.Modulus() < NORMIN) return;
  nSP.Normalize();

  Standard_Real cos_normales = normale*nSP;
  if( fabs(cos_normales)<COSMIN) return;
  Standard_Real invcos = 1./cos_normales;

  gp_XYZ du = nSP* -(normale*D1S.Du)*invcos;
  gp_XYZ dv = nSP* -(normale*D1S.Dv)*invcos;

  myPPC[0] = Plate_PinpointConstraint(pnt2d, du,1,0);
  myPPC[1] = Plate_PinpointConstraint(pnt2d, dv,0,1);
  nb_PPConstraints = 2;

// G2 Constraints  
  gp_XYZ Su = D1S.Du+du;
  gp_XYZ Sv = D1S.Dv+dv;

  math_Matrix mat(0,1,0,1);
  mat(0,0) = Su*D1T.Du;
  mat(0,1) = Su*D1T.Dv;
  mat(1,0) = Sv*D1T.Du;
  mat(1,1) = Sv*D1T.Dv;
  math_Gauss gauss(mat);
  if(!gauss.IsDone()) return;

  math_Vector vec(0,1);
  vec(0) = Su*Su;
  vec(1) = Su*Sv;
  math_Vector sol(0,1);

  gauss.Solve(vec,sol);
  Standard_Real a = sol(0);
  Standard_Real b = sol(1);

  vec(0) = Sv*Su;
  vec(1) = Sv*Sv;

  gauss.Solve(vec,sol);
  Standard_Real c = sol(0);
  Standard_Real d = sol(1);

  gp_XYZ Suu = D2T.Duu*(a*a) + D2T.Duv*(2*a*b) + D2T.Dvv*(b*b);
  gp_XYZ Suv = D2T.Duu*(a*c) + D2T.Duv*(a*d+b*c) + D2T.Dvv*(b*d);
  gp_XYZ Svv = D2T.Duu*(c*c) + D2T.Duv*(2*c*d) + D2T.Dvv*(d*d);  

  gp_XYZ duu = nSP * (normale*(Suu-D2S.Duu))*invcos;
  gp_XYZ duv = nSP * (normale*(Suv-D2S.Duv))*invcos;
  gp_XYZ dvv = nSP * (normale*(Svv-D2S.Dvv))*invcos;


  myPPC[2] = Plate_PinpointConstraint(pnt2d, duu,2,0);
  myPPC[3] = Plate_PinpointConstraint(pnt2d, duv,1,1);
  myPPC[4] = Plate_PinpointConstraint(pnt2d, dvv,0,2);
  nb_PPConstraints = 5;
}


Plate_GtoCConstraint::Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, 
					   const Plate_D2& D2S, const Plate_D2& D2T,
					   const Plate_D3& D3S, const Plate_D3& D3T)
:myD1SurfInit(D1S)
{
  pnt2d = point2d;
  nb_PPConstraints = 0;

  gp_XYZ normale = D1T.Du^D1T.Dv;
  if(normale.Modulus() < NORMIN) return;
  normale.Normalize();

// G1 Constraints
  gp_XYZ normaleS = D1S.Du^D1S.Dv;
  if(normaleS.Modulus() < NORMIN) return;
  normaleS.Normalize();

  Standard_Real cos_normales = normale*normaleS;
  if( fabs(cos_normales)<COSMIN) return;
  Standard_Real invcos = 1./cos_normales;

  gp_XYZ du = normaleS* -(normale*D1S.Du)*invcos;
  gp_XYZ dv = normaleS* -(normale*D1S.Dv)*invcos;

  myPPC[0] = Plate_PinpointConstraint(pnt2d, du,1,0);
  myPPC[1] = Plate_PinpointConstraint(pnt2d, dv,0,1);
  nb_PPConstraints = 2;

// G2 Constraints  
  gp_XYZ Su = D1S.Du+du;
  gp_XYZ Sv = D1S.Dv+dv;

  math_Matrix mat(0,1,0,1);
  mat(0,0) = Su*D1T.Du;
  mat(0,1) = Su*D1T.Dv;
  mat(1,0) = Sv*D1T.Du;
  mat(1,1) = Sv*D1T.Dv;
  math_Gauss gauss(mat);
  if(!gauss.IsDone()) return;

  math_Vector vec(0,1);
  vec(0) = Su*Su;
  vec(1) = Su*Sv;
  math_Vector sol(0,1);

  gauss.Solve(vec,sol);
  Standard_Real a = sol(0);
  Standard_Real b = sol(1);

  vec(0) = Sv*Su;
  vec(1) = Sv*Sv;

  gauss.Solve(vec,sol);
  Standard_Real c = sol(0);
  Standard_Real d = sol(1);

  gp_XYZ Suu = D2T.Duu*(a*a) + D2T.Duv*(2*a*b)   + D2T.Dvv*(b*b);
  gp_XYZ Suv = D2T.Duu*(a*c) + D2T.Duv*(a*d+b*c) + D2T.Dvv*(b*d);
  gp_XYZ Svv = D2T.Duu*(c*c) + D2T.Duv*(2*c*d)   + D2T.Dvv*(d*d);  

  gp_XYZ duu = normaleS * (normale*(Suu-D2S.Duu))*invcos;
  gp_XYZ duv = normaleS * (normale*(Suv-D2S.Duv))*invcos;
  gp_XYZ dvv = normaleS * (normale*(Svv-D2S.Dvv))*invcos;


  myPPC[2] = Plate_PinpointConstraint(pnt2d, duu,2,0);
  myPPC[3] = Plate_PinpointConstraint(pnt2d, duv,1,1);
  myPPC[4] = Plate_PinpointConstraint(pnt2d, dvv,0,2);
  nb_PPConstraints = 5;

// G3 Constraints  

  vec(0) = (D2S.Duu + duu - Suu)*Su;
  vec(1) = (D2S.Duu + duu - Suu)*Sv;
  gauss.Solve(vec,sol);
  Standard_Real B0uu = sol(0);
  Standard_Real B1uu = sol(1);

  vec(0) = (D2S.Duv + duv - Suv)*Su;
  vec(1) = (D2S.Duv + duv - Suv)*Sv;
  gauss.Solve(vec,sol);
  Standard_Real B0uv = sol(0);
  Standard_Real B1uv = sol(1);

  vec(0) = (D2S.Dvv + dvv - Svv)*Su;
  vec(1) = (D2S.Dvv + dvv - Svv)*Sv;
  gauss.Solve(vec,sol);
  Standard_Real B0vv = sol(0);
  Standard_Real B1vv = sol(1);

  gp_XYZ Suuu = D3T.Duuu*(a*a*a) + D3T.Duuv*(3*a*a*b)       + D3T.Duvv*(3*a*b*b)       + D3T.Dvvv*(b*b*b);
  gp_XYZ Suuv = D3T.Duuu*(a*a*c) + D3T.Duuv*(a*a*d+2*a*b*c) + D3T.Duvv*(b*b*c+2*a*b*d) + D3T.Dvvv*(b*b*d);
  gp_XYZ Suvv = D3T.Duuu*(a*c*c) + D3T.Duuv*(b*c*c+2*a*c*d) + D3T.Duvv*(a*d*d+2*b*c*d) + D3T.Dvvv*(b*d*d);
  gp_XYZ Svvv = D3T.Duuu*(c*c*c) + D3T.Duuv*(3*c*c*d)       + D3T.Duvv*(3*c*d*d)       + D3T.Dvvv*(d*d*d);

  Standard_Real &A0u = a;
  Standard_Real &A1u = b;
  Standard_Real &A0v = c;
  Standard_Real &A1v = d;
  Suuu += D2T.Duu*(3*A0u*B0uu)          + D2T.Duv*(3*(A0u*B1uu+A1u*B0uu))                     + D2T.Dvv*(3*A1u*B1uu);
  Suuv += D2T.Duu*(2*A0u*B0uv+A0v*B0uu) + D2T.Duv*(2*(A0u*B1uv+A1u*B0uv)+A0v*B1uu+A1v*B0uu)   + D2T.Dvv*(2*A1u*B1uv+A1v*B1uu);
  Suvv += D2T.Duu*(A0u*B0vv+2*A0v*B0uv) + D2T.Duv*(2*(A0v*B1uv+A1v*B0uv)+A0u*B1vv+A1u*B0vv)   + D2T.Dvv*(2*A1v*B1uv+A1u*B1vv);
  Svvv += D2T.Duu*(3*A0v*B0vv)          + D2T.Duv*(3*(A0v*B1vv+A1v*B0vv))                     + D2T.Dvv*(3*A1v*B1vv);

  gp_XYZ duuu = normaleS * (normale*(Suuu-D3S.Duuu))*invcos;
  gp_XYZ duuv = normaleS * (normale*(Suuv-D3S.Duuv))*invcos;
  gp_XYZ duvv = normaleS * (normale*(Suvv-D3S.Duvv))*invcos;
  gp_XYZ dvvv = normaleS * (normale*(Svvv-D3S.Dvvv))*invcos;

  myPPC[5] = Plate_PinpointConstraint(pnt2d, duuu,3,0);
  myPPC[6] = Plate_PinpointConstraint(pnt2d, duuv,2,1);
  myPPC[7] = Plate_PinpointConstraint(pnt2d, duvv,1,2);
  myPPC[8] = Plate_PinpointConstraint(pnt2d, dvvv,0,3);
  nb_PPConstraints = 9;
}


Plate_GtoCConstraint::Plate_GtoCConstraint(const gp_XY& point2d, const Plate_D1& D1S, const Plate_D1& D1T, 
					   const Plate_D2& D2S, const Plate_D2& D2T,
					   const Plate_D3& D3S, const Plate_D3& D3T,
//                                           const gp_XYZ& nP)
                                           const gp_XYZ& )
:myD1SurfInit(D1S)
{

  pnt2d = point2d;
  nb_PPConstraints = 0;

  gp_XYZ normale = D1T.Du^D1T.Dv;
  if(normale.Modulus() < NORMIN) return;
  normale.Normalize();

// G1 Constraints
  gp_XYZ normaleS = D1S.Du^D1S.Dv;
  if(normaleS.Modulus() < NORMIN) return;
  normaleS.Normalize();

  Standard_Real cos_normales = normale*normaleS;
  if( fabs(cos_normales)<COSMIN) return;
  Standard_Real invcos = 1./cos_normales;

  gp_XYZ du = normaleS* -(normale*D1S.Du)*invcos;
  gp_XYZ dv = normaleS* -(normale*D1S.Dv)*invcos;

  myPPC[0] = Plate_PinpointConstraint(pnt2d, du,1,0);
  myPPC[1] = Plate_PinpointConstraint(pnt2d, dv,0,1);
  nb_PPConstraints = 2;

// G2 Constraints  
  gp_XYZ Su = D1S.Du+du;
  gp_XYZ Sv = D1S.Dv+dv;

  math_Matrix mat(0,1,0,1);
  mat(0,0) = Su*D1T.Du;
  mat(0,1) = Su*D1T.Dv;
  mat(1,0) = Sv*D1T.Du;
  mat(1,1) = Sv*D1T.Dv;
  math_Gauss gauss(mat);

  if(!gauss.IsDone()) return;

  math_Vector vec(0,1);
  vec(0) = Su*Su;
  vec(1) = Su*Sv;
  math_Vector sol(0,1);

  gauss.Solve(vec,sol);
  Standard_Real a = sol(0);
  Standard_Real b = sol(1);

  vec(0) = Sv*Su;
  vec(1) = Sv*Sv;

  gauss.Solve(vec,sol);
  Standard_Real c = sol(0);
  Standard_Real d = sol(1);

  gp_XYZ Suu = D2T.Duu*(a*a) + D2T.Duv*(2*a*b)   + D2T.Dvv*(b*b);
  gp_XYZ Suv = D2T.Duu*(a*c) + D2T.Duv*(a*d+b*c) + D2T.Dvv*(b*d);
  gp_XYZ Svv = D2T.Duu*(c*c) + D2T.Duv*(2*c*d)   + D2T.Dvv*(d*d);  

  gp_XYZ duu = normaleS * (normale*(Suu-D2S.Duu))*invcos;
  gp_XYZ duv = normaleS * (normale*(Suv-D2S.Duv))*invcos;
  gp_XYZ dvv = normaleS * (normale*(Svv-D2S.Dvv))*invcos;

  myPPC[2] = Plate_PinpointConstraint(pnt2d, duu,2,0);
  myPPC[3] = Plate_PinpointConstraint(pnt2d, duv,1,1);
  myPPC[4] = Plate_PinpointConstraint(pnt2d, dvv,0,2);
  nb_PPConstraints = 5;

// G3 Constraints  
  vec(0) = (D2S.Duu + duu - Suu)*Su;
  vec(1) = (D2S.Duu + duu - Suu)*Sv;
  gauss.Solve(vec,sol);
  Standard_Real B0uu = sol(0);
  Standard_Real B1uu = sol(1);

  vec(0) = (D2S.Duv + duv - Suv)*Su;
  vec(1) = (D2S.Duv + duv - Suv)*Sv;
  gauss.Solve(vec,sol);
  Standard_Real B0uv = sol(0);
  Standard_Real B1uv = sol(1);

  vec(0) = (D2S.Dvv + dvv - Svv)*Su;
  vec(1) = (D2S.Dvv + dvv - Svv)*Sv;
  gauss.Solve(vec,sol);
  Standard_Real B0vv = sol(0);
  Standard_Real B1vv = sol(1);

  gp_XYZ Suuu = D3T.Duuu*(a*a*a) + D3T.Duuv*(3*a*a*b)       + D3T.Duvv*(3*a*b*b)       + D3T.Dvvv*(b*b*b);
  gp_XYZ Suuv = D3T.Duuu*(a*a*c) + D3T.Duuv*(a*a*d+2*a*b*c) + D3T.Duvv*(b*b*c+2*a*b*d) + D3T.Dvvv*(b*b*d);
  gp_XYZ Suvv = D3T.Duuu*(a*c*c) + D3T.Duuv*(b*c*c+2*a*c*d) + D3T.Duvv*(a*d*d+2*b*c*d) + D3T.Dvvv*(b*d*d);
  gp_XYZ Svvv = D3T.Duuu*(c*c*c) + D3T.Duuv*(3*c*c*d)       + D3T.Duvv*(3*c*d*d)       + D3T.Dvvv*(d*d*d);

  Standard_Real &A0u = a;
  Standard_Real &A1u = b;
  Standard_Real &A0v = c;
  Standard_Real &A1v = d;
  Suuu += D2T.Duu*(3*A0u*B0uu)          + D2T.Duv*(3*(A0u*B1uu+A1u*B0uu))                     + D2T.Dvv*(3*A1u*B1uu);
  Suuv += D2T.Duu*(2*A0u*B0uv+A0v*B0uu) + D2T.Duv*(2*(A0u*B1uv+A1u*B0uv)+A0v*B1uu+A1v*B0uu)   + D2T.Dvv*(2*A1u*B1uv+A1v*B1uu);
  Suvv += D2T.Duu*(A0u*B0vv+2*A0v*B0uv) + D2T.Duv*(2*(A0v*B1uv+A1v*B0uv)+A0u*B1vv+A1u*B0vv)   + D2T.Dvv*(2*A1v*B1uv+A1u*B1vv);
  Svvv += D2T.Duu*(3*A0v*B0vv)          + D2T.Duv*(3*(A0v*B1vv+A1v*B0vv))                     + D2T.Dvv*(3*A1v*B1vv);

  gp_XYZ duuu = normaleS * (normale*(Suuu-D3S.Duuu))*invcos;
  gp_XYZ duuv = normaleS * (normale*(Suuv-D3S.Duuv))*invcos;
  gp_XYZ duvv = normaleS * (normale*(Suvv-D3S.Duvv))*invcos;
  gp_XYZ dvvv = normaleS * (normale*(Svvv-D3S.Dvvv))*invcos;

  myPPC[5] = Plate_PinpointConstraint(pnt2d, duuu,3,0);
  myPPC[6] = Plate_PinpointConstraint(pnt2d, duuv,2,1);
  myPPC[7] = Plate_PinpointConstraint(pnt2d, duvv,1,2);
  myPPC[8] = Plate_PinpointConstraint(pnt2d, dvvv,0,3);
  nb_PPConstraints = 9;
}
