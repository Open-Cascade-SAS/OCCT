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

IMPLEMENT_STANDARD_RTTIEXT(AIS_TextLabel, AIS_InteractiveObject)

//=================================================================================================

AIS_TextLabel::AIS_TextLabel()
    : myText("?"),
      myHasOrientation3D(false),
      myHasOwnAnchorPoint(true),
      myHasFlipping(false)
{
  myDrawer->SetTextAspect(new Prs3d_TextAspect());
  myDrawer->SetDisplayMode(0);
}

//=================================================================================================

void AIS_TextLabel::SetColor(const Quantity_Color& theColor)
{
  hasOwnColor = true;
  myDrawer->SetColor(theColor);
  myDrawer->TextAspect()->SetColor(theColor);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_TextLabel::SetTransparency(const double theValue)
{
  Quantity_ColorRGBA aTextColor(myDrawer->TextAspect()->Aspect()->Color());
  aTextColor.SetAlpha(float(1.0 - theValue));

  Quantity_ColorRGBA aSubColor(myDrawer->TextAspect()->Aspect()->ColorSubTitle());
  aSubColor.SetAlpha(aTextColor.Alpha());

  myDrawer->TextAspect()->Aspect()->SetColor(aTextColor);
  myDrawer->TextAspect()->Aspect()->SetColorSubTitle(aSubColor);
  myDrawer->SetTransparency(float(theValue));
  SynchronizeAspects();
}

//=================================================================================================

void AIS_TextLabel::SetText(const TCollection_ExtendedString& theText)
{
  myText = theText;
}

//=================================================================================================

void AIS_TextLabel::SetPosition(const gp_Pnt& thePosition)
{
  myOrientation3D.SetLocation(thePosition);
}

//=================================================================================================

void AIS_TextLabel::SetHJustification(const Graphic3d_HorizontalTextAlignment theHJust)
{
  myDrawer->TextAspect()->SetHorizontalJustification(theHJust);
}

//=======================================================================
// function : SetVJustification
// purpose  : Setup vertical justification.
//=======================================================================
void AIS_TextLabel::SetVJustification(const Graphic3d_VerticalTextAlignment theVJust)
{
  myDrawer->TextAspect()->SetVerticalJustification(theVJust);
}

//=================================================================================================

void AIS_TextLabel::SetAngle(const double theAngle)
{
  myDrawer->TextAspect()->Aspect()->SetTextAngle(theAngle * 180.0 / M_PI);
}

//=================================================================================================

void AIS_TextLabel::SetZoomable(const bool theIsZoomable)
{
  myDrawer->TextAspect()->Aspect()->SetTextZoomable(theIsZoomable == true);
}

//=================================================================================================

void AIS_TextLabel::SetHeight(const double theHeight)
{
  myDrawer->TextAspect()->SetHeight(theHeight);
}

//=================================================================================================

void AIS_TextLabel::SetFontAspect(const Font_FontAspect theFontAspect)
{
  myDrawer->TextAspect()->Aspect()->SetTextFontAspect(theFontAspect);
}

//=================================================================================================

void AIS_TextLabel::SetFont(const char* theFont)
{
  myDrawer->TextAspect()->SetFont(theFont);
}

//=================================================================================================

void AIS_TextLabel::SetOrientation3D(const gp_Ax2& theOrientation)
{
  myHasOrientation3D = true;
  myOrientation3D    = theOrientation;
}

//=================================================================================================

void AIS_TextLabel::UnsetOrientation3D()
{
  myHasOrientation3D = false;
}

//=================================================================================================

const gp_Pnt& AIS_TextLabel::Position() const
{
  return myOrientation3D.Location();
}

//=================================================================================================

const TCollection_AsciiString& AIS_TextLabel::FontName() const
{
  return myDrawer->TextAspect()->Aspect()->Font();
}

//=================================================================================================

Font_FontAspect AIS_TextLabel::FontAspect() const
{
  return myDrawer->TextAspect()->Aspect()->GetTextFontAspect();
}

//=================================================================================================

const gp_Ax2& AIS_TextLabel::Orientation3D() const
{
  return myOrientation3D;
}

//=================================================================================================

bool AIS_TextLabel::HasOrientation3D() const
{
  return myHasOrientation3D;
}

//=================================================================================================

void AIS_TextLabel::SetFlipping(const bool theIsFlipping)
{
  myHasFlipping = theIsFlipping;
}

//=================================================================================================

bool AIS_TextLabel::HasFlipping() const
{
  return myHasFlipping;
}

//=================================================================================================

void AIS_TextLabel::SetDisplayType(const Aspect_TypeOfDisplayText theDisplayType)
{
  myDrawer->TextAspect()->Aspect()->SetDisplayType(theDisplayType);
}

//=================================================================================================

void AIS_TextLabel::SetColorSubTitle(const Quantity_Color& theColor)
{
  myDrawer->TextAspect()->Aspect()->SetColorSubTitle(theColor);
}

//=================================================================================================

void AIS_TextLabel::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                            const occ::handle<Prs3d_Presentation>& thePrs,
                            const int                              theMode)
{
  switch (theMode)
  {
    case 0: {
      occ::handle<Prs3d_TextAspect> anAsp     = myDrawer->TextAspect();
      gp_Pnt                        aPosition = Position();

      const bool isTextZoomable = anAsp->Aspect()->GetTextZoomable();
      if (myHasOrientation3D)
      {
        anAsp->Aspect()->SetTextZoomable(myHasFlipping ? true : false);
        SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_ZoomPers, aPosition));
        aPosition = gp::Origin();
      }
      else if (isTextZoomable || TransformPersistence().IsNull()
               || TransformPersistence()->Mode() != Graphic3d_TMF_2d)
      {
        occ::handle<Graphic3d_TransformPers> aTrsfPers = new Graphic3d_TransformPers(
          isTextZoomable ? Graphic3d_TMF_RotatePers : Graphic3d_TMF_ZoomRotatePers,
          aPosition);
        SetTransformPersistence(aTrsfPers);
        aPosition = gp::Origin();
      }

      gp_Pnt aCenterOfLabel;
      double aWidth, aHeight;

      bool isInit = calculateLabelParams(aPosition, aCenterOfLabel, aWidth, aHeight);
      if (myHasOrientation3D)
      {
        if (myHasFlipping)
        {
          gp_Ax2 aFlippingAxes(aCenterOfLabel,
                               myOrientation3D.Direction(),
                               myOrientation3D.XDirection());
          thePrs->CurrentGroup()->SetFlippingOptions(true, aFlippingAxes);
        }
        gp_Ax2 anOrientation = myOrientation3D;
        anOrientation.SetLocation(aPosition);
        bool aHasOwnAnchor = HasOwnAnchorPoint();
        if (myHasFlipping)
        {
          aHasOwnAnchor = false; // always not using own anchor if flipping
        }
        occ::handle<Graphic3d_Text> aText =
          Prs3d_Text::Draw(thePrs->CurrentGroup(), anAsp, myText, anOrientation, aHasOwnAnchor);
        aText->SetTextFormatter(myFormatter);
        if (myHasFlipping && isInit)
        {
          thePrs->CurrentGroup()->SetFlippingOptions(false, gp_Ax2());
        }
      }
      else
      {
        occ::handle<Graphic3d_Text> aText =
          Prs3d_Text::Draw(thePrs->CurrentGroup(), anAsp, myText, aPosition);
        aText->SetTextFormatter(myFormatter);
      }

      if (isInit)
      {
        const double aDx         = aWidth * 0.5;
        const double aDy         = aHeight * 0.5;
        gp_Trsf      aLabelPlane = calculateLabelTrsf(aPosition, aCenterOfLabel);

        gp_Pnt aMinPnt = gp_Pnt(-aDx, -aDy, 0.0).Transformed(aLabelPlane);
        gp_Pnt aMaxPnt = gp_Pnt(aDx, aDy, 0.0).Transformed(aLabelPlane);

        Graphic3d_BndBox4f& aBox = thePrs->CurrentGroup()->ChangeBoundingBox();
        aBox.Add(
          NCollection_Vec4<float>((float)aMinPnt.X(), (float)aMinPnt.Y(), (float)aMinPnt.Z(), 1.0));
        aBox.Add(
          NCollection_Vec4<float>((float)aMaxPnt.X(), (float)aMaxPnt.Y(), (float)aMaxPnt.Z(), 1.0));
      }

      break;
    }
  }
}

