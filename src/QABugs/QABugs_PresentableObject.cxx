// Created on: 2002-04-09
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#include <QABugs_PresentableObject.ixx>
#include <QABugs_PresentableObject.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Aspect_Array1OfEdge.hxx>
#include <Graphic3d_VertexC.hxx>
#include <Graphic3d_VertexNC.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>
#include <Graphic3d_Array1OfVertexNC.hxx>
#include <AIS_Drawer.hxx>

QABugs_PresentableObject::QABugs_PresentableObject(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d)
     :AIS_InteractiveObject(aTypeOfPresentation3d)
{
}

void QABugs_PresentableObject::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
				const Handle(Prs3d_Presentation)& aPresentation,
				const Standard_Integer aMode )
{
  Handle(Graphic3d_Structure) theStructure = Handle(Graphic3d_Structure)::DownCast(aPresentation);
  Handle(Graphic3d_Group) theGroup= new  Graphic3d_Group(theStructure);
  Handle_Prs3d_ShadingAspect theAspect = myDrawer->ShadingAspect();
  Graphic3d_MaterialAspect mat = theAspect->Aspect()->FrontMaterial();
  mat.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
  mat.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
  mat.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
  mat.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
  theAspect->SetMaterial(mat);
  theGroup->SetPrimitivesAspect(theAspect->Aspect());
  
  Aspect_Array1OfEdge aListEdge(1, 3);
  aListEdge.SetValue(1, Aspect_Edge(1, 2, Aspect_TOE_VISIBLE));
  aListEdge.SetValue(2, Aspect_Edge(2, 3, Aspect_TOE_VISIBLE));
  aListEdge.SetValue(3, Aspect_Edge(3, 1, Aspect_TOE_VISIBLE));
  theGroup->BeginPrimitives();

  switch (aMode) 
    {
    case 0://using VertexC
      {
	Graphic3d_Array1OfVertexC theArray1(1, 3);

	theArray1.SetValue(1, Graphic3d_VertexC(0,0,0,Quantity_NOC_RED));
	theArray1.SetValue(2, Graphic3d_VertexC(0,5,1,Quantity_NOC_BLUE1));
	theArray1.SetValue(3, Graphic3d_VertexC(5,0,1,Quantity_NOC_YELLOW));
	theGroup->TriangleSet(theArray1, aListEdge);
	
	theArray1.SetValue(1, Graphic3d_VertexC(0,5,1,Quantity_NOC_BLUE1));
	theArray1.SetValue(2, Graphic3d_VertexC(5,5,-1,Quantity_NOC_GREEN));
	theArray1.SetValue(3, Graphic3d_VertexC(5,0,1,Quantity_NOC_YELLOW));
	theGroup->TriangleSet(theArray1, aListEdge);
      }
      break;
    case 1://using VertexNC
      {
	Graphic3d_Array1OfVertexNC theArray1(1, 3);
	
	theArray1.SetValue(1, Graphic3d_VertexNC(5,0,0, //coord
						 0,0,1, //normal
						 Quantity_NOC_RED));
	theArray1.SetValue(2, Graphic3d_VertexNC(5,5,1, //coord
						 1,1,1, //normal
						 Quantity_NOC_BLUE1));
	theArray1.SetValue(3, Graphic3d_VertexNC(10,0,1, //coord
						 0,1,1,  //normal
						 Quantity_NOC_YELLOW));
	theGroup->TriangleSet(theArray1, aListEdge);
	
	theArray1.SetValue(1, Graphic3d_VertexNC(5,5,1, //coord
						 1,1,1, //normal
						 Quantity_NOC_BLUE1));
	theArray1.SetValue(2, Graphic3d_VertexNC(10,5,-1, //coord
						 0,0,-1,  //normal
						 Quantity_NOC_GREEN));
	theArray1.SetValue(3, Graphic3d_VertexNC(10,0,1, //coord
						 0,1,1,  //normal
						 Quantity_NOC_YELLOW));
	theGroup->TriangleSet(theArray1, aListEdge);
      }
      break;
    }
  theGroup->EndPrimitives();
  
}

void QABugs_PresentableObject::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
					 const Standard_Integer aMode) {
}
