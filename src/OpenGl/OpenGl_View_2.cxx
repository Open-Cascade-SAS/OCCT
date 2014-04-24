// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <stdio.h>
#include <stdlib.h>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_tgl_funcs.hxx>

#include <Image_AlienPixMap.hxx>
#include <Visual3d_Layer.hxx>

#include <NCollection_Mat4.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Display.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>

#define EPSI 0.0001

namespace
{

  static const GLfloat THE_DEFAULT_AMBIENT[4]    = { 0.0f, 0.0f, 0.0f, 1.0f };
  static const GLfloat THE_DEFAULT_SPOT_DIR[3]   = { 0.0f, 0.0f, -1.0f };
  static const GLfloat THE_DEFAULT_SPOT_EXPONENT = 0.0f;
  static const GLfloat THE_DEFAULT_SPOT_CUTOFF   = 180.0f;

};

extern void InitLayerProp (const int theListId); //szvgl: defined in OpenGl_GraphicDriver_Layer.cxx

/*----------------------------------------------------------------------*/

struct OPENGL_CLIP_PLANE
{
  GLboolean isEnabled;
  GLdouble Equation[4];
  DEFINE_STANDARD_ALLOC
};

/*----------------------------------------------------------------------*/
/*
* Fonctions privees
*/

/*-----------------------------------------------------------------*/
/*
*  Set des lumieres
*/
static void bind_light (const OpenGl_Light& theLight,
                        GLenum&             theLightGlId,
                        Graphic3d_Vec4&     theAmbientColor)
{
  // Only 8 lights in OpenGL...
  if (theLightGlId > GL_LIGHT7)
  {
    return;
  }

  if (theLight.Type == Visual3d_TOLS_AMBIENT)
  {
    // add RGBA intensity of the ambient light
    theAmbientColor += theLight.Color;
    return;
  }

  // the light is a headlight?
  GLint aMatrixModeOld = 0;
  if (theLight.IsHeadlight)
  {
    glGetIntegerv (GL_MATRIX_MODE, &aMatrixModeOld);
    glMatrixMode  (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  }

  // setup light type
  switch (theLight.Type)
  {
    case Visual3d_TOLS_DIRECTIONAL:
    {
      // if the last parameter of GL_POSITION, is zero, the corresponding light source is a Directional one
      const OpenGl_Vec4 anInfDir = -theLight.Direction;

      // to create a realistic effect,  set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE.
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              anInfDir.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
      break;
    }
    case Visual3d_TOLS_POSITIONAL:
    {
      // to create a realistic effect, set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              theLight.Position.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
      glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
      glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
      glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0);
      break;
    }
    case Visual3d_TOLS_SPOT:
    {
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              theLight.Position.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        theLight.Direction.GetData());
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         theLight.Concentration() * 128.0f);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,          (theLight.Angle() * 180.0f) / GLfloat(M_PI));
      glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
      glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
      glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0f);
      break;
    }
  }

  // restore matrix in case of headlight
  if (theLight.IsHeadlight)
  {
    glPopMatrix();
    glMatrixMode (aMatrixModeOld);
  }

  glEnable (theLightGlId++);
}

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

static void call_util_apply_trans2( float ix, float iy, float iz, matrix3 mat,
                                   float *ox, float *oy, float *oz );
static void call_util_mat_mul( matrix3 mat_a, matrix3 mat_b, matrix3 mat_c);

/*----------------------------------------------------------------------*/
/*
* Fonctions externes
*/

/*
*  Evaluates orientation matrix.
*/
/* OCC18942: obsolete in OCCT6.3, might be removed in further versions! */
void call_func_eval_ori_matrix3 (const point3* vrp,        // view reference point
                                 const vec3*   vpn,        // view plane normal
                                 const vec3*   vup,        // view up vector
                                 int*          err_ind,
                                 float         mout[4][4]) // OUT view orientation matrix
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

