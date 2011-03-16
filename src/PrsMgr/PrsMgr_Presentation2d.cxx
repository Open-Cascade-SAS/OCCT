// File:	PrsMgr_Presentation2d.cxx
// Created:	Thu Oct 21 13:11:19 1993
// Author:	Jean-Louis FRENKEL
// Modified by  rob 09-oct-96
//		<jlf@stylox>


#include <PrsMgr_Presentation2d.ixx>
#include <PrsMgr_PresentationManager.hxx>

PrsMgr_Presentation2d::PrsMgr_Presentation2d(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager)
:PrsMgr_Presentation(aPresentationManager), myStructure(new Graphic2d_GraphicObject(aPresentationManager->StructureManager())) {}

PrsMgr_KindOfPrs PrsMgr_Presentation2d::KindOfPresentation() const
{return PrsMgr_KOP_2D;}

void PrsMgr_Presentation2d::Display (){
  myStructure->Display();}

void PrsMgr_Presentation2d::Erase () const {
  myStructure->Erase();}

void PrsMgr_Presentation2d::Highlight () {
  myStructure->Highlight();}

void PrsMgr_Presentation2d::Unhighlight () const {
  myStructure->Unhighlight();}

void PrsMgr_Presentation2d::Highlight(const Standard_Integer anIndex) const{ 
  myStructure->Highlight(anIndex);}

Standard_Boolean PrsMgr_Presentation2d::IsDisplayed () const {
  return myStructure->IsDisplayed();
}        

Standard_Boolean PrsMgr_Presentation2d::IsHighlighted () const {
  return myStructure->IsHighlighted();
}


Standard_Integer PrsMgr_Presentation2d::DisplayPriority() const {
return -1;
}

void PrsMgr_Presentation2d::SetDisplayPriority(const Standard_Integer TheNewPrior)
{
  // peut etre Raise ?
}




void PrsMgr_Presentation2d::EnablePlot () const {
  myStructure->EnablePlot();
}

void PrsMgr_Presentation2d::DisablePlot () const {
  myStructure->DisablePlot();
}

Standard_Boolean PrsMgr_Presentation2d::IsPlottable () const {
  return myStructure->IsPlottable();
}

void PrsMgr_Presentation2d::Clear () {
  myStructure->RemovePrimitives();
}

Handle(Graphic2d_GraphicObject) PrsMgr_Presentation2d::Presentation() const {
  return myStructure;
}
void PrsMgr_Presentation2d::SetOffset(const Standard_Integer anOffset) { 
  myStructure->SetOffSet(anOffset);}

Standard_Integer PrsMgr_Presentation2d::Offset () const {
  return myStructure->OffSet();}


void PrsMgr_Presentation2d::Destroy () {
}
