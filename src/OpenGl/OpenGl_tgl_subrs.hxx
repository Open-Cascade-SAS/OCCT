/*
File OpenGl_tgl_subrs.h

Modified 16/06/2000 : ATS,SPK : G005 : Group of functions 
call_subr_parray_xxx
call_subr_darray_xxx
*/

#ifndef  OPENGL_TGL_SUBRS_H
#define  OPENGL_TGL_SUBRS_H

#include <InterfaceGraphic_Graphic3d.hxx>

#include <OpenGl_telem.hxx>

extern  void  call_subr_set_line_colr(Tfloat, Tfloat, Tfloat); /* r, g, b */
extern  void  call_subr_polyline(CALL_DEF_LISTPOINTS *); /* alpoints */
extern  void  call_subr_polyline_data(CALL_DEF_LISTPOINTS *); /* alpoints */
extern  void  call_subr_addnameset(Tint, Tint *); /* num, names */
extern  void  call_subr_remnameset(Tint, Tint *); /* num, names */

extern  void  call_subr_set_int_colr(Tfloat, Tfloat, Tfloat, Tfloat);/* r, g, b, a */
extern  void  call_subr_set_back_int_colr(Tfloat, Tfloat, Tfloat, Tfloat);
extern  void  call_subr_set_text_colr(Tfloat, Tfloat, Tfloat); /* r, g, b */
extern  void  call_subr_set_text_colr_subtitle(Tfloat, Tfloat, Tfloat); /* r, g, b */
extern  void  call_subr_set_edge_colr(Tfloat, Tfloat, Tfloat); /* r, g, b */
extern  void  call_subr_set_marker_colr(Tfloat, Tfloat, Tfloat); /* r, g, b */
extern  void  call_subr_set_refl_props(CALL_DEF_MATERIAL *, Tint); /* mat, tag */
extern  void  call_subr_marker(CALL_DEF_MARKER *); /* amarker */
extern  void  call_subr_marker_set(CALL_DEF_LISTMARKERS *); /* almarkers */

extern  TStatus  call_subr_get_exec_struct(Tint, Tint, Tint *); /* F, S, &ind */
extern  void  call_subr_polygon_data(CALL_DEF_FACET *); /* afacet */
extern  void  call_subr_polygon(CALL_DEF_FACET *); /* afacet */
extern  void  call_subr_polygon_holes(CALL_DEF_LISTFACETS *); /* alfacets */
extern  void  call_subr_polygon_holes_data(CALL_DEF_LISTFACETS *); /* alfacets */
extern  void  call_subr_polygon_indices(CALL_DEF_LISTPOINTS *,    /* alpoints */
                                        CALL_DEF_LISTEDGES *,     /* aledges */
                                        CALL_DEF_LISTINTEGERS *); /* albounds */
extern  void  call_subr_polygon_set(CALL_DEF_LISTFACETS *); /* alfacets */
extern  void  call_subr_quadrangle(CALL_DEF_QUAD *); /* aquad */
extern  void  call_subr_polygon_holes(CALL_DEF_LISTFACETS *); /* alfacets */

extern  void  call_subr_polygon_holes_data(CALL_DEF_LISTFACETS *); /*alfacets*/

extern  void  call_subr_triangle(CALL_DEF_TRIKE *); /* atrike */
extern  void  call_subr_set_highl_rep(Tint); /* index */
extern  void  call_subr_structure_exploration(Tint,              /*Id*/
                                              Tint,              /*LabelBegin*/
                                              Tint,              /*LabelEnd*/
                                              Tint,              /*elem_num*/
                                              Tint*,             /*elem_type*/
                                              CALL_DEF_POINT *,  /*pt*/
                                              CALL_DEF_NORMAL *, /*npt*/
                                              CALL_DEF_COLOR *,  /*cpt*/
                                              CALL_DEF_NORMAL *  /*nfa*/); 
/* struct_id, ws_ids */
extern  Tint  call_subr_inquirews(Tint, Tint *);
extern  void  call_subr_curve(CALL_DEF_LISTPOINTS *);

/*  PARRAY functions */

extern void call_subr_parray( CALL_DEF_PARRAY *parray );
extern void call_subr_userdraw( CALL_DEF_USERDRAW *anobject );

#endif
