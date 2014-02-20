// Created on: 1994-11-04
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

//:n9 abv 16.02.99: PRO10107: check parameters on curve with PConfusion()!
//:o6 abv 18.02.99: r0301_db #63077: application of parameter Factor moved
//:p0 abv 19.02.99: management of 'done' flag improved
//:p3 abv 23.02.99: bm4_id_punch_d.stp #1313: shift of parameters on ellipse with R1 < R2
//    abv 09.02.99: S4136: eliminate using BRepAPI::Precision()

#include <StepToGeom_MakeTrimmedCurve.ixx>
#include <gp_Pnt.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Curve.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Circle.hxx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Parabola.hxx>
#include <StepGeom_Hyperbola.hxx>
#include <StepGeom_TrimmingSelect.hxx>
#include <StepGeom_HArray1OfTrimmingSelect.hxx>
#include <StepGeom_TrimmedCurve.hxx>
#include <StepToGeom_MakeTrimmedCurve.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <StepToGeom_MakeCurve.hxx>
//#include <GeomAPI_ProjectPointOnCurve.hxx>
//#include <BRepAPI.hxx>
#include <ElCLib.hxx>

#include <UnitsMethods.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <StepGeom_Axis2Placement3d.hxx>

// ----------------------------------------------------------------
// ExtractParameter
// ----------------------------------------------------------------
//:o6 abv 18 Feb 99: parameter Factor added
//:p3 abv 23 Feb 99: parameter Shift added

static Standard_Boolean  ExtractParameter
(const Handle(Geom_Curve) &  aGeomCurve,
 const Handle(StepGeom_HArray1OfTrimmingSelect) & TS,
 const Standard_Integer nbSel,
 const Standard_Integer MasterRep,
 const Standard_Real Factor,
 const Standard_Real Shift,
 Standard_Real & aParam)     
{
  Handle(StepGeom_CartesianPoint) aPoint;
  Handle(Geom_CartesianPoint) theGeomPnt;
  Standard_Integer i;
//:S4136  Standard_Real precBrep = BRepAPI::Precision();
  for ( i = 1 ; i <= nbSel ; i++) {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (MasterRep == 2 && theSel.CaseMember() > 0) {
      aParam = Shift + Factor * theSel.ParameterValue();
      return Standard_True;
    }
    else if (MasterRep == 1 && theSel.CaseNumber() > 0) {
      aPoint = theSel.CartesianPoint();
	  StepToGeom_MakeCartesianPoint::Convert(aPoint,theGeomPnt);
      gp_Pnt thegpPnt = theGeomPnt->Pnt();
      
      //:S4136: use advanced algorithm
      ShapeAnalysis_Curve sac;
      gp_Pnt p;
      sac.Project ( aGeomCurve, thegpPnt, Precision::Confusion(), p, aParam );
/* //:S4136
      //Trim == natural boundary ?
      if(aGeomCurve->IsKind(STANDARD_TYPE(Geom_BoundedCurve))) {
	Standard_Real frstPar = aGeomCurve->FirstParameter();
	Standard_Real lstPar = aGeomCurve->LastParameter();
	gp_Pnt frstPnt = aGeomCurve->Value(frstPar);
	gp_Pnt lstPnt = aGeomCurve->Value(lstPar);
	if(frstPnt.IsEqual(thegpPnt,precBrep)) {
	  aParam = frstPar;
	  return Standard_True;
	}
	if(lstPnt.IsEqual(thegpPnt,precBrep)) {
	  aParam = lstPar;
	  return Standard_True;
	}
      }
      // Project Point On Curve
      GeomAPI_ProjectPointOnCurve PPOC(thegpPnt, aGeomCurve);
      if (PPOC.NbPoints() == 0) {
	return Standard_False;
      }
      aParam = PPOC.LowerDistanceParameter();
*/
      return Standard_True;
    }
  }
// if the MasterRepresentation is unspecified:
// if a ParameterValue exists, it is prefered

  for ( i = 1 ; i <= nbSel ; i++) {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (theSel.CaseMember() > 0) {
      aParam = Shift + Factor * theSel.ParameterValue();
      
      return Standard_True;
    }
  }
// if no ParameterValue exists, it is created from the CartesianPointValue

  for ( i = 1 ; i <= nbSel ; i++) {
    StepGeom_TrimmingSelect theSel = TS->Value(i);
    if (theSel.CaseNumber() > 0) {
      aPoint = theSel.CartesianPoint();
	  StepToGeom_MakeCartesianPoint::Convert(aPoint,theGeomPnt);
      gp_Pnt thegpPnt = theGeomPnt->Pnt();
      // Project Point On Curve
      ShapeAnalysis_Curve sac;
      gp_Pnt p;
      sac.Project ( aGeomCurve, thegpPnt, Precision::Confusion(), p, aParam );
/*
      GeomAPI_ProjectPointOnCurve PPOC(thegpPnt, aGeomCurve);
      if (PPOC.NbPoints() == 0) {
	return Standard_False;
      }
      aParam = PPOC.LowerDistanceParameter();
*/
      return Standard_True;
    }
  }
  return Standard_False;  // I suppose
}


