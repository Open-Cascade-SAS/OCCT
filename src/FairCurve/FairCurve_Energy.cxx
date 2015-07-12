// Created on: 1996-01-22
// Created by: Philippe MANGIN
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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <FairCurve_Energy.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_IntegerVector.hxx>
#include <math_Matrix.hxx>

//=======================================================================
FairCurve_Energy::FairCurve_Energy(const Handle(TColgp_HArray1OfPnt2d)& Poles, 
				   const Standard_Integer ContrOrder1, 
				   const Standard_Integer ContrOrder2, 
				   const Standard_Boolean WithAuxValue,
				   const Standard_Real Angle1, 
				   const Standard_Real Angle2,
				   const Standard_Integer Degree,
				   const Standard_Real Curvature1,
				   const Standard_Real Curvature2 )
//=======================================================================
                         : MyPoles (Poles), 
                           MyContrOrder1(ContrOrder1),     
			   MyContrOrder2(ContrOrder2),
                           MyWithAuxValue(WithAuxValue), 
                           MyNbVar(2*(Poles->Length()-2) - ContrOrder2 - ContrOrder1 + WithAuxValue),
                           MyNbValues(2*Poles->Length() +  WithAuxValue),
                           MyLinearForm(0, 1),
                           MyQuadForm(0, 1),
			   MyGradient( 0, MyNbValues),
                           MyHessian( 0, MyNbValues + MyNbValues*(MyNbValues+1)/2 )
{
  // chesk angles in reference (Ox,Oy)
  gp_XY L0 (Cos(Angle1), Sin(Angle1)), L1 (-Cos(Angle2), Sin(Angle2));
  MyLinearForm.SetValue(0, L0);
  MyLinearForm.SetValue(1, L1);
  gp_XY Q0(-Sin(Angle1), Cos(Angle1)), Q1 (Sin(Angle2), Cos(Angle2));
  MyQuadForm.SetValue(0, ((double)Degree) / (Degree-1) * Curvature1 * Q0);
  MyQuadForm.SetValue(1, ((double)Degree) / (Degree-1) * Curvature2 * Q1);
}

//=======================================================================
Standard_Boolean FairCurve_Energy::Value(const math_Vector& X, 
						 Standard_Real& E)
//=======================================================================
{
   Standard_Boolean IsDone;
   math_Vector Energie(0,0);
   ComputePoles(X);
   IsDone = Compute(0, Energie);
   E  = Energie(0);
   return IsDone;
}

//=======================================================================
Standard_Boolean FairCurve_Energy::Gradient(const math_Vector& X,
						  math_Vector& G)
//=======================================================================
{
   Standard_Boolean IsDone;
   Standard_Real E;
 
   IsDone = Values(X, E, G);
   return IsDone;    
}

//=======================================================================
void FairCurve_Energy::Gradient1(const math_Vector& Vect,
				       math_Vector& Grad)
//=======================================================================
{
  Standard_Integer ii,
                   DebG = Grad.Lower(), FinG = Grad.Upper();
  Standard_Integer Vdeb = 3, 
                   Vfin = 2*MyPoles->Length()-2;

// .... by calculation 
  if (MyContrOrder1 >= 1) {
     gp_XY DPole (Vect(Vdeb), Vect(Vdeb+1));
     Grad(DebG) = MyLinearForm(0) * DPole;
     Vdeb += 2;
     DebG += 1;
  }
  if(MyContrOrder1 == 2) {
     Standard_Real Lambda0 = MyPoles->Value(MyPoles->Lower())
                            .Distance( MyPoles->Value(MyPoles->Lower()+1) );
     gp_XY DPole (Vect(Vdeb), Vect(Vdeb+1));
     Grad(DebG-1) += (MyLinearForm(0) + 2*Lambda0*MyQuadForm(0)) * DPole;
     Grad(DebG) = MyLinearForm(0) * DPole;
     Vdeb += 2;
     DebG += 1;
  }
  if (MyWithAuxValue) { 
     Grad(FinG) = Vect( 2*MyPoles->Length()+1 );
     FinG -= 1;
  }  
  if (MyContrOrder2 >= 1) {
     gp_XY DPole (Vect(Vfin-1), Vect(Vfin));
     Grad(FinG) = MyLinearForm(1) * DPole;
     FinG -= 1;
  }
  if(MyContrOrder2 == 2) {
     Standard_Real Lambda1 = MyPoles->Value(MyPoles->Upper()) 
                            .Distance(MyPoles->Value(MyPoles->Upper()-1) );
     gp_XY DPole (Vect(Vfin-3), Vect(Vfin-2));
     Grad(FinG) =  Grad(FinG+1) +  (MyLinearForm(1) + 2*Lambda1*MyQuadForm(1)) * DPole;
     Grad(FinG+1) = MyLinearForm(1) * DPole;     
     FinG -= 1;
  }
// ... by recopy
   for (ii=DebG; ii<=FinG; ii++) {
     Grad(ii) = Vect(Vdeb);
     Vdeb += 1;
   }
}

