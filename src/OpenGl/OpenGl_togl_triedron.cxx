/***********************************************************************

FONCTION :
----------
File OpenGl_togl_triedron :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
17-09-98 : FMN/BGN ; Creation des points d'entree.
14-12-98 : BGN; Developpement (S3989, Phase "triedre")
15-12-98 : BGN; Ajout des WSUpdateState .

************************************************************************/

/*----------------------------------------------------------------------*/
/*
* Includes
*/


#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

/* pour utiliser WSUpdateState et  TsmSetWSAttri */
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_tsm_ws.hxx>

#include <OpenGl_triedron.hxx>

/*----------------------------------------------------------------------*/
/* 
* Constantes
*/

#define NO_PRINT

/*----------------------------------------------------------------------*/

void EXPORT 
call_togl_ztriedron_setup 
(
 float* xcolor,
 float* ycolor,
 float* zcolor,
 float  sizeratio,
 float  axisdiameter,
 int    nbfacettes          
 )
{
  call_ztriedron_setup(xcolor, ycolor, zcolor, 
    sizeratio, axisdiameter, nbfacettes);
}

void EXPORT
call_togl_triedron_display
(
 CALL_DEF_VIEW * aview, 
 int   aPosition, 
 float r,
 float g,
 float b, 
 float aScale, 
 int   asWireframe 
 )
{

  CMN_KEY_DATA  data;
  TStatus status;

#ifdef PRINT
  printf("call_togl_triedron_display  r = %f, g = %f, b = %f", r, g, b);
  printf(", aScale = %f, aPosition = %d \n", aScale, aPosition );
#endif
  status = call_triedron_init (aview, aPosition, r, g, b, aScale, asWireframe);

  /* Mise a jour de l'update_mode */
  data.ldata = TNotDone;
  TsmSetWSAttri(aview->WsId, WSUpdateState, &data );


  return;
}


void EXPORT
call_togl_triedron_erase
(
 CALL_DEF_VIEW * aview   
 )
{

  CMN_KEY_DATA  data;

#ifdef PRINT
  printf("call_togl_triedron_erase \n");
#endif
  call_triedron_erase (aview);

  /* Mise a jour de l'update_mode */
  data.ldata = TNotDone;
  TsmSetWSAttri(aview->WsId, WSUpdateState, &data );

  return;
}


void EXPORT
call_togl_triedron_echo
(
 CALL_DEF_VIEW * aview, 
 int AType 
 )
{
#ifdef PRINT
  printf("call_togl_triedron_echo \n");
#endif
  return;
}
