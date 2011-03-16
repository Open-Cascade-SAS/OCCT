#define GER61454  /*GG 14-09-99 Activates the model clipping planes
// GG 110800  UNDER LINUX and MESA 3.2, nothing can be done until
//    gl context is open first.
*/
#ifdef DEBUG
#include <stdio.h>
#endif

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl.hxx>

int EXPORT
call_togl_inquireplane ()
{
  GLint maxplanes = 0;
  if( GET_GL_CONTEXT() ) {
#ifdef GER61454
    glGetIntegerv( GL_MAX_CLIP_PLANES, &maxplanes);
    maxplanes -= 2;   /* NOTE the 2 first planes are reserved for ZClipping */
    if( maxplanes < 0 ) 
      maxplanes = 0;
    call_facilities_list.MaxPlanes = maxplanes;
#endif
  }
#ifdef DEBUG
  printf(" @@@ call_togl_inquireplane. GL_MAX_CLIP_PLANES is %d\n",maxplanes);
#endif
  return maxplanes;
}
