/***********************************************************************

FONCTION :
----------
File OpenGl_togl_inquirelight.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
30-06-97 : FMN ; Appel de la toolkit OpenGl_LightBox.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_LightBox.hxx>

int EXPORT
call_togl_inquirelight ()
{
  return call_facilities_list.MaxLights = OpenGLMaxLights;
}
