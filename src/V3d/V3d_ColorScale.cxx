// Created on: 2004-06-22
// Created by: STV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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


#include <V3d_ColorScale.ixx>

#include <V3d_View.hxx>
#include <V3d_LayerMgr.hxx>

#include <Visual3d_View.hxx>
#include <Visual3d_Layer.hxx>
#include <Visual3d_ViewManager.hxx>

#include <Aspect_Window.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_SequenceOfColor.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Font_NameOfFont.hxx>

V3d_ColorScale::V3d_ColorScale( const Handle(V3d_LayerMgr)& aMgr )
: Aspect_ColorScale(),
myLayerMgr( aMgr.operator->() ),
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

void V3d_ColorScale::PaintRect( const Standard_Integer X, const Standard_Integer Y,
                                const Standard_Integer W, const Standard_Integer H,
                                const Quantity_Color& aColor, const Standard_Boolean aFilled )
{
  const Handle(Visual3d_Layer) &theLayer = myLayerMgr->Overlay();
  if ( theLayer.IsNull() )
    return;

  theLayer->SetColor( aColor );
  if ( aFilled )
    theLayer->DrawRectangle( X, Y, W, H );
  else {
    theLayer->SetLineAttributes( Aspect_TOL_SOLID, 0.5 );
    theLayer->BeginPolyline();
    theLayer->AddVertex( X, Y, Standard_False );
    theLayer->AddVertex( X, Y + H, Standard_True );
    theLayer->AddVertex( X + W, Y + H, Standard_True );
    theLayer->AddVertex( X + W, Y, Standard_True );
    theLayer->AddVertex( X, Y, Standard_True );
    theLayer->ClosePrimitive();
  }
}

void V3d_ColorScale::PaintText( const TCollection_ExtendedString& aText,
                                const Standard_Integer X, const Standard_Integer Y,
                                const Quantity_Color& aColor )
{
  const Handle(Visual3d_Layer) &theLayer = myLayerMgr->Overlay();
  if ( theLayer.IsNull() )
    return;

  theLayer->SetColor( aColor );
  theLayer->SetTextAttributes( Font_NOF_ASCII_MONO, Aspect_TODT_NORMAL, aColor );
  TCollection_AsciiString theText( aText.ToExtString(), '?' );
  Standard_Integer aTextH = GetTextHeight();
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize(aText, aTextH, aWidth, anAscent, aDescent);
  theLayer->DrawText( theText.ToCString(), X, Y + anAscent, aTextH);
}

Standard_Integer V3d_ColorScale::TextWidth( const TCollection_ExtendedString& aText ) const
{
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize(aText, GetTextHeight(), aWidth, anAscent, aDescent);
  return aWidth;
}

Standard_Integer V3d_ColorScale::TextHeight( const TCollection_ExtendedString& aText ) const
{
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize(aText, GetTextHeight(), aWidth, anAscent, aDescent);
  return anAscent+aDescent;
}

void V3d_ColorScale::TextSize (const TCollection_ExtendedString& AText,  const Standard_Integer AHeight, Standard_Integer& AWidth, Standard_Integer& AnAscent, Standard_Integer& ADescent) const 
{
  const Handle(Visual3d_Layer) &theLayer = myLayerMgr->Overlay();
  if ( !theLayer.IsNull() ) {
    Standard_Real aWidth, anAscent, aDescent;
    TCollection_AsciiString theText( AText.ToExtString(), '?' );
    theLayer->TextSize(theText.ToCString(),AHeight,aWidth,anAscent,aDescent);
    AWidth = (Standard_Integer)aWidth;
    AnAscent = (Standard_Integer)anAscent;
    ADescent = (Standard_Integer)aDescent;
  } 
  else {
    AWidth=AnAscent=ADescent=0;
  }
}

void V3d_ColorScale::DrawScale ()
{
  const Handle(V3d_View) &theView = myLayerMgr->View();
  if ( theView.IsNull() )
    return;

  const Handle(Aspect_Window) &theWin = theView->Window();
  if ( theWin.IsNull() )
    return;

  Standard_Integer WinWidth( 0 ), WinHeight( 0 );
  theWin->Size( WinWidth, WinHeight );

  const Standard_Integer X = RealToInt(GetXPosition() * WinWidth);
  const Standard_Integer Y = RealToInt(GetYPosition() * WinHeight);

  const Standard_Integer W = RealToInt(GetWidth() * WinWidth);
  const Standard_Integer H = RealToInt(GetHeight() * WinHeight);

  Aspect_ColorScale::DrawScale( theView->BackgroundColor(), X, Y, W, H );
}
