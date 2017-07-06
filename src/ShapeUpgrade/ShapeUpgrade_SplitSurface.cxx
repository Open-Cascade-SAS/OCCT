// Created on: 1998-03-16
// Created by: Pierre BARRAS
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

//    gka 30.04.99 S4137: extended for all types of surfaces

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Ax1.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <ShapeUpgrade_SplitSurface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColGeom_HArray2OfSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurface,Standard_Transient)

//=======================================================================
//function : ShapeUpgrade_SplitSurface
//purpose  : 
//=======================================================================
ShapeUpgrade_SplitSurface::ShapeUpgrade_SplitSurface() : myStatus(0)
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurface::Init(const Handle(Geom_Surface)& S)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);

  myUSplitValues = new TColStd_HSequenceOfReal();
  myVSplitValues = new TColStd_HSequenceOfReal();
  mySurface = S;
  myResSurfaces = new ShapeExtend_CompositeSurface();
  myNbResultingRow =1;
  myNbResultingCol =1;
  Standard_Real U1,U2,V1,V2;
  mySurface->Bounds(U1,U2,V1,V2);

  myUSplitValues->Append(U1);
  myUSplitValues->Append(U2);

  myVSplitValues->Append(V1);
  myVSplitValues->Append(V2);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurface::Init(const Handle(Geom_Surface)& S, const Standard_Real UFirst,const Standard_Real ULast,
				     const Standard_Real VFirst, const Standard_Real VLast)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  
  mySurface = S;
  myResSurfaces  = new ShapeExtend_CompositeSurface();
  myUSplitValues = new TColStd_HSequenceOfReal();
  myVSplitValues = new TColStd_HSequenceOfReal();
  
  myNbResultingRow =1;
  myNbResultingCol =1;

  Standard_Real U1,U2,V1,V2;
  mySurface->Bounds(U1,U2,V1,V2);
  Standard_Real precision = Precision::PConfusion();
  if ( mySurface->IsUPeriodic() && 
       ULast - UFirst <= U2 - U1 + precision ) { U1 = UFirst; U2 = U1 + mySurface->UPeriod(); }
  if ( mySurface->IsVPeriodic() && 
       VLast - VFirst <= V2 - V1 + precision ) { V1 = VFirst; V2 = V1 + mySurface->VPeriod(); }
  Standard_Real UF,UL,VF,VL;
  if( UFirst > U2-precision || 
      ULast  < U1-precision ) {
    UF =U1; UL = U2;
  }
  else {
    UF = Max(U1,UFirst);
    UL = Min(U2,ULast);
  }
  if( VFirst > V2-precision || 
     VLast  < V1-precision ) {
    VF =V1; VL = V2;
  }
  else {
    VF = Max(V1,VFirst);
    VL = Min(V2,VLast);
  }
  
  if(UL-UF < precision) {
    Standard_Real p2 = precision/2.;
    UF-= p2;
    UL+= p2;
  }
  if(VL-VF < precision) {
    Standard_Real p2 = precision/2.;
    VF-= p2;
    VL+= p2;
  }
  
  myUSplitValues->Append(UF);
  myUSplitValues->Append(UL);
  myVSplitValues->Append(VF);
  myVSplitValues->Append(VL);
}

//=======================================================================
//function : SetSplitValues
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurface::SetUSplitValues(const Handle(TColStd_HSequenceOfReal)& UValues)
{
  if(UValues.IsNull()) return;
  Standard_Real precision = Precision::PConfusion();
  Standard_Real UFirst = myUSplitValues->Value(1),
  ULast = myUSplitValues->Value(myUSplitValues->Length()); 
  Standard_Integer i =1;
  Standard_Integer len = UValues->Length(); 
  
  for(Standard_Integer ku =2; ku <= myUSplitValues->Length();ku++) {
    ULast = myUSplitValues->Value(ku);
    for(; i <= len; i++) {
      if( (UFirst + precision) >= UValues->Value(i)) continue;
      if((ULast - precision) <= UValues->Value(i)) break;
      myUSplitValues->InsertBefore(ku++,UValues->Value(i));
    }
    UFirst = ULast;
  }
}

