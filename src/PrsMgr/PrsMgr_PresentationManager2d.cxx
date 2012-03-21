// Copyright (c) 1998-1999 Matra Datavision
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

#define G002	// TCL 040500 The Transientmanager has changed
//		to avoid unavailable cyclic dependency under WNT.

#include <PrsMgr_PresentationManager2d.ixx>
#include <PrsMgr_Presentation2d.hxx>
#include <Standard_NotImplemented.hxx>
#include <Graphic2d_TransientManager.hxx>
#include <V2d_View.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>



PrsMgr_PresentationManager2d::PrsMgr_PresentationManager2d (const Handle(Graphic2d_View)& aStructureManager):myStructureManager(aStructureManager) {
#ifdef G002
  myTM = new Graphic2d_TransientManager(myStructureManager.operator -> ());
#else
  myTM = new Graphic2d_TransientManager(myStructureManager);
#endif
  myTM->SetOverrideColor(32);
  myTM->SetMapping(Standard_True);
}


Standard_Boolean PrsMgr_PresentationManager2d::Is3D() const
{return Standard_False;}

void PrsMgr_PresentationManager2d::ColorHighlight(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aColor, const Standard_Integer aMode) {
  if (!HasPresentation(aPresentableObject,aMode)){
    AddPresentation(aPresentableObject,aMode);
  }
  CastPresentation(aPresentableObject,aMode)->Highlight(aColor); 

  // immediate mode
//  Add(aPresentableObject,aMode);
}

void PrsMgr_PresentationManager2d::EnablePlot(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) {
  CastPresentation(aPresentableObject,aMode)->EnablePlot();
}

void PrsMgr_PresentationManager2d::DisablePlot(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) {
  CastPresentation(aPresentableObject,aMode)->DisablePlot();
}

Standard_Boolean PrsMgr_PresentationManager2d::IsPlottable(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) const {
  return CastPresentation(aPresentableObject,aMode)->IsPlottable();
}

void PrsMgr_PresentationManager2d::SetHighlightColor(const Standard_Integer anIndex) {
  myStructureManager->SetDefaultOverrideColor(anIndex);
}

Handle(PrsMgr_Presentation) PrsMgr_PresentationManager2d::newPresentation(const Handle(PrsMgr_PresentableObject) & aPresentableObject) {
 return new PrsMgr_Presentation2d(this);
}

Handle(Graphic2d_View) PrsMgr_PresentationManager2d::StructureManager () const {
  return myStructureManager;
}


void PrsMgr_PresentationManager2d::Dump
       (const Handle(Graphic2d_Buffer)& aBuffer,
	const Handle(PrsMgr_PresentableObject)& aPresentableObject, 
	const Standard_Integer aMode) {
       if (!HasPresentation(aPresentableObject,aMode)){
	 AddPresentation(aPresentableObject,aMode);
       }
       aBuffer->Add(CastPresentation(aPresentableObject,aMode)->Presentation());
       
     }

Handle(PrsMgr_Presentation2d) PrsMgr_PresentationManager2d::CastPresentation(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) const {
  return Handle(PrsMgr_Presentation2d)::DownCast(Presentation(aPresentableObject,aMode));
}
void PrsMgr_PresentationManager2d::InitMinMax() {
  MinMaxList = new Graphic2d_DisplayList;
}
void PrsMgr_PresentationManager2d::AddMinMax(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer aMode) {

  if (HasPresentation(aPresentableObject,aMode)){
    MinMaxList->Append(CastPresentation(aPresentableObject,aMode)->Presentation());}
}


void PrsMgr_PresentationManager2d::MinMax(Standard_Real& MinX,
					  Standard_Real& MaxX,
					  Standard_Real& MinY,
					  Standard_Real& MaxY) const {

	 myStructureManager->MinMax(MinMaxList,MinX,MaxX,MinY,MaxY);
}
void PrsMgr_PresentationManager2d::SetOffset(const Handle(PrsMgr_PresentableObject)& aPresentableObject, const Standard_Integer anOffset,const Standard_Integer aMode)  {
  if (HasPresentation(aPresentableObject,aMode)){
    CastPresentation(aPresentableObject,aMode)->SetOffset(anOffset);
  }

  // immediate mode
//  Add(aPresentableObject,aMode);
}

Standard_Integer PrsMgr_PresentationManager2d::Offset(const Handle(PrsMgr_PresentableObject)& aPresentableObject,const Standard_Integer aMode) const{ 
  return CastPresentation(aPresentableObject,aMode)->Offset();}



//=======================================================================
//function : BeginDraw
//purpose  : 
//=======================================================================
void  PrsMgr_PresentationManager2d::BeginDraw()
{
  PrsMgr_PresentationManager::BeginDraw();
  myGOList.Clear();
}

//=======================================================================
//function : AddToImmediateList
//purpose  : 
//=======================================================================
void PrsMgr_PresentationManager2d::AddToImmediateList(const Handle(Graphic2d_GraphicObject)& GO)
{myGOList.Append(GO);}

//=======================================================================
//function : EndDraw
//purpose  : 
//=======================================================================

void PrsMgr_PresentationManager2d::EndDraw(const Handle(Viewer_View)& aView, const Standard_Boolean DoubleBuffer) {
  Handle(V2d_View) aV2d = Handle(V2d_View)::DownCast(aView);  
  myTM->BeginDraw(Handle(Aspect_WindowDriver)::DownCast(aV2d->Driver()),Standard_True);

  TColStd_ListIteratorOfListOfTransient it(myImmediateList);
  while (it.More()) {
    Handle(Graphic2d_GraphicObject) aGO = Handle(PrsMgr_Presentation2d)::DownCast(it.Value())->Presentation();
    if (!aGO.IsNull()) myTM->Draw(aGO);
    it.Next();
  }
  
  Handle(Standard_Transient) Tr;
  for(it.Initialize(myGOList);it.More();it.Next()){
    Tr =it.Value();
    myTM->Draw(*((Handle(Graphic2d_GraphicObject)*)&Tr));
  }

  myTM->EndDraw(Standard_True);
  myImmediateMode = Standard_False;
}

