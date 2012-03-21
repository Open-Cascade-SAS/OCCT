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

#include <VrmlConverter_WFDeflectionShape.ixx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_ShapeTool.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_LineAspect.hxx>
#include <VrmlConverter_WFDeflectionRestrictedFace.hxx>
#include <VrmlConverter_DeflectionCurve.hxx>
#include <TColgp_HArray1OfVec.hxx> 
#include <BRep_Tool.hxx> 
#include <VrmlConverter_PointAspect.hxx> 
#include <Vrml_Separator.hxx>
#include <Vrml_Coordinate3.hxx>
#include <Vrml_Material.hxx>
#include <Vrml_PointSet.hxx>
#include <Precision.hxx>


//=========================================================================
// function: Add
// purpose
//=========================================================================
void VrmlConverter_WFDeflectionShape::Add( Standard_OStream&                   anOStream,
					  const TopoDS_Shape&                  aShape,
					  const Handle (VrmlConverter_Drawer)& aDrawer)
{

    Prs3d_ShapeTool Tool(aShape);

    Standard_Real theRequestedDeflection;
    if(aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)   // TOD_RELATIVE, TOD_ABSOLUTE
      {
	Bnd_Box box;
	BRepBndLib::AddClose(aShape, box);

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

      }
    else 
      {
	theRequestedDeflection = aDrawer->MaximalChordialDeviation(); 
      }
//==
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
	    VrmlConverter_WFDeflectionRestrictedFace::Add(anOStream, HS,
							  isoU, isoV,
							  theRequestedDeflection,
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
	    VrmlConverter_WFDeflectionRestrictedFace::Add(anOStream, HS,
							  isoU, Standard_False,
							  theRequestedDeflection,
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
	    VrmlConverter_WFDeflectionRestrictedFace::Add(anOStream, HS,
							  Standard_False, isoV,
							  theRequestedDeflection,
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
		VrmlConverter_DeflectionCurve::Add(anOStream, C, theRequestedDeflection, aDrawer);
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
		VrmlConverter_DeflectionCurve::Add(anOStream, C, theRequestedDeflection, aDrawer);
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
	      VrmlConverter_DeflectionCurve::Add(anOStream, C, theRequestedDeflection, aDrawer);
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
