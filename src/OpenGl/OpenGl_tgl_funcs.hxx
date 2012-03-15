// File:      OpenGl_tgl_funcs.hxx
// Created:   ??-??-??
// Author:    PCT
// Copyright: OPEN CASCADE 2012

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
