// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

/***********************************************************************

FONCTION :
----------
File OpenGl_telem_view :

************************************************************************/

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_telem_view.hxx>
#include <OpenGl_telem_util.hxx>
#include <OpenGl_Display.hxx>

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
  if (openglDisplay.IsNull() || !openglDisplay->Walkthrough())
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
  else if (!openglDisplay.IsNull() && !openglDisplay->SymPerspective())/* TelPerspective */
  {
    pmat[0][0] = pmat[1][1] = mapping->prp[2] - mapping->vpd;
    pmat[2][0] = -gx; 
    pmat[2][1] = -gy; 
    pmat[2][3] = ( float )-1.0;
    pmat[3][0] = mapping->vpd * gx; 
    pmat[3][1] = mapping->vpd * gy; 
    pmat[3][3] = mapping->prp[2];

    /* modify the next two cells to change clipping policy */
    if (!openglDisplay.IsNull() && !openglDisplay->Walkthrough())
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

void
TelEvalViewMappingMatrix( tel_view_mapping mapping /* View Mapping */,
                          Tint *error_ind          /* Out: Error Indicator */,
                          Tmatrix3 mat             /* Out: Mapping Matrix */
                         )
{
  EvalViewMappingMatrix( mapping, error_ind, mat, 0, ( float )0.0, ( float )0.0, 0, 0 );
}

