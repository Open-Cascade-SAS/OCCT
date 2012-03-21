// Copyright (c) 1991-1999 Matra Datavision
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


/*
 * Attention, ce fichier passe par InterfaceGraphic_Cextern.awk.
 * Il faut donc respecter le format de declaration des fonctions.
 * Voir les regles precedees par !
 *
 * <type_retourne> EXPORT call_<nom_fonction> (
 * #ifdef INTERFACEGRAPHIC_PROTOTYPE  ! ne pas changer son nom mais facultatif
 *  <type_argument> <nom_argument>, ! virgule collee au nom
 *  <type_argument> *<nom_pointeur> ! * collee au nom
 *          ! une declaration d'argument par ligne
 * #endif       ! facultatif
 * );         ! ); ensembles sur une ligne
 *          !    et en premiere colonne
 *
 * Merci, CAL
 *
 * Fonctions enlevees le 6/3/97
 *
 * call_togl_light
 * call_togl_plane
 * call_togl_text3
 * call_togl_light_exploration
 * call_togl_build_matrix
 * call_tox_xgrigri
 * call_tox_set_colormap_mapping
 * call_util_osd_getenv
 *
 * 5/08/97 ; PCT : ajout texture mapping
 * 17/09/98 ; FMN/BGN : ajout (S3819) des Ponts d'entree du Triedre.
 * 15/11/99 ; GG PRO603 Add call_togl_redraw_area
 * 24/01/00 ; EUG G003 Add call_togl_degeneratestructure() and
 *                         call_togl_backfacing() functions.
 * 27/0302  ; GG RIC120302 Add call_togl_begin_display function
 * 16/06/2000 : ATS : G005 : Add function call_togl_parray
 * 23/10/01 ; SAV Add call_togl_depthtest()
 *                    call_togl_isdepthtest()
 * 25/10/01 ; SAV Add call_togl_gllight()
 *                    call_togl_isgllight()
 * 23/12/02 ; SAV Added call_togl_create_bg_texture() and 
 *            call_togl_set_bg_texture_style() to set background image
 * 15/08/05 ; SZV Added call_togl_userdraw()
 *
 */

#ifndef InterfaceGraphic_CexternHeader
#define InterfaceGraphic_CexternHeader

#if defined (__STDC__) || defined (__cplusplus) || defined (c_plusplus)
#define INTERFACEGRAPHIC_PROTOTYPE 1
#endif

#define InterfaceGraphic_True   1
#define InterfaceGraphic_False    0

#define InterfaceGraphic_Success  1
#define InterfaceGraphic_Failure  0

#define OCC1188 /* SAV : added methods to set background image */

#include <InterfaceGraphic_X11.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <Graphic3d_CGraduatedTrihedron.hxx>

#define  BUC61044    /* 25/10/01 SAV ; added functionality to control gl depth testing
      from higher API */
#define  BUC61045    /* 25/10/01 SAV ; added functionality to control gl lighting 
      from higher API */

  /* Declarations  des subroutines  triedron */

void EXPORT call_togl_ztriedron_setup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float* xcolor,
  float* ycolor,
  float* zcolor,
  float  sizeratio,
  float  axisdiameter,
  int    nbfacettes          
#endif
);

void EXPORT call_togl_triedron_display (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW * aview, 
  int APosition, 
  float r,
  float g,
  float b,
  float AScale,
  int asWireframe 
#endif
);