void OpenGl_View::DrawBackground (const Handle(OpenGl_Workspace) &AWorkspace)
{
  /////////////////////////////////////////////////////////////////////////////
  // Step 1: Prepare for redraw

  // Render background
  if ( (AWorkspace->NamedStatus & OPENGL_NS_WHITEBACK) == 0 &&
    ( myBgTexture.TexId != 0 || myBgGradient.type != Aspect_GFM_NONE ) )
  {
    const Standard_Integer aViewWidth = AWorkspace->Width();
    const Standard_Integer aViewHeight = AWorkspace->Height();

    glPushAttrib( GL_ENABLE_BIT | GL_TEXTURE_BIT );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    if ( glIsEnabled( GL_DEPTH_TEST ) )
      glDisable( GL_DEPTH_TEST ); //push GL_ENABLE_BIT

    // drawing bg gradient if:
    // - gradient fill type is not Aspect_GFM_NONE and
    // - either background texture is no specified or it is drawn in Aspect_FM_CENTERED mode
    if ( ( myBgGradient.type != Aspect_GFM_NONE ) &&
      ( myBgTexture.TexId == 0 || myBgTexture.Style == Aspect_FM_CENTERED ||
      myBgTexture.Style == Aspect_FM_NONE ) )
    {
      Tfloat* corner1 = 0;/* -1,-1*/
      Tfloat* corner2 = 0;/*  1,-1*/
      Tfloat* corner3 = 0;/*  1, 1*/
      Tfloat* corner4 = 0;/* -1, 1*/
      Tfloat dcorner1[3];
      Tfloat dcorner2[3];

      switch( myBgGradient.type )
      {
      case Aspect_GFM_HOR:
        corner1 = myBgGradient.color1.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color1.rgb;
        break;
      case Aspect_GFM_VER:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color1.rgb;
        corner4 = myBgGradient.color1.rgb;
        break;
      case Aspect_GFM_DIAG1:
        corner2 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color1.rgb;
        dcorner1 [0] = dcorner2[0] = 0.5F * (corner2[0] + corner4[0]);
        dcorner1 [1] = dcorner2[1] = 0.5F * (corner2[1] + corner4[1]);
        dcorner1 [2] = dcorner2[2] = 0.5F * (corner2[2] + corner4[2]);
        corner1 = dcorner1;
        corner3 = dcorner2;
        break;
      case Aspect_GFM_DIAG2:
        corner1 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color1.rgb;
        dcorner1 [0] = dcorner2[0] = 0.5F * (corner1[0] + corner3[0]);
        dcorner1 [1] = dcorner2[1] = 0.5F * (corner1[1] + corner3[1]);
        dcorner1 [2] = dcorner2[2] = 0.5F * (corner1[2] + corner3[2]);
        corner2 = dcorner1;
        corner4 = dcorner2;
        break;
      case Aspect_GFM_CORNER1:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color1.rgb;
        break;
      case Aspect_GFM_CORNER2:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color1.rgb;
        corner4 = myBgGradient.color2.rgb;
        break;
      case Aspect_GFM_CORNER3:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color1.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color2.rgb;
        break;
      case Aspect_GFM_CORNER4:
        corner1 = myBgGradient.color1.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color2.rgb;
        break;
      default:
        //printf("gradient background type not right\n");
        break;
      }

      // Save GL parameters
      glDisable( GL_LIGHTING ); //push GL_ENABLE_BIT

      GLint curSM;
      glGetIntegerv( GL_SHADE_MODEL, &curSM );
      if ( curSM != GL_SMOOTH )
        glShadeModel( GL_SMOOTH ); //push GL_LIGHTING_BIT

      glBegin(GL_TRIANGLE_FAN);
      if( myBgGradient.type != Aspect_GFM_CORNER1 && myBgGradient.type != Aspect_GFM_CORNER3 )
      {
        glColor3f(corner1[0],corner1[1],corner1[2]); glVertex2f(-1.,-1.);
        glColor3f(corner2[0],corner2[1],corner2[2]); glVertex2f( 1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]); glVertex2f( 1., 1.);
        glColor3f(corner4[0],corner4[1],corner4[2]); glVertex2f(-1., 1.);
      }
      else //if ( myBgGradient.type == Aspect_GFM_CORNER1 || myBgGradient.type == Aspect_GFM_CORNER3 )
      {
        glColor3f(corner2[0],corner2[1],corner2[2]); glVertex2f( 1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]); glVertex2f( 1., 1.);
        glColor3f(corner4[0],corner4[1],corner4[2]); glVertex2f(-1., 1.);
        glColor3f(corner1[0],corner1[1],corner1[2]); glVertex2f(-1.,-1.);
      }
      glEnd();

      // Restore GL parameters
      if ( curSM != GL_SMOOTH )
        glShadeModel( curSM );
    }
    // drawing bg image if:
    // - it is defined and
    // - fill type is not Aspect_FM_NONE
    if ( myBgTexture.TexId != 0 && myBgTexture.Style != Aspect_FM_NONE )
    {
      GLfloat texX_range = 1.F; // texture <s> coordinate
      GLfloat texY_range = 1.F; // texture <t> coordinate

      // Set up for stretching or tiling
      GLfloat x_offset, y_offset;
      if ( myBgTexture.Style == Aspect_FM_CENTERED )
      {
        x_offset = (GLfloat)myBgTexture.Width / (GLfloat)aViewWidth;
        y_offset = (GLfloat)myBgTexture.Height / (GLfloat)aViewHeight;
      }
      else
      {
        x_offset = 1.F;
        y_offset = 1.F;
        if ( myBgTexture.Style == Aspect_FM_TILED )
        {
          texX_range = (GLfloat)aViewWidth / (GLfloat)myBgTexture.Width;
          texY_range = (GLfloat)aViewHeight / (GLfloat)myBgTexture.Height;
        }
      }

      // OCCT issue 0023000: Improve the way the gradient and textured
      // background is managed in 3d viewer (note 0020339)
      // Setting this coefficient to -1.F allows to tile textures relatively
      // to the top-left corner of the view (value 1.F corresponds to the
      // initial behaviour - tiling from the bottom-left corner)
      GLfloat aCoef = -1.F;

      glEnable( GL_TEXTURE_2D ); //push GL_ENABLE_BIT
      glBindTexture( GL_TEXTURE_2D, myBgTexture.TexId ); //push GL_TEXTURE_BIT

      glDisable( GL_BLEND ); //push GL_ENABLE_BIT

      glColor3fv( AWorkspace->BackgroundColor().rgb );
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //push GL_TEXTURE_BIT

      // Note that texture is mapped using GL_REPEAT wrapping mode so integer part
      // is simply ignored, and negative multiplier is here for convenience only
      // and does not result e.g. in texture mirroring
      glBegin( GL_QUADS );
      glTexCoord2f(0.F, 0.F); glVertex2f( -x_offset, -aCoef * y_offset );
      glTexCoord2f(texX_range, 0.F); glVertex2f( x_offset, -aCoef * y_offset );
      glTexCoord2f(texX_range, aCoef * texY_range); glVertex2f( x_offset, aCoef * y_offset );
      glTexCoord2f(0.F, aCoef * texY_range); glVertex2f( -x_offset, aCoef * y_offset );
      glEnd();
    }

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glPopAttrib(); //GL_ENABLE_BIT | GL_TEXTURE_BIT

    if ( AWorkspace->UseZBuffer() )
      glEnable( GL_DEPTH_TEST );

    /* GL_DITHER on/off pour le trace */
    if (AWorkspace->Dither())
      glEnable (GL_DITHER);
    else
      glDisable (GL_DITHER);
  }
}

