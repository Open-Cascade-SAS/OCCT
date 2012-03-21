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
 
