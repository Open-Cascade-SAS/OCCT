/*============================================================================*/
/*==== Titre: Graphic3d_CView.hxx					      */
/*==== Role : The header file of primitive type "CView" from Graphic3d       */
/*====									      */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Graphic3d_CView_HeaderFile
#define _Graphic3d_CView_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
typedef CALL_DEF_VIEW Graphic3d_CView;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/
#include <Standard_Type.hxx>
const Handle(Standard_Type)& TYPE(Graphic3d_CView);
/*============================================================================*/

#endif
#endif /*Graphic3d_CView_HeaderFile*/
