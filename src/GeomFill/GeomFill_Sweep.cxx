// Created on: 1997-11-21
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

//  Modified by skv - Fri Feb  6 11:44:48 2004 OCC5073

#include <AdvApprox_ApproxAFunction.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Approx_SweepApproximation.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_LocFunction.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_Sweep.hxx>
#include <GeomFill_SweepFunction.hxx>
#include <GeomLib.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Lin.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Sphere.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

//#include <GeomLib_Array1OfMat.hxx>
//=======================================================================
//class : GeomFill_Sweep_Eval
//purpose: The evaluator for curve approximation
//=======================================================================
class GeomFill_Sweep_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  GeomFill_Sweep_Eval (GeomFill_LocFunction& theTool)
    : theAncore(theTool) {}
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  GeomFill_LocFunction& theAncore;
};

void GeomFill_Sweep_Eval::Evaluate (Standard_Integer *,/*Dimension*/
                                    Standard_Real     StartEnd[2],
                                    Standard_Real    *Parameter,
                                    Standard_Integer *DerivativeRequest,
                                    Standard_Real    *Result,// [Dimension]
                                    Standard_Integer *ErrorCode)
{
  theAncore.DN (*Parameter,
		StartEnd[0],
		StartEnd[1],
		*DerivativeRequest, 
		Result[0],
		ErrorCode[0]);
}

//===============================================================
// Function : Create
// Purpose :
//===============================================================
GeomFill_Sweep::GeomFill_Sweep(const Handle(GeomFill_LocationLaw)& Location,
			       const Standard_Boolean WithKpart)
{
  done = Standard_False;

  myLoc =  Location;
  myKPart =  WithKpart;
  SetTolerance(1.e-4);
  myForceApproxC1 = Standard_False;

  myLoc->GetDomain(First, Last);
  SFirst = SLast = 30.081996;
  SError = RealLast();
}

//===============================================================
// Function : SetDomain
// Purpose :
//===============================================================
 void GeomFill_Sweep::SetDomain(const Standard_Real LocFirst,
				const Standard_Real LocLast,
				const Standard_Real SectionFirst,
				const Standard_Real SectionLast) 
{
  First = LocFirst;
  Last =  LocLast;
  SFirst = SectionFirst;
  SLast = SectionLast;
}

//===============================================================
// Function : SetTolerance
// Purpose :
//===============================================================
 void GeomFill_Sweep::SetTolerance(const Standard_Real Tolerance3d,
				   const Standard_Real BoundTolerance,
				   const Standard_Real Tolerance2d,
				   const Standard_Real ToleranceAngular) 
{
  Tol3d = Tolerance3d;
  BoundTol = BoundTolerance;
  Tol2d =Tolerance2d;
  TolAngular = ToleranceAngular;
}

//=======================================================================
//Function : SetForceApproxC1
//Purpose  : Set the flag that indicates attempt to approximate
//           a C1-continuous surface if a swept surface proved
//           to be C0.
//=======================================================================
 void GeomFill_Sweep::SetForceApproxC1(const Standard_Boolean ForceApproxC1)
{
  myForceApproxC1 = ForceApproxC1;
}


//===============================================================
// Function : ExchangeUV
// Purpose :
//===============================================================
 Standard_Boolean GeomFill_Sweep::ExchangeUV() const
{
  return myExchUV;
}

//===============================================================
// Function : UReversed
// Purpose :
//===============================================================
 Standard_Boolean GeomFill_Sweep::UReversed() const
{
  return isUReversed;
}

//===============================================================
// Function : VReversed
// Purpose :
//===============================================================
 Standard_Boolean GeomFill_Sweep::VReversed() const
{
  return isVReversed;
}

//===============================================================
// Function : Build
// Purpose :
//===============================================================
 void GeomFill_Sweep::Build(const Handle(GeomFill_SectionLaw)& Section,	       
			    const GeomFill_ApproxStyle Methode,
			    const GeomAbs_Shape Continuity,
			    const Standard_Integer Degmax,
			    const Standard_Integer Segmax) 
{
  // Inits
  done = Standard_False;
  myExchUV = Standard_False;
  isUReversed = isVReversed = Standard_False;
  mySec =  Section;

  if ((SFirst == SLast) &&  (SLast == 30.081996)) {
    mySec->GetDomain(SFirst, SLast);
  }
   
  Standard_Boolean isKPart = Standard_False, 
                   isProduct  = Standard_False;
 
 // Traitement des KPart
 if (myKPart)  isKPart = BuildKPart();
 
 // Traitement des produits Formelles
 if ((!isKPart) && (Methode == GeomFill_Location)) {
     Handle(Geom_BSplineSurface) BS;
     BS = mySec->BSplineSurface();
     if (! BS.IsNull()) {
      // Approx de la loi
//    isProduct = BuildProduct(Continuity, Degmax, Segmax);
     }
   }

 if (isKPart || isProduct) {
 // Approx du 2d
   done =  Build2d(Continuity, Degmax, Segmax);
 }
  else {
 // Approx globale
    done =  BuildAll(Continuity, Degmax, Segmax);
  }
}