/*----------------------------------------------------------------------*/

//call_func_redraw_all_structs_proc
void OpenGl_View::Render (const Handle(OpenGl_PrinterContext)& thePrintContext,
                          const Handle(OpenGl_Workspace) &AWorkspace,
                          const Graphic3d_CView& ACView,
                          const Aspect_CLayer2d& ACUnderLayer,
                          const Aspect_CLayer2d& ACOverLayer)
{
  // Store and disable current clipping planes
  const Handle(OpenGl_Context)& aContext = AWorkspace->GetGlContext();
  const Standard_Integer aMaxClipPlanes = aContext->MaxClipPlanes();
  const GLenum lastid = GL_CLIP_PLANE0 + aMaxClipPlanes;
  OPENGL_CLIP_PLANE *oldPlanes = new OPENGL_CLIP_PLANE[aMaxClipPlanes];
  OPENGL_CLIP_PLANE *ptrPlane = oldPlanes;
  GLenum planeid = GL_CLIP_PLANE0;
  for ( ; planeid < lastid; planeid++, ptrPlane++ )
  {
    glGetClipPlane( planeid, ptrPlane->Equation );
    if ( ptrPlane->isEnabled )
    {
      glDisable( planeid );
      ptrPlane->isEnabled = GL_TRUE;
    }
    else
    {
      ptrPlane->isEnabled = GL_FALSE;
    }
  }

  // Set OCCT state uniform variables
  const Handle(OpenGl_ShaderManager) aManager = aContext->ShaderManager();
  if (StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index()) != myLastLightSourceState)
  {
    aManager->UpdateLightSourceStateTo (&myLights);
    myLastLightSourceState = StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index());
  }
  if (StateInfo (myCurrViewMappingState, aManager->ProjectionState().Index()) != myLastViewMappingState)
  {
    aManager->UpdateProjectionStateTo (myMappingMatrix);
    myLastViewMappingState = StateInfo (myCurrViewMappingState, aManager->ProjectionState().Index());
  }
  if (StateInfo (myCurrOrientationState, aManager->WorldViewState().Index()) != myLastOrientationState)
  {
    aManager->UpdateWorldViewStateTo (myOrientationMatrix);
    myLastOrientationState = StateInfo (myCurrOrientationState, aManager->WorldViewState().Index());
  }
  if (aManager->ModelWorldState().Index() == 0)
  {
    Tmatrix3 aModelWorldState = { { 1.f, 0.f, 0.f, 0.f },
                                  { 0.f, 1.f, 0.f, 0.f },
                                  { 0.f, 0.f, 1.f, 0.f },
                                  { 0.f, 0.f, 0.f, 1.f } };
    
    aManager->UpdateModelWorldStateTo (aModelWorldState);
  }

  /////////////////////////////////////////////////////////////////////////////
  // Step 1: Prepare for redraw

  // Render background
  DrawBackground (AWorkspace);

  // Switch off lighting by default
  glDisable(GL_LIGHTING);

  /////////////////////////////////////////////////////////////////////////////
  // Step 2: Draw underlayer
  RedrawLayer2d (thePrintContext, ACView, ACUnderLayer);

  /////////////////////////////////////////////////////////////////////////////
  // Step 3: Redraw main plane

  // Setup face culling
  GLboolean isCullFace = GL_FALSE;
  if ( myBackfacing )
  {
    isCullFace = glIsEnabled( GL_CULL_FACE );
    if ( myBackfacing < 0 )
    {
      glEnable( GL_CULL_FACE );
      glCullFace( GL_BACK );
    }
	else
      glDisable( GL_CULL_FACE );
  }

  //TsmPushAttri(); /* save previous graphics context */

  // if the view is scaled normal vectors are scaled to unit length for correct displaying of shaded objects
  if(myExtra.scaleFactors[0] != 1.F ||
     myExtra.scaleFactors[1] != 1.F ||
     myExtra.scaleFactors[2] != 1.F)
    glEnable(GL_NORMALIZE);
  else if(glIsEnabled(GL_NORMALIZE))
    glDisable(GL_NORMALIZE);

  // Apply View Projection
  // This routine activates the Projection matrix for a view.

  glMatrixMode( GL_PROJECTION );