//=======================================================================
//function : SetSplitVValues
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurface::SetVSplitValues(const Handle(TColStd_HSequenceOfReal)& VValues) 
{
  if(VValues.IsNull()) return;
  Standard_Real precision = Precision::PConfusion();
  Standard_Real VFirst = myVSplitValues->Value(1), 
  VLast = myVSplitValues->Value(myVSplitValues->Length()); 
  Standard_Integer i =1;
  Standard_Integer len = VValues->Length();
  for(Standard_Integer kv =2; kv <= myVSplitValues->Length();kv++) {
    VLast = myVSplitValues->Value(kv);
    for(; i <= len;  i++) {
      if( (VFirst + precision) >= VValues->Value(i)) continue;
      if((VLast - precision) <= VValues->Value(i)) break;
      myVSplitValues->InsertBefore(kv++,VValues->Value(i));
    }
    VFirst = VLast;
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurface::Build(const Standard_Boolean Segment) 
{
  
  Standard_Real UFirst = myUSplitValues->Value(1);
  Standard_Real ULast = myUSplitValues->Value(myUSplitValues->Length());
  Standard_Real VFirst =  myVSplitValues->Value(1);
  Standard_Real VLast =   myVSplitValues->Value(myVSplitValues->Length());
  
  if(myUSplitValues->Length() > 2 || myVSplitValues->Length() > 2)
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  
  Standard_Real U1,U2,V1,V2;
  mySurface->Bounds(U1, U2, V1, V2);
  if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {      
    Handle(Geom_SurfaceOfRevolution) Surface = Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
    Handle(Geom_Curve) BasCurve = Surface->BasisCurve();
    ShapeUpgrade_SplitCurve3d spc;
    spc.Init(BasCurve,VFirst,VLast);
    spc.SetSplitValues(myVSplitValues);
    spc.Build(Segment);
    Handle(TColGeom_HArray2OfSurface) Surfaces;
    myNbResultingCol = spc.GetCurves()->Length();
    if(myUSplitValues->Length()> 2) {
      myNbResultingRow =  myUSplitValues->Length() -1;
      Surfaces = new TColGeom_HArray2OfSurface(1, myNbResultingRow,1,myNbResultingCol);
      for(Standard_Integer nc =1; nc <= myNbResultingCol; nc++) {
	Handle(Geom_SurfaceOfRevolution) NewSurfaceRev = 
	  new Geom_SurfaceOfRevolution(spc.GetCurves()->Value(nc),Surface->Axis()); 
	Standard_Real U1p,U2p,V1p,V2p;
	NewSurfaceRev->Bounds(U1p,U2p,V1p,V2p);
	for(Standard_Integer nc1 =1; nc1 <= myNbResultingRow; nc1++) { 
	  Handle(Geom_RectangularTrimmedSurface) NewSurf = 
	    new Geom_RectangularTrimmedSurface ( NewSurfaceRev, myUSplitValues->Value(nc1),
						 myUSplitValues->Value(nc1+1), V1p, V2p );
	  Surfaces->SetValue(nc1,nc,NewSurf);
	}
      }
    }
    else {
      Surfaces = new TColGeom_HArray2OfSurface(1,1,1,myNbResultingCol);
      
      for(Standard_Integer nc =1; nc <= spc.GetCurves()->Length(); nc++) {
	Handle(Geom_SurfaceOfRevolution) NewSurfaceRev = 
	  new Geom_SurfaceOfRevolution ( spc.GetCurves()->Value(nc), Surface->Axis() ); 
	NewSurfaceRev->Bounds(U1, U2, V1, V2);
	if( UFirst == U1 && ULast == U2)
	  Surfaces ->SetValue(1,nc,NewSurfaceRev);
	else {
	  Handle(Geom_RectangularTrimmedSurface) NewSurf = new Geom_RectangularTrimmedSurface
	    (NewSurfaceRev,UFirst,ULast,V1,V2); //pdn correction for main seq
	  Surfaces ->SetValue(1,nc,NewSurf);
	}
      }
    }
    myResSurfaces->Init(Surfaces);
    myResSurfaces->SetUFirstValue ( myUSplitValues->Sequence().First() );
    myResSurfaces->SetVFirstValue ( myVSplitValues->Sequence().First() );
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
    Handle(Geom_Curve) BasCurve = Surface->BasisCurve(); 
    ShapeUpgrade_SplitCurve3d spc;
    spc.Init(BasCurve,UFirst,ULast);
    spc.SetSplitValues(myUSplitValues);
    spc.Build(Segment); 
    myNbResultingRow = spc.GetCurves()->Length();
    Handle(TColGeom_HArray2OfSurface) Surfaces;
    if(myVSplitValues->Length() > 2) {
      myNbResultingCol = myVSplitValues->Length() - 1;
      Surfaces = new TColGeom_HArray2OfSurface(1,myNbResultingRow,1,myNbResultingCol);
      for(Standard_Integer nc1 =1; nc1 <= myNbResultingRow; nc1++) {
	Handle(Geom_SurfaceOfLinearExtrusion) NewSurfaceEx = new Geom_SurfaceOfLinearExtrusion(spc.GetCurves()->Value(nc1),
											       Surface->Direction()); 
	Standard_Real U1p,U2p,V1p,V2p;
	NewSurfaceEx->Bounds(U1p,U2p,V1p,V2p);
	for(Standard_Integer nc2 =1; nc2 <= myNbResultingCol; nc2++) {
	  Handle(Geom_RectangularTrimmedSurface) NewSurf = new Geom_RectangularTrimmedSurface
	    (NewSurfaceEx,U1p,U2p,myVSplitValues->Value(nc2),myVSplitValues->Value(nc2+1));
	  Surfaces ->SetValue(nc1,nc2,NewSurf);
	} 
      }
    }
    else {
      Surfaces = new TColGeom_HArray2OfSurface(1,myNbResultingRow,1,1);
      
      for(Standard_Integer nc1 =1; nc1 <= myNbResultingRow; nc1++) {
	Handle(Geom_SurfaceOfLinearExtrusion) NewSurfaceEx = new Geom_SurfaceOfLinearExtrusion(spc.GetCurves()->Value(nc1),Surface->Direction()); 
	NewSurfaceEx->Bounds(U1,U2,V1,V2);
	if(VFirst == V1 && VLast == V2)
	  Surfaces -> SetValue(nc1,1,NewSurfaceEx);
	else {
	  Handle(Geom_RectangularTrimmedSurface) NewSurf = new Geom_RectangularTrimmedSurface
	    (NewSurfaceEx,Max(U1,UFirst),Min(ULast,U2),Max(VFirst,V1),Min(VLast,V2));
	  Surfaces ->SetValue(nc1,1,NewSurf);
	}
      } 
    }
    myResSurfaces->Init(Surfaces);
    myResSurfaces->SetUFirstValue ( myUSplitValues->Sequence().First() );
    myResSurfaces->SetVFirstValue ( myVSplitValues->Sequence().First() );
    if ( spc.Status ( ShapeExtend_DONE1 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    if ( spc.Status ( ShapeExtend_DONE2 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE2 );
    if ( spc.Status ( ShapeExtend_DONE3 ) )
      myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
    return;
  }
  
  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) tmp = Handle(Geom_RectangularTrimmedSurface)::DownCast (mySurface);
    Handle(Geom_Surface) theSurf = tmp->BasisSurface();
    ShapeUpgrade_SplitSurface sps;
    sps.Init(theSurf,UFirst,ULast,VFirst,VLast);
    sps.SetUSplitValues(myUSplitValues);
    sps.SetVSplitValues(myVSplitValues);
    sps.myStatus = myStatus;
    sps.Build(Segment);
    myStatus |= sps.myStatus;
    myResSurfaces = sps.myResSurfaces;
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) tmp = Handle(Geom_OffsetSurface)::DownCast (mySurface);
    Handle(Geom_Surface) theSurf = tmp->BasisSurface();
    ShapeUpgrade_SplitSurface sps;
    sps.Init(theSurf,UFirst,ULast,VFirst,VLast);
    sps.SetUSplitValues(myUSplitValues);
    sps.SetVSplitValues(myVSplitValues);
    sps.myStatus = myStatus;
    sps.Build(Segment);
    Handle(TColGeom_HArray2OfSurface) Patches = new TColGeom_HArray2OfSurface
      (1,sps.ResSurfaces()->NbUPatches(),1,sps.ResSurfaces()->NbVPatches());
    for(Standard_Integer i1 = 1; i1 <= sps.ResSurfaces()->NbUPatches(); i1++) {
      for(Standard_Integer j1 = 1 ; j1 <= sps.ResSurfaces()->NbVPatches(); j1++) {
	Handle(Geom_OffsetSurface) NewOffSur = new Geom_OffsetSurface(sps.ResSurfaces()->Patch(i1,j1),tmp->Offset());
 	Patches->SetValue(i1,j1,NewOffSur);
      }
    }
    myResSurfaces->Init(Patches);
    myResSurfaces->SetUFirstValue ( myUSplitValues->Sequence().First() );
    myResSurfaces->SetVFirstValue ( myVSplitValues->Sequence().First() );
    myStatus |= sps.myStatus;
    return;
  }
  
  // splitting the surfaces:
  myNbResultingRow = myUSplitValues->Length() -1;
  myNbResultingCol = myVSplitValues->Length() -1;
  Handle(TColGeom_HArray2OfSurface) Surfaces =new TColGeom_HArray2OfSurface(1,myNbResultingRow,1,myNbResultingCol);
  Standard_Boolean isBSpline = mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface));
  Standard_Boolean isBezier  = mySurface->IsKind(STANDARD_TYPE(Geom_BezierSurface));
  
 // Standard_Real U1,U2,V1,V2;
 // U1=UFirst;
 // U2 = ULast;
 // V1 = VFirst;
 // V2 = VLast;

  if(myNbResultingRow == 1 && myNbResultingCol == 1) {
    mySurface->Bounds(U1, U2, V1, V2);
    Standard_Boolean filled = Standard_True;
    if ( Abs ( U1 - UFirst ) < Precision::PConfusion() && 
	 Abs ( U2 - ULast  ) < Precision::PConfusion() &&
         Abs ( V1 - VFirst ) < Precision::PConfusion() && 
	 Abs ( V2 - VLast  ) < Precision::PConfusion() )
      Surfaces->SetValue(1,1,mySurface);
    else if ( ! Segment || ! mySurface->IsKind (STANDARD_TYPE (Geom_BSplineSurface) ) ||
	      ! Status ( ShapeExtend_DONE2 ) ) {
      //pdn copying of surface
      Handle(Geom_Surface) tmp = Handle(Geom_Surface)::DownCast(mySurface->Copy());
      Handle(Geom_RectangularTrimmedSurface) Surf= 
	new Geom_RectangularTrimmedSurface(tmp,UFirst,ULast,VFirst,VLast);
      Surfaces->SetValue(1,1,Surf);  
    }
    else filled = Standard_False;
    if ( filled ) {
      myResSurfaces->Init(Surfaces);
      myResSurfaces->SetUFirstValue ( myUSplitValues->Sequence().First() );
      myResSurfaces->SetVFirstValue ( myVSplitValues->Sequence().First() );
      return;
    }
  }
  if (mySurface->IsKind (STANDARD_TYPE (Geom_BSplineSurface))) {
    Handle(Geom_BSplineSurface) BsSurface = Handle(Geom_BSplineSurface)::DownCast(mySurface->Copy());
    Standard_Integer FirstInd =BsSurface->FirstUKnotIndex(), 
    LastInd = BsSurface->LastUKnotIndex();
    Standard_Integer j =  FirstInd;
    for(Standard_Integer ii =1 ; ii <= myUSplitValues->Length(); ii++) {
      Standard_Real spval = myUSplitValues->Value(ii);
      for(; j <=LastInd;j++) {
	if( spval > BsSurface->UKnot(j) + Precision::PConfusion()) continue;
	if( spval < BsSurface->UKnot(j) - Precision::PConfusion()) break;
	 myUSplitValues->ChangeValue(ii) = BsSurface->UKnot(j);
       }
       if(j == LastInd) break;
     }
     FirstInd =BsSurface->FirstVKnotIndex(), 
     LastInd = BsSurface->LastVKnotIndex(); 
     j = FirstInd;
     for(Standard_Integer ii1 =1 ; ii1 <= myVSplitValues->Length(); ii1++) {
       Standard_Real spval = myVSplitValues->Value(ii1);
       for(; j <=LastInd;j++) {
	 if( spval > BsSurface->VKnot(j) + Precision::PConfusion()) continue;
	if( spval < BsSurface->VKnot(j) - Precision::PConfusion()) break;
	myVSplitValues->ChangeValue(ii1) =BsSurface->VKnot(j);
      }
      if(j == LastInd) break;
    }
  }
  U1 = myUSplitValues->Value(1);
  V1 = myVSplitValues->Value(1);
  for(Standard_Integer  irow = 2; irow <= myUSplitValues->Length(); irow++) {
    U2 = myUSplitValues->Value(irow);
    for(Standard_Integer  icol = 2; icol <= myVSplitValues->Length(); icol++) {
      V2 = myVSplitValues->Value(icol);
//      if (ShapeUpgrade::Debug())  {
//	cout<<".. bounds    ="<<U1    <<","<<U2   <<","<<V1    <<","<<V2   <<endl;
//	cout<<".. -> pos ="<<irow  <<","<<icol<<endl;
//      }
      // creates a copy of theSurf before to segment:
      Handle(Geom_Surface) theNew = Handle(Geom_Surface)::DownCast ( mySurface->Copy() );
      if ( isBSpline || isBezier ) {
	try {
	  OCC_CATCH_SIGNALS
	  if ( isBSpline ) 
	    Handle(Geom_BSplineSurface)::DownCast(theNew)->Segment(U1,U2,V1,V2);
	  else if ( isBezier ) {
	    //pdn K4L+ (work around)
	    // Standard_Real u1 = 2*U1 - 1;
	    // Standard_Real u2 = 2*U2 - 1;
	    // Standard_Real v1 = 2*V1 - 1;
	    // Standard_Real v2 = 2*V2 - 1; 
	    //rln C30 (direct use)
	    Standard_Real u1 = U1;
	    Standard_Real u2 = U2;
	    Standard_Real v1 = V1;
	    Standard_Real v2 = V2;
	    Handle(Geom_BezierSurface)::DownCast(theNew)->Segment(u1,u2,v1,v2);
	  }
	  myStatus |= ShapeExtend::EncodeStatus ( ShapeExtend_DONE3 );
	}
	catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	  cout << "Warning: ShapeUpgrade_SplitSurface::Build(): Exception in Segment      :";
	  anException.Print(cout); cout << endl;
#endif
	  (void)anException;
	  Handle(Geom_Surface) theNewSurf = theNew;
	  theNew = new Geom_RectangularTrimmedSurface(theNewSurf,U1,U2,V1,V2);
	}
	Surfaces->SetValue((irow-1),(icol-1),theNew);
      }
      else {
	// not a BSpline: trimming instead of segmentation
	Handle(Geom_RectangularTrimmedSurface) SplittedSurf= 
	  new Geom_RectangularTrimmedSurface(theNew,U1,U2,V1,V2);
	Surfaces->SetValue((irow-1),(icol-1),SplittedSurf);
      }
      
      V1=V2;
    }
    U1=U2; 
    V1 = myVSplitValues->Value(1);
  }
  Standard_Integer nbU =  myUSplitValues->Length();
  TColStd_Array1OfReal UJoints(1,nbU);
  Standard_Integer i;//svv Jan 10 2000 : porting on DEC
  for(i = 1; i <= nbU; i++)
    UJoints(i) = myUSplitValues->Value(i);
  
  Standard_Integer nbV=  myVSplitValues->Length();
  TColStd_Array1OfReal VJoints(1,nbV);
  for(i = 1; i <= nbV; i++)
    VJoints(i) = myVSplitValues->Value(i);
  myResSurfaces->Init(Surfaces,UJoints,VJoints);
//  if (ShapeUpgrade::Debug()) cout<<"SplitSurface::Build - end"<<endl;
}


//=======================================================================
//function : GlobalUKnots
//purpose  : 
//=======================================================================

const Handle(TColStd_HSequenceOfReal)& ShapeUpgrade_SplitSurface::USplitValues() const
{
  return myUSplitValues;
}
//=======================================================================
//function : GlobalVKnots
//purpose  : 
//=======================================================================

const Handle(TColStd_HSequenceOfReal)& ShapeUpgrade_SplitSurface::VSplitValues() const
{
  return myVSplitValues;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void ShapeUpgrade_SplitSurface::Perform(const Standard_Boolean Segment) 
{
  Compute(Segment);
//  SetUSplitValues(myUSplitValues);
//  SetVSplitValues(myVSplitValues);
  Build (Segment);
  
}
//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
 
void ShapeUpgrade_SplitSurface::Compute(const Standard_Boolean /*Segment*/)
{
  myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK); 
}
//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

Standard_Boolean ShapeUpgrade_SplitSurface::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus (myStatus, status);
}

const Handle(ShapeExtend_CompositeSurface)& ShapeUpgrade_SplitSurface::ResSurfaces() const
{
  return myResSurfaces;
}
