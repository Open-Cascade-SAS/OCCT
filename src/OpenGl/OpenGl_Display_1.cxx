// Created on: 2011-10-25
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


#include <InterfaceGraphic.hxx>
#include <OpenGl_Display.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <OpenGl_FontMgr.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_AspectText.hxx>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

/*-----------------------------------------------------------------------------*/
/*
* Prototypes variables statiques
*/                                                 

struct FontMapNode
{
  const char *    EnumName;
  const char *    FontName;
  Font_FontAspect FontAspect;
};

static const FontMapNode myFontMap[] =
{

#ifdef WNT

  { "Courier"                  , "Courier New"    , Font_FA_Regular },
  { "Times-Roman"              , "Times New Roman", Font_FA_Regular  },
  { "Times-Bold"               , "Times New Roman", Font_FA_Bold },
  { "Times-Italic"             , "Times New Roman", Font_FA_Italic  },
  { "Times-BoldItalic"         , "Times New Roman", Font_FA_BoldItalic  },
  { "ZapfChancery-MediumItalic", "Script"         , Font_FA_Regular  },
  { "Symbol"                   , "Symbol"         , Font_FA_Regular  },
  { "ZapfDingbats"             , "WingDings"      , Font_FA_Regular  },
  { "Rock"                     , "Arial"          , Font_FA_Regular  },
  { "Iris"                     , "Lucida Console" , Font_FA_Regular  }

#else   //X11

  { "Courier"                  , "Courier"      , Font_FA_Regular },
  { "Times-Roman"              , "Times"        , Font_FA_Regular  },
  { "Times-Bold"               , "Times"        , Font_FA_Bold },
  { "Times-Italic"             , "Times"        , Font_FA_Italic  },
  { "Times-BoldItalic"         , "Times"        , Font_FA_BoldItalic  },
  { "Arial"                    , "Helvetica"    , Font_FA_Regular  }, 
  { "ZapfChancery-MediumItalic", "-adobe-itc zapf chancery-medium-i-normal--*-*-*-*-*-*-iso8859-1"              , Font_FA_Regular  },
  { "Symbol"                   , "-adobe-symbol-medium-r-normal--*-*-*-*-*-*-adobe-fontspecific"                , Font_FA_Regular  },
  { "ZapfDingbats"             , "-adobe-itc zapf dingbats-medium-r-normal--*-*-*-*-*-*-adobe-fontspecific"     , Font_FA_Regular  },
  { "Rock"                     , "-sgi-rock-medium-r-normal--*-*-*-*-p-*-iso8859-1"                             , Font_FA_Regular  },
  { "Iris"                     , "--iris-medium-r-normal--*-*-*-*-m-*-iso8859-1"                                , Font_FA_Regular  }
#endif

};

#define NUM_FONT_ENTRIES (sizeof(myFontMap)/sizeof(FontMapNode))

/*-----------------------------------------------------------------------------*/

/*
*  Constants
*/

#ifdef HAVE_GL2PS
void OpenGl_Display::getGL2PSFontName (const char *src_font, char *ps_font)
{
  /* 
  Convert font name used for rendering to some "good" font names
  that produce good vector text 
  */
  static char const *family[] = {"Helvetica", "Courier", "Times"};
  static char const *italic[] = {"Oblique", "Oblique", "Italic"};
  static char const *base[] = {"", "", "-Roman"};

  int font = 0;
  int isBold = 0;
  int isItalic = 0;

  if( strstr( src_font, "Symbol" ) ){
    sprintf( ps_font, "%s", "Symbol" );
    return;
  }

  if( strstr( src_font, "ZapfDingbats" ) ){
    sprintf( ps_font, "%s", "WingDings" );
    return;
  }

  if ( strstr( src_font, "Courier" ) ){
    font = 1;
  }
  else if ( strstr( src_font, "Times" ) ){
    font = 2;
  }

  if ( strstr( src_font, "Bold" ) ){
    isBold = 1;
  }

  if ( strstr( src_font, "Italic" ) || strstr( src_font, "Oblique" ) ){
    isItalic = 1;
  }

  if ( isBold ){
    sprintf( ps_font, "%s-%s", family[font], "Bold");
    if ( isItalic ){
      sprintf(ps_font, "%s%s", ps_font, italic[font]);
    }
  }
  else if ( isItalic )
  {
    sprintf( ps_font, "%s-%s", family[font], italic[font] );
  }
  else
  {
    sprintf( ps_font, "%s%s", family[font], base[font] );
  }
}
#endif

