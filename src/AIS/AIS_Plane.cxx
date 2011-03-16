// File:	AIS_Plane.cxx
// Created:	Wed Aug  2 16:20:31 1995
// Author:	Arnaud BOUZY/Odile Olivier
//		<adn>

#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <AIS_Plane.ixx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <ElSLib.hxx>

#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <StdPrs_Plane.hxx>
#include <StdPrs_ShadedSurface.hxx>

#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>

#include <DsgPrs_XYZPlanePresentation.hxx>
#include <DsgPrs_ShadedPlanePresentation.hxx>
#include <UnitsAPI.hxx>

#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <StdPrs_ShadedShape.hxx>

#include <AIS_Drawer.hxx>

#include <TCollection_AsciiString.hxx>

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
myIsXYZPlane(Standard_False)
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
myIsXYZPlane(Standard_False)
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
myIsXYZPlane(Standard_False)
{
  InitDrawerAttributes();
  SetHilightMode(0);
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
myIsXYZPlane(Standard_True)
{
  InitDrawerAttributes();
  ComputeFields();
  SetHilightMode(0);

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
  aPresentation->Clear();

  ComputeFields();
  aPresentation->SetInfiniteState(myInfiniteState);
  if (myCurrentMode) 
    myDrawer->PlaneAspect()->EdgesAspect()->SetWidth(3);
  else 
    myDrawer->PlaneAspect()->EdgesAspect()->SetWidth(1);
  if(aMode == 0){
    if (!myIsXYZPlane){
      ComputeFrame();
      const Handle(Geom_Plane)& pl = myComponent;
      const Handle(Geom_Plane)& thegoodpl = Handle(Geom_Plane)::DownCast
	(pl->Translated(pl->Location(),myCenter));
      GeomAdaptor_Surface surf(thegoodpl);
      StdPrs_Plane::Add(aPresentation,surf,myDrawer);
    }
    else {
      DsgPrs_XYZPlanePresentation::Add(aPresentation,myDrawer,myCenter,myPmin,myPmax);
    }
  }
  else if (aMode == 1){
    if (!myIsXYZPlane){
      ComputeFrame();
      // a mettre ailleurs sans doute...
      Handle(Prs3d_PlaneAspect) theaspect = myDrawer->PlaneAspect();
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
      gp_Pnt p1;
      Standard_Real Xmax,Ymax;
      Xmax = Standard_Real(theaspect->PlaneXLength())/2.;
      Ymax = Standard_Real(theaspect->PlaneYLength())/2.;
      static Graphic3d_Array1OfVertex vertices(1,5);
      TheGroup->SetPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
      myComponent->D0(-Xmax,Ymax,p1);
      vertices(1).SetCoord(p1.X(),p1.Y(),p1.Z());
      vertices(5).SetCoord(p1.X(),p1.Y(),p1.Z());
      myComponent->D0(Xmax,Ymax,p1);
      vertices(2).SetCoord(p1.X(),p1.Y(),p1.Z());
      myComponent->D0(Xmax,-Ymax,p1);
      vertices(3).SetCoord(p1.X(),p1.Y(),p1.Z());
      myComponent->D0(-Xmax,-Ymax,p1);
      vertices(4).SetCoord(p1.X(),p1.Y(),p1.Z());
      TheGroup->Polygon(vertices);
	
      }
    else{
      DsgPrs_ShadedPlanePresentation::Add(aPresentation,myDrawer,myCenter,myPmin,myPmax);
      
    }
  }

}

void AIS_Plane::Compute(const Handle_Prs3d_Projector& aProjector, const Handle_Geom_Transformation& aTransformation, const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_Plane::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Plane::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				    const Standard_Integer)
{
  aSelection->Clear();
  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this,10);
  Handle(Select3D_SensitiveFace) sfac;

  if (!myIsXYZPlane){

    Handle(TColgp_HArray1OfPnt) harr = new TColgp_HArray1OfPnt(1,5);
    TColgp_Array1OfPnt& arr = harr->ChangeArray1();
    Standard_Real lx = myDrawer->PlaneAspect()->PlaneXLength()/2.;
    Standard_Real ly = myDrawer->PlaneAspect()->PlaneYLength()/2.;
    const Handle(Geom_Plane)& pl = myComponent;
    const Handle(Geom_Plane)& thegoodpl = Handle(Geom_Plane)::DownCast(pl->Translated(pl->Location(),myCenter));
    
    thegoodpl->D0(lx,ly,arr(1));
    thegoodpl->D0(lx,-ly,arr(2));
    thegoodpl->D0(-lx,-ly,arr(3));
    thegoodpl->D0(-lx,ly,arr(4));
    arr(5) = arr(1);
    sfac = new Select3D_SensitiveFace(eown,harr,Select3D_TOS_BOUNDARY);

  }
  else {
    Handle(TColgp_HArray1OfPnt) harr1 = new TColgp_HArray1OfPnt(1,4);
    TColgp_Array1OfPnt& arr1 = harr1->ChangeArray1();
    
    arr1(1) = myCenter;
    arr1(2) = myPmin;
    arr1(3) = myPmax;
    arr1(4) = myCenter;
    sfac = new Select3D_SensitiveFace(eown,harr1,Select3D_TOS_BOUNDARY);

  }
    aSelection->Add(sfac);
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
  //si le plan a deja une couleur ou une taille propre, 
  //il y a deja un PlaneAspect et un DatumAspect specifique 
    
  Handle(Prs3d_PlaneAspect) PA; 
  Handle(Prs3d_DatumAspect) DA;

  PA = myDrawer->PlaneAspect();
  DA = myDrawer->DatumAspect();

  Standard_Boolean yenavaitPA(Standard_True),yenavaitDA(Standard_True);
  if(myDrawer->Link()->PlaneAspect() == PA){
    yenavaitPA = Standard_False;
    PA = new Prs3d_PlaneAspect();}
  if(myDrawer->Link()->DatumAspect() == DA){
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
//purpose  : S'il ya une couleur, on recupere la taille du drawer du context...
//=======================================================================
void AIS_Plane::UnsetSize()
{
  
  if(!myHasOwnSize) return;
  if(!hasOwnColor){
    myDrawer->PlaneAspect().Nullify();
    myDrawer->DatumAspect().Nullify();
  }
  else{
    const Handle(Prs3d_PlaneAspect)& PA = myDrawer->Link()->PlaneAspect();
    const Handle(Prs3d_DatumAspect)& DA = myDrawer->Link()->DatumAspect();

    myDrawer->PlaneAspect()->SetPlaneLength(PA->PlaneXLength(),PA->PlaneYLength());
    myDrawer->DatumAspect()->SetAxisLength(DA->FirstAxisLength(),
					   DA->SecondAxisLength(),
					   DA->ThirdAxisLength());
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


void AIS_Plane::SetColor(const Quantity_NameOfColor aCol)
#ifdef GER61351
{
  SetColor(Quantity_Color(aCol));
}

void AIS_Plane::SetColor(const Quantity_Color &aCol)
#endif
{
  // si le plan a deja sa propre taille, il y a un deja planeaspect cree
//  Standard_Boolean yenadeja = hasOwnColor || myHasOwnSize;
  Handle(Prs3d_PlaneAspect) PA; 
  Handle(Prs3d_DatumAspect) DA;

  PA = myDrawer->PlaneAspect();
  DA = myDrawer->DatumAspect();

  Standard_Boolean yenavaitPA(Standard_True),yenavaitDA(Standard_True);
  if(myDrawer->Link()->PlaneAspect() == PA){
    yenavaitPA = Standard_False;
    PA = new Prs3d_PlaneAspect();}
  if(myDrawer->Link()->DatumAspect() == DA){
    yenavaitDA = Standard_False;
    DA = new Prs3d_DatumAspect();
  }
  
  PA->EdgesAspect()->SetColor(aCol);
  DA->FirstAxisAspect()->SetColor(aCol);
  DA->SecondAxisAspect()->SetColor(aCol);
  DA->ThirdAxisAspect()->SetColor(aCol);

  if(!yenavaitPA)
    myDrawer->SetPlaneAspect(PA);
  if(!yenavaitDA)
    myDrawer->SetDatumAspect(DA);

  myDrawer->ShadingAspect()->SetColor(aCol);
  
  hasOwnColor=Standard_True;
  myOwnColor = aCol;
}
//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void AIS_Plane::UnsetColor()
{
  if(!hasOwnColor) return;
  if(!myHasOwnSize){
    myDrawer->PlaneAspect().Nullify();
    myDrawer->DatumAspect().Nullify();
  }
  else{
    const Handle(Prs3d_PlaneAspect)& PA = myDrawer->Link()->PlaneAspect();
//    const Handle(Prs3d_DatumAspect)& DA = myDrawer->Link()->DatumAspect();
    Quantity_Color C;Aspect_TypeOfLine T;Standard_Real W;
    PA->EdgesAspect()->Aspect()->Values(C,T,W);
    Quantity_NameOfColor Col = C.Name();
    myDrawer->PlaneAspect()->EdgesAspect()->SetColor(Col);
    
    myDrawer->DatumAspect()->FirstAxisAspect()->SetColor(Col);
    myDrawer->DatumAspect()->SecondAxisAspect()->SetColor(Col);
    myDrawer->DatumAspect()->ThirdAxisAspect()->SetColor(Col);
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
    const Handle(Geom_Plane)& thegoodpl = 
      Handle(Geom_Plane)::DownCast(pl->Translated(pl->Location(),myCenter));
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
void AIS_Plane::Compute(const Handle(PrsMgr_PresentationManager2d)&, 
			   const Handle(Graphic2d_GraphicObject)&,
			   const Standard_Integer)
{
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
    Standard_Real DS1 = DA->FirstAxisLength();
    Standard_Real DS2 = DA->SecondAxisLength();
    Standard_Real DS3 = DA->ThirdAxisLength();
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
  asp.SetTransparency(0.8);
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
