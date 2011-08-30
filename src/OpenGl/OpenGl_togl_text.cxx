
#include <math.h>
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_utilgr.hxx>

void EXPORT
call_togl_text
(
 CALL_DEF_GROUP * agroup,
 CALL_DEF_TEXT * atext
 )
{
  Tint path;
  Tfloat up_vect[2];
  TEL_POINT text_pt;
  Tint hor_align, ver_align;

  up_vect[0] = ( float )cos (atext->Angle);
  up_vect[1] = ( float )sin (atext->Angle);

  switch (atext->Path) {
    case 0: /* Graphic3d_TP_UP */
      path = CALL_PHIGS_PATH_UP;
     break;
    case 1: /* Graphic3d_TP_DOWN */
      path = CALL_PHIGS_PATH_DOWN;
      break;
    case 2: /* Graphic3d_TP_LEFT */
      path = CALL_PHIGS_PATH_LEFT;
      break;
    case 3: /* Graphic3d_TP_RIGHT */
      path = CALL_PHIGS_PATH_RIGHT;
      break;
    default:
      path = CALL_PHIGS_PATH_RIGHT;
      break;
  }

  switch (atext->HAlign) {
    case 0: /* Graphic3d_HTA_LEFT */
      hor_align = CALL_PHIGS_HOR_LEFT;
      break;    
    case 1: /* Graphic3d_HTA_CENTER */
      hor_align = CALL_PHIGS_HOR_CENTER;
      break;    
    case 2: /* Graphic3d_HTA_RIGHT */
      hor_align = CALL_PHIGS_HOR_RIGHT;
      break;    
    default:
      hor_align = CALL_PHIGS_HOR_LEFT;
      break;
  }

  switch (atext->VAlign) {
    case 0: /* Graphic3d_VTA_BOTTOM */
      ver_align = CALL_PHIGS_VERT_BOTTOM;
      break;
    case 1: /* Graphic3d_VTA_CENTER */
      ver_align = CALL_PHIGS_VERT_CENTER;
      break;
    case 2: /* Graphic3d_VTA_TOP */
      ver_align = CALL_PHIGS_VERT_TOP;
      break;
    default:
      ver_align = CALL_PHIGS_VERT_BOTTOM;
      break;
  }

  text_pt.xyz[0] = atext->Position.x;
  text_pt.xyz[1] = atext->Position.y;
  text_pt.xyz[2] = atext->Position.z;

  if (! agroup->IsOpen) call_togl_opengroup (agroup);
  call_func_set_anno_char_ht (atext->Height);
  call_func_set_anno_char_up_vec (up_vect);
  call_func_set_anno_path (path);
  call_func_set_anno_align (hor_align, ver_align);
  call_func_anno_text_rel3 (&text_pt, (Techar*)atext->string);
  if (! agroup->IsOpen) call_togl_closegroup (agroup);
}
