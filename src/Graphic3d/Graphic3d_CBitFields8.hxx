/*===========================================================================*/
/*==== Titre: Graphic3d_CBitFields8.hxx				             */
/*==== Role : The header file of primitive type "CBitFields8" from Graphic3d */
/*====									     */
/*==== Implementation:  This is a primitive type implemented with typedef    */
/*===========================================================================*/

#ifndef _Graphic3d_CBitFields8_HeaderFile
#define _Graphic3d_CBitFields8_HeaderFile

typedef struct {

	unsigned bool1	:1;
	unsigned bool2	:1;
	unsigned bool3	:1;
	unsigned bool4	:1;

	unsigned bool5	:1;
	unsigned bool6	:1;
	unsigned bool7	:1;
	unsigned bool8	:1;

} Graphic3d_CBitFields8;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/
#include <Standard_Type.hxx> 
const Handle(Standard_Type)&  STANDARD_TYPE(Graphic3d_CBitFields8);
/*============================================================================*/

#endif
#endif /*Graphic3d_CBitFields8_HeaderFile*/
