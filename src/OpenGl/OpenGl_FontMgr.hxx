#ifndef OPENGL_FONT_MGR_H
#define OPENGL_FONT_MGR_H


#ifdef WNT
# include <windows.h>
# include <stdlib.h>
#endif

#include <FTFont.h>

#include <InterfaceGraphic.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <Graphic3d_NListOfHAsciiString.hxx>
#include <OSD_FontMgr.hxx>

void dump_texture();

class OpenGl_FontMgr
{
public:
  static OpenGl_FontMgr*  instance();

  int request_font( const Handle(TCollection_HAsciiString)& fontName,
    const OSD_FontAspect                   fontAspect,
    const Standard_Integer                 fontHeight );

  void render_text( const Standard_Integer id,
    const char* text,
    const Standard_Boolean is2d = 0 );

  //render text by last requested font
  void render_text( const char* text, const Standard_Boolean is2d = 0 );

  //returns direct access to FTGL font
  //Warning: don't change font pointer.
  const FTFont*   fontById( const Standard_Integer id );

  //returns width of string
  Standard_ShortReal computeWidth( const Standard_Integer id, const char* str );

  bool requestFontList( Graphic3d_NListOfHAsciiString& );

  void setCurrentScale( const Standard_ShortReal xScale = 1.f,
                        const Standard_ShortReal yScale = 1.f);

private:
  OpenGl_FontMgr();
  OpenGl_FontMgr( const OpenGl_FontMgr& ){};
  OpenGl_FontMgr& operator = ( const OpenGl_FontMgr&){ return *this;};
  ~OpenGl_FontMgr(){};

  void                          _initializeFontDB();

  typedef NCollection_List<Standard_Integer> IDList;

  struct OGLFont_SysInfo {
    Handle(OSD_SystemFont)            SysFont;
    IDList                            GeneratedFonts;
  };

  struct OGLFont_Cache {
    FTFont*            Font;
    Standard_Integer   FontHeight;
    GLCONTEXT          GlContext;
  };

  typedef NCollection_List<OGLFont_SysInfo*> FontDataBase;
  typedef FontDataBase::Iterator             FontDBIt;
  typedef NCollection_DataMap<Standard_Integer,OGLFont_Cache> FontCache;
  typedef FontCache::Iterator                                 FCacheIt;

  FontDataBase            _FontDB;
  FontCache               _FontCache;

  Standard_Integer        _CurrentFontId;

  Standard_ShortReal      _XCurrentScale,
                          _YCurrentScale;
};

#endif //OPENGL_FONT_MGR_H
