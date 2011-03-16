/***********************************************************************

FONCTION :
----------
File OpenGl_togl_pick :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
06-03-96 : FMN ; Ajout OpenGl_tgl_tox.h pour call_thedisplay
01-04-96 : CAL ; Integration MINSK portage WNT
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>


/*----------------------------------------------------------------------*/

void EXPORT
call_togl_pick
(
 CALL_DEF_PICK *apick
 )
{
  call_tox_rect( apick->WsId,
    TxglGetSubWindow( call_thedisplay, (WINDOW)apick->DefWindow.XWindow)
    , apick->x, apick->y );
  apick->Pick.depth = 0;
  if( apick->x <= apick->DefWindow.dx &&
    apick->y <= apick->DefWindow.dy )
    call_subr_pick( apick );
  return;
}
/*----------------------------------------------------------------------*/
