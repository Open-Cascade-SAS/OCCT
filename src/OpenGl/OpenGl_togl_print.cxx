/* File         OpenGL_togl_print.c 
Created      March 2000
Author       THA
e-mail t-hartl@muenchen.matra-dtv.fr  */

#define RIC120302       /* GG Enable to use the application display
//                      callback at end of traversal
*/

#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_tgl.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_animation.hxx>

#include <string.h>

/* SAV - begin */
/* MSDN says: point is 1/72 inch. But in our case text height in the 3D view
differs from printed one. An experiment showed that delimeter equal to 2*72 gives
practically equal text heights. */
int defaultDelimeter = 72;
int delimeter = 144;
int defaultPntSize = 12;

GLuint printerFontBase = 0;

/* printer DC needed to avoid passing OS specific type as a procedure parameter */
#ifdef WNT
HDC     printer;
HGDIOBJ oldObj;
#endif

GLuint createFont( char* typeFace, int height, int weight, int italic )
{
#ifdef WNT
  /* no unicode support yet*/
  HFONT  font;
  GLuint base;
  DWORD  charSet = ANSI_CHARSET;

  if ( ( base = glGenLists( 96 ) ) == 0 )
    return 0;

  if ( _stricmp( typeFace, "symbol" ) == 0 )
    charSet = SYMBOL_CHARSET;

  font = CreateFont( height, 0, 0, 0, weight, italic, FALSE, FALSE,
    charSet, OUT_TT_PRECIS,
    CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
    DEFAULT_PITCH, typeFace );
  oldObj = SelectObject( printer, font );
  wglUseFontBitmaps( printer, 32, 96, base );
  return base;
#endif
  return 0;
}

void deleteFont( GLuint base )
{
#ifdef WNT
  HFONT currentFont;

  if ( base == 0 )
    /* no font created */
    return;
  /* deleting font list id */
  glDeleteLists( base, 96 );
  currentFont = (HFONT)SelectObject( printer, oldObj );
  /* deleting current font structure */
  DeleteObject( currentFont );
#endif
}

void updatePrinterFont( char* type, int height )
{
#ifdef WNT
  int delim = delimeter;
  if ( height <= 0 ) {
    height = defaultPntSize;
    delim = defaultDelimeter;
  }
  /* deleting old font */
  deleteFont( printerFontBase );
  /* creating new one */
  printerFontBase = createFont( type, -MulDiv( height, GetDeviceCaps( printer, LOGPIXELSY ), delim ),
    (int)FW_NORMAL, 0 );
#endif
}
/* SAV - end */


void EXPORT
call_togl_print
(
 CALL_DEF_VIEW *aview,
 CALL_DEF_LAYER *anunderlayer,
 CALL_DEF_LAYER *anoverlayer,
 const Aspect_Drawable hPrintDC,
 const int background,
 const char* filename
 )
{

#ifdef WNT

  CMN_KEY_DATA data;
  Tint swap = 1; /* swap buffers ? yes */


  BOOL bRet = FALSE;
  HDC hPrnDC;
  DOCINFO   di;

  hPrnDC = (HDC) hPrintDC;
  printer = hPrnDC;

  /* Begin main routine **************************************************************/
  TsmGetWSAttri (aview->WsId, WSWindow, &data);
  if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) == TSuccess) 
  {
    static PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
        1,        // version number
        PFD_SUPPORT_OPENGL|
        PFD_DRAW_TO_BITMAP,
        PFD_TYPE_RGBA,
        0, 0, 0, 0, 0, 0,   // color bits ignored
        0,        // no alpha buffer
        0,        // shift bit ignored
        0,        // no accumulation buffer
        0, 0, 0, 0,       // accum bits ignored
        32,       // 32-bit z-buffer  
        0,        // no stencil buffer
        0,        // no auxiliary buffer
        PFD_MAIN_PLANE,     // main layer
        0,        // reserved
        0, 0, 0       // layer masks ignored
    };
    HWND w;
    HDC hDC;    
    HDC hMemDC;
    HGLRC hGLRC;
    RECT rect;

    BITMAPINFO* pBMI;
    BITMAPINFOHEADER* bmHeader;

    BYTE  biInfo[sizeof(BITMAPINFOHEADER) + 256 * sizeof (RGBQUAD)];
    HGDIOBJ hBmOld;
    HBITMAP hBm;
    VOID* base;

    int nIndex;
    int bmiSize;

    float viewRatio;
    int devWidth;
    int devHeight;
    int viewWidth;
    int viewHeight;
    int width;
    int height;
    float tempWidth;
    float tempHeight;
    int offsetx = 0;
    int offsety = 0;

