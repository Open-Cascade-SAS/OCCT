// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
  Handle(OpenGl_Context) aGlCtx = GetGlContext();
  OpenGl_ResourceCleaner::GetInstance()->Cleanup (aGlCtx);

  // cache render mode state
  GLint aRendMode = GL_RENDER;
  glGetIntegerv (GL_RENDER_MODE, &aRendMode);
  aGlCtx->SetFeedback (aRendMode == GL_FEEDBACK);

  Tint toSwap = (aRendMode == GL_RENDER); // swap buffers
  GLint aViewPortBack[4];
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theCView.ptrFBO;
  if (aFrameBuffer != NULL)
  {
    glGetIntegerv (GL_VIEWPORT, aViewPortBack);
    aFrameBuffer->SetupViewport();
    aFrameBuffer->BindBuffer (aGlCtx);
    toSwap = 0; // no need to swap buffers
  }

  Redraw1 (theCView, theCUnderLayer, theCOverLayer, toSwap);
  RedrawImmediatMode();

  if (aFrameBuffer != NULL)
  {
    aFrameBuffer->UnbindBuffer (aGlCtx);
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

  // reset render mode state
  aGlCtx->SetFeedback (Standard_False);
}
