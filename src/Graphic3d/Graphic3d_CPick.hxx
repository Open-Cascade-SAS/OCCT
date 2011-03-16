/*============================================================================*/
/*==== Titre: Graphic3d_CPick.hxx					      */
/*==== Role : The header file of primitive type "CPick" from Graphic3d       */
/*====									      */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Graphic3d_CPick_HeaderFile
#define _Graphic3d_CPick_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
typedef CALL_DEF_PICK Graphic3d_CPick;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Type.hxx>
const Handle(Standard_Type)& TYPE(Graphic3d_CPick);
/*============================================================================*/

#endif
#endif /*Graphic3d_CPick_HeaderFile*/
