// Copyright (c) 1995-1999 Matra Datavision
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
 
