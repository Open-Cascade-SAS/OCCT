// Created on: 1995-08-02
// Created by: Arnaud BOUZY/Odile Olivier
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

#include <AIS_Plane.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <DsgPrs_ShadedPlanePresentation.hxx>
#include <DsgPrs_XYZPlanePresentation.hxx>
#include <ElSLib.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Transformation.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_Structure.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Plane.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ShadedSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Plane,AIS_InteractiveObject)

//=======================================================================
//function : AIS_Plane
//purpose  : 
//=======================================================================
AIS_Plane::AIS_Plane(const Handle(Geom_Plane)& aComponent,
		     const Standard_Boolean aCurrentMode):
myComponent(aComponent),
myCenter(gp_Pnt(0.,0.,0.)),
myCurrentMode(aCurrentMode),
myAutomaticPosition(Standard_True),
myTypeOfPlane(AIS_TOPL_Unknown),
myIsXYZPlane(Standard_False),
myTypeOfSensitivity (Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
}

//=======================================================================
//function : AIS_Plane 
//purpose  : avec une position
//=======================================================================
AIS_Plane::AIS_Plane(const Handle(Geom_Plane)& aComponent,
		     const gp_Pnt& aCenter,
		     const Standard_Boolean aCurrentMode):
myComponent(aComponent),
myCenter(aCenter),
myCurrentMode(aCurrentMode),
myAutomaticPosition(Standard_True),
myTypeOfPlane(AIS_TOPL_Unknown),
myIsXYZPlane(Standard_False),
myTypeOfSensitivity (Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
}

//=======================================================================
//function : AIS_Plane
//purpose  : 
//=======================================================================
AIS_Plane::AIS_Plane(const Handle(Geom_Plane)& aComponent,
			       const gp_Pnt& aCenter,
			       const gp_Pnt& aPmin,
			       const gp_Pnt& aPmax,
		               const Standard_Boolean aCurrentMode):
myComponent(aComponent),
myCenter(aCenter),
myPmin(aPmin),
myPmax(aPmax),
myCurrentMode(aCurrentMode),
myAutomaticPosition(Standard_False),
myTypeOfPlane(AIS_TOPL_Unknown),
myIsXYZPlane(Standard_False),
myTypeOfSensitivity (Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
}

//=======================================================================
//function : AIS_Plane
//purpose  : XYPlane, XZPlane, YZPlane
//=======================================================================
AIS_Plane::AIS_Plane(const Handle(Geom_Axis2Placement)& aComponent,
		     const AIS_TypeOfPlane aPlaneType,
		     const Standard_Boolean aCurrentMode):
myAx2(aComponent),
myCurrentMode(aCurrentMode),
myAutomaticPosition(Standard_True),
myTypeOfPlane(aPlaneType),
myIsXYZPlane(Standard_True),
myTypeOfSensitivity (Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
  ComputeFields();
}


//=======================================================================
//function : SetComponent
//purpose  : 
//=======================================================================

