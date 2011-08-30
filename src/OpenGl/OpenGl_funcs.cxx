/***********************************************************************

FONCTION :
----------
File OpenGl_funcs :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT
13-06-96 : CAL ; Gestion de la transparence dans redraw_all_structs
09-07-97 : FMN ; Verification calcul matrice orientation et projection.
Je n'ai verifie que le mode parallele pas le mode perspective.
18-07-97 : FMN ; Desactivation des lights au demarrage
05-08-97 : PCT ; Support texture mapping
19-08-97 : PCT ; ajout reflexion
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
et TelBackInteriorShadingMethod
13-01-98 : FMN ; Oublie printf
03-03-98 : FMN ; Suppression variable externe TglWhetherPerspective 
13-03-98 : FMN ; Suppression variable externe TglUpdateView0
17-03-98 : FMN ; Ajout mode animation
- deplacement TelSetViewIndex de OpenGl_execstruct.c ici.
- gestion du mode animation.
29-04-98 : FMN ; Mode animation
- ajout TsmPushAttri et TsmPopAttri necessaire pour
sauvegarder la point de vue.
- ajout init de la var TglActiveWs
08-07-98 : FMN ; Mode animation: ajout print debug
27-11-98 : CAL ; S4062. AJout des layers.
30-11-98 : FMN ; S3819 : Textes toujours visibles
14-12-98 : BGN ; (S3989, Phase "triedre") ajout du reaffichage
du triedre .
05-01-99 : CAL ; Warning WNT
22-03-04 : SAN ; OCC4895 High-level interface for controlling polygon offsets

************************************************************************/

#define G003    /* EUG 20-09-99 ; Animation management
*/


/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <stdio.h>
#include <stdlib.h>

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_util.hxx> 
#include <OpenGl_LightBox.hxx>
#include <OpenGl_TextureBox.hxx>
#include <OpenGl_animation.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_telem.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_trsf_stack.hxx>
#include <OpenGl_triedron.hxx>
#include <OpenGl_graduatedtrihedron.hxx>

#ifdef G003
# include <OpenGl_degeneration.hxx>
#endif  /* G003 */

#if defined(WNT)
#include <GL/glu.h>
#endif

#include <OSD_FontAspect.hxx>
/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define NO_PRINT
#define NO_DEBUG
#define NO_DEBUG_ANIMATION

#define EPSI 0.0001

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

tsm_trsf_stack trsf_stack = NULL;
tsm_trsf_stack cur_trsf_stack = NULL;

Tint    ForbidSetTextureMapping;    /* valid only during traversal */
Tint    SecondPassNeeded;           /* valid only during traversal */
Tint    SecondPassDo;               /* valid only during traversal */

#ifdef G003
extern GLboolean g_fUpdateAM;
extern GLboolean g_fList;
int    g_nBackfacing;
#endif  /* G003 */


/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

static void call_util_apply_trans2( float ix, float iy, float iz, matrix3 mat,
                                   float *ox, float *oy, float *oz );
static void call_util_mat_mul( matrix3 mat_a, matrix3 mat_b, matrix3 mat_c);

#ifdef DEBUG
static void pr_matrix( matrix3 mat );
#endif

/*----------------------------------------------------------------------*/
/*
* Fonctions externes
*/

