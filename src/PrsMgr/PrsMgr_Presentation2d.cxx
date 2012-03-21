// Created on: 1993-10-21
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

// Modified by  rob 09-oct-96


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

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================

void PrsMgr_Presentation2d::SetZLayer (Standard_Integer theLayer)
{
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================

Standard_Integer PrsMgr_Presentation2d::GetZLayer () const
{
  return 0;
}
