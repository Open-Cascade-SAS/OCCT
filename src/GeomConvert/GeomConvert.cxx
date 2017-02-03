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

// Jean-Claude Vauthier Novembre 1991
// Passage sur C1 Aout 1992 et ajout transformation Bezier->BSpline + Debug
// Modif JCV correction bug le 2/08/1993

#include <BSplCLib.hxx>
#include <Convert_CircleToBSplineCurve.hxx>
#include <Convert_ConicToBSplineCurve.hxx>
#include <Convert_EllipseToBSplineCurve.hxx>
#include <Convert_HyperbolaToBSplineCurve.hxx>
#include <Convert_ParabolaToBSplineCurve.hxx>
#include <ElCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomLProp.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Hermit.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <TColGeom_Array1OfCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : BSplineCurveBuilder
//purpose  : 
//=======================================================================
static Handle(Geom_BSplineCurve) BSplineCurveBuilder 
       (const Handle(Geom_Conic)&          TheConic,
	const Convert_ConicToBSplineCurve& Convert) 

{
  Handle(Geom_BSplineCurve) TheCurve;
  Standard_Integer NbPoles = Convert.NbPoles();
  Standard_Integer NbKnots = Convert.NbKnots();
  TColgp_Array1OfPnt      Poles   (1, NbPoles);
  TColStd_Array1OfReal    Weights (1, NbPoles);
  TColStd_Array1OfReal    Knots   (1, NbKnots);
  TColStd_Array1OfInteger Mults   (1, NbKnots);
  Standard_Integer i;
  gp_Pnt2d P2d;
  gp_Pnt   P3d;
  for (i = 1; i <= NbPoles; i++) {
    P2d = Convert.Pole (i);
    P3d.SetCoord (P2d.X(), P2d.Y(), 0.0);
    Poles (i) = P3d;
    Weights (i) = Convert.Weight (i);         
  }
  for (i = 1; i <= NbKnots; i++) {
    Knots (i) = Convert.Knot (i);
    Mults (i) = Convert.Multiplicity (i);
  }
  TheCurve = 
    new Geom_BSplineCurve (Poles, Weights, Knots, Mults, 
			   Convert.Degree(), Convert.IsPeriodic());
  gp_Trsf T;
  T.SetTransformation (TheConic->Position(), gp::XOY());
  Handle(Geom_BSplineCurve) Cres;
  Cres = Handle(Geom_BSplineCurve)::DownCast(TheCurve->Transformed (T));
  return Cres;
}



//=======================================================================
//function : SplitBSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) GeomConvert::SplitBSplineCurve 
  (const Handle(Geom_BSplineCurve)& C,
   const Standard_Integer               FromK1, 
   const Standard_Integer               ToK2,
   const Standard_Boolean               SameOrientation) 
{
  Standard_Integer TheFirst = C->FirstUKnotIndex ();
  Standard_Integer TheLast  = C->LastUKnotIndex ();
  if (FromK1 == ToK2)  throw Standard_DomainError();
  Standard_Integer FirstK = Min (FromK1, ToK2);
  Standard_Integer LastK  = Max (FromK1, ToK2);
  if (FirstK < TheFirst || LastK > TheLast) throw Standard_DomainError();

  Handle(Geom_BSplineCurve) C1
    = Handle(Geom_BSplineCurve)::DownCast(C->Copy ());

  C1->Segment( C->Knot(FirstK),C->Knot(LastK));

  if (C->IsPeriodic()) {
    if (!SameOrientation) C1->Reverse();
  }
  else {
    if (FromK1 > ToK2)    C1->Reverse();
  }
  return C1;
}


//=======================================================================
//function : SplitBSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) GeomConvert::SplitBSplineCurve 
  (const Handle(Geom_BSplineCurve)& C, 
   const Standard_Real              FromU1, 
   const Standard_Real              ToU2,
   const Standard_Real              , //ParametricTolerance,
   const Standard_Boolean            SameOrientation    ) 
{
  Standard_Real FirstU = Min( FromU1, ToU2);
  Standard_Real LastU  = Max( FromU1, ToU2);

  Handle (Geom_BSplineCurve) C1 
    = Handle(Geom_BSplineCurve)::DownCast(C->Copy());

  C1->Segment(FirstU, LastU);

  if (C->IsPeriodic()) {
     if (!SameOrientation) C1->Reverse();
   }
  else {
    if (FromU1 > ToU2)    C1->Reverse();
  }

  return C1;
}




