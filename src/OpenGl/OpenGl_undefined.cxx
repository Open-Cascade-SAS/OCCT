/***********************************************************************

FONCTION :
----------
file OpenGl_undefined :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
01-04-96 : CAL ; Integration MINSK portage WNT
22-04-96 : FMN ; Suppression prototype inutile.
25-06-96 : FMN ; Suppression utilisation de glScissor.
02-07-96 : FMN ; Modification parametre glViewport.
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
04-02-97 : FMN ; Suppression call_togl_ps, call_togl_xwd ...
02-04-97 : FMN ; Correction call_tox_rect,  on repasse en GL_BACK a la fin.call_tox_rect
05-01-98 : FMN ; Suppression WNT inutile
Suppression call_tox_set_colormap_mapping()

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <string.h>
#include <stdio.h>

#include <OpenGl_tgl_all.hxx>

#include <GL/gl.h>
#ifndef WNT
# include <GL/glx.h>
#else
# include <InterfaceGraphic_WNT.hxx>
#endif  /* WNT */

#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_inquire.hxx>
#include <OpenGl_tgl_elems.hxx>
#include <OpenGl_tgl_funcs.hxx>

/*static  GLboolean         lighting_mode;*/
extern DISPLAY *call_thedisplay;
#ifndef WNT
/*static GC  gcx;*/
#endif  /* WNT */
/*static int ft=1;*/

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define XTRACE

#define CALL_DEF_BUFFER_SIZE 1024
#define CALL_DEF_STRING_LENGTH 132

/*----------------------------------------------------------------------*/
EXPORT void call_togl_build_matrix (
                                    CALL_DEF_VIEW * aview,
                                    CALL_DEF_MATRIX4X4 gl_ori_matrix, CALL_DEF_MATRIX4X4 gl_map_matrix,
                                    CALL_DEF_MATRIX4X4 eval_ori_matrix, CALL_DEF_MATRIX4X4 eval_map_matrix )
{
  int i, j;
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      if (i == j) {
        gl_ori_matrix[i][j] = ( float )1.0;
        gl_map_matrix[i][j] = ( float )1.0;
        eval_ori_matrix[i][j] = ( float )1.0;
        eval_map_matrix[i][j] = ( float )1.0;
      }
      else {
        gl_ori_matrix[i][j] = ( float )0.0;
        gl_map_matrix[i][j] = ( float )0.0;
        eval_ori_matrix[i][j] = ( float )0.0;
        eval_map_matrix[i][j] = ( float )0.0;
      }
}

/*----------------------------------------------------------------------*/
EXPORT void call_togl_element_type (
                                    long Id,
                                    long elem_num,
                                    long *elem_type )
{
#ifdef PHIGS_VERSION
  Pint err_ind;   /* OUT error indicator */
  Pint size;    /* OUT element size */
  Pint type;    /* OUT element type */
  Pint cur_index;   /* OUT element pointer value */

  type   = 0;
  call_func_open_struct (Id);
  call_func_set_elem_ptr (elem_num);
  call_func_inq_elem_ptr (&err_ind, &cur_index);
  /* it is not the end */
  if ((elem_num==cur_index) && (! err_ind)) {
    call_func_inq_cur_elem_type_size
      (&err_ind, &type, &size);
    if (err_ind) type = 0;
  }
  call_func_close_struct ();

  switch (type)
  {
  case CALL_PHIGS_ELEM_POLYLINE3 :
  case CALL_PHIGS_ELEM_POLYLINE :
  case CALL_PHIGS_ELEM_POLYLINE_SET3_DATA :
    *elem_type  = 1;
    break;

  case CALL_PHIGS_ELEM_FILL_AREA3 :
  case CALL_PHIGS_ELEM_FILL_AREA :
  case CALL_PHIGS_ELEM_FILL_AREA_SET3 :
  case CALL_PHIGS_ELEM_FILL_AREA_SET :
  case CALL_PHIGS_ELEM_FILL_AREA_SET3_DATA :
  case CALL_PHIGS_ELEM_SET_OF_FILL_AREA_SET3_DATA :
    *elem_type  = 2;
    break;

  case CALL_PHIGS_ELEM_TRI_STRIP3_DATA :
    *elem_type  = 3;
    break;

  case CALL_PHIGS_ELEM_QUAD_MESH3_DATA :
    *elem_type  = 4;
    break;

  case CALL_PHIGS_ELEM_TEXT3 :
  case CALL_PHIGS_ELEM_TEXT :
  case CALL_PHIGS_ELEM_ANNO_TEXT_REL3 :
  case CALL_PHIGS_ELEM_ANNO_TEXT_REL :
    *elem_type  = 5;
    break;

  case CALL_PHIGS_ELEM_POLYMARKER3 :
  case CALL_PHIGS_ELEM_POLYMARKER :
    *elem_type  = 6;
    break;

  default :
    *elem_type  = 0;
    break;
  }
#endif
  TelType type;
  Tint    cur_index;
  Tint    err_ind=0;
  Tint    size;

  type = TelNil;

  call_func_open_struct (Id);
  call_func_set_elem_ptr (elem_num);
  if( TsmGetCurElemPtr( &cur_index ) == TFailure )
    err_ind = 1;
  /* it is not the end */
  if ((elem_num==cur_index) && (! err_ind))
    TelInqCurElemTypeSize( &type, &size );
  call_func_close_struct ();

  switch (type)
  {
  case CALL_PHIGS_ELEM_POLYLINE_SET3_DATA :
    *elem_type  = 1;
    break;

  case CALL_PHIGS_ELEM_FILL_AREA3 :
  case CALL_PHIGS_ELEM_FILL_AREA_SET3 :
    *elem_type  = 2;
    break;

  case CALL_PHIGS_ELEM_TRI_STRIP3_DATA :
    *elem_type  = 3;
    break;

  case CALL_PHIGS_ELEM_QUAD_MESH3_DATA :
    *elem_type  = 4;
    break;

  case CALL_PHIGS_ELEM_ANNO_TEXT_REL3 :

    *elem_type  = 5;
    break;

  case CALL_PHIGS_ELEM_POLYMARKER3 :
    *elem_type  = 6;
    break;

  default :
    *elem_type  = 0;
    break;
  }
  *elem_type  = 0;
}

