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
#include <OpenGl_telem.hxx>
#include <OpenGl_telem_pick.hxx>
#include <OpenGl_tsm.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OSD_FontAspect.hxx>

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

extern  void  call_func_label(Tint); /* lid */
extern  void  call_func_set_view_ind(Tint); /* vid */
extern  void  call_func_appl_data(void*); /* data */
extern  void  call_func_set_local_tran3(Tmatrix3, TComposeType); /* mat, mode */
extern  void  call_func_set_linewidth(Tfloat); /* lw */
extern  void  call_func_set_linetype(Tint); /* lt */
extern  void  call_func_set_int_style(Tint); /* is */
extern  void  call_func_set_edge_flag(Tint); /* state */
extern  void  call_func_set_edge_type(Tint); /* type */
extern  void  call_func_set_edgewidth(Tfloat); /* width */
extern  void  call_func_set_int_style_ind(Tint); /* ind */
extern  void  call_func_set_face_disting_mode(Tint); /* mode */
extern  void  call_func_set_face_cull_mode(Tint); /* mode */
extern  void  call_func_set_marker_type(Tint); /* type */
extern  void  call_func_set_marker_size(Tfloat); /* size */
extern  void  call_func_exec_struct(Tint); /* stid */
extern  void  call_func_init_tgl(void);
extern  void  call_func_post_struct(Tint, Tint, Tfloat); /* ws, stid, prio */
extern  void  call_func_set_int_shad_meth(Tint); /* method */
extern  void  call_func_set_refl_eqn(Tint); /* eqn */
extern  void  call_func_set_back_refl_eqn(Tint); /* eqn */
extern  void  call_func_set_text_font(Tchar*); /* font */
extern  void  call_func_set_char_space(Tfloat); /* sp */
extern  void  call_func_set_char_expan(Tfloat); /* exp */
extern  void  call_func_set_pick_id(Tint); /* pickid */
extern  void  call_func_redraw_all_structs (Tint, Tint); /* wsid, swap */
extern  void  call_func_redraw_all_structs_begin (Tint); /* wsid */
extern  void  call_func_redraw_all_structs_proc (Tint); /* wsid */
extern  void  call_func_redraw_all_structs_end (Tint, Tint); /* wsid, swap */
extern  void  call_func_set_anno_char_ht(Tfloat); /* height */
extern  void  call_func_set_anno_char_up_vec(Tfloat[2]); /* up_vector */
extern  void  call_func_set_anno_path(Tint); /* path */
extern  void  call_func_set_anno_align(Tint, Tint); /* hor, ver */
extern  void  call_func_anno_text_rel3( tel_point, Tchar* );
extern  void  call_func_dcue_ind(Tint); /* dcid */
extern  void  call_func_set_do_texturemap(Tint); /* bool */
extern  void  call_func_set_texture_id(Tint); /* TextureID */
extern  void  call_func_set_text_style(int); /* style */
extern  void  call_func_set_text_display_type(int); /* display_type */
#ifdef G003
extern  void call_func_set_degenerate_model ( Tint, Tfloat ); /* degenerate model */
#endif  /* G003 */

extern  void  call_func_set_text_zoomable(Tint); /*Text Zoomable usage*/
extern  void  call_func_set_text_angle(Tfloat); /*Text Angle usage*/
extern  void  call_func_set_text_fontaspect(OSD_FontAspect); /*Text Zoomable usage*/

/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
extern  void  call_func_set_polygon_offset_params( Tint,     /* mode */
                                                  Tfloat,   /* factor */
                                                  Tfloat ); /* units */
/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
extern  void call_func_set_transform_persistence( Tint, Tfloat, Tfloat, Tfloat );
/* extern  void  call_func_set_transform_persistence( tel_transform_persistence tp );*/
/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */


/* OCC18942: obsolete in OCCT6.3, might be removed in further versions! */
extern void call_func_eval_ori_matrix3( point3 *vrp, vec3 *vpn, vec3 *vup,
                                       int *err_ind, float mout[4][4] );
