/***********************************************************************

FONCTION :
----------
File OpenGl_tgl_subrvis :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
17-07-96 : FMN ; Modification de call_subr_hlhsr et call_util_setvisualization
29-01-97 : FMN ; Suppression de call_subr_doublebuffer().

************************************************************************/

#ifndef  OPENGL_TGL_SUBRVIS_H
#define  OPENGL_TGL_SUBRVIS_H

#ifndef RIC120302
#define RIC120302 /* GG Add call_subr_displayCB function
*/
#endif

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_telem.hxx>

#ifdef RIC120302
extern  int      call_subr_displayCB(CALL_DEF_VIEW* /*aview*/, int /*reason*/ );
#endif
extern  int      call_subr_open_ws(CALL_DEF_VIEW *); /* aview */
extern  void     call_subr_enable_polygon_offset(); 
extern  void     call_subr_disable_polygon_offset(); 
extern  void     call_subr_resize(CALL_DEF_VIEW *); /* aview */
extern  void     call_subr_antialiasing(CALL_DEF_VIEW *, Tint); /* aview, tag */
extern  void     call_subr_hlhsr(CALL_DEF_VIEW *, Tint); /* aview, flag */
extern  void     call_subr_depthcueing(CALL_DEF_VIEW *, Tint); /* aview, tag */
extern  TStatus  call_subr_get_view_index(Tint, Tint *); /* stid, &vid */
extern  void     call_subr_pick(CALL_DEF_PICK *); /* apick */
extern  void     call_subr_set_light_src_rep(CALL_DEF_LIGHT *); /* alight */
extern  void     call_subr_set_background(Tint, tel_colour); /* ws, rgb */
extern  void     call_subr_set_gradient_background(Tint, Tint, tel_colour, tel_colour); /* ws, type, rgb ,rgb */
extern  void     call_subr_close_ws(CALL_DEF_VIEW *);
extern  void     call_util_setvisualization(CALL_DEF_VIEW *, int, int);
extern  void     call_subr_transparency (int , int , int );

#endif
