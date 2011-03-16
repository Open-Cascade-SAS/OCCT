#include <OpenGl_FontMgr.hxx>

#include <FTGLTextureFont.h>        
#include <FTLibrary.h>
#include <FTFace.h>
#include <Standard_Stream.hxx>

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
  Handle(OSD_FontMgr) fntMgr = OSD_FontMgr::GetInstance();
  if ( !fntMgr.IsNull() ) {

    OSD_NListOfSystemFont fontList = fntMgr->GetAvalableFonts();
    if ( fontList.Size() != 0 ) {

      OSD_NListOfSystemFont::Iterator it(fontList);
      for ( ; it.More(); it.Next() ) {
        OGLFont_SysInfo* info = new OGLFont_SysInfo();
        if ( it.Value()->FontAspect() == OSD_FA_Regular ) {
          //this workaround for fonts with names dependent on system locale.
          //for example: "Times New Roman Fett Kursive" or "Times New Roman Gras Italiqui"
          FTFace face(it.Value()->FontPath()->ToCString());
              
          if ( face.Error() == FT_Err_Ok ) {
            if ( (*face.Face())->style_flags == 0 ) {
              info->SysFont = it.Value();
            }
            else {
              //new aspect detected for current font item
#ifdef TRACE
              cout << "TKOpenGl::initializeFontDB() detected new font!\n"
                << "\tFont Previous Name: " << it.Value()->FontName()->ToCString() << endl
                << "\tFont New Name: " << (*face.Face())->family_name << endl
                << "\tFont Aspect: " << (*face.Face())->style_flags << endl;
#endif
              OSD_FontAspect aspect = OSD_FA_Regular;
              if ( (*face.Face())->style_flags == (FT_STYLE_FLAG_ITALIC | FT_STYLE_FLAG_BOLD) )
                aspect = OSD_FA_BoldItalic;
              else if ( (*face.Face())->style_flags == FT_STYLE_FLAG_ITALIC )
                aspect = OSD_FA_Italic;
              else if ( (*face.Face())->style_flags == FT_STYLE_FLAG_BOLD )
                aspect = OSD_FA_Bold;

#ifdef TRACE
              cout << "\tOSD_FontAspect: " << aspect << endl;
#endif
              Handle(TCollection_HAsciiString) aFontName =
                new TCollection_HAsciiString( (*face.Face())->family_name );
              info->SysFont = new OSD_SystemFont( aFontName, aspect, it.Value()->FontPath() );
            }
          }
          else
            continue;
        } else {
          info->SysFont = it.Value();
        }
        _FontDB.Append(info);

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
// fontAspect == OSD_FA_Undefined means ANY font aspect is acceptable.
// fontheight == -1 means ANY font height is acceptable.
int OpenGl_FontMgr::request_font( const Handle(TCollection_HAsciiString)& fontName,
                                 const OSD_FontAspect                    fontAspect,
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
      if (fontAspect != OSD_FA_Undefined && DBit.Value()->SysFont->FontAspect() != fontAspect) {
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
      case OSD_FA_Bold:
        cout << "OSD_FA_Bold\n";
        break;
      case OSD_FA_BoldItalic:
        cout << "OSD_FA_BoldItalic\n";
        break;
      case OSD_FA_Italic:
        cout << "OSD_FA_Italic\n";
        break;
      case OSD_FA_Regular:
        cout << "OSD_FA_Regular\n";
        break;
      default:
        cout << "OSD_FA_Undefined\n";
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

void OpenGl_FontMgr::render_text( const Standard_Integer id, const char* text)
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
    if( !enableDepthTest )
      glEnable(GL_DEPTH_TEST);

    GLint* param = new GLint;    
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, param);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
    glAlphaFunc(GL_GEQUAL, 0.285f);    
    glEnable(GL_ALPHA_TEST);   
    OGLFont_Cache cache = _FontCache.Find( id );
    cache.Font->Render( text );

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, *param);

    if( !enableTexture )
      glDisable(GL_TEXTURE_2D);
    if( !enableDepthTest )
      glDisable(GL_DEPTH_TEST);

    delete param;

    glPopAttrib();
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
  }

}

void OpenGl_FontMgr::render_text( const char* text){
  render_text( _CurrentFontId, text );
}


const FTFont* OpenGl_FontMgr::fontById( const Standard_Integer id ){
  return _FontCache.IsBound( id ) ? _FontCache.Find( id ).Font: NULL;
}

Standard_ShortReal OpenGl_FontMgr::computeWidth( const Standard_Integer id, const char* str ){
  if( !_FontCache.IsBound( id ) )
    return 0.f;

  OGLFont_Cache cache = _FontCache.Find( id );
  GLenum err = glGetError();

  Standard_ShortReal w = cache.Font->Advance( str );

  return w;
}

void OpenGl_FontMgr::setCurrentScale( const Standard_ShortReal xScale,
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

void dump_texture( int id) {
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
