/*============================================================================*/
/*==== Titre: Aspect_Handle.hxx                                               */
/*==== Role : The header file of primitive type "Handle" from packages        */
/*==== "Xw" & "WNT"                                                           */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Aspect_Handle_HeaderFile
#define _Aspect_Handle_HeaderFile

#ifdef WNT
 typedef void*         HANDLE;
 typedef HANDLE        Aspect_Handle;
#else
 typedef unsigned long Aspect_Handle;
#endif /* WNT */

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Macro.hxx>
class Handle(Standard_Type);
const Handle(Standard_Type)& STANDARD_TYPE(Aspect_Handle);

/*============================================================================*/
#endif

#endif /* _Aspect_Handle_HeaderFile */