void EXPORT call_togl_triedron_erase (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_triedron_echo (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int AType
#endif
);

  /* Declarations  des subroutines  mode immediat */

int EXPORT call_togl_begin_ajout_mode (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_end_ajout_mode (
);

int EXPORT call_togl_begin_immediat_mode (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  CALL_DEF_LAYER *anunderlayer,
  CALL_DEF_LAYER *anoverlayer,
  int doublebuffer,
  int   retainmode
#endif
);

void EXPORT call_togl_end_immediat_mode (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int synchronize
#endif
);

void EXPORT call_togl_clear_immediat_mode (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int aFlush
#endif
);

void EXPORT call_togl_transform (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_MATRIX4X4 amatrix,
  int mode
#endif
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
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float z
#endif
);

void EXPORT call_togl_movenormal (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float z,
  float nx,
  float ny,
  float nz
#endif
);

void EXPORT call_togl_moveweight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float z,
  float w
#endif
);

void EXPORT call_togl_draw (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float z
#endif
);

void EXPORT call_togl_drawnormal (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float z,
  float nx,
  float ny,
  float nz
#endif
);

void EXPORT call_togl_drawweight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float z,
  float w
#endif
);

void EXPORT call_togl_draw_text (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_TEXT *atext
#endif
);

void EXPORT call_togl_draw_structure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int StructId
#endif
);

void EXPORT call_togl_set_linecolor (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float r,
  float g,
  float b
#endif
);

void EXPORT call_togl_set_interiorcolor (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float r,
  float g,
  float b
#endif
);

void EXPORT call_togl_set_linetype (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long type
#endif
);

void EXPORT call_togl_set_interiortype (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long type
#endif
);

void EXPORT call_togl_set_linewidth (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float width
#endif
);

void EXPORT call_togl_set_textcolor (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float r,
  float g,
  float b
#endif
);

void EXPORT call_togl_set_textfont (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  char *police,
  float factor,
  float space
#endif
);

void EXPORT call_togl_set_markercolor (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float r,
  float g,
  float b
#endif
);

void EXPORT call_togl_set_markertype (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long type,
  float scale
#endif
);

void EXPORT call_togl_set_minmax (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x1,
  float y1,
  float z1,
  float x2,
  float y2,
  float z2
#endif
);

void EXPORT call_togl_set_rgb (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float r,
  float g,
  float b
#endif
);

  /* Declarations des subroutines appelees du C++ */
  /*    dans les packages Graphic3d et Visual3d   */

void EXPORT call_togl_opengroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup
#endif
);

void EXPORT call_togl_closegroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup
#endif
);

void EXPORT call_togl_cleargroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup
#endif
);

void EXPORT call_togl_linecontextgroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  int noinsert
#endif
);

void EXPORT call_togl_facecontextgroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  int noinsert
#endif
);

void EXPORT call_togl_markercontextgroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  int noinsert
#endif
);

void EXPORT call_togl_textcontextgroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  int noinsert
#endif
);

void EXPORT call_togl_group (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup
#endif
);

void EXPORT call_togl_marker (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_MARKER *amarker
#endif
);

void EXPORT call_togl_marker_set (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_LISTMARKERS *almarkers
#endif
);

void EXPORT call_togl_polyline (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_LISTPOINTS *alpoints
#endif
);

void EXPORT call_togl_polygon (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_FACET *afacet
#endif
);

void EXPORT call_togl_polygon_holes (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_LISTFACETS *alfacets
#endif
);

void EXPORT call_togl_polygon_indices (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_LISTPOINTS *alpoints,
  CALL_DEF_LISTEDGES *aledges,
  CALL_DEF_LISTINTEGERS *albounds
#endif
);

void EXPORT call_togl_pickid (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup
#endif
);

void EXPORT call_togl_quadrangle (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_QUAD *aquad
#endif
);

void EXPORT call_togl_removegroup (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup
#endif
);

void EXPORT call_togl_text (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_TEXT *atext
#endif
);

void EXPORT call_togl_triangle (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_TRIKE *atrike
#endif
);

void EXPORT call_togl_bezier (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_LISTPOINTS *alpoints
#endif
);

void EXPORT call_togl_bezier_weight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_LISTPOINTS *alpoints,
  CALL_DEF_LISTREALS *alweights
#endif
);

void EXPORT call_togl_blink (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure,
  int create
#endif
);

void EXPORT call_togl_boundarybox (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure,
  int create
#endif
);

