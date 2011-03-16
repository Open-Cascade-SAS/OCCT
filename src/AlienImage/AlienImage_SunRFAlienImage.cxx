#include <AlienImage_SunRFAlienImage.ixx>

AlienImage_SunRFAlienImage::AlienImage_SunRFAlienImage()

{ // il faut faire un new si mydata est du type HANDLE
  myData = new AlienImage_SunRFAlienData() ;
}

void AlienImage_SunRFAlienImage::Clear()

{  myData->Clear() ; }

Standard_Boolean AlienImage_SunRFAlienImage::Write( OSD_File& file ) const

{  return( myData->Write( file ) ) ; }

Standard_Boolean AlienImage_SunRFAlienImage::Read( OSD_File& file )

{ return( myData->Read( file ) ) ; }

Handle_Image_Image AlienImage_SunRFAlienImage::ToImage() const

{ return( myData->ToImage() ) ; }

void AlienImage_SunRFAlienImage::FromImage( const Handle_Image_Image& anImage )

{ myData->FromImage( anImage ) ; }

void AlienImage_SunRFAlienImage::SetFormat( 
	const AlienImage_SUNRFFormat aFormat )

{ myData->SetFormat( aFormat ) ; }

AlienImage_SUNRFFormat AlienImage_SunRFAlienImage::Format() const

{ return myData->Format(  ) ; }
 