//=============================================================================
// Creation d' une Trimmed Curve de Geom a partir d' une Trimmed Curve de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeTrimmedCurve::Convert (const Handle(StepGeom_TrimmedCurve)& SC, Handle(Geom_TrimmedCurve)& CC)
{
  const Handle(StepGeom_Curve) theSTEPCurve = SC->BasisCurve();
  Handle(Geom_Curve) theCurve;
  if (!StepToGeom_MakeCurve::Convert(theSTEPCurve,theCurve))
    return Standard_False;

  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel1 = SC->Trim1();
  const Handle(StepGeom_HArray1OfTrimmingSelect)& theTrimSel2 = SC->Trim2();
  const Standard_Integer nbSel1 = SC->NbTrim1();
  const Standard_Integer nbSel2 = SC->NbTrim2();

  Standard_Integer MasterRep;
  switch (SC->MasterRepresentation())
  {
    case StepGeom_tpCartesian: MasterRep = 1; break;
	case StepGeom_tpParameter: MasterRep = 2; break;
	default: MasterRep = 0;
  }

  //gka 18.02.04 analysis for case when MasterRep = .Unspecified 
  //and parameters are specified as CARTESIAN_POINT
  Standard_Boolean isPoint = Standard_False;
  if(MasterRep == 0 || (MasterRep == 2 && nbSel1 >1 && nbSel2 > 1)) {
    Standard_Integer ii;
    for(ii = 1; ii <= nbSel1; ii++)
    {
      if (!(theTrimSel1->Value(ii).CartesianPoint().IsNull()))
      {
        for(ii = 1; ii <= nbSel2; ii++)
        {
          if (!(theTrimSel2->Value(ii).CartesianPoint().IsNull()))
          {
            isPoint = Standard_True;
            break;
          }
        }
        break;
      }
	}
  }
  
  //:o6 abv 18 Feb 99: computation of factor moved 
  Standard_Real fact = 1., shift = 0.;
  if (theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Line))) {
    const Handle(StepGeom_Line) theLine = 
      Handle(StepGeom_Line)::DownCast(theSTEPCurve);
    fact = theLine->Dir()->Magnitude() * UnitsMethods::LengthFactor();
  }
  else if (theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Circle)) ||
           theSTEPCurve->IsKind(STANDARD_TYPE(StepGeom_Ellipse))) {
//    if (trim1 > 2.1*M_PI || trim2 > 2.1*M_PI) fact = M_PI / 180.;
    fact = UnitsMethods::PlaneAngleFactor();
    //:p3 abv 23 Feb 99: shift on pi/2 on ellipse with R1 < R2
    const Handle(StepGeom_Ellipse) ellipse = Handle(StepGeom_Ellipse)::DownCast(theSTEPCurve);
    if ( !ellipse.IsNull() && ellipse->SemiAxis1() - ellipse->SemiAxis2() < 0. )
      shift = 0.5 * M_PI;

    // skl 04.02.2002 for OCC133: we can not make TrimmedCurve if
    // there is no X-direction in StepGeom_Axis2Placement3d
    const Handle(StepGeom_Conic) conic = Handle(StepGeom_Conic)::DownCast(theSTEPCurve);
    // CKY 6-FEB-2004 for Airbus-MedialAxis : 
    // this restriction does not apply for trimming by POINTS 
    if(!conic.IsNull() && MasterRep != 1) {
      const StepGeom_Axis2Placement a2p = conic->Position();
      if(a2p.CaseNum(a2p.Value())==2) {
        if( !a2p.Axis2Placement3d()->HasRefDirection() ) {
          ////gka 18.02.04 analysis for case when MasterRep = .Unspecified 
          //and parameters are specified as CARTESIAN_POINT
          if(isPoint /*&& !MasterRep*/)
            MasterRep =1;
          else {
            if ( SC->SenseAgreement() ) 
              CC = new Geom_TrimmedCurve(theCurve, 0., 2.*M_PI, Standard_True);
            else 
              CC = new Geom_TrimmedCurve(theCurve, 2.*M_PI, 0., Standard_False);
            return Standard_True;
          }
        }
      }
    }
  }

  Standard_Real trim1 = 0.;
  Standard_Real trim2 = 0.;
  Handle(StepGeom_CartesianPoint) TrimCP1, TrimCP2;
  const Standard_Boolean FoundParam1 = ExtractParameter(theCurve, theTrimSel1, nbSel1, MasterRep, fact, shift, trim1);
  const Standard_Boolean FoundParam2 = ExtractParameter(theCurve, theTrimSel2, nbSel2, MasterRep, fact, shift, trim2);

  if (FoundParam1 && FoundParam2) {
    const Standard_Real cf = theCurve->FirstParameter();
    const Standard_Real cl = theCurve->LastParameter();
    //: abv 09.04.99: S4136: bm2_ug_t4-B.stp #70610: protect against OutOfRange
    if ( !theCurve->IsPeriodic() ) {
      if ( trim1 < cf ) trim1 = cf;
      else if ( trim1 > cl ) trim1 = cl;
      if ( trim2 < cf ) trim2 = cf;
      else if ( trim2 > cl ) trim2 = cl;
    }
    if (Abs(trim1 - trim2) < Precision::PConfusion()) {
      if (theCurve->IsPeriodic()) {
        ElCLib::AdjustPeriodic(cf,cl,Precision::PConfusion(),trim1,trim2);
      }
      else if (theCurve->IsClosed()) {
        if (Abs(trim1 - cf) < Precision::PConfusion()) {
          trim2 += cl;
        }
        else {
          trim1 -= cl;
        }
      }
      else {
//#ifdef DEBUG
//	cout << "Trimming Failed" << endl;
//#endif
        return Standard_False;
      }
    }
//  CKY 16-DEC-1997 : USA60035 le texte de Part42 parle de degres
//    mais des systemes ecrivent en radians. Exploiter UnitsMethods
//:o6    trim1 = trim1 * fact;
//:o6    trim2 = trim2 * fact;
    if ( SC->SenseAgreement() ) 
      CC = new Geom_TrimmedCurve(theCurve, trim1, trim2, Standard_True);
    else //:abv 29.09.00 PRO20362: reverse parameters in case of reversed curve
      CC = new Geom_TrimmedCurve(theCurve, trim2, trim1, Standard_False);
    return Standard_True;
  }
  return Standard_False;
}
