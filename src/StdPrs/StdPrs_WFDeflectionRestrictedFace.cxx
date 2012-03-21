// Created on: 1995-08-07
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <StdPrs_WFDeflectionRestrictedFace.ixx>

#include <Hatch_Hatcher.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_Group.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Precision.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Surface.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>




#ifdef DEB_MESH
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
   const Quantity_Length Deflection,
   const Standard_Integer NBUiso,
   const Standard_Integer NBViso,
   const Handle(Prs3d_Drawer)& aDrawer,
   Prs3d_NListOfSequenceOfPnt& Curves) {

#ifdef DEB_MESH
  FFaceTimer1.Start();
#endif

  Standard_Boolean isPA = Graphic3d_ArrayOfPrimitives::IsEnable();

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
	else {
	  cout << "Cannot evaluate curve on surface"<<endl;
	}
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


#ifdef DEB_MESH
  FFaceTimer1.Stop();

  FFaceTimer2.Start();
#endif

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

#ifdef DEB_MESH
  FFaceTimer2.Stop();
  FFaceTimer3.Start();
#endif

  
  Standard_Integer ll = tabP.Length();
  for (i = 1; i <= ll; i+=2) {
    isobuild.Trim(tabP(i),tabP(i+1));
  }


#ifdef DEB_MESH  
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
	  TColgp_SequenceOfPnt Points;
	  StdPrs_DeflectionCurve::Add(aPresentation, GC, b1, b2, Deflection, Points, anAngle, !isPA);
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
	  TColgp_SequenceOfPnt Points;
	  StdPrs_DeflectionCurve::Add(aPresentation, anIso, b1, b2, Deflection, Points, anAngle, !isPA);
	  Curves.Append(Points);
	}
      }
    }
  }
#ifdef DEB_MESH
  FFaceTimer4.Stop();
#endif
}


//=========================================================================
// function: Match
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::Match
  (const Quantity_Length X,
   const Quantity_Length Y,
   const Quantity_Length Z,
   const Quantity_Length aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle(Prs3d_Drawer)& aDrawer,
   const Standard_Boolean DrawUIso,
   const Standard_Boolean DrawVIso,
   const Quantity_Length Deflection,
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
      else {
	cout << "Cannot evaluate curve on surface"<<endl;
      }
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
   const Handle (Prs3d_Drawer)& aDrawer){

  Quantity_Length Deflection = aDrawer->MaximalChordialDeviation();
  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();

  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  TheGroup->BeginPrimitives();

  Prs3d_NListOfSequenceOfPnt Curves;
  StdPrs_WFDeflectionRestrictedFace::Add (aPresentation,
					  aFace,
					  Standard_True,
					  Standard_True,
					  Deflection,
					  finu,
					  finv,
					  aDrawer,
					  Curves);

  TheGroup->EndPrimitives();
}


//=========================================================================
// function: AddUIso
// purpose
//=========================================================================
void StdPrs_WFDeflectionRestrictedFace::AddUIso
  (const Handle (Prs3d_Presentation)& aPresentation,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer) {

  Quantity_Length Deflection = aDrawer->MaximalChordialDeviation();
  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  Prs3d_NListOfSequenceOfPnt Curves;
  StdPrs_WFDeflectionRestrictedFace::Add ( 
		      aPresentation,
		      aFace,
		      Standard_True,
		      Standard_False,
		      Deflection,
		      finu,
		      finv,
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
   const Handle (Prs3d_Drawer)& aDrawer) {

  Quantity_Length Deflection = aDrawer->MaximalChordialDeviation();
  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  Prs3d_NListOfSequenceOfPnt Curves;
  StdPrs_WFDeflectionRestrictedFace::Add ( 
		      aPresentation,
		      aFace,
		      Standard_False,
		      Standard_True,
		      Deflection,
		      finu,
		      finv,
		      aDrawer,
		      Curves);
}


//=========================================================================
// function: Match
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::Match
  (const Quantity_Length X,
   const Quantity_Length Y,
   const Quantity_Length Z,
   const Quantity_Length aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer){

  Quantity_Length Deflection = aDrawer->MaximalChordialDeviation();
  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  return StdPrs_WFDeflectionRestrictedFace::Match (  
                      X,Y,Z,aDistance,
		      aFace,
		      aDrawer,
		      Standard_True,
		      Standard_True,
		      Deflection,
		      finu,
		      finv);
}


//=========================================================================
// function: MatchUIso
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::MatchUIso
  (const Quantity_Length X,
   const Quantity_Length Y,
   const Quantity_Length Z,
   const Quantity_Length aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer) {

  Quantity_Length Deflection = aDrawer->MaximalChordialDeviation();
  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  return StdPrs_WFDeflectionRestrictedFace::Match ( 
                      X,Y,Z,aDistance,
		      aFace,
		      aDrawer,
		      Standard_True,
		      Standard_False,
		      Deflection,
		      finu,
		      finv);
}


//=========================================================================
// function: MatchVIso
// purpose
//=========================================================================
Standard_Boolean StdPrs_WFDeflectionRestrictedFace::MatchVIso
  (const Quantity_Length X,
   const Quantity_Length Y,
   const Quantity_Length Z,
   const Quantity_Length aDistance,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (Prs3d_Drawer)& aDrawer) {

  Quantity_Length Deflection = aDrawer->MaximalChordialDeviation();
  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  return StdPrs_WFDeflectionRestrictedFace::Match ( 
                      X,Y,Z,aDistance,
		      aFace,
		      aDrawer,
		      Standard_False,
		      Standard_True,
		      Deflection,
		      finu,
		      finv);
}


