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

//-- IntAna_IntLinTorus.cxx 
//-- lbr : la methode avec les coefficients est catastrophique. 
//--       Mise en place d'une vraie solution. 

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <IntAna_IntLinTorus.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

IntAna_IntLinTorus::IntAna_IntLinTorus () : done(Standard_False)
{}

IntAna_IntLinTorus::IntAna_IntLinTorus (const gp_Lin& L, const gp_Torus& T)  {
  Perform(L,T);
}


void IntAna_IntLinTorus::Perform (const gp_Lin& L, const gp_Torus& T) {
  gp_Pnt PL=L.Location();
  gp_Dir DL=L.Direction();

  // Reparametrize the line:
  // set its location as nearest to the location of torus
  gp_Pnt TorLoc = T.Location();
  Standard_Real ParamOfNewPL = gp_Vec(PL, TorLoc).Dot(gp_Vec(DL));
  gp_Pnt NewPL( PL.XYZ() + ParamOfNewPL * DL.XYZ() );

  //--------------------------------------------------------------
  //-- Coefficients de la ligne dans le repere du cone
  //-- 
  gp_Trsf trsf;
  trsf.SetTransformation(T.Position());
  NewPL.Transform(trsf);
  DL.Transform(trsf);

  Standard_Real a,b,c,x1,y1,z1,x0,y0,z0;
  Standard_Real a0,a1,a2,a3,a4;
  Standard_Real R,r,R2,r2;

  x1 = DL.X(); y1 = DL.Y(); z1 = DL.Z();
  x0 = NewPL.X(); y0 = NewPL.Y(); z0 = NewPL.Z();
  R = T.MajorRadius(); R2 = R*R;
  r = T.MinorRadius(); r2 = r*r;

  a = x1*x1+y1*y1+z1*z1;
  b = 2.0*(x1*x0+y1*y0+z1*z0);
  c = x0*x0+y0*y0+z0*z0 - (R2+r2);

  a4 = a*a;
  a3 = 2.0*a*b;
  a2 = 2.0*a*c+4.0*R2*z1*z1+b*b;
  a1 = 2.0*b*c+8.0*R2*z1*z0;
  a0 = c*c+4.0*R2*(z0*z0-r2);

  Standard_Real u,v;
  math_DirectPolynomialRoots mdpr(a4,a3,a2,a1,a0);
  if(mdpr.IsDone()) {
     Standard_Integer nbsolvalid = 0; 
     Standard_Integer n = mdpr.NbSolutions();
     for(Standard_Integer i = 1; i<=n ; i++) { 
	Standard_Real t = mdpr.Value(i);
	t += ParamOfNewPL;
        gp_Pnt PSolL(ElCLib::Value(t,L));
        ElSLib::Parameters(T,PSolL,u,v);
	gp_Pnt PSolT(ElSLib::Value(u,v,T));
        a0 = PSolT.SquareDistance(PSolL); 

	if(a0>0.0000000001) { 
#if 0 
	   cout<<" ------- Erreur : P Ligne < > P Tore "<<endl;
           cout<<"Ligne :  X:"<<PSolL.X()<<"  Y:"<<PSolL.Y()<<"  Z:"<<PSolL.Z()<<" l:"<<t<<endl;
	   cout<<"Tore  :  X:"<<PSolT.X()<<"  Y:"<<PSolT.Y()<<"  Z:"<<PSolT.Z()<<" u:"<<u<<" v:"<<v<<endl;
#endif
	   }         
        else { 
	  theParam[nbsolvalid] = t;
          theFi[nbsolvalid]    = u;
          theTheta[nbsolvalid] = v;
          thePoint[nbsolvalid] = PSolL;
          nbsolvalid++;
        }
      }
      nbpt = nbsolvalid;
      done = Standard_True;
   }
   else { 
      nbpt = 0;
      done = Standard_False;
   }
}


#if 0 

static void MULT_A3_B1(Standard_Real& c4,
                       Standard_Real& c3,
                       Standard_Real& c2,
                       Standard_Real& c1,
                       Standard_Real& c0,
                       const Standard_Real a3,
                       const Standard_Real a2,
                       const Standard_Real a1,
                       const Standard_Real a0,
                       const Standard_Real b1,
                       const Standard_Real b0) { 
  c4 = a3 * b1;
  c3 = a3 * b0  + a2 * b1;
  c2 =            a2 * b0  + a1 * b1;
  c1 =                       a1 * b0  + a0 * b1;
  c0 =                                  a0 * b0;
}
                       