#ifdef mydebug
    devWidth = 640;
    tempWidth = (float) devWidth;
    devHeight = 480;
    tempHeight = (float) devHeight;
#else
    devWidth = GetDeviceCaps(hPrnDC, HORZRES);
    tempWidth = (float) devWidth;
    devHeight = GetDeviceCaps(hPrnDC, VERTRES);
    tempHeight = (float) devHeight;
#endif
    GetClientRect((WINDOW) data.ldata, &rect);
    viewWidth = rect.right-rect.left;
    viewHeight = rect.bottom-rect.top;

    viewRatio = (float) viewWidth/(float) viewHeight;

    // Calculate correct width/height ratio
    if (tempHeight < tempWidth/viewRatio)
      tempWidth = tempHeight*viewRatio;
    if (tempWidth < tempHeight*viewRatio)
      tempHeight = tempWidth/viewRatio;

    width = (int) tempWidth;
    height = (int) tempHeight;

    // Create virtual window    
    w = CreateWindow(
      "Button",              
      "",      
      WS_OVERLAPPEDWINDOW |   
      WS_CLIPCHILDREN |
      WS_CLIPSIBLINGS,
      0, 0,             
      width, height,   
      NULL,                
      NULL,         
      NULL,        
      NULL);  
#ifdef mydebug
    ShowWindow(w, SW_SHOW);
#endif

    hDC = GetDC(w);
    if (!hDC)
    {
      MessageBox(0,"hDC == NULL", "Fehler", MB_OK);
      return;
    }

    // Initialize Bitmap Information

    pBMI = (BITMAPINFO *) biInfo;
    ZeroMemory(pBMI, sizeof(*pBMI));
    bmiSize = sizeof(*pBMI);

    pBMI = (BITMAPINFO *) calloc(1, bmiSize);
    bmHeader = &pBMI->bmiHeader;

    bmHeader->biSize = sizeof(*bmHeader);
    bmHeader->biWidth = width;
    bmHeader->biHeight = height;
    bmHeader->biPlanes = 1;                     /* must be 1 */
    bmHeader->biBitCount = 24;
    bmHeader->biXPelsPerMeter = 0;
    bmHeader->biYPelsPerMeter = 0;
    bmHeader->biClrUsed = 0;                    /* all are used */
    bmHeader->biClrImportant = 0;               /* all are important */
    bmHeader->biCompression = BI_RGB;
    bmHeader->biSizeImage = 0;

    // Create Device Independent Bitmap 
    hMemDC = CreateCompatibleDC(hPrnDC);
    hBm = CreateDIBSection(hMemDC, pBMI, DIB_RGB_COLORS, &base, NULL, 0); 
    hBmOld = SelectObject(hMemDC, hBm);

    // Release Memory   
    free(pBMI);
    //    free(bmHeader);

    // further initialization
