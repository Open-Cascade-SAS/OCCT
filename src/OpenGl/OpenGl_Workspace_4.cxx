// File:      OpenGl_Workspace_3.cxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GlCore11.hxx>

#if (defined(_WIN32) || defined(__WIN32__))
  #include <OpenGl_AVIWriter.hxx>
#endif

#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_ResourceCleaner.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_Workspace.hxx>

//call_togl_redraw
void OpenGl_Workspace::Redraw (const Graphic3d_CView& theCView,
                               const Aspect_CLayer2d& theCUnderLayer,
                               const Aspect_CLayer2d& theCOverLayer)
{
  if (!Activate())
    return;

  // release pending GL resources
  OpenGl_ResourceCleaner::GetInstance()->Cleanup (GetGlContext());

  Tint toSwap = 1; // swap buffers
  GLint aViewPortBack[4];
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theCView.ptrFBO;
  if (aFrameBuffer != NULL)
  {
    glGetIntegerv (GL_VIEWPORT, aViewPortBack);
    aFrameBuffer->SetupViewport();
    aFrameBuffer->BindBuffer (GetGlContext());
    toSwap = 0; // no need to swap buffers
  }

  Redraw1 (theCView, theCUnderLayer, theCOverLayer, toSwap);
  RedrawImmediatMode();

  if (aFrameBuffer != NULL)
  {
    aFrameBuffer->UnbindBuffer (GetGlContext());
    // move back original viewport
    glViewport (aViewPortBack[0], aViewPortBack[1], aViewPortBack[2], aViewPortBack[3]);
  }

#if (defined(_WIN32) || defined(__WIN32__))
  if (OpenGl_AVIWriter_AllowWriting (theCView.DefWindow.XWindow))
  {
    GLint params[4];
    glGetIntegerv (GL_VIEWPORT, params);
    int nWidth  = params[2] & ~0x7;
    int nHeight = params[3] & ~0x7;

    const int nBitsPerPixel = 24;
    GLubyte* aDumpData = new GLubyte[nWidth * nHeight * nBitsPerPixel / 8];

    glPixelStorei (GL_PACK_ALIGNMENT, 1);
    glReadPixels (0, 0, nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, aDumpData);
    OpenGl_AVIWriter_AVIWriter (aDumpData, nWidth, nHeight, nBitsPerPixel);
    delete[] aDumpData;
  }
#endif
}
