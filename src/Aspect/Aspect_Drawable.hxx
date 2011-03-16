/*============================================================================*/
/*==== Titre: Aspect_Drawable.hxx                                               */
/*==== Role : The header file of primitive type "Handle" from packages        */
/*==== "Xw" & "WNT"                                                           */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Aspect_Drawable_HeaderFile
#define _Aspect_Drawable_HeaderFile

#ifdef WNT
 typedef void*        Aspect_Drawable;	/* HDC under WNT */
#else
 typedef unsigned long Aspect_Drawable;	/* Window or Pixmap under UNIX */
#endif /* WNT */

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Macro.hxx>
class Handle(Standard_Type);
const Handle(Standard_Type)& STANDARD_TYPE(Aspect_Drawable);

/*============================================================================*/
#endif

#endif /* _Aspect_Drawable_HeaderFile */
