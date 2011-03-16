// File:	AIS_Point.cxx
// Created:	Wed Aug  9 14:38:46 1995
// Author:	Arnaud BOUZY/Odile Olivier
//		<adn>

#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <AIS_Point.ixx>
#include <Aspect_TypeOfLine.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <StdPrs_Point.hxx>
#include <Geom_Point.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <AIS_Drawer.hxx>

#include <AIS_InteractiveContext.hxx>
//=======================================================================
//function : AIS_Point
//purpose  : 
//=======================================================================
AIS_Point::AIS_Point(const Handle(Geom_Point)& aComponent):
myComponent(aComponent),
myHasTOM(Standard_False),
myTOM(Aspect_TOM_PLUS)
{
  myHilightMode=-99;
}

//=======================================================================
//function : Component
//purpose  : 
//=======================================================================

Handle(Geom_Point) AIS_Point::Component()
{
  return myComponent;
}

//=======================================================================
//function : SetComponent
//purpose  : 
//=======================================================================

 void AIS_Point::SetComponent(const Handle(Geom_Point)& aComponent)
{
  myComponent = aComponent;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_Point::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                        const Handle(Prs3d_Presentation)& aPresentation, 
                        const Standard_Integer aMode)
{
  aPresentation->Clear();

  aPresentation->SetInfiniteState(myInfiniteState);

  if (aMode==0)
    StdPrs_Point::Add(aPresentation,myComponent,myDrawer);
  else if (aMode== -99)
    {
      // Beeurk.. a revoir - rob-25/04/97
      static Handle(Graphic3d_AspectMarker3d) PtA = 
	new Graphic3d_AspectMarker3d ();
      PtA->SetType(Aspect_TOM_PLUS);
      PtA->SetScale(3.);
      Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
      TheGroup->SetPrimitivesAspect(PtA);
      Graphic3d_Vertex Vtx(myComponent->X(),myComponent->Y(),myComponent->Z());
      TheGroup->Marker(Vtx);
    }
    
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_Point::Compute(const Handle_Prs3d_Projector& aProjector,
                        const Handle_Geom_Transformation& aTransformation,
                        const Handle_Prs3d_Presentation& aPresentation)
{
// Standard_NotImplemented::Raise("AIS_Point::Compute(const Handle_Prs3d_Projector&, const Handle_Geom_Transformation&, const Handle_Prs3d_Presentation&)");
 PrsMgr_PresentableObject::Compute( aProjector , aTransformation , aPresentation) ;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Point::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                 const Standard_Integer /*aMode*/)
{
  Handle(SelectMgr_EntityOwner) eown = new SelectMgr_EntityOwner(this,10);
//  eown -> SelectBasics_EntityOwner::Set(5);
  Handle(Select3D_SensitivePoint) sp = new Select3D_SensitivePoint(eown,
								   myComponent->Pnt());
  aSelection->Add(sp);
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_Point::SetColor(const Quantity_NameOfColor aCol)
#ifdef GER61351
{
  SetColor(Quantity_Color(aCol));
}

void AIS_Point::SetColor(const Quantity_Color &aCol)
#endif
{
  hasOwnColor=Standard_True;
  myOwnColor=aCol;
  UpdatePointValues();
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================
void AIS_Point::UnsetColor()
{
  hasOwnColor=Standard_False;
  UpdatePointValues();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
TopoDS_Vertex AIS_Point::Vertex() const
{
  gp_Pnt P = myComponent->Pnt();
  return BRepBuilderAPI_MakeVertex(P);
}

//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Point::Compute(const Handle(PrsMgr_PresentationManager2d)&, 
			   const Handle(Graphic2d_GraphicObject)&,
			   const Standard_Integer)
{
}
//=======================================================================
//function : Compute
//purpose  : to avoid warning
//=======================================================================
void AIS_Point::Compute(const Handle(Prs3d_Projector)&, 
			   const Handle(Prs3d_Presentation)&)
{
}

//=======================================================================
//function : SetMarker
//purpose  : 
//=======================================================================

void AIS_Point::SetMarker(const Aspect_TypeOfMarker aTOM)
{
  myTOM = aTOM;
  myHasTOM = Standard_True;
  UpdatePointValues();
}

//=======================================================================
//function : UnsetMarker
//purpose  : 
//=======================================================================
void AIS_Point::UnsetMarker()
{
  myHasTOM = Standard_False;
  UpdatePointValues();
}

//=======================================================================
//function : AcceptDisplayMode
//purpose  : 
//=======================================================================

 Standard_Boolean  AIS_Point::
AcceptDisplayMode(const Standard_Integer aMode) const
{return aMode == 0;}


//=======================================================================
//function : UpdatePointValues
//purpose  : 
//=======================================================================

void AIS_Point::UpdatePointValues()
{

  if(!hasOwnColor && myOwnWidth==0.0 && !myHasTOM){
    myDrawer->PointAspect().Nullify();
    return;
  }
#ifdef GER61351
  Quantity_Color aCol;
#else
  Quantity_NameOfColor aCol;
#endif 
  Quantity_Color QCO;
  Aspect_TypeOfMarker  aTOM;
  Standard_Real        aScale;
  
  if(myDrawer->HasLink()){
    myDrawer->Link()->PointAspect()->Aspect()->Values(QCO,aTOM,aScale);
    aCol = QCO.Name();
  }
  else{
    aCol = Quantity_NOC_YELLOW;
    aTOM = Aspect_TOM_PLUS;
    aScale = 1;
  }
  if(hasOwnColor) aCol = myOwnColor;
  if(myOwnWidth!=0.0) aScale = myOwnWidth;
  if(myHasTOM) aTOM = myTOM;
  
  
  if(myDrawer->HasPointAspect()){
    // CLE
    // const Handle(Prs3d_PointAspect) PA =  myDrawer->PointAspect();
    Handle(Prs3d_PointAspect) PA =  myDrawer->PointAspect();
    // ENDCLE
    PA->SetColor(aCol);
    PA->SetTypeOfMarker(aTOM);
    PA->SetScale(aScale);
  }
  else
    myDrawer->SetPointAspect(new Prs3d_PointAspect(aTOM,aCol,aScale));
}

