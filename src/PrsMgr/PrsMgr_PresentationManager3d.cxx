
#include <PrsMgr_PresentationManager3d.ixx>
#include <PrsMgr_Presentation3d.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Visual3d_TransientManager.hxx>
#include <V3d_View.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>

PrsMgr_PresentationManager3d::PrsMgr_PresentationManager3d (const Handle(Graphic3d_StructureManager)& aStructureManager):myStructureManager(aStructureManager) {}

Standard_Boolean PrsMgr_PresentationManager3d::Is3D() const
{return Standard_True;}

Handle(PrsMgr_Presentation) PrsMgr_PresentationManager3d::newPresentation (const Handle(PrsMgr_PresentableObject) & aPresentableObject) {
  
  return new PrsMgr_Presentation3d(this,aPresentableObject);
}

void PrsMgr_PresentationManager3d::Connect(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Handle(PrsMgr_PresentableObject)& anOtherObject, const Standard_Integer aMode, const Standard_Integer anOtherMode)  {
  if (!HasPresentation(aPresentableObject,aMode)){
    AddPresentation(aPresentableObject,aMode);
  }
  if (!HasPresentation(anOtherObject,anOtherMode)){
    AddPresentation(anOtherObject,anOtherMode);
  }
   CastPresentation(aPresentableObject,aMode)->Connect(CastPresentation(anOtherObject,aMode));
}

//=======================================================================
//function : Transform
//purpose  : 
//=======================================================================
void PrsMgr_PresentationManager3d::Transform(const Handle(PrsMgr_PresentableObject)& aPresentableObject,const Handle(Geom_Transformation)& aTransformation, const Standard_Integer aMode) {
CastPresentation(aPresentableObject,aMode)->Transform(aTransformation);
} 

//=======================================================================
//function : Place
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::Place(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Quantity_Length X,const Quantity_Length Y,const Quantity_Length Z,const Standard_Integer aMode) {
  CastPresentation(aPresentableObject,aMode)->Place(X,Y,Z);
}

//=======================================================================
//function : Multiply
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::Multiply(const Handle(PrsMgr_PresentableObject)& aPresentableObject,const Handle(Geom_Transformation)& aTransformation, const Standard_Integer aMode) {
  CastPresentation(aPresentableObject,aMode)->Multiply(aTransformation);
}

//=======================================================================
//function : Move
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::Move(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Quantity_Length X,const Quantity_Length Y,const Quantity_Length Z,const Standard_Integer aMode) {
  CastPresentation(aPresentableObject,aMode)->Move(X,Y,Z);
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::Color(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Quantity_NameOfColor aColor, const Standard_Integer aMode) {
  if (!HasPresentation(aPresentableObject,aMode)){
    AddPresentation(aPresentableObject,aMode);
  }
  else if(Presentation(aPresentableObject,aMode)->MustBeUpdated()){
    Update(aPresentableObject,aMode);
  }
  CastPresentation(aPresentableObject,aMode)->Color(aColor); 
  
  if(myImmediateMode)
    Add(aPresentableObject,aMode);

}

//=======================================================================
//function : BoundBox
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::BoundBox(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) {
  if (!HasPresentation(aPresentableObject,aMode)){
    AddPresentation(aPresentableObject,aMode);
  }
  else if(Presentation(aPresentableObject,aMode)->MustBeUpdated()){
    Update(aPresentableObject,aMode);
  }
  CastPresentation(aPresentableObject,aMode)->BoundBox();
}
//=======================================================================
//function : SetShadingAspect
//purpose  : 
//=======================================================================

//POP pour K4L
void PrsMgr_PresentationManager3d::SetShadingAspect(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Quantity_NameOfColor aColor, const Graphic3d_NameOfMaterial aMaterial, const Standard_Integer aMode) {
//void PrsMgr_PresentationManager3d::SetShadingAspect(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Quantity_NameOfColor aColor, const Graphic3d_NameOfPhysicalMaterial aMaterial, const Standard_Integer aMode) {

  Handle(Prs3d_ShadingAspect) sa = new Prs3d_ShadingAspect;
  sa->SetColor(aColor);
  sa->SetMaterial(aMaterial);
  SetShadingAspect(aPresentableObject,sa,aMode);
}

void PrsMgr_PresentationManager3d::SetShadingAspect(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Handle(Prs3d_ShadingAspect)& aShadingAspect, const Standard_Integer aMode) {

  if (HasPresentation(aPresentableObject,aMode)){
    CastPresentation(aPresentableObject,aMode)->SetShadingAspect(aShadingAspect);
  }
}


//=======================================================================
//function : CastPresentation
//purpose  : 
//=======================================================================

Handle(PrsMgr_Presentation3d) PrsMgr_PresentationManager3d::CastPresentation(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) const {
  return Handle(PrsMgr_Presentation3d)::DownCast(Presentation(aPresentableObject,aMode));
}



//=======================================================================
//function : BeginDraw
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::BeginDraw()
{
  PrsMgr_PresentationManager::BeginDraw();
  myStrList.Clear();
}

//=======================================================================
//function : EndDraw
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager3d::EndDraw(const Handle(Viewer_View)& aView, const Standard_Boolean DoubleBuffer) {
  
  
  Handle(V3d_View) v;
  v =*((Handle(V3d_View)*)&aView);
  if (!v->TransientManagerBeginDraw(DoubleBuffer,Standard_True))
  {
    myImmediateMode = Standard_False;
    return;
  }
  if(myImmediateList.IsEmpty() && myStrList.IsEmpty()) {
    Visual3d_TransientManager::EndDraw();
    myImmediateMode = Standard_False;
    return;
  }
  TColStd_ListIteratorOfListOfTransient it(myImmediateList);
  Handle(PrsMgr_Presentation3d) P3d;
  for (; it.More(); it.Next()) {
    P3d = *((Handle(PrsMgr_Presentation3d)*)&(it.Value()));
    Visual3d_TransientManager::DrawStructure(P3d->Presentation());
  }
  
  Handle(Standard_Transient) Tr;
  for(it.Initialize(myStrList);it.More();it.Next()){
    Tr = it.Value();
    Visual3d_TransientManager::DrawStructure(*((Handle(Prs3d_Presentation)*)&Tr));
  }
  
  Visual3d_TransientManager::EndDraw();
  myImmediateMode = Standard_False;
  
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
void PrsMgr_PresentationManager3d::AddToImmediateList(const Handle(Prs3d_Presentation)& P){
  myStrList.Append(P);
}
