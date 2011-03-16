/*============================================================================*/
/*==== Titre: Aspect_RenderingContext.hxx                                               */
/*==== Role : The header file of primitive type "RenderingContext" from package        */
/*==== "V3d"                                                           */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Aspect_RenderingContext_HeaderFile
#define _Aspect_RenderingContext_HeaderFile

 typedef void*	       Aspect_RenderingContext;	/* GLXContext under UNIX */
						/* HGLRC under WNT */

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Macro.hxx>
class Handle(Standard_Type);
const Handle(Standard_Type)& STANDARD_TYPE(Aspect_RenderingContext);

/*============================================================================*/
#endif

#endif /* _Aspect_RenderingContext_HeaderFile */
