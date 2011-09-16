/***********************************************************************

FONCTION :
----------
File OpenGl_txgl :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-02-96 : FMN ; Suppression code inutile:
- TxglLink() et TxglUnlink()
08-03-96 : FMN ; Suppression variables globales
Ajout cmn_delete_from_htbl() dans TxglDestroyWindow()
21-03-96 : CAL ; test sur previous_ctx dans TxglDestroyWindow()
et dans TxglSetDbuff()
01-04-96 : CAL ; Integration MINSK portage WNT
15-04-96 : CAL ; Integration travail PIXMAP de Jim ROTH
26-04-96 : FMN ; Correction warning de compilation
20-06-96 : CAL ; Retrait du XDestroyWindow dans TxglDestroyWindow
18-07-96 : FMN ; Suppression code inutile: TxglSetWindow().
27-09-96 : CAL ; Portage WNT
16-10-96 : GG  ; Coder le parametre de GLX_DEPTH_SIZE a 1 plutot
que 0 si l'on souhaite accroitre les performances
de 50% en utilisant le ZBuffer hardware !!!
Si la fenetre fournie a deja le bon visual pas
la peine de creer une sous-fenetre.
16-10-96 : GG  ; Le dithering doit etre active aussi avec 12 plans
de maniere a ameliorer la qualite
17-10-96 : FMN ; Ajout fonction printVisualInfo()
06-11-96 : CAL ; Remise a True du BackDitherProp pour < 12 plans
12-11-96 : CAL ; BackDitherProp = True pour <= 8 plans
BackDitherProp = False pour > 8 plans
29-01-97 : FMN ; Amelioration du tests pour le dithering
DitherProp = True pour <= 8 plans red
DitherProp = False pour > 8 plans red
Suppression de TxglSetDbuff()
06-06-97 : FMN ; Meilleure gestion glXMakeCurrent (pb avec LightWoks)
Suppression de previous_win
02-07-97 : FMN ; Suppression variable ESSAI
07-10-97 : FMN ; Simplification WNT 
13-10-97 : FMN ; Ajout wglShareLists
06-02-98 : FMN ; PRO11674: Suppression XSetErrorHandler(0) inutile
23-11-98 : CAL ; PRO16603: previous_ctx jamais remis a 0 pour eviter
la perte des lists.
07-12-98 : CAL ; PRO 16311 et PRO 11821
02.14.100 : JR : Warnings on WNT
14.07.06 : SAN : OCC12977: update previous_ctx properly in TxglDestroyWindow.
Old code resulted in crashes on some ATI Radeon cards under Linux.

************************************************************************/

#define BUC60691  /*GG 06/06/00 Due to a severe bug in secondary 
//      table hash-code computation not yet solve,
//      It's necessary to compute the primary hash-key key
//      correctly under WNT/W98. The actual method is wrong
//      because a size 4 is used for this table instead
//      a conventional prime number as under UNIX system (23).
//       Under W98 sometimes the function wglMakeContext() does
//      not work for an UNKNOWN reason, the number of DC
//      seems limited to 5 but nothing tell that the limit is
//      reached !
//      We try right now to recover this error by creating a new DC.
*/

#define RIC120303 /*GG Add new function TxglSetWindow using
//      the GLXContext defined by the user
//      Add new function TxglGetContext.
*/

#define OCC954  /*SAV: 13/11/02 - check GLRC before deleting it*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_telem_util.hxx>
#include <stdio.h>

#include <OpenGl_Memory.hxx>
#include <OpenGl_ResourceCleaner.hxx>


#ifdef WNT
struct HTBL_ENTRY {
  HDC   hDC;
  HGLRC hGLRC;
  int   nUsed;
  IMPLEMENT_MEMORY_OPERATORS
};


int call_util_osd_getenv ( char*, char*, int );
#endif  /* WNT */

#include <OpenGl_txgl.hxx>
int call_util_osd_getenv( char * , char * , int ) ;

/*----------------------------------------------------------------------*/
/*
* Variables statiques
*/

#ifndef WNT
typedef NCollection_DataMap<Tint, GLCONTEXT> GLContextMap;
#else
typedef NCollection_DataMap<Tint, HTBL_ENTRY*> GLContextMap;
#endif

static GLContextMap _Txgl_Map;


#ifndef WNT
static  int BackDitherProp = False; /* Dithering pour le background */
static  int DitherProp = True;    /* Dithering pour le trace  */
static  GLXContext previous_ctx = 0;  /* Use for share display list */
static  GLXContext dead_ctx;            /* Context to be destroyed */
static  Display *dead_dpy;              /* Display associated with dead_ctx */
#else
static  int BackDitherProp = FALSE; /* Dithering pour le background */
static  int DitherProp = TRUE;    /* Dithering pour le trace  */
static  BOOL s_sysPalInUse;             /* Flag to check system colors usage */ 
static  HGLRC previous_ctx = 0;   /* Use for share display list */
#endif /* WNT */