/*-----------------------------------------------------------------------------*/

/*
* Fonctions publiques 
*/

/*-----------------------------------------------------------------------------*/

int OpenGl_Display::FindFont (const char* AFontName, const Font_FontAspect AFontAspect,
                             const int ABestSize, const float AXScale, const float AYScale)
{   
  if (!AFontName)
    return -1;

  if (ABestSize != -1)
    myFontSize = ABestSize;

  OpenGl_FontMgr* mgr = OpenGl_FontMgr::instance();

  Handle(TCollection_HAsciiString) family_name = new TCollection_HAsciiString(AFontName);
  myFont = mgr->request_font( family_name, AFontAspect, myFontSize );

  if( myFont == -1 )
  {
    //try to use font names mapping
    FontMapNode newTempFont = myFontMap[0];
    for ( int i = 0; i < NUM_FONT_ENTRIES; ++i )
    {
      if ( TCollection_AsciiString(myFontMap[i].EnumName).IsEqual( family_name->ToCString() ) )
      {
        newTempFont = myFontMap[i];
        break;
      }
    }
    family_name = new TCollection_HAsciiString(newTempFont.FontName);
    myFont = mgr->request_font( family_name, newTempFont.FontAspect, myFontSize );
  }

  // Requested family name not found -> serach for any font family with given aspect and height
  if ( myFont == -1 )
  {
    family_name = new TCollection_HAsciiString( "" );
    myFont = mgr->request_font( family_name, AFontAspect, myFontSize );
  }

  // The last resort: trying to use ANY font available in the system
  if ( myFont == -1 )
  {
    myFont = mgr->request_font( family_name, Font_FA_Undefined, -1 );
  }

  if ( myFont != -1 )
    mgr->setCurrentScale( AXScale, AYScale );

  return myFont;
}

/*-----------------------------------------------------------------------------*/

void OpenGl_Display::StringSize (const wchar_t *str, int &width, int &ascent, int &descent)
{
  ascent = 0;
  descent = 0;
  width = 0;
  if (myFont != -1) {
    OpenGl_FontMgr* mgr = OpenGl_FontMgr::instance();
    const FTFont* font = mgr->fontById( myFont );
    if ( font ) {
      width = int( mgr->computeWidth( myFont, str ) );
      ascent = int( font->Ascender() );
      descent = int( font->Descender() );
    }
  }
}

/*
  Class       : MultilineTextRenderer
  Description : Class for constructing text and multi-line text
*/

class MultilineTextRenderer
{
  private:

  Standard_Integer        myLFNum;        // Number of '\n' (Line Feed) '\x00\x0A' 
  Standard_Integer        myCurrPos;      // Position of the current substring
  Standard_Integer        myNewStrLen;    // Length of the new string 
  Standard_Integer        mySubstrNum;    // Number of substrings
  wchar_t                 *myNewStr;      // New string
  const wchar_t           *myStrPtr;      // Pointer to the original string

  public:

  // ----------------------------------------------
  // Function: MultilineTextRenderer
  // Purpose:  Constructor with initialization
  // ----------------------------------------------
  MultilineTextRenderer ( const wchar_t           *theStr,
                          GLdouble                &theXdis,
                          GLdouble                &theYdis,
                          const OpenGl_TextParam  *theParam,
                          const FTFont            *theFnt,
                          GLint                   theWidthFont,
                          GLint                   theAscentFont,
                          GLint                   theDescentFont ) :

