/*============================================================================*/
/*==== Titre: Graphic3d_CLight.hxx					      */
/*==== Role : The header file of primitive type "CLight" from Graphic3d       */
/*====									      */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Graphic3d_CLight_HeaderFile
#define _Graphic3d_CLight_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
typedef CALL_DEF_LIGHT Graphic3d_CLight;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Type.hxx>
const Handle(Standard_Type)& TYPE(Graphic3d_CLight);
/*============================================================================*/

#endif
#endif /*Graphic3d_CLight_HeaderFile*/
