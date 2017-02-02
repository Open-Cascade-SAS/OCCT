// Created on: 1997-06-25
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


#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_Cutting.hxx>
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_SweepApproximation.hxx>
#include <Approx_SweepFunction.hxx>
#include <BSplCLib.hxx>
#include <gp_XYZ.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//class : Approx_SweepApproximation_Eval
//purpose: evaluator class for approximation
//=======================================================================
class Approx_SweepApproximation_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  Approx_SweepApproximation_Eval (Approx_SweepApproximation& theTool)
    : Tool(theTool) {}
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  Approx_SweepApproximation &Tool;
};

void Approx_SweepApproximation_Eval::Evaluate (Standard_Integer *,/*Dimension*/
                                               Standard_Real     StartEnd[2],
                                               Standard_Real    *Parameter,
                                               Standard_Integer *DerivativeRequest,
                                               Standard_Real    *Result,// [Dimension]
                                               Standard_Integer *ErrorCode)
{
  *ErrorCode = Tool.Eval (*Parameter, *DerivativeRequest, 
                          StartEnd[0], StartEnd[1], Result[0]);
}

Approx_SweepApproximation::
Approx_SweepApproximation(const Handle(Approx_SweepFunction)& Func) 
{
  myFunc  = Func;
  //  Init of variables of control
  myParam = 0;
  myOrder = -1;
  first = 1.e100; last = -1.e100;
  done = Standard_False;
}