      myLFNum         (0),
      myCurrPos       (0),
      myNewStrLen     (0),
      mySubstrNum     (0),
      myNewStr        (0),
      myStrPtr        (&theStr[0])
  {
    const Standard_Integer  aStrLen       = (Standard_Integer) wcslen(theStr); // Length of the original string
    Standard_Integer        aNextCRChar   = 0;  // Character after '\r' (Carriage Return)        '\x00\x0D'
    Standard_Integer        aHTNum        = 0;  // Number of       '\t' (Horizontal Tabulation)  '\x00\x09'
    Standard_Integer        aDumpNum      = 0;  // Number of '\a', '\b', '\v' and '\f'
    Standard_Integer        anAllSpaces   = 0;  // Number of spaces instead of all '\t' characters
    Standard_Integer        aMaxSubstrLen = 0;  // Length of the largest substring in the new string

    Standard_Integer        aTimeVar      = 0;

    // Calculation index after last '\r' character
    for ( Standard_Integer anIndex = 0; anIndex < aStrLen; ++anIndex )
    {
      if ( theStr[anIndex] == '\r' )  aNextCRChar = anIndex+1;
    }

    // Calculation numbers of the some control characters
    for (Standard_Integer anIndex = aNextCRChar; anIndex < aStrLen; anIndex++)
    {
      ++aTimeVar;
      switch ( theStr[anIndex] )
      {
      case '\n':
        ++myLFNum;
        aTimeVar = 0;
        break;
      case '\b':
      case '\v':
      case '\f':
      case '\a':
        ++aDumpNum;
        --aTimeVar;
        break;
      case '\t':
        ++aHTNum;
        anAllSpaces += ( 8 - ( aTimeVar - 1 )%8 );
        aTimeVar = 0;
        break;
      }
    }

    // Calculation length of the new string
    myStrPtr += aNextCRChar;
    myNewStrLen = aStrLen - aNextCRChar + anAllSpaces - aHTNum - aDumpNum;

    myNewStr = new wchar_t[myNewStrLen + 1];
    myNewStr[myNewStrLen]='\0';

    CalcString (aStrLen, aMaxSubstrLen);
    CalcHAlign (theXdis, theParam, theWidthFont, aStrLen, aMaxSubstrLen);
    CalcVAlign (theYdis, theParam, theFnt, theAscentFont, theDescentFont);
  }

  // ----------------------------------------------
  // Function: ~MultilineTextRenderer
  // Purpose:  Default destructor
  // ----------------------------------------------
  ~MultilineTextRenderer ()
  {
    delete[] myNewStr;
  }

  // ----------------------------------------------
  // Function: Next
  // Purpose:  Calculate position of the next substring
  // ----------------------------------------------
  void Next ()
  {
    for ( Standard_Integer anIndex = 0; anIndex <= myNewStrLen; ++anIndex )
    {
      if ( myNewStr[myCurrPos + anIndex] == '\0' )
      {
        ++mySubstrNum;
        myCurrPos += anIndex + 1;
        break;
      }
    }
  }

  // ----------------------------------------------
  // Function: More
  // Purpose:  Calculate position of the next substring
  // ----------------------------------------------
  Standard_Boolean More ()
  {
    if ( mySubstrNum <= myLFNum )  return Standard_True;
    else                           return Standard_False;
  }

  // ----------------------------------------------
  // Function: GetValue
  // Purpose:  Returns current substring
  // ----------------------------------------------
  wchar_t* GetValue ()
  {
    return ( myNewStr + myCurrPos );
  }

  private:

  // ----------------------------------------------
  // Function: CalcString
  // Purpose:  Calculate new string separated by '\0' without '\n', '\t', '\b', '\v', '\f', '\a'
  // ----------------------------------------------
  void CalcString ( const Standard_Integer theStrLen, Standard_Integer &theMaxSubstrLen )
  {
    Standard_Integer
        aHelpIndex  = 0,
        aTimeVar    = 0,
        aSpacesNum  = 0;

    for ( Standard_Integer anIndex1 = 0, anIndex2 = 0;
          (anIndex1 < theStrLen)&&(anIndex2 < myNewStrLen);
          ++anIndex1, ++anIndex2 )
    {
      ++aTimeVar;

      if ( *(myStrPtr + anIndex1) == '\n' ) aTimeVar = 0;

      while ( (*(myStrPtr + anIndex1)=='\b')||(*(myStrPtr + anIndex1)=='\f')
            ||(*(myStrPtr + anIndex1)=='\v')||(*(myStrPtr + anIndex1)=='\a') )
      {
        ++anIndex1;
      }

      if ( *(myStrPtr + anIndex1) == '\t' )
      {
        aSpacesNum = ( 8 - ( aTimeVar - 1 )%8 );

        for ( aHelpIndex = 0; aHelpIndex < aSpacesNum; aHelpIndex++ )
        {
          myNewStr[anIndex2 + aHelpIndex] = ' ';
        }
        anIndex2 += aHelpIndex - 1;
        aTimeVar = 0;
      }
      else 
      {
        myNewStr[anIndex2] = *(myStrPtr + anIndex1);
      }
    }

    // After this part of code we will have a string separated by '\0' characters
    Standard_Integer aHelpLength = 0;

    if( myNewStr )
    {
      for( Standard_Integer anIndex = 0; anIndex <= myNewStrLen; ++anIndex )
      {
        if ( myNewStr[anIndex] == '\n' )
        {
          myNewStr[anIndex] = '\0';
        }

        // Calculating length of the largest substring of the new string.
        // It's needed for horizontal alignment
        if ( myNewStr[anIndex] != '\0' )
        {
          ++aHelpLength;
        }
        else
        {
          if ( aHelpLength > theMaxSubstrLen ) theMaxSubstrLen = aHelpLength;

          aHelpLength = 0;
        }
      }
    }
  }

