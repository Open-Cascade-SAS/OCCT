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

#ifndef OPENGL_FONT_MGR_H
#define OPENGL_FONT_MGR_H

#ifdef WNT
# include <windows.h>
# include <stdlib.h>
#endif

#ifdef HAVE_FTGL_UPPERCASE
#include <FTGLTextureFont.h>
#else
#include <FTGL/ftgl.h>
#endif

#include <InterfaceGraphic.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <Graphic3d_NListOfHAsciiString.hxx>
#include <Font_FontMgr.hxx>

void dump_texture();

class OpenGl_FontMgr
{
 public:
  static OpenGl_FontMgr*  instance();

  int request_font( const Handle(TCollection_HAsciiString)& fontName,
                    const Font_FontAspect                   fontAspect,
                    const Standard_Integer                  fontHeight );

  void render_text( const Standard_Integer id,
    const wchar_t* text,
    const Standard_Boolean is2d = Standard_False );

  //render text by last requested font
  void render_text( const wchar_t* text,
    const Standard_Boolean is2d = Standard_False );

  //returns direct access to FTGL font
  //Warning: don't change font pointer.
  const FTFont*   fontById( const Standard_Integer id );

  //returns width of string
  Standard_ShortReal computeWidth( const Standard_Integer id, const wchar_t *str );

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
    Handle(Font_SystemFont)           SysFont;
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