//=======================================================================
//function : CurveToBSplineCurve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve)  GeomConvert::CurveToBSplineCurve 
  (const Handle(Geom_Curve)&          C,
   const Convert_ParameterisationType Parameterisation) 
{
  Handle (Geom_BSplineCurve) TheCurve;

  if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_Curve) Curv;
    Handle(Geom_TrimmedCurve) Ctrim = Handle(Geom_TrimmedCurve)::DownCast(C);
    Curv = Ctrim->BasisCurve();
    Standard_Real U1 = Ctrim->FirstParameter();
    Standard_Real U2 = Ctrim->LastParameter();

    // Si la courbe n'est pas vraiment restreinte, on ne risque pas 
    // le Raise dans le BS->Segment.
    if (!Curv->IsPeriodic()) {
      if (U1 < Curv->FirstParameter())
	U1 =  Curv->FirstParameter();
      if (U2 > Curv->LastParameter())
	U2 = Curv->LastParameter();
    }

    if (Curv->IsKind(STANDARD_TYPE(Geom_Line))) {
       gp_Pnt Pdeb = Ctrim->StartPoint();
       gp_Pnt Pfin = Ctrim->EndPoint();
       TColgp_Array1OfPnt Poles (1, 2);
       Poles (1) = Pdeb;
       Poles (2) = Pfin;
       TColStd_Array1OfReal Knots (1, 2);
       Knots (1) = Ctrim->FirstParameter ();
       Knots (2) = Ctrim->LastParameter  ();
       TColStd_Array1OfInteger Mults (1, 2);
       Mults (1) = 2;
       Mults (2) = 2;
       Standard_Integer Degree = 1;
       TheCurve = new Geom_BSplineCurve (Poles, Knots, Mults, Degree);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Circle))) {
      Handle(Geom_Circle) TheConic = Handle(Geom_Circle)::DownCast(Curv);
      gp_Circ2d C2d (gp::OX2d(), TheConic->Radius());
      if(Parameterisation != Convert_RationalC1) {
	Convert_CircleToBSplineCurve Convert (C2d,
					      U1, 
					      U2, 
					      Parameterisation);
	TheCurve = BSplineCurveBuilder (TheConic, Convert);
      }
      else {
	if(U2 - U1 < 6.) {
	  Convert_CircleToBSplineCurve Convert (C2d,
						U1, 
						U2, 
						Parameterisation);
	  TheCurve = BSplineCurveBuilder (TheConic, Convert);
	}
	else { // split circle to avoide numerical 
	       // overflow when U2 - U1 =~ 2*PI

	  Standard_Real Umed = (U1 + U2) * .5;
	  Convert_CircleToBSplineCurve Convert1 (C2d, 
						 U1, 
						 Umed, 
						 Parameterisation);

	  Handle (Geom_BSplineCurve) TheCurve1 = BSplineCurveBuilder (TheConic, Convert1);

	  Convert_CircleToBSplineCurve Convert2 (C2d, 
						 Umed, 
						 U2, 
						 Parameterisation);

	  Handle (Geom_BSplineCurve) TheCurve2 = BSplineCurveBuilder (TheConic, Convert2);

	  GeomConvert_CompCurveToBSplineCurve CCTBSpl(TheCurve1,
						      Parameterisation);

	  CCTBSpl.Add(TheCurve2, Precision::PConfusion(), Standard_True);

	  
	  TheCurve = CCTBSpl.BSplineCurve();
	}
      }

    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
      Handle(Geom_Ellipse) TheConic = Handle(Geom_Ellipse)::DownCast(Curv);
      gp_Elips2d E2d (gp::OX2d(),
		      TheConic->MajorRadius(),
		      TheConic->MinorRadius());
      if(Parameterisation != Convert_RationalC1) {
	Convert_EllipseToBSplineCurve Convert (E2d, 
					       U1, 
					       U2,
					       Parameterisation);
	TheCurve = BSplineCurveBuilder (TheConic, Convert);
      }
      else {
	if(U2 - U1 < 6.) {
	  Convert_EllipseToBSplineCurve Convert (E2d, 
						 U1, 
						 U2,
						 Parameterisation);
	  TheCurve = BSplineCurveBuilder (TheConic, Convert);
	}	
	else { // split ellipse to avoide numerical 
	       // overflow when U2 - U1 =~ 2*PI

	  Standard_Real Umed = (U1 + U2) * .5;
	  Convert_EllipseToBSplineCurve Convert1 (E2d, 
						  U1, 
						  Umed, 
						  Parameterisation);

	  Handle (Geom_BSplineCurve) TheCurve1 = BSplineCurveBuilder (TheConic, Convert1);

	  Convert_EllipseToBSplineCurve Convert2 (E2d, 
						Umed, 
						U2, 
						Parameterisation);

	  Handle (Geom_BSplineCurve) TheCurve2 = BSplineCurveBuilder (TheConic, Convert2);

	  GeomConvert_CompCurveToBSplineCurve CCTBSpl(TheCurve1,
						      Parameterisation);

	  CCTBSpl.Add(TheCurve2, Precision::PConfusion(), Standard_True);

	  
	  TheCurve = CCTBSpl.BSplineCurve();
	}
      }
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Hyperbola))) {
      Handle(Geom_Hyperbola) TheConic = Handle(Geom_Hyperbola)::DownCast(Curv);
      gp_Hypr2d H2d (gp::OX2d(), 
		     TheConic->MajorRadius(), TheConic->MinorRadius());
      Convert_HyperbolaToBSplineCurve Convert (H2d, U1, U2);
      TheCurve = BSplineCurveBuilder (TheConic, Convert);
    }

    else if (Curv->IsKind(STANDARD_TYPE(Geom_Parabola))) {
      Handle(Geom_Parabola) TheConic = Handle(Geom_Parabola)::DownCast(Curv);
      gp_Parab2d Prb2d (gp::OX2d(), TheConic->Focal());
      Convert_ParabolaToBSplineCurve Convert (Prb2d, U1, U2);
      TheCurve = BSplineCurveBuilder (TheConic, Convert);
    }

    else if (Curv->IsKind (STANDARD_TYPE(Geom_BezierCurve))) {
 
      Handle(Geom_BezierCurve) CBez 
	= Handle(Geom_BezierCurve)::DownCast(Curv->Copy());
      CBez->Segment (U1, U2);
      Standard_Integer NbPoles = CBez->NbPoles();
      Standard_Integer Degree  = CBez->Degree();
      TColgp_Array1OfPnt     Poles   (1, NbPoles);
      TColStd_Array1OfReal    Knots   (1, 2);
      TColStd_Array1OfInteger Mults   (1, 2);
      Knots (1) = 0.0;
      Knots (2) = 1.0;
      Mults (1) = Degree + 1;
      Mults (2) = Degree + 1;
      CBez->Poles (Poles);
      if (CBez->IsRational()) {    
        TColStd_Array1OfReal    Weights (1, NbPoles);
        CBez->Weights (Weights);
        TheCurve = 
	  new Geom_BSplineCurve (Poles, Weights, Knots, Mults, Degree);
      }
      else {
        TheCurve = 
	  new Geom_BSplineCurve (Poles, Knots, Mults, Degree);
      }
    }
    else if (Curv->IsKind (STANDARD_TYPE(Geom_BSplineCurve))) {
      TheCurve = Handle(Geom_BSplineCurve)::DownCast(Curv->Copy());
      //// modified by jgv, 14.01.05 for OCC7355 ////
      if (TheCurve->IsPeriodic())
	{
	  Standard_Real Uf = TheCurve->FirstParameter();
	  Standard_Real Ul = TheCurve->LastParameter();
	  ElCLib::AdjustPeriodic( Uf, Ul, Precision::Confusion(), U1, U2 );
	  if (Abs(U1 - Uf) <= Precision::Confusion() &&
	      Abs(U2 - Ul) <= Precision::Confusion())
	    TheCurve->SetNotPeriodic();
	}
      ///////////////////////////////////////////////
      TheCurve->Segment(U1,U2);
    }
    else if (Curv->IsKind (STANDARD_TYPE(Geom_OffsetCurve))) {
     Standard_Real Tol3d = 1.e-4;
     GeomAbs_Shape Order = GeomAbs_C2;
     Standard_Integer MaxSegments = 16, MaxDegree = 14; 
     GeomConvert_ApproxCurve ApprCOffs(C, Tol3d, Order, 
					 MaxSegments, MaxDegree);
     if (ApprCOffs.HasResult())
       TheCurve = ApprCOffs.Curve();
     else  throw Standard_ConstructionError();
    }
    else { throw Standard_DomainError("No such curve"); }
  }

  else { 
    if (C->IsKind(STANDARD_TYPE(Geom_Ellipse))) {
      Handle(Geom_Ellipse) TheConic= Handle(Geom_Ellipse)::DownCast(C);
      gp_Elips2d E2d (gp::OX2d(), 
		      TheConic->MajorRadius(), TheConic->MinorRadius());
/*      if (Parameterisation == Convert_TgtThetaOver2_1 ||
	  Parameterisation == Convert_TgtThetaOver2_2) {
	throw Standard_DomainError(); }
 
      else if ( Parameterisation == Convert_QuasiAngular) {
	Convert_EllipseToBSplineCurve Convert (E2d,
					       0.0e0,
					       2.0e0 * M_PI,
					       Parameterisation);

	TheCurve = BSplineCurveBuilder (TheConic, Convert);
	TheCurve->SetPeriodic();
      }
      else {*/
	Convert_EllipseToBSplineCurve Convert (E2d,
					      Parameterisation);
	TheCurve = BSplineCurveBuilder (TheConic, Convert);
        TheCurve->SetPeriodic(); // pour polynomial et quasi angular
//      }
    }
    
    else if (C->IsKind(STANDARD_TYPE(Geom_Circle))) {
      Handle(Geom_Circle) TheConic = Handle(Geom_Circle)::DownCast(C);
      gp_Circ2d C2d (gp::OX2d(), TheConic->Radius());
/*      if (Parameterisation == Convert_TgtThetaOver2_1 ||
	  Parameterisation == Convert_TgtThetaOver2_2) {
	throw Standard_DomainError(); }
 
      else if ( Parameterisation == Convert_QuasiAngular) {
	Convert_CircleToBSplineCurve Convert (C2d,
					      0.0e0,
					      2.0e0 * M_PI,
					      Parameterisation);
	
	TheCurve = BSplineCurveBuilder (TheConic, Convert);				     
      }
      else {*/
	Convert_CircleToBSplineCurve Convert (C2d,
					      Parameterisation);
	TheCurve = BSplineCurveBuilder (TheConic, Convert);
	TheCurve->SetPeriodic();
//      }
    }
    
    else if (C->IsKind (STANDARD_TYPE(Geom_BezierCurve))) {
      Handle(Geom_BezierCurve) CBez= Handle(Geom_BezierCurve)::DownCast(C);
      Standard_Integer NbPoles = CBez->NbPoles();
      Standard_Integer Degree  = CBez->Degree();
      TColgp_Array1OfPnt     Poles   (1, NbPoles);
      TColStd_Array1OfReal    Knots   (1, 2);
      TColStd_Array1OfInteger Mults   (1, 2);
      Knots (1) = 0.0;
      Knots (2) = 1.0;
      Mults (1) = Degree + 1;
      Mults (2) = Degree + 1;
      CBez->Poles (Poles);
      if (CBez->IsRational()) {    
	TColStd_Array1OfReal    Weights (1, NbPoles);
	CBez->Weights (Weights);
	TheCurve = 
	  new Geom_BSplineCurve (Poles, Weights, Knots, Mults, Degree);
      }
      else {
	TheCurve = new Geom_BSplineCurve (Poles, Knots, Mults, Degree);
      }
    }
    
    else if (C->IsKind (STANDARD_TYPE(Geom_BSplineCurve))) {
      TheCurve = Handle(Geom_BSplineCurve)::DownCast(C->Copy());
    }

    else if (C->IsKind (STANDARD_TYPE(Geom_OffsetCurve))) {
     Standard_Real Tol3d = 1.e-4;
     GeomAbs_Shape Order = GeomAbs_C2;
     Standard_Integer MaxSegments = 16, MaxDegree = 14; 
     GeomConvert_ApproxCurve ApprCOffs(C, Tol3d, Order, 
					 MaxSegments, MaxDegree);
     if (ApprCOffs.HasResult())
       TheCurve = ApprCOffs.Curve();
     else  throw Standard_ConstructionError();
   }
    else { throw Standard_DomainError("No such curve"); }
  }
  
  return TheCurve;
}


