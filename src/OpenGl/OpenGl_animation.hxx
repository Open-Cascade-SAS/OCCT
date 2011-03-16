/***********************************************************************

FONCTION :
----------
File OpenGl_animation.h :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
17-03-98 : FMN ; Creation.
08-07-98 : FMN ; Changement du nom de la primitive call_togl_erase_immediat_mode()
-> call_togl_erase_animation_mode.
15-07-98 : FMN ; Portage Optimizer

************************************************************************/

#ifndef  OPENGL_ANIMATION_H
#define  OPENGL_ANIMATION_H

/*----------------------------------------------------------------------*/
/*
* Include
*/

#include <GL/gl.h>

/*----------------------------------------------------------------------*/
/*
* Fonctions externes
*/

extern  void  call_togl_erase_animation_mode(void);


/*----------------------------------------------------------------------*/
/*
* Variables externes
*/

/* La display-list utilisee pour le mode animation  */
extern GLuint    listIndex;

/* La vue de reference  pour le mode animation  */    
extern int     listIndexView;

/* Flag si la display-list a ete initialise*/
extern GLboolean   listIndexFlag;

/* Flag mode animation en cours  */
extern GLboolean   animationFlag;

/* Flag si le mode animation est disponible 
* (cf. var environnement CALL_ANIMATION)  */   
extern GLboolean   animationUseFlag;  

/*----------------------------------------------------------------------*/

#endif /* OPENGL_ANIMATION_H */