void Approx_SweepApproximation::Perform(const Standard_Real First,
					const Standard_Real Last,
					const Standard_Real Tol3d,
					const Standard_Real BoundTol,
					const Standard_Real Tol2d,
					const Standard_Real TolAngular,
					const GeomAbs_Shape Continuity,
					const Standard_Integer Degmax,
					const Standard_Integer Segmax)
{
 Standard_Integer NbPolSect, NbKnotSect, ii;
 Standard_Real Tol, Tol3dMin = Tol3d, The3D2DTol=0 ; 
 GeomAbs_Shape   continuity = Continuity;

// (1) Characteristics of a section
 myFunc->SectionShape(NbPolSect, NbKnotSect, udeg);
 Num2DSS = myFunc->Nb2dCurves();
 tabUKnots = new (TColStd_HArray1OfReal)    (1, NbKnotSect);
 tabUMults = new (TColStd_HArray1OfInteger) (1, NbKnotSect);
 myFunc->Knots(tabUKnots->ChangeArray1());
 myFunc->Mults(tabUMults->ChangeArray1());

// (2) Decompositition into sub-spaces
 Handle(TColStd_HArray1OfReal) OneDTol, TwoDTol, ThreeDTol;
 Num3DSS = NbPolSect;

// (2.1) Tolerance 3d and 1d
 OneDTol = new (TColStd_HArray1OfReal) (1, Num3DSS);
 ThreeDTol = new (TColStd_HArray1OfReal) (1, Num3DSS);

 myFunc->GetTolerance(BoundTol, Tol3d, TolAngular, 
		      ThreeDTol->ChangeArray1());

 for (ii=1; ii<=Num3DSS; ii++) 
   if (ThreeDTol->Value(ii) < Tol3dMin) Tol3dMin = ThreeDTol->Value(ii);

 if (myFunc->IsRational()) {
   Standard_Real Size;
   Num1DSS = NbPolSect;
   TColStd_Array1OfReal Wmin(1, Num1DSS);
   myFunc->GetMinimalWeight(Wmin);
   Size =  myFunc->MaximalSection();
   Translation.SetXYZ
     (myFunc->BarycentreOfSurf().XYZ());
   for (ii=1; ii<=Num3DSS; ii++) {
     Tol =  ThreeDTol->Value(ii)/2; // To take accout of the error on the final result.
     OneDTol->SetValue(ii, Tol * Wmin(ii) / Size);
     Tol *= Wmin(ii); //Factor of projection
     ThreeDTol->SetValue(ii, Max(Tol, 1.e-20) );
   }
 }
 else { Num1DSS = 0; }

 
// (2.2) Tolerance and Transformation 2d.
 if (Num2DSS == 0) {TwoDTol.Nullify();}
 else {
   // for 2d define affinity using resolutions, to 
   // avoid homogenuous tolerance of approximation (u/v and 2d/3d)
   Standard_Real res, tolu, tolv; 
   TwoDTol = new (TColStd_HArray1OfReal) (1, Num2DSS);
   AAffin = new (Approx_HArray1OfGTrsf2d) (1, Num2DSS);
   The3D2DTol= 0.9*BoundTol; // 10% of security
   for (ii=1; ii<=Num2DSS; ii++) {
     myFunc->Resolution(ii, The3D2DTol, tolu, tolv);   
     if ( tolu> tolv ) {
       res = tolv;
       AAffin->ChangeValue(ii).SetValue(1,1, tolv/tolu); 
     }
     else {
       res = tolu;
       AAffin->ChangeValue(ii).SetValue(2,2, tolu/tolv);
     }
     TwoDTol->SetValue(ii, Min( Tol2d, res));
   }
 }

// (3) Approximation

// Init
 myPoles = new (TColgp_HArray1OfPnt)(1, Num3DSS);
 myDPoles = new (TColgp_HArray1OfVec)(1, Num3DSS);
 myD2Poles = new (TColgp_HArray1OfVec)(1, Num3DSS);

 myWeigths = new (TColStd_HArray1OfReal)(1, Num3DSS);
 myDWeigths = new (TColStd_HArray1OfReal)(1, Num3DSS);
 myD2Weigths = new (TColStd_HArray1OfReal)(1, Num3DSS);
 
 if (Num2DSS>0) {
   myPoles2d   = new (TColgp_HArray1OfPnt2d)(1, Num2DSS);
   myDPoles2d  = new (TColgp_HArray1OfVec2d)(1, Num2DSS);
   myD2Poles2d = new (TColgp_HArray1OfVec2d)(1, Num2DSS);  
   COnSurfErr  = new (TColStd_HArray1OfReal)(1, Num2DSS);
 }
// Checks if myFunc->D2 is implemented
 if (continuity >= GeomAbs_C2) {
   Standard_Boolean B;
   B = myFunc->D2(First, First, Last, 
		  myPoles->ChangeArray1(), myDPoles->ChangeArray1(), 
		  myD2Poles->ChangeArray1(),
		  myPoles2d->ChangeArray1(), myDPoles2d->ChangeArray1(), 
		  myD2Poles2d->ChangeArray1(),
		  myWeigths->ChangeArray1(), myDWeigths->ChangeArray1(),
		  myD2Weigths->ChangeArray1());
   if (!B) continuity =  GeomAbs_C1;
 }
// Checks if myFunc->D1 is implemented
 if (continuity == GeomAbs_C1) {
   Standard_Boolean B;
   B = myFunc->D1(First, First, Last, 
		  myPoles->ChangeArray1(), myDPoles->ChangeArray1(), 
		  myPoles2d->ChangeArray1(), myDPoles2d->ChangeArray1(), 
		  myWeigths->ChangeArray1(), myDWeigths->ChangeArray1());
   if (!B) continuity =  GeomAbs_C0;
 }

// So that F was at least 20 times more exact than its approx
 myFunc->SetTolerance(Tol3dMin/20, Tol2d/20);
 
 Standard_Integer NbIntervalC2 = myFunc->NbIntervals(GeomAbs_C2);
 Standard_Integer NbIntervalC3 = myFunc->NbIntervals(GeomAbs_C3);

 if (NbIntervalC3 > 1) {
// (3.1) Approximation with preferential cut
   TColStd_Array1OfReal Param_de_decoupeC2 (1, NbIntervalC2+1);
   myFunc->Intervals(Param_de_decoupeC2, GeomAbs_C2);
   TColStd_Array1OfReal Param_de_decoupeC3 (1, NbIntervalC3+1);
   myFunc->Intervals(Param_de_decoupeC3, GeomAbs_C3);


   AdvApprox_PrefAndRec Preferentiel(Param_de_decoupeC2,
				     Param_de_decoupeC3);
   
   Approx_SweepApproximation_Eval ev (*this);
   Approximation(OneDTol,  TwoDTol, ThreeDTol,
		 The3D2DTol,
		 First,     Last, 
		 continuity,
		 Degmax,    Segmax, 
		 ev,
		 Preferentiel);
 }
 else {
// (3.2) Approximation without preferential cut
   AdvApprox_DichoCutting Dichotomie;
   Approx_SweepApproximation_Eval ev (*this);
   Approximation(OneDTol,  TwoDTol, ThreeDTol,
		 The3D2DTol,
		 First,     Last, 
		 continuity,
		 Degmax,    Segmax, 
		 ev,
		 Dichotomie);
 }
}

