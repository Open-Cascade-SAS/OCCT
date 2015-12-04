// Created on: 1999-05-21
// Created by: Pavel DURANDIN
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

//   svv  10.01.00 porting on DEC

#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert_BSplineSurfaceToBezierSurface.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeUpgrade_ConvertCurve3dToBezier.hxx>
#include <ShapeUpgrade_ConvertSurfaceToBezierBasis.hxx>
#include <Standard_Type.hxx>
#include <TColGeom_Array2OfBezierSurface.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColGeom_HArray2OfSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ConvertSurfaceToBezierBasis,ShapeUpgrade_SplitSurface)

ShapeUpgrade_ConvertSurfaceToBezierBasis::ShapeUpgrade_ConvertSurfaceToBezierBasis()
{
  myPlaneMode      = Standard_True;
  myRevolutionMode = Standard_True;
  myExtrusionMode  = Standard_True;
  myBSplineMode    = Standard_True;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void ShapeUpgrade_ConvertSurfaceToBezierBasis::Compute(const Standard_Boolean Segment)
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
  Standard_Real precision = Precision::PConfusion();
  
  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) Surface = Handle(Geom_RectangularTrimmedSurface)::DownCast(mySurface);
    Handle(Geom_Surface) BasSurf = Surface->BasisSurface();
    ShapeUpgrade_ConvertSurfaceToBezierBasis converter;
    converter.Init(BasSurf,UFirst,ULast,VFirst,VLast);
    converter.SetUSplitValues(myUSplitValues);
    converter.SetVSplitValues(myVSplitValues);
    converter.Compute(Standard_True);
    myUSplitValues->ChangeSequence() = converter.USplitValues()->Sequence();
    myVSplitValues->ChangeSequence() = converter.VSplitValues()->Sequence();
    myStatus |= converter.myStatus;
    mySegments = converter.Segments();
    return;
  } else if(mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) Offset = Handle(Geom_OffsetSurface)::DownCast(mySurface);
    Handle(Geom_Surface) BasSurf = Offset->BasisSurface();
    ShapeUpgrade_ConvertSurfaceToBezierBasis converter;
    converter.Init(BasSurf,UFirst,ULast,VFirst,VLast);
    converter.SetUSplitValues(myUSplitValues);
    converter.SetVSplitValues(myVSplitValues);
    converter.Compute(Standard_True);
    myUSplitValues->ChangeSequence() = converter.USplitValues()->Sequence();
    myVSplitValues->ChangeSequence() = converter.VSplitValues()->Sequence();
    myStatus |= converter.myStatus;
    mySegments = converter.Segments();
    return;
  } else if(mySurface->IsKind(STANDARD_TYPE(Geom_Plane))&&myPlaneMode) {
    Handle(Geom_Plane) pln = Handle(Geom_Plane)::DownCast(mySurface);
    TColgp_Array2OfPnt poles(1,2,1,2);
    gp_Pnt dp;
    poles(1,1) = dp = pln->Value(UFirst,VFirst); poles(1,2) = dp = pln->Value(UFirst,VLast);
    poles(2,1) = dp = pln->Value(ULast,VFirst);  poles(2,2) = dp = pln->Value(ULast,VLast);
    Handle(Geom_BezierSurface) bezier = new Geom_BezierSurface(poles);
    TColStd_Array1OfReal UJoints(1,2);
    UJoints(1) = UFirst; UJoints(2) = ULast;
    TColStd_Array1OfReal VJoints(1,2);
    VJoints(1) = VFirst; VJoints(2) = VLast;
    Handle(TColGeom_HArray2OfSurface) surf = new TColGeom_HArray2OfSurface(1,1,1,1);
    surf->SetValue(1,1,bezier);
    mySegments = new ShapeExtend_CompositeSurface(surf,UJoints,VJoints);
    myStatus = ShapeExtend::EncodeStatus ( ShapeExtend_DONE1 );
    return;
  } else if(mySurface->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
    Handle(Geom_BezierSurface) bezier = Handle(Geom_BezierSurface)::DownCast(mySurface);
    Handle(TColGeom_HArray2OfSurface) surf = new TColGeom_HArray2OfSurface(1,1,1,1);
    TColStd_Array1OfReal UJoints(1,2);
    UJoints(1) = UFirst; UJoints(2) = ULast;
    TColStd_Array1OfReal VJoints(1,2);
    VJoints(1) = VFirst; VJoints(2) = VLast;
    if(UFirst < precision && ULast > 1 - precision &&
       VFirst < precision && VLast > 1 - precision) {
      surf->SetValue(1,1,bezier);
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
    } else {
      Handle(Geom_BezierSurface) besNew = Handle(Geom_BezierSurface)::DownCast(bezier->Copy());
      //pdn K4L+ (work around)
      // Standard_Real u1 = 2*UFirst - 1;
      // Standard_Real u2 = 2*ULast - 1;
      // Standard_Real v1 = 2*VFirst - 1;
      // Standard_Real v2 = 2*VLast - 1;
      //rln C30 (direct use)
      Standard_Real u1 = UFirst;
      Standard_Real u2 = ULast;
      Standard_Real v1 = VFirst;
      Standard_Real v2 = VLast;
      besNew->Segment(u1,u2,v1,v2);
      surf->SetValue(1,1,besNew);
      myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2);
    }
    mySegments = new ShapeExtend_CompositeSurface(surf,UJoints,VJoints);
    return;
  } else if(mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))&&myBSplineMode) {
    Handle(Geom_BSplineSurface) bspline = Handle(Geom_BSplineSurface)::DownCast(mySurface);
    //pdn
    Standard_Real u1,u2,v1,v2;
    bspline->Bounds(u1,u2,v1,v2);
    GeomConvert_BSplineSurfaceToBezierSurface converter(bspline);//,UFirst,ULast,VFirst,VLast,precision;
    Standard_Integer nbUPatches = converter.NbUPatches();
    Standard_Integer nbVPatches = converter.NbVPatches();
    TColStd_Array1OfReal UJoints(1, nbUPatches+1);
    TColStd_Array1OfBoolean UReject(1, nbUPatches+1);
    UReject.Init(Standard_False);
    TColStd_Array1OfReal VJoints(1, nbVPatches+1);
    TColStd_Array1OfBoolean VReject(1, nbVPatches+1);
    VReject.Init(Standard_False);
    Standard_Integer NbUFiltered = 0;
    Standard_Integer NbVFiltered = 0;
    
    converter.UKnots(UJoints);
    TColStd_SequenceOfReal UFilteredJoints;
    UFilteredJoints.Append(UJoints(1));
    Standard_Integer i;
    for(i = 2; i <= nbUPatches+1; i++)
      if(UJoints(i)-UJoints(i-1) < precision) {
	NbUFiltered++;
	UReject(i-1) = Standard_True;
      }
      else
	UFilteredJoints.Append(UJoints(i));
    
    converter.VKnots(VJoints);
    TColStd_SequenceOfReal VFilteredJoints;
    VFilteredJoints.Append(VJoints(1));
    for( i = 2; i <= nbVPatches+1; i++)
      if(VJoints(i)-VJoints(i-1) < precision) {
	NbVFiltered++;
	VReject(i-1) = Standard_True;
      }
      else
	VFilteredJoints.Append(VJoints(i));

#ifdef OCCT_DEBUG
    if(NbVFiltered || NbUFiltered)
      cout<<"Warning: ShapeUpgrade_ConvertSurfaceToBezierBasis: thin patches dropped."<<endl;
#endif
    
    TColGeom_Array2OfBezierSurface Surfaces(1, nbUPatches, 1, nbVPatches);
    converter.Patches(Surfaces);
    Handle(TColGeom_HArray2OfSurface) srf = 
      new TColGeom_HArray2OfSurface(1,nbUPatches-NbUFiltered,1,nbVPatches-NbVFiltered);
    Standard_Integer indApp1 = 0;
    for(Standard_Integer ind1 = 1; ind1 <= nbUPatches; ind1++) {
      if(UReject(ind1)) continue;
      indApp1++;
      Standard_Integer indApp2 = 0;
      for(Standard_Integer ind2 = 1; ind2 <= nbVPatches; ind2++) {
	if(VReject(ind2)) continue;
	indApp2++;
	srf->SetValue(indApp1,indApp2,Surfaces(ind1,ind2));
      }
    }
    
    TColStd_Array1OfReal uj (1,UFilteredJoints.Length());
    for(i = 1; i <=UFilteredJoints.Length(); i++)
      uj(i) = UFilteredJoints.Value(i);
    
    TColStd_Array1OfReal vj (1,VFilteredJoints.Length());
    for(i = 1; i <=VFilteredJoints.Length(); i++)
      vj(i) = VFilteredJoints.Value(i);
    
    mySegments = new ShapeExtend_CompositeSurface(srf,uj,vj);
    
    Standard_Integer j; // svv #1
    for(j = 2; j <= myUSplitValues->Length(); j++) {
      ULast =  myUSplitValues->Value(j);
      for(Standard_Integer ii = 2; ii <= nbUPatches+1; ii++) {
	Standard_Real valknot = UJoints(ii);
	if(valknot-UFirst <=  precision) continue;
	if(ULast -valknot <=  precision) break;
	myUSplitValues->InsertBefore(j++,valknot);
      }
      UFirst = ULast;
    }
    for(j = 2; j <= myVSplitValues->Length(); j++) {
      VLast =  myVSplitValues->Value(j);
      for(Standard_Integer ii = 2; ii <= nbVPatches+1; ii++) {
	Standard_Real valknot = VJoints(ii);
	if(valknot-VFirst <= precision) continue;
	if(VLast -valknot <= precision) break;
	myVSplitValues->InsertBefore(j++,valknot);
      }
      VFirst = VLast;
    }
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
    return;
  } else if(mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))&&myRevolutionMode) {
    Handle(Geom_SurfaceOfRevolution) revol = Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
    Handle(Geom_Curve) basis = revol->BasisCurve();
    if(basis->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(basis);
      basis = tc->BasisCurve();
    }
    Handle(TColGeom_HArray1OfCurve) curves;
    Standard_Integer nbCurves;
    Handle(TColStd_HSequenceOfReal) vPar = new TColStd_HSequenceOfReal;
    Handle(TColStd_HSequenceOfReal) vSVal= new TColStd_HSequenceOfReal;
    if(basis->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
      Handle(Geom_OffsetCurve) offset = Handle(Geom_OffsetCurve)::DownCast(basis);
      Standard_Real value = offset->Offset();
      gp_Dir direction = offset->Direction();
      Handle(Geom_Curve) bas = offset->BasisCurve();
      ShapeUpgrade_ConvertCurve3dToBezier converter;
      converter.Init(bas,VFirst,VLast);
      converter.Perform(Standard_True);
      if(converter.Status(ShapeExtend_DONE)) 
	myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
      else
	myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
      
      vPar->ChangeSequence() = converter.SplitParams()->Sequence();
      vSVal->ChangeSequence()= converter.SplitValues()->Sequence();
      curves = converter.GetCurves();
      nbCurves = curves->Length();
      for(Standard_Integer i = 1; i <= nbCurves; i++) {
	Handle(Geom_OffsetCurve) offCur = new Geom_OffsetCurve(curves->Value(i),value,direction);
	curves->SetValue(i,offCur);
      }
    } else {
      ShapeUpgrade_ConvertCurve3dToBezier converter;
      converter.Init(basis,VFirst,VLast);
      converter.Perform(Standard_True);
      if(converter.Status(ShapeExtend_DONE)) 
	myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
      else
	myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);

      vPar->ChangeSequence() = converter.SplitParams()->Sequence();
      vSVal->ChangeSequence()= converter.SplitValues()->Sequence();
      curves = converter.GetCurves();
      nbCurves = curves->Length();
    }
    
    gp_Ax1 axis = revol->Axis();
    Handle(TColGeom_HArray2OfSurface) surf = new TColGeom_HArray2OfSurface(1,1,1,nbCurves);
    Standard_Real Umin,Umax,Vmin,Vmax;
    mySurface->Bounds(Umin,Umax,Vmin,Vmax);
    Standard_Integer i; // svv #1
    for(i = 1; i <= nbCurves; i++) {
      Handle(Geom_SurfaceOfRevolution) rev = new Geom_SurfaceOfRevolution(curves->Value(i),axis);
      if( UFirst-Umin < Precision::PConfusion() &&
	 Umax-ULast < Precision::PConfusion() )
	surf->SetValue(1,i,rev);
      else {
	Handle(Geom_RectangularTrimmedSurface) rect = new Geom_RectangularTrimmedSurface(rev,UFirst,ULast,Standard_True);
	surf->SetValue(1,i,rect);
      }
    }
    TColStd_Array1OfReal UJoints(1, 2);
    TColStd_Array1OfReal VJoints(1, nbCurves+1);
    UJoints(1) = UFirst;  UJoints(2) = ULast;
    for(i = 1 ; i <= nbCurves+1; i++)
      VJoints(i) = vPar->Value(i);
    
    mySegments = new ShapeExtend_CompositeSurface(surf,UJoints,VJoints);
    
    for(Standard_Integer  j = 2; j <= myVSplitValues->Length(); j++) {
      VLast =  myVSplitValues->Value(j);
      for(Standard_Integer ii = 2; ii <= nbCurves+1; ii++) {
	Standard_Real valknot = vSVal->Value(ii);
	if(valknot-VFirst <= precision) continue;
	if(VLast -valknot <= precision) break;
	myVSplitValues->InsertBefore(j++,valknot);
      }
      VFirst = VLast;
    }
    return;
  } else if(mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))&&myExtrusionMode) {
    Handle(Geom_SurfaceOfLinearExtrusion) extr = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface);
    Handle(Geom_Curve) basis = extr->BasisCurve();
    //gp_Dir direction = extr->Direction(); // direction not used (skl)
    
    Handle(TColGeom_HArray1OfCurve) curves;
    Standard_Integer nbCurves;
    Handle(TColStd_HSequenceOfReal) uPar = new TColStd_HSequenceOfReal;
    Handle(TColStd_HSequenceOfReal) uSVal= new TColStd_HSequenceOfReal;
    ShapeUpgrade_ConvertCurve3dToBezier converter;
    converter.Init(basis,UFirst,ULast);
    converter.Perform(Standard_True);
    uPar->ChangeSequence() = converter.SplitParams()->Sequence();
    uSVal->ChangeSequence()= converter.SplitValues()->Sequence();
    curves = converter.GetCurves();
    nbCurves = curves->Length();
    
    gp_Trsf shiftF,shiftL;
    shiftF.SetTranslation(extr->Value(UFirst,0),extr->Value(UFirst,VFirst));
    shiftL.SetTranslation(extr->Value(UFirst,0),extr->Value(UFirst,VLast));
    Handle(TColGeom_HArray2OfSurface) surf = new TColGeom_HArray2OfSurface(1,nbCurves,1,1);
    
    Standard_Integer i; // svv #1
    for(i = 1; i <= nbCurves; i++) {
      Handle(Geom_BezierCurve) bez = Handle(Geom_BezierCurve)::DownCast(curves->Value(i));
      Standard_Integer nbPoles = bez->NbPoles();
      TColgp_Array1OfPnt poles(1,nbPoles);
      bez->Poles(poles);
      TColgp_Array2OfPnt resPoles(1,nbPoles,1,2);
      for(Standard_Integer j = 1; j <= nbPoles; j++) {
	resPoles(j,1) = poles(j).Transformed(shiftF);
	resPoles(j,2) = poles(j).Transformed(shiftL);
      }
      Handle(Geom_BezierSurface) bezier = new Geom_BezierSurface(resPoles);
      surf->SetValue(i,1,bezier);
    }
    
    TColStd_Array1OfReal UJoints(1, nbCurves+1);
    TColStd_Array1OfReal VJoints(1, 2);
    VJoints(1) = VFirst;  VJoints(2) = VLast;
    for(i = 1 ; i <= nbCurves+1; i++)
      UJoints(i) = uPar->Value(i);
    
    mySegments = new ShapeExtend_CompositeSurface(surf,UJoints,VJoints);
    
    for(Standard_Integer j = 2; j <= myUSplitValues->Length(); j++) {
      ULast =  myUSplitValues->Value(j);
      for(Standard_Integer ii = 2; ii <= nbCurves+1; ii++) {
	Standard_Real valknot = uSVal->Value(ii);
	if(valknot+UFirst <= precision) continue;
	if(ULast -valknot <= precision) break;
	myUSplitValues->InsertBefore(j++,valknot);
      }
      UFirst = ULast;
    }
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
    return;
  }
  else {
    TColStd_Array1OfReal UJoints(1,2);
    UJoints(1) = UFirst; UJoints(2) = ULast;
    TColStd_Array1OfReal VJoints(1,2);
    VJoints(1) = VFirst; VJoints(2) = VLast;
    Handle(TColGeom_HArray2OfSurface) surf = new TColGeom_HArray2OfSurface(1,1,1,1);
    Standard_Real U1,U2,V1,V2;
    mySurface->Bounds(U1,U2,V1,V2);
    Handle(Geom_Surface) S;
    if(U1-UFirst < precision && ULast - U2 < precision &&
       V2-VFirst < precision && VLast - V2 < precision)
      S = mySurface;
    else {
      Handle(Geom_RectangularTrimmedSurface) rts = new Geom_RectangularTrimmedSurface(mySurface,UFirst,ULast,VFirst,VLast);
      S = rts;
    }
    surf->SetValue(1,1,S);
    mySegments = new ShapeExtend_CompositeSurface(surf,UJoints,VJoints);
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
    return;
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

static Handle(Geom_Surface) GetSegment(const Handle(Geom_Surface) surf,
				       const Standard_Real U1,
				       const Standard_Real U2,
				       const Standard_Real V1,
				       const Standard_Real V2)
{
  if(surf->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
    Handle(Geom_BezierSurface) bezier = Handle(Geom_BezierSurface)::DownCast(surf->Copy());
    Standard_Real prec = Precision::PConfusion();
    if(U1 < prec && U2 > 1-prec && V1 < prec && V2 > 1-prec)
      return bezier;
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
    bezier->Segment(u1,u2,v1,v2);
    return bezier;
  }
  
  Handle(Geom_Surface) S;
  if(surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) rect = Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);
    S = rect->BasisSurface();
  } else 
    S = surf;
  
  if(S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) revol = Handle(Geom_SurfaceOfRevolution)::DownCast(S->Copy());
    Standard_Real Umin,Umax,Vmin,Vmax;
    revol->Bounds(Umin,Umax,Vmin,Vmax);
    Handle(Geom_Curve) basis = revol->BasisCurve();
    if(basis->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
      Handle(Geom_OffsetCurve) offset = Handle(Geom_OffsetCurve)::DownCast(basis);
      basis = offset->BasisCurve();
    }
    if(basis->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
      Handle(Geom_BezierCurve) bezier = Handle(Geom_BezierCurve)::DownCast(basis);
      bezier->Segment(V1,V2);    
    }
    else {
#ifdef OCCT_DEBUG
      cout <<"Warning: Resulting path is not surface of revolution basis on bezier curve"<<endl;
#endif
    }
    if(Abs(U1-Umin) < Precision::PConfusion() &&
       Abs(U2-Umax) < Precision::PConfusion() )
      return revol;
        
    Handle(Geom_RectangularTrimmedSurface) res = new Geom_RectangularTrimmedSurface(revol,U1,U2,Standard_True);
    return res;
  }
  else {
    Standard_Real Umin,Umax,Vmin,Vmax;
    surf->Bounds(Umin,Umax,Vmin,Vmax);
    if( U1-Umin < Precision::PConfusion() &&
        Umax-U2 < Precision::PConfusion() &&
        V1-Vmin < Precision::PConfusion() &&
        Vmax-V2 < Precision::PConfusion() )
      return surf;
    
    Handle(Geom_RectangularTrimmedSurface) res = new Geom_RectangularTrimmedSurface(surf,U1,U2,V1,V2);
    return res;
  }
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void ShapeUpgrade_ConvertSurfaceToBezierBasis::Build(const Standard_Boolean /*Segment*/)
{
  Standard_Boolean isOffset = Standard_False;
  Standard_Real offsetValue=0;
  Handle(Geom_Surface) S;
  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) Surface = Handle(Geom_RectangularTrimmedSurface)::DownCast(mySurface);
    S = Surface->BasisSurface();
  }
  else 
    S = mySurface;
  if(S->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) offSur = Handle(Geom_OffsetSurface)::DownCast(S);
    isOffset = Standard_True;
    offsetValue = offSur->Offset();
  }

  Standard_Real prec = Precision::PConfusion();
  Handle(TColStd_HArray1OfReal) myUSplitParams = mySegments->UJointValues();
  Handle(TColStd_HArray1OfReal) myVSplitParams = mySegments->VJointValues();
  Standard_Integer nbU = myUSplitValues->Length();
  Standard_Integer nbV = myVSplitValues->Length();
  
  Handle(TColGeom_HArray2OfSurface) resSurfaces = new TColGeom_HArray2OfSurface(1,nbU-1,1,nbV-1);
  Standard_Integer j1 = 2;
  for(Standard_Integer i1 = 2; i1 <= nbU; i1++) {
    Standard_Real parU = myUSplitValues->Value(i1);
    for(; j1<= myUSplitParams->Length(); j1++) {
      Standard_Real param = myUSplitParams->Value(j1);
      if(parU - param < prec)
	break;
    }
    
    Standard_Integer j2 = 2;
    for(Standard_Integer i2 = 2; i2 <= nbV; i2++) {
      Standard_Real parV = myVSplitValues->Value(i2);
      for(; j2<= myVSplitParams->Length(); j2++) 
	if(parV - myVSplitParams->Value(j2) < prec)
	  break;
      
      Handle(Geom_Surface) patch = mySegments->Patch(j1-1,j2-1);
      Standard_Real U1, U2, V1, V2;
      patch->Bounds(U1,U2,V1,V2);
      //linear recomputation of part:
      Standard_Real uFirst = myUSplitParams->Value(j1-1);
      Standard_Real uLast  = myUSplitParams->Value(j1);
      Standard_Real vFirst = myVSplitParams->Value(j2-1);
      Standard_Real vLast  = myVSplitParams->Value(j2);
      Standard_Real uFact = (U2-U1)/(uLast - uFirst);
      Standard_Real vFact = (V2-V1)/(vLast - vFirst);
      Standard_Real ppU = myUSplitValues->Value(i1-1);
      Standard_Real ppV = myVSplitValues->Value(i2-1);
      //defining a part
      Standard_Real uL1 = U1+(ppU - uFirst)*uFact;
      Standard_Real uL2 = U1+(parU- uFirst)*uFact;
      Standard_Real vL1 = V1+(ppV - vFirst)*vFact;
      Standard_Real vL2 = V1+(parV- vFirst)*vFact;
      Handle(Geom_Surface) res = GetSegment(patch,uL1, uL2, vL1, vL2);
      if(isOffset) {
	Handle(Geom_OffsetSurface) resOff = new Geom_OffsetSurface(res,offsetValue);
	res = resOff;
      }
      resSurfaces->SetValue(i1-1,i2-1,res);
    }
  }
  
  TColStd_Array1OfReal UJoints(1,nbU);
  Standard_Integer i; // svv #1
  for(i = 1; i <= nbU; i++)
    UJoints(i) = myUSplitValues->Value(i);
  
  TColStd_Array1OfReal VJoints(1,nbV);
  for(i = 1; i <= nbV; i++)
    VJoints(i) = myVSplitValues->Value(i);
      
  myResSurfaces = new ShapeExtend_CompositeSurface(resSurfaces,UJoints,VJoints);
}

//=======================================================================
//function : Segments
//purpose  : 
//=======================================================================

Handle(ShapeExtend_CompositeSurface) ShapeUpgrade_ConvertSurfaceToBezierBasis::Segments() const
{
  return mySegments;
}
