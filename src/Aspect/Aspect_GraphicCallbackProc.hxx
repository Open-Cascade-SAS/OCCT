/*============================================================================*/
/*==== Titre: Aspect_GraphicCallbackProc.hxx                                               */
/*==== Role : The header file of primitive type "GraphicCallbackProc" from package        */
/*==== "V3d"                                                           */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Aspect_GraphicCallbackProc_HeaderFile
#define _Aspect_GraphicCallbackProc_HeaderFile
#include <Aspect_Display.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_RenderingContext.hxx>

#define OCC_REDRAW_WINDOW 1
#define OCC_REDRAW_WINDOWAREA 2
#define OCC_REDRAW_BITMAP 3

/* 
   This flag, when bitwise OR`ed with the "reason" value, 
   informs the callback that it is called before redrawing the overlayer .
   Otherwise, the callback is invoked after the overlayer is redrawn.        
*/
#define OCC_PRE_OVERLAY 0x8000

 typedef struct {
   int reason;
   int wsID;
   int viewID;
   Aspect_Display display;
   Aspect_Drawable window;
   Aspect_RenderingContext gcontext;
 } Aspect_GraphicCallbackStruct;

 typedef int (*Aspect_GraphicCallbackProc)(
    Aspect_Drawable   	/* Window ID */,
    void*           	/* user data */, 
    Aspect_GraphicCallbackStruct*  /* call data */ 
 );

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Macro.hxx>
class Handle(Standard_Type);
const Handle(Standard_Type)& STANDARD_TYPE(Aspect_GraphicCallbackProc);

/*============================================================================*/
#endif

#endif /* _Aspect_GraphicCallbackProc_HeaderFile */