//========================================================================
//function : Approximation
//purpose  : Call F(t) and store the results
//========================================================================
void Approx_SweepApproximation::
Approximation(const Handle(TColStd_HArray1OfReal)& OneDTol,
	      const Handle(TColStd_HArray1OfReal)& TwoDTol,
	      const Handle(TColStd_HArray1OfReal)& ThreeDTol,
	      const Standard_Real BoundTol,
	      const Standard_Real First,const Standard_Real Last,
	      const GeomAbs_Shape Continuity,const Standard_Integer Degmax,
	      const Standard_Integer Segmax,
	      const AdvApprox_EvaluatorFunction& TheApproxFunction,
	      const AdvApprox_Cutting& TheCuttingTool) 
{
 AdvApprox_ApproxAFunction Approx(Num1DSS, 
				  Num2DSS,
				  Num3DSS,
				  OneDTol,
				  TwoDTol,
				  ThreeDTol,
				  First,
				  Last, 
				  Continuity,
				  Degmax,
				  Segmax, 
				  TheApproxFunction,
				  TheCuttingTool);
 done = Approx.HasResult();

 if (done) {
 // --> Fill Champs of the surface ----
   Standard_Integer ii, jj;

   vdeg = Approx.Degree();
   // Unfortunately Adv_Approx stores the transposition of the required 
   // so, writing tabPoles = Approx.Poles() will give an erroneous result
   // It is only possible to allocate and recopy term by term...
   tabPoles = new (TColgp_HArray2OfPnt)
     (1, Num3DSS, 1, Approx.NbPoles());
   tabWeights = new (TColStd_HArray2OfReal)
     (1, Num3DSS, 1, Approx.NbPoles());

   if (Num1DSS == Num3DSS) {
     Standard_Real wpoid;
     gp_Pnt P;
     for (ii=1; ii <=Num3DSS; ii++) {
       for (jj=1; jj <=Approx.NbPoles() ; jj++) {
         P = Approx.Poles()->Value(jj,ii);
         wpoid = Approx.Poles1d()->Value(jj,ii);
         P.ChangeCoord() /= wpoid; // It is necessary to divide poles by weight
         P.Translate(Translation); 
	 tabPoles->SetValue  (ii, jj, P);
	 tabWeights->SetValue(ii, jj, wpoid );
       }
     }
   }
   else {
     tabWeights->Init(1);
     for (ii=1; ii <=Num3DSS; ii++) {
       for (jj=1; jj <=Approx.NbPoles() ; jj++) {
	 tabPoles->SetValue  (ii, jj, Approx.Poles  ()->Value(jj,ii) );
       }
     }
   }
   
   // this is better
   tabVKnots = Approx.Knots();
   tabVMults = Approx.Multiplicities();

   

  // --> Filling of curves 2D  ----------
   if (Num2DSS>0) {
     gp_GTrsf2d TrsfInv;
     deg2d = vdeg;
     tab2dKnots = Approx.Knots();
     tab2dMults = Approx.Multiplicities();

     for (ii=1; ii<=Num2DSS; ii++) {
       TrsfInv = AAffin->Value(ii).Inverted();
       Handle(TColgp_HArray1OfPnt2d) P2d = 
	 new (TColgp_HArray1OfPnt2d) (1, Approx.NbPoles());
       Approx.Poles2d( ii, P2d->ChangeArray1() );
       // do not forget to apply inverted homothety.
       for (jj=1; jj<=Approx.NbPoles(); jj++) {
	  TrsfInv.Transforms(P2d->ChangeValue(jj).ChangeCoord());
	}
       seqPoles2d.Append(P2d);
     }
   }
  // ---> Filling of errors
   MError3d = new (TColStd_HArray1OfReal) (1,Num3DSS);
   AError3d = new (TColStd_HArray1OfReal) (1,Num3DSS);
   for (ii=1; ii<=Num3DSS; ii++) {
     MError3d->SetValue(ii, Approx.MaxError(3, ii));
     AError3d->SetValue(ii, Approx.AverageError(3, ii));
   }

   if (myFunc->IsRational()) {
     MError1d = new (TColStd_HArray1OfReal) (1,Num3DSS);
     AError1d = new (TColStd_HArray1OfReal) (1,Num3DSS);
     for (ii=1; ii<=Num1DSS; ii++) {
       MError1d->SetValue(ii, Approx.MaxError(1, ii));
       AError1d->SetValue(ii, Approx.AverageError(1, ii));
     }
   }

   if (Num2DSS>0) {
     tab2dError = new (TColStd_HArray1OfReal) (1,Num2DSS);
     Ave2dError = new (TColStd_HArray1OfReal) (1,Num2DSS);
     for (ii=1; ii<=Num2DSS; ii++) {
       tab2dError->SetValue(ii, Approx.MaxError(2, ii));
       Ave2dError->SetValue(ii, Approx.AverageError(2, ii));
       COnSurfErr->SetValue(ii,
	  (tab2dError->Value(ii)/TwoDTol->Value(ii))*BoundTol);
     }
   }     
 }
}

