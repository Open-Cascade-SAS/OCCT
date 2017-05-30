// Created on: 1995-08-07
// Created by: Modelistation
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


#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Hatch_Hatcher.hxx>
#include <Precision.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <StdPrs_WFDeflectionRestrictedFace.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

#ifdef OCCT_DEBUG_MESH
#include <OSD_Chronometer.hxx>
extern OSD_Chronometer FFaceTimer1,FFaceTimer2,FFaceTimer3,FFaceTimer4;
#endif


//==================================================================
// function: FindLimits
// purpose:
//==================================================================
static void FindLimits(const Adaptor3d_Curve& aCurve,
		       const Standard_Real  aLimit,
		       Standard_Real&       First,
		       Standard_Real&       Last)
{
  First = Max(aCurve.FirstParameter(), First);
  Last  = Min(aCurve.LastParameter(), Last);
  Standard_Boolean firstInf = Precision::IsNegativeInfinite(First);
  Standard_Boolean lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf) {
    gp_Pnt P1,P2;
    Standard_Real delta = 1;
    if (firstInf && lastInf) {
      do {
	delta *= 2;
	First = - delta;
	Last  =   delta;
	aCurve.D0(First,P1);
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf) {
      aCurve.D0(Last,P2);
      do {
	delta *= 2;
	First = Last - delta;
	aCurve.D0(First,P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf) {
      aCurve.D0(First,P1);
      do {
	delta *= 2;
	Last = First + delta;
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }

}


//=========================================================================
// function: Add
// purpose
//=========================================================================
void StdPrs_WFDeflectionRestrictedFace::Add
  (const Handle (Prs3d_Presentation)& aPresentation,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Standard_Boolean DrawUIso,
   const Standard_Boolean DrawVIso,
   const Standard_Real Deflection,
   const Standard_Integer NBUiso,
   const Standard_Integer NBViso,
   const Handle(Prs3d_Drawer)& aDrawer,
   Prs3d_NListOfSequenceOfPnt& Curves) {

#ifdef OCCT_DEBUG_MESH
  FFaceTimer1.Start();
#endif

  StdPrs_ToolRFace ToolRst (aFace);
  Standard_Real UF, UL, VF, VL;
  UF = aFace->FirstUParameter();
  UL = aFace->LastUParameter();
  VF = aFace->FirstVParameter();
  VL = aFace->LastVParameter();

  Standard_Real aLimit = aDrawer->MaximalParameterValue();

  // compute bounds of the restriction
  Standard_Real UMin,UMax,VMin,VMax;
  //Standard_Real u,v,step;
  Standard_Integer i;//,nbPoints = 10;

  UMin = Max(UF, -aLimit);
  UMax = Min(UL, aLimit);
  VMin = Max(VF, -aLimit);
  VMax = Min(VL, aLimit);


  // update min max for the hatcher.
  gp_Pnt2d P1,P2;
  Standard_Real U1, U2;
  gp_Pnt dummypnt;
  Standard_Real ddefle= Max(UMax-UMin, VMax-VMin) * aDrawer->DeviationCoefficient();
  TColgp_SequenceOfPnt2d tabP;
  Standard_Real aHatchingTol = 1.e100;

  UMin = VMin = 1.e100;
  UMax = VMax = -1.e100;
  
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next()) {
    TopAbs_Orientation Orient = ToolRst.Orientation();
    if ( Orient == TopAbs_FORWARD || Orient == TopAbs_REVERSED ) {
      Adaptor2d_Curve2dPtr TheRCurve = ToolRst.Value();
      if (TheRCurve->GetType() != GeomAbs_Line) {
        GCPnts_QuasiUniformDeflection UDP(*TheRCurve, ddefle);
	if (UDP.IsDone()) {
	  Standard_Integer NumberOfPoints = UDP.NbPoints();
	  if ( NumberOfPoints >= 2 ) {
	    dummypnt = UDP.Value(1);
	    P2.SetCoord(dummypnt.X(), dummypnt.Y());
	    UMin = Min(P2.X(), UMin);
	    UMax = Max(P2.X(), UMax);
	    VMin = Min(P2.Y(), VMin);
	    VMax = Max(P2.Y(), VMax);
	    for (i = 2; i <= NumberOfPoints; i++) {
	      P1 = P2;
	      dummypnt = UDP.Value(i);
	      P2.SetCoord(dummypnt.X(), dummypnt.Y());
	      UMin = Min(P2.X(), UMin);
	      UMax = Max(P2.X(), UMax);
	      VMin = Min(P2.Y(), VMin);
	      VMax = Max(P2.Y(), VMax);
              aHatchingTol = Min(P1.SquareDistance(P2), aHatchingTol);

	      if(Orient == TopAbs_FORWARD ) {
		//isobuild.Trim(P1,P2);
		tabP.Append(P1);
		tabP.Append(P2);
	      }
	      else {
		//isobuild.Trim(P2,P1);
		tabP.Append(P2);
		tabP.Append(P1);
	      }
	    }
	  }
	}
#ifdef OCCT_DEBUG
	else {
	  cout << "Cannot evaluate curve on surface"<<endl;
	}
#endif
      }
      else {
	U1 = TheRCurve->FirstParameter();
	U2 = TheRCurve->LastParameter();
        // MSV 17.08.06 OCC13144: U2 occured less than U1, to overcome it
        // ensure that distance U2-U1 is not greater than aLimit*2,
        // if greater then choose an origin and use aLimit to define
        // U1 and U2 anew
        Standard_Real aOrigin = 0.;
        if (!Precision::IsNegativeInfinite(U1) || !Precision::IsPositiveInfinite(U2)) {
          if (Precision::IsNegativeInfinite(U1))
            aOrigin = U2 - aLimit;
          else if (Precision::IsPositiveInfinite(U2))
            aOrigin = U1 + aLimit;
          else
            aOrigin = (U1 + U2) * 0.5;
        }
	U1 = Max(aOrigin - aLimit, U1);
	U2 = Min(aOrigin + aLimit, U2);
	P1 = TheRCurve->Value(U1);
	P2 = TheRCurve->Value(U2);
	UMin = Min(P1.X(), UMin);
	UMax = Max(P1.X(), UMax);
	VMin = Min(P1.Y(), VMin);
	VMax = Max(P1.Y(), VMax);
	UMin = Min(P2.X(), UMin);
	UMax = Max(P2.X(), UMax);
	VMin = Min(P2.Y(), VMin);
	VMax = Max(P2.Y(), VMax);
        aHatchingTol = Min(P1.SquareDistance(P2), aHatchingTol);

	if(Orient == TopAbs_FORWARD ) {
	 // isobuild.Trim(P1,P2);
	  tabP.Append(P1);
	  tabP.Append(P2);
	}
	else {
	  //isobuild.Trim(P2,P1);
	  tabP.Append(P2);
	  tabP.Append(P1);
	}
      }
    }
  }


#ifdef OCCT_DEBUG_MESH
  FFaceTimer1.Stop();

  FFaceTimer2.Start();
#endif

  // Compute the hatching tolerance.
  aHatchingTol *= 0.1;
  aHatchingTol = Max(Precision::Confusion(), aHatchingTol);
  aHatchingTol = Min(1.e-5, aHatchingTol);

  // load the isos
  Hatch_Hatcher isobuild(aHatchingTol, ToolRst.IsOriented());
  Standard_Boolean UClosed = aFace->IsUClosed();
  Standard_Boolean VClosed = aFace->IsVClosed();

  if ( ! UClosed ) {
    UMin = UMin + ( UMax - UMin) /1000.;
    UMax = UMax - ( UMax - UMin) /1000.; 
  }

  if ( ! VClosed ) {
    VMin = VMin + ( VMax - VMin) /1000.;
    VMax = VMax - ( VMax - VMin) /1000.; 
  }

  if (DrawUIso){
    if (NBUiso > 0) {
      UClosed = Standard_False; // En attendant un hatcher de course.
      Standard_Real du= UClosed ? (UMax-UMin)/NBUiso : (UMax-UMin)/(1+NBUiso);
      for (i=1; i<=NBUiso;i++){
	isobuild.AddXLine(UMin+du*i);
      }
    }
  }
  if (DrawVIso){
    if ( NBViso > 0) {
      VClosed = Standard_False;
      Standard_Real dv= VClosed ?(VMax-VMin)/NBViso : (VMax-VMin)/(1+NBViso);
      for (i=1; i<=NBViso;i++){
	isobuild.AddYLine(VMin+dv*i);
      }
    }
  }

#ifdef OCCT_DEBUG_MESH
  FFaceTimer2.Stop();
  FFaceTimer3.Start();
#endif

  
  Standard_Integer ll = tabP.Length();
  for (i = 1; i <= ll; i+=2) {
    isobuild.Trim(tabP(i),tabP(i+1));
  }


#ifdef OCCT_DEBUG_MESH  
  FFaceTimer3.Stop();
  FFaceTimer4.Start();
#endif

  // draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aFace);
  Handle(Geom_Curve) BC;
  const BRepAdaptor_Surface& BS = *(BRepAdaptor_Surface*)&(aFace->Surface());
  GeomAbs_SurfaceType thetype = aFace->GetType();

  Standard_Integer NumberOfLines = isobuild.NbLines();
  Handle(Geom_Surface) GB;
  if (thetype == GeomAbs_BezierSurface) {
    GB = BS.Bezier();
  }
  else if (thetype == GeomAbs_BSplineSurface){
    GB = BS.BSpline();
  }

  Standard_Real anAngle = aDrawer->DeviationAngle();

  for (i = 1; i <= NumberOfLines; i++) {
    Standard_Integer NumberOfIntervals = isobuild.NbIntervals(i);
    Standard_Real Coord = isobuild.Coordinate(i);
    for (Standard_Integer j = 1; j <= NumberOfIntervals; j++) {
      Standard_Real b1=isobuild.Start(i,j),b2=isobuild.End(i,j);


      if (!GB.IsNull()) {
	if (isobuild.IsXLine(i))
	  BC = GB->UIso(Coord);
	else 
	  BC = GB->VIso(Coord);
	GeomAdaptor_Curve GC(BC);
	FindLimits(GC, aLimit,b1, b2);
	if (b2-b1>Precision::Confusion()) {
	  Handle(TColgp_HSequenceOfPnt) Points = new TColgp_HSequenceOfPnt;
	  StdPrs_DeflectionCurve::Add (aPresentation, GC, b1, b2, Deflection, 
                                       Points->ChangeSequence(), anAngle, Standard_False);
	  Curves.Append(Points);
	}
      }
      else {
	if (isobuild.IsXLine(i))
	  anIso.Load(GeomAbs_IsoU,Coord,b1,b2);
	else
	  anIso.Load(GeomAbs_IsoV,Coord,b1,b2);
	FindLimits(anIso, aLimit,b1, b2);
	if (b2-b1>Precision::Confusion()) {
	  Handle(TColgp_HSequenceOfPnt) Points = new TColgp_HSequenceOfPnt;
	  StdPrs_DeflectionCurve::Add (aPresentation, anIso, b1, b2, Deflection, 
                                       Points->ChangeSequence(), anAngle, Standard_False);
	  Curves.Append(Points);
	}
      }
    }
  }
#ifdef OCCT_DEBUG_MESH
  FFaceTimer4.Stop();
#endif
}


//=========================================================================
// function: Match
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::Match
  (const Standard_Real X,
   const Standard_Real Y,
   const Standard_Real Z,
   const Standard_Real aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle(Prs3d_Drawer)& aDrawer,
   const Standard_Boolean DrawUIso,
   const Standard_Boolean DrawVIso,
   const Standard_Real Deflection,
   const Standard_Integer NBUiso,
   const Standard_Integer NBViso)
{

   StdPrs_ToolRFace ToolRst (aFace);
   const Standard_Real aLimit = aDrawer->MaximalParameterValue();

  // compute bounds of the restriction
  Standard_Real UMin,UMax,VMin,VMax;
  Standard_Real u,v,step;
  Standard_Integer i,nbPoints = 10;
  UMin = VMin = RealLast();
  UMax = VMax = RealFirst();
  
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next()) {
    Adaptor2d_Curve2dPtr TheRCurve = ToolRst.Value();
    u = TheRCurve->FirstParameter();
    v = TheRCurve->LastParameter();
    step = ( v - u) / nbPoints;
    for (i = 0; i <= nbPoints; i++) {
      gp_Pnt2d P = TheRCurve->Value(u);
      if (P.X() < UMin) UMin = P.X();
      if (P.X() > UMax) UMax = P.X();
      if (P.Y() < VMin) VMin = P.Y();
      if (P.Y() > VMax) VMax = P.Y();
      u += step;
    }
  }
  
  // load the isos
  Hatch_Hatcher isobuild(1.e-5,ToolRst.IsOriented());
  Standard_Boolean UClosed = aFace->IsUClosed();
  Standard_Boolean VClosed = aFace->IsVClosed();

  if ( ! UClosed ) {
    UMin = UMin + ( UMax - UMin) /1000.;
    UMax = UMax - ( UMax - UMin) /1000.; 
  }

  if ( ! VClosed ) {
    VMin = VMin + ( VMax - VMin) /1000.;
    VMax = VMax - ( VMax - VMin) /1000.; 
  }

  if (DrawUIso){
    if (NBUiso > 0) {
      UClosed = Standard_False; // En attendant un hatcher de course.
      Standard_Real du= UClosed ? (UMax-UMin)/NBUiso : (UMax-UMin)/(1+NBUiso);
      for (i=1; i<=NBUiso;i++){
	isobuild.AddXLine(UMin+du*i);
      }
    }
  }
  if (DrawVIso){
    if ( NBViso > 0) {
      VClosed = Standard_False;
      Standard_Real dv= VClosed ?(VMax-VMin)/NBViso : (VMax-VMin)/(1+NBViso);
      for (i=1; i<=NBViso;i++){
	isobuild.AddYLine(VMin+dv*i);
      }
    }
  }

  // trim the isos
  gp_Pnt2d P1,P2;
  gp_Pnt dummypnt;
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next()) {
    TopAbs_Orientation Orient = ToolRst.Orientation();
    if ( Orient == TopAbs_FORWARD || Orient == TopAbs_REVERSED ) {
      Adaptor2d_Curve2dPtr TheRCurve = ToolRst.Value();
      GCPnts_QuasiUniformDeflection UDP(*TheRCurve, Deflection);
      if (UDP.IsDone()) {
	Standard_Integer NumberOfPoints = UDP.NbPoints();
	if ( NumberOfPoints >= 2 ) {
	  dummypnt = UDP.Value(1);
	  P2.SetCoord(dummypnt.X(), dummypnt.Y());
	  for (i = 2; i <= NumberOfPoints; i++) {
	    P1 = P2;
	    dummypnt = UDP.Value(i);
	    P2.SetCoord(dummypnt.X(), dummypnt.Y());
	    if(Orient == TopAbs_FORWARD )
              isobuild.Trim(P1,P2);
            else
              isobuild.Trim(P2,P1);
	  }
	}
      }
#ifdef OCCT_DEBUG
      else {
	cout << "Cannot evaluate curve on surface"<<endl;
      }
#endif
    }
  }
  
  // draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aFace);
  Standard_Integer NumberOfLines = isobuild.NbLines();
  Standard_Real anAngle = aDrawer->DeviationAngle();

  for (i = 1; i <= NumberOfLines; i++) {
    Standard_Integer NumberOfIntervals = isobuild.NbIntervals(i);
    Standard_Real Coord = isobuild.Coordinate(i);
    for (Standard_Integer j = 1; j <= NumberOfIntervals; j++) {
      Standard_Real b1=isobuild.Start(i,j),b2=isobuild.End(i,j);

      b1 = b1 == RealFirst() ? - aLimit : b1;
      b2 = b2 == RealLast()  ?   aLimit : b2;

      if (isobuild.IsXLine(i))
	anIso.Load(GeomAbs_IsoU,Coord,b1,b2);
      else
	anIso.Load(GeomAbs_IsoV,Coord,b1,b2);
    
      if (StdPrs_DeflectionCurve::Match(X,Y,Z,aDistance,anIso, b1, b2, Deflection, anAngle))
	  return Standard_True;
    }
  }
  return Standard_False;
}


//=========================================================================
// function: Add
// purpose
//=========================================================================
void StdPrs_WFDeflectionRestrictedFace::Add
  (const Handle (Prs3d_Presentation)& aPresentation,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer)
{
  Prs3d_NListOfSequenceOfPnt Curves;
  StdPrs_WFDeflectionRestrictedFace::Add (aPresentation,
					  aFace,
					  Standard_True,
					  Standard_True,
					  aDrawer->MaximalChordialDeviation(),
					  aDrawer->UIsoAspect()->Number(),
					  aDrawer->VIsoAspect()->Number(),
					  aDrawer,
					  Curves);
}


//=========================================================================
// function: AddUIso
// purpose
//=========================================================================
void StdPrs_WFDeflectionRestrictedFace::AddUIso
  (const Handle (Prs3d_Presentation)& aPresentation,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer)
{
  Prs3d_NListOfSequenceOfPnt Curves;
  StdPrs_WFDeflectionRestrictedFace::Add ( 
		      aPresentation,
		      aFace,
		      Standard_True,
		      Standard_False,
		      aDrawer->MaximalChordialDeviation(),
		      aDrawer->UIsoAspect()->Number(),
		      aDrawer->VIsoAspect()->Number(),
		      aDrawer,
		      Curves);
}


//=========================================================================
// function: AddVIso
// purpose
//=========================================================================
void StdPrs_WFDeflectionRestrictedFace::AddVIso
  (const Handle (Prs3d_Presentation)& aPresentation,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer)
{
  Prs3d_NListOfSequenceOfPnt Curves;
  StdPrs_WFDeflectionRestrictedFace::Add ( 
		      aPresentation,
		      aFace,
		      Standard_False,
		      Standard_True,
		      aDrawer->MaximalChordialDeviation(),
		      aDrawer->UIsoAspect()->Number(),
		      aDrawer->VIsoAspect()->Number(),
		      aDrawer,
		      Curves);
}


//=========================================================================
// function: Match
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::Match
  (const Standard_Real X,
   const Standard_Real Y,
   const Standard_Real Z,
   const Standard_Real aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer)
{
  return StdPrs_WFDeflectionRestrictedFace::Match (  
                      X,Y,Z,aDistance,
                      aFace,
                      aDrawer,
                      Standard_True,
                      Standard_True,
                      aDrawer->MaximalChordialDeviation(),
                      aDrawer->UIsoAspect()->Number(),
                      aDrawer->VIsoAspect()->Number());
}


//=========================================================================
// function: MatchUIso
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::MatchUIso
  (const Standard_Real X,
   const Standard_Real Y,
   const Standard_Real Z,
   const Standard_Real aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer)
{
  return StdPrs_WFDeflectionRestrictedFace::Match ( 
                      X,Y,Z,aDistance,
                      aFace,
                      aDrawer,
                      Standard_True,
                      Standard_False,
                      aDrawer->MaximalChordialDeviation(),
                      aDrawer->UIsoAspect()->Number(),
                      aDrawer->VIsoAspect()->Number());
}


//=========================================================================
// function: MatchVIso
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::MatchVIso
  (const Standard_Real X,
   const Standard_Real Y,
   const Standard_Real Z,
   const Standard_Real aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer)
{
  return StdPrs_WFDeflectionRestrictedFace::Match ( 
                      X,Y,Z,aDistance,
                      aFace,
                      aDrawer,
                      Standard_False,
                      Standard_True,
                      aDrawer->MaximalChordialDeviation(),
                      aDrawer->UIsoAspect()->Number(),
                      aDrawer->VIsoAspect()->Number());
}
