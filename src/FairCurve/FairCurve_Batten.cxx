// Created on: 1996-02-05
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
#include <FairCurve_Batten.hxx>
#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_EnergyOfBatten.hxx>
#include <FairCurve_Newton.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NullValue.hxx>

// ==================================================================
FairCurve_Batten::FairCurve_Batten(const gp_Pnt2d& P1, 
                                   const gp_Pnt2d& P2, 
                                   const Standard_Real Height, 
                                   const Standard_Real Slope)
// ==================================================================
                  : myCode(FairCurve_OK),
                    OldP1(P1),
                    OldP2(P2),
                    OldAngle1(0),
		    OldAngle2(0),
		    OldHeight(Height),
                    OldSlope(Slope),
		    OldSlidingFactor(1),
                    OldFreeSliding(0),
                    OldConstraintOrder1(1),
                    OldConstraintOrder2(1),
                    NewP1(P1),
                    NewP2(P2),
                    NewAngle1(0),
                    NewAngle2(0),
                    NewHeight(Height),
                    NewSlope(Slope),
                    NewSlidingFactor(1),
                    NewFreeSliding(0),
                    NewConstraintOrder1(1),
                    NewConstraintOrder2(1),
                    Degree(9)
{
  if (P1.IsEqual(P2, Precision::Confusion())) 
    throw Standard_NullValue("FairCurve : P1 and P2 are confused");
  if (Height <= 0) 
    throw Standard_NegativeValue("FairCurve : Height is not positive");
//
//   Initialize by a straight line (2 poles)
//
  Handle(TColStd_HArray1OfReal)    Iknots =  new TColStd_HArray1OfReal(1,2);
  Handle(TColStd_HArray1OfInteger) Imults =  new TColStd_HArray1OfInteger(1,2);
  Handle(TColgp_HArray1OfPnt2d)    Ipoles = new TColgp_HArray1OfPnt2d(1,2);

  Iknots->SetValue(1, 0);
  Iknots->SetValue(2, 1);
  
  Imults->SetValue(1, 2);
  Imults->SetValue(2, 2);

  Ipoles->SetValue(1, P1);
  Ipoles->SetValue(2, P2);

//  Increase the degree
  
  Handle(TColgp_HArray1OfPnt2d) Npoles = new  TColgp_HArray1OfPnt2d(1, Degree+1);
  Handle(TColStd_HArray1OfReal) Nweight = new TColStd_HArray1OfReal(1, 2);
  Handle(TColStd_HArray1OfReal) Nknots = new TColStd_HArray1OfReal(1, 2);
  Handle(TColStd_HArray1OfInteger) Nmults = new TColStd_HArray1OfInteger(1, 2);
    
  BSplCLib::IncreaseDegree (1, Degree, Standard_False,
                            Ipoles->Array1(),
			    BSplCLib::NoWeights(),
			    Iknots->Array1(), 
			    Imults->Array1(), 
			    Npoles->ChangeArray1(),
			    &Nweight->ChangeArray1(),
			    Nknots->ChangeArray1(),
			    Nmults->ChangeArray1() );

   // and impact the result in our fields

  Poles = Npoles;
  Knots = Nknots;
  Mults = Nmults;

   // calculate "plane" nodes

  Flatknots = new TColStd_HArray1OfReal
    (1, BSplCLib::KnotSequenceLength(Mults->Array1(), Degree, Standard_False));

  BSplCLib::KnotSequence (Knots->Array1(), 
			  Mults->Array1(),
			  Degree, Standard_False,
			  Flatknots->ChangeArray1());     
}
// ==================================================================
FairCurve_Batten::~FairCurve_Batten()
{}
// ==================================================================
void FairCurve_Batten::Angles(const gp_Pnt2d& P1, 
                              const gp_Pnt2d& P2)
// ==================================================================
{
  gp_Vec2d VOld(NewP1, NewP2), VNew(P1, P2);
  Standard_Real Dangle = VOld.Angle(VNew);
  NewAngle1 -= Dangle;
  NewAngle2 += Dangle; 
}

// ==================================================================
void FairCurve_Batten::SetP1(const gp_Pnt2d& P1)
// ==================================================================
{
  if (P1.IsEqual(NewP2, Precision::Confusion())) 
    throw Standard_NullValue("FairCurve : P1 and P2 are confused");
  Angles(P1, NewP2);
  NewP1 = P1;
}

