// Created by: PCT
// Copyright (c) 1995-1999 Matra Datavision
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


#ifndef OPENGL_TGL_FUNCS_H
#define OPENGL_TGL_FUNCS_H

typedef float matrix3[4][4];

typedef enum {
  TYPE_PARAL,
  TYPE_PERSPECT
} projtype;

typedef enum {
  IND_NO_CLIP,
  IND_CLIP
} clip_ind;

typedef struct {
  float   x;  /* x coordinate */
  float   y;  /* y coordinate */
  float   z;  /* z coordinate */
} point3;

typedef struct {
  float   delta_x;    /* x magnitude */
  float   delta_y;    /* y magnitude */
  float   delta_z;    /* z magnitude */
} vec3;

typedef struct {
  float   x_min;  /* x min */
  float   x_max;  /* x max */
  float   y_min;  /* y min */
  float   y_max;  /* y max */
  float   z_min;  /* z min */
  float   z_max;  /* z max */
} limit3;

typedef struct {
  float   x_min;  /* x min */
  float   x_max;  /* x max */
  float   y_min;  /* y min */
  float   y_max;  /* y max */
} limit;

typedef struct {
  limit   win;    /* window limits */
  limit3  proj_vp;    /* viewport limits */
  projtype    proj_type;  /* projection type */
  point3  proj_ref_point; /* projection reference point */
  float   view_plane; /* view plane distance */
  float   back_plane; /* back plane distance */
  float   front_plane;    /* front plane distance */
} view_map3;

extern void call_func_eval_ori_matrix3 (const point3* vrp,
                                        const vec3*   vpn,
                                        const vec3*   vup,
                                        int* err_ind,
                                        float mout[4][4]);

extern void call_func_eval_map_matrix3(view_map3 *Map, int *err_ind, matrix3 mat);
									   		
#endif // OPENGL_TGL_FUNCS_H