/*----------------------------------------------------------------------*/
EXPORT int call_togl_inquiremat (
                                 CALL_DEF_VIEW * aview,
                                 CALL_DEF_MATRIX4X4 ori_matrix,
                                 CALL_DEF_MATRIX4X4 map_matrix )
{
  TEL_VIEW_REP rep;
  int          i, j, error;
  view_map3    map;

  TelGetViewRepresentation( aview->WsId, aview->ViewId, &rep );
  if(aview->Orientation.IsCustomMatrix) {
    /* OCC18942: Trying to return the current matrices instead of calculating them anew.
    This in particular allows using application-defined matrices. */
  } else {
    memcpy( &map.win, &rep.extra.map.window, sizeof( Tlimit ) );
    memcpy( &map.proj_vp, &rep.extra.map.viewport, sizeof( Tlimit3 ) );
    switch(rep.extra.map.proj){
      default:
      case TelParallel :    map.proj_type = TYPE_PARAL;    break;
      case TelPerspective : map.proj_type = TYPE_PERSPECT; break;
    }
    map.proj_ref_point.x = rep.extra.map.prp[0];
    map.proj_ref_point.y = rep.extra.map.prp[1];
    map.proj_ref_point.z = rep.extra.map.prp[2];
    map.view_plane  = rep.extra.map.vpd;
    map.back_plane  = rep.extra.map.bpd;
    map.front_plane = rep.extra.map.fpd;

    call_func_eval_ori_matrix3( (point3 *)rep.extra.vrp,
      (vec3 *)rep.extra.vpn,
      (vec3 *)rep.extra.vup,
      &error, rep.orientation_matrix );
    if( error )
      return 1;

    call_func_eval_map_matrix3( &map, &error, rep.mapping_matrix );

    if( error )
      return 1;
  }
  for( i = 0; i < 4; i++ )
    for( j = 0; j < 4; j++ )
    {
      ori_matrix[i][j] = rep.orientation_matrix[j][i];
      map_matrix[i][j] = rep.mapping_matrix[j][i];
    }

    return 0;
}

/*----------------------------------------------------------------------*/
int call_tox_window_exists ( WINDOW w )
{
  return (1);
}

/*----------------------------------------------------------------------*/
extern void
call_tox_rect( int wsid, WINDOW win, int u, int v )
{
#define STEP 2.0
  Tint        w, h;
  CMN_KEY_DATA k;
  float        p[3];
  float        r[3] = { ( float )0.0, ( float )1.0, ( float )1.0 };

  TsmGetWSAttri( wsid, WSWidth, &k );
  w = k.ldata;
  TsmGetWSAttri( wsid, WSHeight, &k );
  h = k.ldata;

  TxglWinset( call_thedisplay, win );
  glViewport( 0, 0, w, h);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0.0, ( GLdouble ) w, 0.0, ( GLdouble ) h, -100000.0, 100000.0 );

  p[0] = ( float )(u-STEP);
  p[1] = ( float )(v-STEP);
  p[2] = ( float )(-9999.0);
  glDrawBuffer(GL_FRONT) ;
  glColor3fv( r );
  glBegin(GL_QUADS);
  glVertex3fv( p );
  p[0] = ( float )(u+STEP);
  glVertex3fv( p );
  p[1] = ( float )(v+STEP);
  glVertex3fv( p );
  p[0] = ( float )(u-STEP);
  glVertex3fv( p );
  glEnd();

  TelFlush(0);

  glDrawBuffer(GL_BACK) ;
}

/*----------------------------------------------------------------------*/

#if !defined(GLU_VERSION_1_2)
void gluTessBeginContour() {
  printf(" UNIMPLEMENTED function gluTessBeginContour()\n");
}
void gluTessBeginPolygon() {
  printf(" UNIMPLEMENTED function gluTessBeginPolygon()\n");
}
void gluTessEndPolygon() {
  printf(" UNIMPLEMENTED function gluTessEndPolygon()\n");
}
void gluTessEndContour() {
  printf(" UNIMPLEMENTED function gluTessEndContour()\n");
}
#endif

/*----------------------------------------------------------------------*/
