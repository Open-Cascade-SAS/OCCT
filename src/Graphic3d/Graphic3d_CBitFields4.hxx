/*===========================================================================*/
/*==== Titre: Graphic3d_CBitFields4.hxx				             */
/*==== Role : The header file of primitive type "CBitFields4" from Graphic3d */
/*====									     */
/*==== Implementation:  This is a primitive type implemented with typedef    */
/*===========================================================================*/

#ifndef _Graphic3d_CBitFields4_HeaderFile
#define _Graphic3d_CBitFields4_HeaderFile

typedef struct {

	unsigned bool1	:1;
	unsigned bool2	:1;
	unsigned bool3	:1;
	unsigned bool4	:1;

} Graphic3d_CBitFields4;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/
#include <Standard_Type.hxx> 
const Handle(Standard_Type)&  STANDARD_TYPE(Graphic3d_CBitFields4);
/*============================================================================*/

#endif
#endif /*Graphic3d_CBitFields4_HeaderFile*/