#ifdef mydebug
    pfd.cColorBits = GetDeviceCaps(hDC, BITSPIXEL);
    nIndex = ChoosePixelFormat(hDC, &pfd);
    if (nIndex == 0)
    {
      MessageBox(0,"ChoosePixelFormat failed", "Error", MB_OK | MB_ICONSTOP);
      goto Error;
    }


    if (!SetPixelFormat(hDC, nIndex, &pfd))
    {
      MessageBox(0,"SetPixelFormat failed", "Error", MB_OK | MB_ICONSTOP);
      goto Error;
    }

    // Create Rendering Context
    hGLRC = wglCreateContext(hDC);

    if (hGLRC == NULL) 
    {
      MessageBox(0,"No Rendering Context", "Error", MB_OK | MB_ICONSTOP);
      goto Error;
    }
    wglMakeCurrent(hDC,hGLRC);
#else
    pfd.cColorBits = GetDeviceCaps(hMemDC, BITSPIXEL);
    nIndex = ChoosePixelFormat(hMemDC, &pfd);
    if (nIndex == 0)
    {
      MessageBox(0,"ChoosePixelFormat failed", "Error", MB_OK | MB_ICONSTOP);
      goto Error;
    }


    if (!SetPixelFormat(hMemDC, nIndex, &pfd))
    {
      MessageBox(0,"SetPixelFormat failed", "Error", MB_OK | MB_ICONSTOP);
      goto Error;
    }

    // Create Rendering Context
    hGLRC = wglCreateContext(hMemDC);

    if (hGLRC == NULL) 
    {
      MessageBox(0,"No Rendering Context", "Error", MB_OK | MB_ICONSTOP);
      goto Error;
    }
    wglMakeCurrent(hMemDC,hGLRC);
#endif

    /* creating default font */
    printerFontBase = 
      createFont( "", -MulDiv( defaultPntSize, GetDeviceCaps( hPrnDC, LOGPIXELSY ), defaultDelimeter ),
      (int)FW_NORMAL, 0 );

    // redraw to new Rendering Context
    call_func_redraw_all_structs_begin (aview->WsId);

    call_togl_setplane( aview ); /* update clipping planes */

    if (background == 0)
    {
      glClearColor(1.0, 1.0, 1.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (anunderlayer->ptrLayer)
      call_togl_redraw_layer2d (aview, anunderlayer);
    call_func_redraw_all_structs_proc (aview->WsId);

    if (anoverlayer->ptrLayer)
      call_togl_redraw_layer2d (aview, anoverlayer);
#ifdef RIC120302
    call_subr_displayCB(aview,OCC_REDRAW_BITMAP);
#endif

    call_func_redraw_all_structs_end (aview->WsId, swap);

    call_togl_redraw_immediat_mode (aview);
#ifndef mydebug
    /* Start printing of DIB ********************************************************/
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = "Text";
    di.lpszOutput = filename;

    if (StartDoc(hPrnDC, &di) == SP_ERROR) goto Error;
    if (StartPage(hPrnDC) <= 0) goto Error;

    // calculate offset for centered printing
    if (width < devWidth)
      offsetx = (devWidth - width)/2;
    if (height < devHeight)
      offsety = (devHeight - height)/2;

    BitBlt(hPrnDC, offsetx, offsety, width, height, hMemDC, 0, 0, SRCCOPY);

    EndPage(hPrnDC);
    EndDoc(hPrnDC);
    /* releasing created font */
    deleteFont( printerFontBase );
    printerFontBase = 0;

    /* End of printing section ******************************************************/
#else
    Sleep(5000);
#endif

Error:
    /* Clean memory *****************************************************************/
    if(hBm != NULL)
    {
      SelectObject(hMemDC, hBmOld);
      DeleteObject(hBm);
      DeleteObject(hBmOld);
    }
    wglMakeCurrent(NULL, NULL);
    if (hGLRC != NULL)
    {
      wglDeleteContext(hGLRC);
    }
    if (hMemDC != NULL)
    {
      DeleteDC(hMemDC);
    }

    if (hDC != NULL)
    {
      ReleaseDC(w, hDC);
    }
    if (w != NULL)
    {
      DestroyWindow(w);
    }

    /* End of clean memory *****************************************************************/
  }

  /* End main routine ********************************************************************/
  return;
#endif /*WNT*/
}
