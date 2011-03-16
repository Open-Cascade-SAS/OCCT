/***********************************************************************

FONCTION :
----------
file OpenGl_togl_displaystructure.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h
10-04-97 : CAL ; Modification pour que last draw => last execute.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/


#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_view.hxx>

void EXPORT
call_togl_displaystructure
(
 CALL_DEF_VIEW *aview,
 int StructId,
 int Priority
 )
{
  int labelPriority;

  switch (Priority) {
    case 0 : labelPriority = View_LABPriority01; break;
    case 1 : labelPriority = View_LABPriority02; break;
    case 2 : labelPriority = View_LABPriority03; break;
    case 3 : labelPriority = View_LABPriority04; break;
    case 4 : labelPriority = View_LABPriority05; break;
    case 5 : labelPriority = View_LABPriority06; break;
    case 6 : labelPriority = View_LABPriority07; break;
    case 7 : labelPriority = View_LABPriority08; break;
    case 8 : labelPriority = View_LABPriority09; break;
    case 9 : labelPriority = View_LABPriority10; break;
    case 10: labelPriority = View_LABEnd; break;

    default: labelPriority = Priority; break;
  }

  call_func_set_edit_mode (CALL_PHIGS_EDIT_INSERT);
  call_func_open_struct (aview->ViewId);
  call_func_set_elem_ptr (0);
  call_func_set_elem_ptr_label (labelPriority);
  call_func_offset_elem_ptr (-1);
  call_func_exec_struct (StructId);
  call_func_close_struct ();
  return;
}
