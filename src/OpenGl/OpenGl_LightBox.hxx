/***********************************************************************

FONCTION :
----------
Gestion des light sous OpenGL


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
20-06-97 : PCT ; creation
30-06-97 : FMN ; Integration
18-07-97 : FMN ; Ajout IsLightOn()
02-10-97 : FMN ; Ajout gl.h pour WNT

************************************************************************/

/*----------------------------------------------------------------------*/

#ifndef __OPENGL_LIGHTBOX_H_
#define __OPENGL_LIGHTBOX_H_

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

#define  OpenGLMaxLights    8

/*----------------------------------------------------------------------*/
/*
* Types definis
*/

typedef  struct
{
  TLightType  type;
  int   HeadLight;
  TEL_COLOUR  col;
  Tfloat      pos[3];
  Tfloat      dir[3];
  Tfloat      shine;
  Tfloat      atten[2];
  Tfloat      angle;
} TEL_LIGHT, *tel_light;


/*----------------------------------------------------------------------*/
/*
* Prototypes
*/

/*
* Ajout d'une lumiere dans la Wks
*/
extern TStatus AddLight(Tint WksID, Tint LightID, tel_light light);

/*
* Maj des lumieres de la Wks
*/
extern TStatus UpdateLight(Tint WksID);

/*
* Remove une lumiere de la Wks
*/
extern TStatus RemoveLight(Tint WksID, Tint LightID);

/*
* Remove des lumieres de la Wks
*/
extern TStatus RemoveWksLight(Tint WksID);

/*
* Reset de toutes les lights d'une Wks 
*/
extern TStatus ResetWksLight(Tint WksID);

/*
* Enable des lights
*/
extern void LightOn(void);

/*
* Disable des lights
*/
extern void LightOff(void);

/*
* IsEnable des lights
*/
extern GLboolean IsLightOn(void);

/*----------------------------------------------------------------------*/

#endif /* __OPENGL_LIGHTBOX_H_ */

/*----------------------------------------------------------------------*/