//=======================================================================
//class : law_evaluator
//purpose  : usefull to estimate the value of a function
//=======================================================================

class GeomConvert_law_evaluator : public BSplCLib_EvaluatorFunction
{

public:

  GeomConvert_law_evaluator (const Handle(Geom2d_BSplineCurve)& theAncore)
  : myAncore (theAncore) {}

  virtual void Evaluate (const Standard_Integer theDerivativeRequest,
                         const Standard_Real*   theStartEnd,
                         const Standard_Real    theParameter,
                         Standard_Real&         theResult,
                         Standard_Integer&      theErrorCode) const
  {
    theErrorCode = 0;
    if (!myAncore.IsNull() &&
        theParameter >= theStartEnd[0] &&
        theParameter <= theStartEnd[1] && 
        theDerivativeRequest == 0)
    {
      gp_Pnt2d aPoint;
      myAncore->D0 (theParameter, aPoint);
      theResult = aPoint.Coord(2);
    }    
    else 
      theErrorCode = 1;
  }

private:

  Handle(Geom2d_BSplineCurve) myAncore;

};

//=======================================================================
//function : MultNumandDenom
//purpose  : Multiply two BSpline curves to make one
//=======================================================================


static Handle(Geom_BSplineCurve) MultNumandDenom(const Handle(Geom2d_BSplineCurve)& a ,
						 const Handle(Geom_BSplineCurve)&   BS )
     
{ TColStd_Array1OfReal               aKnots(1,a->NbKnots());
  TColStd_Array1OfReal               BSKnots(1,BS->NbKnots());
  TColStd_Array1OfReal               BSFlatKnots(1,BS->NbPoles()+BS->Degree()+1);
  TColStd_Array1OfReal               BSWeights(1,BS->NbPoles()); 
  TColStd_Array1OfInteger            aMults(1,a->NbKnots());
  TColStd_Array1OfInteger            BSMults(1,BS->NbKnots());
  TColgp_Array1OfPnt2d               aPoles(1,a->NbPoles());
  TColgp_Array1OfPnt                 BSPoles(1,BS->NbPoles());
  Handle(Geom_BSplineCurve)          res;
  Handle(TColStd_HArray1OfReal)      resKnots;
  Handle(TColStd_HArray1OfInteger)   resMults; 
  Standard_Real                      start_value,end_value;
  Standard_Real                      tolerance=Precision::PConfusion();
  Standard_Integer                   resNbPoles,degree,
                                     ii,jj,
				     aStatus;
  
  BS->Knots(BSKnots);            //storage of the two BSpline 
  BS->Multiplicities(BSMults);   //features
  BS->Poles(BSPoles);
  BS->Weights(BSWeights);
  BS->KnotSequence(BSFlatKnots);
  start_value = BSKnots(1);
  end_value = BSKnots(BS->NbKnots());
  if ((end_value - start_value)/5 < tolerance) 
    tolerance = (end_value - start_value)/5;

  a->Knots(aKnots);
  a->Poles(aPoles);
  a->Multiplicities(aMults);
  BSplCLib::Reparametrize(BS->FirstParameter(),BS->LastParameter(),aKnots);
  Handle(Geom2d_BSplineCurve) anAncore = new Geom2d_BSplineCurve (aPoles, aKnots, aMults, a->Degree());

  BSplCLib::MergeBSplineKnots(tolerance,start_value,end_value, //merge of the knots
			      a->Degree(),aKnots,aMults,
			      BS->Degree(),BSKnots,BSMults,
			      resNbPoles,resKnots,resMults);
  degree=BS->Degree()+a->Degree();
  TColgp_Array1OfPnt resNumPoles(1,resNbPoles);
  TColStd_Array1OfReal resDenPoles(1,resNbPoles);
  TColgp_Array1OfPnt resPoles(1,resNbPoles);
  TColStd_Array1OfReal resFlatKnots(1,resNbPoles+degree+1);
  BSplCLib::KnotSequence(resKnots->Array1(),resMults->Array1(),resFlatKnots);
  for (ii=1;ii<=BS->NbPoles();ii++)
    for (jj=1;jj<=3;jj++)
      BSPoles(ii).SetCoord(jj,BSPoles(ii).Coord(jj)*BSWeights(ii));
//POP pour WNT
  GeomConvert_law_evaluator ev (anAncore);

  BSplCLib::FunctionMultiply(ev,                             
			     BS->Degree(),
			     BSFlatKnots,
			     BSPoles,
			     resFlatKnots,
			     degree,
			     resNumPoles,
			     aStatus);

  BSplCLib::FunctionMultiply(ev,
			     BS->Degree(),
			     BSFlatKnots,
			     BSWeights,
			     resFlatKnots,
			     degree,
			     resDenPoles,
			     aStatus);
  for (ii=1;ii<=resNbPoles;ii++)
    for(jj=1;jj<=3;jj++) 
      resPoles(ii).SetCoord(jj,resNumPoles(ii).Coord(jj)/resDenPoles(ii));
  res = new Geom_BSplineCurve(resPoles,resDenPoles,resKnots->Array1(),resMults->Array1(),degree);
  return res;
}