//===============================================================
// Function ::Build2d
// Purpose :A venir...
//===============================================================
// Standard_Boolean GeomFill_Sweep::Build2d(const GeomAbs_Shape Continuity,
 Standard_Boolean GeomFill_Sweep::Build2d(const GeomAbs_Shape ,
//					  const Standard_Integer Degmax,
					  const Standard_Integer ,
//					  const Standard_Integer Segmax) 
					  const Standard_Integer ) 
{
  Standard_Boolean Ok = Standard_False;
  if (myLoc->Nb2dCurves() == 0) {
    Ok = Standard_True;
  }
  return Ok;
}

//===============================================================
// Function : BuildAll
// Purpose :
//===============================================================
 Standard_Boolean GeomFill_Sweep::BuildAll(const GeomAbs_Shape Continuity,
					   const Standard_Integer Degmax,
					   const Standard_Integer Segmax) 
{
  Standard_Boolean Ok = Standard_False;

  Handle(GeomFill_SweepFunction) Func 
    = new (GeomFill_SweepFunction) (mySec, myLoc, First, SFirst,
				   (SLast-SFirst)/(Last-First) );
  Approx_SweepApproximation Approx( Func );

  Approx.Perform(First,  Last,
		 Tol3d,  BoundTol,  Tol2d,  TolAngular,
		 Continuity, Degmax,  Segmax);

  if (Approx.IsDone()) {
    Ok = Standard_True;

#ifdef OCCT_DEBUG
    Approx.Dump(cout);
#endif
    
    // La surface
    Standard_Integer UDegree,VDegree,NbUPoles,
                     NbVPoles,NbUKnots,NbVKnots;
    Approx.SurfShape(UDegree,VDegree,NbUPoles,
                     NbVPoles,NbUKnots,NbVKnots);

    TColgp_Array2OfPnt Poles(1,NbUPoles, 1,NbVPoles);
    TColStd_Array2OfReal Weights(1,NbUPoles, 1,NbVPoles);
    TColStd_Array1OfReal UKnots(1, NbUKnots),VKnots(1, NbVKnots); 
    TColStd_Array1OfInteger UMults(1, NbUKnots), VMults(1, NbVKnots);

    Approx.Surface(Poles, Weights,
		   UKnots,VKnots,
		   UMults,VMults);

    mySurface = new (Geom_BSplineSurface)
                   (Poles, Weights,
		    UKnots,VKnots,
		    UMults,VMults,
		    Approx.UDegree(),  Approx.VDegree(),
		    mySec->IsUPeriodic());
    SError = Approx. MaxErrorOnSurf();

    if (myForceApproxC1 && !mySurface->IsCNv(1))
    {
      Standard_Real theTol = 1.e-4;
      GeomAbs_Shape theUCont = GeomAbs_C1, theVCont = GeomAbs_C1;
      Standard_Integer degU = 14, degV = 14;
      Standard_Integer nmax = 16;
      Standard_Integer thePrec = 1;  
      
      GeomConvert_ApproxSurface ConvertApprox(mySurface,theTol,theUCont,theVCont,
                                              degU,degV,nmax,thePrec);
      if (ConvertApprox.HasResult())
      {
        mySurface = ConvertApprox.Surface();
        myCurve2d = new  (TColGeom2d_HArray1OfCurve) (1, 2);
        CError =  new (TColStd_HArray2OfReal) (1,2, 1,2);

        Handle(Geom_BSplineSurface) BSplSurf (Handle(Geom_BSplineSurface)::DownCast(mySurface));
        
        gp_Dir2d D(0., 1.);
        gp_Pnt2d P(BSplSurf->UKnot(1), 0);
        Handle(Geom2d_Line) LC1 = new (Geom2d_Line) (P, D);
        Handle(Geom2d_TrimmedCurve) TC1 =
          new (Geom2d_TrimmedCurve) (LC1, 0, BSplSurf->VKnot(BSplSurf->NbVKnots()));
        
        myCurve2d->SetValue(1, TC1);
        CError->SetValue(1, 1, 0.);
        CError->SetValue(2, 1, 0.);
 
        P.SetCoord(BSplSurf->UKnot(BSplSurf->NbUKnots()), 0);
        Handle(Geom2d_Line) LC2 = new (Geom2d_Line) (P, D);
        Handle(Geom2d_TrimmedCurve) TC2 = 
          new (Geom2d_TrimmedCurve) (LC2, 0, BSplSurf->VKnot(BSplSurf->NbVKnots()));
        
        myCurve2d->SetValue(myCurve2d->Length(), TC2);
        CError->SetValue(1, myCurve2d->Length(), 0.);
        CError->SetValue(2, myCurve2d->Length(), 0.);
        
        SError = theTol;
      }
    } //if (!mySurface->IsCNv(1))
    
    // Les Courbes 2d
    if (myCurve2d.IsNull())
    {
      myCurve2d = new  (TColGeom2d_HArray1OfCurve) (1, 2+myLoc->TraceNumber());
      CError =  new (TColStd_HArray2OfReal) (1,2, 1, 2+myLoc->TraceNumber());
      Standard_Integer kk,ii, ifin = 1, ideb;
      
      if (myLoc->HasFirstRestriction()) {
        ideb = 1;
      }
      else {
        ideb = 2;
      }
      ifin += myLoc->TraceNumber();
      if (myLoc->HasLastRestriction()) ifin++;
      
      for (ii=ideb, kk=1; ii<=ifin; ii++, kk++) {
        Handle(Geom2d_BSplineCurve) C 
          = new (Geom2d_BSplineCurve) (Approx.Curve2dPoles(kk),
                                       Approx.Curves2dKnots(),
                                       Approx.Curves2dMults(),
                                       Approx.Curves2dDegree());
        myCurve2d->SetValue(ii, C);
        CError->SetValue(1, ii,  Approx.Max2dError(kk));
        CError->SetValue(2, ii,  Approx.Max2dError(kk));
      }
      
      // Si les courbes de restriction, ne sont pas calcules, on prend
      // les iso Bords.
      if (! myLoc->HasFirstRestriction()) {
        gp_Dir2d D(0., 1.);
        gp_Pnt2d P(UKnots(UKnots.Lower()), 0);
        Handle(Geom2d_Line) LC = new (Geom2d_Line) (P, D);
        Handle(Geom2d_TrimmedCurve) TC = new (Geom2d_TrimmedCurve)
          (LC, First, Last);
        
        myCurve2d->SetValue(1, TC);
        CError->SetValue(1, 1, 0.);
        CError->SetValue(2, 1, 0.);
      }
      
      if (! myLoc->HasLastRestriction()) {
        gp_Dir2d D(0., 1.);
        gp_Pnt2d P(UKnots(UKnots.Upper()), 0);
        Handle(Geom2d_Line) LC = new (Geom2d_Line) (P, D);
        Handle(Geom2d_TrimmedCurve) TC = 
          new (Geom2d_TrimmedCurve) (LC, First, Last);
        myCurve2d->SetValue(myCurve2d->Length(), TC);
        CError->SetValue(1, myCurve2d->Length(), 0.);
        CError->SetValue(2, myCurve2d->Length(), 0.);
      }
    } //if (myCurve2d.IsNull())
  } 
  return Ok;
}