#ifdef _WIN32
  // add printing scale/tiling transformation
  if (!thePrintContext.IsNull())
  {
    thePrintContext->LoadProjTransformation();
  }
  else
#endif
    glLoadIdentity();

  glMultMatrixf( (const GLfloat *) myMappingMatrix );

  // Add translation necessary for the environnement mapping
  if (mySurfaceDetail != Visual3d_TOD_NONE)
  {
    // OCC280: FitAll work incorrect for perspective view if the SurfaceDetail mode is V3d_TEX_ENVIRONMENT or V3d_TEX_ALL
    // const GLfloat dep = vptr->vrep.extra.map.fpd * 0.5F;
    const GLfloat dep = (myExtra.map.fpd + myExtra.map.bpd) * 0.5F;
    glTranslatef(-dep*myExtra.vpn[0],-dep*myExtra.vpn[1],-dep*myExtra.vpn[2]);
  }

  // Apply matrix
  AWorkspace->SetViewMatrix((const OpenGl_Matrix *)myOrientationMatrix);

/*
While drawing after a clipplane has been defined and enabled, each vertex
is transformed to eye-coordinates, where it is dotted with the transformed
clipping plane equation.  Eye-coordinate vertexes whose dot product with
the transformed clipping plane equation is positive or zero are in, and
require no clipping.  Those eye-coordinate vertexes whose dot product is
negative are clipped.  Because clipplane clipping is done in eye-
coordinates, changes to the projection matrix have no effect on its
operation.

A point and a normal are converted to a plane equation in the following manner:

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

  // Apply Fog
  if ( myFog.IsOn )
  {
    const GLfloat ramp = myExtra.map.fpd - myExtra.map.bpd;
    const GLfloat fog_start = myFog.Front * ramp - myExtra.map.fpd;
    const GLfloat fog_end   = myFog.Back  * ramp - myExtra.map.fpd;

    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, fog_start);
    glFogf(GL_FOG_END, fog_end);
    glFogfv(GL_FOG_COLOR, myFog.Color.rgb);
    glEnable(GL_FOG);
  }
  else
    glDisable(GL_FOG);

  // Apply Lights
  {
    // setup lights
    Graphic3d_Vec4 anAmbientColor (THE_DEFAULT_AMBIENT[0],
                                   THE_DEFAULT_AMBIENT[1],
                                   THE_DEFAULT_AMBIENT[2],
                                   THE_DEFAULT_AMBIENT[3]);
    GLenum aLightGlId = GL_LIGHT0;
    for (OpenGl_ListOfLight::Iterator aLightIt (myLights);
         aLightIt.More(); aLightIt.Next())
    {
      bind_light (aLightIt.Value(), aLightGlId, anAmbientColor);
    }

    // apply accumulated ambient color
    anAmbientColor.a() = 1.0f;
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, anAmbientColor.GetData());

    if (aLightGlId != GL_LIGHT0)
    {
      glEnable (GL_LIGHTING);
    }
    // switch off unused lights
    for (; aLightGlId <= GL_LIGHT7; ++aLightGlId)
    {
      glDisable (aLightGlId);
    }
  }

  // Apply InteriorShadingMethod
  glShadeModel( myIntShadingMethod == TEL_SM_FLAT ? GL_FLAT : GL_SMOOTH );

  // Apply clipping planes
  {
    if (myZClip.Back.IsOn || myZClip.Front.IsOn)
    {
      const GLdouble ramp = myExtra.map.fpd - myExtra.map.bpd;

      Handle(Graphic3d_ClipPlane) aPlaneBack;
      Handle(Graphic3d_ClipPlane) aPlaneFront;

      if (myZClip.Back.IsOn)
      {
        const GLdouble back = ramp * myZClip.Back.Limit + myExtra.map.bpd;
        const Graphic3d_ClipPlane::Equation aBackEquation (0.0, 0.0, 1.0, -back);
        aPlaneBack = new Graphic3d_ClipPlane (aBackEquation);
      }

      if (myZClip.Front.IsOn)
      {
        const GLdouble front = ramp * myZClip.Front.Limit + myExtra.map.bpd;
        const Graphic3d_ClipPlane::Equation aFrontEquation (0.0, 0.0, -1.0, front);
        aPlaneFront = new Graphic3d_ClipPlane (aFrontEquation);
      }

      // do some "memory allocation"-wise optimization
      if (!aPlaneBack.IsNull() || !aPlaneFront.IsNull())
      {
        Graphic3d_SequenceOfHClipPlane aSlicingPlanes;
        if (!aPlaneBack.IsNull())
        {
          aSlicingPlanes.Append (aPlaneBack);
        }

        if (!aPlaneFront.IsNull())
        {
          aSlicingPlanes.Append (aPlaneFront);
        }

        // add planes at loaded view matrix state
        aContext->ChangeClipping().AddView (aSlicingPlanes, AWorkspace);
      }
    }

    // Apply user clipping planes
    if (!myClipPlanes.IsEmpty())
    {
      Graphic3d_SequenceOfHClipPlane aUserPlanes;
      Graphic3d_SequenceOfHClipPlane::Iterator aClippingIt (myClipPlanes);
      for (; aClippingIt.More(); aClippingIt.Next())
      {
        const Handle(Graphic3d_ClipPlane)& aClipPlane = aClippingIt.Value();
        if (aClipPlane->IsOn())
        {
          aUserPlanes.Append (aClipPlane);
        }
      }

      if (!aUserPlanes.IsEmpty())
      {
        // add planes at actual matrix state.
        aContext->ChangeClipping().AddWorld (aUserPlanes);
      }
    }
    
    if (!aManager->IsEmpty())
    {
      aManager->UpdateClippingState();
    }
  }

  // Apply AntiAliasing
  {
    if (myAntiAliasing)
      AWorkspace->NamedStatus |= OPENGL_NS_ANTIALIASING;
	else
      AWorkspace->NamedStatus &= ~OPENGL_NS_ANTIALIASING;
  }

  // Clear status bitfields
  AWorkspace->NamedStatus &= ~(OPENGL_NS_2NDPASSNEED | OPENGL_NS_2NDPASSDO);

  // Added PCT for handling of textures
  switch (mySurfaceDetail)
  {
    case Visual3d_TOD_NONE:
      AWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;
      AWorkspace->DisableTexture();
      // Render the view
      RenderStructs(AWorkspace);
      break;

    case Visual3d_TOD_ENVIRONMENT:
      AWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;
      AWorkspace->EnableTexture (myTextureEnv);
      // Render the view
      RenderStructs(AWorkspace);
      AWorkspace->DisableTexture();
      break;

    case Visual3d_TOD_ALL:
      // First pass
      AWorkspace->NamedStatus &= ~OPENGL_NS_FORBIDSETTEX;
      // Render the view
      RenderStructs(AWorkspace);
      AWorkspace->DisableTexture();

      // Second pass
      if (AWorkspace->NamedStatus & OPENGL_NS_2NDPASSNEED)
      {
        AWorkspace->NamedStatus |= OPENGL_NS_2NDPASSDO;
        AWorkspace->EnableTexture (myTextureEnv);

        /* sauvegarde de quelques parametres OpenGL */
        GLint blend_dst, blend_src;
        GLint zbuff_f;
        GLboolean zbuff_w;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &zbuff_w);
        glGetIntegerv(GL_DEPTH_FUNC, &zbuff_f);
        glGetIntegerv(GL_BLEND_DST, &blend_dst);
        glGetIntegerv(GL_BLEND_SRC, &blend_src);
        GLboolean zbuff_state = glIsEnabled(GL_DEPTH_TEST);
        GLboolean blend_state = glIsEnabled(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glDepthFunc(GL_EQUAL);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);

        AWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;

        // Render the view
        RenderStructs(AWorkspace);
        AWorkspace->DisableTexture();

        /* restauration des parametres OpenGL */
        glBlendFunc(blend_src, blend_dst);
        if (!blend_state) glDisable(GL_BLEND);

        glDepthFunc(zbuff_f);
        glDepthMask(zbuff_w);
        if (!zbuff_state) glDisable(GL_DEPTH_FUNC);
      }
      break;
  }

  // Resetting GL parameters according to the default aspects
  // in order to synchronize GL state with the graphic driver state
  // before drawing auxiliary stuff (trihedrons, overlayer)
  // and invoking optional callbacks
  AWorkspace->ResetAppliedAspect();

  aContext->ChangeClipping().RemoveAll();

  if (!aManager->IsEmpty())
  {
    aManager->ResetMaterialStates();
    aManager->RevertClippingState();

    // We need to disable (unbind) all shaders programs to ensure
    // that all objects without specified aspect will be drawn
    // correctly (such as background)
    OpenGl_ShaderProgram::Unbind (aContext);
  }

  // display global trihedron
  if (myTrihedron != NULL)
  {
    myTrihedron->Render (AWorkspace);
  }
  if (myGraduatedTrihedron != NULL)
  {
    myGraduatedTrihedron->Render (AWorkspace);
  }

  // Restore face culling
  if ( myBackfacing )
  {
    if ( isCullFace )
    {
      glEnable   ( GL_CULL_FACE );
      glCullFace ( GL_BACK      );
    }
    else
      glDisable ( GL_CULL_FACE );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Step 6: Draw overlayer
  const int aMode = 0;
  AWorkspace->DisplayCallback (ACView, (aMode | OCC_PRE_OVERLAY));

  RedrawLayer2d (thePrintContext, ACView, ACOverLayer);

  AWorkspace->DisplayCallback (ACView, aMode);

  // Restore clipping planes
  for ( ptrPlane = oldPlanes, planeid = GL_CLIP_PLANE0; planeid < lastid; planeid++, ptrPlane++ )
  {
    glClipPlane( planeid, ptrPlane->Equation );
    if ( ptrPlane->isEnabled )
      glEnable( planeid );
    else
      glDisable( planeid );
  }
  delete[] oldPlanes;
}

