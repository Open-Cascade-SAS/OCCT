// Copyright (c) 2013 OPEN CASCADE SAS
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

#include <DsgPrs_DatumPrs.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>

void DsgPrs_DatumPrs::Add (const Handle(Prs3d_Presentation)& thePresentation,
                           const gp_Ax2&                     theDatum,
                           const Handle(Prs3d_Drawer)&       theDrawer)
{
  Handle(Prs3d_DatumAspect) aDatumAspect = theDrawer->DatumAspect();
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(thePresentation);

  Quantity_Color aColor;
  Aspect_TypeOfLine aTypeOfLine;
  Standard_Real aWidth;
  aDatumAspect->FirstAxisAspect()->Aspect()->Values(aColor, aTypeOfLine, aWidth);

  gp_Ax2 anAxis(theDatum);
  gp_Pnt anOrigin = anAxis.Location();
  gp_Dir aXDir = anAxis.XDirection();
  gp_Dir aYDir = anAxis.YDirection();
  gp_Dir aZDir = anAxis.Direction();

  Quantity_Length anAxisLength;
  Quantity_Length anArrowAngle = theDrawer->ArrowAspect()->Angle();

  Handle(Graphic3d_ArrayOfSegments) aPrims;
  if (aDatumAspect->DrawFirstAndSecondAxis())
  {
    anAxisLength = aDatumAspect->FirstAxisLength();
    const gp_Pnt aPoint1(anOrigin.XYZ() + aXDir.XYZ()*anAxisLength);
    
    aGroup->SetPrimitivesAspect(aDatumAspect->FirstAxisAspect()->Aspect());
    aPrims = new Graphic3d_ArrayOfSegments(2);
    aPrims->AddVertex(anOrigin);
    aPrims->AddVertex(aPoint1);
    aGroup->AddPrimitiveArray(aPrims);

    aGroup->SetPrimitivesAspect(theDrawer->ArrowAspect()->Aspect());
    Prs3d_Arrow::Draw(thePresentation,aPoint1,aXDir,anArrowAngle,anAxisLength/10.);
    aGroup->SetPrimitivesAspect(theDrawer->TextAspect()->Aspect());
    Graphic3d_Vertex aVertex1(aPoint1.X(),aPoint1.Y(),aPoint1.Z());
    aGroup->Text(Standard_CString("X"), aVertex1,16.);

    anAxisLength = aDatumAspect->SecondAxisLength();
    const gp_Pnt aPoint2(anOrigin.XYZ() + aYDir.XYZ()*anAxisLength);

    aGroup->SetPrimitivesAspect(aDatumAspect->SecondAxisAspect()->Aspect());
    aPrims = new Graphic3d_ArrayOfSegments(2);
    aPrims->AddVertex(anOrigin);
    aPrims->AddVertex(aPoint2);
    aGroup->AddPrimitiveArray(aPrims);

    aGroup->SetPrimitivesAspect(theDrawer->ArrowAspect()->Aspect());
    Prs3d_Arrow::Draw(thePresentation,aPoint2,aYDir,anArrowAngle,anAxisLength/10.);
    aGroup->SetPrimitivesAspect(theDrawer->TextAspect()->Aspect());
    Graphic3d_Vertex aVertex2(aPoint2.X(),aPoint2.Y(),aPoint2.Z());
    aGroup->Text(Standard_CString("Y"), aVertex2,16.);
  }
  if (aDatumAspect->DrawThirdAxis())
  {
    anAxisLength = aDatumAspect->ThirdAxisLength();
    const gp_Pnt aPoint3(anOrigin.XYZ() + aZDir.XYZ()*anAxisLength);

    aGroup->SetPrimitivesAspect(aDatumAspect->ThirdAxisAspect()->Aspect());
    aPrims = new Graphic3d_ArrayOfSegments(2);
    aPrims->AddVertex(anOrigin);
    aPrims->AddVertex(aPoint3);
    aGroup->AddPrimitiveArray(aPrims);

    aGroup->SetPrimitivesAspect(theDrawer->ArrowAspect()->Aspect());
    Prs3d_Arrow::Draw(thePresentation,aPoint3,aZDir,anArrowAngle,anAxisLength/10.);
    aGroup->SetPrimitivesAspect(theDrawer->TextAspect()->Aspect());
    Graphic3d_Vertex aVertex3(aPoint3.X(),aPoint3.Y(),aPoint3.Z());
    aGroup->Text(Standard_CString("Z"), aVertex3,16.);
  }
}