//===============================================================
// Function : BuildProduct
// Purpose : A venir...
//===============================================================
 Standard_Boolean GeomFill_Sweep::BuildProduct(const GeomAbs_Shape Continuity,
					  const Standard_Integer Degmax,
					  const Standard_Integer Segmax) 
{
  Standard_Boolean Ok = Standard_False;

  Handle(Geom_BSplineSurface) BSurf;
  BSurf = Handle(Geom_BSplineSurface)::DownCast(
	   mySec->BSplineSurface()->Copy());
  if (BSurf.IsNull()) return Ok; // Ce mode de construction est impossible  


  Standard_Integer NbIntervalC2,  NbIntervalC3;
  GeomFill_LocFunction Func(myLoc);

  NbIntervalC2 = myLoc->NbIntervals(GeomAbs_C2);
  NbIntervalC3 = myLoc->NbIntervals(GeomAbs_C3);
  TColStd_Array1OfReal Param_de_decoupeC2 (1, NbIntervalC2+1);
  myLoc->Intervals(Param_de_decoupeC2, GeomAbs_C2);
  TColStd_Array1OfReal Param_de_decoupeC3 (1, NbIntervalC3+1);
  myLoc->Intervals(Param_de_decoupeC3, GeomAbs_C3);


  AdvApprox_PrefAndRec Preferentiel(Param_de_decoupeC2,
				     Param_de_decoupeC3);
   
  Handle(TColStd_HArray1OfReal) ThreeDTol = new (TColStd_HArray1OfReal) (1,4);
  ThreeDTol->Init(Tol3d); // A Affiner...

  GeomFill_Sweep_Eval eval (Func);
  AdvApprox_ApproxAFunction Approx(0, 0, 4,
				   ThreeDTol,
				   ThreeDTol,
				   ThreeDTol,
				   First,
				   Last, 
				   Continuity,
				   Degmax,
				   Segmax, 
				   eval,
				   Preferentiel);
#ifdef OCCT_DEBUG
  Approx.Dump(cout);
#endif

  Ok = Approx.HasResult();
  if (Ok) {
/*    TColgp_Array1OfMat TM(1, nbpoles);
    Handle(TColgp_HArray2OfPnt) ResPoles;
    ResPoles = Approx.Poles();

    // Produit Tensoriel
    for (ii=1; ii<=nbpoles; ii++) {
      TM(ii).SetCols(ResPoles->Value(ii,2).XYZ(), 
		     ResPoles->Value(ii,3).XYZ(),  
		     ResPoles->Value(ii,4).XYZ());
      TR(ii) = ResPoles->Value(ii,1);
    }
    GeomLib::TensorialProduct(BSurf, TM, TR,
			      Approx.Knots()->Array1(), 
			      Approx.Multiplicities()->Array1());

    // Somme
    TColgp_Array1OfPnt TPoles(1, nbpoles);
    for (ii=1; ii<=nbpoles; ii++) {
      TPoles(ii) = ResPoles->Value(ii,1);
    }
    Handle(Geom_BsplineCurve) BS = 
      new (Geom_BsplineCurve) (Poles, 
			       Approx.Knots()->Array1(), 
			       Approx.Multiplicities()->Array1(),
			       Approx.Degree());
    for (ii=1; ii<=BSurf->NbVKnots(); ii++)
      BS->InsertKnot( BSurf->VKnot(ii), 
		     BSurf->VMultiplicity(ii), 
		     Precision::Confusion());
   TColgp_Array2OfPnt SurfPoles (1, BSurf->NbUPoles());
   for (ii=1; 
	
*/
    mySurface = BSurf;
  }
  return Ok;
}