//=======================================================================
//function : Pretreatment 
//purpose  : Put the two first and two last weigths at one if they are 
//           equal
//=======================================================================

static void Pretreatment(TColGeom_Array1OfBSplineCurve& tab)

{Standard_Integer i,j;
 Standard_Real a;

 for (i=0;i<=(tab.Length()-1);i++){
   if (tab(i)->IsRational()) {
     a=tab(i)->Weight(1);                                 
     if ((tab(i)->Weight(2)==a)&&
	 (tab(i)->Weight(tab(i)->NbPoles()-1)==a)&&
	 (tab(i)->Weight(tab(i)->NbPoles())==a))
       
       for (j=1;j<=tab(i)->NbPoles();j++)
	 tab(i)->SetWeight(j,tab(i)->Weight(j)/a);
   }
 }
}
	 
//=======================================================================
//function : NeedToBeTreated
//purpose  : Say if the BSpline is rationnal and if the two first and two
//           last weigths are different
//=======================================================================    

static Standard_Boolean NeedToBeTreated(const Handle(Geom_BSplineCurve)& BS)

{
  TColStd_Array1OfReal  tabWeights(1,BS->NbPoles());
  if (BS->IsRational()) {
    BS->Weights(tabWeights);
    if ((BSplCLib::IsRational(tabWeights,1,BS->NbPoles()))&&
	((BS->Weight(1)<(1-Precision::Confusion()))||
	 (BS->Weight(1)>(1+Precision::Confusion()))||
	 (BS->Weight(2)<(1-Precision::Confusion()))||
	 (BS->Weight(2)>(1+Precision::Confusion()))||
	 (BS->Weight(BS->NbPoles()-1)<(1-Precision::Confusion()))||
	 (BS->Weight(BS->NbPoles()-1)>(1+Precision::Confusion()))||
	 (BS->Weight(BS->NbPoles())<(1-Precision::Confusion()))||
	 (BS->Weight(BS->NbPoles())>(1+Precision::Confusion()))))
      return Standard_True;
    else
      return Standard_False;
  }
  else 
    return Standard_False ;
 
}

//=======================================================================
//function : Need2DegRepara
//purpose  : in the case of wire closed G1 it says if you will to use a 
//           two degree reparametrisation to close it C1
//=======================================================================

static Standard_Boolean Need2DegRepara(const TColGeom_Array1OfBSplineCurve& tab)

{Standard_Integer        i;
 gp_Vec                  Vec1,Vec2;
 gp_Pnt                  Pint;
 Standard_Real           Rapport=1.0e0;

 for (i=0;i<=tab.Length()-2;i++){
   tab(i+1)->D1(tab(i+1)->FirstParameter(),Pint,Vec1);
   tab(i)->D1(tab(i)->LastParameter(),Pint,Vec2);
   Rapport=Rapport*Vec2.Magnitude()/Vec1.Magnitude();
 }
 if ((Rapport<=(1.0e0+Precision::Confusion()))&&(Rapport>=(1.0e0-Precision::Confusion())))
   return Standard_False;
 else
   return Standard_True;
}

//=======================================================================
//function : Indexmin
//purpose  : Give the index of the curve which has the lowest degree
//=======================================================================

static Standard_Integer Indexmin(const TColGeom_Array1OfBSplineCurve& tab)
{
  Standard_Integer i = 0, index = 0, degree = 0;
  
  degree=tab(0)->Degree();
  for (i=0;i<=tab.Length()-1;i++)
    if (tab(i)->Degree()<=degree){
      degree=tab(i)->Degree();
      index=i;
    }
  return index;
}

//=======================================================================
//function : NewTabClosedG1
//purpose  : Sort the array of BSplines to start at the nb_vertex_group0 index
//=======================================================================

static void ReorderArrayOfG1Curves(TColGeom_Array1OfBSplineCurve&    ArrayOfCurves, 
			   TColStd_Array1OfReal&             ArrayOfToler,
			   TColStd_Array1OfBoolean&          tabG1,
			   const Standard_Integer            StartIndex,
			   const Standard_Real               ClosedTolerance)

{Standard_Integer i;
 TColGeom_Array1OfBSplineCurve  ArraybisOfCurves(0,ArrayOfCurves.Length()-1);  //temporary
 TColStd_Array1OfReal           ArraybisOfToler(0,ArrayOfToler.Length()-1);    //arrays
 TColStd_Array1OfBoolean        tabbisG1(0,tabG1.Length()-1);

 for (i=0;i<=ArrayOfCurves.Length()-1;i++){
   if (i!=ArrayOfCurves.Length()-1){
     ArraybisOfCurves(i)=ArrayOfCurves(i);
     ArraybisOfToler(i)=ArrayOfToler(i);
     tabbisG1(i)=tabG1(i);
   }
   else
     ArraybisOfCurves(i)=ArrayOfCurves(i);
 }

 for (i=0;i<=(ArrayOfCurves.Length()-(StartIndex+2));i++){
   ArrayOfCurves(i)=ArraybisOfCurves(i+StartIndex+1);
   if (i!=(ArrayOfCurves.Length()-(StartIndex+2))){
     ArrayOfToler(i)=ArraybisOfToler(i+StartIndex+1);
     tabG1(i)=tabbisG1(i+StartIndex+1);
   }
 }

 ArrayOfToler(ArrayOfCurves.Length()-(StartIndex+2))=ClosedTolerance;
 tabG1(ArrayOfCurves.Length()-(StartIndex+2))=Standard_True;

 for (i=(ArrayOfCurves.Length()-(StartIndex+1));i<=(ArrayOfCurves.Length()-1);i++){
   if (i!=ArrayOfCurves.Length()-1){
     ArrayOfCurves(i)=ArraybisOfCurves(i-(ArrayOfCurves.Length()-(StartIndex+1)));
     ArrayOfToler(i)=ArraybisOfToler(i-(ArrayOfCurves.Length()-(StartIndex+1)));
     tabG1(i)=tabbisG1(i-(ArrayOfCurves.Length()-(StartIndex+1)));
   }
   else
     ArrayOfCurves(i)=ArraybisOfCurves(i-(ArrayOfCurves.Length()-(StartIndex+1)));
 }
}