void EXPORT call_togl_clearstructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_contextstructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_highlightcolor (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure,
  CALL_DEF_COLOR *acolor,
  int create
#endif
);

void EXPORT call_togl_namesetstructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_removestructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_structure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_transformstructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_degeneratestructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
        CALL_DEF_STRUCTURE *astructure
#endif
);

void EXPORT call_togl_connect (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *afather,
  CALL_DEF_STRUCTURE *ason
#endif
);

void EXPORT call_togl_disconnect (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_STRUCTURE *afather,
  CALL_DEF_STRUCTURE *ason
#endif
);

int EXPORT call_togl_begin_animation (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_end_animation (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

int EXPORT call_togl_begin (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  char *adisplay
#endif
);

/*RIC120302*/
int EXPORT call_togl_begin_display (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  void *pdisplay
#endif
);
/*RIC120302*/

void EXPORT call_togl_end (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
#endif
);

void EXPORT call_togl_structure_exploration (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long Id,
  long LabelBegin,
  long LabelEnd
#endif
);

void EXPORT call_togl_element_exploration (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long Id,
  long elem_num,
  long *type,
  CALL_DEF_POINT *pt,
  CALL_DEF_NORMAL *npt,
  CALL_DEF_COLOR *cpt,
  CALL_DEF_NORMAL *nfa
#endif
);

void EXPORT call_togl_element_type (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long Id,
  long elem_num,
  long *type
#endif
);

void EXPORT call_togl_activateview (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_antialiasing (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int tag
#endif
);

void EXPORT call_togl_background (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_gradient_background
(
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int ws, int type,
  tel_colour color1,
  tel_colour color2
#endif
);

void EXPORT call_togl_set_gradient_type
(
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int ws, int type
#endif
);  

int EXPORT call_togl_project_raster (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int wsid,
  float x,
  float y,
  float z,
  int *ixr,
  int *iyr
#endif
);

int EXPORT call_togl_unproject_raster (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
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
#endif
);

int EXPORT call_togl_unproject_raster_with_ray (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
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
#endif
);

void EXPORT call_togl_cliplimit (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int wait
#endif
);

void EXPORT call_togl_deactivateview (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_depthcueing (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int tag
#endif
);

void EXPORT call_togl_displaystructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int StructId,
  int Priority
#endif
);

void EXPORT call_togl_erasestructure (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int ViewId,
  int StructId
#endif
);

void EXPORT call_togl_init_pick (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
#endif
);

void EXPORT call_togl_pick (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_PICK *apick
#endif
);

void EXPORT call_togl_ratio_window (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_redraw (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  CALL_DEF_LAYER *anunderlayer,
  CALL_DEF_LAYER *anoverlayer
#endif
);

void EXPORT call_togl_redraw_area (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  CALL_DEF_LAYER *anunderlayer,
  CALL_DEF_LAYER *anoverlayer,
  int x, int y, int width, int height
#endif
);

void EXPORT call_togl_removeview (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_setlight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_setplane (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_setvisualisation (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_transparency (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int WsId,
  int ViewId,
  int Activity
#endif
);

void EXPORT call_togl_update (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  CALL_DEF_LAYER *anunderlayer,
  CALL_DEF_LAYER *anoverlayer
#endif
);

int EXPORT call_togl_view (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

int EXPORT call_togl_viewmapping (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int wait
#endif
);

int EXPORT call_togl_vieworientation (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int wait
#endif
);

int EXPORT call_togl_inquiretexture (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
#endif
);

int EXPORT call_togl_inquirelight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
#endif
);

int EXPORT call_togl_inquireplane (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
#endif
);

int EXPORT call_togl_inquireview (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
#endif
);

int EXPORT call_togl_inquiremat (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  CALL_DEF_MATRIX4X4 ori_matrix,
  CALL_DEF_MATRIX4X4 map_matrix
#endif
);

void EXPORT call_togl_view_exploration (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long Id
#endif
);

#ifdef OCC1188
int EXPORT call_togl_create_texture (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int Type,
  unsigned int Width,
  unsigned int Height,
  unsigned char *Data,
  char *FileName             
#endif
);

void EXPORT call_togl_create_bg_texture (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int width,
  int height,
  unsigned char *data,
    int style
#endif
);
#endif /* OCC1188 */

void EXPORT call_togl_set_bg_texture_style( 
#ifdef INTERFACEGRAPHIC_PROTOTYPE
    CALL_DEF_VIEW* aview,
    int style
#endif
);

void EXPORT call_togl_destroy_texture (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int TexId
#endif
);

void EXPORT call_togl_modify_texture (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  int TexId,
  CALL_DEF_INIT_TEXTURE *init_tex
#endif
);

void EXPORT call_togl_environment (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

  /* Declarations des subroutines layer mode */

void EXPORT call_togl_layer2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_LAYER *alayer
#endif
);

void EXPORT call_togl_removelayer2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_LAYER *alayer
#endif
);

void EXPORT call_togl_begin_layer2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_LAYER *alayer
#endif
);

void EXPORT call_togl_end_layer2d (
);

void EXPORT call_togl_clear_layer2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_LAYER *alayer
#endif
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
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y
#endif
);

void EXPORT call_togl_draw2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y
#endif
);

void EXPORT call_togl_edge2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y
#endif
);

void EXPORT call_togl_rectangle2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float x,
  float y,
  float w,
  float h
#endif
);

void EXPORT call_togl_set_color (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float r,
  float g,
  float b
#endif
);

void EXPORT call_togl_set_transparency (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  float a
#endif
);

void EXPORT call_togl_unset_transparency (
);

void EXPORT call_togl_set_line_attributes (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  long type,
  float width
#endif
);

void EXPORT call_togl_set_text_attributes (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  char* font,
  long type,
  float r,
  float g,
  float b
#endif
);

void EXPORT call_togl_text2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  char *s,
  float x,
  float y,
  float height
#endif
);

void EXPORT call_togl_textsize2d (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  char *s,
  float height,
  float *width,
  float *ascent,
  float *descent
#endif
);

void EXPORT call_togl_backfacing (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
             CALL_DEF_VIEW*
#endif  /* INTERFACEGRAPHIC_PROTOTYPE */
            );

/* *** group of parray functions *** */

void EXPORT call_togl_parray (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP *agroup,
  CALL_DEF_PARRAY* parray
#endif
);

void EXPORT call_togl_userdraw (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_GROUP    *agroup,
  CALL_DEF_USERDRAW *anobject
#endif
);

/* ------------------------- */
void EXPORT call_togl_print (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  CALL_DEF_LAYER *anunderlayer,
  CALL_DEF_LAYER *anoverlayer,
  const Aspect_Drawable hPrintDC,
  const int background,
  const char* filename
#endif
);


#ifdef BUC61044
void EXPORT call_togl_depthtest (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int            dFlag
#endif
);

int EXPORT call_togl_isdepthtest (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);
#endif

#ifdef BUC61045
void EXPORT call_togl_gllight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview,
  int            dFlag
#endif
);

int EXPORT call_togl_isgllight (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
  CALL_DEF_VIEW *aview
#endif
);

void EXPORT call_togl_graduatedtrihedron_display (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
    /* View id */
	CALL_DEF_VIEW* aview, 
    /* Graduated trihedron data */
    const Graphic3d_CGraduatedTrihedron &cubic
#endif
);

void EXPORT call_togl_graduatedtrihedron_erase (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
    /* View id */
	CALL_DEF_VIEW* aview
#endif
);

void EXPORT call_togl_graduatedtrihedron_minmaxvalues (
#ifdef INTERFACEGRAPHIC_PROTOTYPE
    float xmin,
    float ymin,
    float zmin,
    float xmax,
    float ymax,
    float zmax
#endif
);

#endif

#endif /* InterfaceGraphic_CexternHeader */
