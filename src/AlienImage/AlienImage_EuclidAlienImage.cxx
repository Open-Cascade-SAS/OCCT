#include <AlienImage_EuclidAlienImage.ixx>

AlienImage_EuclidAlienImage::AlienImage_EuclidAlienImage()

{ // il faut faire un new si mydata est du type HANDLE
  myData = new AlienImage_EuclidAlienData() ;
}

void AlienImage_EuclidAlienImage::Clear()

{  myData->Clear() ; }

Standard_Boolean AlienImage_EuclidAlienImage::Write( OSD_File& file ) const

{  return( myData->Write( file ) ) ; }

Standard_Boolean AlienImage_EuclidAlienImage::Read( OSD_File& file )

{ return( myData->Read( file ) ) ; }

Handle_Image_Image AlienImage_EuclidAlienImage::ToImage() const

{ return( myData->ToImage() ) ; }

void AlienImage_EuclidAlienImage::FromImage( const Handle_Image_Image& anImage )

{ myData->FromImage( anImage ) ; }
 
