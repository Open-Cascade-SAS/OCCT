#include <AlienImage_SGIRGBAlienImage.ixx>

AlienImage_SGIRGBAlienImage::AlienImage_SGIRGBAlienImage()

{ // il faut faire un new si mydata est du type HANDLE
  myData = new AlienImage_SGIRGBAlienData() ;
}

void AlienImage_SGIRGBAlienImage::SetName( const TCollection_AsciiString& aName)

{ myData->SetName( aName ) ; }

const TCollection_AsciiString&  AlienImage_SGIRGBAlienImage::Name() const
{  return( myData->Name() ) ; }

void AlienImage_SGIRGBAlienImage::Clear()

{  myData->Clear() ; }

Standard_Boolean AlienImage_SGIRGBAlienImage::Write( OSD_File& file ) const

{  return( myData->Write( file ) ) ; }

Standard_Boolean AlienImage_SGIRGBAlienImage::Read( OSD_File& file )

{ return( myData->Read( file ) ) ; }

Handle_Image_Image AlienImage_SGIRGBAlienImage::ToImage() const

{ return( myData->ToImage() ) ; }

void AlienImage_SGIRGBAlienImage::FromImage( const Handle_Image_Image& anImage )

{ myData->FromImage( anImage ) ; }
 
