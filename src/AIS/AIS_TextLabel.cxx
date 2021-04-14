// Created on: 2014-11-10
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <AIS_TextLabel.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Font_FTFont.hxx>
#include <Font_FontMgr.hxx>
#include <Font_Rect.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_Text.hxx>

#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>

#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_TextLabel,AIS_InteractiveObject)

//=======================================================================
//function : AIS_TextLabel
//purpose  :
//=======================================================================
AIS_TextLabel::AIS_TextLabel()
: myText              ("?"),
  myHasOrientation3D  (Standard_False),
  myHasOwnAnchorPoint (Standard_True),
  myHasFlipping       (Standard_False)
{
  myDrawer->SetTextAspect (new Prs3d_TextAspect());
  myDrawer->SetDisplayMode (0);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_TextLabel::SetColor (const Quantity_Color& theColor)
{
  hasOwnColor = Standard_True;
  myDrawer->SetColor (theColor);
  myDrawer->TextAspect()->SetColor (theColor);
  SynchronizeAspects();
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void AIS_TextLabel::SetTransparency (const Standard_Real theValue)
{
  Quantity_ColorRGBA aTextColor (myDrawer->TextAspect()->Aspect()->Color());
  aTextColor.SetAlpha (Standard_ShortReal(1.0 - theValue));

  Quantity_ColorRGBA aSubColor (myDrawer->TextAspect()->Aspect()->ColorSubTitle());
  aSubColor.SetAlpha (aTextColor.Alpha());

  myDrawer->TextAspect()->Aspect()->SetColor (aTextColor);
  myDrawer->TextAspect()->Aspect()->SetColorSubTitle (aSubColor);
  myDrawer->SetTransparency (Standard_ShortReal(theValue));
  SynchronizeAspects();
}

//=======================================================================
//function : SetText
//purpose  :
//=======================================================================
void AIS_TextLabel::SetText (const TCollection_ExtendedString& theText)
{
  myText = theText;
}

//=======================================================================
//function : SetPosition
//purpose  :
//=======================================================================
void AIS_TextLabel::SetPosition (const gp_Pnt& thePosition)
{
  myOrientation3D.SetLocation (thePosition);
}

//=======================================================================
//function : SetHJustification
//purpose  :
//=======================================================================
void AIS_TextLabel::SetHJustification (const Graphic3d_HorizontalTextAlignment theHJust)
{
  myDrawer->TextAspect()->SetHorizontalJustification (theHJust);
}

//=======================================================================
//function : SetVJustification
//purpose  : Setup vertical justification.
//=======================================================================
void AIS_TextLabel::SetVJustification (const Graphic3d_VerticalTextAlignment theVJust)
{
  myDrawer->TextAspect()->SetVerticalJustification (theVJust);
}

//=======================================================================
//function : SetAngle
//purpose  :
//=======================================================================
void AIS_TextLabel::SetAngle (const Standard_Real theAngle)
{
  myDrawer->TextAspect()->Aspect()->SetTextAngle (theAngle * 180.0 / M_PI);
}

//=======================================================================
//function : SetZoom
//purpose  :
//=======================================================================
void AIS_TextLabel::SetZoomable (const Standard_Boolean theIsZoomable)
{
  myDrawer->TextAspect()->Aspect()->SetTextZoomable (theIsZoomable == Standard_True);
}

//=======================================================================
//function : SetHeight
//purpose  :
//=======================================================================
void AIS_TextLabel::SetHeight (const Standard_Real theHeight)
{
  myDrawer->TextAspect()->SetHeight (theHeight);
}

//=======================================================================
//function : SetAngle
//purpose  :
//=======================================================================
void AIS_TextLabel::SetFontAspect (const Font_FontAspect theFontAspect)
{
  myDrawer->TextAspect()->Aspect()->SetTextFontAspect (theFontAspect);
}

//=======================================================================
//function : SetFont
//purpose  :
//=======================================================================
void AIS_TextLabel::SetFont (Standard_CString theFont)
{
  myDrawer->TextAspect()->SetFont (theFont);
}

//=======================================================================
//function : SetOrientation3D
//purpose  :
//=======================================================================
void AIS_TextLabel::SetOrientation3D (const gp_Ax2& theOrientation)
{
  myHasOrientation3D = Standard_True;
  myOrientation3D    = theOrientation;
}

//=======================================================================
//function : UnsetOrientation3D
//purpose  :
//=======================================================================
void AIS_TextLabel::UnsetOrientation3D ()
{
  myHasOrientation3D = Standard_False;
}

//=======================================================================
//function : Position
//purpose  :
//=======================================================================
const gp_Pnt& AIS_TextLabel::Position() const
{
  return myOrientation3D.Location();
}

//=======================================================================
//function : FontName
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_TextLabel::FontName() const
{
  return myDrawer->TextAspect()->Aspect()->Font();
}

//=======================================================================
//function : FontAspect
//purpose  :
//=======================================================================
Font_FontAspect AIS_TextLabel::FontAspect() const
{
  return myDrawer->TextAspect()->Aspect()->GetTextFontAspect();
}

//=======================================================================
//function : Orientation3D
//purpose  :
//=======================================================================
const gp_Ax2& AIS_TextLabel::Orientation3D() const
{
  return myOrientation3D;
}

//=======================================================================
//function : HasOrientation3D
//purpose  :
//=======================================================================
Standard_Boolean AIS_TextLabel::HasOrientation3D() const
{
  return myHasOrientation3D;
}

//=======================================================================
//function : SetFlipping
//purpose  :
//=======================================================================
void AIS_TextLabel::SetFlipping (const Standard_Boolean theIsFlipping)
{
  myHasFlipping = theIsFlipping;
}

//=======================================================================
//function : HasFlipping
//purpose  :
//=======================================================================
Standard_Boolean AIS_TextLabel::HasFlipping() const
{
  return myHasFlipping;
}

//=======================================================================
//function : SetDisplayType
//purpose  :
//=======================================================================
void AIS_TextLabel::SetDisplayType (const Aspect_TypeOfDisplayText theDisplayType)
{
  myDrawer->TextAspect()->Aspect()->SetDisplayType(theDisplayType);
}

//=======================================================================
//function : SetColorSubTitle
//purpose  :
//=======================================================================
void AIS_TextLabel::SetColorSubTitle (const Quantity_Color& theColor)
{
  myDrawer->TextAspect()->Aspect()->SetColorSubTitle(theColor);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_TextLabel::Compute (const Handle(PrsMgr_PresentationManager)& ,
                             const Handle(Prs3d_Presentation)& thePrs,
                             const Standard_Integer theMode)
{
  switch (theMode)
  {
    case 0:
    {
      Handle(Prs3d_TextAspect) anAsp = myDrawer->TextAspect();
      gp_Pnt aPosition = Position();

      const Standard_Boolean isTextZoomable = anAsp->Aspect()->GetTextZoomable();
      if (myHasOrientation3D)
      {
        anAsp->Aspect()->SetTextZoomable (myHasFlipping ? Standard_True : Standard_False);
        SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_ZoomPers, aPosition));
        aPosition = gp::Origin();
      }
      else if (isTextZoomable
            || TransformPersistence().IsNull()
            || TransformPersistence()->Mode() != Graphic3d_TMF_2d)
      {
        Handle(Graphic3d_TransformPers) aTrsfPers =
          new Graphic3d_TransformPers (isTextZoomable ? Graphic3d_TMF_RotatePers : Graphic3d_TMF_ZoomRotatePers, aPosition);
        SetTransformPersistence (aTrsfPers);
        aPosition = gp::Origin();
      }

      gp_Pnt aCenterOfLabel;
      Standard_Real aWidth, aHeight;

      Standard_Boolean isInit = calculateLabelParams (aPosition, aCenterOfLabel, aWidth, aHeight);
      if (myHasOrientation3D)
      {
        if (myHasFlipping)
        {
          gp_Ax2 aFlippingAxes (aCenterOfLabel, myOrientation3D.Direction(), myOrientation3D.XDirection());
          thePrs->CurrentGroup()->SetFlippingOptions (Standard_True, aFlippingAxes);
        }
        gp_Ax2 anOrientation = myOrientation3D;
        anOrientation.SetLocation (aPosition);
        Standard_Boolean aHasOwnAnchor = HasOwnAnchorPoint();
        if (myHasFlipping)
        {
          aHasOwnAnchor = Standard_False; // always not using own anchor if flipping
        }
        Handle(Graphic3d_Text) aText = 
          Prs3d_Text::Draw (thePrs->CurrentGroup(), anAsp, myText, anOrientation, aHasOwnAnchor);
        aText->SetTextFormatter (myFormatter);
        if (myHasFlipping && isInit)
        {
          thePrs->CurrentGroup()->SetFlippingOptions (Standard_False, gp_Ax2());
        }
      }
      else
      {
        Handle(Graphic3d_Text) aText =
          Prs3d_Text::Draw (thePrs->CurrentGroup(), anAsp, myText, aPosition);
        aText->SetTextFormatter (myFormatter);
      }

      if (isInit)
      {
        const Standard_Real aDx = aWidth * 0.5;
        const Standard_Real aDy = aHeight * 0.5;
        gp_Trsf aLabelPlane = calculateLabelTrsf (aPosition, aCenterOfLabel);

        gp_Pnt aMinPnt = gp_Pnt (-aDx, -aDy, 0.0).Transformed (aLabelPlane);
        gp_Pnt aMaxPnt = gp_Pnt ( aDx,  aDy, 0.0).Transformed (aLabelPlane);

        Graphic3d_BndBox4f& aBox = thePrs->CurrentGroup()->ChangeBoundingBox();
        aBox.Add (Graphic3d_Vec4 ((float) aMinPnt.X(), (float) aMinPnt.Y(), (float) aMinPnt.Z(), 1.0));
        aBox.Add (Graphic3d_Vec4 ((float) aMaxPnt.X(), (float) aMaxPnt.Y(), (float) aMaxPnt.Z(), 1.0));
      }

      break;
    }
  }
}

//=======================================================================
//function : ComputeSelection
//purpose  :
//=======================================================================
void AIS_TextLabel::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                      const Standard_Integer             theMode)
{
  switch (theMode)
  {
    case 0:
    {
      Handle(SelectMgr_EntityOwner) anEntityOwner   = new SelectMgr_EntityOwner (this, 10);

      gp_Pnt aPosition = Position();
      if (!TransformPersistence().IsNull() && TransformPersistence()->Mode() != Graphic3d_TMF_2d)
      {
        aPosition = gp::Origin();
      }

      gp_Pnt aCenterOfLabel;
      Standard_Real aWidth, aHeight;

      if (!calculateLabelParams (aPosition, aCenterOfLabel, aWidth, aHeight))
      {
        Handle(Select3D_SensitivePoint) aTextSensitive = new Select3D_SensitivePoint (anEntityOwner, aPosition);
        theSelection->Add (aTextSensitive);
        break;
      }

      const Standard_Real aDx = aWidth * 0.5;
      const Standard_Real aDy = aHeight * 0.5;
      gp_Trsf aLabelPlane = calculateLabelTrsf (aPosition, aCenterOfLabel);

      // sensitive planar rectangle for text
      TColgp_Array1OfPnt aRectanglePoints (1, 5);
      aRectanglePoints.ChangeValue (1) = gp_Pnt (-aDx, -aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (2) = gp_Pnt (-aDx,  aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (3) = gp_Pnt ( aDx,  aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (4) = gp_Pnt ( aDx, -aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (5) = aRectanglePoints.Value (1);

      Handle(Select3D_SensitiveFace) aTextSensitive =
        new Select3D_SensitiveFace (anEntityOwner, aRectanglePoints, Select3D_TOS_INTERIOR);
      theSelection->Add (aTextSensitive);

      break;
    }
  }
}

//=======================================================================
//function : calculateLabelParams
//purpose  :
//=======================================================================
Standard_Boolean AIS_TextLabel::calculateLabelParams (const gp_Pnt& thePosition,
                                                      gp_Pnt& theCenterOfLabel,
                                                      Standard_Real& theWidth,
                                                      Standard_Real& theHeight) const
{
  // Get width and height of text
  Handle(Prs3d_TextAspect) anAsp = myDrawer->TextAspect();
  const Graphic3d_RenderingParams& aRendParams = GetContext()->CurrentViewer()->DefaultRenderingParams();
  Font_FTFontParams aFontParams;
  aFontParams.PointSize = (unsigned int) anAsp->Height();
  aFontParams.Resolution = aRendParams.Resolution;
  aFontParams.FontHinting = aRendParams.FontHinting;

  Handle(Font_FTFont) aFont = Font_FTFont::FindAndCreate (anAsp->Aspect()->Font(),
                                                          anAsp->Aspect()->GetTextFontAspect(),
                                                          aFontParams);
  if (aFont.IsNull())
  { 
    return Standard_False;
  }

  const NCollection_String aText (myText.ToExtString());
  Font_Rect aBndBox = aFont->BoundingBox (aText, anAsp->HorizontalJustification(), anAsp->VerticalJustification());
  theWidth = Abs (aBndBox.Width());
  theHeight = Abs (aBndBox.Height());

  theCenterOfLabel = thePosition;
  if (anAsp->VerticalJustification() == Graphic3d_VTA_BOTTOM)
  {
    theCenterOfLabel.ChangeCoord() += myOrientation3D.YDirection().XYZ() * theHeight * 0.5;
  }
  else if (anAsp->VerticalJustification() == Graphic3d_VTA_TOP)
  {
    theCenterOfLabel.ChangeCoord() -= myOrientation3D.YDirection().XYZ() * theHeight * 0.5;
  }
  if (anAsp->HorizontalJustification() == Graphic3d_HTA_LEFT)
  {
    theCenterOfLabel.ChangeCoord() += myOrientation3D.XDirection().XYZ() * theWidth * 0.5;
  }
  else if (anAsp->HorizontalJustification() == Graphic3d_HTA_RIGHT)
  {
    theCenterOfLabel.ChangeCoord() -= myOrientation3D.XDirection().XYZ() * theWidth * 0.5;
  }

  return Standard_True;
}

//=======================================================================
//function : calculateLabelTrsf
//purpose  :
//=======================================================================
gp_Trsf AIS_TextLabel::calculateLabelTrsf (const gp_Pnt& thePosition, gp_Pnt& theCenterOfLabel) const
{
  const Standard_Real anAngle = myDrawer->TextAspect()->Aspect()->TextAngle() * M_PI / 180.0;
  const gp_Ax1 aRotAxis (thePosition, gp_Dir (0.0, 0.0, 1.0));

  gp_Ax2 anOrientation = myOrientation3D;
  anOrientation.Rotate (aRotAxis, anAngle);
  theCenterOfLabel.Rotate (aRotAxis, anAngle);

  gp_Trsf aLabelPlane;
  aLabelPlane.SetTransformation (anOrientation, gp::XOY());
  aLabelPlane.SetTranslationPart (theCenterOfLabel.XYZ());

  return aLabelPlane;
}
