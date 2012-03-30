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
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <AIS_Drawer.hxx>

QABugs_PresentableObject::QABugs_PresentableObject(const PrsMgr_TypeOfPresentation3d theTypeOfPresentation3d)
     :AIS_InteractiveObject(theTypeOfPresentation3d)
{
}

void QABugs_PresentableObject::Compute(const Handle(PrsMgr_PresentationManager3d)& ,
				const Handle(Prs3d_Presentation)& thePrs,
				const Standard_Integer theMode)
{
  Handle(Graphic3d_Structure) aStructure = Handle(Graphic3d_Structure)::DownCast (thePrs);
  Handle(Graphic3d_Group) aGroup = new Graphic3d_Group (aStructure);
  Handle_Prs3d_ShadingAspect anAspect = myDrawer->ShadingAspect();
  Graphic3d_MaterialAspect aMat = anAspect->Aspect()->FrontMaterial();
  aMat.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  aMat.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  aMat.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  aMat.SetReflectionModeOff (Graphic3d_TOR_EMISSION);
  anAspect->SetMaterial (aMat);
  aGroup->SetPrimitivesAspect (anAspect->Aspect());

  Handle(Graphic3d_ArrayOfTriangles) aPrims
    = new Graphic3d_ArrayOfTriangles (6, 0,
                                      theMode == 1,   // normals
                                      Standard_True); // color per vertex
  switch (theMode)
  {
    case 0:
    {
      aPrims->AddVertex (gp_Pnt (0.0,  0.0,  0.0), Quantity_Color (Quantity_NOC_RED));
      aPrims->AddVertex (gp_Pnt (0.0,  5.0,  1.0), Quantity_Color (Quantity_NOC_BLUE1));
      aPrims->AddVertex (gp_Pnt (5.0,  0.0,  1.0), Quantity_Color (Quantity_NOC_YELLOW));

      aPrims->AddVertex (gp_Pnt (0.0,  5.0,  1.0), Quantity_Color (Quantity_NOC_BLUE1));
      aPrims->AddVertex (gp_Pnt (5.0,  5.0, -1.0), Quantity_Color (Quantity_NOC_GREEN));
      aPrims->AddVertex (gp_Pnt (5.0,  0.0,  1.0), Quantity_Color (Quantity_NOC_YELLOW));
      break;
    }
    case 1:
    {
      aPrims->AddVertex (gp_Pnt ( 5.0, 0.0,  0.0), gp_Dir (0.0, 0.0,  1.0), Quantity_Color (Quantity_NOC_RED));
      aPrims->AddVertex (gp_Pnt ( 5.0, 5.0,  1.0), gp_Dir (1.0, 1.0,  1.0), Quantity_Color (Quantity_NOC_BLUE1));
      aPrims->AddVertex (gp_Pnt (10.0, 0.0,  1.0), gp_Dir (0.0, 1.0,  1.0), Quantity_Color (Quantity_NOC_YELLOW));

      aPrims->AddVertex (gp_Pnt ( 5.0, 5.0,  1.0), gp_Dir (1.0, 1.0,  1.0), Quantity_Color (Quantity_NOC_BLUE1));
      aPrims->AddVertex (gp_Pnt (10.0, 5.0, -1.0), gp_Dir (0.0, 0.0, -1.0), Quantity_Color (Quantity_NOC_GREEN));
      aPrims->AddVertex (gp_Pnt (10.0, 0.0,  1.0), gp_Dir (0.0, 1.0,  1.0), Quantity_Color (Quantity_NOC_YELLOW));
    }
    break;
  }

  aGroup->BeginPrimitives();
  aGroup->AddPrimitiveArray (aPrims);
  aGroup->EndPrimitives();
}

void QABugs_PresentableObject::ComputeSelection(const Handle(SelectMgr_Selection)& ,
					 const Standard_Integer ) {
}
