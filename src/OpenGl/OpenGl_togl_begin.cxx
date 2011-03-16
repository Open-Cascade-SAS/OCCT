/***********************************************************************

FONCTION :
----------
file OpenGl_togl_begin.c :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
21-01-97 : CAL : Ajout temporaire d'un XSynchronize pour
eviter un SEG V lors de la destruction d'une vue.
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h
25-03-97 : CAL : getenv de CSF_GraphicSync
04-11-97 : FMN : ajout InitExtensionGLX();
17-03-98 : FMN ; Ajout mode animation
24-11-98 : FMN ; Correction sur la gestion de la perspective (cf Programming Guide)
22-12-98 : FMN ; Rename CSF_WALKTHROW en CSF_WALKTHROUGH
08/04/02 : GG ; RIC120302 Add call_togl_begin_display() function

************************************************************************/

#define G003  /* GG 25-01-00 Enable animation always ...
//    See V3d_View::SetAnimationMode()
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <stdlib.h>

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_Extension.hxx>
#include <OpenGl_animation.hxx>

/*----------------------------------------------------------------------*/
/*
* Variable globales
*/

EXPORT GLboolean env_walkthrow = GL_FALSE;
/* OCC18942: The new symmetric perspective projection matrix is off by default */
EXPORT GLboolean env_sym_perspective = GL_FALSE;

/*----------------------------------------------------------------------*/
/*

Opens and initialises Graphic Library environment.
Returns true  if the Graphic Library environment is open.
Returns false if the Graphic Library environment is closed (error).

*/

int EXPORT
call_togl_begin
(
 char *adisplay
 )
{
  char *pvalue;

  call_togl_set_environment (adisplay);
#ifndef WNT
  if ((pvalue = getenv ("CSF_GraphicSync")) != NULL)
    XSynchronize (call_thedisplay, 1);
  InitExtensionGLX(call_thedisplay);
#endif

  /* Test si mode workthrough oui/non */
  if ((pvalue = getenv ("CSF_WALKTHROUGH")) != NULL)
    env_walkthrow = GL_TRUE;

  /* Activation/Desactivation du mode Animation */
#ifndef G003
  if ((pvalue = getenv ("CALL_ANIMATION")) != NULL)
    animationUseFlag = GL_TRUE;
  else
    animationUseFlag = GL_FALSE;
#endif

  /* OCC18942: Test if symmetric perspective projection should be turned on */
  if ((pvalue = getenv ("CSF_SYM_PERSPECTIVE")) != NULL)
    env_sym_perspective = GL_TRUE;

  call_togl_inquirefacilities ();
  call_func_init_tgl ();
  return (1);
}

/*RIC120302*/
/*

Sets the Graphic Library environment.
Returns true  if the Graphic Library environment is open.
Returns false if the Graphic Library environment is closed (error).


*/

int EXPORT
call_togl_begin_display
(
 void *pdisplay
 )
{
  char *pvalue = NULL;
  int status;

  status = call_tox_set_display (pdisplay);
  if( status ) {
#ifndef WNT
    if ((pvalue = getenv ("CSF_GraphicSync")) != NULL)
      XSynchronize (call_thedisplay, 1);
    InitExtensionGLX(call_thedisplay);
#endif
    call_togl_inquirefacilities ();
    call_func_init_tgl ();
  }
  return (status);
}
/*RIC120302*/