//=======================================================================
//class   : reparameterise_evaluator
//purpose :
//=======================================================================

class GeomConvert_reparameterise_evaluator : public BSplCLib_EvaluatorFunction
{

public:

  GeomConvert_reparameterise_evaluator (const Standard_Real thePolynomialCoefficient[3])
  {
    memcpy (myPolynomialCoefficient, thePolynomialCoefficient, sizeof(myPolynomialCoefficient));
  }

  virtual void Evaluate (const Standard_Integer theDerivativeRequest,
                         const Standard_Real*   /*theStartEnd*/,
                         const Standard_Real    theParameter,
                         Standard_Real&         theResult,
                         Standard_Integer&      theErrorCode) const
  {
    theErrorCode = 0;
    PLib::EvalPolynomial (theParameter,
                          theDerivativeRequest,
                          2,
                          1,
                          *((Standard_Real* )myPolynomialCoefficient), // function really only read values from this array
                          theResult);
  }

private:

  Standard_Real myPolynomialCoefficient[3];

};

//=======================================================================
//function : ConcatG1
//purpose  : 
//=======================================================================

 void  GeomConvert::ConcatG1(TColGeom_Array1OfBSplineCurve&           ArrayOfCurves, 
			     const TColStd_Array1OfReal&              ArrayOfToler,
			     Handle(TColGeom_HArray1OfBSplineCurve) & ArrayOfConcatenated,
			     const Standard_Boolean                   ClosedG1Flag,
			     const Standard_Real                      ClosedTolerance) 

{Standard_Integer             nb_curve=ArrayOfCurves.Length(),
                              nb_vertexG1=0,
                              nb_group=0,
                              index=0,i,ii,j,jj,
                              indexmin,
                              nb_vertex_group0=0;
 Standard_Real                lambda,                      //G1 coefficient
                              First;
 Standard_Real PreLast = 0.;
 GeomAbs_Shape                Cont;
 gp_Vec                       Vec1,Vec2;                   //concecutive tangential vectors 
 gp_Pnt                       Pint;
 Handle(Geom_BSplineCurve)    Curve1,Curve2;                       
 TColStd_Array1OfBoolean      tabG1(0,nb_curve-2);         //array of the G1 continuity at the intersections
 TColStd_Array1OfReal         local_tolerance(0,
					      ArrayOfToler.Length()-1) ;
 
 for (i=0 ; i < ArrayOfToler.Length() ; i++) {
   local_tolerance(i) = ArrayOfToler(i) ;
 }
 for (i=0 ;i<nb_curve; i++){
   if (i >= 1){
     First=ArrayOfCurves(i)->FirstParameter();
     Cont = GeomLProp::Continuity(ArrayOfCurves(i-1),
				  ArrayOfCurves(i),
				  PreLast,First,
				  Standard_True,Standard_True);
     if (Cont<GeomAbs_C0)
       throw Standard_ConstructionError("GeomConvert curves not C0") ;                
     else{
       if (Cont>=GeomAbs_G1)
	 tabG1(i-1)=Standard_True;                   //True=G1 continuity
       else 
	 tabG1(i-1)=Standard_False;
     }
   }
   PreLast=ArrayOfCurves(i)->LastParameter();     
 }
 

 while (index<=nb_curve-1){ //determination of the Wire features
   nb_vertexG1=0;
   while(((index+nb_vertexG1)<=nb_curve-2)&&
	 (tabG1(index+nb_vertexG1)==Standard_True))
     nb_vertexG1++;
   nb_group++;
   if (index==0)
     nb_vertex_group0=nb_vertexG1;
   index=index+1+nb_vertexG1;
 }

 if ((ClosedG1Flag)&&(nb_group!=1)){                            //sort of the array
   nb_group--;
   ReorderArrayOfG1Curves(ArrayOfCurves,
		  local_tolerance,
		  tabG1,
		  nb_vertex_group0,
		  ClosedTolerance);
 }

 ArrayOfConcatenated = new TColGeom_HArray1OfBSplineCurve(0,nb_group-1);
 Standard_Boolean       fusion;

 index=0;
 Pretreatment(ArrayOfCurves);
 Standard_Real aPolynomialCoefficient[3];

 if ((nb_group==1) && (ClosedG1Flag)){ //treatment of a particular case
   indexmin=Indexmin(ArrayOfCurves);
   if (indexmin!=(ArrayOfCurves.Length()-1))
     ReorderArrayOfG1Curves(ArrayOfCurves,
		    local_tolerance,
		    tabG1,
		    indexmin,
		    ClosedTolerance);
   Curve2=ArrayOfCurves(0);
   for (j=1;j<=nb_curve-1;j++){                          //secondary loop inside each group
     Curve1=ArrayOfCurves(j);
     if ( (j==(nb_curve-1)) &&(Need2DegRepara(ArrayOfCurves))){ 
       Curve2->D1(Curve2->LastParameter(),Pint,Vec1);
       Curve1->D1(Curve1->FirstParameter(),Pint,Vec2);
       lambda=Vec2.Magnitude()/Vec1.Magnitude();
       TColStd_Array1OfReal KnotC1 (1, Curve1->NbKnots());
       Curve1->Knots(KnotC1);
       Curve1->D1(Curve1->LastParameter(),Pint,Vec2);
       ArrayOfCurves(0)->D1(ArrayOfCurves(0)->FirstParameter(),Pint,Vec1);
       Standard_Real lambda2=Vec1.Magnitude()/Vec2.Magnitude();
       Standard_Real tmax,a,b,c,
       umin=Curve1->FirstParameter(),umax=Curve1->LastParameter();
       tmax=2*lambda*(umax-umin)/(1+lambda*lambda2);
       a=(lambda*lambda2-1)/(2*lambda*tmax);
       aPolynomialCoefficient[2] = a;
       b=(1/lambda); 
       aPolynomialCoefficient[1] = b;
       c=umin;
       aPolynomialCoefficient[0] = c;
       TColStd_Array1OfReal  Curve1FlatKnots(1,Curve1->NbPoles()+Curve1->Degree()+1);
       TColStd_Array1OfInteger  KnotC1Mults(1,Curve1->NbKnots());
       Curve1->Multiplicities(KnotC1Mults);
       BSplCLib::KnotSequence(KnotC1,KnotC1Mults,Curve1FlatKnots);
       KnotC1(1)=0.0;
       for (ii=2;ii<=KnotC1.Length();ii++) {
//	 KnotC1(ii)=(-b+Abs(a)/a*Sqrt(b*b-4*a*(c-KnotC1(ii))))/(2*a);
	 KnotC1(ii)=(-b+Sqrt(b*b-4*a*(c-KnotC1(ii))))/(2*a); // ifv 17.05.00 buc60667
       }
       TColgp_Array1OfPnt  Curve1Poles(1,Curve1->NbPoles());
       Curve1->Poles(Curve1Poles);
       
       for (ii=1;ii<=Curve1->NbKnots();ii++)
	 KnotC1Mults(ii)=(Curve1->Degree()+KnotC1Mults(ii));
       
       TColStd_Array1OfReal FlatKnots(1,Curve1FlatKnots.Length()+(Curve1->Degree()*Curve1->NbKnots()));
       
       BSplCLib::KnotSequence(KnotC1,KnotC1Mults,FlatKnots);
       TColgp_Array1OfPnt  NewPoles(1,FlatKnots.Length()-(2*Curve1->Degree()+1));
       Standard_Integer      aStatus;
       TColStd_Array1OfReal Curve1Weights(1,Curve1->NbPoles());
       Curve1->Weights(Curve1Weights);
       for (ii=1;ii<=Curve1->NbPoles();ii++)
	 for (jj=1;jj<=3;jj++)
	   Curve1Poles(ii).SetCoord(jj,Curve1Poles(ii).Coord(jj)*Curve1Weights(ii));
//POP pour WNT
       GeomConvert_reparameterise_evaluator ev (aPolynomialCoefficient);
//       BSplCLib::FunctionReparameterise(reparameterise_evaluator,
       BSplCLib::FunctionReparameterise(ev,
					Curve1->Degree(),
					Curve1FlatKnots,
					Curve1Poles,
					FlatKnots,
					2*Curve1->Degree(),
					NewPoles,
					aStatus
					);
       TColStd_Array1OfReal NewWeights(1,FlatKnots.Length()-(2*Curve1->Degree()+1));
//       BSplCLib::FunctionReparameterise(reparameterise_evaluator,
       BSplCLib::FunctionReparameterise(ev,
					Curve1->Degree(),
					Curve1FlatKnots,
					Curve1Weights,
					FlatKnots,
					2*Curve1->Degree(),
					NewWeights,
					aStatus
					);
       for (ii=1;ii<=NewPoles.Length();ii++)
	 for (jj=1;jj<=3;jj++)
	   NewPoles(ii).SetCoord(jj,NewPoles(ii).Coord(jj)/NewWeights(ii));
       Curve1= new Geom_BSplineCurve(NewPoles,NewWeights,KnotC1,KnotC1Mults,2*Curve1->Degree());
     }
     GeomConvert_CompCurveToBSplineCurve C (Curve2);
     fusion=C.Add(Curve1,
		  local_tolerance(j-1));                //merge of two consecutive curves               
     if (fusion==Standard_False)
       throw Standard_ConstructionError("GeomConvert Concatenation Error") ;
     Curve2=C.BSplineCurve();
   }
   Curve2->SetPeriodic();      
   Curve2->RemoveKnot(Curve2->LastUKnotIndex(),
			 Curve2->Multiplicity(Curve2->LastUKnotIndex())-1,
			 Precision::Confusion());
   ArrayOfConcatenated->SetValue(0,Curve2);
 }
 
 else
   for (i=0;i<=nb_group-1;i++){                             //principal loop on each G1 continuity 
     nb_vertexG1=0;                                         //group
     
     while (((index+nb_vertexG1)<=nb_curve-2)&&(tabG1(index+nb_vertexG1)==Standard_True))
       nb_vertexG1++;
      
     for (j=index;j<=index+nb_vertexG1;j++){                //secondary loop inside each group
       Curve1=ArrayOfCurves(j);
       
       if (index==j)                                        //initialisation at the begining of the loop
	 ArrayOfConcatenated->SetValue(i,Curve1);
       else{
	 GeomConvert_CompCurveToBSplineCurve C (ArrayOfConcatenated->Value(i));
	 fusion=C.Add(Curve1,ArrayOfToler(j-1));            //merge of two consecutive curves               
	 if (fusion==Standard_False)
	   throw Standard_ConstructionError("GeomConvert Concatenation Error") ;
	 ArrayOfConcatenated->SetValue(i,C.BSplineCurve());
       }
     }
     index=index+1+nb_vertexG1;
   }
}  
//=======================================================================
//function : ConcatC1
//purpose  : 
//=======================================================================

