/***********************************************************************

FONCTION :
----------
file OpenGl_tgl_utilgr :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
29-04-96 : FMN ; Correction Warning.
30-01-97 : FMN ; Suppression call_util_init_indexes()
03-03-98 : CAL ; Modification des structures CALL_DEF_GROUP et STRUCTURE

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/


#include <OpenGl_tgl_all.hxx>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_context.hxx>

#include <OpenGl_tgl_utilgr.hxx>
#include <OpenGl_Memory.hxx>

Tint
call_util_context_group_place( CALL_DEF_GROUP * agroup )
{
  Tint  n;

  n = 0;
  if( agroup->ContextLine.IsSet )
    n = n + CALL_DEF_CONTEXTLINE_SIZE;
  if( agroup->ContextFillArea.IsSet )
    n = n + CALL_DEF_CONTEXTFILLAREA_SIZE;
  if( agroup->ContextMarker.IsSet )
    n = n + CALL_DEF_CONTEXTMARKER_SIZE;
  if( agroup->ContextText.IsSet )
    n = n + CALL_DEF_CONTEXTTEXT_SIZE;

  return n;
}


Tint
call_util_osd_putenv( Tchar *symbol, Tchar *value )
{
#ifndef WNT
  Tchar   *s;
  Tint    l;

  l = strlen((char*)symbol) + strlen((char*)value) + 2;
  s = new Tchar[l];;
  if( s )
  {
    sprintf( (char*)s,"%s=%s", symbol, value );
    if( !putenv((char*)s) )
      return 1;
  }
  return 0;
#else
  return SetEnvironmentVariable ( (LPCSTR)symbol, (LPCSTR)value );
#endif  /* WNT */
}

Tint
call_util_rgbindex( Tfloat r, Tfloat g, Tfloat b )
{
  union
  {
    Tint      colpack;
    unsigned  char  rgb[3];
  } rgb2ind;

  rgb2ind.colpack = 0;
  rgb2ind.rgb[0] = ( unsigned char )(r*100);
  rgb2ind.rgb[1] = ( unsigned char )(g*100);
  rgb2ind.rgb[2] = ( unsigned char )(b*100);

  return rgb2ind.colpack;
}
