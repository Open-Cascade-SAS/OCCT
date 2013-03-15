// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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
#include <OpenGl_Text.hxx>

#include <OpenGl_AspectText.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Workspace.hxx>

#include <Font_FontMgr.hxx>
#include <TCollection_HAsciiString.hxx>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_GL2PS
  #include <gl2ps.h>
#endif

namespace
{
  static const GLdouble THE_IDENTITY_MATRIX[4][4] =
  {
    {1.,0.,0.,0.},
    {0.,1.,0.,0.},
    {0.,0.,1.,0.},
    {0.,0.,0.,1.}
  };

#ifdef HAVE_GL2PS
  static char const* TheFamily[] = {"Helvetica", "Courier", "Times"};
  static char const* TheItalic[] = {"Oblique",   "Oblique", "Italic"};
  static char const* TheBase[]   = {"", "", "-Roman"};

  //! Convert font name used for rendering to some "good" font names
  //! that produce good vector text.
  static void getGL2PSFontName (const char* theSrcFont,
                                char*       thePsFont)
  {
    if (strstr (theSrcFont, "Symbol"))
    {
      sprintf (thePsFont, "%s", "Symbol");
      return;
    }
    else if (strstr (theSrcFont, "ZapfDingbats"))
    {
      sprintf (thePsFont, "%s", "WingDings");
      return;
    }

    int  aFontId  = 0;
    bool isBold   = false;
    bool isItalic = false;
    if (strstr (theSrcFont, "Courier"))
    {
      aFontId = 1;
    }
    else if (strstr (theSrcFont, "Times"))
    {
      aFontId = 2;
    }

    if (strstr (theSrcFont, "Bold"))
    {
      isBold = true;
    }
    if (strstr (theSrcFont, "Italic")
     || strstr (theSrcFont, "Oblique"))
    {
      isItalic = true;
    }

    if (isBold)
    {
      if (isItalic)
      {
        sprintf (thePsFont, "%s-Bold%s", TheFamily[aFontId], TheItalic[aFontId]);
      }
      else
      {
        sprintf (thePsFont, "%s-Bold", TheFamily[aFontId]);
      }
    }
    else if (isItalic)
    {
      sprintf (thePsFont, "%s-%s", TheFamily[aFontId], TheItalic[aFontId]);
    }
    else
    {
      sprintf (thePsFont, "%s%s", TheFamily[aFontId], TheBase[aFontId]);
    }
  }

  static void exportText (const NCollection_String& theText,
                          const Standard_Boolean    theIs2d,
                          const OpenGl_AspectText&  theAspect,
                          const Standard_Integer    theHeight)
  {

    char aPsFont[64];
    getGL2PSFontName (theAspect.FontName().ToCString(), aPsFont);

    if (theIs2d)
    {
      glRasterPos2f (0.0f, 0.0f);
    }
    else
    {
      glRasterPos3f (0.0f, 0.0f, 0.0f);
    }

    GLubyte aZero = 0;
    glBitmap (1, 1, 0, 0, 0, 0, &aZero);

    // Standard GL2PS's alignment isn't used, because it doesn't work correctly
    // for all formats, therefore alignment is calculated manually relative
    // to the bottom-left corner, which corresponds to the GL2PS_TEXT_BL value
    gl2psTextOpt (theText.ToCString(), aPsFont, theHeight, GL2PS_TEXT_BL, theAspect.Angle());
  }
#endif

};

// =======================================================================
// function : OpenGl_Text
// purpose  :
// =======================================================================
OpenGl_Text::OpenGl_Text()
: myWinX (0.0f),
  myWinY (0.0f),
  myWinZ (0.0f),
  myScaleHeight (1.0f),
  myPoint  (0.0f, 0.0f, 0.0f),
  myIs2d   (false)
{
  myParams.Height = 10;
  myParams.HAlign = Graphic3d_HTA_LEFT;
  myParams.VAlign = Graphic3d_VTA_BOTTOM;
}

// =======================================================================
// function : OpenGl_Text
// purpose  :
// =======================================================================
OpenGl_Text::OpenGl_Text (const TCollection_ExtendedString& theText,
                          const OpenGl_Vec3&                thePoint,
                          const OpenGl_TextParam&           theParams)
