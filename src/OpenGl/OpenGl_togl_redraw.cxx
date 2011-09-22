/***********************************************************************

FONCTION :
----------
file OpenGl_togl_redraw.c :


REMARQUES:
----------


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : CAL ; Creation.
23-01-97 : CAL : Ajout pour mettre des traces facilement
apres lancement de Designer
05-02-97 : FMN ; Suppression de OpenGl_tgl_vis.h
03-03-97 : FMN ; (PRO4063) Ajout displaylist pour le mode transient
07-10-97 : FMN ; Simplification WNT
08-08-98 : FMN ; ajout PRINT debug
18-11-98 : CAL ; S4062. Ajout des layers.
15-11-99 : GG  ; Add call_togl_redraw_area()
15-11-99 : VKH ; G004 redrawing view to a large pixmap
02.01.100 : JR : = 0 for Integer and = NULL for pointers
02.02.100   "    #include <GL/glu.h> for declaration of gluErrorString
07-03-00 : GG : G004 use the already created pixmap.
Enable two side lighting before redrawing in pixmap.

************************************************************************/

#define G004    /* VKH 15-11-99 redrawing view to a large pixmap
*/

#define RIC120302 /*  GG Enable to use the application display
callback at end of traversal
Modified P. Dolbey 09/06/07 to call back
before redrawing the overlayer
*/

/*----------------------------------------------------------------------*/
/*
* Includes
*/

#ifdef G004
#ifndef WNT
#define CALL_DEF_STRING_LENGTH 132
#else
#include <OpenGl_AVIWriter.hxx>
#endif
# include <OpenGl_tgl_all.hxx>
GLboolean g_fBitmap;
#endif  /*G004*/

#include <OpenGl_tgl.hxx>
#include <OpenGl_tsm_ws.hxx>
#include <OpenGl_tgl_tox.hxx>
#include <OpenGl_txgl.hxx>
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrvis.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_ResourceCleaner.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

void EXPORT
call_togl_redraw
(
 CALL_DEF_VIEW * aview,
 CALL_DEF_LAYER * anunderlayer,
 CALL_DEF_LAYER * anoverlayer
 )
{
  CMN_KEY_DATA data;
  Tint swap = 1; /* swap buffers ? yes */

  if ( TsmGetWSAttri (aview->WsId, WSWindow, &data) != TSuccess ) return;

  WINDOW aWin = (WINDOW )data.ldata;
  if (TxglWinset (call_thedisplay, aWin) == TSuccess)
  {
    OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )aview->ptrFBO;
    GLint aViewPortBack[4]; glGetIntegerv (GL_VIEWPORT, aViewPortBack);
    if (aFrameBuffer != NULL)
    {
      aFrameBuffer->SetupViewport();
      aFrameBuffer->BindBuffer();
      swap = 0; // no need to swap buffers
    }
    OpenGl_ResourceCleaner::GetInstance()->Cleanup();
    call_func_redraw_all_structs_begin (aview->WsId);
    call_subr_displayCB (aview, OCC_REDRAW_WINDOW | OCC_PRE_REDRAW);
    call_togl_setplane( aview );
    if (anunderlayer->ptrLayer)
    {
      call_togl_redraw_layer2d (aview, anunderlayer);
    }
    call_func_redraw_all_structs_proc (aview->WsId);
    call_subr_displayCB (aview, OCC_REDRAW_WINDOW | OCC_PRE_OVERLAY);
    if (anoverlayer->ptrLayer)
    {
      call_togl_redraw_layer2d (aview, anoverlayer);
    }
    call_subr_displayCB (aview, OCC_REDRAW_WINDOW);
    call_func_redraw_all_structs_end (aview->WsId, swap);
    call_togl_redraw_immediat_mode (aview);

    if (aFrameBuffer != NULL)
    {
      aFrameBuffer->UnbindBuffer();
      // move back original viewport
      glViewport (aViewPortBack[0], aViewPortBack[1], aViewPortBack[2], aViewPortBack[3]);
    }
  }

#ifdef WNT
  if (OpenGl_AVIWriter_AllowWriting (aview->DefWindow.XWindow))
  {
    GLint params[4];
    glGetIntegerv (GL_VIEWPORT, params);
    int nWidth = params[2] & ~0x7;
    int nHeight = params[3] & ~0x7;

    const int nBitsPerPixel = 24;
    GLubyte* aDumpData = new GLubyte[nWidth * nHeight * nBitsPerPixel / 8];

    glPixelStorei (GL_PACK_ALIGNMENT, 1);
    glReadPixels (0, 0, nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, aDumpData);
    OpenGl_AVIWriter_AVIWriter (aDumpData, nWidth, nHeight, nBitsPerPixel);
    delete[] aDumpData;
  }
#endif

  return;
}

void EXPORT
call_togl_redraw_area
(
 CALL_DEF_VIEW * aview,
 CALL_DEF_LAYER * anunderlayer,
 CALL_DEF_LAYER * anoverlayer,
 int x, int y, int width, int height
 )
{
  CMN_KEY_DATA data;

  /*
  When the exposure area size is > window size / 2 do a full redraw.
  */
  if( width*height >
    (int)(aview->DefWindow.dx*aview->DefWindow.dy)/2 ) {
      call_togl_redraw(aview,anunderlayer,anoverlayer);
      return;
    }
    /*
    Or redraw only the area in the front buffer
    */
    TsmGetWSAttri (aview->WsId, WSWindow, &data);
    if (TxglWinset (call_thedisplay, (WINDOW) data.ldata) == TSuccess)
    {
      GLint buffer;
      glGetIntegerv (GL_DRAW_BUFFER, &buffer);
      if (buffer != GL_FRONT) glDrawBuffer (GL_FRONT);
      glEnable (GL_SCISSOR_TEST);
      glScissor ((GLint )x,
                 (GLint )((int )aview->DefWindow.dy - (y + height)),
                 (GLsizei )width, (GLsizei )height);
      OpenGl_ResourceCleaner::GetInstance()->Cleanup();
      call_func_redraw_all_structs_begin (aview->WsId);
      call_subr_displayCB (aview, OCC_REDRAW_WINDOWAREA | OCC_PRE_REDRAW);
      call_togl_setplane( aview );
      if (anunderlayer->ptrLayer)
      {
        call_togl_redraw_layer2d (aview, anunderlayer);
      }
      call_func_redraw_all_structs_proc (aview->WsId);
      call_subr_displayCB (aview, OCC_REDRAW_WINDOWAREA | OCC_PRE_OVERLAY);
      if (anoverlayer->ptrLayer)
      {
        call_togl_redraw_layer2d (aview, anoverlayer);
      }
      call_subr_displayCB(aview,OCC_REDRAW_WINDOWAREA);
      call_func_redraw_all_structs_end (aview->WsId, 0);
      call_togl_redraw_immediat_mode (aview);
      glFlush();
      glDisable (GL_SCISSOR_TEST);
      if (buffer != GL_FRONT) glDrawBuffer (buffer);
    }
}
