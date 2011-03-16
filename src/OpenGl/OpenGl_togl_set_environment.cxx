/***********************************************************************

FONCTION :
----------
file OpenGl_togl_set_environment :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
29-04-96 : FMN ; Correction Warning.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <string.h>

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_utilgr.hxx>


void EXPORT
call_togl_set_environment
(
 char * adisplay
 )
{
  int status;

  if (call_thedisplay == NULL) {
    if (strlen (adisplay) != 0)
      call_util_osd_putenv ((Tchar*)"DISPLAY", (Tchar*)adisplay);
    status = call_tox_open_display ();
  }
  return;
}
