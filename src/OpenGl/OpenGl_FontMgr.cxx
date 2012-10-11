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

#include <OpenGl_FontMgr.hxx>
#include <OpenGl_GlCore11.hxx>

#include <Standard_Stream.hxx>

#include <ft2build.h>

#ifdef _MSC_VER
#pragma comment( lib, "ftgl.lib" )
#pragma comment( lib, "freetype.lib" )
#endif

#undef TRACE

#define DEFAULT_FONT_HEIGHT 16

float h_scale = 0;

void dump_texture( int id);

class Font_DataMap:
  public NCollection_DataMap< Handle(TCollection_HAsciiString),
  Handle(TCollection_HAsciiString)>
{
public:
  inline Font_DataMap
    (const Standard_Integer NbBuckets = 1,
    const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    :  NCollection_DataMap<Handle(TCollection_HAsciiString),
    Handle(TCollection_HAsciiString)> (NbBuckets, theAllocator)
  {}

  inline Font_DataMap (const Font_DataMap& theOther)
    :  NCollection_DataMap<Handle(TCollection_HAsciiString),
    Handle(TCollection_HAsciiString)> (theOther)
  {}  
  friend Standard_Boolean IsEqual( const Handle(TCollection_HAsciiString)& h1,
    const Handle(TCollection_HAsciiString)& h2 );
};
inline Standard_Boolean
IsEqual( const Handle(TCollection_HAsciiString)& h1,
        const Handle(TCollection_HAsciiString)& h2 )
{
  return (!h1->IsLess(h2) && !h1->IsGreater(h2));
}

OpenGl_FontMgr::OpenGl_FontMgr()
: _CurrentFontId(-1),
_XCurrentScale(1.f),
_YCurrentScale(1.f) 
{
}

OpenGl_FontMgr* OpenGl_FontMgr::instance() 
{
  static OpenGl_FontMgr* _mgr = NULL;
  if ( _mgr == NULL )
  {
    _mgr = new OpenGl_FontMgr();
    _mgr->_initializeFontDB();
  }
  return _mgr;
}

void OpenGl_FontMgr::_initializeFontDB()
{
  Handle(Font_FontMgr) fntMgr = Font_FontMgr::GetInstance();
  if ( !fntMgr.IsNull() ) {

    Font_NListOfSystemFont fontList = fntMgr->GetAvalableFonts();
    if ( fontList.Size() != 0 ) {

      // The library used as a tool for checking font aspect since Font_FontMgr
      // fails to get aspect for the fonts that have name dependant
      // on system locale.
      FT_Library aFtLibrary;
      FT_Error aLibError = FT_Init_FreeType(&aFtLibrary);
      
      Font_NListOfSystemFont::Iterator it(fontList);
      for ( ; it.More(); it.Next() ) {
        OGLFont_SysInfo* info = new OGLFont_SysInfo();
        if ( it.Value()->FontAspect() == Font_FA_Regular ) {
          
          Handle(TCollection_HAsciiString) aFontPath = it.Value()->FontPath();
            
          //this workaround for fonts with names dependent on system locale.
          //for example: "Times New Roman Fett Kursive" or "Times New Roman Gras Italiqui"
          FT_Face aFontFace;
          FT_Error aFaceError = FT_New_Face(aFtLibrary,
                                            aFontPath->ToCString(), 0,
                                            &aFontFace);
              
          if ( aFaceError == FT_Err_Ok ) {
            if ( aFontFace->style_flags == 0 ) {
              info->SysFont = it.Value();
            }
            else {
              //new aspect detected for current font item
#ifdef TRACE
              cout << "TKOpenGl::initializeFontDB() detected new font!\n"
                << "\tFont Previous Name: " << it.Value()->FontName()->ToCString() << endl
                << "\tFont New Name: " << aFontFace->family_name << endl
                << "\tFont Aspect: " << aFontFace->style_flags << endl;
#endif
              Font_FontAspect aspect = Font_FA_Regular;
              if ( aFontFace->style_flags == (FT_STYLE_FLAG_ITALIC | FT_STYLE_FLAG_BOLD) )
                aspect = Font_FA_BoldItalic;
              else if ( aFontFace->style_flags == FT_STYLE_FLAG_ITALIC )
                aspect = Font_FA_Italic;
              else if ( aFontFace->style_flags == FT_STYLE_FLAG_BOLD )
                aspect = Font_FA_Bold;

#ifdef TRACE
              cout << "\tFont_FontAspect: " << aspect << endl;
#endif
              Handle(TCollection_HAsciiString) aFontName =
                new TCollection_HAsciiString( aFontFace->family_name );
              info->SysFont = new Font_SystemFont( aFontName, aspect, aFontPath );
            }
            
            FT_Done_Face(aFontFace);
          }
          else
            continue;
        } else {
          info->SysFont = it.Value();
        }
        _FontDB.Append(info);

      }
      
      // finalize library instance
      if ( aLibError == FT_Err_Ok )
      {
        FT_Done_FreeType(aFtLibrary);
      }
    }
  }

#ifdef TRACE
  if ( !_FontDB.Size() ) {
    cout << "\nTKOpenGl::initializeFontDB() FAILED!!!\n"
      << "No fonts detected in the system!\n"
      << "Text rendering in 3D viewer will not be available." << endl;
  }
#endif    
}

bool OpenGl_FontMgr::requestFontList( Graphic3d_NListOfHAsciiString& lst)
{
  FontDBIt DBit(_FontDB);
  lst.Clear();
  for ( ; DBit.More(); DBit.Next() ) {
    lst.Append( DBit.Value()->SysFont->FontName() );
  }
  return true;
}

// Empty fontName means that ANY family name can be used.
// fontAspect == Font_FA_Undefined means ANY font aspect is acceptable.
// fontheight == -1 means ANY font height is acceptable.
int OpenGl_FontMgr::request_font( const Handle(TCollection_HAsciiString)& fontName,
                                 const Font_FontAspect                   fontAspect,
                                 const Standard_Integer                  fontHeight )
{
  Standard_Integer aFontHeight = fontHeight;
  if ( aFontHeight < 2 && aFontHeight != -1 )
  {
#ifdef TRACE
    cout << "TKOpenGl::request_font\n"
      << "  Error: font height is invalid!!!\n"
      << "  font height:" << aFontHeight << "\n";
#endif //TRACE
    return -1;
  } 

#ifdef TRACE
  cout << "TKOpenGl::request_font\n"
    << "  font name: " << fontName->ToCString() << endl
    << "  font aspect: " << fontAspect << endl
    << "  font height: " << aFontHeight << endl;
#endif

  GLCONTEXT ctx = GET_GL_CONTEXT();

  //check for font 
  FontDBIt DBit(_FontDB);

#ifdef TRACE
  cout << "Searching font in font database...\n";
#endif

  for ( ; DBit.More(); DBit.Next() ) {
    // san (23.07.2010): comparing font names in case-insensitive mode,
    // as on SUN and SGI system font names are in lower-case
    if ( fontName->IsEmpty() || DBit.Value()->SysFont->FontName()->IsSameString(fontName, Standard_False) ) {
#ifdef TRACE
      cout << "\tName is found...\n\tCheking aspect...\n";
#endif

      //check for font aspect
      if (fontAspect != Font_FA_Undefined && DBit.Value()->SysFont->FontAspect() != fontAspect) {
#ifdef TRACE
        cout << "\tAspect of candidate font: " << DBit.Value()->SysFont->FontAspect() << endl;
        cout << "\tAspects are not equal! Continue seaching...\n";
#endif
        continue;
      }

#ifdef TRACE
      cout << "\tAspect is found...\n\tCheking height...\n";
#endif
      //check for fixed height
      if (DBit.Value()->SysFont->FontHeight() != -1) {
#ifdef TRACE
        cout << "\tChecking fixed height: " << DBit.Value()->SysFont->FontHeight() << endl;
#endif
        //fixed height font
        if ( aFontHeight != -1 && DBit.Value()->SysFont->FontHeight() != aFontHeight ){
#ifdef TRACE
          cout << "\tHeights are not equal! Continue seaching...\n";
#endif
          continue;
        }
        else
        {
          // We need to remember the font height to be used
          if ( aFontHeight == -1 )
            aFontHeight = DBit.Value()->SysFont->FontHeight();
#ifdef TRACE
          cout << "\tHeight is found\n";
#endif    
        }
      }
      else 
      {
        // If font height is not specified -> use DEFAULT_FONT_HEIGHT for variable-height fonts
        if ( aFontHeight == -1 )
            aFontHeight = DEFAULT_FONT_HEIGHT;
#ifdef TRACE
        cout << "\tFont has variable height == -1. height is found\n";
#endif
      }
#ifdef TRACE
      cout << "\tChecking font manager cache...\n";
#endif  
      //check in loaded fonts
      IDList::Iterator fIt(DBit.Value()->GeneratedFonts);
      for ( ; fIt.More(); fIt.Next() ) {
        OGLFont_Cache cache = _FontCache.Find( fIt.Value() );
        if ( cache.FontHeight == aFontHeight && cache.GlContext == ctx ) {
          //requested font is generated already

#ifdef TRACE
          cout << "\tRequested font is generated already\n\tId = "  ;
          cout << fIt.Value() << endl;
#endif                                                    
          _CurrentFontId = fIt.Value();
          return _CurrentFontId;
        } else {
#ifdef TRACE
          cout << "\t\tHeights or contexts are not equal:\n"
            << "\t\t\tfont height: "  << aFontHeight << "\tchache height: " << cache.FontHeight << endl
            << "\t\t\tfont context: " << ctx << "\tchache context: " << cache.GlContext << endl;
          cout << "\t\tContinue searching in cache...\n";
#endif
        }
      }
#ifdef TRACE
      cout << "\tRequested font is not found among genarated fonts.\n\tCreating new...\n";
#endif
      //if not found in generated fonts
      //create new
      FTGLTextureFont* font = new FTGLTextureFont(DBit.Value()->SysFont->FontPath()->ToCString());
      //by default creates regular font
      if ( ! font || font->Error() != FT_Err_Ok) {
#ifdef TRACE
        cout << "\t\tError during creation FTGL font object!\n";
#endif
        return -1;
      }

      if ( ! font->FaceSize( aFontHeight) || font->Error() != FT_Err_Ok ){
#ifdef TRACE
        cout << "\t\tError during setup FTGL font height!\n";
#endif
        return -1;
      }
      font->UseDisplayList( false );

      //put font to cache
      OGLFont_Cache cache;
      cache.Font = font;
      cache.FontHeight = aFontHeight;     
      cache.GlContext = ctx;

      _CurrentFontId =_FontCache.Size() + 1;
      _FontCache.Bind( _CurrentFontId, cache );
      DBit.Value()->GeneratedFonts.Append(_CurrentFontId);

#ifdef TRACE
      cout << "TKOpenGl::Loaded New FTGL font:\n"
        << "  font name: " << fontName->ToCString() << "\n"
        << "  font aspect: ";

      switch( fontAspect ) 
      {
      case Font_FA_Bold:
        cout << "Font_FA_Bold\n";
        break;
      case Font_FA_BoldItalic:
        cout << "Font_FA_BoldItalic\n";
        break;
      case Font_FA_Italic:
        cout << "Font_FA_Italic\n";
        break;
      case Font_FA_Regular:
        cout << "Font_FA_Regular\n";
        break;
      default:
        cout << "Font_FA_Undefined\n";
        break;
      }
      cout  << "  font height: "<<aFontHeight<<"\n";
      cout << "  font id: " << _CurrentFontId << "\n";
#endif

      return _CurrentFontId;
    }
  }
  //invalid family name
#ifdef TRACE
  cout << "\n---Invalid Family Name!!!---\n";
#endif
  return -1;
}

void OpenGl_FontMgr::render_text( const Standard_Integer id, const wchar_t* text,
                                  const Standard_Boolean is2d )
{
#ifdef TRACE
  cout << "TKOpenGl::render_text\n"
    << "\tfont id = " << id << endl
    << "\ttext = " << text << endl;
#endif
  if ( text && _FontCache.IsBound( id ) ) {
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    glScalef( _XCurrentScale, _YCurrentScale, 1 );
    glPushAttrib( GL_ENABLE_BIT );

    GLboolean enableTexture = glIsEnabled(GL_TEXTURE_2D);
    GLboolean enableDepthTest = glIsEnabled(GL_DEPTH_TEST);

    if( !enableTexture )
      glEnable(GL_TEXTURE_2D);
    if ( !is2d ) {
      if ( !enableDepthTest )
        glEnable(GL_DEPTH_TEST);
    }
    else if ( enableDepthTest ) {
        glDisable(GL_DEPTH_TEST);
    }

    GLint param;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &param);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    glAlphaFunc(GL_GEQUAL, 0.285f);    
    glEnable(GL_ALPHA_TEST);   
    OGLFont_Cache cache = _FontCache.Find( id );
    cache.Font->Render( text );

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, param);

    if( !enableTexture )
      glDisable(GL_TEXTURE_2D);
    if( !enableDepthTest )
      glDisable(GL_DEPTH_TEST);

    glPopAttrib();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
  }

}

