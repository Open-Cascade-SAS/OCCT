#define GER61454        //GG 14-09-99 Activates the model clipping planes

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl.hxx>
#include <GL/gl.h>

void EXPORT
/* unimplemented */
call_togl_setplane
(
 CALL_DEF_VIEW * aview
 )
{
#ifdef GER61454
  CALL_DEF_PLANE *plane;
  int j,planeid ;

  if( aview->Context.NbActivePlane > 0 ) {
    for( j=0 ; j<aview->Context.NbActivePlane ; j++ ) {
      plane = &aview->Context.ActivePlane[j];
      if( plane->PlaneId > 0 ) {
        planeid = GL_CLIP_PLANE2 + j;
        if( plane->Active ) {
          GLdouble equation[4];
          equation[0] = plane->CoefA;      
          equation[1] = plane->CoefB;      
          equation[2] = plane->CoefC;      
          equation[3] = plane->CoefD;
          /*
          Activates new clip planes
          */ 
          glClipPlane( planeid , equation );     
          if( !glIsEnabled( planeid ) ) glEnable( planeid );     
        } else {
          if( glIsEnabled( planeid ) ) glDisable( planeid );     
        }
      }
    }
  }
  /*
  Disable the remainder Clip planes
  */
  for( j=aview->Context.NbActivePlane ; j<call_facilities_list.MaxPlanes ; j++ ) {
    planeid = GL_CLIP_PLANE2 + j;
    if( glIsEnabled( planeid ) ) glDisable( planeid );
  }
#endif
  return;
}
