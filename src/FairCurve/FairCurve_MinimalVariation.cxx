// Created on: 1996-02-26
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


#include <BSplCLib.hxx>
#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_EnergyOfMVC.hxx>
#include <FairCurve_MinimalVariation.hxx>
#include <FairCurve_Newton.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NullValue.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//======================================================================================
FairCurve_MinimalVariation::FairCurve_MinimalVariation(const gp_Pnt2d& P1,
						       const gp_Pnt2d& P2, 
						       const Standard_Real Heigth,
						       const Standard_Real Slope,
						       const Standard_Real PhysicalRatio)
//======================================================================================
                                       :FairCurve_Batten(P1, P2, Heigth, Slope),
					OldCurvature1(0), OldCurvature2(0),
                                        OldPhysicalRatio(PhysicalRatio),
					NewCurvature1(0),  NewCurvature2(0),  
					NewPhysicalRatio(PhysicalRatio)
{
}

//======================================================================================
Standard_Boolean FairCurve_MinimalVariation::Compute(FairCurve_AnalysisCode& ACode,
						     const Standard_Integer NbIterations,
						     const Standard_Real Tolerance)
//======================================================================================
{
  Standard_Boolean Ok=Standard_True, End=Standard_False;
  Standard_Real AngleMax = 0.7;      // parameter regulating the function of increment ( 40 degrees )
  Standard_Real AngleMin = 2*M_PI/100; // parameter regulating the function of increment 
                                     // full passage should not contain more than 100 steps.
  Standard_Real DAngle1, DAngle2,  DRho1, DRho2, Ratio, Fraction, Toler;
  Standard_Real OldDist, NewDist;

//  Loop of Homotopy : calculation of the step and optimisation 

  while (Ok && !End) {
     DAngle1 = NewAngle1-OldAngle1;
     DAngle2 = NewAngle2-OldAngle2;
     DRho1 = NewCurvature1 - OldCurvature1;
     DRho2 = NewCurvature2 - OldCurvature2;
     Ratio = 1;

     if (NewConstraintOrder1>0) {
        Fraction = Abs(DAngle1) / (AngleMax * Exp (-Abs(OldAngle1)/AngleMax) + AngleMin);
        if (Fraction > 1) Ratio = 1 / Fraction;
     }
     if (NewConstraintOrder2>0) {
        Fraction = Abs(DAngle2) / (AngleMax * Exp (-Abs(OldAngle2)/AngleMax) + AngleMin);
        if (Fraction > 1)  Ratio = (Ratio < 1 / Fraction ? Ratio : 1 / Fraction);
     }
     
     OldDist = OldP1.Distance(OldP2);
     NewDist = NewP1.Distance(NewP2);
     Fraction = Abs(OldDist-NewDist) / (OldDist/3);
     if ( Fraction > 1) Ratio = (Ratio < 1 / Fraction ? Ratio : 1 / Fraction); 

     if (NewConstraintOrder1>1) {
       Fraction = Abs(DRho1)*OldDist / (2+Abs(OldAngle1) + Abs(OldAngle2));   
       if ( Fraction > 1) Ratio = (Ratio < 1 / Fraction ? Ratio : 1 / Fraction);
     }

     if (NewConstraintOrder2>1) {
       Fraction = Abs(DRho2)*OldDist/ (2+Abs(OldAngle1) + Abs(OldAngle2));   
       if ( Fraction > 1) Ratio = (Ratio < 1 / Fraction ? Ratio : 1 / Fraction); 
     }

     gp_Vec2d DeltaP1(OldP1, NewP1) , DeltaP2(OldP2, NewP2);
     if ( Ratio == 1) {
        End = Standard_True;
        Toler = Tolerance;
      }
     else {
       DeltaP1 *= Ratio;
       DeltaP2 *= Ratio;
       DAngle1 *= Ratio;
       DAngle2 *= Ratio;
       DRho1 *= Ratio;
       DRho2 *= Ratio;
       Toler =  10 * Tolerance;
     }
 
     Ok = Compute( DeltaP1, DeltaP2, 
	           DAngle1, DAngle2,
		   DRho1,   DRho2,           
	           ACode,
                   NbIterations,
                   Toler);

     if (ACode != FairCurve_OK) End = Standard_True;
     if (NewFreeSliding) NewSlidingFactor = OldSlidingFactor;
     if (NewConstraintOrder1 == 0) NewAngle1 = OldAngle1;
     if (NewConstraintOrder1 < 2)  NewCurvature1 = OldCurvature1;
     if (NewConstraintOrder2 == 0) NewAngle2 = OldAngle2; 
     if (NewConstraintOrder2 < 2)  NewCurvature2 = OldCurvature2;
  }
  myCode = ACode; 
  return Ok;
}