/*----------------------------------------------------------------------*/

//ExecuteViewDisplay
void OpenGl_View::RenderStructs (const Handle(OpenGl_Workspace) &AWorkspace)
{
  if ( myZLayers.NbStructures() <= 0 )
    return;

  glPushAttrib ( GL_DEPTH_BUFFER_BIT );

  //TsmPushAttri(); /* save previous graphics context */

  if ( (AWorkspace->NamedStatus & OPENGL_NS_2NDPASSNEED) == 0 )
  {
    const int antiAliasingMode = AWorkspace->GetDisplay()->AntiAliasingMode();

    if ( !myAntiAliasing )
    {
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_LINE_SMOOTH);
      if( antiAliasingMode & 2 ) glDisable(GL_POLYGON_SMOOTH);
      glBlendFunc (GL_ONE, GL_ZERO);
      glDisable (GL_BLEND);
    }
    else
    {
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_LINE_SMOOTH);
      if( antiAliasingMode & 2 ) glEnable(GL_POLYGON_SMOOTH);
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_BLEND);
    }
  }

  myZLayers.Render (AWorkspace);

  //TsmPopAttri(); /* restore previous graphics context; before update lights */
  glPopAttrib();
}

/*----------------------------------------------------------------------*/

//call_togl_redraw_layer2d
void OpenGl_View::RedrawLayer2d (const Handle(OpenGl_PrinterContext)& thePrintContext,
                                 const Graphic3d_CView&               ACView,
                                 const Aspect_CLayer2d&               ACLayer)
{
  if (&ACLayer == NULL
   || ACLayer.ptrLayer == NULL
   || ACLayer.ptrLayer->listIndex == 0) return;

  GLsizei dispWidth  = (GLsizei )ACLayer.viewport[0];
  GLsizei dispHeight = (GLsizei )ACLayer.viewport[1];

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix ();
  glLoadIdentity ();

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();

  if (!ACLayer.sizeDependent)
    glViewport (0, 0, dispWidth, dispHeight);

  float left = ACLayer.ortho[0];
  float right = ACLayer.ortho[1];
  float bottom = ACLayer.ortho[2];
  float top = ACLayer.ortho[3];

  int attach = ACLayer.attach;

  float ratio;
  if (!ACLayer.sizeDependent)
    ratio = (float) dispWidth/dispHeight;
  else
    ratio = ACView.DefWindow.dx/ACView.DefWindow.dy;

  float delta;
  if (ratio >= 1.0) {
    delta = (float )((top - bottom)/2.0);
    switch (attach) {
      case 0: /* Aspect_TOC_BOTTOM_LEFT */
        top = bottom + 2*delta/ratio;
        break;
      case 1: /* Aspect_TOC_BOTTOM_RIGHT */
        top = bottom + 2*delta/ratio;
        break;
      case 2: /* Aspect_TOC_TOP_LEFT */
        bottom = top - 2*delta/ratio;
        break;
      case 3: /* Aspect_TOC_TOP_RIGHT */
        bottom = top - 2*delta/ratio;
        break;
    }
  }
  else {
    delta = (float )((right - left)/2.0);
    switch (attach) {
      case 0: /* Aspect_TOC_BOTTOM_LEFT */
        right = left + 2*delta*ratio;
        break;
      case 1: /* Aspect_TOC_BOTTOM_RIGHT */
        left = right - 2*delta*ratio;
        break;
      case 2: /* Aspect_TOC_TOP_LEFT */
        right = left + 2*delta*ratio;
        break;
      case 3: /* Aspect_TOC_TOP_RIGHT */
        left = right - 2*delta*ratio;
        break;
    }
  }

#ifdef _WIN32
  // Check printer context that exists only for print operation
  if (!thePrintContext.IsNull())
  {
    // additional transformation matrix could be applied to
    // render only those parts of viewport that will be
    // passed to a printer as a current "frame" to provide
    // tiling; scaling of graphics by matrix helps render a
    // part of a view (frame) in same viewport, but with higher
    // resolution
    thePrintContext->LoadProjTransformation();

    // printing operation also assumes other viewport dimension
    // to comply with transformation matrix or graphics scaling
    // factors for tiling for layer redraw
    GLsizei anViewportX = 0;
    GLsizei anViewportY = 0;
    thePrintContext->GetLayerViewport (anViewportX, anViewportY);
    if (anViewportX != 0 && anViewportY != 0)
      glViewport (0, 0, anViewportX, anViewportY);
  }
#endif

  glOrtho (left, right, bottom, top, -1.0, 1.0);

  glPushAttrib (
    GL_LIGHTING_BIT | GL_LINE_BIT | GL_POLYGON_BIT |
    GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT );

  glDisable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_1D);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_LIGHTING);

  // TODO: Obsolete code, the display list is always empty now, to be removed
  glCallList (ACLayer.ptrLayer->listIndex);

  //calling dynamic render of LayerItems
  if ( ACLayer.ptrLayer->layerData )
  {
    InitLayerProp (ACLayer.ptrLayer->listIndex);
    ((Visual3d_Layer*)ACLayer.ptrLayer->layerData)->RenderLayerItems();
    InitLayerProp (0);
  }

  glPopAttrib ();

  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix ();

  if (!ACLayer.sizeDependent)
    glViewport (0, 0, (GLsizei) ACView.DefWindow.dx, (GLsizei) ACView.DefWindow.dy);

  glFlush ();
}