  // ----------------------------------------------
  // Function: CalcVAlign
  // Purpose:  Calculate vertical alignment for text
  // ----------------------------------------------
  void CalcVAlign ( GLdouble                &theYdis,
                    const OpenGl_TextParam  *theParam,
                    const FTFont            *theFnt,
                    GLint                   theAscentFont,
                    GLint                   theDescentFont )
  {
    switch (theParam->VAlign)
    {
    case Graphic3d_VTA_BOTTOM:
      theYdis = (GLdouble)(myLFNum * theFnt->FaceSize());
      break;
    case Graphic3d_VTA_CENTER:
      if ( (myLFNum%2) == 0 )   // An odd number of strings
      {
        theYdis = (GLdouble)((myLFNum/2.0) * theFnt->FaceSize()) + theDescentFont; 
      }
      else                      // An even number of strings
      {
        theYdis = (GLdouble)((myLFNum - 1)/2.0 * theFnt->FaceSize()) - theDescentFont/2.0;
      }
      break;
    case Graphic3d_VTA_TOP:
      theYdis = -(GLdouble)theAscentFont - theDescentFont;
      break;
    default:
      theYdis = (GLdouble)(myLFNum * theFnt->FaceSize());
      break;
    }
  }

  // ----------------------------------------------
  // Function: CalcHAlign
  // Purpose:  Calculate horizontal alignment for text
  // ----------------------------------------------
  void CalcHAlign ( GLdouble                &theXdis,
                    const OpenGl_TextParam  *theParam,
                    GLint                   theWidthFont,
                    const Standard_Integer  theStrLen,
                    Standard_Integer        theMaxSubstrLen)
  {
    GLdouble aWidth = (GLdouble)(theMaxSubstrLen * theWidthFont)/theStrLen;

    switch (theParam->HAlign)
    {
    case Graphic3d_HTA_LEFT:
      theXdis = 0.;
      break;
    case Graphic3d_HTA_CENTER:
      theXdis = -0.5 * (GLdouble)aWidth;
      break;
    case Graphic3d_HTA_RIGHT:
      theXdis = -(GLdouble)aWidth;
      break;
    default:
      theXdis = 0.;
      break;
    }
  }
};

/*-----------------------------------------------------------------------------*/