/* OCC18942: obsolete in OCCT6.3, might be removed in further versions! */
extern void call_func_eval_map_matrix3( view_map3 *Map, int *err_ind,
                                       matrix3 mat );

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

/* Edge types */
#define  CALL_PHIGS_EDGE_OFF           TOff
#define  CALL_PHIGS_EDGE_ON            TOn

/* Edge data flags */
#define  CALL_PHIGS_EDGE_NONE          TOff
#define  CALL_PHIGS_EDGE_VISIBILITY    TOn

/* Line Types */
#define  CALL_PHIGS_LINE_SOLID         TEL_LS_SOLID
#define  CALL_PHIGS_LINE_DASH          TEL_LS_DASH
#define  CALL_PHIGS_LINE_DOT           TEL_LS_DOT
#define  CALL_PHIGS_LINE_DASH_DOT      TEL_LS_DASH_DOT

/* Marker types */
#define  CALL_PHIGS_MARKER_DOT          TEL_PM_DOT
#define  CALL_PHIGS_MARKER_PLUS         TEL_PM_PLUS
#define  CALL_PHIGS_MARKER_ASTERISK     TEL_PM_STAR
#define  CALL_PHIGS_MARKER_CIRCLE       TEL_PM_CIRC
#define  CALL_PHIGS_MARKER_CROSS        TEL_PM_CROSS
#define  CALL_PHIGS_MARKER_USER_DEFINED TEL_PM_USERDEFINED

/* Text : Path */
#define  CALL_PHIGS_PATH_RIGHT         0
#define  CALL_PHIGS_PATH_LEFT          0
#define  CALL_PHIGS_PATH_UP            0
#define  CALL_PHIGS_PATH_DOWN          0

/* Text : Horizontal alignment */
#define  CALL_PHIGS_HOR_LEFT           0
#define  CALL_PHIGS_HOR_CENTER         1
#define  CALL_PHIGS_HOR_RIGHT          2

/* Text : Vertical alignment */
#define  CALL_PHIGS_VERT_BOTTOM        0
#define  CALL_PHIGS_VERT_CENTER        1
#define  CALL_PHIGS_VERT_TOP           2

/* Interior styles */
#define  CALL_PHIGS_STYLE_HOLLOW       TSM_HOLLOW
#define  CALL_PHIGS_STYLE_SOLID        TSM_SOLID
#define  CALL_PHIGS_STYLE_HATCH        TSM_HATCH
#define  CALL_PHIGS_STYLE_EMPTY        TSM_EMPTY
#define  CALL_PHIGS_STYLE_HIDDENLINE   TSM_HIDDENLINE

/* Light Source Types */
#define  CALL_PHIGS_LIGHT_AMBIENT      TLightAmbient
#define  CALL_PHIGS_LIGHT_DIRECTIONAL  TLightDirectional
#define  CALL_PHIGS_LIGHT_POSITIONAL   TLightPositional
#define  CALL_PHIGS_LIGHT_SPOT         TLightSpot

/* Hatch styles */
#define  CALL_PHIGS_HATCH_HORIZ        TEL_HS_HORIZONTAL
#define  CALL_PHIGS_HATCH_HORIZ_DBL    TEL_HS_HORIZONTAL_SPARSE
#define  CALL_PHIGS_HATCH_VERT         TEL_HS_VERTICAL
#define  CALL_PHIGS_HATCH_VERT_DBL     TEL_HS_VERTICAL_SPARSE
#define  CALL_PHIGS_HATCH_DIAG_45      TEL_HS_DIAG_45
#define  CALL_PHIGS_HATCH_DIAG_45_DBL  TEL_HS_DIAG_45_SPARSE
#define  CALL_PHIGS_HATCH_DIAG_135     TEL_HS_DIAG_135
#define  CALL_PHIGS_HATCH_DIAG_135_DBL TEL_HS_DIAG_135_SPARSE
#define  CALL_PHIGS_HATCH_GRID_R       TEL_HS_GRID
#define  CALL_PHIGS_HATCH_GRID_R_DBL   TEL_HS_GRID_SPARSE
#define  CALL_PHIGS_HATCH_GRID_D       TEL_HS_CROSS
#define  CALL_PHIGS_HATCH_GRID_D_DBL   TEL_HS_CROSS_SPARSE

