#include <OSD_FontAspect.hxx>
#include <InterfaceGraphic_tgl_all.hxx>
#include <OpenGl_tgl_all.hxx>
#include <InterfaceGraphic_telem.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <FTFont.h>

#define NUM_FONT_SIZES 40
#define NUM_CHAR_FONT 1024
#define CACHE_SIZE 30

class OpenGl_TextRender
{
 public:

  static OpenGl_TextRender* instance();
  Tint FindFont ( Tchar*, OSD_FontAspect, Tfloat, Tfloat = 1. , Tfloat = 1.);

  void StringSize( const wchar_t *text, GLint *width, GLint *ascent, GLint *descent);
  void RenderText( const wchar_t*, GLuint, int, GLfloat, GLfloat, GLfloat );
  void ExportText( const wchar_t* text, char* fontname, GLfloat height, GLfloat angle, GLint alingment, GLfloat x, GLfloat y, GLfloat z, GLboolean is2d );
#ifdef HAVE_GL2PS
  static void getGL2PSFontName(char *src_font, char *ps_font);
#endif

 private:

  static int curFont  ;
  static int curSize  ;
  static int curScale ;
  static int curTexFont ; 
#ifdef HAVE_GL2PS
  int alignmentforgl2ps(int Hmode, int Vmode);
#endif

  struct FontMapNode 
  {
    char*     enumName;
    char*     FontName;
    OSD_FontAspect  fontAspect;
  };

  struct FontEntry
  {
    char*     name;
    char*     xlfd;
    float     xsizes[NUM_FONT_SIZES];
    int       count;
  };

  FontMapNode searchFontInMap( Handle(TCollection_HAsciiString)& fontName );

  static FontMapNode  fontMap[];
  static FontEntry    fontEntry[];

  OpenGl_TextRender();
  OpenGl_TextRender( const OpenGl_TextRender& ){};
  OpenGl_TextRender& operator = ( const OpenGl_TextRender&){ return *this;};
 ~OpenGl_TextRender(){};


  struct OGLFont_Cache 
  {
    FTFont*            Font;
    Standard_Integer   FontHeight;
    GLCONTEXT          GlContext;
  };

  typedef NCollection_DataMap<Standard_Integer,OGLFont_Cache> FontCache;
  FontCache               _FontCache;
  Standard_Integer        _CurrentFontId;
  Standard_ShortReal      _XCurrentScale,
                          _YCurrentScale; 

};
