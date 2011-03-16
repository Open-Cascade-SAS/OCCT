/***********************************************************************

FONCTION :
----------
File OpenGl_triedron.h : gestion du triedre non zoomable.


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
24-10-98 : BGN ; Creation.

************************************************************************/

/*----------------------------------------------------------------------*/

#ifndef __OPENGL_TRIEDRON_H_
#define __OPENGL_TRIEDRON_H_

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#include <GL/gl.h>

#include <OpenGl_tgl_all.hxx>
#include <OpenGl_telem.hxx>
#include <OpenGl_tsm.hxx>


/*----------------------------------------------------------------------*/
/*
* Constantes
*/


/*----------------------------------------------------------------------*/
/*
* Types definis
*/


/*----------------------------------------------------------------------*/
/*
* variables statiques
*/ 


/*----------------------------------------------------------------------*/
/*
* Prototypes
*/
/*
* initialisation of zbuffer trihedron
*/
extern TStatus  call_ztriedron_setup (
                                      float* xcolor,
                                      float* ycolor,
                                      float* zcolor,
                                      float  sizeratio,
                                      float  axisdiameter,
                                      int    nbfacettes          
                                      );

/*
* initialisation d'un triedre non zoomable dans la Wks
*/
extern TStatus  call_triedron_init (
                                    CALL_DEF_VIEW * aview, 
                                    int aPosition, 
                                    float r,
                                    float g,
                                    float b, 
                                    float aScale,
                                    int asWireframe);


/*
* affichage d'un triedre non zoomable dans la vue aview.
*/

extern TStatus call_triedron_redraw_from_view (CALL_DEF_VIEW * aview);


/*
* affichage d'un triedre non zoomable dans la workstation awsid
*/

extern TStatus call_triedron_redraw_from_wsid (Tint awsid);


/*
* destruction  du triedre non zoomable d' une vue.
*/

extern TStatus call_triedron_erase (CALL_DEF_VIEW * aview);

/*
* gestion d'un echo de designation du triedre non zoomable d' une vue.
*/
extern TStatus call_triedron_echo (
                                   CALL_DEF_VIEW * aview,
                                   int aType);


/*----------------------------------------------------------------------*/

#endif /* __OPENGL_TRIEDRON_H_ */

/*----------------------------------------------------------------------*/