void OpenGl_FontMgr::render_text ( const wchar_t* text, const Standard_Boolean is2d )
{
  render_text( _CurrentFontId, text, is2d );
}

const FTFont* OpenGl_FontMgr::fontById (const Standard_Integer id)
{
  return _FontCache.IsBound( id ) ? _FontCache.Find( id ).Font: NULL;
}

Standard_ShortReal OpenGl_FontMgr::computeWidth( const Standard_Integer id, const wchar_t* text )
{
  if( !_FontCache.IsBound( id ) )
    return 0.f;

  OGLFont_Cache cache = _FontCache.Find( id );

  Standard_ShortReal w = cache.Font->Advance( text );

  return w;
}

void OpenGl_FontMgr::setCurrentScale (const Standard_ShortReal xScale,
                                      const Standard_ShortReal yScale)
{
  _XCurrentScale = xScale;
  _YCurrentScale = yScale;
}

#include <AlienImage_BMPAlienData.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Image_ColorImage.hxx>
#include <Quantity_Color.hxx>

void dump_texture( int id)
{
  Handle(AlienImage_BMPAlienData) image = new AlienImage_BMPAlienData();

  if (!glIsTexture(id))
    return;

  unsigned char* pixels = new unsigned char[8192*1024];
  memset( pixels, 0, 8192*1024 );
  static int index = 0;
  index++;

  glBindTexture(GL_TEXTURE_2D, id );
  glGetTexImage( GL_TEXTURE_2D , 
    0,
    GL_ALPHA,
    GL_UNSIGNED_BYTE,
    pixels );

  Handle(Image_ColorImage) anImage = new Image_ColorImage( 0, 0, 1024, 8192 );

  Aspect_ColorPixel mark( Quantity_Color (0.,0.5,1., Quantity_TOC_RGB ) ),
    space( Quantity_Color (1.,1.,1., Quantity_TOC_RGB ) );

  for( int i = 0; i < 1024; i++ ) {
    for (int j = 0; j < 8192; j++ )
      if (pixels[i*8192+j]>0) {
        anImage->SetPixel( anImage->LowerX()+i, 
          anImage->LowerY()+j,
          mark );
      }
      else {
        anImage->SetPixel( anImage->LowerX()+i, 
          anImage->LowerY()+j,
          space );
      }
  }

  image->FromImage( anImage );
  TCollection_AsciiString name( index );
  name.Prepend( "D:\\Temp_image" );
  name += ".bmp";
  OSD_File file ( name );
  file.Build( OSD_WriteOnly, OSD_Protection());
  image->Write(file);
  file.Close();
  delete []pixels;
}
