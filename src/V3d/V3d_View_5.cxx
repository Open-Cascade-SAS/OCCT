// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
