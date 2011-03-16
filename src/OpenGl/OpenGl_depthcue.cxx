/***********************************************************************

FONCTION :
----------
File OpenGl_depthcue :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
12-02-97 : FMN ; Suppression de TelGetDepthCueRep

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/


#include <OpenGl_tgl_all.hxx>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_depthcue.hxx>
#include <OpenGl_Memory.hxx>

/*----------------------------------------------------------------------*/
/*
* Definition des types
*/

struct TEL_DEPTHCUE_DATA
{
  Tint           dcid;
  TEL_DEPTHCUE   dc;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_DEPTHCUE_DATA* tel_depthcue_data;

struct TEL_WS_DEPTHCUES
{
  Tint    num;    /* number of depthcues for a workstation */
  Tint    siz;    /* size allocated */
  tel_depthcue_data  data; /* depthcue definitions for the workstation */
};
typedef TEL_WS_DEPTHCUES* tel_ws_depthcues;

#define  GROW_SIZE   25

/*----------------------------------------------------------------------*/

static  Tint
b_s( Tint  id, tel_depthcue_data tbl, Tint low, Tint high )
{
  register  Tint  mid, i;

  if( low > high )
    return -1;

  mid = low+high, mid /= 2;
  i = id - tbl[mid].dcid;

  return i == 0 ? mid : i < 0 ? b_s( id, tbl, low, mid-1 ) :
  b_s( id, tbl, mid+1, high );
}

/*----------------------------------------------------------------------*/

static  int
lnumcomp( const void *a, const void *b )
{
  return *(  ( int* )a  )- *(  ( int* )b  );
}

/*----------------------------------------------------------------------*/

TStatus
TelSetDepthCueRep( Tint Wsid, Tint dcid, tel_depthcue dc )
{
  CMN_KEY_DATA    key;
  tel_depthcue_data  dcptr;
  tel_ws_depthcues   ws_dcues;
  register  Tint  i;

  TsmGetWSAttri( Wsid, WSDepthCues, &key );
  ws_dcues = (tel_ws_depthcues)key.pdata ; /* Obtain list of defined depthcues*/

  if( !ws_dcues )  /* no depthcues defined yet */
  {                 /* allocate */
    //cmn_memreserve( ws_dcues, 1, 0 );
    ws_dcues = new TEL_WS_DEPTHCUES();
    if( !ws_dcues )
      return TFailure;

    ws_dcues->data = new TEL_DEPTHCUE_DATA[GROW_SIZE];
    if( !ws_dcues->data )
      return TFailure;

    ws_dcues->siz = GROW_SIZE;
    ws_dcues->num = 0;

    key.pdata = ws_dcues;/* Set list for the workstation */
    TsmSetWSAttri( Wsid, WSDepthCues, &key );
  }
  else if( ws_dcues->num == ws_dcues->siz ) /* insufficient memory */
  {                                           /* realloc */
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
    ws_dcues->data = (TEL_DEPTHCUE_DATA*)realloc( ws_dcues->data, (ws_dcues->siz
      + GROW_SIZE)*sizeof(TEL_DEPTHCUE_DATA) );
#else
    ws_dcues->data = cmn_resizemem<TEL_DEPTHCUE_DATA>( ws_dcues->data, ws_dcues->siz
      + GROW_SIZE );
#endif
    if( !ws_dcues )
      return TFailure;

    ws_dcues->siz += GROW_SIZE;

    key.pdata = ws_dcues;             /* Reset list for the workstation */
    TsmSetWSAttri( Wsid, WSDepthCues, &key );
  }
  dcptr = 0; /* Locate depthcue if already defined */
  i = b_s( dcid, ws_dcues->data, 0, ws_dcues->num-1 );
  if( i != -1 )
    dcptr = &ws_dcues->data[i]; /* depthcue already defined */

  if( !dcptr ) /* new depthcue */
  {
    dcptr = &ws_dcues->data[ws_dcues->num];
    dcptr->dcid  = dcid;           /* depthcues for the workstation */
    ws_dcues->num++;           /* and sort the entries */
    qsort( ws_dcues->data, ws_dcues->num,
      sizeof(TEL_DEPTHCUE_DATA), lnumcomp );
  }

  dcptr->dc = *dc; /* copy depthcue definition */

  return TSuccess;
}

/*----------------------------------------------------------------------*/

TStatus
TelGetGLDepthCue( Tint Wsid, Tint Vid, Tint dcid, tel_gl_depthcue gldc )
{
  CMN_KEY_DATA    key;
  tel_depthcue_data  dcptr;
  tel_ws_depthcues   ws_dcues;
  TEL_VIEW_REP       vrep;
  register  Tint     i;
  Tfloat             ramp;

  TsmGetWSAttri( Wsid, WSDepthCues, &key );
  ws_dcues = (tel_ws_depthcues)key.pdata ; /* Obtain list of defined depthcues */

  if( !ws_dcues )
    return TFailure; /* no depthcues defined */

  dcptr = 0; /* Locate depthcue if already defined */
  i = b_s( dcid, ws_dcues->data, 0, ws_dcues->num-1 );
  if( i == -1 )
    return TFailure; /* dcid undefined */

  if( TelGetViewRepresentation( Wsid, Vid, &vrep ) == TFailure )
    return TFailure; /* Bad View Rep */

  dcptr = &ws_dcues->data[i];
  gldc->dcrep = dcptr->dc;
  if( dcptr->dc.mode == TelDCAllowed )
  {
    ramp = (dcptr->dc.planes[1] - dcptr->dc.planes[0]) /
      (dcptr->dc.scales[1] - dcptr->dc.scales[0]) ;
    gldc->dist[0] = dcptr->dc.planes[0] - (1-dcptr->dc.scales[0]) * ramp;
    gldc->dist[1] = dcptr->dc.planes[1] +    dcptr->dc.scales[1]  * ramp;
    ramp = vrep.extra.map.fpd - vrep.extra.map.bpd;
    gldc->dist[0] = gldc->dist[0]*ramp - vrep.extra.map.fpd;
    gldc->dist[1] = gldc->dist[1]*ramp - vrep.extra.map.fpd;
#ifdef TRACE
    printf( "Dist: Near: %f\tFar: %f\n", gldc->dist[0], gldc->dist[1] );
    printf( "Scal: Near: %f\tFar: %f ramp %f\n",dcptr->dc.scales[0], dcptr->dc.scales[1],ramp );
    printf( "map Near: %f\tFar: %f\n",vrep.extra.map.fpd,vrep.extra.map.bpd);
#endif
  }

  return TSuccess;
}

/*----------------------------------------------------------------------*/

void
TelPrintDepthCueRep( Tint Wsid, Tint dcid )
{
  CMN_KEY_DATA    key;
  tel_depthcue_data  dcptr;
  tel_ws_depthcues   ws_dcues;
  register  Tint  i;

  TsmGetWSAttri( Wsid, WSDepthCues, &key );
  ws_dcues = (tel_ws_depthcues)key.pdata ; /* Obtain list of defined depthcues */

  if( !ws_dcues )
    return; /* no depthcues defined */

  dcptr = 0; /* Locate depthcue if already defined */
  i = b_s( dcid, ws_dcues->data, 0, ws_dcues->num-1 );
  if( i == -1 )
  {
    fprintf( stdout, "\nBad DepthCue %d\n", dcid );
    return; /* dcid undefined */
  }

  dcptr = &ws_dcues->data[i];
  fprintf( stdout, "\nDepthCue Id: %d", dcid );
  fprintf( stdout, "\nDepthCue mode: %s", dcptr->dc.mode == TelDCAllowed ?
    "TelDCAllowed" : "TelDCSuppressed" );
  fprintf( stdout, "\n\tPlanes: Near %f\tFar %f",
    dcptr->dc.planes[0], dcptr->dc.planes[1] );
  fprintf( stdout, "\n\tScales: Near %f\tFar %f",
    dcptr->dc.scales[0], dcptr->dc.scales[1] );
  fprintf( stdout, "\n\tColour: %f %f %f",
    dcptr->dc.col.rgb[0],
    dcptr->dc.col.rgb[1],
    dcptr->dc.col.rgb[2] );
  fprintf( stdout, "\n" );

  return;
}

/*----------------------------------------------------------------------*/

TStatus
TelDeleteDepthCuesForWS( Tint wsid )
{
  CMN_KEY_DATA key;
  tel_ws_depthcues d;

  TsmGetWSAttri( wsid, WSDepthCues, &key );
  d = (tel_ws_depthcues)key.pdata;

  if( !d )
    return TSuccess;

  //cmn_freemem( d );
  delete d;

  return TSuccess;
}

/*----------------------------------------------------------------------*/
