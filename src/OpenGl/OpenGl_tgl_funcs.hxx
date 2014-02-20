// Created by: PCT
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