//======================================================================================
Standard_Boolean FairCurve_MinimalVariation::Compute(const gp_Vec2d& DeltaP1,
						     const gp_Vec2d& DeltaP2,
						     const Standard_Real DeltaAngle1,
						     const Standard_Real DeltaAngle2,
						     const Standard_Real DeltaCurvature1,
						     const Standard_Real DeltaCurvature2,
						           FairCurve_AnalysisCode& ACode,
						     const Standard_Integer NbIterations,
						     const Standard_Real Tolerance)
//======================================================================================
{
 Standard_Boolean Ok, OkCompute=Standard_True;
 ACode = FairCurve_OK;

// Deformation of the curve by adding a polynom of interpolation
   Standard_Integer L = 2 + NewConstraintOrder1 + NewConstraintOrder2,
                    kk, ii;
//                    NbP1 = Poles->Length()-1, kk, ii;
#ifdef OCCT_DEBUG
   Standard_Integer NbP1 = 
#endif
                           Poles->Length() ;
#ifdef OCCT_DEBUG
   NbP1 = NbP1 - 1 ;
#endif
   TColStd_Array1OfReal knots (1,2);
   knots(1) = 0;
   knots(2) = 1;
   TColStd_Array1OfInteger mults (1,2);
   TColgp_Array1OfPnt2d HermitePoles(1,L);
   TColgp_Array1OfPnt2d Interpolation(1,L);
   Handle(TColgp_HArray1OfPnt2d) NPoles = new  TColgp_HArray1OfPnt2d(1, Poles->Length());

// Polynomes of Hermite
   math_Matrix HermiteCoef(1, L, 1, L);
   Ok = PLib::HermiteCoefficients(0,1, NewConstraintOrder1,  NewConstraintOrder2,
                                  HermiteCoef);
   if (!Ok) return Standard_False;

// Definition of constraints of interpolation
   TColgp_Array1OfXY ADelta(1,L);
   gp_Vec2d VOld(OldP1, OldP2), VNew( -(OldP1.XY()+DeltaP1.XY()) + (OldP2.XY()+DeltaP2.XY()) );
   Standard_Real DAngleRef = VNew.Angle(VOld);
   Standard_Real DAngle1 = DeltaAngle1 - DAngleRef,
                 DAngle2 = DAngleRef   - DeltaAngle2; // Correction of Delta by the Delta induced by the points.


   ADelta(1) = DeltaP1.XY();
   kk = 2;
   if (NewConstraintOrder1>0) {
      // rotation of the derivative premiereDeltaAngle1
      gp_Vec2d OldDerive( Poles->Value(Poles->Lower()), 
                          Poles->Value(Poles->Lower()+1) );
      OldDerive *= Degree / (Knots->Value(Knots->Lower()+1)-Knots->Value(Knots->Lower()) ); 
      ADelta(kk) = (OldDerive.Rotated(DAngle1) -  OldDerive).XY();
      kk += 1;
   
      if (NewConstraintOrder1>1) {
	 // rotation of the second derivative + adding 
         gp_Vec2d OldSeconde( Poles->Value(Poles->Lower()).XY() + Poles->Value(Poles->Lower()+2).XY()  
                            - 2*Poles->Value(Poles->Lower()+1).XY() );
         OldSeconde *=  Degree*( Degree-1)
	             /  pow (Knots->Value(Knots->Lower()+1)-Knots->Value(Knots->Lower()), 2);
         Standard_Real CPrim = OldDerive.Magnitude();
         ADelta(kk) = ( OldSeconde.Rotated(DAngle1) -  OldSeconde 
		      + DeltaCurvature1*CPrim*OldDerive.Rotated(M_PI/2+DAngle1) ).XY();
         kk += 1;
      }
   }
   ADelta(kk) = DeltaP2.XY();
   kk += 1;  
   if (NewConstraintOrder2>0) {
      gp_Vec2d OldDerive( Poles->Value(Poles->Upper()-1), 
                          Poles->Value(Poles->Upper()) );
      OldDerive *= Degree / (Knots->Value(Knots->Upper()) - Knots->Value(Knots->Upper()-1) );
      ADelta(kk) = (OldDerive.Rotated(DAngle2) -  OldDerive).XY();
      kk += 1;
      if (NewConstraintOrder2>1) {
	 // rotation of the second derivative + adding 
         gp_Vec2d OldSeconde( Poles->Value(Poles->Upper()).XY() + Poles->Value(Poles->Upper()-2).XY()  
                            - 2*Poles->Value(Poles->Upper()-1).XY() );
         OldSeconde *=  Degree*( Degree-1)
	             /  pow (Knots->Value(Knots->Upper())-Knots->Value(Knots->Upper()-1), 2);
         Standard_Real CPrim = OldDerive.Magnitude();
         ADelta(kk) = ( OldSeconde.Rotated(DAngle2) -  OldSeconde 
		      + DeltaCurvature2*CPrim*OldDerive.Rotated(M_PI/2+DAngle2) ).XY();
         kk += 1;
      }
   }

// Interpolation
  gp_XY AuxXY (0,0);
  for (ii=1; ii<=L; ii++) {
      AuxXY.SetCoord(0.0, 0);
      for (kk=1; kk<=L; kk++) {
          AuxXY +=  HermiteCoef(kk, ii) * ADelta(kk);       
      }
      Interpolation(ii).SetXY(AuxXY);
  }
// Conversion into BSpline of the same structure as the current batten.
  PLib::CoefficientsPoles(Interpolation,  PLib::NoWeights(), 
                          HermitePoles,  PLib::NoWeights()); 

  mults.Init(L);

  Handle(Geom2d_BSplineCurve) DeltaCurve = 
    new  Geom2d_BSplineCurve( HermitePoles, 
                              knots, mults, L-1);

  DeltaCurve->IncreaseDegree(Degree);
  if (Mults->Length()>2) {
     DeltaCurve->InsertKnots(Knots->Array1(), Mults->Array1(), 1.e-10);
  }

// Summing
  DeltaCurve->Poles( NPoles->ChangeArray1() );
  for (kk= NPoles->Lower(); kk<=NPoles->Upper(); kk++) { 
     NPoles->ChangeValue(kk).ChangeCoord() += Poles->Value(kk).Coord(); 
   }

// Intermediaires

 Standard_Real Angle1, Angle2, SlidingLength, 
               Alph1 =  OldAngle1 + DeltaAngle1, 
               Alph2 =  OldAngle2 + DeltaAngle2,
               Rho1 =   OldCurvature1 + DeltaCurvature1,
               Rho2 =   OldCurvature2 + DeltaCurvature2,
               Dist  =  NPoles->Value(NPoles->Upper()) 
                      . Distance( NPoles->Value( NPoles->Lower() ) ),
	       LReference = SlidingOfReference(Dist, Alph1, Alph2);
 gp_Vec2d Ox(1, 0),
                P1P2 (  NPoles->Value(NPoles->Upper()).Coord()
                      - NPoles->Value(NPoles->Lower()).Coord() );

// Angles corresponding to axis ox

 Angle1 =  Ox.Angle(P1P2) + Alph1;
 Angle2 = -Ox.Angle(P1P2) + Alph2;

// Calculation of the length of sliding (imposed or intial);
 
 if (!NewFreeSliding) {
    SlidingLength = NewSlidingFactor * LReference;
  }
 else {
   if (OldFreeSliding) {
     SlidingLength = OldSlidingFactor *  LReference;
   }
   else {
     SlidingLength = SlidingOfReference(Dist, Alph1, Alph2);
   }
 }


     
// Energy and vectors of initialization
 FairCurve_BattenLaw LBatten (NewHeight, NewSlope, SlidingLength ); 
 FairCurve_EnergyOfMVC EMVC (Degree+1, Flatknots, NPoles,  
			     NewConstraintOrder1,  NewConstraintOrder2, 
			     LBatten, NewPhysicalRatio, SlidingLength, NewFreeSliding,
                             Angle1, Angle2, Rho1, Rho2);
 math_Vector VInit (1, EMVC.NbVariables());

 // The value below gives an idea about the smallest value of the criterion of flexion.
 Standard_Real VConvex = 0.01 * pow(NewHeight / SlidingLength, 3);
 if (VConvex < 1.e-12) {VConvex = 1.e-12;}

 Ok = EMVC.Variable(VInit);
 
// Minimisation
 FairCurve_Newton Newton(EMVC,
			 Tolerance*(P1P2.Magnitude()/10),
			 Tolerance,
			 NbIterations,
			 VConvex);
 Newton.Perform(EMVC, VInit);
 Ok = Newton.IsDone();
 
 if (Ok) {
    gp_Vec2d Tangente, PseudoNormale;
    Poles = NPoles;
    Newton.Location(VInit);

    if (NewFreeSliding) { OldSlidingFactor = VInit(VInit.Upper()) / LReference;}
    else                { OldSlidingFactor = NewSlidingFactor; }

    if (NewConstraintOrder1 < 2) {
       Tangente.SetXY(  Poles->Value(Poles->Lower()+1).XY()
                      - Poles->Value(Poles->Lower()).XY() );

       if (NewConstraintOrder1 == 0) {OldAngle1 = P1P2.Angle(Tangente);}
       else {OldAngle1 = Alph1;}

       PseudoNormale.SetXY ( Poles->Value(Poles->Lower()).XY()
                            - 2 * Poles->Value(Poles->Lower()+1).XY()
		            + Poles->Value(Poles->Lower()+2).XY());
       OldCurvature1 = (((double)Degree-1) /Degree) * (Tangente.Normalized()^PseudoNormale)
	               / Tangente.SquareMagnitude();
     }
    else {OldCurvature1 = Rho1;
	  OldAngle1 = Alph1; }

    if (NewConstraintOrder2 < 2) {
       Tangente.SetXY ( Poles->Value(Poles->Upper()).XY()
                      - Poles->Value(Poles->Upper()-1).XY() );
       if (NewConstraintOrder2 == 0) OldAngle2 = (-Tangente).Angle(-P1P2);
       else { OldAngle2 = Alph2;}
       PseudoNormale.SetXY ( Poles->Value(Poles->Upper()).XY()
                            - 2 * Poles->Value(Poles->Upper()-1).XY()
		            + Poles->Value(Poles->Upper()-2).XY());
       OldCurvature2 = (((double)Degree-1) /Degree) * (Tangente.Normalized()^PseudoNormale)
	             / Tangente.SquareMagnitude();
     }
    else { OldAngle2 = Alph2;
	   OldCurvature2 = Rho2;
    }

    OldP1 = Poles->Value(Poles->Lower());
    OldP2 = Poles->Value(Poles->Upper());
    OldConstraintOrder1 = NewConstraintOrder1;
    OldConstraintOrder2 = NewConstraintOrder2;
    OldFreeSliding      = NewFreeSliding;
    OldSlope = NewSlope;
    OldHeight = NewHeight;
    OldPhysicalRatio =  NewPhysicalRatio;
  }
  else {
    Standard_Real V;
    ACode = EMVC.Status();
    if (!LBatten.Value(0, V) || !LBatten.Value(1, V)) {
       ACode = FairCurve_NullHeight;
    }
    else { OkCompute = Standard_False;}
    return OkCompute;
  }

 Ok = EMVC.Variable(VInit);

 // Processing of non convergence
 if (!Newton.IsConverged()) {
    ACode = FairCurve_NotConverged;
  }


 // Prevention of infinite sliding
 if (NewFreeSliding &&  VInit(VInit.Upper()) > 2*LReference) ACode = FairCurve_InfiniteSliding;  
    

// Eventual insertion of Nodes
 Standard_Boolean  NewKnots = Standard_False;
 Standard_Integer NbKnots = Knots->Length();
 Standard_Real ValAngles = (Abs(OldAngle1) +  Abs(OldAngle2) 
                         + 2 * Abs(OldAngle2 - OldAngle1) ) ;
 while ( ValAngles > (2*(NbKnots-2) + 1)*(1+2*NbKnots) ) {
   NewKnots = Standard_True;
   NbKnots += NbKnots-1;
 }

 if  (NewKnots) {  
   Handle(Geom2d_BSplineCurve) NewBS = 
    new  Geom2d_BSplineCurve( NPoles->Array1(), Knots->Array1(), 
			      Mults->Array1(), Degree);

   Handle(TColStd_HArray1OfInteger) NMults  =
      new TColStd_HArray1OfInteger (1,NbKnots);
   NMults->Init(Degree-3);

    Handle(TColStd_HArray1OfReal) NKnots  =
      new TColStd_HArray1OfReal (1,NbKnots);
   for (ii=1; ii<=NbKnots; ii++) {
       NKnots->ChangeValue(ii) = (double) (ii-1) / (NbKnots-1);
   } 

   NewBS -> InsertKnots(NKnots->Array1(), NMults->Array1(), 1.e-10);
   Handle(TColgp_HArray1OfPnt2d) NewNPoles =
      new  TColgp_HArray1OfPnt2d(1, NewBS->NbPoles());
   NewBS -> Poles(NewNPoles->ChangeArray1() );
   NewBS -> Multiplicities( NMults->ChangeArray1() );
   NewBS -> Knots( NKnots->ChangeArray1() );
   Handle(TColStd_HArray1OfReal) FKnots  =
      new TColStd_HArray1OfReal (1, NewBS->NbPoles() + Degree+1);
   NewBS -> KnotSequence( FKnots->ChangeArray1()); 

   Poles = NewNPoles;
   Mults = NMults;
   Knots = NKnots;
   Flatknots = FKnots;		      
 } 


// For eventual debug Newton.Dump(cout);
   
 return OkCompute;
} 