/*----------------------------------------------------------------------*/

//call_togl_create_bg_texture
void OpenGl_View::CreateBackgroundTexture (const Standard_CString  theFilePath,
                                           const Aspect_FillMethod theFillStyle)
{
  if (myBgTexture.TexId != 0)
  {
    // delete existing texture
    glDeleteTextures (1, (GLuint* )&(myBgTexture.TexId));
    myBgTexture.TexId = 0;
  }

  // load image from file
  Image_AlienPixMap anImageLoaded;
  if (!anImageLoaded.Load (theFilePath))
  {
    return;
  }

  Image_PixMap anImage;
  if (anImageLoaded.RowExtraBytes() == 0 &&
      (anImageLoaded.Format() == Image_PixMap::ImgRGB
    || anImageLoaded.Format() == Image_PixMap::ImgRGB32
    || anImageLoaded.Format() == Image_PixMap::ImgRGBA))
  {
    anImage.InitWrapper (anImageLoaded.Format(), anImageLoaded.ChangeData(),
                         anImageLoaded.SizeX(), anImageLoaded.SizeY(), anImageLoaded.SizeRowBytes());
  }
  else
  {
    // convert image to RGB format
    if (!anImage.InitTrash (Image_PixMap::ImgRGB, anImageLoaded.SizeX(), anImageLoaded.SizeY()))
    {
      return;
    }

    anImage.SetTopDown (false);
    Image_PixMapData<Image_ColorRGB>& aDataNew = anImage.EditData<Image_ColorRGB>();
    Quantity_Color aSrcColor;
    for (Standard_Size aRow = 0; aRow < anImage.SizeY(); ++aRow)
    {
      for (Standard_Size aCol = 0; aCol < anImage.SizeX(); ++aCol)
      {
        aSrcColor = anImageLoaded.PixelColor ((Standard_Integer )aCol, (Standard_Integer )aRow);
        Image_ColorRGB& aColor = aDataNew.ChangeValue (aRow, aCol);
        aColor.r() = Standard_Byte(255.0 * aSrcColor.Red());
        aColor.g() = Standard_Byte(255.0 * aSrcColor.Green());
        aColor.b() = Standard_Byte(255.0 * aSrcColor.Blue());
      }
    }
    anImageLoaded.Clear();
  }

  // create MipMapped texture
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  GLuint aTextureId = 0;
  glGenTextures (1, &aTextureId);
  glBindTexture (GL_TEXTURE_2D, aTextureId);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

  const GLenum aDataFormat = (anImage.Format() == Image_PixMap::ImgRGB) ? GL_RGB : GL_RGBA;
  gluBuild2DMipmaps (GL_TEXTURE_2D, 3/*4*/,
                     GLint(anImage.SizeX()), GLint(anImage.SizeY()),
                     aDataFormat, GL_UNSIGNED_BYTE, anImage.Data());

  myBgTexture.TexId  = aTextureId;
  myBgTexture.Width  = (Standard_Integer )anImage.SizeX();
  myBgTexture.Height = (Standard_Integer )anImage.SizeY();
  myBgTexture.Style  = theFillStyle;
}