//  Modified by skv - Thu Feb  5 18:05:03 2004 OCC5073 Begin
//  Conditions:
//     * theSec should be constant
//     * the type of section should be a line
//     * theLoc should represent a translation.

static Standard_Boolean IsSweepParallelSpine (const Handle(GeomFill_LocationLaw) &theLoc,
			     const Handle(GeomFill_SectionLaw)  &theSec,
			     const Standard_Real                 theTol)
{
  // Get the first and last transformations of the location
  Standard_Real aFirst;
  Standard_Real aLast;
  gp_Vec        VBegin;
  gp_Vec        VEnd;
  gp_Mat        M;
  gp_GTrsf      GTfBegin;
  gp_Trsf       TfBegin;
  gp_GTrsf      GTfEnd;
  gp_Trsf       TfEnd;

  theLoc->GetDomain(aFirst, aLast);

// Get the first transformation
  theLoc->D0(aFirst, M, VBegin);

  GTfBegin.SetVectorialPart(M);
  GTfBegin.SetTranslationPart(VBegin.XYZ());

  TfBegin.SetValues(GTfBegin(1,1), GTfBegin(1,2), GTfBegin(1,3), GTfBegin(1,4),
		    GTfBegin(2,1), GTfBegin(2,2), GTfBegin(2,3), GTfBegin(2,4),
		    GTfBegin(3,1), GTfBegin(3,2), GTfBegin(3,3), GTfBegin(3,4));

// Get the last transformation
  theLoc->D0(aLast, M, VEnd);

  GTfEnd.SetVectorialPart(M);
  GTfEnd.SetTranslationPart(VEnd.XYZ());

  TfEnd.SetValues(GTfEnd(1,1), GTfEnd(1,2), GTfEnd(1,3), GTfEnd(1,4),
		  GTfEnd(2,1), GTfEnd(2,2), GTfEnd(2,3), GTfEnd(2,4),
		  GTfEnd(3,1), GTfEnd(3,2), GTfEnd(3,3), GTfEnd(3,4));

  Handle(Geom_Surface) aSurf = theSec->BSplineSurface();
  Standard_Real Umin;
  Standard_Real Umax;
  Standard_Real Vmin;
  Standard_Real Vmax;

  aSurf->Bounds(Umin, Umax, Vmin, Vmax);

  // Get and transform the first section
  Handle(Geom_Curve) FirstSection = theSec->ConstantSection();
  GeomAdaptor_Curve  ACFirst(FirstSection);

  Standard_Real UFirst = ACFirst.FirstParameter();
  gp_Lin        L      = ACFirst.Line();

  L.Transform(TfBegin);

  // Get and transform the last section
  Handle(Geom_Curve) aLastSection    = aSurf->VIso(Vmax);
  Standard_Real      aFirstParameter = aLastSection->FirstParameter();
  gp_Pnt             aPntLastSec     = aLastSection->Value(aFirstParameter);

  aPntLastSec.Transform(TfEnd);

  gp_Pnt        aPntFirstSec = ElCLib::Value( UFirst, L );
  gp_Vec        aVecSec( aPntFirstSec, aPntLastSec );
  gp_Vec        aVecSpine = VEnd - VBegin;

  Standard_Boolean isParallel = aVecSec.IsParallel(aVecSpine, theTol);

  return isParallel;
}
//  Modified by skv - Thu Feb  5 18:05:01 2004 OCC5073 End

