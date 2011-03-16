/***********************************************************************

FONCTION :
----------
File OpenGl_highlight :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_highlight.hxx>
#include <OpenGl_Memory.hxx>

struct TEL_HIGHLIGHT_DATA
{
  Tint           hid;
  TEL_HIGHLIGHT  highl;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_HIGHLIGHT_DATA* tel_highlight_data;

struct TEL_WS_HIGHLIGHTS
{
  Tint    num;    /* number of highlights for a workstation */
  Tint    siz;    /* size allocated */
  TEL_HIGHLIGHT_DATA*  data; /* highlight definitions for the workstation */
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_WS_HIGHLIGHTS* tel_ws_highlights;

#define  GROW_SIZE   25

static  tel_ws_highlights    ws_hlights;

static  Tint
b_s( Tint  id, tel_highlight_data tbl, Tint low, Tint high )
{
  register  Tint  mid, i;

  if( low > high )
    return -1;

  mid = low+high, mid /= 2;
  i = id - tbl[mid].hid;

  return i == 0 ? mid : i < 0 ? b_s( id, tbl, low, mid-1 ) :
  b_s( id, tbl, mid+1, high );
}


static  int
lnumcomp( const void* a, const void* b )
{
  return *( Tint* )a - *( Tint* )b;
}


TStatus
TelSetHighlightRep( Tint Wsid, Tint hid, tel_highlight highl )
{
#if 0
  CMN_KEY_DATA    key;
#endif
  tel_highlight_data  hptr;
  register  Tint  i;
#if 0
  TsmGetWSAttri( Wsid, WSHighlights, &key );
  ws_hlights = key.pdata ; /* Obtain list of defined highlights */
#endif
  if( !ws_hlights )  /* no highlights defined yet */
  {                 /* allocate */
    //cmn_memreserve( ws_hlights, 1, 0 );
    ws_hlights = new TEL_WS_HIGHLIGHTS();
    if( !ws_hlights )
      return TFailure;
    //cmn_memreserve( ws_hlights->data, GROW_SIZE, 0 );
    ws_hlights->data = new TEL_HIGHLIGHT_DATA[GROW_SIZE];
    if( !ws_hlights )
      return TFailure;

    ws_hlights->siz = GROW_SIZE;
    ws_hlights->num = 0;
#if 0
    key.pdata = ws_hlights;/* Set list for the workstation */
    TsmSetWSAttri( Wsid, WSHighlights, &key );
#endif
  }
  else if( ws_hlights->num == ws_hlights->siz ) /* insufficient memory */
  {                                           /* realloc */
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
    ws_hlights->data = (TEL_HIGHLIGHT_DATA*)realloc( ws_hlights->data, (ws_hlights->siz
      + GROW_SIZE)*sizeof(TEL_HIGHLIGHT_DATA) );
#else
    ws_hlights->data = cmn_resizemem<TEL_HIGHLIGHT_DATA>( ws_hlights->data, ws_hlights->siz
      + GROW_SIZE );
#endif
    if( !ws_hlights )
      return TFailure;

    ws_hlights->siz += GROW_SIZE;
#if 0
    key.pdata = ws_hlights;             /* Reset list for the workstation */
    TsmSetWSAttri( Wsid, WSHighlights, &key );
#endif
  }
  hptr = 0; /* Locate highlight if already defined */
  i = b_s( hid, ws_hlights->data, 0, ws_hlights->num-1 );
  if( i != -1 )
  {
    hptr = &ws_hlights->data[i]; /* highlight already defined */
    hptr->highl = *highl; /* copy highlight definition */
  }

  if( !hptr ) /* new highlight */
  {
    hptr = &ws_hlights->data[ws_hlights->num];
    hptr->highl = *highl; /* copy highlight definition */
    hptr->hid  = hid;           /* highlights for the workstation */
    ws_hlights->num++;           /* and sort the entries */
    qsort( ws_hlights->data, ws_hlights->num,
      sizeof(TEL_HIGHLIGHT_DATA), lnumcomp );
  }


  return TSuccess;
}


TStatus
TelGetHighlightRep( Tint Wsid, Tint hid, tel_highlight highl )
{
#if 0
  CMN_KEY_DATA    key;
#endif
  tel_highlight_data  hptr;
  register  Tint  i;
#if 0
  TsmGetWSAttri( Wsid, WSHighlights, &key );
  ws_hlights = key.pdata ; /* Obtain list of defined highlights */
#endif
  if( !ws_hlights )
    return TFailure; /* no highlights defined */

  hptr = 0; /* Locate highlight if already defined */
  i = b_s( hid, ws_hlights->data, 0, ws_hlights->num-1 );
  if( i == -1 )
    return TFailure; /* hid undefined */

  hptr = &ws_hlights->data[i];
  *highl = hptr->highl;

  return TSuccess;
}


void
TelPrintHighlightRep( Tint Wsid, Tint hid )
{
#if 0
  CMN_KEY_DATA    key;
#endif
  tel_highlight_data  hptr;
  register  Tint  i;

#if 0
  TsmGetWSAttri( Wsid, WSHighlights, &key );
  ws_hlights = key.pdata ; /* Obtain list of defined highlights */
#endif
  if( !ws_hlights )
    return; /* no highlights defined */

  hptr = 0; /* Locate highlight if already defined */
  i = b_s( hid, ws_hlights->data, 0, ws_hlights->num-1 );
  if( i == -1 )
  {
    fprintf( stdout, "\nBad Highlight %d\n", hid );
    return; /* hid undefined */
  }

  hptr = &ws_hlights->data[i];
  fprintf( stdout, "\nHighlight Id: %d", hid );
  switch( hptr->highl.type )
  {
  case  TelHLColour:
    fprintf( stdout, "\nHighlight type: TelHLColour" );
    fprintf( stdout, "\n\tColour: %f %f %f",
      hptr->highl.col.rgb[0],
      hptr->highl.col.rgb[1],
      hptr->highl.col.rgb[2] );
    break;

  case  TelHLForcedColour:
    fprintf( stdout, "\nHighlight type: TelHLForcedColour" );
    fprintf( stdout, "\n\tColour: %f %f %f",
      hptr->highl.col.rgb[0],
      hptr->highl.col.rgb[1],
      hptr->highl.col.rgb[2] );
    break;
  }
  fprintf( stdout, "\n" );

  return;
}

TStatus
TelDeleteHighlightsForWS( Tint wsid )
{
  CMN_KEY_DATA key;
  tel_ws_highlights h;

  TsmGetWSAttri( wsid, WSHighlights, &key );
  h = (tel_ws_highlights)key.pdata;

  if( !h )
    return TSuccess;

  //cmn_freemem( h );
  if ( h->data )
    delete[] h->data;
  delete h;
  return TSuccess;
}
