/***********************************************************************

FONCTION :
----------
File OpenGl_view :


REMARQUES:
---------- 

An implementation of the PHIGS viewing pipeline

This module provides services for setting view representations and view
indices, viewport management, coordinate conversion routines and help
functions.

The corresponding header file is telem_view.h


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
20-03-96 : CAL ; Modif de TelClearViews
appel a call_tox_getscreen pour effacer tout
meme apres un resize
01-04-96 : CAL ; Integration MINSK portage WNT
22-04-96 : FMN ; Suppression prototype inutile.
25-06-96 : FMN ; Suppression utilisation de glScissor.
28-06-96 : CAL ; Simplification de TelClearViews
02-07-96 : FMN ; Suppression WSWSHeight et WSWSWidth
Suppression de TelSetWSWindow
Suppression glViewport inutile
03-07-96 : FMN ; A une workstation correspond une vue.
12-07-96 : FMN ; Correction calcul matrice orientatione.
17-07-96 : FMN ; Ajout clear pour le zbuffer.
21-10-96 : FMN ; Suppression LMC_COLOR fait dans OpenGl_execstruct.c
24-01-97 : CAL ; Suppression code inutile (DEBUG_ONLY)
30-06-97 : FMN ; Suppression OpenGl_telem_light.h
27-08-97 : FMN ; Ajout glDepthMask avant le clear (PRO8939) 
09-12-97 : FMN ; Prise en compte environnement mapping. 
03-03-98 : FMN ; Suppression variable externe TglWhetherPerspective 
13-03-98 : FMN ; Suppression variable WSNeedsClear
10-04-98 : CAL ; Suppression du calcul de inverse_matrix
30-04-98 : FMN ; Suppression variable externe TglUpdateView0
24-11-98 : FMN ; PRO16215: Suppression test sur le GL_MAX_CLIP_PLANES 
Si OpenGL n'a pas de plans de clipping on ne le simule pas.
24-11-98 : FMN ; Correction sur la gestion de la perspective (cf Programming Guide)
- Suppression du test (prp between front and back planes)
- Suppression matrice perspective (modify the next two cells to change clipping policy)
Ces modifs sont necessaires pour pouvoir mettre le prp entre le Front et Back plane

************************************************************************/

#define BUC60920        /* GG 010601
Change the z buffer comparaison for minimizing
unavailable or unviewable drawing of pixels in the same plane
*/

#define BUC61044       /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */

#define OCC1188         /* SAV 23/12/02 TelClearBackground() draws background texture
if any was defined
*/


/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <OpenGl_cmn_varargs.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_tsm.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_Memory.hxx>
#include <Standard_TypeDef.hxx>

/*----------------------------------------------------------------------*/
/*
* Variable globales
*/

EXPORT extern GLboolean env_walkthrow; /* definit dans OpenGl_togl_begin.c */
/* OCC18942: The new perspective projection matrix is off by default */
EXPORT extern GLboolean env_sym_perspective; /* defined in OpenGl_togl_begin.c */

/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

#define NO_TRACE_MAT    
#define NO_TRACE_MATRIX 
#define NO_PRINT

#define ENVTEX


/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

#ifdef DEB
static void pr_matrix( Tmatrix3 );
#endif


/*----------------------------------------------------------------------*/
/*
* Type definitions
*/

