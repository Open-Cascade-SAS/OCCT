// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_View :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      22-10-01 : SAV ; Created

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Includes
 */
#include <V3d_View.jxx>

//BUC61044 25/10/01 SAV ; added functionality to control gl depth testing from higher API
//BUC61045 25/10/01 SAV ; added functionality to control gl lighting from higher API


void V3d_View::EnableDepthTest( const Standard_Boolean enable ) const
{
  MyView->EnableDepthTest( enable );
}

Standard_Boolean V3d_View::IsDepthTestEnabled() const
{
  return MyView->IsDepthTestEnabled();
}

void V3d_View::EnableGLLight( const Standard_Boolean enable ) const
{
  MyView->EnableGLLight( enable );
}

Standard_Boolean V3d_View::IsGLLightEnabled() const
{
  return MyView->IsGLLightEnabled();
}

void V3d_View::SetRaytracingMode()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsRaytracing = 1;
}

void V3d_View::SetRasterizationMode()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsRaytracing = 0;
}

void V3d_View::EnableRaytracedShadows()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsShadowsEnabled = 1;
}

void V3d_View::EnableRaytracedReflections()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsReflectionsEnabled = 1;
}

void V3d_View::EnableRaytracedAntialiasing()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsAntialiasingEnabled = 1;
}

void V3d_View::DisableRaytracedShadows()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsShadowsEnabled = 0;
}

void V3d_View::DisableRaytracedReflections()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsReflectionsEnabled = 0;
}

void V3d_View::DisableRaytracedAntialiasing()
{
  Graphic3d_CView* cView = (Graphic3d_CView* )MyView->CView();

  cView->IsAntialiasingEnabled = 0;
}

void V3d_View::SetLayerMgr(const Handle(V3d_LayerMgr)& aMgr)
{
  MyLayerMgr = aMgr;
}

void V3d_View::ColorScaleDisplay()
{
  if ( MyLayerMgr.IsNull() )
    MyLayerMgr = new V3d_LayerMgr( this );

  MyLayerMgr->ColorScaleDisplay();
  MustBeResized();
  if ( !Window().IsNull() ) {
    Standard_Integer aW( 0 ), aH( 0 );
    Window()->Size( aW, aH );
    Redraw( 0, 0, aW, aH );
  }
}

void V3d_View::ColorScaleErase()
{
  if ( !MyLayerMgr.IsNull() )
    MyLayerMgr->ColorScaleErase();
}

Standard_Boolean V3d_View::ColorScaleIsDisplayed() const
{
  Standard_Boolean aStatus = Standard_False;
  if ( !MyLayerMgr.IsNull() )
    aStatus = MyLayerMgr->ColorScaleIsDisplayed();

  return aStatus;
}

Handle(Aspect_ColorScale) V3d_View::ColorScale() const
{
  if ( MyLayerMgr.IsNull() ) {
    Handle(V3d_View) that = this;
    that->MyLayerMgr = new V3d_LayerMgr( this );
  }

  return MyLayerMgr->ColorScale();
}
