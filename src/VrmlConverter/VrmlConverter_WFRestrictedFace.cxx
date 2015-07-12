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


#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <gp_Pnt.hxx>
#include <Hatch_Hatcher.hxx>
#include <Precision.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_Curve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_WFRestrictedFace.hxx>

//=========================================================================
// function: Add 1
// purpose
//=========================================================================
void VrmlConverter_WFRestrictedFace::Add
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Standard_Boolean              DrawUIso,
   const Standard_Boolean              DrawVIso,
   const Standard_Integer              NBUiso,
   const Standard_Integer              NBViso,
   const Handle(VrmlConverter_Drawer)& aDrawer)
{
  Standard_Real aLimit = aDrawer->MaximalParameterValue();
  Standard_Integer nbPoints = aDrawer->Discretisation();

  StdPrs_ToolRFace ToolRst (aFace);

  // compute bounds of the restriction
  Standard_Real UMin,UMax,VMin,VMax;
  Standard_Integer i;
  gp_Pnt2d P1,P2;
  Bnd_Box2d B;
  
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next()) {
    Adaptor2d_Curve2dPtr TheRCurve = ToolRst.Value();
    BndLib_Add2dCurve::Add(*TheRCurve, Precision::PConfusion(), B);
  }

  B.Get(UMin, VMin, UMax, VMax);

  // load the infinite isos 
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
  Standard_Real U1, U2, U, DU;

  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next()) {
    TopAbs_Orientation Orient = ToolRst.Orientation();
    if ( Orient == TopAbs_FORWARD || Orient == TopAbs_REVERSED ) {
      Adaptor2d_Curve2dPtr TheRCurve = ToolRst.Value();
      U1 = TheRCurve->FirstParameter();
      U2 = TheRCurve->LastParameter();
      if (TheRCurve->GetType() != GeomAbs_Line) {
	DU = (U2-U1)/(nbPoints-1);
	P2 = TheRCurve->Value(U1);
	for (i = 2; i <= nbPoints; i++) {
	  U = U1 + (i-1)*DU;
	  P1 = P2;
	  P2 = TheRCurve->Value(U);
	  if(Orient == TopAbs_FORWARD )
	    isobuild.Trim(P1,P2);
	  else
	    isobuild.Trim(P2,P1);
	}
      }
      else {
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

//     cout << endl;

//     cout << "Face:aDrawer>UMaterial1 = " << aDrawer->UIsoAspect()->HasMaterial() << endl;
//     cout << "Face:aDrawer>VMaterial1 = " << aDrawer->VIsoAspect()->HasMaterial() << endl;
//     cout << "Face:aDrawer>LineAspect1 = " << aDrawer->LineAspect()->HasMaterial() << endl;
//     cout << "Face:la1 = " << latmp->HasMaterial() << endl;
//     cout << "Face:laU1 = " << laU->HasMaterial() << endl;
//     cout << "Face:laV1 = " << laV->HasMaterial() << endl;
//     cout << "Face:iau1 = " << iautmp->HasMaterial() << endl;
//     cout << "Face:iav1 = " << iavtmp->HasMaterial() << endl;


// creation of Vrml objects

// Separator 1 {
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
	      
	      VrmlConverter_Curve::Add(anIso, aDrawer, anOStream);
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
	      
	      VrmlConverter_Curve::Add(anIso, aDrawer, anOStream); 
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

//     cout << endl;

//     cout << "Face:aDrawer>UMaterial4 = " << aDrawer->UIsoAspect()->HasMaterial() << endl;
//     cout << "Face:aDrawer>VMaterial4 = " << aDrawer->VIsoAspect()->HasMaterial() << endl;
//     cout << "Face:aDrawer>LineAspect4 = " << aDrawer->LineAspect()->HasMaterial() << endl;
//     cout << "Face:la4 = " << latmp->HasMaterial() << endl;
//     cout << "Face:laU4 = " << laU->HasMaterial() << endl;
//     cout << "Face:laV4 = " << laV->HasMaterial() << endl;
//     cout << "Face:iau4 = " << iautmp->HasMaterial() << endl;
//     cout << "Face:iav4 = " << iavtmp->HasMaterial() << endl;

} 


//=========================================================================
// function: Add 2
// purpose
//=========================================================================
void VrmlConverter_WFRestrictedFace::Add
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)&    aFace,
   const Handle (VrmlConverter_Drawer)&       aDrawer){

  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFRestrictedFace::Add (  
		      anOStream,
		      aFace,
		      Standard_True,
		      Standard_True,
		      finu,
		      finv,
		      aDrawer);

}


//=========================================================================
// function: AddUIso
// purpose
//=========================================================================
void VrmlConverter_WFRestrictedFace::AddUIso
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (VrmlConverter_Drawer)& aDrawer) {

  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFRestrictedFace::Add ( 
		      anOStream,
		      aFace,
		      Standard_True,
		      Standard_False,
		      finu,
		      finv,
		      aDrawer);
}


//=========================================================================
// function: AddVIso
// purpose
//=========================================================================
void VrmlConverter_WFRestrictedFace::AddVIso
  (Standard_OStream&                   anOStream,
   const Handle(BRepAdaptor_HSurface)& aFace,
   const Handle (VrmlConverter_Drawer)& aDrawer) {

  Standard_Integer finu = aDrawer->UIsoAspect()->Number();
  Standard_Integer finv = aDrawer->VIsoAspect()->Number();

  VrmlConverter_WFRestrictedFace::Add ( 
		      anOStream,
		      aFace,
		      Standard_False,
		      Standard_True,
		      finu,
		      finv,
		      aDrawer);
}