/* Edit modes */
#define  CALL_PHIGS_EDIT_INSERT        TEditInsert
#define  CALL_PHIGS_EDIT_REPLACE       TEditReplace

/* Transformation */
#define  CALL_PHIGS_TYPE_PRECONCAT     TPreConcatenate
#define  CALL_PHIGS_TYPE_POSTCONCAT    TPostConcatenate
#define  CALL_PHIGS_TYPE_REPLACE       TReplace

/* Clipping indicators */
#define  CALL_PHIGS_IND_NO_CLIP        TOff
#define  CALL_PHIGS_IND_CLIP           TOn

/* Projection types */
#define  CALL_PHIGS_TYPE_PARAL         TelParallel
#define  CALL_PHIGS_TYPE_PERSPECT      TelPerspective

/* shading methods */
#define  CALL_PHIGS_SD_NONE            TEL_SM_FLAT
#define  CALL_PHIGS_SD_COLOUR          TEL_SM_GOURAUD
#define  CALL_PHIGS_SD_DOT_PRODUCT     TEL_SM_GOURAUD
#ifndef BUC60570
#define  CALL_PHIGS_SD_NORMAL          TEL_SM_GOURAUD
#else
#define  CALL_PHIGS_SD_NORMAL          TEL_SM_FLAT
#endif

/* lighting methods */
#define  CALL_PHIGS_REFL_NONE               0x0000
#define  AMBIENT_MASK                       0x0001
#define  DIFFUSE_MASK                       0x0002
#define  SPECULAR_MASK                      0x0004
#define  EMISSIVE_MASK                      0x0008

/* cull mode */
#define  CALL_PHIGS_CULL_NONE          TelCullNone
#define  CALL_PHIGS_CULL_BACKFACE      TelCullBack
#define  CALL_PHIGS_CULL_FRONTFACE     TelCullFront

/* depth cue mode */
#define  CALL_PHIGS_SUPPRESSED         TelDCSuppressed
#define  CALL_PHIGS_ALLOWED            TelDCAllowed

/* distinguish mode */
#define  CALL_PHIGS_DISTING_NO         TOff
#define  CALL_PHIGS_DISTING_YES        TOn

#define  CALL_DEF_ENABLE_ZBUFFER       TOn
#define  CALL_DEF_DISABLE_ZBUFFER      TOff

/* style of text */
#define  ASPECT_TOST_NORMAL     0
#define  ASPECT_TOST_ANNOTATION 1

/* display type of the text */
#define  ASPECT_TODT_NORMAL     0
#define  ASPECT_TODT_SUBTITLE   1
#define  ASPECT_TODT_DEKALE     2
#define  ASPECT_TODT_BLEND      3

/* Pick Order */
#define  PTOP_FIRST                       TTopFirst
#define  PBOTTOM_FIRST                    TBottomFirst

/* Facet types */
#define  CALL_PHIGS_SHAPE_COMPLEX         TEL_SHAPE_COMPLEX
#define  CALL_PHIGS_SHAPE_NONCONVEX       TEL_SHAPE_CONCAVE
#define  CALL_PHIGS_SHAPE_CONVEX          TEL_SHAPE_CONVEX
#define  CALL_PHIGS_SHAPE_UNKNOWN         TEL_SHAPE_UNKNOWN

/* Vertex flags */
#define CALL_PHIGS_VERT_COORD                   TEL_VTFLAG_NONE
#define CALL_PHIGS_VERT_COORD_COLOUR            TEL_VTFLAG_COLOUR
#define CALL_PHIGS_VERT_COORD_NORMAL            TEL_VTFLAG_NORMAL
#define CALL_PHIGS_VERT_COORD_COLOUR_NORMAL     TEL_VTFLAG_COLOURNORMAL

/* Facet data flags */
#define CALL_PHIGS_FACET_NONE                   TEL_FAFLAG_NONE
#define CALL_PHIGS_FACET_COLOUR                 TEL_FAFLAG_COLOUR
#define CALL_PHIGS_FACET_NORMAL                 TEL_FAFLAG_NORMAL
#define CALL_PHIGS_FACET_COLOUR_NORMAL          TEL_FAFLAG_COLOURNORMAL

