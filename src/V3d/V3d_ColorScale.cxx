// Created on: 2004-06-22
// Created by: STV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <V3d_ColorScale.ixx>

#include <V3d_View.hxx>
#include <V3d_LayerMgr.hxx>

#include <Visual3d_View.hxx>
#include <Visual3d_Layer.hxx>
#include <Visual3d_ViewManager.hxx>

#include <Aspect_Window.hxx>
#include <Aspect_SequenceOfColor.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Font_NameOfFont.hxx>

V3d_ColorScale::V3d_ColorScale (const Handle(V3d_LayerMgr)& theMgr)
: Aspect_ColorScale(),
myLayerMgr( theMgr.operator->() ),
myDisplay( Standard_False )
{
}

void V3d_ColorScale::Display()
{
  myDisplay = Standard_True;
  UpdateColorScale();
}

void V3d_ColorScale::Erase()
{
  myDisplay = Standard_False;
  UpdateColorScale();
}

Standard_Boolean V3d_ColorScale::IsDisplayed() const
{
  return myDisplay;
}

void V3d_ColorScale::UpdateColorScale()
{
  myLayerMgr->Compute();
}

void V3d_ColorScale::PaintRect (const Standard_Integer theX, const Standard_Integer theY,
                                const Standard_Integer theWidth, const Standard_Integer theHeight,
                                const Quantity_Color& theColor, const Standard_Boolean theFilled)
{
  const Handle(Visual3d_Layer) &theLayer = myLayerMgr->Overlay();
  if (theLayer.IsNull())
    return;

  theLayer->SetColor (theColor);
  if (theFilled)
    theLayer->DrawRectangle (theX, theY, theWidth, theHeight);
  else {
    theLayer->SetLineAttributes( Aspect_TOL_SOLID, 0.5 );
    theLayer->BeginPolyline();
    theLayer->AddVertex (theX, theY, Standard_False);
    theLayer->AddVertex (theX, theY + theHeight, Standard_True);
    theLayer->AddVertex (theX + theWidth, theY + theHeight, Standard_True);
    theLayer->AddVertex (theX + theWidth, theY, Standard_True);
    theLayer->AddVertex (theX, theY, Standard_True);
    theLayer->ClosePrimitive();
  }
}

void V3d_ColorScale::PaintText (const TCollection_ExtendedString& theText,
                                const Standard_Integer theX, const Standard_Integer theY,
                                const Quantity_Color& theColor)
{
  const Handle(Visual3d_Layer) &theLayer = myLayerMgr->Overlay();
  if (theLayer.IsNull())
    return;

  theLayer->SetColor (theColor);
  theLayer->SetTextAttributes (Font_NOF_ASCII_MONO, Aspect_TODT_NORMAL, theColor);
  TCollection_AsciiString aText (theText.ToExtString(), '?');
  Standard_Integer aTextH = GetTextHeight();
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize (theText, aTextH, aWidth, anAscent, aDescent);
  theLayer->DrawText (aText.ToCString(), theX, theY + anAscent, aTextH);
}

Standard_Integer V3d_ColorScale::TextWidth (const TCollection_ExtendedString& theText) const
{
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize (theText, GetTextHeight(), aWidth, anAscent, aDescent);
  return aWidth;
}

Standard_Integer V3d_ColorScale::TextHeight (const TCollection_ExtendedString& theText) const
{
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize (theText, GetTextHeight(), aWidth, anAscent, aDescent);
  return anAscent+aDescent;
}

void V3d_ColorScale::TextSize (const TCollection_ExtendedString& theText,  const Standard_Integer theHeight, Standard_Integer& theWidth, Standard_Integer& theAscent, Standard_Integer& theDescent) const
{
  const Handle(Visual3d_Layer) &theLayer = myLayerMgr->Overlay();
  if (!theLayer.IsNull()) {
    Standard_Real aWidth, anAscent, aDescent;
    TCollection_AsciiString aText (theText.ToExtString(), '?');
    theLayer->TextSize (aText.ToCString(),theHeight,aWidth,anAscent,aDescent);
    theWidth = (Standard_Integer)aWidth;
    theAscent = (Standard_Integer)anAscent;
    theDescent = (Standard_Integer)aDescent;
  } 
  else {
    theWidth=theAscent=theDescent=0;
  }
}

void V3d_ColorScale::DrawScale ()
{
  const Handle(V3d_View) &theView = myLayerMgr->View();
  if (theView.IsNull())
    return;

  const Handle(Aspect_Window) &theWin = theView->Window();
  if (theWin.IsNull())
    return;

  Standard_Integer WinWidth( 0 ), WinHeight( 0 );
  theWin->Size (WinWidth, WinHeight);

  const Standard_Integer X = RealToInt (GetXPosition() * WinWidth);
  const Standard_Integer Y = RealToInt (GetYPosition() * WinHeight);

  const Standard_Integer W = RealToInt (GetWidth() * WinWidth);
  const Standard_Integer H = RealToInt (GetHeight() * WinHeight);

  Aspect_ColorScale::DrawScale (theView->BackgroundColor(), X, Y, W, H);
}