// ==================================================================
void FairCurve_Batten::SetP2(const gp_Pnt2d& P2)
// ==================================================================
{
 if (NewP1.IsEqual(P2, Precision::Confusion())) 
    throw Standard_NullValue("FairCurve : P1 and P2 are confused");
  Angles(NewP1, P2);
  NewP2 = P2;
}

// ==================================================================
Standard_Boolean FairCurve_Batten::Compute(FairCurve_AnalysisCode& ACode, 
                                           const Standard_Integer NbIterations, 
                                           const Standard_Real Tolerance)
// ==================================================================
{
  Standard_Boolean Ok=Standard_True, End=Standard_False;
  Standard_Real AngleMax = 0.7;      // parameter ruling the function of increment ( 40 degrees )
  Standard_Real AngleMin = 2*M_PI/100; // parameter ruling the function of increment 
                                     // full passage should not cost more than 100 steps.
  Standard_Real DAngle1, DAngle2,  Ratio, Fraction, Toler;
  Standard_Real OldDist, NewDist;

//  Loop of Homotopy : calculation of the step and optimisation 

  while (Ok && !End) {
     DAngle1 = NewAngle1-OldAngle1;
     DAngle2 = NewAngle2-OldAngle2;
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
       Toler =  10 * Tolerance;
     }
 
     Ok = Compute( DeltaP1, DeltaP2, 
	           DAngle1, DAngle2,
	           ACode,
                   NbIterations,
                   Toler);

     if (ACode != FairCurve_OK) End = Standard_True;
     if (NewFreeSliding) NewSlidingFactor = OldSlidingFactor;
     if (NewConstraintOrder1 == 0) NewAngle1 = OldAngle1;
     if (NewConstraintOrder2 == 0) NewAngle2 = OldAngle2; 
  }
  myCode = ACode;  
  return Ok;
}
// =============================================================================
Standard_Boolean FairCurve_Batten::Compute(const gp_Vec2d& DeltaP1, 
					   const gp_Vec2d& DeltaP2, 
					   const Standard_Real DeltaAngle1, 
					   const Standard_Real DeltaAngle2,
					   FairCurve_AnalysisCode& ACode, 
					   const Standard_Integer NbIterations, 
					   const Standard_Real Tolerance)
