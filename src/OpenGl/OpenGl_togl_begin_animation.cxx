/***********************************************************************

FONCTION :
----------
File OpenGl_togl_begin_animation :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
05-01-98 : CAL ; Creation.
21-01-98 : FMN ; Ajout call_togl_end_animation().
17-03-98 : FMN ; Ajout mode animation
08-07-98 : FMN ; Changement du nom de la primitive call_togl_erase_immediat_mode()
-> call_togl_erase_animation_mode.
24-08-98 : BGN ; Correction de diag de compilation sur NT :
Suppression de l'include OpenGl_animation.h qui 
provoque des doubles declarations.

************************************************************************/

#define G003    /* EUG 07-10-99 Animation management 
*/

#define BUC60876        /* GG 5/4/2001 Disable local display list
//                      when animation is not required
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/ 

#include <OpenGl_tgl_all.hxx>

#include <InterfaceGraphic_Labels.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

/* 24-08-98 : BGN ;  include <OpenGl_animation.h> */

#include <GL/gl.h>
#ifdef G003
# include <OpenGl_tsm_ws.hxx>
void  EXPORT call_togl_erase_animation_mode ( void );
#endif  /* G003 */

/*----------------------------------------------------------------------*/
/*
* Constantes
*/ 

#define NO_DEBUG


/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/ 

/* Indique si la display-list a ete cree */
static GLboolean    listIndexCreate = GL_FALSE; 

/*----------------------------------------------------------------------*/
/*
* Variables externes
*/ 

/* La display-list utilisee pour le mode animation  */
GLuint      listIndex = 0;

/* La vue de reference  pour le mode animation  */    
int     listIndexView = -1;   

/* Flag si la display-list a ete initialise*/
GLboolean   listIndexFlag = GL_FALSE; 

/* Flag mode animation en cours  */
GLboolean   animationFlag = GL_FALSE; 

/* Flag si le mode animation est disponible 
* (cf. var environnement CALL_ANIMATION)  */   
#ifdef G003
GLboolean   animationUseFlag = GL_TRUE;  
GLboolean g_fAnimation;
GLboolean g_fUpdateAM;
GLboolean g_fDegeneration;
GLboolean g_fList;
#else
GLboolean   animationUseFlag = GL_FALSE;  
#endif  /* G003 */

/*----------------------------------------------------------------------*/

int EXPORT
call_togl_begin_animation
(
 CALL_DEF_VIEW * aview
 )
{
#ifdef G003
  g_fAnimation               = GL_TRUE;
  g_fList                    = GL_FALSE;
  g_fDegeneration            = aview -> IsDegenerates;
  g_fUpdateAM                = ( aview -> IsDegenerates ==
    aview -> IsDegeneratesPrev
    ) ? GL_FALSE : GL_TRUE;
  aview -> IsDegeneratesPrev = aview -> IsDegenerates;
#endif  /* G003 */

  if (animationUseFlag)
  {
    if (!listIndexCreate) 
    {
      listIndex = glGenLists(1);
      listIndexCreate = GL_TRUE;
    }

    if (listIndex == 0) 
      animationFlag = GL_FALSE;
    else
      animationFlag = GL_TRUE;
  }
#ifdef DEBUG
  printf("call_togl_begin_animation(%d) animationFlag: %d\n", aview->WsId, animationFlag);
#endif
  return (animationFlag);
}

/*----------------------------------------------------------------------*/

void EXPORT
call_togl_end_animation
(
 CALL_DEF_VIEW * aview
 )
{
#ifdef G003
  CMN_KEY_DATA data;

#ifdef BUC60876
  if( !g_fAnimation ) return;
#else
  if( !animationFlag ) return;
#endif

  data.ldata = TNotDone;
  TsmSetWSAttri ( aview -> WsId, WSUpdateState, &data );
  g_fAnimation    = GL_FALSE;
#ifndef BUC60876
  g_fUpdateAM     = GL_TRUE;
#endif
  g_fDegeneration = GL_FALSE;
#endif  /* G003 */

#ifdef DEBUG
  printf("call_togl_end_animation(%d) \n", aview->WsId); 
#endif
  animationFlag = GL_FALSE;

}

/*----------------------------------------------------------------------*/
/*
* Permet de terminer effectivement le mode Animation.
* Ceci est realise des qu'une structure a ete modifiee.
*/

void  EXPORT call_togl_erase_animation_mode(void)
{
#ifdef DEBUG
  printf("call_togl_erase_animation_mode \n"); 
#endif
  listIndexView = -1;
  listIndexFlag = GL_FALSE;   
  animationFlag = GL_FALSE;
#ifdef G003
  g_fAnimation    = GL_FALSE;
  g_fUpdateAM     = GL_FALSE;
  g_fDegeneration = GL_FALSE;
  g_fList         = GL_FALSE;
#endif  /* G003 */
}

/*----------------------------------------------------------------------*/
