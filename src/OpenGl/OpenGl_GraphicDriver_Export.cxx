// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_CView.hxx>
#include <OSD_Localizer.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

#include <locale.h>

/************************************************************************/
/* Print Methods                                                        */
/************************************************************************/

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
#ifdef HAVE_GL2PS
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
  OSD_Localizer locate (LC_NUMERIC, "C");

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

  locate.Restore();
  return anErrCode == GL2PS_SUCCESS;
#else
  return Standard_False;
#endif
}
