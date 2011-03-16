/***********************************************************************

FONCTION :
----------
File OpenGl_pick :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
25-06-96 : FMN ; Suppression utilisation de glScissor.
03-07-96 : FMN ; Suppression TelGetViewportAtLocation.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stdio.h>

#include <GL/gl.h>

#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_pick.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_Memory.hxx>


/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

#define ARRAY_GROW_SIZE 10


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/ 

static  GLuint *tgl_pick_buffer;  /* buffer to be given to GL */
static  Tint   tgl_pick_bufsize;


/*----------------------------------------------------------------------*/

static TStatus
allocate( Tint *cur_size, Tint size_reqd, void **arr, Tint elem_size )
{
  size_reqd = ( ( size_reqd / ARRAY_GROW_SIZE ) + 1 ) * ARRAY_GROW_SIZE;

  if( !( *cur_size ) ) {
    *arr = malloc( size_reqd * elem_size );
    memset( *arr, 0, size_reqd * elem_size );
  }
  else
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)    
    *arr = realloc( *arr, size_reqd * elem_size );
#else
    *arr = (void*)cmn_resizemem<char>( (char*)*arr, size_reqd * elem_size );
#endif

  if( !(*arr) )
    return TFailure;

  *cur_size = size_reqd;
  return TSuccess;
}

static TStatus
Initialize( Tint wsid, Tint *view_stid )
{
  Tint         depth;
  CMN_KEY_DATA key;
  Tint         buf_siz_reqd = 0;

  TsmGetWSAttri( wsid, WSViewStid, &key );
  if( key.ldata == -1 )
    return TFailure;

  *view_stid = key.ldata;
  TsmGetStructureDepth( *view_stid, &depth );

  buf_siz_reqd = depth*6 + 2;

  if( tgl_pick_bufsize < buf_siz_reqd )
  {
    if( allocate( &tgl_pick_bufsize, buf_siz_reqd, (void **)&tgl_pick_buffer,
      sizeof( int ) ) == TFailure )
      return TFailure;
  }

  return TSuccess;
}
/*----------------------------------------------------------------------*/

static void
fill_pick_report( tel_pick_report rep, TPickOrder order, Tint depth )
{
  Tint  i, j;
  TEL_PACKED_NAME pn;

  rep->depth = tgl_pick_buffer[0]/6;

  if( order == TTopFirst )
  {
    for( i = 0, j = 3; i < ( Tint )(tgl_pick_buffer[0]/6) && i < depth; i++ )
    {         /* skip hit numb. zmin, zmax -> j = 3 */
      pn.s[0] = tgl_pick_buffer[j++];
      pn.s[1] = tgl_pick_buffer[j++];
      rep->pick_path[i].struct_id = pn.i;
      pn.s[0] = tgl_pick_buffer[j++];
      pn.s[1] = tgl_pick_buffer[j++];
      rep->pick_path[i].pick_id = pn.i;
      pn.s[0] = tgl_pick_buffer[j++];
      pn.s[1] = tgl_pick_buffer[j++];
      rep->pick_path[i].el_num = pn.i;
    }
  }
  else
  {
    Tint is, js;

    is = rep->depth < depth ? rep->depth : depth;
    js = 3 + ( rep->depth - is )*6;  /* skip hit numb. zmin, zmax -> j = 3 */
    is--;

    for( i = is, j = js; i >= 0; i-- )
    {
      pn.s[0] = tgl_pick_buffer[j++];
      pn.s[1] = tgl_pick_buffer[j++];
      rep->pick_path[i].struct_id = pn.i;
      pn.s[0] = tgl_pick_buffer[j++];
      pn.s[1] = tgl_pick_buffer[j++];
      rep->pick_path[i].pick_id = pn.i;
      pn.s[0] = tgl_pick_buffer[j++];
      pn.s[1] = tgl_pick_buffer[j++];
      rep->pick_path[i].el_num = pn.i;
    }
  }

  return;
}
/*----------------------------------------------------------------------*/

Tint TglVpBeingPicked = -1;

TStatus
TPick( Tint Wsid, Tint x, Tint y, Tfloat apw, Tfloat aph,
      TPickOrder order, Tint depth, tel_pick_report rep )
{
  Tint           vid;
  TSM_ELEM_DATA  data;
  CMN_KEY        key;
  CMN_KEY_DATA   key1;
  TEL_VIEW_REP   ovrep, vrep;

  if( Initialize( Wsid, &(data.ldata) ) == TFailure )
    return TFailure;

  TglActiveWs = Wsid;
  key.id      = Wsid;

  {
    /* TStatus  stat; */
    Tfloat   xsf, ysf, x1, x2, y1, y2, W, H, xm, xp, ym, yp, cx, cy;
    Tint              err;
    TEL_VIEW_MAPPING  map;

    vid  = Wsid;
    TglVpBeingPicked = vid;
    TelGetViewRepresentation( Wsid, vid, &vrep );
    ovrep = vrep;

    TsmGetWSAttri( Wsid, WSWidth, &key1 );
    W = ( float )key1.ldata;
    TsmGetWSAttri( Wsid, WSHeight, &key1 );
    H = ( float )key1.ldata;

    xm = vrep.extra.map.window.xmax, xp = vrep.extra.map.window.xmin;
    ym = vrep.extra.map.window.ymax, yp = vrep.extra.map.window.ymin;
    cx = xm + xp, cx /= 2;
    cy = ym + yp, cy /= 2;
    x1 = x - apw/2, x2 = x1 + apw, y1 = y - aph/2, y2 = y1 + aph;
    xsf = xm - xp, xsf /= W;
    ysf = ym - yp, ysf /= H;
    x1 = xsf * x1 + xp;
    y1 = ysf * y1 + yp;
    x2 = xsf * x2 + xp;
    y2 = ysf * y2 + yp;

    map = vrep.extra.map;
    map.window.xmin = x1, map.window.xmax = x2;
    map.window.ymin = y1, map.window.ymax = y2;
    TelEvalViewMappingMatrixPick( &map, &err, vrep.mapping_matrix, cx, cy );
    if( err )
      printf( "Error in Mapping pick\n" );

    if( TelSetViewRepresentation( Wsid, vid, &vrep ) == TFailure )
      printf( "Error in Set vrep for pick\n" );
  }

  glMatrixMode(GL_MODELVIEW);
  tgl_pick_buffer[0] = 0;
  glSelectBuffer( tgl_pick_bufsize ,  tgl_pick_buffer);
  glRenderMode(GL_SELECT);
  glLoadName(55);
  TsmSendMessage( TelExecuteStructure, PickTraverse, data, 1, &key );
  glRenderMode(GL_RENDER);
  TglVpBeingPicked = -1;

  /* print( ); */
  fill_pick_report( rep, order, depth );
  TelSetViewRepresentation( Wsid, vid, &ovrep );

  return TSuccess;
}
/*----------------------------------------------------------------------*/