Standard_Integer Approx_SweepApproximation::Eval(const Standard_Real Parameter,
						 const Standard_Integer DerivativeRequest,
						 const Standard_Real First,
						 const Standard_Real Last,
						 Standard_Real& Result) 
{
 Standard_Integer ier=0;
 switch (DerivativeRequest) {
 case 0 : 
   ier = ( ! D0(Parameter, First, Last, Result));
   break;
 case 1 : 
   ier = ( ! D1(Parameter, First, Last, Result));
   break;
 case 2 : 
   ier = ( ! D2(Parameter, First, Last,Result));
   break;
 default :
   ier = 2;
 }
 return ier;
}

Standard_Boolean Approx_SweepApproximation::D0(const Standard_Real Param,
					       const Standard_Real First,
					       const Standard_Real Last,
					       Standard_Real& Result) 
{
  Standard_Integer index, ii;
  Standard_Boolean Ok=Standard_True;
  Standard_Real * LocalResult =  &Result;

  // Management of limits
  if ((first!=First) || (Last!=last)) {
       myFunc->SetInterval(First, Last);
     }

  if (! ( (Param==myParam) && (myOrder>=0)
	 && (first==First) && (Last==last)) ) {
    // Positioning in case when the last operation is not repeated.
    Ok = myFunc->D0(Param, First, Last,
		    myPoles->ChangeArray1(),
		    myPoles2d->ChangeArray1(),
		    myWeigths->ChangeArray1());

    //  poles3d are multiplied by weight after tranlation.
    for (ii=1; ii<=Num1DSS; ii++) {
      myPoles->ChangeValue(ii).ChangeCoord()
	-= Translation.XYZ();
      myPoles->ChangeValue(ii).ChangeCoord() 
	*= myWeigths->Value(ii);
    }

    //  The transformation is applied to poles 2d.
    for (ii=1; ii<=Num2DSS; ii++) {
      AAffin->Value(ii).Transforms(myPoles2d->ChangeValue(ii).ChangeCoord());
    }

    // Update variables of controle and return
    first = First;
    last  = Last;
    myOrder = 0;
    myParam = Param;
  }

  //  Extraction of results
  index = 0;
  for (ii=1; ii<=Num1DSS; ii++) {
    LocalResult[index] = myWeigths->Value(ii);
    index++;
  }
  for (ii=1; ii<=Num2DSS; ii++) {
    LocalResult[index] =    myPoles2d->Value(ii).X();
    LocalResult[index+1] =  myPoles2d->Value(ii).Y();
    index += 2;
  }
  for (ii=1; ii<=Num3DSS; ii++, index+=3) {
    LocalResult[index]   = myPoles->Value(ii).X();
    LocalResult[index+1] = myPoles->Value(ii).Y();
    LocalResult[index+2] = myPoles->Value(ii).Z();
  }

  return Ok;
}