//=================================================================================================

void AIS_TextLabel::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                     const int                               theMode)
{
  switch (theMode)
  {
    case 0: {
      occ::handle<SelectMgr_EntityOwner> anEntityOwner = new SelectMgr_EntityOwner(this, 10);

      gp_Pnt aPosition = Position();
      if (!TransformPersistence().IsNull() && TransformPersistence()->Mode() != Graphic3d_TMF_2d)
      {
        aPosition = gp::Origin();
      }

      gp_Pnt aCenterOfLabel;
      double aWidth, aHeight;

      if (!calculateLabelParams(aPosition, aCenterOfLabel, aWidth, aHeight))
      {
        occ::handle<Select3D_SensitivePoint> aTextSensitive =
          new Select3D_SensitivePoint(anEntityOwner, aPosition);
        theSelection->Add(aTextSensitive);
        break;
      }

      const double aDx         = aWidth * 0.5;
      const double aDy         = aHeight * 0.5;
      gp_Trsf      aLabelPlane = calculateLabelTrsf(aPosition, aCenterOfLabel);

      // sensitive planar rectangle for text
      NCollection_Array1<gp_Pnt> aRectanglePoints(1, 5);
      aRectanglePoints.ChangeValue(1) = gp_Pnt(-aDx, -aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(2) = gp_Pnt(-aDx, aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(3) = gp_Pnt(aDx, aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(4) = gp_Pnt(aDx, -aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(5) = aRectanglePoints.Value(1);

      occ::handle<Select3D_SensitiveFace> aTextSensitive =
        new Select3D_SensitiveFace(anEntityOwner, aRectanglePoints, Select3D_TOS_INTERIOR);
      theSelection->Add(aTextSensitive);

      break;
    }
  }
}

//=================================================================================================

bool AIS_TextLabel::calculateLabelParams(const gp_Pnt& thePosition,
                                         gp_Pnt&       theCenterOfLabel,
                                         double&       theWidth,
                                         double&       theHeight) const
{
  // Get width and height of text
  occ::handle<Prs3d_TextAspect>    anAsp = myDrawer->TextAspect();
  const Graphic3d_RenderingParams& aRendParams =
    GetContext()->CurrentViewer()->DefaultRenderingParams();
  Font_FTFontParams aFontParams;
  aFontParams.PointSize   = (unsigned int)anAsp->Height();
  aFontParams.Resolution  = aRendParams.Resolution;
  aFontParams.FontHinting = aRendParams.FontHinting;

  occ::handle<Font_FTFont> aFont = Font_FTFont::FindAndCreate(anAsp->Aspect()->Font(),
                                                              anAsp->Aspect()->GetTextFontAspect(),
                                                              aFontParams);
  if (aFont.IsNull())
  {
    return false;
  }

  const NCollection_String aText(myText.ToExtString());
  Font_Rect                aBndBox =
    aFont->BoundingBox(aText, anAsp->HorizontalJustification(), anAsp->VerticalJustification());
  theWidth  = std::abs(aBndBox.Width());
  theHeight = std::abs(aBndBox.Height());

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

  return true;
}

//=================================================================================================

gp_Trsf AIS_TextLabel::calculateLabelTrsf(const gp_Pnt& thePosition, gp_Pnt& theCenterOfLabel) const
{
  const double anAngle = myDrawer->TextAspect()->Aspect()->TextAngle() * M_PI / 180.0;
  const gp_Ax1 aRotAxis(thePosition, gp_Dir(gp_Dir::D::Z));

  gp_Ax2 anOrientation = myOrientation3D;
  anOrientation.Rotate(aRotAxis, anAngle);
  theCenterOfLabel.Rotate(aRotAxis, anAngle);

  gp_Trsf aLabelPlane;
  aLabelPlane.SetTransformation(anOrientation, gp::XOY());
  aLabelPlane.SetTranslationPart(theCenterOfLabel.XYZ());

  return aLabelPlane;
}
