/***********************************************************************

FONCTION :
----------
File OpenGl_togl_structure_exploration :


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

void EXPORT
call_togl_structure_exploration
(
 long Id,
 long LabelBegin,
 long LabelEnd
 )
{
  Tint            type;
  CALL_DEF_POINT  pt;
  CALL_DEF_NORMAL npt, nfa;
  CALL_DEF_COLOR  cpt;

  call_subr_structure_exploration (Id, LabelBegin, LabelEnd, 0, &type, &pt, &npt, &cpt, &nfa);

  return;
}
