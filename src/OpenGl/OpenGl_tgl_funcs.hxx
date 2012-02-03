/***********************************************************************

FONCTION :
----------
Fichier OpenGl_tgl_funcs.h


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
??-??-?? : PCT ; creation
10-07-96 : FMN ; Suppression #define sur calcul matrice
05-08-97 : PCT ; support texture mapping
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
15-01-98 : FMN ; Ajout Hidden line
08-04-98 : FGU ; Ajout emission 
27-11-98 : CAL ; S4062. Ajout des layers.
30-11-98 : FMN ; S3819 : Textes toujours visibles
22-03-04 : SAN ; OCC4895 High-level interface for controlling polygon offsets
04-10-04 : ABD ; Added User Defined marker type

************************************************************************/

/*----------------------------------------------------------------------*/

#ifndef  OPENGL_TGL_FUNCS_H
#define  OPENGL_TGL_FUNCS_H

#define BUC60570        /* GG 06-09-99
//              The model view SD_NORMAL must shown objects with FLAT shading
*/

#ifndef G003
#define G003            /* EUG 06-10-99 Degeneration support
*/
#endif


#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
from higher API */


/*----------------------------------------------------------------------*/
/*
* Includes
*/
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OSD_FontAspect.hxx>
#include <Graphic3d_CGraduatedTrihedron.hxx>

/*----------------------------------------------------------------------*/
/*
* Types definis
*/

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

/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

extern void call_func_eval_ori_matrix3 (const point3* vrp,
                                        const vec3*   vpn,
                                        const vec3*   vup,
                                        int* err_ind,
                                        float mout[4][4]);

extern void call_func_eval_map_matrix3(view_map3 *Map, int *err_ind, matrix3 mat);
									   
int EXPORT call_togl_create_texture (int Type, unsigned int Width, unsigned int Height, unsigned char *Data, char *FileName);

void EXPORT call_togl_destroy_texture (int TexId);

void EXPORT call_togl_modify_texture (int TexId, CALL_DEF_INIT_TEXTURE *init_tex);
			
int EXPORT call_togl_inquiretexture ();

int EXPORT call_togl_inquireplane ();

#endif