static void MULT_A2_B2(Standard_Real& c4,
                       Standard_Real& c3,
                       Standard_Real& c2,
                       Standard_Real& c1,
                       Standard_Real& c0,
                       const Standard_Real a2,
                       const Standard_Real a1,
                       const Standard_Real a0,
                       const Standard_Real b2,
                       const Standard_Real b1,
                       const Standard_Real b0) {
  c4 = a2 * b2;
  c3 = a2 * b1 + a1 * b2;
  c2 = a2 * b0 + a1 * b1 + a0 * b2;
  c1 =           a1 * b0 + a0 * b1;
  c0 =                     a0 * b0;
}

static void MULT_A2_B1(Standard_Real& c3,
                       Standard_Real& c2,
                       Standard_Real& c1,
                       Standard_Real& c0,
                       const Standard_Real a2,
                       const Standard_Real a1,
                       const Standard_Real a0,
                       const Standard_Real b1,
                       const Standard_Real b0) {
  c3 = a2 * b1;
  c2 = a2 * b0 + a1 * b1;
  c1 =           a1 * b0 + a0 * b1;
  c0 =                     a0 * b0;
}

void IntAna_IntLinTorus::Perform (const gp_Lin& L, const gp_Torus& T) {
  TColStd_Array1OfReal C(1,31);
  T.Coefficients(C);
  const gp_Pnt& PL=L.Location();
  const gp_Dir& DL=L.Direction();

  //----------------------------------------------------------------
  //-- X   = ax1 l  + ax0   
  //-- X2  = ax2 l2 + 2 ax1 ax0 l    + bx2
  //-- X3  = ax3 l3 + 3 ax2 ax0 l2  + 3 ax1 bx2 l    + bx3
  //-- X4  = ax4 l4 + 4 ax3 ax0 l3  + 6 ax2 bx2 l2  + 4 ax1 bx3 l + bx4

  Standard_Real ax1,ax2,ax3,ax4,ax0,bx2,bx3,bx4;
  Standard_Real ay1,ay2,ay3,ay4,ay0,by2,by3,by4;
  Standard_Real az1,az2,az3,az4,az0,bz2,bz3,bz4;
  Standard_Real c0,c1,c2,c3,c4;
  ax1=DL.X(); ax0=PL.X();  ay1=DL.Y(); ay0=PL.Y(); az1=DL.Z(); az0=PL.Z();
  ax2=ax1*ax1; ax3=ax2*ax1; ax4=ax3*ax1; bx2=ax0*ax0; bx3=bx2*ax0; bx4=bx3*ax0;
  ay2=ay1*ay1; ay3=ay2*ay1; ay4=ay3*ay1; by2=ay0*ay0; by3=by2*ay0; by4=by3*ay0;
  az2=az1*az1; az3=az2*az1; az4=az3*az1; bz2=az0*az0; bz3=bz2*az0; bz4=bz3*az0;
	
  //--------------------------------------------------------------------------- Terme X**4
  Standard_Real c=C(1);  
  Standard_Real a4 = c *ax4;
  Standard_Real a3 = c *4.0*ax3*ax0;
  Standard_Real a2 = c *6.0*ax2*bx2;
  Standard_Real a1 = c *4.0*ax1*bx3;
  Standard_Real a0 = c *bx4;
  //--------------------------------------------------------------------------- Terme Y**4
  c = C(2);
  a4+=  c*ay4; 
  a3+=  c*4.0*ay3*ay0;
  a2+=  c*6.0*ay2*by2;
  a1+=  c*4.0*ay1*by3;
  a0+=  c*by4;
  //--------------------------------------------------------------------------- Terme Z**4
  c = C(3);
  a4+=  c*az4    ; 
  a3+=  c*4.0*az3*az0;
  a2+=  c*6.0*az2*bz2;
  a1+=  c*4.0*az1*bz3;
  a0+=  c*bz4;
  //--------------------------------------------------------------------------- Terme X**3 Y   
  c = C(4);
  MULT_A3_B1(c4,c3,c2,c1,c0,    ax3, 3.0*ax2*ax0, 3.0*ax1*bx2, bx3,     ay1,ay0);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 	
  //--------------------------------------------------------------------------- Terme X**3 Z   
  c = C(5);
  MULT_A3_B1(c4,c3,c2,c1,c0,    ax3, 3.0*ax2*ax0, 3.0*ax1*bx2, bx3,     az1,az0);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 	
  //--------------------------------------------------------------------------- Terme Y**3 X   
  c = C(6);
  MULT_A3_B1(c4,c3,c2,c1,c0,    ay3, 3.0*ay2*ay0, 3.0*ay1*by2, by3,     ax1,ax0);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 	
  //--------------------------------------------------------------------------- Terme Y**3 Z   
  c = C(7);
  MULT_A3_B1(c4,c3,c2,c1,c0,    ay3, 3.0*ay2*ay0, 3.0*ay1*by2, by3,     az1,az0);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 	
  //--------------------------------------------------------------------------- Terme Z**3 X   
  c = C(8);
  MULT_A3_B1(c4,c3,c2,c1,c0,    az3, 3.0*az2*az0, 3.0*az1*bz2, bz3,     ax1,ax0);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 	
  //--------------------------------------------------------------------------- Terme Z**3 Y   
  c = C(9);
  MULT_A3_B1(c4,c3,c2,c1,c0,    az3, 3.0*az2*az0, 3.0*az1*bz2, bz3,     ay1,ay0);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 	


  //--------------------------------------------------------------------------- Terme X**2 Y**2
  c = C(10); 
  MULT_A2_B2(c4,c3,c2,c1,c0,  ax2, 2.0*ax1*ax0, bx2,    ay2,2.0*ay1*ay0, by2);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 
  //--------------------------------------------------------------------------- Terme X**2 Z**2
  c = C(11);
  MULT_A2_B2(c4,c3,c2,c1,c0,  ax2, 2.0*ax1*ax0, bx2,    az2,2.0*az1*az0, bz2);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 
  //--------------------------------------------------------------------------- Terme Y**2 Z**2
  c = C(12);
  MULT_A2_B2(c4,c3,c2,c1,c0,  ay2, 2.0*ay1*ay0, by2,    az2,2.0*az1*az0, bz2);
  a4+=  c*c4; a3+=  c*c3; a2+=  c*c2;  a1+=  c*c1; a0+=  c*c0; 


  //--------------------------------------------------------------------------- Terme X**3
  c = C(13);
  a3+= c*( ax3 );
  a2+= c*( 3.0*ax2*ax0 );
  a1+= c*( 3.0*ax1*bx2 );
  a0+= c*( bx3 );
  //--------------------------------------------------------------------------- Terme Y**3
  c = C(14);
  a3+= c*( ay3 );
  a2+= c*( 3.0*ay2*ay0 );
  a1+= c*( 3.0*ay1*by2 );
  a0+= c*( by3 );
  //--------------------------------------------------------------------------- Terme Y**3
  c = C(15);
  a3+= c*( az3 );
  a2+= c*( 3.0*az2*az0 );
  a1+= c*( 3.0*az1*bz2 );
  a0+= c*( bz3 );  


  //--------------------------------------------------------------------------- Terme X**2 Y
  c = C(16);
  MULT_A2_B1(c3,c2,c1,c0,   ax2, 2.0*ax1*ax0, bx2,   ay1,ay0);
  a3+= c*c3; a2+= c* c2; a1+= c* c1; a0+= c*c0;
  //--------------------------------------------------------------------------- Terme X**2 Z
  c = C(17);
  MULT_A2_B1(c3,c2,c1,c0,   ax2, 2.0*ax1*ax0, bx2,   az1,az0);
  a3+= c*c3; a2+= c* c2; a1+= c* c1; a0+= c*c0;
  //--------------------------------------------------------------------------- Terme Y**2 X
  c = C(18);
  MULT_A2_B1(c3,c2,c1,c0,   ay2, 2.0*ay1*ay0, by2,   ax1,ax0);
  a3+= c*c3; a2+= c* c2; a1+= c* c1; a0+= c*c0;
  //--------------------------------------------------------------------------- Terme Y**2 Z
  c = C(19);
  MULT_A2_B1(c3,c2,c1,c0,   ay2, 2.0*ay1*ay0, by2,   az1,az0);
  a3+= c*c3; a2+= c* c2; a1+= c* c1; a0+= c*c0;
  //--------------------------------------------------------------------------- Terme Z**2 X
  c = C(20);
  MULT_A2_B1(c3,c2,c1,c0,   az2, 2.0*az1*az0, bz2,   ax1,ax0);
  a3+= c*c3; a2+= c* c2; a1+= c* c1; a0+= c*c0;
  //--------------------------------------------------------------------------- Terme Z**2 Y
  c = C(21);
  MULT_A2_B1(c3,c2,c1,c0,   az2, 2.0*az1*az0, bz2,   ay1,ay0);
  a3+= c*c3; a2+= c* c2; a1+= c* c1; a0+= c*c0;


  //--------------------------------------------------------------------------- Terme X**2 
  c = C(22);
  a2+= c*ax2; 
  a1+= c*2.0*ax1*ax0; 
  a0+= c*bx2;
  //--------------------------------------------------------------------------- Terme Y**2 
  c = C(23);
  a2+= c*ay2; 
  a1+= c*2.0*ay1*ay0; 
  a0+= c*by2;
  //--------------------------------------------------------------------------- Terme Z**2 
  c = C(24);
  a2+= c*az2; 
  a1+= c*2.0*az1*az0; 
  a0+= c*bz2;
 

  //--------------------------------------------------------------------------- Terme X  Y  
  c = C(25);
  a2+= c*(ax1*ay1); 
  a1+= c*(ax1*ay0 + ax0*ay1); 
  a0+= c*(ax0*ay0);
  //--------------------------------------------------------------------------- Terme X  Z  
  c = C(26);
  a2+= c*(ax1*az1); 
  a1+= c*(ax1*az0 + ax0*az1); 
  a0+= c*(ax0*az0);
  //--------------------------------------------------------------------------- Terme Y  Z  
  c = C(27);
  a2+= c*(ay1*az1); 
  a1+= c*(ay1*az0 + ay0*az1); 
  a0+= c*(ay0*az0);

  //--------------------------------------------------------------------------- Terme X 
  c = C(28);
  a1+= c*ax1; 
  a0+= c*ax0;
  //--------------------------------------------------------------------------- Terme Y 
  c = C(29);
  a1+= c*ay1; 
  a0+= c*ay0;
  //--------------------------------------------------------------------------- Terme Z 
  c = C(30);
  a1+= c*az1; 
  a0+= c*az0;

  //--------------------------------------------------------------------------- Terme  Constant
  c = C(31);
  a0+=c;



  cout<<"\n ---------- Coefficients Line - Torus  : "<<endl;
  cout<<" a0 : "<<a0<<endl;
  cout<<" a1 : "<<a1<<endl;
  cout<<" a2 : "<<a2<<endl;
  cout<<" a3 : "<<a3<<endl;
  cout<<" a4 : "<<a4<<endl;

  Standard_Real u,v;
  math_DirectPolynomialRoots mdpr(a4,a3,a2,a1,a0);
  if(mdpr.IsDone()) {
     Standard_Integer nbsolvalid = 0; 
     Standard_Integer n = mdpr.NbSolutions();
     for(Standard_Integer i = 1; i<=n ; i++) { 
	Standard_Real t = mdpr.Value(i);
        gp_Pnt PSolL(ax0+ax1*t, ay0+ay1*t, az0+az1*t);
        ElSLib::Parameters(T,PSolL,u,v);
	gp_Pnt PSolT(ElSLib::Value(u,v,T));
        
        a0 = PSolT.SquareDistance(PSolL); 
	if(a0>0.0000000001) { 
	   cout<<" ------- Erreur : P Ligne < > P Tore ";
           cout<<"Ligne :  X:"<<PSolL.X()<<"  Y:"<<PSolL.Y()<<"  Z:"<<PSolL.Z()<<" l:"<<t<<endl;
	   cout<<"Tore  :  X:"<<PSolL.X()<<"  Y:"<<PSolL.Y()<<"  Z:"<<PSolL.Z()<<" u:"<<u<<" v:"<<v<<endl;
	   }         
        else { 
	  theParam[nbsolvalid] = t;
          theFi[nbsolvalid]    = v;
          theTheta[nbsolvalid] = u;
          thePoint[nbsolvalid] = PSolL;
          nbsolvalid++;
        }
      }
      nbpt = nbsolvalid;
      done = Standard_True;
   }
   else { 
      nbpt = 0;
      done = Standard_False;
   }
} 
#endif


