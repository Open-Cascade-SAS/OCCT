// Created on: 2008-04-17
// Created by: Customer Support
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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


#include <V3d_LayerMgr.ixx>

#include <Aspect_Window.hxx>
#include <Visual3d_View.hxx>
#include <V3d_ColorScale.hxx>
#include <Font_NameOfFont.hxx>


V3d_LayerMgr::V3d_LayerMgr( const Handle(V3d_View)& AView )
: myView(AView.operator->())
{
  Handle(Visual3d_View) theView = View()->View();
  if ( !theView.IsNull() ) {
    Handle(Visual3d_ViewManager) theViewMgr = theView->ViewManager();
    if ( !theViewMgr.IsNull() ) {
      V3d_LayerMgr* that = (V3d_LayerMgr*)this;
      that->myOverlay = new Visual3d_Layer( theViewMgr, Aspect_TOL_OVERLAY, Standard_False );
    }
  }
}

void V3d_LayerMgr::Compute()
{
  if (Begin())
  {
    Redraw();
    End();
  }
}

void V3d_LayerMgr::Resized()
{
  Compute();
}

void V3d_LayerMgr::ColorScaleDisplay()
{
  ColorScale();
  myColorScale->Display();
  myOverlay->AddLayerItem( myColorScaleLayerItem );
}

void V3d_LayerMgr::ColorScaleErase()
{
  if ( !myColorScale.IsNull() )
    myColorScale->Erase();
  myOverlay->RemoveLayerItem( myColorScaleLayerItem );
}

Standard_Boolean V3d_LayerMgr::ColorScaleIsDisplayed() const
{
  return ( myColorScale.IsNull() ? Standard_False : myColorScale->IsDisplayed() );
}

Handle(Aspect_ColorScale) V3d_LayerMgr::ColorScale() const
{
  if ( myColorScale.IsNull() ) {
    Handle(V3d_LayerMgr) that = this;
    that->myColorScale = new V3d_ColorScale( this );
    that->myColorScaleLayerItem = new V3d_ColorScaleLayerItem( that->myColorScale );
  }

  return myColorScale;
}

Standard_Boolean V3d_LayerMgr::Begin()
{
  if ( myOverlay.IsNull() )
    return Standard_False;

  const Handle(Aspect_Window) &theWin = View()->Window();
  if ( theWin.IsNull() )
    return Standard_False;

  Standard_Integer aW( 0 ), aH( 0 );
  theWin->Size( aW, aH );

  myOverlay->Clear();
  myOverlay->SetViewport( aW, aH ); //szv:!!!
  myOverlay->Begin();
  myOverlay->SetTextAttributes( Font_NOF_ASCII_MONO, Aspect_TODT_NORMAL, Quantity_Color() );
  myOverlay->SetOrtho( 0, Max( aW, aH ), Max( aW, aH ), 0, Aspect_TOC_TOP_LEFT );

  return Standard_True;
}

void V3d_LayerMgr::Redraw()
{

}

void V3d_LayerMgr::End()
{
  if ( !myOverlay.IsNull() )
    myOverlay->End();
}