struct TEL_VIEW_DATA
{
  TEL_VIEW_REP    vrep;
#ifdef CAL_100498
  Tmatrix3        inverse_matrix;/* accelerates UVN2XYZ conversion */
#endif
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TEL_VIEW_DATA *tel_view_data;   /* Internal data stored for every view rep */

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

static void set_clipplanes( tel_view_rep );      /* front & back clip planes */
static void set_userclipplanes( tel_view_rep );  /* user-defined clipping planes */

#ifdef CAL_100498
static  void  TelEvalInverseMatrix( Tfloat*, Tfloat*, Tfloat*, /*vrp,vpn,vup*/
                                    Tfloat, Tmatrix3 );        /*vpd,inverse*/
#endif
static  Tmatrix3  ident = {
  { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
  { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
  { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
  { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 }
};

/*----------------------------------------------------------------------*/
/*+
Qsearch number comparison routine

Compares view ids during sorting of the list of defined views
for a workstation
+*/

/*----------------------------------------------------------------------*/
/*+
Evaluate View Mapping Matrix

This routine computes the mapping matrix based on the Window and Viewport
limits, Projection type and View plane, Front plane and Back plane
distances.
+*/

static  void
EvalViewMappingMatrix( tel_view_mapping mapping /* View Mapping */,
                       Tint *error_ind          /* Out: Error Indicator */,
                       Tmatrix3 mat             /* Out: Mapping Matrix * */,
                       Tint     flag,
                       Tfloat   cx,
                       Tfloat   cy,
                       Tint     clip_flag,
                       Tlimit3  *clip_limit
                      )
{
  Tfloat    gx, gy, xsf, ysf, zsf;
  Tfloat    fpd, bpd;

  /* OCC18942 */
  Tfloat    n, f, r, l, t, b, Zprp, dx, dy, VPD;

  /* FSXXX */
  /* GLint gdtmp; */
  Tlimit3   vp = { ( float )-1.0, ( float )1.0, ( float )-1.0, ( float )1.0, ( float )1.0, ( float )-1.0 };
  Tmatrix3  pmat = { { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
  { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
  { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
  { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 } };
  Tmatrix3  mmat = { { ( float )1.0, ( float )0.0, ( float )0.0, ( float )0.0 },
  { ( float )0.0, ( float )1.0, ( float )0.0, ( float )0.0 },
  { ( float )0.0, ( float )0.0, ( float )1.0, ( float )0.0 },
  { ( float )0.0, ( float )0.0, ( float )0.0, ( float )1.0 } };

  fpd = mapping->fpd;
  bpd = mapping->bpd;

  /* invalid window */
  if( mapping->window.xmin >= mapping->window.xmax ||
    mapping->window.ymin >= mapping->window.ymax )
  {
    *error_ind = 1;    
    return;
  }

  /* invalid viewport */
  if( mapping->viewport.xmin >= mapping->viewport.xmax ||
    mapping->viewport.ymin >= mapping->viewport.ymax ||
    mapping->viewport.zmin >= mapping->viewport.zmax )
  {
    *error_ind = 2;   
    return;
  }

  /* invalid back/front plane distances */
  if( mapping->bpd >= mapping->fpd )
  {
    *error_ind = 3;   
    return;
  }

  /* prp between front and back planes */
  if (!env_walkthrow)
  {
    if( mapping->prp[2] < mapping->fpd &&
      mapping->prp[2] > mapping->bpd )
    {
      *error_ind = 4;   
      return;
    } 
  }

  if( mapping->prp[2] == mapping->vpd )
  {
    *error_ind = 5;   /* prp on view plane */
    return;
  }

  if( mapping->viewport.xmin < 0 ||
    mapping->viewport.xmax > 1 ||
    mapping->viewport.ymin < 0 ||
    mapping->viewport.ymax > 1 ||
    mapping->viewport.zmin < 0 ||
    mapping->viewport.zmax > 1 )
  {
    *error_ind = 6;   /* viewport limits outside NPC space */
    return;
  }

  *error_ind = 0;

  /* OCC18942: Moved here while debugging perspective projection matrix */
  /* centers */
  if( flag == 0 )
  {
    cx = mapping->window.xmin + mapping->window.xmax, cx /= ( float )2.0;
    cy = mapping->window.ymin + mapping->window.ymax, cy /= ( float )2.0;
  }

  gx = (cx - mapping->prp[0]) / (mapping->vpd - mapping->prp[2]);
  gy = (cy - mapping->prp[1]) / (mapping->vpd - mapping->prp[2]);

#ifdef PRINT
  printf("EvalViewMappingMatrix \n");
  printf("prp %f %f %f \n", mapping->prp[0], mapping->prp[1], mapping->prp[2]);
  printf("vpd fpd bpd %f %f %f \n", mapping->vpd, mapping->fpd, mapping->bpd);
  printf("window limit %f %f %f %f\n", mapping->window.xmin, mapping->window.xmax, 
    mapping->window.ymin, mapping->window.ymax);
  printf("viewport limit %f %f %f %f\n", mapping->viewport.xmin, mapping->viewport.xmax, 
    mapping->viewport.ymin, mapping->viewport.ymax);
#endif

  /* projection matrix */
  if( mapping->proj == TelParallel )
  {

    pmat[2][0] = -gx; pmat[3][0] = mapping->vpd*gx;
    pmat[2][1] = -gy; pmat[3][1] = mapping->vpd*gy;
  }
  else if ( !env_sym_perspective )/* TelPerspective */
  {
    pmat[0][0] = pmat[1][1] = mapping->prp[2] - mapping->vpd;
    pmat[2][0] = -gx; 
    pmat[2][1] = -gy; 
    pmat[2][3] = ( float )-1.0;
    pmat[3][0] = mapping->vpd * gx; 
    pmat[3][1] = mapping->vpd * gy; 
    pmat[3][3] = mapping->prp[2];

    /* modify the next two cells to change clipping policy */
    if (!env_walkthrow)
    {
      pmat[2][2] = mapping->prp[2] - ( fpd + bpd );
      pmat[3][2] = fpd * bpd; 
    }
  }
  /* OCC18942: New approach to calculation of mapping (projection) matrix */
  else 
  {
    dx = mapping->window.xmax - mapping->window.xmin;
    dy = mapping->window.ymax - mapping->window.ymin;
    Zprp = mapping->prp[2];
    VPD = Zprp - mapping->vpd;

    /* 
    Calculate canonical perspective projection parameters as if we were about 
    to use glFrustum() to create symmetric perspective frustum.

    After the view orientation matrix is applied, the coordinate system origin is located 
    at the VRP and oriented properly. However, the viewplane has width = dx and height = dy 
    and its center (cx, cy, VPD) is not always located at the view Z axis.
    The canonical perspective projection matrix assumes the eye is located at (0, 0, 0).
    Thus the old approach resulted in a non-symmetric perspective, 
    as X and Y coordinates of the projection reference point (PRP) were not updated
    when cx and cy changed. Moreover, such "static" perspective had some other disadvantages,
    such as non-realistic panning, i.e. instead of moving the eye (or camera) over the model
    a sort of "frame" moved over the static perspective projection picture, 
    exposing a part of this static picture to the user.

    In order to make the perspective symmetric, we need to translate 
    the coordinate system to PRP before projection.
    Thus we translate X, Y and Z co-ordinates by -cx, -cy and -Zprp respectively.

    NOTE: mat[i][j] means j-th element of i-th column, as OpenGL accepts the matrices
    in column-major order, while in C two-dimensional arrays are stored in memory
    in row-major order!

    VPD is used below instead of near clipping plane dispance (n) in order to simplify 
    calculation of l and r values. If we did not use VPD in the matrix calculation, we would have to 
    project 0.5 * dx, -0.5 * dx, 0.5 * dy and - 0.5 * dy onto the near clipping plane 
    to calculate these values.

    Pending issues:
    1. It is still necessary to find a way to calculate the perspective projection matrix 
    for TPM_WALKTHROUGH projection model. This projection model is not supported yet 
    by the new code.
    */
    r = .5f * dx;
    l = -r;
    t = .5f * dy;
    b = -t;
    n = Zprp - fpd; f = Zprp - bpd;

    mat[0][0] = 2.f * VPD / (r - l);
    mat[1][1] = 2.f * VPD / (t - b);
    mat[2][0] = (r + l) / (r - l);
    mat[2][1] = (t + b) / (t - b);
    mat[2][2] = - (f + n) / (f - n);
    mat[2][3] = -1.f;
    /* 
    The last column takes into account translation along X, Y and Z axis
    before projecting. This can be considered as a result of right-multiplying the canonical 
    perspective projection matrix P by a translation matrix T 
    (it differs form the canonical matrix by the last column only):
    | 1 0 0  -cx  |
    | 0 1 0  -cy  |
    mat = P * T, where T = | 0 0 1 -Zprp |
    | 0 0 0   1   |
    */
    mat[3][0] = -mat[2][0] * Zprp - mat[0][0] * cx;
    mat[3][1] = -mat[2][1] * Zprp - mat[1][1] * cy;
    mat[3][2] = -2.f * f * n / (f - n) - mat[2][2] * Zprp;
    mat[3][3] = Zprp;

#ifdef PRINT
    printf("r l t b n f: %f %f %f %f %f %f \n", r,l,t,b,n,f);
    printf( "mapping_matrix (new code):\n" );
    pr_matrix(mat);
#endif

    /* return here, as further calculations are related to the old approach */
    return;
  }

  /* scale factors */
  xsf = (vp.xmax - vp.xmin) / (mapping->window.xmax - mapping->window.xmin);
  ysf = (vp.ymax - vp.ymin) / (mapping->window.ymax - mapping->window.ymin);
  zsf = (vp.zmax - vp.zmin) / (fpd - bpd);

  /* map matrix */
  mmat[0][0] = xsf, mmat[1][1] = ysf, mmat[2][2] = zsf;
  mmat[3][0] = vp.xmin - xsf*mapping->window.xmin;
  mmat[3][1] = vp.ymin - ysf*mapping->window.ymin;
  mmat[3][2] = vp.zmin - zsf*bpd;

  /* multiply to obtain mapping matrix */
  TelMultiplymat3( mat, pmat, mmat );

#ifdef PRINT
  printf( "mapping_matrix :\n" );
  pr_matrix(mat);
#endif
}

/*----------------------------------------------------------------------*/
/*+
Set View Representation

This routine defines a view representation for a workstation.

The representation of the view is stored in a storage table.
An entry is made into the list of defined views for the workstation.
If the view id being defined already exists, its representation gets
modified, else a new representation is created.
An inverse transformation matrix is computed and stored in the internal
data for the view to accelerate UVN2XYZ conversions. To be able to compute
this matrix, it requires some extra information about the view orientation,
view plane distance and viewport limits.
+*/

TStatus
TelSetViewRepresentation( Tint  Wsid /* Workstation id*/,
                          Tint  Vid  /* View id */,
                          tel_view_rep vrep /* view repesentation */
                         )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  if( Vid == 0 )
    return TFailure; /* attempt to modify default view */

  /* Mise a jour de l'update_mode */
  key.ldata = TNotDone;
  TsmSetWSAttri( Wsid, WSUpdateState, &key );

  TsmGetWSAttri( Wsid, WSViews, &key );
  vptr = (tel_view_data)(key.pdata) ; /* Obtain defined view data*/

  if( !vptr )  /* no view defined yet */
  {                 /* allocate */
    //cmn_memreserve(vptr, 1, 0 );
    vptr = new TEL_VIEW_DATA();
    if( !vptr ) return TFailure;

    key.pdata = vptr;
    TsmSetWSAttri( Wsid, WSViews, &key ); /* Set defined view data*/
  }

  /* copy view definition to storage table record */
  /* NOTE: Use the matrices already calculated and stored in vrep */
  vptr->vrep = *vrep; 

#ifdef CAL_100498
  /* compute inverse transformation matrix */
  TelEvalInverseMatrix( vrep->extra.vrp, vrep->extra.vpn, vrep->extra.vup,
    vrep->extra.map.vpd, vptr->inverse_matrix );
#endif
#ifdef TRACE_MAT
  printf( "\nTelSetViewRepresentation WS : %d, view : %d\n", Wsid, Vid );
  printf( "orientation_matrix :\n" );
  pr_matrix( vptr->vrep.orientation_matrix );
  printf( "mapping_matrix :\n" );
  pr_matrix( vptr->vrep.mapping_matrix );
#endif
  return  TSuccess;
}


/*----------------------------------------------------------------------*/
TStatus
TelGetViewRepresentation( Tint  Wsid /* Workstation id*/,
                          Tint  Vid  /* View id */,
                          tel_view_rep vrep /* view representation */
                         )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  if( Vid == 0 )
  {
    matcpy( vrep->orientation_matrix, ident );
    matcpy( vrep->mapping_matrix, ident );
    vrep->clip_limit.xmin = vrep->clip_limit.ymin = 
      vrep->clip_limit.zmin = ( float )0.0;
    vrep->clip_limit.xmax = vrep->clip_limit.ymax =
      vrep->clip_limit.zmax = ( float )1.0;
    vrep->clip_xy = TOff;
    vrep->clip_back = vrep->clip_front = TOn;
    vrep->shield_indicator = TOn;
    vrep->shield_colour.rgb[0] = vrep->shield_colour.rgb[1] =
      vrep->shield_colour.rgb[2] = ( float )0.0;
    vrep->border_indicator = TOff; /* non utilise */
    vrep->border_colour.rgb[0] = vrep->border_colour.rgb[1] =
      vrep->border_colour.rgb[2] = ( float )0.0;
    vrep->active_status = TOn;
    vrep->extra.vrp[0] = vrep->extra.vrp[1] = vrep->extra.vrp[2] = ( float )0.0;
    vrep->extra.vpn[0] = ( float )0.0,
      vrep->extra.vpn[1] = ( float )0.0,
      vrep->extra.vpn[2] = ( float )1.0;
    vrep->extra.vup[0] = ( float )0.0,
      vrep->extra.vup[1] = ( float )1.0,
      vrep->extra.vup[2] = ( float )0.0;
    vrep->extra.map.vpd = ( float )0.0,
      vrep->extra.map.fpd = ( float )0.0,
      vrep->extra.map.bpd = ( float )-1.0;
    vrep->extra.map.window.xmin = vrep->extra.map.window.ymin = ( float )0.0;
    vrep->extra.map.window.xmax = vrep->extra.map.window.ymax = ( float )1.0;
    vrep->extra.map.viewport.xmin = vrep->extra.map.viewport.ymin =
      vrep->extra.map.viewport.zmin = ( float )0.0;
    vrep->extra.map.viewport.xmax = vrep->extra.map.viewport.ymax =
      vrep->extra.map.viewport.zmax = ( float )1.0;
    vrep->clipping_planes.Clear();
    return TSuccess;
  }

  if(TsmGetWSAttri( Wsid, WSViews, &key ) != TSuccess) {
    return TFailure;
  }
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
  if( !vptr ) return TFailure; /* no view defined yet */ 
  *vrep = vptr->vrep; /* copy view definition */

  return  TSuccess;
}

/*----------------------------------------------------------------------*/
/*+
Evaluate View Orientation Matrix

This routine computes the orientation matrix based on the View Reference
Point, View Plane Normal and the View Up Vector.
+*/

void
TelEvalViewOrientationMatrix( Tfloat *vrp    /* View Reference Point */,
                              Tfloat *vpn    /* View Plane Normal */,
                              Tfloat *vup    /* View Up Vector */,
                              Tfloat *asf    /* Axial Scale Factors */,
                              Tint *error_ind/* Out: Error indicator */,
                              Tmatrix3  rmat  /* Out: Orientation Matrix  */
                             )
{
  Tfloat  u[3], v[3], n[3], f;

  /* view plane normal of zero length */
  if( vecmag(vpn) == 0.0 )
  {
    *error_ind = 1;   
    return;
  }

  /* view up vector of zero length */
  if( vecmag(vup) == 0.0 )
  {
    *error_ind = 2;    
    return;
  }

  /* view up vector parallel to view plane normal */
  vecang(vup, vpn, f);
  if( f == 0.0 )
  {
    *error_ind = 3;    
    return;
  }

  *error_ind = 0;

  veccpy(n, vpn);
  veccpy(v, vup);
  vecnrm(n);  

  veccrs(u,v,n);      /* up vector cross plane normal gives U axis */
  vecnrm(u);

  veccrs(v,n,u);      /* plane normal cross U axis gives modified up vector */
  vecnrm(v); /* redundant ? */

  /* rotate to align along u, v, n */
  rmat[0][0] = ( float )u[0] * asf[0],
    rmat[0][1] = ( float )v[0] * asf[0],
    rmat[0][2] = ( float )n[0] * asf[0],
    rmat[0][3] = ( float )0.0;

  rmat[1][0] = ( float )u[1] * asf[1],
    rmat[1][1] = ( float )v[1] * asf[1],
    rmat[1][2] = ( float )n[1] * asf[1],
    rmat[1][3] = ( float )0.0;

  rmat[2][0] = ( float )u[2] * asf[2],
    rmat[2][1] = ( float )v[2] * asf[2],
    rmat[2][2] = ( float )n[2] * asf[2],
    rmat[2][3] = ( float )0.0;

  /* translate to centre at vrp */

  rmat[3][0] = - ( float ) (u[0]*vrp[0] + u[1]*vrp[1] + u[2]*vrp[2]);
  rmat[3][1] = - ( float ) (v[0]*vrp[0] + v[1]*vrp[1] + v[2]*vrp[2]);
  rmat[3][2] = - ( float ) (n[0]*vrp[0] + n[1]*vrp[1] + n[2]*vrp[2]);
  rmat[3][3] = ( float )1.0;

#ifdef PRINT
  printf("TelEvalViewOrientationMatrix \n");
  printf("view_ref_pt %f %f %f \n", vrp[0], vrp[1], vrp[2]);
  printf("view_up_vec %f %f %f \n", vup[0], vup[1], vup[2]);
  printf("view_plane_normal %f %f %f \n", vpn[0], vpn[1], vpn[2]);
  pr_matrix(rmat);
#endif

  return;
}
/*----------------------------------------------------------------------*/

void
TelEvalViewMappingMatrix( tel_view_mapping mapping /* View Mapping */,
                          Tint *error_ind          /* Out: Error Indicator */,
                          Tmatrix3 mat             /* Out: Mapping Matrix */
                         )
{
  EvalViewMappingMatrix( mapping, error_ind, mat, 0, ( float )0.0, ( float )0.0, 0, 0 );
}

/*----------------------------------------------------------------------*/

void
TelEvalViewMappingMatrixPick( tel_view_mapping mapping /* View Mapping */,
                              Tint *error_ind          /* Out: Error Indicator */,
                              Tmatrix3 mat             /* Out: Mapping Matrix */,
                              Tfloat   cx,
                              Tfloat   cy
                             )
{
  EvalViewMappingMatrix( mapping, error_ind, mat, 1, cx, cy, 0, 0 );
}

/*----------------------------------------------------------------------*/
/*+
Evalute inverse transformation matrix.

This routine computes a matrix required to convert UVN coordinates to XYZ
coordinates. It is called every time a View Representation is created or
modified, and the inverse matrix is stored along with the internal data
for the view representation.
+*/

#ifdef CAL_100498
static  void
TelEvalInverseMatrix( Tfloat *vrp    /* View Reference Point */,
                      Tfloat *vpn    /* View Plane Normal */,
                      Tfloat *vup    /* View Up Vector */,
                      Tfloat vpd     /* View Plane Distance */,
                      Tmatrix3 mat   /* Out: Inverse Transformation Matrix */
                     )
{
  Tfloat  u[3], v[3], n[3], p[3], t[3];
  Tfloat  dist = ( float )0.0;

  veccpy(n, vpn);            /* compute UVN orientation */
  veccpy(v, vup);
  veccpy(p, vrp);

  veccrs(u,v,n);
  vecnrm(u);

  veccrs(v,n,u);
  vecnrm(v);

  dist = vpd;
  veccpy(t,n);
  vecscl(dist,t);
  vecadd(t,t,p);           /* translation vector */

  veccpy(mat[0],u);
  veccpy(mat[1],v);
  veccpy(mat[2],n);
  veccpy(mat[3],t);

  mat[0][3] = mat[1][3] = mat[2][3] = ( float )0.0, mat[3][3] = ( float )1.0;

  return;
}
#endif

#ifdef CAL_100498
/*----------------------------------------------------------------------*/
/*+
Convert Coordinates utility routine

This routine converts coordinates from UVN space to XYZ space and
vice versa. For UVN2XYZ, the inverse transformation matrix is used and for
XYZ2UVN, the orientation matrix is used.
+*/

TStatus
TelConvertCoord( Tint  Wsid        /* Workstation id */,
                 Tint  Vid         /* View id */,
                 TelCoordType type /* Conversion type (UVN2XYZ or XYZ2UVN) */,
                 Tfloat *uvn       /* UVN coords if type is UVN2XYZ else Out */,
                 Tfloat *xyz       /* XYZ coords if type is XYZ2UVN else Out */
                )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;
  register  Tint  i;

  Tfloat  pt[4], tpt[4];

  if( Vid == 0 )
  {                            /* default view */
    switch( type )
    {
    case UVN2XYZ:
      veccpy(xyz,uvn);
      break;
    case XYZ2UVN:
      veccpy(uvn,xyz);
      break;
    }
    return TSuccess;
  }

  TsmGetWSAttri( Wsid, WSViews, &key );
  vptr = key.pdata ; /* Obtain defined view data*/
  if( !vptr ) return TFailure; /* no view defined yet */ 

  switch( type )
  {
  case UVN2XYZ:
    veccpy(pt,uvn);
    pt[3] = ( float )1.0;
    TelTranpt3( tpt, pt, vptr->inverse_matrix );
    veccpy(xyz,tpt);
    break;

  case XYZ2UVN:
    veccpy(pt,xyz);
    pt[3] = ( float )1.0;
    TelTranpt3( tpt, pt, vptr->vrep.orientation_matrix );
    veccpy(uvn,tpt);
    break;
  }

  return TSuccess;
}
#endif

/*----------------------------------------------------------------------*/
/*+
Print Matrix

Debug tool
+*/

#ifdef DEB
void
pr_matrix( Tmatrix3 mat )
{
  printf( "%f %f %f %f\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3] );
  printf( "%f %f %f %f\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3] );
  printf( "%f %f %f %f\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3] );
  printf( "%f %f %f %f\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3] );
  printf( "\n" );
  return;
}
#endif

/*----------------------------------------------------------------------*/
/*+
Set View Index

This routine activates the view representation which has been defined
previously.
+*/

TStatus
TelSetViewIndex( Tint  Wsid /* Workstation id */,
                 Tint  Vid  /* View id */ )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  if( Vid == 0 )
  {                    /* default view */
    GLint mm;
    glGetIntegerv(GL_MATRIX_MODE, &mm);
#ifdef TRACE_MATRIX
    printf("OpenGl_view.c::TelSetViewIndex::glMatrixMode(GL_PROJECTION) \n"); 
#endif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( -0.5,  0.5,  -0.5,  0.5,  -0.5,  0.5 );
    glMatrixMode(mm);
    return TSuccess;
  }

  TsmGetWSAttri( Wsid, WSViews, &key );
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
  if( !vptr ) return TFailure; /* no view defined yet */ 

 
#ifdef TRACE_MATRIX
  printf("OpenGl_view.c::TelSetViewIndex::glMatrixMode(GL_MODELVIEW) \n"); 
#endif
  if(vptr->vrep.extra.scaleFactors[0] != 1. || 
    vptr->vrep.extra.scaleFactors[1] != 1. ||
    vptr->vrep.extra.scaleFactors[2] != 1.)
    glEnable(GL_NORMALIZE);           /* if the view is scaled normal vectors are scaled to unit length for correct displaying of shaded objects*/
  else if(glIsEnabled(GL_NORMALIZE))  
    glDisable(GL_NORMALIZE);
  glMatrixMode(GL_MODELVIEW);
  set_clipplanes( &(vptr->vrep) );
  

  glLoadMatrixf((GLfloat *) vptr->vrep.orientation_matrix );
  set_userclipplanes( &(vptr->vrep) ); /* set clipping planes defined by user */

#ifdef ENVTEX
  /*
  * Ajout translation necessaire pour l'environnement mapping
  */
  {
    GLfloat dep;  

    TsmGetWSAttri(Wsid, WSSurfaceDetail, &key);
    /* TOD_ENVIRONMENT ou TOD_ALL */
    if (key.ldata == 1 || key.ldata == 2)
    {
      /*          dep = vptr->vrep.extra.map.fpd * 0.5F; */
      dep = (vptr->vrep.extra.map.fpd + vptr->vrep.extra.map.bpd) * 0.5F; /* OCC280: FitAll work incorrect for perspective view if the SurfaceDetail mode is V3d_TEX_ENVIRONMENT or V3d_TEX_ALL */
      glTranslatef(-dep*vptr->vrep.extra.vpn[0],
        -dep*vptr->vrep.extra.vpn[1],
        -dep*vptr->vrep.extra.vpn[2]); 
#ifdef PRINT
      printf("glTranslatef: %f %f %f \n", 
        -dep*vptr->vrep.extra.vpn[0],
        -dep*vptr->vrep.extra.vpn[1],
        -dep*vptr->vrep.extra.vpn[2]);
#endif
    }
  }
#endif

#ifdef TRACE_MATRIX
  printf("OpenGl_view.c::TelSetViewIndex::glMatrixMode(GL_PROJECTION) \n"); 
#endif
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((GLfloat *) vptr->vrep.mapping_matrix );

#ifdef TRACE_MAT
  printf( "\nTelSetViewIndex WS : %d, view : %d", Wsid, Vid );
  printf( "orientation_matrix :\n" );
  pr_matrix( vptr->vrep.orientation_matrix );
  printf( "mapping_matrix :\n" );
  pr_matrix( vptr->vrep.mapping_matrix );
#endif

  return vptr->vrep.active_status == TOn ? TSuccess : TFailure;
}


/*----------------------------------------------------------------------*/
/*+
Set View Projection

This routine activates the Projection matrix for a previously defined
view representation. It is meant to be used exclusively to restore the
Projection transformation at the end of an ExecuteStructure element
traversal. The restoration of the Viewing matrix is done by a GL popmatrix
call.
+*/

TStatus
TelSetViewProjection( Tint  Wsid /* Workstation id */,
                      Tint  Vid  /* View id */ )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  if( Vid == 0 )
  {                    /* default view */
    GLint mm;
    glGetIntegerv(GL_MATRIX_MODE, &mm);
#ifdef TRACE_MATRIX
    printf("OpenGl_view.c::TelSetViewProjection::glMatrixMode(GL_PROJECTION) \n"); 
#endif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( 0,  1,  0,  1,  0,  1 );
    glMatrixMode(mm);


    return TSuccess;
  }

  TsmGetWSAttri( Wsid, WSViews, &key );
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
  if( !vptr ) return TFailure; /* no view defined yet */ 

  set_clipplanes( &(vptr->vrep) );
  set_userclipplanes( &(vptr->vrep) );

#ifdef TRACE_MATRIX
  printf("OpenGl_view.c::TelSetViewProjection::glMatrixMode(GL_PROJECTION) \n"); 
#endif
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf((GLfloat *) vptr->vrep.mapping_matrix );
 
  return vptr->vrep.active_status == TOn ? TSuccess : TFailure;
}

/*----------------------------------------------------------------------*/
/*+
Clear all active views in a workstation

This routine clears all the viewports, draws the viewport borders and
sets the viewport background if the corresponding indicators are on.
It also clears view 0 (default view), if the workstation state demands it.
+*/

void
TelClearViews( Tint  Wsid /* Workstation id */ )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  TelResetMaterial();

  TsmGetWSAttri( Wsid, WSViews, &key );
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
  if( !vptr ) return; /* no view defined yet */ 

  if( vptr->vrep.active_status == TOn )  /* for each active view only */
  {
    if( vptr->vrep.shield_indicator == TOn ) TelClearBackground( Wsid );
  }

  return;
}

/*----------------------------------------------------------------------*/
/* Function for printing view information */

TStatus
TelPrintViewRepresentation( Tint  Wsid /* Workstation id*/,
                            Tint  Vid  /* View id */
                           )
{
  CMN_KEY_DATA    key;
  tel_view_data   vptr;

  printf( "\n\tPrint : Workstation %d View %d", Wsid, Vid );

  if( Vid == 0 )
    return TSuccess;

  TsmGetWSAttri( Wsid, WSViews, &key );
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/
  if( !vptr ) return TFailure; /* no view defined yet */ 

  /* Print the information */
  printf( "\n\tshield indicator : %s",
    vptr->vrep.shield_indicator == TOn ? "ON" : "OFF" );
  printf( "\n\tshield_colour r = %f, g = %f, b = %f",
    vptr->vrep.shield_colour.rgb[0],
    vptr->vrep.shield_colour.rgb[1],
    vptr->vrep.shield_colour.rgb[2] );
  printf( "\n\tactive_status : %s",
    vptr->vrep.active_status == TOn ? "ON":"OFF");
  printf( "\n\tVRP : %f %f %f", vptr->vrep.extra.vrp[0],
    vptr->vrep.extra.vrp[1],
    vptr->vrep.extra.vrp[2] );
  printf( "\n\tVPN : %f %f %f", vptr->vrep.extra.vpn[0],
    vptr->vrep.extra.vpn[1],
    vptr->vrep.extra.vpn[2] );
  printf( "\n\tVUP : %f %f %f", vptr->vrep.extra.vup[0],
    vptr->vrep.extra.vup[1],
    vptr->vrep.extra.vup[2] );
  printf( "\n\tPRP : %f %f %f", vptr->vrep.extra.map.prp[0],
    vptr->vrep.extra.map.prp[1],
    vptr->vrep.extra.map.prp[2] );
  printf( "\n\tWindow Limits xmin xmax ymin ymax :\n\t\t%f %f %f %f",
    vptr->vrep.extra.map.window.xmin,
    vptr->vrep.extra.map.window.xmax,
    vptr->vrep.extra.map.window.ymin,
    vptr->vrep.extra.map.window.ymax );
  printf( "\n\tViewport Limits xmin xmax ymin ymax zmin zmax :\n\t\t%f %f %f %f %f %f", vptr->vrep.extra.map.viewport.xmin,
    vptr->vrep.extra.map.viewport.xmax,
    vptr->vrep.extra.map.viewport.ymin,
    vptr->vrep.extra.map.viewport.ymax,
    vptr->vrep.extra.map.viewport.zmin,
    vptr->vrep.extra.map.viewport.zmax );
  printf( "\n\tProjection type : %s",
    vptr->vrep.extra.map.proj == TelParallel
    ? "PARALLEL" : "PERSPECTIVE" );
  printf( "\n\tVPD FPD BPD : %f %f %f\n", vptr->vrep.extra.map.vpd,
    vptr->vrep.extra.map.fpd,
    vptr->vrep.extra.map.bpd );
  return  TSuccess;
}


/*----------------------------------------------------------------------*/

#define FRONT_CLIPPING_PLANE (GL_CLIP_PLANE0 + 0)
#define BACK_CLIPPING_PLANE  (GL_CLIP_PLANE0 + 1)
/*

While drawing after a clipplane has been defined and enabled, each vertex
is transformed to eye-coordinates, where it is dotted with the transformed
clipping plane equation.  Eye-coordinate vertexes whose dot product with
the transformed clipping plane equation is positive or zero are in, and
require no clipping.  Those eye-coordinate vertexes whose dot product is
negative are clipped.  Because clipplane clipping is done in eye-
coordinates, changes to the projection matrix have no effect on its
operation.

A point and a normal are converted to a plane equation in the following
manner:

point = [Px,Py,Pz]

normal = |Nx|
|Ny|
|Nz|

plane equation = |A|
|B|
|C|
|D|
A = Nx
B = Ny
C = Nz
D = -[Px,Py,Pz] dot |Nx|
|Ny|
|Nz|

*/

/*----------------------------------------------------------------------*/

static void
set_clipplanes( tel_view_rep vrep )
{
  GLdouble arr[4];
  Tfloat front, back;
  GLfloat mat[4][4];
#ifdef TRACE_MATRIX
  printf("OpenGl_view.c::set_clipplanes::glMatrixMode(GL_MODELVIEW) \n"); 
#endif
  glMatrixMode( GL_MODELVIEW );
  glGetFloatv( GL_MODELVIEW_MATRIX,(GLfloat *) mat );
  glLoadIdentity();

  if( vrep->clip_limit.zmin < 0.0 ||
    vrep->clip_limit.zmin > 1.0 ||
    vrep->clip_back == TOff )
    glDisable( BACK_CLIPPING_PLANE );
  else
  {
    /* cf Opengl_togl_cliplimit */
    back = (vrep->extra.map.fpd - vrep->extra.map.bpd) *
      vrep->clip_limit.zmin + vrep->extra.map.bpd;
    arr[0] = 0.0;  /* Nx */
    arr[1] = 0.0;  /* Ny */
    arr[2] = 1.0;  /* Nz */
    arr[3] = -( (GLdouble) (back) * arr[2] ); /* P dot N */
    glClipPlane( BACK_CLIPPING_PLANE, arr );
    glEnable( BACK_CLIPPING_PLANE );
  }

  if( vrep->clip_limit.zmax < 0.0 ||
    vrep->clip_limit.zmax > 1.0 ||
    vrep->clip_front == TOff )
    glDisable( FRONT_CLIPPING_PLANE );
  else
  {
    /* cf Opengl_togl_cliplimit */
    front = (vrep->extra.map.fpd - vrep->extra.map.bpd) *
      vrep->clip_limit.zmax + vrep->extra.map.bpd;
    arr[0] = 0.0;  /* Nx */
    arr[1] = 0.0;  /* Ny */
    arr[2] = -1.0; /* Nz */
    arr[3] = -( (GLdouble) (front) * arr[2] ); /* P dot N */
    glClipPlane( FRONT_CLIPPING_PLANE, arr );
    glEnable( FRONT_CLIPPING_PLANE );
  }

  glLoadMatrixf( (GLfloat *) mat );
}

static void
set_userclipplanes( tel_view_rep vrep )
{
  int j,planeid;
  CALL_DEF_PLANE* plane;

#ifdef TRACE_MATRIX
  printf("OpenGl_view.c::set_userclipplanes::glMatrixMode(GL_MODELVIEW) \n"); 
#endif

  
  NCollection_List<CALL_DEF_PLANE>::Iterator planeIter(vrep->clipping_planes);
  
  // go through all of planes in the list & preview them
  for( j=0 ; planeIter.More(); planeIter.Next(), j++ ) 
  {
    plane = const_cast<CALL_DEF_PLANE*>(&planeIter.Value());
    
    if( plane->PlaneId > 0 ) 
    {
      planeid = GL_CLIP_PLANE2 + j;

      if( plane->Active ) 
      {
	  // Activates new clip planes
	  GLdouble equation[4];
	  equation[0] = plane->CoefA;
	  equation[1] = plane->CoefB;
	  equation[2] = plane->CoefC;
	  equation[3] = plane->CoefD;
	  
          glClipPlane( planeid , equation );     
          if( !glIsEnabled( planeid ) ) glEnable( planeid );     
      } 
      else 
      {
	if( glIsEnabled( planeid ) ) glDisable( planeid );     
      }
    }
  }   //for( ; planeIter.More(); planeIter.Next() ) 

  // Disable the remainder Clip planes

  for( j=vrep->clipping_planes.Size(); j < call_facilities_list.MaxPlanes; j++ )
  {
    planeid = GL_CLIP_PLANE2 + j; 

    if( glIsEnabled( planeid ) ) 
      glDisable( planeid );
   }

  
}

/*----------------------------------------------------------------------*/

TStatus
Tel2Dto3D( Tint ws, Tint vid, Tint x, Tint y,
          Tfloat *x1, Tfloat *y1, Tfloat *z1,
          Tfloat *x2, Tfloat *y2, Tfloat *z2 )
          /* x is from bottom */
          /* y is from top */
{
  CMN_KEY_DATA key;
  TEL_VIEW_REP vrep;
  Tint         w, h;
  GLint        viewp[4];
  GLdouble xx1, yy1, zz1, xx2, yy2, zz2;

  TsmGetWSAttri( ws, WSWidth, &key );
  w = key.ldata;
  TsmGetWSAttri( ws, WSHeight, &key );
  h = key.ldata;
  y = key.ldata - y;

  /* FSXXX */
  printf("WARNING: Tel2Dto3D non verifie\n");

  TelGetViewRepresentation( ws, vid, &vrep );

  viewp[0] = 0;     viewp[2] = w;
  viewp[1] = 0;     viewp[3] = h;


  /* OGLXXX XXX I think this is backwards */
  gluProject((GLdouble) x, (GLdouble) y, 0.0,
    (GLdouble *) vrep.orientation_matrix,
    (GLdouble *) vrep.mapping_matrix,
    viewp, &xx1, &yy1, &zz1);
  gluProject((GLdouble) x, (GLdouble) y, 1.0,
    (GLdouble *) vrep.orientation_matrix,
    (GLdouble *) vrep.mapping_matrix,
    viewp, &xx2, &yy2, &zz2);

  *x1 = (float) xx1; *y1 =  (float) yy1; *z1 =  (float) zz1;
  *x2 = (float) xx2; *y2 =  (float) yy2; *z2 =  (float) zz2;

  return TSuccess;

}

/*----------------------------------------------------------------------*/

TStatus
TelDeleteViewsForWS( Tint wsid )
{
  CMN_KEY_DATA key;
  tel_view_data vptr;

  TsmGetWSAttri( wsid, WSViews, &key );
  vptr = (tel_view_data)key.pdata ; /* Obtain defined view data*/

  if( !vptr ) return TSuccess;
  delete vptr;

  return TSuccess;
}

/*----------------------------------------------------------------------*/

void
TelClearBackground( Tint  Wsid /* Workstation id */ )
{
  CMN_KEY_DATA key;
  Tfloat *rgb;
  Tint   zbuffer;
#ifdef BUC61044
  Tint   dTest;
#endif
#ifdef OCC1188
  tsm_bg_texture texture;
#endif  
  tsm_bg_gradient gradient;


  TsmGetWSAttri (Wsid, WSBackground, &key);
  rgb = (Tfloat *)key.pdata;

#ifdef OCC1188
  TsmGetWSAttri (Wsid, WSBgTexture, &key);
  texture = (tsm_bg_texture)key.pdata;
#endif
  TsmGetWSAttri (Wsid, WSZbuffer, &key);
  zbuffer = key.ldata;

  TsmGetWSAttri (Wsid, WSBgGradient, &key);
  gradient = (tsm_bg_gradient)key.pdata;
  
  /* GL_DITHER on/off pour le background */
  if (TxglGetBackDither ())
    glEnable (GL_DITHER);
  else
    glDisable (GL_DITHER);

  if (zbuffer)
  {
#ifdef BUC60920 
    glDepthFunc(GL_LEQUAL);
#else
    glDepthFunc(GL_LESS);
#endif
    glDepthMask(GL_TRUE);

#ifdef BUC61044
    /* getting depth testing flag */
    TsmGetWSAttri( Wsid, WSDepthTest, &key );
    dTest = key.ldata;
    /* SAV checking if depth test was depricated somewhere outside */
    if ( dTest == TOn ) 
      glEnable(GL_DEPTH_TEST);
    else
      glDisable(GL_DEPTH_TEST);
#else
    glEnable(GL_DEPTH_TEST);
#endif

    glClearDepth(1.0);
    glClearColor (rgb[0], rgb[1], rgb[2], ( float )0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
    glClearColor (rgb[0], rgb[1], rgb[2], ( float )0.0);
    glClear (GL_COLOR_BUFFER_BIT);
  }
#ifdef OCC1188
  glPushAttrib( GL_ENABLE_BIT | GL_TEXTURE_BIT );
  /* drawing bg image if any */
  if ( texture->texId != 0 ) {
    GLint width, height; /* window dimensions*/
    GLfloat x_center, y_center; /* window center */
    GLfloat x_offset, y_offset; /* half of the texture size */
    GLfloat texX_range = 1.0;
    /* texture <s> coordinate */
    GLfloat texY_range = 1.0; /* texture <t> coordinate */

    TsmGetWSAttri( Wsid, WSWidth, &key );
    width = key.ldata;
    x_center = (GLfloat)( width / 2. );
    TsmGetWSAttri( Wsid, WSHeight, &key );
    height = key.ldata;
    y_center = (GLfloat)( height / 2. );

    x_offset = (GLfloat)( texture->width / 2. ); /* style = center */
    y_offset = (GLfloat)( texture->height / 2. ); /* style = center */
    if ( texture->style != 0 ) { /* stretch or tile */
      x_offset = x_center;
      y_offset = y_center;
      if ( texture->style == 1 ) { /* tile */
        texX_range = (float)( width / texture->width );
        if ( texX_range < 1.0 )
          texX_range = 1.0;
        texY_range = (float)( height / texture->height );
        if ( texY_range < 1.0 )
          texY_range = 1.0;
      }
    }
    /* drawing background texture */
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0, (GLdouble)width, 0.0, (GLdouble)height );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture->texId );
    glDisable( GL_DEPTH_TEST );

    glDisable( GL_BLEND );
    glColor3f( rgb[0], rgb[1], rgb[2] );
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBegin( GL_QUADS );
    glTexCoord2f(0.0f, 0.0f);glVertex2f( x_center - x_offset, y_center - y_offset );
    glTexCoord2f(texX_range, 0.0f);glVertex2f( x_center + x_offset, y_center - y_offset );
    glTexCoord2f(texX_range, texY_range);glVertex2f( x_center + x_offset, y_center + y_offset );
    glTexCoord2f(0.0f, texY_range);glVertex2f( x_center - x_offset, y_center + y_offset );
    glEnd();
    if ( zbuffer )
      glEnable( GL_DEPTH_TEST );
    glDisable( GL_TEXTURE_2D );
    glPopMatrix(); /*modelview*/
    glMatrixMode( GL_PROJECTION );
    glPopMatrix(); /*projection*/
    glMatrixMode( GL_MODELVIEW );
  }  
  else if( gradient->type > 0 )
  {

    Tfloat* corner1 = 0;/* -1,-1*/
    Tfloat* corner2 = 0;/*  1,-1*/
    Tfloat* corner3 = 0;/*  1, 1*/
    Tfloat* corner4 = 0;/* -1, 1*/
    Tfloat* dcorner1 = (Tfloat*)malloc(3*sizeof(Tfloat));
    Tfloat* dcorner2 = (Tfloat*)malloc(3*sizeof(Tfloat));

    int upset[3] = {0} ;

    switch( gradient->type )
    {
      case 1:
       corner1 = gradient->color2.rgb;
       corner2 = gradient->color2.rgb;
       corner3 = gradient->color1.rgb;
       corner4 = gradient->color1.rgb;
       break;
      case 2:    
       corner1 = gradient->color2.rgb;
       corner2 = gradient->color1.rgb;
       corner3 = gradient->color1.rgb;
       corner4 = gradient->color2.rgb;
       break;
      case 3:
       corner2 = gradient->color2.rgb;
       corner4 = gradient->color1.rgb;        
       dcorner1 [0] = dcorner2[0] = (corner2[0] + corner4[0]) / 2.0;
       dcorner1 [1] = dcorner2[1] = (corner2[1] + corner4[1]) / 2.0;
       dcorner1 [2] = dcorner2[2] = (corner2[2] + corner4[2]) / 2.0;   
       corner1 = dcorner1;
       corner3 = dcorner2;  
       break;
      case 4:  
       corner1 = gradient->color2.rgb;  
       corner3 = gradient->color1.rgb;       
       dcorner1 [0] = dcorner2[0] = (corner1[0] + corner3[0]) / 2.0;
       dcorner1 [1] = dcorner2[1] = (corner1[1] + corner3[1]) / 2.0;
       dcorner1 [2] = dcorner2[2] = (corner1[2] + corner3[2]) / 2.0;   
       corner2 = dcorner1;
       corner4 = dcorner2;  
       break;
      case 5:
       corner1 = gradient->color1.rgb;
       corner2 = gradient->color2.rgb;
       corner3 = gradient->color2.rgb;
       corner4 = gradient->color2.rgb;
       break;
      case 6:
       corner1 = gradient->color2.rgb;
       corner2 = gradient->color1.rgb;
       corner3 = gradient->color2.rgb;
       corner4 = gradient->color2.rgb;
       break;
      case 7:
       corner1 = gradient->color2.rgb;
       corner2 = gradient->color2.rgb;
       corner3 = gradient->color1.rgb;
       corner4 = gradient->color2.rgb;
       break;
      case 8:
       corner1 = gradient->color2.rgb;
       corner2 = gradient->color2.rgb;
       corner3 = gradient->color2.rgb;
       corner4 = gradient->color1.rgb;
       break;
      default:
       printf("gradient background type not right\n");

    }    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    if ( glIsEnabled( GL_DEPTH_TEST) )
    {
      upset[0] = 1;
      glDisable( GL_DEPTH_TEST );
    }
    if ( glIsEnabled( GL_LIGHTING ) )
    {
      upset[1] = 1;
      glDisable( GL_LIGHTING );
    }
    if ( !glIsEnabled( GL_SMOOTH ) )
    {
      upset[2] = 1; 
      glShadeModel( GL_SMOOTH ) ;
    }    

    if( gradient->type <= 5 || gradient->type == 7 )
    {
      glBegin(GL_TRIANGLES);
        glColor3f(corner1[0],corner1[1],corner1[2]);
        glVertex2f(-1.,-1.);
        glColor3f(corner2[0],corner2[1],corner2[2]);
        glVertex2f( 1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]);
        glVertex2f( 1., 1.);          
      glEnd(); 
      glBegin(GL_TRIANGLES);
        glColor3f(corner1[0],corner1[1],corner1[2]);
        glVertex2f(-1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]);
        glVertex2f( 1., 1.);
        glColor3f(corner4[0],corner4[1],corner4[2]);
        glVertex2f(-1., 1.);          
      glEnd();
    }         
    else if( gradient->type == 6 || gradient->type == 8 )
    {
      glBegin(GL_TRIANGLES);
        glColor3f(corner1[0],corner1[1],corner1[2]);
        glVertex2f(-1.,-1.);
        glColor3f(corner2[0],corner2[1],corner2[2]);
        glVertex2f( 1.,-1.);
        glColor3f(corner4[0],corner4[1],corner4[2]);
        glVertex2f(-1., 1.);          
      glEnd(); 
      glBegin(GL_TRIANGLES);
        glColor3f(corner2[0],corner2[1],corner2[2]);
        glVertex2f( 1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]);
        glVertex2f( 1., 1.);
        glColor3f(corner4[0],corner4[1],corner4[2]);
        glVertex2f(-1., 1.);          
      glEnd(); 

    }    

    if ( upset[0] )
      glEnable( GL_DEPTH_TEST );   

    if ( upset[1] )
      glEnable( GL_LIGHTING ); 

    if ( upset[2] )
      glShadeModel( GL_FLAT );      
 
    if ( zbuffer )
      glEnable( GL_DEPTH_TEST );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    free(dcorner1);
    free(dcorner2);
    dcorner1 = 0;
    dcorner2 = 0;

  } 

#endif /* OCC1188*/

  glPopAttrib();

  /* GL_DITHER on/off pour le trace */
  if (TxglGetDither ())
    glEnable (GL_DITHER);
  else
    glDisable (GL_DITHER);


}
/*----------------------------------------------------------------------*/