/*----------------------------------------------------------------------*/
/*
* Constantes
*/

#define NO_TRACE

#define CALL_DEF_STRING_LENGTH 132

#define WIN_HTBL_SIZE 23

/*----------------------------------------------------------------------*/
/*
* Fonctions statiques
*/

#ifndef WNT
#ifdef TRACE
static GLvoid printVisualInfo( Display *, XVisualInfo *glxVisual );
#endif
#else
#ifdef BUC60691
static BOOL win95 = FALSE;
#endif

static int find_pixel_format(HTBL_ENTRY * hte, PIXELFORMATDESCRIPTOR * pfd);

__declspec( dllexport ) int __fastcall __OpenGl_INIT__ ( 
  unsigned hInstance, unsigned long reason_for_call
  ) {
    if ( reason_for_call == DLL_PROCESS_ATTACH ) {

    }
    return 1;

  }  /* end __OpenGl_INIT__ */
#endif  /* WNT */

  /*----------------------------------------------------------------------*/

  WINDOW
    TxglCreateWindow( DISPLAY  *disp, WINDOW par,
    Tint x, Tint y, Tint w, Tint h, Tint bw,
    Tfloat bgcolr, Tfloat bgcolg, Tfloat bgcolb )
  {

#ifndef WNT

    GLCONTEXT ctx;
    Colormap cmap;
    XVisualInfo* vis=NULL;
    /*    XVisualInfo tmplt;*/
    XSetWindowAttributes cwa;
    XColor color;
    int value;
    char string[CALL_DEF_STRING_LENGTH];
    WINDOW win;

    int DBuffer = (call_util_osd_getenv ("CALL_OPENGL_NO_DBF", string, CALL_DEF_STRING_LENGTH)) ? False : True;

    if (call_util_osd_getenv("JWR_PIXMAP_DB", string, CALL_DEF_STRING_LENGTH))
      TelSetPixmapDB(1);

    XWindowAttributes wattr;
    XGetWindowAttributes (disp, par, &wattr);
    Tint scr = DefaultScreen (disp);

#if defined(__linux) || defined(Linux)
    {
      XVisualInfo aVisInfo;
      int aNbItems;
      int isGl, isDoubleBuffer, isRGBA, aDepthSize;
      unsigned long aVisInfoMask = VisualIDMask | VisualScreenMask;
      aVisInfo.visualid = wattr.visual->visualid;
      aVisInfo.screen   = DefaultScreen (disp);
      vis = XGetVisualInfo (disp, aVisInfoMask, &aVisInfo, &aNbItems);
      if (vis != NULL)
      {
        // check Visual for OpenGl context's parameters compability
        if (glXGetConfig (disp, vis, GLX_USE_GL, &isGl) != 0)
          isGl = 0;

        if (glXGetConfig (disp, vis, GLX_RGBA, &isRGBA) != 0)
          isRGBA = 0;

        if (glXGetConfig (disp, vis, GLX_DOUBLEBUFFER, &isDoubleBuffer) != 0)
          isDoubleBuffer = 0;

        if (glXGetConfig (disp, vis, GLX_DEPTH_SIZE, &aDepthSize) != 0)
          aDepthSize = 0;

        if (!isGl || !aDepthSize || !isRGBA  || isDoubleBuffer != DBuffer)
        {
          XFree (vis);
          vis = NULL;
        }
      }
    }
#endif

    if (vis == NULL)
    {
      int anIter = 0;
      int anAttribs[11];
      anAttribs[anIter++] = GLX_RGBA;

      anAttribs[anIter++] = GLX_DEPTH_SIZE;
      anAttribs[anIter++] = 1;

      anAttribs[anIter++] = GLX_RED_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      anAttribs[anIter++] = GLX_GREEN_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      anAttribs[anIter++] = GLX_BLUE_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      if (DBuffer)
        anAttribs[anIter++] = GLX_DOUBLEBUFFER;

      anAttribs[anIter++] = None;

      vis = glXChooseVisual (disp, scr, anAttribs);
      if (vis == NULL) return TFailure;
    }

#ifdef TRACE
    printf ("TxglCreateWindow \n");
    printf ("Informations sur le visual\n");
    printf ("par visualid %x%x %d\n", wattr.visual->visualid, wattr.visual->visualid);
    printf ("vis visualid 0x%x %d\n", vis->visualid, vis->visualid);
    printf ("vis depth %d\n", vis->depth);
    printf ("vis class %d\n", vis->class);
    printf ("vis red_mask %ld\n", vis->red_mask);
    printf ("vis green_mask %ld\n", vis->green_mask);
    printf ("vis blue_mask %ld\n", vis->blue_mask);
    printf ("vis colormap_size %d\n", vis->colormap_size);
    printf ("vis bits_per_rgb %d\n", vis->bits_per_rgb);
    printVisualInfo( disp, vis );
#endif

    /*
    * Le BackDitherProp est utilise pour le clear du background
    * Pour eviter une difference de couleurs avec la couleur choisie
    * par l'application (XWindow) il faut desactiver le dithering
    * au dessus de 8 plans.
    * 
    * Pour le DitherProp:
    * On cherchera a activer le Dithering que si le Visual a au moins
    * 8 plans pour le GLX_RED_SIZE. Le test est plus sur car on peut
    * avoir une profondeur superieure a 12 mais avoir besoin du dithering.
    * (Carte Impact avec GLX_RED_SIZE a 5 par exemple)
    */

    glXGetConfig( disp, vis, GLX_RED_SIZE, &value );
    DitherProp = (value < 8) ? True : False;
    BackDitherProp = (vis->depth <= 8) ? True : False;

#ifdef TRACE
    printf("Dithering %d BackDithering %d \n",DitherProp,BackDitherProp);
#endif

    if (call_util_osd_getenv ("CALL_OPENGL_NO_DITHER", string, CALL_DEF_STRING_LENGTH))
      DitherProp = False;

    if (call_util_osd_getenv ("CALL_OPENGL_NO_BACKDITHER", string, CALL_DEF_STRING_LENGTH))
      BackDitherProp = False;

    if (dead_ctx) {
      /* recover display lists from dead_ctx, then destroy it */
      ctx = glXCreateContext( disp, vis, dead_ctx, GL_TRUE );
      glXDestroyContext(dead_dpy, dead_ctx);
    } else if (previous_ctx == 0) {
      ctx = glXCreateContext( disp, vis, NULL, GL_TRUE );
    } else {
      /* ctx est une copie du previous */
      ctx = glXCreateContext( disp, vis, previous_ctx, GL_TRUE );
    }
    previous_ctx = ctx;

    if( ctx )
      OpenGl_ResourceCleaner::GetInstance()->AppendContext( ctx, true );

    // remove the dead_ctx for ResourceCleaner after appending shared ctx
    if (dead_ctx) {
      OpenGl_ResourceCleaner::GetInstance()->RemoveContext(dead_ctx);
      dead_ctx = 0;
    }

    if( !ctx) return TFailure;

    cmap = XCreateColormap( disp,  par, vis->visual, AllocNone );

    color.red = (unsigned short) (bgcolr * 0xFFFF);
    color.green = (unsigned short) (bgcolg * 0xFFFF);
    color.blue  = (unsigned short) (bgcolb * 0xFFFF);
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor( disp, cmap, &color );

    cwa.colormap  = cmap;
    cwa.event_mask  = StructureNotifyMask;
    cwa.border_pixel  = color.pixel;
    cwa.background_pixel = color.pixel;

    unsigned long mask = CWBackPixel | CWColormap | CWBorderPixel | CWEventMask;

    if( vis->visualid == wattr.visual->visualid ) {
      win = par;
    } 
    else 
    {
      win = XCreateWindow( disp, par, x, y, w, h, bw,
        vis->depth, InputOutput, vis->visual,
        mask, &cwa );
    }

#ifdef TRACE
    printf ("TxglCreateWindow win %x par %x \n", win, par);
#endif

    XSetWindowBackground( disp, win, cwa.background_pixel );
    XClearWindow( disp, win );

    /* if in Pixmap double buffering mode, set up pixmap */

    if (TelTestPixmapDB())
    {
      GC gc;
      Pixmap pixmap;
      GLXPixmap glxpixmap;

      printf("setting up pixmap double buffering\n");

      gc = XCreateGC(disp, win, 0, NULL);

      pixmap = XCreatePixmap(disp, win, w, h, vis->depth);

      glxpixmap = glXCreateGLXPixmap(disp, vis, pixmap);

      glXMakeCurrent(disp, glxpixmap, ctx);

      glDrawBuffer(GL_FRONT);

      TelSetPixmapDBParams(disp, win, w, h, vis->depth, gc, pixmap, glxpixmap, ctx);
    }

    XFree((char*)vis);  

    _Txgl_Map.Bind( (Tint)win, ctx );

    return win;

#else /* WNT */

    HTBL_ENTRY*           hte;
    PIXELFORMATDESCRIPTOR pfd;
    BOOL                  DBuffer = TRUE;
    int                   iPixelFormat;
    char                  string[ CALL_DEF_STRING_LENGTH ];

#ifdef BUC60691
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx(&os);
    if( os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) win95 = TRUE; 
#endif

    if ( _Txgl_Map.IsBound( (Tint)par ) ) {
      hte = _Txgl_Map.ChangeFind( (Tint)par );
      if ( hte ) 
      {
        ++hte -> nUsed;
        printf("*TxglCreateWindow.window %d is alreday created\n",par);
        return par;
      }
    }

    hte = new HTBL_ENTRY();

    if ( !hte ) return 0;
    hte -> hDC   = GetDC ( par );

    iPixelFormat = find_pixel_format(hte, &pfd);

    if ( !iPixelFormat ) 
    {
      printf ("*OpenGL interface: ChoosePixelFormat failed. Error code: %d\n",GetLastError ());

      ReleaseDC ( par, hte -> hDC );
      delete hte;

      return 0;
    }  

    if ( pfd.dwFlags & PFD_NEED_PALETTE ) 
    {
      WINDOW_DATA* wd = ( WINDOW_DATA* )GetWindowLongPtr ( par, GWLP_USERDATA );

      InterfaceGraphic_RealizePalette (hte -> hDC, wd -> hPal, FALSE,
        s_sysPalInUse = pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE ? TRUE : FALSE); 
    }  

    if ( pfd.cColorBits <= 8 ) 
    { 
      DitherProp     = TRUE;
      BackDitherProp = TRUE;    
    }  

    if (call_util_osd_getenv ("CALL_OPENGL_NO_DITHER", string, CALL_DEF_STRING_LENGTH))
      DitherProp = FALSE;

    if (call_util_osd_getenv ("CALL_OPENGL_NO_BACKDITHER", string, CALL_DEF_STRING_LENGTH))
      BackDitherProp = FALSE;

    if (  !SetPixelFormat ( hte -> hDC, iPixelFormat, &pfd )  ) 
    {
      printf ("*OpenGL interface: SetPixelFormat failed. Error code %d\n",GetLastError ()); 
      ReleaseDC ( par, hte -> hDC );
      delete hte;
      return 0; 
    }  

    hte -> hGLRC = wglCreateContext ( hte -> hDC );

    if ( !hte -> hGLRC ) 
    { 
      printf ("*OpenGL interface: wglCreateContext failed. Error code: %d\n",GetLastError ());    
      return 0;     
    }  

    Standard_Boolean isShared = Standard_True;

    if (previous_ctx == 0 )
      previous_ctx = hte -> hGLRC;
    // if we already have some shared context
    else
    {
      // try to share context with one from resource cleaner list
      GLCONTEXT shareCtx = OpenGl_ResourceCleaner::GetInstance()->GetSharedContext();
      
      if (shareCtx != 0)
        isShared = (Standard_Boolean)wglShareLists(shareCtx, hte -> hGLRC);
      else
      {
        isShared = (Standard_Boolean)wglShareLists(previous_ctx, hte -> hGLRC);
	      // add shared previous_ctx to a control list if it's not there
        if (isShared)
          OpenGl_ResourceCleaner::GetInstance()->AppendContext(previous_ctx, isShared);
      }
    }

    // add the context to OpenGl_ResourceCleaner control list
    OpenGl_ResourceCleaner::GetInstance()->AppendContext( hte -> hGLRC, isShared);
    _Txgl_Map.Bind( (Tint)par, hte );

    return par;

#endif  /* WNT */

  }

