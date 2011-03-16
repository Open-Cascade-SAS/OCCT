
#include <stdio.h>

#include <AlienImage_XAlienImage.hxx>
#include <OSD_Protection.hxx>
#include <OSD_File.hxx>
#include <AlienImage_AidaAlienImage.ixx>

AlienImage_AidaAlienImage::AlienImage_AidaAlienImage()

{ // il faut faire un new si mydata est du type HANDLE
  myData = new AlienImage_AidaAlienData() ;
}

void AlienImage_AidaAlienImage::Clear()

{  myData->Clear() ; }

Standard_Boolean AlienImage_AidaAlienImage::Write( OSD_File& file ) const

{  return( myData->Write( file ) ) ; }

Standard_Boolean AlienImage_AidaAlienImage::Read( OSD_File& file )

{ return( myData->Read( file ) ) ; }

Handle(Image_Image) AlienImage_AidaAlienImage::ToImage() const

{ return( myData->ToImage() ) ; }

void AlienImage_AidaAlienImage::FromImage( const Handle(Image_Image)& anImage )

{ myData->FromImage( anImage ) ; }


void AlienImage_AidaAlienImage::SetColorImageDitheringMethod( 
	const Image_DitheringMethod aMethod ,
	const Handle(Aspect_ColorMap)& aColorMap )

{ myData->SetColorImageDitheringMethod( aMethod, aColorMap ) ; }
 
