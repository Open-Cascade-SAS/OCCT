// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
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

// Modified 09/09/1996 PMN Adde Nb(Intervalls), IsRationnal
//                         Optimisation, use of GetCircle
// Modified 20/02/1998 PMN Singular surfaces management

#include <Adaptor3d_Curve.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc.hxx>
#include <BlendFunc_ConstRad.hxx>
#include <ElCLib.hxx>
#include <GeomFill.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_SVD.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_TypeDef.hxx>

#define Eps 1.e-15


//=======================================================================
//function : BlendFunc_ConstRad
//purpose  : 
//=======================================================================

BlendFunc_ConstRad::BlendFunc_ConstRad(const Handle(Adaptor3d_Surface)& S1,
                                       const Handle(Adaptor3d_Surface)& S2,
                                       const Handle(Adaptor3d_Curve)& C)
                                 :
                  surf1(S1),surf2(S2),
                  curv(C), tcurv(C),
                  istangent(Standard_True), param(0.0),
                  ray1(0.0), ray2(0.0),
                  choix(0), xval(1, 4),
                  E(1,4), DEDX(1,4,1,4),  DEDT(1,4), 
                  D2EDX2(4,4,4),
                  D2EDXDT(1,4,1,4), D2EDT2(1,4),
                  maxang(RealFirst()), minang(RealLast()),
                  distmin(RealLast()),
                  mySShape(BlendFunc_Rational)
{ 
// Initialisation of cash control variables.
  tval = -9.876e100;
  xval.Init(-9.876e100);
  myXOrder = -1;
  myTOrder = -1;
  myTConv = Convert_TgtThetaOver2;
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_ConstRad::NbEquations () const
{
  return 4;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Set(const Standard_Real Radius, const Standard_Integer Choix)
{
  choix = Choix;
  switch (choix) {
  case 1:
  case 2:
    {
      ray1 = -Radius;
      ray2 = -Radius;
    }
    break;
  case 3:
  case 4:
    {
      ray1 = Radius;
      ray2 = -Radius;
    }
    break;
  case 5:
  case 6:
    {
      ray1 = Radius;
      ray2 = Radius;
    }
    break;
  case 7:
  case 8:
    {
      ray1 = -Radius;
      ray2 = Radius;
    }
    break;
  default:
    ray1 = ray2 = -Radius;
  }
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Set(const BlendFunc_SectionShape TypeSection)
{
  mySShape = TypeSection;
}


//=======================================================================
//function : ComputeValues
//purpose  : OBLIGATORY passage for all calculations
//           This method manages positioning on Surfaces and Curves
//           Calculate the equations and their partial derivates
//           Stock certain intermediate results in fields to 
//           use in other methods.
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::ComputeValues(const math_Vector& X,
                                                   const Standard_Integer Order,
                                                   const Standard_Boolean byParam,
                                                   const Standard_Real Param)
{
 // static declaration to avoid systematic reallocation
 
 static gp_Vec d3u1,d3v1,d3uuv1,d3uvv1,d3u2,d3v2,d3uuv2,d3uvv2; 
 static gp_Vec d1gui, d2gui, d3gui;
 static gp_Pnt ptgui;
 static Standard_Real invnormtg, dinvnormtg;
 Standard_Real T =  Param, aux;

 // Case of implicite parameter
 if ( !byParam) { T = param;}

 // Is the work already done ?
 Standard_Boolean myX_OK = (Order<=myXOrder) ;
 for (Standard_Integer ii=1; ((ii<=X.Length()) && myX_OK); ii++) {
   myX_OK = ( X(ii) == xval(ii) );
 }

 Standard_Boolean t_OK =( (T == tval) 
                       && ((Order<=myTOrder)||(!byParam)) ); 

 if (myX_OK && (t_OK) ) {
   return Standard_True;
 }

 // Processing of t
 if (!t_OK) {
   tval = T;
   if (byParam) { myTOrder = Order;}
   else         { myTOrder = 0;}
   //----- Positioning on the curve ----------------
   switch (myTOrder) {
   case 0 :
     {
       tcurv->D1(T, ptgui, d1gui);
       nplan  = d1gui.Normalized();
     }
     break;

   case 1 :
     { 
       tcurv->D2(T,ptgui,d1gui,d2gui);
       nplan  = d1gui.Normalized();
       invnormtg = ((Standard_Real) 1 ) / d1gui.Magnitude();
       dnplan.SetLinearForm(invnormtg, d2gui,
			    -invnormtg*(nplan.Dot(d2gui)), nplan);
     break;
     }
   case 2 :
     { 
       tcurv->D3(T,ptgui,d1gui,d2gui,d3gui);
       nplan  = d1gui.Normalized();
       invnormtg = ((Standard_Real) 1 ) / d1gui.Magnitude();
       dnplan.SetLinearForm(invnormtg, d2gui,
			   -invnormtg*(nplan.Dot(d2gui)), nplan);
       dinvnormtg = - nplan.Dot(d2gui)*invnormtg*invnormtg;
       d2nplan.SetLinearForm(invnormtg,  d3gui,
			     dinvnormtg, d2gui);
       aux = dinvnormtg*(nplan.Dot(d2gui)) + invnormtg*( dnplan.Dot(d2gui) 
						       + nplan.Dot(d3gui) ); 
       d2nplan.SetLinearForm(-invnormtg*(nplan.Dot(d2gui)), dnplan,
			     -aux, nplan, d2nplan );
       break;
   }
   default:
     return Standard_False;
   }
 }

 // Processing of X
 if (!myX_OK) {
   xval = X;
   myXOrder = Order;
   //-------------- Positioning on surfaces -----------------
   switch (myXOrder) {
   case 0 :
     {
       surf1->D1(X(1),X(2),pts1,d1u1,d1v1);
       nsurf1 = d1u1.Crossed(d1v1);
       surf2->D1(X(3),X(4),pts2,d1u2,d1v2);
       nsurf2 = d1u2.Crossed(d1v2);
       break;
     }
   case 1 :
     {
       surf1->D2(X(1),X(2),pts1,d1u1,d1v1,d2u1,d2v1,d2uv1);
       nsurf1 = d1u1.Crossed(d1v1);
       dns1u1 = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
       dns1v1 = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));
       surf2->D2(X(3),X(4),pts2,d1u2,d1v2,d2u2,d2v2,d2uv2);
       nsurf2 = d1u2.Crossed(d1v2);
       dns1u2 = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
       dns1v2 = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
       break;
     }
   case 2 :
     {
       surf1->D3(X(1),X(2),pts1,d1u1,d1v1,d2u1,d2v1,d2uv1,d3u1,d3v1,d3uuv1,d3uvv1);
       nsurf1 = d1u1.Crossed(d1v1);
       dns1u1 = d2u1.Crossed(d1v1).Added(d1u1.Crossed(d2uv1));
       dns1v1 = d2uv1.Crossed(d1v1).Added(d1u1.Crossed(d2v1));

       surf2->D3(X(3),X(4),pts2,d1u2,d1v2,d2u2,d2v2,d2uv2,d3u2,d3v2,d3uuv2,d3uvv2);
       nsurf2 = d1u2.Crossed(d1v2);
       dns1u2 = d2u2.Crossed(d1v2).Added(d1u2.Crossed(d2uv2));
       dns1v2 = d2uv2.Crossed(d1v2).Added(d1u2.Crossed(d2v2));
       break;
     }
   default:
     return Standard_False;
   }
   // Case of degenerated surfaces
   if (nsurf1.Magnitude() < Eps ) {
     //gp_Vec normal;
     gp_Pnt2d P(X(1), X(2)); 
     if (Order == 0) BlendFunc::ComputeNormal(surf1, P, nsurf1);
     else  BlendFunc::ComputeDNormal(surf1, P, nsurf1, dns1u1, dns1v1);
   }
  if (nsurf2.Magnitude() < Eps) {
     //gp_Vec normal;
     gp_Pnt2d P(X(3), X(4));
     if (Order==0) BlendFunc::ComputeNormal(surf2, P, nsurf2);
     else BlendFunc::ComputeDNormal(surf2, P, nsurf2, dns1u2, dns1v2);
   }
 }

 // -------------------- Positioning of order 0 ---------------------
 Standard_Real invnorm1, invnorm2, ndotns1, ndotns2, theD;
 gp_Vec ncrossns1,ncrossns2,resul,temp;

 theD =  - (nplan.XYZ().Dot(ptgui.XYZ()));

 E(1) = (nplan.X() * (pts1.X() + pts2.X()) + 
	 nplan.Y() * (pts1.Y() + pts2.Y()) + 
	 nplan.Z() * (pts1.Z() + pts2.Z())) /2 + theD;

 ncrossns1 = nplan.Crossed(nsurf1);
 ncrossns2 = nplan.Crossed(nsurf2);

 invnorm1 = ncrossns1.Magnitude();
 invnorm2 = ncrossns2.Magnitude();

 if (invnorm1 > Eps) invnorm1 = ((Standard_Real) 1) /invnorm1;
 else {
   invnorm1 = 1; // Unsatisfactory, but it is not necessary to crash
#ifdef OCCT_DEBUG
   std::cout << " ConstRad : Surface singuliere " << std::endl;
#endif
 }
 if (invnorm2 > Eps) invnorm2 = ((Standard_Real) 1) /invnorm2;
 else {
   invnorm2 = 1; //  Unsatisfactory, but it is not necessary to crash
#ifdef OCCT_DEBUG
   std::cout << " ConstRad : Surface singuliere " << std::endl;
#endif
 }

 ndotns1 = nplan.Dot(nsurf1);
 ndotns2 = nplan.Dot(nsurf2);

 temp.SetLinearForm(ndotns1,nplan,-1.,nsurf1);
 temp.Multiply(invnorm1);

 resul.SetLinearForm(ray1,temp,gp_Vec(pts2,pts1));
 temp.SetLinearForm(ndotns2,nplan,-1.,nsurf2);
 temp.Multiply(invnorm2);
 resul.Subtract(ray2*temp);

 E(2) = resul.X();
 E(3) = resul.Y();
 E(4) = resul.Z();

 // -------------------- Positioning of order 1 ---------------------
 if (Order >= 1) {
   Standard_Real  grosterme, cube, carre;
  

   DEDX(1,1) = nplan.Dot(d1u1)/2;
   DEDX(1,2) = nplan.Dot(d1v1)/2;
   DEDX(1,3) = nplan.Dot(d1u2)/2;
   DEDX(1,4) = nplan.Dot(d1v2)/2;

   cube =invnorm1*invnorm1*invnorm1;
   // Derived in relation to u1
   grosterme = - ncrossns1.Dot(nplan.Crossed(dns1u1))*cube;
   dndu1.SetLinearForm( grosterme*ndotns1
		      + invnorm1*nplan.Dot(dns1u1), nplan,
                      - grosterme, nsurf1,
                      - invnorm1,  dns1u1 );

   resul.SetLinearForm(ray1, dndu1, d1u1);
   DEDX(2,1) = resul.X();
   DEDX(3,1) = resul.Y();
   DEDX(4,1) = resul.Z();

   // Derived in relation to v1

   grosterme = - ncrossns1.Dot(nplan.Crossed(dns1v1))*cube;
   dndv1.SetLinearForm( grosterme*ndotns1
		       +invnorm1*nplan.Dot(dns1v1), nplan,
		       -grosterme, nsurf1,
		       -invnorm1, dns1v1);
   
   resul.SetLinearForm(ray1, dndv1, d1v1);
   DEDX(2,2) = resul.X();
   DEDX(3,2) = resul.Y();
   DEDX(4,2) = resul.Z();

   cube = invnorm2*invnorm2*invnorm2;
   // Derived in relation to u2
   grosterme = - ncrossns2.Dot(nplan.Crossed(dns1u2))*cube;
   dndu2.SetLinearForm( grosterme*ndotns2
		       +invnorm2*nplan.Dot(dns1u2), nplan,
		       -grosterme, nsurf2,
		       -invnorm2, dns1u2);

   resul.SetLinearForm(-ray2, dndu2, -1, d1u2);
   DEDX(2,3) = resul.X();
   DEDX(3,3) = resul.Y();
   DEDX(4,3) = resul.Z();

   // Derived in relation to v2
   grosterme = -ncrossns2.Dot(nplan.Crossed(dns1v2))*cube;
   dndv2.SetLinearForm( grosterme*ndotns2
		       +invnorm2*nplan.Dot(dns1v2), nplan,
		       -grosterme, nsurf2,
		       -invnorm2 , dns1v2);

   resul.SetLinearForm(-ray2,dndv2, -1, d1v2);   
   DEDX(2,4) = resul.X();
   DEDX(3,4) = resul.Y();
   DEDX(4,4) = resul.Z();

   if (byParam) {
     temp.SetXYZ( (pts1.XYZ()+pts2.XYZ())/2 - ptgui.XYZ());
     // Derived from n1 in relation to w     
     grosterme = ncrossns1.Dot(dnplan.Crossed(nsurf1))*invnorm1*invnorm1;
     dn1w.SetLinearForm((dnplan.Dot(nsurf1)-grosterme*ndotns1)*invnorm1, nplan,
			 ndotns1*invnorm1,dnplan,
			 grosterme*invnorm1,nsurf1);
  
     // Derivee from n2 in relation to w
     grosterme = ncrossns2.Dot(dnplan.Crossed(nsurf2))*invnorm2*invnorm2;
     dn2w.SetLinearForm((dnplan.Dot(nsurf2)-grosterme*ndotns2)*invnorm2,nplan,
			ndotns2*invnorm2,dnplan,
			grosterme*invnorm2,nsurf2);


     DEDT(1) =  dnplan.Dot(temp) - 1./invnormtg ;
     DEDT(2) =  ray1*dn1w.X() - ray2*dn2w.X();
     DEDT(3) =  ray1*dn1w.Y() - ray2*dn2w.Y();
     DEDT(4) =  ray1*dn1w.Z() - ray2*dn2w.Z();
   }
   // ------   Positioning of order 2  -----------------------------
   if (Order == 2) {
//     gp_Vec d2ndu1,  d2ndu2, d2ndv1, d2ndv2, d2nduv1, d2nduv2;
     gp_Vec d2ns1u1,  d2ns1u2, d2ns1v1, d2ns1v2, d2ns1uv1, d2ns1uv2;
     Standard_Real uterm, vterm, smallterm, p1, p2, p12;
     Standard_Real DPrim, DSecn;
     D2EDX2.Init(0);

     D2EDX2(1, 1, 1) = nplan.Dot(d2u1)/2;
     D2EDX2(1, 2, 1) = D2EDX2(1, 1, 2) = nplan.Dot(d2uv1)/2;
     D2EDX2(1, 2, 2) = nplan.Dot(d2v1)/2;

     D2EDX2(1, 3, 3) = nplan.Dot(d2u2)/2;
     D2EDX2(1, 4, 3) = D2EDX2(1, 3, 4) = nplan.Dot(d2uv2)/2;
     D2EDX2(1, 4, 4) = nplan.Dot(d2v2)/2;
                               // ================
                               // ==  Surface 1 ==
                               // ================
     carre = invnorm1*invnorm1;
     cube  = carre*invnorm1;
     // Derived double compared to u1       
       // Derived from the norm
     d2ns1u1.SetLinearForm(1, d3u1.Crossed(d1v1),
			   2, d2u1.Crossed(d2uv1),
			   1, d1u1.Crossed(d3uuv1));
     DPrim = ncrossns1.Dot(nplan.Crossed(dns1u1));
     smallterm =  - 2*DPrim*cube;
     DSecn = ncrossns1.Dot(nplan.Crossed(d2ns1u1))
          + (nplan.Crossed(dns1u1)).SquareMagnitude();
     grosterme  = (3*DPrim*DPrim*carre - DSecn) * cube;     

     temp.SetLinearForm( grosterme*ndotns1, nplan,
			-grosterme        , nsurf1);     
     p1 =  nplan.Dot(dns1u1);
     p2 =  nplan.Dot(d2ns1u1);    
     d2ndu1.SetLinearForm( invnorm1*p2 
			  +smallterm*p1, nplan,
			  -smallterm,    dns1u1,
			  -invnorm1,     d2ns1u1);
     d2ndu1 += temp;
     resul.SetLinearForm(ray1, d2ndu1, d2u1);
     D2EDX2(2,1,1) = resul.X();
     D2EDX2(3,1,1) = resul.Y();
     D2EDX2(4,1,1) = resul.Z();

     // Derived double compared to u1, v1       
       // Derived from the norm
     d2ns1uv1 =  (d3uuv1.Crossed(d1v1))
              +  (d2u1  .Crossed(d2v1))
	      +  (d1u1  .Crossed(d3uvv1));
     uterm =  ncrossns1.Dot(nplan.Crossed(dns1u1));
     vterm =  ncrossns1.Dot(nplan.Crossed(dns1v1));
     DSecn = (nplan.Crossed(dns1v1)).Dot(nplan.Crossed(dns1u1))
           +  ncrossns1.Dot(nplan.Crossed(d2ns1uv1));
     grosterme  = (3*uterm*vterm*carre-DSecn)*cube;
     uterm *= -cube; //and only now
     vterm *= -cube;
       
     p1 = nplan.Dot(dns1u1);
     p2 = nplan.Dot(dns1v1);
     temp.SetLinearForm(  grosterme*ndotns1, nplan,
			- grosterme, nsurf1,
                        - invnorm1, d2ns1uv1);
     d2nduv1.SetLinearForm( invnorm1*nplan.Dot(d2ns1uv1)
			  + uterm*p2 
			  + vterm*p1, nplan,
                          - uterm,    dns1v1,
			  - vterm,    dns1u1);
			
     d2nduv1 += temp;
     resul.SetLinearForm(ray1, d2nduv1, d2uv1);

     D2EDX2(2,2,1) = D2EDX2(2,1,2) = resul.X();
     D2EDX2(3,2,1) = D2EDX2(3,1,2) = resul.Y();
     D2EDX2(4,2,1) = D2EDX2(4,1,2) = resul.Z();    

     // Derived double compared to v1       
       // Derived from the norm
     d2ns1v1.SetLinearForm(1, d1u1.Crossed(d3v1),
			   2, d2uv1.Crossed(d2v1),
			   1, d3uvv1.Crossed(d1v1));
     DPrim = ncrossns1.Dot(nplan.Crossed(dns1v1));
     smallterm =  - 2*DPrim*cube;
     DSecn = ncrossns1.Dot(nplan.Crossed(d2ns1v1))
          + (nplan.Crossed(dns1v1)).SquareMagnitude();
     grosterme  = (3*DPrim*DPrim*carre - DSecn) * cube;
       
     p1 =  nplan.Dot(dns1v1);
     p2 =  nplan.Dot(d2ns1v1);
     temp.SetLinearForm( grosterme*ndotns1, nplan,
			-grosterme        , nsurf1);
     d2ndv1.SetLinearForm( invnorm1*p2 
			  +smallterm*p1, nplan,
			  -smallterm,    dns1v1,
			  -invnorm1,     d2ns1v1);
     d2ndv1 += temp;
     resul.SetLinearForm(ray1, d2ndv1, d2v1);

     D2EDX2(2,2,2) = resul.X();
     D2EDX2(3,2,2) = resul.Y();
     D2EDX2(4,2,2) = resul.Z();
                               // ================
                               // ==  Surface 2 ==
                               // ================
     carre = invnorm2*invnorm2;
     cube  = carre*invnorm2;
     // Derived double compared to u2       
       // Derived from the norm
     d2ns1u2.SetLinearForm(1, d3u2.Crossed(d1v2),
			   2, d2u2.Crossed(d2uv2),
			   1, d1u2.Crossed(d3uuv2));
     DPrim = ncrossns2.Dot(nplan.Crossed(dns1u2));
     smallterm =  - 2*DPrim*cube;
     DSecn = ncrossns2.Dot(nplan.Crossed(d2ns1u2))
          + (nplan.Crossed(dns1u2)).SquareMagnitude();
     grosterme  = (3*DPrim*DPrim*carre - DSecn) * cube;     

     temp.SetLinearForm( grosterme*ndotns2, nplan,
			-grosterme        , nsurf2);     
     p1 =  nplan.Dot(dns1u2);
     p2 =  nplan.Dot(d2ns1u2);    
     d2ndu2.SetLinearForm( invnorm2*p2 
			  +smallterm*p1, nplan,
			  -smallterm,    dns1u2,
			  -invnorm2,     d2ns1u2);
     d2ndu2 += temp;
     resul.SetLinearForm(-ray2, d2ndu2, -1, d2u2);
     D2EDX2(2,3,3) = resul.X();
     D2EDX2(3,3,3) = resul.Y();
     D2EDX2(4,3,3) = resul.Z();

     // Derived double compared to u2, v2       
       // Derived from the norm
     d2ns1uv2 =  (d3uuv2.Crossed(d1v2))
              +  (d2u2  .Crossed(d2v2))
	      +  (d1u2  .Crossed(d3uvv2));
     uterm =  ncrossns2.Dot(nplan.Crossed(dns1u2));
     vterm =  ncrossns2.Dot(nplan.Crossed(dns1v2));
     DSecn = (nplan.Crossed(dns1v2)).Dot(nplan.Crossed(dns1u2))
           +  ncrossns2.Dot(nplan.Crossed(d2ns1uv2));
     grosterme  = (3*uterm*vterm*carre-DSecn)*cube;
     uterm *= -cube; //and only now
     vterm *= -cube;
       
     p1 = nplan.Dot(dns1u2);
     p2 = nplan.Dot(dns1v2);
     temp.SetLinearForm(  grosterme*ndotns2, nplan,
			- grosterme, nsurf2,
                        - invnorm2, d2ns1uv2);
     d2nduv2.SetLinearForm( invnorm2*nplan.Dot(d2ns1uv2)
			  + uterm*p2 
			  + vterm*p1, nplan,
                          - uterm,    dns1v2,
			  - vterm,    dns1u2);
			
     d2nduv2 += temp;
     resul.SetLinearForm(-ray2, d2nduv2, -1, d2uv2);

     D2EDX2(2,4,3) = D2EDX2(2,3,4) = resul.X();
     D2EDX2(3,4,3) = D2EDX2(3,3,4) = resul.Y();
     D2EDX2(4,4,3) = D2EDX2(4,3,4) = resul.Z();    

     // Derived double compared to v2       
       // Derived from the norm
     d2ns1v2.SetLinearForm(1, d1u2.Crossed(d3v2),
			   2, d2uv2.Crossed(d2v2),
			   1, d3uvv2.Crossed(d1v2));
     DPrim = ncrossns2.Dot(nplan.Crossed(dns1v2));
     smallterm =  - 2*DPrim*cube;
     DSecn = ncrossns2.Dot(nplan.Crossed(d2ns1v2))
          + (nplan.Crossed(dns1v2)).SquareMagnitude();
     grosterme  = (3*DPrim*DPrim*carre - DSecn) * cube;
       
     p1 =  nplan.Dot(dns1v2);
     p2 =  nplan.Dot(d2ns1v2);
     temp.SetLinearForm( grosterme*ndotns2, nplan,
			-grosterme        , nsurf2);
     d2ndv2.SetLinearForm( invnorm2*p2 
			  +smallterm*p1, nplan,
			  -smallterm,    dns1v2,
			  -invnorm2,     d2ns1v2);
     d2ndv2 += temp;
     resul.SetLinearForm(-ray2, d2ndv2, -1, d2v2);

     D2EDX2(2,4,4) = resul.X();
     D2EDX2(3,4,4) = resul.Y();
     D2EDX2(4,4,4) = resul.Z();

     if (byParam) {
       Standard_Real tterm;
        //  ---------- Derivation double in t, X --------------------------
       D2EDXDT(1,1) = dnplan.Dot(d1u1)/2;
       D2EDXDT(1,2) = dnplan.Dot(d1v1)/2;
       D2EDXDT(1,3) = dnplan.Dot(d1u2)/2;
       D2EDXDT(1,4) = dnplan.Dot(d1v2)/2;

       carre = invnorm1*invnorm1;
       cube = carre*invnorm1;
       //--> Derived compared to u1 and t
       tterm =  ncrossns1.Dot(dnplan.Crossed(nsurf1));
       smallterm  = - tterm*cube;
       // Derived from the norm
       uterm =  ncrossns1.Dot(nplan. Crossed(dns1u1));
       DSecn = (nplan.Crossed(dns1u1)).Dot(dnplan.Crossed(nsurf1))
	      + ncrossns1.Dot(dnplan.Crossed(dns1u1));
       grosterme = (3*uterm*tterm*carre - DSecn) * cube;
       uterm *= -cube;

       p1 = dnplan.Dot(nsurf1);
       p2 = nplan. Dot(dns1u1);
       p12 = dnplan.Dot(dns1u1);  
     
       d2ndtu1.SetLinearForm( invnorm1*p12 
			    + smallterm*p2
			    + uterm*p1
                            + grosterme*ndotns1, nplan,
			      invnorm1*p2 
			    + uterm*ndotns1,     dnplan,
                            - smallterm,         dns1u1);
       d2ndtu1 -= grosterme*nsurf1;

       resul = ray1*d2ndtu1;
       D2EDXDT(2,1) = resul.X();
       D2EDXDT(3,1) = resul.Y();
       D2EDXDT(4,1) = resul.Z();

       //--> Derived compared to v1 and t
       // Derived from the norm
       uterm =  ncrossns1.Dot(nplan. Crossed(dns1v1));
       DSecn = (nplan. Crossed(dns1v1)).Dot(dnplan.Crossed(nsurf1))
	      + ncrossns1.Dot(dnplan.Crossed(dns1v1));
       grosterme = (3*uterm*tterm*carre - DSecn) * cube;
       uterm *= -cube;

       p1 =  dnplan.Dot(nsurf1);
       p2 =  nplan. Dot(dns1v1);
       p12 = dnplan.Dot(dns1v1);       
       d2ndtv1.SetLinearForm( invnorm1*p12 
			   + uterm*p1 
			   + smallterm*p2
                           + grosterme*ndotns1, nplan,
			     invnorm1*p2 
			   + uterm*ndotns1,    dnplan,
                           - smallterm     ,   dns1v1);
       d2ndtv1 -= grosterme*nsurf1;

       resul = ray1* d2ndtv1;
       D2EDXDT(2,2) = resul.X();
       D2EDXDT(3,2) = resul.Y();
       D2EDXDT(4,2) = resul.Z();     

       carre = invnorm2*invnorm2;
       cube = carre*invnorm2;
       //--> Derived compared to u2 and t
       tterm =  ncrossns2.Dot(dnplan.Crossed(nsurf2));
       smallterm = -tterm*cube;
       // Derived from the norm
       uterm =  ncrossns2.Dot(nplan. Crossed(dns1u2));
       DSecn = (nplan. Crossed(dns1u2)).Dot(dnplan.Crossed(nsurf2))
	     + ncrossns2.Dot(dnplan.Crossed(dns1u2));
       grosterme = (3*uterm*tterm*carre - DSecn) * cube;
       uterm *= -cube;

       p1 =  dnplan.Dot(nsurf2);
       p2 =  nplan. Dot(dns1u2);
       p12 = dnplan.Dot(dns1u2);
       
       d2ndtu2.SetLinearForm( invnorm2*p12
			   + smallterm*p2
			   + uterm*p1
                           + grosterme*ndotns2, nplan,
			     invnorm2*p2
			   + uterm*ndotns2,    dnplan,
                            -smallterm     ,   dns1u2);
       d2ndtu2 -= grosterme*nsurf2;

       resul = - ray2*d2ndtu2;
       D2EDXDT(2,3) = resul.X();
       D2EDXDT(3,3) = resul.Y();
       D2EDXDT(4,3) = resul.Z();

       //--> Derived compared to v2 and t
       // Derived from the norm
       uterm =  ncrossns2.Dot(nplan. Crossed(dns1v2));
       DSecn = (nplan.Crossed(dns1v2)).Dot(dnplan.Crossed(nsurf2))
	     +  ncrossns2.Dot(dnplan.Crossed(dns1v2)); 
       grosterme = (3*uterm*tterm*carre - DSecn) * cube;
       uterm *= - cube;

       p1 =  dnplan.Dot(nsurf2);
       p2 =  nplan. Dot(dns1v2);
       p12 = dnplan.Dot(dns1v2); 
      
       d2ndtv2.SetLinearForm( invnorm2*p12 
			    + smallterm*p2
			    + uterm*p1
                            + grosterme*ndotns2, nplan,
			      invnorm2*p2
			    + uterm*ndotns2,     dnplan,
                             -smallterm        , dns1v2);
       d2ndtv2 -= grosterme*nsurf2;

       resul = - ray2 * d2ndtv2;
       D2EDXDT(2,4) = resul.X();
       D2EDXDT(3,4) = resul.Y();
       D2EDXDT(4,4) = resul.Z();     


        //  ---------- Derivation double in t -----------------------------
    // Derived from n1 compared to w
       carre = invnorm1*invnorm1;
       cube = carre*invnorm1;
       // Derived from the norm
       DPrim =  ncrossns1.Dot(dnplan.Crossed(nsurf1));
       smallterm = - 2*DPrim*cube;
       DSecn     = (dnplan.Crossed(nsurf1)).SquareMagnitude()
		  + ncrossns1.Dot(d2nplan.Crossed(nsurf1));
       grosterme = (3*DPrim*DPrim*carre - DSecn) * cube; 
      
       p1 =  dnplan. Dot(nsurf1);
       p2 =  d2nplan.Dot(nsurf1);

       temp.SetLinearForm( grosterme*ndotns1, nplan,
			  -grosterme , nsurf1);
       d2n1w.SetLinearForm(  smallterm*p1
			   + invnorm1*p2,      nplan,
			     smallterm*ndotns1
			   + 2*invnorm1*p1,    dnplan,
			     ndotns1*invnorm1,   d2nplan);
       d2n1w += temp;
  
     // Derived from n2 compared to w
       carre = invnorm2*invnorm2;
       cube = carre*invnorm2;
       // Derived from the norm
       DPrim =  ncrossns2.Dot(dnplan.Crossed(nsurf2));
       smallterm = - 2*DPrim*cube;
       DSecn     = (dnplan.Crossed(nsurf2)).SquareMagnitude()
		  + ncrossns2.Dot(d2nplan.Crossed(nsurf2));
       grosterme = (3*DPrim*DPrim*carre - DSecn) * cube;
   
       p1 =  dnplan. Dot(nsurf2);
       p2 =  d2nplan.Dot(nsurf2);

       temp.SetLinearForm( grosterme*ndotns2, nplan,
			  -grosterme ,        nsurf2);
       d2n2w.SetLinearForm(  smallterm*p1
			   + invnorm2*p2,        nplan,
			     smallterm*ndotns2
			   + 2*invnorm2*p1,     dnplan,
			     ndotns2*invnorm2,  d2nplan);
       d2n2w += temp;

       temp.SetXYZ( (pts1.XYZ()+pts2.XYZ())/2 - ptgui.XYZ());
       D2EDT2(1) = d2nplan.Dot(temp) - 2*dnplan.Dot(d1gui) - nplan.Dot(d2gui);
       D2EDT2(2) = ray1*d2n1w.X() - ray2*d2n2w.X();
       D2EDT2(3) = ray1*d2n1w.Y() - ray2*d2n2w.Y();
       D2EDT2(4) = ray1*d2n1w.Z() - ray2*d2n2w.Z();
     }
   }
 }
 return Standard_True; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Set(const Standard_Real Param)
{
  param = Param;
}

//=======================================================================
//function : Set
//purpose  : Segmentation of the useful part of the curve
//           Precision is taken at random and small !?
//=======================================================================

void BlendFunc_ConstRad::Set(const Standard_Real First, const Standard_Real Last)
{
  tcurv = curv->Trim(First, Last, 1.e-12);
}

//=======================================================================
//function : GetTolerance
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::GetTolerance(math_Vector& Tolerance, const Standard_Real Tol) const
{
  Tolerance(1) = surf1->UResolution(Tol);
  Tolerance(2) = surf1->VResolution(Tol);
  Tolerance(3) = surf2->UResolution(Tol);
  Tolerance(4) = surf2->VResolution(Tol);
}


//=======================================================================
//function : GetBounds
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::GetBounds(math_Vector& InfBound, math_Vector& SupBound) const
{
  InfBound(1) = surf1->FirstUParameter();
  InfBound(2) = surf1->FirstVParameter();
  InfBound(3) = surf2->FirstUParameter();
  InfBound(4) = surf2->FirstVParameter();
  SupBound(1) = surf1->LastUParameter();
  SupBound(2) = surf1->LastVParameter();
  SupBound(3) = surf2->LastUParameter();
  SupBound(4) = surf2->LastVParameter();

  for(Standard_Integer i = 1; i <= 4; i++){
    if(!Precision::IsInfinite(InfBound(i)) &&
       !Precision::IsInfinite(SupBound(i))) {
      Standard_Real range = (SupBound(i) - InfBound(i));
      InfBound(i) -= range;
      SupBound(i) += range;
    }
  }
}


//=======================================================================
//function : IsSolution
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::IsSolution(const math_Vector& Sol, const Standard_Real Tol)
{
  Standard_Real norm, Cosa, Sina, Angle;
  Standard_Boolean Ok=Standard_True;

  Ok = ComputeValues(Sol, 1, Standard_True, param);

  if (Abs(E(1)) <= Tol &&
      E(2)*E(2) + E(3)*E(3) +  E(4)*E(4) <= Tol*Tol) { 

    // ns1, ns2 and  np are copied locally to avoid crushing the fields !
    gp_Vec ns1,ns2,np;
    ns1 = nsurf1;
    ns2 = nsurf2;
    np = nplan;
  
    norm = nplan.Crossed(ns1).Magnitude();
    if (norm < Eps)  {
      norm = 1; // Unsatisfactory, but it is not necessary to stop
    }
    ns1.SetLinearForm(nplan.Dot(ns1)/norm,nplan, -1./norm, ns1);

    norm = nplan.Crossed(ns2).Magnitude();
    if (norm < Eps)  {
      norm = 1; // Unsatisfactory, but it is not necessary to stop
    }
    ns2.SetLinearForm(nplan.Dot(ns2)/norm,nplan, -1./norm, ns2);   

    Standard_Real maxpiv = 1.e-9;
    math_Vector controle(1,4),solution(1,4), tolerances(1,4);
    GetTolerance(tolerances,Tol);

    istangent = Standard_True;
    math_Gauss Resol(DEDX,maxpiv);
    if (Resol.IsDone()) {
      Resol.Solve(-DEDT,solution);
      istangent = Standard_False;
      controle = DEDT.Added(DEDX.Multiplied(solution));
      if(Abs(controle(1)) > tolerances(1) ||
	 Abs(controle(2)) > tolerances(2) ||
	 Abs(controle(3)) > tolerances(3) ||
	 Abs(controle(4)) > tolerances(4)){
	istangent = Standard_True;
      } 
    }

    if (istangent) {
      math_SVD SingRS (DEDX);
      if (SingRS.IsDone()) {
	SingRS.Solve(-DEDT, solution, 1.e-6);
	istangent = Standard_False;
	controle = DEDT.Added(DEDX.Multiplied(solution));
	if(Abs(controle(1)) > tolerances(1) ||
	   Abs(controle(2)) > tolerances(2) ||
	   Abs(controle(3)) > tolerances(3) ||
	   Abs(controle(4)) > tolerances(4)){
#ifdef OCCT_DEBUG
	std::cout<<"Cheminement : echec calcul des derivees"<<std::endl;
#endif
	  istangent = Standard_True;
	} 
      }
    }

    if(!istangent){
      tg1.SetLinearForm(solution(1),d1u1,solution(2),d1v1);
      tg2.SetLinearForm(solution(3),d1u2,solution(4),d1v2);
      tg12d.SetCoord(solution(1),solution(2));
      tg22d.SetCoord(solution(3),solution(4));
    }
 
  // update of maxang

    if (ray1 > 0.) {
      ns1.Reverse();
    }
    if (ray2 >0.) {
      ns2.Reverse();
    }
    Cosa = ns1.Dot(ns2);
    Sina = np.Dot(ns1.Crossed(ns2));
    if (choix%2 != 0) {
      Sina = -Sina;  //nplan is changed in -nplan
    }

    if(Cosa > 1.) {Cosa = 1.; Sina = 0.;}
    Angle = ACos(Cosa);

 // Reframing on  ]-pi/2, 3pi/2]
    if (Sina <0.) {
      if (Cosa > 0.) Angle = -Angle;
      else           Angle =  2.*M_PI - Angle;
    }

//    std::cout << "Angle : " <<Angle << std::endl;
//    if ((Angle < 0) || (Angle > M_PI)) {
//      std::cout << "t = " << param << std::endl;
//    }

    if (Abs(Angle)>maxang) {maxang = Abs(Angle);}
    if (Abs(Angle)<minang) {minang = Abs(Angle);}
    distmin = Min( distmin, pts1.Distance(pts2));

    return Ok;
  }
  istangent = Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetMinimalDistance
//purpose  : 
//=======================================================================

Standard_Real BlendFunc_ConstRad::GetMinimalDistance() const
{
  return distmin;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::Value(const math_Vector& X, math_Vector& F)
{
  const Standard_Boolean Ok = ComputeValues(X, 0);
  F = E;
  return Ok;
}



//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::Derivatives(const math_Vector& X, math_Matrix& D)
{
  const Standard_Boolean Ok = ComputeValues(X, 1);
  D = DEDX;
  return Ok;
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::Values(const math_Vector& X, math_Vector& F, math_Matrix& D)
{
  const Standard_Boolean Ok = ComputeValues(X, 1);
  F = E;
  D = DEDX;
  return Ok;
}  

//=======================================================================
//function : PointOnS1
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_ConstRad::PointOnS1 () const
{
  return pts1;
}


//=======================================================================
//function : PointOnS2
//purpose  : 
//=======================================================================

const gp_Pnt& BlendFunc_ConstRad::PointOnS2 () const
{
  return pts2;
}


//=======================================================================
//function : IsTangencyPoint
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::IsTangencyPoint () const
{
  return istangent;
}


//=======================================================================
//function : TangentOnS1
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ConstRad::TangentOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstRad::TangentOnS1");
  return tg1;
}


//=======================================================================
//function : TangentOnS2
//purpose  : 
//=======================================================================

const gp_Vec& BlendFunc_ConstRad::TangentOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstRad::TangentOnS2");
  return tg2;
}


//=======================================================================
//function : Tangent2dOnS1
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ConstRad::Tangent2dOnS1 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstRad::Tangent2dOnS1");
  return tg12d;
}


//=======================================================================
//function : Tangent2dOnS2
//purpose  : 
//=======================================================================

const gp_Vec2d& BlendFunc_ConstRad::Tangent2dOnS2 () const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstRad::Tangent2dOnS2");
  return tg22d;
}


//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Tangent(const Standard_Real U1,
				 const Standard_Real V1,
				 const Standard_Real U2,
				 const Standard_Real V2,
				 gp_Vec& TgF,
				 gp_Vec& TgL,
				 gp_Vec& NmF,
				 gp_Vec& NmL) const
{
  gp_Pnt Center;
  gp_Vec ns1;
  Standard_Real  invnorm1;

  if ((U1!=xval(1)) || (V1!=xval(2)) || 
      (U2!=xval(3)) || (V2!=xval(4))) {
    gp_Vec d1u,d1v;
    gp_Pnt bid;
    surf1->D1(U1,V1,bid,d1u,d1v);  
    NmF = ns1 = d1u.Crossed(d1v);
    surf2->D1(U2,V2,bid,d1u,d1v);
    NmL = d1u.Crossed(d1v);
  }
  else {
    NmF = ns1 = nsurf1;
    NmL = nsurf2;
  }

  invnorm1 = nplan.Crossed(ns1).Magnitude();
  if (invnorm1 < Eps) invnorm1 = 1;
  else invnorm1 = 1. / invnorm1;

  ns1.SetLinearForm(nplan.Dot(ns1)*invnorm1,nplan, -invnorm1,ns1);
  Center.SetXYZ(pts1.XYZ()+ray1*ns1.XYZ());

  TgF = nplan.Crossed(gp_Vec(Center,pts1));
  TgL = nplan.Crossed(gp_Vec(Center,pts2));
  if (choix%2 == 1) {
    TgF.Reverse();
    TgL.Reverse();
  }
}