void  GeomConvert::ConcatC1(TColGeom_Array1OfBSplineCurve&           ArrayOfCurves, 
			    const TColStd_Array1OfReal&              ArrayOfToler,
			    Handle(TColStd_HArray1OfInteger)&        ArrayOfIndices,
			    Handle(TColGeom_HArray1OfBSplineCurve)&  ArrayOfConcatenated,
			    const Standard_Boolean                   ClosedG1Flag,
			    const Standard_Real                      ClosedTolerance) 
{
  ConcatC1(ArrayOfCurves,
	   ArrayOfToler,
	   ArrayOfIndices,
	   ArrayOfConcatenated,
	   ClosedG1Flag,
	   ClosedTolerance,
	   Precision::Angular()) ;
}
//=======================================================================
//function : ConcatC1
//purpose  : 
//=======================================================================

void  GeomConvert::ConcatC1(TColGeom_Array1OfBSplineCurve&           ArrayOfCurves, 
			    const TColStd_Array1OfReal&              ArrayOfToler,
			    Handle(TColStd_HArray1OfInteger)&        ArrayOfIndices,
			    Handle(TColGeom_HArray1OfBSplineCurve)&  ArrayOfConcatenated,
			    const Standard_Boolean                   ClosedG1Flag,
			    const Standard_Real                      ClosedTolerance,
			    const Standard_Real                      AngularTolerance)

