/*============================================================================*/
/*==== Titre: Aspect_Display.hxx                                               */
/*==== Role : The header file of primitive type "Display" from package        */
/*==== "V3d"                                                           */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Aspect_Display_HeaderFile
#define _Aspect_Display_HeaderFile

 typedef void*	       Aspect_Display;	/* Display* under UNIX */

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Macro.hxx>
class Handle(Standard_Type);
const Handle(Standard_Type)& STANDARD_TYPE(Aspect_Display);

/*============================================================================*/
#endif

#endif /* _Aspect_Display_HeaderFile */
