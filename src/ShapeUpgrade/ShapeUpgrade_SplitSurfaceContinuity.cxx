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


#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <gp_Ax1.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve3dContinuity.hxx>
#include <ShapeUpgrade_SplitSurfaceContinuity.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColGeom_HArray2OfSurface.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurfaceContinuity,ShapeUpgrade_SplitSurface)

//======================================================================
//function : ShapeUpgrade_SplitSurface
//purpose  : 
//=======================================================================
ShapeUpgrade_SplitSurfaceContinuity::ShapeUpgrade_SplitSurfaceContinuity()
{
  myCriterion = GeomAbs_C1;
  myTolerance = Precision::Confusion();
  
}

//=======================================================================
//function : SetCrierion
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitSurfaceContinuity::SetCriterion(const GeomAbs_Shape Criterion)
{
  myCriterion = Criterion;
  switch (myCriterion) {
    default         :
    case GeomAbs_C1 : myCont = 1; break;
    case GeomAbs_C2 : myCont = 2; break;
    case GeomAbs_C3 : myCont = 3; break;
    case GeomAbs_CN : myCont = 4; break;
  }
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitSurfaceContinuity::SetTolerance(const Standard_Real Tol)
{
  myTolerance = Tol;
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

 void ShapeUpgrade_SplitSurfaceContinuity::Compute(const Standard_Boolean Segment) 
{
  if(!Segment) {
    Standard_Real UF,UL,VF,VL;
    mySurface->Bounds(UF,UL,VF,VL);
    if(!Precision::IsInfinite(UF)) myUSplitValues->SetValue(1,UF);
    if(!Precision::IsInfinite(UL)) myUSplitValues->SetValue(myUSplitValues->Length(),UL);
    if(!Precision::IsInfinite(VF)) myVSplitValues->SetValue(1,VF);
    if(!Precision::IsInfinite(VL)) myVSplitValues->SetValue(myVSplitValues->Length(),VL);
  }
  
  Standard_Real UFirst = myUSplitValues->Value(1);
  Standard_Real ULast  = myUSplitValues->Value(myUSplitValues->Length());
  Standard_Real VFirst = myVSplitValues->Value(1);
  Standard_Real VLast  = myVSplitValues->Value(myVSplitValues->Length());
  Standard_Real precision = Precision::Confusion();
//  if (ShapeUpgrade::Debug()) cout << "SplitSurfaceContinuity::Build" << endl;
  if(mySurface->Continuity() < myCriterion) 
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
  if (myUSplitValues->Length() >2 || myVSplitValues->Length() >2 )
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  
  if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {      
    Handle(Geom_SurfaceOfRevolution) Surface = Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
    if(Surface->Continuity() >=  myCriterion && myUSplitValues->Length() ==2 && myVSplitValues->Length() ==2 ) {
      return;
    }
    Handle(Geom_Curve) BasCurve = Surface->BasisCurve(); 
    ShapeUpgrade_SplitCurve3dContinuity spc;
    spc.Init(BasCurve,VFirst,VLast);
    spc.SetCriterion(myCriterion);
    spc.SetTolerance(myTolerance);
    spc.SetSplitValues(myVSplitValues);
    spc.Compute();
    myVSplitValues->Clear();
    myVSplitValues->ChangeSequence() = spc.SplitValues()->Sequence();
    if ( spc.Status ( ShapeExtend_DONE1 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    if ( spc.Status ( ShapeExtend_DONE2 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
    if ( spc.Status ( ShapeExtend_DONE3 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
    return;
  }   
  if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) Surface = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface);
   if(Surface->Continuity() >=  myCriterion && myUSplitValues->Length() ==2 && myVSplitValues->Length() == 2) {
      return;
    }
    Handle(Geom_Curve) BasCurve = Surface->BasisCurve();
    ShapeUpgrade_SplitCurve3dContinuity spc;
    spc.Init(BasCurve,UFirst,ULast);
    spc.SetCriterion(myCriterion);
    spc.SetTolerance(myTolerance);
    spc.SetSplitValues(myUSplitValues);
    spc.Compute(); 
    myUSplitValues->Clear();
    myUSplitValues->ChangeSequence() = spc.SplitValues()->Sequence();
    if ( spc.Status ( ShapeExtend_DONE1 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    if ( spc.Status ( ShapeExtend_DONE2 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
    if ( spc.Status ( ShapeExtend_DONE3 ) ) {
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
      Handle(Geom_Curve) aNewBascurve = spc.GetCurve();
      Surface->SetBasisCurve(aNewBascurve);
    }
    return;
  }
  
  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) tmp = Handle(Geom_RectangularTrimmedSurface)::DownCast (mySurface);
    if(tmp->Continuity() >=  myCriterion && myUSplitValues->Length() ==2 && myVSplitValues->Length() == 2) {
      return;
    }
    Standard_Real U1,U2,V1,V2;
    tmp->Bounds(U1,U2,V1,V2);
    Handle(Geom_Surface) theSurf = tmp->BasisSurface();
    ShapeUpgrade_SplitSurfaceContinuity sps;
    sps.Init(theSurf,Max(U1,UFirst),Min(U2,ULast),Max(V1,VFirst),Min(V2,VLast));
    sps.SetUSplitValues(myUSplitValues);
    sps.SetVSplitValues(myVSplitValues);
    sps.SetTolerance(myTolerance);
    sps.SetCriterion(myCriterion);
    sps.Compute(Standard_True);
    myUSplitValues->Clear();
    myUSplitValues->ChangeSequence() = sps.USplitValues()->Sequence();
    myVSplitValues->Clear();
    myVSplitValues->ChangeSequence() = sps.VSplitValues()->Sequence();
    myStatus |= sps.myStatus;
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    GeomAbs_Shape BasCriterion;
    switch (myCriterion) {
      default         :
      case GeomAbs_C1 : BasCriterion = GeomAbs_C2; break;
      case GeomAbs_C2 : BasCriterion = GeomAbs_C3; break;
      case GeomAbs_C3 : //if (ShapeUpgrade::Debug()) cout<<". this criterion is not suitable for a Offset Surface"<<endl;; 
#ifdef OCCT_DEBUG
	                cout << "Warning: ShapeUpgrade_SplitSurfaceContinuity: criterion C3 for Offset surface" << endl; 
#endif
      case GeomAbs_CN : BasCriterion = GeomAbs_CN; break;
      
    }
    Handle(Geom_OffsetSurface) tmp = Handle(Geom_OffsetSurface)::DownCast (mySurface);
    Handle(Geom_Surface) theSurf = tmp->BasisSurface();
    if(theSurf->Continuity() >=  BasCriterion && myUSplitValues->Length() ==2 && myVSplitValues->Length() == 2) {
      return;
    }
    ShapeUpgrade_SplitSurfaceContinuity sps;
    sps.Init(theSurf,UFirst,ULast,VFirst,VLast);
    sps.SetUSplitValues(myUSplitValues);
    sps.SetVSplitValues(myVSplitValues);
    sps.SetTolerance(myTolerance);
    sps.SetCriterion(BasCriterion);
    sps.Compute(Standard_True);
    myUSplitValues->Clear();
    myUSplitValues->ChangeSequence() = sps.USplitValues()->Sequence();
    myVSplitValues->Clear();
    myVSplitValues->ChangeSequence() = sps.VSplitValues()->Sequence();
    myStatus |= sps.myStatus;
    return;
  }
  
  Handle(Geom_BSplineSurface) MyBSpline;
  if(mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    MyBSpline = Handle(Geom_BSplineSurface)::DownCast(mySurface->Copy());
  if (MyBSpline.IsNull()) {
//    if (ShapeUpgrade::Debug()) cout<<".  Surface is not a Bspline"<<endl;
    return;
  } 
  if(mySurface->Continuity() >= myCriterion) {
    return;
  }
  
  // it is a BSplineSurface
  Standard_Integer UDeg=MyBSpline->UDegree();
  Standard_Integer VDeg=MyBSpline->VDegree();
  Standard_Integer NbUKnots= MyBSpline->NbUKnots(); 
  Standard_Integer UFirstInd =MyBSpline->FirstUKnotIndex()+1,
  ULastInd = MyBSpline->LastUKnotIndex()-1,
  VFirstInd =MyBSpline->FirstVKnotIndex()+1,
  VLastInd = MyBSpline->LastVKnotIndex()-1;
  Standard_Integer NbVKnots= MyBSpline->NbVKnots();
  
//  if (ShapeUpgrade::Debug()) cout<<". NbUKnots="<<NbUKnots<<endl;
  if (NbUKnots>2) {
    // Only the internal knots are checked.
    Standard_Integer iknot= UFirstInd;
    for(Standard_Integer j =2; j <= myUSplitValues->Length(); j++) {
      ULast =  myUSplitValues->Value(j);
      
      for (; iknot <= ULastInd; iknot++) {
	Standard_Real valknot = MyBSpline->UKnot(iknot);
	if(valknot <= UFirst + precision) continue;
	  if( valknot >= ULast - precision) break;
	Standard_Integer Continuity=UDeg-MyBSpline->UMultiplicity(iknot);
	if (Continuity < myCont) { 
	  // At this knot, the Surface is C0; try to remove Knot.
	  Standard_Integer newMultiplicity=UDeg - myCont;
	  Standard_Boolean corrected = Standard_False;
	  if ( newMultiplicity >= 0 )
	    corrected=MyBSpline->RemoveUKnot(iknot, newMultiplicity, myTolerance);
	  if (corrected && newMultiplicity > 0) {
		Continuity=UDeg-MyBSpline->UMultiplicity(iknot);
		corrected = (Continuity >= myCont);
		}
	  if (corrected) {
	    // at this knot, the continuity is now C1. Nothing else to do.
//	    if (ShapeUpgrade::Debug()) cout<<". Correction at UKnot "<<iknot<<endl;
            // PTV 15.05.2002 decrease iknot and ULastIndex values if knot removed
            if (newMultiplicity ==0) { iknot--; ULastInd--; }
	    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
	  }
	  else {
	    // impossible to force C1 within the tolerance: 
	    // this knot will be a splitting value.
	    Standard_Real u=MyBSpline->UKnot(iknot);
	    myUSplitValues->InsertBefore(j++,u);
	    myNbResultingRow++;
//	    if (ShapeUpgrade::Debug()) cout<<". Splitting at Knot "<<iknot<<endl;
	  }
	}
      }
      UFirst = ULast;
    }
  }
//  if (ShapeUpgrade::Debug()) cout<<". NbVKnots="<<NbVKnots<<endl;
  if (NbVKnots>2) {
    // Only the internal knots are checked.
    Standard_Integer iknot=VFirstInd;
    for(Standard_Integer j1 =2; j1 <= myVSplitValues->Length(); j1++) {
      VLast =  myVSplitValues->Value(j1);
      for (; iknot <= VLastInd; iknot++) {
	Standard_Real valknot = MyBSpline->VKnot(iknot);
	if(valknot <= VFirst + precision) continue;
	if( valknot >= VLast - precision) break;
	Standard_Integer Continuity=VDeg-MyBSpline->VMultiplicity(iknot);
	if (Continuity < myCont) { 
	  // At this knot, the Surface is C0; try to remove Knot.
	  Standard_Integer newMultiplicity=VDeg - myCont;
	  Standard_Boolean corrected = Standard_False;
	  if( newMultiplicity >= 0 )
	    corrected=MyBSpline->RemoveVKnot(iknot, newMultiplicity, myTolerance);
	  if (corrected && newMultiplicity > 0) {
		Continuity=VDeg-MyBSpline->VMultiplicity(iknot);
		corrected = (Continuity >= myCont);
	  }
	  if (corrected ) {
	    // at this knot, the continuity is now Criterion. Nothing else to do.
//	    if (ShapeUpgrade::Debug()) cout<<". Correction at VKnot "<<iknot<<endl;
            // PTV 15.05.2002 decrease iknot and ULastIndex values if knot removed
            if (newMultiplicity ==0) { iknot--; VLastInd--; }
	    myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
	  }
	  else {
	    // this knot will be a splitting value.
	    Standard_Real v=MyBSpline->VKnot(iknot);
	    myVSplitValues->InsertBefore(j1++,v);
	    myNbResultingCol++;
//	    if (ShapeUpgrade::Debug()) cout<<". Splitting at Knot "<<iknot<<endl;
	  }
	}
      }
      VFirst = VLast;
    }
  }
  if ( Status ( ShapeExtend_DONE3 ) ) {
    mySurface = MyBSpline;
  }

  if (myUSplitValues->Length() >2 || myVSplitValues->Length() >2 )
    myStatus |= ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
}