// =============================================================================
{
 Standard_Boolean Ok, OkCompute=Standard_True;
 ACode = FairCurve_OK;

// Deformation of the curve by adding a polynom of interpolation
   Standard_Integer L = 2 + NewConstraintOrder1 + NewConstraintOrder2, kk, ii;
   TColStd_Array1OfReal knots (1,2);
   knots(1) = 0;
   knots(2) = 1;
   TColStd_Array1OfInteger mults (1,2);
   TColgp_Array1OfPnt2d HermitePoles(1,L);
   TColgp_Array1OfPnt2d Interpolation(1,L);
   Handle(TColgp_HArray1OfPnt2d) NPoles = new  TColgp_HArray1OfPnt2d(1, Poles->Length());

// Polynoms of Hermite
   math_Matrix HermiteCoef(1, L, 1, L);
   Ok = PLib::HermiteCoefficients(0,1, NewConstraintOrder1,  NewConstraintOrder2,
                                  HermiteCoef);
   if (!Ok) return Standard_False;

// Definition of constraints of interpolation
   TColgp_Array1OfXY ADelta(1,L);
   gp_Vec2d VOld(OldP1, OldP2), VNew( -(OldP1.XY()+DeltaP1.XY()) + (OldP2.XY()+DeltaP2.XY()) );
   Standard_Real DAngleRef = VNew.Angle(VOld);

   ADelta(1) = DeltaP1.XY();
   kk = 2;
   if (NewConstraintOrder1>0) {
      gp_Vec2d OldDerive( Poles->Value(Poles->Lower()), 
                          Poles->Value(Poles->Lower()+1) );
      OldDerive *= Degree / (Knots->Value(2) - Knots->Value(1));
      ADelta(kk) = (OldDerive.Rotated(DeltaAngle1-DAngleRef) -  OldDerive).XY();
      kk += 1;
   }
   ADelta(kk) = DeltaP2.XY();
   kk += 1;  
   if (NewConstraintOrder2>0) {
      gp_Vec2d OldDerive( Poles->Value(Poles->Upper()-1), 
                          Poles->Value(Poles->Upper()) );
      OldDerive *= Degree / (Knots->Value(Knots->Upper())  - Knots->Value(Knots->Upper()-1) );
      ADelta(kk) = (OldDerive.Rotated(DAngleRef-DeltaAngle2) -  OldDerive).XY();
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
  PLib::CoefficientsPoles( Interpolation, PLib::NoWeights(), 
                           HermitePoles,  PLib::NoWeights() ); 

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

// Intermediary data

 Standard_Real Angle1, Angle2, SlidingLength, 
               Alph1 =  OldAngle1 + DeltaAngle1, 
               Alph2 =  OldAngle2 + DeltaAngle2,
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


     
// Energy and vectors of initialisation
 FairCurve_BattenLaw LBatten (NewHeight, NewSlope, SlidingLength ); 
 FairCurve_EnergyOfBatten EBatten (Degree+1, Flatknots, NPoles,  
				   NewConstraintOrder1,  NewConstraintOrder2, 
				   LBatten, SlidingLength, NewFreeSliding,
                                   Angle1, Angle2);
 math_Vector VInit (1, EBatten.NbVariables());

 // The valeur below is the smallest value of the criterion of flexion.
 Standard_Real VConvex = 0.01 * pow(NewHeight / SlidingLength, 3);
 if (VConvex < 1.e-12) {VConvex = 1.e-12;}

 Ok = EBatten.Variable(VInit);
 
// Minimisation
 FairCurve_Newton Newton(EBatten,
			 Tolerance*P1P2.Magnitude()/10,
			 Tolerance,
			 NbIterations,
			 VConvex);
 Newton.Perform(EBatten, VInit);
 Ok = Newton.IsDone();
 
 if (Ok) {
    Poles = NPoles;
    Newton.Location(VInit);
    if (NewFreeSliding) { OldSlidingFactor = VInit(VInit.Upper()) / LReference;}
    else                { OldSlidingFactor = NewSlidingFactor; }

    if (NewConstraintOrder1 == 0) {
       gp_Vec2d Tangente (  Poles->Value(Poles->Lower()+1).Coord()
                                   - Poles->Value(Poles->Lower()).Coord() );
       OldAngle1 = P1P2.Angle(Tangente);
     }
    else {OldAngle1 = Alph1;}

    if (NewConstraintOrder2 == 0) {
       gp_Vec2d Tangente (  Poles->Value(Poles->Upper()).Coord()
                                   - Poles->Value(Poles->Upper()-1).Coord() );
       OldAngle2 = (-Tangente).Angle(-P1P2);
     } 
    else {OldAngle2 = Alph2;}

    OldP1 = Poles->Value(Poles->Lower());
    OldP2 = Poles->Value(Poles->Upper());
    OldConstraintOrder1 = NewConstraintOrder1;
    OldConstraintOrder2 = NewConstraintOrder2;
    OldFreeSliding      = NewFreeSliding;
    OldSlope = NewSlope;
    OldHeight = NewHeight;
  }
  else {
    Standard_Real V;
    ACode = EBatten.Status();
    if (!LBatten.Value(0, V) || !LBatten.Value(1, V)) {
       ACode = FairCurve_NullHeight;
    }
    else { OkCompute = Standard_False;}
    return OkCompute;
  }

 Ok = EBatten.Variable(VInit);

 // Processing of non-convergence
 if (!Newton.IsConverged()) {
    ACode = FairCurve_NotConverged;
  }


 // Prevention of infinite sliding
 if (NewFreeSliding &&  VInit(VInit.Upper()) > 2*LReference) 
   ACode = FairCurve_InfiniteSliding;  
    

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

// For eventual debug
//  Newton.Dump(cout);
   
 return OkCompute;
} 


// ==================================================================
Standard_Real FairCurve_Batten::SlidingOfReference() const
// ================================================================== 
{
 return SlidingOfReference(NewP1.Distance(NewP2), NewAngle1, NewAngle2 );
}
// ==================================================================
Standard_Real FairCurve_Batten::SlidingOfReference(const Standard_Real Dist,
						   const Standard_Real Angle1,
						   const Standard_Real Angle2) const
// ==================================================================
{
 Standard_Real a1, a2;

// case of angle without constraints
 if ( (NewConstraintOrder1 == 0) && (NewConstraintOrder2 == 0)) return Dist;

 if (NewConstraintOrder1 == 0) a1 = Abs( Abs(NewAngle2)<M_PI ? Angle2/2 : M_PI/2);
 else a1 = Abs(Angle1);

 if (NewConstraintOrder2 == 0) a2 = Abs( Abs(NewAngle1)<M_PI ? Angle1/2 : M_PI/2);
 else a2 = Abs(Angle2);

// case of angle of the same sign 
 if (Angle1 * Angle2 >= 0 ) {
     return Compute(Dist, a1, a2);
   }

// case of angle of opposite sign
 else {
    Standard_Real Ratio = a1 / ( a1 + a2 );
    Standard_Real AngleMilieu = pow(1-Ratio,2) * a1 + pow(Ratio,2) * a2;
    if (AngleMilieu > M_PI/2) AngleMilieu = M_PI/2;

    return   Ratio     * Compute(Dist, a1,  AngleMilieu )
           + (1-Ratio) * Compute(Dist, a2,  AngleMilieu );
  }
}


// ==================================================================
Standard_Real FairCurve_Batten::Compute(const Standard_Real Dist,
					const Standard_Real Angle1,
					const Standard_Real Angle2) const
// ==================================================================
{
    Standard_Real L1 = Compute(Dist, Angle1);
    Standard_Real L2 = Compute(Dist, Angle2), Aux;
    if (L1 < L2) { 
      Aux = L2;
      L2 = L1;
      L1 = Aux;
    }
    return 0.3 * L1 + 0.7 * L2;
}

// ==================================================================
Standard_Real FairCurve_Batten::Compute(const Standard_Real Dist,
					const Standard_Real Angle) const
// ==================================================================
{
  if (Angle < Precision::Angular() ) { return Dist; } // length of segment P1P2
  if (Angle < M_PI/2) { return Angle*Dist / sin(Angle); } // length of circle P1P2 respecting ANGLE
  if (Angle > M_PI) { return Sqrt(Angle*M_PI) * Dist;}
  else  { return Angle * Dist; }                      // linear interpolation
}

// ==================================================================
Handle(Geom2d_BSplineCurve) FairCurve_Batten::Curve() const
// ================================================================== 
{
   Handle(Geom2d_BSplineCurve) TheCurve = new 
                        Geom2d_BSplineCurve ( Poles->Array1(),
                                              Knots->Array1(),
					      Mults->Array1(),
                                              Degree);
   return TheCurve; 
}

// ==================================================================
void FairCurve_Batten::Dump(Standard_OStream& o) const 
// ==================================================================
{

o << "  Batten       |"; o.width(7); o<< "Old  |   New" << endl;
o << "  P1    X      |"; o.width(7); o<<  OldP1.X() << " | " << NewP1.X() << endl;
o << "        Y      |"; o.width(7); o<<  OldP1.Y() << " | " << NewP1.Y() << endl;
o << "  P2    X      |"; o.width(7); o<<  OldP2.X() << " | " << NewP2.X() << endl;
o << "        Y      |"; o.width(7); o<<  OldP2.Y() << " | " << NewP2.Y() << endl;
o << "      Angle1   |"; o.width(7); o<<  OldAngle1 << " | " << NewAngle1 << endl;
o << "      Angle2   |"; o.width(7); o<<  OldAngle2 << " | " << NewAngle2 << endl;
o << "      Height   |"; o.width(7); o<<  OldHeight << " | " << NewHeight << endl;
o << "      Slope    |"; o.width(7); o<<  OldSlope  << " | " << NewSlope << endl; 
o << " SlidingFactor |"; o.width(7); o<<  OldSlidingFactor << " | " << NewSlidingFactor << endl;
o << " FreeSliding   |"; o.width(7); o<<  OldFreeSliding << " | " << NewFreeSliding << endl; 
o << " ConstrOrder1  |"; o.width(7); o<<  OldConstraintOrder1 << " | " << NewConstraintOrder1 << endl; 
o << " ConstrOrder2  |" ; o.width(7); o<<  OldConstraintOrder2 << " | " << NewConstraintOrder2 << endl;
 switch (myCode) {
   case FairCurve_OK : 
     o  << "AnalysisCode : Ok" << endl;
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