{Standard_Integer             nb_curve=ArrayOfCurves.Length(),
                              nb_vertexG1,
                              nb_group=0,
                              index=0,i,ii,j,jj,
                              indexmin,
                              nb_vertex_group0=0;
 Standard_Real                lambda,                      //G1 coefficient
                              First;
 Standard_Real PreLast = 0.;

 GeomAbs_Shape                Cont;
 gp_Vec                       Vec1,Vec2;                   //concecutive tangential vectors
 gp_Pnt                       Pint;
 Handle(Geom_BSplineCurve)    Curve1,Curve2;                       
 TColStd_Array1OfBoolean      tabG1(0,nb_curve-2);         //array of the G1 continuity at the intersections
 TColStd_Array1OfReal         local_tolerance(0,
					      ArrayOfToler.Length()-1) ;
 
 for (i=0 ; i < ArrayOfToler.Length() ; i++) {
   local_tolerance(i) = ArrayOfToler(i) ;
 }
 for (i=0 ;i<nb_curve; i++){
   if (i >= 1){
     First=ArrayOfCurves(i)->FirstParameter();
     Cont = GeomLProp::Continuity(ArrayOfCurves(i-1),
			       ArrayOfCurves(i),
			       PreLast,
			       First,
			       Standard_True,
			       Standard_True,
			       local_tolerance(i-1),
			       AngularTolerance);
     if (Cont<GeomAbs_C0)
       throw Standard_ConstructionError("GeomConvert curves not C0");
     else{
       if (Cont>=GeomAbs_G1)
	 tabG1(i-1)=Standard_True;                   //True=G1 continuity
       else 
	 tabG1(i-1)=Standard_False;
     }
   }
   PreLast=ArrayOfCurves(i)->LastParameter();     
 }
 

 while (index<=nb_curve-1){                                 //determination of the Wire features
   nb_vertexG1=0;
   while(((index+nb_vertexG1)<=nb_curve-2)&&(tabG1(index+nb_vertexG1)==Standard_True))
     nb_vertexG1++;
   nb_group++;
   if (index==0)
     nb_vertex_group0=nb_vertexG1;
   index=index+1+nb_vertexG1;
 }

 if ((ClosedG1Flag)&&(nb_group!=1)){                            //sort of the array
   nb_group--;
   ReorderArrayOfG1Curves(ArrayOfCurves,
			  local_tolerance,
			  tabG1,
			  nb_vertex_group0,
			  ClosedTolerance);
 }

 ArrayOfIndices = 
   new TColStd_HArray1OfInteger(0,nb_group);
 ArrayOfConcatenated = 
   new TColGeom_HArray1OfBSplineCurve(0,nb_group-1);

 Standard_Boolean       fusion;
 Standard_Integer       k=0;
 index=0;
 Pretreatment(ArrayOfCurves);
 Standard_Real aPolynomialCoefficient[3];

 if ((nb_group==1) && (ClosedG1Flag)){                       //treatment of a particular case
   ArrayOfIndices->SetValue(0,0);
   ArrayOfIndices->SetValue(1,0);
   indexmin=Indexmin(ArrayOfCurves);
   if (indexmin!=(ArrayOfCurves.Length()-1))
     ReorderArrayOfG1Curves(ArrayOfCurves,
			    local_tolerance,
			    tabG1,
			    indexmin,
			    ClosedTolerance);
   for (j=0;j<=nb_curve-1;j++){                          //secondary loop inside each group
     if (NeedToBeTreated(ArrayOfCurves(j)))
       Curve1=MultNumandDenom(Hermit::Solution(ArrayOfCurves(j)),ArrayOfCurves(j));
     else
       Curve1=ArrayOfCurves(j);
     
     if (j==0)                                           //initialisation at the begining of the loop
       Curve2=Curve1;
     else{
       if ( (j==(nb_curve-1)) &&(Need2DegRepara(ArrayOfCurves))){ 
	 Curve2->D1(Curve2->LastParameter(),Pint,Vec1);
	 Curve1->D1(Curve1->FirstParameter(),Pint,Vec2);
	 lambda=Vec2.Magnitude()/Vec1.Magnitude();
	 TColStd_Array1OfReal KnotC1 (1, Curve1->NbKnots());
	 Curve1->Knots(KnotC1);
	 Curve1->D1(Curve1->LastParameter(),Pint,Vec2);
	 ArrayOfCurves(0)->D1(ArrayOfCurves(0)->FirstParameter(),Pint,Vec1);
	 Standard_Real lambda2=Vec1.Magnitude()/Vec2.Magnitude();
	 Standard_Real tmax,a,b,c,
	 umin=Curve1->FirstParameter(),umax=Curve1->LastParameter();
	 tmax=2*lambda*(umax-umin)/(1+lambda*lambda2);
	 a=(lambda*lambda2-1)/(2*lambda*tmax);
	 aPolynomialCoefficient[2] = a;
	 b=(1/lambda); 
	 aPolynomialCoefficient[1] = b;
	 c=umin;
	 aPolynomialCoefficient[0] = c;
	 TColStd_Array1OfReal  Curve1FlatKnots(1,Curve1->NbPoles()+Curve1->Degree()+1);
	 TColStd_Array1OfInteger  KnotC1Mults(1,Curve1->NbKnots());
	 Curve1->Multiplicities(KnotC1Mults);
	 BSplCLib::KnotSequence(KnotC1,KnotC1Mults,Curve1FlatKnots);
	 KnotC1(1)=0.0;
	 for (ii=2;ii<=KnotC1.Length();ii++) {
//	   KnotC1(ii)=(-b+Abs(a)/a*Sqrt(b*b-4*a*(c-KnotC1(ii))))/(2*a);
	   KnotC1(ii)=(-b+Sqrt(b*b-4*a*(c-KnotC1(ii))))/(2*a); // ifv 17.05.00 buc60667
	 }
	 TColgp_Array1OfPnt  Curve1Poles(1,Curve1->NbPoles());
	 Curve1->Poles(Curve1Poles);
	 
	 for (ii=1;ii<=Curve1->NbKnots();ii++)
	   KnotC1Mults(ii)=(Curve1->Degree()+KnotC1Mults(ii));
	 
	 TColStd_Array1OfReal FlatKnots(1,Curve1FlatKnots.Length()+(Curve1->Degree()*Curve1->NbKnots()));
	 
	 BSplCLib::KnotSequence(KnotC1,KnotC1Mults,FlatKnots);
	 TColgp_Array1OfPnt  NewPoles(1,FlatKnots.Length()-(2*Curve1->Degree()+1));
	 Standard_Integer      aStatus;
	 TColStd_Array1OfReal Curve1Weights(1,Curve1->NbPoles());
	 Curve1->Weights(Curve1Weights);
	 for (ii=1;ii<=Curve1->NbPoles();ii++)
	   for (jj=1;jj<=3;jj++)
	     Curve1Poles(ii).SetCoord(jj,Curve1Poles(ii).Coord(jj)*Curve1Weights(ii));
//POP pour WNT
	 GeomConvert_reparameterise_evaluator ev (aPolynomialCoefficient);

	 BSplCLib::FunctionReparameterise(ev,
					  Curve1->Degree(),
					  Curve1FlatKnots,
					  Curve1Poles,
					  FlatKnots,
					  2*Curve1->Degree(),
					  NewPoles,
					  aStatus
					  );
	 TColStd_Array1OfReal NewWeights(1,FlatKnots.Length()-(2*Curve1->Degree()+1));

	 BSplCLib::FunctionReparameterise(ev,
					  Curve1->Degree(),
					  Curve1FlatKnots,
					  Curve1Weights,
					  FlatKnots,
					  2*Curve1->Degree(),
					  NewWeights,
					  aStatus
					  );
	 for (ii=1;ii<=NewPoles.Length();ii++)
	   for (jj=1;jj<=3;jj++)
	     NewPoles(ii).SetCoord(jj,NewPoles(ii).Coord(jj)/NewWeights(ii));
	 Curve1= new Geom_BSplineCurve(NewPoles,NewWeights,KnotC1,KnotC1Mults,2*Curve1->Degree());
       }
       GeomConvert_CompCurveToBSplineCurve C (Curve2);
       fusion=C.Add(Curve1,
		    local_tolerance(j-1));          //merge of two consecutive curves               
       if (fusion==Standard_False)
	 throw Standard_ConstructionError("GeomConvert Concatenation Error") ;
       Curve2=C.BSplineCurve();
     }
   }
   Curve2->SetPeriodic();                               //only one C1 curve
   Curve2->RemoveKnot(Curve2->LastUKnotIndex(),
			 Curve2->Multiplicity(Curve2->LastUKnotIndex())-1,
			 Precision::Confusion());
   ArrayOfConcatenated->SetValue(0,Curve2);
 }
 
 else
   for (i=0;i<=nb_group-1;i++){                             //principal loop on each G1 continuity 
     nb_vertexG1=0;                                         //group
      
     while (((index+nb_vertexG1)<=nb_curve-2)&&(tabG1(index+nb_vertexG1)==Standard_True))
       nb_vertexG1++;
      
     if ((!ClosedG1Flag)||(nb_group==1)){                        //filling of the array of index which are kept
       k++;
       ArrayOfIndices->SetValue(k-1,index);
       if (k==nb_group)
	 ArrayOfIndices->SetValue(k,0);
     }
     else{
       k++;
       ArrayOfIndices->SetValue(k-1,index+nb_vertex_group0+1);
       if (k==nb_group)
	 ArrayOfIndices->SetValue(k,nb_vertex_group0+1);
     }
      
     for (j=index;j<=index+nb_vertexG1;j++){                //secondary loop inside each group
       if (NeedToBeTreated(ArrayOfCurves(j)))
	 Curve1=MultNumandDenom(Hermit::Solution(ArrayOfCurves(j)),ArrayOfCurves(j));
       else
	 Curve1=ArrayOfCurves(j);
       
       if (index==j)                                      //initialisation at the begining of the loop
	 ArrayOfConcatenated->SetValue(i,Curve1);
       else
       {
         // Merge of two consecutive curves.
         GeomConvert_CompCurveToBSplineCurve C (ArrayOfConcatenated->Value(i));
         fusion=C.Add(Curve1, local_tolerance(j-1), Standard_True);
         if (fusion==Standard_False)
           throw Standard_ConstructionError("GeomConvert Concatenation Error");
         ArrayOfConcatenated->SetValue(i,C.BSplineCurve());
       }
     }
     index=index+1+nb_vertexG1;
   }
}
	   