//=======================================================================
//function : TwistOnS1
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::TwistOnS1() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstRad::TwistOnS1");
  return tg1.Dot(nplan) < 0.;
}

//=======================================================================
//function : TwistOnS2
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::TwistOnS2() const
{
  if (istangent)
    throw Standard_DomainError("BlendFunc_ConstRad::TwistOnS2");
  return tg2.Dot(nplan) < 0.;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Section(const Standard_Real Param,
				 const Standard_Real U1,
				 const Standard_Real V1,
				 const Standard_Real U2,
				 const Standard_Real V2,
				 Standard_Real& Pdeb,
				 Standard_Real& Pfin,
				 gp_Circ& C)
{
  gp_Pnt Center;
  gp_Vec ns1,np;

  math_Vector X(1,4);
  X(1) = U1; X(2) = V1; X(3) = U2; X(4) = V2; 
  Standard_Real prm = Param;

  ComputeValues(X, 0, Standard_True, prm);

  ns1 = nsurf1;
  np = nplan;

  Standard_Real  norm1;
  norm1 = nplan.Crossed(ns1).Magnitude();
  if (norm1 < Eps)  {
    norm1 = 1; // Unsatisfactory, but it is not necessary to stop
  }   
  ns1.SetLinearForm(nplan.Dot(ns1)/norm1,nplan, -1./norm1,ns1);
  Center.SetXYZ(pts1.XYZ()+ray1*ns1.XYZ());

// ns1 is oriented from the center to pts1,

  if (ray1 > 0.) {
    ns1.Reverse();
  }
  if (choix%2 != 0) {
    np.Reverse();
  }
  C.SetRadius(Abs(ray1));
  C.SetPosition(gp_Ax2(Center,np,ns1));
  Pdeb = 0.;
  Pfin = ElCLib::Parameter(C,pts2);
  // Test negative and almost null angles : Singular Case
  if (Pfin>1.5*M_PI) {
    np.Reverse();
    C.SetPosition(gp_Ax2(Center,np,ns1));
    Pfin = ElCLib::Parameter(C,pts2);
  }
  if (Pfin < Precision::PConfusion()) Pfin += Precision::PConfusion();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::IsRational () const
{
 return  (mySShape==BlendFunc_Rational || mySShape==BlendFunc_QuasiAngular);
}

//=======================================================================
//function : GetSectionSize
//purpose  : 
//=======================================================================
Standard_Real BlendFunc_ConstRad::GetSectionSize() const 
{
  return maxang*Abs(ray1);
}

//=======================================================================
//function : GetMinimalWeight
//purpose  : 
//=======================================================================
void BlendFunc_ConstRad::GetMinimalWeight(TColStd_Array1OfReal& Weigths) const 
{
  BlendFunc::GetMinimalWeights(mySShape, myTConv, minang, maxang, Weigths );
  // It is supposed that it does not depend on the Radius!
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer BlendFunc_ConstRad::NbIntervals (const GeomAbs_Shape S) const
{
  return curv->NbIntervals(BlendFunc::NextShape(S));
}


//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Intervals (TColStd_Array1OfReal& T,
				    const GeomAbs_Shape S) const
{
  curv->Intervals(T, BlendFunc::NextShape(S));
}

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::GetShape (Standard_Integer& NbPoles,
				   Standard_Integer& NbKnots,
				   Standard_Integer& Degree,
				   Standard_Integer& NbPoles2d)
{
  NbPoles2d = 2;
  BlendFunc::GetShape(mySShape,maxang,NbPoles,NbKnots,Degree,myTConv);
}

//=======================================================================
//function : GetTolerance
//purpose  : Determine Tolerances used for approximations.
//=======================================================================
void BlendFunc_ConstRad::GetTolerance(const Standard_Real BoundTol, 
				      const Standard_Real SurfTol, 
				      const Standard_Real AngleTol, 
				      math_Vector& Tol3d, 
				      math_Vector& Tol1d) const
{
 Standard_Integer low = Tol3d.Lower() , up=Tol3d.Upper();
 Standard_Real Tol;
 Tol= GeomFill::GetTolerance(myTConv, minang, Abs(ray1),
			     AngleTol, SurfTol);
 Tol1d.Init(SurfTol);
 Tol3d.Init(SurfTol);
 Tol3d(low+1) = Tol3d(up-1) = Min(Tol, SurfTol);
 Tol3d(low) = Tol3d(up) = Min(Tol, BoundTol);
    
}

//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Knots(TColStd_Array1OfReal& TKnots)
{
  GeomFill::Knots(myTConv,TKnots);
}


//=======================================================================
//function : Mults
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Mults(TColStd_Array1OfInteger& TMults)
{
  GeomFill::Mults(myTConv,TMults);
}


//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

void BlendFunc_ConstRad::Section(const Blend_Point& P,
				 TColgp_Array1OfPnt& Poles,
				 TColgp_Array1OfPnt2d& Poles2d,
				 TColStd_Array1OfReal& Weights)
{
  gp_Pnt Center;
  gp_Vec ns1,ns2,np;

  math_Vector X(1,4);
  Standard_Real prm = P.Parameter();

  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();

  P.ParametersOnS1(X(1), X(2));
  P.ParametersOnS2(X(3), X(4));

  ComputeValues(X, 0, Standard_True, prm);
  distmin = Min (distmin, pts1.Distance(pts2));

  // ns1, ns2, np are copied locally to avoid crushing the fields !
  ns1 = nsurf1;
  ns2 = nsurf2;
  np = nplan;

  
  Poles2d(Poles2d.Lower()).SetCoord(X(1), X(2));
  Poles2d(Poles2d.Upper()).SetCoord(X(3), X(4));

  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts1;
    Poles(upp) = pts2;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    return;
  }

  Standard_Real  norm1, norm2;
  norm1 = nplan.Crossed(ns1).Magnitude();
  norm2 = nplan.Crossed(ns2).Magnitude();
  if (norm1 < Eps)  {
    norm1 = 1; // Unsatisfactory, but it is not necessary to stop
  }
  if (norm2 < Eps) {
    norm2 = 1; // Unsatisfactory, but it is not necessary to stop
  }

  ns1.SetLinearForm(nplan.Dot(ns1)/norm1,nplan, -1./norm1,ns1);
  ns2.SetLinearForm(nplan.Dot(ns2)/norm2,nplan, -1./norm2,ns2);

  Center.SetXYZ(pts1.XYZ()+ray1*ns1.XYZ());

// ns1 (resp. ns2) is oriented from center to pts1 (resp. pts2),
// and the triedron ns1,ns2,nplan is made direct.

  if (ray1 > 0.) {
    ns1.Reverse();
  }
  if (ray2 >0.) {
    ns2.Reverse();
  }
  if (choix%2 != 0) {
    np.Reverse();
  }

  GeomFill::GetCircle(myTConv,
		      ns1, ns2, 
		      np, pts1, pts2,
		      Abs(ray1), Center, 
		      Poles, Weights);
}
 

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::Section
  (const Blend_Point& P,
   TColgp_Array1OfPnt& Poles,
   TColgp_Array1OfVec& DPoles,
   TColgp_Array1OfPnt2d& Poles2d,
   TColgp_Array1OfVec2d& DPoles2d,
   TColStd_Array1OfReal& Weights,
   TColStd_Array1OfReal& DWeights)
{
  gp_Vec ns1, ns2, np, dnp, dnorm1w, dnorm2w, tgc;
  Standard_Real norm1, norm2;

  gp_Pnt Center;
  math_Vector sol(1,4), secmember(1,4);

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  Standard_Boolean istgt = Standard_True;

  P.ParametersOnS1(sol(1),sol(2));
  P.ParametersOnS2(sol(3),sol(4));

  // Calculation of equations
  ComputeValues(sol, 1, Standard_True, prm);
  distmin = Min (distmin, pts1.Distance(pts2));

  // ns1, ns2, np are copied locally to avoid crushing the fields !
  ns1 = nsurf1;
  ns2 = nsurf2;
  np = nplan;
  dnp = dnplan;

  if ( ! pts1.IsEqual(pts2, 1.e-4)) {

    // Calculation of derivates Processing Normal
    math_Gauss Resol(DEDX, 1.e-9);

    if (Resol.IsDone()) {  
      Resol.Solve(-DEDT, secmember);
      istgt = Standard_False;
    }
  }

 if (istgt) {
    math_SVD SingRS (DEDX);
    if (SingRS.IsDone()) {
      SingRS.Solve(-DEDT, secmember, 1.e-6);
      istgt = Standard_False;
      }
  }

 if (!istgt) {
    tg1.SetLinearForm(secmember(1),d1u1, secmember(2),d1v1);
    tg2.SetLinearForm(secmember(3),d1u2, secmember(4),d1v2);

    dnorm1w.SetLinearForm(secmember(1),dndu1, secmember(2),dndv1, dn1w);
    dnorm2w.SetLinearForm(secmember(3),dndu2, secmember(4),dndv2, dn2w);
  }


  // Tops 2d  
  Poles2d(Poles2d.Lower()).SetCoord(sol(1),sol(2));
  Poles2d(Poles2d.Upper()).SetCoord(sol(3),sol(4));
  if (!istgt) {
    DPoles2d(Poles2d.Lower()).SetCoord(secmember(1),secmember(2));
    DPoles2d(Poles2d.Upper()).SetCoord(secmember(3),secmember(4));
  }

  // the linear case is processed...
  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts1;
    Poles(upp) = pts2;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    if (!istgt) {
      DPoles(low) = tg1;
      DPoles(upp) = tg2;
      DWeights(low) = 0.0;
      DWeights(upp) = 0.0;
    }
    return (!istgt);
  }

  // Case of the circle
  norm1 = nplan.Crossed(ns1).Magnitude();
  norm2 = nplan.Crossed(ns2).Magnitude();
  if (norm1 < Eps) {
    norm1 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRad : Surface singuliere " << std::endl;
#endif
  }
  if (norm2 < Eps) {
   norm2 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
   std::cout << " ConstRad : Surface singuliere " << std::endl;
#endif
 }

  ns1.SetLinearForm(nplan.Dot(ns1)/norm1,nplan, -1./norm1,ns1);
  ns2.SetLinearForm(nplan.Dot(ns2)/norm2,nplan, -1./norm2,ns2);

  Center.SetXYZ(pts1.XYZ()+ray1*ns1.XYZ());
  if (!istgt) {
    tgc.SetLinearForm(ray1,dnorm1w,tg1); //  = tg1.Added(ray1*dn1w);
  }

  // ns1 is oriented from the center to pts1, and ns2 from the center to pts2
  // and the trihedron ns1,ns2,nplan is made direct

  if (ray1 > 0.) {
    ns1.Reverse();
    if (!istgt) {
      dnorm1w.Reverse();
    }
  }
  if (ray2 >0.) {
    ns2.Reverse();
    if (!istgt) {
      dnorm2w.Reverse();
    }
  }
  if (choix%2 != 0) {
    np.Reverse();
    dnp.Reverse();
  }
  
  if (!istgt) {
    return GeomFill::GetCircle(myTConv, 
			       ns1, ns2, 
			       dnorm1w, dnorm2w, 
			       np, dnp, 
			       pts1, pts2, 
			       tg1, tg2, 
			       Abs(ray1), 0, 
			       Center, tgc, 
			       Poles, 
			       DPoles,
			       Weights, 
			       DWeights); 
  }
  else {
    GeomFill::GetCircle(myTConv,
			ns1, ns2, 
			np, pts1, pts2,
			Abs(ray1), Center, 
			Poles, Weights);
    return Standard_False;
  }
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc_ConstRad::Section
  (const Blend_Point& P,
   TColgp_Array1OfPnt& Poles,
   TColgp_Array1OfVec& DPoles,
   TColgp_Array1OfVec& D2Poles,
   TColgp_Array1OfPnt2d& Poles2d,
   TColgp_Array1OfVec2d& DPoles2d,
   TColgp_Array1OfVec2d& D2Poles2d,
   TColStd_Array1OfReal& Weights,
   TColStd_Array1OfReal& DWeights,
   TColStd_Array1OfReal& D2Weights)
{
  gp_Vec ns1,ns2, np, dnp, d2np, dnorm1w, dnorm2w, d2norm1w, d2norm2w;
  gp_Vec tgc, dtgc, dtg1, dtg2, temp, tempbis;
  Standard_Real norm1, norm2;

  gp_Pnt Center;
  math_Vector X(1,4), sol(1,4), secmember(1,4);
  math_Matrix D2DXdSdt(1,4,1,4);

  Standard_Real prm = P.Parameter();
  Standard_Integer low = Poles.Lower();
  Standard_Integer upp = Poles.Upper();
  Standard_Boolean istgt = Standard_True;

  P.ParametersOnS1(X(1), X(2));
  P.ParametersOnS2(X(3), X(4));

/*  Pour debuger par des D.F
#ifdef OCCT_DEBUG
  Standard_Real deltat = 1.e-7;
  if (prm==tcurv->LastParameter()){deltat *= -1;} //Pour les discont
  Standard_Real deltaX = 1.e-7;
  Standard_Real seuil = 1.e-3;
  Standard_Integer ii, jj;
  gp_Vec d_plan, d1, d2, pdiff;
  math_Matrix M(1,4,1,4), MDiff(1,4,1,4);
  math_Matrix Mu1(1,4,1,4), Mv1(1,4,1,4);
  math_Matrix Mu2(1,4,1,4), Mv2(1,4,1,4);  
  math_Vector V(1,4), VDiff(1,4),dx(1,4);

  dx = X;
  dx(1)+=deltaX;
  ComputeValues(dx, 1, Standard_True, prm );
  Mu1 = DEDX;

  dx = X;
  dx(2)+=deltaX;
  ComputeValues(dx, 1, Standard_True, prm);
  Mv1 = DEDX;

  dx = X;
  dx(3)+=deltaX;
  ComputeValues(dx, 1, Standard_True, prm  );
  Mu2 = DEDX;

  dx = X;
  dx(4)+=deltaX;
  ComputeValues(dx, 1,  Standard_True, prm );
  Mv2 = DEDX;

  ComputeValues(X, 1, Standard_True, prm+deltat);
  M = DEDX;
  V = DEDT;
  d_plan = dnplan;
  d1 = dn1w;
  d2 = dn2w;
# endif 
*/

  // Calculation of equations
  ComputeValues(X, 2, Standard_True, prm);
  distmin = Min (distmin, pts1.Distance(pts2));

/*
#ifdef OCCT_DEBUG
  MDiff = (M - DEDX)*(1/deltat);
  VDiff = (V - DEDT)*(1/deltat);

  pdiff = (d_plan - dnplan)*(1/deltat);
  if ((pdiff-d2nplan).Magnitude() > seuil*(pdiff.Magnitude()+1))
    {
      std::cout << "d2nplan = (" << d2nplan.X() << ","<< d2nplan.Y() << ","<< d2nplan.Z() << ")"<<std::endl;
      std::cout << "Diff fi = (" << pdiff.X() << ","<<  pdiff.Y() << ","<<  pdiff.Z() << ")"<<std::endl;
    }

  pdiff = (d1 - dn1w)*(1/deltat);
  if ((pdiff-d2n1w).Magnitude() > seuil*(pdiff.Magnitude()+1))
    {
      std::cout << "d2n1w   = (" << d2n1w.X() << ","<< d2n1w.Y() << ","<< d2n1w.Z() << ")"<<std::endl;
      std::cout << "Diff fi = (" << pdiff.X() << ","<<  pdiff.Y() << ","<<  pdiff.Z() << ")"<<std::endl;
    }
  pdiff = (d2 - dn2w)*(1/deltat);
  if ((pdiff-d2n2w).Magnitude() > seuil*(pdiff.Magnitude()+1))
    {
      std::cout << "d2n2w   = (" << d2n2w.X() << ","<< d2n2w.Y() << ","<< d2n2w.Z() << ")"<<std::endl;
      std::cout << "Diff fi = (" << pdiff.X() << ","<<  pdiff.Y() << ","<<  pdiff.Z() << ")"<<std::endl;
    }
 

  for ( ii=1; ii<=4; ii++) {
    if (Abs(VDiff(ii)-D2EDT2(ii)) > seuil*(Abs(D2EDT2(ii))+1)) 
      {
	  std::cout << "erreur sur D2EDT2 : "<< ii << std::endl;
          std::cout << D2EDT2(ii) << " D.F = " << VDiff(ii) << std::endl;
	} 
	for (jj=1; jj<=4; jj++) {
	  if (Abs(MDiff(ii,jj)-D2EDXDT(ii, jj)) > 
	      1.e-3*(Abs(D2EDXDT(ii, jj))+1.e-2)) 
	      {
		std::cout << "erreur sur D2EDXDT : "<< ii << " , " << jj << std::endl;
		std::cout << D2EDXDT(ii,jj) << " D.F = " << MDiff(ii,jj) << std::endl;
	      }
	}
  }
// Test de D2EDX2 en u1
  MDiff = (Mu1 - DEDX)/deltaX;
  for (ii=1; ii<=4; ii++) {
    for (jj=1; jj<=4; jj++) {
      if (Abs(MDiff(ii,jj)-D2EDX2(ii, jj, 1)) > 
	  seuil*(Abs(D2EDX2(ii, jj, 1))+1)) 
	{
	  std::cout << "erreur sur D2EDX2 : "<< ii << " , " << jj << " , " << 1 << std::endl;
	  std::cout << D2EDX2(ii,jj, 1) << " D.F = " << MDiff(ii,jj) << std::endl;
	}
    }
  }

// Test de D2EDX2 en v1
  MDiff = (Mv1 - DEDX)/deltaX;
  for (ii=1; ii<=4; ii++) {
    for (jj=1; jj<=4; jj++) {
      if (Abs(MDiff(ii,jj)-D2EDX2(ii, jj, 2)) > 
	  seuil*(Abs(D2EDX2(ii, jj, 2))+1)) 
	{
	  std::cout << "erreur sur D2EDX2 : "<< ii << " , " << jj << " , " << 2 << std::endl;
	  std::cout << D2EDX2(ii,jj, 2) << " D.F = " << MDiff(ii,jj) << std::endl;
	}
    }
  }
// Test de D2EDX2 en u2
  MDiff = (Mu2 - DEDX)/deltaX;
  for (ii=1; ii<=4; ii++) {
    for (jj=1; jj<=4; jj++) {
      if (Abs(MDiff(ii,jj)-D2EDX2(ii, jj, 3)) > 
	  seuil*(Abs(D2EDX2(ii, jj, 3))+1)) 
	{
	  std::cout << "erreur sur D2EDX2 : "<< ii << " , " << jj << " , " << 3 << std::endl;
	  std::cout << D2EDX2(ii,jj, 3) << " D.F = " << MDiff(ii,jj) << std::endl;
	}
    }
  }

// Test de D2EDX2 en v2
  MDiff = (Mv2 - DEDX)/deltaX;
  for (ii=1; ii<=4; ii++) {
    for (jj=1; jj<=4; jj++) {
      if (Abs(MDiff(ii,jj)-D2EDX2(ii, jj, 4)) > 
	  seuil*(Abs(D2EDX2(ii, jj, 4))+1)) 
	{
	  std::cout << "erreur sur D2EDX2 : "<< ii << " , " << jj << " , " << 4 << std::endl;
	  std::cout << D2EDX2(ii,jj, 4) << " D.F = " << MDiff(ii,jj) << std::endl;
	}
    }
  }

#endif
*/
  // ns1, ns2, np are copied locally to avois crushing the fields !
  ns1 = nsurf1;
  ns2 = nsurf2;
  np  = nplan;
  dnp = dnplan;
  d2np = d2nplan;

  // Calculation of derivatives


  if ( ! pts1.IsEqual(pts2, 1.e-4)) {
    math_Gauss Resol(DEDX, 1.e-9); // Precise tolerance !!!!! 
    // Calculation of derivatives Processing Normal
    if (Resol.IsDone()) {  
      Resol.Solve(-DEDT, sol);
      D2EDX2.Multiply(sol, D2DXdSdt);    
      secmember = - (D2EDT2 + (2*D2EDXDT+D2DXdSdt)*sol);
      Resol.Solve(secmember);
      istgt = Standard_False;
    }
  }
  
  if (istgt) {
    math_SVD SingRS (DEDX);
    math_Vector Vbis(1,4);
    if (SingRS.IsDone()) {
      SingRS.Solve(-DEDT, sol, 1.e-6);
      D2EDX2.Multiply(sol, D2DXdSdt);
      Vbis = - (D2EDT2 + (2*D2EDXDT+D2DXdSdt)*sol);      
      SingRS.Solve( Vbis, secmember, 1.e-6);
      istgt = Standard_False;
    }
  }

  if (!istgt) { 
    tg1.SetLinearForm(sol(1),d1u1, sol(2),d1v1);
    tg2.SetLinearForm(sol(3),d1u2, sol(4),d1v2);

    dnorm1w.SetLinearForm(sol(1),dndu1, sol(2),dndv1, dn1w);
    dnorm2w.SetLinearForm(sol(3),dndu2, sol(4),dndv2, dn2w);
    temp.SetLinearForm(sol(1)*sol(1), d2u1, 
		       2*sol(1)*sol(2), d2uv1,
		       sol(2)*sol(2), d2v1);

    dtg1.SetLinearForm(secmember(1),d1u1, secmember(2),d1v1, temp);

    temp.SetLinearForm(sol(3)*sol(3), d2u2, 
		       2*sol(3)*sol(4), d2uv2,
		       sol(4)*sol(4), d2v2);
    dtg2.SetLinearForm(secmember(3),d1u2, secmember(4),d1v2, temp);    

    temp.SetLinearForm(sol(1)*sol(1), d2ndu1, 
		       2*sol(1)*sol(2), d2nduv1,
		       sol(2)*sol(2), d2ndv1);

    tempbis.SetLinearForm(2*sol(1), d2ndtu1, 
			  2*sol(2), d2ndtv1,
		          d2n1w);
    temp+= tempbis;
    d2norm1w.SetLinearForm(secmember(1),dndu1, secmember(2),dndv1, temp);


    temp.SetLinearForm(sol(3)*sol(3),   d2ndu2, 
		       2*sol(3)*sol(4), d2nduv2,
		       sol(4)*sol(4),   d2ndv2);
    tempbis.SetLinearForm(2*sol(3), d2ndtu2, 
			  2*sol(4), d2ndtv2,
		          d2n2w);
    temp+= tempbis;
    d2norm2w.SetLinearForm(secmember(3),dndu2, secmember(4),dndv2, temp);
  }

  // Tops 2d  
  Poles2d(Poles2d.Lower()).SetCoord(X(1),X(2));
  Poles2d(Poles2d.Upper()).SetCoord(X(3),X(4));
  if (!istgt) {
    DPoles2d(Poles2d.Lower()) .SetCoord(sol(1),sol(2));
    DPoles2d(Poles2d.Upper()) .SetCoord(sol(3),sol(4));
    D2Poles2d(Poles2d.Lower()).SetCoord(secmember(1), secmember(2));
    D2Poles2d(Poles2d.Upper()).SetCoord(secmember(3), secmember(4));    
  }

  // linear case is processed...
  if (mySShape == BlendFunc_Linear) {
    Poles(low) = pts1;
    Poles(upp) = pts2;
    Weights(low) = 1.0;
    Weights(upp) = 1.0;
    if (!istgt) {
      DPoles(low) = tg1;
      DPoles(upp) = tg2;
      DPoles(low) = dtg1;
      DPoles(upp) = dtg2;
      DWeights(low) = 0.0;
      DWeights(upp) = 0.0;
      D2Weights(low) = 0.0;
      D2Weights(upp) = 0.0;
    }
    return (!istgt);
  }

  // Case of circle
  norm1 = nplan.Crossed(ns1).Magnitude();
  norm2 = nplan.Crossed(ns2).Magnitude();
  if (norm1 < Eps) {
    norm1 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRad : Surface singuliere " << std::endl;
#endif
  }
  if (norm2 < Eps) {
    norm2 = 1; // Unsatisfactory, but it is not necessary to stop
#ifdef OCCT_DEBUG
    std::cout << " ConstRad : Surface singuliere " << std::endl;
#endif
 }

  ns1.SetLinearForm(nplan.Dot(ns1)/norm1,nplan, -1./norm1, ns1);
  ns2.SetLinearForm(nplan.Dot(ns2)/norm2,nplan, -1./norm2, ns2);

  Center.SetXYZ(pts1.XYZ()+ray1*ns1.XYZ());
  if (!istgt) {
    tgc.SetLinearForm(ray1,dnorm1w,tg1); 
    dtgc.SetLinearForm(ray1, d2norm1w, dtg1);
  }

  // ns1 is oriented from the center to pts1 and ns2 from the center to pts2
  // trihedron ns1,ns2,nplan is made direct

  if (ray1 > 0.) {
    ns1.Reverse();
    if (!istgt) {
      dnorm1w.Reverse();
      d2norm1w.Reverse();
    }
  }
  if (ray2 >0.) {
    ns2.Reverse();
    if (!istgt) {
      dnorm2w.Reverse();
      d2norm2w.Reverse();
    }
  }
  if (choix%2 != 0) {
    np.Reverse();
    dnp.Reverse();
    d2np.Reverse();
  }
  
  if (!istgt) {
    return GeomFill::GetCircle(myTConv, 
			       ns1, ns2, 
			       dnorm1w, dnorm2w,
			       d2norm1w, d2norm2w, 
			       np, dnp, d2np,
			       pts1, pts2, 
			       tg1, tg2, 
			       dtg1, dtg2,
			       Abs(ray1), 0, 0, 
			       Center, tgc, dtgc, 
			       Poles, 
			       DPoles,
			       D2Poles,
			       Weights, 
			       DWeights,
			       D2Weights); 
  }
  else {
    GeomFill::GetCircle(myTConv,
			ns1, ns2, 
			nplan, pts1, pts2,
			Abs(ray1), Center, 
			Poles, Weights);
    return Standard_False;
  }
}

//=======================================================================
//function : AxeRot
//purpose  : 
//=======================================================================

gp_Ax1 BlendFunc_ConstRad::AxeRot (const Standard_Real Prm)
{
  gp_Ax1 axrot;
  gp_Vec dirax, d1gui, d2gui, np, dnp;
  gp_Pnt oriax, ptgui;

  curv->D2(Prm,ptgui,d1gui,d2gui);

  Standard_Real normtg = d1gui.Magnitude();
  np  = d1gui.Normalized();
  dnp.SetLinearForm(1./normtg, d2gui,
		   -1./normtg*(np.Dot(d2gui)), np);

  dirax = np.Crossed(dnp);
  if (dirax.Magnitude() >= gp::Resolution()) {

    axrot.SetDirection(dirax);
  }
  else {
    axrot.SetDirection(np);  // To avoid stop
  }
  if (dnp.Magnitude() >= gp::Resolution()) {
    oriax.SetXYZ(ptgui.XYZ()+
		 (normtg/dnp.Magnitude())*dnp.Normalized().XYZ());
  }
  else {
    oriax.SetXYZ(ptgui.XYZ());
  }
  axrot.SetLocation(oriax);
  return axrot;
}

void BlendFunc_ConstRad::Resolution(const Standard_Integer IC2d,
				    const Standard_Real Tol,
				    Standard_Real& TolU,
				    Standard_Real& TolV) const
{
  if(IC2d == 1){
    TolU = surf1->UResolution(Tol);
    TolV = surf1->VResolution(Tol);
  }
  else {
    TolU = surf2->UResolution(Tol);
    TolV = surf2->VResolution(Tol);
  }
}