Standard_Boolean Approx_SweepApproximation::D1(const Standard_Real Param,
					       const Standard_Real First,
					       const Standard_Real Last,
					       Standard_Real& Result) 
{
  gp_XY Vcoord;
  gp_Vec Vaux;
  Standard_Integer index, ii;
  Standard_Boolean Ok=Standard_True;
  Standard_Real * LocalResult =  &Result;


  if ((first!=First) || (Last!=last)) {
    myFunc->SetInterval(First, Last);
  }

  if (! ( (Param==myParam) && (myOrder>=1)
	 && (first==First) && (Last==last)) ){

    // Positioning 
    Ok = myFunc->D1(Param, First, Last,
		    myPoles->ChangeArray1(),
		    myDPoles->ChangeArray1(),
		    myPoles2d->ChangeArray1(),
		    myDPoles2d->ChangeArray1(),
		    myWeigths->ChangeArray1(),
		    myDWeigths->ChangeArray1());

    //  Take into account the multiplication of poles3d by weights.
    //  and the translation.
    for ( ii=1; ii<=Num1DSS; ii++) {
      //Translation on the section
      myPoles->ChangeValue(ii).ChangeCoord()
	-= Translation.XYZ();
      // Homothety on all. 
      const Standard_Real aWeight = myWeigths->Value(ii);
      myDPoles->ChangeValue(ii) *= aWeight;
      Vaux.SetXYZ( myPoles->Value(ii).Coord());
      myDPoles->ChangeValue(ii) += myDWeigths->Value(ii)*Vaux;
      myPoles->ChangeValue(ii).ChangeCoord() *= aWeight; // for the cash
    }
      

    //  Apply transformation 2d to suitable vectors
    for (ii=1; ii<=Num2DSS; ii++) {
      Vcoord =  myDPoles2d->Value(ii).XY();
      AAffin->Value(ii).Transforms(Vcoord);
      myDPoles2d->ChangeValue(ii).SetXY(Vcoord);
      AAffin->Value(ii).Transforms(myPoles2d->ChangeValue(ii).ChangeCoord());
    }

    // Update control variables and return
    first = First;
    last  = Last;
    myOrder = 1;
    myParam = Param;
  }
  
  //  Extraction of results
  index = 0;
  for (ii=1; ii<=Num1DSS; ii++) {
    LocalResult[index] = myDWeigths->Value(ii);
    index++;
  }
  for (ii=1; ii<=Num2DSS; ii++) {
    LocalResult[index] =    myDPoles2d->Value(ii).X();
    LocalResult[index+1] =  myDPoles2d->Value(ii).Y();
    index += 2;
  }
  for (ii=1; ii<=Num3DSS; ii++, index+=3) {
    LocalResult[index]   = myDPoles->Value(ii).X();
    LocalResult[index+1] = myDPoles->Value(ii).Y();
    LocalResult[index+2] = myDPoles->Value(ii).Z();
  }
  return Ok;
}

