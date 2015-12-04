// Created on: 1999-04-14
// Created by: Roman LYGIN
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


#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve2dContinuity.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve2dContinuity,ShapeUpgrade_SplitCurve2d)

//=======================================================================
//function : ShapeUpgrade_SplitCurve2dContinuity
//purpose  : 
//=======================================================================
ShapeUpgrade_SplitCurve2dContinuity::ShapeUpgrade_SplitCurve2dContinuity()
{
  myCriterion = GeomAbs_C1;
  myTolerance = Precision::PConfusion();
  myCont =1;
}

//=======================================================================
//function : SetCriterion
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve2dContinuity::SetCriterion(const GeomAbs_Shape Criterion) 
{
  myCriterion = Criterion;
  switch (myCriterion) {
    case GeomAbs_C0 : myCont = 0; break;
    case GeomAbs_C1 : myCont = 1; break;
    case GeomAbs_C2 : myCont = 2; break;
    case GeomAbs_C3 : myCont = 3; break;
    case GeomAbs_CN : myCont = 4; break;
    default         : myCont = 1;  
  }
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve2dContinuity::SetTolerance(const Standard_Real Tol) 
{
  myTolerance = Tol;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitCurve2dContinuity::Compute()    
{ 
  if(myCurve->Continuity() < myCriterion) 
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
  if (mySplitValues->Length() > 2)
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  Standard_Real precision = Precision::PConfusion();
  Standard_Real First =  mySplitValues->Value(1);
  Standard_Real Last = mySplitValues->Value(mySplitValues->Length());
  if (myCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (myCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    ShapeUpgrade_SplitCurve2dContinuity spc;
//    spc.Init(BasCurve,Max(First,tmp->FirstParameter()),Min(Last,tmp->LastParameter())); 
    spc.Init(BasCurve,First,Last);
    spc.SetSplitValues(mySplitValues);
    spc.SetTolerance(myTolerance);
    spc.SetCriterion(myCriterion);
    spc.Compute();
    mySplitValues->Clear();
    mySplitValues->ChangeSequence() = spc.SplitValues()->Sequence();
    //mySplitValues = spc.SplitValues();
    myStatus |= spc.myStatus;
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) {
    GeomAbs_Shape BasCriterion;
    switch (myCriterion) {
      default         :
      case GeomAbs_C1 : BasCriterion = GeomAbs_C2; break;
      case GeomAbs_C2 : BasCriterion = GeomAbs_C3; break;
      case GeomAbs_C3 : //if (ShapeUpgrade::Debug()) cout<<". this criterion is not suitable for a Offset curve"<<endl; 
#ifdef OCCT_DEBUG
			cout << "Warning: ShapeUpgrade_SplitCurve2dContinuity: criterion C3 for Offset curve" << endl; 
#endif
      case GeomAbs_CN : BasCriterion = GeomAbs_CN; break;
    }
    Handle(Geom2d_OffsetCurve) tmp = Handle(Geom2d_OffsetCurve)::DownCast (myCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    //Standard_Real Offset = tmp->Offset(); // Offset not used (skl)
    ShapeUpgrade_SplitCurve2dContinuity spc;
//    spc.Init(BasCurve,Max(tmp->FirstParameter(),First),Min(tmp->LastParameter(),Last));
    spc.Init(BasCurve,First,Last);
    spc.SetSplitValues(mySplitValues);
    spc.SetTolerance(myTolerance);
    spc.SetCriterion(BasCriterion); 
    spc.Compute();
    mySplitValues->Clear();
    mySplitValues->ChangeSequence() = spc.SplitValues()->Sequence();
    myStatus |= spc.myStatus;
    return;
  }

  Handle(Geom2d_BSplineCurve) MyBSpline = Handle(Geom2d_BSplineCurve)::DownCast (myCurve);
  if (MyBSpline.IsNull()) {
//    if (ShapeUpgrade::Debug()) cout<<". curve is not a Bspline"<<endl;
    return;
  }
  
  myNbCurves=1;
  Standard_Integer Deg=MyBSpline->Degree();
  Standard_Integer NbKnots= MyBSpline->NbKnots(); 
//  if (ShapeUpgrade::Debug()) cout<<". NbKnots="<<NbKnots<<endl;
  if (NbKnots <= 2) {
    return;
  }
  Standard_Integer FirstInd =MyBSpline->FirstUKnotIndex()+1,
  LastInd = MyBSpline->LastUKnotIndex()-1;
  Standard_Integer iknot = FirstInd;
  for(Standard_Integer j =2; j <= mySplitValues->Length(); j++) {
    Last =  mySplitValues->Value(j);
    for (; iknot <= LastInd; iknot++) {
      Standard_Real valknot = MyBSpline->Knot(iknot);
      if(valknot <= First + precision) continue;
      if(valknot >= Last - precision) break;
      Standard_Integer Continuity=Deg-MyBSpline->Multiplicity(iknot);
      //Standard_Real tt = MyBSpline->Knot(iknot); // tt not used (skl)
      if (Continuity < myCont) { 
	// At this knot, the curve is C0; try to remove Knot.
	Standard_Boolean corrected = Standard_False;
	Standard_Integer newMultiplicity = Deg - myCont;
	if (newMultiplicity < 0) newMultiplicity = 0;
	{
	  try {
	    OCC_CATCH_SIGNALS
	    corrected = MyBSpline->RemoveKnot(iknot, newMultiplicity, myTolerance);
	  }
	  catch (Standard_Failure) {
	    corrected = Standard_False;
	  }
	}
	if (corrected && newMultiplicity > 0) {
	 Continuity=Deg-MyBSpline->Multiplicity(iknot);
	 corrected = (Continuity >= myCont);
	}
	if (corrected) {
	  // at this knot, the continuity is now C1. Nothing else to do.
//	  if (ShapeUpgrade::Debug()) cout<<". Correction at Knot "<<iknot<<endl;
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
	}
	else {
	  // impossible to force C1 within the tolerance: 
	  // this knot will be a splitting value.
	  mySplitValues->InsertBefore(j++,MyBSpline->Knot(iknot));
	  myNbCurves++;
//	  if (ShapeUpgrade::Debug()) cout<<". Splitting at Knot "<<iknot<<endl;
	}
      }
    }
    
    First = Last;
  }
  
  if (mySplitValues->Length() > 2)
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
}