void AIS_Plane::SetComponent(const Handle(Geom_Plane)& aComponent)
{
  myComponent = aComponent;
  myTypeOfPlane = AIS_TOPL_Unknown;
  myIsXYZPlane = Standard_False;
  //myCenter = gp_Pnt(0.,0.,0.);
  myAutomaticPosition = Standard_True;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

AIS_KindOfInteractive AIS_Plane::Type() const 
{return AIS_KOI_Datum;}


//=======================================================================
//function : Signature
//purpose  : 
//=======================================================================

Standard_Integer AIS_Plane::Signature() const 
{return 7;}


//=======================================================================
//function : Axis2Placement
//purpose  : 
//=======================================================================

Handle(Geom_Axis2Placement) AIS_Plane::Axis2Placement()
{
  Handle(Geom_Axis2Placement) Bid;
  return IsXYZPlane() ? myAx2:Bid;
}
//=======================================================================
//function : SetAxis2Placement
//purpose  : 
//=======================================================================

 void AIS_Plane::SetAxis2Placement(const Handle(Geom_Axis2Placement)& aComponent,
				   const AIS_TypeOfPlane aPlaneType)
{
  myTypeOfPlane = aPlaneType;
  myIsXYZPlane = Standard_True;
  myAx2= aComponent;
  myAutomaticPosition = Standard_True;
  ComputeFields();
}

//=======================================================================
//function : PlaneAttributes
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Plane::PlaneAttributes(Handle(Geom_Plane)& aComponent,
					    gp_Pnt& aCenter,
					    gp_Pnt& aPmin,
					    gp_Pnt& aPmax)
{
  Standard_Boolean aStatus (Standard_False);
  if (!myAutomaticPosition){
    aComponent = myComponent;
    aCenter = myCenter;
    aPmin = myPmin;
    aPmax = myPmax;
    aStatus = Standard_True;
  }
  return aStatus;
}

//=======================================================================
//function : SetPlaneAttributes
//purpose  : 
//=======================================================================
void AIS_Plane::SetPlaneAttributes(const Handle(Geom_Plane)& aComponent,
			      const gp_Pnt& aCenter,
			      const gp_Pnt& aPmin,
			      const gp_Pnt& aPmax)
{
  myAutomaticPosition = Standard_False;
  myComponent = aComponent;
  myCenter = aCenter;
  myPmin = aPmin;
  myPmax = aPmax;
  myTypeOfPlane = AIS_TOPL_Unknown;
  myIsXYZPlane = Standard_False;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Plane::Compute(const Handle(PrsMgr_PresentationManager3d)& ,
			const Handle(Prs3d_Presentation)& aPresentation, 
			const Standard_Integer aMode)
{
  ComputeFields();
  aPresentation->SetInfiniteState(myInfiniteState);
  myDrawer->PlaneAspect()->EdgesAspect()->SetWidth(myCurrentMode == 0? 1 : 3);

  switch (aMode)
  {
    case 0:
    {
      if (!myIsXYZPlane)
      {
        ComputeFrame();
        const Handle(Geom_Plane)& pl = myComponent;
        Handle(Geom_Plane) thegoodpl (Handle(Geom_Plane)::DownCast(pl->Translated(pl->Location(),myCenter)));
        GeomAdaptor_Surface surf(thegoodpl);
        StdPrs_Plane::Add(aPresentation,surf,myDrawer);
      }
      else
        DsgPrs_XYZPlanePresentation::Add(aPresentation,myDrawer,myCenter,myPmin,myPmax);
      break;
    }
    case 1:
    {
      if (!myIsXYZPlane)
      {
        ComputeFrame();
        Handle(Prs3d_PlaneAspect) theaspect = myDrawer->PlaneAspect();
        Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
        TheGroup->SetPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
        gp_Pnt p1;
        const Standard_Real Xmax = 0.5*Standard_Real(theaspect->PlaneXLength());
        const Standard_Real Ymax = 0.5*Standard_Real(theaspect->PlaneYLength());

        Handle(Graphic3d_ArrayOfQuadrangles) aQuads = new Graphic3d_ArrayOfQuadrangles(4);

        myComponent->D0(-Xmax,Ymax,p1);
        aQuads->AddVertex(p1);
        myComponent->D0(Xmax,Ymax,p1);
        aQuads->AddVertex(p1);
        myComponent->D0(Xmax,-Ymax,p1);
        aQuads->AddVertex(p1);
        myComponent->D0(-Xmax,-Ymax,p1);
        aQuads->AddVertex(p1);

        TheGroup->AddPrimitiveArray(aQuads);
      }
      else
        DsgPrs_ShadedPlanePresentation::Add(aPresentation,myDrawer,myCenter,myPmin,myPmax);
      break;
    }
  }
}

void AIS_Plane::Compute(const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTransformation, const Handle(Prs3d_Presentation)& aPresentation)
{
  PrsMgr_PresentableObject::Compute(aProjector, aTransformation, aPresentation);
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Plane::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection, const Standard_Integer /*theMode*/)
{
  theSelection->Clear();
  Handle(SelectMgr_EntityOwner) aSensitiveOwner = new SelectMgr_EntityOwner (this, 10);
  Handle(Poly_Triangulation) aSensitivePoly;

  if (!myIsXYZPlane)
  {
    // plane representing rectangle
    Standard_Real aLengthX = myDrawer->PlaneAspect()->PlaneXLength() / 2.0;
    Standard_Real aLengthY = myDrawer->PlaneAspect()->PlaneYLength() / 2.0;
    Handle(Geom_Plane) aPlane = 
      Handle(Geom_Plane)::DownCast (myComponent->Translated (myComponent->Location(), myCenter));

    TColgp_Array1OfPnt aRectanglePoints (1, 4);
    aPlane->D0 ( aLengthX,  aLengthY, aRectanglePoints.ChangeValue (1));
    aPlane->D0 ( aLengthX, -aLengthY, aRectanglePoints.ChangeValue (2));
    aPlane->D0 (-aLengthX, -aLengthY, aRectanglePoints.ChangeValue (3));
    aPlane->D0 (-aLengthX,  aLengthY, aRectanglePoints.ChangeValue (4));

    Poly_Array1OfTriangle aTriangles (1, 2);
    aTriangles.ChangeValue (1) = Poly_Triangle (1, 2, 3);
    aTriangles.ChangeValue (2) = Poly_Triangle (1, 3, 4);

    aSensitivePoly = new Poly_Triangulation (aRectanglePoints, aTriangles);
  }
  else
  {
    // plane representing triangle
    TColgp_Array1OfPnt aTrianglePoints (1, 3);
    aTrianglePoints.ChangeValue (1) = myCenter;
    aTrianglePoints.ChangeValue (2) = myPmin;
    aTrianglePoints.ChangeValue (3) = myPmax;

    Poly_Array1OfTriangle aTriangles (1, 1);
    aTriangles.ChangeValue (1) = Poly_Triangle(1, 2, 3);

    aSensitivePoly = new Poly_Triangulation (aTrianglePoints, aTriangles);
  }

  Standard_Boolean isSensitiveInterior = myTypeOfSensitivity == Select3D_TOS_INTERIOR;

  Handle(Select3D_SensitiveTriangulation) aSensitive =
    new Select3D_SensitiveTriangulation (aSensitiveOwner,
                                         aSensitivePoly,
                                         TopLoc_Location(),
                                         isSensitiveInterior);

  theSelection->Add(aSensitive);
}

//=======================================================================
//function : SetSize
//purpose  : 
//=======================================================================
void AIS_Plane::SetSize(const Standard_Real aLength)
{
  SetSize(aLength,aLength);
}

void AIS_Plane::SetSize(const Standard_Real aXLength,
			const Standard_Real aYLength)
{
  //if the plane already has a proper color or size, 
  //there is already a specific PlaneAspect and DatumAspect 
    
  Handle(Prs3d_PlaneAspect) PA; 
  Handle(Prs3d_DatumAspect) DA;

  PA = myDrawer->PlaneAspect();
  DA = myDrawer->DatumAspect();

  Standard_Boolean yenavaitPA(Standard_True),yenavaitDA(Standard_True);
  if(myDrawer->HasLink() && myDrawer->Link()->PlaneAspect() == PA){
    yenavaitPA = Standard_False;
    PA = new Prs3d_PlaneAspect();}
  if(myDrawer->HasLink() && myDrawer->Link()->DatumAspect() == DA){
    yenavaitDA = Standard_False;
    DA = new Prs3d_DatumAspect();
  }
  
  PA->SetPlaneLength(aXLength,aYLength);
  DA->SetAxisLength(aXLength,aYLength,aXLength);
  
  if(!yenavaitPA)
    myDrawer->SetPlaneAspect(PA);
  if(!yenavaitDA)
    myDrawer->SetDatumAspect(DA);
  
  
  myHasOwnSize = Standard_True;
  Update();
  UpdateSelection();
}


//=======================================================================
//function : UnsetSize
//purpose  : If there is a color, the size is restaured from the drawer of the context...
//=======================================================================
void AIS_Plane::UnsetSize()
{
  
  if(!myHasOwnSize) return;
  if(!hasOwnColor)
  {
    myDrawer->SetPlaneAspect (Handle(Prs3d_PlaneAspect)());
    myDrawer->SetDatumAspect (Handle(Prs3d_DatumAspect)());
  }
  else{
    const Handle(Prs3d_PlaneAspect) PA = myDrawer->HasLink() ? myDrawer->Link()->PlaneAspect() :
                                                               new Prs3d_PlaneAspect();
    const Handle(Prs3d_DatumAspect) DA = myDrawer->HasLink() ? myDrawer->Link()->DatumAspect() :
                                                               new Prs3d_DatumAspect();

    myDrawer->PlaneAspect()->SetPlaneLength(PA->PlaneXLength(),PA->PlaneYLength());
    myDrawer->DatumAspect()->SetAxisLength(DA->AxisLength(Prs3d_DP_XAxis),
					   DA->AxisLength(Prs3d_DP_YAxis),
					   DA->AxisLength(Prs3d_DP_ZAxis));
  }
  
  myHasOwnSize = Standard_False;
  Update();
  UpdateSelection();

}

//=======================================================================
//function : Size
//purpose  : 
//=======================================================================

Standard_Boolean AIS_Plane::Size(Standard_Real& X,Standard_Real& Y) const 
{
  X = myDrawer->PlaneAspect()->PlaneXLength();
  Y = myDrawer->PlaneAspect()->PlaneYLength();
  return Abs(X-Y)<=Precision::Confusion();
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void AIS_Plane::SetColor(const Quantity_Color &aCol)
{
  // if the plane already has its proper size, there is an already created planeaspect 
//  Standard_Boolean yenadeja = hasOwnColor || myHasOwnSize;
  Handle(Prs3d_PlaneAspect) PA; 
  Handle(Prs3d_DatumAspect) DA;

  PA = myDrawer->PlaneAspect();
  DA = myDrawer->DatumAspect();

  Standard_Boolean yenavaitPA(Standard_True),yenavaitDA(Standard_True);
  if(myDrawer->HasLink() && myDrawer->Link()->PlaneAspect() == PA){
    yenavaitPA = Standard_False;
    PA = new Prs3d_PlaneAspect();}
  if(myDrawer->HasLink() && myDrawer->Link()->DatumAspect() == DA){
    yenavaitDA = Standard_False;
    DA = new Prs3d_DatumAspect();
  }
  
  PA->EdgesAspect()->SetColor(aCol);
  DA->LineAspect(Prs3d_DP_XAxis)->SetColor(aCol);
  DA->LineAspect(Prs3d_DP_YAxis)->SetColor(aCol);
  DA->LineAspect(Prs3d_DP_ZAxis)->SetColor(aCol);

  if(!yenavaitPA)
    myDrawer->SetPlaneAspect(PA);
  if(!yenavaitDA)
    myDrawer->SetDatumAspect(DA);

  myDrawer->ShadingAspect()->SetColor(aCol);
  
  hasOwnColor=Standard_True;
  myDrawer->SetColor (aCol);
}
//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void AIS_Plane::UnsetColor()
{
  if(!hasOwnColor) return;
  if(!myHasOwnSize)
  {
    myDrawer->SetPlaneAspect (Handle(Prs3d_PlaneAspect)());
    myDrawer->SetDatumAspect (Handle(Prs3d_DatumAspect)());
  }
  else{
    const Handle(Prs3d_PlaneAspect) PA = myDrawer->HasLink() ? myDrawer->Link()->PlaneAspect() :
                                                               new Prs3d_PlaneAspect();
    Quantity_Color Col = PA->EdgesAspect()->Aspect()->Color();
    myDrawer->PlaneAspect()->EdgesAspect()->SetColor(Col);
    
    myDrawer->DatumAspect()->LineAspect(Prs3d_DP_XAxis)->SetColor(Col);
    myDrawer->DatumAspect()->LineAspect(Prs3d_DP_YAxis)->SetColor(Col);
    myDrawer->DatumAspect()->LineAspect(Prs3d_DP_ZAxis)->SetColor(Col);
  }
 
 
  hasOwnColor=Standard_False;

}

//=======================================================================
//function : ComputeFrame
//purpose  : 
//=======================================================================
void AIS_Plane::ComputeFrame()
{

  const Handle(Geom_Plane)& pl = myComponent;
  Standard_Real U,V;

  if (myAutomaticPosition) {
    ElSLib::Parameters(pl->Pln(),myCenter,U,V);
    pl->D0 (U, V, myCenter);     
  }
  else {
    Handle(Geom_Plane) thegoodpl (Handle(Geom_Plane)::DownCast(pl->Translated(pl->Location(),myCenter)));
    ElSLib::Parameters(thegoodpl->Pln(),myPmin,U,V);
    
    U = 2.4*Abs(U);
    V = 2.4*Abs(V);
    if (U < 10*Precision::Confusion()) U=0.1;
    if (V < 10*Precision::Confusion()) V=0.1;
    SetSize(U,V);
    myDrawer->PlaneAspect()->SetPlaneLength(U,V);
  }
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Plane::Compute(const Handle(Prs3d_Projector)&, 
			   const Handle(Prs3d_Presentation)&)
{
}

//=======================================================================
//function : ComputeFields
//purpose  : 
//=======================================================================
void AIS_Plane::ComputeFields()
{
  if (myIsXYZPlane){
    Handle (Prs3d_DatumAspect) DA = myDrawer->DatumAspect();

    gp_Pnt Orig = myAx2->Ax2().Location();
    gp_Dir oX = myAx2->Ax2().XDirection();
    gp_Dir oY = myAx2->Ax2().YDirection();
    gp_Dir oZ = myAx2->Ax2().Direction();
    myCenter = Orig;
    Standard_Real xo,yo,zo,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4=0,y4=0,z4=0;
    Standard_Real x5=0,y5=0,z5=0;
    Orig.Coord(xo,yo,zo);
    oX.Coord(x1,y1,z1);
    oY.Coord(x2,y2,z2);
    oZ.Coord(x3,y3,z3);
    Standard_Real DS1 = DA->AxisLength(Prs3d_DP_XAxis);
    Standard_Real DS2 = DA->AxisLength(Prs3d_DP_YAxis);
    Standard_Real DS3 = DA->AxisLength(Prs3d_DP_ZAxis);
//    gp_Pnt aPt2,aPt3;

    switch (myTypeOfPlane) {
    case AIS_TOPL_XYPlane:
      {
	gp_Pln XYP(0,0,1,0);
	myComponent = new Geom_Plane(XYP);
	x4=xo+x1*DS1; y4=yo+y1*DS1; z4=zo+z1*DS1;
	x5=xo+x2*DS2; y5=yo+y2*DS2; z5=zo+z2*DS2;
	break;
      }
    case AIS_TOPL_XZPlane:
      {
	gp_Pln XZP(0,1,0,0);
	myComponent = new Geom_Plane(XZP);
	x4=xo+x1*DS1; y4=yo+y1*DS1; z4=zo+z1*DS1;
	x5=xo+x3*DS3; y5=yo+y3*DS3; z5=zo+z3*DS3;
	break;
      }
    case AIS_TOPL_YZPlane:
      {
	gp_Pln XZP(1,0,0,0);
	myComponent = new Geom_Plane(XZP);
	x4=xo+x2*DS2; y4=yo+y2*DS2; z4=zo+z2*DS2;
	x5=xo+x3*DS3; y5=yo+y3*DS3; z5=zo+z3*DS3;
	break;
      }
    default:
      break;
    }
    myPmin.SetCoord(x4,y4,z4);
    myPmax.SetCoord(x5,y5,z5);
  }

}
//=======================================================================
//function : InitDrawerAttributes
//purpose  : 
//=======================================================================

void AIS_Plane::InitDrawerAttributes()
{
  Handle(Prs3d_ShadingAspect) shasp = new Prs3d_ShadingAspect();
  shasp->SetMaterial(Graphic3d_NOM_PLASTIC);
  shasp->SetColor(Quantity_NOC_GRAY40);
  myDrawer->SetShadingAspect(shasp);
  Handle(Graphic3d_AspectFillArea3d) asf = shasp->Aspect();
  Graphic3d_MaterialAspect asp = asf->FrontMaterial();
  asp.SetTransparency (0.8f);
  asf->SetFrontMaterial(asp);
  asf->SetBackMaterial(asp);
}

//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================

Standard_Boolean  AIS_Plane::
AcceptDisplayMode(const Standard_Integer aMode) const
{return aMode == 0;}


//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void AIS_Plane::SetContext(const Handle(AIS_InteractiveContext)& Ctx)
{
  AIS_InteractiveObject::SetContext(Ctx);
  ComputeFields();

}
