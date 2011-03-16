/***********************************************************************

FONCTION :
----------
Include OpenGl_txgl :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-02-96 : FMN ; Suppression code inutile:
- TxglLink() et TxglUnlink()
Ajout prototype de TxglGetBackDither()
18-07-96 : FMN ; Suppression code inutile: TxglSetWindow().
29-01-97 : FMN ; Suppression de TxglSetDbuff().

************************************************************************/

#ifndef  OPENGL_TXGL_H
#define  OPENGL_TXGL_H

#define RIC120302 /* GG  ; Add NEW TxglSetWindow function
//      Add NEW TxglGetContext function
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

# include <InterfaceGraphic.hxx>

/*----------------------------------------------------------------------*/
/*
* Prototypes fonctions externes
*/

extern  WINDOW  TxglCreateWindow( DISPLAY*, WINDOW, /* x, y, w, h, bw, bgcol */
                                 Tint, Tint, Tint, Tint, Tint, Tfloat, Tfloat, Tfloat );
extern  TStatus TxglWinset( DISPLAY*, WINDOW );
#ifdef RIC120302
extern  WINDOW  TxglSetWindow( DISPLAY*, WINDOW, GLCONTEXT );
extern  GLCONTEXT TxglGetContext( WINDOW );
#endif
extern  WINDOW  TxglGetSubWindow( DISPLAY*, WINDOW );
extern  void    TxglDestroyWindow( DISPLAY *, WINDOW );
extern  int     TxglGetDither(void);
extern  int TxglGetBackDither(void);

/*----------------------------------------------------------------------*/

#endif
