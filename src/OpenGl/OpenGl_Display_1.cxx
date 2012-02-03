// File:      OpenGl_Display_1.cxx
// Created:   25 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_Display.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <OpenGl_FontMgr.hxx>

#include <OpenGl_AspectText.hxx>

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
  OSD_FontAspect  FontAspect;
};

static const FontMapNode myFontMap[] =
{

#ifdef WNT

  { "Courier"                  , "Courier New"    , OSD_FA_Regular },
  { "Times-Roman"              , "Times New Roman", OSD_FA_Regular  },
  { "Times-Bold"               , "Times New Roman", OSD_FA_Bold },
  { "Times-Italic"             , "Times New Roman", OSD_FA_Italic  },
  { "Times-BoldItalic"         , "Times New Roman", OSD_FA_BoldItalic  },
  { "ZapfChancery-MediumItalic", "Script"         , OSD_FA_Regular  },
  { "Symbol"                   , "Symbol"         , OSD_FA_Regular  },
  { "ZapfDingbats"             , "WingDings"      , OSD_FA_Regular  },
  { "Rock"                     , "Arial"          , OSD_FA_Regular  },
  { "Iris"                     , "Lucida Console" , OSD_FA_Regular  }

#else   //X11

  { "Courier"                  , "Courier"      , OSD_FA_Regular },
  { "Times-Roman"              , "Times"        , OSD_FA_Regular  },
  { "Times-Bold"               , "Times"        , OSD_FA_Bold },
  { "Times-Italic"             , "Times"        , OSD_FA_Italic  },
  { "Times-BoldItalic"         , "Times"        , OSD_FA_BoldItalic  },
  { "Arial"                    , "Helvetica"    , OSD_FA_Regular  }, 
  { "ZapfChancery-MediumItalic", "-adobe-itc zapf chancery-medium-i-normal--*-*-*-*-*-*-iso8859-1"              , OSD_FA_Regular  },
  { "Symbol"                   , "-adobe-symbol-medium-r-normal--*-*-*-*-*-*-adobe-fontspecific"                , OSD_FA_Regular  },
  { "ZapfDingbats"             , "-adobe-itc zapf dingbats-medium-r-normal--*-*-*-*-*-*-adobe-fontspecific"     , OSD_FA_Regular  },
  { "Rock"                     , "-sgi-rock-medium-r-normal--*-*-*-*-p-*-iso8859-1"                             , OSD_FA_Regular  },
  { "Iris"                     , "--iris-medium-r-normal--*-*-*-*-m-*-iso8859-1"                                , OSD_FA_Regular  }
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

int OpenGl_Display::FindFont (const char* AFontName, const OSD_FontAspect AFontAspect,
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
    myFont = mgr->request_font( family_name, OSD_FA_Undefined, -1 );
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

  GLdouble xdis = 0.;
  switch (param->HAlign)
  {
    case Graphic3d_HTA_CENTER:
      xdis = -0.5 * (GLdouble)widthFont;
      break;
    case Graphic3d_HTA_RIGHT:
      xdis = -(GLdouble)widthFont;
      break;
    //case Graphic3d_HTA_LEFT:
    //default: break;
  }
  GLdouble ydis = 0.;
  switch (param->VAlign)
  {
    case Graphic3d_VTA_CENTER:
      ydis = -0.5 * (GLdouble)ascentFont - descentFont;
      break;
    case Graphic3d_VTA_TOP:
      ydis = -(GLdouble)ascentFont - descentFont;
      break;
    //case Graphic3d_VTA_BOTTOM:
    //default: break;
  }

  float export_h = 1.;

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
      glScaled( h, h, h );
    }
    else
    {
      export_h = (float )h;
    }
  }

  GLint renderMode;  
  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  if ( renderMode == GL_FEEDBACK ) 
  {
#ifdef HAVE_GL2PS
    export_h = (GLdouble)fnt->FaceSize() / export_h;
    glPopMatrix();
    ExportText( str, is2d, x, y, z, aspect, param, export_h );
#endif
  }
  else
  {
    mgr->render_text( myFont, str, is2d );
    glPopMatrix();
  }
  glPopAttrib();
}

/*-----------------------------------------------------------------------------*/

static const int alignmentforgl2ps[3][3] = { {5,2,8}, {4,1,7}, {6,3,9} };

void OpenGl_Display::ExportText (const wchar_t* text, const int is2d, const float x, const float y, const float z,
                                const OpenGl_AspectText *aspect, const OpenGl_TextParam *param, const float height)
{
#ifdef HAVE_GL2PS

  int vh = 1;
  switch (param->HAlign)
  {
    case Graphic3d_HTA_CENTER: vh = 2; break;
    case Graphic3d_HTA_RIGHT: vh = 3; break;
    //case Graphic3d_HTA_LEFT:
    //default: break;
  }

  int vv = 1;
  switch (param->VAlign)
  {
    case Graphic3d_VTA_CENTER: vv = 2; break;
    case Graphic3d_VTA_TOP: vv = 3; break;
    //case Graphic3d_VTA_BOTTOM:
    //default: break;
  }

  const int alignment = alignmentforgl2ps[vh-1][vv-1];

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
  gl2psTextOpt(astr, ps_font, height, alignment, angle);
  delete[] astr;

#endif
}