//=======================================================================
Standard_Boolean FairCurve_Energy::Values(const math_Vector& X, 
					        Standard_Real& E, 
					        math_Vector& G)
//=======================================================================
{
   Standard_Boolean IsDone;

   ComputePoles(X);
   IsDone = Compute(1, MyGradient);
   if (IsDone) {
     E = MyGradient(0);
     Gradient1(MyGradient, G);
   }
   return IsDone;
}

//=======================================================================
Standard_Boolean FairCurve_Energy::Values(const math_Vector& X, 
					        Standard_Real& E, 
						math_Vector& G, 
						math_Matrix& H)
//=======================================================================
{
   Standard_Boolean IsDone;

   ComputePoles(X);
   IsDone = Compute(2, MyHessian);
   if (IsDone) {
     E = MyHessian(0);
     Gradient1(MyHessian, G);
     Hessian1 (MyHessian, H);
   }
   return IsDone;
}


//=======================================================================
void FairCurve_Energy::Hessian1(const math_Vector& Vect,
			              math_Matrix& H)
//=======================================================================
{

  Standard_Integer ii, jj, kk, Vk;
  Standard_Integer Vdeb = 3 + 2*MyContrOrder1, 
                   Vfin = 2*MyPoles->Length() - 2*(MyContrOrder2+1),
                   Vup  = 2*MyPoles->Length()+MyWithAuxValue;
  Standard_Integer DebH = 1+MyContrOrder1, 
                   FinH = MyNbVar - MyWithAuxValue - MyContrOrder2 ;
  Standard_Real Cos0 = pow(MyLinearForm(0).X(),2),
                Sin0 = pow(MyLinearForm(0).Y(),2),
                CosSin0 = 2 * MyLinearForm(0).X() * MyLinearForm(0).Y(),
                Cos1 = pow(MyLinearForm(1).X(),2),
                Sin1 = pow(MyLinearForm(1).Y(),2),
                CosSin1 =  2 * MyLinearForm(1).X() * MyLinearForm(1).Y() ;  
  Standard_Real Lambda0=0, Lambda1=0; 

  if (MyContrOrder1 >= 1) {
     Lambda0 = MyPoles->Value(MyPoles->Lower())
              .Distance( MyPoles->Value(MyPoles->Lower()+1) );}
      
  if (MyContrOrder2 >= 1) {
     Lambda1 = MyPoles->Value(MyPoles->Upper()) 
              .Distance(MyPoles->Value(MyPoles->Upper()-1) );}
 

  if (MyContrOrder1 >= 1) {

// calculate the left lambda column --------------------------------

     jj =  Vdeb-2*MyContrOrder1;
     kk=Indice(jj, jj); // X2X2
     ii=Indice(jj+1, jj); // X2Y2
     H(1, 1) = Cos0 * Vect(kk) + CosSin0*Vect(ii) + Sin0 * Vect(ii+1);

     if (MyContrOrder1 >= 2) {	             
       gp_XY Laux = (MyLinearForm(0) + 2*Lambda0*MyQuadForm(0)); 
       jj = Vdeb-2*(MyContrOrder1-1);
       kk=Indice(jj, jj-2); // X1X2
       ii=Indice(jj+1, jj-2);  //X1Y2
       gp_XY Aux(Vect(kk+2), Vect(ii+3));

       H (1, 1) += 2 * (
		   ( MyQuadForm(0).X() * Vect(5) + MyQuadForm(0).Y() * Vect(6) )
                 + ( Laux.X() * ( MyLinearForm(0).X()*Vect(kk) + MyLinearForm(0).Y()*Vect(kk+1))
                 +   Laux.Y() * ( MyLinearForm(0).X()*Vect(ii) + MyLinearForm(0).Y()*Vect(ii+1)) )
                 +   Laux.X() * Laux.Y() * Vect(ii+2) )
                 + ( Pow(Laux.X(),2) * Vect(kk+2) + Pow(Laux.Y(),2) * Vect(ii+3));
                 
       H(2,1) = (Cos0 * Vect(kk) + CosSin0*(Vect(ii)+Vect(kk+1))/2 + Sin0 * Vect(ii+1))
              +  Laux * MyLinearForm(0).Multiplied(Aux)
              + (Laux.X()*MyLinearForm(0).Y() + Laux.Y()*MyLinearForm(0).X()) * Vect(ii+2);
     }
       
      
     if (MyWithAuxValue) { 
        kk =Indice(Vup, Vdeb-2*MyContrOrder1); 
        H(MyNbVar, 1) = MyLinearForm(0).X() * Vect(kk)
                      + MyLinearForm(0).Y() * Vect(kk+1);
     }
  
     if (MyContrOrder2 >= 1) {    
        H(MyNbVar-MyWithAuxValue, 1) = 0; // correct if there are less than 3 nodes
        if (MyContrOrder2 == 2)  {H(MyNbVar-MyWithAuxValue-1, 1) = 0;}
     }


     Vk = Vdeb;
     kk = Indice(Vk, Vdeb-2*MyContrOrder1);
     for (ii=DebH; ii<=FinH; ii++) {
        H(ii, 1) = MyLinearForm(0).X() * Vect(kk)
	         + MyLinearForm(0).Y() * Vect(kk+1);
        kk += Vk;
        Vk++;
     }
   }

// calculate the left line mu ----------------------
  if (MyContrOrder1 >= 2) {
     jj = Vdeb-2*(MyContrOrder1-1);
     kk=Indice(jj, jj); // X3X3
     ii=Indice(jj+1, jj); // X3Y3
     H(2, 2) = Cos0 * Vect(kk) + CosSin0*Vect(ii) + Sin0 * Vect(ii+1);

     if (MyWithAuxValue) { 
        kk =Indice(Vup, Vdeb-2*(MyContrOrder1-1));
	gp_XY Pole (Vect(kk), Vect(kk+1));
        H(MyNbVar, 1) += (MyLinearForm(0) + 2*Lambda0*MyQuadForm(0)) * Pole;
        H(MyNbVar, 2) = MyLinearForm(0).X() * Vect(kk)
                      + MyLinearForm(0).Y() * Vect(kk+1);
     }
  
     if (MyContrOrder2 >= 1) {    
        H(MyNbVar-MyWithAuxValue, 2) = 0; // correct if there are less than 3 nodes
        if (MyContrOrder2 == 2)  {H(MyNbVar-MyWithAuxValue-1, 2) = 0;}
     }
     Vk = Vdeb;

     Standard_Real Xaux = (MyLinearForm(0) + 2*Lambda0*MyQuadForm(0)).X(),
                   Yaux = (MyLinearForm(0) + 2*Lambda0*MyQuadForm(0)).Y();

     kk = Indice(Vk, Vdeb-2*MyContrOrder1+2);
     for (ii=DebH; ii<=FinH; ii++) {
        H(ii, 2) = MyLinearForm(0).X() * Vect(kk)
	         + MyLinearForm(0).Y() * Vect(kk+1);
        H(ii, 1) +=  Xaux * Vect(kk) + Yaux*Vect(kk+1);
        kk += Vk;
        Vk++;
     }
   }
     
// calculate the right lambda line -----------------------
  if (MyContrOrder2 >= 1) {

     jj = FinH + 1;
     Vk = Vfin + 2*MyContrOrder2 - 1;
     kk = Indice(Vk, Vdeb);
     for (ii=DebH; ii<=FinH; ii++) {
       H(jj, ii) = MyLinearForm(1).X() * Vect(kk)
	         + MyLinearForm(1).Y() * Vect(kk+Vk);
       kk++;
     }

     kk = Indice(Vk, Vk);
     H(jj, jj) = Cos1 * Vect(kk) + CosSin1 * Vect(kk+Vk) + Sin1 * Vect(kk+Vk+1);

     if (MyContrOrder2 >= 2) {
     // H(jj,jj) +=
       gp_XY Laux = (MyLinearForm(1) + 2*Lambda1*MyQuadForm(1)); 
       jj = Vfin + 2*MyContrOrder2 - 3;
       kk=Indice(jj+2, jj); // Xn-1Xn-2
       ii=Indice(jj+3, jj);  //Yn-1Xn-2
       Standard_Integer ll = Indice(jj, jj);

       H (FinH+1, FinH+1) += 2 * (
		   ( MyQuadForm(1).X() * Vect(jj) + MyQuadForm(1).Y() * Vect(jj+1) )
                 + ( Laux.X() * ( MyLinearForm(1).X()*Vect(kk) + MyLinearForm(1).Y()*Vect(ii))
                 +   Laux.Y() * ( MyLinearForm(1).X()*Vect(kk+1) + MyLinearForm(1).Y()*Vect(ii+1)) )
                 +   Laux.X() * Laux.Y() * Vect(ll+jj) )
                 + ( Pow(Laux.X(),2) * Vect(ll) + Pow(Laux.Y(),2) * Vect(ll+jj+1));

       H(FinH+2, FinH+1) =  Cos1 * Vect(kk) + CosSin1*(Vect(ii)+Vect(kk+1))/2 + Sin1 * Vect(ii+1);
       gp_XY Aux(Vect(ll), Vect(ll+jj+1));
       H(FinH+2, FinH+1) += Laux * MyLinearForm(1).Multiplied(Aux)
                         + (Laux.X()*MyLinearForm(1).Y() + Laux.Y()*MyLinearForm(1).X()) 
			   * Vect(ll+jj);
//       H(FinH+2, FinH+1) = 0; // No better alternative. Bug in the previous expression
     }
   }

// calculate the right line mu  -----------------------
  if (MyContrOrder2 >= 2) {
     jj = FinH + 2;
     Vk = Vfin + 2*MyContrOrder2 - 3;
     kk = Indice(Vk, Vdeb);

     Standard_Real Xaux = (MyLinearForm(1) + 2*Lambda1*MyQuadForm(1)).X(),
                   Yaux = (MyLinearForm(1) + 2*Lambda1*MyQuadForm(1)).Y();
 
     for (ii=DebH; ii<=FinH; ii++) {
        H(jj, ii) = MyLinearForm(1).X() * Vect(kk)
	          + MyLinearForm(1).Y() * Vect(kk+Vk);
        // update the right line Lambda 
        H(jj-1,ii) += Xaux*Vect(kk) + Yaux*Vect(kk+Vk);
	kk++;
     }
     kk = Indice(Vk, Vk);
     ii = Indice(Vk+1, Vk);
     H(jj,jj) = Cos1*Vect(kk) + CosSin1*Vect(ii) + Sin1*Vect(ii+1);
   }

// calculate the Auxiliary Variable line -----------------------
   if (MyWithAuxValue) {

     kk = Indice(Vup, Vdeb);
     for (ii=DebH; ii<=FinH; ii++) {
        H(MyNbVar, ii) = Vect(kk);
	kk++;
     }
   
     if (MyContrOrder2 >= 1) {
       kk = Indice(Vup, Vfin+2*MyContrOrder2-1);
       H(MyNbVar, FinH+1) = 
               MyLinearForm(1).X() * Vect(kk) + MyLinearForm(1).Y() * Vect(kk+1);
     }
     if (MyContrOrder2 >= 2) {
       kk = Indice(Vup, Vfin+2*MyContrOrder2-3);
       gp_XY Pole( Vect(kk), Vect(kk+1));
       H(MyNbVar, FinH+1) +=  (MyLinearForm(1) + 2*Lambda1*MyQuadForm(1)) * Pole;
       H(MyNbVar, FinH+2) =  MyLinearForm(1) * Pole;
     }
       kk = Indice(Vup, Vup); 
       H(H.UpperRow(), H.UpperRow()) =  Vect(kk);
   }      

// recopy the internal block -----------------------------------

   kk = Indice(Vdeb, Vdeb);
   for (ii = DebH; ii <=FinH; ii++) {
     for (jj = DebH; jj<=ii; jj++) {
         H(ii,jj) = Vect(kk);
         kk++;
     }
     kk += Vdeb-1;
  }
// symmetry
   for (ii = H.LowerRow(); ii <= H.UpperRow(); ii++) 
     for (jj = ii+1; jj <= H.UpperRow(); jj++) H(ii,jj) = H(jj,ii);        
}

