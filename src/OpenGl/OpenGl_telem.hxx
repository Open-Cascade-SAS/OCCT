/***********************************************************************

FONCTION :
----------
Include OpenGl_telem :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-02-96 : FMN ; Ajout prototype de TsmInitAttributes()
11-03-96 : FMN ; Ajout prototype de TelResetMaterial()
13-06-96 : CAL ; Ajout prototype de TelSetTransparency()
05-08-97 : PCT ; support texture mapping
21-08-97 : PCT ; ajout deuxieme passe pour les textures
28-08-97 : PCT ; ajot coordonnee textures
15-01-98 : FMN ; Ajout Hidden line
08-04-98 : FGU ; Ajout champs pour emission
24-06-98 : FMN ; Correction indice pour les type de trait. 
La reference se trouve dans Aspect.cdl
16-06-2000 : ATS : G005 : Move type declarations to InterfaceGraphic_telem.h


************************************************************************/

#ifndef  OPENGL_TELEM_H
#define  OPENGL_TELEM_H

#include <InterfaceGraphic_telem.hxx>

extern  void  TelHookOnAllClasses(void);
extern  Tchar*TelGetStringForPM( Tint, Tfloat ); /* marker type, marker size */
extern  void  TelUpdateMaterial( Tint ); /* TEL_FRONT_MATERIAL or
                                         TEL_BACK_MATERIAL */
/* FSXXX  NEW */
extern  void  TelForceMaterial( Tint ); 
extern  void  TsmInitAttributes( void );
extern  void  TelResetMaterial( void );

extern  void  TelSetTransparency( Tint );

#endif  /* OPENGL_TELEM_H */
