
#ifndef WNT
/***********************************************************************

FONCTION :
----------
File OpenGl_tox :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
20-02-96 : FMN ; Suppression code inutile:
on ne garde que  :
- call_tox_open_display()
- call_tox_getscreen(Tint *d_wdth, Tint *d_hght)
01-04-96 : CAL ; Integration MINSK portage WNT
20-11-97 : CAL ; RererereMerge avec le source de portage.
12-02-01 : GG  ; BUC60821 Add getpitchsize() function
27-03-02 : GG  ; RIC120202 Add new function call_tox_set_display

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <InterfaceGraphic_Aspect.hxx>

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_util.hxx>

/*----------------------------------------------------------------------*/
/*
* Variables globales
*/

Display * call_thedisplay; /* display pointer; defined in tgl/tox */

/*----------------------------------------------------------------------*/
/*
*Fonctions externes
*/

/*----------------------------------------------------------------------*/

Tint
call_tox_open_display (void)
{
  char synchro[132];

  if (call_thedisplay == NULL)
    /*
    * display_name
    * Specifies the hardware display name, which determines the
    * display and communications domain to be used.
    * On a POSIX system, if the display_name is NULL, it defaults
    * to the value of the DISPLAY environment variable.
    */
    call_thedisplay = XOpenDisplay ((char*) NULL);

  if (call_thedisplay != NULL)
    if (call_util_osd_getenv ("CALL_SYNCHRO_X", synchro, 132))
      XSynchronize (call_thedisplay, 1);
    else
      XSynchronize (call_thedisplay, 0);
  else
    fprintf (stderr, "Cannot connect to X server %s\n",
    XDisplayName ((char*) NULL));

  return (call_thedisplay == NULL);
}

/*RIC120302*/
Tint
call_tox_set_display (void *vdisplay)
{
  Display *pdisplay = (Display*) vdisplay;
  char synchro[132];

  if ( pdisplay ) {
    call_thedisplay = pdisplay;

    if (call_util_osd_getenv ("CALL_SYNCHRO_X", synchro, 132))
      XSynchronize (call_thedisplay, 1);
    else
      XSynchronize (call_thedisplay, 0);
  } else {
    fprintf (stderr, "Cannot connect to X server %s\n",
      XDisplayName ((char*) NULL));
  }
  return (call_thedisplay != NULL);
}
/*RIC120302*/

/*----------------------------------------------------------------------*/

void
call_tox_getscreen( Tint *d_wdth, Tint *d_hght )
{
  *d_wdth = DisplayWidth( call_thedisplay, DefaultScreen(call_thedisplay) );
  *d_hght = DisplayHeight( call_thedisplay, DefaultScreen(call_thedisplay) );
  return;
}

/*----------------------------------------------------------------------*/

Tfloat
call_tox_getpitchsize( )
{
  static float pitch_size = 0.;

  if( pitch_size == 0. ) {
    int width = DisplayWidth( call_thedisplay, DefaultScreen(call_thedisplay) );
    int height = DisplayHeight( call_thedisplay, DefaultScreen(call_thedisplay) );
    int mmwidth = DisplayWidthMM( call_thedisplay, DefaultScreen(call_thedisplay) );
    int mmheight = DisplayHeightMM( call_thedisplay, DefaultScreen(call_thedisplay) );
    pitch_size = ((float)mmwidth/width + (float)mmheight/height)/2.;
  }

  return pitch_size;
}

/*----------------------------------------------------------------------*/
#else
# include <windows.h>
# include <OpenGl_tgl_all.hxx>
# include <OpenGl_txgl.hxx>

DISPLAY* call_thedisplay = "DISPLAY";  /* Dummy for Windows NT */

Tint
call_tox_open_display ( void )
{
  return 1;
}

/*RIC120302*/
Tint
call_tox_set_display ( void* vdisplay)
{
  return 1;
}
/*RIC120302*/

void
call_tox_getscreen ( Tint *d_wdth, Tint *d_hght )
{
  *d_wdth = GetSystemMetrics(SM_CXSCREEN);
  *d_hght = GetSystemMetrics(SM_CYSCREEN);
  return;
}

#endif  /* WNT */
