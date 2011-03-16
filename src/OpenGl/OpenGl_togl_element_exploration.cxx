/***********************************************************************

FONCTION :
----------
File OpenGl_togl_element_exploration :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
11-03-96 : FMN ; Correction warning compilation
01-04-96 : CAL ; Integration MINSK portage WNT

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 


#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_tgl_elems.hxx>

void EXPORT
call_togl_element_exploration
(
 long Id,
 long elem_num,
 long *type,
 CALL_DEF_POINT *pt,
 CALL_DEF_NORMAL *npt,
 CALL_DEF_COLOR *cpt, 
 CALL_DEF_NORMAL *nfa
 )
{
  call_subr_structure_exploration (Id, 0, 0, elem_num, (Tint *)type,
    pt, npt, cpt, nfa);

  switch( *type )
  {
  case CALL_PHIGS_ELEM_POLYLINE_SET3_DATA :
    *type = 1;
    break;

  case CALL_PHIGS_ELEM_FILL_AREA3 :
    *type = 2;
    break;

  case CALL_PHIGS_ELEM_TRI_STRIP3_DATA :
    *type = 3;
    break;

  case CALL_PHIGS_ELEM_QUAD_MESH3_DATA :
    *type        = 4;
    break;

  case CALL_PHIGS_ELEM_ANNO_TEXT_REL3 :
    *type = 5;
    break;

  case CALL_PHIGS_ELEM_POLYMARKER3 :
    *type = 6;
    break;

  default :
    *type = 0;
    break;
  }
  return;
}
