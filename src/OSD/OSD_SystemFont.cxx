// Copyright:     OpenCASCADE

// File:          OSD_SystemFont.cxx
// Created:       20.01.2008
// Author:        Alexander A. BORODIN
// Updated:

#include <OSD_SystemFont.ixx>
#include <OSD_Path.hxx>
#include <TCollection_HAsciiString.hxx>


OSD_SystemFont::OSD_SystemFont():
MyFontName(),
MyFontAspect(OSD_FA_Undefined),
MyFaceSize(-1),
MyVerification(Standard_False)
{
}

OSD_SystemFont::OSD_SystemFont( const Handle(TCollection_HAsciiString)& FontName,
                                const OSD_FontAspect FontAspect,
                                const Handle(TCollection_HAsciiString)& FilePath ):
MyFontName(FontName),
MyFontAspect(FontAspect),
MyFilePath(FilePath),
MyFaceSize(-1),
MyVerification(Standard_True)
{

}

OSD_SystemFont::OSD_SystemFont( const Handle(TCollection_HAsciiString)& XLFD,
                                const Handle(TCollection_HAsciiString)& FilePath) :
MyFilePath(FilePath),
MyFontAspect(OSD_FA_Undefined)
{
  MyVerification = Standard_True;
  if ( XLFD.IsNull() )
  {
    MyVerification=Standard_False;
    printf("NULL XLFD handler \n");
  }
  if ( XLFD->IsEmpty() )
  {
    MyVerification=Standard_False;
    printf("EMPTY XLFD handler \n");
  }

  if(MyVerification)
  {
    MyFontName = XLFD->Token( "-", 2 );
    TCollection_AsciiString str( XLFD->ToCString() );

    if ( str.Search( "-0-0-0-0-" ) >=0 )
      MyFaceSize = -1;
    else
      //TODO catch exeption
      MyFaceSize =  str.Token( "-", 7 ).RealValue();

    //detect aspect
    if ( str.Token("-", 3).IsEqual( "bold" ) )
      MyFontAspect = OSD_FA_Bold;
    else if ( str.Token("-", 3).IsEqual( "medium" ) ||
      str.Token("-", 3).IsEqual( "normal" ) )
      MyFontAspect = OSD_FA_Regular;

    if ( MyFontAspect != OSD_FA_Undefined && 
      ( str.Token("-",4 ).IsEqual( "i" ) || str.Token("-",4 ).IsEqual( "o" ) ) ) 
    {
      if ( MyFontAspect == OSD_FA_Bold )
        MyFontAspect = OSD_FA_BoldItalic;
      else
        MyFontAspect = OSD_FA_Italic;
    }
  }
}

Standard_Boolean OSD_SystemFont::IsValid() const{
  if ( !MyVerification)
    return Standard_False;

  if ( MyFontAspect == OSD_FA_Undefined )
    return Standard_False;

  if ( MyFontName->IsEmpty() || !MyFontName->IsAscii() )
    return Standard_False;

  OSD_Path path;  
  return path.IsValid( MyFilePath->String() );
}

Handle(TCollection_HAsciiString) OSD_SystemFont::FontPath() const{
  return MyFilePath;
}

Handle(TCollection_HAsciiString) OSD_SystemFont::FontName() const{
  return MyFontName;
}

OSD_FontAspect OSD_SystemFont::FontAspect() const{
  return MyFontAspect;
}

Standard_Integer OSD_SystemFont::FontHeight() const {
  return MyFaceSize;
}