#ifdef RIC120302
  WINDOW
    TxglSetWindow( DISPLAY  *disp, WINDOW par, GLCONTEXT ctx)
  {
#ifndef WNT
    XVisualInfo* vis;
    char string[CALL_DEF_STRING_LENGTH];
    XWindowAttributes wattr;

    XGetWindowAttributes( disp , par , &wattr );
    {
      unsigned long vmask = VisualIDMask |  VisualScreenMask;
      XVisualInfo vinfo;
      int ninfo;
      vinfo.visualid = wattr.visual->visualid;
      vinfo.screen = DefaultScreen( disp );
      vis = XGetVisualInfo( disp, vmask, &vinfo, &ninfo);
    }

    if( !vis) return TFailure;

#ifdef TRACE
    printf ("TxglSetWindow \n");
    printf ("Informations sur le visual\n");
    printf ("par visualid %x%x %d\n", wattr.visual->visualid, wattr.visual->visualid);
    printf ("vis visualid 0x%x %d\n", vis->visualid, vis->visualid);
    printf ("vis depth %d\n", vis->depth);
    printf ("vis class %d\n", vis->class);
    printf ("vis red_mask %ld\n", vis->red_mask);
    printf ("vis green_mask %ld\n", vis->green_mask);
    printf ("vis blue_mask %ld\n", vis->blue_mask);
    printf ("vis colormap_size %d\n", vis->colormap_size);
    printf ("vis bits_per_rgb %d\n", vis->bits_per_rgb);
    printVisualInfo( disp, vis );
#endif

    /*
    * Le BackDitherProp est utilise pour le clear du background
    * Pour eviter une difference de couleurs avec la couleur choisie
    * par l'application (XWindow) il faut desactiver le dithering
    * au dessus de 8 plans.
    * 
    * Pour le DitherProp:
    * On cherchera a activer le Dithering que si le Visual a au moins
    * 8 plans pour le GLX_RED_SIZE. Le test est plus sur car on peut
    * avoir une profondeur superieure a 12 mais avoir besoin du dithering.
    * (Carte Impact avec GLX_RED_SIZE a 5 par exemple)
    */

    {
      int value;
      glXGetConfig( disp, vis, GLX_RED_SIZE, &value );

      if ( value < 8 ) {
        DitherProp = True;
      } else {
        DitherProp = False;
      }

      if ( vis->depth <= 8 ) {
        BackDitherProp = True;
      } else {
        BackDitherProp = False;
      }
    }

#ifdef TRACE
    printf("Dithering %d BackDithering %d \n",DitherProp,BackDitherProp);
#endif

    if (call_util_osd_getenv ("CALL_OPENGL_NO_DITHER", string, CALL_DEF_STRING_LENGTH))
      DitherProp = False;

    if (call_util_osd_getenv ("CALL_OPENGL_NO_BACKDITHER", string, CALL_DEF_STRING_LENGTH))
      BackDitherProp = False;

    previous_ctx = ctx;

    XFree((char*)vis);  

    _Txgl_Map.Bind( ( Tint )par, ctx );
#else /* WNT */

    /* abd
    cmn_htbl_elem         rec;*/
    HTBL_ENTRY*           hte;
    PIXELFORMATDESCRIPTOR pfd;
    BOOL                  DBuffer = TRUE;
    int                   iPixelFormat;
    char                  string[ CALL_DEF_STRING_LENGTH ];

#ifdef BUC60691
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx(&os);
    if( os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) win95 = TRUE; 
#endif

    if ( _Txgl_Map.IsBound( ( Tint )par ) )
    {
      hte = _Txgl_Map.ChangeFind( ( Tint )par );
      ++hte -> nUsed;
      printf("*TxglSetWindow.window %d is alreday created\n",par);
      return par;
    }

    hte = new HTBL_ENTRY();

    if ( !hte ) return 0;

    hte -> hDC   = GetDC ( par );
    iPixelFormat = find_pixel_format(hte, &pfd);

    if ( !iPixelFormat ) 
    {
      printf ("*OpenGL interface: ChoosePixelFormat failed. Error code: %d\n",GetLastError ());

      ReleaseDC ( par, hte -> hDC );
      delete hte;

      return 0;
    }  

    if ( pfd.dwFlags & PFD_NEED_PALETTE ) 
    {
      WINDOW_DATA* wd = ( WINDOW_DATA* )GetWindowLongPtr ( par, GWLP_USERDATA );

      InterfaceGraphic_RealizePalette (hte -> hDC, wd -> hPal, FALSE,
        s_sysPalInUse = pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE ? TRUE : FALSE); 
    }  

    if ( pfd.cColorBits <= 8 ) 
    { 
      DitherProp     = TRUE;
      BackDitherProp = TRUE;    
    }  

    if (call_util_osd_getenv ("CALL_OPENGL_NO_DITHER", string, CALL_DEF_STRING_LENGTH))
      DitherProp = FALSE;

    if (call_util_osd_getenv ("CALL_OPENGL_NO_BACKDITHER", string, CALL_DEF_STRING_LENGTH))
      BackDitherProp = FALSE;

    if (  !SetPixelFormat ( hte -> hDC, iPixelFormat, &pfd )  ) 
    {
      printf ("*OpenGL interface: SetPixelFormat failed. Error code %d\n",GetLastError ()); 
      ReleaseDC ( par, hte -> hDC );
      delete hte;
      return 0; 
    }  

    hte -> hGLRC = previous_ctx = ctx;

    _Txgl_Map.Bind( ( Tint )par, hte );
#endif  /* WNT */

    return par;

  }