: myWinX (0.0f),
  myWinY (0.0f),
  myWinZ (0.0f),
  myScaleHeight  (1.0f),
  myExportHeight (1.0f),
  myParams (theParams),
  myString ((Standard_Utf16Char* )theText.ToExtString()),
  myPoint  (thePoint),
  myIs2d   (false)
{
  //
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void OpenGl_Text::SetPosition (const OpenGl_Vec3& thePoint)
{
  myPoint = thePoint;
}

// =======================================================================
// function : SetFontSize
// purpose  :
// =======================================================================
void OpenGl_Text::SetFontSize (const Handle(OpenGl_Context)& theCtx,
                               const Standard_Integer        theFontSize)
{
  if (myParams.Height != theFontSize)
  {
    Release (theCtx);
  }
  myParams.Height = theFontSize;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Text::Init (const Handle(OpenGl_Context)& theCtx,
                        const Standard_Utf8Char*      theText,
                        const OpenGl_Vec3&            thePoint)
{
  releaseVbos (theCtx);
  myIs2d   = false;
  myPoint  = thePoint;
  myString.FromUnicode (theText);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Text::Init (const Handle(OpenGl_Context)& theCtx,
                        const Standard_Utf8Char*      theText,
                        const OpenGl_Vec3&            thePoint,
                        const OpenGl_TextParam&       theParams)
{
  if (myParams.Height != theParams.Height)
  {
    Release (theCtx);
  }
  else
  {
    releaseVbos (theCtx);
  }
  myIs2d   = false;
  myParams = theParams;
  myPoint  = thePoint;
  myString.FromUnicode (theText);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Text::Init (const Handle(OpenGl_Context)&     theCtx,
                        const TCollection_ExtendedString& theText,
                        const OpenGl_Vec2&                thePoint,
                        const OpenGl_TextParam&           theParams)
{
  if (myParams.Height != theParams.Height)
  {
    Release (theCtx);
  }
  else
  {
    releaseVbos (theCtx);
  }
  myIs2d       = true;
  myParams     = theParams;
  myPoint.xy() = thePoint;
  myPoint.z()  = 0.0f;
  myString.FromUnicode ((Standard_Utf16Char* )theText.ToExtString());
}

// =======================================================================
// function : ~OpenGl_Text
// purpose  :
// =======================================================================
OpenGl_Text::~OpenGl_Text()
{
  //
}

// =======================================================================
// function : releaseVbos
// purpose  :
// =======================================================================
void OpenGl_Text::releaseVbos (const Handle(OpenGl_Context)& theCtx)
{
  for (Standard_Integer anIter = 0; anIter < myVertsVbo.Length(); ++anIter)
  {
    Handle(OpenGl_VertexBuffer)& aVerts = myVertsVbo.ChangeValue (anIter);
    Handle(OpenGl_VertexBuffer)& aTCrds = myTCrdsVbo.ChangeValue (anIter);

    if (!theCtx.IsNull())
    {
      theCtx->DelayedRelease (aVerts);
      theCtx->DelayedRelease (aTCrds);
    }
    aVerts.Nullify();
    aTCrds.Nullify();
  }
  myTextures.Clear();
  myVertsVbo.Clear();
  myTCrdsVbo.Clear();
  myVertsArray.Clear();
  myTCrdsArray.Clear();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Text::Release (const Handle(OpenGl_Context)& theCtx)
{
  releaseVbos (theCtx);
  if (!myFont.IsNull())
  {
    Handle(OpenGl_Context) aCtx = theCtx;
    const TCollection_AsciiString aKey = myFont->ResourceKey();
    myFont.Nullify();
    aCtx->ReleaseResource (aKey, Standard_True);
  }
}

// =======================================================================
// function : StringSize
// purpose  :
// =======================================================================
void OpenGl_Text::StringSize (const Handle(OpenGl_Context)& theCtx,
                              const NCollection_String&     theText,
                              const OpenGl_AspectText&      theTextAspect,
                              const OpenGl_TextParam&       theParams,
                              Standard_ShortReal&           theWidth,
                              Standard_ShortReal&           theAscent,
                              Standard_ShortReal&           theDescent)
{
  theWidth   = 0.0f;
  theAscent  = 0.0f;
  theDescent = 0.0f;
  const TCollection_AsciiString aFontKey = FontKey (theTextAspect, theParams.Height);
  Handle(OpenGl_Font) aFont = FindFont (theCtx, theTextAspect, theParams.Height, aFontKey);
  if (aFont.IsNull() || !aFont->IsValid())
  {
    return;
  }

  theAscent  = aFont->Ascender();
  theDescent = aFont->Descender();

  GLfloat aWidth = 0.0f;
  for (NCollection_Utf8Iter anIter = theText.Iterator(); *anIter != 0;)
  {
    const Standard_Utf32Char aCharThis =   *anIter;
    const Standard_Utf32Char aCharNext = *++anIter;

    if (aCharThis == '\x0D' // CR  (carriage return)
     || aCharThis == '\a'   // BEL (alarm)
     || aCharThis == '\f'   // FF  (form feed) NP (new page)
     || aCharThis == '\b'   // BS  (backspace)
     || aCharThis == '\v')  // VT  (vertical tab)
    {
      continue; // skip unsupported carriage control codes
    }
    else if (aCharThis == '\x0A') // LF (line feed, new line)
    {
      theWidth = Max (theWidth, aWidth);
      aWidth   = 0.0f;
      continue;
    }
    else if (aCharThis == ' ')
    {
      aWidth += aFont->AdvanceX (aCharThis, aCharNext);
      continue;
    }
    else if (aCharThis == '\t')
    {
      aWidth += aFont->AdvanceX (' ', aCharNext) * 8.0f;
      continue;
    }

    aWidth += aFont->AdvanceX (aCharThis, aCharNext);
  }
  theWidth = Max (theWidth, aWidth);

  Handle(OpenGl_Context) aCtx = theCtx;
  aFont.Nullify();
  aCtx->ReleaseResource (aFontKey, Standard_True);
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Text::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_AspectText* aTextAspect = theWorkspace->AspectText (Standard_True);
  const Handle(OpenGl_Texture) aPrevTexture = theWorkspace->DisableTexture();

  // use highlight color or colors from aspect
  if (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)
  {
    render (theWorkspace->PrinterContext(),
            theWorkspace->GetGlContext(),
            *aTextAspect,
            *theWorkspace->HighlightColor,
            *theWorkspace->HighlightColor);
  }
  else
  {
    render (theWorkspace->PrinterContext(),
            theWorkspace->GetGlContext(),
            *aTextAspect,
            aTextAspect->Color(),
            aTextAspect->SubtitleColor());
  }

  // restore aspects
  if (!aPrevTexture.IsNull())
  {
    theWorkspace->EnableTexture (aPrevTexture);
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Text::Render (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                          const Handle(OpenGl_Context)&        theCtx,
                          const OpenGl_AspectText&             theTextAspect) const
{
  render (thePrintCtx, theCtx, theTextAspect, theTextAspect.Color(), theTextAspect.SubtitleColor());
}

// =======================================================================
// function : setupMatrix
// purpose  :
// =======================================================================
void OpenGl_Text::setupMatrix (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                               const Handle(OpenGl_Context)&        theCtx,
                               const OpenGl_AspectText&             theTextAspect,
                               const OpenGl_Vec3                    theDVec) const
{
  // setup matrix
  if (myIs2d)
  {
    glLoadIdentity();
    glTranslatef (myPoint.x() + theDVec.x(), myPoint.y() + theDVec.y(), 0.0f);
    glRotatef (180.0f, 1.0f, 0.0f, 0.0f);
  }
  else
  {
    // align coordinates to the nearest integer
    // to avoid extra interpolation issues
    GLdouble anObjX, anObjY, anObjZ;
    gluUnProject (std::floor (myWinX + (GLdouble )theDVec.x()),
                  std::floor (myWinY + (GLdouble )theDVec.y()),
                  myWinZ + (GLdouble )theDVec.z(),
                  (GLdouble* )THE_IDENTITY_MATRIX, myProjMatrix, myViewport,
                  &anObjX, &anObjY, &anObjZ);

    glLoadIdentity();
    glTranslated (anObjX, anObjY, anObjZ);
    glRotated (theTextAspect.Angle(), 0.0, 0.0, 1.0);
    if (!theTextAspect.IsZoomable())
    {
    #ifdef _WIN32
      // if the context has assigned printer context, use it's parameters
      if (!thePrintCtx.IsNull())
      {
        // get printing scaling in x and y dimensions
        GLfloat aTextScalex = 1.0f, aTextScaley = 1.0f;
        thePrintCtx->GetScale (aTextScalex, aTextScaley);

        // text should be scaled in all directions with same
        // factor to save its proportions, so use height (y) scaling
        // as it is better for keeping text/3d graphics proportions
        glScalef (aTextScaley, aTextScaley, aTextScaley);
      }
    #endif
      glScaled (myScaleHeight, myScaleHeight, myScaleHeight);
    }
  }
}

// =======================================================================
// function : drawText
// purpose  :
// =======================================================================
void OpenGl_Text::drawText (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                            const Handle(OpenGl_Context)&        theCtx,
                            const OpenGl_AspectText&             theTextAspect) const
{
#ifdef HAVE_GL2PS
  if (theCtx->IsFeedback())
  {
    // position of the text and alignment is calculated by transformation matrix
    exportText (myString, myIs2d, theTextAspect, (Standard_Integer )myExportHeight);
    return;
  }
#endif

  if (myVertsVbo.Length() == myTextures.Length())
  {
    for (Standard_Integer anIter = 0; anIter < myTextures.Length(); ++anIter)
    {
      const GLuint aTexId = myTextures.Value (anIter);
      const Handle(OpenGl_VertexBuffer)& aVerts = myVertsVbo.Value (anIter);
      const Handle(OpenGl_VertexBuffer)& aTCrds = myTCrdsVbo.Value (anIter);
      aVerts->BindFixed (theCtx, GL_VERTEX_ARRAY);
      aTCrds->BindFixed (theCtx, GL_TEXTURE_COORD_ARRAY);
      glBindTexture (GL_TEXTURE_2D, aTexId);

      glDrawArrays (GL_TRIANGLES, 0, GLsizei(aVerts->GetElemsNb()));

      glBindTexture (GL_TEXTURE_2D, 0);
      aTCrds->UnbindFixed (theCtx, GL_TEXTURE_COORD_ARRAY);
      aVerts->UnbindFixed (theCtx, GL_VERTEX_ARRAY);
    }
  }
  else if (myVertsArray.Length() == myTextures.Length())
  {
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    for (Standard_Integer anIter = 0; anIter < myTextures.Length(); ++anIter)
    {
      const GLuint aTexId = myTextures.Value (anIter);
      const Handle(OpenGl_Vec2Array)& aVerts = myVertsArray.Value (anIter);
      const Handle(OpenGl_Vec2Array)& aTCrds = myTCrdsArray.Value (anIter);

      glVertexPointer   (2, GL_FLOAT, 0, (GLfloat* )&aVerts->First());
      glTexCoordPointer (2, GL_FLOAT, 0, (GLfloat* )&aTCrds->First());
      glBindTexture (GL_TEXTURE_2D, aTexId);

      glDrawArrays (GL_TRIANGLES, 0, aVerts->Length());

      glBindTexture (GL_TEXTURE_2D, 0);
    }
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glDisableClientState (GL_VERTEX_ARRAY);
  }
}

// =======================================================================
// function : FontKey
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Text::FontKey (const OpenGl_AspectText& theAspect,
                                              const Standard_Integer   theHeight)
{
  const Font_FontAspect anAspect = (theAspect.FontAspect() != Font_FA_Undefined) ? theAspect.FontAspect() : Font_FA_Regular;
  return theAspect.FontName()
       + TCollection_AsciiString(":") + Standard_Integer(anAspect)
       + TCollection_AsciiString(":") + theHeight;
}

// =======================================================================
// function : FindFont
// purpose  :
// =======================================================================
Handle(OpenGl_Font) OpenGl_Text::FindFont (const Handle(OpenGl_Context)& theCtx,
                                           const OpenGl_AspectText&      theAspect,
                                           const Standard_Integer        theHeight,
                                           const TCollection_AsciiString theKey)
{
  Handle(OpenGl_Font) aFont;
  if (theHeight < 2)
  {
    return aFont; // invalid parameters
  }

  if (!theCtx->GetResource (theKey, aFont))
  {
    Handle(Font_FontMgr) aFontMgr = Font_FontMgr::GetInstance();
    const Handle(TCollection_HAsciiString) aFontName = new TCollection_HAsciiString (theAspect.FontName());
    const Font_FontAspect anAspect = (theAspect.FontAspect() != Font_FA_Undefined) ? theAspect.FontAspect() : Font_FA_Regular;
    Handle(Font_SystemFont) aRequestedFont = aFontMgr->FindFont (aFontName, anAspect, theHeight);
    if (aRequestedFont.IsNull())
    {
      return aFont;
    }

    Handle(Font_FTFont) aFontFt = new Font_FTFont (NULL);
    if (!aFontFt->Init (aRequestedFont->FontPath()->ToCString(), theHeight))
    {
      return aFont;
    }

    Handle(OpenGl_Context) aCtx = theCtx;
    glPushAttrib (GL_TEXTURE_BIT);
    aFont = new OpenGl_Font (aFontFt, theKey);
    if (!aFont->Init (aCtx))
    {
      //glPopAttrib();
      //return aFont; // out of resources?
    }
    glPopAttrib(); // texture bit

    aCtx->ShareResource (theKey, aFont);
  }
  return aFont;
}

// =======================================================================
// function : render
// purpose  :
// =======================================================================
void OpenGl_Text::render (const Handle(OpenGl_PrinterContext)& thePrintCtx,
                          const Handle(OpenGl_Context)&        theCtx,
                          const OpenGl_AspectText&             theTextAspect,
                          const TEL_COLOUR&                    theColorText,
                          const TEL_COLOUR&                    theColorSubs) const
{
  if (myString.IsEmpty())
  {
    return;
  }

  const TCollection_AsciiString aFontKey = FontKey (theTextAspect, myParams.Height);
  if (!myFont.IsNull()
   && !myFont->ResourceKey().IsEqual (aFontKey))
  {
    // font changed
    const_cast<OpenGl_Text* > (this)->Release (theCtx);
  }

  if (myFont.IsNull())
  {
    myFont = FindFont (theCtx, theTextAspect, myParams.Height, aFontKey);
    if (myFont.IsNull())
    {
      return;
    }
  }

  if (myTextures.IsEmpty())
  {
    OpenGl_TextFormatter aFormatter;
    aFormatter.SetupAlignment (myParams.HAlign, myParams.VAlign);
    aFormatter.Reset();
    aFormatter.Append (theCtx, myString, *myFont.operator->());
    aFormatter.Format();

    if (OpenGl_GraphicDriver::ToUseVBO() && theCtx->core15 != NULL)
    {
      aFormatter.Result (theCtx, myTextures, myVertsVbo, myTCrdsVbo);
    }
    else
    {
      aFormatter.Result (theCtx, myTextures, myVertsArray, myTCrdsArray);
    }
    aFormatter.BndBox (myBndBox);
  }

  if (myTextures.IsEmpty())
  {
    return;
  }

  myExportHeight = 1.0f;
  myScaleHeight  = 1.0f;

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix();
  if (!myIs2d)
  {
    // retrieve active matrices for project/unproject calls
    glGetDoublev  (GL_MODELVIEW_MATRIX,  myModelMatrix);
    glGetDoublev  (GL_PROJECTION_MATRIX, myProjMatrix);
    glGetIntegerv (GL_VIEWPORT,          myViewport);
    gluProject (myPoint.x(), myPoint.y(), myPoint.z(),
                myModelMatrix, myProjMatrix, myViewport,
                &myWinX, &myWinY, &myWinZ);

    // compute scale factor for constant text height
    GLdouble x1, y1, z1;
    gluUnProject (myWinX, myWinY, myWinZ,
                  (GLdouble* )THE_IDENTITY_MATRIX, myProjMatrix, myViewport,
                  &x1, &y1, &z1);

    GLdouble x2, y2, z2;
    const GLdouble h = (GLdouble )myFont->FTFont()->PointSize();
    gluUnProject (myWinX, myWinY + h - 1.0, myWinZ,
                  (GLdouble* )THE_IDENTITY_MATRIX, myProjMatrix, myViewport,
                  &x2, &y2, &z2);

    myScaleHeight = (y2 - y1) / h;
    if (theTextAspect.IsZoomable())
    {
      myExportHeight = (float )h;
    }
  }
  myExportHeight = (float )myFont->FTFont()->PointSize() / myExportHeight;

  // push enabled flags to the stack
  glPushAttrib (GL_ENABLE_BIT);

  // setup depth test
  if (!myIs2d
   && theTextAspect.StyleType() != Aspect_TOST_ANNOTATION)
  {
    glEnable (GL_DEPTH_TEST);
  }
  else
  {
    glDisable (GL_DEPTH_TEST);
  }

  // setup alpha test
  GLint aTexEnvParam = GL_REPLACE;
  glGetTexEnviv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &aTexEnvParam);
  if (aTexEnvParam != GL_REPLACE)
  {
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  }
  glAlphaFunc (GL_GEQUAL, 0.285f);
  glEnable (GL_ALPHA_TEST);

  // setup blending
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE

  // activate texture unit
  glDisable (GL_TEXTURE_1D);
  glEnable  (GL_TEXTURE_2D);
  if (theCtx->core13 != NULL)
  {
    theCtx->core13->glActiveTexture (GL_TEXTURE0);
  }

  // extra drawings
  switch (theTextAspect.DisplayType())
  {
    case Aspect_TODT_BLEND:
    {
      glEnable  (GL_COLOR_LOGIC_OP);
      glLogicOp (GL_XOR);
      break;
    }
    case Aspect_TODT_SUBTITLE:
    {
      glColor3fv  (theColorSubs.rgb);
      setupMatrix (thePrintCtx, theCtx, theTextAspect, OpenGl_Vec3 (0.0f, 0.0f, 0.00001f));

      glBindTexture (GL_TEXTURE_2D, 0);
      glBegin (GL_QUADS);
      glVertex2f (myBndBox.Left,  myBndBox.Top);
      glVertex2f (myBndBox.Right, myBndBox.Top);
      glVertex2f (myBndBox.Right, myBndBox.Bottom);
      glVertex2f (myBndBox.Left,  myBndBox.Bottom);
      glEnd();
      break;
    }
    case Aspect_TODT_DEKALE:
    {
      glColor3fv  (theColorSubs.rgb);
      setupMatrix (thePrintCtx, theCtx, theTextAspect, OpenGl_Vec3 (+1.0f, +1.0f, 0.00001f));
      drawText    (thePrintCtx, theCtx, theTextAspect);
      setupMatrix (thePrintCtx, theCtx, theTextAspect, OpenGl_Vec3 (-1.0f, -1.0f, 0.00001f));
      drawText    (thePrintCtx, theCtx, theTextAspect);
      setupMatrix (thePrintCtx, theCtx, theTextAspect, OpenGl_Vec3 (-1.0f, +1.0f, 0.00001f));
      drawText    (thePrintCtx, theCtx, theTextAspect);
      setupMatrix (thePrintCtx, theCtx, theTextAspect, OpenGl_Vec3 (+1.0f, -1.0f, 0.00001f));
      drawText    (thePrintCtx, theCtx, theTextAspect);
      break;
    }
    case Aspect_TODT_NORMAL:
    {
      break;
    }
  }

  // main draw call
  glColor3fv  (theColorText.rgb);
  setupMatrix (thePrintCtx, theCtx, theTextAspect, OpenGl_Vec3 (0.0f, 0.0f, 0.0f));
  drawText    (thePrintCtx, theCtx, theTextAspect);

  // revert OpenGL state
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, aTexEnvParam);
  glPopAttrib(); // enable bit
  glPopMatrix(); // model view matrix was modified
}
