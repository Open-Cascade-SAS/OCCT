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


#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_ShapeTool.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TopoDS_Shape.hxx>
#include <Vrml_Coordinate3.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_PointSet.hxx>
#include <Vrml_Separator.hxx>
#include <VrmlConverter_Curve.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_LineAspect.hxx>
#include <VrmlConverter_PointAspect.hxx>
#include <VrmlConverter_WFRestrictedFace.hxx>
#include <VrmlConverter_WFShape.hxx>

//=========================================================================
// function: Add
// purpose
//=========================================================================
void VrmlConverter_WFShape::Add(Standard_OStream&                    anOStream,
				const TopoDS_Shape&                  aShape,
				const Handle(VrmlConverter_Drawer)&  aDrawer)
{

    Prs3d_ShapeTool Tool(aShape);

    if (aDrawer->UIsoAspect()->Number() != 0 ||
	aDrawer->VIsoAspect()->Number() != 0 ) {

      BRepAdaptor_Surface S;
      Standard_Boolean isoU, isoV;
      for(Tool.InitFace();Tool.MoreFace();Tool.NextFace()){
	isoU = (aDrawer->UIsoAspect()->Number() != 0);
	isoV = (aDrawer->VIsoAspect()->Number() != 0);
	if (Tool.HasSurface()) {
	  if (Tool.IsPlanarFace()) {
	    isoU = (isoU && aDrawer->IsoOnPlane());
	    isoV = (isoV && aDrawer->IsoOnPlane()); 
	  }
	  if (isoU || isoV) {
	    S.Initialize(Tool.GetFace());
	    Handle(BRepAdaptor_HSurface) HS = new BRepAdaptor_HSurface(S);
	    VrmlConverter_WFRestrictedFace::Add(anOStream, HS,
							  isoU, isoV,
							  aDrawer->UIsoAspect()->Number(),
							  aDrawer->VIsoAspect()->Number(),
							  aDrawer);
	  }
	}
      }
    }

  else {

    if (aDrawer->UIsoAspect()->Number() != 0) {

      BRepAdaptor_Surface S;
      for(Tool.InitFace();Tool.MoreFace();Tool.NextFace()){
	Standard_Boolean isoU = Standard_True;
	if (Tool.HasSurface()) {
	  if (Tool.IsPlanarFace()) isoU = aDrawer->IsoOnPlane();
	  if (isoU) {
	    S.Initialize(Tool.GetFace());
	    Handle(BRepAdaptor_HSurface) HS = new BRepAdaptor_HSurface(S);
	    VrmlConverter_WFRestrictedFace::Add(anOStream, HS,
							  isoU, Standard_False,
							  aDrawer->UIsoAspect()->Number(),
							  0,
							  aDrawer);
	  }
	}
      }
    }

    if (aDrawer->VIsoAspect()->Number() != 0) {

      BRepAdaptor_Surface S;
      for(Tool.InitFace();Tool.MoreFace();Tool.NextFace()){
	Standard_Boolean isoV = Standard_True;
	if (Tool.HasSurface()) {
	  if (Tool.IsPlanarFace()) isoV = aDrawer->IsoOnPlane();
	  if (isoV) {
	    S.Initialize(Tool.GetFace());
	    Handle(BRepAdaptor_HSurface) HS = new BRepAdaptor_HSurface(S);
	    VrmlConverter_WFRestrictedFace::Add(anOStream, HS,
							  Standard_False, isoV,
							  0,
							  aDrawer->VIsoAspect()->Number(),
							  aDrawer);
	  }
	}
      }
    }
  }

//====
    Standard_Integer qnt=0;
    for(Tool.InitCurve();Tool.MoreCurve();Tool.NextCurve())
      {
	qnt++;
      }

//   cout << "Quantity of Curves  = " << qnt << endl;

// Wire (without any neighbour)

    if (aDrawer->WireDraw()) {
      if (qnt != 0)
	{
	  Handle(VrmlConverter_LineAspect) latmp = new VrmlConverter_LineAspect; 
	  latmp->SetMaterial(aDrawer->LineAspect()->Material());
	  latmp->SetHasMaterial(aDrawer->LineAspect()->HasMaterial());
	  
	  aDrawer->SetLineAspect(aDrawer->WireAspect());
	  
	  for(Tool.InitCurve();Tool.MoreCurve();Tool.NextCurve()){
	    if (Tool.Neighbours() == 0) {
	      if (Tool.HasCurve()) {
		BRepAdaptor_Curve C(Tool.GetCurve());
		VrmlConverter_Curve::Add(C, aDrawer, anOStream);
	      }
	    }
	  }
	  aDrawer->SetLineAspect(latmp);
	}
    }
//end of wire

// Free boundaries;
    if (aDrawer->FreeBoundaryDraw()) {
      if (qnt != 0)
	{
	  Handle(VrmlConverter_LineAspect) latmp = new VrmlConverter_LineAspect; 
	  latmp->SetMaterial(aDrawer->LineAspect()->Material());
	  latmp->SetHasMaterial(aDrawer->LineAspect()->HasMaterial());
	  
	  aDrawer->SetLineAspect(aDrawer->FreeBoundaryAspect());
	  
	  for(Tool.InitCurve();Tool.MoreCurve();Tool.NextCurve()){
	    if (Tool.Neighbours() == 1) {
	      if (Tool.HasCurve()) {
		BRepAdaptor_Curve C(Tool.GetCurve());
		VrmlConverter_Curve::Add(C, aDrawer, anOStream);
	      }
	    }
	  }
	  aDrawer->SetLineAspect(latmp);
	}
    }
// end of Free boundaries

// Unfree boundaries;
  if (aDrawer->UnFreeBoundaryDraw()) { 
    if (qnt != 0)
      {
 	Handle(VrmlConverter_LineAspect) latmp = new VrmlConverter_LineAspect; 
	latmp->SetMaterial(aDrawer->LineAspect()->Material());
 	latmp->SetHasMaterial(aDrawer->LineAspect()->HasMaterial());

	aDrawer->SetLineAspect(aDrawer->UnFreeBoundaryAspect());

	for(Tool.InitCurve();Tool.MoreCurve();Tool.NextCurve()){
	  if (Tool.Neighbours() >= 2) {
	    if (Tool.HasCurve()) {
	      BRepAdaptor_Curve C(Tool.GetCurve());
	      VrmlConverter_Curve::Add(C, aDrawer, anOStream);
	    }
	  }
	}
	aDrawer->SetLineAspect(latmp);
      }
  }
// end of Unfree boundaries

// Points
 
    qnt=0;
    for(Tool.InitVertex();Tool.MoreVertex();Tool.NextVertex())
      {
	qnt++;
      }

//   cout << "Quantity of Vertexes  = " << qnt << endl;

    if (qnt != 0)
      {
	Handle(TColgp_HArray1OfVec) HAV = new TColgp_HArray1OfVec(1,qnt);
	gp_Vec V;
	gp_Pnt P;
        Standard_Integer i=0;

	for(Tool.InitVertex();Tool.MoreVertex();Tool.NextVertex())
	  {
	    i++;
	    P = BRep_Tool::Pnt(Tool.GetVertex());
	    V.SetX(P.X()); V.SetY(P.Y()); V.SetZ(P.Z());
	    HAV->SetValue (i,V);
	  }

	Handle(VrmlConverter_PointAspect) PA = new VrmlConverter_PointAspect;
	PA = aDrawer->PointAspect();

	// Separator P {
	Vrml_Separator SEP;
	SEP.Print(anOStream);

	// Material
	if (PA->HasMaterial()){

	  Handle(Vrml_Material) MP;
	  MP = PA->Material();
	  
	  MP->Print(anOStream);
	}
	// Coordinate3
	Handle(Vrml_Coordinate3)  C3 = new Vrml_Coordinate3(HAV);
	C3->Print(anOStream);
	
	// PointSet
	Vrml_PointSet PS;
	PS.Print(anOStream);

	// Separator P }
	SEP.Print(anOStream);
      }

}