/* functions */
#define  call_func_open_struct         TsmOpenStructure
#define  call_func_close_struct        TsmCloseStructure
#define  call_func_set_edit_mode       TsmSetEditMode
#define  call_func_set_elem_ptr        TsmSetElementPointer
#define  call_func_offset_elem_ptr     TsmOffsetElementPointer
#define  call_func_set_elem_ptr_label  TsmSetElementPointerAtLabel
#define  call_func_del_elem            TsmDeleteElement
#define  call_func_del_elem_range      TsmDeleteElementRange
#define  call_func_del_elems_labels    TsmDeleteElementsBetweenLabels
#define  call_func_del_struct          TsmDeleteStructure 

#define call_func_inq_elem_ptr             TsmGetCurElemPtr

/* Declarations  des subroutines  triedron */

void EXPORT call_togl_ztriedron_setup (
                                       float* xcolor,
                                       float* ycolor,
                                       float* zcolor,
                                       float  sizeratio,
                                       float  axisdiameter,
                                       int    nbfacettes          
                                       );

void EXPORT call_togl_triedron_display (
                                        CALL_DEF_VIEW * aview, 
                                        int APosition, 
                                        float r,
                                        float g,
                                        float b,
                                        float AScale,
                                        int asWireframe 
                                        );

void EXPORT call_togl_triedron_erase (
                                      CALL_DEF_VIEW *aview
                                      );

void EXPORT call_togl_triedron_echo (
                                     CALL_DEF_VIEW *aview,
                                     int AType
                                     );

/* Graduated trihedron */

void EXPORT call_togl_graduatedtrihedron_get(CALL_DEF_VIEW* view, 
                                             CALL_DEF_GRADUATEDTRIHEDRON* cubic);

void EXPORT call_togl_graduatedtrihedron_display(CALL_DEF_VIEW* view, 
                                                 CALL_DEF_GRADUATEDTRIHEDRON* cubic);

void EXPORT call_togl_graduatedtrihedron_erase(CALL_DEF_VIEW* view);

void EXPORT call_togl_graduatedtrihedron_minmaxvalues(float xmin, float ymin, float zmin,
                                                      float xmax, float ymax, float zmax);

/* Declarations  des subroutines  mode immediat */

int EXPORT call_togl_begin_ajout_mode (
                                       CALL_DEF_VIEW *aview
                                       );

void EXPORT call_togl_end_ajout_mode (
                                      );

int EXPORT call_togl_begin_immediat_mode (
  CALL_DEF_VIEW *aview,
  CALL_DEF_LAYER *anunderlayer,
  CALL_DEF_LAYER *anoverlayer,
  int doublebuffer,
  int   retainmode
  );

void EXPORT call_togl_end_immediat_mode (
  int synchronize
  );

void EXPORT call_togl_clear_immediat_mode (
  CALL_DEF_VIEW *aview,
  int aFlush
  );

void EXPORT call_togl_transform (
                                 CALL_DEF_MATRIX4X4 amatrix,
                                 int mode
                                 );

void EXPORT call_togl_begin_polyline (
                                      );

void EXPORT call_togl_end_polyline (
                                    );

void EXPORT call_togl_begin_polygon (
                                     );

void EXPORT call_togl_end_polygon (
                                   );

void EXPORT call_togl_begin_trianglemesh (
  );

void EXPORT call_togl_end_trianglemesh (
                                        );

void EXPORT call_togl_begin_marker (
                                    );

void EXPORT call_togl_end_marker (
                                  );

void EXPORT call_togl_begin_bezier (
                                    );

void EXPORT call_togl_end_bezier (
                                  );

void EXPORT call_togl_move (
                            float x,
                            float y,
                            float z
                            );

void EXPORT call_togl_movenormal (
                                  float x,
                                  float y,
                                  float z,
                                  float nx,
                                  float ny,
                                  float nz
                                  );