/*----------------------------------------------------------------------*/

//call_togl_set_bg_texture_style
void OpenGl_View::SetBackgroundTextureStyle (const Aspect_FillMethod AFillStyle)
{
  myBgTexture.Style = AFillStyle;
}

/*----------------------------------------------------------------------*/

//call_togl_gradient_background
void OpenGl_View::SetBackgroundGradient (const Quantity_Color& AColor1,
                                        const Quantity_Color& AColor2,
                                        const Aspect_GradientFillMethod AType)
{
  Standard_Real R,G,B;
  AColor1.Values( R, G, B, Quantity_TOC_RGB );
  myBgGradient.color1.rgb[0] = ( Tfloat )R;
  myBgGradient.color1.rgb[1] = ( Tfloat )G;
  myBgGradient.color1.rgb[2] = ( Tfloat )B;
  myBgGradient.color1.rgb[3] = 0.F;

  AColor2.Values( R, G, B, Quantity_TOC_RGB );
  myBgGradient.color2.rgb[0] = ( Tfloat )R;
  myBgGradient.color2.rgb[1] = ( Tfloat )G;
  myBgGradient.color2.rgb[2] = ( Tfloat )B;
  myBgGradient.color2.rgb[3] = 0.F;

  myBgGradient.type = AType;
}

