/*============================================================================*/
/*==== Titre: Graphic3d_CTexture.hxx				              */
/*==== Role : The header file of primitive type "CTexture" from Graphic3d     */
/*====									      */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*==== created: 1/07/97 ; PCT : texture mapping                               */
/*============================================================================*/

#ifndef _Graphic3d_CTexture_HeaderFile
#define _Graphic3d_CTexture_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
typedef CALL_DEF_TEXTURE Graphic3d_CTexture;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/
#include <Standard_Type.hxx>
const Handle(Standard_Type)& TYPE(Graphic3d_CTexture);
/*============================================================================*/

#endif
#endif /*Graphic3d_CTexture_HeaderFile*/
