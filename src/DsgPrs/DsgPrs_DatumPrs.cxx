// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

