// Created on: 1998-03-12
// Created by: Roman LYGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve3d,ShapeUpgrade_SplitCurve)

//=======================================================================
//function : ShapeUpgrade_SplitCurve3d
//purpose  : 
//=======================================================================
ShapeUpgrade_SplitCurve3d::ShapeUpgrade_SplitCurve3d()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve3d::Init(const Handle(Geom_Curve)& C) 
{
  Init (C, C->FirstParameter(), C->LastParameter());
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve3d::Init(const Handle(Geom_Curve)& C,
				      const Standard_Real First,
				      const Standard_Real Last) 
{
//  if (ShapeUpgrade::Debug()) cout << "SplitCurve3d::Init"<<endl;
  Handle(Geom_Curve) CopyOfC = Handle(Geom_Curve)::DownCast(C->Copy());
  myCurve = CopyOfC;
  Standard_Real precision = Precision::PConfusion();
  Standard_Real firstPar = First;
  Standard_Real lastPar = Last;
  Handle (Geom_Curve) aCurve = myCurve;
  if(aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) 
    aCurve=Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
  // 15.11.2002 PTV OCC966
  if(!ShapeAnalysis_Curve::IsPeriodic(C)) {
    Standard_Real fP = aCurve->FirstParameter();
    Standard_Real lP  = aCurve->LastParameter();
    if(Abs(firstPar-fP) < precision)
       firstPar = fP;
    if(Abs(lastPar-lP) < precision)
      lastPar = lP;
    if(firstPar < fP){
#ifdef OCCT_DEBUG
      cout <<"Warning: The range of the edge exceeds the curve domain" <<endl;
#endif
      firstPar = fP;
    }
    if(lastPar > lP){
#ifdef OCCT_DEBUG
      cout <<"Warning: The range of the edge exceeds the curve domain" <<endl;
#endif
      lastPar = lP;
    }
    if( (lastPar-firstPar) < precision)
      lastPar=firstPar+2*precision;
  }
  
  ShapeUpgrade_SplitCurve::Init (firstPar, lastPar);

  // first, we make a copy of C to prevent modification:
//  if (ShapeUpgrade::Debug()) cout << ". copy of the curve"<<endl;
  
  myNbCurves = 1;
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitCurve3d::Build(const Standard_Boolean Segment) 
{
//  if (ShapeUpgrade::Debug()) cout<<"ShapeUpgrade_SplitCurve3d::Build"<<endl;
  Standard_Real First =  mySplitValues->Value(1);
  Standard_Real Last =  mySplitValues->Value(mySplitValues->Length());
    
  if (mySplitValues->Length() >2)
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  if (myCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) tmp = Handle(Geom_TrimmedCurve)::DownCast (myCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    ShapeUpgrade_SplitCurve3d spc;
    spc.Init(BasCurve,First,Last);
    spc.SetSplitValues(mySplitValues);
    spc.Build(Segment);
    myNbCurves = spc.GetCurves()->Length();
    myResultingCurves=new TColGeom_HArray1OfCurve (1,myNbCurves);
    if(myNbCurves == 1) {
      Handle(Geom_TrimmedCurve) NewTrimCurve = 
	new Geom_TrimmedCurve(spc.GetCurves()->Value(1),First,Last);
      myResultingCurves->SetValue(1,NewTrimCurve);
    }
    else 
      myResultingCurves = spc.GetCurves();   
    myStatus |= spc.myStatus;
    return;
  }
  else if (myCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    Handle(Geom_OffsetCurve) tmp = Handle(Geom_OffsetCurve)::DownCast (myCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    Standard_Real Offset = tmp->Offset();
    gp_Dir Direct = tmp->Direction();
    ShapeUpgrade_SplitCurve3d spc;
    spc.Init(BasCurve,First,Last);
    spc.SetSplitValues(mySplitValues);  
    spc.Build(Segment);
    myNbCurves = spc.GetCurves()->Length();
    myResultingCurves = new TColGeom_HArray1OfCurve (1,myNbCurves);
    for(Standard_Integer i = 1; i <= myNbCurves; i++) {
      Handle(Geom_OffsetCurve) NewOffsetCurve = 
	new Geom_OffsetCurve(spc.GetCurves()->Value(i),Offset,Direct);
      myResultingCurves->SetValue(i,NewOffsetCurve);
    }
    myStatus |= spc.myStatus;
    return;
  }
  
  //pdn fix on BuildCurve 3d
  // 15.11.2002 PTV OCC966
  if(!ShapeAnalysis_Curve::IsPeriodic(myCurve)) {
    //pdn exceptons only on non periodic curves
    Standard_Real precision = Precision::PConfusion();
    Standard_Real firstPar = myCurve->FirstParameter();
    Standard_Real lastPar  = myCurve->LastParameter();
    if(Abs(First-firstPar) < precision)
      First = firstPar;
    if(Abs(Last-lastPar) < precision)
      Last = lastPar;
    if(First < firstPar){
#ifdef OCCT_DEBUG
      cout <<"Warning: The range of the edge exceeds the curve domain" <<endl;
#endif
      First = firstPar;
      mySplitValues->SetValue(1,First);
    }
    if(Last > lastPar){
#ifdef OCCT_DEBUG
      cout <<"Warning: The range of the edge exceeds the curve domain" <<endl;
#endif
      Last = lastPar;
      mySplitValues->SetValue(mySplitValues->Length(),Last);
    }
  }

  myNbCurves = mySplitValues->Length() -1;
  myResultingCurves = new TColGeom_HArray1OfCurve (1, myNbCurves);
  if (myNbCurves == 1) {
    Standard_Boolean filled = Standard_True;
    if ( Abs ( myCurve->FirstParameter() - First ) < Precision::PConfusion() && 
	 Abs ( myCurve->LastParameter()  - Last  ) < Precision::PConfusion() )
      myResultingCurves->SetValue(1,myCurve);
    
    else if ( ! Segment || (!myCurve->IsKind (STANDARD_TYPE (Geom_BSplineCurve)) &&
	     !myCurve->IsKind (STANDARD_TYPE (Geom_BezierCurve))) || ! Status ( ShapeExtend_DONE2 ) ) {
/*      if(myCurve->IsKind (STANDARD_TYPE (Geom_BSplineCurve)) ||
	     myCurve->IsKind (STANDARD_TYPE (Geom_BezierCurve) )) {
	Handle(Geom_Curve) theNewCurve = Handle(Geom_Curve)::DownCast(myCurve->Copy());
	try {  
	  OCC_CATCH_SIGNALS
	  if (myCurve->IsKind (STANDARD_TYPE (Geom_BSplineCurve))) 
	    Handle(Geom_BSplineCurve)::DownCast(theNewCurve)->Segment (First, Last);
	  else if (myCurve->IsKind (STANDARD_TYPE (Geom_BezierCurve)))
	    Handle(Geom_BezierCurve)::DownCast(theNewCurve)->Segment (First, Last);
	}
      catch (Standard_Failure) {
#ifdef OCCT_DEBUG
	  cout << "Warning: ShapeUpgrade_Split3dCurve::Build(): Exception in Segment      :";
	  Standard_Failure::Caught()->Print(cout); cout << endl;
#endif
	  theNewCurve = new Geom_TrimmedCurve(Handle(Geom_Curve)::DownCast(myCurve->Copy()),First,Last);  
	}
	myResultingCurves->SetValue (1, theNewCurve);
      }
      else {*/
	Handle(Geom_TrimmedCurve) theNewCurve = new Geom_TrimmedCurve(Handle(Geom_Curve)::DownCast(myCurve->Copy()),First,Last);
	myResultingCurves->SetValue (1, theNewCurve);
     // }
    }
    else filled = Standard_False;
    if ( filled ) return;
  }
  if (myCurve->IsKind (STANDARD_TYPE (Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) BsCurve = Handle(Geom_BSplineCurve)::DownCast(myCurve->Copy());
    Standard_Integer FirstInd =BsCurve->FirstUKnotIndex(), 
    LastInd = BsCurve->LastUKnotIndex();
    Standard_Integer j =  FirstInd;
    for(Standard_Integer ii =1; ii <= mySplitValues->Length(); ii++) {
      Standard_Real spval = mySplitValues->Value(ii);
      for(; j <=LastInd;j++) {
	if( spval > BsCurve->Knot(j) + Precision::PConfusion()) continue;
	if( spval < BsCurve->Knot(j) - Precision::PConfusion()) break;
	mySplitValues->SetValue(ii,BsCurve->Knot(j));
      }
      if(j == LastInd) break;
    }
  }
  
  for (Standard_Integer i = 1; i <= myNbCurves; i++) {
    // skl : in the next block I change "First","Last" to "Firstt","Lastt"
    Standard_Real Firstt = mySplitValues->Value(i), Lastt = mySplitValues->Value(i+1);
    Handle(Geom_Curve) theNewCurve;
    if(Segment) {
      if (myCurve->IsKind (STANDARD_TYPE (Geom_BSplineCurve)) ||
	  myCurve->IsKind (STANDARD_TYPE (Geom_BezierCurve))) {
	theNewCurve = Handle(Geom_Curve)::DownCast(myCurve->Copy());
	try {  
	  OCC_CATCH_SIGNALS
	  if (myCurve->IsKind (STANDARD_TYPE (Geom_BSplineCurve))) 
	    Handle(Geom_BSplineCurve)::DownCast(theNewCurve)->Segment (Firstt, Lastt);
	  else if (myCurve->IsKind (STANDARD_TYPE (Geom_BezierCurve)))
	    Handle(Geom_BezierCurve)::DownCast(theNewCurve)->Segment (Firstt, Lastt);
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
	}
	catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	  cout << "Warning: ShapeUpgrade_Split3dCurve::Build(): Exception in Segment      :";
	  anException.Print(cout); cout << endl;
#endif
	  (void)anException;
	  theNewCurve = new Geom_TrimmedCurve(Handle(Geom_Curve)::DownCast(myCurve->Copy()),Firstt,Lastt);  
	}
      }
      else theNewCurve = new Geom_TrimmedCurve(Handle(Geom_Curve)::DownCast(myCurve->Copy()),Firstt,Lastt);  
    }
    myResultingCurves->SetValue (i, theNewCurve);
  }

}

//=======================================================================
//function : GetCurves
//purpose  : 
//=======================================================================

const Handle(TColGeom_HArray1OfCurve)& ShapeUpgrade_SplitCurve3d::GetCurves() const
{
  return myResultingCurves;
}
