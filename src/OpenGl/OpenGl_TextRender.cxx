/*
* Includes
*/
#include <stdio.h>
#include <string.h>

#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <Standard_Stream.hxx>

#include <OpenGl_FontMgr.hxx>   
#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_TextRender.hxx>
#include <OpenGl_telem_attri.hxx>
#include <OpenGl_cmn_varargs.hxx>

#include <OSD_Environment.hxx>
#include <Quantity_NameOfColor.hxx>
#include <AlienImage.hxx>

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif


/*-----------------------------------------------------------------------------*/
/*
* Prototypes variables statiques
*/                                                 

int OpenGl_TextRender::curFont    = -1;
int OpenGl_TextRender::curSize    = -1;
int OpenGl_TextRender::curScale   = -1;
int OpenGl_TextRender::curTexFont = -1;

OpenGl_TextRender::FontMapNode OpenGl_TextRender::fontMap[] = {

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

OpenGl_TextRender::FontEntry OpenGl_TextRender::fontEntry[] = {

  { "Courier"                  , "Courier New"                 },
  { "Times-Roman"              , "Times New Roman"             },
  { "Times-Bold"               , "Times New Roman Bold"        },
  { "Times-Italic"             , "Times New Roman Italic"      },
  { "Times-BoldItalic"         , "Times New Roman Bold Italic" },
  { "ZapfChancery-MediumItalic", "Script"                      },
  { "Symbol"                   , "Symbol"                      },
  { "ZapfDingbats"             , "WingDings"                   },
  { "Rock"                     , "Arial"                       },
  { "Iris"                     , "Lucida Console"              }

};

#define NUM_FONT_ENTRIES (sizeof(fontMap)/sizeof(FontMapNode))

/*-----------------------------------------------------------------------------*/

/*
*  Constants
*/

#ifdef HAVE_GL2PS
void OpenGl_TextRender::getGL2PSFontName(char *src_font, char *ps_font)
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
* Constructors
*/

OpenGl_TextRender::OpenGl_TextRender()
: _CurrentFontId(-1),
_XCurrentScale(1.f),
_YCurrentScale(1.f) {
}



/*-----------------------------------------------------------------------------*/

/*
* Fonctions publiques 
*/

OpenGl_TextRender* OpenGl_TextRender::instance() {
  static OpenGl_TextRender* _textRend = NULL;
  if ( _textRend == NULL )
  {
    _textRend = new OpenGl_TextRender();
  }

  return _textRend;
}


/*----------------------------------------------------------------------*/
OpenGl_TextRender::FontMapNode OpenGl_TextRender::searchFontInMap( Handle(TCollection_HAsciiString)& fontName ) {
  for ( int i = 0; i < NUM_FONT_ENTRIES; ++i )
  {
    TCollection_AsciiString compare_String(fontMap[i].enumName) ;
    if(compare_String.IsEqual( fontName->ToCString() ))
    {
      return fontMap[i];
    }  
  }   
  //default font returns
  return fontMap[0];
}

/*-----------------------------------------------------------------------------*/

Tint OpenGl_TextRender::FindFont ( Tchar* fontName,
                                  OSD_FontAspect aspect,
                                  Tfloat bestSize,
                                  Tfloat xScale ,
                                  Tfloat yScale ) 
{   
  if (!fontName)
    return -1;
  OpenGl_FontMgr* mgr =  OpenGl_FontMgr::instance();  

  Handle(TCollection_HAsciiString) family_name
    = new TCollection_HAsciiString((char*)fontName);

  curFont = mgr->request_font(family_name,
    aspect,
    Standard_Integer(bestSize) );

  if( curFont == -1 ) {
    //try to use font names mapping
    FontMapNode newTempFont = searchFontInMap ( family_name );
    curFont = mgr->request_font( new TCollection_HAsciiString(newTempFont.FontName),
      newTempFont.fontAspect,
      Standard_Integer(bestSize) );
  }

  // Requested family name not found -> serach for any font family with given aspect and height
  family_name = new TCollection_HAsciiString( "" );
  if ( curFont == -1 ) {
    curFont = mgr->request_font(family_name, aspect, Standard_Integer(bestSize) );
  }

  // The last resort: trying to use ANY font available in the system
  if ( curFont == -1 ) {
    curFont = mgr->request_font(family_name, OSD_FA_Undefined, -1 );
  }

  if ( curFont != -1 )
    mgr->setCurrentScale( xScale, yScale );
  return curFont;

}

/*-----------------------------------------------------------------------------*/
void OpenGl_TextRender::StringSize(char *str, GLint *Width, GLint *Ascent, GLint *Descent)
{

  /*    int       dir, asc, des;*/
  *Ascent = 0;
  *Descent = 0;
  *Width = 0;
  if (curFont != -1) {
    OpenGl_FontMgr* mgr = OpenGl_FontMgr::instance();
    const FTFont* font = mgr->fontById( curFont );
    if ( font ) {
      *Width = GLint( mgr->computeWidth( curFont, str ) );
      *Ascent = GLint( font->Ascender() );
      *Descent = GLint( font->Descender() );
    }
  }
#ifdef TRACE
  printf("sizeString::asc = %d des = %d width = %d \n", *Ascent, *Descent, *Width); 
#endif

}

/*-----------------------------------------------------------------------------*/

void OpenGl_TextRender::RenderText ( char* str, GLuint base, int is2d, GLfloat x, GLfloat y, GLfloat z ) 
{
  GLdouble projMatrix[4][4], modelMatrix[4][4];
  GLint viewport[4];
  GLint widthFont, ascentFont, descentFont;
  GLdouble xdis = 0., ydis = 0.;
  GLint renderMode;  

  // FTFont changes texture state when it renders and computes size for the text
  glPushAttrib(GL_TEXTURE_BIT);
  StringSize(str, &widthFont, &ascentFont, &descentFont );

  GLdouble identityMatrix[4][4] = 
  {
    {1.,0.,0.,0.},
    {0.,1.,0.,0.},
    {0.,0.,1.,0.},
    {0.,0.,0.,1.}
  };

  TEL_ALIGN_DATA align;

  CMN_KEY key;
  key.id = TelTextAlign;
  key.data.pdata = &align;
  TsmGetAttri( 1, &key );

  Tfloat angle = 0;
  CMN_KEY keyAngle;
  keyAngle.id = TelTextAngle;//This flag responding about Angle text
  TsmGetAttri( 1, &keyAngle );
  angle = keyAngle.data.ldata;

  Tint zoom = 0;
  CMN_KEY keyZoom;
  keyZoom.id = TelTextZoomable;//This flag responding about Zoomable text
  TsmGetAttri( 1, &keyZoom );
  zoom = keyZoom.data.ldata;

  OpenGl_TextRender* textRender = OpenGl_TextRender::instance(); 
  int vh = 2 ;
  int vv = 2 ;

  switch( align.Hmode )
  {
  case CALL_PHIGS_HOR_LEFT://0
    xdis =0.; vh = 1;
    break;
  case CALL_PHIGS_HOR_CENTER://1
    xdis = -(GLdouble)widthFont / 2.0;  vh = 2;
    break;
  case CALL_PHIGS_HOR_RIGHT://2
    xdis = -(GLdouble)widthFont; vh = 3;
    break;
  default:
    xdis = 0.;
  }

  switch( align.Vmode )
  {
  case CALL_PHIGS_VERT_BOTTOM://0
    ydis = 0.; vv = 1;
    break;
  case CALL_PHIGS_VERT_CENTER://1
    ydis = -(GLdouble)(ascentFont) / 2.0 - descentFont; vv = 2;
    break;
  case CALL_PHIGS_VERT_TOP://2
    ydis = -(GLdouble)ascentFont - descentFont; vv= 3;
    break;    
  default:
    ydis = 0.;
  }

  OpenGl_FontMgr* mgr = OpenGl_FontMgr::instance();

  const FTFont* fnt = mgr->fontById( curFont );
  if ( !fnt ) {
    glPopAttrib();
    return; 
  }

  float export_h = 1.;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  if (is2d) {
    glLoadIdentity();
    glTranslatef(x, y, 0.f);
    glRotatef( 180, 1, 0, 0 );
  }
  else {   
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
    glRotated(angle, 0, 0, 1); 
    glTranslated(xdis, ydis, 0);  

    if( ! zoom )
    {
      glScaled( h, h, h );
    }
    else
    {
      export_h = h;
    }
  }
  glGetIntegerv(GL_RENDER_MODE, &renderMode);
  if ( renderMode == GL_FEEDBACK ) 
  {
#ifdef HAVE_GL2PS
    CMN_KEY keyfontName;
    keyfontName.id = TelTextFont;//This flag responding about TextFontName
    TsmGetAttri( 1, &keyfontName );
    char *fontName = new char[strlen((char*)keyfontName.data.pdata) + 1];
    strcpy(fontName,(char*)keyfontName.data.pdata);

    export_h = (GLdouble)fnt->FaceSize() / export_h;
    int aligment = alignmentforgl2ps( vh, vv );
    glPopMatrix();
    ExportText( str, fontName, export_h, angle, aligment, x, y, z, is2d!=0 );
    delete [] fontName;
#endif
  }
  else
  {
    mgr->render_text( curFont, str, is2d );
    glPopMatrix();
  }
  glPopAttrib();
  return;

}

#ifdef HAVE_GL2PS
int OpenGl_TextRender::alignmentforgl2ps(int vh, int vy)
{
  if( vh == 1 && vy == 1)
  {
    return 5;
  }
  if( vh == 2 && vy == 1)
  {
    return 4;
  }
  if( vh == 3 && vy == 1)
  {
    return 6;
  }

  if( vh == 1 && vy == 2)
  {
    return 2;
  }
  if( vh == 2 && vy == 2)
  {
    return 1;
  }
  if( vh == 3 && vy == 2)
  {
    return 3;
  }

  if( vh == 1 && vy == 3)
  {
    return 8;
  }
  if( vh == 2 && vy == 3)
  {
    return 7;
  }
  if( vh == 3 && vy == 3)
  {
    return 9;
  }
  return 0;
};
#endif

/*-----------------------------------------------------------------------------*/
void OpenGl_TextRender::ExportText( char* str, char* fontname, GLfloat height, GLfloat angle, GLint alignment,
                                    GLfloat x, GLfloat y, GLfloat z, GLboolean is2d )
{
#ifdef HAVE_GL2PS

  GLubyte zero = 0;
  char ps_font[64];

  getGL2PSFontName(fontname, ps_font);

  if( is2d )
    glRasterPos2f( x, y );
  else
    glRasterPos3f( x, y, z );
  
  glBitmap( 1, 1, 0, 0, 0, 0, &zero );

  gl2psTextOpt( str, ps_font, height, alignment, angle);

#endif

}
