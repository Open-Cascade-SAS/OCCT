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
#include <TColStd_SequenceOfHAsciiString.hxx>

#ifdef _MSC_VER
#pragma comment( lib, "ftgl.lib" )
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
: myCurrentFontId(-1),
myXCurrentScale(1.f),
myYCurrentScale(1.f) 
{
}

OpenGl_FontMgr* OpenGl_FontMgr::instance() 
{
  static OpenGl_FontMgr* _mgr = NULL;
  if ( _mgr == NULL )
  {
    _mgr = new OpenGl_FontMgr();
  }
  return _mgr;
}

// Empty fontName means that ANY family name can be used.
// fontAspect == Font_FA_Undefined means ANY font aspect is acceptable.
// fontheight == -1 means ANY font height is acceptable.
int OpenGl_FontMgr::request_font (const Handle(TCollection_HAsciiString)& theFontName,
                                  const Font_FontAspect                   theFontAspect,
                                  const Standard_Integer&                 theFontHeight)
{
  Handle(Font_FontMgr) aFontMgr = Font_FontMgr::GetInstance();
  Handle(Font_SystemFont) aRequestedFont = aFontMgr->FindFont (theFontName, theFontAspect, theFontHeight);

  if (aRequestedFont.IsNull())
  {
    return -1;
  }

  // Setting font height
  Standard_Integer aFontHeight = theFontHeight;
  if (theFontHeight < 2 && aRequestedFont->FontHeight() == -1)
  {
    // Font height is not specified -> use DEFAULT_FONT_HEIGHT for variable-height fonts
    aFontHeight = DEFAULT_FONT_HEIGHT;
  }
  else if (theFontHeight < 2)
  {
    // Font height is not specified -> use font height for fixed size fonts
    aFontHeight = aRequestedFont->FontHeight();
  }

  GLCONTEXT aContext = GET_GL_CONTEXT();

  // Check in already generated fonts.
  if (myGeneratedFontDB.IsBound (aRequestedFont))
  {
    const IDList& anIDList = myGeneratedFontDB.Find (aRequestedFont);
    for (IDList::Iterator anIDListIterator (anIDList); anIDListIterator.More();
         anIDListIterator.Next())
    {
      OGLFont_Cache aFontCache = myGeneratedFontCache (anIDListIterator.Value());
      if (aFontCache.FontHeight == aFontHeight && aFontCache.GlContext == aContext)
      {
        // Requested font is already generated, returning it cache ID.
        myCurrentFontId = anIDListIterator.Value();
        return myCurrentFontId;
      }
    }
  }

  // Cache for requested font is not found. Generating new FTGL font.
  FTGLTextureFont* aFTGLFont = new FTGLTextureFont(aRequestedFont->FontPath()->ToCString());
  if ( !aFTGLFont || aFTGLFont->Error() != FT_Err_Ok)
  {
    return -1; // Error during creation FTGL font object!
  }

  if ( !aFTGLFont->FaceSize (aFontHeight) || aFTGLFont->Error() != FT_Err_Ok )
  {
    return -1; // Error during setup FTGL font height!
  }

  aFTGLFont->UseDisplayList (false);

  // Adding font to cache.
  OGLFont_Cache aCache;
  aCache.Font = aFTGLFont;
  aCache.FontHeight = aFontHeight;
  aCache.GlContext = aContext;

  myCurrentFontId = myGeneratedFontCache.Size() + 1;
  myGeneratedFontCache.Bind ( myCurrentFontId, aCache);

  if (myGeneratedFontDB.IsBound (aRequestedFont))
  {
    myGeneratedFontDB.ChangeFind (aRequestedFont).Append (myCurrentFontId);
  }
  else
  {
    IDList anIDList;
    anIDList.Append (myCurrentFontId);
    myGeneratedFontDB.Bind (aRequestedFont, anIDList);
  }

  return myCurrentFontId;
}

void OpenGl_FontMgr::render_text( const Standard_Integer id, const wchar_t* text,
                                  const Standard_Boolean is2d )
{
#ifdef TRACE
  cout << "TKOpenGl::render_text\n"
    << "\tfont id = " << id << endl
    << "\ttext = " << text << endl;
#endif
  if ( text && myGeneratedFontCache.IsBound( id ) ) {
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    glScalef( myXCurrentScale, myYCurrentScale, 1 );
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
    OGLFont_Cache cache = myGeneratedFontCache.Find( id );
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
  render_text( myCurrentFontId, text, is2d );
}

const FTFont* OpenGl_FontMgr::fontById (const Standard_Integer id)
{
  return myGeneratedFontCache.IsBound( id ) ? myGeneratedFontCache.Find( id ).Font: NULL;
}

Standard_ShortReal OpenGl_FontMgr::computeWidth( const Standard_Integer id, const wchar_t* text )
{
  if( !myGeneratedFontCache.IsBound( id ) )
    return 0.f;

  OGLFont_Cache cache = myGeneratedFontCache.Find( id );

  Standard_ShortReal w = cache.Font->Advance( text );

  return w;
}

void OpenGl_FontMgr::setCurrentScale (const Standard_ShortReal xScale,
                                      const Standard_ShortReal yScale)
{
  myXCurrentScale = xScale;
  myYCurrentScale = yScale;
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