//=======================================================================
Standard_Boolean FairCurve_Energy::Variable(math_Vector& X) const
//======================================================================= 
{
  Standard_Integer ii,
                   IndexDeb1 = MyPoles->Lower()+1, 
                   IndexDeb2 = X.Lower(),
                   IndexFin1 = MyPoles->Upper()-1,
                   IndexFin2 = X.Upper() - MyWithAuxValue; //  decrease by 1 if the sliding is  
                                                           // free as the last value of X is reserved.
                   

// calculation of variables of constraints  
  if (MyContrOrder1 >= 1) {
     X(IndexDeb2) = MyPoles->Value(MyPoles->Lower())
                   .Distance( MyPoles->Value(MyPoles->Lower()+1) );
     IndexDeb1 += 1;
     IndexDeb2 += 1; 
  }
  if (MyContrOrder1 == 2) {
     gp_Vec2d b1b2( MyPoles->Value(MyPoles->Lower()+1),
		    MyPoles->Value(MyPoles->Lower()+2) );
     X(IndexDeb2) = b1b2.XY() * MyLinearForm(0);                   
     IndexDeb1 += 1;
     IndexDeb2 += 1; 
  }
  if (MyContrOrder2 == 2) {
     gp_Vec2d bn2bn1( MyPoles->Value(MyPoles->Upper()-2),
		      MyPoles->Value(MyPoles->Upper()-1));
     X(IndexFin2) = - bn2bn1.XY() * MyLinearForm(1);                   
     IndexFin1 -= 1;
     IndexFin2 -= 1; 
  }
  if (MyContrOrder2 >= 1) {
     X(IndexFin2) = MyPoles->Value(MyPoles->Upper()) 
                   .Distance(MyPoles->Value(MyPoles->Upper()-1) );
     IndexFin1 -= 1;
  }
 
//  Recopy of auxiliary variables is not done in the abstract class

// copy poles to variables
  for (ii=IndexDeb1; ii<=IndexFin1; ii++) {
     X(IndexDeb2)   =  MyPoles->Value(ii).X();
     X(IndexDeb2+1) =  MyPoles->Value(ii).Y();
     IndexDeb2 +=2;
  }   
  return Standard_True;      
}