//======================================================================================
void FairCurve_MinimalVariation::Dump(Standard_OStream& o) const 
//======================================================================================
{

o << "  MVCurve      |"; o.width(7); o<< "Old  |   New" << endl;
o << "  P1    X      |"; o.width(7); o<<  OldP1.X() << " | " << NewP1.X() << endl;
o << "        Y      |"; o.width(7); o<<  OldP1.Y() << " | " << NewP1.Y() << endl;
o << "  P2    X      |"; o.width(7); o<<  OldP2.X() << " | " << NewP2.X() << endl;
o << "        Y      |"; o.width(7); o<<  OldP2.Y() << " | " << NewP2.Y() << endl;
o << "      Angle1   |"; o.width(7); o<<  OldAngle1 << " | " << NewAngle1 << endl;
o << "      Angle2   |"; o.width(7); o<<  OldAngle2 << " | " << NewAngle2 << endl;
o << " Curvature1    |"; o.width(7); o<<  OldCurvature1 << " | " << NewCurvature1 << endl;
o << " Curvature2    |"; o.width(7); o<<  OldCurvature2 << " | " << NewCurvature2 << endl;
o << "      Height   |"; o.width(7); o<<  OldHeight << " | " << NewHeight << endl;
o << "      Slope    |"; o.width(7); o<<  OldSlope  << " | " << NewSlope << endl; 
o << " PhysicalRatio |"; o.width(7); o<<  OldPhysicalRatio << " | " << NewPhysicalRatio << endl;
o << " SlidingFactor |"; o.width(7); o<<  OldSlidingFactor << " | " << NewSlidingFactor << endl;
o << " FreeSliding   |"; o.width(7); o<<  OldFreeSliding << " | " << NewFreeSliding << endl; 
o << " ConstrOrder1  |"; o.width(7); o<<  OldConstraintOrder1 << " | " << NewConstraintOrder1 << endl; 
o << " ConstrOrder2  |"; o.width(7); o<<  OldConstraintOrder2 << " | " << NewConstraintOrder2 << endl;
 switch (myCode) {
   case  FairCurve_OK : 
     o << "AnalysisCode : Ok" << endl;
     break;
   case  FairCurve_NotConverged : 
     o << "AnalysisCode : NotConverged" << endl;
     break;
   case  FairCurve_InfiniteSliding : 
     o << "AnalysisCode : InfiniteSliding" << endl;
     break;
   case  FairCurve_NullHeight : 
     o << "AnalysisCode : NullHeight" << endl;
     break;
     }   
}