void EXPORT call_togl_moveweight (
                                  float x,
                                  float y,
                                  float z,
                                  float w
                                  );

void EXPORT call_togl_draw (
                            float x,
                            float y,
                            float z
                            );

void EXPORT call_togl_drawnormal (
                                  float x,
                                  float y,
                                  float z,
                                  float nx,
                                  float ny,
                                  float nz
                                  );

void EXPORT call_togl_drawweight (
                                  float x,
                                  float y,
                                  float z,
                                  float w
                                  );

void EXPORT call_togl_draw_text (
                                 CALL_DEF_TEXT *atext
                                 );

void EXPORT call_togl_draw_structure (
                                      int StructId
                                      );

void EXPORT call_togl_set_linecolor (
                                     float r,
                                     float g,
                                     float b
                                     );

void EXPORT call_togl_set_interiorcolor (
  float r,
  float g,
  float b
  );

void EXPORT call_togl_set_linetype (

                                    long type

                                    );

void EXPORT call_togl_set_interiortype (

                                        long type

                                        );

void EXPORT call_togl_set_linewidth (

                                     float width

                                     );

void EXPORT call_togl_set_textcolor (

                                     float r,
                                     float g,
                                     float b

                                     );

void EXPORT call_togl_set_textfont (

                                    char *police,
                                    float factor,
                                    float space

                                    );

void EXPORT call_togl_set_markercolor (

                                       float r,
                                       float g,
                                       float b

                                       );

void EXPORT call_togl_set_markertype (

                                      long type,
                                      float scale

                                      );

void EXPORT call_togl_set_minmax (

                                  float x1,
                                  float y1,
                                  float z1,
                                  float x2,
                                  float y2,
                                  float z2

                                  );

void EXPORT call_togl_set_rgb (

                               float r,
                               float g,
                               float b

                               );

/* Declarations des subroutines appelees du C++ */
/*    dans les packages Graphic3d et Visual3d   */

void EXPORT call_togl_opengroup (

                                 CALL_DEF_GROUP *agroup

                                 );

void EXPORT call_togl_closegroup (

                                  CALL_DEF_GROUP *agroup

                                  );

void EXPORT call_togl_cleargroup (

                                  CALL_DEF_GROUP *agroup

                                  );

void EXPORT call_togl_linecontextgroup (

                                        CALL_DEF_GROUP *agroup,
                                        int noinsert

                                        );

void EXPORT call_togl_facecontextgroup (

                                        CALL_DEF_GROUP *agroup,
                                        int noinsert

                                        );

void EXPORT call_togl_markercontextgroup (

  CALL_DEF_GROUP *agroup,
  int noinsert

  );

void EXPORT call_togl_textcontextgroup (

                                        CALL_DEF_GROUP *agroup,
                                        int noinsert

                                        );

void EXPORT call_togl_group (

                             CALL_DEF_GROUP *agroup

                             );

void EXPORT call_togl_marker (

                              CALL_DEF_GROUP *agroup,
                              CALL_DEF_MARKER *amarker

                              );

void EXPORT call_togl_marker_set (

                                  CALL_DEF_GROUP *agroup,
                                  CALL_DEF_LISTMARKERS *almarkers

                                  );

void EXPORT call_togl_polyline (

                                CALL_DEF_GROUP *agroup,
                                CALL_DEF_LISTPOINTS *alpoints

                                );

void EXPORT call_togl_polygon (

                               CALL_DEF_GROUP *agroup,
                               CALL_DEF_FACET *afacet

                               );

void EXPORT call_togl_polygon_holes (

                                     CALL_DEF_GROUP *agroup,
                                     CALL_DEF_LISTFACETS *alfacets

                                     );

void EXPORT call_togl_polygon_indices (

                                       CALL_DEF_GROUP *agroup,
                                       CALL_DEF_LISTPOINTS *alpoints,
                                       CALL_DEF_LISTEDGES *aledges,
                                       CALL_DEF_LISTINTEGERS *albounds

                                       );

void EXPORT call_togl_pickid (

                              CALL_DEF_GROUP *agroup

                              );

