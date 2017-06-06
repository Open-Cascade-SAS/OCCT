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
#include <BndLib_AddSurface.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Hatch_Hatcher.hxx>
#include <Precision.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_DeflectionCurve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_WFDeflectionRestrictedFace.hxx>

//==================================================================
// function: GetDeflection
// purpose:
//==================================================================
static Standard_Real GetDeflection(const Handle(BRepAdaptor_HSurface)& aFace,
				   const Handle (VrmlConverter_Drawer)& aDrawer) {

  Standard_Real theRequestedDeflection;
  if(aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)   // TOD_RELATIVE, TOD_ABSOLUTE
    {
      Bnd_Box box;
      BndLib_AddSurface::Add(aFace->Surface(), Precision::Confusion(), box);

      Standard_Real  Xmin, Xmax, Ymin, Ymax, Zmin, Zmax, diagonal;
      box.Get( Xmin, Ymin, Zmin, Xmax, Ymax, Zmax );
      if (!(box.IsOpenXmin() || box.IsOpenXmax() ||
	    box.IsOpenYmin() || box.IsOpenYmax() ||
	    box.IsOpenZmin() || box.IsOpenZmax()))
	{
	  diagonal = Sqrt ((Xmax - Xmin)*( Xmax - Xmin) + ( Ymax - Ymin)*( Ymax - Ymin) + ( Zmax - Zmin)*( Zmax - Zmin));
	  diagonal = Max(diagonal, Precision::Confusion());
	  theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;      
	}  
      else
	{
	  diagonal =1000000.;
	  theRequestedDeflection = aDrawer->DeviationCoefficient() * diagonal;  
	}
//      cout << "diagonal = " << diagonal << endl;
//      cout << "theRequestedDeflection = " << theRequestedDeflection << endl;
    }
  else 
    {
      theRequestedDeflection = aDrawer->MaximalChordialDeviation(); 
    }
  return theRequestedDeflection;
}

