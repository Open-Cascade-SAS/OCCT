#define GER61454        //GG 14-09-99 Activates the model clipping planes

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <GL/gl.h>


struct TEL_VIEW_DATA
{
  TEL_VIEW_REP    vrep;
#ifdef CAL_100498
  Tmatrix3        inverse_matrix;/* accelerates UVN2XYZ conversion */
#endif
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_VIEW_DATA *tel_view_data;   /* Internal data stored for every view rep */


void EXPORT
/* unimplemented */
call_togl_setplane
(
 CALL_DEF_VIEW * aview
 )
{
#ifdef GER61454

  CMN_KEY_DATA    key;
  tel_view_data   vptr;
  TEL_VIEW_REP   *call_viewrep;
  CALL_DEF_PLANE *plane;
  int j;
  
  // return view representation 
  
  if( aview->ViewId == 0 )
    return;                 // no modifications on default view

  if( TsmGetWSAttri( aview->WsId, WSViews, &key ) != TSuccess )
    return;

  vptr = (tel_view_data)key.pdata;
  if( !vptr )
    return;

  call_viewrep = &vptr->vrep;
  
  // clear clipping planes information
  call_viewrep->clipping_planes.Clear();
  // update information 
  if( aview->Context.NbActivePlane > 0 )
    for( j=0 ; j<aview->Context.NbActivePlane ; j++ )
      call_viewrep->clipping_planes.Append( aview->Context.ActivePlane[j] );
 
#endif
return;
}