void EXPORT call_togl_quadrangle (

                                  CALL_DEF_GROUP *agroup,
                                  CALL_DEF_QUAD *aquad

                                  );

void EXPORT call_togl_removegroup (

                                   CALL_DEF_GROUP *agroup

                                   );

void EXPORT call_togl_text (

                            CALL_DEF_GROUP *agroup,
                            CALL_DEF_TEXT *atext

                            );

void EXPORT call_togl_triangle (

                                CALL_DEF_GROUP *agroup,
                                CALL_DEF_TRIKE *atrike

                                );

void EXPORT call_togl_bezier (

                              CALL_DEF_GROUP *agroup,
                              CALL_DEF_LISTPOINTS *alpoints

                              );

void EXPORT call_togl_bezier_weight (

                                     CALL_DEF_GROUP *agroup,
                                     CALL_DEF_LISTPOINTS *alpoints,
                                     CALL_DEF_LISTREALS *alweights

                                     );

void EXPORT call_togl_blink (

                             CALL_DEF_STRUCTURE *astructure,
                             int create

                             );

void EXPORT call_togl_boundarybox (

                                   CALL_DEF_STRUCTURE *astructure,
                                   int create

                                   );

void EXPORT call_togl_clearstructure (

                                      CALL_DEF_STRUCTURE *astructure

                                      );

void EXPORT call_togl_contextstructure (

                                        CALL_DEF_STRUCTURE *astructure

                                        );

void EXPORT call_togl_highlightcolor (

                                      CALL_DEF_STRUCTURE *astructure,
                                      CALL_DEF_COLOR *acolor,
                                      int create

                                      );

void EXPORT call_togl_namesetstructure (

                                        CALL_DEF_STRUCTURE *astructure

                                        );

void EXPORT call_togl_removestructure (

                                       CALL_DEF_STRUCTURE *astructure

                                       );

void EXPORT call_togl_structure (

                                 CALL_DEF_STRUCTURE *astructure

                                 );

void EXPORT call_togl_transformstructure (

  CALL_DEF_STRUCTURE *astructure

  );

void EXPORT call_togl_degeneratestructure (

  CALL_DEF_STRUCTURE *astructure

  );

void EXPORT call_togl_connect (

                               CALL_DEF_STRUCTURE *afather,
                               CALL_DEF_STRUCTURE *ason

                               );

void EXPORT call_togl_disconnect (

                                  CALL_DEF_STRUCTURE *afather,
                                  CALL_DEF_STRUCTURE *ason

                                  );

int EXPORT call_togl_begin_animation (

                                      CALL_DEF_VIEW *aview

                                      );

void EXPORT call_togl_end_animation (

                                     CALL_DEF_VIEW *aview

                                     );

int EXPORT call_togl_begin (

                            char *adisplay

                            );

/*RIC120302*/
int EXPORT call_togl_begin_display (

                                    void *pdisplay

                                    );
/*RIC120302*/

void EXPORT call_togl_end (


                           );

void EXPORT call_togl_structure_exploration (

  long Id,
  long LabelBegin,
  long LabelEnd

  );

void EXPORT call_togl_element_exploration (

  long Id,
  long elem_num,
  long *type,
  CALL_DEF_POINT *pt,
  CALL_DEF_NORMAL *npt,
  CALL_DEF_COLOR *cpt,
  CALL_DEF_NORMAL *nfa

  );

void EXPORT call_togl_element_type (

                                    long Id,
                                    long elem_num,
                                    long *type

                                    );

void EXPORT call_togl_activateview (

                                    CALL_DEF_VIEW *aview

                                    );

void EXPORT call_togl_antialiasing (

                                    CALL_DEF_VIEW *aview,
                                    int tag

                                    );

void EXPORT call_togl_background (

                                  CALL_DEF_VIEW *aview

                                  );

void EXPORT call_togl_gradient_background (

                                           int ws, int type,
                                           tel_colour tcolor1, tel_colour tcolor2
                                          
                                          );

void EXPORT call_togl_set_gradient_type (
 
                                         int ws, int type

                                        );