Standard_Boolean Approx_SweepApproximation::D2(const Standard_Real Param,
					       const Standard_Real First,
					       const Standard_Real Last,
					       Standard_Real& Result) 
{
  gp_XY Vcoord;  
  gp_Vec Vaux;
  Standard_Integer index, ii;
  Standard_Boolean Ok=Standard_True;
  Standard_Real * LocalResult =  &Result;

  // management of limits
  if ((first!=First) || (Last!=last)) {
       myFunc->SetInterval(First, Last);
     }

  if (! ( (Param==myParam) && (myOrder>=2)
	 && (first==First) && (Last==last)) ) {
    // Positioning in case when the last operation is not repeated
    Ok = myFunc->D2(Param, First, Last,
		    myPoles->ChangeArray1(),
		    myDPoles->ChangeArray1(),
		    myD2Poles->ChangeArray1(),
		    myPoles2d->ChangeArray1(),
		    myDPoles2d->ChangeArray1(),
		    myD2Poles2d->ChangeArray1(),
		    myWeigths->ChangeArray1(),
		    myDWeigths->ChangeArray1(),
		    myD2Weigths->ChangeArray1());

    //  Multiply poles3d by the weight after tranlations.
    for (ii=1; ii<=Num1DSS; ii++) {
      // First translate
      myPoles->ChangeValue(ii).ChangeCoord()
	-= Translation.XYZ();
	
      //Calculate the second derivative
      myD2Poles->ChangeValue(ii) *= myWeigths->Value(ii);
      Vaux.SetXYZ( myDPoles->Value(ii).XYZ());
      myD2Poles->ChangeValue(ii) += (2*myDWeigths->Value(ii))*Vaux;
      Vaux.SetXYZ( myPoles->Value(ii).Coord());
      myD2Poles->ChangeValue(ii) += myD2Weigths->Value(ii)*Vaux;

      //Then the remainder for the cash
      myDPoles->ChangeValue(ii) *= myWeigths->Value(ii);
      Vaux.SetXYZ( myPoles->Value(ii).Coord());
      myDPoles->ChangeValue(ii) += myDWeigths->Value(ii)*Vaux;
      myPoles->ChangeValue(ii).ChangeCoord() 
	*= myWeigths->Value(ii); 
    }

    //  Apply transformation to poles 2d.
    for (ii=1; ii<=Num2DSS; ii++) {
      Vcoord =  myD2Poles2d->Value(ii).XY();
      AAffin->Value(ii).Transforms(Vcoord);
      myD2Poles2d->ChangeValue(ii).SetXY(Vcoord);
      Vcoord =  myDPoles2d->Value(ii).XY();
      AAffin->Value(ii).Transforms(Vcoord);
      myDPoles2d->ChangeValue(ii).SetXY(Vcoord);
      AAffin->Value(ii).Transforms(myPoles2d->ChangeValue(ii).ChangeCoord());
    }

    // Update variables of control and return
    first = First;
    last  = Last;
    myOrder = 2;
    myParam = Param;
  }

  //  Extraction of results
  index = 0;
  for (ii=1; ii<=Num1DSS; ii++) {
    LocalResult[index] = myD2Weigths->Value(ii);
    index++;
  }
  for (ii=1; ii<=Num2DSS; ii++) {
    LocalResult[index] =    myD2Poles2d->Value(ii).X();
    LocalResult[index+1] =  myD2Poles2d->Value(ii).Y();
    index += 2;
  }
  for (ii=1; ii<=Num3DSS; ii++, index+=3) {
    LocalResult[index]   = myD2Poles->Value(ii).X();
    LocalResult[index+1] = myD2Poles->Value(ii).Y();
    LocalResult[index+2] = myD2Poles->Value(ii).Z();
  }

  return Ok;
}

void Approx_SweepApproximation::
SurfShape(Standard_Integer& UDegree,
	  Standard_Integer& VDegree,Standard_Integer& NbUPoles,
	  Standard_Integer& NbVPoles,
	  Standard_Integer& NbUKnots,
	  Standard_Integer& NbVKnots) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  UDegree  = udeg;
  VDegree  = vdeg;
  NbUPoles = tabPoles->ColLength();
  NbVPoles = tabPoles->RowLength();
  NbUKnots = tabUKnots->Length();
  NbVKnots = tabVKnots->Length();
}

void Approx_SweepApproximation::
Surface(TColgp_Array2OfPnt& TPoles,
	TColStd_Array2OfReal& TWeights,
	TColStd_Array1OfReal& TUKnots,
	TColStd_Array1OfReal& TVKnots,
	TColStd_Array1OfInteger& TUMults,
	TColStd_Array1OfInteger& TVMults) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  TPoles   = tabPoles->Array2();
  TWeights = tabWeights->Array2();
  TUKnots  = tabUKnots->Array1();
  TUMults  = tabUMults->Array1();
  TVKnots  = tabVKnots->Array1();
  TVMults  = tabVMults->Array1();
}

