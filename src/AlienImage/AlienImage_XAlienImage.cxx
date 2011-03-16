#include <AlienImage_XAlienImage.ixx>

AlienImage_XAlienImage::AlienImage_XAlienImage()

{ // il faut faire un new si mydata est du type HANDLE
  myData = new AlienImage_X11XWDAlienData() ;
}

void AlienImage_XAlienImage::SetName( const TCollection_AsciiString& aName)

{ myData->SetName( aName ) ; }

const TCollection_AsciiString&  AlienImage_XAlienImage::Name() const
{  return( myData->Name() ) ; }

void AlienImage_XAlienImage::Clear()

{  myData->Clear() ; }

Standard_Boolean AlienImage_XAlienImage::Write( OSD_File& file ) const

{  return( myData->Write( file ) ) ; }

Standard_Boolean AlienImage_XAlienImage::Read( OSD_File& file )

{ return( myData->Read( file ) ) ; }

Handle_Image_Image AlienImage_XAlienImage::ToImage() const

{ return( myData->ToImage() ) ; }

void AlienImage_XAlienImage::FromImage( const Handle_Image_Image& anImage )

{ myData->FromImage( anImage ) ; }
 