int EXPORT call_togl_project_raster (

                                     int wsid,
                                     float x,
                                     float y,
                                     float z,
                                     int *ixr,
                                     int *iyr

                                     );

int EXPORT call_togl_adopt_to_rect( 
                                   int,
                                   int,
                                   int 
                                   );

int EXPORT call_togl_unproject_raster (

                                       int wsid,
                                       int xm,
                                       int ym,
                                       int xM,
                                       int yM,
                                       int ixr,
                                       int iyr,
                                       float *x,
                                       float *y,
                                       float *z

                                       );

int EXPORT call_togl_unproject_raster_with_ray (

  int wsid,
  int xm,
  int ym,
  int xM,
  int yM,
  int ixr,
  int iyr,
  float *x,
  float *y,
  float *z,
  float *dx,
  float *dy,
  float *dz

  );

void EXPORT call_togl_cliplimit (

                                 CALL_DEF_VIEW *aview,
                                 int wait

                                 );

void EXPORT call_togl_deactivateview (

                                      CALL_DEF_VIEW *aview

                                      );

void EXPORT call_togl_depthcueing (

                                   CALL_DEF_VIEW *aview,
                                   int tag

                                   );

void EXPORT call_togl_displaystructure (

                                        CALL_DEF_VIEW *aview,
                                        int StructId,
                                        int Priority

                                        );

void EXPORT call_togl_erasestructure (

                                      int ViewId,
                                      int StructId

                                      );

void EXPORT call_togl_init_pick (


                                 );

void EXPORT call_togl_pick (

                            CALL_DEF_PICK *apick

                            );

void EXPORT call_togl_ratio_window (

                                    CALL_DEF_VIEW *aview

                                    );

void EXPORT call_togl_redraw (

                              CALL_DEF_VIEW *aview,
                              CALL_DEF_LAYER *anunderlayer,
                              CALL_DEF_LAYER *anoverlayer

                              );

void EXPORT call_togl_redraw_area (

                                   CALL_DEF_VIEW *aview,
                                   CALL_DEF_LAYER *anunderlayer,
                                   CALL_DEF_LAYER *anoverlayer,
                                   int x, int y, int width, int height

                                   );

void EXPORT call_togl_removeview (

                                  CALL_DEF_VIEW *aview

                                  );

void EXPORT call_togl_setlight (

                                CALL_DEF_VIEW *aview

                                );

void EXPORT call_togl_setplane (

                                CALL_DEF_VIEW *aview

                                );

void EXPORT call_togl_setvisualisation (

                                        CALL_DEF_VIEW *aview

                                        );

void EXPORT call_togl_transparency (

                                    int WsId,
                                    int ViewId,
                                    int Activity

                                    );

void EXPORT call_togl_update (

                              CALL_DEF_VIEW *aview,
                              CALL_DEF_LAYER *anunderlayer,
                              CALL_DEF_LAYER *anoverlayer

                              );

int EXPORT call_togl_view (

                           CALL_DEF_VIEW *aview

                           );

int EXPORT call_togl_viewmapping (

                                  CALL_DEF_VIEW *aview,
                                  int wait

                                  );

int EXPORT call_togl_vieworientation (

                                      CALL_DEF_VIEW *aview,
                                      int wait

                                      );

int EXPORT call_togl_inquiretexture (


                                     );

int EXPORT call_togl_inquirelight (


                                   );

int EXPORT call_togl_inquireplane (


                                   );

int EXPORT call_togl_inquireview (


                                  );

int EXPORT call_togl_inquiremat (

                                 CALL_DEF_VIEW *aview,
                                 CALL_DEF_MATRIX4X4 ori_matrix,
                                 CALL_DEF_MATRIX4X4 map_matrix

                                 );

void EXPORT call_togl_view_exploration (

                                        long Id

                                        );

#ifdef OCC1188
int EXPORT call_togl_create_texture (

                                     int Type,
                                     unsigned int Width,
                                     unsigned int Height,
                                     unsigned char *Data,
                                     char *FileName             

                                     );

void EXPORT call_togl_create_bg_texture (

  CALL_DEF_VIEW *aview,
  int width,
  int height,
  unsigned char *data,
  int style

  );
