/*============================================================================*/
/*==== Titre: Graphic3d_CBitFields20.hxx				      */
/*==== Role : The header file of primitive type "CBitFields20" from Graphic3d */
/*====									      */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Graphic3d_CBitFields20_HeaderFile
#define _Graphic3d_CBitFields20_HeaderFile

typedef struct {

	unsigned bool1	:1;
	unsigned bool2	:1;
	unsigned bool3	:1;
	unsigned bool4	:1;

	unsigned bool5	:1;
	unsigned bool6	:1;
	unsigned bool7	:1;
	unsigned bool8	:1;

	unsigned bool9	:1;
	unsigned bool10	:1;
	unsigned bool11	:1;
	unsigned bool12	:1;

	unsigned bool13	:1;
	unsigned bool14	:1;
	unsigned bool15	:1;
	unsigned bool16	:1;

	unsigned bool17	:1;
	unsigned bool18	:1;
	unsigned bool19	:1;
	unsigned bool20	:1;

} Graphic3d_CBitFields20;

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/
#include <Standard_Type.hxx> 
const Handle(Standard_Type)&  STANDARD_TYPE(Graphic3d_CBitFields20);
/*============================================================================*/

#endif
#endif /*Graphic3d_CBitFields20_HeaderFile*/