#endif /*RIC120302*/

  /*----------------------------------------------------------------------*/

  TStatus
    TxglWinset( DISPLAY *disp, WINDOW win )
  {

#ifndef WNT

    Bool  i;
    GLXContext ctx;
    GLenum errorcode;
    const GLubyte *errorstring;

    if ( !_Txgl_Map.IsBound( win ) )
      return TFailure;

    ctx = _Txgl_Map.ChangeFind( win );


#ifdef TRACE
    printf ("TxglWinset::glXMakeCurrent %x \n", win);
#endif
    if (TelTestPixmapDB())
    {
      i = glXMakeCurrent(disp, TelGetGLXPixmap(), ctx);
    }
    else
    {
      i = glXMakeCurrent(disp, win, ctx);  /* TRUE/FALSE */
    }
    if (!i)
    {
      // if there is no current context it might be impossible to use
      // glGetError correctly
      printf("glXMakeCurrent failed!\n");
    }

    return  i == True ? TSuccess : TFailure;

#else /* WNT */

    HTBL_ENTRY*   hte;
    HDC    hdc    = NULL;
    HGLRC  hglrc  = NULL;
    TStatus       retVal = TFailure;

    __try {

      if ( !_Txgl_Map.IsBound( ( Tint )win ) ) {
        printf("OpenGL interface:  TxglWinset failed.UNKNOWN win %x\n",win);
        __leave;
      }
      hte = _Txgl_Map.ChangeFind( ( Tint )win );

#ifdef BUC60691
      if( win95 ) {
        retVal = (TStatus)ReleaseDC ( win, hte -> hDC );
        hte -> hDC   = GetDC ( win );
      }   
#endif
      if (  !wglMakeCurrent ( hte -> hDC, hte -> hGLRC )  ) 
      {
#ifdef BUC60691
        GLenum errorcode;
        const GLubyte *errorstring;

        errorcode = glGetError();
        errorstring = gluErrorString(errorcode);
        printf("wglMakeCurrent failed: %d %s\n", errorcode, errorstring);
#else
        printf ("OpenGL interface: wglMakeCurrent failed. Error code: %d\n",GetLastError ());
#endif
        retVal = TFailure;  
      } else retVal = TSuccess;

    }  /* end __try */


    __finally 
    {    
    }  

    return retVal;

#endif  /* WNT */

  }

  /*----------------------------------------------------------------------*/

  WINDOW
    TxglGetSubWindow( DISPLAY *disp, WINDOW win )
    /* This function assumes that there is only
    one child for the parent */
  {

#ifndef WNT

    Window root, parent, *child, w;
    unsigned int num;

    if( XQueryTree( disp, win, &root, &parent, &child, &num ) )
    {
      if (! num) return win;
      w = child[0];
      XFree( (char *)child );
      return w;
    }
    else
      return 0;

#else /* WNT */

    return win;

#endif  /* WNT */

  }

  /*----------------------------------------------------------------------*/

  void
    TxglDestroyWindow( DISPLAY *disp, WINDOW win )
  {

#ifndef WNT

    GLXContext ctx;
    Tint dummy;

    if ( !_Txgl_Map.IsBound(win) )
      return;
    ctx = _Txgl_Map.ChangeFind(win);

    /* FSXXX sync necessary if non-direct rendering */
    glXWaitGL();

    _Txgl_Map.UnBind( win );

    if (previous_ctx == ctx) {
      /* san -- OCC12977: it's important to put some valid GLXContext or null into
      previous_ctx here, otherwise next glxCreateContext() will crash on some ATI Radeon cards
      */
      previous_ctx = NULL;
      if ( _Txgl_Map.Size() > 0 ) {
        GLContextMap::Iterator it(_Txgl_Map);
        previous_ctx = it.Value();
      }
      /*
      * if this is the last remaining context, do not destroy it yet, to avoid
      * losing any shared display lists (fonts...)
      */
      if (previous_ctx) {
        OpenGl_ResourceCleaner::GetInstance()->RemoveContext(ctx);
        glXDestroyContext(disp, ctx);
      } else {
        dead_ctx = ctx;
        dead_dpy = disp;
      }
    } else {
      OpenGl_ResourceCleaner::GetInstance()->RemoveContext(ctx);
      glXDestroyContext(disp, ctx);
    }

#else /* WNT */

    HTBL_ENTRY*   hte;
#ifdef _DEBUG 
    WINDOW_DATA*  wd;
#endif  /* _DEBUG */
    if ( !_Txgl_Map.IsBound(( Tint )win) )
      return;
    hte = _Txgl_Map.ChangeFind(( Tint )win);

#ifdef _DEBUG 
    /* In release version of application we need to process    */
    /*  palette messages in the main application message loop. */
    /*  In debug version we don't have message loop for most   */
    /*  cases. So, let's restore system colors here now.       */
    wd = ( WINDOW_DATA* )GetWindowLongPtr ( win, GWLP_USERDATA );

    if ( wd != NULL ) InterfaceGraphic_RealizePalette (
      hte -> hDC, wd -> hPal, TRUE, s_sysPalInUse);
#endif  /* _DEBUG */

    if ( --hte -> nUsed == 0 ) 
    { 
      OpenGl_ResourceCleaner::GetInstance()->RemoveContext( hte -> hGLRC );
#ifdef OCC954    
      if ( wglGetCurrentContext() != NULL )
#endif
        wglDeleteContext ( hte -> hGLRC );
      ReleaseDC ( win, hte -> hDC );
      _Txgl_Map.UnBind( (Tint ) win );
      if( _Txgl_Map.Size() == 0 ) {
        previous_ctx = 0;
      }
      delete hte;
    }  

#endif  /* WNT */

  }

  /*----------------------------------------------------------------------*/

  int
    TxglGetDither(void)
  {
    return DitherProp;
  }

  /*----------------------------------------------------------------------*/

  int
    TxglGetBackDither(void)
  {
    return BackDitherProp;
  }


  /*----------------------------------------------------------------------*/
  /*RIC120302*/
  GLCONTEXT 
    TxglGetContext( WINDOW win )
  {
    GLCONTEXT ctx = NULL;
    if ( _Txgl_Map.IsBound( (Tint) win ) )
      ctx = 
#ifdef WNT    
      _Txgl_Map.Find( (Tint) win )->hGLRC;
#else
      _Txgl_Map.Find( win );
#endif

    return ctx;  
  }
  /*RIC120302*/

  /*----------------------------------------------------------------------*/
  enum { ZERO = 0, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN };

  /* Unused :*/
