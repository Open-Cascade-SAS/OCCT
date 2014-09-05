// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_CView.hxx>

#include <Standard_CLocaleSentry.hxx>

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

/************************************************************************/
/* Print Methods                                                        */
/************************************************************************/

#ifdef HAVE_GL2PS
Standard_Boolean OpenGl_GraphicDriver::Export (const Standard_CString theFileName,
                                               const Graphic3d_ExportFormat theFormat,
                                               const Graphic3d_SortType theSortType,
                                               const Standard_Integer theWidth,
                                               const Standard_Integer theHeight,
                                               const Graphic3d_CView& theView,
                                               const Aspect_CLayer2d& theLayerUnder,
                                               const Aspect_CLayer2d& theLayerOver,
                                               const Standard_Real    /*thePrecision*/,
                                               const Standard_Address /*theProgressBarFunc*/,
                                               const Standard_Address /*theProgressObject*/)
{
  // gl2psBeginPage() will call OpenGL functions
  // so we should activate correct GL context before redraw scene call
  const OpenGl_CView* aCView = (const OpenGl_CView* )theView.ptrView;
  if (aCView == NULL || !aCView->WS->GetGlContext()->MakeCurrent())
  {
    return Standard_False;
  }

  Standard_Integer aFormat = -1;
  Standard_Integer aSortType = Graphic3d_ST_BSP_Tree;
  switch (theFormat)
  {
    case Graphic3d_EF_PostScript:
      aFormat = GL2PS_PS;
      break;
    case Graphic3d_EF_EnhPostScript:
      aFormat = GL2PS_EPS;
      break;
    case Graphic3d_EF_TEX:
      aFormat = GL2PS_TEX;
      break;
    case Graphic3d_EF_PDF:
      aFormat = GL2PS_PDF;
      break;
    case Graphic3d_EF_SVG:
      aFormat = GL2PS_SVG;
      break;
    case Graphic3d_EF_PGF:
      aFormat = GL2PS_PGF;
      break;
    case Graphic3d_EF_EMF:
      //aFormat = GL2PS_EMF;
      aFormat = GL2PS_PGF + 1; // 6
      break;
    default:
      // unsupported format
      return Standard_False;
  }

  switch (theSortType)
  {
    case Graphic3d_ST_Simple:
      aSortType = GL2PS_SIMPLE_SORT;
      break;
    case Graphic3d_ST_BSP_Tree:
      aSortType = GL2PS_BSP_SORT;
      break;
  }

  GLint aViewport[4];
  aViewport[0] = 0;
  aViewport[1] = 0;
  aViewport[2] = theWidth;
  aViewport[3] = theHeight;

  GLint aBufferSize = 1024 * 1024;
  GLint anErrCode = GL2PS_SUCCESS;

  // gl2ps uses standard write functions and do not check locale
  Standard_CLocaleSentry aLocaleSentry;

  while (aBufferSize > 0)
  {
    // current patch for EMF support in gl2ps uses WinAPI functions to create file
    FILE* aFileH = (theFormat != Graphic3d_EF_EMF) ? fopen (theFileName, "wb") : NULL;
    anErrCode = gl2psBeginPage ("", "", aViewport, aFormat, aSortType,
                    GL2PS_DRAW_BACKGROUND | GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT/* | GL2PS_SIMPLE_LINE_OFFSET*/,
                    GL_RGBA, 0, NULL,
                    0, 0, 0, aBufferSize, aFileH, theFileName);
    if (anErrCode != GL2PS_SUCCESS)
    {
      // initialization failed
      if (aFileH != NULL)
        fclose (aFileH);
      break;
    }
    Redraw (theView, theLayerUnder, theLayerOver);

    anErrCode = gl2psEndPage();
    if (aFileH != NULL)
      fclose (aFileH);

    if (anErrCode == GL2PS_OVERFLOW)
      aBufferSize *= 2;
    else
      break;
  }
  return anErrCode == GL2PS_SUCCESS;
}
#else
Standard_Boolean OpenGl_GraphicDriver::Export (const Standard_CString /*theFileName*/,
                                               const Graphic3d_ExportFormat /*theFormat*/,
                                               const Graphic3d_SortType /*theSortType*/,
                                               const Standard_Integer /*theWidth*/,
                                               const Standard_Integer /*theHeight*/,
                                               const Graphic3d_CView& /*theView*/,
                                               const Aspect_CLayer2d& /*theLayerUnder*/,
                                               const Aspect_CLayer2d& /*theLayerOver*/,
                                               const Standard_Real    /*thePrecision*/,
                                               const Standard_Address /*theProgressBarFunc*/,
                                               const Standard_Address /*theProgressObject*/)
{
    return Standard_False;
}
#endif