//===============================================================
// Function : BuildKPart
// Purpose :
//===============================================================
 Standard_Boolean GeomFill_Sweep::BuildKPart() 
{
  Standard_Boolean Ok = Standard_False;
  Standard_Boolean isUPeriodic = Standard_False;
  Standard_Boolean isVPeriodic = Standard_False;
  Standard_Boolean IsTrsf = Standard_True;

  isUPeriodic = mySec->IsUPeriodic();
  Handle(Geom_Surface) S;
  GeomAbs_CurveType SectionType;
  gp_Vec V;
  gp_Mat M;
  Standard_Real levier, error = 0 ;
  Standard_Real UFirst=0, VFirst=First, ULast=0, VLast=Last;
  Standard_Real Tol = Min (Tol3d, BoundTol);

  // (1) Trajectoire Rectilignes -------------------------
  if (myLoc->IsTranslation(error)) {
    // Donne de la translation
    gp_Vec DP, DS;
    myLoc->D0(1, M, DS);
    myLoc->D0(0, M, V);
    DP = DS - V;
    DP.Normalize();
    gp_GTrsf Tf;
    gp_Trsf Tf2;
    Tf.SetVectorialPart(M);
    Tf.SetTranslationPart(V.XYZ());
    try { // Pas joli mais il n'y as pas d'autre moyens de tester SetValues
      OCC_CATCH_SIGNALS
      Tf2.SetValues(Tf(1,1), Tf(1,2), Tf(1,3), Tf(1,4),
		    Tf(2,1), Tf(2,2), Tf(2,3), Tf(2,4),
		    Tf(3,1), Tf(3,2), Tf(3,3), Tf(3,4));
    }
    catch (Standard_ConstructionError) {
      IsTrsf = Standard_False;
    }
    if (!IsTrsf) {
      return Standard_False;
    }

    // (1.1) Cas Extrusion
    if (mySec->IsConstant(error)) {
      Handle(Geom_Curve) Section;
      Section = mySec->ConstantSection();
      GeomAdaptor_Curve AC(Section);
      SectionType = AC.GetType();
      UFirst = AC.FirstParameter();
      ULast  = AC.LastParameter();
  // (1.1.a) Cas Plan
      if ( (SectionType == GeomAbs_Line) && IsTrsf) {
//  Modified by skv - Thu Feb  5 11:39:06 2004 OCC5073 Begin
	if (!IsSweepParallelSpine(myLoc, mySec, Tol))
	  return Standard_False;
//  Modified by skv - Thu Feb  5 11:39:08 2004 OCC5073 End
	gp_Lin L = AC.Line();
        L.Transform(Tf2);
	DS.SetXYZ(L.Position().Direction().XYZ());
	DS.Normalize();
        levier = Abs(DS.Dot(DP));
        SError = error + levier * Abs(Last-First);
	if (SError <= Tol) {
	  Ok = Standard_True;
	  gp_Ax2 AxisOfPlane (L.Location(), DS^DP, DS);
	  S = new (Geom_Plane) (AxisOfPlane);
	}
	else SError = 0.;
      }
    
  // (1.1.b) Cas Cylindrique
      if ( (SectionType == GeomAbs_Circle) && IsTrsf) {
        const Standard_Real TolProd = 1.e-6;
        
	gp_Circ C = AC.Circle();
	C.Transform(Tf2);
	
	DS.SetXYZ (C.Position().Direction().XYZ());
	DS.Normalize();
        levier = Abs(DS.CrossMagnitude(DP)) * C.Radius();
        SError = levier * Abs(Last - First);
	if (SError <= TolProd) {
	  Ok = Standard_True;
          gp_Ax3 axe (C.Location(), DP, C.Position().XDirection());
	  S = new (Geom_CylindricalSurface) 
	    (axe, C.Radius());
	    if (C.Position().Direction().
		IsOpposite(axe.Direction(), 0.1) ) {
	      Standard_Real f, l;
	      // L'orientation parametrique est inversee
	      l = 2*M_PI - UFirst;
	      f = 2*M_PI - ULast;
	      UFirst = f;
	      ULast  = l;
	      isUReversed = Standard_True;
	    }
	}
	else SError = 0.;
      }

  // (1.1.c) C'est bien une extrusion
      if (!Ok) {
	if (IsTrsf) {
	  Section->Transform(Tf2);
	  S = new (Geom_SurfaceOfLinearExtrusion)
	    (Section, DP);
	  SError = 0.;
	  Ok = Standard_True;
	}
	else { // extrusion sur BSpline
	  
	}
      }
    }
  
  // (1.2) Cas conique
   else if (mySec->IsConicalLaw(error)) {

     gp_Pnt P1, P2, Centre0, Centre1, Centre2;
     gp_Vec dsection;
     Handle(Geom_Curve) Section;
     GeomAdaptor_Curve AC;
     gp_Circ C;
     Standard_Real R1, R2;


     Section = mySec->CirclSection(SLast);
     Section->Transform(Tf2);
     Section->Translate(Last*DP);
     AC.Load(Section);
     C = AC.Circle();
     Centre2 = C.Location();
     AC.D1(0, P2, dsection);
     R2 = C.Radius();
    
     Section = mySec->CirclSection(SFirst);
     Section->Transform(Tf2);
     Section->Translate(First*DP);
     AC.Load(Section);
     C =  AC.Circle();
     Centre1 = C.Location();
     P1 = AC.Value(0);
     R1 = C.Radius();

     Section = mySec->CirclSection(SFirst - First*(SLast-SFirst)/(Last-First));
     Section->Transform(Tf2);
     AC.Load(Section);
     C =  AC.Circle();     
     Centre0 = C.Location();
     
     Standard_Real Angle;
     gp_Vec  N(Centre1, P1);
     if (N.Magnitude() < 1.e-9) {
       gp_Vec Bis(Centre2, P2);
       N = Bis;
     }
     gp_Vec  L(P1, P2), Dir(Centre1,Centre2);

     Angle = L.Angle(Dir);
     if ((Angle > 0.01) && (Angle < M_PI/2-0.01)) {
       if (R2<R1) Angle = -Angle;
       SError = error;
       gp_Ax3 Axis(Centre0, Dir, N);
       S = new (Geom_ConicalSurface) 
	 (Axis, Angle, C.Radius());
       // Calcul du glissement parametrique
       VFirst = First / Cos(Angle);
       VLast  = Last  / Cos(Angle);

       // Bornes en U
       UFirst = AC.FirstParameter();
       ULast  = AC.LastParameter();
       gp_Vec diso;
       gp_Pnt pbis;
       S->VIso(VLast)->D1(0, pbis, diso);
       if (diso.Magnitude()>1.e-9 && dsection.Magnitude()>1.e-9)
	 isUReversed = diso.IsOpposite(dsection, 0.1);
       if (isUReversed ) {
	 Standard_Real f, l;
	 // L'orientation parametrique est inversee
	 l = 2*M_PI - UFirst;
	 f = 2*M_PI - ULast;
	 UFirst = f;
	 ULast  = l;
       } 

       // C'est un cone
       Ok = Standard_True;
     }
   }
  }

  // (2) Trajectoire Circulaire
  if (myLoc->IsRotation(error)) {
    if (mySec->IsConstant(error)) {
      // La trajectoire
      gp_Pnt Centre;
      isVPeriodic = (Abs(Last-First -2*M_PI) < 1.e-15);
      Standard_Real RotRadius;
      gp_Vec DP, DS, DN;
      myLoc->D0(0.1, M, DS);
      myLoc->D0(0, M, V);
      myLoc->Rotation(Centre);

      DP = DS - V;
      DS.SetXYZ(V.XYZ() - Centre.XYZ());   
      RotRadius = DS.Magnitude();
      if (RotRadius > 1.e-15) DS.Normalize();
      else return Standard_False; // Pas de KPart, rotation degeneree
      DN = DS ^ DP;
      DN.Normalize();
      DP = DN ^ DS;
      DP.Normalize();

      gp_GTrsf Tf;
      gp_Trsf Tf2;
      Tf.SetVectorialPart(M);
      Tf.SetTranslationPart(V.XYZ());
//      try { // Pas joli mais il n'y as pas d'autre moyens de tester SetValues
//        OCC_CATCH_SIGNALS
	Tf2.SetValues(Tf(1,1), Tf(1,2), Tf(1,3), Tf(1,4),
		      Tf(2,1), Tf(2,2), Tf(2,3), Tf(2,4),
		      Tf(3,1), Tf(3,2), Tf(3,3), Tf(3,4));
//      }
//      catch (Standard_ConstructionError) {
//	IsTrsf = Standard_False;
//      }
      // La section
      Handle(Geom_Curve) Section;
      Section = mySec->ConstantSection();
      GeomAdaptor_Curve AC(Section);
      SectionType = AC.GetType();
      UFirst = AC.FirstParameter();
      ULast  = AC.LastParameter();

      // (2.1) Tore/Sphere ?
      if ((SectionType == GeomAbs_Circle) && IsTrsf) {
	gp_Circ C = AC.Circle();
        Standard_Real Radius;
	Standard_Boolean IsGoodSide = Standard_True;;
	C.Transform(Tf2);
	gp_Vec DC;
        // On calcul le centre eventuel
        DC.SetXYZ(C.Location().XYZ() - Centre.XYZ());
	Centre.ChangeCoord() += (DC.Dot(DN))*DN.XYZ();
	DC.SetXYZ(C.Location().XYZ() - Centre.XYZ());
        Radius = DC.Magnitude(); //grand Rayon du tore
	if ((Radius > Tol) && (DC.Dot(DS) < 0)) IsGoodSide = Standard_False;
        if (Radius < Tol/100) DC = DS; // Pour definir le tore

	// On verifie d'abord que le plan de la section est // a 
	// l'axe de rotation
	gp_Vec NC;
	NC.SetXYZ (C.Position().Direction().XYZ());
	NC.Normalize();
	error =  Abs(NC.Dot(DN));
	// Puis on evalue l'erreur commise sur la section, 
        // en pivotant son plan ( pour contenir l'axe de rotation)
        error += Abs(NC.Dot(DS));
        error *= C.Radius();
	if (error <= Tol) {
	  SError = error;
	  error += Radius;
          if (Radius <= Tol) {
	    // (2.1.a) Sphere
	    Standard_Real f = UFirst , l =  ULast, aRadius = 0.0;
	    SError = error;
	    Centre.BaryCenter(1.0, C.Location(), 1.0); 
	    gp_Ax3 AxisOfSphere(Centre, DN, DS);
            aRadius = C.Radius();
            gp_Sphere theSphere( AxisOfSphere, aRadius );
	    S = new Geom_SphericalSurface(theSphere);
	    // Pour les spheres on ne peut pas controler le parametre
            // V (donc U car  myExchUV = Standard_True)
            // Il faut donc modifier UFirst, ULast...
            Standard_Real fpar = AC.FirstParameter();
            Standard_Real lpar = AC.LastParameter();
            Handle(Geom_Curve) theSection = new Geom_TrimmedCurve(Section, fpar, lpar);
            theSection->Transform(Tf2);
            gp_Pnt FirstPoint = theSection->Value(theSection->FirstParameter());
            gp_Pnt LastPoint  = theSection->Value(theSection->LastParameter());
            Standard_Real UfirstOnSec, VfirstOnSec, UlastOnSec, VlastOnSec;
            ElSLib::Parameters(theSphere, FirstPoint, UfirstOnSec, VfirstOnSec);
            ElSLib::Parameters(theSphere, LastPoint, UlastOnSec, VlastOnSec);
            if (VfirstOnSec < VlastOnSec)
            {
              f = VfirstOnSec;
              l = VlastOnSec;
            }
            else
            {
	      // L'orientation parametrique est inversee
              f = VlastOnSec;
              l = VfirstOnSec;
	      isUReversed = Standard_True;
	    }

	    if ( (f >= -M_PI/2) && (l <= M_PI/2)) {
	      Ok = Standard_True;
	      myExchUV = Standard_True;
	      UFirst = f;
	      ULast  = l;
	    }
	    else { // On restaure ce qu'il faut
	      isUReversed = Standard_False;
	    }
	  }
	  else if (IsGoodSide) {         
	    // (2.1.b) Tore
	    gp_Ax3 AxisOfTore(Centre, DN, DC);
	    S = new (Geom_ToroidalSurface) (AxisOfTore, 
		   Radius , C.Radius());
	
	    // Pour les tores on ne peut pas controler le parametre
            // V (donc U car  myExchUV = Standard_True)
            // Il faut donc modifier UFirst, ULast...
	    Handle(Geom_Circle) Iso;
	    Iso =  Handle(Geom_Circle)::DownCast(S->UIso(0.));
	    gp_Ax2 axeiso;
	    axeiso = Iso->Circ().Position();
	      
	    if (C.Position().Direction().
		IsOpposite(axeiso.Direction(), 0.1) ) {
	      Standard_Real f, l;
	      // L'orientation parametrique est inversee
	      l = 2*M_PI - UFirst;
	      f = 2*M_PI - ULast;
	      UFirst = f;
	      ULast  = l;
	      isUReversed = Standard_True;
	    }
	    // On calcul le "glissement" parametrique.
	    Standard_Real rot; 
	    rot = C.Position().XDirection().AngleWithRef
	      (axeiso.XDirection(), axeiso.Direction());
	    UFirst -= rot;
	    ULast  -= rot;

	    myExchUV = Standard_True;
            // Attention l'arete de couture dans le cas periodique 
            // n'est peut etre pas a la bonne place...
            if (isUPeriodic && Abs(UFirst)>Precision::PConfusion()) 
	      isUPeriodic = Standard_False; //Pour trimmer la surface...
	    Ok = Standard_True;
	  }
	}
	else {
	  SError = 0.;
	}
      }
      // (2.2) Cone / Cylindre
      if ((SectionType == GeomAbs_Line) && IsTrsf)  {
	gp_Lin L =  AC.Line();
	L.Transform(Tf2);
	gp_Vec DL;
	DL.SetXYZ(L.Direction().XYZ());
	levier = Max(Abs(AC.FirstParameter()), AC.LastParameter());
	// si la line est ortogonale au cercle de rotation  
	SError = error + levier * Abs(DL.Dot(DP));
	if (SError <= Tol) {
          Standard_Boolean reverse;
	  gp_Lin Dir(Centre, DN);
	  Standard_Real aux;
	  aux = DL.Dot(DN);
	  reverse = (aux < 0);  // On choisit ici le sens de parametrisation
	    
          // Calcul du centre du vecteur supportant la "XDirection"
	  gp_Pnt CentreOfSurf;
	  gp_Vec O1O2(Centre, L.Location()), trans;
	  trans = DN;
	  trans *= DN.Dot(O1O2);
	  CentreOfSurf = Centre.Translated(trans);
          DS.SetXYZ(L.Location().XYZ() - CentreOfSurf.XYZ());

	  error = SError;
	  error += (DL.XYZ()).CrossMagnitude(DN.XYZ())*levier;
	  if (error <= Tol) {
	    // (2.2.a) Cylindre
            // si la line est orthogonale au plan de rotation
	    SError = error;
	    gp_Ax3 Axis(CentreOfSurf, Dir.Direction(), DS);
	    S = new (Geom_CylindricalSurface) 
	            (Axis, L.Distance(CentreOfSurf));
	    Ok = Standard_True;
            myExchUV = Standard_True;
	  }
	  else {
	    // On evalue l'angle du cone
	    Standard_Real Angle = Abs(Dir.Angle(L));
	    if (Angle > M_PI/2) Angle = M_PI -Angle;
            if (reverse) Angle = -Angle;
	    aux = DS.Dot(DL);
	    if (aux < 0) {
	      Angle = - Angle;
	    }
	    if (Abs(Abs(Angle) - M_PI/2) > 0.01) {
	      // (2.2.b) Cone
	      // si les 2 droites ne sont pas orthogonales
	      Standard_Real Radius = CentreOfSurf.Distance(L.Location());
	      gp_Ax3 Axis(CentreOfSurf, Dir.Direction(), DS);
	      S = new (Geom_ConicalSurface) 
	            (Axis, Angle, Radius);
	      myExchUV = Standard_True;
	      Ok = Standard_True;
	    }
	    else {
	      // On n'as pas conclue, on remet l'erreur a 0.
	      SError = 0.;
	    }
	  }
	  if (Ok && reverse) {
	    // On reverse le parametre
	    Standard_Real uf, ul;
	    Handle(Geom_Line) CL = new (Geom_Line)(L);
	    uf = CL->ReversedParameter(ULast);
	    ul = CL->ReversedParameter(UFirst);
	    UFirst = uf;
	    ULast = ul;
	    isUReversed = Standard_True;
	  }
	}
	else SError = 0.;
      }
  
      // (2.3) Revolution
      if (!Ok) {
	if (IsTrsf) { 
	  Section->Transform(Tf2);
	  gp_Ax1 Axis (Centre, DN);
	  S = new (Geom_SurfaceOfRevolution)
	    (Section, Axis);
	  myExchUV = Standard_True;
	  SError = 0.;
	  Ok = Standard_True;
	}
      }
    }
  }


  if (Ok) { // On trimme la surface
    if (myExchUV) {
      Standard_Boolean b;
      b = isUPeriodic; isUPeriodic = isVPeriodic;  isVPeriodic = b;
      Standard_Real r;
      r = UFirst; UFirst = VFirst; VFirst = r;
      r = ULast; ULast = VLast; VLast = r;
    }

    if (!isUPeriodic && !isVPeriodic)
      mySurface = new (Geom_RectangularTrimmedSurface)
	(S, UFirst, ULast, VFirst, VLast);
    else if (isUPeriodic) {
      if (isVPeriodic) mySurface = S;
      else mySurface = new (Geom_RectangularTrimmedSurface)
	(S, VFirst, VLast, Standard_False);
    }
    else
      mySurface = new (Geom_RectangularTrimmedSurface)
	(S,UFirst, ULast, Standard_True);

#ifdef OCCT_DEBUG
  if (isUPeriodic && !mySurface->IsUPeriodic()) 
    cout<<"Pb de periodicite en U" << endl;
  if (isUPeriodic && !mySurface->IsUClosed())
    cout<<"Pb de fermeture en U" << endl;
  if (isVPeriodic && !mySurface->IsVPeriodic()) 
    cout << "Pb de periodicite en V" << endl;
  if (isVPeriodic && !mySurface->IsVClosed())
    cout<<"Pb de fermeture en V" << endl;
#endif
  }


  return Ok;
}