//=======================================================================
//function : C0BSplineToC1BSplineCurve
//purpose  : 
//=======================================================================

void GeomConvert::C0BSplineToC1BSplineCurve(Handle(Geom_BSplineCurve)& BS,
				            const Standard_Real tolerance, 
					    const Standard_Real AngularTol)

{
  Standard_Boolean fusion;
  Handle(TColGeom_HArray1OfBSplineCurve) ArrayOfConcatenated; 
  //the array with the resulting curves
    
  GeomConvert::C0BSplineToArrayOfC1BSplineCurve(BS, ArrayOfConcatenated, 
						AngularTol, tolerance);
    
  GeomConvert_CompCurveToBSplineCurve C (ArrayOfConcatenated->Value(0));
  if (ArrayOfConcatenated->Length()>=2){
    Standard_Integer i;
    for (i=1;i<ArrayOfConcatenated->Length();i++){
      fusion=C.Add(ArrayOfConcatenated->Value(i),tolerance);
      if (fusion==Standard_False)
	throw Standard_ConstructionError("GeomConvert Concatenation Error") ;
    }
  }
  BS=C.BSplineCurve();
}

//=======================================================================
//function : C0BSplineToArrayOfC1BSplineCurve
//purpose  : 
//=======================================================================

void GeomConvert::C0BSplineToArrayOfC1BSplineCurve(
				const Handle(Geom_BSplineCurve) &        BS,
				Handle(TColGeom_HArray1OfBSplineCurve) & tabBS,
				const Standard_Real                      tolerance)
{
   C0BSplineToArrayOfC1BSplineCurve(BS,
				    tabBS,
				    Precision::Angular(),
				    tolerance) ;
}

//=======================================================================
//function : C0BSplineToArrayOfC1BSplineCurve
//purpose  : 
//=======================================================================

void GeomConvert::C0BSplineToArrayOfC1BSplineCurve(
				const Handle(Geom_BSplineCurve) &        BS,
				Handle(TColGeom_HArray1OfBSplineCurve) & tabBS,
				const Standard_Real                      AngularTolerance,
				const Standard_Real                      tolerance)

{TColStd_Array1OfInteger          BSMults(1,BS->NbKnots());
 TColStd_Array1OfReal             BSKnots(1,BS->NbKnots());
 Standard_Integer                 i,j,nbcurveC1=1;
 Standard_Real                    U1,U2;
 Standard_Boolean                 closed_flag= Standard_False ;
 gp_Pnt                           point;
 gp_Vec                           V1,V2;
// Standard_Boolean                 fusion;

 BS->Knots(BSKnots);
 BS->Multiplicities(BSMults);
 for (i=BS->FirstUKnotIndex() ;i<=(BS->LastUKnotIndex()-1);i++){                                 //give the number of C1 curves
   if (BSMults(i)==BS->Degree())
     nbcurveC1++;   
 }

 if (nbcurveC1>1){
   TColGeom_Array1OfBSplineCurve    ArrayOfCurves(0,nbcurveC1-1);
   TColStd_Array1OfReal             ArrayOfToler(0,nbcurveC1-2);
   
   for (i=0;i<=nbcurveC1-2;i++)                                      
     //filling of the array of tolerances  
     ArrayOfToler(i)=tolerance;                                      
   //with the variable tolerance
   U2=BS->FirstParameter() ;
   j=BS->FirstUKnotIndex() + 1;
   for (i=0;i<nbcurveC1;i++){                                        
     //filling of the array of curves
     U1=U2;                                                          
     //with the curves C1 segmented
     while (BSMults(j)<BS->Degree() && j < BS->LastUKnotIndex())
       j++;
     U2=BSKnots(j);
     j++;
     Handle(Geom_BSplineCurve) 
       BSbis=Handle(Geom_BSplineCurve)::DownCast(BS->Copy());
     BSbis->Segment(U1,U2);
     ArrayOfCurves(i)=BSbis;
   }
    
   Handle(TColStd_HArray1OfInteger) ArrayOfIndices;
     
   BS->D1(BS->FirstParameter(),point,V1);  
   BS->D1(BS->LastParameter(),point,V2);
    
   if ((BS->IsClosed())&&(V1.IsParallel(V2,AngularTolerance))){
     //check if the BSpline is closed G1
     closed_flag = Standard_True ;
   }
    
   GeomConvert::ConcatC1(ArrayOfCurves,
			 ArrayOfToler,
			 ArrayOfIndices,
			 tabBS,
			 closed_flag,
			 tolerance,
			 AngularTolerance);
 }
 else{
   tabBS = new TColGeom_HArray1OfBSplineCurve(0,0);
   tabBS->SetValue(0,BS);
 }
}  