//=======================================================================
void FairCurve_Energy::ComputePoles(const math_Vector& X)
//======================================================================= 
{
  Standard_Integer ii,
                   IndexDeb1 = MyPoles->Lower()+1, 
                   IndexDeb2 = X.Lower(),
                   IndexFin1 = MyPoles->Upper()-1,
                   IndexFin2 = X.Upper() - MyWithAuxValue; // decrease by 1 if the sliding is 
                                                           // is free as the last value of X is reserved.
// calculation of pole constraints
// for (ii=MyPoles->Lower();ii<=MyPoles->Upper();ii++) {
// cout << ii << " X = " <<  MyPoles->Value(ii).X() << 
//                " Y = " <<  MyPoles->Value(ii).Y() << endl;}
  
  if (MyContrOrder1 >= 1) {
     IndexDeb1 += 1;
     IndexDeb2 += 1;
     ComputePolesG1( 0, X(1), MyPoles->Value(MyPoles->Lower()), 
                              MyPoles->ChangeValue(MyPoles->Lower()+1) );
  }
  if (MyContrOrder1 == 2) {
     IndexDeb1 += 1;
     IndexDeb2 += 1;
     ComputePolesG2( 0, X(1), X(2), MyPoles->Value(MyPoles->Lower()), 
		                    MyPoles->ChangeValue(MyPoles->Lower()+2) );
  }
  if (MyContrOrder2 == 2) {
     IndexFin1 -= 1;
     IndexFin2 -= 1;
     ComputePolesG2( 1, X(IndexFin2),  X(IndexFin2+1), 
		     MyPoles->Value(MyPoles->Upper()), 
		     MyPoles->ChangeValue(MyPoles->Upper()-2) );
  } 
  if (MyContrOrder2 >= 1) {
     IndexFin1 -= 1;
     ComputePolesG1( 1, X(IndexFin2), MyPoles->Value(MyPoles->Upper()), 
                               MyPoles->ChangeValue(MyPoles->Upper()-1) );
  }

//  if (MyWithAuxValue) { MyLengthSliding = X(X.Upper()); }
// recopy others
  for (ii=IndexDeb1; ii<=IndexFin1; ii++) {
     MyPoles -> ChangeValue(ii).SetX( X(IndexDeb2) );
     MyPoles -> ChangeValue(ii).SetY( X(IndexDeb2+1) );
     IndexDeb2 += 2;
  }    
}

//=======================================================================
void FairCurve_Energy::ComputePolesG1(const Standard_Integer Side, 
				      const Standard_Real Lambda, 
				      const gp_Pnt2d& P1, 
				            gp_Pnt2d& P2) const
//======================================================================= 
{   P2.SetXY ( P1.XY() + MyLinearForm(Side) * Lambda );  }  

//=======================================================================
void FairCurve_Energy::ComputePolesG2(const Standard_Integer Side, 
				      const Standard_Real Lambda, 
				      const Standard_Real Rho,
				      const gp_Pnt2d& P1, 
				            gp_Pnt2d& P2) const
//======================================================================= 
{   P2.SetXY ( P1.XY() 
  + MyLinearForm(Side) * (Lambda + Rho ) 
  + MyQuadForm(Side)   * (Lambda * Lambda) ) ;  }  