/*----------------------------------------------------------------------*/

//call_togl_set_gradient_type
void OpenGl_View::SetBackgroundGradientType (const Aspect_GradientFillMethod AType)
{
  myBgGradient.type = AType;
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================

void OpenGl_View::AddZLayer (const Standard_Integer theLayerId)
{
  myZLayers.AddLayer (theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================

void OpenGl_View::RemoveZLayer (const Standard_Integer theLayerId)
{
  myZLayers.RemoveLayer (theLayerId);
}

//=======================================================================
//function : DisplayStructure
//purpose  :
//=======================================================================

void OpenGl_View::DisplayStructure (const OpenGl_Structure *theStructure,
                                    const Standard_Integer  thePriority)
{
  Standard_Integer aZLayer = theStructure->GetZLayer ();
  myZLayers.AddStructure (theStructure, aZLayer, thePriority);
}

//=======================================================================
//function : EraseStructure
//purpose  :
//=======================================================================

void OpenGl_View::EraseStructure (const OpenGl_Structure *theStructure)
{
  Standard_Integer aZLayer = theStructure->GetZLayer ();
  myZLayers.RemoveStructure (theStructure, aZLayer);
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void OpenGl_View::ChangeZLayer (const OpenGl_Structure *theStructure,
                                const Standard_Integer  theNewLayerId)
{
  Standard_Integer anOldLayer = theStructure->GetZLayer ();
  myZLayers.ChangeLayer (theStructure, anOldLayer, theNewLayerId);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void OpenGl_View::SetZLayerSettings (const Standard_Integer theLayerId,
                                     const Graphic3d_ZLayerSettings theSettings)
{
  myZLayers.Layer (theLayerId).SetLayerSettings (theSettings);
}