#ifdef TRACE
  static const char*
    className( int class )
  {
    static char      *classes[] = {
      "StaticGray",
        "GrayScale",
        "StaticColor",
        "PseudoColor",
        "TrueColor",
        "DirectColor",
    };

    if ( class < ZERO || class > FIVE )
      return "unknown";
    else
      return classes[class];
  }
#endif
  /*----------------------------------------------------------------------*/
#ifndef WNT

#ifdef TRACE
  static GLvoid printVisualInfo( DISPLAY  *display, XVisualInfo *glxVisual )
  {

#define TrueFalse(x)  ( x ? "True" : "False" )

    int        tmp;

    printf("\n" );
    printf("   X Visual Information ...\n\n" );
    printf("\tvisualid : 0x%x\n", glxVisual->visualid );
    printf("\tclass    : %s\n\n", className( glxVisual->class ) );

    glXGetConfig(display, glxVisual, GLX_USE_GL, &tmp);
    printf( "\tSupport GL ( GLX_USE_GL ) : %s\n", TrueFalse(tmp) );

    glXGetConfig(display, glxVisual, GLX_LEVEL, &tmp);
    printf( "\tFramebuffer ( GLX_LEVEL ) : %s\n\n",
      (tmp < ZERO) ? "Underlay" : (tmp == ZERO ? "Normal"  : tmp > ONE ? "Overlay"  : "Popup") );

    glXGetConfig(display, glxVisual, GLX_BUFFER_SIZE, &tmp);
    printf( "\tFramebuffer depth ( GLX_BUFFER_SIZE )     : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_DOUBLEBUFFER, &tmp);
    printf( "\tDoublebuffer ( GLX_DOUBLEBUFFER )         : %s\n",
      TrueFalse(tmp) );

    glXGetConfig(display, glxVisual, GLX_DEPTH_SIZE, &tmp);
    printf( "\tDepth buffer depth ( GLX_DEPTH_SIZE )     : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_STENCIL_SIZE, &tmp);
    printf( "\tStencil buffer depth ( GLX_STENCIL_SIZE ) : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_STEREO, &tmp);
    printf( "\tStereo Buffer ( GLX_STEREO )              : %s\n",
      TrueFalse(tmp) );

    glXGetConfig(display, glxVisual, GLX_AUX_BUFFERS, &tmp);
    printf( "\tAuxillary Buffers ( GLX_AUX_BUFFERS)      : %d\n\n", tmp );

    glXGetConfig(display, glxVisual, GLX_RGBA, &tmp);
    printf( "\tColor mode ( GLX_RGBA )       : %s\n", tmp ? "RGBA" :
    "Color Index" );

    glXGetConfig(display, glxVisual, GLX_RED_SIZE, &tmp);
    printf( "\tRed Bits ( GLX_RED_SIZE )     : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_GREEN_SIZE, &tmp);
    printf( "\tGreen Bits ( GLX_GREEN_SIZE ) : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_BLUE_SIZE, &tmp);
    printf( "\tBlue Bits ( GLX_BLUE_SIZE )   : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_ALPHA_SIZE, &tmp);
    printf( "\tAlpha Bits ( GLX_ALPHA_SIZE ) : %d\n\n", tmp );

    glXGetConfig(display, glxVisual, GLX_ACCUM_RED_SIZE, &tmp);
    printf( "\tRed Accumulation Bits ( GLX_ACCUM_RED_SIZE )     : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_ACCUM_GREEN_SIZE, &tmp);
    printf( "\tGreen Accumulation Bits ( GLX_ACCUM_GREEN_SIZE ) : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_ACCUM_BLUE_SIZE, &tmp);
    printf( "\tBlue Accumulation Bits ( GLX_ACCUM_BLUE_SIZE )   : %d\n", tmp );

    glXGetConfig(display, glxVisual, GLX_ACCUM_ALPHA_SIZE, &tmp);
    printf( "\tAlpha Accumulation Bits ( GLX_ACCUM_ALPHA_SIZE ) : %d\n\n", tmp );
  }
#endif //TRACE
#else //WNT
  int find_pixel_format(HTBL_ENTRY * hte, PIXELFORMATDESCRIPTOR * pfd)
  {
    int                   iPixelFormat = 0;
    int                   iGood = 0;
    int                   i, j;
    PIXELFORMATDESCRIPTOR pfd0;
    char                  string[ CALL_DEF_STRING_LENGTH ];
    BOOL                  DBuffer = TRUE;
    const int             cBits[] = { 32, 24 };
    const int             dBits[] = { 32, 24, 16 };

    if (call_util_osd_getenv ("CALL_OPENGL_NO_DBF",
      string, CALL_DEF_STRING_LENGTH)) 
      DBuffer = FALSE;

    pfd0.nSize           = sizeof ( PIXELFORMATDESCRIPTOR );
    pfd0.nVersion        = 1;
    pfd0.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd0.dwFlags        |= ( DBuffer ) ? PFD_DOUBLEBUFFER : PFD_SUPPORT_GDI;
    pfd0.iPixelType      = PFD_TYPE_RGBA;
    pfd0.cRedBits        = 0;
    pfd0.cRedShift       = 0;
    pfd0.cGreenBits      = 0;
    pfd0.cGreenShift     = 0;
    pfd0.cBlueBits       = 0;
    pfd0.cBlueShift      = 0;
    pfd0.cAlphaBits      = 0;
    pfd0.cAlphaShift     = 0;
    pfd0.cAccumBits      = 0;
    pfd0.cAccumRedBits   = 0;
    pfd0.cAccumGreenBits = 0;
    pfd0.cAccumBlueBits  = 0;
    pfd0.cAccumAlphaBits = 0;
    pfd0.cStencilBits    = 0;
    pfd0.cAuxBuffers     = 0;
    pfd0.iLayerType      = PFD_MAIN_PLANE;
    pfd0.bReserved       = 0;
    pfd0.dwLayerMask     = 0;
    pfd0.dwVisibleMask   = 0;
    pfd0.dwDamageMask    = 0;

    hte -> nUsed = 1;    

    for (i = 0; i < sizeof(dBits) / sizeof(int); i++) {

      pfd0.cDepthBits = dBits[i];
      iGood = 0;
      for (j = 0; j < sizeof(cBits) / sizeof(int); j++) {

        pfd0.cColorBits = cBits[j];
        iPixelFormat = ChoosePixelFormat ( hte -> hDC, &pfd0 );

        if (iPixelFormat) {
          pfd->cDepthBits = 0;
          pfd->cColorBits = 0;
          DescribePixelFormat (hte -> hDC, iPixelFormat,
            sizeof ( PIXELFORMATDESCRIPTOR ), pfd);
          if (pfd->cColorBits >= cBits[j] && pfd->cDepthBits >= dBits[i])
            break;
          if (iGood == 0)
            iGood = iPixelFormat;
        }
      }
      if (j < sizeof(cBits) / sizeof(int))
        break;
    }

    if ( !iPixelFormat )
      iPixelFormat = iGood;

    return iPixelFormat;
  }

#endif  /* WNT */
  /*----------------------------------------------------------------------*/