void
call_func_set_text_style(int lid)
{
  CMN_KEY  key;

  key.data.ldata = lid;
  TsmAddToStructure( TelTextStyle, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_text_display_type(int lid)
{
  CMN_KEY  key;

  key.data.ldata = lid;
  TsmAddToStructure( TelTextDisplayType, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/

void
call_func_label(Tint lid)
{
  CMN_KEY  key;

  key.data.ldata = lid;
  TsmAddToStructure( TelLabel, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_appl_data(void *ptr)
{
  CMN_KEY  key;

  key.data.pdata = ptr;
  TsmAddToStructure( TelApplicationData, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_view_ind(Tint vid)
{
  CMN_KEY  key;

  key.data.ldata = vid;
  TsmAddToStructure( TelViewIndex, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_local_tran3( Tmatrix3  mat, TComposeType  mode )
{
  CMN_KEY  key;

  key.id = mode;
  key.data.pdata = mat;
  TsmAddToStructure( TelLocalTran3, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_linewidth( Tfloat  lw )
{
  CMN_KEY  key;

  key.data.fdata = lw;
  TsmAddToStructure( TelPolylineWidth, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_linetype( Tint  lt )
{
  CMN_KEY  key;

  key.data.ldata = lt;
  TsmAddToStructure( TelPolylineType, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_edge_type( Tint  lt )
{
  CMN_KEY  key;

  key.data.ldata = lt;
  TsmAddToStructure( TelEdgeType, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_int_style( Tint  is )
{
  CMN_KEY  key;

  key.data.ldata = is;
  TsmAddToStructure( TelInteriorStyle, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_edge_flag( Tint st )
{
  CMN_KEY  key;

  key.data.ldata = st;
  TsmAddToStructure( TelEdgeFlag, 1, &key );

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_edgewidth( Tfloat  ew )
{
  CMN_KEY  key;

  key.data.fdata = ew;
  TsmAddToStructure( TelEdgeWidth, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_int_style_ind( Tint  ind )
{
  CMN_KEY  key;

  key.data.ldata = ind;
  TsmAddToStructure( TelInteriorStyleIndex, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_face_disting_mode( Tint  mode )
{
  CMN_KEY  key;

  key.data.ldata = mode;
  TsmAddToStructure( TelFaceDistinguishingMode, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_face_cull_mode( Tint  mode )
{
  CMN_KEY  key;

  key.data.ldata = mode;
  TsmAddToStructure( TelFaceCullingMode, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_marker_type( Tint type )
{
  CMN_KEY  key;

  key.data.ldata = type;
  TsmAddToStructure( TelPolymarkerType, 1, &key );

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_marker_size( Tfloat size )
{
  CMN_KEY  key;

  key.data.fdata = size;
  TsmAddToStructure( TelPolymarkerSize, 1, &key );

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_exec_struct(Tint stid)
{
  CMN_KEY  key;

  key.data.ldata = stid;
  TsmAddToStructure( TelExecuteStructure, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_init_tgl()
{
  TelHookOnAllClasses();
  return;
}

/*----------------------------------------------------------------------*/

void                            /* Ignoring priority */
call_func_post_struct( Tint ws, Tint stid, Tfloat priority )
{
  CMN_KEY_DATA k;

  k.ldata = stid;
  TsmSetWSAttri( ws, WSViewStid, &k );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_int_shad_meth(Tint mtd)
{
  CMN_KEY  key;

  key.data.ldata = mtd;
  TsmAddToStructure( TelInteriorShadingMethod, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_refl_eqn( Tint  eqn )
{
  CMN_KEY  key;

  key.data.ldata = eqn;
  TsmAddToStructure( TelInteriorReflectanceEquation, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_back_refl_eqn( Tint  eqn )
{
  CMN_KEY  key;

  key.data.ldata = eqn;
  TsmAddToStructure( TelBackInteriorReflectanceEquation, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_text_font( Tchar *font )
{
  CMN_KEY  key;


  key.data.pdata = font;
  TsmAddToStructure( TelTextFont, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_char_space( Tfloat sp )
{
  CMN_KEY  key;

  key.data.fdata = sp;
  TsmAddToStructure( TelCharacterSpacing, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_char_expan( Tfloat exp )
{
  CMN_KEY  key;

  key.data.fdata = exp;
  TsmAddToStructure( TelCharacterExpansionFactor, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/



void
call_func_set_text_zoomable( Tint flag )
{
  CMN_KEY  key;

  key.data.ldata = flag;
  TsmAddToStructure( TelTextZoomable, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/


void
call_func_set_text_angle( Tfloat angl )
{
  CMN_KEY  key;

  key.data.ldata = angl;
  TsmAddToStructure( TelTextAngle, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/


void
call_func_set_text_fontaspect( OSD_FontAspect fontaspect )
{
  CMN_KEY  key;

  key.data.ldata = fontaspect;
  TsmAddToStructure( TelTextFontAspect, 1, &key );
  return;
}



/*----------------------------------------------------------------------*/

/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
void call_func_set_polygon_offset_params( Tint mode, Tfloat factor, Tfloat units )
{
  CMN_KEY  key;
  TEL_POFFSET_PARAM  param;

  param.mode   = mode;
  param.factor = factor;
  param.units  = units;

  key.data.pdata = &param;
  TsmAddToStructure( TelPolygonOffset, 1, &key );
}
/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

/*----------------------------------------------------------------------*/

void
call_func_set_pick_id(Tint pid)
{
  CMN_KEY  key;

  key.data.ldata = pid;
  TsmAddToStructure( TelPickId, 1, &key );
  return;
}
/*----------------------------------------------------------------------*/

static void redraw_all_structs( Tint wsid, Tint vstid )
{
  CMN_KEY_DATA   k;
  GLint blend_dst, blend_src;
  GLboolean blend_state;    
  GLint zbuff_f;
  GLboolean zbuff_state, zbuff_w;

  /* Est-il necessaire de faire de la transparence ? */
  TsmGetWSAttri( wsid, WSTransparency, &k );
  TelSetTransparency( k.ldata );

  /* ajout PCT pour gestion des textures */
  TsmGetWSAttri(wsid, WSSurfaceDetail, &k);
  switch (k.ldata)
  {
    /* TOD_NONE */
  case 0:
    ForbidSetTextureMapping = 1;
    SecondPassNeeded = 0;
    SecondPassDo = 0;
    DisableTexture();
    TsmDisplayStructure( vstid, wsid );
    break;

    /* TOD_ENVIRONMENT */
  case 1:
    ForbidSetTextureMapping = 1;
    SecondPassNeeded = 0;
    SecondPassDo = 0;
    TsmGetWSAttri(wsid, WSTextureEnv, &k);
    SetCurrentTexture(k.ldata);
    EnableTexture();
    TsmDisplayStructure( vstid, wsid );
    DisableTexture();
    break;

    /* TOD_ALL */
  case 2:
    /* premiere passe */
    ForbidSetTextureMapping = 0;
    SecondPassNeeded = 0;
    SecondPassDo = 0;
    TsmDisplayStructure( vstid, wsid );
    DisableTexture();

    /* deuxieme passe */
    if (SecondPassNeeded)
    {
      SecondPassDo = 1;
      TsmGetWSAttri(wsid, WSTextureEnv, &k);
      SetCurrentTexture(k.ldata);
      EnableTexture();

      /* sauvegarde de quelques parametres OpenGL */
      glGetBooleanv(GL_DEPTH_WRITEMASK, &zbuff_w);
      glGetIntegerv(GL_DEPTH_FUNC, &zbuff_f);
      glGetIntegerv(GL_BLEND_DST, &blend_dst); 
      glGetIntegerv(GL_BLEND_SRC, &blend_src);    
      zbuff_state = glIsEnabled(GL_DEPTH_TEST);
      blend_state = glIsEnabled(GL_BLEND);


      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
      glEnable(GL_BLEND);

      glDepthFunc(GL_EQUAL);
      glDepthMask(GL_FALSE);
      glEnable(GL_DEPTH_TEST);

      ForbidSetTextureMapping = 1;
      TsmDisplayStructure(vstid, wsid );
      DisableTexture();

      /* restauration des parametres OpenGL */
      glBlendFunc(blend_src, blend_dst);
      if (!blend_state) glDisable(GL_BLEND);

      glDepthFunc(zbuff_f);
      glDepthMask(zbuff_w);
      if (!zbuff_state) glDisable(GL_DEPTH_FUNC);
    }
    break;
  }  
}

/*----------------------------------------------------------------------*/

void
call_func_redraw_all_structs( Tint wsid, Tint swap )
{
  call_func_redraw_all_structs_begin (wsid);
  call_func_redraw_all_structs_proc (wsid);
  call_func_redraw_all_structs_end (wsid, swap);
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_anno_char_ht( Tfloat h )
{
  CMN_KEY  key;

  key.data.fdata = h;
  TsmAddToStructure( TelTextHeight, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void /* unimplemented */
call_func_set_anno_char_up_vec( Tfloat path[2] )
{
  return;
}

/*----------------------------------------------------------------------*/

void /* unimplemented */
call_func_set_anno_path( Tint path )
{
  return;
}

/*----------------------------------------------------------------------*/


void /* unimplemented */
call_func_set_anno_align( Tint hor, Tint ver )
{
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  CMN_KEY  k;
  TEL_ALIGN_DATA data;
  data.Hmode = hor;
  data.Vmode = ver;
  k.data.pdata = &data;

  TsmAddToStructure( TelTextAlign, 1, &k );
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_anno_text_rel3( tel_point pt, Techar *str )
{
  CMN_KEY  k[2];

  k[0].id = TEXT_ATTACH_PT_ID;
  k[0].data.pdata = pt;
  k[1].id = TEXT_STRING_ID;
  k[1].data.pdata = str;
  TsmAddToStructure( TelText, 2, &k[0], &k[1] );

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_dcue_ind(Tint dcid)
{
  CMN_KEY  key;

  key.data.ldata = dcid;
  TsmAddToStructure( TelDepthCueIndex, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_texture_id(Tint dcid)
{
  CMN_KEY  key;

  key.data.ldata = dcid;
  TsmAddToStructure( TelTextureId, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

void
call_func_set_do_texturemap(Tint dcid)
{
  CMN_KEY  key;

  key.data.ldata = dcid;
  TsmAddToStructure( TelDoTextureMap, 1, &key );
  return;
}

/*----------------------------------------------------------------------*/

#define EPSI 0.0001

#ifdef DEBUG
static void
pr_matrix( matrix3 mat )
{
  printf( "%1.12f %1.12f %1.12f %1.12f\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3] );
  printf( "%1.12f %1.12f %1.12f %1.12f\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3] );
  printf( "%1.12f %1.12f %1.12f %1.12f\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3] );
  printf( "%1.12f %1.12f %1.12f %1.12f\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3] );
  printf( "\n" );
  return;
}
#endif

/*
*  Evaluates orientation matrix.
*/
/* OCC18942: obsolete in OCCT6.3, might be removed in further versions! */
void call_func_eval_ori_matrix3 (
                                 point3  *vrp,               /* view reference point */
                                 vec3    *vpn,               /* view plane normal    */
                                 vec3    *vup,               /* view up vector       */
                                 int     *err_ind, 
                                 float   mout[4][4]) /* OUT view orientation matrix  */
{

  /* Translate to VRP then change the basis.
  * The old basis is: e1 = < 1, 0, 0>, e2 = < 0, 1, 0>, e3 = < 0, 0, 1>.
  * The new basis is: ("x" means cross product)
  *  e3' = VPN / |VPN|
  *  e1' = VUP x VPN / |VUP x VPN|
  *  e2' = e3' x e1'
  * Therefore the transform from old to new is x' = TAx, where:
  *
  *       | e1'x e2'x e3'x 0 |         |   1      0      0      0 |
  *   A = | e1'y e2'y e3'y 0 |,    T = |   0      1      0      0 |
  *       | e1'z e2'z e3'z 0 |         |   0      0      1      0 |
  *       |  0    0    0   1 |         | -vrp.x -vrp.y -vrp.z   1 |
  *
  */

  /*
  * These ei's are really ei primes.
  */
  register float      (*m)[4][4];
  point3      e1, e2, e3, e4;
  double      s, v;

  /*
  * e1' = VUP x VPN / |VUP x VPN|, but do the division later.
  */
  e1.x = vup->delta_y * vpn->delta_z - vup->delta_z * vpn->delta_y;
  e1.y = vup->delta_z * vpn->delta_x - vup->delta_x * vpn->delta_z;
  e1.z = vup->delta_x * vpn->delta_y - vup->delta_y * vpn->delta_x;
  s = sqrt( e1.x * e1.x + e1.y * e1.y + e1.z * e1.z);
  e3.x = vpn->delta_x;
  e3.y = vpn->delta_y;
  e3.z = vpn->delta_z;
  v = sqrt( e3.x * e3.x + e3.y * e3.y + e3.z * e3.z);
  /*
  * Check for vup and vpn colinear (zero dot product).
  */
  if ((s > -EPSI) && (s < EPSI))
    *err_ind = 2;
  else
    /*
    * Check for a normal vector not null.
    */
    if ((v > -EPSI) && (v < EPSI))
      *err_ind = 3;
    else {
      /*
      * Normalize e1
      */
      e1.x /= ( float )s;
      e1.y /= ( float )s;
      e1.z /= ( float )s;
      /*
      * e3 = VPN / |VPN|
      */
      e3.x /= ( float )v;
      e3.y /= ( float )v;
      e3.z /= ( float )v;
      /*
      * e2 = e3 x e1
      */
      e2.x = e3.y * e1.z - e3.z * e1.y;
      e2.y = e3.z * e1.x - e3.x * e1.z;
      e2.z = e3.x * e1.y - e3.y * e1.x;
      /*
      * Add the translation
      */
      e4.x = -( e1.x * vrp->x + e1.y * vrp->y + e1.z * vrp->z);
      e4.y = -( e2.x * vrp->x + e2.y * vrp->y + e2.z * vrp->z);
      e4.z = -( e3.x * vrp->x + e3.y * vrp->y + e3.z * vrp->z);
      /*
      * Homogeneous entries
      *
      *  | e1.x  e2.x  e3.x  0.0 |   | 1  0  0  0 |
      *  | e1.y  e2.y  e3.y  0.0 | * | 0  1  0  0 |
      *  | e1.z  e2.z  e3.z  0.0 |   | a  b  1  c |
      *  | e4.x  e4.y  e4.z  1.0 |   | 0  0  0  1 |
      */

      m = (float (*)[4][4])mout;

      (*m)[0][0] = e1.x;
      (*m)[0][1] = e2.x;
      (*m)[0][2] = e3.x;
      (*m)[0][3] = ( float )0.0;

      (*m)[1][0] = e1.y;
      (*m)[1][1] = e2.y;
      (*m)[1][2] = e3.y;
      (*m)[1][3] = ( float )0.0;

      (*m)[2][0] = e1.z;
      (*m)[2][1] = e2.z;
      (*m)[2][2] = e3.z;
      (*m)[2][3] = ( float )0.0;

      (*m)[3][0] = e4.x;
      (*m)[3][1] = e4.y;
      (*m)[3][2] = e4.z;
      (*m)[3][3] = ( float )1.0;

      *err_ind = 0;

#ifdef DEBUG
      printf( "\n->call_func_eval_ori_matrix3 \n" );
      printf( "orientation_matrix :\n" );
      pr_matrix(mout);
#endif
    }
}

/*----------------------------------------------------------------------*/
/*
*  Evaluates mapping matrix.
*/
/* OCC18942: obsolete in OCCT6.3, might be removed in further versions! */
void call_func_eval_map_matrix3(
                                view_map3 *Map, 
                                int *err_ind, 
                                matrix3 mat)
{
  int i, j;
  matrix3 Tpar, Spar;
  matrix3 Tper, Sper;
  matrix3 Shear;
  matrix3 Scale;
  matrix3 Tprp;
  matrix3 aux_mat1, aux_mat2, aux_mat3;
  point3 Prp;

  *err_ind = 0;
  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      Spar[i][j] = Sper[i][j] = aux_mat1[i][j] = aux_mat2[i][j] =
      aux_mat3[i][j] = Tper[i][j] = Tpar[i][j] = Tprp[i][j] =
      Shear[i][j] = Scale[i][j] = ( float )(i == j);

  Prp.x = Map->proj_ref_point.x;
  Prp.y = Map->proj_ref_point.y;
  Prp.z = Map->proj_ref_point.z;

  /*
  * Type Parallele
  */    
  if (Map->proj_type == TYPE_PARAL)
  {
    float umid, vmid;
    point3 temp;

#ifdef FMN
    float    cx, cy, gx, gy, xsf, ysf, zsf;
    float    fpd, bpd;
    float    dopx, dopy, dopz;
    matrix3  tmat = { { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 } };
    matrix3  smat = { { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 } };
    matrix3 shmat = { { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 } };
    matrix3 tshmat = { { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
    { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 } };

    /* centers */
    cx = Map->win.x_min + Map->win.x_max, cx /= ( float )2.0;
    cy = Map->win.y_min + Map->win.y_max, cy /= ( float )2.0;

    gx = 2.0/ (Map->win.x_max - Map->win.x_min);
    gy = 2.0/ (Map->win.y_max - Map->win.y_min);

    tmat[0][3] = -cx;
    tmat[1][3] = -cy;
    tmat[2][3] = (Map->front_plane + Map->back_plane)/(Map->front_plane - Map->back_plane);

    smat[0][0] = gx;
    smat[1][1] = gy;
    smat[2][2] = -2./(Map->front_plane - Map->back_plane);

    /* scale factors */
    dopx = cx - Prp.x;
    dopy = cy - Prp.y;
    dopz = - Prp.z;

    /* map matrix */
    shmat[0][2] = -(dopx/dopz);
    shmat[1][2] = -(dopy/dopz);

    /* multiply to obtain mapping matrix */
    call_util_mat_mul( tmat, shmat, tshmat );
    call_util_mat_mul( smat, tshmat, mat );

#ifdef DEBUG
    printf( "\n->call_func_eval_map_matrix3 FMN\n" );
    printf("prp %f %f %f \n", Prp.x, Prp.y, Prp.z);
    printf("vpd fpd bpd %f %f %f \n", Map->view_plane, Map->front_plane, Map->back_plane);
    printf("window limits %f %f %f %f\n", Map->win.x_min, Map->win.x_max, 
      Map->win.y_min, Map->win.y_max);
    printf( "mapping_matrix :\n" );
    pr_matrix(mat);
#endif
    return;         
#endif

    /* CAL */
    Map->proj_vp.z_min = ( float )0.0;
    Map->proj_vp.z_max = ( float )1.0;
    /* CAL */

    /* Shear matrix calculation */
    umid = ( float )(Map->win.x_min+Map->win.x_max)/( float )2.0;
    vmid = ( float )(Map->win.y_min+Map->win.y_max)/( float )2.0;
    if(Prp.z == Map->view_plane){
      /* Projection reference point is on the view plane */
      *err_ind = 1;
      return;
    }
    Shear[2][0] = ( float )(-1.0) * ((Prp.x-umid)/(Prp.z-Map->view_plane));
    Shear[2][1] = ( float )(-1.0) * ((Prp.y-vmid)/(Prp.z-Map->view_plane));

    /*
    * Calculate the lower left coordinate of the view plane
    * after the Shearing Transformation.
    */
    call_util_apply_trans2(Map->win.x_min, Map->win.y_min,
      Map->view_plane, Shear, &(temp.x), &(temp.y), &(temp.z));

    /* Translate the back plane to the origin */
    Tpar[3][0] = ( float )(-1.0) * temp.x;
    Tpar[3][1] = ( float )(-1.0) * temp.y;
    Tpar[3][2] = ( float )(-1.0) * Map->back_plane;

    call_util_mat_mul(Shear, Tpar, aux_mat1);

    /* Calculation of Scaling transformation */
    Spar[0][0] = ( float )1.0 / (Map->win.x_max - Map->win.x_min);
    Spar[1][1] = ( float )1.0 / (Map->win.y_max - Map->win.y_min);
    Spar[2][2] = ( float )1.0 / (Map->front_plane - Map->back_plane );
    call_util_mat_mul (aux_mat1, Spar, aux_mat2);
    /* Atlast we transformed view volume to NPC */

    /* Translate and scale the view plane to projection view port */
    if(Map->proj_vp.x_min < 0.0 || Map->proj_vp.y_min < 0.0 ||
      Map->proj_vp.z_min < 0.0 || Map->proj_vp.x_max > 1.0 ||
      Map->proj_vp.y_max > 1.0 || Map->proj_vp.z_max > 1.0 ||
      Map->proj_vp.x_min > Map->proj_vp.x_max ||
      Map->proj_vp.y_min > Map->proj_vp.y_max ||
      Map->proj_vp.z_min > Map->proj_vp.z_max){
        *err_ind = 1;
        return;
      }
      for(i=0; i<4; i++)
        for(j=0; j<4; j++)
          aux_mat1[i][j] = (float)(i==j);
      aux_mat1[0][0] = Map->proj_vp.x_max-Map->proj_vp.x_min;
      aux_mat1[1][1] = Map->proj_vp.y_max-Map->proj_vp.y_min;
      aux_mat1[2][2] = Map->proj_vp.z_max-Map->proj_vp.z_min;
      aux_mat1[3][0] = Map->proj_vp.x_min;
      aux_mat1[3][1] = Map->proj_vp.y_min;
      aux_mat1[3][2] = Map->proj_vp.z_min;
      call_util_mat_mul (aux_mat2, aux_mat1, mat);

#ifdef DEBUG
      printf( "\n->call_func_eval_map_matrix3 - ORTHOGRAPHIC projection\n" );
      printf("prp %f %f %f \n", Prp.x, Prp.y, Prp.z);
      printf("vpd fpd bpd %f %f %f \n", Map->view_plane, Map->front_plane, Map->back_plane);
      printf("window limits %f %f %f %f\n", Map->win.x_min, Map->win.x_max, 
        Map->win.y_min, Map->win.y_max);
      printf( "mapping_matrix :\n" );
      pr_matrix(mat);
#endif

      return;         
  } 

  /*
  * Type Perspective
  */    
  else if (Map->proj_type == TYPE_PERSPECT)
  {
    float umid, vmid;
    float B, F, V;
    float Zvmin;

    /* CAL */
    Map->proj_vp.z_min = ( float )0.0;
    Map->proj_vp.z_max = ( float )1.0;
    /* CAL */

    B = Map->back_plane;
    F = Map->front_plane;
    V = Map->view_plane;

    if(Prp.z == Map->view_plane){
      /* Centre of Projection is on the view plane */
      *err_ind = 1;
      return;
    }
    if(Map->proj_vp.x_min < 0.0 || Map->proj_vp.y_min < 0.0 ||
      Map->proj_vp.z_min < 0.0 || Map->proj_vp.x_max > 1.0 ||
      Map->proj_vp.y_max > 1.0 || Map->proj_vp.z_max > 1.0 ||
      Map->proj_vp.x_min > Map->proj_vp.x_max ||
      Map->proj_vp.y_min > Map->proj_vp.y_max ||
      Map->proj_vp.z_min > Map->proj_vp.z_max ||
      F < B){
        *err_ind = 1;
        return;
      }

      /* This is the transformation to move VRC to Center Of Projection */
      Tprp[3][0] = ( float )(-1.0)*Prp.x;
      Tprp[3][1] = ( float )(-1.0)*Prp.y;
      Tprp[3][2] = ( float )(-1.0)*Prp.z;

      /* Calculation of Shear matrix */
      umid = ( float )(Map->win.x_min+Map->win.x_max)/( float )2.0-Prp.x;
      vmid = ( float )(Map->win.y_min+Map->win.y_max)/( float )2.0-Prp.y;
      Shear[2][0] = ( float )(-1.0)*umid/(Map->view_plane-Prp.z);
      Shear[2][1] = ( float )(-1.0)*vmid/(Map->view_plane-Prp.z);
      call_util_mat_mul(Tprp, Shear, aux_mat3);

      /* Scale the view volume to canonical view volume
      * Centre of projection at origin.
      * 0 <= N <= -1, -0.5 <= U <= 0.5, -0.5 <= V <= 0.5
      */
      Scale[0][0] =  (( float )(-1.0)*Prp.z+V)/
        ((Map->win.x_max-Map->win.x_min)*(( float )(-1.0)*Prp.z+B));
      Scale[1][1] =  (( float )(-1.0)*Prp.z+V)/
        ((Map->win.y_max-Map->win.y_min)*(( float )(-1.0)*Prp.z+B));
      Scale[2][2] =  ( float )(-1.0) / (( float )(-1.0)*Prp.z+B);

      call_util_mat_mul(aux_mat3, Scale, aux_mat1);

      /*
      * Transform the Perspective view volume into
      * Parallel view volume.
      * Lower left coordinate: (-0.5,-0.5, -1)
      * Upper right coordinate: (0.5, 0.5, 1.0)
      */
      Zvmin = ( float )(-1.0*(-1.0*Prp.z+F)/(-1.0*Prp.z+B));
      aux_mat2[2][2] = ( float )1.0/(( float )1.0+Zvmin);
      aux_mat2[2][3] = ( float )(-1.0);
      aux_mat2[3][2] = ( float )(-1.0)*Zvmin*aux_mat2[2][2];
      aux_mat2[3][3] = ( float )0.0;
      call_util_mat_mul(aux_mat1, aux_mat2, Shear);

      for(i=0; i<4; i++)
        for(j=0; j<4; j++)
          aux_mat1[i][j] = aux_mat2[i][j] = (float)(i==j);

      /* Translate and scale the view plane to projection view port */
      aux_mat2[0][0] = (Map->proj_vp.x_max-Map->proj_vp.x_min);
      aux_mat2[1][1] = (Map->proj_vp.y_max-Map->proj_vp.y_min);
      aux_mat2[2][2] = (Map->proj_vp.z_max-Map->proj_vp.z_min);
      aux_mat2[3][0] = aux_mat2[0][0]/( float )2.0+Map->proj_vp.x_min;
      aux_mat2[3][1] = aux_mat2[1][1]/( float )2.0+Map->proj_vp.y_min;
      aux_mat2[3][2] = aux_mat2[2][2]+Map->proj_vp.z_min;
      call_util_mat_mul (Shear, aux_mat2, mat);

#ifdef DEBUG
      printf( "\n->call_func_eval_map_matrix3 - PERSPECTIVE projection\n" );
      printf("prp %f %f %f \n", Prp.x, Prp.y, Prp.z);
      printf("vpd fpd bpd %f %f %f \n", Map->view_plane, Map->front_plane, Map->back_plane);
      printf("window limits %f %f %f %f\n", Map->win.x_min, Map->win.x_max, 
        Map->win.y_min, Map->win.y_max);
      printf("viewport limits %f %f %f %f %f %f\n", Map->proj_vp.x_min, Map->proj_vp.x_max, 
        Map->proj_vp.y_min, Map->proj_vp.y_max,
        Map->proj_vp.z_min, Map->proj_vp.z_max);
      printf( "mapping_matrix :\n" );
      pr_matrix(mat);
#endif

      return;
  }
  else
    *err_ind = 1;
}

/*----------------------------------------------------------------------*/

static void
call_util_apply_trans2( float ix, float iy, float iz, matrix3 mat,
                       float *ox, float *oy, float *oz )
{
  float temp;
  *ox = ix*mat[0][0]+iy*mat[1][0]+iz*mat[2][0]+mat[3][0];
  *oy = ix*mat[0][1]+iy*mat[1][1]+iz*mat[2][1]+mat[3][1];
  *oz = ix*mat[0][2]+iy*mat[1][2]+iz*mat[2][2]+mat[3][2];
  temp = ix * mat[0][3]+iy * mat[1][3]+iz * mat[2][3]+mat[3][3];
  *ox /= temp;
  *oy /= temp;
  *oz /= temp;
}

/*----------------------------------------------------------------------*/

static void
call_util_mat_mul( matrix3 mat_a, matrix3 mat_b, matrix3 mat_c)
{
  int i, j, k;

  for (i=0; i<4; i++)
    for (j=0; j<4; j++)
      for (mat_c[i][j] = ( float )0.0,k=0; k<4; k++)
        mat_c[i][j] += mat_a[i][k] * mat_b[k][j];
}

/*----------------------------------------------------------------------*/

void
call_func_redraw_all_structs_begin( Tint wsid )
{

  static Tmatrix3 identity = {
    {( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0},
    {( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0},
    {( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0},
    {( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0}
  };

  /*
  * Gestion de la pile des transformations pour les structures
  * connectees dans la vue <wsid>
  */
  if (trsf_stack == NULL) {
    trsf_stack = (tsm_trsf_stack) malloc (sizeof (TSM_TRSF_STACK));
    trsf_stack->next = NULL;
    trsf_stack->prev = NULL;
    matcpy (trsf_stack->LocalTran3, identity);
    cur_trsf_stack = trsf_stack;
  }

  TelClearViews(wsid);

  /* Par defaut on desactive les lumieres */
  LightOff();

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_redraw_all_structs_proc( Tint wsid )
{

  CMN_KEY        kk;
  CMN_KEY_DATA   k;
#ifdef G003
  CMN_KEY_DATA   kbf;
#endif  /* G003 */
  Tint           vstid;

  TsmGetWSAttri( wsid, WSViewStid, &k );
#ifdef G003
  TsmGetWSAttri ( wsid, WSBackfacing, &kbf );
  g_nBackfacing = kbf.ldata;

  if ( g_nBackfacing > 0 )

    glDisable ( GL_CULL_FACE );

  else if ( g_nBackfacing < 0 ) {

    glEnable   ( GL_CULL_FACE );
    glCullFace ( GL_BACK      );

  }  /* end if */
#endif  /* G003 */
  vstid = k.ldata;
  if( vstid != -1 )
  {

#ifdef PRINT
    printf("OpenGl_funcs::call_func_redraw_all_structs:TsmPushAttri \n");
#endif
    TsmPushAttri(); /* save previous graphics context */

    TglActiveWs = wsid; /* Indispensable precedemment dans TsmDisplayStructure */
    /* Avec le mode Ajout cette init n'etait plus faite */

    /* mise en place des matrices de projection et de mapping */
    kk.id = TelViewIndex;
    kk.data.ldata = vstid;
    TsmSetAttri( 1, &kk );
    TelSetViewIndex (wsid, vstid);
#ifdef G003
    if ( g_fUpdateAM ) goto forceRedraw;
#endif  /* G003 */
    /* Mode animation */
    if (animationFlag)
    {
      if ((listIndexFlag) && (listIndexView == vstid))
      {
#ifdef DEBUG_ANIMATION
        printf("call_func_redraw_all_structs::glCallList1 \n");
#endif
        glCallList(listIndex);
      }
      else
      {
#ifdef G003
        if ( !g_fList ) {

          g_fList = GL_TRUE;
          goto forceRedraw;

        }  /* end if */
#endif  /* G003 */

#ifdef DEBUG_ANIMATION
        printf("call_func_redraw_all_structs::glNewList \n");
#endif
        glNewList(listIndex, GL_COMPILE_AND_EXECUTE);
        redraw_all_structs( wsid, vstid );
#ifdef DEBUG_ANIMATION
        printf("call_func_redraw_all_structs::glEndList \n");
#endif
        glEndList();
        listIndexFlag = GL_TRUE;
        listIndexView = vstid;
      }    
    }
    /* Mode normal */
    else
    {
      /* Optimisation si displaylist est toujours valable */
      if ((listIndexFlag) && (listIndexView == vstid))
      {
#ifdef DEBUG_ANIMATION
        printf("call_func_redraw_all_structs::glCallList2 \n");
#endif
        glCallList(listIndex);
      }
      else
      {
#ifdef DEBUG_ANIMATION
        printf("call_func_redraw_all_structs::redraw_all_structs \n");
#endif

#ifdef G003
forceRedraw:
#endif  /* G003 */
        redraw_all_structs( wsid, vstid );
      }    
    } /* Mode Animation */

#ifdef PRINT
    printf("OpenGl_funcs::call_func_redraw_all_structs:TsmPopAttri \n");
#endif
    /* restore previous graphics context; before update lights */
    TsmPopAttri();



    /* affichage de Triedre Non Zoomable de la vue s'il existe */
#ifdef PRINT
    printf("call_func_redraw_all_structS => CALL_TRIEDRON... \n");
#endif
    call_triedron_redraw_from_wsid (wsid);
    call_graduatedtrihedron_redraw(wsid);


  } /* Test vue valide */

  return;
}

/*----------------------------------------------------------------------*/

void
call_func_redraw_all_structs_end( Tint wsid, Tint swap )
{

  /*    CMN_KEY      kk;*/
  CMN_KEY_DATA   k;
  Tint           vstid;

  TsmGetWSAttri( wsid, WSViewStid, &k );
  vstid = k.ldata;
  if( vstid != -1 )
  {

    /* On swap les buffers */
    TsmGetWSAttri( wsid, WSDbuff, &k );
    if( (k.ldata == TOn) && (swap) )
      TelSwapBuffers( wsid );
    else
      TelFlush(0);

  } /* Test vue valide */

  /*
  * Mise a jour de l'update_state
  */
  k.ldata = TDone;
  TsmSetWSAttri( wsid, WSUpdateState, &k );

  return;
}

#ifdef G003
void call_func_set_degenerate_model ( Tint model, Tfloat skipRatio ) {

  CMN_KEY      key;
  DEGENERATION deg;

  deg.mode      = model;
  deg.skipRatio = skipRatio;

  key.data.pdata = &deg;
  TsmAddToStructure ( TelDegenerationMode, 1, &key );

}  /* end call_func_set_degenerate_model */
#endif  /* G003 */


void
call_func_set_transform_persistence( Tint mode, Tfloat x, Tfloat y, Tfloat z )
{
  CMN_KEY  key;
  TEL_TRANSFORM_PERSISTENCE  pers;

  pers.mode   = mode;

  pers.pointX = x;
  pers.pointY = y;
  pers.pointZ = z;

  key.data.pdata = &pers;
  TsmAddToStructure( TelTransformPersistence, 1, &key );
  return;
}
/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