Standard_Real Approx_SweepApproximation::MaxErrorOnSurf() const
{
 Standard_Integer ii;
 Standard_Real MaxError = 0, err;
 if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}

 if (myFunc->IsRational()) {
   TColStd_Array1OfReal Wmin(1, Num1DSS);
   myFunc->GetMinimalWeight(Wmin);
   Standard_Real Size = myFunc->MaximalSection();
   for (ii=1; ii<=Num3DSS; ii++) {
     err = (Size*MError1d->Value(ii) + MError3d->Value(ii)) / Wmin(ii);
     if (err>MaxError) MaxError = err;
   }
 }
 else {
  for (ii=1; ii<=Num3DSS; ii++) {
     err = MError3d->Value(ii);
     if (err>MaxError) MaxError = err;
   } 
 } 
 return MaxError;
}

 Standard_Real Approx_SweepApproximation::AverageErrorOnSurf() const
{
 Standard_Integer ii;
 Standard_Real MoyError = 0, err;
 if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}

 if (myFunc->IsRational()) {
   TColStd_Array1OfReal Wmin(1, Num1DSS);
   myFunc->GetMinimalWeight(Wmin);
   Standard_Real Size = myFunc->MaximalSection();
   for (ii=1; ii<=Num3DSS; ii++) {
     err = (Size*AError1d->Value(ii) + AError3d->Value(ii)) / Wmin(ii);
     MoyError += err;
   }
 }
 else {
  for (ii=1; ii<=Num3DSS; ii++) {
     err = AError3d->Value(ii);
     MoyError += err;
   } 
 } 
 return MoyError/Num3DSS;
}


void Approx_SweepApproximation::Curves2dShape(Standard_Integer& Degree,
					      Standard_Integer& NbPoles,
					      Standard_Integer& NbKnots) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  if (seqPoles2d.Length() == 0) {throw Standard_DomainError("Approx_SweepApproximation");}
  Degree = deg2d;
  NbPoles = seqPoles2d(1)->Length();
  NbKnots = tab2dKnots->Length();
}

void Approx_SweepApproximation::Curve2d(const Standard_Integer Index,
					TColgp_Array1OfPnt2d& TPoles,
					TColStd_Array1OfReal& TKnots,
					TColStd_Array1OfInteger& TMults) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  if (seqPoles2d.Length() == 0) {throw Standard_DomainError("Approx_SweepApproximation");}
  TPoles = seqPoles2d(Index)->Array1();
  TKnots  = tab2dKnots->Array1();
  TMults  = tab2dMults->Array1(); 
}

 Standard_Real Approx_SweepApproximation::Max2dError(const Standard_Integer Index) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  return tab2dError->Value(Index);
}

 Standard_Real Approx_SweepApproximation::Average2dError(const Standard_Integer Index) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  return Ave2dError->Value(Index); 
}

Standard_Real Approx_SweepApproximation::TolCurveOnSurf(const Standard_Integer Index) const
{
  if (!done) {throw StdFail_NotDone("Approx_SweepApproximation");}
  return  COnSurfErr->Value(Index);
}

 void Approx_SweepApproximation::Dump(Standard_OStream& o) const
{
  o << "Dump of SweepApproximation" << endl;
  if (done) { 
    o << "Error 3d = " << MaxErrorOnSurf() << endl;

    if (Num2DSS>0) {
      o << "Error 2d = ";
      for (Standard_Integer ii=1; ii<=Num2DSS; ii++) 
	{  o << Max2dError(ii);
	   if (ii < Num2DSS) o << " , " << endl;
	 }
      cout << endl;
    }
    o <<  tabVKnots->Length()-1 <<" Segment(s) of degree " << vdeg << endl;
  }
  else cout << " Not Done " << endl;
}