void OpenGl_Display::RenderText (const wchar_t* str, const int is2d, const float x, const float y, const float z,
                                const OpenGl_AspectText *aspect, const OpenGl_TextParam *param)
{
  OpenGl_FontMgr* mgr = OpenGl_FontMgr::instance();
  const FTFont* fnt = mgr->fontById( myFont );
  if ( !fnt )
    return; 

  // FTFont changes texture state when it renders and computes size for the text
  glPushAttrib(GL_TEXTURE_BIT);

  int widthFont, ascentFont, descentFont;
  StringSize( str, widthFont, ascentFont, descentFont );

  GLdouble xdis = 0., ydis = 0.;

  float export_h = 1.;

  MultilineTextRenderer aRenderer( str,
                                   xdis,
                                   ydis,
                                   param,
                                   fnt,
                                   widthFont,
                                   ascentFont,
                                   descentFont );

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  if (is2d)
  {
    glLoadIdentity();
    glTranslatef(x, y, 0.f);
    glRotatef( 180, 1, 0, 0 );
  }
  else
  {
    const GLdouble identityMatrix[4][4] =
    {
      {1.,0.,0.,0.},
      {0.,1.,0.,0.},
      {0.,0.,1.,0.},
      {0.,0.,0.,1.}
    };

    GLdouble projMatrix[4][4], modelMatrix[4][4];
    GLint viewport[4];

    GLdouble wx, wy, wz;
    GLdouble x1, y1, z1;
    GLdouble x2, y2, z2;

    glGetDoublev( GL_MODELVIEW_MATRIX, (GLdouble*)modelMatrix );
    glGetDoublev( GL_PROJECTION_MATRIX, (GLdouble*)projMatrix );
    glGetIntegerv( GL_VIEWPORT, (GLint*)viewport );

    gluProject( x, y, z,
      (GLdouble*)modelMatrix,
      (GLdouble*)projMatrix,
      (GLint*)viewport,
      &wx, &wy, &wz );
    glLoadIdentity();
    gluUnProject( wx, wy, wz, 
      (GLdouble*)identityMatrix, (GLdouble*)projMatrix, (GLint*)viewport,
      &x1, &y1 , &z1 );    

    GLdouble h = (GLdouble)fnt->FaceSize();

    gluUnProject( wx, wy + h - 1., wz,
      (GLdouble*)identityMatrix, (GLdouble*)projMatrix, (GLint*)viewport,
      &x2, &y2, &z2 );

    h = (y2-y1)/h;

    glTranslated( x1, y1 , z1 );   
    glRotated(aspect->Angle(), 0, 0, 1);
    glTranslated(xdis, ydis, 0);  

    if( !aspect->IsZoomable() )
    {
#ifdef WNT
      // if the context has assigned printer context, use it's parameters
      OpenGl_PrinterContext* aPrinterContext = 
        OpenGl_PrinterContext::GetPrinterContext( GET_GL_CONTEXT() );
      if( aPrinterContext )
      {
        // get printing scaling in x and y dimensions
        GLfloat aTextScalex = 1, aTextScaley = 1;
        aPrinterContext->GetScale( aTextScalex, aTextScaley );
        
        // text should be scaled in all directions with same
        // factor to save its proportions, so use height (y) scaling
        // as it is better for keeping text/3d graphics proportions
        glScalef( aTextScaley, aTextScaley, aTextScaley );
      }
#endif
      glScaled( h, h, h );
    }
    else
    {
      export_h = (float)h;
    }
  }

  GLint renderMode;  
  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  if ( renderMode == GL_FEEDBACK ) 
  {
#ifdef HAVE_GL2PS
    export_h = (float)fnt->FaceSize() / export_h;
    for ( ; aRenderer.More(); aRenderer.Next() )
    {
      // x, y, z coordinates are used here as zero values, because position of the text
      // and alignment is calculated in the code above using glTranslated methods
      ExportText( aRenderer.GetValue(), is2d, 0, 0, 0, aspect, param, (short)export_h );
      glTranslated(0, -(GLdouble)fnt->FaceSize(), 0);
    }
#endif
  }
  else
  {
    for ( ; aRenderer.More(); aRenderer.Next() )
    {
      mgr->render_text( myFont, aRenderer.GetValue(), is2d );
      glTranslated(0, -(GLdouble)fnt->FaceSize(), 0);
    }
  }
  glPopMatrix();
  glPopAttrib();
}

/*-----------------------------------------------------------------------------*/

void OpenGl_Display::ExportText (const wchar_t* text, const int is2d, const float x, const float y, const float z,
                                const OpenGl_AspectText *aspect, const OpenGl_TextParam *param, const short height)
{
#ifdef HAVE_GL2PS

  const char* fontname = aspect->Font();
  float angle = aspect->Angle();

  GLubyte zero = 0;
  char ps_font[64];

  getGL2PSFontName(fontname, ps_font);

  if( is2d )
    glRasterPos2f( x, y );
  else
    glRasterPos3f( x, y, z );

  glBitmap( 1, 1, 0, 0, 0, 0, &zero );

  //szv: workaround for gl2ps!
  const int len = 4 * (wcslen(text) + 1); //szv: should be more than enough
  char *astr = new char[len];
  wcstombs(astr,text,len);

  // Standard GL2PS's alignment isn't used, because it doesn't work correctly
  // for all formats, therefore alignment is calculated manually relative
  // to the bottom-left corner, which corresponds to the GL2PS_TEXT_BL value
  gl2psTextOpt(astr, ps_font, height, GL2PS_TEXT_BL, angle);
  delete[] astr;

#endif
}
