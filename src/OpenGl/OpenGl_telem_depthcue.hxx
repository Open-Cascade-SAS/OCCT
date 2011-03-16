/***********************************************************************

FONCTION :
----------
File OpenGl_telem_depthcue.h :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
12-02-97 : FMN ; Suppression de TelGetDepthCueRep

************************************************************************/

#ifndef  OPENGL_TELEM_DEPTHCUE_H
#define  OPENGL_TELEM_DEPTHCUE_H

#include <OpenGl_telem.hxx>

struct TEL_DEPTHCUE
{
  TDepthCueType  mode;
  Tfloat         planes[2]; /* in the range 0.0 & 1.0 */
  Tfloat         scales[2]; /* in the range 0.0 & 1.0. This is fraction
                               of object colour to be visible */
  TEL_COLOUR     col;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_DEPTHCUE* tel_depthcue;

struct TEL_GL_DEPTHCUE
{
  TEL_DEPTHCUE    dcrep;
  Tfloat          dist[2];
};
typedef TEL_GL_DEPTHCUE* tel_gl_depthcue;

/* ws, dcid, dcrep */
extern  TStatus  TelSetDepthCueRep( Tint, Tint, tel_depthcue );
/* ws, dcid, dcrep */

extern  void     TelPrintDepthCueRep( Tint, Tint ); /* ws, hid */
/* ws,  viewid, dcid, data */
extern  TStatus  TelGetGLDepthCue( Tint, Tint, Tint, tel_gl_depthcue );
extern  TStatus  TelDeleteDepthCuesForWS( Tint wsid );

#endif
