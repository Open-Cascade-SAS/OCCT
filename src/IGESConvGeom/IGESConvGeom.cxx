// Created on: 1994-09-01
// Created by: Christian CAILLET
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

// modif du 31/01/97 : mjm
// on commence par les SplineCurves.
// modif du 17/03/97 : mjm
// SplineSurfaces.
//%13 pdn 12.02.99: USA60293 avoid applying transformation twice

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>
#include <IGESConvGeom.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <PLib.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : IGESConvGeom::SplineCurveFromIGES
//purpose  : 
//=======================================================================
Standard_Integer  IGESConvGeom::SplineCurveFromIGES
  (const Handle(IGESGeom_SplineCurve)& st,
   const Standard_Real /*epscoef*/,  const Standard_Real epsgeom,
   Handle(Geom_BSplineCurve)& res)
{
  Standard_Integer returned = 0;

  // on recupere le degre
  Standard_Integer degree = st->SplineType();
  if (degree > 3) degree = 3;

  // on recupere le nombre de segments.
  Standard_Integer nbSegs  = st->NbSegments();  
  if (nbSegs < 1) return 5;            // FAIL : no segment

  Standard_Integer nbKnots = nbSegs+1;          

  // Array of multiplicities.  
  TColStd_Array1OfInteger multi(1, nbKnots); 
  multi.Init(degree);
  multi.SetValue(multi.Lower(), degree+1);
  multi.SetValue(multi.Upper(), degree+1);

  // Array of knots.  
  TColStd_Array1OfReal knots(1, nbKnots);                 
  TColStd_Array1OfReal delta(1, nbSegs);
  Standard_Integer i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i<= nbKnots; i++)
    knots.SetValue(i, st->BreakPoint(i));

  for (i = 1; i <= nbSegs; i++) 
    delta.SetValue(i, st->BreakPoint(i+1) - st->BreakPoint(i));

  TColgp_Array1OfPnt bspoles(1, nbSegs*degree+1);
  Standard_Integer ibspole = bspoles.Lower()-1;              // Bspole Index.
  // il faut reparametrer avant de passer dans PLib.
  // on est entre[0, T(i+1)-T(i)] et on veut [0,1]

  for (i = 1; i <= nbSegs; i++) {
    Standard_Real AX,BX,CX,DX,AY,BY,CY,DY,AZ,BZ,CZ,DZ;
    st->XCoordPolynomial(i, AX, BX, CX, DX);
    st->YCoordPolynomial(i, AY, BY, CY, DY);
    st->ZCoordPolynomial(i, AZ, BZ, CZ, DZ);
    if (st->NbDimensions() == 2 ) BZ=0.,CZ=0.,DZ=0.; 
    Standard_Real Di  = delta(i);
    Standard_Real Di2 = delta(i)*delta(i);
    Standard_Real Di3 = delta(i)*delta(i)*delta(i);

    TColgp_Array1OfPnt coeff(0, degree);
    switch (degree) {
    case 3 :
      coeff.SetValue(coeff.Lower()+3, gp_Pnt(DX*Di3, DY*Di3, DZ*Di3));
    case 2 :
      coeff.SetValue(coeff.Lower()+2, gp_Pnt(CX*Di2, CY*Di2, CZ*Di2));
    case 1 :
      coeff.SetValue(coeff.Lower()+1, gp_Pnt(BX*Di, BY*Di, BZ*Di));
      coeff.SetValue(coeff.Lower()+0, gp_Pnt(AX, AY, AZ));
      break;
    default:
      break;
    }


    TColgp_Array1OfPnt bzpoles(0, degree);
    PLib::CoefficientsPoles(coeff,PLib::NoWeights(),bzpoles,PLib::NoWeights());

    // C0 test.
    // Not to check the first pole of the first segment.
    if (ibspole > bspoles.Lower()) {  
      Standard_Integer bzlow = bzpoles.Lower();
      if (!(bspoles.Value(ibspole).IsEqual(bzpoles.Value(bzlow), epsgeom))) {
	returned = 1;
	// Medium point computing.
	bspoles.SetValue (ibspole, 
	   gp_Pnt((bspoles.Value(ibspole).X() + bzpoles.Value(bzlow).X())/2.,
		  (bspoles.Value(ibspole).Y() + bzpoles.Value(bzlow).Y())/2.,
		  (bspoles.Value(ibspole).Z() + bzpoles.Value(bzlow).Z())/2.));
      }
    }
    if (i == 1) bspoles.SetValue(++ibspole, bzpoles.Value(bzpoles.Lower()));

    for (Standard_Integer j = bzpoles.Lower()+1; j <= bzpoles.Upper(); j++) 
      bspoles.SetValue(++ibspole, bzpoles.Value(j));
  }
  if (ibspole != bspoles.Upper()) {   
    // Just to be sure.
    return 3;           // FAIL : Error during creation of control points
  }

  //  Building result taking into account transformation if any :
  //  ===========================================================
  
  //%13 pdn 12.02.99 USA60293