//=========================================================================
// function: Add 1
// purpose
//=========================================================================
void VrmlConverter_WFDeflectionRestrictedFace::Add
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Standard_Boolean              DrawUIso,
   const Standard_Boolean              DrawVIso,
   const Standard_Real               Deflection,
   const Standard_Integer              NBUiso,
   const Standard_Integer              NBViso,
   const Handle(VrmlConverter_Drawer)& aDrawer)
{

  StdPrs_ToolRFace ToolRst (aFace);
  Standard_Real UF, UL, VF, VL;
  UF = aFace->FirstUParameter();
  UL = aFace->LastUParameter();
  VF = aFace->FirstVParameter();
  VL = aFace->LastVParameter();

  Standard_Real aLimit = aDrawer->MaximalParameterValue();

  // compute bounds of the restriction
  Standard_Real UMin,UMax,VMin,VMax;
  Standard_Integer i;

  UMin = UF;
  UMax = UL;
  VMin = VF;
  VMax = VL;

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
      UClosed = Standard_False; 
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
  Standard_Real U1, U2;
  gp_Pnt dummypnt;
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next()) {
    TopAbs_Orientation Orient = ToolRst.Orientation();
    if ( Orient == TopAbs_FORWARD || Orient == TopAbs_REVERSED ) {
      Adaptor2d_Curve2dPtr TheRCurve = ToolRst.Value();
      if (TheRCurve->GetType() != GeomAbs_Line) {
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
      else {
	U1 = TheRCurve->FirstParameter();
	U2 = TheRCurve->LastParameter();
	P1 = TheRCurve->Value(U1);
	P2 = TheRCurve->Value(U2);
	if(Orient == TopAbs_FORWARD )
	  isobuild.Trim(P1,P2);
	else
	  isobuild.Trim(P2,P1);
      }
    }
  }

  // draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aFace);
  Standard_Integer NumberOfLines = isobuild.NbLines();

  Handle(VrmlConverter_LineAspect) latmp = new VrmlConverter_LineAspect; 
  latmp->SetMaterial(aDrawer->LineAspect()->Material());
  latmp->SetHasMaterial(aDrawer->LineAspect()->HasMaterial());

  Handle(VrmlConverter_IsoAspect) iautmp = new VrmlConverter_IsoAspect;
  iautmp->SetMaterial(aDrawer->UIsoAspect()->Material());
  iautmp->SetHasMaterial(aDrawer->UIsoAspect()->HasMaterial());
  iautmp->SetNumber(aDrawer->UIsoAspect()->Number());

  Handle(VrmlConverter_IsoAspect) iavtmp = new VrmlConverter_IsoAspect; 
  iavtmp->SetMaterial(aDrawer->VIsoAspect()->Material());
  iavtmp->SetHasMaterial(aDrawer->VIsoAspect()->HasMaterial());
  iavtmp->SetNumber(aDrawer->VIsoAspect()->Number());

  Handle(VrmlConverter_LineAspect) laU = new VrmlConverter_LineAspect;
  Handle(VrmlConverter_LineAspect) laV = new VrmlConverter_LineAspect; 

  laU = aDrawer->UIsoAspect();
  laV = aDrawer->VIsoAspect();

  Vrml_Separator SE1;
  Vrml_Separator SE2;
  Vrml_Separator SE3;

  Standard_Boolean flag = Standard_False; // to check a call of Vrml_Separator.Print(anOStream)
  
  SE1.Print(anOStream);

  if (DrawUIso) {
    if (NBUiso > 0) {
      
      if (laU->HasMaterial()){
	
	Handle(Vrml_Material) MU;
	MU = laU->Material();
	
	MU->Print(anOStream);
	laU->SetHasMaterial(Standard_False);

	flag = Standard_True;
	// Separator 2 {
	SE2.Print(anOStream);
      }
      aDrawer->SetLineAspect(laU);

      for (i = 1; i <= NumberOfLines; i++) {
	if (isobuild.IsXLine(i))
	  {
	    Standard_Integer NumberOfIntervals = isobuild.NbIntervals(i);
	    Standard_Real Coord = isobuild.Coordinate(i);
	    for (Standard_Integer j = 1; j <= NumberOfIntervals; j++) {
	      Standard_Real b1=isobuild.Start(i,j),b2=isobuild.End(i,j);
	      
	      b1 = b1 == RealFirst() ? - aLimit : b1;
	      b2 = b2 == RealLast()  ?   aLimit : b2;
	      
	      anIso.Load(GeomAbs_IsoU,Coord,b1,b2);
	      
	      VrmlConverter_DeflectionCurve::Add(anOStream, anIso, Deflection, aDrawer);
	    }
	  }
      }
      if (flag){
    // Separator 2 }
	SE2.Print(anOStream);
	flag = Standard_False;
      }
    }
  }
  
  if (DrawVIso) {
    if ( NBViso > 0) {
      if (laV->HasMaterial()){
	
	Handle(Vrml_Material) MV;
	MV = laV->Material();
	
	MV->Print(anOStream);
	laV->SetHasMaterial(Standard_False);
	flag = Standard_True;
	
	// Separator 3 {
	SE3.Print(anOStream);
      }
      
      aDrawer->SetLineAspect(laV);

      for (i = 1; i <= NumberOfLines; i++) {
	if (isobuild.IsYLine(i))
	  {
	    Standard_Integer NumberOfIntervals = isobuild.NbIntervals(i);
	    Standard_Real Coord = isobuild.Coordinate(i);
	    for (Standard_Integer j = 1; j <= NumberOfIntervals; j++) {
	      Standard_Real b1=isobuild.Start(i,j),b2=isobuild.End(i,j);
	  
	      b1 = b1 == RealFirst() ? - aLimit : b1;
	      b2 = b2 == RealLast()  ?   aLimit : b2;
	      
	      anIso.Load(GeomAbs_IsoV,Coord,b1,b2);
	      
	      VrmlConverter_DeflectionCurve::Add(anOStream, anIso, Deflection, aDrawer);
	    }
	  } 
      } 
      
      if (flag){
	// Separator 3 }
	SE3.Print(anOStream);
	flag = Standard_False;
      }
    }
  }

  // Separator 1 }
  SE1.Print(anOStream);
  
  aDrawer->SetLineAspect(latmp);
  aDrawer->SetUIsoAspect(iautmp);
  aDrawer->SetVIsoAspect(iavtmp);
} 

//=========================================================================
// function: Add 2
// purpose
//=========================================================================
void VrmlConverter_WFDeflectionRestrictedFace::Add
  (Standard_OStream&                          anOStream,
   const Handle(BRepAdaptor_HSurface)&        aFace,
   const Handle (VrmlConverter_Drawer)&       aDrawer){

  Standard_Real Deflection = GetDeflection(aFace, aDrawer);

  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFDeflectionRestrictedFace::Add (  
		      anOStream,
		      aFace,
		      Standard_True,
		      Standard_True,
		      Deflection,
		      finu,
		      finv,
		      aDrawer);

}


//=========================================================================
// function: AddUIso 
// purpose
//=========================================================================
void VrmlConverter_WFDeflectionRestrictedFace::AddUIso
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (VrmlConverter_Drawer)& aDrawer) {

  Standard_Real Deflection = GetDeflection(aFace, aDrawer);

  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  VrmlConverter_WFDeflectionRestrictedFace::Add ( 
		      anOStream,
		      aFace,
		      Standard_True,
		      Standard_False,
		      Deflection,
		      finu,
		      finv,
		      aDrawer);
}


//=========================================================================
// function: AddVIso
// purpose
//=========================================================================
void VrmlConverter_WFDeflectionRestrictedFace::AddVIso
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (VrmlConverter_Drawer)& aDrawer) {

  Standard_Real Deflection = GetDeflection(aFace, aDrawer);

  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();
  VrmlConverter_WFDeflectionRestrictedFace::Add ( 
		      anOStream,
		      aFace,
		      Standard_False,
		      Standard_True,
		      Deflection,
		      finu,
		      finv,
		      aDrawer);
}
