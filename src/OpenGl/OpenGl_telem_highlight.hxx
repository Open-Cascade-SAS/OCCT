#ifndef  OPENGL_TELEM_HIGHLIGHT_H
#define  OPENGL_TELEM_HIGHLIGHT_H

#include <OpenGl_telem.hxx>

typedef  struct
{
  THighlightType    type;
  TEL_COLOUR   col;
} TEL_HIGHLIGHT, *tel_highlight;

/* currently, the routines ignore wsid */

/* ws, hid, highlighrep */
extern  TStatus  TelSetHighlightRep( Tint, Tint, tel_highlight );
/* ws, hid, highlighrep */
extern  TStatus  TelGetHighlightRep( Tint, Tint, tel_highlight );

extern  void     TelPrintHighlightRep( Tint, Tint ); /* ws, hid */
extern  TStatus  TelDeleteHighlightsForWS( Tint wsid );

#endif