//  if (st->HasTransf()) {
//    gp_Trsf trsf;
//    Standard_Real epsilon = 1.E-04;
//    if (IGESData_ToolLocation::ConvertLocation
//	(epsilon,st->CompoundLocation(),trsf)) { 
//      for (Standard_Integer i = bspoles.Lower(); i <= bspoles.Upper(); i++) 
//	bspoles.SetValue(i, bspoles.Value(i).Transformed(trsf));
//    }
//    else
//      AddFail(st, "Transformation : not a similarity");
//  }
  res = new Geom_BSplineCurve (bspoles, knots, multi, degree);
//  GeomConvert_CompCurveToBSplineCurve CompCurve = 
//    GeomConvert_CompCurveToBSplineCurve(res);
//  res = CompCurve.BSplineCurve();
  return returned;
}



//=======================================================================
//function : IGESConvGeom::IncreaseCurveContinuity
//purpose  : 
//=======================================================================
Standard_Integer IGESConvGeom::IncreaseCurveContinuity (const Handle(Geom_BSplineCurve)& res,
                                                        const Standard_Real epsgeom,
                                                        const Standard_Integer continuity)
{
  if (continuity < 1) return continuity;
  Standard_Boolean isC1 = Standard_True, isC2 = Standard_True;
  Standard_Integer degree = res->Degree();

  
  Standard_Boolean isModified;
  do {
    isModified = Standard_False;
    for (Standard_Integer i = res->FirstUKnotIndex()+1; i < res->LastUKnotIndex(); i++) 
      if(degree - res->Multiplicity(i) < continuity) {
        if (continuity >= 2) {
          if (!res->RemoveKnot(i, degree-2, epsgeom)) {
            isC2 = Standard_False;
            Standard_Boolean locOK = res->RemoveKnot(i, degree-1, epsgeom);   // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = Standard_True;
        }
        else {
          Standard_Boolean locOK = res->RemoveKnot(i, degree-1, epsgeom);   // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  }
  while (isModified);
    
  if (!isC1) return 0;
  if (continuity >= 2 && !isC2) return 1;
  return continuity;
}

//=======================================================================
//function : IncreaseCurveContinuity
//purpose  : 
//=======================================================================

Standard_Integer IGESConvGeom::IncreaseCurveContinuity (const Handle(Geom2d_BSplineCurve)& res,
                                                        const Standard_Real epsgeom,
                                                        const Standard_Integer continuity)
{
  if (continuity < 1) return continuity;
  Standard_Boolean isC1 = Standard_True, isC2 = Standard_True;
  Standard_Integer degree = res->Degree();

  Standard_Boolean isModified;
  do {
    isModified = Standard_False;
    for (Standard_Integer i = res->FirstUKnotIndex()+1; i < res->LastUKnotIndex(); i++) 
      if(degree - res->Multiplicity(i) < continuity) {
        if (continuity >= 2) {
          if (!res->RemoveKnot(i, degree-2, epsgeom)) {
            isC2 = Standard_False;
            Standard_Boolean locOK = res->RemoveKnot(i, degree-1, epsgeom);   // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = Standard_True;
        }
        else {
          Standard_Boolean locOK = res->RemoveKnot(i, degree-1, epsgeom);   // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  }
  while (isModified);

  if (!isC1) return 0;
  if (continuity >= 2 && !isC2) return 1;
  return continuity;
}


//=======================================================================
//function : IGESConvGeom::SplineSurfaceFromIGES
//purpose  : 
//=======================================================================
Standard_Integer  IGESConvGeom::SplineSurfaceFromIGES
  (const Handle(IGESGeom_SplineSurface)& st,
   const Standard_Real /*epscoef*/,  const Standard_Real epsgeom,
   Handle(Geom_BSplineSurface)& res)
{
  Standard_Integer returned = 0;
  Standard_Integer degree = st->BoundaryType();
  if (degree > 3) degree = 3;
  Standard_Integer DegreeU = degree;
  Standard_Integer DegreeV = degree;
  
  Standard_Integer  NbUSeg = st->NbUSegments();  
  Standard_Integer  NbVSeg = st->NbVSegments();  
  
  if ((NbUSeg < 1) || (NbVSeg < 1)) return 5;
  
  //  Output BSpline knots & multiplicities arraies for U & V :
  //  =========================================================

  TColStd_Array1OfReal  UKnot(1,NbUSeg+1);
  TColStd_Array1OfReal  VKnot(1,NbVSeg+1);
  TColStd_Array1OfReal  deltaU(1,NbUSeg);
  TColStd_Array1OfReal  deltaV(1,NbVSeg);

  Standard_Integer i; // svv Jan 10 2000 : porting on DEC
  for (i=1; i <= NbUSeg+1; i++)
    UKnot.SetValue(i, st->UBreakPoint(i));

  for (i=1; i <= NbUSeg; i++)
    deltaU.SetValue(i, st->UBreakPoint(i+1)- st->UBreakPoint(i));

  for (i=1; i <= NbVSeg+1; i++)
    VKnot.SetValue(i, st->VBreakPoint(i));

  for (i=1; i <= NbVSeg; i++) 
    deltaV.SetValue(i, st->VBreakPoint(i+1)- st->VBreakPoint(i));

  TColStd_Array1OfInteger  UMult(1,NbUSeg+1); UMult.Init(DegreeU);
  UMult.SetValue(UMult.Lower(),DegreeU+1);
  UMult.SetValue(UMult.Upper(),DegreeU+1);

  TColStd_Array1OfInteger  VMult(1,NbVSeg+1); VMult.Init(DegreeV);
  VMult.SetValue(VMult.Lower(),DegreeV+1);
  VMult.SetValue(VMult.Upper(),DegreeV+1);
      
  
  //  Poles computing
  //  ===============
  
  Standard_Integer  NbUPoles  =  NbUSeg * DegreeU + 1;
  Standard_Integer  NbVPoles  =  NbVSeg * DegreeV + 1;  
  
  TColgp_Array2OfPnt  BsPole(1, NbUPoles, 1, NbVPoles);

  Standard_Integer  iBs, jBs, iBz, jBz;
  Standard_Boolean  wasC0 = Standard_True;
  
  //  Patch (1,1)
  //  ===========
  Standard_Integer USeg, VSeg, j;
  USeg = 1;
  VSeg = 1;
  
  Handle(TColStd_HArray1OfReal) XPoly = st->XPolynomial(USeg, VSeg);
  Handle(TColStd_HArray1OfReal) YPoly = st->YPolynomial(USeg, VSeg);
  Handle(TColStd_HArray1OfReal) ZPoly = st->ZPolynomial(USeg, VSeg);
  
  TColgp_Array2OfPnt Coef(1, DegreeU+1, 1, DegreeV+1);
  Standard_Real ParamU, ParamV;
  ParamU = 1.;
  for (i=1; i<=DegreeU+1; i++) {
    ParamV = 1.;
    for (j=1; j<=DegreeV+1; j++) {
      Standard_Integer PolyIndex = i + 4*(j-1);
      gp_Pnt aPoint(XPoly->Value(PolyIndex)*ParamU*ParamV, 
		    YPoly->Value(PolyIndex)*ParamU*ParamV, 
		    ZPoly->Value(PolyIndex)*ParamU*ParamV);
      Coef.SetValue(i, j, aPoint);
      ParamV = ParamV *deltaV(VSeg);
    }
    ParamU = ParamU * deltaU(USeg);
  }  
  TColgp_Array2OfPnt BzPole(1, DegreeU+1, 1, DegreeV+1);
  PLib::CoefficientsPoles(Coef,PLib::NoWeights2(),BzPole,PLib::NoWeights2());
  
  iBs = BsPole.LowerRow();
  jBs = BsPole.LowerCol();
  
  //  Making output BSpline poles array :
  for (iBz=BzPole.LowerRow(); iBz<=BzPole.UpperRow(); iBz++) {
    for (jBz=BzPole.LowerCol(); jBz<=BzPole.UpperCol(); jBz++)
      BsPole.SetValue(iBs, jBs++, BzPole.Value(iBz,jBz));
    jBs = BsPole.LowerCol();
    iBs++;
  }


  //  Patches (1<USeg<NbUSeg, 1)
  //  ==========================
  
  VSeg = 1;
  for (USeg=2; USeg<=NbUSeg; USeg++) {
    XPoly = st->XPolynomial(USeg, VSeg);
    YPoly = st->YPolynomial(USeg, VSeg);
    ZPoly = st->ZPolynomial(USeg, VSeg);
    ParamU = 1.;
    for (i=Coef.LowerRow(); i<=Coef.UpperRow(); i++) {
      ParamV = 1.;
      for (j=Coef.LowerCol(); j<=Coef.UpperCol(); j++) {
	Standard_Integer PolyIndex = i + 4*(j-1);
	gp_Pnt aPoint;
	aPoint.SetCoord(XPoly->Value(PolyIndex)*ParamU*ParamV, 
			YPoly->Value(PolyIndex)*ParamU*ParamV, 
			ZPoly->Value(PolyIndex)*ParamU*ParamV);
	Coef.SetValue(i, j, aPoint);
	ParamV = ParamV *deltaV(VSeg);
      }
      ParamU = ParamU * deltaU(USeg);
    }
    PLib::CoefficientsPoles(Coef,PLib::NoWeights2(),BzPole,PLib::NoWeights2());

    //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
    Standard_Integer  iBsPole = BsPole.LowerRow() + (USeg-1)*DegreeU;
    Standard_Integer  jBsPole = BsPole.LowerCol();
    iBz = BzPole.LowerRow();
    for (jBz=BzPole.LowerCol(); jBz<=BzPole.UpperCol(); jBz++) {
     if (!BzPole.Value(iBz,jBz).IsEqual(BsPole.Value(iBsPole,jBsPole), epsgeom)) {
	wasC0=Standard_False;
	gp_Pnt MidPoint;
	Standard_Real  XCoord = 
	  0.5 * (BzPole.Value(iBz,jBz).X() + BsPole.Value(iBsPole,jBsPole).X());
	Standard_Real  YCoord = 
	  0.5 * (BzPole.Value(iBz,jBz).Y() + BsPole.Value(iBsPole,jBsPole).Y());
	Standard_Real  ZCoord = 
	  0.5 * (BzPole.Value(iBz,jBz).Z() + BsPole.Value(iBsPole,jBsPole).Z());
	MidPoint.SetCoord(XCoord, YCoord, ZCoord);
	BsPole.SetValue(iBsPole, jBsPole++, MidPoint);
      }
      else {
	BsPole.SetValue(iBsPole, jBsPole++, BzPole.Value(iBz,jBz));
      }
    }
    
    //  Other poles (no check about C0) :
    iBsPole++;
    jBsPole = BsPole.LowerCol();
    for (iBz=BzPole.LowerRow()+1; iBz<=BzPole.UpperRow(); iBz++) {
      for (jBz=BzPole.LowerCol(); jBz<=BzPole.UpperCol(); jBz++)
	BsPole.SetValue(iBsPole, jBsPole++, BzPole.Value(iBz,jBz));
      iBsPole++;
      jBsPole = BsPole.LowerCol();
    }
  }
      
      
      
  //  Patches (1, 1<VSeg<NbVSeg)
  //  ==========================
  
  USeg = 1;
  for (VSeg=2; VSeg <= NbVSeg; VSeg++) {
    XPoly = st->XPolynomial(USeg, VSeg);
    YPoly = st->YPolynomial(USeg, VSeg);
    ZPoly = st->ZPolynomial(USeg, VSeg);
    ParamU = 1.;
    for (i=Coef.LowerRow(); i<=Coef.UpperRow(); i++) {
      ParamV = 1.;
      for (j=Coef.LowerCol(); j<=Coef.UpperCol(); j++) {
	Standard_Integer PolyIndex = i + 4*(j-1);
	gp_Pnt aPoint;
	aPoint.SetCoord(XPoly->Value(PolyIndex)*ParamU*ParamV, 
			YPoly->Value(PolyIndex)*ParamU*ParamV, 
			ZPoly->Value(PolyIndex)*ParamU*ParamV);
	Coef.SetValue(i, j, aPoint);
	ParamV = ParamV *deltaV(VSeg);
      }
      ParamU = ParamU * deltaU(USeg);
    }
    PLib::CoefficientsPoles(Coef,PLib::NoWeights2(),BzPole,PLib::NoWeights2());

    //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
    iBs = BsPole.LowerRow();
    jBs = BsPole.LowerCol() + (VSeg-1)*DegreeV;
    jBz = BzPole.LowerCol();
    for (iBz=BzPole.LowerRow(); iBz<=BzPole.UpperRow(); iBz++) {
      if (!BzPole.Value(iBz,jBz).IsEqual(BsPole.Value(iBs,jBs), epsgeom)) {
	wasC0=Standard_False;
	gp_Pnt MidPoint;
	Standard_Real  XCoord = 0.5 * 
	  (BzPole.Value(iBz,jBz).X() + BsPole.Value(iBs,jBs).X());
	Standard_Real  YCoord = 0.5 * 
	  (BzPole.Value(iBz,jBz).Y() + BsPole.Value(iBs,jBs).Y());
	Standard_Real  ZCoord = 0.5 * 
	  (BzPole.Value(iBz,jBz).Z() + BsPole.Value(iBs,jBs).Z());
	MidPoint.SetCoord(XCoord, YCoord, ZCoord);
	BsPole.SetValue(iBs++, jBs, MidPoint);
      }
      else{
	BsPole.SetValue(iBs++, jBs, BzPole.Value(iBz,jBz));
      }
    }

    jBs++;
    iBs = BsPole.LowerRow();
    for (jBz=BzPole.LowerCol()+1; jBz<=BzPole.UpperCol(); jBz++) {
      for (iBz=BzPole.LowerRow(); iBz<=BzPole.UpperRow(); iBz++) 
        BsPole.SetValue(iBs++, jBs, BzPole.Value(iBz,jBz));
      iBs = BsPole.LowerRow();
      jBs++;
    }
  }
  
  
  //  Patches (1<USeg<NbUSeg, 1<VSeg<NbVSeg)
  //  ======================================  
  
  for (VSeg=2; VSeg <= NbVSeg; VSeg++) {
    for (USeg=2; USeg <= NbUSeg; USeg++) {
      XPoly = st->XPolynomial(USeg, VSeg);
      YPoly = st->YPolynomial(USeg, VSeg);
      ZPoly = st->ZPolynomial(USeg, VSeg);
      ParamU = 1.;
      for (i=Coef.LowerRow(); i<=Coef.UpperRow(); i++) {
	ParamV = 1.;
	for (j=Coef.LowerCol(); j<=Coef.UpperCol(); j++) {
	  Standard_Integer PolyIndex = i + 4*(j-1);
	  gp_Pnt aPoint;
	  aPoint.SetCoord(XPoly->Value(PolyIndex)*ParamU*ParamV, 
			  YPoly->Value(PolyIndex)*ParamU*ParamV, 
			  ZPoly->Value(PolyIndex)*ParamU*ParamV);
	  Coef.SetValue(i, j, aPoint);
	  ParamV = ParamV *deltaV(VSeg);	  
	}
	ParamU = ParamU * deltaU(USeg);
      }
      PLib::CoefficientsPoles(Coef,PLib::NoWeights2(),BzPole,PLib::NoWeights2());

      //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
      iBs = (USeg-1)*DegreeU + BsPole.LowerRow();
      jBs = (VSeg-1)*DegreeV + BsPole.LowerCol();
      jBz = BzPole.LowerCol();
      for (iBz=BzPole.LowerRow(); iBz<=BzPole.UpperRow(); iBz++) {
	if (!BzPole.Value(iBz,jBz).IsEqual(BsPole.Value(iBs,jBs), epsgeom)) {
	  wasC0=Standard_False;
	  gp_Pnt MidPoint;
	  Standard_Real  XCoord = 0.5 * 
	    (BzPole.Value(iBz,jBz).X() + BsPole.Value(iBs,jBs).X());
	  Standard_Real  YCoord = 0.5 * 
	    (BzPole.Value(iBz,jBz).Y() + BsPole.Value(iBs,jBs).Y());
	  Standard_Real  ZCoord = 0.5 * 
	    (BzPole.Value(iBz,jBz).Z() + BsPole.Value(iBs,jBs).Z());
	  MidPoint.SetCoord(XCoord, YCoord, ZCoord);
	  BsPole.SetValue(iBs++, jBs, MidPoint);
	}
	else
	  BsPole.SetValue(iBs++, jBs, BzPole.Value(iBz,jBz));
      }
      
      iBs = (USeg-1)*DegreeU + BsPole.LowerRow();
      iBz = BzPole.LowerRow();
      for (jBz=BzPole.LowerCol(); jBz<=BzPole.UpperCol(); jBz++) {
	//  C0 check and correction for poles lying on isoparametrics U=0 & V=0
	if (!BzPole.Value(iBz,jBz).IsEqual(BsPole.Value(iBs,jBs), epsgeom)) {
	  wasC0=Standard_False;
	  gp_Pnt MidPoint;
	  Standard_Real  XCoord = 0.5 * 
	    (BzPole.Value(iBz,jBz).X() + BsPole.Value(iBs,jBs).X());
	  Standard_Real  YCoord = 0.5 * 
	    (BzPole.Value(iBz,jBz).Y() + BsPole.Value(iBs,jBs).Y());
	  Standard_Real  ZCoord = 0.5 * 
	    (BzPole.Value(iBz,jBz).Z() + BsPole.Value(iBs,jBs).Z());
	  MidPoint.SetCoord(XCoord, YCoord, ZCoord);
	  BsPole.SetValue(iBs, jBs++, MidPoint);
	}
	else 
	  BsPole.SetValue(iBs, jBs++, BzPole.Value(iBz,jBz));
      }

      iBs = BsPole.LowerRow() + (USeg-1)*DegreeU + 1;
      jBs = BsPole.LowerCol() + (VSeg-1)*DegreeV + 1;
      for (iBz=BzPole.LowerRow()+1; iBz<=BzPole.UpperRow(); iBz++) {
	for (jBz=BzPole.LowerCol()+1; jBz<=BzPole.UpperCol(); jBz++)
	  BsPole.SetValue(iBs, jBs++, BzPole.Value(iBz,jBz));
	jBs = BsPole.LowerCol() + (VSeg-1)*DegreeV + 1;
	iBs++;
      }
    }
  }

  //  Building result taking into account transformation if any :
  //  ===========================================================

  if (st->HasTransf()) {
    gp_GTrsf GSplTrsf(st->CompoundLocation());
    gp_Trsf  SplTrsf;
    Standard_Real epsilon = 1.E-04;
    if (IGESData_ToolLocation::ConvertLocation(epsilon,GSplTrsf,SplTrsf)) 
      for (iBs=BsPole.LowerRow(); iBs<=BsPole.UpperRow(); iBs++) 
	for (jBs=BsPole.LowerCol(); jBs<=BsPole.UpperCol(); jBs++) 
	  BsPole.SetValue(iBs, jBs, BsPole.Value(iBs,jBs).Transformed(SplTrsf));
//    else
//      AddWarning(start, "Transformation skipped : Not a similarity");
  }

  res = new Geom_BSplineSurface
    (BsPole, UKnot, VKnot, UMult, VMult, DegreeU, DegreeV);
  if (wasC0) returned += 1;
  return returned;
}


//=======================================================================
//function : IGESConvGeom::IncreaseSurfaceContinuity
//purpose  : 
//=======================================================================
Standard_Integer IGESConvGeom::IncreaseSurfaceContinuity  (const Handle(Geom_BSplineSurface)& res,
                                                           const Standard_Real epsgeom,
                                                           const Standard_Integer continuity)
{
  if (continuity < 1) return continuity;
  Standard_Boolean isC1 = Standard_True, isC2 = Standard_True;
  Standard_Integer DegreeU = res->UDegree();
  
  Standard_Boolean isModified;
  do {
    isModified = Standard_False;
    for (Standard_Integer i = res->FirstUKnotIndex()+1; i < res->LastUKnotIndex(); i++) 
      if(DegreeU - res->UMultiplicity(i) < continuity) {
        if (continuity >= 2) {
          if (!res->RemoveUKnot(i, DegreeU-2, epsgeom)) {
            isC2 = Standard_False;
            Standard_Boolean locOK = res->RemoveUKnot(i, DegreeU-1, epsgeom);   // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = Standard_True;
        }
        else {
          Standard_Boolean locOK = res->RemoveUKnot(i, DegreeU-1, epsgeom);   // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  }
  while (isModified);
  
  Standard_Integer DegreeV = res->VDegree();
  do {
    isModified = Standard_False;
    for (Standard_Integer i = res->FirstVKnotIndex()+1; i < res->LastVKnotIndex(); i++) 
      if(DegreeV - res->VMultiplicity(i) < continuity) {
        if (continuity >= 2) {
          if (!res->RemoveVKnot(i, DegreeV-2, epsgeom)) {
            isC2 = Standard_False;
            Standard_Boolean locOK = res->RemoveVKnot(i, DegreeV-1, epsgeom);   // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = Standard_True;
        }
        else {
          Standard_Boolean locOK = res->RemoveVKnot(i, DegreeV-1, epsgeom);   // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  }
  while (isModified);
    
  /*
  while (--i > j) {                                      // from 2 to NbKnots-1
    if (continuity >= 2) {
      if (!res->RemoveUKnot(i, DegreeU-2, epsgeom)) {    // is C2 ?
	isC2 = Standard_False;
	isC1 &= res->RemoveUKnot(i, DegreeU-1, epsgeom); // is C1 ?
      }
    }
    else {
      isC1 &= res->RemoveUKnot(i, DegreeU-1, epsgeom); // is C1 ?
    }
  }

  i = res->LastVKnotIndex();   //knots.Upper();
  j = res->FirstVKnotIndex();  //knots.Lower();
  Standard_Integer DegreeV = res->VDegree();
  while (--i > j) {                                      // from 2 to NbKnots-1
    if (continuity >= 2) {
      if (!res->RemoveVKnot(i, DegreeV-2, epsgeom)) {    // is C2 ?
	isC2 = Standard_False;
	isC1 &= res->RemoveVKnot(i, DegreeV-1, epsgeom); // is C1 ?
      }
    }
    else {
      isC1 &= res->RemoveVKnot(i, DegreeV-1, epsgeom); // is C1 ?
    }
  }*/
  
  
  if (!isC1) return 0;
  if (continuity >= 2 && !isC2) return 1;
  return continuity;
}

