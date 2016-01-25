// Created on: 1999-05-13
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2dConvert_BSplineCurveToBezierCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomTools.hxx>
#include <Precision.hxx>
#include <ShapeCustom_Curve2d.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_ConvertCurve2dToBezier.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ConvertCurve2dToBezier,ShapeUpgrade_SplitCurve2d)

ShapeUpgrade_ConvertCurve2dToBezier::ShapeUpgrade_ConvertCurve2dToBezier()
{
  mySegments = new TColGeom2d_HSequenceOfCurve;
  mySplitParams = new TColStd_HSequenceOfReal;
}

static Handle(Geom2d_BezierCurve) MakeBezier2d(const Handle(Geom2d_Curve)& theCurve2d,
                                               const Standard_Real theFirst,
                                               const Standard_Real theLast)
{
  TColgp_Array1OfPnt2d poles(1,2);
  poles(1) = theCurve2d->Value(theFirst);
  poles(2) = theCurve2d->Value(theLast);
  Handle(Geom2d_BezierCurve) bezier = new Geom2d_BezierCurve(poles);
  return bezier;
}


//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void ShapeUpgrade_ConvertCurve2dToBezier::Compute()
{
  mySegments->Clear();
  mySplitParams->Clear();
  Standard_Real precision = Precision::PConfusion();
  Standard_Real First =  mySplitValues->Value(1);
  Standard_Real Last = mySplitValues->Value(mySplitValues->Length());
  
  // PTV Try to create line2d from myCurve
  if ( myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) ||
       myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) )
  {
    // static function`s code getted from ShapeConvert
    Standard_Real tmpF, tmpL, aDeviation;
    Handle(Geom2d_Line) aTmpLine2d = 
      ShapeCustom_Curve2d::ConvertToLine2d(myCurve, First, Last, Precision::Approximation(),
                                               tmpF, tmpL, aDeviation);
    if (!aTmpLine2d.IsNull() && (aDeviation <= Precision::Approximation()) )
    {
      Handle(Geom2d_BezierCurve) bezier = MakeBezier2d(aTmpLine2d, tmpF, tmpL);
      mySegments->Append(bezier);
      mySplitParams->Append(First);
      mySplitParams->Append(Last);
      myNbCurves = mySplitValues->Length()-1;
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
      return;
    }
  }
  
  if(myCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (myCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    ShapeUpgrade_ConvertCurve2dToBezier converter;
    //converter.Init(BasCurve,Max(First,BasCurve->FirstParameter()),Min(Last,BasCurve->LastParameter())); //???
    converter.Init(BasCurve,First,Last);
    converter.SetSplitValues(mySplitValues);
    converter.Compute();
    mySplitValues->Clear();
    mySplitValues->ChangeSequence() = converter.SplitValues()->Sequence();
    myNbCurves = mySplitValues->Length()-1;
    myStatus |= converter.myStatus;
    mySegments->ChangeSequence() = converter.Segments()->Sequence();
    mySplitParams->ChangeSequence() = converter.SplitParams()->Sequence();
    return;
  }
  else if(myCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
    Handle(Geom2d_BezierCurve) bezier = Handle(Geom2d_BezierCurve)::DownCast (myCurve);
    myNbCurves = mySplitValues->Length()-1;
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    if(First < precision && Last > 1 - precision) {
      mySegments->Append(bezier);
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
    } else {
      Handle(Geom2d_BezierCurve) besNew = Handle(Geom2d_BezierCurve)::DownCast(bezier->Copy());
      besNew->Segment(First,Last);
      mySegments->Append(besNew);
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
    }
    return;
  }
  else if(myCurve->IsKind(STANDARD_TYPE(Geom2d_Line))) {
    Handle(Geom2d_Line) aLine2d = Handle(Geom2d_Line)::DownCast(myCurve);
    Handle(Geom2d_BezierCurve) bezier = MakeBezier2d(aLine2d, First, Last);    
    mySegments->Append(bezier);
    mySplitParams->Append(First);
    mySplitParams->Append(Last);
    myNbCurves = mySplitValues->Length()-1;
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
    return;
  }
  else {
    Handle(Geom2d_BSplineCurve) aBSpline2d;
    Standard_Real Shift = 0.;
    if(myCurve->IsKind(STANDARD_TYPE(Geom2d_Conic))) {
      Handle(Geom2d_Curve) tcurve = new Geom2d_TrimmedCurve(myCurve,First,Last); //protection agains parabols ets
      Geom2dConvert_ApproxCurve approx (tcurve, Precision::Approximation(), 
					GeomAbs_C1, 100, 6 );
      if ( approx.HasResult() )
	aBSpline2d = approx.Curve();
      else 
	aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve,Convert_QuasiAngular);
      
      Shift = First - aBSpline2d->FirstParameter();
      First = aBSpline2d->FirstParameter();
      Last = aBSpline2d->LastParameter();
    }
    else if(!myCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(myCurve,Convert_QuasiAngular);
    }
    else
      aBSpline2d = Handle(Geom2d_BSplineCurve)::DownCast(myCurve);
    
    Standard_Real bf = aBSpline2d->FirstParameter();
    Standard_Real bl = aBSpline2d->LastParameter(); 
    if(Abs(First-bf) < precision)
      First = bf;
    if(Abs(Last-bl) < precision)
      Last = bl;
    if(First < bf){
#ifdef OCCT_DEBUG
      cout <<"Warning: The range of the edge exceeds the pcurve domain" <<endl;
#endif
      First = bf;
      mySplitValues->SetValue(1,First);
    }
    if(Last > bl){
#ifdef OCCT_DEBUG
      cout <<"Warning: The range of the edge exceeds the pcurve domain" <<endl;
#endif
      Last = bl;
      mySplitValues->SetValue(mySplitValues->Length(),Last);
    }

    // PTV 20.12.2001 Try to simpify BSpline Curve
    ShapeCustom_Curve2d::SimplifyBSpline2d (aBSpline2d, Precision::Approximation());
    
    Geom2dConvert_BSplineCurveToBezierCurve tool(aBSpline2d,First,Last,precision);
    Standard_Integer nbArcs = tool.NbArcs();
    TColStd_Array1OfReal knots(1,nbArcs+1);
    tool.Knots(knots);
    mySplitParams->Append(First+Shift);
    Standard_Integer j; // svv Jan 10 2000 : porting on DEC
    
    Standard_Real newFirst = First+Shift;
    Standard_Real newLast = First+Shift;
    
    for(j = 1; j <=nbArcs; j++) {
      Standard_Real nextKnot = knots(j+1)+Shift;
      if(nextKnot - mySplitParams->Value(mySplitParams->Length()) > precision) {
        Handle(Geom2d_Curve) aCrv2d = tool.Arc(j);
        if ( aCrv2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) )
        {
          newFirst = newLast;
          newLast = nextKnot;
          Standard_Real tmpF, tmpL, aDeviation;
          Handle(Geom2d_Line) aTmpLine2d = 
	    ShapeCustom_Curve2d::ConvertToLine2d(aCrv2d, newFirst, newLast, Precision::Approximation(),
                                                     tmpF, tmpL, aDeviation);
          if (!aTmpLine2d.IsNull() && (aDeviation <= Precision::Approximation()) )
          {
            Handle(Geom2d_BezierCurve) bezier = MakeBezier2d(aBSpline2d, newFirst, newLast);
            mySegments->Append(bezier);
            mySplitParams->Append(newLast);
            continue;
          }
        }
        mySegments->Append(aCrv2d);
        mySplitParams->Append(nextKnot);
      }
    }
    
    First = mySplitValues->Value(1);
    for(j = 2; j <= mySplitValues->Length(); j++) {
      Last =  mySplitValues->Value(j);
      for(Standard_Integer i = 2; i <= nbArcs+1; i++) {
	Standard_Real valknot = knots(i)+Shift;
	if(valknot <= First + precision) continue;
	if(valknot >= Last - precision) break;
	mySplitValues->InsertBefore(j++,valknot);
      }
      First = Last;
    }
    myNbCurves = mySplitValues->Length()-1;
  }
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void ShapeUpgrade_ConvertCurve2dToBezier::Build(const Standard_Boolean /*Segment*/)
{
  Standard_Real prec = Precision::PConfusion();
  Standard_Integer nb = mySplitValues->Length();
  myResultingCurves =  new TColGeom2d_HArray1OfCurve (1,nb-1);
  Standard_Real prevPar =0.;
  Standard_Integer j=2;
  for(Standard_Integer i = 2; i <= nb; i++) {
    Standard_Real par = mySplitValues->Value(i);
    for(; j<= mySplitParams->Length(); j++) 
      if(mySplitParams->Value(j)+prec > par)
	break;
      else
	prevPar = 0.;

    Handle(Geom2d_BezierCurve) bes = Handle(Geom2d_BezierCurve)
      ::DownCast(mySegments->Value(j-1)->Copy());
    Standard_Real uFact = mySplitParams->Value(j) - mySplitParams->Value(j-1);
    Standard_Real pp = mySplitValues->Value(i-1);
    Standard_Real length = (par - pp)/uFact;
    bes->Segment(prevPar, prevPar+length);
    prevPar += length;
    myResultingCurves->SetValue(i-1,bes);
  }
}

//=======================================================================
//function : Segments
//purpose  : 
//=======================================================================

Handle(TColGeom2d_HSequenceOfCurve) ShapeUpgrade_ConvertCurve2dToBezier::Segments() const
{
  return mySegments;
}

//=======================================================================
//function : SplitParams
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfReal) ShapeUpgrade_ConvertCurve2dToBezier::SplitParams() const
{
  return mySplitParams;
}
