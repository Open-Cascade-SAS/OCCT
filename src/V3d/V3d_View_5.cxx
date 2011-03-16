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

#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
			from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
			from higher API */


void V3d_View::EnableDepthTest( const Standard_Boolean enable ) const
{
#ifdef BUC61044
  MyView->EnableDepthTest( enable );
#endif
}

Standard_Boolean V3d_View::IsDepthTestEnabled() const
{
#ifdef BUC61044
  return MyView->IsDepthTestEnabled();
#else
  return Standard_True;
#endif
}

void V3d_View::EnableGLLight( const Standard_Boolean enable ) const
{
#ifdef BUC61045
  MyView->EnableGLLight( enable );
#endif
}

Standard_Boolean V3d_View::IsGLLightEnabled() const
{
#ifdef BUC61045
  return MyView->IsGLLightEnabled();
#else
  return Standard_True;
#endif
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
