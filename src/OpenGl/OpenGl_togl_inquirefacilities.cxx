/***********************************************************************

FONCTION :
----------
File OpenGl_togl_inquirefacilities :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
12-02-97 : FMN ; Suppression TelEnquireFacilities()
En fait la plupart de ces valeurs ne servent a rien.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_telem_util.hxx>

int call_togl_inquirelight (void);

CALL_DEF_INQUIRE call_facilities_list;

void EXPORT
call_togl_inquirefacilities ()
{
  call_facilities_list.AntiAliasing = 1;
  call_facilities_list.DepthCueing  = 1;
  call_facilities_list.DoubleBuffer = 1;
  call_facilities_list.ZBuffer  = 1;

  call_togl_inquirelight();
  call_togl_inquireplane();
  call_togl_inquireview();

  return;
}