#endif /* OCC1188 */

void EXPORT call_togl_set_bg_texture_style( 

  CALL_DEF_VIEW* aview,
  int style

  );

void EXPORT call_togl_destroy_texture (

                                       int TexId

                                       );

void EXPORT call_togl_modify_texture (

                                      int TexId,
                                      CALL_DEF_INIT_TEXTURE *init_tex

                                      );

void EXPORT call_togl_environment (

                                   CALL_DEF_VIEW *aview

                                   );           

/* Declarations des subroutines layer mode */

void EXPORT call_togl_layer2d (

                               CALL_DEF_LAYER *alayer

                               );

void EXPORT call_togl_removelayer2d (

                                     CALL_DEF_LAYER *alayer

                                     );

void EXPORT call_togl_begin_layer2d (

                                     CALL_DEF_LAYER *alayer

                                     );

void EXPORT call_togl_end_layer2d (
                                   );

void EXPORT call_togl_clear_layer2d (

                                     CALL_DEF_LAYER *alayer

                                     );

void EXPORT call_togl_begin_polyline2d (
                                        );

void EXPORT call_togl_end_polyline2d (
                                      );

void EXPORT call_togl_begin_polygon2d (
                                       );

void EXPORT call_togl_end_polygon2d (
                                     );

void EXPORT call_togl_move2d (

                              float x,
                              float y

                              );

void EXPORT call_togl_draw2d (

                              float x,
                              float y

                              );

void EXPORT call_togl_edge2d (

                              float x,
                              float y

                              );

void EXPORT call_togl_rectangle2d (

                                   float x,
                                   float y,
                                   float w,
                                   float h

                                   );

void EXPORT call_togl_set_color (

                                 float r,
                                 float g,
                                 float b

                                 );

void EXPORT call_togl_set_transparency (

                                        float a

                                        );

void EXPORT call_togl_unset_transparency (  );

void EXPORT call_togl_set_line_attributes (

                                           long type,
                                           float width

                                           );

void EXPORT call_togl_set_text_attributes (

                                           Tchar* font,
                                           long type,
                                           float r,
                                           float g,
                                           float b

                                           );

void EXPORT call_togl_set_text_attributes (

                                           Tchar* font,
                                           Tint height,
                                           OSD_FontAspect type,
                                           float r,
                                           float g,
                                           float b

                                           );

void EXPORT call_togl_set_text_scale_factors (

                                              Tfloat x,
                                              Tfloat y

                                              );

void EXPORT call_togl_text2d (

                              char *s,
                              float x,
                              float y,
                              float height

                              );

void EXPORT call_togl_textsize2d (

                                  char *s,
                                  float height,
                                  float *width,
                                  float *ascent,
                                  float *descent

                                  );

void EXPORT call_togl_backfacing (

                                  CALL_DEF_VIEW*

                                  );

/* *** group of parray functions *** */

void EXPORT call_togl_parray (

                              CALL_DEF_GROUP *agroup,
                              CALL_DEF_PARRAY* parray

                              );

void EXPORT call_togl_userdraw (

                                CALL_DEF_GROUP    *agroup,
                                CALL_DEF_USERDRAW *anobject

                                );

/* ------------------------- */
void EXPORT call_togl_print (

                             CALL_DEF_VIEW *aview,
                             CALL_DEF_LAYER *anunderlayer,
                             CALL_DEF_LAYER *anoverlayer,
                             const Aspect_Drawable hPrintDC,
                             const int background,
                             const char* filename

                             );


#ifdef BUC61044
void EXPORT call_togl_depthtest (

                                 CALL_DEF_VIEW *aview,
                                 int            dFlag

                                 );

int EXPORT call_togl_isdepthtest (

                                  CALL_DEF_VIEW *aview

                                  );
#endif

#ifdef BUC61045
void EXPORT call_togl_gllight (

                               CALL_DEF_VIEW *aview,
                               int            dFlag

                               );

int EXPORT call_togl_isgllight (

                                CALL_DEF_VIEW *aview

                                );
#endif

/*----------------------------------------------------------------------*/

#endif