//===============================================================
// Function : IsDone
// Purpose :
//===============================================================
 Standard_Boolean GeomFill_Sweep::IsDone() const
{
  return done;
}

//===============================================================
// Function :ErrorOnSurface
// Purpose :
//===============================================================
 Standard_Real GeomFill_Sweep::ErrorOnSurface() const
{
  return SError;
}

//===============================================================
// Function ::ErrorOnRestriction
// Purpose :
//===============================================================
 void GeomFill_Sweep::ErrorOnRestriction(const Standard_Boolean IsFirst,
					 Standard_Real& UError,
					 Standard_Real& VError) const
{
  Standard_Integer ind;
  if (IsFirst) ind=1;
  else ind = myCurve2d->Length();

  UError =  CError->Value(1, ind);
  VError =  CError->Value(2, ind);
}

//===============================================================
// Function :ErrorOnTrace
// Purpose :
//===============================================================
 void GeomFill_Sweep::ErrorOnTrace(const Standard_Integer IndexOfTrace,
				   Standard_Real& UError,
				   Standard_Real& VError) const
{
  Standard_Integer ind = IndexOfTrace+1;
  if (IndexOfTrace > myLoc->TraceNumber())
    throw Standard_OutOfRange(" GeomFill_Sweep::ErrorOnTrace");

  UError =  CError->Value(1, ind);
  VError =  CError->Value(2, ind);
}

//===============================================================
// Function :Surface
// Purpose :
//===============================================================
 Handle(Geom_Surface) GeomFill_Sweep::Surface() const
{
  return mySurface;
}

//===============================================================
// Function ::Restriction
// Purpose :
//===============================================================
 Handle(Geom2d_Curve) GeomFill_Sweep::Restriction(const Standard_Boolean IsFirst) const
{
  if (IsFirst)
    return  myCurve2d->Value(1);
  return  myCurve2d->Value(myCurve2d->Length());
 
}

//===============================================================
// Function :
// Purpose :
//===============================================================
 Standard_Integer GeomFill_Sweep::NumberOfTrace() const
{
  return myLoc->TraceNumber();
}

//===============================================================
// Function : 
// Purpose :
//===============================================================
 Handle(Geom2d_Curve) 
     GeomFill_Sweep::Trace(const Standard_Integer IndexOfTrace) const
{
  Standard_Integer ind = IndexOfTrace+1;
  if (IndexOfTrace > myLoc->TraceNumber())
    throw Standard_OutOfRange(" GeomFill_Sweep::Trace");
  return  myCurve2d->Value(ind);  
}
